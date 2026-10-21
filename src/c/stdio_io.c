/* @file: stdio_io.c
 * #desc:
 *    The implementations of standard input/output.
 *
 * #copy:
 *    Copyright (C) 2026 - Alex Chen <chenalex.cy@gmail.com>
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License as published by the Free Software Foundation; either
 *    version 2.1 of the License, or (at your option) any later version.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not,
 *    see <https://www.gnu.org/licenses/>.
 */

#include <ecp/config.h>
#include <ecp/c/stddef.h>
#include <ecp/c/stdint.h>
#include <ecp/c/string.h>
#include <ecp/c/stdio.h>
#include <ecp/c/stdlib.h>
#include <ecp/c/unistd.h>
#include <ecp/c/fcntl.h>
#include <ecp/c/atomic.h>
#include <ecp/ds_list.h>

struct stdio_file {
	int32_t flags;
	int32_t fd;
	uint8_t *buf;
	size_t buf_size;
	uint8_t *rpos;
	uint8_t *rend;
	uint8_t *wbase;
	uint8_t *wpos;
	uint8_t *wend;
	uint8_t vbuf[X_BUFSIZ];
	spinlock_t lock;
	struct list_node list;
};

#define FG_EOF  0x01
#define FG_ERR  0x02
#define FG_NORD 0x04
#define FG_NOWR 0x08
#define FG_SEEK 0x10
#define FG_TEXT 0x20
#define FG_PERM 0x40

static struct stdio_file __stdin = {
	.flags = FG_PERM | FG_NOWR,
	.fd = X_STDIN_FILENO,
	.buf = __stdin.vbuf,
	.buf_size = X_BUFSIZ,
	.rpos = NULL,
	.rend = NULL,
	.wbase = NULL,
	.wpos = NULL,
	.wend = NULL,
	.lock = 0
	};

static struct stdio_file __stdout = {
	.flags = FG_PERM | FG_TEXT | FG_NORD,
	.fd = X_STDOUT_FILENO,
	.buf = __stdout.vbuf,
	.buf_size = X_BUFSIZ,
	.rpos = NULL,
	.rend = NULL,
	.wbase = NULL,
	.wpos = NULL,
	.wend = NULL,
	.lock = 0
	};

static struct stdio_file __stderr = {
	.flags = FG_PERM | FG_NORD,
	.fd = X_STDERR_FILENO,
	.buf = NULL,
	.buf_size = 0,
	.rpos = NULL,
	.rend = NULL,
	.wbase = NULL,
	.wpos = NULL,
	.wend = NULL,
	.lock = 0
	};

/* @fvar: __ecp_stdin
 */
xFILE *__ecp_stdin = (xFILE *)&__stdin;
/* @fvar: __ecp_stdout
 */
xFILE *__ecp_stdout = (xFILE *)&__stdout;
/* @fvar: __ecp_stderr
 */
xFILE *__ecp_stderr = (xFILE *)&__stderr;

static LIST_HEAD_NEW(_fp_list);
static spinlock_t _fp_list_lock = 0;

/* @func: _stdio_feof (static)
 * #desc:
 *    get the eof flags function.
 *
 * #1: fp [in/out] stdio file struct
 * #r:    [ret]    0: no eof, -1: eof
 */
static int32_t _stdio_feof(xFILE *fp)
{
	struct stdio_file *f = (struct stdio_file *)fp;

	return (f->flags & FG_EOF) ? X_EOF : 0;
}

/* @func: _stdio_ferror (static)
 * #desc:
 *    get the error flags function.
 *
 * #1: fp [in/out] stdio file struct
 * #r:    [ret]    0: no error, -1: error
 */
static int32_t _stdio_ferror(xFILE *fp)
{
	struct stdio_file *f = (struct stdio_file *)fp;

	return (f->flags & FG_ERR) ? -1 : 0;
}

/* @func: _stdio_clearerr (static)
 * #desc:
 *    clear error flags function.
 *
 * #1: fp [in/out] stdio file struct
 */
static void _stdio_clearerr(xFILE *fp)
{
	struct stdio_file *f = (struct stdio_file *)fp;

	f->flags &= ~FG_ERR;
}

/* @func: _stdio_write (static)
 * #desc:
 *    write to the file from stream buffer and input buffer.
 *
 * #1: fp  [in/out] stdio file struct
 * #2: buf [in]     input buffer
 * #3: len [in]     input length
 * #r:     [ret]    write length of the input buffer
 */
static size_t _stdio_write(xFILE *fp, const uint8_t *buf, size_t len)
{
	struct stdio_file *f = (struct stdio_file *)fp;
	size_t k = (size_t)(f->wpos - f->wbase), t = len;
	ssize_t r;

	/* stream buffer */
	while (k) {
		r = ecp_write(f->fd, f->wbase, k);
		if (r < 0) {
			f->wpos = f->wend = f->wbase = NULL;
			f->flags |= FG_ERR;
			return 0;
		}
		f->wbase += r;
		k -= r;
	}

	/* input buffer */
	while (len) {
		r = ecp_write(f->fd, buf, len);
		if (r < 0) {
			f->wpos = f->wend = f->wbase = NULL;
			f->flags |= FG_ERR;
			return t - len;
		}
		buf += r;
		len -= r;
	}

	f->wpos = f->wend = f->wbase = f->buf;
	f->wend += f->buf_size;

	return t;
}

/* @func: _stdio_fflush (static)
 * #desc:
 *    flush stream buffer.
 *
 * #1: fp [in/out] stdio file struct
 * #r:    [ret]    0: no error, -1: eof
 */
static int32_t _stdio_fflush(xFILE *fp)
{
	struct stdio_file *f = (struct stdio_file *)fp;
	xoff_t r;

	/* flush write */
	if (f->wpos != f->wbase) {
		_stdio_write(fp, NULL, 0);
		if (!f->wpos)
			return X_EOF;
	}

	/* flush read */
	if (f->flags & FG_SEEK && f->rpos != f->rend) {
		r = ecp_lseek(f->fd, 0, X_SEEK_CUR);
		if (r > 0) {
			r -= (xoff_t)(f->rend - f->rpos);
			ecp_lseek(f->fd, r, X_SEEK_SET);
		}
	}

	f->wpos = f->wend = f->wbase = NULL;
	f->rpos = f->rend = NULL;

	return 0;
}

/* @func: _stdio_fread (static)
 * #desc:
 *    read from the input stream into the buffer.
 *
 * #1: t  [out]    output buffer
 * #2: m  [in]     items size
 * #3: n  [in]     items number
 * #4: fp [in/out] stdio file struct
 * #r:    [ret]    items number of the read
 */
static size_t _stdio_fread(void *t, size_t m, size_t n, xFILE *fp)
{
	struct stdio_file *f = (struct stdio_file *)fp;
	uint8_t *buf = t;
	size_t k, len = m * n;
	ssize_t r;

	if (!len)
		return 0;

	/* initialize read stream buffer */
	if (!f->rend || f->wend) {
		if (f->wpos != f->wbase) /* flush write */
			_stdio_write(fp, NULL, 0);

		f->wpos = f->wend = f->wbase = NULL;
		if (f->flags & FG_NORD) {
			f->flags |= FG_ERR;
			return 0;
		}
		if (f->flags & FG_EOF)
			return 0;
	}

	/* read to stream buffer */
	if (f->rpos == f->rend && f->buf_size) {
		f->rpos = f->rend = f->buf;
		r = ecp_read(f->fd, f->buf, f->buf_size);
		if (r <= 0) {
			f->flags |= r ? FG_ERR : FG_EOF;
			return 0;
		}
		f->rend += r;
	}

#undef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

	k = (size_t)(f->rend - f->rpos);
	if (k) {
		k = MIN(k, len);
		ecp_memcpy(buf, f->rpos, k);
		f->rpos += k;
		buf += k;
		len -= k;
	}

	while (len) {
		r = ecp_read(f->fd, buf, len);
		if (r <= 0) {
			f->flags |= r ? FG_ERR : FG_EOF;
			return ((m * n) - len) / m;
		}
		buf += r;
		len -= r;
	}

	return n;
}

/* @func: _stdio_fwrite (static)
 * #desc:
 *    write from the buffer to the output stream.
 *
 * #1: s  [in]     input buffer
 * #2: m  [in]     items size
 * #3: n  [in]     items number
 * #4: fp [in/out] stdio file struct
 * #r:    [ret]    items number of the write
 */
static size_t _stdio_fwrite(const void *s, size_t m, size_t n, xFILE *fp)
{
	struct stdio_file *f = (struct stdio_file *)fp;
	const uint8_t *buf = s;
	size_t k, w, len = m * n;

	if (!len)
		return 0;

	/* initialize write stream buffer */
	if (!f->wend) {
		if (f->flags & FG_NOWR) {
			f->flags |= FG_ERR;
			return 0;
		}

		f->rpos = f->rend = NULL;
		f->wpos = f->wend = f->wbase = f->buf;
		f->wend += f->buf_size;
	}

	k = (size_t)(f->wend - f->wpos);
	if (len > k) { /* len > remaining */
		if (k) {
			ecp_memcpy(f->wpos, buf, k);
			f->wpos += k;
			buf += k;
			len -= k;
		}
		k += _stdio_write(fp, buf, len);
		return (k == len) ? n : (k / m);
	}

	/* text stream */
	if (f->flags & FG_TEXT) {
		for (k = len; k && buf[k - 1] != '\n'; k--);
		if (len == 1 && k == 1) {
			*f->wpos++ = *buf;
			_stdio_write(fp, NULL, 0);
			return (f->flags & FG_ERR) ? 0 : 1;
		}

		if (k) {
			w = _stdio_write(fp, buf, k);
			if (w < k)
				return w / m;
			buf += w;
			len -= w;
		}
	}

	ecp_memcpy(f->wpos, buf, len);
	f->wpos += len;

	return n;
}

/* @func: _stdio_fseek (static)
 * #desc:
 *    reposition a stream.
 *
 * #1: fp     [in/out] stdio file struct
 * #2: off    [in]     file offset
 * #3: whence [in]     offset type
 * #r:        [ret]    0: no error, -1: error
 */
static int32_t _stdio_fseek(xFILE *fp, int64_t off, int32_t whence)
{
	struct stdio_file *f = (struct stdio_file *)fp;
	xoff_t r;

	switch (whence) {
		case X_SEEK_SET: case X_SEEK_CUR: case X_SEEK_END:
			break;
		default:
			return -1;
	}

	if (_stdio_fflush(fp))
		return -1;

	if (f->flags & FG_SEEK) {
		r = ecp_lseek(f->fd, off, whence);
		if (r < 0)
			return -1;
	}
	f->flags &= ~FG_EOF;

	return 0;
}

/* @func: _stdio_ftell (static)
 * #desc:
 *    get the stream position.
 *
 * #1: fp [in]  stdio file struct
 * #r:    [ret] >=0: file position, -1: error
 */
static int64_t _stdio_ftell(xFILE *fp)
{
	struct stdio_file *f = (struct stdio_file *)fp;
	xoff_t r;

	if (f->flags & FG_SEEK) {
		r = ecp_lseek(f->fd, 0, X_SEEK_CUR);
		if (r < 0)
			return -1;

		if (f->rend) {
			r -= (xoff_t)(f->rpos - f->buf);
		} else if (f->wend) {
			r += (xoff_t)(f->wpos - f->buf);
		}
		return r;
	}

	return -1;
}

/* @func: _stdio_rewind (static)
 * #desc:
 *    reposition stream to the start.
 *
 * #1: fp [in/out] stdio file struct
 */
static void _stdio_rewind(xFILE *fp)
{
	_stdio_fseek(fp, 0, X_SEEK_SET);
}

/* @func: _stdio_fgetc (static)
 * #desc:
 *    get the character function.
 *
 * #1: fp [in/out] stdio file struct
 * #r:    [ret]    >=0: character, -1: eof
 */
static int32_t _stdio_fgetc(xFILE *fp)
{
	struct stdio_file *f = (struct stdio_file *)fp;
	uint8_t c;

	if (f->rpos != f->rend) {
		return *f->rpos++;
	} else {
		if (!_stdio_fread(&c, 1, 1, fp))
			return X_EOF;
		return c;
	}

	return X_EOF;
}

/* @func: _stdio_fgets (static)
 * #desc:
 *    get the character function.
 *
 * #1: s   [out]    output string
 * #2: len [in]     output length
 * #3: fp  [in/out] stdio file struct
 * #r:     [ret]    returns the string / NULL
 */
static char *_stdio_fgets(char *s, int32_t len, xFILE *fp)
{
	struct stdio_file *f = (struct stdio_file *)fp;
	char *p = s;
	uint8_t *z;
	int32_t c;
	size_t k;

	if (len <= 1) {
		if (len < 1)
			return NULL;
		*p = '\0';
		return s;
	}
	len--;

#undef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

	while (len) {
		k = (size_t)(f->rend - f->rpos);
		if (k) {
			z = ecp_memchr(f->rpos, '\n', k);
			if (z)
				k = (size_t)(z - f->rpos + 1);

			k = MIN(k, (size_t)len);
			ecp_memcpy(p, f->rpos, k);
			f->rpos += k;
			p += k;
			len -= k;

			if (z || !len)
				break;
		}

		c = _stdio_fgetc(fp);
		if (c < 0) {
			if (p == s || !_stdio_feof(fp))
				return NULL;
			break;
		}

		*p++ = (char)c;
		len--;
		if (c == '\n')
			break;
	}
	*p = '\0';

	return s;
}

/* @func: _stdio_fputc (static)
 * #desc:
 *    put the character function.
 *
 * #1: c  [in]     input character
 * #2: fp [in/out] stdio file struct
 * #r:    [ret]    0: no error, -1: error
 */
static int32_t _stdio_fputc(int32_t c, xFILE *fp)
{
	if (!_stdio_fwrite(&c, 1, 1, fp))
		return -1;

	return 0;
}

/* @func: _stdio_fputs (static)
 * #desc:
 *    put the string function.
 *
 * #1: s  [in]     input string
 * #2: fp [in/out] stdio file struct
 * #r:    [ret]    0: no error, -1: error
 */
static int32_t _stdio_fputs(const char *s, xFILE *fp)
{
	size_t n = ecp_strlen(s);
	if (!_stdio_fwrite(s, 1, n, fp))
		return -1;

	return 0;
}

/* @func: _stdio_fopen (static)
 * #desc:
 *    open the file stream function.
 *
 * #1: path [in]  path name
 * #2: mode [in]  access mode
 * #r:      [ret] returns the stdio pointer / NULL
 */
static xFILE *_stdio_fopen(const char *path, const char *mode)
{
	struct stdio_file *f;
	int32_t flags;

	f = ecp_malloc(sizeof(struct stdio_file));
	if (!f)
		return NULL;

	ecp_memset(f, 0, sizeof(struct stdio_file));
	f->buf = f->vbuf;
	f->buf_size = X_BUFSIZ;

	switch (*mode) {
		case 'r':
			flags = X_O_RDONLY;
			if (mode[1] == '+') {
				flags = X_O_RDWR;
			} else {
				f->flags |= FG_NOWR;
			}
			break;
		case 'w':
			flags = X_O_CREAT | X_O_TRUNC;
			if (mode[1] == '+') {
				flags |= X_O_RDWR;
			} else {
				flags |= X_O_WRONLY;
				f->flags |= FG_NORD;
			}
			break;
		case 'a':
			flags = X_O_CREAT | X_O_APPEND;
			if (mode[1] == '+') {
				flags |= X_O_RDWR;
			} else {
				flags |= X_O_WRONLY;
				f->flags |= FG_NORD;
			}
			break;
		default:
			ecp_free(f);
			return NULL;
	}

	if (ecp_strchr(mode, 'x'))
		flags |= X_O_EXCL;
	if (ecp_strchr(mode, 'e'))
		flags |= X_O_CLOEXEC;

	f->fd = ecp_open(path, flags, 0666);
	if (f->fd < 0) {
		ecp_free(f);
		return NULL;
	}

	if (ecp_lseek(f->fd, 0, X_SEEK_CUR) >= 0)
		f->flags |= FG_SEEK;

	SPIN_LOCK(&_fp_list_lock);
	ecp_list_add(&_fp_list, &f->list);
	SPIN_UNLOCK(&_fp_list_lock);

	return (xFILE *)f;
}

/* @func: _stdio_fclose (static)
 * #desc:
 *    close the file stream function.
 *
 * #1: fp [in/out] stdio file struct
 * #r:    [ret]    0: no error, -1: error
 */
static int32_t _stdio_fclose(xFILE *fp)
{
	struct stdio_file *f = (struct stdio_file *)fp;

	if (!fp)
		return -1;

	_stdio_fflush(fp);

	if (f->flags & FG_PERM)
		return -1;
	if (ecp_close(f->fd))
		return -1;

	SPIN_LOCK(&_fp_list_lock);
	ecp_list_del(&_fp_list, &f->list);
	SPIN_UNLOCK(&_fp_list_lock);

	ecp_free(f);

	return 0;
}

/* @func: _stdio_setvbuf (static)
 * #desc:
 *    stream buffer operations function.
 *
 * #1: fp   [in/out] stdio file struct
 * #2: buf  [in/out] stream buffer / NULL
 * #3: type [in]     buffer type
 * #4: size [in]     buffer size
 * #r:      [ret]    0: no error, -1: error
 */
static int32_t _stdio_setvbuf(xFILE *fp, uint8_t *buf, int32_t type,
		size_t size)
{
	struct stdio_file *f = (struct stdio_file *)fp;
	f->flags &= ~FG_TEXT;

	if (type == X_IONBF) {
		f->buf_size = 0;
	} else if (type == X_IOFBF || type == X_IOLBF) {
		if (buf && size) {
			f->buf = buf;
			f->buf_size = size;
		}
		if (type == X_IOLBF)
			f->flags |= FG_TEXT;
	} else {
		return -1;
	}

	return 0;
}

/* @func: _stdio_fgetpos (static)
 * #desc:
 *    get the stream position.
 *
 * #1: fp  [in]  stdio file struct
 * #2: pos [out] position pointer
 * #r:     [ret] 0: no error, -1: error
 */
static int32_t _stdio_fgetpos(xFILE *fp, xfpos_t *pos)
{
	int64_t r = _stdio_ftell(fp);
	if (r < 0)
		return -1;

	*(int64_t *)pos = r;

	return 0;
}

/* @func: _stdio_fsetpos (static)
 * #desc:
 *    set the stream position.
 *
 * #1: fp  [in]  stdio file struct
 * #2: pos [out] position pointer
 * #r:     [ret] 0: no error, -1: error
 */
static int32_t _stdio_fsetpos(xFILE *fp, const xfpos_t *pos)
{
	if (_stdio_fseek(fp, *(int64_t *)pos, X_SEEK_SET))
		return -1;

	return 0;
}

/* @func: ecp_feof
 * #desc:
 *    get the eof flags function.
 *
 * #1: fp [in/out] stdio file struct
 * #r:    [ret]    0: no eof, -1: eof
 */
int32_t ecp_feof(xFILE *fp)
{
	struct stdio_file *f = (struct stdio_file *)fp;

	SPIN_LOCK(&f->lock);
	int32_t ret = _stdio_feof(fp);
	SPIN_UNLOCK(&f->lock);

	return ret;
}

/* @func: ecp_ferror
 * #desc:
 *    get the error flags function.
 *
 * #1: fp [in/out] stdio file struct
 * #r:    [ret]    0: no error, -1: error
 */
int32_t ecp_ferror(xFILE *fp)
{
	struct stdio_file *f = (struct stdio_file *)fp;

	SPIN_LOCK(&f->lock);
	int32_t ret = _stdio_ferror(fp);
	SPIN_UNLOCK(&f->lock);

	return ret;
}

/* @func: ecp_clearerr
 * #desc:
 *    clear error flags function.
 *
 * #1: fp [in/out] stdio file struct
 */
void ecp_clearerr(xFILE *fp)
{
	struct stdio_file *f = (struct stdio_file *)fp;

	SPIN_LOCK(&f->lock);
	_stdio_clearerr(fp);
	SPIN_UNLOCK(&f->lock);
}

/* @func: ecp_fflush
 * #desc:
 *    flush stream buffer.
 *
 * #1: fp [in/out] stdio file struct
 * #r:    [ret]    0: no error, -1: eof
 */
int32_t ecp_fflush(xFILE *fp)
{
	struct stdio_file *f = (struct stdio_file *)fp;
	int32_t ret = 0;

	if (f) {
		SPIN_LOCK(&f->lock);
		ret = _stdio_fflush(fp);
		SPIN_UNLOCK(&f->lock);
	} else {
		if (SPIN_FETCH(&__stdout.lock)) {
			ret |= _stdio_fflush(__ecp_stdout);
			SPIN_UNLOCK(&__stdout.lock);
		}
		if (SPIN_FETCH(&__stderr.lock)) {
			ret |= _stdio_fflush(__ecp_stderr);
			SPIN_UNLOCK(&__stderr.lock);
		}

		SPIN_LOCK(&_fp_list_lock);
		LIST_FOR_EACH(_fp_list.node, pos) {
			f = LIST_ENTRY(pos, struct stdio_file, list);
			if (SPIN_FETCH(&f->lock)) {
				ret |= _stdio_fflush((xFILE *)f);
				SPIN_UNLOCK(&f->lock);
			}
		}
		SPIN_UNLOCK(&_fp_list_lock);
	}

	return ret;
}

/* @func: ecp_fread
 * #desc:
 *    read from the input stream into the buffer.
 *
 * #1: t  [out]    output buffer
 * #2: m  [in]     items size
 * #3: n  [in]     items number
 * #4: fp [in/out] stdio file struct
 * #r:    [ret]    items number of the read
 */
size_t ecp_fread(void *t, size_t m, size_t n, xFILE *fp)
{
	struct stdio_file *f = (struct stdio_file *)fp;

	SPIN_LOCK(&f->lock);
	size_t ret = _stdio_fread(t, m, n, fp);
	SPIN_UNLOCK(&f->lock);

	return ret;
}

/* @func: ecp_fwrite
 * #desc:
 *    write from the buffer to the output stream.
 *
 * #1: s  [in]     input buffer
 * #2: m  [in]     items size
 * #3: n  [in]     items number
 * #4: fp [in/out] stdio file struct
 * #r:    [ret]    items number of the write
 */
size_t ecp_fwrite(const void *s, size_t m, size_t n, xFILE *fp)
{
	struct stdio_file *f = (struct stdio_file *)fp;

	SPIN_LOCK(&f->lock);
	size_t ret = _stdio_fwrite(s, m, n, fp);
	SPIN_UNLOCK(&f->lock);

	return ret;
}

/* @func: ecp_fseek
 * #desc:
 *    reposition stream to the start.
 *
 * #1: fp     [in/out] stdio file struct
 * #2: off    [in]     file offset
 * #3: whence [in]     seek type
 * #r:        [ret]    0: no error, -1: error
 */
int32_t ecp_fseek(xFILE *fp, int64_t off, int32_t whence)
{
	struct stdio_file *f = (struct stdio_file *)fp;

	SPIN_LOCK(&f->lock);
	int32_t ret = _stdio_fseek(fp, off, whence);
	SPIN_UNLOCK(&f->lock);

	return ret;
}

/* @func: ecp_ftell
 * #desc:
 *    get the stream position.
 *
 * #1: fp [in]  stdio file struct
 * #r:    [ret] >=0: file position, -1: error
 */
int64_t ecp_ftell(xFILE *fp)
{
	struct stdio_file *f = (struct stdio_file *)fp;

	SPIN_LOCK(&f->lock);
	int64_t ret = _stdio_ftell(fp);
	SPIN_UNLOCK(&f->lock);

	return ret;
}

/* @func: ecp_rewind
 * #desc:
 *    reposition stream to the start.
 *
 * #1: fp [in/out] stdio file struct
 */
void ecp_rewind(xFILE *fp)
{
	struct stdio_file *f = (struct stdio_file *)fp;

	SPIN_LOCK(&f->lock);
	_stdio_rewind(fp);
	SPIN_UNLOCK(&f->lock);
}

/* @func: ecp_fgetc
 * #desc:
 *    get the character function.
 *
 * #1: fp [in/out] stdio file struct
 * #r:    [ret]    >=0: character, -1: eof
 */
int32_t ecp_fgetc(xFILE *fp)
{
	struct stdio_file *f = (struct stdio_file *)fp;

	SPIN_LOCK(&f->lock);
	int32_t ret = _stdio_fgetc(fp);
	SPIN_UNLOCK(&f->lock);

	return ret;
}

/* @func: ecp_fgets
 * #desc:
 *    get the character function.
 *
 * #1: s   [out]    output string
 * #2: len [in]     output length
 * #3: fp  [in/out] stdio file struct
 * #r:     [ret]    returns the string
 */
char *ecp_fgets(char *s, int32_t len, xFILE *fp)
{
	struct stdio_file *f = (struct stdio_file *)fp;

	SPIN_LOCK(&f->lock);
	char *ret = _stdio_fgets(s, len, fp);
	SPIN_UNLOCK(&f->lock);

	return ret;
}

/* @func: ecp_fputc
 * #desc:
 *    put the character function.
 *
 * #1: c  [in]     input character
 * #2: fp [in/out] stdio file struct
 * #r:    [ret]    0: no error, -1: error
 */
int32_t ecp_fputc(int32_t c, xFILE *fp)
{
	struct stdio_file *f = (struct stdio_file *)fp;

	SPIN_LOCK(&f->lock);
	int32_t ret = _stdio_fputc(c, fp);
	SPIN_UNLOCK(&f->lock);

	return ret;
}

/* @func: ecp_fputs
 * #desc:
 *    put the string function.
 *
 * #1: s  [in]     input string
 * #2: fp [in/out] stdio file struct
 * #r:    [ret]    0: no error, -1: error
 */
int32_t ecp_fputs(const char *s, xFILE *fp)
{
	struct stdio_file *f = (struct stdio_file *)fp;

	SPIN_LOCK(&f->lock);
	int32_t ret = _stdio_fputs(s, fp);
	SPIN_UNLOCK(&f->lock);

	return ret;
}

/* @func: ecp_fopen
 * #desc:
 *    open the file stream function.
 *
 * #1: path [in]  path name
 * #2: mode [in]  access mode
 * #r:      [ret] returns the stdio pointer / NULL
 */
xFILE *ecp_fopen(const char *path, const char *mode)
{
	return _stdio_fopen(path, mode);
}

/* @func: ecp_fclose
 * #desc:
 *    close the file stream function.
 *
 * #1: fp [in/out] stdio file struct
 * #r:    [ret]    0: no error, -1: error
 */
int32_t ecp_fclose(xFILE *fp)
{
	struct stdio_file *f = (struct stdio_file *)fp;

	SPIN_LOCK(&f->lock);
	int32_t ret = _stdio_fclose(fp);
	if (ret)
		SPIN_UNLOCK(&f->lock);

	return ret;
}

/* @func: ecp_setvbuf
 * #desc:
 *    stream buffer operations function.
 *
 * #1: fp   [in/out] stdio file struct
 * #2: buf  [in/out] stream buffer / NULL
 * #3: type [in]     buffer type
 * #4: size [in]     buffer size
 * #r:      [ret]    0: no error, -1: error
 */
int32_t ecp_setvbuf(xFILE *fp, uint8_t *buf, int32_t type, size_t size)
{
	struct stdio_file *f = (struct stdio_file *)fp;

	SPIN_LOCK(&f->lock);
	int32_t ret = _stdio_setvbuf(fp, buf, type, size);
	SPIN_UNLOCK(&f->lock);

	return ret;
}

/* @func: ecp_fgetpos
 * #desc:
 *    get the stream position.
 *
 * #1: fp  [in]  stdio file struct
 * #2: pos [out] position pointer
 * #r:     [ret] 0: no error, -1: error
 */
int32_t ecp_fgetpos(xFILE *fp, xfpos_t *pos)
{
	struct stdio_file *f = (struct stdio_file *)fp;

	SPIN_LOCK(&f->lock);
	int32_t ret = _stdio_fgetpos(fp, pos);
	SPIN_UNLOCK(&f->lock);

	return ret;
}

/* @func: ecp_fsetpos
 * #desc:
 *    set the stream position.
 *
 * #1: fp  [in]  stdio file struct
 * #2: pos [out] position pointer
 * #r:     [ret] 0: no error, -1: error
 */
int32_t ecp_fsetpos(xFILE *fp, const xfpos_t *pos)
{
	struct stdio_file *f = (struct stdio_file *)fp;

	SPIN_LOCK(&f->lock);
	int32_t ret = _stdio_fsetpos(fp, pos);
	SPIN_UNLOCK(&f->lock);

	return ret;
}

/* @file: stdio.h
 * #desc:
 *    The definitions of standard input/output.
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

#ifndef _ECP_C_STDIO_H
#define _ECP_C_STDIO_H

#include <ecp/config.h>
#include <ecp/c/stddef.h>
#include <ecp/c/stdint.h>
#include <ecp/c/stdarg.h>

#define X_EOF (-1) /* end-of-file */

#define X_SEEK_SET 0 /* absolute */
#define X_SEEK_CUR 1 /* relative */
#define X_SEEK_END 2 /* tail start */

#define X_IOFBF 0 /* fully buffer */
#define X_IOLBF 1 /* line buffer */
#define X_IONBF 2 /* not buffer */

#define X_BUFSIZ       4096  /* buffer size */
#define X_FOPEN_MAX    1000  /* min number of open file */
#define X_FILENAME_MAX 4096  /* max length of filename */
#define X_TMP_MAX      10000 /* max number of tmpfile */
#define X_L_tmpnam     20    /* name length of tmpfile */

typedef struct {
	char _; /* internal implemented */
} xFILE;

typedef int64_t xfpos_t;

#define x_stdin __ecp_stdin
#define x_stdout __ecp_stdout
#define x_stderr __ecp_stderr

/* posix */
#define X_STDIN_FILENO  0
#define X_STDOUT_FILENO 1
#define X_STDERR_FILENO 2

#ifdef __cplusplus
extern "C" {
#endif

/* c/stdio__printf.c */
extern
int32_t __ecp_printf(const char *fmt, va_list *ap, void *arg,
		int32_t (*out)(const char *, int32_t, void *))
;

/* c/stdio__scanf.c */
extern
int32_t __ecp_scanf(const char *fmt, va_list *ap, void *arg,
		int32_t (*get)(int32_t, void *))
;

/* c/stdio_io.c */
extern
xFILE *__ecp_stdin
;
extern
xFILE *__ecp_stdout
;
extern
xFILE *__ecp_stderr
;
extern
int32_t ecp_feof(xFILE *fp)
;
extern
int32_t ecp_ferror(xFILE *fp)
;
extern
void ecp_clearerr(xFILE *fp)
;
extern
int32_t ecp_fflush(xFILE *fp)
;
extern
size_t ecp_fread(void *t, size_t m, size_t n, xFILE *fp)
;
extern
size_t ecp_fwrite(const void *s, size_t m, size_t n, xFILE *fp)
;
extern
int32_t ecp_fseek(xFILE *fp, int64_t off, int32_t whence)
;
extern
int64_t ecp_ftell(xFILE *fp)
;
extern
void ecp_rewind(xFILE *fp)
;
extern
int32_t ecp_fgetc(xFILE *fp)
;
extern
char *ecp_fgets(char *s, int32_t len, xFILE *fp)
;
extern
int32_t ecp_fputc(int32_t c, xFILE *fp)
;
extern
int32_t ecp_fputs(const char *s, xFILE *fp)
;
extern
xFILE *ecp_fopen(const char *path, const char *mode)
;
extern
int32_t ecp_fclose(xFILE *fp)
;
extern
int32_t ecp_setvbuf(xFILE *fp, uint8_t *buf, int32_t type, size_t size)
;
extern
int32_t ecp_fgetpos(xFILE *fp, xfpos_t *pos)
;
extern
int32_t ecp_fsetpos(xFILE *fp, const xfpos_t *pos)
;

/* c/stdio_printf.c */
extern
int32_t ecp_vfprintf(xFILE *fp, const char *fmt, va_list ap)
;
extern
int32_t ecp_fprintf(xFILE *fp, const char *fmt, ...)
;
extern
int32_t ecp_vprintf(const char *fmt, va_list ap)
;
extern
int32_t ecp_printf(const char *fmt, ...)
;

/* c/stdio_scanf.c */
extern
int32_t ecp_vfscanf(xFILE *fp, const char *fmt, va_list ap)
;
extern
int32_t ecp_vscanf(const char *fmt, va_list ap)
;
extern
int32_t ecp_scanf(const char *fmt, ...)
;

/* c/stdio_snprintf.c */
extern
int32_t ecp_vsnprintf(char *buf, size_t len, const char *fmt, va_list ap)
;
extern
int32_t ecp_snprintf(char *buf, size_t len, const char *fmt, ...)
;

/* c/stdio_sscanf.c */
extern
int32_t ecp_vsscanf(const char *s, const char *fmt, va_list ap)
;
extern
int32_t ecp_sscanf(const char *s, const char *fmt, ...)
;

#ifdef __cplusplus
}
#endif

#endif

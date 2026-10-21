/* @file: stdio_printf.c
 * #desc:
 *    The implementations of C standard input/output.
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
#include <ecp/c/stdarg.h>
#include <ecp/c/string.h>
#include <ecp/c/stdio.h>

#define BUF_SIZE 512

struct out_ctx {
	xFILE *fp;
	int32_t len;
	int32_t buf_len;
	char buf[BUF_SIZE];
};

/* @func: _flush_out (static)
 * #desc:
 *    printf output buffer flush.
 *
 * #1: ctx [in/out] output buffer context
 * #r:     [ret]    0: no error, -1: output error
 */
static int32_t _flush_out(struct out_ctx *ctx)
{
	size_t r = ecp_fwrite(ctx->buf, 1, (size_t)ctx->buf_len, ctx->fp);
	if (r != (size_t)ctx->buf_len && ecp_ferror(ctx->fp))
		return -1;

	ctx->len += ctx->buf_len;
	ctx->buf_len = 0;

	return 0;
}

/* @func: _call_out (static)
 * #desc:
 *    printf output callback function.
 *
 * #1: s   [in]     input buffer
 * #2: len [in]     input length
 * #3: arg [in/out] output buffer context
 * #r:     [ret]    0: no error, -1: output error
 */
static int32_t _call_out(const char *s, int32_t len, void *arg)
{
	struct out_ctx *ctx = arg;
	int32_t n;

	do {
		if (len > (BUF_SIZE - ctx->buf_len)) {
			n = BUF_SIZE - ctx->buf_len;
			ecp_memcpy(ctx->buf + ctx->buf_len, s, n);
			ctx->buf_len += n;
			s += n;
			len -= n;

			if (_flush_out(ctx))
				return -1;
		} else {
			ecp_memcpy(ctx->buf + ctx->buf_len, s, len);
			ctx->buf_len += len;
			len = 0;
		}
	} while (len);

	return 0;
}

/* @func: ecp_vfprintf
 * #desc:
 *    formatted output conversion.
 *
 * #1: fp  [in/out] stdio file struct
 * #2: fmt [in]     format string
 * #3: ap  [in]     variable parameter
 * #r:     [ret]    >=0: output length, -1: error
 */
int32_t ecp_vfprintf(xFILE *fp, const char *fmt, va_list ap)
{
	struct out_ctx ctx;
	ctx.fp = fp; ctx.len = 0; ctx.buf_len = 0;
	va_list _ap;
	va_copy(_ap, ap);

	if (__ecp_printf(fmt, &_ap, &ctx, _call_out)) {
		va_end(_ap);
		_flush_out(&ctx);
		return -1;
	}

	va_end(_ap);
	if (_flush_out(&ctx))
		return -1;

	return ctx.len;
}

/* @func: ecp_fprintf
 * #desc:
 *    formatted output conversion.
 *
 * #1: fp  [in/out] stdio file struct
 * #2: fmt [in]     format string
 * #N:     [in]     variable parameter
 * #r:     [ret]    >=0: output length, -1: error
 */
int32_t ecp_fprintf(xFILE *fp, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);

	int32_t ret = ecp_vfprintf(fp, fmt, ap);
	va_end(ap);

	return ret;
}

/* @func: ecp_vprintf
 * #desc:
 *    formatted output conversion.
 *
 * #1: fmt [in]  format string
 * #2: ap  [in]  variable parameter
 * #r:     [ret] >=0: output length, -1: error
 */
int32_t ecp_vprintf(const char *fmt, va_list ap)
{
	va_list _ap;
	va_copy(_ap, ap);

	int32_t ret = ecp_vfprintf(x_stdout, fmt, _ap);
	va_end(_ap);

	return ret;
}

/* @func: ecp_printf
 * #desc:
 *    formatted output conversion.
 *
 * #1: fmt [in]  format string
 * #N:     [in]  variable parameter
 * #r:     [ret] >=0: output length, -1: error
 */
int32_t ecp_printf(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);

	int32_t ret = ecp_vfprintf(x_stdout, fmt, ap);
	va_end(ap);

	return ret;
}

/* @file: stdio_snprintf.c
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

struct out_ctx {
	char *p;
	char *e;
	int32_t len;
};

/* @func: _call_out (static)
 * #desc:
 *    snprintf output callback function.
 *
 * #1: s   [in]     input buffer
 * #2: len [in]     input length
 * #3: arg [in/out] output buffer context
 * #r:     [ret]    0: no error, -1: output error
 */
static int32_t _call_out(const char *s, int32_t len, void *arg)
{
	struct out_ctx *ctx = arg;

	if (len) {
		ctx->len += len;
		if (ctx->p < ctx->e) {
			if ((ctx->p + len) > ctx->e)
				len = (int32_t)(ctx->e - ctx->p);

			ecp_memcpy(ctx->p, s, len);
			ctx->p += len;
		} else {
			return -1;
		}
	}

	return 0;
}

/* @func: ecp_vsnprintf
 * #desc:
 *    convert formatted output to the buffer.
 *
 * #1: buf [out] output buffer
 * #2: len [in]  buffer length
 * #3: fmt [in]  format string
 * #4: ap  [in]  variable parameter
 * #r:     [ret] >=0: output length, -1: error
 */
int32_t ecp_vsnprintf(char *buf, size_t len, const char *fmt, va_list ap)
{
	if (!len)
		return 0;

	struct out_ctx ctx = { .p = buf, .e = buf + len - 1, .len = 0 };
	va_list _ap;
	va_copy(_ap, ap);

	if (__ecp_printf(fmt, &_ap, &ctx, _call_out)) {
		va_end(_ap);
		return -1;
	}

	va_end(_ap);

	return ctx.len;
}

/* @func: ecp_snprintf
 * #desc:
 *    convert formatted output to the buffer.
 *
 * #1: buf [out] output buffer
 * #2: len [in]  buffer length
 * #3: fmt [in]  format string
 * #N:     [in]  variable parameter
 * #r:     [ret] >=0: output length, -1: error
 */
int32_t ecp_snprintf(char *buf, size_t len, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);

	int32_t ret = ecp_vsnprintf(buf, len, fmt, ap);
	va_end(ap);

	return ret;
}

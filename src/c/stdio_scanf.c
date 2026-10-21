/* @file: stdio_sacnf.c
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
#include <ecp/c/stdint.h>
#include <ecp/c/stdarg.h>
#include <ecp/c/stdio.h>


struct getc_ctx {
	xFILE *fp;
	int32_t st;
	int32_t c;
};


/* @func: _call_getc (static)
 * #desc:
 *    get the input character callback function.
 *
 * #1: peek [in]     is peek
 * #2: arg  [in/out] input buffer context
 * #r:      [ret]    0: no error, -1: error
 */
static int32_t _call_getc(int32_t peek, void *arg)
{
	struct getc_ctx *ctx = arg;
	int32_t c;

	switch (ctx->st) {
		case 0:
			ctx->c = c = ecp_fgetc(ctx->fp);
			if (peek)
				ctx->st = 1;
			break;
		case 1:
			c = ctx->c;
			if (!peek)
				ctx->st = 0;
			break;
		default:
			return -1;
	}

	return c;
}

/* @func: ecp_vfscanf
 * #desc:
 *    input string formatted value conversion.
 *
 * #1: fp  [in]  stdio file struct
 * #2: fmt [in]  format string
 * #3: ap  [out] variable parameter
 * #r:     [ret] >0: formatted value number
 */
int32_t ecp_vfscanf(xFILE *fp, const char *fmt, va_list ap)
{
	struct getc_ctx ctx = { fp, 0, 0 };
	va_list _ap;
	va_copy(_ap, ap);

	int32_t ret = __ecp_scanf(fmt, &ap, &ctx, _call_getc);
	if (ret < 0) {
		va_end(_ap);
		return -ret;
	}

	va_end(_ap);

	return ret;
}

/* @func: ecp_vscanf
 * #desc:
 *    input string formatted value conversion.
 *
 * #1: fmt [in]  format string
 * #2: ap  [out] variable parameter
 * #r:     [ret] >0: formatted value number
 */
int32_t ecp_vscanf(const char *fmt, va_list ap)
{
	va_list _ap;
	va_copy(_ap, ap);

	int32_t ret = ecp_vfscanf(x_stdin, fmt, _ap);
	va_end(_ap);

	return ret;
}

/* @func: ecp_scanf
 * #desc:
 *    input string formatted value conversion.
 *
 * #1: fmt [in]  format string
 * #N:     [out] variable parameter
 * #r:     [ret] >0: formatted value number
 */
int32_t ecp_scanf(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);

	int32_t ret = ecp_vfscanf(x_stdin, fmt, ap);
	va_end(ap);

	return ret;
}

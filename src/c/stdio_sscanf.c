/* @file: stdio_sscanf.c
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
#include <ecp/c/stdint.h>
#include <ecp/c/stdarg.h>
#include <ecp/c/stdio.h>

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
	const char **p = (const char **)arg;

	return peek ? **p : *((*p)++);
}

/* @func: ecp_vsscanf
 * #desc:
 *    input string formatted value conversion.
 *
 * #1: s   [in]  input string
 * #2: fmt [in]  format string
 * #3: ap  [out] variable parameter
 * #r:     [ret] >0: formatted value number
 */
int32_t ecp_vsscanf(const char *s, const char *fmt, va_list ap)
{
	const char *p = s;
	va_list _ap;
	va_copy(_ap, ap);

	int32_t ret = __ecp_scanf(fmt, &_ap, (void *)&p, _call_getc);
	if (ret < 0) {
		va_end(_ap);
		return -ret;
	}

	va_end(_ap);

	return ret;
}

/* @func: ecp_sscanf
 * #desc:
 *    input string formatted value conversion.
 *
 * #1: s   [in]  input string
 * #2: fmt [in]  format string
 * #N:     [out] variable parameter
 * #r:     [ret] >0: formatted value number
 */
int32_t ecp_sscanf(const char *s, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);

	int32_t ret = ecp_vsscanf(s, fmt, ap);
	va_end(ap);

	return ret;
}

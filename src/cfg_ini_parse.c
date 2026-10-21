/* @file: cfg_ini_parse.c
 * #desc:
 *    The implementations of ini (initial configuration) parser.
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
#include <ecp/c/ctype.h>
#include <ecp/cfg_ini.h>

// [a] a
// [ b ] b
//  [ a b ] c
// 
// # ;
// ; #
// ω=
// 
// a=;
//  b = 	# ω	#
//  	c = ω

/* @func: ecp_ini_parse
 * #desc:
 *    ini (initial configuration) parser.
 *
 * #1: s        [in]  input string
 * #2: err_line [out] error line
 * #3: arg      [in]  callback arg
 * #4: call     [in]  callback (type, string, length, arg)
 * #r:          [ret] 0: no error, -1: parse error, -2: callback error
 */
int32_t ecp_ini_parse(const char *s, int32_t *err_line, void *arg,
		int32_t (*call)(int32_t, const char *, int32_t, void *))
{
	const char *p, *p2;
	int32_t len;
	size_t n;
	*err_line = 0;

	while (*s != '\0') {
		(*err_line)++;

		p = s;
		n = ecp_strcspn(s, "\n");
		s += n + 1;
		if (!n || *p == '\r') /* '\n' and '\r' next */
			continue;

		p += ecp_strspn(p, "\t ");
		if (ecp_strchr("#;", *p)) /* '#' or ';' next */
			continue;

		if (*p == '[') {
			p++;
			p2 = p += ecp_strspn(p, " ");
			len = 0;

			/* '['\s*'<section>'\s*']' */
			for (; *p != '\0'; p++) {
				if (ecp_strchr("]", *p))
					break;
				if ((uint8_t)*p > 0x7f
						|| ecp_isprint(*p)) {
					len++;
					continue;
				}
				return -1;
			}
			if (*p != ']')
				return -1;

			/* skip the tail space */
			for (p--; p != p2 && ecp_strchr("\t ", *p); p--)
				len--;

			/* section */
			if (call(INI_SECTION_TYPE, p2, len, arg))
				return -2;
		} else {
			p2 = p;
			len = 0;

			/* '<key>'\s*= */
			for (; *p != '\0'; p++) {
				if (ecp_strchr("\t =", *p)) {
					p += ecp_strspn(p, "\t ");
					break;
				}
				if ((uint8_t)*p > 0x7f
						|| ecp_isprint(*p)) {
					len++;
					continue;
				}
				return -1;
			}
			if (*p++ != '=')
				return -1;

			/* key */
			if (call(INI_KEY_TYPE, p2, len, arg))
				return -2;

			p2 = p += ecp_strspn(p, "\t ");
			len = 0;
			if (*p == '\0' || ecp_strchr("\n", *p)) /* next */
				continue;

			/* =\s*'<value>'\s* */
			for (; *p != '\0'; p++) {
				if (ecp_strchr("\n", *p))
					break;
				if (*p == '\t' || (uint8_t)*p > 0x7f
						|| ecp_isprint(*p)) {
					len++;
					continue;
				}
				return -1;
			}

			/* skip the tail space */
			for (p--; p != p2 && ecp_strchr("\t ", *p); p--)
				len--;

			/* value */
			if (call(INI_VALUE_TYPE, p2, len, arg))
				return -2;
		}
	}

	return 0;
}

/* @file: string_strdup.c
 * #desc:
 *    The implementations of string operations.
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
#include <ecp/c/string.h>
#include <ecp/c/stdlib.h>

/* @func: ecp_strndup
 * #desc:
 *    duplicate a string.
 *
 * #1: s   [in]  input string
 * #2: len [in]  input length
 * #r:     [ret] return a new string
 */
char *ecp_strndup(const char *s, size_t len)
{
	char *p = ecp_malloc(len + 1);
	p[len] = '\0';

	ecp_memcpy(p, s, len);

	return p;
}

/* @func: ecp_strdup
 * #desc:
 *    duplicate a string.
 *
 * #1: s [in]  input string
 * #r:   [ret] return a new string
 */
char *ecp_strdup(const char *s)
{
	return ecp_strndup(s, ecp_strlen(s));
}

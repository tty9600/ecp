/* @file: assert.h
 * #desc:
 *    The definitions of program assertion.
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

#ifndef _ECP_C_ASSERT_H
#define _ECP_C_ASSERT_H

#include <ecp/config.h>

#undef x_assert
#ifdef NDEBUG
# define x_assert(x) ((void)0)
#else
# define x_assert(x) ((void)((x) \
	|| (__ecp_assert(#x, __FILE__, __LINE__, __func__), 0)))
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* c/assert.c */
extern
void __ecp_assert(const char *expr, const char *file, long line,
		const char *func)
;

#ifdef __cplusplus
}
#endif

#endif

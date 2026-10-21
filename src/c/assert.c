/* @file: assert.c
 * #desc:
 *    The implementation of program assertion.
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
#include <ecp/c/assert.h>

/* @func: __ecp_assert
 * #desc:
 *    internal function for program assertion.
 *
 * #1: expr [in] if expression assertion
 * #2: file [in] assertion file name
 * #3: line [in] assertion file line
 * #4: func [in] assertion function name
 */
void __ecp_assert(const char *expr, const char *file, long line,
		const char *func)
{
	volatile char *p = (char *)0;
	*p = 0;

	(void)expr;
	(void)file;
	(void)line;
	(void)func;
}

/* @file: stdlib_abs.c
 * #desc:
 *    The implementations of standard library.
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
#include <ecp/c/stdlib.h>

/* @func: ecp_abs
 * #desc:
 *    returns the integer absolute value.
 *
 * #1: n [in]  number
 * #r:   [ret] absolute value
 */
int ecp_abs(int n)
{
	return (n < 0) ? -n : n;
}

/* @func: ecp_labs
 * #desc:
 *    returns the integer absolute value.
 *
 * #1: n [in]  number
 * #r:   [ret] absolute value
 */
long ecp_labs(long n)
{
	return (n < 0) ? -n : n;
}

/* @func: ecp_llabs
 * #desc:
 *    returns the integer absolute value.
 *
 * #1: n [in]  number
 * #r:   [ret] absolute value
 */
long long ecp_llabs(long long n)
{
	return (n < 0) ? -n : n;
}

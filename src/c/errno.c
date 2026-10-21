/* @file: errno.c
 * #desc:
 *    The implementations of system error numbers.
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
#include <ecp/c/errno.h>

/* @func: __ecp_errno
 * #desc:
 *    internal function for get and set error number.
 *
 * #r: [ret] return the errno internal pointer
 */
int32_t *__ecp_errno(void)
{
	/* TODO: not thread safe */
	static int32_t a;

	return &a;
}

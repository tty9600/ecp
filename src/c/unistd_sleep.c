/* @file: unistd_sleep.c
 * #desc:
 *    The implementations of unix standard.
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
#include <ecp/c/time.h>
#include <ecp/c/unistd.h>

/* @func: ecp_sleep
 * #desc:
 *    sleep for a specified number of seconds.
 *
 * #1: n [in]  seconds
 * #r:   [ret] 0: no error, -1: errno
 */
int32_t ecp_sleep(uint32_t n)
{
	struct xtimespec a, b;
	a.tv_sec = n;
	a.tv_nsec = 0;

	if (ecp_nanosleep(&a, &b))
		return -1;

	return b.tv_sec;
}

/* @file: time_timer.c
 * #desc:
 *    The implementations of timer functions.
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
#include <ecp/c/sys_types.h>
#include <ecp/c/errno.h>
#include <ecp/c/time.h>
#include <ecp/c/syscall.h>

/* @func: ecp_setitimer
 * #desc:
 *    set the new interval timer.
 *
 * #1: which [in]  timer type
 * #2: new   [in]  new itimerval
 * #3: old   [out] old itimerval / NULL
 * #r:       [ret] 0: no error, -1: errno
 */
int32_t ecp_setitimer(int32_t which, const struct xitimerval *new,
		struct xitimerval *old)
{
#if defined(ECP_PLATFORM_LINUX)

	int32_t ret;

	ret = (int32_t)ecp_syscall_linux(__NR_setitimer,
		which,
		new,
		old);

	if (SYSCALL_LINUX_ISERR(ret)) {
		x_errno = -ret;
		return -1;
	}

	return ret;

#else
# error "!!!unknown platform!!!"
#endif
}

/* @func: ecp_getitimer
 * #desc:
 *    get the current interval timer.
 *
 * #1: which [in]  timer type
 * #2: cur   [out] current itimerval
 * #r:       [ret] 0: no error, -1: errno
 */
int32_t ecp_getitimer(int32_t which, struct xitimerval *cur)
{
#if defined(ECP_PLATFORM_LINUX)

	int32_t ret;

	ret = (int32_t)ecp_syscall_linux(__NR_getitimer,
		which,
		cur);

	if (SYSCALL_LINUX_ISERR(ret)) {
		x_errno = -ret;
		return -1;
	}

	return ret;

#else
# error "!!!unknown platform!!!"
#endif
}

/* @file: sys_wait.c
 * #desc:
 *    The implementations of wait for child process to change state.
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
#include <ecp/c/errno.h>
#include <ecp/c/sys_wait.h>
#include <ecp/c/syscall.h>

/* @func: ecp_waitpid
 * #desc:
 *    wait for child process to change state.
 *
 * #1: pid     [in]  process id (-1: all child)
 * #2: wstatus [out] child process status
 * #3: option  [in]  wait options
 * #r:         [ret] 0: no error, -1: errno
 */
int32_t ecp_waitpid(int32_t pid, int32_t *wstatus, int32_t option)
{
#if defined(ECP_PLATFORM_LINUX)

	int32_t ret;

	ret = (int32_t)ecp_syscall_linux(__NR_wait4,
		pid,
		wstatus,
		option,
		NULL);

	if (SYSCALL_LINUX_ISERR(ret)) {
		x_errno = -ret;
		return -1;
	}

	return ret;

#else
# error "!!!unknown platform!!!"
#endif
}

/* @func: ecp_wait
 * #desc:
 *    wait for child process to change state.
 *
 * #1: wstatus [out] child process status
 * #r:         [ret] 0: no error, -1: errno
 */
int32_t ecp_wait(int32_t *wstatus)
{
	return ecp_waitpid(-1, wstatus, 0);
}

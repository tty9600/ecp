/* @file: unistd_fork.c
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
#include <ecp/c/stdint.h>
#include <ecp/c/errno.h>
#include <ecp/c/signal.h>
#include <ecp/c/unistd.h>
#include <ecp/c/syscall.h>

/* @func: ecp_fork
 * #desc:
 *    create a child process.
 *
 * #r: [ret] 0: current process, >0: child process id, -1: errno
 */
int32_t ecp_fork(void)
{
#if defined(ECP_PLATFORM_LINUX)

	int32_t ret;

	ret = (int32_t)ecp_syscall_linux(__NR_clone,
		X_SIGCHLD,
		0);

	if (SYSCALL_LINUX_ISERR(ret)) {
		x_errno = -ret;
		return -1;
	}

	return ret;

#else
# error "!!!unknown platform!!!"
#endif
}

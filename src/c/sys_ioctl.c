/* @file: sys_ioctl.c
 * #desc:
 *    The implementations of control device.
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
#include <ecp/c/stdarg.h>
#include <ecp/c/errno.h>
#include <ecp/c/sys_ioctl.h>
#include <ecp/c/syscall.h>

/* @func: ecp_ioctl
 * #desc:
 *   control device based on file descriptor.
 *
 * #1: fd  [in]     file descriptor
 * #2: cmd [in]     command
 * #N:     [in/out] command argument
 * #r:     [ret]    return value of command
 */
int32_t ecp_ioctl(int32_t fd, int32_t cmd, ...)
{
#if defined(ECP_PLATFORM_LINUX)

	int32_t ret;
	va_list ap;
	va_start(ap, cmd);

	unsigned long arg = va_arg(ap, unsigned long);
	va_end(ap);

	ret = (int32_t)ecp_syscall_linux(__NR_ioctl,
		fd,
		cmd,
		arg);

	if (SYSCALL_LINUX_ISERR(ret)) {
		x_errno = -ret;
		return -1;
	}

	return ret;

#else
# error "!!!unknown platform!!!"
#endif
}

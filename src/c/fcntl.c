/* @file: fcntl.c
 * #desc:
 *    The implementations of file control options.
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
#include <ecp/c/sys_types.h>
#include <ecp/c/fcntl.h>
#include <ecp/c/errno.h>
#include <ecp/c/syscall.h>

/* @func: ecp_fcntl
 * #desc:
 *    file descriptor operations.
 *
 * #1: fd  [in]     file descriptor
 * #2: cmd [in]     command
 * #N:     [in/out] command argument
 * #r:     [ret]    return value of command
 */
int32_t ecp_fcntl(int32_t fd, int32_t cmd, ...)
{
#if defined(ECP_PLATFORM_LINUX)

	int32_t ret;
	va_list ap;
	va_start(ap, cmd);

	unsigned long arg = va_arg(ap, unsigned long);
	va_end(ap);

#if defined(ECP_MARCH_BITS_32)

	ret = (int32_t)ecp_syscall_linux(__NR_fcntl64,
		fd,
		cmd,
		arg);

#elif defined(ECP_MARCH_BITS_64)

	ret = (int32_t)ecp_syscall_linux(__NR_fcntl,
		fd,
		cmd,
		arg);

#else
# error "!!!unknown machine bits!!!"
#endif

	if (SYSCALL_LINUX_ISERR(ret)) {
		x_errno = -ret;
		return -1;
	}

	return ret;

#else
# error "!!!unknown platform!!!"
#endif
}

/* @func: _openat (static)
 * #desc:
 *    open and possibly create a file (TOCTOU).
 *
 * #1: dfd   [in]  directory file descriptor
 * #2: path  [in]  path name
 * #3: flags [in]  open flags
 * #4: mode  [in]  file mode (O_CREAT)
 * #r:       [ret] >=0: file descriptor, -1: errno
 */
static int32_t _openat(int32_t dfd, const char *path, int32_t flags,
		uint32_t mode)
{
#if defined(ECP_PLATFORM_LINUX)

	int32_t ret;

	ret = (int32_t)ecp_syscall_linux(__NR_openat,
		dfd,
		path,
		flags,
		mode);

	if (SYSCALL_LINUX_ISERR(ret)) {
		x_errno = -ret;
		return -1;
	}

	if (flags & X_O_CLOEXEC)
		ecp_fcntl(ret, X_F_SETFD, X_FD_CLOEXEC);

	return ret;

#else
# error "!!!unknown platform!!!"
#endif
}

/* @func: ecp_openat
 * #desc:
 *    open and possibly create a file (TOCTOU).
 *
 * #1: dfd   [in]  directory file descriptor
 * #2: path  [in]  path name
 * #3: flags [in]  open flags
 * #N:       [in]  file mode (O_CREAT)
 * #r:       [ret] >=0: file descriptor, -1: errno
 */
int32_t ecp_openat(int32_t dfd, const char *path, int32_t flags, ...)
{
	uint32_t mode = 0;
	va_list ap;
	va_start(ap, flags);

	if (flags & X_O_CREAT)
		mode = va_arg(ap, uint32_t);

	va_end(ap);

	return _openat(dfd, path, flags, mode);
}

/* @func: ecp_open
 * #desc:
 *    open and possibly create a file.
 *
 * #1: path  [in]  path name
 * #2: flags [in]  open flags
 * #N:       [in]  file mode (O_CREAT)
 * #r:       [ret] >=0: file descriptor, -1: errno
 */
int32_t ecp_open(const char *path, int32_t flags, ...)
{
	uint32_t mode = 0;
	va_list ap;
	va_start(ap, flags);

	if (flags & X_O_CREAT)
		mode = va_arg(ap, uint32_t);

	va_end(ap);

	return _openat(X_AT_FDCWD, path, flags, mode);
}

/* @func: ecp_creat
 * #desc:
 *    possibly create a file.
 *
 * #1: path [in]  path name
 * #2: mode [in]  file mode
 * #r:      [ret] 0: no errno, -1: errno
 */
int32_t ecp_creat(const char *path, uint32_t mode)
{
	return ecp_open(path, X_O_CREAT | X_O_WRONLY | X_O_TRUNC, mode);
}

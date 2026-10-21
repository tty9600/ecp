/* @file: sys_stat.c
 * #desc:
 *    The implementations of file stat structure.
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
#include <ecp/c/sys_types.h>
#include <ecp/c/sys_stat.h>
#include <ecp/c/fcntl.h>
#include <ecp/c/time.h>
#include <ecp/c/syscall.h>

/* @func: ecp_umask
 * #desc:
 *    set file mode creation mask.
 *
 * #1: mode [in]  file mode
 * #r:      [ret] return the old umask
 */
xmode_t ecp_umask(xmode_t mode)
{
#if defined(ECP_PLATFORM_LINUX)

	return (xmode_t)ecp_syscall_linux(__NR_umask,
		mode);

#else
# error "!!!unknown platform!!!"
#endif
}

/* @func: ecp_fchmodat
 * #desc:
 *    change file permissions.
 *
 * #1: dfd   [in]  directory file descriptor
 * #2: path  [in]  path name
 * #3: mode  [in]  file mode
 * #4: flags [in]  flags
 * #r:       [ret] 0: no error, -1: errno
 */
int32_t ecp_fchmodat(int32_t dfd, const char *path, xmode_t mode,
		int32_t flags)
{
#if defined(ECP_PLATFORM_LINUX)

	int32_t ret;

	ret = (int32_t)ecp_syscall_linux(__NR_fchmodat,
		dfd,
		path,
		mode,
		flags);

	if (SYSCALL_LINUX_ISERR(ret)) {
		x_errno = -ret;
		ret = -1;
	}

	return ret;

#else
# error "!!!unknown platform!!!"
#endif
}

/* @func: ecp_chmod
 * #desc:
 *    change file permissions.
 *
 * #1: path [in]  path name
 * #2: mode [in]  file mode
 * #r:      [ret] 0: no error, -1: errno
 */
int32_t ecp_chmod(const char *path, xmode_t mode)
{
	return ecp_fchmodat(X_AT_FDCWD, path, mode, 0);
}

/* @func: ecp_fchmod
 * #desc:
 *    change file permissions.
 *
 * #1: fd   [in]  file descriptor
 * #2: mode [in]  file mode
 * #r:      [ret] 0: no error, -1: errno
 */
int32_t ecp_fchmod(int32_t fd, xmode_t mode)
{
#if defined(ECP_PLATFORM_LINUX)

	int32_t ret;

	ret = (int32_t)ecp_syscall_linux(__NR_fchmod,
		fd,
		mode);

	if (SYSCALL_LINUX_ISERR(ret)) {
		x_errno = -ret;
		ret = -1;
	}

	return ret;

#else
# error "!!!unknown platform!!!"
#endif
}

/* @func: ecp_fstatat
 * #desc:
 *    get the file status.
 *
 * #1: dfd   [in]  directory file descriptor
 * #2: path  [in]  path name
 * #3: st    [out] stat struct
 * #4: flags [in]  flags
 * #r:       [ret] 0: no error, -1: errno
 */
int32_t ecp_fstatat(int32_t dfd, const char *path, struct xstat *st,
		int32_t flags)
{
#if defined(ECP_PLATFORM_LINUX)

	int32_t ret;

#if (defined(ECP_MARCH_X86_32) || defined(ECP_MARCH_ARM_32) \
	|| defined(ECP_MARCH_RISCV_32))

	ret = (int32_t)ecp_syscall_linux(__NR_fstatat64,
		dfd,
		path,
		st,
		flags);

#elif (defined(ECP_MARCH_X86_64) || defined(ECP_MARCH_ARM_64) \
	|| defined(ECP_MARCH_RISCV_64))

	ret = (int32_t)ecp_syscall_linux(__NR_newfstatat,
		dfd,
		path,
		st,
		flags);

#else
# error "!!!unknown architecture!!!"
#endif

	if (SYSCALL_LINUX_ISERR(ret)) {
		x_errno = -ret;
		ret = -1;
	}

	return ret;

#else
# error "!!!unknown platform!!!"
#endif
}

/* @func: ecp_stat
 * #desc:
 *    get the file status.
 *
 * #1: path [in]  path name
 * #2: st   [out] stat struct
 * #r:      [ret] 0: no error, -1: errno
 */
int32_t ecp_stat(const char *path, struct xstat *st)
{
	return ecp_fstatat(X_AT_FDCWD, path, st, 0);
}

/* @func: ecp_fstat
 * #desc:
 *    get the file status.
 *
 * #1: fd [in]  file descriptor
 * #2: st [out] stat struct
 * #r:    [ret] 0: no error, -1: errno
 */
int32_t ecp_fstat(int32_t fd, struct xstat *st)
{
	return ecp_fstatat(fd, NULL, st, 0);
}

/* @func: ecp_lstat
 * #desc:
 *    get the file status (not dereferences symbolic link).
 *
 * #1: path [in]  path name
 * #2: st   [out] stat struct
 * #r:      [ret] 0: no error, -1: errno
 */
int32_t ecp_lstat(const char *path, struct xstat *st)
{
	return ecp_fstatat(X_AT_FDCWD, path, st, X_AT_SYMLINK_NOFOLLOW);
}

/* @func: ecp_mkdirat
 * #desc:
 *    create a directory.
 *
 * #1: dfd  [in]  directory file descriptor
 * #2: path [in]  path name
 * #3: mode [in]  file mode
 * #r:      [ret] 0: no error, -1: errno
 */
int32_t ecp_mkdirat(int32_t dfd, const char *path, xmode_t mode)
{
#if defined(ECP_PLATFORM_LINUX)

	int32_t ret;

	ret = (int32_t)ecp_syscall_linux(__NR_mkdirat,
		dfd,
		path,
		mode);

	if (SYSCALL_LINUX_ISERR(ret)) {
		x_errno = -ret;
		ret = -1;
	}

	return ret;

#else
# error "!!!unknown platform!!!"
#endif
}

/* @func: ecp_mkdir
 * #desc:
 *    create a directory.
 *
 * #1: path [in]  path name
 * #2: mode [in]  file mode
 * #r:      [ret] 0: no error, -1: errno
 */
int32_t ecp_mkdir(const char *path, xmode_t mode)
{
	return ecp_mkdirat(X_AT_FDCWD, path, mode);
}

/* @func: ecp_mknodat
 * #desc:
 *    create a special or ordinary file.
 *
 * #1: dfd  [in]  directory file descriptor
 * #2: path [in]  path name
 * #3: mode [in]  file mode
 * #4: dev  [in]  device id
 * #r:      [ret] 0: no error, -1: errno
 */
int32_t ecp_mknodat(int32_t dfd, const char *path, xmode_t mode,
		xdev_t dev)
{
#if defined(ECP_PLATFORM_LINUX)

	int32_t ret;

	ret = (int32_t)ecp_syscall_linux(__NR_mknodat,
		dfd,
		path,
		mode,
		dev);

	if (SYSCALL_LINUX_ISERR(ret)) {
		x_errno = -ret;
		ret = -1;
	}

	return ret;

#else
# error "!!!unknown platform!!!"
#endif
}

/* @func: ecp_mknod
 * #desc:
 *    create a special or ordinary file.
 *
 * #1: path [in]  path name
 * #2: mode [in]  file mode
 * #3: dev  [in]  device id
 * #r:      [ret] 0: no error, -1: errno
 */
int32_t ecp_mknod(const char *path, xmode_t mode, xdev_t dev)
{
	return ecp_mknodat(X_AT_FDCWD, path, mode, dev);
}

/* @func: ecp_utimensat
 * #desc:
 *    change file timestamps with nanosecond precision.
 *
 * #1: dfd   [in]  directory file descriptor
 * #2: path  [in]  path name
 * #3: ts    [in]  timestamps
 * #4: flags [in]  flags
 * #r:       [ret] 0: no error, -1: errno
 */
int32_t ecp_utimensat(int32_t dfd, const char *path,
		const struct xtimespec ts[2], int32_t flags)
{
#if defined(ECP_PLATFORM_LINUX)

	int32_t ret;

	ret = (int32_t)ecp_syscall_linux(__NR_utimensat,
		dfd,
		path,
		ts,
		flags);

	if (SYSCALL_LINUX_ISERR(ret)) {
		x_errno = -ret;
		ret = -1;
	}

	return ret;

#else
# error "!!!unknown platform!!!"
#endif
}

/* @func: ecp_futimens
 * #desc:
 *    change file timestamps with nanosecond precision.
 *
 * #1: fd [in]  file descriptor
 * #2: ts [in]  timestamps
 * #r:    [ret] 0: no error, -1: errno
 */
int32_t ecp_futimens(int32_t fd, const struct xtimespec ts[2])
{
	return ecp_utimensat(fd, NULL, ts, 0);
}

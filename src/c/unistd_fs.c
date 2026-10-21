/* @file: unistd_fs.c
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
#include <ecp/c/sys_types.h>
#include <ecp/c/errno.h>
#include <ecp/c/unistd.h>
#include <ecp/c/syscall.h>

/* @func: ecp_read
 * #desc:
 *    read from a file descriptor.
 *
 * #1: fd  [in]  file descriptor
 * #2: buf [out] buffer
 * #3: len [in]  buffer length
 * #r:     [ret] >=0: read length, -1: errno
 */
ssize_t ecp_read(int32_t fd, void *buf, size_t len)
{
#if defined(ECP_PLATFORM_LINUX)

	ssize_t ret;

	ret = (ssize_t)ecp_syscall_linux(__NR_read,
		fd,
		buf,
		len);

	if (SYSCALL_LINUX_ISERR(ret)) {
		x_errno = -(int32_t)ret;
		return -1;
	}

	return ret;

#else
# error "!!!unknown platform!!!"
#endif
}

/* @func: ecp_write
 * #desc:
 *    write to a file descriptor.
 *
 * #1: fd  [in]  file descriptor
 * #2: buf [in]  input buffer
 * #3: len [in]  buffer length
 * #r:     [ret] >=0: write length, -1: errno
 */
ssize_t ecp_write(int32_t fd, const void *buf, size_t len)
{
#if defined(ECP_PLATFORM_LINUX)

	ssize_t ret;

	ret = (ssize_t)ecp_syscall_linux(__NR_write,
		fd,
		buf,
		len);

	if (SYSCALL_LINUX_ISERR(ret)) {
		x_errno = -(int32_t)ret;
		return -1;
	}

	return ret;

#else
# error "!!!unknown platform!!!"
#endif
}

/* @func: ecp_lseek
 * #desc:
 *    repositions the file offset.
 *
 * #1: fd     [in]  file descriptor
 * #2: off    [in]  file offset
 * #3: whence [in]  seek type
 * #r:        [ret] 0: no error, -1: errno
 */
int64_t ecp_lseek(int32_t fd, int64_t off, int32_t whence)
{
#if defined(ECP_PLATFORM_LINUX)

	int64_t ret, _off;

#if defined(ECP_MARCH_BITS_32)

	ret = (int64_t)ecp_syscall_linux(__NR__llseek,
		fd,
		(uint64_t)off >> 32,
		(uint64_t)off & 0xffffffff,
		&_off,
		whence);

#elif defined(ECP_MARCH_BITS_64)

	ret = _off = (int64_t)ecp_syscall_linux(__NR_lseek,
		fd,
		off,
		whence);

#else
# error "!!!unknown machine bits!!!"
#endif

	if (SYSCALL_LINUX_ISERR(ret)) {
		x_errno = -(int32_t)ret;
		return -1;
	}

	return _off;

#else
# error "!!!unknown platform!!!"
#endif
}

/* @func: ecp_close
 * #desc:
 *    close a file descriptor.
 *
 * #1: fd [in]  file descriptor
 * #r:    [ret] 0: no error, -1: errno
 */
int32_t ecp_close(int32_t fd)
{
#if defined(ECP_PLATFORM_LINUX)

	int32_t ret;

	ret = (int32_t)ecp_syscall_linux(__NR_close,
		fd);

	if (SYSCALL_LINUX_ISERR(ret)) {
		x_errno = -ret;
		return -1;
	}

	return ret;

#else
# error "!!!unknown platform!!!"
#endif
}

/* @file: sys_mman.c
 * #desc:
 *    The implementations of memory management.
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
#include <ecp/c/sys_mman.h>
#include <ecp/c/syscall.h>

/* @func: ecp_mmap
 * #desc:
 *    map memory pages (assigned by page, 4096 aligned).
 *
 * #1: addr  [in/out] fixed address / NULL (kernel decision)
 * #2: len   [in]     pages length
 * #3: prot  [in]     pages permission
 * #4: flags [in]     mapping flags
 * #5: fd    [in]     file descriptor
 * #6: off   [in]     file offset
 * #r:       [ret]    return address (X_MAP_FAILED: errno)
 */
void *ecp_mmap(void *addr, size_t len, int32_t prot, int32_t flags,
		int32_t fd, int64_t off)
{
#if defined(ECP_PLATFORM_LINUX)

	uintptr_t ret;

	if ((addr && (uintptr_t)addr & 4095) || len & 4095 || off & 4095) {
		x_errno = -X_EINVAL;
		return X_MAP_FAILED;
	}

#if defined(ECP_MARCH_BITS_32)

	ret = (uintptr_t)ecp_syscall_linux(__NR_mmap2,
		addr,
		len,
		prot,
		flags,
		fd,
		(size_t)(off / 4096));

#elif defined(ECP_MARCH_BITS_64)

	ret = (uintptr_t)ecp_syscall_linux(__NR_mmap,
		addr,
		len,
		prot,
		flags,
		fd,
		off);

#else
# error "!!!unknown machine bits!!!"
#endif

	if (SYSCALL_LINUX_ISERR(ret)) {
		x_errno = -(int32_t)ret;
		ret = (uintptr_t)X_MAP_FAILED;
	}

	return (void *)ret;

#else
# error "!!!unknown platform!!!"
#endif
}

/* @func: ecp_munmap
 * #desc:
 *    unmap memory pages (4096 aligned).
 *
 * #1: addr [in/out] pages address
 * #2: len  [in]     pages length
 * #r:      [ret]    0: no error, -1: errno
 */
int32_t ecp_munmap(void *addr, size_t len)
{
#if defined(ECP_PLATFORM_LINUX)

	int32_t ret;

	if ((addr && (uintptr_t)addr & 4095) || len & 4095) {
		x_errno = -X_EINVAL;
		return -1;
	}

	ret = (int32_t)ecp_syscall_linux(__NR_munmap,
		addr,
		len);

	if (SYSCALL_LINUX_ISERR(ret)) {
		x_errno = -ret;
		ret = -1;
	}

	return ret;

#else
# error "!!!unknown platform!!!"
#endif
}

/* @func: ecp_mprotect
 * #desc:
 *    set memory pages permission (4096 aligned).
 *
 * #1: addr [in/out] pages address
 * #2: len  [in]     pages length
 * #3: prot [in]     pages permission
 * #r:      [ret]    0: no error, -1: errno
 */
int32_t ecp_mprotect(void *addr, size_t len, int32_t prot)
{
#if defined(ECP_PLATFORM_LINUX)

	int32_t ret;

	if ((addr && (uintptr_t)addr & 4095) || len & 4095) {
		x_errno = -X_EINVAL;
		return -1;
	}

	ret = (int32_t)ecp_syscall_linux(__NR_mprotect,
		addr,
		prot);

	if (SYSCALL_LINUX_ISERR(ret)) {
		x_errno = -ret;
		ret = -1;
	}

	return ret;

#else
# error "!!!unknown platform!!!"
#endif
}

/* @func: ecp_msync
 * #desc:
 *    synchronize a file with a memory map.
 *
 * #1: addr  [in/out] pages address
 * #2: len   [in]     pages length
 * #3: flags [in]     flags
 * #r:       [ret]    0: no error, -1: errno
 */
int32_t ecp_msync(void *addr, size_t len, int32_t flags)
{
#if defined(ECP_PLATFORM_LINUX)

	int32_t ret;

	if ((addr && (uintptr_t)addr & 4095) || len & 4095) {
		x_errno = -X_EINVAL;
		return -1;
	}

	ret = (int32_t)ecp_syscall_linux(__NR_msync,
		addr,
		len,
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

/* @func: ecp_mlock
 * #desc:
 *    lock memory pages.
 *
 * #1: addr [in/out] pages address
 * #2: len  [in]     pages length
 * #r:      [ret]    0: no error, -1: errno
 */
int32_t ecp_mlock(const void *addr, size_t len)
{
#if defined(ECP_PLATFORM_LINUX)

	int32_t ret;

	if ((addr && (uintptr_t)addr & 4095) || len & 4095) {
		x_errno = -X_EINVAL;
		return -1;
	}

	ret = (int32_t)ecp_syscall_linux(__NR_mlock,
		addr,
		len);

	if (SYSCALL_LINUX_ISERR(ret)) {
		x_errno = -ret;
		ret = -1;
	}

	return ret;

#else
# error "!!!unknown platform!!!"
#endif
}

/* @func: ecp_munlock
 * #desc:
 *    unlock memory pages.
 *
 * #1: addr [in/out] pages address
 * #2: len  [in]     pages length
 * #r:      [ret]    0: no error, -1: errno
 */
int32_t ecp_munlock(const void *addr, size_t len)
{
#if defined(ECP_PLATFORM_LINUX)

	int32_t ret;

	if ((addr && (uintptr_t)addr & 4095) || len & 4095) {
		x_errno = -X_EINVAL;
		return -1;
	}

	ret = (int32_t)ecp_syscall_linux(__NR_munlock,
		addr,
		len);

	if (SYSCALL_LINUX_ISERR(ret)) {
		x_errno = -ret;
		ret = -1;
	}

	return ret;

#else
# error "!!!unknown platform!!!"
#endif
}

/* @func: ecp_mlockall
 * #desc:
 *    lock all memory pages.
 *
 * #1: flags [in]  lock flags
 * #r:       [ret] 0: no error, -1: errno
 */
int32_t ecp_mlockall(int32_t flags)
{
#if defined(ECP_PLATFORM_LINUX)

	int32_t ret;

	ret = (int32_t)ecp_syscall_linux(__NR_mlockall,
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

/* @func: ecp_munlockall
 * #desc:
 *    unlock all memory pages.
 *
 * #1: flags [in]  lock flags
 * #r:       [ret] 0: no error, -1: errno
 */
int32_t ecp_munlockall(int32_t flags)
{
#if defined(ECP_PLATFORM_LINUX)

	int32_t ret;

	ret = (int32_t)ecp_syscall_linux(__NR_munlockall,
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

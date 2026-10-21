/* @file: sys_mman.h
 * #desc:
 *    The definitions of memory management.
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

#ifndef _ECP_C_SYS_MMAN_H
#define _ECP_C_SYS_MMAN_H

#include <ecp/config.h>
#include <ecp/c/stddef.h>
#include <ecp/c/stdint.h>

#if defined(ECP_PLATFORM_LINUX)

#define X_PROT_NONE     0x00 /* page cannot be accessed */
#define X_PROT_READ     0x01 /* page can be read */
#define X_PROT_WRITE    0x02 /* page can be write */
#define X_PROT_EXEC     0x04 /* page can be execute */

#define X_MAP_FILE      0x00 /* file mapping to page (default) */
#define X_MAP_SHARED    0x01 /* share page mapping */
#define X_MAP_PRIVATE   0x02 /* private page mapping (cow) */
#define X_MAP_FIXED     0x10 /* fixed address mapping */

/* anonymous page mapping (conjunction with MAP_PRIVATE, fd: -1) */
#define X_MAP_ANONYMOUS 0x20

#define X_MAP_FAILED ((void *)-1) /* error return */

#define X_MS_ASYNC      0x01 /* non-blocking synchronous */
#define X_MS_INVALIDATE 0x02 /* synchronous and marked as dirty page */
#define X_MS_SYNC       0x04 /* blocking synchronous */

#define X_MCL_CURRENT   0x01 /* lock currently mapped pages */
#define X_MCL_FUTURE    0x02 /* lock pages that become mapped */
#define X_MCL_ONFAULT   0x04 /* lock pages that are faulted in */

#else
# error "!!!unknown platform!!!"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* c/sys_mman.c */
extern
void *ecp_mmap(void *addr, size_t len, int32_t prot, int32_t flags,
		int32_t fd, int64_t off)
;
extern
int32_t ecp_munmap(void *addr, size_t len)
;
extern
int32_t ecp_mprotect(void *addr, size_t len, int32_t prot)
;
extern
int32_t ecp_msync(void *addr, size_t len, int32_t flags)
;
extern
int32_t ecp_mlock(const void *addr, size_t len)
;
extern
int32_t ecp_munlock(const void *addr, size_t len)
;
extern
int32_t ecp_mlockall(int32_t flags)
;
extern
int32_t ecp_munlockall(int32_t flags)
;

#ifdef __cplusplus
}
#endif

#endif

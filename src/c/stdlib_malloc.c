/* @file: stdlib_malloc.c
 * #desc:
 *    The implementations of standard library.
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
#include <ecp/c/stdlib.h>
#include <ecp/c/sys_mman.h>
#include <ecp/c/umalloc.h>
#include <ecp/c/atomic.h>

static void *_alloc(size_t size, void *arg)
;
static int32_t _free(void *p, size_t size, void *arg)
;

UMALLOC_NEW(__umalloc_ctx, _alloc, _free, NULL);
static spinlock_t _lock = 0;

/* @func: _alloc (static)
 * #desc:
 *    heap memory alloc callback function.
 *
 * #1: size [in]  alloc size
 * #2: arg  [in]  callback arg
 * #r:      [ret] heap memory pointer
 */
static void *_alloc(size_t size, void *arg)
{
	(void)arg;
	void *p = ecp_mmap(NULL, (size + 4095) & ~4095UL,
		X_PROT_READ | X_PROT_WRITE,
		X_MAP_PRIVATE | X_MAP_ANONYMOUS, -1, 0);

	return (p == X_MAP_FAILED) ? NULL : p;
}

/* @func: _free (static)
 * #desc:
 *    heap memory free callback function.
 *
 * #1: p    [in]  heap memory pointer
 * #2: size [in]  alloc size
 * #3: arg  [in]  callback arg
 * #r:      [ret] 0: no error
 */
static int32_t _free(void *p, size_t size, void *arg)
{
	(void)arg;
	ecp_munmap(p, (size + 4095) & ~4095UL);

	return 0;
}

/* @func: ecp_malloc
 * #desc:
 *    heap memory allocator.
 *
 * #1: size [in]  alloc length
 * #r:      [ret] heap pointer
 */
void *ecp_malloc(size_t size)
{
	SPIN_LOCK(&_lock);
	void *p = ecp_umalloc(&__umalloc_ctx, size);
	SPIN_UNLOCK(&_lock);

	return p;
}

/* @func: ecp_free
 * #desc:
 *    heap memory free.
 *
 * #1: p [in/out] heap pointer
 */
void ecp_free(void *p)
{
	if (!p)
		return;

	SPIN_LOCK(&_lock);
	ecp_umalloc_free(&__umalloc_ctx, p);
	SPIN_UNLOCK(&_lock);
}

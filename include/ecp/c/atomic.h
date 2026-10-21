/* @file: atomic.h
 * #desc:
 *    The definitions of atomic operations.
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

#ifndef _ECP_C_ATOMIC_H
#define _ECP_C_ATOMIC_H

#include <ecp/config.h>
#include <ecp/c/stdint.h>

typedef volatile int32_t spinlock_t;

#define SPIN_FETCH(x) (ecp_atomic_cas(x, 0, 1) == 0)
#define SPIN_LOCK(x) \
	do { \
		while (ecp_atomic_cas(x, 0, 1)); \
	} while (0)
#define SPIN_UNLOCK(x) \
	do { \
		while (!ecp_atomic_cas(x, 1, 0)); \
	} while (0)

#ifdef __cplusplus
extern "C" {
#endif

/* c/atomic_cas.S */
extern
int32_t ecp_atomic_cas(volatile int32_t *lock, int32_t old, int32_t _new)
;

#ifdef __cplusplus
}
#endif

#endif

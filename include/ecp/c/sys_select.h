/* @file: select.h
 * #desc:
 *    The definitions of synchronous i/o multiplexing.
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

#ifndef _ECP_C_SELECT_H
#define _ECP_C_SELECT_H

#include <ecp/config.h>
#include <ecp/c/stddef.h>
#include <ecp/c/stdint.h>
#include <ecp/c/time.h>
#include <ecp/c/signal.h>

#define X_FD_SETSIZE 1024

typedef struct {
	uint8_t _bits[X_FD_SETSIZE / 8];
} xfd_set;

#define X_FD_ZERO(x) \
	do { \
		for (int32_t __i = 0; __i < (X_FD_SETSIZE / 8); __i++) \
			((uint8_t *)x)[__i] = 0; \
	} while (0)

#define X_FD_SET(n, x) \
	((uint8_t *)x)[(n) / 8] |= 1 << ((n) % 8)
#define X_FD_CLR(n, x) \
	((uint8_t *)x)[(n) / 8] &= ~(1 << ((n) % 8))
#define X_FD_ISSET(n, x) \
	(((uint8_t *)x)[(n) / 8] & (1 << ((n) % 8)))

#ifdef __cplusplus
extern "C" {
#endif

/* c/sys_select.c */
extern
int32_t ecp_pselect(int32_t nfds, xfd_set *rfds, xfd_set *wfds,
		xfd_set *efds, const struct xtimespec *ts,
		const xsigset_t *mask)
;
extern
int32_t ecp_select(int32_t nfds, xfd_set *rfds, xfd_set *wfds,
		xfd_set *efds, struct xtimeval *tv)
;

#ifdef __cplusplus
}
#endif

#endif

/* @file: sm3.h
 * #desc:
 *    The definitions of shangmi-3 cryptographic hash.
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

#ifndef _ECP_SM3_H
#define _ECP_SM3_H

#include <ecp/config.h>
#include <ecp/c/stddef.h>
#include <ecp/c/stdint.h>

#define SM3_LEN 32
#define SM3_BLOCKSIZE 64

struct sm3_ctx {
	uint32_t state[8];
	uint32_t count;
	uint8_t buf[SM3_BLOCKSIZE];
};

#define	SM3_NEW(x) struct sm3_ctx x
#define SM3_STATE(x, n) (((uint8_t *)((x)->state))[n])

#ifdef __cplusplus
extern "C" {
#endif

/* sm3.c */
extern
void ecp_sm3_init(struct sm3_ctx *ctx)
;
extern
void ecp_sm3_process(struct sm3_ctx *ctx, const uint8_t *s, size_t len)
;
extern
void ecp_sm3_finish(struct sm3_ctx *ctx, uint64_t len)
;
extern
void ecp_sm3(struct sm3_ctx *ctx, const uint8_t *s, size_t len)
;

#ifdef __cplusplus
}
#endif

#endif

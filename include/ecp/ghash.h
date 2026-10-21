/* @file: ghash.h
 * #desc:
 *    The definitions of galois message authentication code.
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

#ifndef _ECP_GHASH_H
#define _ECP_GHASH_H

#include <ecp/config.h>
#include <ecp/c/stddef.h>
#include <ecp/c/stdint.h>

#define GHASH_KEY_LEN 16
#define GHASH_TAG_LEN 16
#define GHASH_BLOCKSIZE 16

struct ghash_ctx {
	uint64_t h[16][2];
	uint8_t s[GHASH_BLOCKSIZE];
	uint8_t buf[GHASH_BLOCKSIZE];
	uint32_t count;
};

#define GHASH_NEW(x) struct ghash_ctx x

#define GHASH_IV(x, n) ((x)->s[n])
#define GHASH_TAG(x, n) GHASH_IV(x, n)

#ifdef __cplusplus
extern "C" {
#endif

/* ghash.c */
extern
void ecp_ghash_init(struct ghash_ctx *ctx, const uint8_t *key)
;
extern
void ecp_ghash_gfmul(struct ghash_ctx *ctx, uint8_t x[16])
;
extern
void ecp_ghash_block(struct ghash_ctx *ctx, const uint8_t *s)
;
extern
void ecp_ghash_process(struct ghash_ctx *ctx, const uint8_t *s, size_t len)
;
extern
void ecp_ghash_finish(struct ghash_ctx *ctx)
;
extern
void ecp_ghash(struct ghash_ctx *ctx, const uint8_t *s, size_t len)
;

#ifdef __cplusplus
}
#endif

#endif

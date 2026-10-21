/* @file: sha2.h
 * #desc:
 *    The definitions of secure hash algorithm 2.
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

#ifndef _ECP_SHA2_H
#define _ECP_SHA2_H

#include <ecp/config.h>
#include <ecp/c/stddef.h>
#include <ecp/c/stdint.h>

#define SHA224_LEN 28
#define SHA256_LEN 32
#define SHA256_BLOCKSIZE 64

#define SHA384_LEN 48
#define SHA512_LEN 64
#define SHA512_BLOCKSIZE 128

struct sha256_ctx {
	uint32_t state[8];
	uint32_t count;
	uint8_t buf[SHA256_BLOCKSIZE];
};

struct sha512_ctx {
	uint64_t state[8];
	uint32_t count;
	uint8_t buf[SHA512_BLOCKSIZE];
};

#define	SHA256_NEW(x) struct sha256_ctx x
#define	SHA224_NEW(x) struct sha256_ctx x
#define	SHA512_NEW(x) struct sha512_ctx x
#define	SHA384_NEW(x) struct sha512_ctx x

#define SHA256_STATE(x, n) (((uint8_t *)((x)->state))[n])
#define SHA224_STATE(x, n) (((uint8_t *)((x)->state))[n])
#define SHA512_STATE(x, n) (((uint8_t *)((x)->state))[n])
#define SHA384_STATE(x, n) (((uint8_t *)((x)->state))[n])

#ifdef __cplusplus
extern "C" {
#endif

/* sha256.c */
extern
void ecp_sha256_init(struct sha256_ctx *ctx)
;
extern
void ecp_sha224_init(struct sha256_ctx *ctx)
;
extern
void ecp_sha256_process(struct sha256_ctx *ctx, const uint8_t *s, size_t len)
;
extern
void ecp_sha256_finish(struct sha256_ctx *ctx, uint64_t len)
;
extern
void ecp_sha256(struct sha256_ctx *ctx, const uint8_t *s, size_t len)
;

/* sha512.c */
extern
void ecp_sha512_init(struct sha512_ctx *ctx)
;
extern
void ecp_sha384_init(struct sha512_ctx *ctx)
;
extern
void ecp_sha512_process(struct sha512_ctx *ctx, const uint8_t *s, size_t len)
;
extern
void ecp_sha512_finish(struct sha512_ctx *ctx, uint64_t len)
;
extern
void ecp_sha512(struct sha512_ctx *ctx, const uint8_t *s, size_t len)
;

#ifdef __cplusplus
}
#endif

#endif

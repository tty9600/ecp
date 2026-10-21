/* @file: sha1.h
 * #desc:
 *    The definitions of secure hash algorithm 1.
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

#ifndef _ECP_SHA1_H
#define _ECP_SHA1_H

#include <ecp/config.h>
#include <ecp/c/stddef.h>
#include <ecp/c/stdint.h>

#define SHA1_LEN 20
#define SHA1_BLOCKSIZE 64

struct sha1_ctx {
	uint32_t state[5];
	uint32_t count;
	uint8_t buf[SHA1_BLOCKSIZE];
};

#define	SHA1_NEW(x) struct sha1_ctx x
#define SHA1_STATE(x, n) (((uint8_t *)((x)->state))[n])

#ifdef __cplusplus
extern "C" {
#endif

/* sha1.c */
extern
void ecp_sha1_init(struct sha1_ctx *ctx)
;
extern
void ecp_sha1_process(struct sha1_ctx *ctx, const uint8_t *s, size_t len)
;
extern
void ecp_sha1_finish(struct sha1_ctx *ctx, uint64_t len)
;
extern
void ecp_sha1(struct sha1_ctx *ctx, const uint8_t *s, size_t len)
;

#ifdef __cplusplus
}
#endif

#endif

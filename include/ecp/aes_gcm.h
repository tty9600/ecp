/* @file: aes_gcm.h
 * #desc:
 *    The definitions of aes-gcm galois/counter mode.
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

#ifndef _ECP_AES_GCM_H
#define _ECP_AES_GCM_H

#include <ecp/config.h>
#include <ecp/c/stddef.h>
#include <ecp/c/stdint.h>
#include <ecp/aes.h>
#include <ecp/ghash.h>

#define AES_GCM_IV_LEN 12
#define AES_GCM_TAG_LEN GHASH_TAG_LEN

struct aes_gcm_ctx {
	struct aes_ctx aes;
	struct ghash_ctx gh;
	uint8_t ctr[AES_BLOCKSIZE];
};

#define AES_GCM_NEW(x) struct aes_gcm_ctx x

#ifdef __cplusplus
extern "C" {
#endif

/* aes_gcm.c */
extern
int32_t ecp_aes_gcm_init(struct aes_gcm_ctx *ctx, const uint8_t *key,
		int32_t type)
;
extern
void ecp_aes_gcm_set_iv(struct aes_gcm_ctx *ctx, const uint8_t *iv)
;
extern
void ecp_aes_gcm_crypto(struct aes_gcm_ctx *ctx, uint8_t *buf, size_t len)
;
extern
void ecp_aes_gcm_process(struct aes_gcm_ctx *ctx, const uint8_t *buf,
		size_t len)
;
extern
void ecp_aes_gcm_finish(struct aes_gcm_ctx *ctx, size_t auth_len,
		size_t data_len, uint8_t *tag)
;

#ifdef __cplusplus
}
#endif

#endif

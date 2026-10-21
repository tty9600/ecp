/* @file: aes_gcm.c
 * #desc:
 *    The implementations of aes-gcm galois/counter mode.
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
#include <ecp/c/string.h>
#include <ecp/aes.h>
#include <ecp/ghash.h>
#include <ecp/aes_gcm.h>

/* @func: ecp_aes_gcm_init
 * #desc:
 *    aes-gcm initialization function.
 *
 * #1: ctx  [out] aes-gcm struct context
 * #2: key  [in]  input key
 * #3: type [in]  aes type
 * #r:      [ret] 0: no error, -1: type error
 */
int32_t ecp_aes_gcm_init(struct aes_gcm_ctx *ctx, const uint8_t *key,
		int32_t type)
{
	uint8_t block[AES_BLOCKSIZE];

	if (ecp_aes_init(&ctx->aes, key, type))
		return -1;

	ecp_memset(block, 0, AES_BLOCKSIZE);
	ecp_aes_encrypt(&ctx->aes, block);
	ecp_ghash_init(&ctx->gh, block);

	return 0;
}

/* @func: ecp_aes_gcm_set_iv
 * #desc:
 *    aes-gcm invector initialization function.
 *
 * #1: ctx [out] aes-gcm struct context
 * #2: iv  [in]  invector
 */
void ecp_aes_gcm_set_iv(struct aes_gcm_ctx *ctx, const uint8_t *iv)
{
	ecp_memcpy(ctx->ctr, iv, AES_GCM_IV_LEN);
	ctx->ctr[AES_BLOCKSIZE - 4] = 0;
	ctx->ctr[AES_BLOCKSIZE - 3] = 0;
	ctx->ctr[AES_BLOCKSIZE - 2] = 0;
	ctx->ctr[AES_BLOCKSIZE - 1] = 2;
}

/* @func: ecp_aes_gcm_crypto
 * #desc:
 *    aes-gcm stream encryption galois/counter mode.
 *
 * #1: ctx [in/out] aes-gcm struct context
 * #2: buf [in/out] buffer
 * #3: len [in]     length
 */
void ecp_aes_gcm_crypto(struct aes_gcm_ctx *ctx, uint8_t *buf, size_t len)
{
	ecp_aes_crypto_ctr(&ctx->aes, ctx->ctr, buf, len);
}

/* @func: ecp_aes_gcm_process
 * #desc:
 *    aes-gcm ghash processing input function.
 *
 * #1: ctx [in/out] aes-gcm struct context
 * #2: buf [in]     input buffer
 * #3: len [in]     input length
 */
void ecp_aes_gcm_process(struct aes_gcm_ctx *ctx, const uint8_t *buf,
		size_t len)
{
	uint8_t block[AES_BLOCKSIZE];

	ecp_ghash_process(&ctx->gh, buf, len);
	len %= AES_BLOCKSIZE;
	if (len) {
		ecp_memset(block, 0, AES_BLOCKSIZE);
		ecp_ghash_process(&ctx->gh, block, AES_BLOCKSIZE - len);
	}
}

/* @func: ecp_aes_gcm_finish
 * #desc:
 *    aes-gcm ghash processing finish function.
 *
 * #1: ctx      [in/out] aes-gcm struct context
 * #2: auth_len [in]     authenticate data length
 * #3: data_len [in]     encryption data length
 * #4: tag      [out]    output tag
 */
void ecp_aes_gcm_finish(struct aes_gcm_ctx *ctx, size_t auth_len,
		size_t data_len, uint8_t *tag)
{
	uint8_t block[AES_BLOCKSIZE];

	auth_len *= 8;
	data_len *= 8;
	for (int32_t i = 0; i < 8; i++) {
		block[7 - i] = (uint8_t)auth_len;
		auth_len >>= 8;
	}
	for (int32_t i = 0; i < 8; i++) {
		block[15 - i] = (uint8_t)data_len;
		data_len >>= 8;
	}

	ecp_ghash_process(&ctx->gh, block, AES_BLOCKSIZE);
	ecp_ghash_finish(&ctx->gh);

	ctx->ctr[AES_BLOCKSIZE - 4] = 0;
	ctx->ctr[AES_BLOCKSIZE - 3] = 0;
	ctx->ctr[AES_BLOCKSIZE - 2] = 0;
	ctx->ctr[AES_BLOCKSIZE - 1] = 1;
	ecp_aes_encrypt(&ctx->aes, ctx->ctr);

	for (int32_t i = 0; i < GHASH_TAG_LEN; i++)
		tag[i] = ctx->ctr[i] ^ GHASH_TAG(&ctx->gh, i);
}

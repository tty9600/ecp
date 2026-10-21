/* @file: hmac_sha256.c
 * #desc:
 *    The implementations of hash message authentication code (sha256).
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
#include <ecp/sha2.h>
#include <ecp/hmac.h>

/* @func: ecp_hmac_sha256_init
 * #desc:
 *    hmac-sha256 struct context initialization.
 *
 * #1: ctx     [out] hmac-sha256 struct context
 * #2: key     [in]  input key
 * #3: key_len [in]  key length
 */
void ecp_hmac_sha256_init(struct hmac_sha256_ctx *ctx, const uint8_t *key,
		uint32_t key_len)
{
	SHA256_NEW(sha256_ctx);
	ecp_sha256_init(&sha256_ctx);

	if (key_len > SHA256_BLOCKSIZE) {
		ecp_sha256(&sha256_ctx, key, key_len);
		key = &(SHA256_STATE(&sha256_ctx, 0));
		key_len = SHA256_LEN;
	}

	ecp_memset(ctx->ipad, 0x36, SHA256_BLOCKSIZE);
	ecp_memset(ctx->opad, 0x5c, SHA256_BLOCKSIZE);
	for (uint32_t i = 0; i < key_len; i++) {
		ctx->ipad[i] ^= key[i];
		ctx->opad[i] ^= key[i];
	}

	ecp_sha256_init(&ctx->ipad_ctx);
	ecp_sha256_init(&ctx->opad_ctx);
	ecp_sha256_process(&ctx->ipad_ctx, ctx->ipad, SHA256_BLOCKSIZE);

	ctx->state = &(SHA256_STATE(&ctx->opad_ctx, 0));
}

/* @func: ecp_hmac_sha256_process
 * #desc:
 *    hmac-sha256 processing input function.
 *
 * #1: ctx [in/out] hmac-sha256 struct context
 * #2: s   [in]     input buffer
 * #3: len [in]     input length
 */
void ecp_hmac_sha256_process(struct hmac_sha256_ctx *ctx, const uint8_t *s,
		size_t len)
{
	ecp_sha256_process(&ctx->ipad_ctx, s, len);
}

/* @func: ecp_hmac_sha256_finish
 * #desc:
 *    hmac-sha256 processing finish function.
 *
 * #1: ctx [in/out] hmac-sha256 struct context
 * #2: len [in]     total length
 */
void ecp_hmac_sha256_finish(struct hmac_sha256_ctx *ctx, uint64_t len)
{
	ecp_sha256_finish(&ctx->ipad_ctx, len + SHA256_BLOCKSIZE);

	ecp_sha256_process(&ctx->opad_ctx, ctx->opad, SHA256_BLOCKSIZE);
	ecp_sha256_process(&ctx->opad_ctx,
		&(SHA256_STATE(&ctx->ipad_ctx, 0)), SHA256_LEN);
	ecp_sha256_finish(&ctx->opad_ctx, SHA256_BLOCKSIZE + SHA256_LEN);
}

/* @func: ecp_hmac_sha256
 * #desc:
 *    hmac-sha256 one-time processing function.
 *
 * #1: ctx [in/out] hmac-sha256 struct context
 * #2: s   [in]     input buffer
 * #3: len [in]     input length
 */
void ecp_hmac_sha256(struct hmac_sha256_ctx *ctx, const uint8_t *s, size_t len)
{
	ecp_hmac_sha256_process(ctx, s, len);
	ecp_hmac_sha256_finish(ctx, len);
}

/* @func: ecp_hmac_sha224_init
 * #desc:
 *    hmac-sha224 initialize the hmac-sha256 struct context.
 *
 * #1: ctx     [out] hmac-sha256 struct context
 * #2: key     [in]  input key
 * #3: key_len [in]  key length
 */
void ecp_hmac_sha224_init(struct hmac_sha256_ctx *ctx, const uint8_t *key,
		uint32_t key_len)
{
	SHA224_NEW(sha224_ctx);
	ecp_sha224_init(&sha224_ctx);

	if (key_len > SHA256_BLOCKSIZE) {
		ecp_sha256(&sha224_ctx, key, key_len);
		key = &(SHA224_STATE(&sha224_ctx, 0));
		key_len = SHA224_LEN;
	}

	ecp_memset(ctx->ipad, 0x36, SHA256_BLOCKSIZE);
	ecp_memset(ctx->opad, 0x5c, SHA256_BLOCKSIZE);
	for (uint32_t i = 0; i < key_len; i++) {
		ctx->ipad[i] ^= key[i];
		ctx->opad[i] ^= key[i];
	}

	ecp_sha224_init(&ctx->ipad_ctx);
	ecp_sha224_init(&ctx->opad_ctx);
	ecp_sha256_process(&ctx->ipad_ctx, ctx->ipad, SHA256_BLOCKSIZE);

	ctx->state = &(SHA224_STATE(&ctx->opad_ctx, 0));
}

/* @func: ecp_hmac_sha224_process
 * #desc:
 *    hmac-sha224 processing input function.
 *
 * #1: ctx [in/out] hmac-sha256 struct context
 * #2: s   [in]     input buffer
 * #3: len [in]     input length
 */
void ecp_hmac_sha224_process(struct hmac_sha256_ctx *ctx, const uint8_t *s,
		size_t len)
{
	ecp_sha256_process(&ctx->ipad_ctx, s, len);
}

/* @func: ecp_hmac_sha224_finish
 * #desc:
 *    hmac-sha224 processing finish function.
 *
 * #1: ctx [in/out] hmac-sha256 struct context
 * #2: len [in]     total length
 */
void ecp_hmac_sha224_finish(struct hmac_sha256_ctx *ctx, uint64_t len)
{
	ecp_sha256_finish(&ctx->ipad_ctx, len + SHA256_BLOCKSIZE);

	ecp_sha256_process(&ctx->opad_ctx, ctx->opad, SHA256_BLOCKSIZE);
	ecp_sha256_process(&ctx->opad_ctx,
		&(SHA224_STATE(&ctx->ipad_ctx, 0)), SHA224_LEN);
	ecp_sha256_finish(&ctx->opad_ctx, SHA256_BLOCKSIZE + SHA224_LEN);
}

/* @func: ecp_hmac_sha224
 * #desc:
 *    hmac-sha224 one-time processing function.
 *
 * #1: ctx [in/out] hmac-sha256 struct context
 * #2: s   [in]     input buffer
 * #3: len [in]     input length
 */
void ecp_hmac_sha224(struct hmac_sha256_ctx *ctx, const uint8_t *s, size_t len)
{
	ecp_hmac_sha256_process(ctx, s, len);
	ecp_hmac_sha256_finish(ctx, len);
}

/* @file: hmac_sha512.c
 * #desc:
 *    The implementations of hash message authentication code (sha512).
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

/* @func: ecp_hmac_sha512_init
 * #desc:
 *    hmac-sha512 struct context initialization.
 *
 * #1: ctx     [out] hmac-sha512 struct context
 * #2: key     [in]  input key
 * #3: key_len [in]  key length
 */
void ecp_hmac_sha512_init(struct hmac_sha512_ctx *ctx, const uint8_t *key,
		uint32_t key_len)
{
	SHA512_NEW(sha512_ctx);
	ecp_sha512_init(&sha512_ctx);

	if (key_len > SHA512_BLOCKSIZE) {
		ecp_sha512(&sha512_ctx, key, key_len);
		key = &(SHA512_STATE(&sha512_ctx, 0));
		key_len = SHA512_LEN;
	}

	ecp_memset(ctx->ipad, 0x36, SHA512_BLOCKSIZE);
	ecp_memset(ctx->opad, 0x5c, SHA512_BLOCKSIZE);
	for (uint32_t i = 0; i < key_len; i++) {
		ctx->ipad[i] ^= key[i];
		ctx->opad[i] ^= key[i];
	}

	ecp_sha512_init(&ctx->ipad_ctx);
	ecp_sha512_init(&ctx->opad_ctx);
	ecp_sha512_process(&ctx->ipad_ctx, ctx->ipad, SHA512_BLOCKSIZE);

	ctx->state = &(SHA512_STATE(&ctx->opad_ctx, 0));
}

/* @func: ecp_hmac_sha512_process
 * #desc:
 *    hmac-sha512 processing input function.
 *
 * #1: ctx [in/out] hmac-sha512 struct context
 * #2: s   [in]     input buffer
 * #3: len [in]     input length
 */
void ecp_hmac_sha512_process(struct hmac_sha512_ctx *ctx, const uint8_t *s,
		size_t len)
{
	ecp_sha512_process(&ctx->ipad_ctx, s, len);
}

/* @func: ecp_hmac_sha512_finish
 * #desc:
 *    hmac-sha512 processing finish function.
 *
 * #1: ctx [in/out] hmac-sha512 struct context
 * #2: len [in]     total length
 */
void ecp_hmac_sha512_finish(struct hmac_sha512_ctx *ctx, uint64_t len)
{
	ecp_sha512_finish(&ctx->ipad_ctx, len + SHA512_BLOCKSIZE);

	ecp_sha512_process(&ctx->opad_ctx, ctx->opad, SHA512_BLOCKSIZE);
	ecp_sha512_process(&ctx->opad_ctx,
		&(SHA512_STATE(&ctx->ipad_ctx, 0)), SHA512_LEN);
	ecp_sha512_finish(&ctx->opad_ctx, SHA512_BLOCKSIZE + SHA512_LEN);
}

/* @func: ecp_hmac_sha512
 * #desc:
 *    hmac-sha512 one-time processing function.
 *
 * #1: ctx [in/out] hmac-sha512 struct context
 * #2: s   [in]     input buffer
 * #3: len [in]     input length
 */
void ecp_hmac_sha512(struct hmac_sha512_ctx *ctx, const uint8_t *s, size_t len)
{
	ecp_hmac_sha512_process(ctx, s, len);
	ecp_hmac_sha512_finish(ctx, len);
}

/* @func: ecp_hmac_sha384_init
 * #desc:
 *    hmac-sha384 initialize the hmac-sha512 struct context.
 *
 * #1: ctx     [out] hmac-sha512 struct context
 * #2: key     [in]  input key
 * #3: key_len [in]  key length
 */
void ecp_hmac_sha384_init(struct hmac_sha512_ctx *ctx, const uint8_t *key,
		uint32_t key_len)
{
	SHA384_NEW(sha384_ctx);
	ecp_sha384_init(&sha384_ctx);

	if (key_len > SHA512_BLOCKSIZE) {
		ecp_sha512(&sha384_ctx, key, key_len);
		key = &(SHA384_STATE(&sha384_ctx, 0));
		key_len = SHA384_LEN;
	}

	ecp_memset(ctx->ipad, 0x36, SHA512_BLOCKSIZE);
	ecp_memset(ctx->opad, 0x5c, SHA512_BLOCKSIZE);
	for (uint32_t i = 0; i < key_len; i++) {
		ctx->ipad[i] ^= key[i];
		ctx->opad[i] ^= key[i];
	}

	ecp_sha384_init(&ctx->ipad_ctx);
	ecp_sha384_init(&ctx->opad_ctx);
	ecp_sha512_process(&ctx->ipad_ctx, ctx->ipad, SHA512_BLOCKSIZE);

	ctx->state = &(SHA384_STATE(&ctx->opad_ctx, 0));
}

/* @func: ecp_hmac_sha384_process
 * #desc:
 *    hmac-sha384 processing input function.
 *
 * #1: ctx [in/out] hmac-sha512 struct context
 * #2: s   [in]     input buffer
 * #3: len [in]     input length
 */
void ecp_hmac_sha384_process(struct hmac_sha512_ctx *ctx, const uint8_t *s,
		size_t len)
{
	ecp_sha512_process(&ctx->ipad_ctx, s, len);
}

/* @func: ecp_hmac_sha384_finish
 * #desc:
 *    hmac-sha384 processing finish function.
 *
 * #1: ctx [in/out] hmac-sha512 struct context
 * #2: len [in]     total length
 */
void ecp_hmac_sha384_finish(struct hmac_sha512_ctx *ctx, uint64_t len)
{
	ecp_sha512_finish(&ctx->ipad_ctx, len + SHA512_BLOCKSIZE);

	ecp_sha512_process(&ctx->opad_ctx, ctx->opad, SHA512_BLOCKSIZE);
	ecp_sha512_process(&ctx->opad_ctx,
		&(SHA384_STATE(&ctx->ipad_ctx, 0)), SHA384_LEN);
	ecp_sha512_finish(&ctx->opad_ctx, SHA512_BLOCKSIZE + SHA384_LEN);
}

/* @func: ecp_hmac_sha384
 * #desc:
 *    hmac-sha384 one-time processing function.
 *
 * #1: ctx [in/out] hmac-sha512 struct context
 * #2: s   [in]     input buffer
 * #3: len [in]     input length
 */
void ecp_hmac_sha384(struct hmac_sha512_ctx *ctx, const uint8_t *s, size_t len)
{
	ecp_hmac_sha512_process(ctx, s, len);
	ecp_hmac_sha512_finish(ctx, len);
}

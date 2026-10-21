/* @file: hmac_sm3.c
 * #desc:
 *    The implementations of hash message authentication code (sm3).
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
#include <ecp/sm3.h>
#include <ecp/hmac.h>

/* @func: ecp_hmac_sm3_init
 * #desc:
 *    hmac-sm3 struct context initialization.
 *
 * #1: ctx     [out] hmac-sm3 struct context
 * #2: key     [in]  input key
 * #3: key_len [in]  key length
 */
void ecp_hmac_sm3_init(struct hmac_sm3_ctx *ctx, const uint8_t *key,
		uint32_t key_len)
{
	SM3_NEW(sm3_ctx);
	ecp_sm3_init(&sm3_ctx);

	if (key_len > SM3_BLOCKSIZE) {
		ecp_sm3(&sm3_ctx, key, key_len);
		key = &(SM3_STATE(&sm3_ctx, 0));
		key_len = SM3_LEN;
	}

	ecp_memset(ctx->ipad, 0x36, SM3_BLOCKSIZE);
	ecp_memset(ctx->opad, 0x5c, SM3_BLOCKSIZE);
	for (uint32_t i = 0; i < key_len; i++) {
		ctx->ipad[i] ^= key[i];
		ctx->opad[i] ^= key[i];
	}

	ecp_sm3_init(&ctx->ipad_ctx);
	ecp_sm3_init(&ctx->opad_ctx);
	ecp_sm3_process(&ctx->ipad_ctx, ctx->ipad, SM3_BLOCKSIZE);

	ctx->state = &(SM3_STATE(&ctx->opad_ctx, 0));
}

/* @func: ecp_hmac_sm3_process
 * #desc:
 *    hmac-sm3 processing input function.
 *
 * #1: ctx [in/out] hmac-sm3 struct context
 * #2: s   [in]     input buffer
 * #3: len [in]     input length
 */
void ecp_hmac_sm3_process(struct hmac_sm3_ctx *ctx, const uint8_t *s,
		size_t len)
{
	ecp_sm3_process(&ctx->ipad_ctx, s, len);
}

/* @func: ecp_hmac_sm3_finish
 * #desc:
 *    hmac-sm3 processing finish function.
 *
 * #1: ctx [in/out] hmac-sm3 struct context
 * #2: len [in]     total length
 */
void ecp_hmac_sm3_finish(struct hmac_sm3_ctx *ctx, uint64_t len)
{
	ecp_sm3_finish(&ctx->ipad_ctx, len + SM3_BLOCKSIZE);

	ecp_sm3_process(&ctx->opad_ctx, ctx->opad, SM3_BLOCKSIZE);
	ecp_sm3_process(&ctx->opad_ctx,
		&(SM3_STATE(&ctx->ipad_ctx, 0)), SM3_LEN);
	ecp_sm3_finish(&ctx->opad_ctx, SM3_BLOCKSIZE + SM3_LEN);
}

/* @func: ecp_hmac_sm3
 * #desc:
 *    hmac-sm3 one-time processing function.
 *
 * #1: ctx [in/out] hmac-sm3 struct context
 * #2: s   [in]     input buffer
 * #3: len [in]     input length
 */
void ecp_hmac_sm3(struct hmac_sm3_ctx *ctx, const uint8_t *s, size_t len)
{
	ecp_hmac_sm3_process(ctx, s, len);
	ecp_hmac_sm3_finish(ctx, len);
}

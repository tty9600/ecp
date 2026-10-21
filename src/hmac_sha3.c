/* @file: hmac_sha3.c
 * #desc:
 *    The implementations of hash message authentication code (sha3).
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
#include <ecp/sha3.h>
#include <ecp/hmac.h>

/* @func: ecp_hmac_sha3_init
 * #desc:
 *    hmac-sha3 struct context initialization.
 *
 * #1: ctx     [out] hmac-sha3 struct context
 * #2: key     [in]  input key
 * #3: key_len [in]  key length
 * #4: type    [in]  digest type
 * #r:         [ret] 0: no error, -1: type or init error
 */
int32_t ecp_hmac_sha3_init(struct hmac_sha3_ctx *ctx, const uint8_t *key,
		uint32_t key_len, int32_t type)
{
	SHA3_NEW(sha3_ctx);

	switch (type) {
		case SHA3_224_TYPE: case SHA3_256_TYPE:
		case SHA3_384_TYPE: case SHA3_512_TYPE:
			break;
		default:
			return -1;
	}
	if (ecp_sha3_init(&sha3_ctx, type, 0))
		return -1;

	if (key_len > SHA3_GETRATE(&sha3_ctx)) {
		ecp_sha3(&sha3_ctx, key, key_len);
		key = &(SHA3_STATE(&sha3_ctx, 0));
		key_len = SHA3_GETDSIZE(&sha3_ctx);
	}

	ecp_memset(ctx->ipad, 0x36, SHA3_STATE_SIZE);
	ecp_memset(ctx->opad, 0x5c, SHA3_STATE_SIZE);
	for (uint32_t i = 0; i < key_len; i++) {
		ctx->ipad[i] ^= key[i];
		ctx->opad[i] ^= key[i];
	}

	if (ecp_sha3_init(&ctx->ipad_ctx, type, 0))
		return -1;
	if (ecp_sha3_init(&ctx->opad_ctx, type, 0))
		return -1;

	ecp_sha3_process(&ctx->ipad_ctx, ctx->ipad,
		SHA3_GETRATE(&ctx->ipad_ctx));

	ctx->state = &(SHA3_STATE(&ctx->opad_ctx, 0));

	return 0;
}

/* @func: ecp_hmac_sha3_process
 * #desc:
 *    hmac-sha3 processing input function.
 *
 * #1: ctx [in/out] hmac-sha3 struct context
 * #2: s   [in]     input buffer
 * #3: len [in]     input length
 */
void ecp_hmac_sha3_process(struct hmac_sha3_ctx *ctx, const uint8_t *s,
		size_t len)
{
	ecp_sha3_process(&ctx->ipad_ctx, s, len);
}

/* @func: ecp_hmac_sha3_finish
 * #desc:
 *    hmac-sha3 processing finish function.
 *
 * #1: ctx [in/out] hmac-sha3 struct context
 */
void ecp_hmac_sha3_finish(struct hmac_sha3_ctx *ctx)
{
	ecp_sha3_finish(&ctx->ipad_ctx);

	ecp_sha3_process(&ctx->opad_ctx, ctx->opad,
		SHA3_GETRATE(&ctx->opad_ctx));
	ecp_sha3_process(&ctx->opad_ctx, &(SHA3_STATE(&ctx->ipad_ctx, 0)),
		SHA3_GETDSIZE(&ctx->ipad_ctx));
	ecp_sha3_finish(&ctx->opad_ctx);
}

/* @func: ecp_hmac_sha3
 * #desc:
 *    hmac-sha3 one-time processing function.
 *
 * #1: ctx [in/out] hmac-sha3 struct context
 * #2: s   [in]     input buffer
 * #3: len [in]     input length
 */
void ecp_hmac_sha3(struct hmac_sha3_ctx *ctx, const uint8_t *s, size_t len)
{
	ecp_hmac_sha3_process(ctx, s, len);
	ecp_hmac_sha3_finish(ctx);
}

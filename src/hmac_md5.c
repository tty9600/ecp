/* @file: hmac_md5.c
 * #desc:
 *    The implementations of hash message authentication code (md5).
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
#include <ecp/md5.h>
#include <ecp/hmac.h>

/* @func: ecp_hmac_md5_init
 * #desc:
 *    hmac-md5 struct context initialization.
 *
 * #1: ctx     [out] hmac-md5 struct context
 * #2: key     [in]  input key
 * #3: key_len [in]  key length
 */
void ecp_hmac_md5_init(struct hmac_md5_ctx *ctx, const uint8_t *key,
		uint32_t key_len)
{
	MD5_NEW(md5_ctx);
	ecp_md5_init(&md5_ctx);

	if (key_len > MD5_BLOCKSIZE) {
		ecp_md5(&md5_ctx, key, key_len);
		key = &(MD5_STATE(&md5_ctx, 0));
		key_len = MD5_LEN;
	}

	ecp_memset(ctx->ipad, 0x36, MD5_BLOCKSIZE);
	ecp_memset(ctx->opad, 0x5c, MD5_BLOCKSIZE);
	for (uint32_t i = 0; i < key_len; i++) {
		ctx->ipad[i] ^= key[i];
		ctx->opad[i] ^= key[i];
	}

	ecp_md5_init(&ctx->ipad_ctx);
	ecp_md5_init(&ctx->opad_ctx);
	ecp_md5_process(&ctx->ipad_ctx, ctx->ipad, MD5_BLOCKSIZE);

	ctx->state = &(MD5_STATE(&ctx->opad_ctx, 0));
}

/* @func: ecp_hmac_md5_process
 * #desc:
 *    hmac-md5 processing input function.
 *
 * #1: ctx [in/out] hmac-md5 struct context
 * #2: s   [in]     input buffer
 * #3: len [in]     input length
 */
void ecp_hmac_md5_process(struct hmac_md5_ctx *ctx, const uint8_t *s,
		size_t len)
{
	ecp_md5_process(&ctx->ipad_ctx, s, len);
}

/* @func: ecp_hmac_md5_finish
 * #desc:
 *    hmac-md5 processing finish function.
 *
 * #1: ctx [in/out] hmac-md5 struct context
 * #2: len [in]     total length
 */
void ecp_hmac_md5_finish(struct hmac_md5_ctx *ctx, uint64_t len)
{
	ecp_md5_finish(&ctx->ipad_ctx, len + MD5_BLOCKSIZE);

	ecp_md5_process(&ctx->opad_ctx, ctx->opad, MD5_BLOCKSIZE);
	ecp_md5_process(&ctx->opad_ctx,
		&(MD5_STATE(&ctx->ipad_ctx, 0)), MD5_LEN);
	ecp_md5_finish(&ctx->opad_ctx, MD5_BLOCKSIZE + MD5_LEN);
}

/* @func: ecp_hmac_md5
 * #desc:
 *    hmac-md5 one-time processing function.
 *
 * #1: ctx [in/out] hmac-md5 struct context
 * #2: s   [in]     input buffer
 * #3: len [in]     input length
 */
void ecp_hmac_md5(struct hmac_md5_ctx *ctx, const uint8_t *s, size_t len)
{
	ecp_hmac_md5_process(ctx, s, len);
	ecp_hmac_md5_finish(ctx, len);
}

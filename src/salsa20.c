/* @file: salsa20.c
 * #desc:
 *    The implementations of salsa20 stream encryption.
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
#include <ecp/salsa20.h>

/* key padding constant */
static const uint8_t salsa20_constant[] = "expand 32-byte k";

#define PACK4(x) \
	((uint32_t)((x)[0]) | (uint32_t)((x)[1]) << 8 \
	| (uint32_t)((x)[2]) << 16 | (uint32_t)((x)[3]) << 24)

#define ROTL32(x, n) (((x) << (n)) | ((x) >> (32 - (n))))

/* quarter round */
#define QR(x, a, b, c, d) \
	(x)[b] ^= ROTL32((x)[a] + (x)[d], 7); \
	(x)[c] ^= ROTL32((x)[b] + (x)[a], 9); \
	(x)[d] ^= ROTL32((x)[c] + (x)[b], 13); \
	(x)[a] ^= ROTL32((x)[d] + (x)[c], 18)

/* @func: _hsalsa20_keygen (static)
 * #desc:
 *    xsalsa20 key generation.
 *
 * #1: state [out] salsa20 state
 * #2: key   [in]  input key
 * #3: ran   [in]  nonce
 */
static void _hsalsa20_keygen(uint32_t state[16], const uint8_t *key,
		const uint8_t *ran)
{
	state[0] = PACK4(salsa20_constant);
	state[5] = PACK4(salsa20_constant + 4);
	state[10] = PACK4(salsa20_constant + 8);
	state[15] = PACK4(salsa20_constant + 12);

	state[1] = PACK4(key);
	state[2] = PACK4(key + 4);
	state[3] = PACK4(key + 8);
	state[4] = PACK4(key + 12);
	state[11] = PACK4(key + 16);
	state[12] = PACK4(key + 20);
	state[13] = PACK4(key + 24);
	state[14] = PACK4(key + 28);

	state[6] = PACK4(ran);
	state[7] = PACK4(ran + 4);
	state[8] = PACK4(ran + 8);
	state[9] = PACK4(ran + 12);

	for (int32_t i = 0; i < SALSA20_ROUNDS; i += 2) {
		/* column */
		QR(state, 0, 4, 8, 12);
		QR(state, 5, 9, 13, 1);
		QR(state, 10, 14, 2, 6);
		QR(state, 15, 3, 7, 11);

		/* diagonal */
		QR(state, 0, 1, 2, 3);
		QR(state, 5, 6, 7, 4);
		QR(state, 10, 11, 8, 9);
		QR(state, 15, 12, 13, 14);
	}
}

/* @func: ecp_salsa20_init
 * #desc:
 *    salsa20 initialization function.
 *
 * #1: ctx [out] salsa20 struct context
 * #2: key [in]  input key
 * #3: ran [in]  nonce
 * #4: ctr [in]  counter
 */
void ecp_salsa20_init(struct salsa20_ctx *ctx, const uint8_t *key,
		const uint8_t *ran, const uint8_t *ctr)
{
	uint32_t *state = ctx->state;

	state[0] = PACK4(salsa20_constant);
	state[5] = PACK4(salsa20_constant + 4);
	state[10] = PACK4(salsa20_constant + 8);
	state[15] = PACK4(salsa20_constant + 12);

	state[1] = PACK4(key);
	state[2] = PACK4(key + 4);
	state[3] = PACK4(key + 8);
	state[4] = PACK4(key + 12);
	state[11] = PACK4(key + 16);
	state[12] = PACK4(key + 20);
	state[13] = PACK4(key + 24);
	state[14] = PACK4(key + 28);

	state[6] = PACK4(ran);
	state[7] = PACK4(ran + 4);

	state[8] = PACK4(ctr);
	state[9] = PACK4(ctr + 4);
}

/* @func: ecp_xsalsa20_init
 * #desc:
 *    xsalsa20 initialization function.
 *
 * #1: ctx [out] salsa20 struct context
 * #2: key [in]  input key
 * #3: ran [in]  nonce
 * #4: ctr [in]  counter
 */
void ecp_xsalsa20_init(struct salsa20_ctx *ctx, const uint8_t *key,
		const uint8_t *ran, const uint8_t *ctr)
{
	uint32_t *state = ctx->state;

	_hsalsa20_keygen(ctx->out.state, key, ran);
	key = ctx->out.keystream;

	state[0] = PACK4(salsa20_constant);
	state[5] = PACK4(salsa20_constant + 4);
	state[10] = PACK4(salsa20_constant + 8);
	state[15] = PACK4(salsa20_constant + 12);

	state[1] = PACK4(key);
	state[2] = PACK4(key + 4);
	state[3] = PACK4(key + 8);
	state[4] = PACK4(key + 12);
	state[11] = PACK4(key + 16);
	state[12] = PACK4(key + 20);
	state[13] = PACK4(key + 24);
	state[14] = PACK4(key + 28);

	state[6] = PACK4(ran + 16);
	state[7] = PACK4(ran + 20);

	state[8] = PACK4(ctr);
	state[9] = PACK4(ctr + 4);
}

/* @func: ecp_salsa20_block
 * #desc:
 *    salsa20 block keystream function.
 *
 * #1: ctx [in/out] salsa20 struct context
 */
void ecp_salsa20_block(struct salsa20_ctx *ctx)
{
	uint32_t *state = ctx->state, *out = ctx->out.state;

	out[0] = state[0];
	out[1] = state[1];
	out[2] = state[2];
	out[3] = state[3];
	out[4] = state[4];
	out[5] = state[5];
	out[6] = state[6];
	out[7] = state[7];
	out[8] = state[8];
	out[9] = state[9];
	out[10] = state[10];
	out[11] = state[11];
	out[12] = state[12];
	out[13] = state[13];
	out[14] = state[14];
	out[15] = state[15];

	for (int32_t i = 0; i < SALSA20_ROUNDS; i += 2) {
		/* column */
		QR(out, 0, 4, 8, 12);
		QR(out, 5, 9, 13, 1);
		QR(out, 10, 14, 2, 6);
		QR(out, 15, 3, 7, 11);

		/* diagonal */
		QR(out, 0, 1, 2, 3);
		QR(out, 5, 6, 7, 4);
		QR(out, 10, 11, 8, 9);
		QR(out, 15, 12, 13, 14);
	}

	out[0] += state[0];
	out[1] += state[1];
	out[2] += state[2];
	out[3] += state[3];
	out[4] += state[4];
	out[5] += state[5];
	out[6] += state[6];
	out[7] += state[7];
	out[8] += state[8];
	out[9] += state[9];
	out[10] += state[10];
	out[11] += state[11];
	out[12] += state[12];
	out[13] += state[13];
	out[14] += state[14];
	out[15] += state[15];
}

/* @func: ecp_salsa20_crypto
 * #desc:
 *    salsa20 stream encryption function.
 *
 * #1: ctx [in/out] salsa20 struct context
 * #2: buf [in/out] buffer
 * #3: len [in]     length
 */
void ecp_salsa20_crypto(struct salsa20_ctx *ctx, uint8_t *buf, size_t len)
{
	size_t n = SALSA20_BLOCKSIZE;
	while (len) {
		if (len < n)
			n = len;

		ecp_salsa20_block(ctx);
		for (size_t i = 0; i < n; i++)
			buf[i] ^= SALSA20_KEYSTREAM(ctx, i);

		buf += n;
		len -= n;
		SALSA20_COUNT1(ctx) += !++SALSA20_COUNT0(ctx);
	}
}

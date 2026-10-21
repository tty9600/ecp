/* @file: bits_get.c
 * #desc:
 *    The implementations of get bits from buffer.
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
#include <ecp/c/stdint.h>
#include <ecp/c/string.h>
#include <ecp/bits_get.h>

#undef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

/* @func: ecp_bits_getfill
 * #desc:
 *    fill the get buffer.
 *
 * #1: ctx [in/out] bits-get struct context
 * #2: s   [in]     input buffer
 * #3: len [in]     input length
 * #r:     [ret]    returns the length of fill
 */
uint32_t ecp_bits_getfill(struct bits_get_ctx *ctx, const uint8_t *s,
		uint32_t len)
{
	uint32_t rem = ctx->len - ctx->pos;
	if (rem <= 4 && ctx->len > 4) {
		ecp_memcpy(ctx->buf, ctx->buf + ctx->pos, rem);
		ctx->len = rem;
		ctx->pos = 0;
	}

	rem = BITS_GET_BUFSIZE - ctx->len;
	rem = MIN(rem, len);
	if (!rem)
		return 0;

	ecp_memcpy(ctx->buf + ctx->len, s, rem);
	ctx->len += rem;

	return rem;
}

/* @func: ecp_bits_get
 * #desc:
 *    get bits from the buffer (little-endian / msb-first).
 *
 * #1: ctx  [in/out] bits-get struct context
 * #2: v    [out]    bits result value
 * #3: bits [in]     bits length
 * #4: peek [in]     peeping bits only
 * #r:      [ret]    0: not end, -1: buffer end, >0: remaining unobtained bits
 */
int32_t ecp_bits_get(struct bits_get_ctx *ctx, uint32_t *v, uint32_t bits,
		int32_t peek)
{
	uint32_t n = ctx->pos;
	uint32_t cur = ctx->cur;

	if (n == ctx->len)
		return -1;

	static uint8_t mask[9] = {
		0x00,
		0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff
		};

	uint32_t _v = 0, w = 0, rem;
	while (n != ctx->len && bits) {
		rem = MIN(8 - cur, bits);
		_v |= (uint32_t)((ctx->buf[n] >> cur) & mask[rem]) << w;

		cur += rem;
		if (cur == 8) {
			cur = 0;
			n++;
		}

		w += rem;
		bits -= rem;
	}
	*v = _v;

	if (!peek) {
		ctx->pos = n;
		ctx->cur = cur;
	}

	return bits;
}

/* @func: ecp_bits_beget
 * #desc:
 *    get bits from the buffer (big-endian / lsb-first).
 *
 * #1: ctx  [in/out] bits-get struct context
 * #2: v    [out]    bits result value
 * #3: bits [in]     bits length
 * #4: peek [in]     peeping bits only
 * #r:      [ret]    0: not end, -1: buffer end, >0: remaining unobtained bits
 */
int32_t ecp_bits_beget(struct bits_get_ctx *ctx, uint32_t *v, uint32_t bits,
		int32_t peek)
{
	uint32_t n = ctx->pos;
	uint32_t cur = ctx->cur;

	if (n == ctx->len)
		return -1;

	static uint8_t mask[9] = {
		0x00,
		0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff
		};

	uint32_t _v = 0, _x, rem;
	while (n != ctx->len && bits) {
		rem = MIN(8 - cur, bits);
		_x = (ctx->buf[n] >> (8 - cur - rem)) & mask[rem];
		_v |= _x << (bits - rem);

		cur += rem;
		if (cur == 8) {
			cur = 0;
			n++;
		}

		bits -= rem;
	}
	*v = _v;

	if (!peek) {
		ctx->pos = n;
		ctx->cur = cur;
	}

	return bits;
}

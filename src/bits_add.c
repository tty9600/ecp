/* @file: bits_add.c
 * #desc:
 *    The implementations of adding bits to buffer.
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
#include <ecp/bits_add.h>

#undef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

/* @func: ecp_bits_add
 * #desc:
 *    add bits to the buffer (little-endian / msb-first).  flush the buffer
 *    when it is full.  if there are the remaining bits, then add it again
 *    (keep the original parameters).
 *
 * #1: ctx  [in/out] bits-add struct context
 * #2: v    [in]     bits value
 * #3: bits [in]     bits length
 * #r:      [ret]    0: not full, 1: buffer full
 */
int32_t ecp_bits_add(struct bits_add_ctx *ctx, uint32_t v, uint32_t bits)
{
	uint32_t n = ctx->size;
	uint32_t cur = ctx->cur;
	uint32_t rem = ctx->rem;

	if (n == BITS_ADD_BUFSIZE)
		return 1;

	if (rem) {
		v >>= bits - rem;
		bits = rem;
	}

	static uint8_t mask[9] = {
		0x00,
		0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff
		};

	while (n != BITS_ADD_BUFSIZE && bits) {
		rem = MIN(8 - cur, bits);
		ctx->buf[n] &= mask[cur];
		ctx->buf[n] |= (v & mask[rem]) << cur;

		cur += rem;
		if (cur == 8) {
			cur = 0;
			n++;
		}

		v >>= rem;
		bits -= rem;
	}

	ctx->size = n;
	ctx->cur = cur;
	ctx->rem = bits;

	return n == BITS_ADD_BUFSIZE;
}

/* @func: ecp_bits_beadd
 * #desc:
 *    add bits to the buffer (big-endian / lsb-first).
 *
 * #1: ctx  [in/out] bits-add struct context
 * #2: v    [in]     bits value
 * #3: bits [in]     bits length
 * #r:      [ret]    0: not full, 1: buffer full
 */
int32_t ecp_bits_beadd(struct bits_add_ctx *ctx, uint32_t v, uint32_t bits)
{
	uint32_t n = ctx->size;
	uint32_t cur = ctx->cur;
	uint32_t rem = ctx->rem;

	if (n == BITS_ADD_BUFSIZE)
		return 1;

	if (rem)
		bits = rem;

	static uint8_t mask[9] = {
		0x00,
		0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe, 0xff
		};
	static uint8_t mask2[9] = {
		0x00,
		0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff
		};

	uint32_t _v;
	while (n != BITS_ADD_BUFSIZE && bits) {
		rem = MIN(8 - cur, bits);
		ctx->buf[n] &= mask[cur];

		_v = (v >> (bits - rem)) & mask2[rem];
		ctx->buf[n] |= _v << (8 - cur - rem);

		cur += rem;
		if (cur == 8) {
			cur = 0;
			n++;
		}

		bits -= rem;
	}

	ctx->size = n;
	ctx->cur = cur;
	ctx->rem = bits;

	return n == BITS_ADD_BUFSIZE;
}

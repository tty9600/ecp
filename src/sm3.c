/* @file: sm3.c
 * #desc:
 *    The implementations of shangmi-3 cryptographic hash.
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

/* sm3 round constants (Tj <<< j mod 32) */
static const uint32_t sm3_constants[64] = {
	0x79cc4519, 0xf3988a32, 0xe7311465, 0xce6228cb,
	0x9cc45197, 0x3988a32f, 0x7311465e, 0xe6228cbc,
	0xcc451979, 0x988a32f3, 0x311465e7, 0x6228cbce,
	0xc451979c, 0x88a32f39, 0x11465e73, 0x228cbce6,
	0x9d8a7a87, 0x3b14f50f, 0x7629ea1e, 0xec53d43c,
	0xd8a7a879, 0xb14f50f3, 0x629ea1e7, 0xc53d43ce,
	0x8a7a879d, 0x14f50f3b, 0x29ea1e76, 0x53d43cec,
	0xa7a879d8, 0x4f50f3b1, 0x9ea1e762, 0x3d43cec5,
	0x7a879d8a, 0xf50f3b14, 0xea1e7629, 0xd43cec53,
	0xa879d8a7, 0x50f3b14f, 0xa1e7629e, 0x43cec53d,
	0x879d8a7a, 0x0f3b14f5, 0x1e7629ea, 0x3cec53d4,
	0x79d8a7a8, 0xf3b14f50, 0xe7629ea1, 0xcec53d43,
	0x9d8a7a87, 0x3b14f50f, 0x7629ea1e, 0xec53d43c,
	0xd8a7a879, 0xb14f50f3, 0x629ea1e7, 0xc53d43ce,
	0x8a7a879d, 0x14f50f3b, 0x29ea1e76, 0x53d43cec,
	0xa7a879d8, 0x4f50f3b1, 0x9ea1e762, 0x3d43cec5
	};

#define ROL(a, b) (((a) << (b)) | ((a) >> (32 - (b))))
#define P0(x) ((x) ^ ROL(x, 9) ^ ROL(x, 17))
#define P1(x) ((x) ^ ROL(x, 15) ^ ROL(x, 23))
#define FF0(a, b, c) ((a) ^ (b) ^ (c))
#define FF1(a, b, c) (((a) & (b)) | ((a) & (c)) | ((b) & (c)))
#define GG0(a, b, c) ((a) ^ (b) ^ (c))
#define GG1(a, b, c) (((a) & (b)) | (~(a) & (c)))

#define BSWAP32(x) \
	((((x) & 0xff) << 24) \
	| (((x) & 0xff00) << 8) \
	| (((x) & 0xff0000) >> 8) \
	| (((x) & 0xff000000) >> 24))

/* @func: _sm3_compress (static)
 * #desc:
 *    sm3 compression function.
 *
 * #1: ctx [in/out] sm3 struct context
 * #2: s   [in]     input block
 */
static void _sm3_compress(struct sm3_ctx *ctx, const uint8_t *s)
{
	uint32_t A, B, C, D, E, F, G, H, SS1, SS2, TT1, TT2, W[68];
	A = ctx->state[0];
	B = ctx->state[1];
	C = ctx->state[2];
	D = ctx->state[3];
	E = ctx->state[4];
	F = ctx->state[5];
	G = ctx->state[6];
	H = ctx->state[7];

#if 0

	for (int32_t i = 0; i < 16; i++) {
		W[i] = (uint32_t)s[0] << 24
			| (uint32_t)s[1] << 16
			| (uint32_t)s[2] << 8
			| (uint32_t)s[3];
		s += 4;
	}

	for (int32_t i = 16; i < 68; i++) {
		W[i] = P1(W[i - 16] ^ W[i - 9] ^ ROL(W[i - 3], 15))
			^ ROL(W[i - 13], 7) ^ W[i - 6];
	}

	for (int32_t i = 0; i < 64; i++) {
		SS1 = ROL(ROL(A, 12) + E + sm3_constants[i], 7);
		SS2 = SS1 ^ ROL(A, 12);

		if (i < 16) {
			TT1 = FF0(A, B, C) + D + SS2 + (W[i] ^ W[i + 4]);
			TT2 = GG0(E, F, G) + H + SS1 + W[i];
		} else {
			TT1 = FF1(A, B, C) + D + SS2 + (W[i] ^ W[i + 4]);
			TT2 = GG1(E, F, G) + H + SS1 + W[i];
		}

		D = C;
		C = ROL(B, 9);
		B = A;
		A = TT1;

		H = G;
		G = ROL(F, 19);
		F = E;
		E = P0(TT2);
	}

#else

#define PACK4_BE(x) \
	((uint32_t)((x)[0]) << 24 | (uint32_t)((x)[1]) << 16 \
	| (uint32_t)((x)[2]) << 8 | (uint32_t)((x)[3]))

#define RE(n) \
	W[n] = P1(W[n - 16] ^ W[n - 9] ^ ROL(W[n - 3], 15)) \
		^ ROL(W[n - 13], 7) ^ W[n - 6];

#define RO16(n) \
	SS1 = ROL(ROL(A, 12) + E + sm3_constants[n], 7); \
	SS2 = SS1 ^ ROL(A, 12); \
	TT1 = FF0(A, B, C) + D + SS2 + (W[n] ^ W[n + 4]); \
	TT2 = GG0(E, F, G) + H + SS1 + W[n]; \
	D = C; C = ROL(B, 9); B = A; A = TT1; \
	H = G; G = ROL(F, 19); F = E; E = P0(TT2)

#define RO64(n) \
	SS1 = ROL(ROL(A, 12) + E + sm3_constants[n], 7); \
	SS2 = SS1 ^ ROL(A, 12); \
	TT1 = FF1(A, B, C) + D + SS2 + (W[n] ^ W[n + 4]); \
	TT2 = GG1(E, F, G) + H + SS1 + W[n]; \
	D = C; C = ROL(B, 9); B = A; A = TT1; \
	H = G; G = ROL(F, 19); F = E; E = P0(TT2)

	W[0] = PACK4_BE(s); s += 4;
	W[1] = PACK4_BE(s); s += 4;
	W[2] = PACK4_BE(s); s += 4;
	W[3] = PACK4_BE(s); s += 4;
	W[4] = PACK4_BE(s); s += 4;
	W[5] = PACK4_BE(s); s += 4;
	W[6] = PACK4_BE(s); s += 4;
	W[7] = PACK4_BE(s); s += 4;
	W[8] = PACK4_BE(s); s += 4;
	W[9] = PACK4_BE(s); s += 4;
	W[10] = PACK4_BE(s); s += 4;
	W[11] = PACK4_BE(s); s += 4;
	W[12] = PACK4_BE(s); s += 4;
	W[13] = PACK4_BE(s); s += 4;
	W[14] = PACK4_BE(s); s += 4;
	W[15] = PACK4_BE(s);

	RE(16); RE(17);
	RE(18); RE(19);
	RE(20); RE(21);
	RE(22); RE(23);
	RE(24); RE(25);
	RE(26); RE(27);
	RE(28); RE(29);
	RE(30); RE(31);
	RE(32); RE(33);
	RE(34); RE(35);
	RE(36); RE(37);
	RE(38); RE(39);
	RE(40); RE(41);
	RE(42); RE(43);
	RE(44); RE(45);
	RE(46); RE(47);
	RE(48); RE(49);
	RE(50); RE(51);
	RE(52); RE(53);
	RE(54); RE(55);
	RE(56); RE(57);
	RE(58); RE(59);
	RE(60); RE(61);
	RE(62); RE(63);
	RE(64); RE(65);
	RE(66); RE(67);

	RO16(0); RO16(1);
	RO16(2); RO16(3);
	RO16(4); RO16(5);
	RO16(6); RO16(7);
	RO16(8); RO16(9);
	RO16(10); RO16(11);
	RO16(12); RO16(13);
	RO16(14); RO16(15);

	RO64(16); RO64(17);
	RO64(18); RO64(19);
	RO64(20); RO64(21);
	RO64(22); RO64(23);
	RO64(24); RO64(25);
	RO64(26); RO64(27);
	RO64(28); RO64(29);
	RO64(30); RO64(31);
	RO64(32); RO64(33);
	RO64(34); RO64(35);
	RO64(36); RO64(37);
	RO64(38); RO64(39);
	RO64(40); RO64(41);
	RO64(42); RO64(43);
	RO64(44); RO64(45);
	RO64(46); RO64(47);
	RO64(48); RO64(49);
	RO64(50); RO64(51);
	RO64(52); RO64(53);
	RO64(54); RO64(55);
	RO64(56); RO64(57);
	RO64(58); RO64(59);
	RO64(60); RO64(61);
	RO64(62); RO64(63);

#endif

	ctx->state[0] ^= A;
	ctx->state[1] ^= B;
	ctx->state[2] ^= C;
	ctx->state[3] ^= D;
	ctx->state[4] ^= E;
	ctx->state[5] ^= F;
	ctx->state[6] ^= G;
	ctx->state[7] ^= H;
}

/* @func: ecp_sm3_init
 * #desc:
 *    sm3 struct context initialization.
 *
 * #1: ctx [out] sm3 struct context
 */
void ecp_sm3_init(struct sm3_ctx *ctx)
{
	ctx->count = 0;
	ctx->state[0] = 0x7380166f;
	ctx->state[1] = 0x4914b2b9;
	ctx->state[2] = 0x172442d7;
	ctx->state[3] = 0xda8a0600;
	ctx->state[4] = 0xa96f30bc;
	ctx->state[5] = 0x163138aa;
	ctx->state[6] = 0xe38dee4d;
	ctx->state[7] = 0xb0fb0e4e;
}

/* @func: ecp_sm3_process
 * #desc:
 *    sm3 processing input function.
 *
 * #1: ctx [in/out] sm3 struct context
 * #2: s   [in]     input buffer
 * #3: len [in]     input length
 */
void ecp_sm3_process(struct sm3_ctx *ctx, const uint8_t *s, size_t len)
{
#define BLOCKSIZE SM3_BLOCKSIZE

	size_t n = ctx->count, h;
	switch (n) {
		default:
			h = BLOCKSIZE - n;
			h = (len < h) ? len : h;
			ecp_memcpy(ctx->buf + n, s, h);
			n += h;
			if (n != BLOCKSIZE)
				break;

			/* processing */
			_sm3_compress(ctx, ctx->buf);
			n = 0;
			s += h;
			len -= h;
		case 0:
			while (len >= BLOCKSIZE) {
				/* processing */
				_sm3_compress(ctx, s);
				s += BLOCKSIZE;
				len -= BLOCKSIZE;
			}

			n = len;
			if (n)
				ecp_memcpy(ctx->buf, s, n);
	}
	ctx->count = n;
}

/* @func: ecp_sm3_finish
 * #desc:
 *    sm3 processing finish function.
 *
 * #1: ctx [in/out] sm3 struct context
 * #2: len [in]     total length
 */
void ecp_sm3_finish(struct sm3_ctx *ctx, uint64_t len)
{
	/* padding */
	uint8_t padbuf[SM3_BLOCKSIZE];
	ecp_memset(padbuf, 0, sizeof(padbuf));
	padbuf[0] = 0x80;
	ecp_sm3_process(ctx, padbuf, 1 + ((119 - (len % 64)) % 64));

	/* bit length */
	len *= 8;
	for (int32_t i = 0; i < 8; i++) {
		ctx->buf[63 - i] = (uint8_t)len;
		len >>= 8;
	}
	_sm3_compress(ctx, ctx->buf);

	for (int32_t i = 0; i < 8; i++)
		ctx->state[i] = BSWAP32(ctx->state[i]);
}

/* @func: ecp_sm3
 * #desc:
 *    sm3 one-time processing function.
 *
 * #1: ctx [in/out] sm3 struct context
 * #2: s   [in]     input buffer
 * #3: len [in]     input length
 */
void ecp_sm3(struct sm3_ctx *ctx, const uint8_t *s, size_t len)
{
	ecp_sm3_process(ctx, s, len);
	ecp_sm3_finish(ctx, len);
}

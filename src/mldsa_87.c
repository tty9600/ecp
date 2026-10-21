/* @file: mldsa_87.c
 * #desc:
 *    The implementations of module-lattice-based digital signature
 *    standard (dilithium).
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
#include <ecp/mldsa.h>

/*
 * |   n   q       d  k ell eta tau beta gamma1 gamma2   omega pk   sk   sig |
 * |-------------------------------------------------------------------------|
 * |44 256 8380417 13 4 4   2   39  78   2^17   (q-1)/88 80    1132 2560 2420|
 * |65 256 8380417 13 6 5   4   49  196  2^19   (q-1)/32 55    1952 4032 3309|
 * |87 256 8380417 13 8 7   2   60  120  2^19   (q-1)/32 75    2592 4896 4627|
 */
#define MLDSA_MONT -4186625 /* 2^32 == 4193792 (mod q) */
#define MLDSA_QINV 58728449 /* q^-1 == 58728449 (mod 2^32) */
#define MLDSA_NORM 41978    /* mont^2*(256^-1) == 41978 (mod q) */

/* butterfly operations for root of unity */
static const int32_t zetas[MLDSA_N] = {
	-4186625,    25847, -2608894,  -518909,   237124,  -777960,  -876248,
	  466468,  1826347,  2353451,  -359251, -2091905,  3119733, -2884855,
	 3111497,  2680103,  2725464,  1024112, -1079900,  3585928,  -549488,
	-1119584,  2619752, -2108549, -2118186, -3859737, -1399561, -3277672,
	 1757237,   -19422,  4010497,   280005,  2706023,    95776,  3077325,
	 3530437, -1661693, -3592148, -2537516,  3915439, -3861115, -3043716,
	 3574422, -2867647,  3539968,  -300467,  2348700,  -539299, -1699267,
	-1643818,  3505694, -3821735,  3507263, -2140649, -1600420,  3699596,
	  811944,   531354,   954230,  3881043,  3900724, -2556880,  2071892,
	-2797779, -3930395, -1528703, -3677745, -3041255, -1452451,  3475950,
	 2176455, -1585221, -1257611,  1939314, -4083598, -1000202, -3190144,
	-3157330, -3632928,   126922,  3412210,  -983419,  2147896,  2715295,
	-2967645, -3693493,  -411027, -2477047,  -671102, -1228525,   -22981,
	-1308169,  -381987,  1349076,  1852771, -1430430, -3343383,   264944,
	  508951,  3097992,    44288, -1100098,   904516,  3958618, -3724342,
	   -8578,  1653064, -3249728,  2389356,  -210977,   759969, -1316856,
	  189548, -3553272,  3159746, -1851402, -2409325,  -177440,  1315589,
	 1341330,  1285669, -1584928,  -812732, -1439742, -3019102, -3881060,
	-3628969,  3839961,  2091667,  3407706,  2316500,  3817976, -3342478,
	 2244091, -2446433, -3562462,   266997,  2434439, -1235728,  3513181,
	-3520352, -3759364, -1197226, -3193378,   900702,  1859098,   909542,
	  819034,   495491, -1613174,   -43260,  -522500,  -655327, -3122442,
	 2031748,  3207046, -3556995,  -525098,  -768622, -3595838,   342297,
	  286988, -2437823,  4108315,  3437287, -3342277,  1735879,   203044,
	 2842341,  2691481, -2590150,  1265009,  4055324,  1247620,  2486353,
	 1595974, -3767016,  1250494,  2635921, -3548272, -2994039,  1869119,
	 1903435, -1050970, -1333058,  1237275, -3318210, -1430225,  -451100,
	 1312455,  3306115, -1962642, -1279661,  1917081, -2546312, -1374803,
	 1500165,   777191,  2235880,  3406031,  -542412, -2831860, -1671176,
	-1846953, -2584293, -3724270,   594136, -3776993, -2013608,  2432395,
	 2454455,  -164721,  1957272,  3369112,   185531, -1207385, -3183426,
	  162844,  1616392,  3014001,   810149,  1652634, -3694233, -1799107,
	-3038916,  3523897,  3866901,   269760,  2213111,  -975884,  1717735,
	  472078,  -426683,  1723600, -1803090,  1910376, -1667432, -1104333,
	 -260646, -3833893, -2939036, -2235985,  -420899, -2286327,   183443,
	 -976891,  1612842, -3545687,  -554416,  3919660,   -48306, -1362209,
	 3937738,  1400424,  -846154,  1976782
	};

struct poly {
	int32_t coeffs[MLDSA_N];
};

struct polyveck {
	struct poly vec[MLDSA_87_K];
};

struct polyvecl {
	struct poly vec[MLDSA_87_L];
};

#define MONT_MUL(a, b) _montgomery_reduce((int64_t)(a) * (b))
#define REDUCE_ONCE(a) _barrett_reduce(a)

/* @func: _montgomery_reduce (static)
 * #desc:
 *    montgomery reduction to (-q+1..q-1).
 *
 * #1: a [in]  number
 * #r:   [ret] result
 */
static int32_t _montgomery_reduce(int64_t a)
{
	int32_t t = (int32_t)((uint64_t)a * MLDSA_QINV);
	t = (a - (int64_t)t * MLDSA_Q) >> 32;

	return t;
}

/* @func: _barrett_reduce (static)
 * #desc:
 *    barrett reduction (-(q-1)/2..(q-1)/2).
 *
 * #1: a [in]  number
 * #r:   [ret] result
 */
static int32_t _barrett_reduce(int32_t a)
{
	/* round(x/q) == (x*(2^23/q)+(2^23/2)) / 2^23 */
	int32_t t = (a + 4194304) >> 23;
	t *= MLDSA_Q;

	return a - t;
}

/* @func: _power2round (static)
 * #desc
 *    decomposes into the coefficient 2^d.
 *
 * #1: r  [in]  number
 * #2: r0 [out] result low r0
 * #r:    [ret] result high r1
 */
static int32_t _power2round(int32_t r, int32_t *r0)
{
	int32_t r1;

	r += (r >> 31) & MLDSA_Q;
	r1 = (r + (1 << (MLDSA_D - 1)) - 1) >> MLDSA_D;
	*r0 = r - (r1 << MLDSA_D);

	return r1;
}

/* @func: _decompose (static)
 * #desc
 *    decomposes into the coefficient gamma2.
 *
 * #1: r  [in]  number
 * #2: r0 [out] result low r0
 * #r:    [ret] result high r1
 */
static int32_t _decompose(int32_t r, int32_t *r0)
{
	int32_t r1;

	r += (r >> 31) & MLDSA_Q;

	/* g = 2*gamma2/128, round(x/g) == (x*(2^22/g)+(2^22/2)) / 2^22 */
	r1 = (r + 127) >> 7;
	r1 = (r1 * 1025 + (1 << 21)) >> 22;
	r1 ^= ((15 - r1) >> 31) & r1;

	*r0 = r - (r1 * 2 * MLDSA_87_GAMMA2);
	*r0 -= (((MLDSA_Q - 1) / 2 - *r0) >> 31) & MLDSA_Q;

	return r1;
}

/* @func: _make_hint (static)
 * #desc:
 *    computes hint bit indicating whether adding a0 and a1 will change
 *    the high bit.
 *
 * #1: a0 [in]  number
 * #2: a1 [in]  number
 * #r:    [ret] return the boolean of hint
 */
static int32_t _make_hint(int32_t a0, int32_t a1)
{
	if (a0 > MLDSA_87_GAMMA2 || a0 < -MLDSA_87_GAMMA2
			|| (a0 == -MLDSA_87_GAMMA2 && a1 != 0))
		return 1;

	return 0;
}

/* @func: _use_hint (static)
 * #desc:
 *    adjust the high bit of r according to hint bit.
 *
 * #1: r [in]  number
 * #2: h [in]  boolean of hint
 * #r:   [ret] return the adjusted number
 */
static int32_t _use_hint(int32_t r, int32_t h)
{
	int32_t r0, r1;

	r1 = _decompose(r, &r0);
	if (!h)
		return r1;

	/* 15 = q/(2*gamma2) - 1 */
	if (r0 > 0) {
		if (r1 == 15)
			return 0;
		return r1 + 1;
	}
	if (!r1)
		return 15;

	return r1 - 1;
}

/* @func: _poly_ntt (static)
 * #desc:
 *    polynomial number-theoretic transform.
 *
 * #1: r [in/out] polynomial
 */
static void _poly_ntt(struct poly *r)
{
	int32_t len, start, j, k;
	int32_t t, zeta;
	int32_t *rr = r->coeffs;

	k = 1;
	for (len = MLDSA_N / 2; len > 0; len >>= 1) {
		for (start = 0; start < MLDSA_N; start = j + len) {
			zeta = zetas[k++];
			for (j = start; j < (start + len); j++) {
				t = MONT_MUL(zeta, rr[j + len]);
				rr[j + len] = rr[j] - t;
				rr[j] = rr[j] + t;
			}
		}
	}
}

/* @func: _poly_invntt (static)
 * #desc:
 *    polynomial inverse number-theoretic transform and multiply by
 *    montgomery factor.
 *
 * #1: r [in/out] polynomial
 */
static void _poly_invntt(struct poly *r)
{
	int32_t start, len, j, k;
	int32_t t, zeta;
	int32_t *rr = r->coeffs;

	k = MLDSA_N - 1;
	for (len = 1; len < MLDSA_N; len <<= 1) {
		for (start = 0; start < MLDSA_N; start = j + len) {
			zeta = -zetas[k--];
			for (j = start; j < (start + len); j++) {
				t = rr[j];
				rr[j] = t + rr[j + len];
				rr[j + len] = MONT_MUL(zeta, t - rr[j + len]);
			}
		}
	}

	for (j = 0; j < MLDSA_N; j++)
		rr[j] = MONT_MUL(rr[j], MLDSA_NORM);
}

/* @func: _poly_reduce (static)
 * #desc:
 *    polynomial reduction.
 *
 * #1: r [in/out] polynomial
 */
static void _poly_reduce(struct poly *r)
{
	for (int32_t i = 0; i < MLDSA_N; i++)
		r->coeffs[i] = REDUCE_ONCE(r->coeffs[i]);
}

/* @func: _poly_add (static)
 * #desc:
 *    polynomial addition.
 *
 * #1: r [out] output polynomial
 * #2: a [in]  input polynomial
 * #3: b [in]  input polynomial
 */
static void _poly_add(struct poly *r,
		const struct poly *a, const struct poly *b)
{
	for (int32_t i = 0; i < MLDSA_N; i++)
		r->coeffs[i] = a->coeffs[i] + b->coeffs[i];
}

/* @func: _poly_sub (static)
 * #desc:
 *    polynomial subtraction.
 *
 * #1: r [out] output polynomial
 * #2: a [in]  input polynomial
 * #3: b [in]  input polynomial
 */
static void _poly_sub(struct poly *r,
		const struct poly *a, const struct poly *b)
{
	for (int32_t i = 0; i < MLDSA_N; i++)
		r->coeffs[i] = a->coeffs[i] - b->coeffs[i];
}

/* @func: _poly_pointwise (static)
 * #desc:
 *    polynomial pointwise multiplication in ntt domain.
 *
 * #1: r [out] output polynomial
 * #2: a [in]  input polynomial
 * #3: b [in]  input polynomial
 */
static void _poly_pointwise(struct poly *r,
		const struct poly *a, const struct poly *b)
{
	for (int32_t i = 0; i < MLDSA_N; i++)
		r->coeffs[i] = MONT_MUL(a->coeffs[i], b->coeffs[i]);
}

/* @func: _poly_shiftl (static)
 * #desc:
 *    polynomial multiply by 2^d without modular reduction.
 *
 * #1: r [in/out] polynomial
 */
static void _poly_shiftl(struct poly *r)
{
	for (int32_t i = 0; i < MLDSA_N; i++)
		r->coeffs[i] <<= MLDSA_D;
}

/* @func: _poly_power2round (static)
 * #desc
 *    polynomial decomposes into the coefficients 2^d.
 *
 * #1: r1 [out] output polynomial
 * #2: r0 [out] output polynomial
 * #3: r  [in]  input polynomial
 */
static void _poly_power2round(struct poly *r1, struct poly *r0,
		const struct poly *r)
{
	for (int32_t i = 0; i < MLDSA_N; i++)
		r1->coeffs[i] = _power2round(r->coeffs[i], &r0->coeffs[i]);
}

/* @func: _poly_decompose (static)
 * #desc
 *    polynomial decomposes into the coefficients gamma2.
 *
 * #1: r1 [out] output polynomial
 * #2: r0 [out] output polynomial
 * #3: r  [in]  input polynomial
 */
static void _poly_decompose(struct poly *r1, struct poly *r0,
		const struct poly *r)
{
	for (int32_t i = 0; i < MLDSA_N; i++)
		r1->coeffs[i] = _decompose(r->coeffs[i], &r0->coeffs[i]);
}

/* @func: _poly_make_hint (static)
 * #desc:
 *    polynomial computes hint bit indicating whether adding a0 and a1 will change
 *    the high bit.
 *
 * #1: h  [out] output polynomial
 * #2: a0 [in]  input polynomial
 * #3: a1 [in]  input polynomial
 * #r:    [ret] return the number of boolean of hint
 */
static int32_t _poly_make_hint(struct poly *h,
		const struct poly *a0, const struct poly *a1)
{
	int32_t s = 0;

	for (int32_t i = 0; i < MLDSA_N; i++) {
		h->coeffs[i] = _make_hint(a0->coeffs[i], a1->coeffs[i]);
		s += h->coeffs[i];
	}

	return s;
}

/* @func: _poly_use_hint (static)
 * #desc:
 *    polynomial adjust the high bit of r according to hint bit.
 *
 * #1: r1 [out] output polynomial
 * #2: r  [in]  input polynomial
 * #3: h  [in]  input polynomial
 */
static void _poly_use_hint(struct poly *r1,
		const struct poly *r, const struct poly *h)
{
	for (int32_t i = 0; i < MLDSA_N; i++)
		r1->coeffs[i] = _use_hint(r->coeffs[i], h->coeffs[i]);
}

/* @func: _poly_check_norm (static)
 * #desc:
 *    check polynomial coefficients bound.
 *
 * #1: a [in]  input polynomial
 * #2: b [in]  coefficient bound
 * #r:   [ret] 0: no error, -1: error
 */
static int32_t _poly_check_norm(const struct poly *a, int32_t b)
{
	int32_t t;

	for (int32_t i = 0; i < MLDSA_N; i++) {
		t = a->coeffs[i] >> 31;
		t = a->coeffs[i] - (t & (a->coeffs[i] * 2));
		if (t >= b)
			return -1;
	}

	return 0;
}

/* @func: _polyt1_tobytes (static)
 * #desc:
 *    polynomial t1 to bytes conversion.
 *
 * #1: a [in]  input polynomial
 * #2: r [out] output bytes
 */
static void _polyt1_tobytes(const struct poly *a, uint8_t *r)
{
	for (int32_t i = 0; i < MLDSA_N; i += 4) {
		r[0] = (uint8_t)a->coeffs[i];
		r[1] = (uint8_t)(a->coeffs[i] >> 8
			| a->coeffs[i + 1] << 2);
		r[2] = (uint8_t)(a->coeffs[i + 1] >> 6
			| a->coeffs[i + 2] << 4);
		r[3] = (uint8_t)(a->coeffs[i + 2] >> 4
			| a->coeffs[i + 3] << 6);
		r[4] = (uint8_t)(a->coeffs[i + 3] >> 2);
		r += 5;
	}
}

/* @func: _polyt1_frombytes (static)
 * #desc:
 *    bytes to polynomial t1 conversion.
 *
 * #1: r [in]  output polynomial
 * #2: a [out] input bytes
 */
static void _polyt1_frombytes(struct poly *r, const uint8_t *a)
{
	for (int32_t i = 0; i < MLDSA_N; i += 4) {
		r->coeffs[i] = ((uint32_t)a[0]
			| (uint32_t)a[1] << 8) & 0x3ff;
		r->coeffs[i + 1] = ((uint32_t)a[1] >> 2
			| (uint32_t)a[2] << 6) & 0x3ff;
		r->coeffs[i + 2] = ((uint32_t)a[2] >> 4
			| (uint32_t)a[3] << 4) & 0x3ff;
		r->coeffs[i + 3] = ((uint32_t)a[3] >> 6
			| (uint32_t)a[4] << 2) & 0x3ff;
		a += 5;
	}
}

/* @func: _polyt0_tobytes (static)
 * #desc:
 *    polynomial t0 to bytes conversion.
 *
 * #1: a [in]  input polynomial
 * #2: r [out] output bytes
 */
static void _polyt0_tobytes(const struct poly *a, uint8_t *r)
{
	uint32_t t[8];

	for (int32_t i = 0; i < MLDSA_N; i += 8) {
		for (int32_t j = 0; j < 8; j++)
			t[j] = (1 << (MLDSA_D - 1)) - a->coeffs[i + j];

		r[0] = (uint8_t)t[0];
		r[1] = (uint8_t)(t[0] >> 8 | t[1] << 5);
		r[2] = (uint8_t)(t[1] >> 3);
		r[3] = (uint8_t)(t[1] >> 11 | t[2] << 2);
		r[4] = (uint8_t)(t[2] >> 6 | t[3] << 7);
		r[5] = (uint8_t)(t[3] >> 1);
		r[6] = (uint8_t)(t[3] >> 9 | t[4] << 4);
		r[7] = (uint8_t)(t[4] >> 4);
		r[8] = (uint8_t)(t[4] >> 12 | t[5] << 1);
		r[9] = (uint8_t)(t[5] >> 7 | t[6] << 6);
		r[10] = (uint8_t)(t[6] >> 2);
		r[11] = (uint8_t)(t[6] >> 10 | t[7] << 3);
		r[12] = (uint8_t)(t[7] >> 5);
		r += 13;
	}
}

/* @func: _polyt0_frombytes (static)
 * #desc:
 *    bytes to polynomial eta conversion.
 *
 * #1: r [in]  output polynomial
 * #2: a [out] input bytes
 */
static void _polyt0_frombytes(struct poly *r, const uint8_t *a)
{
	uint32_t t[8];

	for (int32_t i = 0; i < MLDSA_N; i += 8) {
		t[0] = ((uint32_t)a[0]
			| (uint32_t)a[1] << 8) & 0x1fff;
		t[1] = ((uint32_t)a[1] >> 5
			| (uint32_t)a[2] << 3
			| (uint32_t)a[3] << 11) & 0x1fff;
		t[2] = ((uint32_t)a[3] >> 2
			| (uint32_t)a[4] << 6) & 0x1fff;
		t[3] = ((uint32_t)a[4] >> 7
			| (uint32_t)a[5] << 1
			| (uint32_t)a[6] << 9) & 0x1fff;
		t[4] = ((uint32_t)a[6] >> 4
			| (uint32_t)a[7] << 4
			| (uint32_t)a[8] << 12) & 0x1fff;
		t[5] = ((uint32_t)a[8] >> 1
			| (uint32_t)a[9] << 7) & 0x1fff;
		t[6] = ((uint32_t)a[9] >> 6
			| (uint32_t)a[10] << 2
			| (uint32_t)a[11] << 10) & 0x1fff;
		t[7] = ((uint32_t)a[11] >> 3
			| (uint32_t)a[12] << 5) & 0x1fff;
		a += 13;

		for (int32_t j = 0; j < 8; j++)
			r->coeffs[i + j] = (1 << (MLDSA_D - 1)) - t[j];
	}
}

/* @func: _polyeta_tobytes (static)
 * #desc:
 *    polynomial eta to bytes conversion.
 *
 * #1: a [in]  input polynomial
 * #2: r [out] output bytes
 */
static void _polyeta_tobytes(const struct poly *a, uint8_t *r)
{
	uint8_t t[8];

	for (int32_t i = 0; i < MLDSA_N; i += 8) {
		for (int32_t j = 0; j < 8; j++)
			t[j] = MLDSA_87_ETA - a->coeffs[i + j];

		r[0] = (uint8_t)(t[0] | t[1] << 3 | t[2] << 6);
		r[1] = (uint8_t)(t[2] >> 2 | t[3] << 1 | t[4] << 4
			| t[5] << 7);
		r[2] = (uint8_t)(t[5] >> 1 | t[6] << 2 | t[7] << 5);
		r += 3;
	}
}

/* @func: _polyeta_frombytes (static)
 * #desc:
 *    bytes to polynomial eta conversion.
 *
 * #1: r [in]  output polynomial
 * #2: a [out] input bytes
 */
static void _polyeta_frombytes(struct poly *r, const uint8_t *a)
{
	uint8_t t[8];

	for (int32_t i = 0; i < MLDSA_N; i += 8) {
		t[0] = a[0] & 7;
		t[1] = (a[0] >> 3) & 7;
		t[2] = (a[0] >> 6 | a[1] << 2) & 7;
		t[3] = (a[1] >> 1) & 7;
		t[4] = (a[1] >> 4) & 7;
		t[5] = (a[1] >> 7 | a[2] << 1) & 7;
		t[6] = (a[2] >> 2) & 7;
		t[7] = (a[2] >> 5) & 7;
		a += 3;

		for (int32_t j = 0; j < 8; j++)
			r->coeffs[i + j] = MLDSA_87_ETA - t[j];
	}
}

/* @func: _polyz_tobytes (static)
 * #desc:
 *    polynomial z to bytes conversion.
 *
 * #1: a [in]  input polynomial
 * #2: r [out] output bytes
 */
static void _polyz_tobytes(const struct poly *a, uint8_t *r)
{
	uint32_t t[2];

	for (int32_t i = 0; i < MLDSA_N; i += 2) {
		for (int32_t j = 0; j < 2; j++)
			t[j] = MLDSA_87_GAMMA1 - a->coeffs[i + j];

		r[0] = (uint8_t)t[0];
		r[1] = (uint8_t)(t[0] >> 8);
		r[2] = (uint8_t)(t[0] >> 16 | t[1] << 4);
		r[3] = (uint8_t)(t[1] >> 4);
		r[4] = (uint8_t)(t[1] >> 12);
		r += 5;
	}
}

/* @func: _polyz_frombytes (static)
 * #desc:
 *    bytes to polynomial z conversion.
 *
 * #1: r [out] output polynomial
 * #2: a [in]  input bytes
 */
static void _polyz_frombytes(struct poly *r, const uint8_t *a)
{
	uint32_t t[2];

	for (int32_t i = 0; i < MLDSA_N; i += 2) {
		t[0] = ((uint32_t)a[0] | (uint32_t)a[1] << 8
			| (uint32_t)a[2] << 16) & 0xfffff;
		t[1] = ((uint32_t)a[2] >> 4 | (uint32_t)a[3] << 4
			| (uint32_t)a[4] << 12) & 0xfffff;
		a += 5;

		for (int32_t j = 0; j < 2; j++)
			r->coeffs[i + j] = MLDSA_87_GAMMA1 - t[j];
	}
}

/* @func: _poly_w1pack (static)
 * #desc:
 *    polynomial w1 to bytes conversion.
 *
 * #1: a [in]  input polynomial
 * #2: r [out] output bytes
 */
static void _poly_w1pack(const struct poly *a, uint8_t *r)
{
	for (int32_t i = 0; i < MLDSA_N; i += 2)
		*r++ = (uint8_t)(a->coeffs[i] | a->coeffs[i + 1] << 4);
}

/* @func: _polyveck_ntt (static)
 * #desc:
 *    polynomial vector-k number-theoretic transform.
 *
 * #1: r [in/out] polynomial vector-k
 */
static void _polyveck_ntt(struct polyveck *r)
{
	for (int32_t i = 0; i < MLDSA_87_K; i++)
		_poly_ntt(&r->vec[i]);
}

/* @func: _polyveck_invntt (static)
 * #desc:
 *    polynomial vector-k inverse number-theoretic transform and multiply by
 *    montgomery factor.
 *
 * #1: r [in/out] polynomial vector-k
 */
static void _polyveck_invntt(struct polyveck *r)
{
	for (int32_t i = 0; i < MLDSA_87_K; i++)
		_poly_invntt(&r->vec[i]);
}

/* @func: _polyveck_add (static)
 * #desc:
 *    polynomial vector-k addition.
 *
 * #1: r [out] output polynomial vector-k
 * #2: a [in]  input polynomial vector-k
 * #3: b [in]  input polynomial vector-k
 */
static void _polyveck_add(struct polyveck *r,
		const struct polyveck *a, const struct polyveck *b)
{
	for (int32_t i = 0; i < MLDSA_87_K; i++)
		_poly_add(&r->vec[i], &a->vec[i], &b->vec[i]);
}

/* @func: _polyveck_sub (static)
 * #desc:
 *    polynomial vector-k subtraction.
 *
 * #1: r [out] output polynomial vector-k
 * #2: a [in]  input polynomial vector-k
 * #3: b [in]  input polynomial vector-k
 */
static void _polyveck_sub(struct polyveck *r,
		const struct polyveck *a, const struct polyveck *b)
{
	for (int32_t i = 0; i < MLDSA_87_K; i++)
		_poly_sub(&r->vec[i], &a->vec[i], &b->vec[i]);
}

/* @func: _polyveck_reduce (static)
 * #desc:
 *    polynomial vector-k reduction.
 *
 * #1: r [in/out] polynomial vector-k
 */
static void _polyveck_reduce(struct polyveck *r)
{
	for (int32_t i = 0; i < MLDSA_87_K; i++)
		_poly_reduce(&r->vec[i]);
}

/* @func: _polyveck_shiftl (static)
 * #desc:
 *    polynomial vector-k multiply by 2^d without modular reduction.
 *
 * #1: r [in/out] polynomial vector-k
 */
static void _polyveck_shiftl(struct polyveck *r)
{
	for (int32_t i = 0; i < MLDSA_87_K; i++)
		_poly_shiftl(&r->vec[i]);
}

/* @func: _polyveck_power2round (static)
 * #desc
 *    polynomial vector-k decomposes into the coefficients 2^d.
 *
 * #1: r1 [out] output polynomial vector-k
 * #2: r0 [out] output polynomial vector-k
 * #3: r  [in]  input polynomial vector-k
 */
static void _polyveck_power2round(struct polyveck *r1, struct polyveck *r0,
		const struct polyveck *r)
{
	for (int32_t i = 0; i < MLDSA_87_K; i++)
		_poly_power2round(&r1->vec[i], &r0->vec[i], &r->vec[i]);
}

/* @func: _polyveck_decompose (static)
 * #desc
 *    polynomial vector-k decomposes into the coefficients gamma2.
 *
 * #1: r1 [out] output polynomial vector-k
 * #2: r0 [out] output polynomial vector-k
 * #3: r  [in]  input polynomial vector-k
 */
static void _polyveck_decompose(struct polyveck *r1, struct polyveck *r0,
		const struct polyveck *r)
{
	for (int32_t i = 0; i < MLDSA_87_K; i++)
		_poly_decompose(&r1->vec[i], &r0->vec[i], &r->vec[i]);
}

/* @func: _polyveck_w1pack (static)
 * #desc:
 *    polynomial vector w1 to bytes conversion.
 *
 * #1: a [in]  input polynomial vector-k
 * #r:   [ret] output bytes
 */
static void _polyveck_w1pack(const struct polyveck *a, uint8_t *r)
{
	for (int32_t i = 0; i < MLDSA_87_K; i++) {
		_poly_w1pack(&a->vec[i], r);
		r += MLDSA_87_POLYW1_BYTES;
	}
}

/* @func: _polyveck_make_hint (static)
 * #desc:
 *    polynomial vector-k computes hint bit indicating whether adding a0 and
 *    a1 will change the high bit.
 *
 * #1: h  [out] output polynomial vector-k
 * #2: a0 [in]  input polynomial vector-k
 * #3: a1 [in]  input polynomial vector-k
 * #r:    [ret] return the number of boolean of hint
 */
static int32_t _polyveck_make_hint(struct polyveck *h,
		const struct polyveck *a0, const struct polyveck *a1)
{
	int32_t s = 0;

	for (int32_t i = 0; i < MLDSA_87_K; i++)
		s += _poly_make_hint(&h->vec[i], &a0->vec[i], &a1->vec[i]);

	return s;
}

/* @func: _polyveck_use_hint (static)
 * #desc:
 *    polynomial vector-k adjust the high bit of r according to hint bit.
 *
 * #1: r  [out] output polynomial vector-k
 * #2: r0 [in]  input polynomial vector-k
 * #3: h  [in]  input polynomial vector-k
 */
static void _polyveck_use_hint(struct polyveck *r,
		const struct polyveck *r0, const struct polyveck *h)
{
	for (int32_t i = 0; i < MLDSA_87_K; i++)
		_poly_use_hint(&r->vec[i], &r0->vec[i], &h->vec[i]);
}

/* @func: _polyveck_check_norm (static)
 * #desc:
 *    check polynomial vector-k coefficients bound.
 *
 * #1: a [in]  input polynomial vector-k
 * #2: b [in]  coefficient bound
 * #r:   [ret] 0: no error, -1: error
 */
static int32_t _polyveck_check_norm(const struct polyveck *a, int32_t b)
{
	for (int32_t i = 0; i < MLDSA_87_K; i++) {
		if (_poly_check_norm(&a->vec[i], b))
			return -1;
	}

	return 0;
}

/* @func: _polyvecl_ntt (static)
 * #desc:
 *    polynomial vector-l number-theoretic transform.
 *
 * #1: r [in/out] polynomial vector-l
 */
static void _polyvecl_ntt(struct polyvecl *r)
{
	for (int32_t i = 0; i < MLDSA_87_L; i++)
		_poly_ntt(&r->vec[i]);
}

/* @func: _polyvecl_invntt (static)
 * #desc:
 *    polynomial vector-l inverse number-theoretic transform and multiply by
 *    montgomery factor.
 *
 * #1: r [in/out] polynomial vector-l
 */
static void _polyvecl_invntt(struct polyvecl *r)
{
	for (int32_t i = 0; i < MLDSA_87_L; i++)
		_poly_invntt(&r->vec[i]);
}

/* @func: _polyvecl_add (static)
 * #desc:
 *    polynomial vector-l addition.
 *
 * #1: r [out] output polynomial vector-l
 * #2: a [in]  input polynomial vector-l
 * #3: b [in]  input polynomial vector-l
 */
static void _polyvecl_add(struct polyvecl *r,
		const struct polyvecl *a, const struct polyvecl *b)
{
	for (int32_t i = 0; i < MLDSA_87_L; i++)
		_poly_add(&r->vec[i], &a->vec[i], &b->vec[i]);
}

/* @func: _polyvecl_reduce (static)
 * #desc:
 *    polynomial vector-l reduction.
 *
 * #1: r [in/out] polynomial vector-l
 */
static void _polyvecl_reduce(struct polyvecl *r)
{
	for (int32_t i = 0; i < MLDSA_87_L; i++)
		_poly_reduce(&r->vec[i]);
}

/* @func: _polyvecl_pointwise_acc (static)
 * #desc:
 *    polynomial vector-k/l pointwise multiplication in ntt domain, and
 *    accumulate into r.
 *
 * #1: r [out] output polynomial vector-l
 * #2: a [in]  input polynomial vector-l
 * #3: b [in]  input polynomial vector-l
 */
static void _polyvecl_pointwise_acc(struct poly *r,
		const struct polyvecl *a, const struct polyvecl *b)
{
	struct poly t;

	_poly_pointwise(r, &a->vec[0], &b->vec[0]);
	for (int32_t i = 1; i < MLDSA_87_L; i++) {
		_poly_pointwise(&t, &a->vec[i], &b->vec[i]);
		_poly_add(r, r, &t);
	}
}

/* @func: _polyvecl_check_norm (static)
 * #desc:
 *    check polynomial vector-l coefficients bound.
 *
 * #1: a [in]  input polynomial vector-l
 * #2: b [in]  coefficient bound
 * #r:   [ret] 0: no error, -1: error
 */
static int32_t _polyvecl_check_norm(const struct polyvecl *a, int32_t b)
{
	for (int32_t i = 0; i < MLDSA_87_L; i++) {
		if (_poly_check_norm(&a->vec[i], b))
			return -1;
	}

	return 0;
}

/* @func: _rej_uniform (static)
 * #desc:
 *    run rejection sampling on uniform random bytes to generate polynomial
 *    coefficients in ntt representations.
 *
 * #1: r   [out] output coeffs
 * #2: n   [in]  coeffs number
 * #3: buf [in]  input buffer
 * #4: len [in]  input length
 * #r:     [ret] return sample number
 */
static uint32_t _rej_uniform(int32_t *r, uint32_t n, const uint8_t *buf,
		uint32_t len)
{
	uint32_t k = 0, d;

	while (k < n && len > 2) {
		d = (uint16_t)buf[0] | (uint16_t)buf[1] << 8
			| (uint16_t)buf[2] << 16;
		d &= 0x7fffff;
		buf += 3;
		len -= 3;

		if (d < MLDSA_Q)
			r[k++] = d;
	}

	return k;
}

/* @func: _sample_poly_uniform (static)
 * #desc:
 *    run rejection sampling on uniform random bytes to generate polynomial
 *    in ntt representations.
 *
 * #1: r    [out] output polynomial
 * #2: seed [in]  input seed
 * #3: x    [in]  nonce x
 * #4: y    [in]  nonce y
 */
static void _sample_poly_uniform(struct poly *r, const uint8_t *seed,
		uint8_t x, uint8_t y)
{
	uint8_t buf[SHA3_SHAKE128_RATE];
	SHA3_NEW(ctx);

	buf[0] = x;
	buf[1] = y;
	ecp_sha3_init(&ctx, SHA3_SHAKE128_TYPE, 0);
	ecp_sha3_process(&ctx, seed, MLDSA_SYM_LEN);
	ecp_sha3_process(&ctx, buf, 2);
	ecp_sha3_finish(&ctx);

	uint32_t n = 0;
	do {
		ecp_sha3_shake_xof(&ctx, buf, sizeof(buf));
		n += _rej_uniform(r->coeffs + n, MLDSA_N - n, buf, sizeof(buf));
	} while (n < MLDSA_N);
}

/* @func: _rej_eta (static)
 * #desc:
 *    run rejection sampling on uniform random bytes to generate polynomial
 *    coefficients in [-eta, eta].
 *
 * #1: r   [out] output coeffs
 * #2: n   [in]  coeffs number
 * #3: buf [in]  input buffer
 * #4: len [in]  input length
 * #r:     [ret] return sample number
 */
static uint32_t _rej_eta(int32_t *r, uint32_t n, const uint8_t *buf,
		uint32_t len)
{
	uint32_t k = 0, d0, d1;

	while (k < n && len) {
		d0 = buf[0] & 0xf;
		d1 = buf[0] >> 4;
		buf++;
		len--;

		if (d0 < 15) {
			d0 -= ((d0 * 205) >> 10) * 5;
			r[k++] = 2 - d0;
		}
		if (d1 < 15 && k < n) {
			d1 -= ((d1 * 205) >> 10) * 5;
			r[k++] = 2 - d1;
		}
	}

	return k;
}

/* @func: _sample_poly_eta (static)
 * #desc:
 *    run rejection sampling on uniform random bytes to generate polynomial
 *    in coefficients [-eta, eta].
 *
 * #1: r    [out] output polynomial
 * #2: seed [in]  input seed
 * #3: xy    [in] nonce
 */
static void _sample_poly_eta(struct poly *r, const uint8_t *seed,
		uint16_t xy)
{
	uint8_t buf[SHA3_SHAKE256_RATE];
	SHA3_NEW(ctx);

	buf[0] = (uint8_t)xy;
	buf[1] = (uint8_t)(xy >> 8);
	ecp_sha3_init(&ctx, SHA3_SHAKE256_TYPE, 0);
	ecp_sha3_process(&ctx, seed, MLDSA_SYM_LEN * 2);
	ecp_sha3_process(&ctx, buf, 2);
	ecp_sha3_finish(&ctx);

	uint32_t n = 0;
	do {
		ecp_sha3_shake_xof(&ctx, buf, sizeof(buf));
		n += _rej_eta(r->coeffs + n, MLDSA_N - n, buf, sizeof(buf));
	} while (n < MLDSA_N);
}

/* @func: _sample_poly_gamma1 (static)
 * #desc:
 *    extract the polynomial of the coefficient gamma1.
 *
 * #1: r    [out] output polynomial
 * #2: seed [in]  input seed
 * #3: xy   [in]  nonce
 */
static void _sample_poly_gamma1(struct poly *r, const uint8_t *seed,
		uint16_t xy)
{
	uint8_t buf[((MLDSA_87_POLYZ_BYTES + (SHA3_SHAKE256_RATE - 1))
		/ SHA3_SHAKE256_RATE) * SHA3_SHAKE256_RATE];
	SHA3_NEW(ctx);

	buf[0] = (uint8_t)xy;
	buf[1] = (uint8_t)(xy >> 8);
	ecp_sha3_init(&ctx, SHA3_SHAKE256_TYPE, 0);
	ecp_sha3_process(&ctx, seed, MLDSA_SYM_LEN * 2);
	ecp_sha3_process(&ctx, buf, 2);
	ecp_sha3_finish(&ctx);

	ecp_sha3_shake_xof(&ctx, buf, sizeof(buf));
	_polyz_frombytes(r, buf);
}

/* @func: _sample_poly_challenge (static)
 * #desc:
 *    extract the polynomial of the coefficient challenge.
 *
 * #1: r    [out] output polynomial
 * #2: seed [in]  input seed
 */
static void _sample_poly_challenge(struct poly *r, const uint8_t *seed)
{
	uint8_t buf[SHA3_SHAKE256_RATE];
	SHA3_NEW(ctx);

	ecp_sha3_init(&ctx, SHA3_SHAKE256_TYPE, 0);
	ecp_sha3_process(&ctx, seed, MLDSA_87_C_BYTES);
	ecp_sha3_finish(&ctx);
	ecp_sha3_shake_xof(&ctx, buf, sizeof(buf));

	uint64_t signs = 0;
	for (int32_t i = 0; i < 8; i++)
		signs |= (uint64_t)buf[i] << (8 * i);

	for (int32_t i = 0; i < MLDSA_N; i++)
		r->coeffs[i] = 0;

	int32_t pos = 8;
	for (int32_t i = MLDSA_N - MLDSA_87_TAU; i < MLDSA_N; i++) {
		int32_t j = 0;
		do {
			if (pos >= SHA3_SHAKE256_RATE) {
				ecp_sha3_shake_xof(&ctx, buf, sizeof(buf));
				pos = 0;
			}
			j = buf[pos++];
		} while (j > i);

		r->coeffs[i] = r->coeffs[j];
		r->coeffs[j] = 1 - ((signs & 1) * 2);
		signs >>= 1;
	}
}

/* @func: _pack_pk (static)
 * #desc:
 *    public key package encoding.
 *
 * #1: rho [in]  input rho bytes
 * #2: t1  [in]  input t1 polynomial vector-k
 * #3: pk  [out] public key package
 */
static void _pack_pk(const uint8_t *rho, const struct polyveck *t1,
		uint8_t *pk)
{
	ecp_memcpy(pk, rho, MLDSA_SYM_LEN);
	pk += MLDSA_SYM_LEN;

	for (int32_t i = 0; i < MLDSA_87_K; i++) {
		_polyt1_tobytes(&t1->vec[i], pk);
		pk += MLDSA_POLYT1_BYTES;
	}
}

/* @func: _unpack_pk (static)
 * #desc:
 *    public key package decoding.
 *
 * #1: rho [out] output rho bytes
 * #2: t1  [out] output t1 polynomial vector-k
 * #3: pk  [in]  public key package
 */
static void _unpack_pk(uint8_t *rho, struct polyveck *t1,
		const uint8_t *pk)
{
	ecp_memcpy(rho, pk, MLDSA_SYM_LEN);
	pk += MLDSA_SYM_LEN;

	for (int32_t i = 0; i < MLDSA_87_K; i++) {
		_polyt1_frombytes(&t1->vec[i], pk);
		pk += MLDSA_POLYT1_BYTES;
	}
}

/* @func: _pack_sk (static)
 * #desc:
 *    private key package encoding.
 *
 * #1: rho [in]  input rho bytes
 * #2: key [in]  input key bytes
 * #3: tr  [in]  input tr bytes
 * #4: s1  [in]  input s1 polynomial vector-l
 * #5: s2  [in]  input s2 polynomial vector-k
 * #6: t0  [in]  input t0 polynomial vector-k
 * #7: sk  [out] private key package
 */
static void _pack_sk(const uint8_t *rho, const uint8_t *key,
		const uint8_t *tr, const struct polyvecl *s1,
		const struct polyveck *s2, const struct polyveck *t0,
		uint8_t *sk)
{
	ecp_memcpy(sk, rho, MLDSA_SYM_LEN);
	sk += MLDSA_SYM_LEN;

	ecp_memcpy(sk, key, MLDSA_SYM_LEN);
	sk += MLDSA_SYM_LEN;

	ecp_memcpy(sk, tr, MLDSA_SYM_LEN * 2);
	sk += MLDSA_SYM_LEN * 2;

	for (int32_t i = 0; i < MLDSA_87_L; i++) {
		_polyeta_tobytes(&s1->vec[i], sk);
		sk += MLDSA_87_POLYETA_BYTES;
	}

	for (int32_t i = 0; i < MLDSA_87_K; i++) {
		_polyeta_tobytes(&s2->vec[i], sk);
		sk += MLDSA_87_POLYETA_BYTES;
	}

	for (int32_t i = 0; i < MLDSA_87_K; i++) {
		_polyt0_tobytes(&t0->vec[i], sk);
		sk += MLDSA_POLYT0_BYTES;
	}
}

/* @func: _unpack_sk (static)
 * #desc:
 *    private key package decoding.
 *
 * #1: rho [out] output rho bytes
 * #2: key [out] output key bytes
 * #3: tr  [out] output tr bytes
 * #4: s1  [out] output s1 polynomial vector-l
 * #5: s2  [out] output s2 polynomial vector-k
 * #6: t0  [out] output t0 polynomial vector-k
 * #7: sk  [in]  private key package
 */
static void _unpack_sk(uint8_t *rho, uint8_t *key, uint8_t *tr,
		struct polyvecl *s1, struct polyveck *s2, struct polyveck *t0,
		const uint8_t *sk)
{
	ecp_memcpy(rho, sk, MLDSA_SYM_LEN);
	sk += MLDSA_SYM_LEN;

	ecp_memcpy(key, sk, MLDSA_SYM_LEN);
	sk += MLDSA_SYM_LEN;

	ecp_memcpy(tr, sk, MLDSA_SYM_LEN * 2);
	sk += MLDSA_SYM_LEN * 2;

	for (int32_t i = 0; i < MLDSA_87_L; i++) {
		_polyeta_frombytes(&s1->vec[i], sk);
		sk += MLDSA_87_POLYETA_BYTES;
	}

	for (int32_t i = 0; i < MLDSA_87_K; i++) {
		_polyeta_frombytes(&s2->vec[i], sk);
		sk += MLDSA_87_POLYETA_BYTES;
	}

	for (int32_t i = 0; i < MLDSA_87_K; i++) {
		_polyt0_frombytes(&t0->vec[i], sk);
		sk += MLDSA_POLYT0_BYTES;
	}
}

/* @func: _pack_sig (static)
 * #desc:
 *    signature package encoding.
 *
 * #1: c   [in]  input c bytes
 * #2: z   [in]  input z polynomial vector-l
 * #3: h   [in]  input h polynomial vector-k
 * #4: sig [out] signature package
 */
static void _pack_sig(const uint8_t *c, const struct polyvecl *z,
		const struct polyveck *h, uint8_t *sig)
{
	ecp_memcpy(sig, c, MLDSA_87_C_BYTES);
	sig += MLDSA_87_C_BYTES;

	for (int32_t i = 0; i < MLDSA_87_L; i++) {
		_polyz_tobytes(&z->vec[i], sig);
		sig += MLDSA_87_POLYZ_BYTES;
	}

	for (int32_t i = 0; i < (MLDSA_87_OMEGA + MLDSA_87_K); i++)
		sig[i] = 0;

	int32_t k = 0;
	for (int32_t i = 0; i < MLDSA_87_K; i++) {
		for (int32_t j = 0; j < MLDSA_N; j++) {
			if (h->vec[i].coeffs[j] != 0) {
				sig[k++] = (uint8_t)j;
			}
		}
		sig[MLDSA_87_OMEGA + i] = (uint8_t)k;
	}
}

/* @func: _unpack_sig (static)
 * #desc:
 *    signature package decoding.
 *
 * #1: c   [out] output c bytes
 * #2: z   [out] output z polynomial vector-l
 * #3: h   [out] output h polynomial vector-k
 * #4: sig [in]  signature package
 * #r:     [ret] 0: no error, -1: error
 */
static int32_t _unpack_sig(uint8_t *c, struct polyvecl *z,
		struct polyveck *h, const uint8_t *sig)
{
	ecp_memcpy(c, sig, MLDSA_87_C_BYTES);
	sig += MLDSA_87_C_BYTES;

	for (int32_t i = 0; i < MLDSA_87_L; i++) {
		_polyz_frombytes(&z->vec[i], sig);
		sig += MLDSA_87_POLYZ_BYTES;
	}

	int32_t k = 0;
	for (int32_t i = 0; i < MLDSA_87_K; i++) {
		for (int32_t j = 0; j < MLDSA_N; j++)
			h->vec[i].coeffs[j] = 0;

		if (sig[MLDSA_87_OMEGA + i] < k
				|| sig[MLDSA_87_OMEGA + i] > MLDSA_87_OMEGA)
			return -1;

		for (int32_t j = k; j < sig[MLDSA_87_OMEGA + i]; j++) {
			if (j > k && sig[j] <= sig[j - 1])
				return -1;
			h->vec[i].coeffs[sig[j]] = 1;
		}
		k = sig[MLDSA_87_OMEGA + i];
	}

	for (int32_t i = k; i < MLDSA_87_OMEGA; i++) {
		if (sig[i])
			return -1;
	}

	return 0;
}

/* @func: ecp_mldsa87_genkey
 * #desc:
 *    mldsa generate public and private key function.
 *
 * #1: ran [in]  nonce
 * #2: pk  [out] public key
 * #3: sk  [out] private key
 */
void ecp_mldsa87_genkey(const uint8_t *ran, uint8_t *pk, uint8_t *sk)
{
	uint8_t buf[MLDSA_SYM_LEN * 4], tr[MLDSA_SYM_LEN * 2];
	const uint8_t *rho = buf;
	const uint8_t *rhop = buf + MLDSA_SYM_LEN;
	const uint8_t *key = buf + MLDSA_SYM_LEN * 3;
	struct polyvecl A[MLDSA_87_K], s1, s1t;
	struct polyveck s2, t1, t0;
	uint16_t nonce = 0;
	SHA3_NEW(ctx);

	/* (rho, rhop, key) = shake256(ran + k + l, 128) */
	ecp_memcpy(buf, ran, MLDSA_RAN_LEN);
	buf[MLDSA_RAN_LEN] = MLDSA_87_K;
	buf[MLDSA_RAN_LEN + 1] = MLDSA_87_L;
	ecp_sha3_init(&ctx, SHA3_SHAKE256_TYPE, 128);
	ecp_sha3_process(&ctx, buf, MLDSA_RAN_LEN + 2);
	ecp_sha3_finish(&ctx);
	ecp_memcpy(buf, &(SHA3_STATE(&ctx, 0)), 128);

	/* A = gen_matrix(rho, j, i) */
	for (int32_t i = 0; i < MLDSA_87_K; i++) {
		for (int32_t j = 0; j < MLDSA_87_L; j++)
			_sample_poly_uniform(&A[i].vec[j], rho, j, i);
	}

	/* s1 = eta1(noise, nonce++) */
	for (int32_t i = 0; i < MLDSA_87_L; i++)
		_sample_poly_eta(&s1.vec[i], rhop, nonce++);

	/* s2 = eta1(noise, nonce++) */
	for (int32_t i = 0; i < MLDSA_87_K; i++)
		_sample_poly_eta(&s2.vec[i], rhop, nonce++);

	/* s1t = ntt(s1) */
	ecp_memcpy(&s1t, &s1, sizeof(s1));
	_polyvecl_ntt(&s1t);

	/* t1 = A * s1t */
	for (int32_t i = 0; i < MLDSA_87_K; i++)
		_polyvecl_pointwise_acc(&t1.vec[i], &A[i], &s1t);

	/* t1 = invntt(redc(t1)) + s2 */
	_polyveck_reduce(&t1);
	_polyveck_invntt(&t1);
	_polyveck_add(&t1, &t1, &s2);

	/* (t1, t0) = power2round(t1) */
	_polyveck_power2round(&t1, &t0, &t1);

	/* pk = (rho, t1) */
	_pack_pk(rho, &t1, pk);

	/* tr = shake256(pk, 64) */
	ecp_sha3_init(&ctx, SHA3_SHAKE256_TYPE, 64);
	ecp_sha3_process(&ctx, pk, MLDSA_87_PK_LEN);
	ecp_sha3_finish(&ctx);
	ecp_memcpy(tr, &(SHA3_STATE(&ctx, 0)), 64);

	/* sk = (rho, key, tr, s1, s2, t0) */
	_pack_sk(rho, key, tr, &s1, &s2, &t0, sk);
}

/* @func: ecp_mldsa87_sign
 * #desc:
 *    mldsa signature function.
 *
 * #1: sk       [in]  private key
 * #2: ran      [in]  nonce
 * #3: mctx     [in]  message context
 * #4: mctx_len [in]  context length
 * #5: msg      [in]  input message
 * #6: len      [in]  message length
 * #7: sig      [out] signature
 * #r:          [ret] 0: success, -1: fail
 */
int32_t ecp_mldsa87_sign(const uint8_t *sk, const uint8_t *ran,
		const uint8_t *mctx, size_t mctx_len,
		const uint8_t *msg, size_t len, uint8_t *sig)
{
	uint8_t rho[MLDSA_SYM_LEN], key[MLDSA_SYM_LEN],
		mu[MLDSA_SYM_LEN * 2], *tr = mu,
		rhopp[MLDSA_SYM_LEN * 2];
	struct polyvecl A[MLDSA_87_K], s1, y, z;
	struct polyveck s2, t0, w1, w0, h;
	struct poly cp;
	uint16_t nonce = 0;
	SHA3_NEW(ctx);

	/* (rho, key, tr, s1, s2, t0) = sk */
	_unpack_sk(rho, key, tr, &s1, &s2, &t0, sk);

	/* mu = shake256(tr + mctx_len + mctx + msg, 64) */
	ecp_sha3_init(&ctx, SHA3_SHAKE256_TYPE, 64);
	ecp_sha3_process(&ctx, tr, MLDSA_SYM_LEN * 2);
	mu[0] = 0;
	mu[1] = (uint8_t)mctx_len;
	ecp_sha3_process(&ctx, mu, 2);
	ecp_sha3_process(&ctx, mctx, mctx_len);
	ecp_sha3_process(&ctx, msg, len);
	ecp_sha3_finish(&ctx);
	ecp_memcpy(mu, &(SHA3_STATE(&ctx, 0)), 64);

	/* rhopp = shake256(key + ran + mu, 64) */
	ecp_sha3_init(&ctx, SHA3_SHAKE256_TYPE, 64);
	ecp_sha3_process(&ctx, key, MLDSA_SYM_LEN);
	ecp_sha3_process(&ctx, ran, MLDSA_RAN_LEN);
	ecp_sha3_process(&ctx, mu, MLDSA_SYM_LEN * 2);
	ecp_sha3_finish(&ctx);
	ecp_memcpy(rhopp, &(SHA3_STATE(&ctx, 0)), 64);

	/* A = gen_matrix(rho, j, i) */
	for (int32_t i = 0; i < MLDSA_87_K; i++) {
		for (int32_t j = 0; j < MLDSA_87_L; j++)
			_sample_poly_uniform(&A[i].vec[j], rho, j, i);
	}

	/* s1 = ntt(s1) */
	_polyvecl_ntt(&s1);
	/* s2 = ntt(s2) */
	_polyveck_ntt(&s2);
	/* t0 = ntt(t0) */
	_polyveck_ntt(&t0);

rej:
	if (nonce > 65532)
		return -1;

	/* y = gamma1(rhopp, nonce++) */
	for (int32_t i = 0; i < MLDSA_87_L; i++)
		_sample_poly_gamma1(&y.vec[i], rhopp, nonce++);

	/* yt = ntt(y) */
	ecp_memcpy(&z, &y, sizeof(y));
	_polyvecl_ntt(&z);

	/* w = A * yt */
	for (int32_t i = 0; i < MLDSA_87_K; i++)
		_polyvecl_pointwise_acc(&w1.vec[i], &A[i], &z);

	/* w = invntt(redc(w)) */
	_polyveck_reduce(&w1);
	_polyveck_invntt(&w1);

	/* (w1, w0) = decomposes(w) */
	_polyveck_decompose(&w1, &w0, &w1);

	/* w1p = w1pack(w1) */
	_polyveck_w1pack(&w1, sig);

	/* c = shake256(mu + w1p, 64) */
	ecp_sha3_init(&ctx, SHA3_SHAKE256_TYPE, 64);
	ecp_sha3_process(&ctx, mu, MLDSA_SYM_LEN * 2);
	ecp_sha3_process(&ctx, sig, MLDSA_87_POLYVEC_W1PACK_BYTES);
	ecp_sha3_finish(&ctx);
	ecp_memcpy(sig, &(SHA3_STATE(&ctx, 0)), 64);

	/* cp = ntt(challenge(c)) */
	_sample_poly_challenge(&cp, sig);
	_poly_ntt(&cp);

	/* z = s1 * cp */
	for (int32_t i = 0; i < MLDSA_87_L; i++)
		_poly_pointwise(&z.vec[i], &s1.vec[i], &cp);

	/* z = redc(invntt(z) + y) */
	_polyvecl_invntt(&z);
	_polyvecl_add(&z, &z, &y);
	_polyvecl_reduce(&z);

	/* if z >= N(gamma1 - beta) : goto rej */
	if (_polyvecl_check_norm(&z, MLDSA_87_GAMMA1 - MLDSA_87_BETA))
		goto rej;

	/* cs2 = s2 * cp */
	for (int32_t i = 0; i < MLDSA_87_K; i++)
		_poly_pointwise(&h.vec[i], &s2.vec[i], &cp);

	/* w0 = redc(w0 - invntt(cs2)) */
	_polyveck_invntt(&h);
	_polyveck_sub(&w0, &w0, &h);
	_polyveck_reduce(&w0);

	/* if w0 >= N(gamma2 - beta) : goto rej */
	if (_polyveck_check_norm(&w0, MLDSA_87_GAMMA2 - MLDSA_87_BETA))
		goto rej;

	/* ct0 = t0 * cp */
	for (int32_t i = 0; i < MLDSA_87_K; i++)
		_poly_pointwise(&h.vec[i], &t0.vec[i], &cp);

	/* ct0 = redc(invntt(ct0)) */
	_polyveck_invntt(&h);
	_polyveck_reduce(&h);

	/* if ct0 >= N(gamma2) : goto rej */
	if (_polyveck_check_norm(&h, MLDSA_87_GAMMA2))
		goto rej;

	/* w0 = w0 + ct0 */
	_polyveck_add(&w0, &w0, &h);

	/* h = make_hint(w0, w1) */
	int32_t n = _polyveck_make_hint(&h, &w0, &w1);

	/* if n > omega : goto rej */
	if (n > MLDSA_87_OMEGA)
		goto rej;

	/* sig = c + z + h */
	_pack_sig(sig, &z, &h, sig);

	return 0;
}

/* @func: ecp_mldsa87_verify
 * #desc:
 *    mldsa signature verification function.
 *
 * #1: pk       [in]  public key
 * #2: sig      [in]  signature
 * #3: mctx     [in]  message context
 * #4: mctx_len [in]  context length
 * #5: msg      [in]  input message
 * #6: len      [in]  message length
 * #r:          [ret] 0: success, -1: fail
 */
int32_t ecp_mldsa87_verify(const uint8_t *pk, const uint8_t *sig,
		const uint8_t *mctx, size_t mctx_len,
		const uint8_t *msg, size_t len)
{
	uint8_t rho[MLDSA_SYM_LEN], mu[MLDSA_SYM_LEN * 2], *tr = mu,
		w1p[MLDSA_87_POLYVEC_W1PACK_BYTES];
	struct polyvecl A[MLDSA_87_K], z;
	struct polyveck t1, w1, h;
	struct poly cp;
	uint8_t c[MLDSA_87_C_BYTES], c2[MLDSA_87_C_BYTES];
	SHA3_NEW(ctx);

	/* (c, z, h) = sig */
	if (_unpack_sig(c, &z, &h, sig))
		return -1;
	/* if z >= N(gamma1 - beta) : return -1 */
	if (_polyvecl_check_norm(&z, MLDSA_87_GAMMA1 - MLDSA_87_BETA))
		return -1;

	/* (rho, t1) = pk */
	_unpack_pk(rho, &t1, pk);

	/* tr = shake256(pk, 64) */
	ecp_sha3_init(&ctx, SHA3_SHAKE256_TYPE, 64);
	ecp_sha3_process(&ctx, pk, MLDSA_87_PK_LEN);
	ecp_sha3_finish(&ctx);
	ecp_memcpy(tr, &(SHA3_STATE(&ctx, 0)), 64);

	/* mu = shake256(tr + mctx_len + mctx + msg, 64) */
	ecp_sha3_init(&ctx, SHA3_SHAKE256_TYPE, 64);
	ecp_sha3_process(&ctx, tr, MLDSA_SYM_LEN * 2);
	mu[0] = 0;
	mu[1] = (uint8_t)mctx_len;
	ecp_sha3_process(&ctx, mu, 2);
	ecp_sha3_process(&ctx, mctx, mctx_len);
	ecp_sha3_process(&ctx, msg, len);
	ecp_sha3_finish(&ctx);
	ecp_memcpy(mu, &(SHA3_STATE(&ctx, 0)), 64);

	/* cp = ntt(challenge(c)) */
	_sample_poly_challenge(&cp, c);
	_poly_ntt(&cp);

	/* A = gen_matrix(rho, j, i) */
	for (int32_t i = 0; i < MLDSA_87_K; i++) {
		for (int32_t j = 0; j < MLDSA_87_L; j++)
			_sample_poly_uniform(&A[i].vec[j], rho, j, i);
	}

	/* z = ntt(z) */
	_polyvecl_ntt(&z);

	/* w1 = A * z */
	for (int32_t i = 0; i < MLDSA_87_K; i++)
		_polyvecl_pointwise_acc(&w1.vec[i], &A[i], &z);

	/* t1 = ntt(shiftl(t1)) */
	_polyveck_shiftl(&t1);
	_polyveck_ntt(&t1);

	/* t1 = t1 * cp */
	for (int32_t i = 0; i < MLDSA_87_K; i++)
		_poly_pointwise(&t1.vec[i], &t1.vec[i], &cp);

	/* w1 = invntt(redc(w1 - t1)) */
	_polyveck_sub(&w1, &w1, &t1);
	_polyveck_reduce(&w1);
	_polyveck_invntt(&w1);

	/* w1 = use_hint(w1, h) */
	_polyveck_use_hint(&w1, &w1, &h);

	/* w1p = w1pack(w1) */
	_polyveck_w1pack(&w1, w1p);

	/* c2 = shake256(mu + w1p, 64) */
	ecp_sha3_init(&ctx, SHA3_SHAKE256_TYPE, 64);
	ecp_sha3_process(&ctx, mu, MLDSA_SYM_LEN * 2);
	ecp_sha3_process(&ctx, w1p, MLDSA_87_POLYVEC_W1PACK_BYTES);
	ecp_sha3_finish(&ctx);
	ecp_memcpy(c2, &(SHA3_STATE(&ctx, 0)), 64);

	/* if c != c2 : return -1 */
	uint32_t r = 0;
	for (int32_t i = 0; i < MLDSA_87_C_BYTES; i++)
		r |= c[i] ^ c2[i];

	return (int32_t)(((r - 1) >> 31) & 1) - 1;
}

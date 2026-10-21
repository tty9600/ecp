/* @file: bn8448.c
 * #desc:
 *    The implementations of big number calculation (8448-bit).
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
#include <ecp/bn.h>

/* @func: ecp_bn8448_move
 * #desc:
 *    big number move.
 *
 * #1: t [out] target
 * #2: s [in]  source
 */
void ecp_bn8448_move(bn8448_t *t, const bn8448_t *s)
{
	ecp_memcpy(t, s, sizeof(bn8448_t));
}

/* @func: ecp_bn8448_swap
 * #desc:
 *    big number swap.
 *
 * #1: a [in/out] target
 * #2: b [in/out] source
 */
void ecp_bn8448_swap(bn8448_t *a, bn8448_t *b)
{
	bn8448_t t;
	ecp_memcpy(&t, a, sizeof(bn8448_t));
	ecp_memcpy(a, b, sizeof(bn8448_t));
	ecp_memcpy(b, &t, sizeof(bn8448_t));
}

/* @func: ecp_bn8448_set1
 * #desc:
 *    big number value set (single a word).
 *
 * #1: t [out] target
 * #2: n [in]  number
 */
void ecp_bn8448_set1(bn8448_t *t, uint32_t n)
{
	ecp_memset(t, 0, sizeof(bn8448_t));
	t->n[0] = n;
}

/* @func: ecp_bn8448_from_bytes
 * #desc:
 *    copy from bytes to big number.
 *
 * #1: t   [out] target
 * #2: s   [in]  source bytes
 * #3: len [in]  bytes length
 */
void ecp_bn8448_from_bytes(bn8448_t *t, const uint8_t *s, uint32_t len)
{
	ecp_bn8448_set1(t, 0);
	ecp_memcpy(t->n, s, len);
}

/* @func: ecp_bn8448_to_bytes
 * #desc:
 *    copy from big number to bytes.
 *
 * #1: s   [in]  source
 * #2: t   [out] target bytes
 * #3: len [in]  bytes length
 */
void ecp_bn8448_to_bytes(const bn8448_t *s, uint8_t *t, uint32_t len)
{
	ecp_memcpy(t, s->n, len);
}

/* @func: ecp_bn8448_bits
 * #desc:
 *    get the bits length of big number.
 *
 * #1: t [in]  target
 * #r:   [ret] bits number
 */
int32_t ecp_bn8448_bits(const bn8448_t *t)
{
	const uint32_t *_n = t->n;

	static const uint32_t pow2[32] = {
		0x00000001, 0x00000002, 0x00000004,
		0x00000008, 0x00000010, 0x00000020,
		0x00000040, 0x00000080, 0x00000100,
		0x00000200, 0x00000400, 0x00000800,
		0x00001000, 0x00002000, 0x00004000,
		0x00008000, 0x00010000, 0x00020000,
		0x00040000, 0x00080000, 0x00100000,
		0x00200000, 0x00400000, 0x00800000,
		0x01000000, 0x02000000, 0x04000000,
		0x08000000, 0x10000000, 0x20000000,
		0x40000000, 0x80000000
		};

	for (int32_t i = BN8448_N - 1; i >= 0; i--) {
		if (_n[i]) {
			for (int32_t k = 31; k >= 0; k--) {
				if (_n[i] & pow2[k])
					return (i * 32) + k + 1;
			}
		}
	}

	return 0;
}

/* @func: ecp_bn8448_lshift
 * #desc:
 *    big number left shift.
 *
 * #1: t [out] target
 * #2: n [in]  bits number
 */
void ecp_bn8448_lshift(bn8448_t *t, uint32_t n)
{
	bn8448_t r;
	uint32_t *_t = t->n;
	uint32_t *_r = r.n;
	int32_t m, w, x;

	if (!n)
		return;

	ecp_bn8448_set1(&r, 0);
	r.sign = t->sign;

	m = n / 32;
	w = n % 32;
	if (w) {
		x = 32 - w;
		uint32_t carry = 0;
		for (int32_t i = m; i < BN8448_N; i++) {
			uint32_t tmp = _t[i - m] >> x;
			_r[i] = (_t[i - m] << w) | carry;
			carry = tmp;
		}
	} else {
		for (int32_t i = m; i < BN8448_N; i++)
			_r[i] = _t[i - m];
	}

	ecp_bn8448_move(t, &r);
}

/* @func: ecp_bn8448_rshift
 * #desc:
 *    big number right shift.
 *
 * #1: t [in/out] target
 * #2: n [in]     bits number
 */
void ecp_bn8448_rshift(bn8448_t *t, uint32_t n)
{
	bn8448_t r;
	uint32_t *_t = t->n;
	uint32_t *_r = r.n;
	int32_t m, w, x;

	if (!n)
		return;

	ecp_bn8448_set1(&r, 0);
	r.sign = t->sign;

	m = n / 32;
	w = n % 32;
	if (w) {
		x = 32 - w;
		uint32_t u = (1U << w) - 1;
		uint32_t carry = 0;
		for (int32_t i = BN8448_N - 1; i >= m; i--) {
			uint32_t tmp = _t[i] & u;
			_r[i - m] = (_t[i] >> w) | (carry << x);
			carry = tmp;
		}
	} else {
		for (int32_t i = m; i < BN8448_N; i++)
			_r[i - m] = _t[i];
	}

	ecp_bn8448_move(t, &r);
}

/* @func: ecp_bn8448_and
 * #desc:
 *    big number and operation.
 *
 * #1: r [out] result
 * #2: a [in]  number
 * #3: b [in]  number
 */
void ecp_bn8448_and(bn8448_t *r,
		const bn8448_t *a, const bn8448_t *b)
{
	const uint32_t *_a = a->n;
	const uint32_t *_b = b->n;
	uint32_t *_r = r->n;

	for (int32_t i = 0; i < BN8448_N; i++)
		_r[i] = _a[i] & _b[i];

	r->sign = 0;
}

/* @func: ecp_bn8448_or
 * #desc:
 *    big number or operation.
 *
 * #1: r [out] result
 * #2: a [in]  number
 * #3: b [in]  number
 */
void ecp_bn8448_or(bn8448_t *r,
		const bn8448_t *a, const bn8448_t *b)
{
	const uint32_t *_a = a->n;
	const uint32_t *_b = b->n;
	uint32_t *_r = r->n;

	for (int32_t i = 0; i < BN8448_N; i++)
		_r[i] = _a[i] | _b[i];

	r->sign = 0;
}

/* @func: ecp_bn8448_xor
 * #desc:
 *    big number xor operation.
 *
 * #1: r [out] result
 * #2: a [in]  number
 * #3: b [in]  number
 */
void ecp_bn8448_xor(bn8448_t *r,
		const bn8448_t *a, const bn8448_t *b)
{
	const uint32_t *_a = a->n;
	const uint32_t *_b = b->n;
	uint32_t *_r = r->n;

	for (int32_t i = 0; i < BN8448_N; i++)
		_r[i] = _a[i] ^ _b[i];

	r->sign = 0;
}

/* @func: ecp_bn8448_not
 * #desc:
 *    big number not operation.
 *
 * #1: r [out] result
 * #2: a [in]  number
 */
void ecp_bn8448_not(bn8448_t *r,
		const bn8448_t *a)
{
	const uint32_t *_a = a->n;
	uint32_t *_r = r->n;

	for (int32_t i = 0; i < BN8448_N; i++)
		_r[i] = ~_a[i];

	r->sign = 0;
}

/* @func: ecp_bn8448_ucmp
 * #desc:
 *    big number compare (unsigned).
 *
 * #1: a [in]  number
 * #2: b [in]  number
 * #r:   [ret] 0: a==b, 1: a>b, -1: a<b
 */
int32_t ecp_bn8448_ucmp(const bn8448_t *a, const bn8448_t *b)
{
	const uint32_t *_a = a->n;
	const uint32_t *_b = b->n;

	for (int32_t i = BN8448_N - 1; i >= 0; i--) {
		if (_a[i] > _b[i])
			return 1;
		if (_a[i] < _b[i])
			return -1;
	}

	return 0;
}

/* @func: ecp_bn8448_ucmp_1
 * #desc:
 *    big number compare (unsigned, single a word).
 *
 * #1: a [in]  number
 * #2: b [in]  number
 * #r:   [ret] 0: a==b, 1: a>b, -1: a<b
 */
int32_t ecp_bn8448_ucmp_1(const bn8448_t *a, uint32_t b)
{
	const uint32_t *_a = a->n;

	for (int32_t i = BN8448_N - 1; i >= 0; i--) {
		if (i) {
			if (_a[i])
				return 1;
		} else {
			if (_a[i] > b)
				return 1;
			if (_a[i] < b)
				return -1;
		}
	}

	return 0;
}

/* @func: ecp_bn8448_cmp
 * #desc:
 *    big number compare (signed).
 *
 * #1: a [in]  number
 * #2: b [in]  number
 * #r:   [ret] 0: a==b, 1: a>b, -1: a<b
 */
int32_t ecp_bn8448_cmp(const bn8448_t *a, const bn8448_t *b)
{
	const uint32_t *_a = a->n;
	const uint32_t *_b = b->n;

	if (a->sign < b->sign)
		return 1;
	if (a->sign > b->sign)
		return -1;

	for (int32_t i = BN8448_N - 1; i >= 0; i--) {
		if (_a[i] < _b[i])
			return a->sign ? 1 : -1;
		if (_a[i] > _b[i])
			return a->sign ? -1 : 1;
	}

	return 0;
}

/* @func: ecp_bn8448_cmp_1
 * #desc:
 *    big number compare (signed, single a word).
 *
 * #1: a [in]  number
 * #2: b [in]  number
 * #r:   [ret] 0: a==b, 1: a>b, -1: a<b
 */
int32_t ecp_bn8448_cmp_1(const bn8448_t *a, uint32_t b)
{
	const uint32_t *_a = a->n;

	if (a->sign)
		return -1;

	for (int32_t i = BN8448_N - 1; i >= 0; i--) {
		if (i) {
			if (_a[i])
				return 1;
		} else {
			if (_a[i] > b)
				return 1;
			if (_a[i] < b)
				return -1;
		}
	}

	return 0;
}

/* @func: ecp_bn8448_uadd
 * #desc:
 *    big number addition (unsigned).
 *
 * #1: r [out] sum
 * #2: a [in]  addend
 * #3: b [in]  addend
 * #r:   [ret] overflow
 */
uint32_t ecp_bn8448_uadd(bn8448_t *r,
		const bn8448_t *a, const bn8448_t *b)
{
	const uint32_t *_a = a->n;
	const uint32_t *_b = b->n;
	uint32_t *_r = r->n;

	uint32_t carry = 0;
	for (int32_t i = 0; i < BN8448_N; i++) {
		uint64_t tmp = (uint64_t)_a[i] + _b[i] + carry;
		_r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}

	r->sign = 0;

	return carry;
}

/* @func: ecp_bn8448_uadd_1
 * #desc:
 *    big number addition (unsigned, single a word).
 *
 * #1: r [out] sum
 * #2: a [in]  addend
 * #3: b [in]  addend
 * #r:   [ret] overflow
 */
uint32_t ecp_bn8448_uadd_1(bn8448_t *r,
		const bn8448_t *a, uint32_t b)
{
	const uint32_t *_a = a->n;
	uint32_t *_r = r->n;

	uint32_t carry = b;
	for (int32_t i = 0; i < BN8448_N; i++) {
		uint64_t tmp = (uint64_t)_a[i] + carry;
		_r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}

	r->sign = 0;

	return carry;
}

/* @func: ecp_bn8448_usub
 * #desc:
 *    big number subtraction (unsigned).
 *
 * #1: r [out] difference
 * #2: a [in]  minuend
 * #3: b [in]  subtract
 * #r:   [ret] overflow
 */
uint32_t ecp_bn8448_usub(bn8448_t *r,
		const bn8448_t *a, const bn8448_t *b)
{
	const uint32_t *_a = a->n;
	const uint32_t *_b = b->n;
	uint32_t *_r = r->n;

	uint32_t borrow = 0;
	for (int32_t i = 0; i < BN8448_N; i++) {
		uint64_t tmp = (uint64_t)_a[i] - _b[i] - borrow;
		_r[i] = tmp & 0xffffffff;
		borrow = (tmp >> 32) ? 1 : 0;
	}

	r->sign = 0;

	return borrow;
}

/* @func: ecp_bn8448_usub_1
 * #desc:
 *    big number subtraction (unsigned, single a word).
 *
 * #1: r [out] difference
 * #2: a [in]  minuend
 * #3: b [in]  subtract
 * #r:   [ret] overflow
 */
uint32_t ecp_bn8448_usub_1(bn8448_t *r,
		const bn8448_t *a, uint32_t b)
{
	const uint32_t *_a = a->n;
	uint32_t *_r = r->n;

	uint32_t borrow = b;
	for (int32_t i = 0; i < BN8448_N; i++) {
		uint64_t tmp = (uint64_t)_a[i] - borrow;
		_r[i] = tmp & 0xffffffff;
		borrow = (tmp >> 32) ? 1 : 0;
	}

	r->sign = 0;

	return borrow;
}

/* @func: _umul_karatsuba_2 (static)
 * #desc:
 *    big number karatsuba multiplication.
 *
 * #1: r [out] product
 * #2: a [in]  multiplicand
 * #3: b [in]  multiplier
 */
static void _umul_karatsuba_2(uint32_t *r,
		const uint32_t *a, const uint32_t *b)
{
#undef N0
#undef N1
#undef N2
#undef N3
#undef N4
#define N0 (BN8448_N / 2)
#define N1 (N0 / 2) /* 66 */
#define N2 (N1 / 2) /* 33 */
#define N3 (N2 + 1) /* 34 */
#define N4 (N3 * 2) /* 68 */

	uint32_t z0[N4], z1[N4], z2[N4], a1[N3], b1[N3];
	uint32_t carry, borrow;
	uint64_t tmp;

	ecp_memset(z0, 0, sizeof(z0));
	ecp_memset(z1, 0, sizeof(z1));
	ecp_memset(z2, 0, sizeof(z2));
	ecp_memset(a1, 0, sizeof(a1));
	ecp_memset(b1, 0, sizeof(b1));

	/* z0 = a(low) * b(low) */
	for (int32_t i = 0; i < N2; i++) {
		carry = 0;
		for (int32_t j = 0; j < N2; j++) {
			tmp = (uint64_t)a[i] * b[j] + z0[i + j] + carry;
			z0[i + j] = tmp & 0xffffffff;
			carry = tmp >> 32;
		}
		z0[i + N2] = carry;
	}

	/* a1 = a(low) + a(hig) */
	carry = 0;
	for (int32_t i = 0; i < N2; i++) {
		tmp = (uint64_t)a[i] + a[i + N2] + carry;
		a1[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}
	a1[N2] = carry;

	/* b1 = b(low) + b(hig) */
	carry = 0;
	for (int32_t i = 0; i < N2; i++) {
		tmp = (uint64_t)b[i] + b[i + N2] + carry;
		b1[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}
	b1[N2] = carry;

	/* z1 = a1 * b1 */
	for (int32_t i = 0; i < N3; i++) {
		carry = 0;
		for (int32_t j = 0; j < N3; j++) {
			tmp = (uint64_t)a1[i] * b1[j] + z1[i + j] + carry;
			z1[i + j] = tmp & 0xffffffff;
			carry = tmp >> 32;
		}
		z1[i + N3] = carry;
	}

	/* z2 = a(hig) * b(hig) */
	a += N2;
	b += N2;
	for (int32_t i = 0; i < N2; i++) {
		carry = 0;
		for (int32_t j = 0; j < N2; j++) {
			tmp = (uint64_t)a[i] * b[j] + z2[i + j] + carry;
			z2[i + j] = tmp & 0xffffffff;
			carry = tmp >> 32;
		}
		z2[i + N2] = carry;
	}

	/* z1 = z1 - z2 - z0 */
	borrow = 0;
	for (int32_t i = 0; i < N4; i++) {
		tmp = (uint64_t)z1[i] - z2[i] - borrow;
		z1[i] = tmp & 0xffffffff;
		borrow = (tmp >> 32) ? 1 : 0;
	}

	borrow = 0;
	for (int32_t i = 0; i < N4; i++) {
		tmp = (uint64_t)z1[i] - z0[i] - borrow;
		z1[i] = tmp & 0xffffffff;
		borrow = (tmp >> 32) ? 1 : 0;
	}

	/* r = (z2 << N1) + (z1 << N2) + z0 */
	ecp_memset(r, 0, sizeof(uint32_t) * N0);
	for (int32_t i = 0; i < N1; i++)
		r[i + N1] = z2[i];

	carry = 0;
	for (int32_t i = N2; i < (N2 + N4); i++) {
		tmp = (uint64_t)r[i] + z1[i - N2] + carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}
	for (int32_t i = (N2 + N4); i < N0; i++) {
		tmp = (uint64_t)r[i] + carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}

	carry = 0;
	for (int32_t i = 0; i < N1; i++) {
		tmp = (uint64_t)r[i] + z0[i] + carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}
	for (int32_t i = N1; i < N0; i++) {
		tmp = (uint64_t)r[i] + carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}
}

/* @func: _umul_karatsuba_2_1 (static)
 * #desc:
 *    big number karatsuba multiplication.
 *
 * #1: r [out] product
 * #2: a [in]  multiplicand
 * #3: b [in]  multiplier
 */
static void _umul_karatsuba_2_1(uint32_t *r,
		const uint32_t *a, const uint32_t *b)
{
#undef N0
#undef N1
#undef N2
#undef N3
#undef N4
#define N0 (BN8448_N / 2 + 4)
#define N1 (N0 / 2) /* 68 */
#define N2 (N1 / 2) /* 34 */
#define N3 (N2 + 1) /* 35 */
#define N4 (N3 * 2) /* 70 */

	uint32_t z0[N4], z1[N4], z2[N4], a1[N3], b1[N3];
	uint32_t carry, borrow;
	uint64_t tmp;

	ecp_memset(z0, 0, sizeof(z0));
	ecp_memset(z1, 0, sizeof(z1));
	ecp_memset(z2, 0, sizeof(z2));
	ecp_memset(a1, 0, sizeof(a1));
	ecp_memset(b1, 0, sizeof(b1));

	/* z0 = a(low) * b(low) */
	for (int32_t i = 0; i < N2; i++) {
		carry = 0;
		for (int32_t j = 0; j < N2; j++) {
			tmp = (uint64_t)a[i] * b[j] + z0[i + j] + carry;
			z0[i + j] = tmp & 0xffffffff;
			carry = tmp >> 32;
		}
		z0[i + N2] = carry;
	}

	/* a1 = a(low) + a(hig) */
	carry = 0;
	for (int32_t i = 0; i < N2; i++) {
		tmp = (uint64_t)a[i] + a[i + N2] + carry;
		a1[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}
	a1[N2] = carry;

	/* b1 = b(low) + b(hig) */
	carry = 0;
	for (int32_t i = 0; i < N2; i++) {
		tmp = (uint64_t)b[i] + b[i + N2] + carry;
		b1[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}
	b1[N2] = carry;

	/* z1 = a1 * b1 */
	for (int32_t i = 0; i < N3; i++) {
		carry = 0;
		for (int32_t j = 0; j < N3; j++) {
			tmp = (uint64_t)a1[i] * b1[j] + z1[i + j] + carry;
			z1[i + j] = tmp & 0xffffffff;
			carry = tmp >> 32;
		}
		z1[i + N3] = carry;
	}

	/* z2 = a(hig) * b(hig) */
	a += N2;
	b += N2;
	for (int32_t i = 0; i < N2; i++) {
		carry = 0;
		for (int32_t j = 0; j < N2; j++) {
			tmp = (uint64_t)a[i] * b[j] + z2[i + j] + carry;
			z2[i + j] = tmp & 0xffffffff;
			carry = tmp >> 32;
		}
		z2[i + N2] = carry;
	}

	/* z1 = z1 - z2 - z0 */
	borrow = 0;
	for (int32_t i = 0; i < N4; i++) {
		tmp = (uint64_t)z1[i] - z2[i] - borrow;
		z1[i] = tmp & 0xffffffff;
		borrow = (tmp >> 32) ? 1 : 0;
	}

	borrow = 0;
	for (int32_t i = 0; i < N4; i++) {
		tmp = (uint64_t)z1[i] - z0[i] - borrow;
		z1[i] = tmp & 0xffffffff;
		borrow = (tmp >> 32) ? 1 : 0;
	}

	/* r = (z2 << N1) + (z1 << N2) + z0 */
	ecp_memset(r, 0, sizeof(uint32_t) * N0);
	for (int32_t i = 0; i < N1; i++)
		r[i + N1] = z2[i];

	carry = 0;
	for (int32_t i = N2; i < (N2 + N4); i++) {
		tmp = (uint64_t)r[i] + z1[i - N2] + carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}
	for (int32_t i = (N2 + N4); i < N0; i++) {
		tmp = (uint64_t)r[i] + carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}

	carry = 0;
	for (int32_t i = 0; i < N1; i++) {
		tmp = (uint64_t)r[i] + z0[i] + carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}
	for (int32_t i = N1; i < N0; i++) {
		tmp = (uint64_t)r[i] + carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}
}

/* @func: _umul_karatsuba (static)
 * #desc:
 *    big number karatsuba multiplication.
 *
 * #1: r [out] product
 * #2: a [in]  multiplicand
 * #3: b [in]  multiplier
 */
static void _umul_karatsuba(uint32_t *r,
		const uint32_t *a, const uint32_t *b)
{
#undef N0
#undef N1
#undef N2
#undef N3
#undef N4
#define N0 BN8448_N
#define N1 (N0 / 2) /* 132 */
#define N2 (N1 / 2) /* 66 */
#define N3 (N2 + 2) /* 68 */
#define N4 (N3 * 2) /* 136 */

	uint32_t z0[N4], z1[N4], z2[N4], a1[N3], b1[N3];
	uint32_t carry, borrow;
	uint64_t tmp;

	ecp_memset(z0, 0, sizeof(z0));
	ecp_memset(z1, 0, sizeof(z1));
	ecp_memset(z2, 0, sizeof(z2));
	ecp_memset(a1, 0, sizeof(a1));
	ecp_memset(b1, 0, sizeof(b1));

	/* z0 = a(low) * b(low) */
	_umul_karatsuba_2(z0, a, b);

	/* a1 = a(low) + a(hig) */
	carry = 0;
	for (int32_t i = 0; i < N2; i++) {
		tmp = (uint64_t)a[i] + a[i + N2] + carry;
		a1[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}
	a1[N2] = carry;

	/* b1 = b(low) + b(hig) */
	carry = 0;
	for (int32_t i = 0; i < N2; i++) {
		tmp = (uint64_t)b[i] + b[i + N2] + carry;
		b1[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}
	b1[N2] = carry;

	/* z1 = a1 * b1 */
	_umul_karatsuba_2_1(z1, a1, b1);

	/* z2 = a(hig) * b(hig) */
	a += N2;
	b += N2;
	_umul_karatsuba_2(z2, a, b);

	/* z1 = z1 - z2 - z0 */
	borrow = 0;
	for (int32_t i = 0; i < N4; i++) {
		tmp = (uint64_t)z1[i] - z2[i] - borrow;
		z1[i] = tmp & 0xffffffff;
		borrow = (tmp >> 32) ? 1 : 0;
	}

	borrow = 0;
	for (int32_t i = 0; i < N4; i++) {
		tmp = (uint64_t)z1[i] - z0[i] - borrow;
		z1[i] = tmp & 0xffffffff;
		borrow = (tmp >> 32) ? 1 : 0;
	}

	/* r = (z2 << N1) + (z1 << N2) + z0 */
	ecp_memset(r, 0, sizeof(uint32_t) * N0);
	for (int32_t i = 0; i < N1; i++)
		r[i + N1] = z2[i];

	carry = 0;
	for (int32_t i = N2; i < (N2 + N4); i++) {
		tmp = (uint64_t)r[i] + z1[i - N2] + carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}
	for (int32_t i = (N2 + N4); i < N0; i++) {
		tmp = (uint64_t)r[i] + carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}

	carry = 0;
	for (int32_t i = 0; i < N1; i++) {
		tmp = (uint64_t)r[i] + z0[i] + carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}
	for (int32_t i = N1; i < N0; i++) {
		tmp = (uint64_t)r[i] + carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}
}

/* @func: ecp_bn8448_umul
 * #desc:
 *    big number multiplication (unsigned).
 *
 * #1: r [out] product
 * #2: a [in]  multiplicand
 * #3: b [in]  multiplier
 */
void ecp_bn8448_umul(bn8448_t *r,
		const bn8448_t *a, const bn8448_t *b)
{
	bn8448_t rr;
	const uint32_t *_a = a->n;
	const uint32_t *_b = b->n;
	uint32_t *_rr = rr.n;

	ecp_bn8448_set1(&rr, 0);

	int32_t a_len = ecp_bn8448_bits(a);
	int32_t b_len = ecp_bn8448_bits(b);
	if (!a_len || !b_len)
		goto e;

#undef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

	a_len = MAX(a_len, b_len);
	if (a_len <= (BN8448_NB / 4)) {
		_umul_karatsuba_2(_rr, _a, _b);
		goto e;
	} else if (a_len <= (BN8448_NB / 2)) {
		_umul_karatsuba(_rr, _a, _b);
		goto e;
	}

	for (int32_t i = 0; i < BN8448_N; i++) {
		uint32_t carry = 0;
		for (int32_t j = 0; j < BN8448_N; j++) {
			if ((i + j) >= BN8448_N)
				break;
			uint64_t tmp = (uint64_t)_a[i] * _b[j]
				+ _rr[i + j] + carry;
			_rr[i + j] = tmp & 0xffffffff;
			carry = tmp >> 32;
		}
	}

e:
	ecp_bn8448_move(r, &rr);
}

/* @func: ecp_bn8448_umul_1
 * #desc:
 *    big number multiplication (unsigned, single a word).
 *
 * #1: r [out] product
 * #2: a [in]  multiplicand
 * #3: b [in]  multiplier
 * #r:   [ret] overflow
 */
uint32_t ecp_bn8448_umul_1(bn8448_t *r,
		const bn8448_t *a, uint32_t b)
{
	const uint32_t *_a = a->n;
	uint32_t *_r = r->n;

	uint32_t carry = 0;
	for (int32_t i = 0; i < BN8448_N; i++) {
		uint64_t tmp = (uint64_t)_a[i] * b + carry;
		_r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}

	r->sign = 0;

	return carry;
}

/* @func: _udiv_ucmp (static)
 * #desc:
 *    division of the big number compare.
 *
 * #1: a [in]  number
 * #2: b [in]  number
 * #r:   [ret] 0: a==b, 1: a>b, -1: a<b
 */
static int32_t _udiv_ucmp(const uint32_t *a, const uint32_t *b)
{
#undef N0
#undef N1
#define N0 BN8448_N
#define N1 (N0 + 1)

	for (int32_t i = N1 - 1; i >= 0; i--) {
		if (a[i] > b[i])
			return 1;
		if (a[i] < b[i])
			return -1;
	}

	return 0;
}

/* @func: _udiv_lshift (static)
 * #desc:
 *    division of the big number left shift.
 *
 * #1: t [in/out] target
 * #2: n [in]     bits number
 */
static void _udiv_lshift(uint32_t *t, uint32_t n)
{
#undef N0
#undef N1
#define N0 BN8448_N
#define N1 (N0 + 1)

	uint32_t r[N1];
	int32_t m, w, x;

	if (!n)
		return;

	ecp_memset(r, 0, sizeof(r));

	m = n / 32;
	w = n % 32;
	if (w) {
		x = 32 - w;
		uint32_t carry = 0;
		for (int32_t i = m; i < N1; i++) {
			uint32_t tmp = t[i - m] >> x;
			r[i] = (t[i - m] << w) | carry;
			carry = tmp;
		}
	} else {
		for (int32_t i = m; i < N1; i++)
			r[i] = t[i - m];
	}

	ecp_memcpy(t, r, sizeof(r));
}

/* @func: _udiv_rshift (static)
 * #desc:
 *    division of the big number right shift.
 *
 * #1: t [in/out] target
 * #2: n [in]     bits number
 */
static void _udiv_rshift(uint32_t *t, uint32_t n)
{
#undef N0
#undef N1
#define N0 BN8448_N
#define N1 (N0 + 1)

	uint32_t r[N1];
	int32_t m, w, x;

	if (!n)
		return;

	ecp_memset(r, 0, sizeof(r));

	m = n / 32;
	w = n % 32;
	if (w) {
		x = 32 - w;
		uint32_t u = (1U << w) - 1;
		uint32_t carry = 0;
		for (int32_t i = N1 - 1; i >= m; i--) {
			uint32_t tmp = t[i] & u;
			r[i - m] = (t[i] >> w) | (carry << x);
			carry = tmp;
		}
	} else {
		for (int32_t i = m; i < N1; i++)
			r[i - m] = t[i];
	}

	ecp_memcpy(t, r, sizeof(r));
}

/* @func: _udiv_usub (static)
 * #desc:
 *    division of the big number subtraction.
 *
 * #1: r [out] difference
 * #2: a [in]  minuend
 * #3: b [in]  subtract
 */
static void _udiv_usub(uint32_t *r,
		const uint32_t *a, const uint32_t *b)
{
#undef N0
#undef N1
#define N0 BN8448_N
#define N1 (N0 + 1)

	uint32_t borrow = 0;
	for (int32_t i = 0; i < N1; i++) {
		uint64_t tmp = (uint64_t)a[i] - b[i] - borrow;
		r[i] = tmp & 0xffffffff;
		borrow = (tmp >> 32) ? 1 : 0;
	}
}

/* @func: _udiv_base (static)
 * #desc:
 *    big number division.
 *
 * #1: quo   [out] quotient
 * #2: rem   [out] remainder
 * #3: a     [in]  dividend
 * #4: b     [in]  divisor
 * #5: a_len [in]  dividend length
 * #6: b_len [in]  divisor length
 */
static void _udiv_base(uint32_t *quo, uint32_t *rem,
		const uint32_t *a, const uint32_t *b,
		int32_t a_len, int32_t b_len)
{
#undef N0
#undef N1
#define N0 BN8448_N
#define N1 (N0 + 1)

	uint32_t rquo[N1], rrem[N1], sh_b[N1];
	int32_t sh = a_len - b_len;

	ecp_memcpy(rrem, a, sizeof(uint32_t) * N0);
	ecp_memcpy(sh_b, b, sizeof(uint32_t) * N0);
	rrem[N0] = sh_b[N0] = 0;

	_udiv_lshift(sh_b, sh);
	while (_udiv_ucmp(rrem, sh_b) >= 0) {
		_udiv_lshift(sh_b, 1);
		sh++;
	}

	ecp_memset(rquo, 0, sizeof(rquo));
	for (; sh > 0; sh--) {
		_udiv_rshift(sh_b, 1);
		_udiv_lshift(rquo, 1);
		if (_udiv_ucmp(rrem, sh_b) >= 0) {
			_udiv_usub(rrem, rrem, sh_b);
			rquo[0] |= 1;
		}
	}

	ecp_memcpy(rem, rrem, sizeof(uint32_t) * N0);
	ecp_memcpy(quo, rquo, sizeof(uint32_t) * N0);
}

/* @func: ecp_bn8448_udiv
 * #desc:
 *    big number division (unsigned).
 *
 * #1: quo [out] quotient
 * #2: rem [out] remainder
 * #3: a   [in]  dividend
 * #4: b   [in]  divisor
 */
void ecp_bn8448_udiv(bn8448_t *quo, bn8448_t *rem,
		const bn8448_t *a, const bn8448_t *b)
{
	bn8448_t rquo, rrem;
	ecp_bn8448_move(&rrem, a);
	ecp_bn8448_move(&rquo, b);

	if (rem)
		ecp_bn8448_move(rem, &rrem);
	if (quo)
		ecp_bn8448_set1(quo, 0);

	if (!ecp_bn8448_ucmp_1(&rquo, 0)) {
		if (rem)
			ecp_bn8448_set1(rem, 0);
		return;
	}
	if (ecp_bn8448_ucmp(&rrem, &rquo) < 0)
		return;

	int32_t a_len = ecp_bn8448_bits(&rrem);
	int32_t b_len = ecp_bn8448_bits(&rquo);
	if (!a_len || !b_len)
		return;

	_udiv_base(rquo.n, rrem.n, rrem.n, rquo.n, a_len, b_len);
	rrem.sign = rquo.sign = 0;

	if (quo)
		ecp_bn8448_move(quo, &rquo);
	if (rem)
		ecp_bn8448_move(rem, &rrem);
}

/* @func: ecp_bn8448_udiv_1
 * #desc:
 *    big number division (unsigned, single a word).
 *
 * #1: quo [out] quotient
 * #2: rem [out] remainder
 * #3: a   [in]  dividend
 * #4: b   [in]  divisor
 */
void ecp_bn8448_udiv_1(bn8448_t *quo, bn8448_t *rem,
		const bn8448_t *a, uint32_t b)
{
	bn8448_t t;
	ecp_bn8448_set1(&t, b);

	ecp_bn8448_udiv(quo, rem, a, &t);
}

/* @func: ecp_bn8448_add
 * #desc:
 *    big number addition (signed).
 *
 * #1: r [out] sum
 * #2: a [in]  addend
 * #3: b [in]  addend
 */
void ecp_bn8448_add(bn8448_t *r,
		const bn8448_t *a, const bn8448_t *b)
{
	uint32_t sign = a->sign;

	if (sign == b->sign) {
		ecp_bn8448_uadd(r, a, b);
		r->sign = sign;
		return;
	}

	int32_t k = ecp_bn8448_ucmp(a, b);
	if (k > 0) {
		ecp_bn8448_usub(r, a, b);
		r->sign = sign;
	} else if (k < 0) {
		sign = b->sign;
		ecp_bn8448_usub(r, b, a);
		r->sign = sign;
	} else {
		ecp_bn8448_set1(r, 0);
	}
}

/* @func: ecp_bn8448_add_1
 * #desc:
 *    big number addition (signed, single a word).
 *
 * #1: r [out] sum
 * #2: a [in]  addend
 * #3: b [in]  addend
 */
void ecp_bn8448_add_1(bn8448_t *r,
		const bn8448_t *a, uint32_t b)
{
	bn8448_t t;
	ecp_bn8448_set1(&t, b);

	ecp_bn8448_add(r, a, &t);
}

/* @func: ecp_bn8448_sub
 * #desc:
 *    big number subtraction (signed).
 *
 * #1: r [out] difference
 * #2: a [in]  minuend
 * #3: b [in]  subtract
 */
void ecp_bn8448_sub(bn8448_t *r,
		const bn8448_t *a, const bn8448_t *b)
{
	bn8448_t t;
	ecp_bn8448_move(&t, b);
	t.sign = !b->sign;

	ecp_bn8448_add(r, a, &t);
}

/* @func: ecp_bn8448_sub_1
 * #desc:
 *    big number subtraction (signed, single a word).
 *
 * #1: r [out] difference
 * #2: a [in]  minuend
 * #3: b [in]  subtract
 */
void ecp_bn8448_sub_1(bn8448_t *r,
		const bn8448_t *a, uint32_t b)
{
	bn8448_t t;
	ecp_bn8448_set1(&t, b);
	t.sign = 1;

	ecp_bn8448_add(r, a, &t);
}

/* @func: ecp_bn8448_mul
 * #desc:
 *    big number multiplication (signed).
 *
 * #1: r [out] product
 * #2: a [in]  multiplicand
 * #3: b [in]  multiplier
 */
void ecp_bn8448_mul(bn8448_t *r,
		const bn8448_t *a, const bn8448_t *b)
{
	uint32_t sign = a->sign ^ b->sign;

	ecp_bn8448_umul(r, a, b);

	r->sign = ecp_bn8448_bits(r) ? sign : 0;
}

/* @func: ecp_bn8448_mul_1
 * #desc:
 *    big number multiplication (single a word).
 *
 * #1: r [out] product
 * #2: a [in]  multiplicand
 * #3: b [in]  multiplier
 */
void ecp_bn8448_mul_1(bn8448_t *r,
		const bn8448_t *a, uint32_t b)
{
	uint32_t sign = a->sign ^ 0;

	ecp_bn8448_umul_1(r, a, b);

	r->sign = ecp_bn8448_bits(r) ? sign : 0;
}

/* @func: ecp_bn8448_div
 * #desc:
 *    big number division (signed).
 *
 * #1: quo [out] quotient
 * #2: rem [out] remainder
 * #3: a   [in]  dividend
 * #4: b   [in]  divisor
 */
void ecp_bn8448_div(bn8448_t *quo, bn8448_t *rem,
		const bn8448_t *a, const bn8448_t *b)
{
	uint32_t a_sign = a->sign, b_sign = b->sign;

	ecp_bn8448_udiv(quo, rem, a, b);

	if (rem)
		rem->sign = ecp_bn8448_bits(rem) ? a_sign : 0;
	if (quo) {
		a_sign ^= b_sign;
		quo->sign = ecp_bn8448_bits(quo) ? a_sign : 0;
	}
}

/* @func: ecp_bn8448_div_1
 * #desc:
 *    big number division (single a word).
 *
 * #1: quo [out] quotient
 * #2: rem [out] remainder
 * #3: a   [in]  dividend
 * #4: b   [in]  divisor
 */
void ecp_bn8448_div_1(bn8448_t *quo, bn8448_t *rem,
		const bn8448_t *a, uint32_t b)
{
	uint32_t a_sign = a->sign, b_sign = 0;

	ecp_bn8448_udiv_1(quo, rem, a, b);

	if (rem)
		rem->sign = ecp_bn8448_bits(rem) ? a_sign : 0;
	if (quo) {
		a_sign ^= b_sign;
		quo->sign = ecp_bn8448_bits(quo) ? a_sign : 0;
	}
}

/* @func: ecp_bn8448_divmod
 * #desc:
 *    big number division modular (signed).
 *
 * #1: quo [out] quotient
 * #2: rem [out] remainder
 * #3: a   [in]  dividend
 * #4: b   [in]  divisor
 */
void ecp_bn8448_divmod(bn8448_t *quo, bn8448_t *rem,
		const bn8448_t *a, const bn8448_t *b)
{
	bn8448_t t;
	ecp_bn8448_move(&t, b);

	ecp_bn8448_div(quo, rem, a, b);

	if (rem && rem->sign) {
		ecp_bn8448_add(rem, rem, &t);
		rem->sign = 0;
	}
}

/* @func: ecp_bn8448_divmod_1
 * #desc:
 *    big number division modular (signed, single a word).
 *
 * #1: quo [out] quotient
 * #2: rem [out] remainder
 * #3: a   [in]  dividend
 * #4: b   [in]  divisor
 */
void ecp_bn8448_divmod_1(bn8448_t *quo, bn8448_t *rem,
		const bn8448_t *a, uint32_t b)
{
	ecp_bn8448_div_1(quo, rem, a, b);

	if (rem && rem->sign) {
		ecp_bn8448_add_1(rem, rem, b);
		rem->sign = 0;
	}
}

/* @func: ecp_bn8448_modpow
 * #desc:
 *    big number fast modular exponentiation.
 *
 * #1: r [out] result
 * #2: a [in]  number
 * #3: e [in]  exponential
 * #4: m [in]  modules
 */
void ecp_bn8448_modpow(bn8448_t *r,
		const bn8448_t *a, const bn8448_t *e, const bn8448_t *m)
{
	bn8448_t rr, a1;
	ecp_bn8448_set1(&rr, 1);

	/* a1 = a % m */
	ecp_bn8448_divmod(NULL, &a1, a, m);

	int32_t n = ecp_bn8448_bits(e);
	for (int32_t i = 0; i < n; i++) {
		uint32_t k_i = e->n[i / 32] >> (i % 32);
		if (k_i & 1) {
			/* rr = (rr * a1) % m */
			ecp_bn8448_mul(&rr, &rr, &a1);
			ecp_bn8448_divmod(NULL, &rr, &rr, m);
		}

		/* a1 = (a1 * a1) % m */
		ecp_bn8448_mul(&a1, &a1, &a1);
		ecp_bn8448_divmod(NULL, &a1, &a1, m);
	}

	ecp_bn8448_move(r, &rr);
}

/* @func: ecp_bn8448_gcd
 * #desc:
 *    big number extended euclidean algorithm (non-recursive).
 *
 * #1: r [out] greatest common divisor
 * #2: x [out] bezout coefficient
 * #3: y [out] bezout coefficient
 * #4: a [in]  number
 * #5: b [in]  number
 */
void ecp_bn8448_gcd(bn8448_t *r, bn8448_t *x, bn8448_t *y,
		const bn8448_t *a, const bn8448_t *b)
{
	bn8448_t a1, b1, s1, s2, t1, t2, t3, t4;
	ecp_bn8448_move(&a1, a);
	ecp_bn8448_move(&b1, b);
	ecp_bn8448_set1(&s1, 1);
	ecp_bn8448_set1(&s2, 0);
	ecp_bn8448_set1(&t1, 0);
	ecp_bn8448_set1(&t2, 1);

	while (ecp_bn8448_bits(&b1)) {
		/* t3 = a1 / b1 */
		ecp_bn8448_divmod(&t3, NULL, &a1, &b1);

		/* (a1, b1) = (b1, a1 - (b1 * t3)) */
		ecp_bn8448_mul(&t4, &b1, &t3);
		ecp_bn8448_sub(&t4, &a1, &t4);
		ecp_bn8448_move(&a1, &b1);
		ecp_bn8448_move(&b1, &t4);

		/* (s1, s2) = (s2, s1 - (s2 * t3)) */
		ecp_bn8448_mul(&t4, &s2, &t3);
		ecp_bn8448_sub(&t4, &s1, &t4);
		ecp_bn8448_move(&s1, &s2);
		ecp_bn8448_move(&s2, &t4);

		/* (t1, t2) = (t2, t1 - (t2 * t3)) */
		ecp_bn8448_mul(&t4, &t2, &t3);
		ecp_bn8448_sub(&t4, &t1, &t4);
		ecp_bn8448_move(&t1, &t2);
		ecp_bn8448_move(&t2, &t4);
	}

	ecp_bn8448_move(r, &a1);
	ecp_bn8448_move(x, &s1);
	ecp_bn8448_move(y, &t1);
}

/* @func: ecp_bn8448_inv
 * #desc:
 *    big number modular inverse.
 *
 * #1: r [out] inverse modulus
 * #2: a [in]  number
 * #3: b [in]  modulus
 * #r:   [ret] 0: success, -1: fail
 */
int32_t ecp_bn8448_inv(bn8448_t *r,
		const bn8448_t *a, const bn8448_t *b)
{
	bn8448_t g, x, y;

	ecp_bn8448_gcd(&g, &x, &y, a, b);
	if (ecp_bn8448_cmp_1(&g, 1)) {
		ecp_bn8448_set1(r, 0);
		return -1;
	}

	/* r = ((x % b) + b) % b */
	ecp_bn8448_divmod(NULL, &x, &x, b);
	ecp_bn8448_add(&x, &x, b);
	ecp_bn8448_divmod(NULL, r, &x, b);

	return 0;
}

/* @func: ecp_bn8448_redc_init
 * #desc:
 *    big number redc montgomery reduction initialization.
 *
 * #1: s [out] redc context
 * #2: n [in]  modules
 * #r:   [ret] 0: success, -1: fail
 */
int32_t ecp_bn8448_redc_init(bn8448_redc_t *s, const bn8448_t *n)
{
	s->k = ecp_bn8448_bits(n);
	ecp_bn8448_move(&s->n, n);

	/* R = 1 << k */
	ecp_bn8448_set1(&s->R, 1);
	ecp_bn8448_lshift(&s->R, s->k);

	/* n_prime = -inv(n, R) */
	if (ecp_bn8448_inv(&s->np, n, &s->R))
		return -1;
	s->np.sign = 1;

	/* n_prime = n_prime % R */
	ecp_bn8448_divmod(NULL, &s->np, &s->np, &s->R);

	/* mask = R - 1 */
	ecp_bn8448_sub_1(&s->mask, &s->R, 1);

	/* R' = (R * R) % n */
	ecp_bn8448_mul(&s->R, &s->R, &s->R);
	ecp_bn8448_divmod(NULL, &s->R, &s->R, n);

	return 0;
}

/* @func: ecp_bn8448_redc_mod
 * #desc:
 *    big number redc montgomery reduction (norm).
 *
 * #1: s [in]  redc context
 * #2: r [out] result
 * #3: a [in]  number
 */
void ecp_bn8448_redc_mod(const bn8448_redc_t *s,
		bn8448_t *r, const bn8448_t *a)
{
	bn8448_t t;

	/* t = ((a & mask) * n_prime) & mask */
	ecp_bn8448_and(&t, a, &s->mask);
	ecp_bn8448_mul(&t, &t, &s->np);
	ecp_bn8448_and(&t, &t, &s->mask);

	/* t = (a + (t * n)) >> k */
	ecp_bn8448_mul(&t, &t, &s->n);
	ecp_bn8448_add(&t, &t, a);
	ecp_bn8448_rshift(&t, s->k);

	/* t = if t >= n : adjust */
	if (ecp_bn8448_cmp(&t, &s->n) >= 0)
		ecp_bn8448_sub(&t, &t, &s->n);

	ecp_bn8448_move(r, &t);
}

/* @func: ecp_bn8448_redc_mont
 * #desc:
 *    big number redc montgomery domain.
 *
 * #1: s [in]  redc context
 * #2: r [out] result
 * #3: a [in]  number
 */
void ecp_bn8448_redc_mont(const bn8448_redc_t *s,
		bn8448_t *r, const bn8448_t *a)
{
	bn8448_t t;

	/* r = redc(a * R') */
	ecp_bn8448_mul(&t, a, &s->R);
	ecp_bn8448_redc_mod(s, r, &t);
}

/* @func: ecp_bn8448_redc_pow
 * #desc:
 *    big number redc montgomery modular exponentiation.
 *
 * #1: r [out] result
 * #2: a [in]  number
 * #3: e [in]  exponential
 * #4: m [in]  modules
 * #r:   [ret] 0: success, -1: fail
 */
int32_t ecp_bn8448_redc_pow(bn8448_t *r,
		const bn8448_t *a, const bn8448_t *e, const bn8448_t *m)
{
	bn8448_redc_t s;
	bn8448_t rr, a1;

	if (ecp_bn8448_redc_init(&s, m))
		return -1;

	ecp_bn8448_set1(&rr, 1);
	ecp_bn8448_redc_mont(&s, &rr, &rr);
	ecp_bn8448_redc_mont(&s, &a1, a);

	int32_t n = ecp_bn8448_bits(e);
	for (int32_t i = 0; i < n; i++) {
		uint32_t k_i = e->n[i / 32] >> (i % 32);
		if (k_i & 1) {
			/* rr = (rr * a1) % m */
			ecp_bn8448_mul(&rr, &rr, &a1);
			ecp_bn8448_redc_mod(&s, &rr, &rr);
		}

		/* a1 = (a1 * a1) % m */
		ecp_bn8448_mul(&a1, &a1, &a1);
		ecp_bn8448_redc_mod(&s, &a1, &a1);
	}

	/* r = rr % m */
	ecp_bn8448_redc_mod(&s, r, &rr);

	return 0;
}

/* @func: ecp_bn8448_str2num
 * #desc:
 *    string to big number conversion.
 *
 * #1: t [out] result
 * #2: s [in]  string pointer
 * #3: e [out] end pointer / NULL
 * #4: b [in]  base type
 */
void ecp_bn8448_str2num(bn8448_t *t,
		const char *s, char **e, int32_t b)
{
	for (; *s == ' '; s++);

	int32_t neg = 0;
	if (*s == '-' || *s == '+')
		neg = (*s++ == '-') ? 1 : 0;

	if (b) {
		if (*s == '0') {
			s++;
			if (*s == 'B' || *s == 'b') {
				s++;
			} else if (*s == 'X' || *s == 'x') {
				s++;
			}
		}
	} else if (*s == '0') { /* b == 0 */
		s++;
		if (*s == 'B' || *s == 'b') {
			b = 2;
			s++;
		} else if (*s == 'X' || *s == 'x') {
			b = 16;
			s++;
		} else if (*s >= '0' && *s <= '7') {
			b = 8;
		} else {
			b = 10;
		}
	} else { /* default */
		b = 10;
	}

	for (; *s == '0'; s++);

	ecp_bn8448_set1(t, 0);
	for (int32_t d = 0; *s != '\0'; s++) {
		if (*s >= '0' && *s <= '9') {
			d = (*s - '0');
		} else if (*s >= 'A' && *s <= 'Z') {
			d = (*s - 'A') + 10;
		} else if (*s >= 'a' && *s <= 'z') {
			d = (*s - 'a') + 10;
		} else {
			break;
		}
		if (d >= b)
			break;

		ecp_bn8448_umul_1(t, t, b);
		ecp_bn8448_uadd_1(t, t, d);
	}

	if (e) /* end position */
		*e = (char *)s;

	t->sign = neg;
}

/* @func: _out_pad (static)
 * #desc:
 *    padding character to buffer.
 *
 * #1: n [in]  buffer offset
 * #2: p [out] buffer
 * #3: c [in]  character
 * #4: x [in]  padding length
 * #r:   [ret] padding length (+offset)
 */
static int32_t _out_pad(int32_t n, char *p, char c, int32_t x)
{
	while (x-- > 0)
		p[n++] = c;

	return n;
}

/* @func: _int2str_d (static)
 * #desc:
 *    unsigned number to decimal conversion.
 *
 * #1: n [in]  buffer offset
 * #2: p [out] buffer
 * #3: v [in]  input number
 * #r:   [ret] output length (+offset)
 */
static int32_t _int2str_d(int32_t n, char *p, uint64_t v)
{
	int32_t t = n;

	do {
		p[n++] = (v % 10) + '0';
		v /= 10;
	} while (v);

	int32_t k = n;
	while (k > t) {
		char w = p[--k];
		p[k] = p[t];
		p[t++] = w;
	}

	return n;
}

/* @func: ecp_bn8448_num2str
 * #desc:
 *    big number to string conversion.
 *
 * #1: a [in]  number
 * #2: p [out] output buffer
 */
void ecp_bn8448_num2str(const bn8448_t *a, char *p)
{
	bn8448_t _a, r, b;
	ecp_bn8448_move(&_a, a);

	if (a->sign)
		*p++ = '-';

	char buf[24];
	int32_t k = 0, n = 0, t = 0;
	uint64_t tmp = 10000000000000000000ULL;

	ecp_bn8448_set1(&b, (uint32_t)tmp);
	b.n[1] = tmp >> 32;

	do {
		ecp_bn8448_udiv(&_a, &r, &_a, &b);
		tmp = (uint64_t)r.n[0] | (uint64_t)r.n[1] << 32;
		k = _int2str_d(0, buf, tmp);
		n = _out_pad(n, p, '0', 19 - k);
		n = _int2str_d(n, p, tmp);
	} while (ecp_bn8448_cmp_1(&_a, 0));

	k = n;
	while (k > t) {
		k -= 19;
		ecp_memcpy(buf, p + k, 19);
		ecp_memcpy(p + k, p + t, 19);
		ecp_memcpy(p + t, buf, 19);
		t += 19;
	}

	for (k = 0; n > 1 && p[k] == '0'; k++, n--);
	ecp_memcpy(p, p + k, n);
	p[n] = '\0';
}

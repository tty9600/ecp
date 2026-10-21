/* @file: t_mldsa_table.c
 * #desc:
 *
 * #copy:
 *    Copyright (C) 2026 - Alex Chen <chenalex.cy@gmail.com>
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program. If not,
 *    see <https://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <ecp/c/stdint.h>

#define MLDSA_Q 8380417     /* 2^23-2^13+1 */
#define MLDSA_ROOT 1753     /* 1753^(q-1) == 1 (mod q) */
#define MLDSA_MONT -4186625 /* 2^32 == 4193792 (mod q) */
#define MLDSA_QINV 58728449 /* q^-1 == 58728449 (mod 2^32) */

#define MONT_MUL(a, b) _montgomery_reduce((int64_t)(a) * (b))

#define MLDSA_N 256

int32_t mldsa_table[MLDSA_N];
int32_t tmp[MLDSA_N];

static int32_t _bit_reverse(int32_t c, int32_t len)
{
	int32_t r = 0;
	while (len--) {
		r |= c & 1;
		r <<= 1;
		c >>= 1;
	}

	return r >> 1;
}

static int32_t _montgomery_reduce(int64_t a)
{
	int32_t t = (int32_t)a * MLDSA_QINV;
	t = (a - (int64_t)t * MLDSA_Q) >> 32;

	return t;
}

void mldsa_table_init(void)
{
	tmp[0] = MLDSA_MONT; /* multiplied by montgomery factor */
	for (int32_t i = 1; i < MLDSA_N; i++) {
		tmp[i] = MONT_MUL(tmp[i - 1],
			((int64_t)MLDSA_MONT * MLDSA_ROOT) % MLDSA_Q);
	}

	for (int32_t i = 0; i < MLDSA_N; i++) {
		mldsa_table[i] = tmp[_bit_reverse(i, 8)];
		/* centralization */
		if (mldsa_table[i] > (MLDSA_Q / 2))
			mldsa_table[i] -= MLDSA_Q;
		if (mldsa_table[i] < (-MLDSA_Q / 2))
			mldsa_table[i] += MLDSA_Q;
	}
}

void mldsa_table_print(char *s1, char *s2)
{
	printf("/* %s */\n", s1);
	printf("static const int32_t %szetas = {\n\t", s2);
	for (int32_t i = 0; i < MLDSA_N; i++) {
		printf("%8d", mldsa_table[i]);
		if ((i + 1) != MLDSA_N) {
			printf(",");
			if ((i + 1) % 7)
				printf(" ");
			if (!((i + 1) % 7))
				printf("\n\t");
		} else {
			printf("\n\t");
		}
	}
	printf("};\n\n");
}

int main(void)
{
	mldsa_table_init();
	mldsa_table_print("butterfly operations for root of unity", "");

	return 0;
}

/* @file: t_mlkem_table.c
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

#define MLKEM_Q 3329     /* 2^8*13+1 */
#define MLKEM_ROOT 17    /* 17^(q-1) == 1 (mod q) */
#define MLKEM_MONT -1044 /* 2^16 == 2285 (mod q) */
#define MLKEM_QINV -3327 /* q^-1 == 62209 (mod 2^16) */

#define MONT_MUL(a, b) _montgomery_reduce((int32_t)(a) * (b))

#define MLKEM_N 256

int16_t mlkem_table[MLKEM_N / 2];
int16_t tmp[MLKEM_N / 2];

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

static int16_t _montgomery_reduce(int32_t a)
{
	int16_t t = (int16_t)a * MLKEM_QINV;
	t = (a - (int32_t)t * MLKEM_Q) >> 16;

	return t;
}

void mlkem_table_init(void)
{
	tmp[0] = MLKEM_MONT; /* multiplied by montgomery factor */
	for (int32_t i = 1; i < (MLKEM_N / 2); i++) {
		tmp[i] = MONT_MUL(tmp[i - 1],
			(MLKEM_MONT * MLKEM_ROOT) % MLKEM_Q);
	}

	for (int32_t i = 0; i < (MLKEM_N / 2); i++) {
		mlkem_table[i] = tmp[_bit_reverse(i, 7)];
		/* centralization */
		if (mlkem_table[i] > (MLKEM_Q / 2))
			mlkem_table[i] -= MLKEM_Q;
		if (mlkem_table[i] < (-MLKEM_Q / 2))
			mlkem_table[i] += MLKEM_Q;
	}
}

void mlkem_table_print(char *s1, char *s2)
{
	printf("/* %s */\n", s1);
	printf("static const int16_t %szetas = {\n\t", s2);
	for (int32_t i = 0; i < (MLKEM_N / 2); i++) {
		printf("%5d", mlkem_table[i]);
		if ((i + 1) != (MLKEM_N / 2)) {
			printf(",");
			if ((i + 1) % 8)
				printf(" ");
			if (!((i + 1) % 8))
				printf("\n\t");
		} else {
			printf("\n\t");
		}
	}
	printf("};\n\n");
}

int main(void)
{
	mlkem_table_init();
	mlkem_table_print("butterfly operations for root of unity", "");

	return 0;
}

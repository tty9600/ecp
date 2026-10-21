/* @file: t_basenc_table.c
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

uint8_t table[256];

void base16_table_init(void)
{
	/* 0123456789ABCDEF */

	for (int32_t i = 0; i < 256; i++) {
		if (i >= 'A' && i <= 'F') {
			table[i] = i - 'A' + 10;
		} else if (i >= 'a' && i <= 'f') {
			table[i] = i - 'a' + 10;
		} else if (i >= '0' && i <= '9') {
			table[i] = i - '0';
		} else {
			table[i] = 255;
		}
	}
}

void base32_table_init(void)
{
	/* ABCDEFGHIJKLMNOPQRSTUVWXYZ234567 */

	for (int32_t i = 0; i < 256; i++) {
		if (i >= 'A' && i <= 'Z') {
			table[i] = i - 'A';
		} else if (i >= '2' && i <= '7') {
			table[i] = i - '2' + 26;
		} else if (i == '=') {
			table[i] = 254;
		} else {
			table[i] = 255;
		}
	}
}

void base64_table_init(void)
{
	/* ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/ */

	for (int32_t i = 0; i < 256; i++) {
		if (i >= 'A' && i <= 'Z') {
			table[i] = i - 'A';
		} else if (i >= 'a' && i <= 'z') {
			table[i] = i - 'a' + 26;
		} else if (i >= '0' && i <= '9') {
			table[i] = i - '0' + 52;
		} else if (i == '+') {
			table[i] = i - '+' + 62;
		} else if (i == '/') {
			table[i] = i - '/' + 63;
		} else if (i == '=') {
			table[i] = 254;
		} else {
			table[i] = 255;
		}
	}
}

void base64url_table_init(void)
{
	/* ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_ */

	for (int32_t i = 0; i < 256; i++) {
		if (i >= 'A' && i <= 'Z') {
			table[i] = i - 'A';
		} else if (i >= 'a' && i <= 'z') {
			table[i] = i - 'a' + 26;
		} else if (i >= '0' && i <= '9') {
			table[i] = i - '0' + 52;
		} else if (i == '-') {
			table[i] = i - '-' + 62;
		} else if (i == '_') {
			table[i] = i - '_' + 63;
		} else if (i == '=') {
			table[i] = 254;
		} else {
			table[i] = 255;
		}
	}
}

void base64bcrypt_table_init(void)
{
	/* ./ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789 */

	for (int32_t i = 0; i < 256; i++) {
		if (i == '.') {
			table[i] = i - '.';
		} else if (i == '/') {
			table[i] = i - '/' + 1;
		} else if (i >= 'A' && i <= 'Z') {
			table[i] = i - 'A' + 2;
		} else if (i >= 'a' && i <= 'z') {
			table[i] = i - 'a' + 28;
		} else if (i >= '0' && i <= '9') {
			table[i] = i - '0' + 54;
		} else if (i == '=') {
			table[i] = 254;
		} else {
			table[i] = 255;
		}
	}
}

void basenc_table_print(char *s1, char *s2)
{
	printf("/* %s */\n", s1);
	printf("static const uint8_t %s = {\n\t", s2);
	for (int32_t i = 0; i < 256; i++) {
		printf("%3d", table[i]);
		if ((i + 1) != 256) {
			printf(",");
			if ((i + 1) % 12)
				printf(" ");
			if (!((i + 1) % 12))
				printf("\n\t");
		} else {
			printf("\n\t");
		}
	}
	printf("};\n\n");
}

int main(void)
{
	base16_table_init();
	basenc_table_print("base16", "b16d");

	base32_table_init();
	basenc_table_print("base32", "b32d");

	base64_table_init();
	basenc_table_print("base64", "b64d");

	base64url_table_init();
	basenc_table_print("base64url", "b64d");

	base64bcrypt_table_init();
	basenc_table_print("base64bcrypt", "b64d");

	return 0;
}

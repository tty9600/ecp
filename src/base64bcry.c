/* @file: base64bcry.c
 * #desc:
 *    The implementations of base64bcrypt coding.
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
#include <ecp/basenc.h>

static const uint8_t b64e[] = "-_"
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	"abcdefghijklmnopqrstuvwxyz"
	"0123456789";

static const uint8_t b64d[256] = {
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255,   0,   1,
	 54,  55,  56,  57,  58,  59,  60,  61,  62,  63, 255, 255,
	255, 254, 255, 255, 255,   2,   3,   4,   5,   6,   7,   8,
	  9,  10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  20,
	 21,  22,  23,  24,  25,  26,  27, 255, 255, 255, 255, 255,
	255,  28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,
	 39,  40,  41,  42,  43,  44,  45,  46,  47,  48,  49,  50,
	 51,  52,  53, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255
	};

/* @func: ecp_base64bcry_enc
 * #desc:
 *    base64bcrypt encoding function.
 *
 * #1: s    [in]     input buffer
 * #2: slen [in]     input length
 * #3: t    [out]    output buffer
 * #4: tlen [in/out] output length and return the remaining length
 */
void ecp_base64bcry_enc(const char *s, uint32_t slen, char *t, uint32_t *tlen)
{
	uint32_t n = *tlen;
	while (slen && n) {
		*t++ = b64e[((uint8_t)s[0] >> 2) & 0x3f];
		if (!--n)
			break;

		*t++ = b64e[(((uint8_t)s[0] << 4)
			+ (--slen ? ((uint8_t)s[1] >> 4) : 0)) & 0x3f];
		if (!--n)
			break;

		*t++ = slen ? (b64e[(((uint8_t)s[1] << 2)
			+ (--slen ? ((uint8_t)s[2] >> 6) : 0)) & 0x3f]) : '=';
		if (!--n)
			break;

		*t++ = slen ? b64e[((uint8_t)s[2]) & 0x3f] : '=';
		if (!--n)
			break;

		if (slen)
			slen--;
		if (slen)
			s += 3;
	}
	*tlen = n;
}

/* @func: _base64bcry_dec_4 (static)
 * #desc:
 *    base64bcrypt decoding.
 *
 * #1: s    [in]  input buffer (8byte)
 * #2: t    [out] output buffer
 * #3: tlen [in]  output length
 * #r:      [ret]
 *    >0: output size, -1: output buffer full, -2: invalid character error
 */
static int32_t _base64bcry_dec_4(const char *s, char *t, uint32_t tlen)
{
	uint8_t c0, c1, c2, c3;
	c0 = b64d[(uint8_t)s[0]];
	c1 = b64d[(uint8_t)s[1]];
	c2 = b64d[(uint8_t)s[2]];
	c3 = b64d[(uint8_t)s[3]];

	if (c0 == 254 || c1 == 254) /* '==' */
		return -2;

	/* c-1 */
	if (!tlen--)
		return -1;
	*t++ = (c0 << 2) | (c1 >> 4);

	if (c2 == 254) { /* '=' */
		if (c3 == 254)
			return 1;
		return -2;
	}

	/* c-2 */
	if (!tlen--)
		return -1;
	*t++ = ((c1 << 4) & 0xf0) | (c2 >> 2);

	if (c3 == 254) /* '=' */
		return 2;

	/* c-3 */
	if (!tlen--)
		return -1;
	*t++ = ((c2 << 6) & 0xc0) | c3;

	return 3;
}

/* @func: ecp_base64bcry_dec
 * #desc:
 *    base64bcrypt decoding function.
 *
 * #1: s    [in]     input buffer
 * #2: slen [in]     input length
 * #3: t    [out]    output buffer
 * #4: tlen [in/out] output length and return the remaining length
 * #r:      [ret]
 *    0: no error, -1: output buffer full, -2: invalid character error,
 *    -3: input incomplete
 */
int32_t ecp_base64bcry_dec(const char *s, uint32_t slen, char *t,
		uint32_t *tlen)
{
	char buf[4];
	int32_t k = 0;

	for (uint32_t i = 0; i < slen; i++) {
		char c = s[i];
		if (b64d[(uint8_t)c] == 255)
			return -2;

		buf[k++] = c;
		if (k == 4) {
			k = _base64bcry_dec_4(buf, t, *tlen);
			if (k < 0)
				return k;

			*tlen -= k;
			t += k;
			k = 0;
		}
	}

	return k ? -3 : 0;
}

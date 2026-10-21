/* @file: pbkdf2_blake2b.c
 * #desc:
 *    The implementations of password-based key derivation function (blake2b).
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
#include <ecp/hmac.h>
#include <ecp/pbkdf2.h>

/* @func: ecp_pbkdf2_blake2b
 * #desc:
 *    pbkdf2-blake2b key derivation function.
 *
 * #1: pass     [in]  password
 * #2: pass_len [in]  password length
 * #3: salt     [in]  salt
 * #4: salt_len [in]  salt length
 * #5: odk      [out] output key
 * #6: len      [in]  key length
 * #7: k        [in]  iterations number
 */
void ecp_pbkdf2_blake2b(const uint8_t *pass, uint32_t pass_len,
		const uint8_t *salt, uint32_t salt_len, uint8_t *odk,
		uint32_t len, uint32_t k)
{
	HMAC_BLAKE2B_NEW(ctx);
	uint8_t tmp[BLAKE2B_512_LEN], u[BLAKE2B_512_LEN], count[4];
	uint32_t f = 0;

	uint32_t n = (len + BLAKE2B_512_LEN - 1) / BLAKE2B_512_LEN;
	for (uint32_t i = 1; i <= n; i++) {
		count[0] = (i >> 24) & 0xff;
		count[1] = (i >> 16) & 0xff;
		count[2] = (i >> 8) & 0xff;
		count[3] = i & 0xff;

		ecp_hmac_blake2b_init(&ctx, pass, pass_len);
		ecp_hmac_blake2b_process(&ctx, salt, salt_len);
		ecp_hmac_blake2b_process(&ctx, count, 4);
		ecp_hmac_blake2b_finish(&ctx);
		ecp_memcpy(u, &(HMAC_BLAKE2B_STATE(&ctx, 0)),
			BLAKE2B_512_LEN);
		ecp_memcpy(tmp, &(HMAC_BLAKE2B_STATE(&ctx, 0)),
			BLAKE2B_512_LEN);

		for (uint32_t j = 1; j < k; j++) {
			ecp_hmac_blake2b_init(&ctx, pass, pass_len);
			ecp_hmac_blake2b_process(&ctx, u,
				BLAKE2B_512_LEN);
			ecp_hmac_blake2b_finish(&ctx);
			ecp_memcpy(u, &(HMAC_BLAKE2B_STATE(&ctx, 0)),
				BLAKE2B_512_LEN);

			for (int32_t l = 0; l < BLAKE2B_512_LEN; l++)
				tmp[l] ^= HMAC_BLAKE2B_STATE(&ctx, l);
		}

		ecp_memcpy(odk + f, tmp, ((f + BLAKE2B_512_LEN) > len) ?
			(len % BLAKE2B_512_LEN) : BLAKE2B_512_LEN);
		f += BLAKE2B_512_LEN;
	}
}

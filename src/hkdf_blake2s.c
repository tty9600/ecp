/* @file: hkdf_blake2s.c
 * #desc:
 *    The implementations of hmac-base key derivation function (blake2s).
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
#include <ecp/hkdf.h>

/* @func: ecp_hkdf_blake2s
 * #desc:
 *    hkdf-blake2s key derivation function.
 *
 * #1: ikm      [in]  ikm
 * #2: ikm_len  [in]  ikm length
 * #3: salt     [in]  salt
 * #4: salt_len [in]  salt length
 * #5: info     [in]  info
 * #6: info_len [in]  info length
 * #7: okm      [out] output key
 * #8: len      [in]  key length
 * #r:          [ret] 0: no error, -1: key length error
 */
int32_t ecp_hkdf_blake2s(const uint8_t *ikm, uint32_t ikm_len,
		const uint8_t *salt, uint32_t salt_len,
		const uint8_t *info, uint32_t info_len,
		uint8_t *okm, uint32_t len)
{
	HMAC_BLAKE2S_NEW(ctx);
	uint8_t tmp[BLAKE2S_256_LEN], prk[BLAKE2S_256_LEN], count[1];
	uint32_t f = 0, tmp_len = 0;

	if (len < 1 || len > (255 * BLAKE2S_256_LEN))
		return -1;

	ecp_hmac_blake2s_init(&ctx, salt, salt_len);
	ecp_hmac_blake2s_process(&ctx, ikm, ikm_len);
	ecp_hmac_blake2s_finish(&ctx);
	ecp_memcpy(prk, &(HMAC_BLAKE2S_STATE(&ctx, 0)),
		BLAKE2S_256_LEN);

	uint32_t n = (len + BLAKE2S_256_LEN - 1) / BLAKE2S_256_LEN;
	for (uint32_t i = 1; i <= n; i++) {
		count[0] = i & 0xff;

		ecp_hmac_blake2s_init(&ctx, prk, BLAKE2S_256_LEN);
		ecp_hmac_blake2s_process(&ctx, tmp, tmp_len);
		ecp_hmac_blake2s_process(&ctx, info, info_len);
		ecp_hmac_blake2s_process(&ctx, count, 1);
		ecp_hmac_blake2s_finish(&ctx);
		tmp_len = BLAKE2S_256_LEN;

		ecp_memcpy(tmp, &(HMAC_BLAKE2S_STATE(&ctx, 0)),
			BLAKE2S_256_LEN);
		ecp_memcpy(okm + f, tmp, ((f + BLAKE2S_256_LEN) > len) ?
			(len % BLAKE2S_256_LEN) : BLAKE2S_256_LEN);
		f += BLAKE2S_256_LEN;
	}

	return 0;
}

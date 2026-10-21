/* @file: pw_bcrypt.c
 * #desc:
 *    The implementations of bcrypt password-hash.
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
#include <ecp/pw_bcrypt.h>
#include <ecp/blowfish.h>

/* @func: ecp_bcrypt
 * #desc:
 *    bcrypt password-hashing function.
 *
 * #1: pass     [in]  password
 * #2: pass_len [in]  password length
 * #3: salt     [in]  salt
 * #4: salt_len [in]  salt length
 * #5: ohp      [out] output hashpass
 * #6: k        [in]  cost (2^N)
 */
void ecp_bcrypt(const uint8_t *pass, uint32_t pass_len,
		const uint8_t *salt, uint32_t salt_len,
		uint8_t *ohp, uint32_t k)
{
	BLOWFISH_NEW(ctx);
	uint32_t n = 1U << k;

	ecp_blowfish_init(&ctx, NULL, 0);
	ecp_blowfish_ekskey(&ctx, salt, salt_len, pass, pass_len);

	for (uint32_t i = 0; i < n; i++) {
		ecp_blowfish_setkey(&ctx, pass, pass_len);
		ecp_blowfish_setkey(&ctx, salt, salt_len);
	}

	uint32_t cdata[6] = { /* "OrpheanBeholderScryDoubt" 24-byte */
		0x4f727068, 0x65616e42, 0x65686f6c, 0x64657253,
		0x63727944, 0x6f756274
		};

	for (int32_t i = 0; i < 64; i++) {
		ecp_blowfish_encrypt(&ctx, &cdata[0], &cdata[1]);
		ecp_blowfish_encrypt(&ctx, &cdata[2], &cdata[3]);
		ecp_blowfish_encrypt(&ctx, &cdata[4], &cdata[5]);
	}

	for (int32_t i = 0; i < 6; i++) {
		ohp[4 * i] = (cdata[i] >> 24) & 0xff;
		ohp[4 * i + 1] = (cdata[i] >> 16) & 0xff;
		ohp[4 * i + 2] = (cdata[i] >> 8) & 0xff;
		ohp[4 * i + 3] = cdata[i] & 0xff;
	}
}

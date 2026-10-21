/* @file: aes_modes.c
 * #desc:
 *    The implementations of block cipher modes of operation.
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
#include <ecp/aes.h>

/* @func: ecp_aes_encrypt_cbc
 * #desc:
 *    aes encryption cbc (cipher block chaining) mode.
 *
 * #1: ctx   [in]     aes struct context
 * #2: iv    [in/out] invector
 * #3: state [in/out] state buffer
 */
void ecp_aes_encrypt_cbc(struct aes_ctx *ctx, uint8_t *iv, uint8_t *state)
{
	for (int32_t i = 0; i < AES_BLOCKSIZE; i++)
		state[i] ^= iv[i];

	ecp_aes_encrypt(ctx, state);

	for (int32_t i = 0; i < AES_BLOCKSIZE; i++)
		iv[i] = state[i];
}

/* @func: ecp_aes_decrypt_cbc
 * #desc:
 *    aes decryption cbc (cipher block chaining) mode.
 *
 * #1: ctx   [in]     aes struct context
 * #2: iv    [in/out] invector
 * #3: state [in/out] state buffer
 */
void ecp_aes_decrypt_cbc(struct aes_ctx *ctx, uint8_t *iv, uint8_t *state)
{
	uint8_t tmp[AES_BLOCKSIZE];
	for (int32_t i = 0; i < AES_BLOCKSIZE; i++)
		tmp[i] = state[i];

	ecp_aes_decrypt(ctx, state);

	for (int32_t i = 0; i < AES_BLOCKSIZE; i++) {
		state[i] ^= iv[i];
		iv[i] = tmp[i];
	}
}

/* @func: ecp_aes_encrypt_cfb
 * #desc:
 *    aes encryption cfb (cipher feedback) mode.
 *
 * #1: ctx   [in]     aes struct context
 * #2: iv    [in/out] invector
 * #3: state [in/out] state buffer
 */
void ecp_aes_encrypt_cfb(struct aes_ctx *ctx, uint8_t *iv, uint8_t *state)
{
	ecp_aes_encrypt(ctx, iv);

	for (int32_t i = 0; i < AES_BLOCKSIZE; i++) {
		state[i] ^= iv[i];
		iv[i] = state[i];
	}
}

/* @func: ecp_aes_decrypt_cfb
 * #desc:
 *    aes decryption cfb (cipher feedback) mode.
 *
 * #1: ctx   [in]     aes struct context
 * #2: iv    [in/out] invector
 * #3: state [in/out] state buffer
 */
void ecp_aes_decrypt_cfb(struct aes_ctx *ctx, uint8_t *iv, uint8_t *state)
{
	uint8_t tmp[AES_BLOCKSIZE];
	for (int32_t i = 0; i < AES_BLOCKSIZE; i++)
		tmp[i] = state[i];

	ecp_aes_encrypt(ctx, iv);

	for (int32_t i = 0; i < AES_BLOCKSIZE; i++) {
		state[i] ^= iv[i];
		iv[i] = tmp[i];
	}
}

/* @func: ecp_aes_crypto_ofb
 * #desc:
 *    aes stream encryption ofb (output feedback) mode.
 *
 * #1: ctx [in]     aes struct context
 * #2: iv  [in/out] invector
 * #3: buf [in/out] buffer
 * #4: len [in]     length
 */
void ecp_aes_crypto_ofb(struct aes_ctx *ctx, uint8_t *iv, uint8_t *buf,
		size_t len)
{
	size_t n = AES_BLOCKSIZE;
	while (len) {
		if (len < n)
			n = len;

		ecp_aes_encrypt(ctx, iv);
		for (size_t i = 0; i < n; i++)
			buf[i] ^= iv[i];

		buf += n;
		len -= n;
	}
}

/* @func: ecp_aes_crypto_ctr
 * #desc:
 *    aes stream encryption ctr (counter) mode.
 *
 * #1: ctx [in]     aes struct context
 * #2: ran [in/out] nonce
 * #3: buf [in/out] buffer
 * #4: len [in]     length
 */
void ecp_aes_crypto_ctr(struct aes_ctx *ctx, uint8_t *ran, uint8_t *buf,
		size_t len)
{
	union {
		uint32_t count[4];
		uint8_t ctr[AES_BLOCKSIZE];
	} u;

	for (int32_t i = 0; i < AES_BLOCKSIZE; i++)
		u.ctr[i] = ran[i];

	size_t n = AES_BLOCKSIZE;
	while (len) {
		if (len < n)
			n = len;

		ecp_aes_encrypt(ctx, ran);
		for (size_t i = 0; i < n; i++)
			buf[i] ^= ran[i];

		buf += n;
		len -= n;

#define BSWAP32(x) \
	((((x) & 0xff) << 24) \
	| (((x) & 0xff00) << 8) \
	| (((x) & 0xff0000) >> 8) \
	| (((x) & 0xff000000) >> 24))

		uint32_t x = u.count[3];
		x = BSWAP32(x);
		x++;
		x = BSWAP32(x);
		u.count[3] = x;

		for (int32_t i = 0; i < AES_BLOCKSIZE; i++)
			ran[i] = u.ctr[i];
	}
}

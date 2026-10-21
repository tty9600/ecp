/* @file: ecc.h
 * #desc:
 *    The definitions of elliptic curve cryptography.
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

#ifndef _ECP_ECC_H
#define _ECP_ECC_H

#include <ecp/config.h>
#include <ecp/c/stddef.h>
#include <ecp/c/stdint.h>

#define ECDH_X25519_LEN 32
#define ECDH_X25519_PRI_LEN ECDH_X25519_LEN
#define ECDH_X25519_PUB_LEN ECDH_X25519_LEN
#define ECDH_X25519_KEY_LEN ECDH_X25519_LEN
#define EDDSA_ED25519_LEN 32
#define EDDSA_ED25519_PRI_LEN EDDSA_ED25519_LEN
#define EDDSA_ED25519_RAN_LEN EDDSA_ED25519_LEN
#define EDDSA_ED25519_PUB_LEN EDDSA_ED25519_LEN
#define EDDSA_ED25519_SIG_LEN 64

#define ECDH_X448_LEN 56
#define ECDH_X448_PRI_LEN ECDH_X448_LEN
#define ECDH_X448_PUB_LEN ECDH_X448_LEN
#define ECDH_X448_KEY_LEN ECDH_X448_LEN
#define EDDSA_ED448_LEN 57
#define EDDSA_ED448_PRI_LEN EDDSA_ED448_LEN
#define EDDSA_ED448_RAN_LEN EDDSA_ED448_LEN
#define EDDSA_ED448_PUB_LEN EDDSA_ED448_LEN
#define EDDSA_ED448_SIG_LEN 114

#define ECDH_P256_LEN 32
#define ECDH_P256_PRI_LEN ECDH_P256_LEN
#define ECDH_P256_PUB_LEN 64
#define ECDH_P256_KEY_LEN ECDH_P256_LEN
#define ECDSA_P256_LEN 32
#define ECDSA_P256_PRI_LEN ECDSA_P256_LEN
#define ECDSA_P256_RAN_LEN ECDSA_P256_LEN
#define ECDSA_P256_PUB_LEN 64
#define ECDSA_P256_SIG_LEN 64

#define ECDH_P384_LEN 48
#define ECDH_P384_PRI_LEN ECDH_P384_LEN
#define ECDH_P384_PUB_LEN 96
#define ECDH_P384_KEY_LEN ECDH_P384_LEN
#define ECDSA_P384_LEN 48
#define ECDSA_P384_PRI_LEN ECDSA_P384_LEN
#define ECDSA_P384_RAN_LEN ECDSA_P384_LEN
#define ECDSA_P384_PUB_LEN 96
#define ECDSA_P384_SIG_LEN 96

#define ECDH_P521_LEN 66
#define ECDH_P521_PRI_LEN ECDH_P521_LEN
#define ECDH_P521_PUB_LEN 132
#define ECDH_P521_KEY_LEN ECDH_P521_LEN
#define ECDSA_P521_LEN 66
#define ECDSA_P521_PRI_LEN ECDSA_P521_LEN
#define ECDSA_P521_RAN_LEN ECDSA_P521_LEN
#define ECDSA_P521_PUB_LEN 132
#define ECDSA_P521_SIG_LEN 132

#ifdef __cplusplus
extern "C" {
#endif

/* curve25519.c */
extern
void ecp_ecdh_x25519_public_key(const uint8_t *pri, uint8_t *pub)
;
extern
int32_t ecp_ecdh_x25519_shared_key(const uint8_t *pri, const uint8_t *pub,
		uint8_t *key)
;
extern
void ecp_eddsa_ed25519_public_key(const uint8_t *pri, uint8_t *pub)
;
extern
void ecp_eddsa_ed25519_sign(const uint8_t *pri, const uint8_t *pub,
		const uint8_t *msg, size_t len, uint8_t *sig)
;
extern
int32_t ecp_eddsa_ed25519_verify(const uint8_t *pub,
		const uint8_t *sig, const uint8_t *msg, size_t len)
;

/* curve448.c */
extern
void ecp_ecdh_x448_public_key(const uint8_t *pri, uint8_t *pub)
;
extern
int32_t ecp_ecdh_x448_shared_key(const uint8_t *pri, const uint8_t *pub,
		uint8_t *key)
;
extern
void ecp_eddsa_ed448_public_key(const uint8_t *pri, uint8_t *pub)
;
extern
void ecp_eddsa_ed448_sign(const uint8_t *pri, const uint8_t *pub,
		const uint8_t *msg, size_t len, uint8_t *sig)
;
extern
int32_t ecp_eddsa_ed448_verify(const uint8_t *pub,
		const uint8_t *sig, const uint8_t *msg, size_t len)
;

/* nist_p256.c */
extern
int32_t ecp_ecdh_p256_public_key(const uint8_t *pri, uint8_t *pub)
;
extern
int32_t ecp_ecdh_p256_shared_key(const uint8_t *pri, const uint8_t *pub,
		uint8_t *key)
;
extern
int32_t ecp_ecdsa_p256_public_key(const uint8_t *pri, uint8_t *pub)
;
extern
int32_t ecp_ecdsa_p256_sign(const uint8_t *pri, const uint8_t *ran,
		const uint8_t *msg, size_t len, uint8_t *sig)
;
extern
int32_t ecp_ecdsa_p256_verify(const uint8_t *pub,
		const uint8_t *sig, const uint8_t *msg, size_t len)
;
extern
int32_t ecp_p256_check_scalar(const uint8_t *k)
;

/* nist_p384.c */
extern
int32_t ecp_ecdh_p384_public_key(const uint8_t *pri, uint8_t *pub)
;
extern
int32_t ecp_ecdh_p384_shared_key(const uint8_t *pri, const uint8_t *pub,
		uint8_t *key)
;
extern
int32_t ecp_ecdsa_p384_public_key(const uint8_t *pri, uint8_t *pub)
;
extern
int32_t ecp_ecdsa_p384_sign(const uint8_t *pri, const uint8_t *ran,
		const uint8_t *msg, size_t len, uint8_t *sig)
;
extern
int32_t ecp_ecdsa_p384_verify(const uint8_t *pub,
		const uint8_t *sig, const uint8_t *msg, size_t len)
;
extern
int32_t ecp_p384_check_scalar(const uint8_t *k)
;

/* nist_p521.c */
extern
int32_t ecp_ecdh_p521_public_key(const uint8_t *pri, uint8_t *pub)
;
extern
int32_t ecp_ecdh_p521_shared_key(const uint8_t *pri, const uint8_t *pub,
		uint8_t *key)
;
extern
int32_t ecp_ecdsa_p521_public_key(const uint8_t *pri, uint8_t *pub)
;
extern
int32_t ecp_ecdsa_p521_sign(const uint8_t *pri, const uint8_t *ran,
		const uint8_t *msg, size_t len, uint8_t *sig)
;
extern
int32_t ecp_ecdsa_p521_verify(const uint8_t *pub,
		const uint8_t *sig, const uint8_t *msg, size_t len)
;
extern
int32_t ecp_p521_check_scalar(const uint8_t *k)
;

#ifdef __cplusplus
}
#endif

#endif

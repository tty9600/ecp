/* @file: mldsa.h
 * #desc:
 *    The definitions of module-lattice-based digital signature
 *    standard (dilithium).
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

#ifndef _ECP_MLDSA_H
#define _ECP_MLDSA_H

#include <ecp/config.h>
#include <ecp/c/stddef.h>
#include <ecp/c/stdint.h>

/*
 * |   n   q       d  k ell eta tau beta gamma1 gamma2   omega pk   sk   sig |
 * |-------------------------------------------------------------------------|
 * |44 256 8380417 13 4 4   2   39  78   2^17   (q-1)/88 80    1132 2560 2420|
 * |65 256 8380417 13 6 5   4   49  196  2^19   (q-1)/32 55    1952 4032 3309|
 * |87 256 8380417 13 8 7   2   60  120  2^19   (q-1)/32 75    2592 4896 4627|
 */
#define MLDSA_N 256
#define MLDSA_Q 8380417 /* 2^23-2^13+1 */
#define MLDSA_ROOT 1753 /* 1753^(q-1) == 1 (mod q) */
#define MLDSA_D 13

#define MLDSA_SYM_LEN 32
#define MLDSA_RAN_LEN 32
#define MLDSA_POLYT1_BYTES 320
#define MLDSA_POLYT0_BYTES 416

/* mldsa-44 */
#define MLDSA_44_K 4
#define MLDSA_44_L 4
#define MLDSA_44_ETA 2
#define MLDSA_44_TAU 39
#define MLDSA_44_BETA 78
#define MLDSA_44_GAMMA1 (1 << 17)
#define MLDSA_44_GAMMA2 ((MLDSA_Q - 1) / 88)
#define MLDSA_44_OMEGA 80

#define MLDSA_44_C_BYTES 32
#define MLDSA_44_POLYZ_BYTES 576
#define MLDSA_44_POLYW1_BYTES 192
#define MLDSA_44_POLYETA_BYTES 96

#define MLDSA_44_POLYVEC_W1PACK_BYTES \
	(MLDSA_44_K * MLDSA_44_POLYW1_BYTES)

#define MLDSA_44_PK_LEN (MLDSA_SYM_LEN \
	+ MLDSA_44_K * MLDSA_POLYT1_BYTES)
#define MLDSA_44_SK_LEN (MLDSA_SYM_LEN * 4 \
	+ MLDSA_44_L * MLDSA_44_POLYETA_BYTES \
	+ MLDSA_44_K * MLDSA_44_POLYETA_BYTES \
	+ MLDSA_44_K * MLDSA_POLYT0_BYTES)
#define MLDSA_44_SIG_LEN (MLDSA_44_C_BYTES \
	+ MLDSA_44_L * MLDSA_44_POLYZ_BYTES \
	+ MLDSA_44_OMEGA + MLDSA_44_K)

/* mldsa-65 */
#define MLDSA_65_K 6
#define MLDSA_65_L 5
#define MLDSA_65_ETA 4
#define MLDSA_65_TAU 49
#define MLDSA_65_BETA 196
#define MLDSA_65_GAMMA1 (1 << 19)
#define MLDSA_65_GAMMA2 ((MLDSA_Q - 1) / 32)
#define MLDSA_65_OMEGA 55

#define MLDSA_65_C_BYTES 48
#define MLDSA_65_POLYZ_BYTES 640
#define MLDSA_65_POLYW1_BYTES 128
#define MLDSA_65_POLYETA_BYTES 128

#define MLDSA_65_POLYVEC_W1PACK_BYTES \
	(MLDSA_65_K * MLDSA_65_POLYW1_BYTES)

#define MLDSA_65_PK_LEN (MLDSA_SYM_LEN \
	+ MLDSA_65_K * MLDSA_POLYT1_BYTES)
#define MLDSA_65_SK_LEN (MLDSA_SYM_LEN * 4 \
	+ MLDSA_65_L * MLDSA_65_POLYETA_BYTES \
	+ MLDSA_65_K * MLDSA_65_POLYETA_BYTES \
	+ MLDSA_65_K * MLDSA_POLYT0_BYTES)
#define MLDSA_65_SIG_LEN (MLDSA_65_C_BYTES \
	+ MLDSA_65_L * MLDSA_65_POLYZ_BYTES \
	+ MLDSA_65_OMEGA + MLDSA_65_K)

/* mldsa-87 */
#define MLDSA_87_K 8
#define MLDSA_87_L 7
#define MLDSA_87_ETA 2
#define MLDSA_87_TAU 60
#define MLDSA_87_BETA 120
#define MLDSA_87_GAMMA1 (1 << 19)
#define MLDSA_87_GAMMA2 ((MLDSA_Q - 1) / 32)
#define MLDSA_87_OMEGA 75

#define MLDSA_87_C_BYTES 64
#define MLDSA_87_POLYZ_BYTES 640
#define MLDSA_87_POLYW1_BYTES 128
#define MLDSA_87_POLYETA_BYTES 96

#define MLDSA_87_POLYVEC_W1PACK_BYTES \
	(MLDSA_87_K * MLDSA_87_POLYW1_BYTES)

#define MLDSA_87_PK_LEN (MLDSA_SYM_LEN \
	+ MLDSA_87_K * MLDSA_POLYT1_BYTES)
#define MLDSA_87_SK_LEN (MLDSA_SYM_LEN * 4 \
	+ MLDSA_87_L * MLDSA_87_POLYETA_BYTES \
	+ MLDSA_87_K * MLDSA_87_POLYETA_BYTES \
	+ MLDSA_87_K * MLDSA_POLYT0_BYTES)
#define MLDSA_87_SIG_LEN (MLDSA_87_C_BYTES \
	+ MLDSA_87_L * MLDSA_87_POLYZ_BYTES \
	+ MLDSA_87_OMEGA + MLDSA_87_K)

#ifdef __cplusplus
extern "C" {
#endif

/* mldsa_44.c */
extern
void ecp_mldsa44_genkey(const uint8_t *ran, uint8_t *pk, uint8_t *sk)
;
extern
int32_t ecp_mldsa44_sign(const uint8_t *sk, const uint8_t *ran,
		const uint8_t *mctx, size_t mctx_len,
		const uint8_t *msg, size_t len, uint8_t *sig)
;
extern
int32_t ecp_mldsa44_verify(const uint8_t *pk, const uint8_t *sig,
		const uint8_t *mctx, size_t mctx_len,
		const uint8_t *msg, size_t len)
;

/* mldsa_65.c */
extern
void ecp_mldsa65_genkey(const uint8_t *ran, uint8_t *pk, uint8_t *sk)
;
extern
int32_t ecp_mldsa65_sign(const uint8_t *sk, const uint8_t *ran,
		const uint8_t *mctx, size_t mctx_len,
		const uint8_t *msg, size_t len, uint8_t *sig)
;
extern
int32_t ecp_mldsa65_verify(const uint8_t *pk, const uint8_t *sig,
		const uint8_t *mctx, size_t mctx_len,
		const uint8_t *msg, size_t len)
;

/* mldsa_87.c */
extern
void ecp_mldsa87_genkey(const uint8_t *ran, uint8_t *pk, uint8_t *sk)
;
extern
int32_t ecp_mldsa87_sign(const uint8_t *sk, const uint8_t *ran,
		const uint8_t *mctx, size_t mctx_len,
		const uint8_t *msg, size_t len, uint8_t *sig)
;
extern
int32_t ecp_mldsa87_verify(const uint8_t *pk, const uint8_t *sig,
		const uint8_t *mctx, size_t mctx_len,
		const uint8_t *msg, size_t len)
;

#ifdef __cplusplus
}
#endif

#endif

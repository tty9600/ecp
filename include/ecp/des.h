/* @file: des.h
 * #desc:
 *    The definitions of data encryption standard.
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

#ifndef _ECP_DES_H
#define _ECP_DES_H

#include <ecp/config.h>
#include <ecp/c/stdint.h>

#define DES_KEY_LEN 8
#define DES_BLOCKSIZE 8

struct des_ctx {
	uint8_t key_k[17][8];
	uint8_t key_c[17][4];
	uint8_t key_d[17][4];
};

#define DES_NEW(x) struct des_ctx x

#ifdef __cplusplus
extern "C" {
#endif

/* des.c */
extern
void ecp_des_init(struct des_ctx *ctx, const uint8_t *key)
;
extern
void ecp_des_crypto(struct des_ctx *ctx, uint8_t *buf, int32_t is_decrypt)
;

#ifdef __cplusplus
}
#endif

#endif

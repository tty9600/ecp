/* @file: totp.h
 * #desc:
 *    The definitions of time-based one-time password.
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

#ifndef _ECP_TOTP_H
#define _ECP_TOTP_H

#include <ecp/config.h>
#include <ecp/c/stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* totp_sha1.c */
extern
uint32_t ecp_totp_sha1(const uint8_t *key, uint32_t key_len,
		uint64_t time_base, uint32_t n)
;

/* totp_sha256.c */
extern
uint32_t ecp_totp_sha256(const uint8_t *key, uint32_t key_len,
		uint64_t time_base, uint32_t n)
;

/* totp_sha512.c */
extern
uint32_t ecp_totp_sha512(const uint8_t *key, uint32_t key_len,
		uint64_t time_base, uint32_t n)
;

#ifdef __cplusplus
}
#endif

#endif

/* @file: pw_bcrypt.h
 * #desc:
 *    The definitions of bcrypt password-hash.
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

#ifndef _ECP_PW_BCRYPT_H
#define _ECP_PW_BCRYPT_H

#include <ecp/config.h>
#include <ecp/c/stdint.h>

#define BCRYPT_HASHPASS_LEN 24

#ifdef __cplusplus
extern "C" {
#endif

/* pw_bcrypt.c */
extern
void ecp_bcrypt(const uint8_t *pass, uint32_t pass_len,
		const uint8_t *salt, uint32_t salt_len,
		uint8_t *ohp, uint32_t k)
;

#ifdef __cplusplus
}
#endif

#endif

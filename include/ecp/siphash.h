/* @file: siphash.h
 * #desc:
 *    The definitions of siphash secure pseudorandom function,
 *    or message authentication code.
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

#ifndef _ECP_SIPHASH_H
#define _ECP_SIPHASH_H

#include <ecp/config.h>
#include <ecp/c/stddef.h>
#include <ecp/c/stdint.h>

#define SIPHASH24_KEY_LEN 16

#ifdef __cplusplus
extern "C" {
#endif

/* siphash24.c */
extern
uint64_t ecp_siphash24(const uint8_t *s, size_t len, const uint8_t *key)
;
extern
uint64_t ecp_siphashx24(const uint8_t *s, size_t len, const uint8_t *key,
		uint64_t *h2)
;

#ifdef __cplusplus
}
#endif

#endif

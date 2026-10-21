/* @file: arpa_inet.h
 * #desc:
 *    The definitions of internet operations.
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

#ifndef _ECP_C_ARPA_INET_H
#define _ECP_C_ARPA_INET_H

#include <ecp/config.h>
#include <ecp/c/stdint.h>
#include <ecp/c/sys_socket.h>

#define ecp_htonl(x) \
	((((x) & 0xff) << 24) \
	| (((x) & 0xff00) << 8) \
	| (((x) & 0xff0000) >> 8) \
	| (((x) & 0xff000000) >> 24))
#define ecp_htons(x) \
	((((x) & 0xff) << 8) \
	| (((x) & 0xff00) >> 8))
#define ecp_ntohl(x) ecp_htonl(x)
#define ecp_ntohs(x) ecp_htons(x)

#ifdef __cplusplus
extern "C" {
#endif

/* c/arpa_inet.c */
extern
const char *ecp_inet_ntop(int32_t af, const void *src, char *dst,
		xsocklen_t len)
;
extern
int32_t ecp_inet_pton(int32_t af, const char *str, void *dst)
;

#ifdef __cplusplus
}
#endif

#endif

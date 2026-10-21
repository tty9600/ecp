/* @file: sys_socket.h
 * #desc:
 *    The definitions of main sockets.
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

#ifndef _ECP_C_SYS_SOCKET_H
#define _ECP_C_SYS_SOCKET_H

#include <ecp/config.h>
#include <ecp/c/stddef.h>
#include <ecp/c/stdint.h>

#if defined(ECP_PLATFORM_LINUX)

typedef uint32_t xsocklen_t;
typedef uint16_t xsa_family_t;

struct xsockaddr {
	xsa_family_t sa_family; /* address family */
	char sa_data[14];       /* socket address (variable-length data) */
};

#define X_SHUT_RD   0
#define X_SHUT_WR   1
#define X_SHUT_RDWR 2

#define X_SOCK_STREAM    1 /* byte-stream socket */
#define X_SOCK_DGRAM     2 /* datagram socket */
#define X_SOCK_RAW       3 /* raw protocol interface */
#define X_SOCK_SEQPACKET 5 /* sequenced-packet socket */

#define X_AF_LOCAL   1
#define X_AF_UNIX    X_AF_LOCAL
#define X_AF_FILE    X_AF_LOCAL
#define X_AF_INET    2
#define X_AF_AX25    3
#define X_AF_X25     9
#define X_AF_INET6   10
#define X_AF_NETLINK 16
#define X_AF_PACKET  17
#define X_AF_PPPOX   24
#define X_AF_WANPIPE 25
#define X_AF_LLC     26
#define X_AF_IB      27
#define X_AF_MPLS    28
#define X_AF_XDP     44

#else
# error "!!!unknown platform!!!"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* c/sys_socket.c */
extern
int32_t ecp_socket(int32_t domain, int32_t type, int32_t prot)
;
extern
int32_t ecp_socketpair(int32_t domain, int32_t type, int32_t prot,
		int32_t pair[2])
;
extern
int32_t ecp_bind(int32_t fd, const struct xsockaddr *addr,
		xsocklen_t addr_len)
;
extern
int32_t ecp_listen(int32_t fd, int32_t backlog)
;
extern
int32_t ecp_accept(int32_t fd, struct xsockaddr *addr, xsocklen_t *addr_len)
;
extern
int32_t ecp_connect(int32_t fd, const struct xsockaddr *addr,
		xsocklen_t addr_len)
;
extern
int32_t ecp_getpeername(int32_t fd, struct xsockaddr *addr,
		xsocklen_t *addr_len)
;
extern
int32_t ecp_getsockname(int32_t fd, struct xsockaddr *addr,
		xsocklen_t *addr_len)
;
extern
int32_t ecp_getsockopt(int32_t fd, int32_t lev, int32_t opt,
		void *optval, xsocklen_t *optlen)
;
extern
int32_t ecp_setsockopt(int32_t fd, int32_t lev, int32_t opt,
		const void *optval, xsocklen_t optlen)
;
extern
ssize_t ecp_recvfrom(int32_t fd, void *buf, size_t len, int flags,
		struct xsockaddr *addr, xsocklen_t *addr_len)
;
extern
ssize_t ecp_recv(int32_t fd, void *buf, size_t len, int32_t flags)
;
extern
ssize_t ecp_sendto(int32_t fd, const void *buf, size_t len, int32_t flags,
		const struct xsockaddr *addr, xsocklen_t addr_len)
;
extern
ssize_t ecp_send(int32_t fd, const void *buf, size_t len, int32_t flags)
;
extern
int32_t ecp_shutdown(int32_t fd, int32_t how)
;

#ifdef __cplusplus
}
#endif

#endif

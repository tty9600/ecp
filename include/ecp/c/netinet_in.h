/* @file: netinet_in.h
 * #desc:
 *    The definitions of internet address family.
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

#ifndef _ECP_C_NETINET_IN_H
#define _ECP_C_NETINET_IN_H

#include <ecp/config.h>
#include <ecp/c/stdint.h>
#include <ecp/c/sys_socket.h>

#if defined(ECP_PLATFORM_LINUX)

typedef uint16_t xin_port_t;
typedef uint32_t xin_addr_t;

struct xin_addr {
	xin_addr_t s_addr;
};

struct xsockaddr_in {
	xsa_family_t sin_family;
	xin_port_t sin_port;
	struct xin_addr sin_addr;
	uint8_t sin_zero[8];
};

struct xin6_addr {
	uint8_t s6_addr[16];
};

struct xsockaddr_in6 {
	xsa_family_t sin6_family;
	xin_port_t sin6_port;
	uint32_t sin6_flowinfo;
	struct xin6_addr sin6_addr;
	uint32_t sin6_scope_id;
};

#define IPPROTO_IP       0
#define IPPROTO_HOPOPTS  0
#define IPPROTO_ICMP     1
#define IPPROTO_IGMP     2
#define IPPROTO_IPIP     4
#define IPPROTO_TCP      6
#define IPPROTO_EGP      8
#define IPPROTO_PUP      12
#define IPPROTO_UDP      17
#define IPPROTO_IDP      22
#define IPPROTO_TP       29
#define IPPROTO_DCCP     33
#define IPPROTO_IPV6     41
#define IPPROTO_ROUTING  43
#define IPPROTO_FRAGMENT 44
#define IPPROTO_RSVP     46
#define IPPROTO_GRE      47
#define IPPROTO_ESP      50
#define IPPROTO_AH       51
#define IPPROTO_ICMPV6   58
#define IPPROTO_NONE     59
#define IPPROTO_DSTOPTS  60
#define IPPROTO_MTP      92
#define IPPROTO_BEETPH   94
#define IPPROTO_ENCAP    98
#define IPPROTO_PIM      103
#define IPPROTO_COMP     108
#define IPPROTO_SCTP     132
#define IPPROTO_MH       135
#define IPPROTO_UDPLITE  136
#define IPPROTO_MPLS     137
#define IPPROTO_ETHERNET 143
#define IPPROTO_RAW      255
#define IPPROTO_MPTCP    262

#else
# error "!!!unknown platform!!!"
#endif

#endif

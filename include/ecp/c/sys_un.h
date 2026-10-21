/* @file: sys_un.h
 * #desc:
 *    The definitions of unix domain sockets.
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

#ifndef _ECP_C_SYS_UN_H
#define _ECP_C_SYS_UN_H

#include <ecp/config.h>
#include <ecp/c/stdint.h>
#include <ecp/c/sys_socket.h>

#if defined(ECP_PLATFORM_LINUX)

struct xsockaddr_un {
	xsa_family_t sun_family;
	char sun_path[108];
};

#else
# error "!!!unknown platform!!!"
#endif

#endif

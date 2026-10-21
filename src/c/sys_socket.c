/* @file: sys_socket.c
 * #desc:
 *    The implementations of main sockets.
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

#include <ecp/config.h>
#include <ecp/c/stddef.h>
#include <ecp/c/stdint.h>
#include <ecp/c/errno.h>
#include <ecp/c/sys_socket.h>
#include <ecp/c/syscall.h>

/* @func: ecp_socket
 * #desc:
 *    create an endpoint for communication.
 *
 * #1: domain [in]  communication domain
 * #2: type   [in]  socket type
 * #3: prot   [in]  socket protocol
 * #r:        [ret] >=0: file descriptor, -1: errno
 */
int32_t ecp_socket(int32_t domain, int32_t type, int32_t prot)
{
#if defined(ECP_PLATFORM_LINUX)

	int32_t ret;

	ret = (int32_t)ecp_syscall_linux(__NR_socket,
		domain,
		type,
		prot);

	if (SYSCALL_LINUX_ISERR(ret)) {
		x_errno = -ret;
		return -1;
	}

	return ret;

#else
# error "!!!unknown platform!!!"
#endif
}

/* @func: ecp_socketpair
 * #desc:
 *    create a pair of connected sockets.
 *
 * #1: domain [in]  communication domain
 * #2: type   [in]  socket type
 * #3: prot   [in]  socket protocol
 * #4: pair   [out] socket pair a file descriptor
 * #r:        [ret] 0: no error, -1: errno
 */
int32_t ecp_socketpair(int32_t domain, int32_t type, int32_t prot,
		int32_t pair[2])
{
#if defined(ECP_PLATFORM_LINUX)

	int32_t ret;

	ret = (int32_t)ecp_syscall_linux(__NR_socketpair,
		domain,
		type,
		prot,
		pair);

	if (SYSCALL_LINUX_ISERR(ret)) {
		x_errno = -ret;
		return -1;
	}

	return ret;

#else
# error "!!!unknown platform!!!"
#endif
}

/* @func: ecp_bind
 * #desc:
 *    bind a name/address to a socket.
 *
 * #1: fd       [in]  file descriptor
 * #2: addr     [in]  socket address struct
 * #3: addr_len [in]  length of address struct
 * #r:          [ret] 0: no error, -1: errno
 */
int32_t ecp_bind(int32_t fd, const struct xsockaddr *addr,
		xsocklen_t addr_len)
{
#if defined(ECP_PLATFORM_LINUX)

	int32_t ret;

	ret = (int32_t)ecp_syscall_linux(__NR_bind,
		fd,
		addr,
		addr_len);

	if (SYSCALL_LINUX_ISERR(ret)) {
		x_errno = -ret;
		return -1;
	}

	return ret;

#else
# error "!!!unknown platform!!!"
#endif
}

/* @func: ecp_listen
 * #desc:
 *    listen for connections on a socket.
 *
 * #1: fd      [in]  file descriptor
 * #2: backlog [in]  max of pending connection queue
 * #r:         [ret] 0: no error, -1: errno
 */
int32_t ecp_listen(int32_t fd, int32_t backlog)
{
#if defined(ECP_PLATFORM_LINUX)

	int32_t ret;

	ret = (int32_t)ecp_syscall_linux(__NR_listen,
		fd,
		backlog);

	if (SYSCALL_LINUX_ISERR(ret)) {
		x_errno = -ret;
		return -1;
	}

	return ret;

#else
# error "!!!unknown platform!!!"
#endif
}

/* @func: ecp_accept
 * #desc:
 *    accept a connection on a socket.
 *
 * #1: fd       [in]     file descriptor
 * #2: addr     [out]    socket address struct
 * #3: addr_len [in/out] length of address struct and return the real-length
 * #r:          [ret]    0: no error, -1: errno
 */
int32_t ecp_accept(int32_t fd, struct xsockaddr *addr, xsocklen_t *addr_len)
{
#if defined(ECP_PLATFORM_LINUX)

	int32_t ret;

	ret = (int32_t)ecp_syscall_linux(__NR_accept4,
		fd,
		addr,
		addr_len,
		0);

	if (SYSCALL_LINUX_ISERR(ret)) {
		x_errno = -ret;
		return -1;
	}

	return ret;

#else
# error "!!!unknown platform!!!"
#endif
}

/* @func: ecp_connect
 * #desc:
 *    initiate a connection on a socket.
 *
 * #1: fd       [in]  file descriptor
 * #2: addr     [in]  socket address struct
 * #3: addr_len [in]  length of address struct
 * #r:          [ret] 0: no error, -1: errno
 */
int32_t ecp_connect(int32_t fd, const struct xsockaddr *addr,
		xsocklen_t addr_len)
{
#if defined(ECP_PLATFORM_LINUX)

	int32_t ret;

	ret = (int32_t)ecp_syscall_linux(__NR_connect,
		fd,
		addr,
		addr_len);

	if (SYSCALL_LINUX_ISERR(ret)) {
		x_errno = -ret;
		return -1;
	}

	return ret;

#else
# error "!!!unknown platform!!!"
#endif
}

/* @func: ecp_getpeername
 * #desc:
 *    get the name/address of the peer connection socket.
 *
 * #1: fd       [in]     file descriptor
 * #2: addr     [out]    socket address struct
 * #3: addr_len [in/out] length of address struct and return the real-length
 * #r:          [ret]    0: no error, -1: errno
 */
int32_t ecp_getpeername(int32_t fd, struct xsockaddr *addr,
		xsocklen_t *addr_len)
{
#if defined(ECP_PLATFORM_LINUX)

	int32_t ret;

	ret = (int32_t)ecp_syscall_linux(__NR_getpeername,
		fd,
		addr,
		addr_len);

	if (SYSCALL_LINUX_ISERR(ret)) {
		x_errno = -ret;
		return -1;
	}

	return ret;

#else
# error "!!!unknown platform!!!"
#endif
}

/* @func: ecp_getsockname
 * #desc:
 *    get the name/address of socket.
 *
 * #1: fd       [in]     file descriptor
 * #2: addr     [out]    socket address struct
 * #3: addr_len [in/out] length of address struct
 * #r:          [ret]     0: no error, -1: errno
 */
int32_t ecp_getsockname(int32_t fd, struct xsockaddr *addr,
		xsocklen_t *addr_len)
{
#if defined(ECP_PLATFORM_LINUX)

	int32_t ret;

	ret = (int32_t)ecp_syscall_linux(__NR_getsockname,
		fd,
		addr,
		addr_len);

	if (SYSCALL_LINUX_ISERR(ret)) {
		x_errno = -ret;
		return -1;
	}

	return ret;

#else
# error "!!!unknown platform!!!"
#endif
}

/* @func: ecp_getsockopt
 * #desc:
 *    get the socket options.
 *
 * #1: fd     [in]     file descriptor
 * #2: lev    [in]     protocol layer
 * #3: opt    [in]     option name
 * #4: optval [out]    output option buffer
 * #5: optlen [in/out] input length of option buffer and return the real-length
 * #r:        [ret] 0: no error, -1: errno
 */
int32_t ecp_getsockopt(int32_t fd, int32_t lev, int32_t opt,
		void *optval, xsocklen_t *optlen)
{
#if defined(ECP_PLATFORM_LINUX)

	int32_t ret;

	ret = (int32_t)ecp_syscall_linux(__NR_getsockopt,
		fd,
		lev,
		opt,
		optval,
		optlen);

	if (SYSCALL_LINUX_ISERR(ret)) {
		x_errno = -ret;
		return -1;
	}

	return ret;

#else
# error "!!!unknown platform!!!"
#endif
}

/* @func: ecp_setsockopt
 * #desc:
 *    set the socket options.
 *
 * #1: fd     [in]  file descriptor
 * #2: lev    [in]  protocol layer
 * #3: opt    [in]  option name
 * #4: optval [in]  input option buffer
 * #5: optlen [in]  input length of option buffer
 * #r:        [ret] 0: no error, -1: errno
 */
int32_t ecp_setsockopt(int32_t fd, int32_t lev, int32_t opt,
		const void *optval, xsocklen_t optlen)
{
#if defined(ECP_PLATFORM_LINUX)

	int32_t ret;

	ret = (int32_t)ecp_syscall_linux(__NR_setsockopt,
		fd,
		lev,
		opt,
		optval,
		optlen);

	if (SYSCALL_LINUX_ISERR(ret)) {
		x_errno = -ret;
		return -1;
	}

	return ret;

#else
# error "!!!unknown platform!!!"
#endif
}

/* @func: ecp_recvfrom
 * #desc:
 *    receive a message from a socket.
 *
 * #1: fd       [in]     file descriptor
 * #2: buf      [out]    output buffer
 * #3: len      [in]     buffer length
 * #4: flags    [in]     message flags
 * #5: addr     [out]    socket address struct
 * #6: addr_len [in/out] length of address struct and return the real-length
 * #r:          [ret]    >=0: length of receive, -1: errno
 */
ssize_t ecp_recvfrom(int32_t fd, void *buf, size_t len, int flags,
		struct xsockaddr *addr, xsocklen_t *addr_len)
{
#if defined(ECP_PLATFORM_LINUX)

	ssize_t ret;

	ret = (int32_t)ecp_syscall_linux(__NR_recvfrom,
		fd,
		buf,
		len,
		flags,
		addr,
		addr_len);

	if (SYSCALL_LINUX_ISERR(ret)) {
		x_errno = -(int32_t)ret;
		return -1;
	}

	return ret;

#else
# error "!!!unknown platform!!!"
#endif
}

/* @func: ecp_recv
 * #desc:
 *    receive a message from a socket.
 *
 * #1: fd       [in]  file descriptor
 * #2: buf      [out] output buffer
 * #3: len      [in]  buffer length
 * #4: flags    [in]  message flags
 * #r:          [ret] >=0: length of receive, -1: errno
 */
ssize_t ecp_recv(int32_t fd, void *buf, size_t len, int32_t flags)
{
	return ecp_recvfrom(fd, buf, len, flags, NULL, 0);
}

/* @func: ecp_sendto
 * #desc:
 *    send a message on a socket.
 *
 * #1: fd       [in]  file descriptor
 * #2: buf      [in]  input buffer
 * #3: len      [in]  buffer length
 * #4: flags    [in]  message flags
 * #5: addr     [in]  socket address struct
 * #6: addr_len [in]  length of address struct
 * #r:          [ret] >=0: length of send, -1: errno
 */
ssize_t ecp_sendto(int32_t fd, const void *buf, size_t len, int32_t flags,
		const struct xsockaddr *addr, xsocklen_t addr_len)
{
#if defined(ECP_PLATFORM_LINUX)

	ssize_t ret;

	ret = (int32_t)ecp_syscall_linux(__NR_sendto,
		fd,
		buf,
		len,
		flags,
		addr,
		addr_len);

	if (SYSCALL_LINUX_ISERR(ret)) {
		x_errno = -(int32_t)ret;
		return -1;
	}

	return ret;

#else
# error "!!!unknown platform!!!"
#endif
}

/* @func: ecp_send
 * #desc:
 *    send a message on a socket.
 *
 * #1: fd       [in]  file descriptor
 * #2: buf      [in]  input buffer
 * #3: len      [in]  buffer length
 * #4: flags    [in]  message flags
 * #r:          [ret] >=0: length of send, -1: errno
 */
ssize_t ecp_send(int32_t fd, const void *buf, size_t len, int32_t flags)
{
	return ecp_sendto(fd, buf, len, flags, NULL, 0);
}

/* @func: ecp_shutdown
 * #desc:
 *    shut down part of a full-duplex connection.
 *
 * #1: fd  [in]  file descriptor
 * #2: how [in]  shut down option
 * #r:     [ret] 0: no error, -1: errno
 */
int32_t ecp_shutdown(int32_t fd, int32_t how)
{
#if defined(ECP_PLATFORM_LINUX)

	int32_t ret;

	ret = (int32_t)ecp_syscall_linux(__NR_shutdown,
		fd,
		how);

	if (SYSCALL_LINUX_ISERR(ret)) {
		x_errno = -ret;
		return -1;
	}

	return ret;

#else
# error "!!!unknown platform!!!"
#endif
}

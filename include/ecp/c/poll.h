/* @file: poll.h
 * #desc:
 *    The definitions of synchronous i/o multiplexing.
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

#ifndef _ECP_C_POLL_H
#define _ECP_C_POLL_H

#include <ecp/config.h>
#include <ecp/c/stdint.h>
#include <ecp/c/time.h>
#include <ecp/c/signal.h>

#if defined(ECP_PLATFORM_LINUX)

#define X_POLLIN   0x01 /* data other than high-priority data may be read without blocking */
#define X_POLLPRI  0x02 /* high priority data may be read without blocking */
#define X_POLLOUT  0x04 /* normal data may be written without blocking */
#define X_POLLERR  0x08 /* an error has occurred (revents only) */
#define X_POLLHUP  0x10 /* device has been disconnected (revents only) */
#define X_POLLNVAL 0x20 /* invalid fd member (revents only) */

#define X_POLLRDNORM 0x40  /* normal data may be read without blocking */
#define X_POLLRDBAND 0x80  /* priority data may be read without blocking */
#define X_POLLWRNORM 0x100 /* equivalent to POLLOUT */
#define X_POLLWRBAND 0x200 /* priority data may be written */

struct xpollfd {
	int32_t fd;
	int16_t events;
	int16_t revents;
};

#else
# error "!!!unknown platform!!!"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* c/poll.c */
extern
int32_t ecp_ppoll(struct xpollfd *fds, int32_t nfds,
		const struct xtimespec *ts, const xsigset_t *mask)
;
extern
int32_t ecp_poll(struct xpollfd *fds, int32_t nfds, int32_t ms)
;

#ifdef __cplusplus
}
#endif

#endif

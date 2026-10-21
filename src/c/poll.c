/* @file: poll.c
 * #desc:
 *    the implementations of synchronous i/o multiplexing.
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
#include <ecp/c/sys_types.h>
#include <ecp/c/poll.h>
#include <ecp/c/time.h>
#include <ecp/c/signal.h>
#include <ecp/c/syscall.h>

/* @func: ecp_ppoll
 * #desc:
 *    synchronous i/o multiplexing (pending signal).
 *
 * #1: fds  [in/out] pollfd array
 * #2: nfds [in]     number of pollfd
 * #3: ts   [in]     block waiting time / NULL
 * #4: mask [in]     mask signal set
 * #r:      [ret]    >0: number of ready fds, 0: timeout, -1: errno
 */
int32_t ecp_ppoll(struct xpollfd *fds, int32_t nfds,
		const struct xtimespec *ts, const xsigset_t *mask)
{
#if defined(ECP_PLATFORM_LINUX)

	int32_t ret;

	ret = (int32_t)ecp_syscall_linux(__NR_ppoll,
		fds,
		nfds,
		ts,
		mask,
		sizeof(xsigset_t));

	if (SYSCALL_LINUX_ISERR(ret)) {
		x_errno = -ret;
		return -1;
	}

	return ret;

#else
# error "!!!unknown platform!!!"
#endif
}

/* @func: ecp_poll
 * #desc:
 *    synchronous i/o multiplexing.
 *
 * #1: fds  [in/out] pollfd array
 * #2: nfds [in]     number of pollfd
 * #3: ms   [in]     block waiting time / NULL
 * #r:      [ret]    >0: number of ready fds, 0: timeout, -1: errno
 */
int32_t ecp_poll(struct xpollfd *fds, int32_t nfds, int32_t ms)
{
	struct xtimespec ts;

	if (ms >= 0) {
		ts.tv_sec = ms / 1000;
		ts.tv_nsec = (ms % 1000) * 1000000;
	}

	return ecp_ppoll(fds, nfds, (ms < 0) ? NULL : &ts, NULL);
}

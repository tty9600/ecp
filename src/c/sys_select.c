/* @file: sys_select.c
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
#include <ecp/c/sys_select.h>
#include <ecp/c/time.h>
#include <ecp/c/signal.h>
#include <ecp/c/syscall.h>

/* @func: ecp_pselect
 * #desc:
 *    synchronous i/o multiplexing (pending signal).
 *
 * #1: nfds [in]     max file descriptor (+1)
 * #2: rfds [in/out] read fd set / NULL
 * #3: wfds [in/out] write fd set / NULL
 * #4: efds [in/out] except fd set / NULL
 * #5: ts   [in]     block waiting time / NULL
 * #6: mask [in]     mask signal set
 * #r:      [ret]    >0: number of ready fds, 0: timeout, -1: errno
 */
int32_t ecp_pselect(int32_t nfds, xfd_set *rfds, xfd_set *wfds, xfd_set *efds,
		const struct xtimespec *ts, const xsigset_t *mask)
{
#if defined(ECP_PLATFORM_LINUX)

	int32_t ret;

	ret = (int32_t)ecp_syscall_linux(__NR_pselect6,
		nfds,
		rfds,
		wfds,
		efds,
		ts,
		mask);

	if (SYSCALL_LINUX_ISERR(ret)) {
		x_errno = -ret;
		return -1;
	}

	return ret;

#else
# error "!!!unknown platform!!!"
#endif
}

/* @func: ecp_select
 * #desc:
 *    synchronous i/o multiplexing.
 *
 * #1: nfds [in]     max file descriptor (+1)
 * #2: rfds [in/out] read fd set / NULL
 * #3: wfds [in/out] write fd set / NULL
 * #4: efds [in/out] except fd set / NULL
 * #5: tv   [in]     block waiting time / NULL
 * #r:      [ret]    >0: number of ready fds, 0: timeout, -1: errno
 */
int32_t ecp_select(int32_t nfds, xfd_set *rfds, xfd_set *wfds, xfd_set *efds,
		struct xtimeval *tv)
{
	struct xtimespec ts;

	if (tv) {
		if (tv->tv_sec < 0 || tv->tv_usec < 0) {
			x_errno = X_EINVAL;
			return -1;
		}

		ts.tv_sec = tv->tv_sec + (tv->tv_usec / 1000000);
		ts.tv_nsec = (tv->tv_usec % 1000000) * 1000;
	}

	return ecp_pselect(nfds, rfds, wfds, efds, tv ? &ts : NULL, NULL);
}

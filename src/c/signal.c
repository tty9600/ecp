/* @file: signal.c
 * #desc:
 *    The implementations of posix signals.
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
#include <ecp/c/string.h>
#include <ecp/c/errno.h>
#include <ecp/c/signal.h>
#include <ecp/c/syscall.h>

/* @func: ecp_kill
 * #desc:
 *    send a signal to process or process group.
 *
 * #1: pid [in]  process id (0,-pid: process group)
 * #2: sig [in]  signal number
 * #r:     [ret] 0: no error, -1: errno
 */
int32_t ecp_kill(int32_t pid, int32_t sig)
{
#if defined(ECP_PLATFORM_LINUX)

	int32_t ret;

	ret = (int32_t)ecp_syscall_linux(__NR_kill,
		pid,
		sig);

	if (SYSCALL_LINUX_ISERR(ret)) {
		x_errno = -ret;
		return -1;
	}

	return ret;

#else
# error "!!!unknown platform!!!"
#endif
}

/* @func: ecp_killpg
 * #desc:
 *    send a signal to process group.
 *
 * #1: pg  [in]  process id
 * #2: sig [in]  signal number
 * #r:     [ret] 0: no error, -1: errno
 */
int32_t ecp_killpg(int32_t pg, int32_t sig)
{
	return ecp_kill(-pg, sig);
}

/* @func: ecp_sigaction
 * #desc:
 *    examine and change a signal action.
 *
 * #1: sig [in]  signal number
 * #2: act [in]  new action / NULL
 * #3: old [out] old action / NULL
 * #r:     [ret] 0: no error, -1: errno
 */
int32_t ecp_sigaction(int32_t sig, const struct xsigaction *act,
		struct xsigaction *old)
{
#if defined(ECP_PLATFORM_LINUX)

	int32_t ret;

	ret = (int32_t)ecp_syscall_linux(__NR_rt_sigaction,
		sig,
		act,
		old,
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

/* @func: ecp_sigprocmask
 * #desc:
 *    examine and change blocked signals.
 *
 * #1: how [in]  behavior type
 * #2: set [in]  new signal set / NULL
 * #3: old [out] old signal set / NULL
 * #r:     [ret] 0: no error, -1: errno
 */
int32_t ecp_sigprocmask(int32_t how, const xsigset_t *set,
		xsigset_t *old)
{
#if defined(ECP_PLATFORM_LINUX)

	int32_t ret;

	ret = (int32_t)ecp_syscall_linux(__NR_rt_sigprocmask,
		how,
		set,
		old,
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

/* @func: ecp_sigpending
 * #desc:
 *    get the set of signals waiting.
 *
 * #1: set [in]  signal set
 * #r:     [ret] 0: no error, -1: errno
 */
int32_t ecp_sigpending(xsigset_t *set)
{
#if defined(ECP_PLATFORM_LINUX)

	int32_t ret;

	ret = (int32_t)ecp_syscall_linux(__NR_rt_sigpending,
		set,
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

/* @func: ecp_sigsuspend
 * #desc:
 *    replace the signal set and wait for signal (atomic safe).
 *
 * #1: set [in]  signal set
 * #r:     [ret] 0: no error, -1: errno
 */
int32_t ecp_sigsuspend(const xsigset_t *set)
{
#if defined(ECP_PLATFORM_LINUX)

	int32_t ret;

	ret = (int32_t)ecp_syscall_linux(__NR_rt_sigsuspend,
		set,
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

/* @func: ecp_sigaddset
 * #desc:
 *    added a signal to signal set.
 *
 * #1: set [in]  signal set
 * #2: sig [in]  signal number
 * #r:     [ret] 0: no error, -1: errno
 */
int32_t ecp_sigaddset(xsigset_t *set, int32_t sig)
{
	if (sig <= 0 || sig > X_NSIG) {
		x_errno = -X_EINVAL;
		return -1;
	}

	sig--;
	((uint8_t *)set)[sig / 8] |= 1U << (sig % 8);

	return 0;
}

/* @func: ecp_sigdelset
 * #desc:
 *    delete a signal from a signal set.
 *
 * #1: set [in]  signal set
 * #2: sig [in]  signal number
 * #r:     [ret] 0: no error, -1: errno
 */
int32_t ecp_sigdelset(xsigset_t *set, int32_t sig)
{
	if (sig <= 0 || sig > X_NSIG) {
		x_errno = -X_EINVAL;
		return -1;
	}

	sig--;
	((uint8_t *)set)[sig / 8] &= ~(1U << (sig % 8));

	return 0;
}

/* @func: ecp_sigismember
 * #desc:
 *    check the signal in the signal set.
 *
 * #1: set [in]  signal set
 * #2: sig [in]  signal number
 * #r:     [ret] 0: false, 1: true, -1: errno
 */
int32_t ecp_sigismember(const xsigset_t *set, int32_t sig)
{
	if (sig <= 0 || sig > X_NSIG) {
		x_errno = -X_EINVAL;
		return -1;
	}

	sig--;
	if (((uint8_t *)set)[sig / 8] & (1U << (sig % 8)))
		return 1;

	return 0;
}

/* @func: ecp_sigemptyset
 * #desc:
 *    init signal set.
 *
 * #1: set [in]  signal set
 * #r:     [ret] 0: no error
 */
int32_t ecp_sigemptyset(xsigset_t *set)
{
	ecp_memset(set, 0, sizeof(xsigset_t));

	return 0;
}

/* @func: ecp_sigfillset
 * #desc:
 *    filling signal set.
 *
 * #1: set [in]  signal set
 * #r:     [ret] 0: no error, -1: errno
 */
int32_t ecp_sigfillset(xsigset_t *set)
{
	ecp_memset(set, 0xff, sizeof(xsigset_t));

	if (ecp_sigdelset(set, X_SIGHUP))
		return -1;
	if (ecp_sigdelset(set, X_SIGINT))
		return -1;

	return 0;
}

/* @func: ecp_signal
 * #desc:
 *    set signal callback.
 *
 * #1: sig     [in]  signal number
 * #2: handler [in]  callback handler
 * #r:         [ret] old handler (X_SIG_ERR: errno)
 */
void (*ecp_signal(int32_t sig, void (*handler)(int32_t)))(int32_t)
{
	struct xsigaction act, old;
	act.xsa_handler = handler;
	act.sa_flags = X_SA_RESTART;
	ecp_sigemptyset(&act.sa_mask);

	if (ecp_sigaction(sig, &act, &old))
		return X_SIG_ERR;

	return old.xsa_handler;
}

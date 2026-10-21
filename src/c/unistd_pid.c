/* @file: unistd_pid.c
 * #desc:
 *    The implementations of unix standard.
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
#include <ecp/c/stdint.h>
#include <ecp/c/sys_types.h>
#include <ecp/c/errno.h>
#include <ecp/c/unistd.h>
#include <ecp/c/syscall.h>

/* @func: ecp_getpid
 * #desc:
 *    get the process id.
 *
 * #r: [ret] process id
 */
xpid_t ecp_getpid(void)
{
#if defined(ECP_PLATFORM_LINUX)

	return (xpid_t)ecp_syscall_linux(__NR_getpid);

#else
# error "!!!unknown platform!!!"
#endif
}

/* @func: ecp_getppid
 * #desc:
 *    get the parent process id of the process.
 *
 * #r: [ret] parent process id
 */
xpid_t ecp_getppid(void)
{
#if defined(ECP_PLATFORM_LINUX)

	return (xpid_t)ecp_syscall_linux(__NR_getppid);;

#else
# error "!!!unknown platform!!!"
#endif
}

/* @func: ecp_getpgid
 * #desc:
 *    get the process group id of the process.
 *
 * #1: pid [in]  process id
 * #r:     [ret] >=0: process group id, -1: errno
 */
xpid_t ecp_getpgid(xpid_t pid)
{
#if defined(ECP_PLATFORM_LINUX)

	xpid_t ret;

	ret = (xpid_t)ecp_syscall_linux(__NR_getpgid,
		pid);

	if (SYSCALL_LINUX_ISERR(ret)) {
		x_errno = -ret;
		return -1;
	}

	return ret;

#else
# error "!!!unknown platform!!!"
#endif
}

/* @func: ecp_setpgid
 * #desc:
 *    set the process group id of the process.
 *
 * #1: pid [in]  process id
 * #2: pg  [in]  process group id
 * #r:     [ret] 0: no error, -1: errno
 */
int32_t ecp_setpgid(xpid_t pid, xpid_t pg)
{
#if defined(ECP_PLATFORM_LINUX)

	int32_t ret;

	ret = (int32_t)ecp_syscall_linux(__NR_setpgid,
		pid,
		pg);

	if (SYSCALL_LINUX_ISERR(ret)) {
		x_errno = -ret;
		return -1;
	}

	return ret;

#else
# error "!!!unknown platform!!!"
#endif
}

/* @func: ecp_getuid
 * #desc:
 *    get the user id of the process.
 *
 * #r: [ret] user id
 */
xuid_t ecp_getuid(void)
{
#if defined(ECP_PLATFORM_LINUX)

	return (xuid_t)ecp_syscall_linux(__NR_getuid);

#else
# error "!!!unknown platform!!!"
#endif
}

/* @func: ecp_geteuid
 * #desc:
 *    get the effective user id of the process.
 *
 * #r: [ret] effective user id
 */
xuid_t ecp_geteuid(void)
{
#if defined(ECP_PLATFORM_LINUX)

	return (xuid_t)ecp_syscall_linux(__NR_geteuid);

#else
# error "!!!unknown platform!!!"
#endif
}

/* @func: ecp_setuid
 * #desc:
 *    set the user id of the process.
 *
 * #1: uid [in]  user id
 * #r:     [ret] 0: no error, -1: errno
 */
int32_t ecp_setuid(xuid_t uid)
{
#if defined(ECP_PLATFORM_LINUX)

	int32_t ret;

	ret = (int32_t)ecp_syscall_linux(__NR_setuid,
		uid);

	if (SYSCALL_LINUX_ISERR(ret)) {
		x_errno = -ret;
		return -1;
	}

	return ret;

#else
# error "!!!unknown platform!!!"
#endif
}

/* @func: ecp_seteuid
 * #desc:
 *    set the effective user id of the process.
 *
 * #1: euid [in]  effective user id
 * #r:      [ret] 0: no error, -1: errno
 */
int32_t ecp_seteuid(xuid_t euid)
{
#if defined(ECP_PLATFORM_LINUX)

	int32_t ret;

	ret = (int32_t)ecp_syscall_linux(__NR_setresuid,
		-1,
		euid,
		-1);

	if (SYSCALL_LINUX_ISERR(ret)) {
		x_errno = -ret;
		return -1;
	}

	return ret;

#else
# error "!!!unknown platform!!!"
#endif
}

/* @func: ecp_getgid
 * #desc:
 *    get the group id of the process.
 *
 * #r: [ret] group id
 */
xgid_t ecp_getgid(void)
{
#if defined(ECP_PLATFORM_LINUX)

	return (xgid_t)ecp_syscall_linux(__NR_getgid);

#else
# error "!!!unknown platform!!!"
#endif
}

/* @func: ecp_getegid
 * #desc:
 *    get the effective group id of the process.
 *
 * #r: [ret] effective group id
 */
xgid_t ecp_getegid(void)
{
#if defined(ECP_PLATFORM_LINUX)

	return (xgid_t)ecp_syscall_linux(__NR_getegid);

#else
# error "!!!unknown platform!!!"
#endif
}

/* @func: ecp_setgid
 * #desc:
 *    set the group id of the process.
 *
 * #1: gid [in]  group id
 * #r:     [ret] 0: no error, -1: errno
 */
int32_t ecp_setgid(xgid_t gid)
{
#if defined(ECP_PLATFORM_LINUX)

	int32_t ret;

	ret = (int32_t)ecp_syscall_linux(__NR_setgid,
		gid);

	if (SYSCALL_LINUX_ISERR(ret)) {
		x_errno = -ret;
		return -1;
	}

	return ret;

#else
# error "!!!unknown platform!!!"
#endif
}

/* @func: ecp_setegid
 * #desc:
 *    set the effective group id of the process.
 *
 * #1: egid [in]  effective group id
 * #r:      [ret] 0: no error, -1: errno
 */
int32_t ecp_setegid(xgid_t egid)
{
#if defined(ECP_PLATFORM_LINUX)

	int32_t ret;

	ret = (int32_t)ecp_syscall_linux(__NR_setresgid,
		-1,
		egid,
		-1);

	if (SYSCALL_LINUX_ISERR(ret)) {
		x_errno = -ret;
		return -1;
	}

	return ret;

#else
# error "!!!unknown platform!!!"
#endif
}

/* @func: ecp_getgroups
 * #desc:
 *    get the process related group id.
 *
 * #1: size [in]  array size number (0: returns the gids number)
 * #2: gids [out] array of groups id
 * #r:      [ret] >=0: gids number, -1: errno
 */
int32_t ecp_getgroups(int32_t size, xgid_t gids[])
{
#if defined(ECP_PLATFORM_LINUX)

	int32_t ret;

	ret = (int32_t)ecp_syscall_linux(__NR_getgroups,
		size,
		gids);

	if (SYSCALL_LINUX_ISERR(ret)) {
		x_errno = -ret;
		return -1;
	}

	return ret;

#else
# error "!!!unknown platform!!!"
#endif
}

/* @func: ecp_setgroups
 * #desc:
 *    set the related group id of the process.
 *
 * #1: size [in]  array size number
 * #2: gids [in]  array of groups id
 * #r:      [ret] 0: no error, -1: errno
 */
int32_t ecp_setgroups(int32_t size, xgid_t gids[])
{
#if defined(ECP_PLATFORM_LINUX)

	int32_t ret;

	ret = (int32_t)ecp_syscall_linux(__NR_setgroups,
		size,
		gids);

	if (SYSCALL_LINUX_ISERR(ret)) {
		x_errno = -ret;
		return -1;
	}

	return ret;

#else
# error "!!!unknown platform!!!"
#endif
}

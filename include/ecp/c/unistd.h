/* @file: unistd.h
 * #desc:
 *    The definitions of unix standard.
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

#ifndef _ECP_C_UNISTD_H
#define _ECP_C_UNISTD_H

#include <ecp/config.h>
#include <ecp/c/stddef.h>
#include <ecp/c/stdint.h>
#include <ecp/c/sys_types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* c/unistd_fork.c */
extern
int32_t ecp_fork(void)
;

/* c/unistd_fs.c */
extern
ssize_t ecp_read(int32_t fd, void *buf, size_t len)
;
extern
ssize_t ecp_write(int32_t fd, const void *buf, size_t len)
;
extern
int64_t ecp_lseek(int32_t fd, int64_t off, int32_t whence)
;
extern
int32_t ecp_close(int32_t fd)
;

/* c/unistd_pid.c */
extern
xpid_t ecp_getpid(void)
;
extern
xpid_t ecp_getppid(void)
;
extern
xpid_t ecp_getpgid(xpid_t pid)
;
extern
int32_t ecp_setpgid(xpid_t pid, xpid_t pg)
;
extern
xuid_t ecp_getuid(void)
;
extern
xuid_t ecp_geteuid(void)
;
extern
int32_t ecp_setuid(xuid_t uid)
;
extern
int32_t ecp_seteuid(xuid_t euid)
;
extern
xgid_t ecp_getgid(void)
;
extern
xgid_t ecp_getegid(void)
;
extern
int32_t ecp_setgid(xgid_t gid)
;
extern
int32_t ecp_setegid(xgid_t egid)
;
extern
int32_t ecp_getgroups(int32_t size, xgid_t gids[])
;
extern
int32_t ecp_setgroups(int32_t size, xgid_t gids[])
;

/* c/unistd_sleep.c */
extern
int32_t ecp_sleep(uint32_t n)
;

#ifdef __cplusplus
}
#endif

#endif

/* @file: sys_wait.h
 * #desc:
 *    The definitions of wait for child process to change state.
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

#ifndef _ECP_C_SYS_WAIT_H
#define _ECP_C_SYS_WAIT_H

#include <ecp/config.h>
#include <ecp/c/stdint.h>

#if defined(ECP_PLATFORM_LINUX)

#define X_WNOHANG 1
#define X_WUNTRACED 2

#define X_WSTOPPED 2
#define X_WEXITED 4
#define X_WCONTINUED 8
#define X_WNOWAIT 0x1000000

#define X_WEXITSTATUS(x) (((x) & 0xff00) >> 8)
#define X_WTERMSIG(x) ((x) & 0x7f)

#define X_WSTOPSIG(x) X_WEXITSTATUS(x)
#define X_WCOREDUMP(x) ((x) & 0x80)
#define X_WIFEXITED(x) (!X_WTERMSIG(x))
#define X_WIFSTOPPED(x) \
	(((((x) & 0xffff) * 0x10001) >> 8) > 0x7f00)
#define X_WIFSIGNALED(x) ((((x) & 0xffff) - 1) < 0xff)
#define X_WIFCONTINUED(x) ((x) == 0xffff)

#else
# error "!!!unknown platform!!!"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* c/sys_wait.c */
extern
int32_t ecp_waitpid(int32_t pid, int32_t *wstatus, int32_t option)
;
extern
int32_t ecp_wait(int32_t *wstatus)
;

#ifdef __cplusplus
}
#endif

#endif

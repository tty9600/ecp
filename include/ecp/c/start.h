/* @file: start.h
 * #desc:
 *    The definitions of C runtime entry.
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

#ifndef _ECP_C_START_H
#define _ECP_C_START_H

#include <ecp/config.h>

#ifdef __cplusplus
extern "C" {
#endif

/* c/start_main.c */
extern
char **__ecp_envp
;
extern
char **__ecp_auxp
;
extern
void _start_main(long *sp)
;

/* c/start_vdso.c */
extern
void *ecp_vdso_sym(const char *sym)
;

#ifdef __cplusplus
}
#endif

#endif

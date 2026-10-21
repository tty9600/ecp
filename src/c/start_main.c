/* @file: c_start_main.c
 * #desc:
 *    The implementations of runtime entry.
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
#include <ecp/c/stdlib.h>
#include <ecp/c/start.h>

/* @fvar: __ecp_envp
 */
char **__ecp_envp = NULL;
/* @fvar: __ecp_auxp
 */
char **__ecp_auxp = NULL;

extern
int main(int argc, char *const *argv, char *const *envp)
;

/* @func: _start_main
 * #desc:
 *    c runtime entry.
 *
 * #1: sp [in/out] stack pointer
 */
void _start_main(long *sp)
{
	int argc = (int)sp[0];
	char **argv = (char **)&sp[1];
	__ecp_envp = (char **)&sp[argc + 2];

	__ecp_auxp = __ecp_envp;
	for (; __ecp_auxp[0]; __ecp_auxp++);
	__ecp_auxp++;

	int r = main(argc, argv, __ecp_envp);

	ecp_Exit(r);
}

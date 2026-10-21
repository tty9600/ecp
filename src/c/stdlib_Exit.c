/* @file: stdlib_Exit.c
 * #desc:
 *    The implementations of standard library.
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
#include <ecp/c/stdlib.h>
#include <ecp/c/syscall.h>

/* @func: ecp_Exit
 * #desc:
 *    terminate the calling process.
 *
 * #1: status [in] exit status
 */
void ecp_Exit(int32_t status)
{
#if defined(ECP_PLATFORM_LINUX)

	ecp_syscall_linux(__NR_exit_group,
		status);

	while (1) {
		ecp_syscall_linux(__NR_exit,
			status);
	}

#else
# error "!!!unknown platform!!!"
#endif
}

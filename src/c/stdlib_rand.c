/* @file: stdlib_rand.c
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

/* @func: ecp_rand_r
 * #desc:
 *    pseudo-random number generator.
 *
 * #1: state [in/out] seed state
 * #r:       [ret]    random number
 */
int32_t ecp_rand_r(int32_t *state)
{
	int32_t ret;

	/* LCG (Linear Congruential Generator) */
	ret = ((*state * 1103515245) + 12345) & INT32_MAX;
	*state = ret;

	return ret;
}

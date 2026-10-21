/* @file: getopt.h
 * #desc:
 *    The definitions of command-line options parse.
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

#ifndef _ECP_C_GETOPT_H
#define _ECP_C_GETOPT_H

#include <ecp/config.h>
#include <ecp/c/stdint.h>

#define NO_ARGUMENT 0
#define REQUIRED_ARGUMENT 1
#define OPTIONAL_ARGUMENT 2

struct option_r {
	const char *name;
	int32_t has_arg;
	int32_t *flag;
	int32_t val;
};

#define OPT_ARGC(arg, r) ((arg) ? *(arg) : (r))

#ifdef __cplusplus
extern "C" {
#endif

/* c/getopt.c */
extern
int32_t ecp_getopt_r(int32_t argc, char *const *argv,
		const char *optstr, char **arg, int32_t *ind)
;
extern
int32_t ecp_getopt_long_r(int32_t argc, char *const *argv,
		const char *optstr, const struct option_r *longopts,
		int32_t *longind, char **arg, int32_t *ind)
;

#ifdef __cplusplus
}
#endif

#endif

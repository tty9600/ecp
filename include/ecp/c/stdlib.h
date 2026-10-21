/* @file: stdlib.h
 * #desc:
 *    The definitions of standard library.
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

#ifndef _ECP_C_STDLIB_H
#define _ECP_C_STDLIB_H

#include <ecp/config.h>
#include <ecp/c/stddef.h>
#include <ecp/c/stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* c/stdlib_Exit.c */
extern
void ecp_Exit(int32_t status)
;

/* c/stdlib_abort.c */
extern
void ecp_abort(void)
;

/* c/stdlib_abs.c */
extern
int ecp_abs(int n)
;
extern
long ecp_labs(long n)
;
extern
long long ecp_llabs(long long n)
;

/* c/stdlib_atof.c */
extern
double ecp_atof(const char *s)
;
extern
double ecp_strtod(const char *s, char **e)
;

/* c/stdlib_atoi.c */
extern
uint64_t __ecp_strtoull(const char *s, char **e, uint64_t *m, int32_t b)
;
extern
int ecp_atoi(const char *s)
;
extern
long ecp_atol(const char *s)
;
extern
long long ecp_atoll(const char *s)
;
extern
long ecp_strtol(const char *s, char **e, int32_t b)
;
extern
unsigned long ecp_strtoul(const char *s, char **e, int32_t b)
;
extern
long long ecp_strtoll(const char *s, char **e, int32_t b)
;
extern
unsigned long long ecp_strtoull(const char *s, char **e, int32_t b)
;

/* c/stdlib_exit.c */
extern
int32_t ecp_atexit(void (*func)(void))
;
extern
void ecp_exit(int32_t status)
;

/* c/stdlib_malloc.c */
extern
void *ecp_malloc(size_t size)
;
extern
void ecp_free(void *p)
;

/* c/stdlib_qsort.c */
extern
void ecp_qsort(void *b, size_t n, size_t w,
		int32_t (*cmp)(const void *, const void *))
;
extern
void *ecp_bsearch(const void *k, const void *b, size_t n, size_t w,
		int32_t (*cmp)(const void *, const void *))
;

/* c/stdlib_rand.c */
extern
int32_t ecp_rand_r(int32_t *state)
;

#ifdef __cplusplus
}
#endif

#endif

/* @file: string.h
 * #desc:
 *    The definitions of string operations.
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

#ifndef _ECP_C_STRING_H
#define _ECP_C_STRING_H

#include <ecp/config.h>
#include <ecp/c/stddef.h>
#include <ecp/c/stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* c/string.c */
extern
void *ecp_memcpy(void *t, const void *s, size_t len)
;
extern
void *ecp_memmove(void *t, const void *s, size_t len)
;
extern
void *ecp_memset(void *t, int32_t c, size_t len)
;
extern
void *ecp_memchr(const void *s, int32_t c, size_t len)
;
extern
void *ecp_memrchr(const void *s, int32_t c, size_t len)
;
extern
int32_t ecp_memcmp(const void *s1, const void *s2, size_t len)
;
extern
size_t ecp_strlen(const char *s)
;
extern
size_t ecp_strnlen(const char *s, size_t len)
;
extern
char *ecp_strcpy(char *t, const char *s)
;
extern
char *ecp_strncpy(char *t, const char *s, size_t len)
;
extern
char *ecp_strcat(char *t, const char *s)
;
extern
char *ecp_strncat(char *t, const char *s, size_t len)
;
extern
char *ecp_strchr(const char *s, int32_t c)
;
extern
char *ecp_strrchr(const char *s, int32_t c)
;
extern
int32_t ecp_strcmp(const char *s1, const char *s2)
;
extern
int32_t ecp_strncmp(const char *s1, const char *s2, size_t len)
;
extern
char *ecp_strstr(const char *s1, const char *s2)
;
extern
char *ecp_strpbrk(const char *s, const char *cs)
;
extern
size_t ecp_strspn(const char *s, const char *cs)
;
extern
size_t ecp_strcspn(const char *s, const char *cs)
;
extern
char *ecp_strtok_r(char *s, const char *sp, char **sl)
;

/* c/string_strdup.c */
extern
char *ecp_strndup(const char *s, size_t len)
;
extern
char *ecp_strdup(const char *s)
;

/* c/string_strerr.c */
extern
char *ecp_strerror(int32_t err)
;

#ifdef __cplusplus
}
#endif

#endif

/* @file: cfg_json.h
 * #desc:
 *    The definitions of json (javascript object notation) parser.
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

#ifndef _ECP_CFG_JSON_H
#define _ECP_CFG_JSON_H

#include <ecp/config.h>
#include <ecp/c/stdint.h>

/* json callback type */
#define JSON_ARRAY_TYPE 1
#define JSON_ARRAY_END_TYPE 2
#define JSON_OBJECT_TYPE 3
#define JSON_OBJECT_END_TYPE 4
#define JSON_KEYVAL_TYPE 5
#define JSON_STRING_TYPE 6
#define JSON_NUMBER_DEC_TYPE 7
#define JSON_NUMBER_HEX_TYPE 8
#define JSON_NUMBER_FLT_TYPE 9
#define JSON_NUMBER_INF_TYPE 10
#define JSON_NUMBER_NAN_TYPE 11
#define JSON_NULL_TYPE 12
#define JSON_TRUE_TYPE 13
#define JSON_FALSE_TYPE 14

/* json error code */
#define JSON_ERR_INVALID 1
#define JSON_ERR_START_TOKEN 2
#define JSON_ERR_OBJECT_TOKEN 3
#define JSON_ERR_OBJECT_END 4
#define JSON_ERR_OBJECT_KEYVAL 5
#define JSON_ERR_OBJECT_STRING 6
#define JSON_ERR_OBJECT_NUMBER 7
#define JSON_ERR_ARRAY_TOKEN 8
#define JSON_ERR_ARRAY_END 9
#define JSON_ERR_ARRAY_STRING 10
#define JSON_ERR_ARRAY_NUMBER 11
#define JSON_ERR_COMMENT 12
#define JSON_ERR_INCOMP 0x10000

#ifdef __cplusplus
extern "C" {
#endif

/* cfg_json_parse.c */
extern
int32_t ecp_json_parse(const char *s, int32_t *err_len, int32_t *err,
		void *arg,
		int32_t (*call)(int32_t, const char *, int32_t, void *))
;

#ifdef __cplusplus
}
#endif

#endif

/* @file: umalloc.h
 * #desc:
 *    The definitions of a simple memory allocator.
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

#ifndef _ECP_C_UMALLOC_H
#define _ECP_C_UMALLOC_H

#include <ecp/config.h>
#include <ecp/c/stddef.h>
#include <ecp/c/stdint.h>
#include <ecp/ds_list.h>

/* vector 16-byte aligend */
struct umalloc_chunk {
	uint32_t prev_size;
	uint32_t size_flags;
};

struct umalloc_chunk_node {
	struct list_node list;
	size_t size;
#ifdef ECP_MARCH_BITS_32
	uint32_t _allgend[3]; /* 8+4+(4+4+4)+8 */
#endif
	struct umalloc_chunk chunk;
};

struct umalloc_ctx {
	struct list_head chunk;
	struct umalloc_chunk *idle;
	void *arg;
	/* size, arg */
	void *(*call_alloc)(size_t, void *);
	/* alloc pointer, size, arg */
	int32_t (*call_free)(void *, size_t, void *);
};

#define UMALLOC_CTX_SET(alloc, free, _arg) \
	{ \
		.chunk = LIST_HEAD_SET0, \
		.call_alloc = alloc, \
		.call_free = free, \
		.arg = _arg \
	}
#define UMALLOC_NEW(name, alloc, free, _arg) \
	struct umalloc_ctx name = UMALLOC_CTX_SET(alloc, free, _arg)
#define UMALLOC_INIT(x, alloc, free, _arg) \
	do { \
		LIST_HEAD_INIT(&(x)->chunk); \
		(x)->call_alloc = alloc; \
		(x)->call_free = free; \
		(x)->arg = _arg; \
	} while (0)

#ifdef __cplusplus
extern "C" {
#endif

/* c/umalloc.c */
extern
void *ecp_umalloc(struct umalloc_ctx *ctx, size_t size)
;
extern
int32_t ecp_umalloc_free(struct umalloc_ctx *ctx, void *p)
;
extern
int32_t ecp_umalloc_free_all(struct umalloc_ctx *ctx)
;

#ifdef __cplusplus
}
#endif

#endif

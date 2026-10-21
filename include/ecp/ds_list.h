/* @file: ds_list.h
 * #desc:
 *    The definitions of linked list.
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

#ifndef _ECP_DS_LIST_H
#define _ECP_DS_LIST_H

#include <ecp/config.h>
#include <ecp/c/stddef.h>

struct list_node {
	struct list_node *prev, *next;
};

struct list_head {
	struct list_node *node;
};

#define LIST_HEAD_SET0 { NULL }
#define LIST_HEAD_NEW(x) struct list_head x = LIST_HEAD_SET0
#define LIST_HEAD_INIT(x) (x)->node = NULL

#define LIST_ENTRY(x, type, member) container_of(x, type, member)

#define LIST_PREV(x) ((x)->prev ? \
	((x)->prev->next ? (x)->prev : NULL) : NULL)
#define LIST_NEXT(x) ((x)->next)

#define LIST_FOR_EACH(node, name) \
	for (struct list_node *name = node; \
			name; \
			name = LIST_NEXT(name))
#define LIST_FOR_PREV_EACH(node, name) \
	for (struct list_node *name = node; \
			name; \
			name = LIST_PREV(name))

#ifdef __cplusplus
extern "C" {
#endif

/* ds_list.c */
extern
struct list_node *ecp_list_add(struct list_head *head,
		struct list_node *_new)
;
extern
struct list_node *ecp_list_add_tail(struct list_head *head,
		struct list_node *_new)
;
extern
struct list_node *ecp_list_del(struct list_head *head,
		struct list_node *old)
;
extern
struct list_node *ecp_list_insert(struct list_head *head,
		struct list_node *node, struct list_node *_new)
;
extern
struct list_node *ecp_list_insert_tail(struct list_head *head,
		struct list_node *node, struct list_node *_new)
;
extern
void ecp_list_swap(struct list_head *head,
		struct list_node *old, struct list_node *_new)
;

#ifdef __cplusplus
}
#endif

#endif

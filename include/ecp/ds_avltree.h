/* @file: ds_avltree.h
 * #desc:
 *    The definitions of AVL self-balancing binary tree.
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

#ifndef _ECP_DS_AVLTREE_H
#define _ECP_DS_AVLTREE_H

#include <ecp/config.h>
#include <ecp/c/stddef.h>
#include <ecp/c/stdint.h>

struct avl_node {
	int32_t bf;
	struct avl_node *parent, *left, *right;
};

struct avl_root {
	struct avl_node *node;
};

#define AVL_ROOT_SET0 { NULL }
#define AVL_ROOT_NEW(x) struct avl_root x = AVL_ROOT_SET0
#define AVL_ROOT_INIT(x) (x)->node = NULL

#define AVL_ENTRY(x, type, member) container_of(x, type, member)

#define AVL_HEIGHT(x) ((x) ? (x)->bf : 0)
#define AVL_MAX(a, b) (((a) > (b)) ? (a) : (b))

#ifdef __cplusplus
extern "C" {
#endif

/* ds_avltree.c */
extern
void ecp_avl_balance(struct avl_root *root, struct avl_node *node)
;
extern
void ecp_avl_erase_fix(struct avl_root *root, struct avl_node *node)
;
extern
struct avl_node *ecp_avl_insert(struct avl_root *root,
		struct avl_node *_new, int32_t (*cmp)(void *, void *))
;
extern
struct avl_node *ecp_avl_search(struct avl_root *root,
		void *arg, int32_t (*cmp)(void *, void *))
;
extern
struct avl_node *ecp_avl_erase(struct avl_root *root,
		struct avl_node *old)
;
extern
struct avl_node *ecp_avl_erase2(struct avl_root *root,
		void *arg, int32_t (*cmp)(void *, void *))
;
extern
struct avl_node *ecp_avl_first(struct avl_root *root)
;
extern
struct avl_node *ecp_avl_last(struct avl_root *root)
;

#ifdef __cplusplus
}
#endif

#endif

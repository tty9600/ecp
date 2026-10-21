/* @file: ds_kfifo.c
 * #desc:
 *    The implementations of first-in-first-out circular buffer.
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
#include <ecp/c/string.h>
#include <ecp/ds_kfifo.h>

#undef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

/* @func: ecp_kfifo_in
 * #desc:
 *    put the data into the fifo buffer.
 *
 * #1: head [in/out] kfifo head
 * #2: s    [in]     input buffer
 * #3: len  [in]     input length
 * #r:      [ret]    real input length
 */
uint32_t ecp_kfifo_in(struct kfifo_head *head, const void *s, uint32_t len)
{
	uint32_t l, f;

	l = KFIFO_AVAIL(head); /* avail length */
	f = head->in & head->mask; /* first offset */

	head->in += len = MIN(len, l);
	l = MIN(len, head->size - f); /* first length */

	ecp_memcpy((char *)head->buf + f, s, l);
	ecp_memcpy(head->buf, (char *)s + l, len - l);

	return len;
}

/* @func: ecp_kfifo_out
 * #desc:
 *    get the data from the fifo buffer.
 *
 * #1: head [in/out] kfifo head
 * #2: t    [out]    output buffer
 * #3: len  [in]     output length
 * #r:      [ret]    real output length
 */
uint32_t ecp_kfifo_out(struct kfifo_head *head, void *t, uint32_t len)
{
	uint32_t l, f;

	l = KFIFO_USED(head); /* used length */
	f = head->out & head->mask; /* first offset */

	head->out += len = MIN(len, l);
	l = MIN(len, head->size - f); /* first length */

	ecp_memcpy(t, (char *)head->buf + f, l);
	ecp_memcpy((char *)t + l, head->buf, len - l);

	return len;
}

/* @func: ecp_kfifo_peek
 * #desc:
 *    peek only the fifo buffer.
 *
 * #1: head [in/out] kfifo head
 * #2: t    [out]    output buffer
 * #3: len  [in]     output length
 * #4: off  [in]     fifo offset
 * #r:      [ret]    real output length
 */
uint32_t ecp_kfifo_peek(struct kfifo_head *head, void *t, uint32_t len,
		uint32_t off)
{
	uint32_t l, f;

	l = KFIFO_USED(head); /* used length */
	if (off > l)
		return 0;

	l -= off;
	f = (head->out + off) & head->mask; /* first offset */

	len = MIN(len, l);
	l = MIN(len, head->size - f); /* first length */

	ecp_memcpy(t, (char *)head->buf + f, l);
	ecp_memcpy((char *)t + l, head->buf, len - l);

	return len;
}

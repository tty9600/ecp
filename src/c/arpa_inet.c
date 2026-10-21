/* @file: arpa_inet.c
 * #desc:
 *    The implementations of internet operations.
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
#include <ecp/c/stddef.h>
#include <ecp/c/stdint.h>
#include <ecp/c/string.h>
#include <ecp/c/sys_socket.h>

/* @func: _int2str_d (static)
 * #desc:
 *    unsigned number to decimal conversion.
 *
 * #1: n [in]  buffer offset
 * #2: p [out] buffer
 * #3: v [in]  input number
 * #r:   [ret] output length (+offset)
 */
static int32_t _int2str_d(int32_t n, char *p, uint64_t v)
{
	int32_t t = n;

	do {
		p[n++] = (v % 10) + '0';
		v /= 10;
	} while (v);

	int32_t k = n;
	while (k > t) {
		char w = p[--k];
		p[k] = p[t];
		p[t++] = w;
	}

	return n;
}

/* @func: _int2str_x (static)
 * #desc:
 *    unsigned number to hexadecimal conversion.
 *
 * #1: n [in]  buffer offset
 * #2: p [out] buffer
 * #3: v [in]  input number
 * #r:   [ret] output length (+offset)
 */
static int32_t _int2str_x(int32_t n, char *p, uint64_t v)
{
	int32_t t = n;

	do {
		int32_t k = v & 15;
		p[n++] = (k < 10) ? (k + '0') : (k + 'a' - 10);
		v >>= 4;
	} while (v);

	int32_t k = n;
	while (k > t) {
		char w = p[--k];
		p[k] = p[t];
		p[t++] = w;
	}

	return n;
}

/* @func: ecp_inet_ntop
 * #desc:
 *    convert ipv4 and ipv6 addresses from binary to text.
 *
 * #1: af  [in]  address family
 * #2: src [in]  input address of the binary
 * #3: dst [out] output text buffer
 * #4: len [in]  buffer length
 * #r:     [ret] 0: error, >0: output the length
 */
const char *ecp_inet_ntop(int32_t af, const void *src, char *dst,
		xsocklen_t len)
{
	const uint8_t *a = src;
	int32_t o = 0, f = 0, p = 0, n = 0, w = 0;
	char buf[24], buf2[4];

	if (af == X_AF_INET) {
		for (int32_t i = 0; i < 4; i++) {
			o = _int2str_d(o, buf, *a++);
			if (i != 3)
				buf[o++] = '.';
		}
		buf[o] = '\0';
		ecp_strncpy(dst, buf, len);
		return dst;
	} else if (af != X_AF_INET6) {
		return NULL;
	}

	for (int32_t i = 0; i < 16; i += 2) {
		if (!n)
			w = i;
		if (n > p) {
			f = w;
			p = n;
		}

		if (a[i] || a[i + 1]) {
			n = 0;
		} else {
			n++;
		}
	}
	if (n > p) {
		f = w;
		p = n;
	}
	f /= 2;

	for (int32_t i = 0; i < 8; i++) {
		if (p && i == f) {
			buf[o++] = ':';
			if (i == 0)
				buf[o++] = ':';
			i += p - 1;
			a += p * 2;
			continue;
		}

		if (*a) {
			if (_int2str_x(0, buf2, *a) == 1)
				buf[o++] = '0';
			o = _int2str_x(o, buf, *a++);
			if (_int2str_x(0, buf2, *a) == 1)
				buf[o++] = '0';
		} else {
			a++;
		}
		o = _int2str_x(o, buf, *a++);

		if (i != 7)
			buf[o++] = ':';
	}
	buf[o] = '\0';
	ecp_strncpy(dst, buf, len);

	return dst;
}

/* @func: ecp_inet_pton
 * #desc:
 *    convert ipv4 and ipv6 addresses from text to binary.
 *
 * #1: af  [in]  address family
 * #2: str [in]  input text address
 * #3: dst [out] output address of the binary
 * #r:     [ret] 1: no error, 0: format invalid, -1: address error
 */
int32_t ecp_inet_pton(int32_t af, const char *str, void *dst)
{
	const char *pstr = str;
	uint8_t *out = dst;
	int32_t n = 0, p = 0, v4 = 0, t = 0, c;
	uint32_t a[8];

	ecp_memset(a, 0, sizeof(a));

	if (af == X_AF_INET) {
		while (*str != '\0') {
			c = *str++;
			if (c >= '0' && c <= '9') {
				a[n] = (a[n] * 10) + (c - '0');
				continue;
			}
			if (c != '.' || ++n > 3)
				return 0;
		}

		switch (n) {
			case 0:
				a[1] = a[0] & 0xffffff;
				a[0] >>= 24;
			case 1:
				a[2] = a[1] & 0xffff;
				a[1] >>= 16;
			case 2:
				a[3] = a[2] & 0xff;
				a[2] >>= 8;
		}

		for (int32_t i = 0; i < 4; i++) {
			if (a[i] > 255)
				return 0;
			*out++ = (uint8_t)a[i];
		}
		return 1;
	} else if (af != X_AF_INET6) {
		return -1;
	}

	if (str[0] == ':' && str[1] != ':')
		return 0;

	while (*str != '\0') {
		c = *str++;
		if (c >= '0' && c <= '9') {
			a[n] = (a[n] * 16) + (c - '0');
			continue;
		} else if (c >= 'a' && c <= 'f') {
			a[n] = (a[n] * 16) + (c - 'a' + 10);
			continue;
		}
		if (c == '.') {
			if (n && ++n < 7) { /* ::ffff:127.0.0.1 */
				v4 = 1;
				break;
			}
			return 0;
		}

		if (c != ':' || ++n > 7)
			return 0;
		if (*str == ':') { /* :: */
			if (t || *++str == ':')
				return 0;
			p = n++;
			t = 1;
		}
		pstr = str;
	}

	if (n != 7) {
		for (int32_t i = 0; i < (n - p); i++)
			a[7 - i] = a[n - i];
		for (int32_t i = 0; i < (7 - n); i++)
			a[p + i + 1] = 0;
	}

	for (int32_t i = 0; i < 8; i++) {
		if (a[i] > 0xffff)
			return -1;
		*out++ = (uint8_t)(a[i] >> 8);
		*out++ = (uint8_t)a[i];
	}

	if (v4 && ecp_inet_pton(X_AF_INET, pstr, out - 4) != 0)
		return 0;

	return 1;
}

/* @file: util_totp.c
 * #desc:
 *
 * #copy:
 *    Copyright (C) 2026 - Alex Chen <chenalex.cy@gmail.com>
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program. If not,
 *    see <https://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <time.h>
#include <ecp/c/stddef.h>
#include <ecp/c/stdint.h>
#include <ecp/c/string.h>
#include <ecp/c/stdlib.h>
#include <ecp/c/getopt.h>
#include <ecp/totp.h>
#include <ecp/basenc.h>

static void _usage(void)
{
	printf(
		"Usage: totp [OPTION...] <base32/two-factor secret>\n"
		" TOTP two-factor secret utility.\n"
		"\n"
		" -s <sec>  time step of the TOTP (default 30)\n"
		" -l <num>  code length (range 1-8)\n"
		" -h        display help\n"
		"\n"
		);
}

int main(int argc, char *argv[])
{
	int32_t r, ind = 1;
	char *arg = NULL;
	char buf[BASE32_DEC_OALIGN(128)];
	uint32_t len, buf_len = sizeof(buf);
	uint32_t ts = 30, cl = 6;

	if (argc < 2) {
		_usage();
		return 1;
	}

	while ((r = ecp_getopt_r(argc, argv, "-hs:l:", &arg, &ind)) != -1) {
		switch (r) {
			case 1:
				if (buf_len != sizeof(buf)) {
					printf("repeat input secret!\n");
					return 1;
				}

				len = (uint32_t)ecp_strlen(arg);
				if (len > BASE32_DEC_IALIGN(128)) {
					printf("secret length too long!\n");
					return 1;
				}

				r = ecp_base32_dec(arg, len, buf, &buf_len);
				if (r) {
					printf("base32 decoding error!\n");
					return 1;
				}
				arg = NULL;
				break;
			case 's':
				ts = (uint32_t)ecp_atoi(arg);
				arg = NULL;
				break;
			case 'l':
				cl = (uint32_t)ecp_atoi(arg);
				if (cl < 1 || cl > 8) {
					printf("code length error!\n");
					return 1;
				}
				arg = NULL;
				break;
			case 'h':
				_usage();
				return 0;
			default:
				printf("unknown '%c' option!\n", OPT_ARGC(arg, r));
				return 1;
		}
	}

	len = sizeof(buf) - buf_len;
	if (!len) {
		printf("not secret!\n");
		return 1;
	}

	printf("secret:");
	for (uint32_t i = 0; i < len; i++) {
		printf(" %02x", buf[i]);
	}
	printf("\n");

	uint64_t t = (uint64_t)time(NULL);
	uint32_t c = ecp_totp_sha1((uint8_t *)buf, len, t / ts, cl);

	printf("time: %llu / %u (%llu) = %llu\n", t, ts, t % ts, t / ts);
	printf("code: %0*u\n", cl, c);

	return 0;
}

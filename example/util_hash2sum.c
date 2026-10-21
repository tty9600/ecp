/* @file: util_hash2sum.c
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
#include <ecp/c/stddef.h>
#include <ecp/c/stdint.h>
#include <ecp/c/stdlib.h>
#include <ecp/c/getopt.h>
#include <ecp/md5.h>
#include <ecp/sha1.h>
#include <ecp/sha2.h>
#include <ecp/sha3.h>
#include <ecp/blake2.h>
#include <ecp/sm3.h>
#include <ecp/xxhash.h>
#include <ecp/basenc.h>

static void _usage(void)
{
	printf(
		"Usage: hash2sum [OPTION...]\n"
		" hash digest utility (default use all).\n"
		"\n"
		" --md5        only md5 digest\n"
		" --sha1       only sha1 digest\n"
		" --sha2       only sha2 groups\n"
		"  --sha224    only sha224 digest\n"
		"  --sha256    only sha256 digest\n"
		"  --sha384    only sha384 digest\n"
		"  --sha512    only sha512 digest\n"
		" --sha3       only sha3 groups\n"
		"  --sha3-224  only sha3-224 digest\n"
		"  --sha3-256  only sha3-256 digest\n"
		"  --sha3-384  only sha3-384 digest\n"
		"  --sha3-512  only sha3-512 digest\n"
		"  --shake128  only shake128 digest\n"
		"  --shake256  only shake256 digest\n"
		" --blake2b    only blake2b-512 digest\n"
		" --blake2s    only blake2s-256 digest\n"
		" --sm3        only sm3 digest\n"
		" --xxh32      only xxhash32 digest\n"
		" --xxh64      only xxhash64 digest\n"
		" --b16        use base16 output digest\n"
		" --b32        use base32 output digest\n"
		" --b64        use base64 output digest\n"
		" --b64url     use base64url output digest\n"
		" -s <num>     number of xxhash seed\n"
		" -x <num>     length of shake digest (max 8192)\n"
		" -h,--help    display help\n"
		"\n"
		"  Use stdin as the input stream.\n"
		);
}

uint8_t digest[8192];
uint8_t buf[8192 * 2];

static void _out_digest(int32_t btype, const char *name, const uint8_t *dig,
		uint32_t len)
{
	uint32_t tlen = (uint32_t)sizeof(buf);

	printf("(%s) ", name);

	switch (btype) {
		case 1:
			ecp_base16_enc((char *)dig, len, (char *)buf, &tlen);
			break;
		case 2:
			ecp_base32_enc((char *)dig, len, (char *)buf, &tlen);
			break;
		case 3:
			ecp_base64_enc((char *)dig, len, (char *)buf, &tlen);
			break;
		case 4:
			ecp_base64url_enc((char *)dig, len, (char *)buf, &tlen);
			break;
		default:
			for (uint32_t i = 0; i < len; i++)
				printf("%02x", dig[i]);
			printf("\n");
			return;
	}

	printf("%.*s\n", (int32_t)(sizeof(buf) - tlen), buf);
}

MD5_NEW(md5_ctx);
SHA1_NEW(sha1_ctx);
SHA224_NEW(sha224_ctx);
SHA256_NEW(sha256_ctx);
SHA384_NEW(sha384_ctx);
SHA512_NEW(sha512_ctx);
SHA3_NEW(sha3_224_ctx);
SHA3_NEW(sha3_256_ctx);
SHA3_NEW(sha3_384_ctx);
SHA3_NEW(sha3_512_ctx);
SHA3_NEW(sha3_shake128_ctx);
SHA3_NEW(sha3_shake256_ctx);
BLAKE2B_NEW(blake2b_ctx);
BLAKE2S_NEW(blake2s_ctx);
SM3_NEW(sm3_ctx);
XXHASH32_NEW(xxh32_ctx);
XXHASH64_NEW(xxh64_ctx);

int main(int argc, char *argv[])
{
	int32_t r, longind, ind = 1;
	char *arg = NULL;
	uint64_t opt_seed = 0;
	int32_t is_md5 = 0, is_sha1 = 0,
		is_sha224 = 0, is_sha256 = 0,
		is_sha384 = 0, is_sha512 = 0,
		is_sha3_224 = 0, is_sha3_256 = 0,
		is_sha3_384 = 0, is_sha3_512 = 0,
		is_shake128 = 0, is_shake256 = 0,
		is_blake2b = 0, is_blake2s = 0,
		is_sm3 = 0,
		is_xxh32 = 0, is_xxh64 = 0,
		opt_x = 0, opt_btype = 0, is_all = 1;

	static const struct option_r longopts[] = {
		{ "md5",      NO_ARGUMENT, NULL, 1000 },
		{ "sha1",     NO_ARGUMENT, NULL, 2000 },
		{ "sha2",     NO_ARGUMENT, NULL, 3000 },
		{ "sha224",   NO_ARGUMENT, NULL, 3001 },
		{ "sha256",   NO_ARGUMENT, NULL, 3002 },
		{ "sha384",   NO_ARGUMENT, NULL, 3003 },
		{ "sha512",   NO_ARGUMENT, NULL, 3004 },
		{ "sha3",     NO_ARGUMENT, NULL, 4000 },
		{ "sha3-224", NO_ARGUMENT, NULL, 4001 },
		{ "sha3-256", NO_ARGUMENT, NULL, 4002 },
		{ "sha3-384", NO_ARGUMENT, NULL, 4003 },
		{ "sha3-512", NO_ARGUMENT, NULL, 4004 },
		{ "shake128", NO_ARGUMENT, NULL, 4005 },
		{ "shake256", NO_ARGUMENT, NULL, 4006 },
		{ "blake2b",  NO_ARGUMENT, NULL, 5000 },
		{ "blake2s",  NO_ARGUMENT, NULL, 6000 },
		{ "sm3",      NO_ARGUMENT, NULL, 7000 },
		{ "xxh32",    NO_ARGUMENT, NULL, 8000 },
		{ "xxh64",    NO_ARGUMENT, NULL, 9000 },
		{ "b16",      NO_ARGUMENT, NULL, 300  },
		{ "b32",      NO_ARGUMENT, NULL, 301  },
		{ "b64",      NO_ARGUMENT, NULL, 302  },
		{ "b64url",   NO_ARGUMENT, NULL, 303  },
		{ "help",     NO_ARGUMENT, NULL, 'h'  },
		{ NULL, 0, NULL, 0 }
		};

	while ((r = ecp_getopt_long_r(argc, argv, "s:x:h",
			longopts, &longind, &arg, &ind)) != -1) {
		switch (r) {
			case 1000:
			case 2000:
			case 3000:
			case 3001:
			case 3002:
			case 3003:
			case 3004:
			case 4000:
			case 4001:
			case 4002:
			case 4003:
			case 4004:
			case 4005:
			case 4006:
			case 5000:
			case 6000:
			case 7000:
			case 8000:
			case 9000:
				is_all = 0;
				switch (r) {
					case 1000:
						is_md5 = 1;
						break;
					case 2000:
						is_sha1 = 1;
						break;
					case 3000:
						is_sha224 = 1;
						is_sha256 = 1;
						is_sha384 = 1;
						is_sha512 = 1;
						break;
					case 3001:
						is_sha224 = 1;
						break;
					case 3002:
						is_sha256 = 1;
						break;
					case 3003:
						is_sha384 = 1;
						break;
					case 3004:
						is_sha512 = 1;
						break;
					case 4000:
						is_sha3_224 = 1;
						is_sha3_256 = 1;
						is_sha3_384 = 1;
						is_sha3_512 = 1;
						is_shake128 = 1;
						is_shake256 = 1;
						break;
					case 4001:
						is_sha3_224 = 1;
						break;
					case 4002:
						is_sha3_256 = 1;
						break;
					case 4003:
						is_sha3_384 = 1;
						break;
					case 4004:
						is_sha3_512 = 1;
						break;
					case 4005:
						is_shake128 = 1;
						break;
					case 4006:
						is_shake256 = 1;
						break;
					case 5000:
						is_blake2b = 1;
						break;
					case 6000:
						is_blake2s = 1;
						break;
					case 7000:
						is_sm3 = 1;
						break;
					case 8000:
						is_xxh32 = 1;
						break;
					case 9000:
						is_xxh64 = 1;
						break;
				}
				arg = NULL;
				break;
			case 300:
			case 301:
			case 302:
			case 303:
				opt_btype = r - 299;
				arg = NULL;
				break;
			case 's':
				opt_seed = (uint64_t)ecp_atoll(arg);
				arg = NULL;
				break;
			case 'x':
				opt_x = (int32_t)ecp_atoi(arg);
				if (opt_x < 1 || opt_x > 8192) {	
					printf("digest length error!\n");
					return 1;
				}
				arg = NULL;
				break;
			case 'h':
				_usage();
				return 0;
			default:
				if (longind != -1) {
					printf("unknown long '%s' option!\n",
						arg);
				} else {
					printf("unknown '%c' option!\n",
						OPT_ARGC(arg, r));
				}
				return 1;
		}
	}

	if (is_all) {
		is_md5 = 1;
		is_sha1 = 1;
		is_sha224 = 1;
		is_sha256 = 1;
		is_sha384 = 1;
		is_sha512 = 1;
		is_sha3_224 = 1;
		is_sha3_256 = 1;
		is_sha3_384 = 1;
		is_sha3_512 = 1;
		is_shake128 = 1;
		is_shake256 = 1;
		is_blake2b = 1;
		is_blake2s = 1;
		is_sm3 = 1;
		is_xxh32 = 1;
		is_xxh64 = 1;
	}

	if (is_md5)
		ecp_md5_init(&md5_ctx);
	if (is_sha1)
		ecp_sha1_init(&sha1_ctx);
	if (is_sha224)
		ecp_sha224_init(&sha224_ctx);
	if (is_sha256)
		ecp_sha256_init(&sha256_ctx);
	if (is_sha384)
		ecp_sha384_init(&sha384_ctx);
	if (is_sha512)
		ecp_sha512_init(&sha512_ctx);
	if (is_sha3_224)
		ecp_sha3_init(&sha3_224_ctx, SHA3_224_TYPE, 0);
	if (is_sha3_256)
		ecp_sha3_init(&sha3_256_ctx, SHA3_256_TYPE, 0);
	if (is_sha3_384)
		ecp_sha3_init(&sha3_384_ctx, SHA3_384_TYPE, 0);
	if (is_sha3_512)
		ecp_sha3_init(&sha3_512_ctx, SHA3_512_TYPE, 0);
	if (is_shake128)
		ecp_sha3_init(&sha3_shake128_ctx, SHA3_SHAKE128_TYPE, 0);
	if (is_shake256)
		ecp_sha3_init(&sha3_shake256_ctx, SHA3_SHAKE256_TYPE, 0);
	if (is_blake2b)
		ecp_blake2b_init(&blake2b_ctx, BLAKE2B_512_LEN);
	if (is_blake2s)
		ecp_blake2s_init(&blake2s_ctx, BLAKE2S_256_LEN);
	if (is_sm3)
		ecp_sm3_init(&sm3_ctx);
	if (is_xxh32)
		ecp_xxhash32_init(&xxh32_ctx, (uint32_t)opt_seed);
	if (is_xxh64)
		ecp_xxhash64_init(&xxh64_ctx, opt_seed);

	uint64_t total_len = 0;
	size_t len = 0;
	while ((len = fread(buf, 1, sizeof(buf), stdin))) {
		total_len += len;
		if (is_md5)
			ecp_md5_process(&md5_ctx, buf, len);
		if (is_sha1)
			ecp_sha1_process(&sha1_ctx, buf, len);
		if (is_sha224)
			ecp_sha256_process(&sha224_ctx, buf, len);
		if (is_sha256)
			ecp_sha256_process(&sha256_ctx, buf, len);
		if (is_sha384)
			ecp_sha512_process(&sha384_ctx, buf, len);
		if (is_sha512)
			ecp_sha512_process(&sha512_ctx, buf, len);
		if (is_sha3_224)
			ecp_sha3_process(&sha3_224_ctx, buf, len);
		if (is_sha3_256)
			ecp_sha3_process(&sha3_256_ctx, buf, len);
		if (is_sha3_384)
			ecp_sha3_process(&sha3_384_ctx, buf, len);
		if (is_sha3_512)
			ecp_sha3_process(&sha3_512_ctx, buf, len);
		if (is_shake128)
			ecp_sha3_process(&sha3_shake128_ctx, buf, len);
		if (is_shake256)
			ecp_sha3_process(&sha3_shake256_ctx, buf, len);
		if (is_blake2b)
			ecp_blake2b_process(&blake2b_ctx, buf, len);
		if (is_blake2s)
			ecp_blake2s_process(&blake2s_ctx, buf, len);
		if (is_sm3)
			ecp_sm3_process(&sm3_ctx, buf, len);
		if (is_xxh32)
			ecp_xxhash32_process(&xxh32_ctx, buf, len);
		if (is_xxh64)
			ecp_xxhash64_process(&xxh64_ctx, buf, len);
	}

	if (is_md5) {
		ecp_md5_finish(&md5_ctx, total_len);
		_out_digest(opt_btype, "md5",
			&MD5_STATE(&md5_ctx, 0), MD5_LEN);
	}
	if (is_sha1) {
		ecp_sha1_finish(&sha1_ctx, total_len);
		_out_digest(opt_btype, "sha1",
			&SHA1_STATE(&sha1_ctx, 0), SHA1_LEN);
	}
	if (is_sha224) {
		ecp_sha256_finish(&sha224_ctx, total_len);
		_out_digest(opt_btype, "sha224",
			&SHA224_STATE(&sha224_ctx, 0), SHA224_LEN);
	}
	if (is_sha256) {
		ecp_sha256_finish(&sha256_ctx, total_len);
		_out_digest(opt_btype, "sha256",
			&SHA256_STATE(&sha256_ctx, 0), SHA256_LEN);
	}
	if (is_sha384) {
		ecp_sha512_finish(&sha384_ctx, total_len);
		_out_digest(opt_btype, "sha384",
			&SHA384_STATE(&sha384_ctx, 0), SHA384_LEN);
	}
	if (is_sha512) {
		ecp_sha512_finish(&sha512_ctx, total_len);
		_out_digest(opt_btype, "sha512",
			&SHA512_STATE(&sha512_ctx, 0), SHA512_LEN);
	}
	if (is_sha3_224) {
		ecp_sha3_finish(&sha3_224_ctx);
		_out_digest(opt_btype, "sha3-224",
			&SHA3_STATE(&sha3_224_ctx, 0), SHA3_224_LEN);
	}
	if (is_sha3_256) {
		ecp_sha3_finish(&sha3_256_ctx);
		_out_digest(opt_btype, "sha3-256",
			&SHA3_STATE(&sha3_256_ctx, 0), SHA3_256_LEN);
	}
	if (is_sha3_384) {
		ecp_sha3_finish(&sha3_384_ctx);
		_out_digest(opt_btype, "sha3-384",
			&SHA3_STATE(&sha3_384_ctx, 0), SHA3_384_LEN);
	}
	if (is_sha3_512) {
		ecp_sha3_finish(&sha3_512_ctx);
		_out_digest(opt_btype, "sha3-512",
			&SHA3_STATE(&sha3_512_ctx, 0), SHA3_512_LEN);
	}
	if (is_shake128) {
		ecp_sha3_finish(&sha3_shake128_ctx);
		ecp_sha3_shake_xof(&sha3_shake128_ctx, digest, (uint32_t)opt_x);
		_out_digest(opt_btype, "shake128",
			digest, (uint32_t)opt_x);
	}
	if (is_shake256) {
		ecp_sha3_finish(&sha3_shake256_ctx);
		ecp_sha3_shake_xof(&sha3_shake256_ctx, digest, (uint32_t)opt_x);
		_out_digest(opt_btype, "shake256",
			digest, (uint32_t)opt_x);
	}
	if (is_blake2b) {
		ecp_blake2b_finish(&blake2b_ctx);
		_out_digest(opt_btype, "blake2b-512",
			&BLAKE2B_STATE(&blake2b_ctx, 0), BLAKE2B_512_LEN);
	}
	if (is_blake2s) {
		ecp_blake2s_finish(&blake2s_ctx);
		_out_digest(opt_btype, "blake2s-256",
			&BLAKE2S_STATE(&blake2s_ctx, 0), BLAKE2S_256_LEN);
	}
	if (is_sm3) {
		ecp_sm3_finish(&sm3_ctx, total_len);
		_out_digest(opt_btype, "sm3",
			&SM3_STATE(&sm3_ctx, 0), SM3_LEN);
	}
	if (is_xxh32) {
		uint32_t x = ecp_xxhash32_finish(&xxh32_ctx, total_len);
		for (int32_t i = 0; i < 4; i++)
			digest[3 - i] = (uint8_t)(x >> (8 * i));
		_out_digest(opt_btype, "xxh32",
			digest, 4);
	}
	if (is_xxh64) {
		uint64_t x = ecp_xxhash64_finish(&xxh64_ctx, total_len);
		for (int32_t i = 0; i < 8; i++)
			digest[7 - i] = (uint8_t)(x >> (8 * i));
		_out_digest(opt_btype, "xxh64",
			digest, 8);
	}

	return 0;
}

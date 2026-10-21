/* @file: math.h
 * #desc:
 *    The definitions of mathematical.
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

#ifndef _ECP_C_MATH_H
#define _ECP_C_MATH_H

#include <ecp/config.h>
#include <ecp/c/stdint.h>

/* logarithm e */
#undef M_E
#define M_E 2.7182818284590452354
/* log2(e) */
#undef M_LOG2E
#define M_LOG2E 2.7182818284590452354
/* log10(e) */
#undef M_LOG10E
#define M_LOG10E 0.43429448190325182765
/* log(2) */
#undef M_LN2
#define M_LN2 0.69314718055994530942
/* log(10) */
#undef M_LN10
#define M_LN10 2.30258509299404568402
/* pi */
#undef M_PI
#define M_PI 3.14159265358979323846
/* pi/2 */
#undef M_PI_2
#define M_PI_2 1.57079632679489661923
/* pi/4 */
#undef M_PI_4
#define M_PI_4 0.78539816339744830962
/* 1/pi */
#undef M_1_PI
#define M_1_PI 0.31830988618379067154
/* 2/pi */
#undef M_2_PI
#define M_2_PI 0.63661977236758134308
/* 2/sqrt(pi) */
#undef M_2_SQRTPI
#define M_2_SQRTPI 1.12837916709551257390
/* square root of 2 */
#undef M_SQRT2
#define M_SQRT2 1.41421356237309504880
/* 1/sqrt(2) */
#undef M_SQRT1_2
#define M_SQRT1_2 0.70710678118654752440

static const union {
	uint32_t i;
	float f;
} _ZERO = { 0x0 };

static const union {
	uint32_t i;
	float f;
} _NAN = { 0x7fc00000 };

static const union {
	uint32_t i;
	float f;
} _INFINITY = { 0x7f800000 };

#undef ZERO
#undef NAN
#undef INFINITY
#define ZERO (_ZERO.f)
#define NAN (_NAN.f)
#define INFINITY (_INFINITY.f)

#define X_FP_NAN 0
#define X_FP_INFINITE 1
#define X_FP_ZERO 2
#define X_FP_SUBNORMAL 3
#define X_FP_NORMAL 4

#define ecp_isnan(x) (ecp_fpclassify(x) == X_FP_NAN)
#define ecp_isinf(x) (ecp_fpclassify(x) == X_FP_INFINITE)
#define ecp_isnormal(x) (ecp_fpclassify(x) == X_FP_NORMAL)
#define ecp_isfinite(x) (ecp_fpclassify(x) != X_FP_NAN \
	&& ecp_fpclassify(x) != X_FP_INFINITE)

#ifdef __cplusplus
extern "C" {
#endif

/* c/math_double.c */
extern
int32_t ecp_fpclassify(double x)
;
extern
double ecp_fabs(double x)
;
extern
double ecp_ceil(double x)
;
extern
double ecp_floor(double x)
;
extern
double ecp_trunc(double x)
;
extern
double ecp_round(double x)
;
extern
double ecp_fmod(double x, double y)
;
extern
double ecp_frexp(double x, int32_t *e)
;
extern
double ecp_ldexp(double x, int32_t e)
;
extern
double ecp_modf(double x, double *n)
;
extern
double ecp_sqrt(double x)
;
extern
double ecp_sin(double x)
;
extern
double ecp_cos(double x)
;
extern
double ecp_tan(double x)
;
extern
double ecp_acos(double x)
;
extern
double ecp_atan(double x)
;
extern
double ecp_asin(double x)
;
extern
double ecp_exp(double x)
;
extern
double ecp_exp2(double x)
;
extern
double ecp_log(double x)
;
extern
double ecp_log2(double x)
;
extern
double ecp_log10(double x)
;
extern
double ecp_log1p(double x)
;
extern
double ecp_pow(double x, double e)
;
extern
double ecp_erf(double x)
;
extern
double ecp_erfc(double x)
;

#ifdef __cplusplus
}
#endif

#endif

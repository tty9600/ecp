/* @file: time.h
 * #desc:
 *    The definitions of time types.
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

#ifndef _ECP_C_TIME_H
#define _ECP_C_TIME_H

#include <ecp/config.h>
#include <ecp/c/stdint.h>
#include <ecp/c/sys_types.h>

#if defined(ECP_PLATFORM_LINUX)

struct xtm {
	int32_t tm_sec;   /* seconds after the minute [0, 60] */
	int32_t tm_min;   /* minutes after the hour -- [0, 59] */
	int32_t tm_hour;  /* hours since midnight -- [0, 23] */
	int32_t tm_mday;  /* day of the month -- [1, 31] */
	int32_t tm_mon;   /* months since January -- [0, 11] */
	int32_t tm_year;  /* years since 1900 */
	int32_t tm_wday;  /* days since Sunday -- [0, 6] */
	int32_t tm_yday;  /* days since January 1 -- [0, 365] */
	int32_t tm_isdst; /* daylight saving time flag */
};

#define X_CLOCKS_PER_SEC 1000000

/* posix */
struct xtimespec {
	xtime_t tv_sec; /* seconds */
	long tv_nsec;  /* nanoseconds */
};

struct xtimeval {
	xtime_t tv_sec;       /* seconds */
	xsuseconds_t tv_usec; /* microseconds */
};

struct xitimerval {
	struct xtimeval it_interval; /* timer interval */
	struct xtimeval it_value;    /* first start time */
};

/* calendar/date time (system time) */
#define X_CLOCK_REALTIME 0
/* monotonic clock (not affected by system time) */
#define X_CLOCK_MONOTONIC 1
/* process cpu time */
#define X_CLOCK_PROCESS_CPUTIME_ID 2
/* thread cpu time */
#define X_CLOCK_THREAD_CPUTIME_ID 3

/* system time */
#define X_ITIMER_REAL 0
/* userspace process cpu time */
#define X_ITIMER_VIRTUAL 1
/* userspace and kernelspace process cpu time */
#define X_ITIMER_PROF 2

#else
# error "!!!unknown platform!!!"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* c/time_clock.c */
extern
int32_t ecp_clock_gettime(int32_t cid, struct xtimespec *ts)
;
extern
int32_t ecp_clock_settime(int32_t cid, const struct xtimespec *ts)
;
extern
xclock_t ecp_clock(void)
;
extern
xtime_t ecp_time(xtime_t *r)
;

/* c/time_sleep.c */
extern
int32_t ecp_nanosleep(const struct xtimespec *req, struct xtimespec *rem)
;

/* c/time_timer.c */
extern
int32_t ecp_setitimer(int32_t which, const struct xitimerval *_new,
		struct xitimerval *old)
;
extern
int32_t ecp_getitimer(int32_t which, struct xitimerval *cur)
;

/* c/time_tm.c */
extern
struct xtm *ecp_gmtime_r(int64_t t, int32_t off, struct xtm *tm)
;
extern
int64_t ecp_mktime_r(struct xtm *tp)
;

#ifdef __cplusplus
}
#endif

#endif

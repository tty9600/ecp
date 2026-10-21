/* @file: time_tm.c
 * #desc:
 *    The implementations of time function.
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
#include <ecp/c/time.h>

/* NOTE: implementation from glibc */

static const uint16_t __mon_day[2][13] = {
	/* normal years */
	{ 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 },
	/* leap years */
	{ 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 }
	};

#define	SECS_PER_HOUR (60 * 60)
#define	SECS_PER_DAY (SECS_PER_HOUR * 24)
#define ISLEAP(y) (((y) % 4) == 0 && (((y) % 100) != 0 || ((y) % 400 == 0)))
#define DIV(a, b) (((a) / (b)) - (((a) % (b)) < 0))
#define LEAPS_THRU_END_OF(y) (DIV(y, 4) - DIV(y, 100) + DIV(y, 400))

#define EPOCH_YEAR 1970
#define TM_YEAR_BASE 1900
#define LEAPYEAR(y) (((y) % 4) == 0 && (((y) % 100) != 0 \
	|| (((y) / 100) % 4) == (-(TM_YEAR_BASE / 100) % 4)))

/* @func: ecp_gmtime_r
 * #desc:
 *    unix timestamp to date structure conversion.
 *
 * #1: t   [in]  unix timestamp
 * #2: off [in]  time/zone offset (sec)
 * #3: tm  [out] xtm struct pointer
 * #r:     [ret] returns the xtm struct pointer
 */
struct xtm *ecp_gmtime_r(int64_t t, int32_t off, struct xtm *tm)
{
	int64_t days, rem, y, yg;

	days = t / SECS_PER_DAY;
	rem = t % SECS_PER_DAY;
	rem += off;
	for (; rem < 0; days--)
		rem += SECS_PER_DAY;
	for (; rem >= SECS_PER_DAY; days++)
		rem -= SECS_PER_DAY;

	tm->tm_hour = rem / SECS_PER_HOUR;
	rem %= SECS_PER_HOUR;
	tm->tm_min = rem / 60;
	tm->tm_sec = rem % 60;

	/* Thursday, January 1, 1970 */
	tm->tm_wday = (days + 4) % 7;
	if (tm->tm_wday < 0)
		tm->tm_wday += 7;

	y = 1970;
	while (days < 0 || days >= (ISLEAP(y) ? 366 : 365)) {
		/* guess a corrected year, assuming 365 days per year */
		yg = y + DIV(days, 365);

		/* adjust days and y to match the guessed year */
		days -= ((yg - y) * 365
			+ LEAPS_THRU_END_OF(yg - 1)
			- LEAPS_THRU_END_OF(y - 1));
		y = yg;
	}

	tm->tm_year = y - 1900;
	if (tm->tm_year != (y - 1900))
		return NULL;
	tm->tm_yday = days;

	/* adjust days of leap year */
	const uint16_t *ip = __mon_day[ISLEAP(y)];
	for (y = 11; days < (int64_t)ip[y]; y--);
	days -= ip[y];

	tm->tm_mon = y;
	tm->tm_mday = days + 1;

	return tm;
}

/* @func: _ydhms_diff (static)
 * #desc:
 *    returns the difference in seconds between the year, year-day, hour,
 *    minute, and second.
 *
 * #1: year1 [in]  b year
 * #2: yday1 [in]  b year-day
 * #3: hour1 [in]  b hour
 * #4: min1  [in]  b minute
 * #5: sec1  [in]  b second
 * #6: year0 [in]  a year
 * #7: yday0 [in]  a year-day
 * #8: hour0 [in]  a hour
 * #9:  min0 [in]  a minute
 * #10: sec0 [in]  a second
 * #r:       [ret] difference in seconds between
 */
static int64_t _ydhms_diff(int32_t year1, int32_t yday1, int32_t hour1,
		int32_t min1, int32_t sec1, int32_t year0, int32_t yday0,
		int32_t hour0, int32_t min0, int32_t sec0)
{
	int32_t a4 = (year1 / 4) + (TM_YEAR_BASE / 4) - !(year1 % 4);
	int32_t b4 = (year0 / 4) + (TM_YEAR_BASE / 4) - !(year0 % 4);
	int32_t a100 = ((a4 + (a4 < 0)) / 25) - (a4 < 0);
	int32_t b100 = ((b4 + (b4 < 0)) / 25) - (b4 < 0);
	int32_t a400 = a100 / 4;
	int32_t b400 = b100 / 4;
	int32_t leap_days = (a4 - b4) - (a100 - b100) + (a400 - b400);

	int64_t year = year1 - year0;
	int64_t yday = year * 365 + yday1 - yday0 + leap_days;
	int64_t hour = yday * 24 + hour1 - hour0;
	int64_t min = hour * 60 + min1 - min0;
	int64_t sec = min * 60 + sec1 - sec0;

	return sec;
}

/* @func: ecp_mktime_r
 * #desc:
 *    date structure to unix timestamp conversion.
 *
 * #1: tp [in/out] input an xtm struct and return the normalized value
 * #r:    [ret]    returns the unix timestamp
 */
int64_t ecp_mktime_r(struct xtm *tp)
{
	struct xtm tm;
	int32_t sec = tp->tm_sec;
	int32_t min = tp->tm_min;
	int32_t hour = tp->tm_hour;

	/* NOTE: daylight saving time has not yet implemented */

	/* normalization */
	int32_t mon_rem = tp->tm_mon % 12;
	int32_t mon_year = tp->tm_mon / 12 - (mon_rem < 0);
	int32_t year = tp->tm_year + mon_year;
	int32_t mon_yday = __mon_day[LEAPYEAR(year)]
		 [mon_rem + 12 * (mon_rem < 0)] - 1;
	int32_t yday = mon_yday + tp->tm_mday;

	/* leap seconds */
	int32_t sec_req = sec;
	if (sec < 0)
		sec = 0;
	if (sec > 59)
		sec = 59;

	int64_t t = _ydhms_diff(year, yday, hour, min, sec,
		EPOCH_YEAR - TM_YEAR_BASE, 0, 0, 0, 0);

	for (int32_t i = 0; i < 6; i++) {
		if (!ecp_gmtime_r(t, 0, &tm))
			return -1;

		int64_t dt = _ydhms_diff(year, yday, hour, min, sec,
			tm.tm_year, tm.tm_yday, tm.tm_hour, tm.tm_min,
			tm.tm_sec);
		if (!dt)
			break;
		if (i == 5)
			return -1;

		t += dt;
	}

	/* adjust leap seconds */
	if (sec_req != tm.tm_sec) {
		int64_t sec_adjust = sec == 0 && tm.tm_sec == 60;
		sec_adjust -= sec;
		sec_adjust += sec_req;
		t += sec_adjust;
		if (t < 0)
			return -1;

		if (!ecp_gmtime_r(t, 0, &tm))
			return -1;
	}

	ecp_memcpy(tp, &tm, sizeof(struct xtm));

	return t;
}

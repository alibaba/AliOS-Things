/*
 * Copyright (C) 2017 XRADIO TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of XRADIO TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "cmd_util.h"
#include "cmd_rtc.h"
#include "driver/chip/hal_rtc.h"
#include <time.h>

#define	CMD_YEAR0			 	1900 /* the first year for struct tm::tm_year */
#define	CMD_IS_LEAP_YEAR(year)	(!((year) % 4) && (((year) % 100) || !((year) % 400)))

/* due to time_t is singed long, we will get problems on 2038-01-19 03:14:08 */
#define CMD_YEAR_MIN			1970
#define CMD_YEAR_MAX			2037

#define CMD_WDAY_TM2RTC(wday)	(((wday) + 6) % 7)
#define CMD_WDAY_RTC2TM(wday)	(((wday) + 1) % 7)

enum cmd_rtc_alarm {
	CMD_RTC_ALARM_SECOND = 0,
	CMD_RTC_ALARM_WDAY
};

const int8_t g_cmd_mday[2][12] = {
	{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }, /* normal year */
	{ 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }	/* leap year */
};

const char *g_cmd_wday_str[] = {
	"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"
};

/*
 * drv rtc set <year-month-mday,hour:minute:second>
 */
static enum cmd_status cmd_rtc_set_exec(char *cmd)
{
	int year, month, mday, hour, minute, second;
	int32_t cnt;
	struct tm t;
	time_t clock;
	int leap;

	cnt = cmd_sscanf(cmd, "%d-%d-%d,%d:%d:%d",
	                 &year, &month, &mday, &hour, &minute, &second);
	if (cnt != 6) {
		return CMD_STATUS_INVALID_ARG;
	}

	if (year < CMD_YEAR_MIN || year > CMD_YEAR_MAX) {
		CMD_ERR("invalid year %u\n", year);
		return CMD_STATUS_INVALID_ARG;
	}
	leap = CMD_IS_LEAP_YEAR(year);

	if (month < 1 || month > 12) {
		CMD_ERR("invalid month %u\n", month);
		return CMD_STATUS_INVALID_ARG;
	}
	month -= 1;

	if (mday < 1 || mday > g_cmd_mday[leap][month]) {
		CMD_ERR("invalid mday %u\n", mday);
		return CMD_STATUS_INVALID_ARG;
	}

	if (hour < 0 || hour > 23) {
		CMD_ERR("invalid hour %u\n", hour);
		return CMD_STATUS_INVALID_ARG;
	}

	if (minute < 0 || minute > 59) {
		CMD_ERR("invalid minute %u\n", minute);
		return CMD_STATUS_INVALID_ARG;
	}

	if (second < 0 || second > 59) {
		CMD_ERR("invalid second %u\n", second);
		return CMD_STATUS_INVALID_ARG;
	}

	cmd_memset(&t, 0, sizeof(t));
	t.tm_year = year - CMD_YEAR0;
	t.tm_mon = month;
	t.tm_mday = mday;
	t.tm_hour = hour;
	t.tm_min = minute;
	t.tm_sec = second;
	clock = mktime(&t);
	gmtime_r(&clock, &t);

	t.tm_mon += 1;
	HAL_RTC_SetYYMMDD(leap, t.tm_year, t.tm_mon, t.tm_mday);
	HAL_RTC_SetDDHHMMSS(CMD_WDAY_TM2RTC(t.tm_wday), t.tm_hour, t.tm_min, t.tm_sec);

	CMD_DBG("%d-%02d-%02d %s %02d:%02d:%02d, leap %d\n",
		    t.tm_year + CMD_YEAR0, t.tm_mon, t.tm_mday,
		    g_cmd_wday_str[t.tm_wday], t.tm_hour, t.tm_min, t.tm_sec, leap);

	return CMD_STATUS_OK;
}

/*
 * drv rtc get
 */
static enum cmd_status cmd_rtc_get_exec(char *cmd)
{
	uint8_t leap, year, month, mday, hour, minute, second;
	RTC_WeekDay wday;

	HAL_RTC_GetYYMMDD(&leap, &year, &month, &mday);
	HAL_RTC_GetDDHHMMSS(&wday, &hour, &minute, &second);

	cmd_write_respond(CMD_STATUS_OK, "%d-%02d-%02d %s %02d:%02d:%02d",
		    year + CMD_YEAR0, month, mday,
		    g_cmd_wday_str[CMD_WDAY_RTC2TM(wday)],
		    hour, minute, second);

	return CMD_STATUS_ACKED;
}

static void cmd_rtc_alarm_callback(void *arg)
{
	uint8_t leap, year, month, mday, hour, minute, second;
	RTC_WeekDay wday;
	enum cmd_rtc_alarm alarm = (enum cmd_rtc_alarm)arg;

	HAL_RTC_GetYYMMDD(&leap, &year, &month, &mday);
	HAL_RTC_GetDDHHMMSS(&wday, &hour, &minute, &second);

	cmd_write_event(CMD_EVENT_RTC_NOTIFY, "rtc %s-alarm notify at "
	                "%d-%02d-%02d %s %02d:%02d:%02d",
		            alarm == CMD_RTC_ALARM_SECOND ? "sec" : "wday",
		            year + CMD_YEAR0, month, mday,
		            g_cmd_wday_str[CMD_WDAY_RTC2TM(wday)],
		            hour, minute, second);
}

/*
 * drv rtc alarm sec start <seconds>
 */
static enum cmd_status cmd_rtc_sec_alarm_start_exec(char *cmd)
{
	uint32_t second;
	int32_t cnt;
	RTC_SecAlarmStartParam param;

	cnt = cmd_sscanf(cmd, "%u", &second);
	if (cnt != 1) {
		return CMD_STATUS_INVALID_ARG;
	}

	param.alarmSeconds = second;
	param.callback = cmd_rtc_alarm_callback;
	param.arg = (void *)CMD_RTC_ALARM_SECOND;
	HAL_RTC_StartSecAlarm(&param);
	return CMD_STATUS_OK;
}

/*
 * drv rtc alarm sec stop
 */
static enum cmd_status cmd_rtc_sec_alarm_stop_exec(char *cmd)
{
	HAL_RTC_StopSecAlarm();
	return CMD_STATUS_OK;
}

/*
 * drv rtc alarm wday start <hour:minute:second> <wday>...
 */
static enum cmd_status cmd_rtc_wday_alarm_start_exec(char *cmd)
{
	int argc, i, j;
	char *argv[9];
	uint32_t hour, minute, second;
	uint8_t wday_mask;
	int32_t cnt;
	RTC_WDayAlarmStartParam param;

	argc = cmd_parse_argv(cmd, argv, 9);
	if (argc < 2 || argc > 8) {
		return CMD_STATUS_INVALID_ARG;
	}

	cnt = cmd_sscanf(argv[0], "%d:%d:%d", &hour, &minute, &second);
	if (cnt != 3) {
		return CMD_STATUS_INVALID_ARG;
	}

	if (hour < 0 || hour > 23) {
		CMD_ERR("invalid hour %u\n", hour);
		return CMD_STATUS_INVALID_ARG;
	}

	if (minute < 0 || minute > 59) {
		CMD_ERR("invalid minute %u\n", minute);
		return CMD_STATUS_INVALID_ARG;
	}

	if (second < 0 || second > 59) {
		CMD_ERR("invalid second %u\n", second);
		return CMD_STATUS_INVALID_ARG;
	}

	wday_mask = 0;
	for (i = 1; i < argc; ++i) {
		for (j = 0; j < cmd_nitems(g_cmd_wday_str); ++j) {
			if (cmd_strcmp(argv[i], g_cmd_wday_str[j]) == 0) {
				wday_mask |= RTC_WDAY_ALARM_EN_BIT(CMD_WDAY_TM2RTC(j));
				break;
			}
		}
		if (j >= cmd_nitems(g_cmd_wday_str)) {
			wday_mask = 0;
			break; /* Invalid week day */
		}
	}

	if (wday_mask == 0) {
		CMD_ERR("invalid week days\n");
		return CMD_STATUS_INVALID_ARG;
	}

	param.alarmHour = hour;
	param.alarmMinute = minute;
	param.alarmSecond = second;
	param.alarmWDayMask = wday_mask;
	param.callback = cmd_rtc_alarm_callback;
	param.arg = (void *)CMD_RTC_ALARM_WDAY;
	HAL_RTC_StartWDayAlarm(&param);
	return CMD_STATUS_OK;
}

/*
 * drv rtc alarm wday stop
 */
static enum cmd_status cmd_rtc_wday_alarm_stop_exec(char *cmd)
{
	HAL_RTC_StopWDayAlarm();
	return CMD_STATUS_OK;
}

static struct cmd_data g_rtc_cmds[] = {
	{ "set",				cmd_rtc_set_exec },
	{ "get", 				cmd_rtc_get_exec },
	{ "sec-alarm-start",	cmd_rtc_sec_alarm_start_exec },
	{ "sec-alarm-stop",		cmd_rtc_sec_alarm_stop_exec},
	{ "wday-alarm-start",	cmd_rtc_wday_alarm_start_exec },
	{ "wday-alarm-stop",	cmd_rtc_wday_alarm_stop_exec },
};

enum cmd_status cmd_rtc_exec(char *cmd)
{
	return cmd_exec(cmd, g_rtc_cmds, cmd_nitems(g_rtc_cmds));
}

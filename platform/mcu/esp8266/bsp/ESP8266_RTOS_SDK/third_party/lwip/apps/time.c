/*
 time.c - implementation of Time API for esp8266

 Copyright (c) 2014 Ivan Grokhotkov. All rights reserved.
 This file is part of the esp8266 core for Arduino environment.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
#include "c_types.h"
#include "esp_timer.h"

#include "lwip/apps/sntp.h"
#include "lwip/apps/time.h"
#include "lwip/apps/sntp_time.h"

static os_timer_t micros_overflow_timer;
static uint32 micros_at_last_overflow_tick = 0;
static uint32 micros_overflow_count = 0;

static void micros_overflow_tick(void * arg)
{
	uint32 m = system_get_time();
	if (m < micros_at_last_overflow_tick)
		micros_overflow_count ++;

	micros_at_last_overflow_tick = m;
}

static void micros_set_default_time(void)
{
	os_timer_disarm(&micros_overflow_timer);
	os_timer_setfn(&micros_overflow_timer, (os_timer_func_t *)micros_overflow_tick, 0);
	os_timer_arm(&micros_overflow_timer, 60 * 1000, 1);
}

unsigned long millis(void)
{
	uint32 m = system_get_time();
	uint32 c = micros_overflow_count + ((m < micros_at_last_overflow_tick) ? 1 : 0);
	return c * 4294967 + m / 1000000;
}

unsigned long micros(void)
{
	return system_get_time();
}

void updateTime(uint32 ms)
{
	sntp_set_update_delay(ms);
}

static void setServer(int id, const char* name_or_ip)
{
	if (name_or_ip){
		sntp_setservername(id, (char*)name_or_ip);
	}
}

bool configTime(int timezone, int daylightOffset, char *server1, char *server2, char *server3, bool enable)
{
	if (server1 == NULL && server2 == NULL && server3 == NULL)
		return false;

	sntp_stop();

	setServer(0, server1);
	setServer(1, server2);
	setServer(2, server3);

	sntp_set_timezone(timezone);

	sntp_init();

	return true;
}

int __attribute__((weak))
_gettimeofday_r(void *ptr, struct timeval *ptimeval, void *ptimezone)
{
	if (ptimeval){
	    time_t seconds = sntp_get_current_timestamp();
        if (seconds == 0){
            micros_set_default_time();
            seconds = millis();
        }
		ptimeval->tv_sec = seconds;

		ptimeval->tv_usec = 0;
	}

	return 0;
}

char* getrealtimeofday(void)
{
	struct timeval t;
	gettimeofday(&t, NULL);
	return sntp_get_real_time(t.tv_sec);
}

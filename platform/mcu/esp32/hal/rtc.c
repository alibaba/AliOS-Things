/*
* Copyright (C) 2015-2018 Alibaba Group Holding Limited
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <hal/soc/rtc.h>
#include <time.h>
#include <sys/time.h>

static inline time_t get_time(void){
	return time(NULL);
}


int32_t hal_rtc_init(rtc_dev_t *rtc){
	return (0);
}

int32_t hal_rtc_get_time(rtc_dev_t *rtc, rtc_time_t *time){
    time_t temp;
    struct tm * local_time = NULL;
	temp = get_time();
	local_time=localtime (&temp);
	time->sec = local_time->tm_sec;
	time->min = local_time->tm_min;
	time->hr =  local_time->tm_hour;
	time->weekday = local_time->tm_wday;
	time->date = local_time->tm_mday;
	time->month = local_time->tm_mon;
	time->year = local_time->tm_year;
	
	return 0;
}


int32_t hal_rtc_set_time(rtc_dev_t *rtc, const rtc_time_t *time){
	struct tm temp;
	temp.tm_year=time->year;
	temp.tm_mon=time->month;
	temp.tm_mday=time->date;
	temp.tm_hour=time->hr;
	temp.tm_min=time->min;
	temp.tm_sec=time->sec;
	temp.tm_isdst = -1;
	time_t ticks =mktime(&temp);
	struct timeval config;
	config.tv_sec = ticks;
	config.tv_usec = 0;
	
	return settimeofday(&config,NULL);
}


int32_t hal_rtc_finalize(rtc_dev_t *rtc){
	return 0;
}
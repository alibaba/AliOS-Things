/*
 * sntp_time.h
 *
 *  Created on: 2016-11-9
 *      Author: LiuHan
 */

#ifndef SNTP_TIME_H_
#define SNTP_TIME_H_

#include "lwip/opt.h"
#include "lwip/timers.h"

#define SECSPERMIN	60L
#define MINSPERHOUR	60L
#define HOURSPERDAY	24L
#define SECSPERHOUR	(SECSPERMIN * MINSPERHOUR)
#define SECSPERDAY	(SECSPERHOUR * HOURSPERDAY)
#define DAYSPERWEEK	7
#define MONSPERYEAR	12

#define YEAR_BASE	1900
#define EPOCH_YEAR      1970
#define EPOCH_WDAY      4
#define EPOCH_YEARS_SINCE_LEAP 2
#define EPOCH_YEARS_SINCE_CENTURY 70
#define EPOCH_YEARS_SINCE_LEAP_CENTURY 370

#define isleap(y) ((((y) % 4) == 0 && ((y) % 100) != 0) || ((y) % 400) == 0)

typedef long sntp_time_t;

typedef struct{
  int	tm_sec;
  int	tm_min;
  int	tm_hour;
  int	tm_mday;
  int	tm_mon;
  int	tm_year;
  int	tm_wday;
  int	tm_yday;
  int	tm_isdst;
}sntp_tm;

typedef struct{
  char ch;
  int m;
  int n;
  int d;
  int s;
  sntp_time_t change;
  int offset;
}sntp_tm_type;

void sntp_set_system_time(sntp_time_t GMT_Time);
bool sntp_set_timezone(s8_t timezone);
u32_t sntp_get_current_timestamp(void);
char* sntp_get_real_time(sntp_time_t t);


#endif /* SNTP_TIME_H_ */

#ifndef __CLOCK_GETTIME_H
#define __CLOCK_GETTIME_H

/********************************************************************************
 * Included Files
 ********************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>



#define CLOCK_REALTIME     0
#define CLOCK_MONOTONIC  1

#define TIMER_ABSTIME      1

struct tm
{
  int tm_sec;     /* Seconds (0-61, allows for leap seconds) */
  int tm_min;     /* Minutes (0-59) */
  int tm_hour;    /* Hours (0-23) */
  int tm_mday;    /* Day of the month (1-31) */
  int tm_mon;     /* Month (0-11) */
  int tm_year;    /* Years since 1900 */
  int tm_wday;    /* Day of the week (0-6) */
  int tm_yday;    /* Day of the year (0-365) */
  int tm_isdst;   /* Non-0 if daylight savings time is in effect */
};

struct timezone {
  int tz_minuteswest;   /* minutes west of Greenwich */
  int tz_dsttime;       /* type of dst correction */
};

/********************************************************************************
 * Public Function Prototypes
 ********************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

int gettimeofday(struct timeval *tv, struct timezone *tz);
int settimeofday(const struct timeval *tv, struct timezone *tz);

int clock_settime(clockid_t clockid, const struct timespec *tp);
int clock_gettime(clockid_t clockid, struct timespec *tp);

struct tm* localtime(const time_t* t);
time_t mktime(struct tm *tp);
struct tm *gmtime(const time_t *timep);
int stime(time_t *t);
time_t time(  time_t *timep);
int clock_time_init(void);

#ifdef __cplusplus
}
#endif

#endif  /* __CLOCK_GETTIME_H */

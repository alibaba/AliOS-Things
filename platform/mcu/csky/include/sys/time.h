/*
 * time.h - the file definitions time functions.
 *
 * Copyright (C): 2012 Hangzhou C-SKY Microsystem Co.,LTD.
 * Author: zhang wenmeng  (wenmeng_zhang@c-sky.com)
 * Date: 2012-4-26
 */
#ifndef _TIME_H_
#define _TIME_H_
#include <features.h>

#include <ansidef.h>
#include <stdlib.h>

#ifndef NULL
#define	NULL	0
#endif

#define CLOCKS_PER_SEC 1000000l

#ifndef time_t
typedef long time_t;
#endif

#ifndef clock_t
typedef unsigned long int clock_t;
#endif

typedef long long __kernel_time_t;
typedef long long __kernel_suseconds_t;
struct timeval {
        __kernel_time_t         tv_sec;         /* seconds */
        __kernel_suseconds_t    tv_usec;        /* microseconds */
};

struct tm
{
	int	tm_sec;
	int	tm_min;
	int	tm_hour;
	int	tm_mday;
	int	tm_mon;
	int	tm_year;
	int	tm_wday;
	int	tm_yday;
	int	tm_isdst;
#ifdef __UCLIBC_HAS_TM_EXTENSIONS__
#ifdef  __USE_BSD
  long int tm_gmtoff;           /* Seconds east of UTC.  */
  __const char *tm_zone;        /* Timezone abbreviation.  */
#else
  long int __tm_gmtoff;         /* Seconds east of UTC.  */
  __const char *__tm_zone;      /* Timezone abbreviation.  */
#endif
#endif /* __UCLIBC_HAS_TM_EXTENSIONS__ */
};


extern double difftime(time_t tim1, time_t tim2);
extern time_t mktime(struct tm *tim_p);
extern char * asctime(const struct tm *tim_p);
extern char *ctime(const time_t * tim_p);
extern struct tm *gmtime(const time_t * tim_p);
extern size_t strftime(char *s, size_t maxsize, const char *format, 
	const struct tm *tim_p);
extern struct tm *localtime(const time_t * tim_p);

/* there is no function achieve */
extern clock_t clock(void);
extern time_t time(time_t *t);
extern struct tm *gmtime_r (__const time_t *__restrict __timer,
                            struct tm *__restrict __tp);
extern char *asctime_r (__const struct tm *__restrict __tp,
                        char *__restrict __buf);
#endif

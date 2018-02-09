#ifndef __SNTP_H__
#define __SNTP_H__

#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

void sntp_init(void);
void sntp_stop(void);

/* Realtek added */
void sntp_get_lasttime(long *sec, long *usec, unsigned int *tick);
struct tm sntp_gen_system_time(int timezone);

#ifdef __cplusplus
}
#endif

#endif /* __SNTP_H__ */

#ifndef _UNISTD_H_
#define _UNISTD_H_
#include "kernel/os/os.h"

#ifdef __cplusplus
extern "C"
{
#endif
static inline int usleep(unsigned int usecs) {
	OS_MSleep((usecs + 999) / 1000);
//	OS_MSleep((usecs + 1023) & (~1023));
	return 0;
}
static inline int msleep(unsigned int msecs) {
	OS_MSleep(msecs);
	return 0;
}
static inline void sleep(int seconds) {
	OS_Sleep(seconds);
}

#ifdef __cplusplus	
}
#endif

#endif

/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __SYS_TIME_H__
#define __SYS_TIME_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <time.h>

extern int gettimeofday(struct timeval * tp, struct timezone * tzp);
#ifndef suseconds_t
typedef long suseconds_t;

#endif
#ifdef __cplusplus
}
#endif

#endif /* __SYS_TIME_H__ */


/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef _AOS_DELAY_H
#define _AOS_DELAY_H

#include <stdint.h>

#ifndef ndelay
#define ndelay aos_ndelay
#endif

#ifndef udelay
#define udelay aos_udelay
#endif

#ifndef mdelay
#define mdelay aos_mdelay
#endif

#ifdef __cplusplus
extern "C" {
#endif

void aos_ndelay(uint32_t nsecs);
void aos_udelay(uint32_t usecs);
void aos_mdelay(uint32_t msecs);

#ifdef __cplusplus
}
#endif

#endif /*_AOS_DELAY_H*/

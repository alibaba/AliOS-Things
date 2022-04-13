/*
 * Copyright (c) 2013 Nuvoton Technology Corp.
 *
 * Description:   PTP hardware time stamping hardware abstract layer header
 */
#ifndef __LWIP_TIME_STAMP_H__
#define __LWIP_TIME_STAMP_H__

#include "lwip/opt.h"
#include "lwip/err.h"
#include "lwip/sockets.h"

#ifdef __cplusplus
extern "C" {
#endif

struct ts_timeval
{
    s32_t sec;
    s32_t nsec;
};

u32_t ts_init(struct ts_timeval *t);
u32_t ts_update(struct ts_timeval *t);
u32_t ts_settime(struct ts_timeval *t);
u32_t ts_gettime(struct ts_timeval *t);
u32_t ts_adjtimex(s32_t ppb);

#ifdef __cplusplus
}
#endif

#endif /* __LWIP_TIME_STAMP_H__ */

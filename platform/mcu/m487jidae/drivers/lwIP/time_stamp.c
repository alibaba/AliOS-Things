/*
 * Copyright (c) 2016 Nuvoton Technology Corp.
 * Description:   M480 EMAC time stamp driver
 */
#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/time_stamp.h"
#include "netif/m480_eth.h"

#ifdef TIME_STAMPING

u32_t ts_init(struct ts_timeval *t)
{

    ETH_TS_ENABLE();
    ETH_settime(t->sec, t->nsec);
    ETH_adjtimex(0);
    ETH_setinc();
    ETH_TS_START();

    return 0;
}


u32_t ts_update(struct ts_timeval *t)
{
    if(t->sec < 0)
    {
        ETH_updatetime(1, -t->sec, t->nsec);
    }
    else if(t->nsec < 0)
    {
        ETH_updatetime(1, t->sec, -t->nsec);
    }
    else
        ETH_updatetime(0, t->sec, t->nsec);
    return 0;
}

u32_t ts_settime(struct ts_timeval *t)
{

    ETH_updatetime(0, t->sec, t->nsec);
    return 0;
}

u32_t ts_gettime(struct ts_timeval *t)
{
    ETH_gettime((u32_t *)&t->sec, (u32_t *)&t->nsec);
    return 0;

}

u32_t ts_adjtimex(s32_t ppb)
{
    ETH_adjtimex(ppb);
    return 0;

}


#endif

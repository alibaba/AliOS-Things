/*
 * Copyright (c) 2013 Nuvoton Technology Corp.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
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

/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __AWSS_SMARTCONFIG__
#define __AWSS_SMARTCONFIG__

#include <stdint.h>
#include "os.h"
#include "zconfig_ieee80211.h"
#include "zconfig_protocol.h"

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif

/* invalid channel, neither 0, 0xff, nor 1-13 */
#define INVALID_CHANNEL     (20)
#define PAYLOAD_BITS_CNT    (7)
#define PAYLOAD_BITS_MASK   ((1 << PAYLOAD_BITS_CNT) - 1)

int awss_recv_callback_smartconfig(struct parser_res *res);
int awss_ieee80211_smartconfig_process(uint8_t *ieee80211, int len, int link_type,
                                       struct parser_res *res, signed char rssi);

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif
#endif

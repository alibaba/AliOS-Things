/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __AWSS_BEACON_ANNOUNCE_H__
#define __AWSS_BEACON_ANNOUNCE_H__

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#if defined(AWSS_SUPPORT_DISCOVER)
int aws_discover_send_beacon(void);
int aws_discover_callback(uint8_t *mgmt_header, int len, int link_type, struct parser_res *res,
                                  signed char rssi);
#endif

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif
#endif

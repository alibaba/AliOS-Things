/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __DEVMGR_COMMON_UT__
#define __DEVMGR_COMMON_UT__

#include <stdint.h>
#include <unistd.h>
#include "alink_export_zigbee.h"

#if defined(__cplusplus) /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif

int bytes_2_hexstr(const uint8_t *buf, int buf_len, char *hexstr,
                   int hex_buf_len);

int hexstr_2_bytes(const char *hexstr, uint8_t *buf, int *buf_len);

int get_ieeeaddr_by_string(char *ieeeaddr_string,
                           uint8_t extaddr[IEEE_ADDR_BYTES]);

char *get_ieeeaddr_string_by_extaddr(uint8_t extaddr[IEEE_ADDR_BYTES],
                                     char *ieeeaddr_buf, int buf_len);

#if defined(__cplusplus) /* If this is a C++ compiler, use C linkage */
}
#endif

#endif


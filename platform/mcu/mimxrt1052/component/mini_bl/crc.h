/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _crc_h_
#define _crc_h_
#include <stdint.h>
#if defined(__cplusplus) /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif
uint16_t utils_crc16(uint8_t *buf, uint32_t length);

uint32_t utils_crc32(uint8_t *buf, uint32_t length);

#if defined(__cplusplus) /* If this is a C++ compiler, use C linkage */
}
#endif

#endif

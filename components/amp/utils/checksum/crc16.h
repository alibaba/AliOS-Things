/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef _CRC_CRC16_H
#define _CRC_CRC16_H

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus  */

uint16_t utils_crc16(const char *buf, int len);

#ifdef __cplusplus
}
#endif /* __cplusplus  */
#endif
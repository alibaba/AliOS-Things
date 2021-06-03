/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef _CRC_CRC32_H
#define _CRC_CRC32_H

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus  */

uint32_t utils_crc32(const char *s, int len);

#ifdef __cplusplus
}
#endif /* __cplusplus  */
#endif
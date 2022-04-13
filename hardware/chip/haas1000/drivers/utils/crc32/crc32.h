/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __CRC32_H__
#define __CRC32_H__

#ifdef __cplusplus
extern "C" {
#endif

unsigned long crc32(unsigned long crc, const unsigned char *buf, unsigned int len);

#ifdef __cplusplus
}
#endif

#endif


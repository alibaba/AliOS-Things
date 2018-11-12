/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _CRC16_H_
#define _CRC16_H_

#include <stdint.h>

uint16_t checksum_crc16(const uint8_t *data, uint16_t size);

#endif /* _CRC16_H_ */
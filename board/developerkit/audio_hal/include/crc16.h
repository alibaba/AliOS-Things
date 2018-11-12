/*
 *From: andrew cooke <andrew@...>
 *Date: Sat, 3 Aug 2013 15:13:39 -0400
 *Title: CRC16
 */

#ifndef _CRC16_H_
#define _CRC16_H_

#include <stdint.h>

uint16_t checksum_crc16(const uint8_t *data, uint16_t size);

#endif /* _CRC16_H_ */
#ifndef _CRC_H
#define _CRC_H

#ifdef __LINUX_UB2__
#include <stdint.h>
#endif

#if defined(__MTK_7687__)
#include <stdint.h>
#elif defined(__ICOMM_6166__)
#include <stdint.h>
#include "cryptoApi.h"
#endif

#include <unistd.h>
#include <stdio.h>
unsigned short CRC16(const unsigned char* buffer, unsigned int size);
unsigned char crc8(unsigned char *A, unsigned char n);
void make_crc32_table();
uint32_t make_crc(uint32_t crc, unsigned char *string, uint32_t size);

#endif

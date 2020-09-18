/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __RETENTION_RAM_H__
#define __RETENTION_RAM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"


void fillBleBdAddrForOta(uint8_t* bdAddr);
void otaConfigureBleBdAddr(void);


#ifdef __cplusplus
}
#endif

#endif	// #define __RETENTION_RAM_H__


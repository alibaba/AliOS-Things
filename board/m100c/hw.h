/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdint.h>

void HW_Init(void);
uint8_t HW_GetBatteryLevel(void);
void HW_GetUniqueId(uint8_t *id);
uint32_t HW_GetRandomSeed(void);

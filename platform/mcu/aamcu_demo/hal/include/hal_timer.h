/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __HAL_TIMER_AAMCU_H
#define __HAL_TIMER_AAMCU_H

#ifdef __cplusplus
 extern "C" {
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "board.h"

typedef struct{
    uint8_t   port;
    uint32_t  hal_timer;
} TIMER_MAPPING;

extern TIMER_MAPPING TIMER_MAPPING_TABLE[];

#ifdef __cplusplus
}
#endif

#endif /* __HAL_TIMER_AAMCU_H */

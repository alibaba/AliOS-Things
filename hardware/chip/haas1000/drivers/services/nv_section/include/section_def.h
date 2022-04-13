/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __section_def_h__
#define __section_def_h__

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint16_t magic;
    uint16_t version;
    uint32_t crc;
    uint32_t reserved0;
    uint32_t reserved1;
} section_head_t;

#ifdef __cplusplus
}
#endif

#endif

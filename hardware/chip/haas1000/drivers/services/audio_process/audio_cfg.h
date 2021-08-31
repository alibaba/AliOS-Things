/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __AUDIO_CFG_H__
#define __AUDIO_CFG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"

enum AUDIO_PROCESS_TYPE_T {
    AUDIO_PROCESS_TYPE_IIR_EQ = 0,
    AUDIO_PROCESS_TYPE_DRC,
    AUDIO_PROCESS_TYPE_LIMITER,
    AUDIO_PROCESS_TYPE_NUM,
};

struct AUDIO_CFG_T_;
typedef struct AUDIO_CFG_T_ AUDIO_CFG_T;

int sizeof_audio_cfg(void);
int store_audio_cfg_into_audio_section(AUDIO_CFG_T *cfg);
void *load_audio_cfg_from_audio_section(enum AUDIO_PROCESS_TYPE_T type);

#ifdef __cplusplus
}
#endif

#endif

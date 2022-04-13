/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __APP_A2DP_CODEC_H__
#define __APP_A2DP_CODEC_H__

#include "a2dp_codec_sbc.h"
#include "a2dp_codec_aac.h"
#include "a2dp_codec_ldac.h"
#include "a2dp_codec_lhdc.h"
#include "a2dp_codec_opus.h"
#include "a2dp_codec_scalable.h"

#if defined(__cplusplus)
extern "C" {
#endif

int a2dp_codec_init(void);
uint8_t a2dp_codec_confirm_stream_state(uint8_t index, uint8_t old_state, uint8_t new_state);

#if defined(__cplusplus)
}
#endif

#endif /* __APP_A2DP_CODEC_H__ */
/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __A2DP_CODEC_OPUS_H__
#define __A2DP_CODEC_OPUS_H__

#if defined(__cplusplus)
extern "C" {
#endif

#include "avdtp_api.h"
#include "btapp.h"

#if defined(MASTER_USE_OPUS) || defined(ALL_USE_OPUS)
bt_status_t a2dp_codec_opus_init(int index);
#endif

#if defined(__cplusplus)
}
#endif

#endif /* __A2DP_CODEC_OPUS_H__ */
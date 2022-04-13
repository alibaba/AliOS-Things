/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __A2DP_CODEC_SBC_H__
#define __A2DP_CODEC_SBC_H__

#if defined(__cplusplus)
extern "C" {
#endif

#include "avdtp_api.h"

extern btif_avdtp_codec_t a2dp_avdtpcodec;
extern const unsigned char a2dp_codec_elements[];
bt_status_t a2dp_codec_sbc_init(int index);

#if defined(__cplusplus)
}
#endif

#endif /* __A2DP_CODEC_SBC_H__ */
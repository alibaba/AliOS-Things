/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __A2DP_CODEC_LHDC_H__
#define __A2DP_CODEC_LHDC_H__

#if defined(__cplusplus)
extern "C" {
#endif

#include "avdtp_api.h"
#include "codec_lhdc.h"

#if defined(A2DP_LHDC_ON)
extern const unsigned char a2dp_codec_lhdc_elements[A2DP_LHDC_OCTET_NUMBER];
bt_status_t a2dp_codec_lhdc_init(int index);
#endif

#if defined(__cplusplus)
}
#endif

#endif /* __A2DP_CODEC_LHDC_H__ */
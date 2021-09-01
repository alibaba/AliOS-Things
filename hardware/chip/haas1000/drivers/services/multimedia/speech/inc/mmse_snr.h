/***************************************************************************
 *
 * Copyright 2015-2019 BES.
 * All rights reserved. All unpublished rights reserved.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
#ifndef __MMSE_SNR_H__
#define __MMSE_SNR_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    float     snrthd;
	float     energythd;
} MmseSnrConfig;

struct MmseSnrState_;

typedef struct MmseSnrState_ MmseSnrState;

MmseSnrState *mmse_snr_create(int32_t sample_rate, int32_t frame_size, const MmseSnrConfig *cfg);

int32_t mmse_snr_set_config(MmseSnrState *st, const MmseSnrConfig *cfg);

int32_t mmse_snr_process(MmseSnrState *st, short *in, float *snr);

int32_t mmse_snr_destroy(MmseSnrState *st);

#ifdef __cplusplus
}
#endif

#endif
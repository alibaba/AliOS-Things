/***************************************************************************
 *
 * Copyright 2019-2025 BES.
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
#ifndef _AUD_DUMP_H_
#define _AUD_DUMP_H_

#include <stdint.h>
#include "a7_cmd.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AUD_DUMP_IDLE 0
#define AUD_DUMP_INIT 0x12344321
#define AUD_DUMP_READY 0x43211234
#define AUD_DUMP_START 0x12121212
#define AUD_DUMP_MIC12 3 // 011, no ref

#define _AUDIO_DUMP_TAG_ "/audiodump/"
#define _AUDIO_DUMP_SRC_BEFORE_ALG_ "before-alg"
#define _AUDIO_DUMP_SRC_INTER_ALG_ "inter-alg"
#define _AUDIO_DUMP_SRC_AFTER_ALG_ "after-alg"

typedef void (*aud_dump_cb_t)(unsigned char *data, unsigned int len, void *param);

typedef struct
{
    int handle;
    aud_dump_cb_t func;
    void *param;
} multi_audio_dump_t;

void ai_record(unsigned int dump_src, unsigned char *data, unsigned int len);
void aud_dump_cb_register(aud_dump_cb_t cbf, void *param);
void aud_dump_cb_unregister(aud_dump_cb_t cbf, void *param);
int aud_dump_set(AUD_DUMP_SRC_T dump_src, int onoff);
int aud_dump_set_channel(AUD_DUMP_SRC_T dump_src, int ch_map);
int aud_dump_send(void);
void aud_dump_init(void);
uint32_t mic_resample_48_to_16(uint8_t *src, uint8_t *dst, uint32_t len);
void mic_set_samplerate(int mic_samplerate);
int mic_get_samplerate();
int mic_get_handshake_done();
void mic_reset_handshake();
void mic_set_period(int mic_period_ms);
int mic_get_period();

#ifdef __cplusplus
}
#endif

#endif /* _AUD_DUMP_H_ */

/*
 * Copyright (c) 2021 Bestechnic (Shanghai) Co., Ltd. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef __BTPCM_CONFIG_H__
#define __BTPCM_CONFIG_H__

typedef uint32_t (*bt_sco_btpcm_data_callback_t)(uint8_t *buf, uint32_t len);

typedef enum SCO_SAMPRATE {
    SCO_SAMPRATE_8000 = 8000,
    SCO_SAMPRATE_16000 = 16000,
}SCO_SAMPRATE_T;

typedef enum {
    BTPCM_HF_SCO_CODEC_NONE = 0,
    BTPCM_HF_SCO_CODEC_CVSD = 1,
    BTPCM_HF_SCO_CODEC_MSBC = 2,
} btpcm_sco_codec_t;

#ifdef __cplusplus
    extern "C" {
#endif

/*****************************************************************************
 function     : bt_sco_btpcm_data_callback_register
 Description  : register btpcm callback function
 Input        : capture_callback   receive len datas by sco
              : playback_callback  send len datas by sco
 Output       : None
 Date         : 2021/1/15
*****************************************************************************/
void bt_sco_btpcm_data_callback_register(bt_sco_btpcm_data_callback_t capture_callback,
                                                       bt_sco_btpcm_data_callback_t playback_callback);

void btpcm_capture_start(SCO_SAMPRATE_T sample_rate,uint8_t*sco_buff,uint32_t data_size);
void btpcm_playback_start(SCO_SAMPRATE_T sample_rate,uint8_t*sco_buff,uint32_t data_size);
void btpcm_capture_stop(void);
void btpcm_playback_stop(void);
int app_btpcm_set_sco_codec_type(btpcm_sco_codec_t codec_id);
int app_btpcm_get_sco_codec_type(void);
#ifdef __cplusplus
	}
#endif

#endif


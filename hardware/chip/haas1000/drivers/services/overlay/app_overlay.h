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
#ifndef __APP_OVERLAY_H__
#define __APP_OVERLAY_H__

#include "plat_types.h"
#include "hal_overlay.h"

#define app_overlay_load(id)                hal_overlay_load((enum HAL_OVERLAY_ID_T)id)
#define app_overlay_unload(id)              hal_overlay_unload((enum HAL_OVERLAY_ID_T)id)
#define app_overlay_get_text_size(id)       hal_overlay_get_text_size((enum HAL_OVERLAY_ID_T)id)
#define app_overlay_get_text_all_size       hal_overlay_get_text_all_size
#define app_overlay_get_text_address        hal_overlay_get_text_address
#define app_overlay_get_text_free_size(id)  hal_overlay_get_text_free_size((enum HAL_OVERLAY_ID_T)id)
#define app_overlay_get_text_free_addr(id)  hal_overlay_get_text_free_addr((enum HAL_OVERLAY_ID_T)id)
//#define app_overlay_is_used                 hal_overlay_is_used

enum APP_OVERLAY_ID_T {
    APP_OVERLAY_HFP = HAL_OVERLAY_ID_0,
    APP_OVERLAY_A2DP = HAL_OVERLAY_ID_1,
    APP_OVERLAY_FM = HAL_OVERLAY_ID_2,
    APP_OVERLAY_AAC = HAL_OVERLAY_ID_3,
    APP_OVERLAY_A2DP_AAC = HAL_OVERLAY_ID_4,
    APP_OVERLAY_MPA = HAL_OVERLAY_ID_5,
#if	defined(A2DP_SCALABLE_ON)
    APP_OVERLAY_A2DP_SCALABLE= HAL_OVERLAY_ID_6,
#elif defined(A2DP_LHDC_ON)
    APP_OVERLAY_A2DP_LHDC = HAL_OVERLAY_ID_6,
#elif defined(A2DP_LDAC_ON)
	APP_OVERLAY_A2DP_LDAC = HAL_OVERLAY_ID_6,
#else
    APP_OVERLAY_WAV = HAL_OVERLAY_ID_6,
#endif
#ifdef OPUS_IN_OVERLAY
    APP_OVERLAY_OPUS = HAL_OVERLAY_ID_7,
#endif

    APP_OVERLAY_ID_QTY = HAL_OVERLAY_ID_QTY,
    APP_OVERLAY_ID_IN_CFG = HAL_OVERLAY_ID_IN_CFG,
};

#ifdef __cplusplus
extern "C" {
#endif

void app_overlay_select(enum APP_OVERLAY_ID_T id);

void app_overlay_unloadall(void);

void app_overlay_open(void);

void app_overlay_close(void);

enum APP_OVERLAY_ID_T app_get_current_overlay(void);

#ifdef __cplusplus
}
#endif

#endif

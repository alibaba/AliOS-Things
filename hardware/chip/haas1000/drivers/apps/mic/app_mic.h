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
#ifndef __APP_MIC_H__
#define __APP_MIC_H__

#include <stdio.h>
#include <assert.h>

#include "cmsis_os.h"
#include "tgt_hardware.h"
#include "hal_uart.h"
#include "hal_timer.h"
#include "audioflinger.h"
#include "hal_trace.h"
#include "app_bt_stream.h"
#ifdef __cplusplus
extern "C" {
#endif

#include "plat_types.h"

typedef enum {
	MIC_APP_NONE,
	MIC_APP_SOC_CALL,
	MIC_APP_SPEECH_RECO,
	MIC_APP_CSPOTTER,
	MIC_APP_MICRECORD,
	MIC_APP_OTHER,
	MIC_APP_MAX,
}MIC_APP_TYPE;

void app_mic_init();

bool app_mic_start(MIC_APP_TYPE mic_type);

bool app_mic_stop(MIC_APP_TYPE mic_type);

int app_mic_register(MIC_APP_TYPE mic_type, struct AF_STREAM_CONFIG_T *newStream);

int app_mic_deregister(MIC_APP_TYPE mic_type);

bool app_mic_is_registed(MIC_APP_TYPE mic_type);

void app_mic_check(MIC_APP_TYPE mic_type);

MIC_APP_TYPE app_mic_status(void);

#ifdef __cplusplus
}
#endif

#endif//__APP_MIC_H__

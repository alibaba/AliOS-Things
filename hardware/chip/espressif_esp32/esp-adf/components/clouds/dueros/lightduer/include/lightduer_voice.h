/**
 * Copyright (2017) Baidu Inc. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/**
 * File: lightduer_voice.h
 * Auth: Su Hao (suhao@baidu.com)
 * Desc: Light duer voice APIS.
 */

#include "baidu_json.h"

#ifndef BAIDU_DUER_LIGHTDUER_INCLUDE_LIGHTDUER_VOICE_H
#define BAIDU_DUER_LIGHTDUER_INCLUDE_LIGHTDUER_VOICE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum _duer_voice_mode_enum {
    DUER_VOICE_MODE_DEFAULT,
    DUER_VOICE_MODE_CHINESE_TO_ENGLISH,
    DUER_VOICE_MODE_ENGLISH_TO_CHINESE,
    DUER_VOICE_MODE_WCHAT,
    DUER_VOICE_MODE_C2E_BOT,
    DUER_VOICE_MODE_INTERACTIVE_CLASS,
}
duer_voice_mode;

typedef struct _duer_voice_cache_t {
    int head;
    int tail;
    int size;
    char *items[0];
} duer_voice_cache_t;

#ifdef DUER_SECOND_WAKEUP_ENABLE
typedef enum _duer_voice_send_state_enum {
    DUER_VOICE_SEND_STATE_DEFAULT = 1,
    DUER_VOICE_SEND_STATE_CACHE,
    DUER_VOICE_SEND_STATE_WAKEUP,
    DUER_VOICE_SEND_STATE_QUERY,
} duer_voice_send_state;

typedef enum _duer_wakeup_bit_enum {
    DUER_WAKEUP_BIT = 0x01,
    DUER_WAKEUP_EOF_BIT = 0x02,
    DUER_WAKEUP_2ND_WAKEUP_BIT = 0x04,
} duer_wakeup_bit;
#endif//DUER_SECOND_WAKEUP_ENABLE

typedef void (*duer_voice_delay_func)(duer_u32_t);
typedef const char* (*duer_get_dialog_id_func)(void);

void duer_voice_set_delay_threshold(duer_u32_t delay, duer_voice_delay_func);

void duer_voice_set_mode(duer_voice_mode mode);

duer_voice_mode duer_voice_get_mode(void);

int duer_voice_start(int samplerate);

int duer_voice_send(const void *data, size_t size);

int duer_voice_cache(duer_bool cached);

int duer_voice_stop(void);

int duer_voice_terminate(void);

void duer_add_translate_flag(baidu_json *data);

void duer_reg_dialog_id_cb(duer_get_dialog_id_func cb);

#ifdef DUER_SECOND_WAKEUP_ENABLE
void duer_voice_set_send_state(duer_voice_send_state send_state);

/*
 * Restart voice session, but won't clear cached voice.
 * Used for second wakeup, when got new keyword recognition before previous session end.
 */
int duer_voice_restart(void);

/*
 * Send voice with wakeup bit set, to trigger cloud second wakeup.
 *
 * @PARAM wakeup: the wakeup bit needs to be sent.
 */
int duer_voice_send_wakeup(int wakeup);

/*
 * Check if voice started, according to topic id value.
 */
duer_bool duer_voice_is_started();

/*
 * Init second wakeup functionality.
 *
 * @PARAM cache_size: the size of cache of wakeup voice.
 */
void duer_voice_second_wakeup_init(int cache_size);

/*
 * Uninit second wakeup functionality.
 */
void duer_voice_second_wakeup_uninit();

/*
 * Truncate wakeup voice cache.
 *
 * @PARAM count: the cache length after truncated.
 */
void duer_voice_truncate_wakeup_cache(int count);
#endif//DUER_SECOND_WAKEUP_ENABLE

/*
 * topic_id + 1
 *
 * * Return: Other topic_id value
 *           0   Failed
 */
size_t duer_increase_topic_id(void);

int duer_voice_set_speex_quality(int quality);

#ifdef __cplusplus
}
#endif

#endif/*BAIDU_DUER_LIGHTDUER_INCLUDE_LIGHTDUER_VOICE_H*/

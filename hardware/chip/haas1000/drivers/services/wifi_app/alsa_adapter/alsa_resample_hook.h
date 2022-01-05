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
#ifndef __ALSA_RESAMPLE_HOOK_H__
#define __ALSA_RESAMPLE_HOOK_H__
#ifdef ALSA_RESAMPLE_USER_HOOK_EN
/**
 * state:
 * 0: start data stream
 * 1: stop data stream
*/
typedef void (*alsa_resample_user_cb_t)(uint8_t state, void * user_arg);
void alsa_resample_register_user_callback(alsa_resample_user_cb_t cb, void * arg);
#endif
#endif
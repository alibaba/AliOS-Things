/* mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
 *
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
#ifndef ANALOGIN_EX_API_H
#define ANALOGIN_EX_API_H

#include "device.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    ANALOGIN_RX_DMA_COMPLETE     = 0,
}AnalogInCallback;

void analogin_set_user_callback(analogin_t *obj, AnalogInCallback analogin_cb, void(*analogin_callback)(void *));
void analogin_clear_user_callback(analogin_t *obj, AnalogInCallback analogin_cb);
uint8_t analogin_read_u16_dma (analogin_t * obj, uint16_t *buf, uint16_t length);


#ifdef __cplusplus
}
#endif

#endif




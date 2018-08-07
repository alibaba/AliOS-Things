/*
 * Copyright (c) 2014-2016 Alibaba Group. All rights reserved.
 * License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */




#ifndef _IOTX_COMMON_TIMER_H_
#define _IOTX_COMMON_TIMER_H_

#include "iot_import.h"

typedef struct {
    uint32_t time;
} iotx_time_t;


void iotx_time_start(iotx_time_t *timer);

uint32_t utils_time_spend(iotx_time_t *start);

uint32_t iotx_time_left(iotx_time_t *end);

uint32_t utils_time_is_expired(iotx_time_t *timer);

void iotx_time_init(iotx_time_t *timer);

void utils_time_countdown_ms(iotx_time_t *timer, uint32_t millisecond);

uint32_t utils_time_get_ms(void);

#endif /* _IOTX_COMMON_TIMER_H_ */

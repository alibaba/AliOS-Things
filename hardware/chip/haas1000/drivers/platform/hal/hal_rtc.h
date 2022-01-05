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
#ifndef __HAL_RTC_H__
#define __HAL_RTC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "plat_types.h"

struct rtc_time {
        int tm_sec;
        int tm_min;
        int tm_hour;
        int tm_mday;
        int tm_mon;
        int tm_year;
};

typedef void (*HAL_RTC_IRQ_HANDLER_T)(struct rtc_time *time);

int hal_rtc_get(struct rtc_time *time);

int hal_rtc_set(struct rtc_time *time);

int hal_rtc_set_alarm(struct rtc_time *time);

int hal_rtc_clear_alarm(void);

HAL_RTC_IRQ_HANDLER_T hal_rtc_set_irq_handler(HAL_RTC_IRQ_HANDLER_T handler);

#ifdef __cplusplus
}
#endif

#endif

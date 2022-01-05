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
#ifndef __HAL_GPADC_H__
#define __HAL_GPADC_H__

#ifdef __cplusplus
extern "C" {
#endif

#define HAL_GPADC_BAD_VALUE  0xFFFF

enum HAL_GPADC_CHAN_T {
    HAL_GPADC_CHAN_0 = 0,
    HAL_GPADC_CHAN_BATTERY = 1,
    HAL_GPADC_CHAN_2 = 2,
    HAL_GPADC_CHAN_3 = 3,
    HAL_GPADC_CHAN_4 = 4,
    HAL_GPADC_CHAN_5 = 5,
    HAL_GPADC_CHAN_6 = 6,
    HAL_GPADC_CHAN_ADCKEY = 7,
    HAL_GPADC_CHAN_QTY,
};

enum HAL_GPADC_ATP_T {
    HAL_GPADC_ATP_NULL = 0,
    HAL_GPADC_ATP_125US = 125,
    HAL_GPADC_ATP_500US = 500,
    HAL_GPADC_ATP_1MS = 1000,
    HAL_GPADC_ATP_2MS = 2000,
    HAL_GPADC_ATP_5MS = 5000,
    HAL_GPADC_ATP_10MS = 10000,
    HAL_GPADC_ATP_20MS = 20000,
    HAL_GPADC_ATP_30MS = 30000,
    HAL_GPADC_ATP_40MS = 40000,
    HAL_GPADC_ATP_50MS = 50000,
    HAL_GPADC_ATP_60MS = 60000,
    HAL_GPADC_ATP_ONESHOT = -1,
};

enum HAL_ADCKEY_IRQ_T {
    HAL_ADCKEY_IRQ_NONE         = 0,
    HAL_ADCKEY_IRQ_PRESSED      = (1 << 0),
    HAL_ADCKEY_IRQ_RELEASED     = (1 << 1),
    HAL_ADCKEY_IRQ_BOTH         = (HAL_ADCKEY_IRQ_PRESSED | HAL_ADCKEY_IRQ_RELEASED),
};

enum HAL_ADCKEY_IRQ_STATUS_T {
    HAL_ADCKEY_ADC_VALID    = (1 << 0),
    HAL_ADCKEY_RELEASED     = (1 << 1),
    HAL_ADCKEY_PRESSED      = (1 << 2),
    HAL_ADCKEY_ERR0         = (1 << 3),
    HAL_ADCKEY_ERR1         = (1 << 4),
};

typedef uint16_t HAL_GPADC_MV_T;

typedef void (*HAL_GPADC_EVENT_CB_T)(uint16_t raw_val, HAL_GPADC_MV_T volt);
typedef void (*HAL_ADCKEY_EVENT_CB_T)(enum HAL_ADCKEY_IRQ_STATUS_T irq_status, uint16_t raw_val);

int hal_gpadc_open(enum HAL_GPADC_CHAN_T channel, enum HAL_GPADC_ATP_T atp, HAL_GPADC_EVENT_CB_T cb);

bool hal_gpadc_get_volt(enum HAL_GPADC_CHAN_T ch, HAL_GPADC_MV_T *volt);

int hal_gpadc_close(enum HAL_GPADC_CHAN_T channel);

void hal_gpadc_sleep(void);

void hal_gpadc_wakeup(void);

void hal_adckey_set_irq_handler(HAL_ADCKEY_EVENT_CB_T cb);

int hal_adckey_set_irq(enum HAL_ADCKEY_IRQ_T type);

#ifdef __cplusplus
	}
#endif

#endif//__FMDEC_H__

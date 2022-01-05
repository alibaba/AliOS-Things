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
#ifndef __HAL_SENSOR_HUB_H__
#define __HAL_SENSOR_HUB_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_dma.h"
#include "hal_gpio.h"

enum HAL_SENSOR_ENGINE_ID_T {
    HAL_SENSOR_ENGINE_ID_0 = 0,
    HAL_SENSOR_ENGINE_ID_1,
    HAL_SENSOR_ENGINE_ID_2,
    HAL_SENSOR_ENGINE_ID_3,

    HAL_SENSOR_ENGINE_ID_QTY,
};

enum HAL_SENSOR_ENGINE_TRIGGER_T {
    HAL_SENSOR_ENGINE_TRIGGER_TIMER = 0,
    HAL_SENSOR_ENGINE_TRIGGER_GPIO,

    HAL_SENSOR_ENGINE_TRIGGER_QTY,
};

enum HAL_SENSOR_ENGINE_DEVICE_T {
    HAL_SENSOR_ENGINE_DEVICE_I2C0 = 0,
    HAL_SENSOR_ENGINE_DEVICE_I2C1,
    HAL_SENSOR_ENGINE_DEVICE_SPI0 = 4,
    HAL_SENSOR_ENGINE_DEVICE_SPI1,
    HAL_SENSOR_ENGINE_DEVICE_SPI2,
};

typedef void (*HAL_SENSOR_ENG_HANDLER_T)(enum HAL_SENSOR_ENGINE_ID_T id, enum HAL_SENSOR_ENGINE_DEVICE_T device, const uint8_t *buf, uint32_t len);

struct HAL_SENSOR_ENGINE_CFG_T {
    enum HAL_SENSOR_ENGINE_ID_T id;
    enum HAL_SENSOR_ENGINE_DEVICE_T device;
    enum HAL_SENSOR_ENGINE_TRIGGER_T trigger_type;
    enum HAL_GPIO_PIN_T trigger_gpio;
    uint32_t period_us;
    struct HAL_DMA_CH_CFG_T *tx_dma_cfg;
    struct HAL_DMA_CH_CFG_T *rx_dma_cfg;
    uint16_t device_address;
    uint16_t rx_burst_len;
    uint8_t rx_burst_cnt;
    uint8_t data_to_vad;
    HAL_SENSOR_ENG_HANDLER_T handler;
};

int hal_sensor_engine_open(const struct HAL_SENSOR_ENGINE_CFG_T *cfg);
int hal_sensor_engine_close(enum HAL_SENSOR_ENGINE_ID_T id);

void hal_sensor_process_rx_buffer(void);

#ifdef __cplusplus
}
#endif

#endif



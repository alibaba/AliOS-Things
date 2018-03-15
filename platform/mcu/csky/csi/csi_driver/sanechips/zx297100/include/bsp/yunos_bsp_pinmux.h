/*
 * Copyright (C) 2016 YunOS Project. All rights reserved.
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

#ifndef YUNOS_BSP_PINMUX_H
#define YUNOS_BSP_PINMUX_H

#include <stdint.h>
#include <yunos_bsp_errno.h>

/**
 * module api error number related definitions: 0x0000AB00
 */
#define BSP_API_PINMUX_SET            0x01
#define BSP_API_PINMUX_RELEASE        0x02


/**
 * note: definition of devices is to be continued
 */
typedef enum pinmux_device_s {
    PINMUX_DEV_GPIO           = 0,    ///< GPIO is the lowest priority functionality, only other functionality released it can be valid. all GPIO share the same device type
    PINMUX_DEV_UART,
    PINMUX_DEV_I2C,
    PINMUX_DEV_SPI,
    PINMUX_DEV_PWM,
    PINMUX_DEV_CAN,
    PINMUX_DEV_IIS,
    PINMUX_DEV_USB,
    PINMUX_DEV_ADC,
    PINMUX_DEV_SPIFLASH,
    PINMUX_DEV_SDIO,
    PINMUX_DEV_SDMMC,
    PINMUX_DEV_SPDIF_I,
    PINMUX_DEV_SPDIF_O,
    PINMUX_DEV_GMAC,                


    PINMUX_DEV_NUM    ///< not a real device ,but for record the count of devices
} pinmux_device_t;

/**
 * Set pin mux functionality
 * @param[in]   dev         pin mux functionality, valid value refers pinmux_device_t
 * @param[in]   pin_num     pin number in pin_list
 * @param[in]   pin_list    pin list
 * @return      0 - success , other - error code
 */
int yunos_bsp_pinmux_set(pinmux_device_t dev,uint8_t pin_num, const uint16_t *pin_list);


/**
 * Release pin mux functionality
 * @param[in]   dev         pin mux functionality, valid value refers pinmux_device_t
 * @param[in]   pin_num     pin number in pin_list
 * @param[in]   pin_list    pin list
 * @return      0 - success , other - error code
 */
int yunos_bsp_pinmux_release(pinmux_device_t dev,uint8_t pin_num, const uint16_t *pin_list);

#endif /* YUNOS_BSP_PINMUX_H */

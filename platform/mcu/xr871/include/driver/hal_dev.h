/**
  * @file  hal_dev.h
  * @author  XRADIO IOT WLAN Team
  */

/*
 * Copyright (C) 2017 XRADIO TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of XRADIO TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _DRIVER_HAL_DEV_H_
#define _DRIVER_HAL_DEV_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief HAL device number
 *
 * HAL device number is made up of major number and minor number.
 *     - Major number is defined as HAL_DEV_MAJOR_XXX
 *     - minor number is defined by the specific driver
 */
typedef uint32_t HAL_Dev_t;

#define HAL_DEV_MINOR_BITS  16
#define HAL_DEV_MINOR_MASK  ((1U << HAL_DEV_MINOR_BITS) - 1)

/** @brief Get major number from device number */
#define HAL_DEV_MAJOR(dev)  ((uint32_t)((dev) >> HAL_DEV_MINOR_BITS))

/** @brief Get minor number from device number */
#define HAL_DEV_MINOR(dev)  ((uint32_t)((dev) & HAL_DEV_MINOR_MASK))

/** @brief Make device number from major number and minor number */
#define HAL_MKDEV(major, minor) \
    (((HAL_Dev_t)(major) << HAL_DEV_MINOR_BITS) | \
     ((HAL_Dev_t)(minor) & HAL_DEV_MINOR_MASK))

/**
 * @brief Major number of device
 */
enum {
    /* peripheral interface of chip */
    HAL_DEV_MAJOR_UART = 0U,
    HAL_DEV_MAJOR_I2C,
    HAL_DEV_MAJOR_SPI,
    HAL_DEV_MAJOR_IRRX,
    HAL_DEV_MAJOR_IRTX,
    HAL_DEV_MAJOR_I2S,
    HAL_DEV_MAJOR_DMIC,
    HAL_DEV_MAJOR_ADC,
    HAL_DEV_MAJOR_PWM,
    HAL_DEV_MAJOR_FLASHC,   /* FLASH controller interface */
    HAL_DEV_MAJOR_SDC,
    HAL_DEV_MAJOR_CSI,

    /* external component */
    HAL_DEV_MAJOR_FLASH = 100U,
    HAL_DEV_MAJOR_AUDIO_CODEC,
};

#ifdef __cplusplus
}
#endif

#endif /* _DRIVER_HAL_DEV_H_ */

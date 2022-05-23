/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2018 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on all ESPRESSIF SYSTEMS products, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef _TOUCH_DEV_H_
#define _TOUCH_DEV_H_

#include "sys/queue.h"
#include "audio_error.h"
#include "audio_common.h"
#include "esp_peripherals.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief      Touch pad selection
 */
typedef enum {
    TOUCH_PAD_SEL0 = BIT(0),
    TOUCH_PAD_SEL1 = BIT(1),
    TOUCH_PAD_SEL2 = BIT(2),
    TOUCH_PAD_SEL3 = BIT(3),
    TOUCH_PAD_SEL4 = BIT(4),
    TOUCH_PAD_SEL5 = BIT(5),
    TOUCH_PAD_SEL6 = BIT(6),
    TOUCH_PAD_SEL7 = BIT(7),
    TOUCH_PAD_SEL8 = BIT(8),
    TOUCH_PAD_SEL9 = BIT(9),
} esp_touch_pad_sel_t;

/**
 * @brief   The Touch peripheral configuration
 */
typedef struct {
    int touch_mask;             /*!< Touch pad mask using for this Touch peripheral, ex: TOUCH_PAD_SEL0 | TOUCH_PAD_SEL1  */
    int tap_threshold_percent;  /*!< Tap threshold percent, Tap event will be determined if the percentage value is less than the non-touch value */
    int long_tap_time_ms;       /*!< Long tap duration in milliseconds, default is 2000ms, PERIPH_TOUCH_LONG_TAP will be occurred if TAP and time hold longer than this value */
} periph_touch_cfg_t;

/**
 * @brief      Peripheral touch event id
 */
typedef enum {
    PERIPH_TOUCH_UNCHANGE = 0,  /*!< No event */
    PERIPH_TOUCH_TAP,           /*!< When touch pad is tapped */
    PERIPH_TOUCH_RELEASE,       /*!< When touch pad is released after tap */
    PERIPH_TOUCH_LONG_TAP,      /*!< When touch pad is tapped and held after `long_tap_time_ms` time */
    PERIPH_TOUCH_LONG_RELEASE,  /*!< When touch pad is released after long tap */
} periph_touch_event_id_t;


/**
 * @brief      Create the touch peripheral handle for esp_peripherals
 *
 * @note       The handle was created by this function automatically destroy when `esp_periph_destroy` is called
 *
 * @param      config  The configuration
 *
 * @return     The esp peripheral handle
 */
esp_periph_handle_t periph_touch_init(periph_touch_cfg_t* config);

#ifdef __cplusplus
}
#endif

#endif

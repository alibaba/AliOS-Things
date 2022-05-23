/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2020 <ESPRESSIF SYSTEMS (SHANGHAI) CO., LTD>
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
#ifndef _PERIPH_WS2812_DRIVER_H
#define _PERIPH_WS2812_DRIVER_H

#include "esp_peripherals.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   The RGB peripheral value
 */
typedef uint32_t periph_rgb_value;

#define make_rgb_value(x, y, z)  (((int)(z) << 16) + ((int)(y) << 8 )+ (x))

#define LED2812_COLOR_BLACK   make_rgb_value(0,   0,  0)
#define LED2812_COLOR_BLUE    make_rgb_value(0,   0,   255)
#define LED2812_COLOR_GREEN   make_rgb_value(0,   255, 0)
#define LED2812_COLOR_CYAN    make_rgb_value(0,   255, 255)
#define LED2812_COLOR_RED     make_rgb_value(255, 0,   0)
#define LED2812_COLOR_PURPLE  make_rgb_value(255, 0,   255)
#define LED2812_COLOR_YELLOW  make_rgb_value(255, 255, 0)
#define LED2812_COLOR_WHITE   make_rgb_value(255, 255, 255)
#define LED2812_COLOR_ORANGE  make_rgb_value(255, 165, 0)

/**
 * @brief   The ws2812 peripheral configuration
 */
typedef struct {
    int      gpio_num;     /*!< The GPIO number of ws2812*/
    int      led_num;      /*!< The number of ws2812 */
}periph_ws2812_cfg_t;

/**
 * @brief   The periph ws2812 mode
 */
typedef enum {
    PERIPH_WS2812_BLINK,
    PERIPH_WS2812_FADE,
    PERIPH_WS2812_ONE,
} periph_ws2812_mode_t;

/**
 * @brief   The periph ws2812 control config
 */
typedef struct periph_ws2812_ctrl_cfg {
    periph_rgb_value         color;          /*!< The RGB  value */
    uint32_t                 time_on_ms;     /*!< The time on milliseconds, suggest min is 100 ms */
    uint32_t                 time_off_ms;    /*!< The time off milliseconds, suggest min is 100 ms */
    uint32_t                 loop;           /*!< The times offloop */
    periph_ws2812_mode_t     mode;           /*!< ws2812 mode (setting color, blink or fade) */
} periph_ws2812_ctrl_cfg_t;

/**
 * @brief      Create the ws2812 peripheral handle for esp_peripherals
 *
 * @note       The handle was created by this function automatically destroy when `esp_periph_destroy` is called
 *
 * @param      config  The configuration
 *
 * @return     The esp peripheral handle
 */
esp_periph_handle_t periph_ws2812_init(periph_ws2812_cfg_t *config);

/**
 * @brief      Control ws2812 Peripheral
 *
 * @param[in]  periph            The ws2812 periph
 * @param[in]  control_cfg       The ws2812 color config
 * @param[in]  ctx               The ctx
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t periph_ws2812_control(esp_periph_handle_t periph, periph_ws2812_ctrl_cfg_t *control_cfg, void *ctx);

/**
 * @brief      Stop ws2812
 *
 * @param[in]  periph    The periph
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t periph_ws2812_stop(esp_periph_handle_t periph);


#ifdef __cplusplus
}
#endif

#endif

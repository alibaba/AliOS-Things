/**
 * Copyright (c) 2017 - 2017, Nordic Semiconductor ASA
 * 
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 * 
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 * 
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 * 
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 * 
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */

#ifndef HAL_LCD_H__
#define HAL_LCD_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef uint32_t ret_code_t;

/** @file
 *
 * @defgroup hal_lcd LCD Library
 * @{
 * @ingroup hal_gfx
 *
 * @brief This module defines generic API for LCDs.
 */

/**
 * @brief Driver state.
 */
typedef enum
{
    HAL_DRV_STATE_UNINITIALIZED, /**< Uninitialized. */
    HAL_DRV_STATE_INITIALIZED,   /**< Initialized but powered off. */
    HAL_DRV_STATE_POWERED_ON
} hal_drv_state_t;

/**
 * @brief Enumerator with available rotations.
 */
typedef enum
{
    HAL_LCD_ROTATE_0 = 0, /**< Rotate 0 degrees, clockwise. */
    HAL_LCD_ROTATE_90,    /**< Rotate 90 degrees, clockwise. */
    HAL_LCD_ROTATE_180,   /**< Rotate 180 degrees, clockwise. */
    HAL_LCD_ROTATE_270    /**< Rotate 270 degrees, clockwise. */
} hal_lcd_rotation_t;

/**
 * @brief LCD instance control block.
 */
typedef struct
{
    hal_drv_state_t state;       /**< State of LCD instance. */
    uint16_t height;              /**< LCD height. */
    uint16_t width;               /**< LCD width. */
    hal_lcd_rotation_t rotation; /**< LCD rotation. */
} lcd_cb_t;

/**
 * @brief LCD instance type.
 *
 * This structure provides generic API for LCDs.
 */
typedef struct
{
    /**
     * @brief Function for initializing the LCD controller.
     */
    uint32_t (*lcd_init)(void);

    /**
     * @brief Function for uninitializing the LCD controller.
     */
    void (*lcd_uninit)(void);

    /**
     * @brief Function for drawing a single pixel.
     *
     * @param[in] x             Horizontal coordinate of the pixel.
     * @param[in] y             Vertical coordinate of the pixel.
     * @param[in] color         Color of the pixel in LCD accepted format.
     */
    void (*lcd_pixel_draw)(uint16_t x, uint16_t y, uint32_t color);

    /**
     * @brief Function for drawing a filled rectangle.
     *
     * @param[in] x             Horizontal coordinate of the point where to start drawing the rectangle.
     * @param[in] y             Vertical coordinate of the point where to start drawing the rectangle.
     * @param[in] width         Width of the image.
     * @param[in] height        Height of the image.
     * @param[in] color         Color with which to fill the rectangle in LCD accepted format.
     */
    void (*lcd_rect_draw)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);

    /**
     * @brief Function for displaying data from an internal frame buffer.
     *
     * This function may be used when functions for drawing do not write directly to
     * LCD but to an internal frame buffer. It could be implemented to write data from this
     * buffer to LCD.
    */
    void (*lcd_display)(void);

    /**
     * @brief Function for rotating the screen.
     *
     * @param[in] rotation      Rotation as enumerated value.
     */
    void (*lcd_rotation_set)(hal_lcd_rotation_t rotation);

    /**
     * @brief Function for setting inversion of colors on the screen.
     *
     * @param[in] invert        If true, inversion will be set.
     */
    void (*lcd_display_invert)(bool invert);

    void (*lcd_frame_draw)(uint8_t *frame);
    void (*lcd_area_draw)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t *frame, uint32_t areaSize);
    void (*lcd_line_draw)(uint16_t x, uint16_t y, uint16_t length, uint8_t dir, uint32_t color);
    /**
     * @brief Pointer to the LCD instance control block.
     */
    lcd_cb_t *p_lcd_cb;
} hal_lcd_t;

/* @} */

#endif // HAL_LCD_H__

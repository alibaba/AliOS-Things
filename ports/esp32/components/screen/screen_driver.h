// Copyright 2020 Espressif Systems (Shanghai) Co. Ltd.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _IOT_SCREEN_DRIVER_H_
#define _IOT_SCREEN_DRIVER_H_

#include "scr_interface_driver.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
 * @brief Define all screen direction
 *
 */
    typedef enum
    {
        /* @---> X
       |
       Y
    */
        SCR_DIR_LRTB, /**< From left to right then from top to bottom, this consider as the original direction of the screen */

        /*  Y
        |
        @---> X
    */
        SCR_DIR_LRBT, /**< From left to right then from bottom to top */

        /* X <---@
             |
             Y
    */
        SCR_DIR_RLTB, /**< From right to left then from top to bottom */

        /*       Y
             |
       X <---@
    */
        SCR_DIR_RLBT, /**< From right to left then from bottom to top */

        /* @---> Y
       |
       X
    */
        SCR_DIR_TBLR, /**< From top to bottom then from left to right */

        /*  X
        |
        @---> Y
    */
        SCR_DIR_BTLR, /**< From bottom to top then from left to right */

        /* Y <---@
             |
             X
    */
        SCR_DIR_TBRL, /**< From top to bottom then from right to left */

        /*       X
             |
       Y <---@
    */
        SCR_DIR_BTRL, /**< From bottom to top then from right to left */

        SCR_DIR_MAX,

        /* Another way to represent rotation with 3 bit*/
        SCR_MIRROR_X = 0x40, /**< Mirror X-axis */
        SCR_MIRROR_Y = 0x20, /**< Mirror Y-axis */
        SCR_SWAP_XY = 0x80,  /**< Swap XY axis */
    } scr_dir_t;

    /**
 * @brief The types of colors that can be displayed on the screen
 * 
 */
    typedef enum
    {
        SCR_COLOR_TYPE_MONO,   /**< The screen is monochrome */
        SCR_COLOR_TYPE_GRAY,   /**< The screen is gray */
        SCR_COLOR_TYPE_RGB565, /**< The screen is colorful */
    } scr_color_type_t;

    /**
 * @brief All supported screen controllers
 *
 */
    typedef enum
    {
        /* color screen */
        SCREEN_CONTROLLER_ILI9341,
        SCREEN_CONTROLLER_ILI9806,
        SCREEN_CONTROLLER_ILI9486,
        SCREEN_CONTROLLER_ILI9488,
        SCREEN_CONTROLLER_NT35510,
        SCREEN_CONTROLLER_RM68120,
        SCREEN_CONTROLLER_ST7789,
        SCREEN_CONTROLLER_ST7796,
        SCREEN_CONTROLLER_SSD1351,
        SCREEN_CONTROLLER_SSD1963,

        /* monochrome screen */
        SCREEN_CONTROLLER_SSD1306,
        SCREEN_CONTROLLER_SSD1307,
        SCREEN_CONTROLLER_SSD1322,

    } scr_controller_t;

    /**
 * @brief configuration of screen controller
 * 
 */
    typedef struct
    {
        scr_interface_driver_t *interface_drv; /*!< Interface driver for screen */
        int8_t pin_num_rst;                    /*!< Pin to hardreset LCD*/
        int8_t pin_num_bckl;                   /*!< Pin for control backlight */
        uint8_t rst_active_level;              /*!< Reset pin active level */
        uint8_t bckl_active_level;             /*!< Backlight active level */
        uint16_t width;                        /*!< Screen width */
        uint16_t height;                       /*!< Screen height */
        uint16_t offset_hor;                   /*!< Offset of horizontal */
        uint16_t offset_ver;                   /*!< Offset of vertical */
        scr_dir_t rotate;                      /*!< Screen rotate direction */
    } scr_controller_config_t;

    /**
 * @brief Information of screen
 * 
 */
    typedef struct
    {
        uint16_t width;              /*!< Current screen width, it may change when apply to rotate */
        uint16_t height;             /*!< Current screen height, it may change when apply to rotate */
        scr_dir_t dir;               /*!< Current screen direction */
        scr_color_type_t color_type; /*!< Color type of the screen, See scr_color_type_t struct */
        uint8_t bpp;                 /*!< Bits per pixel */
        const char *name;            /*!< Name of the screen */
    } scr_info_t;

    /**
 * @brief Define a screen common function
 *
 */
    typedef struct
    {
        /**
    * @brief Initialize screen
    *
    * @param lcd_conf Pointer to a structure with lcd config arguments. see struct scr_controller_config_t
    *
    * @return
    *      - ESP_OK on success
    *      - ESP_FAIL Driver not installed
    */
        esp_err_t (*init)(const scr_controller_config_t *lcd_conf);

        /**
    * @brief Deinitialize screen
    *
    * @return
    *      - ESP_OK on success
    *      - ESP_FAIL Deinitialize failed
    *      - ESP_ERR_NOT_SUPPORTED unsupported
    */
        esp_err_t (*deinit)(void);

        /**
    * @brief Set screen direction of rotation
    *
    * @param dir Pointer to a scr_dir_t structure. 
    * You can set the direction in two ways, for example, set it to "SCR_DIR_LRBT" or "SCR_MIRROR_Y", They are the same, depending on which expression you want to use
    * 
    * @note Not all screens support eight directions, it depends on the screen controller.
    * 
    * @return
    *      - ESP_OK on success
    *      - ESP_FAIL Failed
    */
        esp_err_t (*set_direction)(scr_dir_t dir);

        /**
    * @brief Set screen window
    *
    * @param x0 Starting point in X direction
    * @param y0 Starting point in Y direction
    * @param x1 End point in X direction
    * @param y1 End point in Y direction
    * 
    * @note When the BPP of the screen controller is less than 8, the coordinate value is limited to a multiple of some number
    *
    * @return
    *      - ESP_OK on success
    *      - ESP_FAIL Failed
    */
        esp_err_t (*set_window)(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);

        /**
    * @brief Write a RAM data
    *
    * @param color New color of a pixel
    *
    * @return
    *      - ESP_OK on success
    *      - ESP_FAIL Failed
    */
        esp_err_t (*write_ram_data)(uint16_t color);

        /**
    * @brief Draw one pixel in screen with color
    *
    * @param x X co-ordinate of set orientation
    * @param y Y co-ordinate of set orientation
    * @param color New color of the pixel
    *
    * @return
    *      - ESP_OK on success
    *      - ESP_FAIL Failed
    */
        esp_err_t (*draw_pixel)(uint16_t x, uint16_t y, uint16_t color);

        /**
    * @brief Fill the pixels on LCD screen with bitmap
    *
    * @param x Starting point in X direction
    * @param y Starting point in Y direction
    * @param w width of image in bitmap array
    * @param h height of image in bitmap array
    * @param bitmap pointer to bitmap array
    *
    * @return
    *      - ESP_OK on success
    *      - ESP_FAIL Failed
    */
        esp_err_t (*draw_bitmap)(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t *bitmap);
        esp_err_t (*draw_row_bitmap)(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t *bitmap);
        /**
    * @brief Get screen information
    *
    * @param info Pointer to a scr_info_t structure.
    *
    * @return
    *      - ESP_OK on success
    *      - ESP_FAIL Failed
    */
        esp_err_t (*get_info)(scr_info_t *info);
    } scr_driver_t;

    /**
 * @brief Find a screen driver
 *
 * @param controller Screen controller to initialize
 * @param out_screen Pointer to a screen driver
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG Arguments is NULL.
 *      - ESP_ERR_NOT_FOUND Screen controller was not found.
 */
    esp_err_t scr_find_driver(scr_controller_t controller, scr_driver_t *out_screen);

#ifdef __cplusplus
}
#endif

#endif

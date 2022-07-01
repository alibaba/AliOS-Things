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

#include "sdkconfig.h"
#include <string.h>
#include "screen_driver.h"
#include "esp_log.h"

static const char *TAG = "screen driver";

#define LCD_CHECK(a, str, ret)  if(!(a)) {                           \
        ESP_LOGE(TAG,"%s:%d (%s):%s", __FILE__, __LINE__, __FUNCTION__, str);   \
        return (ret);                                                           \
    }

/**
 * Define screen instance
 */
/**< Colorful screen */
#ifdef CONFIG_LCD_DRIVER_SCREEN_CONTROLLER_ILI9341
extern scr_driver_t lcd_ili9341_default_driver;
#endif
#ifdef CONFIG_LCD_DRIVER_SCREEN_CONTROLLER_ILI9486
extern scr_driver_t lcd_ili9486_default_driver;
#endif
#ifdef CONFIG_LCD_DRIVER_SCREEN_CONTROLLER_ILI9806
extern scr_driver_t lcd_ili9806_default_driver;
#endif
#ifdef CONFIG_LCD_DRIVER_SCREEN_CONTROLLER_ILI9488
extern scr_driver_t lcd_ili9488_default_driver;
#endif
#ifdef CONFIG_LCD_DRIVER_SCREEN_CONTROLLER_NT35510
extern scr_driver_t lcd_nt35510_default_driver;
#endif
#ifdef CONFIG_LCD_DRIVER_SCREEN_CONTROLLER_RM68120
extern scr_driver_t lcd_rm68120_default_driver;
#endif
#ifdef CONFIG_LCD_DRIVER_SCREEN_CONTROLLER_SSD1351
extern scr_driver_t lcd_ssd1351_default_driver;
#endif
#ifdef CONFIG_LCD_DRIVER_SCREEN_CONTROLLER_SSD1963
extern scr_driver_t lcd_ssd1963_default_driver;
#endif
#ifdef CONFIG_LCD_DRIVER_SCREEN_CONTROLLER_ST7789
extern scr_driver_t lcd_st7789_default_driver;
#endif
#ifdef CONFIG_LCD_DRIVER_SCREEN_CONTROLLER_ST7796
extern scr_driver_t lcd_st7796_default_driver;
#endif

/**< Monochrome screen */
#ifdef CONFIG_LCD_DRIVER_SCREEN_CONTROLLER_SSD1306
extern scr_driver_t lcd_ssd1306_default_driver;
#endif
#ifdef CONFIG_LCD_DRIVER_SCREEN_CONTROLLER_SSD1307
extern scr_driver_t lcd_ssd1307_default_driver;
#endif
#ifdef CONFIG_LCD_DRIVER_SCREEN_CONTROLLER_SSD1322
extern scr_driver_t lcd_ssd1322_default_driver;
#endif

esp_err_t scr_find_driver(scr_controller_t controller, scr_driver_t *out_screen)
{
    LCD_CHECK(NULL != out_screen, "Pointer of screen is invalid", ESP_ERR_INVALID_ARG);

    esp_err_t ret = ESP_OK;
    switch (controller) {
        /**< Colorful screen */
#ifdef CONFIG_LCD_DRIVER_SCREEN_CONTROLLER_ILI9341
    case SCREEN_CONTROLLER_ILI9341:
        *out_screen = lcd_ili9341_default_driver;
        break;
#endif
#ifdef CONFIG_LCD_DRIVER_SCREEN_CONTROLLER_ILI9806
    case SCREEN_CONTROLLER_ILI9806:
        *out_screen = lcd_ili9806_default_driver;
        break;
#endif
#ifdef CONFIG_LCD_DRIVER_SCREEN_CONTROLLER_ILI9486
    case SCREEN_CONTROLLER_ILI9486:
        *out_screen = lcd_ili9486_default_driver;
        break;
#endif
#ifdef CONFIG_LCD_DRIVER_SCREEN_CONTROLLER_ILI9488
    case SCREEN_CONTROLLER_ILI9488:
        *out_screen = lcd_ili9488_default_driver;
        break;
#endif
#ifdef CONFIG_LCD_DRIVER_SCREEN_CONTROLLER_NT35510
    case SCREEN_CONTROLLER_NT35510:
        *out_screen = lcd_nt35510_default_driver;
        break;
#endif
#ifdef CONFIG_LCD_DRIVER_SCREEN_CONTROLLER_RM68120
    case SCREEN_CONTROLLER_RM68120:
        *out_screen = lcd_rm68120_default_driver;
        break;
#endif
#ifdef CONFIG_LCD_DRIVER_SCREEN_CONTROLLER_ST7789
    case SCREEN_CONTROLLER_ST7789:
        *out_screen = lcd_st7789_default_driver;
        break;
#endif
#ifdef CONFIG_LCD_DRIVER_SCREEN_CONTROLLER_ST7796
    case SCREEN_CONTROLLER_ST7796:
        *out_screen = lcd_st7796_default_driver;
        break;
#endif
#ifdef CONFIG_LCD_DRIVER_SCREEN_CONTROLLER_SSD1351
    case SCREEN_CONTROLLER_SSD1351:
        *out_screen = lcd_ssd1351_default_driver;
        break;
#endif
#ifdef CONFIG_LCD_DRIVER_SCREEN_CONTROLLER_SSD1963
    case SCREEN_CONTROLLER_SSD1963:
        *out_screen = lcd_ssd1963_default_driver;
        break;
#endif

        /**< Monochrome screen */
#ifdef CONFIG_LCD_DRIVER_SCREEN_CONTROLLER_SSD1306
    case SCREEN_CONTROLLER_SSD1306:
        *out_screen = lcd_ssd1306_default_driver;
        break;
#endif
#ifdef CONFIG_LCD_DRIVER_SCREEN_CONTROLLER_SSD1307
    case SCREEN_CONTROLLER_SSD1307:
        *out_screen = lcd_ssd1307_default_driver;
        break;
#endif
#ifdef CONFIG_LCD_DRIVER_SCREEN_CONTROLLER_SSD1322
    case SCREEN_CONTROLLER_SSD1322:
        *out_screen = lcd_ssd1322_default_driver;
        break;
#endif
    default:
        ESP_LOGE(TAG, "Not support screen controller, it may not be enabled in menuconfig");
        ret = ESP_ERR_NOT_FOUND;
        break;
    }
    return ret;
}

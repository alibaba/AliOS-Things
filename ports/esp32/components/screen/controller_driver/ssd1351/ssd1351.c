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
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "screen_driver.h"
#include "screen_utility.h"
#include "ssd1351.h"

static const char *TAG = "oled ssd1351";

#define LCD_CHECK(a, str, ret)  if(!(a)) {                                      \
        ESP_LOGE(TAG,"%s:%d (%s):%s", __FILE__, __LINE__, __FUNCTION__, str);   \
        return (ret);                                                                   \
    }

#define LCD_NAME "OLED SSD1351"
#define LCD_BPP  16

#define SSD1351_RESOLUTION_HOR 128
#define SSD1351_RESOLUTION_VER 128

/** commands */
#define SSD1351_CMD_SETCOLUMN 0x15      ///< See datasheet
#define SSD1351_CMD_SETROW 0x75         ///< See datasheet
#define SSD1351_CMD_WRITERAM 0x5C       ///< See datasheet
#define SSD1351_CMD_READRAM 0x5D        ///< Not currently used
#define SSD1351_CMD_SETREMAP 0xA0       ///< See datasheet
#define SSD1351_CMD_STARTLINE 0xA1      ///< See datasheet
#define SSD1351_CMD_DISPLAYOFFSET 0xA2  ///< See datasheet
#define SSD1351_CMD_DISPLAYALLOFF 0xA4  ///< Not currently used
#define SSD1351_CMD_DISPLAYALLON 0xA5   ///< Not currently used
#define SSD1351_CMD_NORMALDISPLAY 0xA6  ///< See datasheet
#define SSD1351_CMD_INVERTDISPLAY 0xA7  ///< See datasheet
#define SSD1351_CMD_FUNCTIONSELECT 0xAB ///< See datasheet
#define SSD1351_CMD_DISPLAYOFF 0xAE     ///< See datasheet
#define SSD1351_CMD_DISPLAYON 0xAF      ///< See datasheet
#define SSD1351_CMD_PRECHARGE 0xB1      ///< See datasheet
#define SSD1351_CMD_DISPLAYENHANCE 0xB2 ///< Not currently used
#define SSD1351_CMD_CLOCKDIV 0xB3       ///< See datasheet
#define SSD1351_CMD_SETVSL 0xB4         ///< See datasheet
#define SSD1351_CMD_SETGPIO 0xB5        ///< See datasheet
#define SSD1351_CMD_PRECHARGE2 0xB6     ///< See datasheet
#define SSD1351_CMD_SETGRAY 0xB8        ///< Not currently used
#define SSD1351_CMD_USELUT 0xB9         ///< Not currently used
#define SSD1351_CMD_PRECHARGELEVEL 0xBB ///< Not currently used
#define SSD1351_CMD_VCOMH 0xBE          ///< See datasheet
#define SSD1351_CMD_CONTRASTABC 0xC1    ///< See datasheet
#define SSD1351_CMD_CONTRASTMASTER 0xC7 ///< See datasheet
#define SSD1351_CMD_MUXRATIO 0xCA       ///< See datasheet
#define SSD1351_CMD_COMMANDLOCK 0xFD    ///< See datasheet
#define SSD1351_CMD_HORIZSCROLL 0x96    ///< Not currently used
#define SSD1351_CMD_STOPSCROLL 0x9E     ///< Not currently used
#define SSD1351_CMD_STARTSCROLL 0x9F    ///< Not currently used

#define MADCTL_MY 0x10
#define MADCTL_MX 0x02
#define MADCTL_MV 0x01

static scr_handle_t g_lcd_handle;

/**
 * This header file is only used to redefine the function to facilitate the call.
 * It can only be placed in this position, not in the head of the file.
 */
#include "interface_drv_def.h"

scr_driver_t lcd_ssd1351_default_driver = {
    .init = lcd_ssd1351_init,
    .deinit = lcd_ssd1351_deinit,
    .set_direction = lcd_ssd1351_set_rotation,
    .set_window = lcd_ssd1351_set_window,
    .write_ram_data = lcd_ssd1351_write_ram_data,
    .draw_pixel = lcd_ssd1351_draw_pixel,
    .draw_bitmap = lcd_ssd1351_draw_bitmap,
    .get_info = lcd_ssd1351_get_info,
};

esp_err_t lcd_ssd1351_init(const scr_controller_config_t *lcd_conf)
{
    LCD_CHECK(lcd_conf->width <= SSD1351_RESOLUTION_HOR, "Width greater than maximum", ESP_ERR_INVALID_ARG);
    LCD_CHECK(lcd_conf->height <= SSD1351_RESOLUTION_VER, "Height greater than maximum", ESP_ERR_INVALID_ARG);
    LCD_CHECK(NULL != lcd_conf, "config pointer invalid", ESP_ERR_INVALID_ARG);
    LCD_CHECK((NULL != lcd_conf->interface_drv->write_cmd && \
              NULL != lcd_conf->interface_drv->write_data && \
              NULL != lcd_conf->interface_drv->write && \
              NULL != lcd_conf->interface_drv->read && \
              NULL != lcd_conf->interface_drv->bus_acquire && \
              NULL != lcd_conf->interface_drv->bus_release),
              "Interface driver invalid", ESP_ERR_INVALID_ARG);

    // Reset the display
    if (lcd_conf->pin_num_rst >= 0) {
        gpio_pad_select_gpio(lcd_conf->pin_num_rst);
        gpio_set_direction(lcd_conf->pin_num_rst, GPIO_MODE_OUTPUT);
        gpio_set_level(lcd_conf->pin_num_rst, (lcd_conf->rst_active_level) & 0x1);
        vTaskDelay(100 / portTICK_RATE_MS);
        gpio_set_level(lcd_conf->pin_num_rst, (~(lcd_conf->rst_active_level)) & 0x1);
        vTaskDelay(100 / portTICK_RATE_MS);
    }

    g_lcd_handle.interface_drv = lcd_conf->interface_drv;
    g_lcd_handle.original_width = lcd_conf->width;
    g_lcd_handle.original_height = lcd_conf->height;
    g_lcd_handle.offset_hor = lcd_conf->offset_hor;
    g_lcd_handle.offset_ver = lcd_conf->offset_ver;

    LCD_WRITE_CMD(SSD1351_CMD_COMMANDLOCK);// Set command lock, 1 arg
    LCD_WRITE_DATA(0x12);
    LCD_WRITE_CMD(SSD1351_CMD_COMMANDLOCK);// Set command lock, 1 arg
    LCD_WRITE_DATA(0xB1);
    LCD_WRITE_CMD(SSD1351_CMD_DISPLAYOFF);// Display off, no args
    LCD_WRITE_CMD(SSD1351_CMD_CLOCKDIV);
    LCD_WRITE_DATA(0xF1); // 7:4 = Oscillator Freq, 3:0 = CLK Div Ratio (A[3:0]+1 = 1..16)
    LCD_WRITE_CMD(SSD1351_CMD_MUXRATIO);
    LCD_WRITE_DATA(127);
    LCD_WRITE_CMD(SSD1351_CMD_DISPLAYOFFSET);
    LCD_WRITE_DATA(0x0);
    LCD_WRITE_CMD(SSD1351_CMD_SETGPIO);
    LCD_WRITE_DATA(0x00);
    LCD_WRITE_CMD(SSD1351_CMD_FUNCTIONSELECT);
    LCD_WRITE_DATA(0x01); // internal (diode drop)
    LCD_WRITE_CMD(SSD1351_CMD_PRECHARGE);
    LCD_WRITE_DATA(0x32);
    LCD_WRITE_CMD(SSD1351_CMD_VCOMH);
    LCD_WRITE_DATA(0x05);
    LCD_WRITE_CMD(SSD1351_CMD_NORMALDISPLAY);
    LCD_WRITE_CMD(SSD1351_CMD_CONTRASTABC);
    LCD_WRITE_DATA(0xC8);
    LCD_WRITE_DATA(0x80);
    LCD_WRITE_DATA(0xC8);
    LCD_WRITE_CMD(SSD1351_CMD_CONTRASTMASTER);
    LCD_WRITE_DATA(0x0F);
    LCD_WRITE_CMD(SSD1351_CMD_SETVSL);
    LCD_WRITE_DATA(0xA0);
    LCD_WRITE_DATA(0xB5);
    LCD_WRITE_DATA(0x55);
    LCD_WRITE_CMD(SSD1351_CMD_PRECHARGE2);
    LCD_WRITE_DATA(0x01);
    LCD_WRITE_CMD(SSD1351_CMD_DISPLAYON);

    // Enable backlight
    if (lcd_conf->pin_num_bckl >= 0) {
        gpio_pad_select_gpio(lcd_conf->pin_num_bckl);
        gpio_set_direction(lcd_conf->pin_num_bckl, GPIO_MODE_OUTPUT);
        gpio_set_level(lcd_conf->pin_num_bckl, (lcd_conf->bckl_active_level) & 0x1);
    }

    return lcd_ssd1351_set_rotation(lcd_conf->rotate);
}

esp_err_t lcd_ssd1351_deinit(void)
{
    memset(&g_lcd_handle, 0, sizeof(scr_handle_t));
    return ESP_OK;
}

esp_err_t lcd_ssd1351_set_rotation(scr_dir_t dir)
{
    esp_err_t ret;
    if (SCR_DIR_MAX < dir) {
        dir >>= 5;
    }
    LCD_CHECK(dir < 8, "Unsupport rotate direction", ESP_ERR_INVALID_ARG);

    /**
     * 6,7 Color depth (01 = 65K)
     * 5   Odd/even split COM (0: disable, 1: enable)
     * 4   Scan direction (0: top-down, 1: bottom-up)
     * 3   Reserved
     * 2   Color remap (0: A->B->C, 1: C->B->A)
     * 1   Column remap (0: 0-127, 1: 127-0)
     * 0   Address increment (0: horizontal, 1: vertical)
     */
    uint8_t reg_data = 0b01100100;

    switch (dir) {
    case SCR_DIR_LRTB:
        g_lcd_handle.width = g_lcd_handle.original_width;
        g_lcd_handle.height = g_lcd_handle.original_height;
        break;
    case SCR_DIR_LRBT:
        reg_data |= MADCTL_MY;
        g_lcd_handle.width = g_lcd_handle.original_width;
        g_lcd_handle.height = g_lcd_handle.original_height;
        break;
    case SCR_DIR_RLTB:
        reg_data |= MADCTL_MX;
        g_lcd_handle.width = g_lcd_handle.original_width;
        g_lcd_handle.height = g_lcd_handle.original_height;
        break;
    case SCR_DIR_RLBT:
        reg_data |= MADCTL_MX | MADCTL_MY;
        g_lcd_handle.width = g_lcd_handle.original_width;
        g_lcd_handle.height = g_lcd_handle.original_height;
        break;

    case SCR_DIR_TBLR:
        reg_data |= MADCTL_MV;
        g_lcd_handle.width = g_lcd_handle.original_height;
        g_lcd_handle.height = g_lcd_handle.original_width;
        break;
    case SCR_DIR_BTLR:
        reg_data |= MADCTL_MY | MADCTL_MV;
        g_lcd_handle.width = g_lcd_handle.original_height;
        g_lcd_handle.height = g_lcd_handle.original_width;
        break;
    case SCR_DIR_TBRL:
        reg_data |= MADCTL_MX | MADCTL_MV;
        g_lcd_handle.width = g_lcd_handle.original_height;
        g_lcd_handle.height = g_lcd_handle.original_width;
        break;
    case SCR_DIR_BTRL:
        reg_data |= MADCTL_MX | MADCTL_MY | MADCTL_MV;
        g_lcd_handle.width = g_lcd_handle.original_height;
        g_lcd_handle.height = g_lcd_handle.original_width;
        break;
    default: break;
    }
    ESP_LOGI(TAG, "MADCTL=%x", reg_data);
    ret = LCD_WRITE_CMD(SSD1351_CMD_SETREMAP);
    ret |= LCD_WRITE_DATA(reg_data);
    // uint8_t startline = (dir < 2) ? g_lcd_handle.original_height : 0;
    // ret |= LCD_WRITE_CMD(SSD1351_CMD_STARTLINE);
    // ret |= LCD_WRITE_DATA(startline);
    LCD_CHECK(ESP_OK == ret, "Set screen rotate failed", ESP_FAIL);
    g_lcd_handle.dir = dir;
    return ESP_OK;
}

esp_err_t lcd_ssd1351_get_info(scr_info_t *info)
{
    LCD_CHECK(NULL != info, "info pointer invalid", ESP_ERR_INVALID_ARG);
    info->width = g_lcd_handle.width;
    info->height = g_lcd_handle.height;
    info->dir = g_lcd_handle.dir;
    info->name = LCD_NAME;
    info->color_type = SCR_COLOR_TYPE_RGB565;
    info->bpp = LCD_BPP;
    return ESP_OK;
}

esp_err_t lcd_ssd1351_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    LCD_CHECK((x1 < g_lcd_handle.width) && (y1 < g_lcd_handle.height), "The set coordinates exceed the screen size", ESP_ERR_INVALID_ARG);
    LCD_CHECK((x0 <= x1) && (y0 <= y1), "Window coordinates invalid", ESP_ERR_INVALID_ARG);
    esp_err_t ret = ESP_OK;
    scr_utility_apply_offset(&g_lcd_handle, SSD1351_RESOLUTION_HOR, SSD1351_RESOLUTION_VER, &x0, &y0, &x1, &y1);

    ret |= LCD_WRITE_CMD(SSD1351_CMD_SETCOLUMN);
    ret |= LCD_WRITE_DATA(x0);
    ret |= LCD_WRITE_DATA(x1);
    ret |= LCD_WRITE_CMD(SSD1351_CMD_SETROW);
    ret |= LCD_WRITE_DATA(y0);
    ret |= LCD_WRITE_DATA(y1);

    ret |= LCD_WRITE_CMD(SSD1351_CMD_WRITERAM);
    LCD_CHECK(ESP_OK == ret, "Set window failed", ESP_FAIL);
    return ESP_OK;
}

esp_err_t lcd_ssd1351_set_invert(bool is_invert)
{
    return LCD_WRITE_CMD(is_invert ? SSD1351_CMD_INVERTDISPLAY  : SSD1351_CMD_NORMALDISPLAY);
}


esp_err_t lcd_ssd1351_write_ram_data(uint16_t color)
{
    static uint8_t data[2];
    data[0] = (uint8_t)(color & 0xff);
    data[1] = (uint8_t)(color >> 8);
    return LCD_WRITE(data, 2);
}

esp_err_t lcd_ssd1351_draw_pixel(uint16_t x, uint16_t y, uint16_t color)
{
    esp_err_t ret;
    ret = lcd_ssd1351_set_window(x, y, x, y);
    if (ESP_OK != ret) {
        return ESP_FAIL;
    }
    return lcd_ssd1351_write_ram_data(color);
}

esp_err_t lcd_ssd1351_draw_bitmap(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t *bitmap)
{
    esp_err_t ret;
    LCD_CHECK(NULL != bitmap, "bitmap pointer invalid", ESP_ERR_INVALID_ARG);

    LCD_IFACE_ACQUIRE();
    ret = lcd_ssd1351_set_window(x, y, x + w - 1, y + h - 1);
    if (ESP_OK != ret) {
        return ESP_FAIL;
    }

    uint32_t len = w * h;
    ret = LCD_WRITE((uint8_t *)bitmap, 2 * len);
    LCD_IFACE_RELEASE();
    LCD_CHECK(ESP_OK == ret, "lcd write ram data failed", ESP_FAIL);
    return ESP_OK;
}

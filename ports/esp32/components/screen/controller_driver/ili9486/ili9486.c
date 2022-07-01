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
#include "ili9486.h"

static const char *TAG = "ILI9486";

#define LCD_CHECK(a, str, ret)  if(!(a)) {                               \
        ESP_LOGE(TAG,"%s:%d (%s):%s", __FILE__, __LINE__, __FUNCTION__, str);   \
        return (ret);                                                           \
    }

#define ILI9486_CASET   0x2A
#define ILI9486_RASET   0x2B
#define ILI9486_RAMWR   0x2C
#define ILI9486_MADCTL  0x36

/* MADCTL Defines */
#define MADCTL_MY  0x80
#define MADCTL_MX  0x40
#define MADCTL_MV  0x20
#define MADCTL_ML  0x10
#define MADCTL_RGB 0x08
#define MADCTL_MH  0x04

#define LCD_NAME "ILI9486"
#define LCD_BPP  16

#define ILI9486_RESOLUTION_HOR 320
#define ILI9486_RESOLUTION_VER 480

static scr_handle_t g_lcd_handle;

/**
 * This header file is only used to redefine the function to facilitate the call.
 * It can only be placed in this position, not in the head of the file.
 */
#include "interface_drv_def.h"

scr_driver_t lcd_ili9486_default_driver = {
    .init = lcd_ili9486_init,
    .deinit = lcd_ili9486_deinit,
    .set_direction = lcd_ili9486_set_rotation,
    .set_window = lcd_ili9486_set_window,
    .write_ram_data = lcd_ili9486_write_ram_data,
    .draw_pixel = lcd_ili9486_draw_pixel,
    .draw_bitmap = lcd_ili9486_draw_bitmap,
    .get_info = lcd_ili9486_get_info,
};


static void lcd_ili9486_init_reg(void);

esp_err_t lcd_ili9486_init(const scr_controller_config_t *lcd_conf)
{
    LCD_CHECK(lcd_conf->width <= ILI9486_RESOLUTION_HOR, "Width greater than maximum", ESP_ERR_INVALID_ARG);
    LCD_CHECK(lcd_conf->height <= ILI9486_RESOLUTION_VER, "Height greater than maximum", ESP_ERR_INVALID_ARG);
    LCD_CHECK(NULL != lcd_conf, "config pointer invalid", ESP_ERR_INVALID_ARG);
    LCD_CHECK((NULL != lcd_conf->interface_drv->write_cmd && \
               NULL != lcd_conf->interface_drv->write_data && \
               NULL != lcd_conf->interface_drv->write && \
               NULL != lcd_conf->interface_drv->read && \
               NULL != lcd_conf->interface_drv->bus_acquire && \
               NULL != lcd_conf->interface_drv->bus_release),
              "Interface driver invalid", ESP_ERR_INVALID_ARG);

    esp_err_t ret;

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

    lcd_ili9486_init_reg();

    // Enable backlight
    if (lcd_conf->pin_num_bckl >= 0) {
        gpio_pad_select_gpio(lcd_conf->pin_num_bckl);
        gpio_set_direction(lcd_conf->pin_num_bckl, GPIO_MODE_OUTPUT);
        gpio_set_level(lcd_conf->pin_num_bckl, (lcd_conf->bckl_active_level) & 0x1);
    }

    ret = lcd_ili9486_set_rotation(lcd_conf->rotate);
    LCD_CHECK(ESP_OK == ret, "set rotation failed", ESP_FAIL);

    return ESP_OK;
}

esp_err_t lcd_ili9486_deinit(void)
{
    memset(&g_lcd_handle, 0, sizeof(scr_handle_t));
    return ESP_OK;
}

esp_err_t lcd_ili9486_set_rotation(scr_dir_t dir)
{
    esp_err_t ret;
    uint8_t reg_data = MADCTL_RGB;
    if (SCR_DIR_MAX < dir) {
        dir >>= 5;
    }
    LCD_CHECK(dir < 8, "Unsupport rotate direction", ESP_ERR_INVALID_ARG);
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
    ESP_LOGI(TAG, "MADCTL=0x%x", reg_data);
    ret = LCD_WRITE_REG(ILI9486_MADCTL, reg_data);
    LCD_CHECK(ESP_OK == ret, "Set screen rotate failed", ESP_FAIL);
    g_lcd_handle.dir = dir;
    return ESP_OK;
}

esp_err_t lcd_ili9486_get_info(scr_info_t *info)
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

esp_err_t lcd_ili9486_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    LCD_CHECK((x1 < g_lcd_handle.width) && (y1 < g_lcd_handle.height), "The set coordinates exceed the screen size", ESP_ERR_INVALID_ARG);
    LCD_CHECK((x0 <= x1) && (y0 <= y1), "Window coordinates invalid", ESP_ERR_INVALID_ARG);
    esp_err_t ret = ESP_OK;
    scr_utility_apply_offset(&g_lcd_handle, ILI9486_RESOLUTION_HOR, ILI9486_RESOLUTION_VER, &x0, &y0, &x1, &y1);

    ret |= LCD_WRITE_CMD(ILI9486_CASET);
    ret |= LCD_WRITE_DATA(x0 >> 8);
    ret |= LCD_WRITE_DATA(x0 & 0xff);
    ret |= LCD_WRITE_DATA(x1 >> 8);
    ret |= LCD_WRITE_DATA(x1 & 0xff);
    ret |= LCD_WRITE_CMD(ILI9486_RASET);
    ret |= LCD_WRITE_DATA(y0 >> 8);
    ret |= LCD_WRITE_DATA(y0 & 0xff);
    ret |= LCD_WRITE_DATA(y1 >> 8);
    ret |= LCD_WRITE_DATA(y1 & 0xff);

    ret |= LCD_WRITE_CMD(ILI9486_RAMWR);
    LCD_CHECK(ESP_OK == ret, "Set window failed", ESP_FAIL);
    return ESP_OK;
}

esp_err_t lcd_ili9486_write_ram_data(uint16_t color)
{
    static uint8_t data[2];
    data[0] = (uint8_t)(color & 0xff);
    data[1] = (uint8_t)(color >> 8);
    return LCD_WRITE(data, 2);
}

esp_err_t lcd_ili9486_draw_pixel(uint16_t x, uint16_t y, uint16_t color)
{
    esp_err_t ret;
    ret = lcd_ili9486_set_window(x, y, x, y);
    if (ESP_OK != ret) {
        return ESP_FAIL;
    }
    return lcd_ili9486_write_ram_data(color);
}

esp_err_t lcd_ili9486_draw_bitmap(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t *bitmap)
{
    esp_err_t ret;
    LCD_CHECK(NULL != bitmap, "bitmap pointer invalid", ESP_ERR_INVALID_ARG);

    LCD_IFACE_ACQUIRE();
    ret = lcd_ili9486_set_window(x, y, x + w - 1, y + h - 1);
    if (ESP_OK != ret) {
        return ESP_FAIL;
    }

    uint32_t len = w * h;
    ret = LCD_WRITE((uint8_t *)bitmap, 2 * len);
    LCD_IFACE_RELEASE();
    LCD_CHECK(ESP_OK == ret, "lcd write ram data failed", ESP_FAIL);
    return ESP_OK;
}


static void lcd_ili9486_init_reg(void)
{
    LCD_WRITE_CMD(0x01); // SW reset
    vTaskDelay(120 / portTICK_RATE_MS);
    // Interface Mode Control
    LCD_WRITE_CMD(0xb0);
    LCD_WRITE_DATA(0x00);
    // Interface Pixel Format, 16 bits / pixel
    LCD_WRITE_CMD(0x3A);
    LCD_WRITE_DATA(0x55); // 5D
    // PGAMCTRL(Positive Gamma Control)
    LCD_WRITE_CMD(0xE0);
    LCD_WRITE_DATA(0x0F);
    LCD_WRITE_DATA(0x1F);
    LCD_WRITE_DATA(0x1C);
    LCD_WRITE_DATA(0x0C);
    LCD_WRITE_DATA(0x0F);
    LCD_WRITE_DATA(0x08);
    LCD_WRITE_DATA(0x48);
    LCD_WRITE_DATA(0x98);
    LCD_WRITE_DATA(0x37);
    LCD_WRITE_DATA(0x0A);
    LCD_WRITE_DATA(0x13);
    LCD_WRITE_DATA(0x04);
    LCD_WRITE_DATA(0x11);
    LCD_WRITE_DATA(0x0D);
    LCD_WRITE_DATA(0x00);
    // NGAMCTRL (Negative Gamma Correction)
    LCD_WRITE_CMD(0xE1);
    LCD_WRITE_DATA(0x0F);
    LCD_WRITE_DATA(0x32);
    LCD_WRITE_DATA(0x2E);
    LCD_WRITE_DATA(0x0B);
    LCD_WRITE_DATA(0x0D);
    LCD_WRITE_DATA(0x05);
    LCD_WRITE_DATA(0x47);
    LCD_WRITE_DATA(0x75);
    LCD_WRITE_DATA(0x37);
    LCD_WRITE_DATA(0x06);
    LCD_WRITE_DATA(0x10);
    LCD_WRITE_DATA(0x03);
    LCD_WRITE_DATA(0x24);
    LCD_WRITE_DATA(0x20);
    LCD_WRITE_DATA(0x00);
    // Digital Gamma Control 1
    LCD_WRITE_CMD(0xE2);
    LCD_WRITE_DATA(0x0F);
    LCD_WRITE_DATA(0x32);
    LCD_WRITE_DATA(0x2E);
    LCD_WRITE_DATA(0x0B);
    LCD_WRITE_DATA(0x0D);
    LCD_WRITE_DATA(0x05);
    LCD_WRITE_DATA(0x47);
    LCD_WRITE_DATA(0x75);
    LCD_WRITE_DATA(0x37);
    LCD_WRITE_DATA(0x06);
    LCD_WRITE_DATA(0x10);
    LCD_WRITE_DATA(0x03);
    LCD_WRITE_DATA(0x24);
    LCD_WRITE_DATA(0x20);
    LCD_WRITE_DATA(0x00);

    // Set rotation
    // setRotation(_rotation);

    // Idle mode control + Power +  Frame rate ctrl
    LCD_WRITE_CMD(0x38);
    // frame rate ctrl
    LCD_WRITE_CMD(0xB1);
    // Frame rate(Hz) (default: 70kHz) /-/ Division Ratio (default: fosc)
    LCD_WRITE_DATA(0xB0);
    // Clock per Line (default: 17 clk cycles)
    LCD_WRITE_DATA(0x11);
    // Power Control 3 (For Normal Mode)
    LCD_WRITE_CMD(0xC2);
    LCD_WRITE_DATA(0x55); // 44

    // Display Inversion Control
    LCD_WRITE_CMD(0xB4);
    LCD_WRITE_DATA(0x02); // 2 dot invercion /-/ disabled | 0x12 to enable
    // Display Function Control
    LCD_WRITE_CMD(0xB6);
    LCD_WRITE_DATA(0x02);
    LCD_WRITE_DATA(0x22);
    LCD_WRITE_DATA(0x3B);
    // # Sleep OUT
    LCD_WRITE_CMD(0x11);
    vTaskDelay(150 / portTICK_RATE_MS);
    // Display ON
    LCD_WRITE_CMD(0x29);
}
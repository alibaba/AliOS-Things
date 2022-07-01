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
#include "rm68120.h"

static const char *TAG = "lcd rm68120";

#define LCD_CHECK(a, str, ret)  if(!(a)) {                               \
        ESP_LOGE(TAG,"%s:%d (%s):%s", __FILE__, __LINE__, __FUNCTION__, str);   \
        return (ret);                                                           \
    }

#define LCD_NAME "RM68120"
#define LCD_BPP  16

/** 
 * RM68120 can select different resolution, but I can't find the way
 */
#define RM68120_RESOLUTION_HOR 480
#define RM68120_RESOLUTION_VER 800

#define RM68120_CASET   0x2A00
#define RM68120_RASET   0x2B00
#define RM68120_RAMWR   0x2C00
#define RM68120_MADCTL  0x3600

/* MADCTL Defines */
#define MADCTL_MY  0x80
#define MADCTL_MX  0x40
#define MADCTL_MV  0x20
#define MADCTL_ML  0x10
#define MADCTL_RGB 0x08
#define MADCTL_MH  0x04

static scr_handle_t g_lcd_handle;

/**
 * This header file is only used to redefine the function to facilitate the call.
 * It can only be placed in this position, not in the head of the file.
 */
#include "interface_drv_def.h"

scr_driver_t lcd_rm68120_default_driver = {
    .init = lcd_rm68120_init,
    .deinit = lcd_rm68120_deinit,
    .set_direction = lcd_rm68120_set_rotation,
    .set_window = lcd_rm68120_set_window,
    .write_ram_data = lcd_rm68120_write_ram_data,
    .draw_pixel = lcd_rm68120_draw_pixel,
    .draw_bitmap = lcd_rm68120_draw_bitmap,
    .get_info = lcd_rm68120_get_info,
};


static void lcd_rm68120_init_reg(void);

esp_err_t lcd_rm68120_init(const scr_controller_config_t *lcd_conf)
{
    LCD_CHECK(lcd_conf->width <= RM68120_RESOLUTION_HOR, "Width greater than maximum", ESP_ERR_INVALID_ARG);
    LCD_CHECK(lcd_conf->height <= RM68120_RESOLUTION_VER, "Height greater than maximum", ESP_ERR_INVALID_ARG);
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

    lcd_rm68120_init_reg();

    // Enable backlight
    if (lcd_conf->pin_num_bckl >= 0) {
        gpio_pad_select_gpio(lcd_conf->pin_num_bckl);
        gpio_set_direction(lcd_conf->pin_num_bckl, GPIO_MODE_OUTPUT);
        gpio_set_level(lcd_conf->pin_num_bckl, (lcd_conf->bckl_active_level) & 0x1);
    }

    ret = lcd_rm68120_set_rotation(lcd_conf->rotate);
    LCD_CHECK(ESP_OK == ret, "set rotation failed", ESP_FAIL);

    return ESP_OK;
}

esp_err_t lcd_rm68120_deinit(void)
{
    memset(&g_lcd_handle, 0, sizeof(scr_handle_t));
    return ESP_OK;
}

esp_err_t lcd_rm68120_set_rotation(scr_dir_t dir)
{
    esp_err_t ret;
    uint8_t reg_data = 0;
    reg_data &= ~MADCTL_RGB;
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
    ret = LCD_WRITE_REG(RM68120_MADCTL, reg_data);
    LCD_CHECK(ESP_OK == ret, "Set screen rotate failed", ESP_FAIL);
    g_lcd_handle.dir = dir;
    return ESP_OK;
}

esp_err_t lcd_rm68120_get_info(scr_info_t *info)
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

esp_err_t lcd_rm68120_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    LCD_CHECK((x1 < g_lcd_handle.width) && (y1 < g_lcd_handle.height), "The set coordinates exceed the screen size", ESP_ERR_INVALID_ARG);
    LCD_CHECK((x0 <= x1) && (y0 <= y1), "Window coordinates invalid", ESP_ERR_INVALID_ARG);
    esp_err_t ret = ESP_OK;
    scr_utility_apply_offset(&g_lcd_handle, RM68120_RESOLUTION_HOR, RM68120_RESOLUTION_VER, &x0, &y0, &x1, &y1);

    ret |= LCD_WRITE_REG(RM68120_CASET, x0 >> 8);
    ret |= LCD_WRITE_REG(RM68120_CASET + 1, x0 & 0xff);
    ret |= LCD_WRITE_REG(RM68120_CASET + 2, x1 >> 8);
    ret |= LCD_WRITE_REG(RM68120_CASET + 3, x1 & 0xff);
    ret |= LCD_WRITE_REG(RM68120_RASET, y0 >> 8);
    ret |= LCD_WRITE_REG(RM68120_RASET + 1, y0 & 0xff);
    ret |= LCD_WRITE_REG(RM68120_RASET + 2, y1 >> 8);
    ret |= LCD_WRITE_REG(RM68120_RASET + 3, y1 & 0xff);

    ret |= LCD_WRITE_CMD(RM68120_RAMWR);
    LCD_CHECK(ESP_OK == ret, "Set window failed", ESP_FAIL);
    return ESP_OK;
}

esp_err_t lcd_rm68120_write_ram_data(uint16_t color)
{
    static uint8_t data[2];
    data[0] = (uint8_t)(color & 0xff);
    data[1] = (uint8_t)(color >> 8);
    return LCD_WRITE(data, 2);
}

esp_err_t lcd_rm68120_draw_pixel(uint16_t x, uint16_t y, uint16_t color)
{
    esp_err_t ret;
    ret = lcd_rm68120_set_window(x, y, x, y);
    if (ESP_OK != ret) {
        return ESP_FAIL;
    }
    return lcd_rm68120_write_ram_data(color);
}

esp_err_t lcd_rm68120_draw_bitmap(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t *bitmap)
{
    esp_err_t ret;
    LCD_CHECK(NULL != bitmap, "bitmap pointer invalid", ESP_ERR_INVALID_ARG);

    LCD_IFACE_ACQUIRE();
    ret = lcd_rm68120_set_window(x, y, x + w - 1, y + h - 1);
    if (ESP_OK != ret) {
        return ESP_FAIL;
    }

    uint32_t len = w * h;
    ret = LCD_WRITE((uint8_t *)bitmap, 2 * len);
    LCD_IFACE_RELEASE();
    LCD_CHECK(ESP_OK == ret, "lcd write ram data failed", ESP_FAIL);
    return ESP_OK;
}

static void lcd_rm68120_init_reg(void)
{
    LCD_WRITE_CMD(0x0100); // Software Reset
    vTaskDelay(10 / portTICK_RATE_MS);
    LCD_WRITE_REG(0xF000, 0x55);
    LCD_WRITE_REG(0xF001, 0xAA);
    LCD_WRITE_REG(0xF002, 0x52);
    LCD_WRITE_REG(0xF003, 0x08);
    LCD_WRITE_REG(0xF004, 0x01);

    //GAMMA SETING  RED
    LCD_WRITE_REG(0xD100, 0x00);
    LCD_WRITE_REG(0xD101, 0x00);
    LCD_WRITE_REG(0xD102, 0x1b);
    LCD_WRITE_REG(0xD103, 0x44);
    LCD_WRITE_REG(0xD104, 0x62);
    LCD_WRITE_REG(0xD105, 0x00);
    LCD_WRITE_REG(0xD106, 0x7b);
    LCD_WRITE_REG(0xD107, 0xa1);
    LCD_WRITE_REG(0xD108, 0xc0);
    LCD_WRITE_REG(0xD109, 0xee);
    LCD_WRITE_REG(0xD10A, 0x55);
    LCD_WRITE_REG(0xD10B, 0x10);
    LCD_WRITE_REG(0xD10C, 0x2c);
    LCD_WRITE_REG(0xD10D, 0x43);
    LCD_WRITE_REG(0xD10E, 0x57);
    LCD_WRITE_REG(0xD10F, 0x55);
    LCD_WRITE_REG(0xD110, 0x68);
    LCD_WRITE_REG(0xD111, 0x78);
    LCD_WRITE_REG(0xD112, 0x87);
    LCD_WRITE_REG(0xD113, 0x94);
    LCD_WRITE_REG(0xD114, 0x55);
    LCD_WRITE_REG(0xD115, 0xa0);
    LCD_WRITE_REG(0xD116, 0xac);
    LCD_WRITE_REG(0xD117, 0xb6);
    LCD_WRITE_REG(0xD118, 0xc1);
    LCD_WRITE_REG(0xD119, 0x55);
    LCD_WRITE_REG(0xD11A, 0xcb);
    LCD_WRITE_REG(0xD11B, 0xcd);
    LCD_WRITE_REG(0xD11C, 0xd6);
    LCD_WRITE_REG(0xD11D, 0xdf);
    LCD_WRITE_REG(0xD11E, 0x95);
    LCD_WRITE_REG(0xD11F, 0xe8);
    LCD_WRITE_REG(0xD120, 0xf1);
    LCD_WRITE_REG(0xD121, 0xfa);
    LCD_WRITE_REG(0xD122, 0x02);
    LCD_WRITE_REG(0xD123, 0xaa);
    LCD_WRITE_REG(0xD124, 0x0b);
    LCD_WRITE_REG(0xD125, 0x13);
    LCD_WRITE_REG(0xD126, 0x1d);
    LCD_WRITE_REG(0xD127, 0x26);
    LCD_WRITE_REG(0xD128, 0xaa);
    LCD_WRITE_REG(0xD129, 0x30);
    LCD_WRITE_REG(0xD12A, 0x3c);
    LCD_WRITE_REG(0xD12B, 0x4A);
    LCD_WRITE_REG(0xD12C, 0x63);
    LCD_WRITE_REG(0xD12D, 0xea);
    LCD_WRITE_REG(0xD12E, 0x79);
    LCD_WRITE_REG(0xD12F, 0xa6);
    LCD_WRITE_REG(0xD130, 0xd0);
    LCD_WRITE_REG(0xD131, 0x20);
    LCD_WRITE_REG(0xD132, 0x0f);
    LCD_WRITE_REG(0xD133, 0x8e);
    LCD_WRITE_REG(0xD134, 0xff);
    //GAMMA SETING GREEN
    LCD_WRITE_REG(0xD200, 0x00);
    LCD_WRITE_REG(0xD201, 0x00);
    LCD_WRITE_REG(0xD202, 0x1b);
    LCD_WRITE_REG(0xD203, 0x44);
    LCD_WRITE_REG(0xD204, 0x62);
    LCD_WRITE_REG(0xD205, 0x00);
    LCD_WRITE_REG(0xD206, 0x7b);
    LCD_WRITE_REG(0xD207, 0xa1);
    LCD_WRITE_REG(0xD208, 0xc0);
    LCD_WRITE_REG(0xD209, 0xee);
    LCD_WRITE_REG(0xD20A, 0x55);
    LCD_WRITE_REG(0xD20B, 0x10);
    LCD_WRITE_REG(0xD20C, 0x2c);
    LCD_WRITE_REG(0xD20D, 0x43);
    LCD_WRITE_REG(0xD20E, 0x57);
    LCD_WRITE_REG(0xD20F, 0x55);
    LCD_WRITE_REG(0xD210, 0x68);
    LCD_WRITE_REG(0xD211, 0x78);
    LCD_WRITE_REG(0xD212, 0x87);
    LCD_WRITE_REG(0xD213, 0x94);
    LCD_WRITE_REG(0xD214, 0x55);
    LCD_WRITE_REG(0xD215, 0xa0);
    LCD_WRITE_REG(0xD216, 0xac);
    LCD_WRITE_REG(0xD217, 0xb6);
    LCD_WRITE_REG(0xD218, 0xc1);
    LCD_WRITE_REG(0xD219, 0x55);
    LCD_WRITE_REG(0xD21A, 0xcb);
    LCD_WRITE_REG(0xD21B, 0xcd);
    LCD_WRITE_REG(0xD21C, 0xd6);
    LCD_WRITE_REG(0xD21D, 0xdf);
    LCD_WRITE_REG(0xD21E, 0x95);
    LCD_WRITE_REG(0xD21F, 0xe8);
    LCD_WRITE_REG(0xD220, 0xf1);
    LCD_WRITE_REG(0xD221, 0xfa);
    LCD_WRITE_REG(0xD222, 0x02);
    LCD_WRITE_REG(0xD223, 0xaa);
    LCD_WRITE_REG(0xD224, 0x0b);
    LCD_WRITE_REG(0xD225, 0x13);
    LCD_WRITE_REG(0xD226, 0x1d);
    LCD_WRITE_REG(0xD227, 0x26);
    LCD_WRITE_REG(0xD228, 0xaa);
    LCD_WRITE_REG(0xD229, 0x30);
    LCD_WRITE_REG(0xD22A, 0x3c);
    LCD_WRITE_REG(0xD22B, 0x4a);
    LCD_WRITE_REG(0xD22C, 0x63);
    LCD_WRITE_REG(0xD22D, 0xea);
    LCD_WRITE_REG(0xD22E, 0x79);
    LCD_WRITE_REG(0xD22F, 0xa6);
    LCD_WRITE_REG(0xD230, 0xd0);
    LCD_WRITE_REG(0xD231, 0x20);
    LCD_WRITE_REG(0xD232, 0x0f);
    LCD_WRITE_REG(0xD233, 0x8e);
    LCD_WRITE_REG(0xD234, 0xff);

    //GAMMA SETING BLUE
    LCD_WRITE_REG(0xD300, 0x00);
    LCD_WRITE_REG(0xD301, 0x00);
    LCD_WRITE_REG(0xD302, 0x1b);
    LCD_WRITE_REG(0xD303, 0x44);
    LCD_WRITE_REG(0xD304, 0x62);
    LCD_WRITE_REG(0xD305, 0x00);
    LCD_WRITE_REG(0xD306, 0x7b);
    LCD_WRITE_REG(0xD307, 0xa1);
    LCD_WRITE_REG(0xD308, 0xc0);
    LCD_WRITE_REG(0xD309, 0xee);
    LCD_WRITE_REG(0xD30A, 0x55);
    LCD_WRITE_REG(0xD30B, 0x10);
    LCD_WRITE_REG(0xD30C, 0x2c);
    LCD_WRITE_REG(0xD30D, 0x43);
    LCD_WRITE_REG(0xD30E, 0x57);
    LCD_WRITE_REG(0xD30F, 0x55);
    LCD_WRITE_REG(0xD310, 0x68);
    LCD_WRITE_REG(0xD311, 0x78);
    LCD_WRITE_REG(0xD312, 0x87);
    LCD_WRITE_REG(0xD313, 0x94);
    LCD_WRITE_REG(0xD314, 0x55);
    LCD_WRITE_REG(0xD315, 0xa0);
    LCD_WRITE_REG(0xD316, 0xac);
    LCD_WRITE_REG(0xD317, 0xb6);
    LCD_WRITE_REG(0xD318, 0xc1);
    LCD_WRITE_REG(0xD319, 0x55);
    LCD_WRITE_REG(0xD31A, 0xcb);
    LCD_WRITE_REG(0xD31B, 0xcd);
    LCD_WRITE_REG(0xD31C, 0xd6);
    LCD_WRITE_REG(0xD31D, 0xdf);
    LCD_WRITE_REG(0xD31E, 0x95);
    LCD_WRITE_REG(0xD31F, 0xe8);
    LCD_WRITE_REG(0xD320, 0xf1);
    LCD_WRITE_REG(0xD321, 0xfa);
    LCD_WRITE_REG(0xD322, 0x02);
    LCD_WRITE_REG(0xD323, 0xaa);
    LCD_WRITE_REG(0xD324, 0x0b);
    LCD_WRITE_REG(0xD325, 0x13);
    LCD_WRITE_REG(0xD326, 0x1d);
    LCD_WRITE_REG(0xD327, 0x26);
    LCD_WRITE_REG(0xD328, 0xaa);
    LCD_WRITE_REG(0xD329, 0x30);
    LCD_WRITE_REG(0xD32A, 0x3c);
    LCD_WRITE_REG(0xD32B, 0x4A);
    LCD_WRITE_REG(0xD32C, 0x63);
    LCD_WRITE_REG(0xD32D, 0xea);
    LCD_WRITE_REG(0xD32E, 0x79);
    LCD_WRITE_REG(0xD32F, 0xa6);
    LCD_WRITE_REG(0xD330, 0xd0);
    LCD_WRITE_REG(0xD331, 0x20);
    LCD_WRITE_REG(0xD332, 0x0f);
    LCD_WRITE_REG(0xD333, 0x8e);
    LCD_WRITE_REG(0xD334, 0xff);


    //GAMMA SETING  RED
    LCD_WRITE_REG(0xD400, 0x00);
    LCD_WRITE_REG(0xD401, 0x00);
    LCD_WRITE_REG(0xD402, 0x1b);
    LCD_WRITE_REG(0xD403, 0x44);
    LCD_WRITE_REG(0xD404, 0x62);
    LCD_WRITE_REG(0xD405, 0x00);
    LCD_WRITE_REG(0xD406, 0x7b);
    LCD_WRITE_REG(0xD407, 0xa1);
    LCD_WRITE_REG(0xD408, 0xc0);
    LCD_WRITE_REG(0xD409, 0xee);
    LCD_WRITE_REG(0xD40A, 0x55);
    LCD_WRITE_REG(0xD40B, 0x10);
    LCD_WRITE_REG(0xD40C, 0x2c);
    LCD_WRITE_REG(0xD40D, 0x43);
    LCD_WRITE_REG(0xD40E, 0x57);
    LCD_WRITE_REG(0xD40F, 0x55);
    LCD_WRITE_REG(0xD410, 0x68);
    LCD_WRITE_REG(0xD411, 0x78);
    LCD_WRITE_REG(0xD412, 0x87);
    LCD_WRITE_REG(0xD413, 0x94);
    LCD_WRITE_REG(0xD414, 0x55);
    LCD_WRITE_REG(0xD415, 0xa0);
    LCD_WRITE_REG(0xD416, 0xac);
    LCD_WRITE_REG(0xD417, 0xb6);
    LCD_WRITE_REG(0xD418, 0xc1);
    LCD_WRITE_REG(0xD419, 0x55);
    LCD_WRITE_REG(0xD41A, 0xcb);
    LCD_WRITE_REG(0xD41B, 0xcd);
    LCD_WRITE_REG(0xD41C, 0xd6);
    LCD_WRITE_REG(0xD41D, 0xdf);
    LCD_WRITE_REG(0xD41E, 0x95);
    LCD_WRITE_REG(0xD41F, 0xe8);
    LCD_WRITE_REG(0xD420, 0xf1);
    LCD_WRITE_REG(0xD421, 0xfa);
    LCD_WRITE_REG(0xD422, 0x02);
    LCD_WRITE_REG(0xD423, 0xaa);
    LCD_WRITE_REG(0xD424, 0x0b);
    LCD_WRITE_REG(0xD425, 0x13);
    LCD_WRITE_REG(0xD426, 0x1d);
    LCD_WRITE_REG(0xD427, 0x26);
    LCD_WRITE_REG(0xD428, 0xaa);
    LCD_WRITE_REG(0xD429, 0x30);
    LCD_WRITE_REG(0xD42A, 0x3c);
    LCD_WRITE_REG(0xD42B, 0x4A);
    LCD_WRITE_REG(0xD42C, 0x63);
    LCD_WRITE_REG(0xD42D, 0xea);
    LCD_WRITE_REG(0xD42E, 0x79);
    LCD_WRITE_REG(0xD42F, 0xa6);
    LCD_WRITE_REG(0xD430, 0xd0);
    LCD_WRITE_REG(0xD431, 0x20);
    LCD_WRITE_REG(0xD432, 0x0f);
    LCD_WRITE_REG(0xD433, 0x8e);
    LCD_WRITE_REG(0xD434, 0xff);

    //GAMMA SETING GREEN
    LCD_WRITE_REG(0xD500, 0x00);
    LCD_WRITE_REG(0xD501, 0x00);
    LCD_WRITE_REG(0xD502, 0x1b);
    LCD_WRITE_REG(0xD503, 0x44);
    LCD_WRITE_REG(0xD504, 0x62);
    LCD_WRITE_REG(0xD505, 0x00);
    LCD_WRITE_REG(0xD506, 0x7b);
    LCD_WRITE_REG(0xD507, 0xa1);
    LCD_WRITE_REG(0xD508, 0xc0);
    LCD_WRITE_REG(0xD509, 0xee);
    LCD_WRITE_REG(0xD50A, 0x55);
    LCD_WRITE_REG(0xD50B, 0x10);
    LCD_WRITE_REG(0xD50C, 0x2c);
    LCD_WRITE_REG(0xD50D, 0x43);
    LCD_WRITE_REG(0xD50E, 0x57);
    LCD_WRITE_REG(0xD50F, 0x55);
    LCD_WRITE_REG(0xD510, 0x68);
    LCD_WRITE_REG(0xD511, 0x78);
    LCD_WRITE_REG(0xD512, 0x87);
    LCD_WRITE_REG(0xD513, 0x94);
    LCD_WRITE_REG(0xD514, 0x55);
    LCD_WRITE_REG(0xD515, 0xa0);
    LCD_WRITE_REG(0xD516, 0xac);
    LCD_WRITE_REG(0xD517, 0xb6);
    LCD_WRITE_REG(0xD518, 0xc1);
    LCD_WRITE_REG(0xD519, 0x55);
    LCD_WRITE_REG(0xD51A, 0xcb);
    LCD_WRITE_REG(0xD51B, 0xcd);
    LCD_WRITE_REG(0xD51C, 0xd6);
    LCD_WRITE_REG(0xD51D, 0xdf);
    LCD_WRITE_REG(0xD51E, 0x95);
    LCD_WRITE_REG(0xD51F, 0xe8);
    LCD_WRITE_REG(0xD520, 0xf1);
    LCD_WRITE_REG(0xD521, 0xfa);
    LCD_WRITE_REG(0xD522, 0x02);
    LCD_WRITE_REG(0xD523, 0xaa);
    LCD_WRITE_REG(0xD524, 0x0b);
    LCD_WRITE_REG(0xD525, 0x13);
    LCD_WRITE_REG(0xD526, 0x1d);
    LCD_WRITE_REG(0xD527, 0x26);
    LCD_WRITE_REG(0xD528, 0xaa);
    LCD_WRITE_REG(0xD529, 0x30);
    LCD_WRITE_REG(0xD52A, 0x3c);
    LCD_WRITE_REG(0xD52B, 0x4a);
    LCD_WRITE_REG(0xD52C, 0x63);
    LCD_WRITE_REG(0xD52D, 0xea);
    LCD_WRITE_REG(0xD52E, 0x79);
    LCD_WRITE_REG(0xD52F, 0xa6);
    LCD_WRITE_REG(0xD530, 0xd0);
    LCD_WRITE_REG(0xD531, 0x20);
    LCD_WRITE_REG(0xD532, 0x0f);
    LCD_WRITE_REG(0xD533, 0x8e);
    LCD_WRITE_REG(0xD534, 0xff);

    //GAMMA SETING BLUE
    LCD_WRITE_REG(0xD600, 0x00);
    LCD_WRITE_REG(0xD601, 0x00);
    LCD_WRITE_REG(0xD602, 0x1b);
    LCD_WRITE_REG(0xD603, 0x44);
    LCD_WRITE_REG(0xD604, 0x62);
    LCD_WRITE_REG(0xD605, 0x00);
    LCD_WRITE_REG(0xD606, 0x7b);
    LCD_WRITE_REG(0xD607, 0xa1);
    LCD_WRITE_REG(0xD608, 0xc0);
    LCD_WRITE_REG(0xD609, 0xee);
    LCD_WRITE_REG(0xD60A, 0x55);
    LCD_WRITE_REG(0xD60B, 0x10);
    LCD_WRITE_REG(0xD60C, 0x2c);
    LCD_WRITE_REG(0xD60D, 0x43);
    LCD_WRITE_REG(0xD60E, 0x57);
    LCD_WRITE_REG(0xD60F, 0x55);
    LCD_WRITE_REG(0xD610, 0x68);
    LCD_WRITE_REG(0xD611, 0x78);
    LCD_WRITE_REG(0xD612, 0x87);
    LCD_WRITE_REG(0xD613, 0x94);
    LCD_WRITE_REG(0xD614, 0x55);
    LCD_WRITE_REG(0xD615, 0xa0);
    LCD_WRITE_REG(0xD616, 0xac);
    LCD_WRITE_REG(0xD617, 0xb6);
    LCD_WRITE_REG(0xD618, 0xc1);
    LCD_WRITE_REG(0xD619, 0x55);
    LCD_WRITE_REG(0xD61A, 0xcb);
    LCD_WRITE_REG(0xD61B, 0xcd);
    LCD_WRITE_REG(0xD61C, 0xd6);
    LCD_WRITE_REG(0xD61D, 0xdf);
    LCD_WRITE_REG(0xD61E, 0x95);
    LCD_WRITE_REG(0xD61F, 0xe8);
    LCD_WRITE_REG(0xD620, 0xf1);
    LCD_WRITE_REG(0xD621, 0xfa);
    LCD_WRITE_REG(0xD622, 0x02);
    LCD_WRITE_REG(0xD623, 0xaa);
    LCD_WRITE_REG(0xD624, 0x0b);
    LCD_WRITE_REG(0xD625, 0x13);
    LCD_WRITE_REG(0xD626, 0x1d);
    LCD_WRITE_REG(0xD627, 0x26);
    LCD_WRITE_REG(0xD628, 0xaa);
    LCD_WRITE_REG(0xD629, 0x30);
    LCD_WRITE_REG(0xD62A, 0x3c);
    LCD_WRITE_REG(0xD62B, 0x4A);
    LCD_WRITE_REG(0xD62C, 0x63);
    LCD_WRITE_REG(0xD62D, 0xea);
    LCD_WRITE_REG(0xD62E, 0x79);
    LCD_WRITE_REG(0xD62F, 0xa6);
    LCD_WRITE_REG(0xD630, 0xd0);
    LCD_WRITE_REG(0xD631, 0x20);
    LCD_WRITE_REG(0xD632, 0x0f);
    LCD_WRITE_REG(0xD633, 0x8e);
    LCD_WRITE_REG(0xD634, 0xff);

    //AVDD VOLTAGE SETTING
    LCD_WRITE_REG(0xB000, 0x05);
    LCD_WRITE_REG(0xB001, 0x05);
    LCD_WRITE_REG(0xB002, 0x05);
    //AVEE VOLTAGE SETTING
    LCD_WRITE_REG(0xB100, 0x05);
    LCD_WRITE_REG(0xB101, 0x05);
    LCD_WRITE_REG(0xB102, 0x05);

    //AVDD Boosting
    LCD_WRITE_REG(0xB600, 0x34);
    LCD_WRITE_REG(0xB601, 0x34);
    LCD_WRITE_REG(0xB603, 0x34);
    //AVEE Boosting
    LCD_WRITE_REG(0xB700, 0x24);
    LCD_WRITE_REG(0xB701, 0x24);
    LCD_WRITE_REG(0xB702, 0x24);
    //VCL Boosting
    LCD_WRITE_REG(0xB800, 0x24);
    LCD_WRITE_REG(0xB801, 0x24);
    LCD_WRITE_REG(0xB802, 0x24);
    //VGLX VOLTAGE SETTING
    LCD_WRITE_REG(0xBA00, 0x14);
    LCD_WRITE_REG(0xBA01, 0x14);
    LCD_WRITE_REG(0xBA02, 0x14);
    //VCL Boosting
    LCD_WRITE_REG(0xB900, 0x24);
    LCD_WRITE_REG(0xB901, 0x24);
    LCD_WRITE_REG(0xB902, 0x24);
    //Gamma Voltage
    LCD_WRITE_REG(0xBc00, 0x00);
    LCD_WRITE_REG(0xBc01, 0xa0);//vgmp=5.0
    LCD_WRITE_REG(0xBc02, 0x00);
    LCD_WRITE_REG(0xBd00, 0x00);
    LCD_WRITE_REG(0xBd01, 0xa0);//vgmn=5.0
    LCD_WRITE_REG(0xBd02, 0x00);
    //VCOM Setting
    LCD_WRITE_REG(0xBe01, 0x3d);//3
    //ENABLE PAGE 0
    LCD_WRITE_REG(0xF000, 0x55);
    LCD_WRITE_REG(0xF001, 0xAA);
    LCD_WRITE_REG(0xF002, 0x52);
    LCD_WRITE_REG(0xF003, 0x08);
    LCD_WRITE_REG(0xF004, 0x00);
    //Vivid Color Function Control
    LCD_WRITE_REG(0xB400, 0x10);
    //Z-INVERSION
    LCD_WRITE_REG(0xBC00, 0x05);
    LCD_WRITE_REG(0xBC01, 0x05);
    LCD_WRITE_REG(0xBC02, 0x05);

    //*************** add on 20111021**********************//
    LCD_WRITE_REG(0xB700, 0x22);//GATE EQ CONTROL
    LCD_WRITE_REG(0xB701, 0x22);//GATE EQ CONTROL
    LCD_WRITE_REG(0xC80B, 0x2A);//DISPLAY TIMING CONTROL
    LCD_WRITE_REG(0xC80C, 0x2A);//DISPLAY TIMING CONTROL
    LCD_WRITE_REG(0xC80F, 0x2A);//DISPLAY TIMING CONTROL
    LCD_WRITE_REG(0xC810, 0x2A);//DISPLAY TIMING CONTROL
    //*************** add on 20111021**********************//
    //PWM_ENH_OE =1
    LCD_WRITE_REG(0xd000, 0x01);
    //DM_SEL =1
    LCD_WRITE_REG(0xb300, 0x10);
    //VBPDA=07h
    LCD_WRITE_REG(0xBd02, 0x07);
    //VBPDb=07h
    LCD_WRITE_REG(0xBe02, 0x07);
    //VBPDc=07h
    LCD_WRITE_REG(0xBf02, 0x07);
    //ENABLE PAGE 2
    LCD_WRITE_REG(0xF000, 0x55);
    LCD_WRITE_REG(0xF001, 0xAA);
    LCD_WRITE_REG(0xF002, 0x52);
    LCD_WRITE_REG(0xF003, 0x08);
    LCD_WRITE_REG(0xF004, 0x02);
    //SDREG0 =0
    LCD_WRITE_REG(0xc301, 0xa9);
    //DS=14
    LCD_WRITE_REG(0xfe01, 0x94);
    //OSC =60h
    LCD_WRITE_REG(0xf600, 0x60);
    //TE ON
    LCD_WRITE_REG(0x3500, 0x00);
    //SLEEP OUT 
    LCD_WRITE_CMD(0x1100);
    vTaskDelay(100 / portTICK_RATE_MS);
    //DISPLY ON
    LCD_WRITE_CMD(0x2900);
    vTaskDelay(100 / portTICK_RATE_MS);

    LCD_WRITE_REG(0x3A00, 0x55);
    LCD_WRITE_REG(0x3600, 0xA3);
}

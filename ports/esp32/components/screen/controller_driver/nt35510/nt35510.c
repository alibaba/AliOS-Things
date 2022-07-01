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
#include "nt35510.h"

static const char *TAG = "lcd nt35510";

#define LCD_CHECK(a, str, ret)  if(!(a)) {                               \
        ESP_LOGE(TAG,"%s:%d (%s):%s", __FILE__, __LINE__, __FUNCTION__, str);   \
        return (ret);                                                           \
    }

#define LCD_NAME "NT35510"
#define LCD_BPP  16

/** 
 * NT35510 can select different resolution, but I can't find the way
 */
#define NT35510_RESOLUTION_HOR 480
#define NT35510_RESOLUTION_VER 800

#define NT35510_CASET   0x2A00
#define NT35510_RASET   0x2B00
#define NT35510_RAMWR   0x2C00
#define NT35510_MADCTL  0x3600

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

scr_driver_t lcd_nt35510_default_driver = {
    .init = lcd_nt35510_init,
    .deinit = lcd_nt35510_deinit,
    .set_direction = lcd_nt35510_set_rotation,
    .set_window = lcd_nt35510_set_window,
    .write_ram_data = lcd_nt35510_write_ram_data,
    .draw_pixel = lcd_nt35510_draw_pixel,
    .draw_bitmap = lcd_nt35510_draw_bitmap,
    .get_info = lcd_nt35510_get_info,
};


static void lcd_nt35510_init_reg(void);

esp_err_t lcd_nt35510_init(const scr_controller_config_t *lcd_conf)
{
    LCD_CHECK(lcd_conf->width <= NT35510_RESOLUTION_HOR, "Width greater than maximum", ESP_ERR_INVALID_ARG);
    LCD_CHECK(lcd_conf->height <= NT35510_RESOLUTION_VER, "Height greater than maximum", ESP_ERR_INVALID_ARG);
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

    lcd_nt35510_init_reg();

    // Enable backlight
    if (lcd_conf->pin_num_bckl >= 0) {
        gpio_pad_select_gpio(lcd_conf->pin_num_bckl);
        gpio_set_direction(lcd_conf->pin_num_bckl, GPIO_MODE_OUTPUT);
        gpio_set_level(lcd_conf->pin_num_bckl, (lcd_conf->bckl_active_level) & 0x1);
    }

    ret = lcd_nt35510_set_rotation(lcd_conf->rotate);
    LCD_CHECK(ESP_OK == ret, "set rotation failed", ESP_FAIL);

    return ESP_OK;
}

esp_err_t lcd_nt35510_deinit(void)
{
    memset(&g_lcd_handle, 0, sizeof(scr_handle_t));
    return ESP_OK;
}

esp_err_t lcd_nt35510_set_rotation(scr_dir_t dir)
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
    ret = LCD_WRITE_REG(NT35510_MADCTL, reg_data);
    LCD_CHECK(ESP_OK == ret, "Set screen rotate failed", ESP_FAIL);
    g_lcd_handle.dir = dir;
    return ESP_OK;
}

esp_err_t lcd_nt35510_get_info(scr_info_t *info)
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

esp_err_t lcd_nt35510_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    LCD_CHECK((x1 < g_lcd_handle.width) && (y1 < g_lcd_handle.height), "The set coordinates exceed the screen size", ESP_ERR_INVALID_ARG);
    LCD_CHECK((x0 <= x1) && (y0 <= y1), "Window coordinates invalid", ESP_ERR_INVALID_ARG);
    esp_err_t ret = ESP_OK;
    scr_utility_apply_offset(&g_lcd_handle, NT35510_RESOLUTION_HOR, NT35510_RESOLUTION_VER, &x0, &y0, &x1, &y1);

    ret |= LCD_WRITE_REG(NT35510_CASET, x0 >> 8);
    ret |= LCD_WRITE_REG(NT35510_CASET + 1, x0 & 0xff);
    ret |= LCD_WRITE_REG(NT35510_CASET + 2, x1 >> 8);
    ret |= LCD_WRITE_REG(NT35510_CASET + 3, x1 & 0xff);
    ret |= LCD_WRITE_REG(NT35510_RASET, y0 >> 8);
    ret |= LCD_WRITE_REG(NT35510_RASET + 1, y0 & 0xff);
    ret |= LCD_WRITE_REG(NT35510_RASET + 2, y1 >> 8);
    ret |= LCD_WRITE_REG(NT35510_RASET + 3, y1 & 0xff);

    ret |= LCD_WRITE_CMD(NT35510_RAMWR);
    LCD_CHECK(ESP_OK == ret, "Set window failed", ESP_FAIL);
    return ESP_OK;
}

esp_err_t lcd_nt35510_write_ram_data(uint16_t color)
{
    static uint8_t data[2];
    data[0] = (uint8_t)(color & 0xff);
    data[1] = (uint8_t)(color >> 8);
    return LCD_WRITE(data, 2);
}

esp_err_t lcd_nt35510_draw_pixel(uint16_t x, uint16_t y, uint16_t color)
{
    esp_err_t ret;
    ret = lcd_nt35510_set_window(x, y, x, y);
    if (ESP_OK != ret) {
        return ESP_FAIL;
    }
    return lcd_nt35510_write_ram_data(color);
}

esp_err_t lcd_nt35510_draw_bitmap(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t *bitmap)
{
    esp_err_t ret;
    LCD_CHECK(NULL != bitmap, "bitmap pointer invalid", ESP_ERR_INVALID_ARG);

    LCD_IFACE_ACQUIRE();
    ret = lcd_nt35510_set_window(x, y, x + w - 1, y + h - 1);
    if (ESP_OK != ret) {
        return ESP_FAIL;
    }

    uint32_t len = w * h;
    ret = LCD_WRITE((uint8_t *)bitmap, 2 * len);
    LCD_IFACE_RELEASE();
    LCD_CHECK(ESP_OK == ret, "lcd write ram data failed", ESP_FAIL);
    return ESP_OK;
}

static void lcd_nt35510_init_reg(void)
{
    LCD_WRITE_CMD(0x0100); // Software Reset
    vTaskDelay(10 / portTICK_RATE_MS);
    LCD_WRITE_CMD(0x1200);
    LCD_WRITE_REG(0xf000, 0x0055);
    LCD_WRITE_REG(0xf001, 0x00aa);
    LCD_WRITE_REG(0xf002, 0x0052);
    LCD_WRITE_REG(0xf003, 0x0008);
    LCD_WRITE_REG(0xf004, 0x0001);

    LCD_WRITE_REG(0xbc01, 0x0086);
    LCD_WRITE_REG(0xbc02, 0x006a);
    LCD_WRITE_REG(0xbd01, 0x0086);
    LCD_WRITE_REG(0xbd02, 0x006a);
    LCD_WRITE_REG(0xbe01, 0x0067);

    LCD_WRITE_REG(0xd100, 0x0000);
    LCD_WRITE_REG(0xd101, 0x005d);
    LCD_WRITE_REG(0xd102, 0x0000);
    LCD_WRITE_REG(0xd103, 0x006b);
    LCD_WRITE_REG(0xd104, 0x0000);
    LCD_WRITE_REG(0xd105, 0x0084);
    LCD_WRITE_REG(0xd106, 0x0000);
    LCD_WRITE_REG(0xd107, 0x009c);
    LCD_WRITE_REG(0xd108, 0x0000);
    LCD_WRITE_REG(0xd109, 0x00b1);
    LCD_WRITE_REG(0xd10a, 0x0000);
    LCD_WRITE_REG(0xd10b, 0x00d9);
    LCD_WRITE_REG(0xd10c, 0x0000);
    LCD_WRITE_REG(0xd10d, 0x00fd);
    LCD_WRITE_REG(0xd10e, 0x0001);
    LCD_WRITE_REG(0xd10f, 0x0038);
    LCD_WRITE_REG(0xd110, 0x0001);
    LCD_WRITE_REG(0xd111, 0x0068);
    LCD_WRITE_REG(0xd112, 0x0001);
    LCD_WRITE_REG(0xd113, 0x00b9);
    LCD_WRITE_REG(0xd114, 0x0001);
    LCD_WRITE_REG(0xd115, 0x00fb);
    LCD_WRITE_REG(0xd116, 0x0002);
    LCD_WRITE_REG(0xd117, 0x0063);
    LCD_WRITE_REG(0xd118, 0x0002);
    LCD_WRITE_REG(0xd119, 0x00b9);
    LCD_WRITE_REG(0xd11a, 0x0002);
    LCD_WRITE_REG(0xd11b, 0x00bb);
    LCD_WRITE_REG(0xd11c, 0x0003);
    LCD_WRITE_REG(0xd11d, 0x0003);
    LCD_WRITE_REG(0xd11e, 0x0003);
    LCD_WRITE_REG(0xd11f, 0x0046);
    LCD_WRITE_REG(0xd120, 0x0003);
    LCD_WRITE_REG(0xd121, 0x0069);
    LCD_WRITE_REG(0xd122, 0x0003);
    LCD_WRITE_REG(0xd123, 0x008f);
    LCD_WRITE_REG(0xd124, 0x0003);
    LCD_WRITE_REG(0xd125, 0x00a4);
    LCD_WRITE_REG(0xd126, 0x0003);
    LCD_WRITE_REG(0xd127, 0x00b9);
    LCD_WRITE_REG(0xd128, 0x0003);
    LCD_WRITE_REG(0xd129, 0x00c7);
    LCD_WRITE_REG(0xd12a, 0x0003);
    LCD_WRITE_REG(0xd12b, 0x00c9);
    LCD_WRITE_REG(0xd12c, 0x0003);
    LCD_WRITE_REG(0xd12d, 0x00cb);
    LCD_WRITE_REG(0xd12e, 0x0003);
    LCD_WRITE_REG(0xd12f, 0x00cb);
    LCD_WRITE_REG(0xd130, 0x0003);
    LCD_WRITE_REG(0xd131, 0x00cb);
    LCD_WRITE_REG(0xd132, 0x0003);
    LCD_WRITE_REG(0xd133, 0x00cc);

    LCD_WRITE_REG(0xd200, 0x0000);
    LCD_WRITE_REG(0xd201, 0x005d);
    LCD_WRITE_REG(0xd202, 0x0000);
    LCD_WRITE_REG(0xd203, 0x006b);
    LCD_WRITE_REG(0xd204, 0x0000);
    LCD_WRITE_REG(0xd205, 0x0084);
    LCD_WRITE_REG(0xd206, 0x0000);
    LCD_WRITE_REG(0xd207, 0x009c);
    LCD_WRITE_REG(0xd208, 0x0000);
    LCD_WRITE_REG(0xd209, 0x00b1);
    LCD_WRITE_REG(0xd20a, 0x0000);
    LCD_WRITE_REG(0xd20b, 0x00d9);
    LCD_WRITE_REG(0xd20c, 0x0000);
    LCD_WRITE_REG(0xd20d, 0x00fd);
    LCD_WRITE_REG(0xd20e, 0x0001);
    LCD_WRITE_REG(0xd20f, 0x0038);
    LCD_WRITE_REG(0xd210, 0x0001);
    LCD_WRITE_REG(0xd211, 0x0068);
    LCD_WRITE_REG(0xd212, 0x0001);
    LCD_WRITE_REG(0xd213, 0x00b9);
    LCD_WRITE_REG(0xd214, 0x0001);
    LCD_WRITE_REG(0xd215, 0x00fb);
    LCD_WRITE_REG(0xd216, 0x0002);
    LCD_WRITE_REG(0xd217, 0x0063);
    LCD_WRITE_REG(0xd218, 0x0002);
    LCD_WRITE_REG(0xd219, 0x00b9);
    LCD_WRITE_REG(0xd21a, 0x0002);
    LCD_WRITE_REG(0xd21b, 0x00bb);
    LCD_WRITE_REG(0xd21c, 0x0003);
    LCD_WRITE_REG(0xd21d, 0x0003);
    LCD_WRITE_REG(0xd21e, 0x0003);
    LCD_WRITE_REG(0xd21f, 0x0046);
    LCD_WRITE_REG(0xd220, 0x0003);
    LCD_WRITE_REG(0xd221, 0x0069);
    LCD_WRITE_REG(0xd222, 0x0003);
    LCD_WRITE_REG(0xd223, 0x008f);
    LCD_WRITE_REG(0xd224, 0x0003);
    LCD_WRITE_REG(0xd225, 0x00a4);
    LCD_WRITE_REG(0xd226, 0x0003);
    LCD_WRITE_REG(0xd227, 0x00b9);
    LCD_WRITE_REG(0xd228, 0x0003);
    LCD_WRITE_REG(0xd229, 0x00c7);
    LCD_WRITE_REG(0xd22a, 0x0003);
    LCD_WRITE_REG(0xd22b, 0x00c9);
    LCD_WRITE_REG(0xd22c, 0x0003);
    LCD_WRITE_REG(0xd22d, 0x00cb);
    LCD_WRITE_REG(0xd22e, 0x0003);
    LCD_WRITE_REG(0xd22f, 0x00cb);
    LCD_WRITE_REG(0xd230, 0x0003);
    LCD_WRITE_REG(0xd231, 0x00cb);
    LCD_WRITE_REG(0xd232, 0x0003);
    LCD_WRITE_REG(0xd233, 0x00cc);

    LCD_WRITE_REG(0xd300, 0x0000);
    LCD_WRITE_REG(0xd301, 0x005d);
    LCD_WRITE_REG(0xd302, 0x0000);
    LCD_WRITE_REG(0xd303, 0x006b);
    LCD_WRITE_REG(0xd304, 0x0000);
    LCD_WRITE_REG(0xd305, 0x0084);
    LCD_WRITE_REG(0xd306, 0x0000);
    LCD_WRITE_REG(0xd307, 0x009c);
    LCD_WRITE_REG(0xd308, 0x0000);
    LCD_WRITE_REG(0xd309, 0x00b1);
    LCD_WRITE_REG(0xd30a, 0x0000);
    LCD_WRITE_REG(0xd30b, 0x00d9);
    LCD_WRITE_REG(0xd30c, 0x0000);
    LCD_WRITE_REG(0xd30d, 0x00fd);
    LCD_WRITE_REG(0xd30e, 0x0001);
    LCD_WRITE_REG(0xd30f, 0x0038);
    LCD_WRITE_REG(0xd310, 0x0001);
    LCD_WRITE_REG(0xd311, 0x0068);
    LCD_WRITE_REG(0xd312, 0x0001);
    LCD_WRITE_REG(0xd313, 0x00b9);
    LCD_WRITE_REG(0xd314, 0x0001);
    LCD_WRITE_REG(0xd315, 0x00fb);
    LCD_WRITE_REG(0xd316, 0x0002);
    LCD_WRITE_REG(0xd317, 0x0063);
    LCD_WRITE_REG(0xd318, 0x0002);
    LCD_WRITE_REG(0xd319, 0x00b9);
    LCD_WRITE_REG(0xd31a, 0x0002);
    LCD_WRITE_REG(0xd31b, 0x00bb);
    LCD_WRITE_REG(0xd31c, 0x0003);
    LCD_WRITE_REG(0xd31d, 0x0003);
    LCD_WRITE_REG(0xd31e, 0x0003);
    LCD_WRITE_REG(0xd31f, 0x0046);
    LCD_WRITE_REG(0xd320, 0x0003);
    LCD_WRITE_REG(0xd321, 0x0069);
    LCD_WRITE_REG(0xd322, 0x0003);
    LCD_WRITE_REG(0xd323, 0x008f);
    LCD_WRITE_REG(0xd324, 0x0003);
    LCD_WRITE_REG(0xd325, 0x00a4);
    LCD_WRITE_REG(0xd326, 0x0003);
    LCD_WRITE_REG(0xd327, 0x00b9);
    LCD_WRITE_REG(0xd328, 0x0003);
    LCD_WRITE_REG(0xd329, 0x00c7);
    LCD_WRITE_REG(0xd32a, 0x0003);
    LCD_WRITE_REG(0xd32b, 0x00c9);
    LCD_WRITE_REG(0xd32c, 0x0003);
    LCD_WRITE_REG(0xd32d, 0x00cb);
    LCD_WRITE_REG(0xd32e, 0x0003);
    LCD_WRITE_REG(0xd32f, 0x00cb);
    LCD_WRITE_REG(0xd330, 0x0003);
    LCD_WRITE_REG(0xd331, 0x00cb);
    LCD_WRITE_REG(0xd332, 0x0003);
    LCD_WRITE_REG(0xd333, 0x00cc);

    LCD_WRITE_REG(0xd400, 0x0000);
    LCD_WRITE_REG(0xd401, 0x005d);
    LCD_WRITE_REG(0xd402, 0x0000);
    LCD_WRITE_REG(0xd403, 0x006b);
    LCD_WRITE_REG(0xd404, 0x0000);
    LCD_WRITE_REG(0xd405, 0x0084);
    LCD_WRITE_REG(0xd406, 0x0000);
    LCD_WRITE_REG(0xd407, 0x009c);
    LCD_WRITE_REG(0xd408, 0x0000);
    LCD_WRITE_REG(0xd409, 0x00b1);
    LCD_WRITE_REG(0xd40a, 0x0000);
    LCD_WRITE_REG(0xd40b, 0x00d9);
    LCD_WRITE_REG(0xd40c, 0x0000);
    LCD_WRITE_REG(0xd40d, 0x00fd);
    LCD_WRITE_REG(0xd40e, 0x0001);
    LCD_WRITE_REG(0xd40f, 0x0038);
    LCD_WRITE_REG(0xd410, 0x0001);
    LCD_WRITE_REG(0xd411, 0x0068);
    LCD_WRITE_REG(0xd412, 0x0001);
    LCD_WRITE_REG(0xd413, 0x00b9);
    LCD_WRITE_REG(0xd414, 0x0001);
    LCD_WRITE_REG(0xd415, 0x00fb);
    LCD_WRITE_REG(0xd416, 0x0002);
    LCD_WRITE_REG(0xd417, 0x0063);
    LCD_WRITE_REG(0xd418, 0x0002);
    LCD_WRITE_REG(0xd419, 0x00b9);
    LCD_WRITE_REG(0xd41a, 0x0002);
    LCD_WRITE_REG(0xd41b, 0x00bb);
    LCD_WRITE_REG(0xd41c, 0x0003);
    LCD_WRITE_REG(0xd41d, 0x0003);
    LCD_WRITE_REG(0xd41e, 0x0003);
    LCD_WRITE_REG(0xd41f, 0x0046);
    LCD_WRITE_REG(0xd420, 0x0003);
    LCD_WRITE_REG(0xd421, 0x0069);
    LCD_WRITE_REG(0xd422, 0x0003);
    LCD_WRITE_REG(0xd423, 0x008f);
    LCD_WRITE_REG(0xd424, 0x0003);
    LCD_WRITE_REG(0xd425, 0x00a4);
    LCD_WRITE_REG(0xd426, 0x0003);
    LCD_WRITE_REG(0xd427, 0x00b9);
    LCD_WRITE_REG(0xd428, 0x0003);
    LCD_WRITE_REG(0xd429, 0x00c7);
    LCD_WRITE_REG(0xd42a, 0x0003);
    LCD_WRITE_REG(0xd42b, 0x00c9);
    LCD_WRITE_REG(0xd42c, 0x0003);
    LCD_WRITE_REG(0xd42d, 0x00cb);
    LCD_WRITE_REG(0xd42e, 0x0003);
    LCD_WRITE_REG(0xd42f, 0x00cb);
    LCD_WRITE_REG(0xd430, 0x0003);
    LCD_WRITE_REG(0xd431, 0x00cb);
    LCD_WRITE_REG(0xd432, 0x0003);
    LCD_WRITE_REG(0xd433, 0x00cc);

    LCD_WRITE_REG(0xd500, 0x0000);
    LCD_WRITE_REG(0xd501, 0x005d);
    LCD_WRITE_REG(0xd502, 0x0000);
    LCD_WRITE_REG(0xd503, 0x006b);
    LCD_WRITE_REG(0xd504, 0x0000);
    LCD_WRITE_REG(0xd505, 0x0084);
    LCD_WRITE_REG(0xd506, 0x0000);
    LCD_WRITE_REG(0xd507, 0x009c);
    LCD_WRITE_REG(0xd508, 0x0000);
    LCD_WRITE_REG(0xd509, 0x00b1);
    LCD_WRITE_REG(0xd50a, 0x0000);
    LCD_WRITE_REG(0xd50b, 0x00D9);
    LCD_WRITE_REG(0xd50c, 0x0000);
    LCD_WRITE_REG(0xd50d, 0x00fd);
    LCD_WRITE_REG(0xd50e, 0x0001);
    LCD_WRITE_REG(0xd50f, 0x0038);
    LCD_WRITE_REG(0xd510, 0x0001);
    LCD_WRITE_REG(0xd511, 0x0068);
    LCD_WRITE_REG(0xd512, 0x0001);
    LCD_WRITE_REG(0xd513, 0x00b9);
    LCD_WRITE_REG(0xd514, 0x0001);
    LCD_WRITE_REG(0xd515, 0x00fb);
    LCD_WRITE_REG(0xd516, 0x0002);
    LCD_WRITE_REG(0xd517, 0x0063);
    LCD_WRITE_REG(0xd518, 0x0002);
    LCD_WRITE_REG(0xd519, 0x00b9);
    LCD_WRITE_REG(0xd51a, 0x0002);
    LCD_WRITE_REG(0xd51b, 0x00bb);
    LCD_WRITE_REG(0xd51c, 0x0003);
    LCD_WRITE_REG(0xd51d, 0x0003);
    LCD_WRITE_REG(0xd51e, 0x0003);
    LCD_WRITE_REG(0xd51f, 0x0046);
    LCD_WRITE_REG(0xd520, 0x0003);
    LCD_WRITE_REG(0xd521, 0x0069);
    LCD_WRITE_REG(0xd522, 0x0003);
    LCD_WRITE_REG(0xd523, 0x008f);
    LCD_WRITE_REG(0xd524, 0x0003);
    LCD_WRITE_REG(0xd525, 0x00a4);
    LCD_WRITE_REG(0xd526, 0x0003);
    LCD_WRITE_REG(0xd527, 0x00b9);
    LCD_WRITE_REG(0xd528, 0x0003);
    LCD_WRITE_REG(0xd529, 0x00c7);
    LCD_WRITE_REG(0xd52a, 0x0003);
    LCD_WRITE_REG(0xd52b, 0x00c9);
    LCD_WRITE_REG(0xd52c, 0x0003);
    LCD_WRITE_REG(0xd52d, 0x00cb);
    LCD_WRITE_REG(0xd52e, 0x0003);
    LCD_WRITE_REG(0xd52f, 0x00cb);
    LCD_WRITE_REG(0xd530, 0x0003);
    LCD_WRITE_REG(0xd531, 0x00cb);
    LCD_WRITE_REG(0xd532, 0x0003);
    LCD_WRITE_REG(0xd533, 0x00cc);

    LCD_WRITE_REG(0xd600, 0x0000);
    LCD_WRITE_REG(0xd601, 0x005d);
    LCD_WRITE_REG(0xd602, 0x0000);
    LCD_WRITE_REG(0xd603, 0x006b);
    LCD_WRITE_REG(0xd604, 0x0000);
    LCD_WRITE_REG(0xd605, 0x0084);
    LCD_WRITE_REG(0xd606, 0x0000);
    LCD_WRITE_REG(0xd607, 0x009c);
    LCD_WRITE_REG(0xd608, 0x0000);
    LCD_WRITE_REG(0xd609, 0x00b1);
    LCD_WRITE_REG(0xd60a, 0x0000);
    LCD_WRITE_REG(0xd60b, 0x00d9);
    LCD_WRITE_REG(0xd60c, 0x0000);
    LCD_WRITE_REG(0xd60d, 0x00fd);
    LCD_WRITE_REG(0xd60e, 0x0001);
    LCD_WRITE_REG(0xd60f, 0x0038);
    LCD_WRITE_REG(0xd610, 0x0001);
    LCD_WRITE_REG(0xd611, 0x0068);
    LCD_WRITE_REG(0xd612, 0x0001);
    LCD_WRITE_REG(0xd613, 0x00b9);
    LCD_WRITE_REG(0xd614, 0x0001);
    LCD_WRITE_REG(0xd615, 0x00fb);
    LCD_WRITE_REG(0xd616, 0x0002);
    LCD_WRITE_REG(0xd617, 0x0063);
    LCD_WRITE_REG(0xd618, 0x0002);
    LCD_WRITE_REG(0xd619, 0x00b9);
    LCD_WRITE_REG(0xd61a, 0x0002);
    LCD_WRITE_REG(0xd61b, 0x00bb);
    LCD_WRITE_REG(0xd61c, 0x0003);
    LCD_WRITE_REG(0xd61d, 0x0003);
    LCD_WRITE_REG(0xd61e, 0x0003);
    LCD_WRITE_REG(0xd61f, 0x0046);
    LCD_WRITE_REG(0xd620, 0x0003);
    LCD_WRITE_REG(0xd621, 0x0069);
    LCD_WRITE_REG(0xd622, 0x0003);
    LCD_WRITE_REG(0xd623, 0x008f);
    LCD_WRITE_REG(0xd624, 0x0003);
    LCD_WRITE_REG(0xd625, 0x00a4);
    LCD_WRITE_REG(0xd626, 0x0003);
    LCD_WRITE_REG(0xd627, 0x00b9);
    LCD_WRITE_REG(0xd628, 0x0003);
    LCD_WRITE_REG(0xd629, 0x00c7);
    LCD_WRITE_REG(0xd62a, 0x0003);
    LCD_WRITE_REG(0xd62b, 0x00c9);
    LCD_WRITE_REG(0xd62c, 0x0003);
    LCD_WRITE_REG(0xd62d, 0x00cb);
    LCD_WRITE_REG(0xd62e, 0x0003);
    LCD_WRITE_REG(0xd62f, 0x00cb);
    LCD_WRITE_REG(0xd630, 0x0003);
    LCD_WRITE_REG(0xd631, 0x00cb);
    LCD_WRITE_REG(0xd632, 0x0003);
    LCD_WRITE_REG(0xd633, 0x00cc);

    LCD_WRITE_REG(0xba00, 0x0024);
    LCD_WRITE_REG(0xba01, 0x0024);
    LCD_WRITE_REG(0xba02, 0x0024);

    LCD_WRITE_REG(0xb900, 0x0024);
    LCD_WRITE_REG(0xb901, 0x0024);
    LCD_WRITE_REG(0xb902, 0x0024);

    LCD_WRITE_REG(0xf000, 0x0055);
    LCD_WRITE_REG(0xf001, 0x00aa);
    LCD_WRITE_REG(0xf002, 0x0052);
    LCD_WRITE_REG(0xf003, 0x0008);
    LCD_WRITE_REG(0xf004, 0x0000);

    LCD_WRITE_REG(0xb100, 0x00cc);
    LCD_WRITE_REG(0xB500, 0x0050);

    LCD_WRITE_REG(0xbc00, 0x0005);
    LCD_WRITE_REG(0xbc01, 0x0005);
    LCD_WRITE_REG(0xbc02, 0x0005);

    LCD_WRITE_REG(0xb800, 0x0001);
    LCD_WRITE_REG(0xb801, 0x0003);
    LCD_WRITE_REG(0xb802, 0x0003);
    LCD_WRITE_REG(0xb803, 0x0003);

    LCD_WRITE_REG(0xbd02, 0x0007);
    LCD_WRITE_REG(0xbd03, 0x0031);
    LCD_WRITE_REG(0xbe02, 0x0007);
    LCD_WRITE_REG(0xbe03, 0x0031);
    LCD_WRITE_REG(0xbf02, 0x0007);
    LCD_WRITE_REG(0xbf03, 0x0031);

    LCD_WRITE_REG(0xff00, 0x00aa);
    LCD_WRITE_REG(0xff01, 0x0055);
    LCD_WRITE_REG(0xff02, 0x0025);
    LCD_WRITE_REG(0xff03, 0x0001);

    LCD_WRITE_REG(0xf304, 0x0011);
    LCD_WRITE_REG(0xf306, 0x0010);
    LCD_WRITE_REG(0xf308, 0x0000);

    LCD_WRITE_REG(0x3500, 0x0000);
    LCD_WRITE_REG(0x3A00, 0x0005);
    //Display On
    LCD_WRITE_CMD(0x2900);
    // Out sleep
    LCD_WRITE_CMD(0x1100);
    // Write continue
    LCD_WRITE_CMD(0x2C00);
}

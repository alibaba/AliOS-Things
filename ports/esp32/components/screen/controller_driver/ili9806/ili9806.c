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
#include "ili9806.h"

static const char *TAG = "lcd ili9806";

#define LCD_CHECK(a, str, ret)  if(!(a)) {                               \
        ESP_LOGE(TAG,"%s:%d (%s):%s", __FILE__, __LINE__, __FUNCTION__, str);   \
        return (ret);                                                           \
    }

#define ILI9806_CASET   0x2A
#define ILI9806_RASET   0x2B
#define ILI9806_RAMWR   0x2C
#define ILI9806_MADCTL  0x36

/* MADCTL Defines */
#define MADCTL_MY  0x80
#define MADCTL_MX  0x40
#define MADCTL_MV  0x20
#define MADCTL_ML  0x10
#define MADCTL_RGB 0x08
#define MADCTL_MH  0x04

#define LCD_NAME "ILI9806"
#define LCD_BPP  16

/** ILI9806 can select different resolution */
#define ILI9806_RESOLUTION_HOR 480  //fixed
#define ILI9806_RESOLUTION_VER 854  //optional

static scr_handle_t g_lcd_handle;

/**
 * This header file is only used to redefine the function to facilitate the call.
 * It can only be placed in this position, not in the head of the file.
 */
#include "interface_drv_def.h"

scr_driver_t lcd_ili9806_default_driver = {
    .init = lcd_ili9806_init,
    .deinit = lcd_ili9806_deinit,
    .set_direction = lcd_ili9806_set_rotation,
    .set_window = lcd_ili9806_set_window,
    .write_ram_data = lcd_ili9806_write_ram_data,
    .draw_pixel = lcd_ili9806_draw_pixel,
    .draw_bitmap = lcd_ili9806_draw_bitmap,
    .get_info = lcd_ili9806_get_info,
};


static void lcd_ili9806_init_reg(void);


esp_err_t lcd_ili9806_init(const scr_controller_config_t *lcd_conf)
{
    LCD_CHECK(lcd_conf->width <= ILI9806_RESOLUTION_HOR, "Width greater than maximum", ESP_ERR_INVALID_ARG);
    LCD_CHECK(lcd_conf->height <= ILI9806_RESOLUTION_VER, "Height greater than maximum", ESP_ERR_INVALID_ARG);
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

    lcd_ili9806_init_reg();

    // Enable backlight
    if (lcd_conf->pin_num_bckl >= 0) {
        gpio_pad_select_gpio(lcd_conf->pin_num_bckl);
        gpio_set_direction(lcd_conf->pin_num_bckl, GPIO_MODE_OUTPUT);
        gpio_set_level(lcd_conf->pin_num_bckl, (lcd_conf->bckl_active_level) & 0x1);
    }

    ret = lcd_ili9806_set_rotation(lcd_conf->rotate);
    LCD_CHECK(ESP_OK == ret, "set rotation failed", ESP_FAIL);

    return ESP_OK;
}

esp_err_t lcd_ili9806_deinit(void)
{
    memset(&g_lcd_handle, 0, sizeof(scr_handle_t));
    return ESP_OK;
}

esp_err_t lcd_ili9806_set_rotation(scr_dir_t dir)
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
    ret = LCD_WRITE_REG(ILI9806_MADCTL, reg_data);
    LCD_CHECK(ESP_OK == ret, "Set screen rotate failed", ESP_FAIL);
    g_lcd_handle.dir = dir;
    return ESP_OK;
}

esp_err_t lcd_ili9806_get_info(scr_info_t *info)
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

esp_err_t lcd_ili9806_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    LCD_CHECK((x1 < g_lcd_handle.width) && (y1 < g_lcd_handle.height), "The set coordinates exceed the screen size", ESP_ERR_INVALID_ARG);
    LCD_CHECK((x0 <= x1) && (y0 <= y1), "Window coordinates invalid", ESP_ERR_INVALID_ARG);
    esp_err_t ret = ESP_OK;
    scr_utility_apply_offset(&g_lcd_handle, ILI9806_RESOLUTION_HOR, ILI9806_RESOLUTION_VER, &x0, &y0, &x1, &y1);

    ret |= LCD_WRITE_CMD(ILI9806_CASET);
    ret |= LCD_WRITE_DATA(x0 >> 8);
    ret |= LCD_WRITE_DATA(x0 & 0xff);
    ret |= LCD_WRITE_DATA(x1 >> 8);
    ret |= LCD_WRITE_DATA(x1 & 0xff);
    ret |= LCD_WRITE_CMD(ILI9806_RASET);
    ret |= LCD_WRITE_DATA(y0 >> 8);
    ret |= LCD_WRITE_DATA(y0 & 0xff);
    ret |= LCD_WRITE_DATA(y1 >> 8);
    ret |= LCD_WRITE_DATA(y1 & 0xff);

    ret |= LCD_WRITE_CMD(ILI9806_RAMWR);
    LCD_CHECK(ESP_OK == ret, "Set window failed", ESP_FAIL);
    return ESP_OK;
}


esp_err_t lcd_ili9806_write_ram_data(uint16_t color)
{
    static uint8_t data[2];
    data[0] = (uint8_t)(color & 0xff);
    data[1] = (uint8_t)(color >> 8);
    return LCD_WRITE(data, 2);
}

esp_err_t lcd_ili9806_draw_pixel(uint16_t x, uint16_t y, uint16_t color)
{
    esp_err_t ret;
    ret = lcd_ili9806_set_window(x, y, x, y);
    if (ESP_OK != ret) {
        return ESP_FAIL;
    }
    return lcd_ili9806_write_ram_data(color);
}

esp_err_t lcd_ili9806_draw_bitmap(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t *bitmap)
{
    esp_err_t ret;
    LCD_CHECK(NULL != bitmap, "bitmap pointer invalid", ESP_ERR_INVALID_ARG);

    LCD_IFACE_ACQUIRE();
    ret = lcd_ili9806_set_window(x, y, x + w - 1, y + h - 1);
    if (ESP_OK != ret) {
        return ESP_FAIL;
    }

    uint32_t len = w * h;
    ret = LCD_WRITE((uint8_t *)bitmap, 2 * len);
    LCD_IFACE_RELEASE();
    LCD_CHECK(ESP_OK == ret, "lcd write ram data failed", ESP_FAIL);
    return ESP_OK;
}

static void lcd_ili9806_init_reg(void)
{
    LCD_WRITE_CMD(0x01); // Software Reset
    vTaskDelay(50 / portTICK_RATE_MS);
    LCD_WRITE_CMD(0xFF); // EXTC Command Set enable register
    LCD_WRITE_DATA(0xFF);
    LCD_WRITE_DATA(0x98);
    LCD_WRITE_DATA(0x06);

    LCD_WRITE_CMD(0xBA); // SPI Interface Setting
    LCD_WRITE_DATA(0xE0);

    LCD_WRITE_CMD(0xBC); // GIP 1
    LCD_WRITE_DATA(0x03);
    LCD_WRITE_DATA(0x0F);
    LCD_WRITE_DATA(0x63);
    LCD_WRITE_DATA(0x69);
    LCD_WRITE_DATA(0x01);
    LCD_WRITE_DATA(0x01);
    LCD_WRITE_DATA(0x1B);
    LCD_WRITE_DATA(0x11);
    LCD_WRITE_DATA(0x70);
    LCD_WRITE_DATA(0x73);
    LCD_WRITE_DATA(0xFF);
    LCD_WRITE_DATA(0xFF);
    LCD_WRITE_DATA(0x08);
    LCD_WRITE_DATA(0x09);
    LCD_WRITE_DATA(0x05);
    LCD_WRITE_DATA(0x00);
    LCD_WRITE_DATA(0xEE);
    LCD_WRITE_DATA(0xE2);
    LCD_WRITE_DATA(0x01);
    LCD_WRITE_DATA(0x00);
    LCD_WRITE_DATA(0xC1);

    LCD_WRITE_CMD(0xBD); // GIP 2
    LCD_WRITE_DATA(0x01);
    LCD_WRITE_DATA(0x23);
    LCD_WRITE_DATA(0x45);
    LCD_WRITE_DATA(0x67);
    LCD_WRITE_DATA(0x01);
    LCD_WRITE_DATA(0x23);
    LCD_WRITE_DATA(0x45);
    LCD_WRITE_DATA(0x67);

    LCD_WRITE_CMD(0xBE); // GIP 3
    LCD_WRITE_DATA(0x00);
    LCD_WRITE_DATA(0x22);
    LCD_WRITE_DATA(0x27);
    LCD_WRITE_DATA(0x6A);
    LCD_WRITE_DATA(0xBC);
    LCD_WRITE_DATA(0xD8);
    LCD_WRITE_DATA(0x92);
    LCD_WRITE_DATA(0x22);
    LCD_WRITE_DATA(0x22);

    LCD_WRITE_CMD(0xC7); // Vcom
    LCD_WRITE_DATA(0x1E);

    LCD_WRITE_CMD(0xED); // EN_volt_reg
    LCD_WRITE_DATA(0x7F);
    LCD_WRITE_DATA(0x0F);
    LCD_WRITE_DATA(0x00);

    LCD_WRITE_CMD(0xC0); // Power Control 1
    LCD_WRITE_DATA(0xE3);
    LCD_WRITE_DATA(0x0B);
    LCD_WRITE_DATA(0x00);

    LCD_WRITE_CMD(0xFC);
    LCD_WRITE_DATA(0x08);

    LCD_WRITE_CMD(0xDF); // Engineering Setting
    LCD_WRITE_DATA(0x00);
    LCD_WRITE_DATA(0x00);
    LCD_WRITE_DATA(0x00);
    LCD_WRITE_DATA(0x00);
    LCD_WRITE_DATA(0x00);
    LCD_WRITE_DATA(0x02);

    LCD_WRITE_CMD(0xF3); // DVDD Voltage Setting
    LCD_WRITE_DATA(0x74);

    LCD_WRITE_CMD(0xB4); // Display Inversion Control
    LCD_WRITE_DATA(0x00);
    LCD_WRITE_DATA(0x00);
    LCD_WRITE_DATA(0x00);

    LCD_WRITE_CMD(0xF7); // Panel Resolution Selection Set.
    switch (ILI9806_RESOLUTION_VER) {
    case 864:
        LCD_WRITE_DATA(0x80); // set to 480x864
        break;
    case 854:
        LCD_WRITE_DATA(0x81); // set to 480x854
        break;
    case 800:
        LCD_WRITE_DATA(0x82); // set to 480x800
        break;
    case 640:
        LCD_WRITE_DATA(0x83); // set to 480x640
        break;
    case 720:
        LCD_WRITE_DATA(0x84); // set to 480x720
        break;
    default:
        ESP_LOGE(TAG, "Unsupported resolution, use default [480x865]");
        break;
    }

    LCD_WRITE_CMD(0xB1); // Frame Rate
    LCD_WRITE_DATA(0x00);
    LCD_WRITE_DATA(0x10);
    LCD_WRITE_DATA(0x14);

    LCD_WRITE_CMD(0xF1); // Panel Timing Control
    LCD_WRITE_DATA(0x29);
    LCD_WRITE_DATA(0x8A);
    LCD_WRITE_DATA(0x07);

    LCD_WRITE_CMD(0xF2); //Panel Timing Control
    LCD_WRITE_DATA(0x40);
    LCD_WRITE_DATA(0xD2);
    LCD_WRITE_DATA(0x50);
    LCD_WRITE_DATA(0x28);

    LCD_WRITE_CMD(0xC1); // Power Control 2
    LCD_WRITE_DATA(0x17);
    LCD_WRITE_DATA(0X85);
    LCD_WRITE_DATA(0x85);
    LCD_WRITE_DATA(0x20);

    LCD_WRITE_CMD(0xE0);
    LCD_WRITE_DATA(0x00); //P1
    LCD_WRITE_DATA(0x0C); //P2
    LCD_WRITE_DATA(0x15); //P3
    LCD_WRITE_DATA(0x0D); //P4
    LCD_WRITE_DATA(0x0F); //P5
    LCD_WRITE_DATA(0x0C); //P6
    LCD_WRITE_DATA(0x07); //P7
    LCD_WRITE_DATA(0x05); //P8
    LCD_WRITE_DATA(0x07); //P9
    LCD_WRITE_DATA(0x0B); //P10
    LCD_WRITE_DATA(0x10); //P11
    LCD_WRITE_DATA(0x10); //P12
    LCD_WRITE_DATA(0x0D); //P13
    LCD_WRITE_DATA(0x17); //P14
    LCD_WRITE_DATA(0x0F); //P15
    LCD_WRITE_DATA(0x00); //P16

    LCD_WRITE_CMD(0xE1);
    LCD_WRITE_DATA(0x00); //P1
    LCD_WRITE_DATA(0x0D); //P2
    LCD_WRITE_DATA(0x15); //P3
    LCD_WRITE_DATA(0x0E); //P4
    LCD_WRITE_DATA(0x10); //P5
    LCD_WRITE_DATA(0x0D); //P6
    LCD_WRITE_DATA(0x08); //P7
    LCD_WRITE_DATA(0x06); //P8
    LCD_WRITE_DATA(0x07); //P9
    LCD_WRITE_DATA(0x0C); //P10
    LCD_WRITE_DATA(0x11); //P11
    LCD_WRITE_DATA(0x11); //P12
    LCD_WRITE_DATA(0x0E); //P13
    LCD_WRITE_DATA(0x17); //P14
    LCD_WRITE_DATA(0x0F); //P15
    LCD_WRITE_DATA(0x00); //P16

    LCD_WRITE_CMD(0x35); //Tearing Effect ON
    LCD_WRITE_DATA(0x00);

    LCD_WRITE_CMD(0x36); //Tearing Effect ON
    LCD_WRITE_DATA(0x60);

    // LCD_WRITE_CMD(0x38);

    LCD_WRITE_CMD(0x3A);
    LCD_WRITE_DATA(0x55);

    LCD_WRITE_CMD(0x11); //Exit Sleep
    vTaskDelay(10 / portTICK_RATE_MS);
    LCD_WRITE_CMD(0x29); // Display On

    LCD_WRITE_CMD(0x36);
    LCD_WRITE_DATA((1 << 6) | (1 << 5));

    LCD_WRITE_CMD(0x2A);
    LCD_WRITE_DATA(0x00);
    LCD_WRITE_DATA(0x00);
    LCD_WRITE_DATA(0x03);
    LCD_WRITE_DATA(0x55);

    LCD_WRITE_CMD(0x2B);
    LCD_WRITE_DATA(0x00);
    LCD_WRITE_DATA(0x00);
    LCD_WRITE_DATA(0x01);
    LCD_WRITE_DATA(0xDF);
    LCD_WRITE_CMD(0x2C);
}

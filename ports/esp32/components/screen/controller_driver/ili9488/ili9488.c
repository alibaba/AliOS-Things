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
#include "ili9488.h"

static const char *TAG = "ILI9488";

#define LCD_CHECK(a, str, ret)  if(!(a)) {                               \
        ESP_LOGE(TAG,"%s:%d (%s):%s", __FILE__, __LINE__, __FUNCTION__, str);   \
        return (ret);                                                           \
    }


#define ILI9488_NOP           0x00
#define ILI9488_SWRESET       0x01
#define ILI9488_RDDID         0x04
#define ILI9488_RDDST         0x09

#define ILI9488_SLPIN         0x10
#define ILI9488_SLPOUT        0x11
#define ILI9488_PTLON         0x12
#define ILI9488_NORON         0x13

#define ILI9488_RDMODE        0x0A
#define ILI9488_RDMADCTL      0x0B
#define ILI9488_RDPIXFMT      0x0C
#define ILI9488_RDIMGFMT      0x0D
#define ILI9488_RDSELFDIAG    0x0F

#define ILI9488_INVOFF        0x20
#define ILI9488_INVON         0x21
#define ILI9488_GAMMASET      0x26
#define ILI9488_DISPOFF       0x28
#define ILI9488_DISPON        0x29

#define ILI9488_CASET         0x2A
#define ILI9488_PASET         0x2B
#define ILI9488_RAMWR         0x2C
#define ILI9488_RAMRD         0x2E

#define ILI9488_PTLAR         0x30
#define ILI9488_VSCRDEF       0x33
#define ILI9488_MADCTL        0x36
#define ILI9488_VSCRSADD      0x37
#define ILI9488_PIXFMT        0x3A
#define ILI9488_RAMWRCONT     0x3C
#define ILI9488_RAMRDCONT     0x3E

#define ILI9488_IMCTR         0xB0
#define ILI9488_FRMCTR1       0xB1
#define ILI9488_FRMCTR2       0xB2
#define ILI9488_FRMCTR3       0xB3
#define ILI9488_INVCTR        0xB4
#define ILI9488_DFUNCTR       0xB6

#define ILI9488_PWCTR1        0xC0
#define ILI9488_PWCTR2        0xC1
#define ILI9488_PWCTR3        0xC2
#define ILI9488_PWCTR4        0xC3
#define ILI9488_PWCTR5        0xC4
#define ILI9488_VMCTR1        0xC5
#define ILI9488_VMCTR2        0xC7

#define ILI9488_RDID1         0xDA
#define ILI9488_RDID2         0xDB
#define ILI9488_RDID3         0xDC
#define ILI9488_RDID4         0xDD

#define ILI9488_GMCTRP1       0xE0
#define ILI9488_GMCTRN1       0xE1
#define ILI9488_IMGFUNCT      0xE9

#define ILI9488_ADJCTR3       0xF7

#define ILI9488_MAD_RGB       0x08
#define ILI9488_MAD_BGR       0x00

#define ILI9488_MAD_VERTICAL  0x20
#define ILI9488_MAD_X_LEFT    0x00
#define ILI9488_MAD_X_RIGHT   0x40
#define ILI9488_MAD_Y_UP      0x80
#define ILI9488_MAD_Y_DOWN    0x00


/* MADCTL Defines */
#define MADCTL_MY  0x80
#define MADCTL_MX  0x40
#define MADCTL_MV  0x20
#define MADCTL_ML  0x10
#define MADCTL_RGB 0x08
#define MADCTL_MH  0x04

#define LCD_NAME "ILI9488"
#define LCD_BPP  16

#define ILI9488_RESOLUTION_HOR 320
#define ILI9488_RESOLUTION_VER 480

static scr_handle_t g_lcd_handle;

/**
 * This header file is only used to redefine the function to facilitate the call.
 * It can only be placed in this position, not in the head of the file.
 */
#include "interface_drv_def.h"

scr_driver_t lcd_ili9488_default_driver = {
    .init = lcd_ili9488_init,
    .deinit = lcd_ili9488_deinit,
    .set_direction = lcd_ili9488_set_rotation,
    .set_window = lcd_ili9488_set_window,
    .write_ram_data = lcd_ili9488_write_ram_data,
    .draw_pixel = lcd_ili9488_draw_pixel,
    .draw_bitmap = lcd_ili9488_draw_bitmap,
    .get_info = lcd_ili9488_get_info,
};


static void lcd_ili9488_init_reg(void);

esp_err_t lcd_ili9488_init(const scr_controller_config_t *lcd_conf)
{
    LCD_CHECK(lcd_conf->width <= ILI9488_RESOLUTION_HOR, "Width greater than maximum", ESP_ERR_INVALID_ARG);
    LCD_CHECK(lcd_conf->height <= ILI9488_RESOLUTION_VER, "Height greater than maximum", ESP_ERR_INVALID_ARG);
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

    lcd_ili9488_init_reg();

    // Enable backlight
    if (lcd_conf->pin_num_bckl >= 0) {
        gpio_pad_select_gpio(lcd_conf->pin_num_bckl);
        gpio_set_direction(lcd_conf->pin_num_bckl, GPIO_MODE_OUTPUT);
        gpio_set_level(lcd_conf->pin_num_bckl, (lcd_conf->bckl_active_level) & 0x1);
    }

    ret = lcd_ili9488_set_rotation(lcd_conf->rotate);
    LCD_CHECK(ESP_OK == ret, "set rotation failed", ESP_FAIL);
    ret = lcd_ili9488_set_invert(1);  /**< ILI9488 setting the reverse color is the normal color */
    LCD_CHECK(ESP_OK == ret, "Set color invert failed", ESP_FAIL);
    return ESP_OK;
}

esp_err_t lcd_ili9488_deinit(void)
{
    memset(&g_lcd_handle, 0, sizeof(scr_handle_t));
    return ESP_OK;
}

esp_err_t lcd_ili9488_set_rotation(scr_dir_t dir)
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
    ret = LCD_WRITE_REG(ILI9488_MADCTL, reg_data);
    LCD_CHECK(ESP_OK == ret, "Set screen rotate failed", ESP_FAIL);
    g_lcd_handle.dir = dir;
    return ESP_OK;
}

esp_err_t lcd_ili9488_get_info(scr_info_t *info)
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

esp_err_t lcd_ili9488_set_invert(bool is_invert)
{
    return LCD_WRITE_CMD(is_invert ? ILI9488_INVON : ILI9488_INVOFF);
}

esp_err_t lcd_ili9488_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    LCD_CHECK((x1 < g_lcd_handle.width) && (y1 < g_lcd_handle.height), "The set coordinates exceed the screen size", ESP_ERR_INVALID_ARG);
    LCD_CHECK((x0 <= x1) && (y0 <= y1), "Window coordinates invalid", ESP_ERR_INVALID_ARG);
    esp_err_t ret = ESP_OK;
    scr_utility_apply_offset(&g_lcd_handle, ILI9488_RESOLUTION_HOR, ILI9488_RESOLUTION_VER, &x0, &y0, &x1, &y1);

    ret |= LCD_WRITE_CMD(ILI9488_CASET);
    ret |= LCD_WRITE_DATA(x0 >> 8);
    ret |= LCD_WRITE_DATA(x0 & 0xff);
    ret |= LCD_WRITE_DATA(x1 >> 8);
    ret |= LCD_WRITE_DATA(x1 & 0xff);
    ret |= LCD_WRITE_CMD(ILI9488_PASET);
    ret |= LCD_WRITE_DATA(y0 >> 8);
    ret |= LCD_WRITE_DATA(y0 & 0xff);
    ret |= LCD_WRITE_DATA(y1 >> 8);
    ret |= LCD_WRITE_DATA(y1 & 0xff);

    ret |= LCD_WRITE_CMD(ILI9488_RAMWR);
    LCD_CHECK(ESP_OK == ret, "Set window failed", ESP_FAIL);
    return ESP_OK;
}

esp_err_t lcd_ili9488_write_ram_data(uint16_t color)
{
    static uint8_t data[2];
    data[0] = (uint8_t)(color & 0xff);
    data[1] = (uint8_t)(color >> 8);
    return LCD_WRITE(data, 2);
}

esp_err_t lcd_ili9488_draw_pixel(uint16_t x, uint16_t y, uint16_t color)
{
    esp_err_t ret;
    ret = lcd_ili9488_set_window(x, y, x, y);
    if (ESP_OK != ret) {
        return ESP_FAIL;
    }
    return lcd_ili9488_write_ram_data(color);
}

esp_err_t lcd_ili9488_draw_bitmap(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t *bitmap)
{
    esp_err_t ret;
    LCD_CHECK(NULL != bitmap, "bitmap pointer invalid", ESP_ERR_INVALID_ARG);

    LCD_IFACE_ACQUIRE();
    ret = lcd_ili9488_set_window(x, y, x + w - 1, y + h - 1);
    if (ESP_OK != ret) {
        return ESP_FAIL;
    }

    uint32_t len = w * h;
    ret = LCD_WRITE((uint8_t *)bitmap, 2 * len);
    LCD_IFACE_RELEASE();
    LCD_CHECK(ESP_OK == ret, "lcd write ram data failed", ESP_FAIL);
    return ESP_OK;
}

static void lcd_ili9488_init_reg(void)
{
    LCD_WRITE_CMD(ILI9488_SWRESET);
    vTaskDelay(120 / portTICK_RATE_MS);
    // positive gamma control
    LCD_WRITE_CMD(ILI9488_GMCTRP1);
    LCD_WRITE_DATA(0x00);
    LCD_WRITE_DATA(0x03);
    LCD_WRITE_DATA(0x09);
    LCD_WRITE_DATA(0x08);
    LCD_WRITE_DATA(0x16);
    LCD_WRITE_DATA(0x0A);
    LCD_WRITE_DATA(0x3F);
    LCD_WRITE_DATA(0x78);
    LCD_WRITE_DATA(0x4C);
    LCD_WRITE_DATA(0x09);
    LCD_WRITE_DATA(0x0A);
    LCD_WRITE_DATA(0x08);
    LCD_WRITE_DATA(0x16);
    LCD_WRITE_DATA(0x1A);
    LCD_WRITE_DATA(0x0F);

    // negative gamma control
    LCD_WRITE_CMD(ILI9488_GMCTRN1);
    LCD_WRITE_DATA(0x00);
    LCD_WRITE_DATA(0x16);
    LCD_WRITE_DATA(0x19);
    LCD_WRITE_DATA(0x03);
    LCD_WRITE_DATA(0x0F);
    LCD_WRITE_DATA(0x05);
    LCD_WRITE_DATA(0x32);
    LCD_WRITE_DATA(0x45);
    LCD_WRITE_DATA(0x46);
    LCD_WRITE_DATA(0x04);
    LCD_WRITE_DATA(0x0E);
    LCD_WRITE_DATA(0x0D);
    LCD_WRITE_DATA(0x35);
    LCD_WRITE_DATA(0x37);
    LCD_WRITE_DATA(0x0F);

    // Power Control 1 (Vreg1out, Verg2out)
    LCD_WRITE_CMD(ILI9488_PWCTR1);
    LCD_WRITE_DATA(0x17);
    LCD_WRITE_DATA(0x15);

    // Power Control 2 (VGH,VGL)
    LCD_WRITE_CMD(ILI9488_PWCTR2);
    LCD_WRITE_DATA(0x41);
    // Power Control 3 (Vcom)
    LCD_WRITE_CMD(ILI9488_VMCTR1);
    LCD_WRITE_DATA(0x00);
    LCD_WRITE_DATA(0x12);
    LCD_WRITE_DATA(0x80);

    LCD_WRITE_CMD(ILI9488_IMCTR); LCD_WRITE_DATA(0x80); // Interface Mode Control (SDO NOT USE)

    LCD_WRITE_CMD(ILI9488_PIXFMT); LCD_WRITE_DATA(0x55); // Interface Pixel Format (16 bit)

    LCD_WRITE_CMD(ILI9488_FRMCTR1); LCD_WRITE_DATA(0xA0); // Frame rate (60Hz)
    LCD_WRITE_CMD(ILI9488_INVCTR); LCD_WRITE_DATA(0x02); // Display Inversion Control (2-dot)
    LCD_WRITE_CMD(ILI9488_DFUNCTR); // Display Function Control RGB/MCU Interface Control
    LCD_WRITE_DATA(0x02);
    LCD_WRITE_DATA(0x02);

    LCD_WRITE_CMD(ILI9488_IMGFUNCT); // Set Image Functio (Disable 24 bit data)
    LCD_WRITE_DATA(0x00);
    LCD_WRITE_CMD(ILI9488_ADJCTR3); // Adjust Control (D7 stream, loose)
    LCD_WRITE_DATA(0xa9);
    LCD_WRITE_DATA(0x51);
    LCD_WRITE_DATA(0x2c);
    LCD_WRITE_DATA(0x82);

    LCD_WRITE_CMD(ILI9488_SLPOUT);      // Exit Sleep
    LCD_WRITE_CMD(ILI9488_DISPON);      // Display on
    vTaskDelay(120 / portTICK_RATE_MS);
}

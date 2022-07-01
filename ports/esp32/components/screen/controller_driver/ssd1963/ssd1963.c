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
#include "ssd1963.h"

static const char *TAG = "lcd ssd1963";

#define LCD_CHECK(a, str, ret)  if(!(a)) {                               \
        ESP_LOGE(TAG,"%s:%d (%s):%s", __FILE__, __LINE__, __FUNCTION__, str);   \
        return (ret);                                                           \
    }

#define LCD_NAME "SSD1963"
#define LCD_BPP  16

#define SSD1963_CASET   0x2A
#define SSD1963_RASET   0x2B
#define SSD1963_RAMWR   0x2C
#define SSD1963_MADCTL  0x36

/* MADCTL Defines */
#define MADCTL_MY  0x01
#define MADCTL_MX  0x02
#define MADCTL_MV  0x20
#define MADCTL_ML  0x10
#define MADCTL_RGB 0x08
#define MADCTL_MH  0x04

#define SSD1963_RESOLUTION_HOR 800
#define SSD1963_RESOLUTION_VER 480

//LCD panel configuration
#define SSD_HOR_PULSE_WIDTH     1
#define SSD_HOR_BACK_PORCH      46
#define SSD_HOR_FRONT_PORCH     210

#define SSD_VER_PULSE_WIDTH     1
#define SSD_VER_BACK_PORCH      23
#define SSD_VER_FRONT_PORCH     22

#define SSD_HT  (SSD1963_RESOLUTION_HOR+SSD_HOR_BACK_PORCH+SSD_HOR_FRONT_PORCH)
#define SSD_HPS (SSD_HOR_BACK_PORCH)
#define SSD_VT  (SSD1963_RESOLUTION_VER+SSD_VER_BACK_PORCH+SSD_VER_FRONT_PORCH)
#define SSD_VPS (SSD_VER_BACK_PORCH)

static scr_handle_t g_lcd_handle;

/**
 * This header file is only used to redefine the function to facilitate the call.
 * It can only be placed in this position, not in the head of the file.
 */
#include "interface_drv_def.h"

scr_driver_t lcd_ssd1963_default_driver = {
    .init = lcd_ssd1963_init,
    .deinit = lcd_ssd1963_deinit,
    .set_direction = lcd_ssd1963_set_rotation,
    .set_window = lcd_ssd1963_set_window,
    .write_ram_data = lcd_ssd1963_write_ram_data,
    .draw_pixel = lcd_ssd1963_draw_pixel,
    .draw_bitmap = lcd_ssd1963_draw_bitmap,
    .get_info = lcd_ssd1963_get_info,
};

static void lcd_ssd1963_init_reg(void);

esp_err_t lcd_ssd1963_init(const scr_controller_config_t *lcd_conf)
{
    LCD_CHECK(lcd_conf->width <= SSD1963_RESOLUTION_HOR, "Width greater than maximum", ESP_ERR_INVALID_ARG);
    LCD_CHECK(lcd_conf->height <= SSD1963_RESOLUTION_VER, "Height greater than maximum", ESP_ERR_INVALID_ARG);
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

    lcd_ssd1963_init_reg();

    // Enable backlight
    if (lcd_conf->pin_num_bckl >= 0) {
        gpio_pad_select_gpio(lcd_conf->pin_num_bckl);
        gpio_set_direction(lcd_conf->pin_num_bckl, GPIO_MODE_OUTPUT);
        gpio_set_level(lcd_conf->pin_num_bckl, (lcd_conf->bckl_active_level) & 0x1);
    }

    ret = lcd_ssd1963_set_rotation(lcd_conf->rotate);
    LCD_CHECK(ESP_OK == ret, "set rotation failed", ESP_FAIL);

    return ESP_OK;
}

esp_err_t lcd_ssd1963_deinit(void)
{
    memset(&g_lcd_handle, 0, sizeof(scr_handle_t));
    return ESP_OK;
}

esp_err_t lcd_ssd1963_set_rotation(scr_dir_t dir)
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
    ret = LCD_WRITE_REG(SSD1963_MADCTL, reg_data);
    LCD_CHECK(ESP_OK == ret, "Set screen rotate failed", ESP_FAIL);
    g_lcd_handle.dir = dir;
    return ESP_OK;
}

esp_err_t lcd_ssd1963_get_info(scr_info_t *info)
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

esp_err_t lcd_ssd1963_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    LCD_CHECK((x1 < g_lcd_handle.width) && (y1 < g_lcd_handle.height), "The set coordinates exceed the screen size", ESP_ERR_INVALID_ARG);
    LCD_CHECK((x0 <= x1) && (y0 <= y1), "Window coordinates invalid", ESP_ERR_INVALID_ARG);
    esp_err_t ret = ESP_OK;
    scr_utility_apply_offset(&g_lcd_handle, SSD1963_RESOLUTION_HOR, SSD1963_RESOLUTION_VER, &x0, &y0, &x1, &y1);

    LCD_WRITE_CMD(SSD1963_CASET);
    LCD_WRITE_DATA(x0 >> 8);
    LCD_WRITE_DATA(x0 & 0XFF);
    LCD_WRITE_DATA(x1 >> 8);
    LCD_WRITE_DATA(x1 & 0XFF);
    LCD_WRITE_CMD(SSD1963_RASET);
    LCD_WRITE_DATA(y0 >> 8);
    LCD_WRITE_DATA(y0 & 0XFF);
    LCD_WRITE_DATA(y1 >> 8);
    LCD_WRITE_DATA(y1 & 0XFF);

    ret |= LCD_WRITE_CMD(SSD1963_RAMWR);
    LCD_CHECK(ESP_OK == ret, "Set window failed", ESP_FAIL);
    return ESP_OK;
}

esp_err_t lcd_ssd1963_write_ram_data(uint16_t color)
{
    static uint8_t data[2];
    data[0] = (uint8_t)(color & 0xff);
    data[1] = (uint8_t)(color >> 8);
    return LCD_WRITE(data, 2);
}

esp_err_t lcd_ssd1963_draw_pixel(uint16_t x, uint16_t y, uint16_t color)
{
    esp_err_t ret;
    ret = lcd_ssd1963_set_window(x, y, x, y);
    if (ESP_OK != ret) {
        return ESP_FAIL;
    }
    return lcd_ssd1963_write_ram_data(color);
}

esp_err_t lcd_ssd1963_draw_bitmap(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t *bitmap)
{
    LCD_CHECK((x + w <= g_lcd_handle.width) && (y + h <= g_lcd_handle.height), "The set coordinates exceed the screen size", ESP_ERR_INVALID_ARG);
    esp_err_t ret = ESP_OK;
    uint8_t *p = (uint8_t *)bitmap;

    LCD_IFACE_ACQUIRE();
    ret = lcd_ssd1963_set_window(x, y, x + w - 1, y + h - 1);
    if (ESP_OK != ret) {
        return ESP_FAIL;
    }

    ret = LCD_WRITE(p, w * LCD_BPP / 8 * h);
    LCD_IFACE_RELEASE();
    LCD_CHECK(ESP_OK == ret, "Draw bitmap failed", ESP_FAIL);
    return ESP_OK;
}

static void lcd_ssd1963_init_reg(void)
{
    LCD_WRITE_CMD(0xE2);        //Set PLL with OSC = 10MHz (hardware),  Multiplier N = 35, 250MHz < VCO < 800MHz = OSC*(N+1), VCO = 300MHz
    LCD_WRITE_DATA(0x1D);       //
    LCD_WRITE_DATA(0x02);       //Divider M = 2, PLL = 300/(M+1) = 100MHz
    LCD_WRITE_DATA(0x04);       //Validate M and N values

    vTaskDelay(pdMS_TO_TICKS(1));

    LCD_WRITE_CMD(0xE0);        // Start PLL command
    LCD_WRITE_DATA(0x01);       // enable PLL

    vTaskDelay(pdMS_TO_TICKS(10));

    LCD_WRITE_CMD(0xE0);        // Start PLL command again
    LCD_WRITE_DATA(0x03);       // now, use PLL output as system clock

    vTaskDelay(pdMS_TO_TICKS(12));

    LCD_WRITE_CMD(0x01);        //soft-reset

    vTaskDelay(pdMS_TO_TICKS(10));

    LCD_WRITE_CMD(0xE6);        //set pixel frequency,33Mhz
    LCD_WRITE_DATA(0x2F);
    LCD_WRITE_DATA(0xFF);
    LCD_WRITE_DATA(0xFF);

    LCD_WRITE_CMD(0xB0);        //set LCD mode
    LCD_WRITE_DATA(0x20);       //24-bit mode
    LCD_WRITE_DATA(0x00);       //TFT
    LCD_WRITE_DATA((SSD1963_RESOLUTION_HOR - 1) >> 8); //set LCD horizontal pixel number
    LCD_WRITE_DATA(SSD1963_RESOLUTION_HOR - 1);
    LCD_WRITE_DATA((SSD1963_RESOLUTION_VER - 1) >> 8); //set LCD vertical pixel number
    LCD_WRITE_DATA(SSD1963_RESOLUTION_VER - 1);
    LCD_WRITE_DATA(0x00);       //RGB

    LCD_WRITE_CMD(0xB4);        //Set horizontal period
    LCD_WRITE_DATA((SSD_HT - 1) >> 8);
    LCD_WRITE_DATA(SSD_HT - 1);
    LCD_WRITE_DATA(SSD_HPS >> 8);
    LCD_WRITE_DATA(SSD_HPS);
    LCD_WRITE_DATA(SSD_HOR_PULSE_WIDTH - 1);
    LCD_WRITE_DATA(0x00);
    LCD_WRITE_DATA(0x00);
    LCD_WRITE_DATA(0x00);

    LCD_WRITE_CMD(0xB6);        //Set vertical period
    LCD_WRITE_DATA((SSD_VT - 1) >> 8);
    LCD_WRITE_DATA(SSD_VT - 1);
    LCD_WRITE_DATA(SSD_VPS >> 8);
    LCD_WRITE_DATA(SSD_VPS);
    LCD_WRITE_DATA(SSD_VER_FRONT_PORCH - 1);
    LCD_WRITE_DATA(0x00);
    LCD_WRITE_DATA(0x00);

    LCD_WRITE_CMD(0xF0);    //set SSD1963 interface is 16bit
    LCD_WRITE_DATA(0x03);   //16-bit(565 format) data for 16bpp

    LCD_WRITE_CMD(0x29);    //display on
    
    LCD_WRITE_CMD(0xD0);
    LCD_WRITE_DATA(0x00);   //disable

    LCD_WRITE_CMD(0xBE);    //configuration PWM output
    LCD_WRITE_DATA(0x05);   //1 PWM frequency
    LCD_WRITE_DATA(0xFE);   //2 PWM duty
    LCD_WRITE_DATA(0x01);   //3 C
    LCD_WRITE_DATA(0x00);   //4 D
    LCD_WRITE_DATA(0x00);   //5 E
    LCD_WRITE_DATA(0x00);   //6 F

    LCD_WRITE_CMD(0xB8);    //set GPIO
    LCD_WRITE_DATA(0x03);
    LCD_WRITE_DATA(0x01);

    LCD_WRITE_CMD(0xBA);
    LCD_WRITE_DATA(0X01);   //GPIO[1:0]=01

}

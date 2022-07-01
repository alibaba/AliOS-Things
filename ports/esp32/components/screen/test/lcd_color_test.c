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
#include "unity.h"
#include "test_utils.h"
#include "esp_log.h"

#include "screen_driver.h"

static const char *TAG = "lcd test";

static void screen_clear(scr_driver_t *lcd, int color)
{
    scr_info_t lcd_info;
    lcd->get_info(&lcd_info);

    uint16_t *buffer = malloc(lcd_info.width * sizeof(uint16_t));
    if (NULL == buffer) {
        for (size_t y = 0; y < lcd_info.height; y++) {
            for (size_t x = 0; x < lcd_info.width; x++) {
                lcd->draw_pixel(x, y, color);
            }
        }
    } else {
        for (size_t i = 0; i < lcd_info.width; i++) {
            buffer[i] = color;
        }

        for (int y = 0; y < lcd_info.height; y++) {
            lcd->draw_bitmap(0, y, lcd_info.width, 1, buffer);
        }

        free(buffer);
    }
}

static void color_hsv_to_rgb(uint16_t h, uint8_t s, uint8_t v, uint8_t *r, uint8_t *g, uint8_t *b)
{
    h = (uint32_t)((uint32_t)h * 255) / 360;
    s = (uint16_t)((uint16_t)s * 255) / 100;
    v = (uint16_t)((uint16_t)v * 255) / 100;

    uint8_t rr, gg, bb;

    uint8_t region, remainder, p, q, t;

    if (s == 0) {
        rr = v;
        gg = v;
        bb = v;
        *r = *g = *b = v;
        return ;
    }

    region = h / 43;
    remainder = (h - (region * 43)) * 6;

    p = (v * (255 - s)) >> 8;
    q = (v * (255 - ((s * remainder) >> 8))) >> 8;
    t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;

    switch (region) {
    case 0:
        rr = v;
        gg = t;
        bb = p;
        break;
    case 1:
        rr = q;
        gg = v;
        bb = p;
        break;
    case 2:
        rr = p;
        gg = v;
        bb = t;
        break;
    case 3:
        rr = p;
        gg = q;
        bb = v;
        break;
    case 4:
        rr = t;
        gg = p;
        bb = v;
        break;
    default:
        rr = v;
        gg = p;
        bb = q;
        break;
    }

    *r = rr;
    *g = gg;
    *b = bb;
}

static uint16_t rgb888_to_565(uint8_t r, uint8_t g, uint8_t b)
{
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

static void screen_color_line(scr_driver_t *lcd, uint16_t *buffer, size_t length, uint8_t mode)
{
    scr_info_t lcd_info;
    lcd->get_info(&lcd_info);
    uint8_t r, g, b;
    uint16_t w = lcd_info.width;
    uint16_t h = lcd_info.height;

    if (length < w) {
        return;
    }

    for (size_t y = 0; y < h; y++) {
        uint16_t color = 0;
        switch (mode) {
        case 0:
            color = rgb888_to_565(y * 256 / h, 0, 0);
            break;
        case 1:
            color = rgb888_to_565(0, y * 256 / h, 0);
            break;
        case 2:
            color = rgb888_to_565(0, 0, y * 256 / h);
            break;

        case 3:
            color_hsv_to_rgb(y * 360 / h, 100, 100, &r, &g, &b);
            color = rgb888_to_565(r, g, b);
            break;
        case 4:
            color_hsv_to_rgb(120, y * 100 / h, 100, &r, &g, &b);
            color = rgb888_to_565(r, g, b);
            break;
        case 5:
            color_hsv_to_rgb(120, 100, y * 100 / h, &r, &g, &b);
            color = rgb888_to_565(r, g, b);
            break;

        default:
            break;
        }
        for (size_t x = 0; x < w; x++) {
            buffer[x] = color;
        }
        lcd->draw_bitmap(0, y, lcd_info.width, 1, buffer);
    }
}

static void lcd_color_test(scr_driver_t *lcd)
{
    scr_info_t lcd_info;
    TEST_ASSERT(ESP_OK == lcd->get_info(&lcd_info));

    const uint16_t color_table[] = {COLOR_RED, COLOR_GREEN, COLOR_BLUE, COLOR_YELLOW};
    for (int c = 0; c < sizeof(color_table) / sizeof(uint16_t); c++) {
        screen_clear(lcd, color_table[c]);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    uint16_t *buffer = malloc(lcd_info.width * sizeof(uint16_t));
    TEST_ASSERT_NOT_NULL(buffer);

    for (size_t i = 0; i < 6; i++) {
        screen_color_line(lcd, buffer, lcd_info.width, i);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    free(buffer);
}

static void lcd_write_ram_data_test(scr_driver_t *lcd)
{
    scr_info_t lcd_info;
    TEST_ASSERT(ESP_OK == lcd->get_info(&lcd_info));
    screen_clear(lcd, COLOR_WHITE);

    uint16_t c = COLOR_ESP_BKGD;
    lcd->set_window(0, 0, 50, 50);
    for (size_t i = 0; i < 2500; i++) {
        lcd->write_ram_data(c);
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

static void lcd_draw_pixel_test(scr_driver_t *lcd)
{
    scr_info_t lcd_info;
    TEST_ASSERT(ESP_OK == lcd->get_info(&lcd_info));
    screen_clear(lcd, COLOR_WHITE);

    uint16_t c = COLOR_ESP_BKGD;
    for (size_t y = 0; y < lcd_info.height; y++) {
        lcd->draw_pixel(0, y, c);
        lcd->draw_pixel(1, y, c);
        lcd->draw_pixel(lcd_info.width - 1, y, c);
        lcd->draw_pixel(lcd_info.width - 2, y, c);
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    for (size_t x = 0; x < lcd_info.width; x++) {
        lcd->draw_pixel(x, 0, c);
        lcd->draw_pixel(x, 1, c);
        lcd->draw_pixel(x, lcd_info.height - 1, c);
        lcd->draw_pixel(x, lcd_info.height - 2, c);
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

static void lcd_rotate_pixel_test(scr_driver_t *lcd)
{
    scr_info_t lcd_info;
    TEST_ASSERT(ESP_OK == lcd->get_info(&lcd_info));
    screen_clear(lcd, COLOR_WHITE);

    uint16_t c = COLOR_ESP_BKGD;
    for (size_t i = 0; i < 8; i++) {
        lcd->set_direction(i);
        lcd->get_info(&lcd_info);
        screen_clear(lcd, COLOR_WHITE);
        ESP_LOGI(TAG, "Screen name:%s | width:%d | height:%d", lcd_info.name, lcd_info.width, lcd_info.height);

        for (size_t y = 0; y < 20; y++) {
            for (size_t x = 0; x < 50; x++) {
                lcd->draw_pixel(x, y, c);
                vTaskDelay(5 / portTICK_PERIOD_MS);
            }
        }
    }
}

static const unsigned char gImage_img_120x39[];

static void lcd_rotate_bitmap_test(scr_driver_t *lcd)
{
    scr_info_t lcd_info;
    TEST_ASSERT(ESP_OK == lcd->get_info(&lcd_info));

    uint16_t *pixels = heap_caps_malloc((120 * 39) * sizeof(uint16_t), MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(pixels);
    for (size_t i = 0; i < 8; i++) {
        lcd->set_direction(i);
        lcd->get_info(&lcd_info);
        screen_clear(lcd, COLOR_WHITE);
        ESP_LOGI(TAG, "Screen name:%s | width:%d | height:%d", lcd_info.name, lcd_info.width, lcd_info.height);

        memcpy(pixels, gImage_img_120x39, (120 * 39) * sizeof(uint16_t));
        lcd->draw_bitmap(0, 0, 120, 39, (uint16_t *)pixels);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
    heap_caps_free(pixels);
}

static void lcd_speed_test(scr_driver_t *lcd)
{
    scr_info_t lcd_info;
    TEST_ASSERT(ESP_OK == lcd->get_info(&lcd_info));

    uint32_t w = 240, h = 240;
    w = lcd_info.width < w ? lcd_info.width : w;
    h = lcd_info.height < h ? lcd_info.height : h;

    const uint32_t buffer_size = w * h;
    uint16_t *pixels = heap_caps_malloc(buffer_size * sizeof(uint16_t), MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(pixels);

    const uint16_t color_table[] = {COLOR_RED, COLOR_GREEN, COLOR_BLUE, COLOR_YELLOW};
    uint32_t times = 256;

    uint64_t s;
    uint64_t t1 = 0;
    for (int i = 0; i < times; i++) {
        for (int j = 0; j < buffer_size; j++) {
            pixels[j] = color_table[i % 4];
        }
        s = esp_timer_get_time();
        lcd->draw_bitmap(0, 0, w, h, pixels);
        t1 += (esp_timer_get_time() - s);
    }
    t1 = t1 / 1000;
    float time_per_frame = (float)t1 / (float)times;
    float fps = (float)times * 1000.f / (float)t1;
    float write_speed = sizeof(uint16_t) * buffer_size * times / 1024.0f / 1.0240f / (float)t1;
    float factor = ((float)w * (float)h) / ((float)lcd_info.width * (float)lcd_info.height);
    ESP_LOGI(TAG, "-------%s Test Result------", lcd_info.name);
    if (w != lcd_info.width || h != lcd_info.height) {
        ESP_LOGI(TAG, "@resolution 240x240          [time per frame=%.2fMS, fps=%.2f, speed=%.2fMB/s]", time_per_frame, fps, write_speed);
        ESP_LOGI(TAG, "@resolution %ux%u infer to [time per frame=%.2fMS, fps=%.2f]",
                 lcd_info.width, lcd_info.height,
                 time_per_frame / factor, factor * fps);
    } else {
        ESP_LOGI(TAG, "@resolution %ux%u          [time per frame=%.2fMS, fps=%.2f, speed=%.2fMB/s]", lcd_info.width, lcd_info.height, time_per_frame, fps, write_speed);
    }
    ESP_LOGI(TAG, "-------------------------------------");

    heap_caps_free(pixels);
}

static scr_interface_driver_t *get_8080_iface(void)
{
    i2s_lcd_config_t i2s_lcd_cfg = {
        .data_width  = 16,
        .pin_data_num = {
#ifdef CONFIG_IDF_TARGET_ESP32
            19, 21, 0, 22, 23, 33, 32, 27, 25, 26, 12, 13, 14, 15, 2, 4,
#else
            // 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1,
            1, 10, 2, 11, 3, 12, 4, 13, 5, 14, 6, 15, 7, 16, 8, 17,
#endif
        },
        .pin_num_cs = -1,
#ifdef CONFIG_IDF_TARGET_ESP32
        .pin_num_wr = 18,
        .pin_num_rs = 5,
#else
        .pin_num_wr = 33,
        .pin_num_rs = 38,
#endif
        .clk_freq = 20000000,
        .i2s_port = I2S_NUM_0,
        .buffer_size = 32000,
        .swap_data = false,
    };

    scr_interface_driver_t *iface_drv_i2s;
    TEST_ASSERT(ESP_OK == scr_interface_create(SCREEN_IFACE_8080, &i2s_lcd_cfg, &iface_drv_i2s));
    return iface_drv_i2s;
}

static void lcd_colorful_test_all(scr_driver_t *lcd)
{
    lcd_color_test(lcd);
    lcd_write_ram_data_test(lcd);
    lcd_draw_pixel_test(lcd);
    lcd_rotate_pixel_test(lcd);
    lcd_rotate_bitmap_test(lcd);
    lcd_speed_test(lcd);
}

TEST_CASE("Screen ILI9806 8080 test", "[screen][iot]")
{
    scr_driver_t lcd;
    scr_interface_driver_t *iface_drv = get_8080_iface();
    scr_controller_config_t lcd_cfg = {0};
    lcd_cfg.interface_drv = iface_drv;
    lcd_cfg.pin_num_rst = -1;
#ifdef CONFIG_IDF_TARGET_ESP32
    lcd_cfg.pin_num_bckl = -1;
#else
    lcd_cfg.pin_num_bckl = 21;
#endif
    lcd_cfg.rst_active_level = 0;
    lcd_cfg.bckl_active_level = 1;
    lcd_cfg.offset_hor = 0;
    lcd_cfg.offset_ver = 0;
    lcd_cfg.width = 480;
    lcd_cfg.height = 854;
    lcd_cfg.rotate = SCR_DIR_LRBT;
    TEST_ASSERT(ESP_OK == scr_find_driver(SCREEN_CONTROLLER_ILI9806, &lcd));
    TEST_ASSERT(ESP_OK == lcd.init(&lcd_cfg));

    lcd_colorful_test_all(&lcd);

    lcd.deinit();
    scr_interface_delete(iface_drv);
}

TEST_CASE("Screen RM68210 8080 test", "[screen][iot]")
{
    scr_driver_t lcd;
    scr_interface_driver_t *iface_drv = get_8080_iface();
    scr_controller_config_t lcd_cfg = {0};
    lcd_cfg.interface_drv = iface_drv;
    lcd_cfg.pin_num_rst = -1;
#ifdef CONFIG_IDF_TARGET_ESP32
    lcd_cfg.pin_num_bckl = -1;
#else
    lcd_cfg.pin_num_bckl = -1;
#endif
    lcd_cfg.rst_active_level = 0;
    lcd_cfg.bckl_active_level = 1;
    lcd_cfg.offset_hor = 0;
    lcd_cfg.offset_ver = 0;
    lcd_cfg.width = 480;
    lcd_cfg.height = 800;
    lcd_cfg.rotate = SCR_DIR_LRBT;
    TEST_ASSERT(ESP_OK == scr_find_driver(SCREEN_CONTROLLER_RM68120, &lcd));
    TEST_ASSERT(ESP_OK == lcd.init(&lcd_cfg));

    lcd_colorful_test_all(&lcd);

    lcd.deinit();
    scr_interface_delete(iface_drv);
}

TEST_CASE("Screen SSD1963 8080 test", "[screen][iot]")
{
    scr_driver_t lcd;
    scr_interface_driver_t *iface_drv = get_8080_iface();
    scr_controller_config_t lcd_cfg = {0};
    lcd_cfg.interface_drv = iface_drv;
    lcd_cfg.pin_num_rst = -1;
#ifdef CONFIG_IDF_TARGET_ESP32
    lcd_cfg.pin_num_bckl = -1;
#else
    lcd_cfg.pin_num_bckl = -1;
#endif
    lcd_cfg.rst_active_level = 0;
    lcd_cfg.bckl_active_level = 1;
    lcd_cfg.offset_hor = 0;
    lcd_cfg.offset_ver = 0;
    lcd_cfg.width = 800;
    lcd_cfg.height = 480;
    lcd_cfg.rotate = SCR_DIR_LRTB;
    TEST_ASSERT(ESP_OK == scr_find_driver(SCREEN_CONTROLLER_SSD1963, &lcd));
    TEST_ASSERT(ESP_OK == lcd.init(&lcd_cfg));

    lcd_colorful_test_all(&lcd);

    lcd.deinit();
    scr_interface_delete(iface_drv);
}

TEST_CASE("Screen ILI9341 SPI test", "[screen][iot]")
{
    scr_driver_t lcd;
    spi_config_t spi_cfg = {
        .miso_io_num = -1,
        .mosi_io_num = 21,
        .sclk_io_num = 22,
        .max_transfer_sz = 320 * 480,
    };
    spi_bus_handle_t spi_bus = spi_bus_create(2, &spi_cfg);
    TEST_ASSERT_NOT_NULL(spi_bus);

    scr_interface_spi_config_t spi_lcd_cfg = {
        .spi_bus = spi_bus,
        .pin_num_cs = 5,
        .pin_num_dc = 19,
        .clk_freq = 20000000,
        .swap_data = true,
    };

    scr_interface_driver_t *iface_drv;
    TEST_ASSERT(ESP_OK == scr_interface_create(SCREEN_IFACE_SPI, &spi_lcd_cfg, &iface_drv));

    scr_controller_config_t lcd_cfg = {0};
    lcd_cfg.interface_drv = iface_drv;
    lcd_cfg.pin_num_rst = 18;
    lcd_cfg.pin_num_bckl = 23;
    lcd_cfg.rst_active_level = 0;
    lcd_cfg.bckl_active_level = 1;
    lcd_cfg.offset_hor = 0;
    lcd_cfg.offset_ver = 0;
    lcd_cfg.width = 240;
    lcd_cfg.height = 320;
    lcd_cfg.rotate = SCR_DIR_LRBT;
    TEST_ASSERT(ESP_OK == scr_find_driver(SCREEN_CONTROLLER_ILI9341, &lcd));
    TEST_ASSERT(ESP_OK == lcd.init(&lcd_cfg));

    lcd_colorful_test_all(&lcd);

    lcd.deinit();
    scr_interface_delete(iface_drv);
    spi_bus_delete(&spi_bus);
}

TEST_CASE("Screen ST7789 SPI test", "[screen][iot]")
{
    scr_driver_t lcd;
    spi_config_t spi_cfg = {
        .miso_io_num = -1,
        .mosi_io_num = 21,
        .sclk_io_num = 22,
        .max_transfer_sz = 320 * 480,
    };
    spi_bus_handle_t spi_bus = spi_bus_create(2, &spi_cfg);
    TEST_ASSERT_NOT_NULL(spi_bus);

    scr_interface_spi_config_t spi_lcd_cfg = {
        .spi_bus = spi_bus,
        .pin_num_cs = 5,
        .pin_num_dc = 19,
        .clk_freq = 20000000,
        .swap_data = true,
    };

    scr_interface_driver_t *iface_drv;
    TEST_ASSERT(ESP_OK == scr_interface_create(SCREEN_IFACE_SPI, &spi_lcd_cfg, &iface_drv));

    scr_controller_config_t lcd_cfg = {0};
    lcd_cfg.interface_drv = iface_drv;
    lcd_cfg.pin_num_rst = 18;
    lcd_cfg.pin_num_bckl = 23;
    lcd_cfg.rst_active_level = 0;
    lcd_cfg.bckl_active_level = 1;
    lcd_cfg.offset_hor = 0;
    lcd_cfg.offset_ver = 0;
    lcd_cfg.width = 240;
    lcd_cfg.height = 320;
    lcd_cfg.rotate = SCR_DIR_LRBT;
    TEST_ASSERT(ESP_OK == scr_find_driver(SCREEN_CONTROLLER_ST7789, &lcd));
    TEST_ASSERT(ESP_OK == lcd.init(&lcd_cfg));

    lcd_colorful_test_all(&lcd);

    lcd.deinit();
    scr_interface_delete(iface_drv);
    spi_bus_delete(&spi_bus);
}

TEST_CASE("Screen SSD1351 SPI test", "[screen][iot]")
{
    scr_driver_t lcd;
    spi_config_t spi_cfg = {
        .miso_io_num = -1,
        .mosi_io_num = 38,
        .sclk_io_num = 39,
        .max_transfer_sz = 320 * 480,
    };
    spi_bus_handle_t spi_bus = spi_bus_create(2, &spi_cfg);
    TEST_ASSERT_NOT_NULL(spi_bus);

    scr_interface_spi_config_t spi_lcd_cfg = {
        .spi_bus = spi_bus,
        .pin_num_cs = 42,
        .pin_num_dc = 40,
        .clk_freq = 10000000,
        .swap_data = true,
    };
    scr_interface_driver_t *iface_drv;
    TEST_ASSERT(ESP_OK == scr_interface_create(SCREEN_IFACE_SPI, &spi_lcd_cfg, &iface_drv));

    scr_controller_config_t lcd_cfg = {0};
    lcd_cfg.interface_drv = iface_drv;
    lcd_cfg.pin_num_rst = 0;
    lcd_cfg.pin_num_bckl = -1;
    lcd_cfg.rst_active_level = 0;
    lcd_cfg.bckl_active_level = 1;
    lcd_cfg.offset_hor = 0;
    lcd_cfg.offset_ver = 0;
    lcd_cfg.width = 128;
    lcd_cfg.height = 128;
    lcd_cfg.rotate = SCR_DIR_LRBT;
    TEST_ASSERT(ESP_OK == scr_find_driver(SCREEN_CONTROLLER_SSD1351, &lcd));
    TEST_ASSERT(ESP_OK == lcd.init(&lcd_cfg));

    lcd_colorful_test_all(&lcd);

    lcd.deinit();
    scr_interface_delete(iface_drv);
    spi_bus_delete(&spi_bus);
}

static const unsigned char gImage_img_120x39[9360] =  { /* 0X00,0X10,0X78,0X00,0X27,0X00,0X01,0X1B, */
    0X9D, 0XFF, 0X7D, 0XFF, 0X1D, 0XFF, 0XFD, 0XFE, 0X1E, 0XFF, 0X1E, 0XFF, 0X1D, 0XFF, 0X1C, 0XFF,
    0X1C, 0XFF, 0X1D, 0XFF, 0XFC, 0XFE, 0X1D, 0XFF, 0X1D, 0XFF, 0XFC, 0XFE, 0X1D, 0XFF, 0X1D, 0XFF,
    0X1C, 0XFF, 0X3C, 0XFF, 0X1D, 0XFF, 0X3D, 0XFF, 0X1D, 0XFF, 0X3D, 0XFF, 0X1D, 0XFF, 0X1D, 0XFF,
    0X1D, 0XFF, 0XFD, 0XFE, 0XFD, 0XFE, 0X1D, 0XFF, 0X1D, 0XFF, 0X3D, 0XFF, 0X3D, 0XFF, 0X1C, 0XFF,
    0X1C, 0XFF, 0X1C, 0XFF, 0X3D, 0XFF, 0X1D, 0XFF, 0X1D, 0XFF, 0X3E, 0XFF, 0X9F, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0X9D, 0XFF, 0X08, 0X8A, 0X46, 0X91, 0X46, 0XA1, 0X27, 0X99, 0X46, 0X99, 0X46, 0X99, 0X66, 0X99,
    0X25, 0X99, 0X46, 0X99, 0X66, 0X99, 0X46, 0X99, 0X25, 0X99, 0X46, 0XA1, 0X66, 0XA1, 0X46, 0X99,
    0X25, 0X99, 0X46, 0X99, 0X66, 0X99, 0X46, 0X99, 0X46, 0X99, 0X66, 0X99, 0X46, 0X99, 0X46, 0X99,
    0X26, 0X99, 0X26, 0XA1, 0X26, 0XA1, 0X26, 0X99, 0X67, 0XA1, 0X66, 0X99, 0X46, 0X99, 0X66, 0XA1,
    0X45, 0X99, 0X46, 0XA1, 0X46, 0X99, 0X26, 0X99, 0X67, 0XA1, 0X05, 0X81, 0X97, 0XE5, 0XDF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0X5D, 0XFF, 0X86, 0X89, 0XC4, 0XA8, 0X83, 0XB8, 0XC5, 0XB8, 0XA4, 0XB0, 0XA3, 0XB0, 0XC3, 0XB0,
    0XA5, 0XB8, 0X84, 0XB0, 0XA3, 0XB8, 0XC3, 0XB8, 0X83, 0XB8, 0X84, 0XB8, 0X84, 0XB8, 0X84, 0XB8,
    0XA4, 0XB8, 0XA4, 0XB8, 0X63, 0XB0, 0X84, 0XB8, 0XA4, 0XB8, 0X83, 0XB0, 0XA3, 0XB8, 0XA3, 0XB8,
    0XA3, 0XB0, 0XA4, 0XB0, 0X84, 0XB8, 0XA4, 0XB8, 0XA3, 0XB0, 0X82, 0XB0, 0XA3, 0XB8, 0X83, 0XB8,
    0XA4, 0XB8, 0XA3, 0XB8, 0X83, 0XB0, 0XA4, 0XB8, 0XA5, 0XB8, 0X63, 0X90, 0X56, 0XF5, 0XBF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0X3D, 0XFF, 0XC7, 0XA1, 0X83, 0XB0, 0X63, 0XC8, 0X84, 0XC8, 0X84, 0XC8, 0XC4, 0XC8, 0X83, 0XC0,
    0X84, 0XD0, 0X64, 0XC8, 0X83, 0XC8, 0X62, 0XC0, 0X84, 0XD0, 0X64, 0XC8, 0X84, 0XD0, 0X63, 0XC8,
    0X84, 0XC8, 0X63, 0XC8, 0X84, 0XC8, 0X64, 0XC0, 0X83, 0XB8, 0XC4, 0XB8, 0X82, 0XB8, 0XA3, 0XC0,
    0XC3, 0XC0, 0X83, 0XC0, 0X84, 0XC8, 0X84, 0XC8, 0X83, 0XC8, 0XA3, 0XC8, 0X83, 0XC8, 0X83, 0XC8,
    0X63, 0XC8, 0X83, 0XD0, 0XA3, 0XC8, 0XA3, 0XC8, 0X42, 0XC0, 0X83, 0XA8, 0X36, 0XF5, 0XBF, 0XFF,
    0XDF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0X1D, 0XFF, 0X46, 0X99, 0XC4, 0XB0, 0XA4, 0XD0, 0X63, 0XC8, 0X63, 0XC8, 0X83, 0XC0, 0XA4, 0XC8,
    0X63, 0XC8, 0X84, 0XD0, 0X83, 0XC8, 0XA4, 0XD0, 0X64, 0XC8, 0X64, 0XC8, 0X84, 0XC8, 0X83, 0XC8,
    0X63, 0XC8, 0XA4, 0XC8, 0X84, 0XB8, 0X27, 0XB9, 0X87, 0XA9, 0X87, 0XA1, 0X25, 0XA1, 0XA3, 0X98,
    0X62, 0XA0, 0XA4, 0XB8, 0X63, 0XC0, 0X63, 0XC8, 0X63, 0XC8, 0X83, 0XC8, 0X83, 0XC8, 0X83, 0XC8,
    0X83, 0XC8, 0X83, 0XC0, 0X62, 0XC0, 0XA3, 0XC8, 0X83, 0XC8, 0X63, 0XA0, 0X76, 0XF5, 0X9E, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0X3D, 0XFF, 0X87, 0XA1, 0XA4, 0XB0, 0X42, 0XC0, 0X83, 0XD0, 0X83, 0XD0, 0X62, 0XC0, 0X83, 0XC8,
    0X83, 0XC8, 0XA4, 0XC8, 0X83, 0XC8, 0X83, 0XC0, 0XA5, 0XB8, 0X84, 0XB0, 0X63, 0XA8, 0X83, 0XA8,
    0XC4, 0XB8, 0X84, 0XB0, 0XA5, 0XA8, 0XBC, 0XFE, 0XFC, 0XFE, 0XFC, 0XFE, 0XFC, 0XFE, 0XF8, 0XFD,
    0X52, 0XF4, 0X09, 0XBA, 0X83, 0XA0, 0X83, 0XB0, 0X63, 0XC0, 0X63, 0XC8, 0XA4, 0XD0, 0X63, 0XC8,
    0X83, 0XC8, 0XA3, 0XC8, 0X82, 0XC0, 0XA3, 0XC8, 0X63, 0XC8, 0X42, 0XA0, 0X76, 0XF5, 0XDF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0X3D, 0XFF, 0X87, 0XA1, 0X83, 0XB8, 0X63, 0XC8, 0X83, 0XD0, 0X42, 0XC8, 0X84, 0XD0, 0X63, 0XC8,
    0X42, 0XB0, 0X83, 0XB8, 0XA4, 0XC0, 0XA4, 0XB0, 0X84, 0X90, 0X6F, 0XDB, 0X10, 0XE4, 0XCB, 0XBA,
    0X86, 0XA1, 0XA3, 0X98, 0X63, 0XA0, 0XA8, 0XC1, 0X52, 0XFC, 0X1D, 0XFF, 0X7E, 0XFF, 0X9E, 0XFF,
    0XBE, 0XFF, 0X7E, 0XFF, 0X18, 0XFE, 0X6B, 0XB2, 0XA4, 0XA0, 0X64, 0XB8, 0X64, 0XC0, 0X64, 0XC8,
    0X84, 0XC8, 0X83, 0XC8, 0X83, 0XC8, 0X83, 0XC8, 0X42, 0XC8, 0XA4, 0XB0, 0X56, 0XF5, 0XBF, 0XFF,
    0XDF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0X1D, 0XFF, 0X87, 0XA1, 0XA3, 0XB8, 0X63, 0XD0, 0X63, 0XC8, 0X83, 0XC8, 0X84, 0XB8, 0XA4, 0XB0,
    0X15, 0XFD, 0X46, 0XC1, 0X83, 0XA8, 0X66, 0XB1, 0X39, 0XFE, 0X5E, 0XFF, 0X9D, 0XFF, 0X7D, 0XFF,
    0X5D, 0XFF, 0X39, 0XFE, 0X10, 0XEC, 0X46, 0X99, 0XC4, 0X88, 0X66, 0X81, 0X93, 0XD4, 0X9E, 0XFF,
    0XBE, 0XFF, 0XDF, 0XFF, 0XBF, 0XFF, 0X7E, 0XFF, 0XD4, 0XFC, 0XA4, 0X98, 0XA4, 0XB8, 0X84, 0XC8,
    0X84, 0XC8, 0X83, 0XC8, 0X83, 0XC8, 0X83, 0XC8, 0X63, 0XD0, 0X43, 0XA0, 0X56, 0XF5, 0XBF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0X1E, 0XFF, 0X47, 0XA1, 0X84, 0XB8, 0X84, 0XD0, 0X43, 0XC8, 0XA4, 0XB8, 0XA4, 0X90, 0XB8, 0XFD,
    0XDC, 0XFE, 0X87, 0XB1, 0X05, 0X99, 0X1C, 0XFF, 0X7D, 0XFF, 0XDF, 0XFF, 0XBF, 0XFF, 0XBF, 0XFF,
    0XDF, 0XFF, 0XBF, 0XFF, 0XBE, 0XFF, 0X9E, 0XFF, 0XD7, 0XF5, 0X09, 0X92, 0XC4, 0X80, 0X87, 0X99,
    0XF7, 0XFD, 0X7D, 0XFF, 0XDF, 0XFF, 0XDF, 0XFF, 0XBF, 0XFF, 0X9B, 0XFE, 0XC4, 0X98, 0X83, 0XB8,
    0X83, 0XC0, 0X63, 0XC8, 0X83, 0XC8, 0X83, 0XC8, 0X63, 0XC8, 0X42, 0XA0, 0X76, 0XF5, 0XDF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XDF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XDF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0X1E, 0XFF, 0X67, 0XA1, 0X84, 0XB8, 0X83, 0XC8, 0XA4, 0XC8, 0X42, 0XA0, 0X52, 0XEC, 0X1C, 0XFF,
    0X09, 0XBA, 0XA3, 0X98, 0X66, 0XA1, 0X1C, 0XFF, 0X9D, 0XFF, 0XDE, 0XFF, 0X9F, 0XFF, 0XDF, 0XFF,
    0XFF, 0XFF, 0XDF, 0XFF, 0XFF, 0XFF, 0XDE, 0XFF, 0X9E, 0XFF, 0X7E, 0XFF, 0X15, 0XFD, 0X25, 0X91,
    0X03, 0X81, 0XCE, 0XCB, 0X9D, 0XFF, 0XBE, 0XFF, 0XDF, 0XFF, 0X9F, 0XFF, 0XBB, 0XFE, 0XE4, 0X98,
    0XA4, 0XB8, 0XA4, 0XC8, 0X63, 0XC8, 0X62, 0XC8, 0XA3, 0XD0, 0X62, 0XA0, 0X55, 0XED, 0XDF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XDF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XBE, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XDF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XDF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XDF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XDF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0X1D, 0XFF, 0X87, 0XA1, 0XA4, 0XB0, 0XA4, 0XC8, 0XA4, 0XB8, 0X45, 0XB1, 0X3C, 0XFF, 0X51, 0XE4,
    0X62, 0XA0, 0X83, 0XB8, 0XE5, 0XB0, 0X9B, 0XFE, 0XDB, 0XFE, 0X5D, 0XFF, 0X5E, 0XFF, 0X7F, 0XFF,
    0XDF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XBF, 0XFF, 0X9E, 0XFF, 0X5D, 0XFF,
    0XAA, 0XAA, 0XE3, 0X80, 0XAA, 0XBA, 0X5D, 0XFF, 0XDE, 0XFF, 0XDF, 0XFF, 0XBE, 0XFF, 0XF8, 0XFD,
    0X63, 0X98, 0XA4, 0XC0, 0X83, 0XC8, 0X43, 0XC8, 0X63, 0XD0, 0X83, 0XA8, 0X55, 0XF5, 0XDE, 0XFF,
    0XFE, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XDF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XDF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XDF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XDF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XDF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XDF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XDF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XDF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0X3C, 0XFF, 0X86, 0XA1, 0XA4, 0XB8, 0X83, 0XB8, 0X82, 0X98, 0X96, 0XFD, 0XDB, 0XFE, 0XC3, 0X88,
    0XC4, 0XB0, 0XA4, 0XB8, 0X84, 0X98, 0XA4, 0X88, 0XE3, 0X78, 0X04, 0X71, 0XE8, 0X89, 0X2E, 0XAB,
    0X55, 0XE5, 0X9E, 0XFF, 0X9E, 0XFF, 0XBF, 0XFF, 0XDF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XBE, 0XFF,
    0X5D, 0XFF, 0XEF, 0XDB, 0XC3, 0X88, 0X49, 0XAA, 0X5D, 0XFF, 0X9E, 0XFF, 0XDF, 0XFF, 0X9E, 0XFF,
    0X8F, 0XE3, 0X63, 0XA8, 0X84, 0XC8, 0X63, 0XD0, 0X83, 0XD0, 0X62, 0XA0, 0X75, 0XF5, 0XDE, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XDF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XDF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XDF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XDF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0X3C, 0XFF, 0X66, 0X99, 0XA4, 0XC0, 0XA4, 0XB8, 0XC8, 0XA1, 0X1D, 0XFF, 0X8B, 0XDA, 0XA4, 0XA8,
    0X83, 0X98, 0XA8, 0XA9, 0XD0, 0XDB, 0XB3, 0XE4, 0X72, 0XD4, 0XCF, 0XBB, 0X6B, 0X9A, 0X26, 0X81,
    0XC4, 0X88, 0XE4, 0X90, 0X4D, 0XDB, 0XBB, 0XFE, 0X9E, 0XFF, 0XBE, 0XFF, 0XFF, 0XFF, 0XDF, 0XFF,
    0XBF, 0XFF, 0X7E, 0XFF, 0X93, 0XEC, 0XC4, 0X80, 0X4A, 0XAA, 0X5D, 0XFF, 0XBE, 0XFF, 0XBE, 0XFF,
    0X1D, 0XFF, 0X26, 0XA1, 0X84, 0XB8, 0X64, 0XC8, 0X84, 0XD0, 0X62, 0XA0, 0X76, 0XF5, 0XDF, 0XFF,
    0XFF, 0XFF, 0XDE, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XDF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0X3D, 0XFF, 0X66, 0XA1, 0XA4, 0XC0, 0X22, 0XA8, 0X73, 0XEC, 0X3D, 0XFF, 0XA4, 0X98, 0X83, 0XA0,
    0XD4, 0XFC, 0X3D, 0XFF, 0X7E, 0XFF, 0X9E, 0XFF, 0X9E, 0XFF, 0X9E, 0XFF, 0X5E, 0XFF, 0X1D, 0XFF,
    0X56, 0XFD, 0X6A, 0XC2, 0XA3, 0X88, 0X04, 0X81, 0X31, 0XCC, 0X9E, 0XFF, 0XBF, 0XFF, 0XDF, 0XFF,
    0XDF, 0XFF, 0XBF, 0XFF, 0X7F, 0XFF, 0X52, 0XDC, 0XC4, 0X88, 0X0C, 0XC3, 0X7E, 0XFF, 0XBE, 0XFF,
    0X9E, 0XFF, 0XD0, 0XDB, 0X63, 0XA8, 0X64, 0XC8, 0X63, 0XC8, 0X63, 0XA0, 0X76, 0XF5, 0XBF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XDE, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XDF, 0XF7, 0XFF, 0XFF,
    0XDF, 0XF7, 0XFF, 0XFF, 0XFF, 0XFF, 0XDF, 0XF7, 0XFF, 0XFF, 0XFF, 0XFF, 0XDF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XDF, 0XF7, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XF7, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XDF, 0XF7, 0XFF, 0XFF, 0XFF, 0XFF, 0XDF, 0XFF,
    0XDF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XDF, 0XF7, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0X3D, 0XFF, 0XA7, 0XA1, 0X83, 0XB0, 0X63, 0XA8, 0X7B, 0XFE, 0X35, 0XED, 0XC3, 0X80, 0X55, 0XFD,
    0X7D, 0XFF, 0XDF, 0XFF, 0XDF, 0XFF, 0XDF, 0XFF, 0XBE, 0XFF, 0XDE, 0XFF, 0XDF, 0XFF, 0XDF, 0XFF,
    0X9F, 0XFF, 0X9E, 0XFF, 0X9A, 0XFE, 0X8A, 0X9A, 0XC4, 0X88, 0X09, 0XAA, 0XDB, 0XFE, 0XBF, 0XFF,
    0XDF, 0XFF, 0XFF, 0XFF, 0XBE, 0XFF, 0X9D, 0XFF, 0X6E, 0XCB, 0XA3, 0X78, 0X35, 0XED, 0X9E, 0XFF,
    0XBE, 0XFF, 0XBA, 0XFE, 0X83, 0XA0, 0X84, 0XC8, 0X63, 0XD0, 0X63, 0XA8, 0X56, 0XF5, 0XBF, 0XFF,
    0XDF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XDF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XBB, 0XCE, 0XD4, 0X94,
    0XD4, 0X94, 0XFC, 0XD6, 0XDF, 0XF7, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0X39, 0XC6, 0XF4, 0X94,
    0XD3, 0X94, 0XB7, 0XB5, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XD7, 0XB5, 0X52, 0X84,
    0X72, 0X8C, 0X93, 0X8C, 0X72, 0X8C, 0XB7, 0XAD, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XBB, 0XCE,
    0X93, 0X8C, 0X72, 0X8C, 0X72, 0X8C, 0X93, 0X8C, 0XD8, 0XB5, 0XFF, 0XFF, 0XDF, 0XF7, 0XFF, 0XFF,
    0XFF, 0XFF, 0XDF, 0XF7, 0XFF, 0XFF, 0X3D, 0XE7, 0X14, 0X9D, 0X92, 0X8C, 0X59, 0XC6, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XBB, 0XD6, 0XD3, 0X94, 0X93, 0X8C, 0X15, 0X9D, 0X9E, 0XEF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0X9A, 0XCE, 0XD3, 0X94, 0XD3, 0X94, 0X55, 0XA5,
    0X7E, 0XEF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XDC, 0XDE, 0XDB, 0XD6, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0X18, 0XBE, 0X31, 0X84, 0X92, 0X8C, 0X7A, 0XCE, 0XFF, 0XFF, 0XFF, 0XFF,
    0X3D, 0XFF, 0X86, 0X99, 0X83, 0XB0, 0X46, 0XC1, 0X1D, 0XFF, 0X0D, 0XAB, 0XCB, 0XAA, 0X7D, 0XFF,
    0XDF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XDF, 0XFF, 0XBE, 0XFF, 0XBE, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XDF, 0XFF, 0X9E, 0XFF, 0XF4, 0XFC, 0XC3, 0X80, 0X65, 0X89, 0X79, 0XFE,
    0XBF, 0XFF, 0XBF, 0XFF, 0XFF, 0XFF, 0XDE, 0XFF, 0X5D, 0XFF, 0X09, 0X9A, 0XE4, 0X78, 0XDB, 0XFE,
    0XBE, 0XFF, 0X7D, 0XFF, 0XA7, 0XB9, 0X83, 0XC0, 0X63, 0XC8, 0X63, 0XA8, 0X56, 0XF5, 0XBF, 0XFF,
    0XDF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XF0, 0X73, 0X87, 0X21, 0XEC, 0X52,
    0XCC, 0X52, 0X66, 0X29, 0X5D, 0XE7, 0XFF, 0XFF, 0XFF, 0XFF, 0X6E, 0X6B, 0X86, 0X29, 0XEC, 0X5A,
    0X0C, 0X5B, 0X08, 0X3A, 0XA7, 0X29, 0X9F, 0XEF, 0XFF, 0XFF, 0XFF, 0XFF, 0X29, 0X3A, 0XAB, 0X52,
    0X8F, 0X6B, 0X6E, 0X63, 0X6E, 0X63, 0XC8, 0X31, 0X4E, 0X63, 0XFF, 0XFF, 0XFF, 0XF7, 0XAF, 0X6B,
    0XA8, 0X31, 0XB0, 0X73, 0X6E, 0X63, 0X4E, 0X63, 0XC8, 0X31, 0XAF, 0X6B, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0X55, 0XA5, 0X45, 0X21, 0XAB, 0X52, 0XEC, 0X5A, 0X66, 0X29, 0XF8, 0XBD,
    0XFF, 0XFF, 0XFF, 0XFF, 0X51, 0X84, 0X66, 0X29, 0XCB, 0X52, 0X0D, 0X5B, 0X8A, 0X4A, 0X45, 0X21,
    0X59, 0XC6, 0XFF, 0XFF, 0XFF, 0XFF, 0XF0, 0X7B, 0X45, 0X21, 0XCB, 0X52, 0X2D, 0X5B, 0X4A, 0X42,
    0X45, 0X21, 0X7A, 0XCE, 0XFF, 0XFF, 0XFF, 0XFF, 0X31, 0X84, 0X6E, 0X6B, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0X2D, 0X63, 0XA7, 0X29, 0XCF, 0X73, 0X2D, 0X5B, 0X66, 0X29, 0XF0, 0X7B, 0XFF, 0XFF,
    0X5C, 0XFF, 0X66, 0X91, 0XA4, 0XA8, 0XC9, 0XD1, 0XDD, 0XFE, 0X88, 0X81, 0X76, 0XE5, 0XBE, 0XFF,
    0XDF, 0XFF, 0XDF, 0XFF, 0XBF, 0XFF, 0X7E, 0XFF, 0X5D, 0XFF, 0X5D, 0XFF, 0X9E, 0XFF, 0XDF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XDF, 0XFF, 0XDF, 0XFF, 0XBE, 0XFF, 0X38, 0XFE, 0XC3, 0X88, 0X04, 0X91,
    0XDC, 0XFE, 0X9F, 0XFF, 0XFE, 0XFF, 0XFE, 0XFF, 0XDF, 0XFF, 0XBB, 0XFE, 0XA3, 0X88, 0XAA, 0XC2,
    0X9D, 0XFF, 0X9D, 0XFF, 0XAB, 0XD2, 0X43, 0XB0, 0X64, 0XC8, 0X84, 0XA8, 0X36, 0XF5, 0XBF, 0XFF,
    0XDF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0X93, 0X84, 0XC8, 0X31, 0X1C, 0XDF, 0XFF, 0XFF,
    0XFF, 0XFF, 0X5D, 0XE7, 0XFF, 0XFF, 0XFF, 0XFF, 0XF4, 0X9C, 0X08, 0X3A, 0X9E, 0XEF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0X39, 0XBE, 0XBF, 0XF7, 0XFF, 0XFF, 0XFF, 0XFF, 0X49, 0X42, 0X7A, 0XC6,
    0XFF, 0XFF, 0XDF, 0XF7, 0XFF, 0XFF, 0XBF, 0XEF, 0X25, 0X19, 0XBB, 0XCE, 0XFF, 0XFF, 0X6F, 0X63,
    0X31, 0X7C, 0XFF, 0XFF, 0XFF, 0XF7, 0XFF, 0XFF, 0X5E, 0XE7, 0X26, 0X21, 0XBC, 0XD6, 0XFF, 0XFF,
    0XFF, 0XF7, 0X7A, 0XC6, 0XE4, 0X10, 0X18, 0XBE, 0XFF, 0XFF, 0XFF, 0XFF, 0X9E, 0XF7, 0XBE, 0XF7,
    0XFF, 0XFF, 0X79, 0XCE, 0X45, 0X29, 0XBB, 0XD6, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XBB, 0XD6,
    0X5D, 0XEF, 0XFF, 0XFF, 0XF9, 0XBD, 0X66, 0X29, 0XFB, 0XDE, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0X9B, 0XCE, 0X3D, 0XE7, 0XFF, 0XFF, 0XFF, 0XFF, 0X31, 0X84, 0X8E, 0X6B, 0XFF, 0XFF, 0XFF, 0XFF,
    0X39, 0XBE, 0X87, 0X29, 0XBF, 0XEF, 0XFF, 0XFF, 0XFF, 0XFF, 0X1C, 0XDF, 0XD3, 0X94, 0XFF, 0XFF,
    0X5D, 0XFF, 0XA7, 0X99, 0X83, 0XA0, 0XCD, 0XF2, 0XBC, 0XFE, 0X05, 0X71, 0X7A, 0XFE, 0XBE, 0XFF,
    0XFF, 0XFF, 0XDF, 0XFF, 0XDB, 0XFE, 0X45, 0X61, 0X05, 0X71, 0XE8, 0X91, 0XAF, 0XBB, 0X79, 0XFE,
    0X5D, 0XFF, 0XBE, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XBE, 0XFF, 0X18, 0XFE, 0XC3, 0X88,
    0X87, 0X89, 0X7E, 0XFF, 0XDE, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0X9E, 0XFF, 0X6E, 0XDB, 0XC3, 0X88,
    0X18, 0XFE, 0X5D, 0XFF, 0XD0, 0XF3, 0X83, 0XB0, 0X84, 0XC8, 0X63, 0XA8, 0X35, 0XED, 0XBF, 0XFF,
    0XDF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0X9F, 0XEF, 0XA8, 0X29, 0X7A, 0XC6, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0X6E, 0X6B, 0XAF, 0X6B, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XDF, 0XF7, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0X29, 0X42, 0X7A, 0XC6,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0X87, 0X29, 0X19, 0XBE, 0XFF, 0XFF, 0X8F, 0X6B,
    0X31, 0X7C, 0XFF, 0XF7, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0X87, 0X29, 0X59, 0XC6, 0XFF, 0XFF,
    0XFF, 0XFF, 0X6A, 0X42, 0X15, 0X9D, 0XFF, 0XF7, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0X55, 0XA5, 0X29, 0X42, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XF4, 0X9C, 0X4A, 0X42, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XDF, 0XFF, 0X52, 0X84, 0X8F, 0X6B, 0XFF, 0XFF, 0XFF, 0XFF,
    0XB7, 0XAD, 0XAB, 0X4A, 0XFF, 0XFF, 0XDF, 0XF7, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0X3D, 0XFF, 0X87, 0X91, 0X83, 0XA0, 0X0D, 0XF3, 0XDC, 0XFE, 0X04, 0X71, 0XBA, 0XFE, 0XDE, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XBD, 0XFF, 0X30, 0XB4, 0X0D, 0XAB, 0X87, 0X91, 0X04, 0X89, 0XC3, 0X88,
    0X6A, 0XBA, 0X9A, 0XFE, 0X9E, 0XFF, 0XDF, 0XFF, 0XDF, 0XFF, 0XBF, 0XFF, 0X9E, 0XFF, 0X76, 0XFD,
    0XA3, 0X88, 0X2D, 0XCB, 0XBE, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XBE, 0XFF, 0XDB, 0XFE, 0X04, 0X81,
    0XEC, 0XAA, 0X3D, 0XFF, 0X31, 0XFC, 0X62, 0XA0, 0XA3, 0XC0, 0X63, 0XA0, 0X76, 0XF5, 0XBF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0X97, 0XAD, 0XE9, 0X29, 0X7F, 0XDF, 0X7E, 0XE7, 0X7E, 0XE7,
    0X5E, 0XE7, 0X7E, 0XEF, 0XFF, 0XFF, 0XFF, 0XFF, 0XDC, 0XD6, 0X67, 0X21, 0X0D, 0X5B, 0XF9, 0XBD,
    0X3D, 0XE7, 0XFF, 0XFF, 0XFF, 0XF7, 0XFF, 0XF7, 0XDF, 0XFF, 0XFF, 0XFF, 0X2A, 0X42, 0X36, 0X9D,
    0XDB, 0XD6, 0XBB, 0XD6, 0XBB, 0XCE, 0X6E, 0X63, 0XA7, 0X29, 0XFF, 0XF7, 0XFF, 0XF7, 0X8F, 0X6B,
    0X6E, 0X63, 0X1D, 0XDF, 0XBB, 0XD6, 0X9B, 0XCE, 0X4E, 0X63, 0X29, 0X42, 0XBF, 0XF7, 0XFF, 0XFF,
    0X9B, 0XCE, 0X25, 0X19, 0XFD, 0XD6, 0X9F, 0XE7, 0X5E, 0XE7, 0X7E, 0XE7, 0X3D, 0XDF, 0XFF, 0XF7,
    0XFF, 0XFF, 0XDF, 0XF7, 0XE8, 0X31, 0X49, 0X3A, 0X97, 0XAD, 0X3D, 0XDF, 0XDF, 0XEF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XDF, 0XF7, 0XBF, 0XEF, 0XE8, 0X31, 0X6A, 0X4A, 0X96, 0XAD, 0X1D, 0XDF, 0XBF, 0XF7,
    0XFF, 0XFF, 0XFF, 0XF7, 0XFF, 0XFF, 0XFF, 0XFF, 0X31, 0X7C, 0X8F, 0X6B, 0XDF, 0XF7, 0XFF, 0XFF,
    0X93, 0X8C, 0X8B, 0X42, 0XDC, 0XD6, 0X9B, 0XCE, 0XBB, 0XD6, 0X7A, 0XCE, 0X1C, 0XE7, 0XFF, 0XFF,
    0X3D, 0XFF, 0X87, 0X91, 0X82, 0XA0, 0X2D, 0XFB, 0XFB, 0XFE, 0X24, 0X71, 0XB2, 0XD4, 0X9D, 0XFF,
    0XDF, 0XFF, 0XFF, 0XFF, 0XFE, 0XFF, 0XBE, 0XFF, 0X5E, 0XFF, 0X3E, 0XFF, 0X7A, 0XFE, 0X6D, 0XD3,
    0XC3, 0X88, 0XE4, 0X80, 0X35, 0XED, 0X9F, 0XFF, 0XBF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0X7E, 0XFF,
    0X4E, 0XD3, 0XC3, 0X88, 0X18, 0XFE, 0XDE, 0XFF, 0XFE, 0XFF, 0XDE, 0XFF, 0X7E, 0XFF, 0X6F, 0XBB,
    0XE5, 0X80, 0X9B, 0XFE, 0XF0, 0XFB, 0X82, 0XB0, 0XA3, 0XC0, 0X63, 0XA0, 0X56, 0XF5, 0X9F, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0X14, 0X9D, 0X05, 0X19, 0XA8, 0X29, 0X87, 0X21, 0X86, 0X21, 0X66, 0X21,
    0X46, 0X21, 0X46, 0X21, 0X3C, 0XE7, 0XFF, 0XFF, 0XFF, 0XFF, 0X5E, 0XDF, 0XF0, 0X7B, 0XE8, 0X39,
    0X46, 0X29, 0X67, 0X29, 0XB0, 0X6B, 0XBF, 0XEF, 0XFF, 0XFF, 0XFF, 0XFF, 0X6B, 0X42, 0X87, 0X29,
    0XC8, 0X31, 0XA7, 0X31, 0XA6, 0X31, 0X2D, 0X63, 0X7E, 0XEF, 0XFF, 0XFF, 0XFF, 0XFF, 0XF0, 0X73,
    0X66, 0X21, 0XA7, 0X31, 0X87, 0X29, 0X46, 0X21, 0X05, 0X19, 0XDB, 0XD6, 0XFF, 0XFF, 0X7B, 0XCE,
    0X25, 0X19, 0XC8, 0X29, 0X66, 0X21, 0X87, 0X21, 0X46, 0X21, 0X66, 0X21, 0X46, 0X21, 0X97, 0XAD,
    0XFF, 0XFF, 0XFF, 0XF7, 0XFF, 0XF7, 0XD4, 0X94, 0X4A, 0X42, 0X46, 0X21, 0X67, 0X21, 0XEC, 0X5A,
    0XFB, 0XD6, 0XFF, 0XFF, 0XFF, 0XF7, 0XBF, 0XF7, 0X92, 0X8C, 0X29, 0X42, 0X46, 0X21, 0X67, 0X29,
    0X0D, 0X5B, 0X3C, 0XDF, 0XFF, 0XFF, 0XFF, 0XFF, 0X31, 0X7C, 0XAF, 0X6B, 0XFF, 0XFF, 0X97, 0XAD,
    0X66, 0X21, 0X87, 0X29, 0XC8, 0X31, 0XA7, 0X31, 0XC7, 0X31, 0X66, 0X29, 0XAF, 0X73, 0XFF, 0XFF,
    0X3D, 0XFF, 0X87, 0X99, 0X83, 0XA8, 0X8B, 0XEA, 0XFB, 0XFE, 0X86, 0X89, 0X65, 0X91, 0X3D, 0XFF,
    0XDF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XDF, 0XFF, 0XBF, 0XFF, 0XBF, 0XFF, 0X9E, 0XFF,
    0X9A, 0XFE, 0XE7, 0X91, 0XA3, 0X80, 0XD4, 0XFC, 0X9E, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XDF, 0XFF,
    0X1C, 0XFF, 0X46, 0X81, 0X49, 0X9A, 0X7D, 0XFF, 0XBE, 0XFF, 0XFF, 0XFF, 0XBF, 0XFF, 0X5B, 0XFE,
    0X84, 0X88, 0X88, 0XD1, 0XE4, 0XC0, 0X83, 0XC0, 0X63, 0XC8, 0X63, 0XA8, 0X56, 0XFD, 0XBF, 0XFF,
    0XDF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0X76, 0XAD, 0XE9, 0X39, 0X9F, 0XE7, 0X7E, 0XE7, 0X7E, 0XE7,
    0X9E, 0XEF, 0X7D, 0XE7, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XF7, 0XFF, 0XF7, 0XFF, 0XFF, 0XFF, 0XFF,
    0XBE, 0XF7, 0X59, 0XC6, 0XAB, 0X4A, 0X2D, 0X5B, 0XFF, 0XFF, 0XFF, 0XFF, 0X49, 0X42, 0X39, 0XBE,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0X8E, 0X6B,
    0X10, 0X7C, 0XFF, 0XFF, 0XFF, 0XFF, 0X9E, 0XEF, 0X6E, 0X6B, 0X0C, 0X5B, 0XFF, 0XFF, 0XFF, 0XFF,
    0XDC, 0XD6, 0X25, 0X19, 0XFC, 0XD6, 0X9E, 0XEF, 0X7E, 0XEF, 0X5D, 0XE7, 0X7E, 0XEF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XBF, 0XF7, 0XDB, 0XD6, 0XAF, 0X73,
    0X08, 0X3A, 0X9E, 0XEF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XDF, 0XF7, 0XDC, 0XCE,
    0X6E, 0X63, 0X2A, 0X42, 0XBF, 0XF7, 0XFF, 0XFF, 0X52, 0X84, 0X8F, 0X6B, 0XFF, 0XFF, 0XFF, 0XFF,
    0X97, 0XAD, 0XAC, 0X4A, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0X1D, 0XFF, 0X87, 0XA1, 0X63, 0XA8, 0X87, 0XC9, 0X1C, 0XFF, 0X49, 0X9A, 0XA3, 0X90, 0X4A, 0XC2,
    0X5E, 0XFF, 0XBF, 0XFF, 0XBE, 0XFF, 0X9E, 0XFF, 0XDF, 0XFF, 0XDF, 0XFF, 0XFF, 0XFF, 0XDF, 0XFF,
    0XBE, 0XFF, 0X5D, 0XFF, 0XC8, 0XA9, 0XC3, 0X88, 0X59, 0XFE, 0XBE, 0XFF, 0XFF, 0XFF, 0XDF, 0XFF,
    0X9E, 0XFF, 0X31, 0XCC, 0XE4, 0X80, 0X18, 0XFE, 0XBE, 0XFF, 0XFF, 0XFF, 0XDF, 0XFF, 0X7F, 0XFF,
    0X68, 0XB1, 0X43, 0XB8, 0X84, 0XC8, 0X63, 0XC8, 0X63, 0XD0, 0X63, 0XA8, 0X35, 0XF5, 0XBF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0X7E, 0XEF, 0X67, 0X29, 0X7B, 0XC6, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XF7, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XF7, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0X5E, 0XDF, 0X26, 0X19, 0X9E, 0XEF, 0XFF, 0XFF, 0X49, 0X42, 0X59, 0XC6,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0X8E, 0X6B,
    0X11, 0X7C, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0X7E, 0XEF, 0X46, 0X21, 0XFC, 0XDE, 0XFF, 0XFF,
    0XFF, 0XFF, 0X6A, 0X42, 0X14, 0X9D, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XDF, 0XF7, 0XFF, 0XFF,
    0X87, 0X29, 0X39, 0XBE, 0XFF, 0XF7, 0XFF, 0XF7, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0X87, 0X29, 0X7A, 0XCE, 0XFF, 0XFF, 0X52, 0X84, 0X8F, 0X6B, 0XFF, 0XFF, 0XFF, 0XFF,
    0X76, 0XA5, 0XCB, 0X4A, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0X1D, 0XFF, 0X46, 0XA1, 0XA3, 0XB8, 0XE4, 0XB8, 0X3D, 0XFF, 0XAF, 0XCB, 0X84, 0XA8, 0X63, 0XB0,
    0X67, 0XB1, 0X55, 0XFD, 0XFC, 0XFE, 0X7D, 0XFF, 0X9E, 0XFF, 0XBE, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XDF, 0XFF, 0XBF, 0XFF, 0XFC, 0XFE, 0X04, 0X79, 0XC7, 0X89, 0X7D, 0XFF, 0XBF, 0XFF, 0XDF, 0XFF,
    0XDF, 0XFF, 0XFB, 0XFE, 0XC3, 0X88, 0X4D, 0XD3, 0XBE, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0X9E, 0XFF,
    0X4E, 0XDB, 0X63, 0XB0, 0XA4, 0XC8, 0X63, 0XC8, 0X83, 0XD0, 0X62, 0XA8, 0X56, 0XF5, 0XBF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0X73, 0X8C, 0XE9, 0X39, 0X1D, 0XDF, 0XFF, 0XF7,
    0XFF, 0XFF, 0X7A, 0XCE, 0X1C, 0XDF, 0XFF, 0XFF, 0XD8, 0XB5, 0X32, 0X7C, 0X7F, 0XEF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XD0, 0X73, 0XAC, 0X4A, 0XFF, 0XFF, 0XDF, 0XFF, 0X4A, 0X42, 0X19, 0XBE,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0X8F, 0X73,
    0XF0, 0X7B, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XDF, 0XF7, 0X86, 0X29, 0X9B, 0XD6, 0XFF, 0XFF,
    0XFF, 0XFF, 0X18, 0XB6, 0X25, 0X19, 0X5A, 0XC6, 0XFF, 0XFF, 0XFF, 0XF7, 0X1D, 0XDF, 0XBB, 0XCE,
    0XFF, 0XF7, 0XDC, 0XD6, 0X8F, 0X6B, 0X3D, 0XDF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XF7, 0X76, 0XA5,
    0X86, 0X29, 0X7E, 0XEF, 0XBB, 0XCE, 0XD0, 0X73, 0X3D, 0XE7, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0X15, 0X9D, 0XA7, 0X29, 0XBE, 0XF7, 0XFF, 0XFF, 0X52, 0X84, 0X8F, 0X6B, 0XFF, 0XFF, 0XFF, 0XFF,
    0X96, 0XAD, 0X8B, 0X4A, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFD, 0XFE, 0X87, 0XA9, 0X63, 0XB8, 0X63, 0XA8, 0XF8, 0XFD, 0XB8, 0XFD, 0X63, 0XA8, 0X84, 0XC0,
    0X83, 0XB8, 0X83, 0XA0, 0X25, 0X99, 0X86, 0X91, 0X6A, 0XAA, 0X55, 0XED, 0X9D, 0XFF, 0XDF, 0XFF,
    0XDF, 0XFF, 0XBF, 0XFF, 0X9E, 0XFF, 0XB2, 0XDC, 0XE3, 0X80, 0X14, 0XFD, 0X9F, 0XFF, 0XDF, 0XFF,
    0XFF, 0XFF, 0X9E, 0XFF, 0XE7, 0XB1, 0X45, 0XA1, 0X9E, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XBF, 0XFF,
    0X14, 0XFD, 0X62, 0XA0, 0X83, 0XC0, 0XA4, 0XD0, 0X62, 0XC8, 0X62, 0XA0, 0X76, 0XF5, 0XBF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XF1, 0X7B, 0X46, 0X21, 0XCC, 0X52,
    0XAB, 0X52, 0X25, 0X21, 0X39, 0XC6, 0XFF, 0XFF, 0XDC, 0XD6, 0X8B, 0X4A, 0X66, 0X29, 0X6B, 0X4A,
    0X0D, 0X5B, 0XA8, 0X31, 0XE8, 0X31, 0X1D, 0XDF, 0XFF, 0XFF, 0XFF, 0XFF, 0X09, 0X3A, 0X39, 0XC6,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0X29, 0X42,
    0X72, 0X8C, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0X86, 0X29, 0X6E, 0X6B, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0X35, 0X9D, 0X46, 0X21, 0X4A, 0X42, 0X0D, 0X5B, 0X25, 0X21, 0XF4, 0X94,
    0XFF, 0XFF, 0X9F, 0XEF, 0X2D, 0X5B, 0X66, 0X21, 0X4A, 0X42, 0X4E, 0X5B, 0X29, 0X3A, 0X86, 0X21,
    0X18, 0XBE, 0XFF, 0XFF, 0X9E, 0XEF, 0X0C, 0X5B, 0X45, 0X21, 0X29, 0X42, 0X4E, 0X63, 0XE8, 0X31,
    0X87, 0X29, 0X18, 0XBE, 0XFF, 0XFF, 0XFF, 0XFF, 0X31, 0X84, 0X6E, 0X6B, 0XFF, 0XFF, 0XFF, 0XFF,
    0X76, 0XAD, 0X6A, 0X42, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0X3D, 0XFF, 0X66, 0XA1, 0X84, 0XB8, 0X43, 0XB0, 0X6E, 0XD3, 0X1D, 0XFF, 0X05, 0XB1, 0X63, 0XB8,
    0XA3, 0XC0, 0XA3, 0XB8, 0XA3, 0XA0, 0X83, 0X98, 0X63, 0XA0, 0XA3, 0X98, 0X4D, 0XC3, 0X7D, 0XFF,
    0XDF, 0XFF, 0XDF, 0XFF, 0XFF, 0XFF, 0X9D, 0XFF, 0XC3, 0X90, 0X29, 0XBA, 0X9E, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XBE, 0XFF, 0XEF, 0XEB, 0XA3, 0X88, 0X1C, 0XFF, 0XDF, 0XFF, 0XFF, 0XFF, 0XBF, 0XFF,
    0X79, 0XFE, 0X62, 0X98, 0XE4, 0XC0, 0X63, 0XC8, 0X83, 0XC8, 0X83, 0XA0, 0X75, 0XED, 0XDF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFC, 0XD6, 0X35, 0X9D,
    0X15, 0XA5, 0XFB, 0XDE, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFC, 0XDE, 0X35, 0XA5,
    0XF4, 0X9C, 0X18, 0XBE, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XDF, 0XFF, 0XBB, 0XD6, 0X9F, 0XF7,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XDF, 0XFF, 0XF8, 0XBD,
    0XBE, 0XF7, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0X7A, 0XCE, 0XF8, 0XBD, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0X5D, 0XE7, 0X76, 0XAD, 0XF4, 0X9C, 0X9A, 0XD6, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0X3D, 0XE7, 0X96, 0XAD, 0XB3, 0X94, 0XB7, 0XB5, 0X7E, 0XEF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0X1C, 0XE7, 0X55, 0XAD, 0XD3, 0X94, 0XB7, 0XB5,
    0X9E, 0XEF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0X3D, 0XE7, 0X1C, 0XDF, 0XFF, 0XFF, 0XFF, 0XFF,
    0X7E, 0XEF, 0X9A, 0XCE, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0X1D, 0XFF, 0XA7, 0XA1, 0XA4, 0XB8, 0X83, 0XB8, 0X05, 0X99, 0XFC, 0XFE, 0X11, 0XFC, 0XA3, 0XA0,
    0XA3, 0XB0, 0XE4, 0XB0, 0X52, 0XFC, 0X36, 0XFD, 0X6B, 0XDA, 0XA3, 0XA0, 0XA3, 0X90, 0XD3, 0XF4,
    0X9E, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0X7D, 0XFF, 0XEB, 0XD2, 0XC3, 0X90, 0X5E, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0X9E, 0XFF, 0X55, 0XFD, 0XC3, 0X88, 0XD7, 0XF5, 0XDF, 0XFF, 0XFF, 0XFF, 0X9E, 0XFF,
    0X39, 0XFE, 0XA3, 0XA0, 0X83, 0XB8, 0XA4, 0XD0, 0X63, 0XC8, 0X62, 0X98, 0X96, 0XF5, 0XDF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0X3D, 0XFF, 0XA7, 0X99, 0XA3, 0XB0, 0X82, 0XC0, 0X62, 0XA8, 0X11, 0XFC, 0XFD, 0XFE, 0X66, 0X89,
    0XE4, 0X88, 0X7A, 0XFE, 0X7E, 0XFF, 0X5E, 0XFF, 0X5E, 0XFF, 0X2E, 0XC3, 0X63, 0X98, 0XE8, 0XB9,
    0XBD, 0XFF, 0XFF, 0XFF, 0XFE, 0XFF, 0XBE, 0XFF, 0X50, 0XF4, 0XC3, 0X88, 0X9A, 0XFE, 0XDF, 0XFF,
    0XFF, 0XFF, 0XBF, 0XFF, 0XD7, 0XFD, 0XE3, 0X88, 0X13, 0XD5, 0XBE, 0XFF, 0XDF, 0XFF, 0X7E, 0XFF,
    0X09, 0XC2, 0X83, 0XB8, 0X63, 0XC8, 0X63, 0XC8, 0X83, 0XD0, 0X83, 0XA0, 0X55, 0XED, 0XDF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XDF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XDF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0X3E, 0XFF, 0X87, 0X99, 0XA2, 0XA8, 0XC3, 0XC8, 0X83, 0XC0, 0X62, 0XA0, 0X1D, 0XFF, 0XF8, 0XFD,
    0X08, 0X8A, 0X7D, 0XFF, 0XBE, 0XFF, 0XDF, 0XFF, 0XBF, 0XFF, 0X19, 0XFE, 0X83, 0XA8, 0XE5, 0XA8,
    0X7D, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XBE, 0XFF, 0XB3, 0XFC, 0XC3, 0X80, 0X59, 0XF6, 0XDF, 0XFF,
    0XFF, 0XFF, 0XBF, 0XFF, 0X7A, 0XFE, 0X82, 0X80, 0X91, 0XE4, 0X5D, 0XFF, 0X5D, 0XFF, 0X11, 0XE4,
    0X43, 0XA0, 0X84, 0XC8, 0X84, 0XC8, 0X84, 0XD0, 0X63, 0XC8, 0X63, 0XA8, 0X56, 0XF5, 0XDF, 0XFF,
    0XDF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XDF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XDF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XDF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XDF, 0XFF, 0XDF, 0XFF,
    0XFF, 0XFF, 0XDF, 0XFF, 0XDF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0X1D, 0XFF, 0X67, 0X99, 0XC4, 0XB0, 0X62, 0XC0, 0XA3, 0XD0, 0X62, 0XB8, 0X29, 0XBA, 0X3D, 0XFF,
    0XD3, 0XCC, 0XBE, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0X9E, 0XFF, 0X55, 0XFD, 0X63, 0XA8, 0X26, 0XB1,
    0X5E, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0X9F, 0XFF, 0XB4, 0XFC, 0XA3, 0X80, 0X59, 0XF6, 0XFF, 0XFF,
    0XFF, 0XFF, 0XBE, 0XFF, 0X3A, 0XFE, 0XA4, 0X90, 0X66, 0XB1, 0X87, 0XC1, 0X67, 0XC1, 0X63, 0XA8,
    0XA4, 0XC0, 0X84, 0XC8, 0X83, 0XC0, 0X84, 0XC8, 0X63, 0XC8, 0X63, 0XA8, 0X56, 0XFD, 0XBF, 0XFF,
    0XDF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XDF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XDF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0X1D, 0XFF, 0X67, 0XA1, 0XA4, 0XB8, 0X83, 0XC8, 0X62, 0XC8, 0XA3, 0XC8, 0X83, 0XA0, 0XB0, 0XE3,
    0X5E, 0XFF, 0XFB, 0XFE, 0XBE, 0XFF, 0XDE, 0XFF, 0XDB, 0XFE, 0XA7, 0XA9, 0X63, 0XA0, 0X6A, 0XCA,
    0X9F, 0XFF, 0XFF, 0XFF, 0XDF, 0XFF, 0X9F, 0XFF, 0X8F, 0XE3, 0XE4, 0X90, 0XDB, 0XFE, 0XFF, 0XFF,
    0XFF, 0XFF, 0XBE, 0XFF, 0XD8, 0XFD, 0XA4, 0X98, 0XA4, 0XB0, 0X83, 0XB0, 0X84, 0XB0, 0XC9, 0XD9,
    0X42, 0XB8, 0X83, 0XC8, 0X83, 0XC8, 0X63, 0XC0, 0X84, 0XC8, 0X83, 0XA8, 0X35, 0XF5, 0XBF, 0XFF,
    0XDF, 0XFF, 0XFF, 0XFF, 0XDF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XDF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XDF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XDF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0X1D, 0XFF, 0X67, 0XA1, 0X84, 0XB8, 0X63, 0XC8, 0XA3, 0XC8, 0X82, 0XC0, 0X83, 0XC0, 0X63, 0XA0,
    0XF1, 0XEB, 0X3E, 0XFF, 0X34, 0XDD, 0X08, 0X8A, 0XC4, 0X98, 0X83, 0XA8, 0XE4, 0XA8, 0X9B, 0XFE,
    0XBF, 0XFF, 0XFF, 0XFF, 0XDF, 0XFF, 0X7D, 0XFF, 0X08, 0XBA, 0X25, 0X99, 0X9E, 0XFF, 0XDF, 0XFF,
    0XFF, 0XFF, 0X9E, 0XFF, 0XD4, 0XFC, 0X83, 0X98, 0XA3, 0XA0, 0XE4, 0X98, 0X19, 0XFE, 0XFD, 0XFE,
    0XE5, 0XB8, 0X83, 0XC8, 0X63, 0XC8, 0X84, 0XD0, 0X63, 0XC8, 0X63, 0XA0, 0X56, 0XF5, 0XBF, 0XFF,
    0XDF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XDF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XDF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0X1D, 0XFF, 0X87, 0XA1, 0X64, 0XB8, 0X84, 0XC8, 0XA3, 0XC0, 0XA3, 0XC0, 0X83, 0XC8, 0X84, 0XC0,
    0X84, 0XA0, 0X4E, 0XE3, 0XFC, 0XFE, 0XF7, 0XFD, 0X46, 0XB1, 0X83, 0XA0, 0XE4, 0X98, 0X72, 0XEC,
    0X7E, 0XFF, 0X9E, 0XFF, 0X9D, 0XFF, 0X99, 0XFE, 0X82, 0X98, 0X49, 0XD2, 0X3D, 0XFF, 0X9E, 0XFF,
    0XBF, 0XFF, 0XDC, 0XFE, 0X4A, 0XB2, 0XA3, 0X90, 0XAB, 0XCA, 0XFC, 0XFE, 0X1D, 0XFF, 0XE9, 0XA9,
    0XA3, 0XB0, 0X63, 0XC8, 0X63, 0XC8, 0X84, 0XD0, 0X63, 0XC8, 0X63, 0XA0, 0X56, 0XF5, 0XBF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XDF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XDF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XDF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0X3D, 0XFF, 0X86, 0XA1, 0XA4, 0XB8, 0X63, 0XC8, 0X83, 0XC8, 0X83, 0XC8, 0X63, 0XC8, 0X84, 0XC8,
    0X84, 0XC0, 0X63, 0XB0, 0XA8, 0XB9, 0XBB, 0XFE, 0X3C, 0XFF, 0XF3, 0XF4, 0XA7, 0X99, 0XC4, 0X80,
    0X0A, 0XAA, 0XF1, 0XE3, 0X76, 0XFD, 0X8A, 0XD2, 0X62, 0XA8, 0X62, 0XB0, 0XE4, 0XA8, 0XAC, 0XD2,
    0XC8, 0X99, 0X05, 0X71, 0X8A, 0XA2, 0X39, 0XFE, 0X1C, 0XFF, 0X55, 0XFD, 0XE5, 0XA8, 0X83, 0XB0,
    0X83, 0XC0, 0X83, 0XC8, 0X83, 0XC0, 0X83, 0XC0, 0X63, 0XC8, 0X63, 0XA8, 0X55, 0XF5, 0XBF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0X3D, 0XFF, 0X86, 0X99, 0XA4, 0XB8, 0X83, 0XC8, 0X63, 0XC0, 0X83, 0XC8, 0XA4, 0XD0, 0X63, 0XC8,
    0X84, 0XD0, 0X63, 0XC0, 0X84, 0XB0, 0X83, 0X90, 0X6E, 0XCB, 0X3D, 0XFF, 0XFC, 0XFE, 0X19, 0XFE,
    0X8F, 0XEB, 0X09, 0XBA, 0X05, 0XA1, 0X83, 0X98, 0XE4, 0XA8, 0XE5, 0XB0, 0X26, 0XC1, 0X4A, 0XDA,
    0X52, 0XFC, 0XBC, 0XFE, 0X1D, 0XFF, 0X59, 0XFE, 0X49, 0XBA, 0XA3, 0X98, 0XA4, 0XC0, 0X43, 0XC0,
    0X63, 0XC8, 0X83, 0XC8, 0X83, 0XC0, 0XA3, 0XC8, 0X84, 0XD0, 0X63, 0XA8, 0X55, 0XF5, 0XBF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0X3D, 0XFF, 0X87, 0XA1, 0X84, 0XB0, 0X84, 0XC8, 0XA4, 0XC8, 0X83, 0XC0, 0X63, 0XC8, 0X43, 0XD0,
    0X63, 0XC8, 0XA4, 0XC8, 0X63, 0XC0, 0XC5, 0XC0, 0X84, 0XB0, 0X63, 0XA0, 0X8B, 0XDA, 0X14, 0XFD,
    0XFC, 0XFE, 0XFD, 0XFE, 0XFD, 0XFE, 0X1D, 0XFF, 0X1D, 0XFF, 0XFD, 0XFE, 0X1D, 0XFF, 0XFC, 0XFE,
    0X9A, 0XFE, 0X92, 0XFC, 0XC7, 0XC9, 0X41, 0XA0, 0XA3, 0XB8, 0XA3, 0XC0, 0X63, 0XC0, 0XA4, 0XC8,
    0X84, 0XC8, 0X83, 0XC8, 0X83, 0XC8, 0X83, 0XC8, 0X63, 0XC8, 0X42, 0XA0, 0X76, 0XFD, 0XBF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFC, 0XFE, 0X87, 0XA1, 0XA5, 0XB8, 0X84, 0XC8, 0XC4, 0XC0, 0XC4, 0XC0, 0X84, 0XC8, 0X84, 0XD0,
    0X63, 0XC8, 0XA4, 0XC8, 0X83, 0XC0, 0X84, 0XC8, 0X64, 0XC8, 0X84, 0XC0, 0X83, 0XA8, 0XC3, 0XA0,
    0XC3, 0X90, 0XC8, 0XA1, 0X8B, 0XBA, 0XED, 0XC2, 0XED, 0XBA, 0XED, 0XBA, 0X6A, 0XAA, 0X86, 0X99,
    0XA2, 0X90, 0X82, 0X98, 0X82, 0XB0, 0XC4, 0XC8, 0X83, 0XC8, 0X63, 0XC0, 0XA4, 0XC8, 0X83, 0XC0,
    0XA4, 0XC0, 0X84, 0XC8, 0X83, 0XC8, 0X83, 0XC8, 0XA4, 0XC8, 0X63, 0XA0, 0X56, 0XFD, 0X9E, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0X3D, 0XFF, 0X46, 0X91, 0X43, 0XA8, 0X44, 0XC0, 0X63, 0XB0, 0X83, 0XB0, 0X83, 0XB8, 0X63, 0XB8,
    0X63, 0XB8, 0X62, 0XB8, 0X83, 0XB8, 0X63, 0XB8, 0X83, 0XB8, 0X63, 0XB8, 0X83, 0XB8, 0X62, 0XA8,
    0XA3, 0XA8, 0X83, 0XA0, 0X62, 0XA0, 0X83, 0XA0, 0X83, 0XA0, 0XA4, 0XA0, 0X63, 0XA0, 0X83, 0XA0,
    0XC4, 0XB0, 0X83, 0XA8, 0X83, 0XB0, 0X83, 0XB8, 0X42, 0XB0, 0X83, 0XB8, 0X84, 0XB8, 0X63, 0XB0,
    0X43, 0XB8, 0X63, 0XB8, 0X63, 0XB8, 0X63, 0XB8, 0X63, 0XB0, 0X83, 0X98, 0X56, 0XF5, 0XBF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0X7E, 0XFF, 0X4E, 0XBB, 0X8C, 0XCA, 0XAC, 0XDA, 0XAC, 0XCA, 0XCC, 0XCA, 0XAB, 0XCA, 0XAB, 0XD2,
    0XAB, 0XD2, 0XAB, 0XCA, 0XEC, 0XDA, 0X8B, 0XD2, 0XAB, 0XD2, 0XAC, 0XD2, 0XCB, 0XDA, 0XAB, 0XD2,
    0XAB, 0XD2, 0XCB, 0XD2, 0X8A, 0XD2, 0XAB, 0XD2, 0X8B, 0XD2, 0X8B, 0XD2, 0X8B, 0XD2, 0XAC, 0XD2,
    0X8B, 0XCA, 0XAC, 0XD2, 0XCC, 0XD2, 0XAC, 0XD2, 0XAC, 0XD2, 0XAC, 0XD2, 0XCC, 0XD2, 0XCC, 0XD2,
    0XAC, 0XD2, 0XAC, 0XD2, 0XAC, 0XD2, 0XAC, 0XD2, 0XAB, 0XCA, 0XCC, 0XBA, 0XF8, 0XF5, 0XBF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XBF, 0XFF, 0X7E, 0XFF, 0X9F, 0XFF, 0X9E, 0XFF, 0X9E, 0XFF, 0XBE, 0XFF, 0X9E, 0XFF, 0X9E, 0XFF,
    0X9E, 0XFF, 0X9E, 0XFF, 0X7E, 0XFF, 0X9F, 0XFF, 0X9E, 0XFF, 0X9E, 0XFF, 0X9E, 0XFF, 0X9E, 0XFF,
    0X9E, 0XFF, 0X9E, 0XFF, 0X9E, 0XFF, 0X9E, 0XFF, 0X9E, 0XFF, 0X9E, 0XFF, 0X9E, 0XFF, 0X9E, 0XFF,
    0X9E, 0XFF, 0X9F, 0XFF, 0X7F, 0XFF, 0X9F, 0XFF, 0X7E, 0XFF, 0X9E, 0XFF, 0X7E, 0XFF, 0X9E, 0XFF,
    0X9F, 0XFF, 0X7E, 0XFF, 0X7E, 0XFF, 0X9E, 0XFF, 0X9E, 0XFF, 0X9D, 0XFF, 0XDF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
};

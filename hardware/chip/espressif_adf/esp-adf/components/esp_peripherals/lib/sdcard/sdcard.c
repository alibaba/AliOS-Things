/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2018 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on all ESPRESSIF SYSTEMS products, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "esp_err.h"
#include "esp_vfs_fat.h"

#include "driver/sdmmc_host.h"
#include "driver/sdmmc_defs.h"
#include "driver/gpio.h"

#include "sdcard.h"
#include "board.h"

static const char *TAG = "SDCARD";
int g_gpio = -1;

#define PIN_NUM_MISO 2
#define PIN_NUM_MOSI 15
#define PIN_NUM_CLK  14
#define PIN_NUM_CS   13

static void sdmmc_card_print_info(const sdmmc_card_t *card)
{
    ESP_LOGD(TAG, "Name: %s\n", card->cid.name);
    ESP_LOGD(TAG, "Type: %s\n", (card->ocr & SD_OCR_SDHC_CAP) ? "SDHC/SDXC" : "SDSC");
    ESP_LOGD(TAG, "Speed: %s\n", (card->csd.tr_speed > 25000000) ? "high speed" : "default speed");
    ESP_LOGD(TAG, "Size: %lluMB\n", ((uint64_t) card->csd.capacity) * card->csd.sector_size / (1024 * 1024));
    ESP_LOGD(TAG, "CSD: ver=%d, sector_size=%d, capacity=%d read_bl_len=%d\n",
             card->csd.csd_ver,
             card->csd.sector_size, card->csd.capacity, card->csd.read_block_len);
    ESP_LOGD(TAG, "SCR: sd_spec=%d, bus_width=%d\n", card->scr.sd_spec, card->scr.bus_width);
}

esp_err_t sdcard_mount(const char *base_path, periph_sdcard_mode_t mode)
{
    if (mode >= SD_MODE_MAX) {
        ESP_LOGE(TAG, "PLease select the correct sd mode: 1-line SD mode, 4-line SD mode or SPI mode!, current mode is %d", mode);
        return ESP_FAIL;
    }

    sdmmc_card_t *card = NULL;
    esp_err_t ret = 0;

    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = get_sdcard_open_file_num_max(),
    };

    if (mode != SD_MODE_SPI) {
        ESP_LOGI(TAG, "Using 1-line SD mode, 4-line SD mode,  base path=%s", base_path);
        sdmmc_host_t host = SDMMC_HOST_DEFAULT();
        // host.max_freq_khz = SDMMC_FREQ_HIGHSPEED;

        sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
        // slot_config.gpio_cd = g_gpio;
        slot_config.width = mode & 0X01;
        // Enable internal pullups on enabled pins. The internal pullups
        // are insufficient however, please make sure 10k external pullups are
        // connected on the bus. This is for debug / example purpose only.
        slot_config.flags |= SDMMC_SLOT_FLAG_INTERNAL_PULLUP;

#if CONFIG_IDF_TARGET_ESP32S3
        slot_config.clk = ESP_SD_PIN_CLK;
        slot_config.cmd = ESP_SD_PIN_CMD;
        slot_config.d0 = ESP_SD_PIN_D0;
        slot_config.d1 = ESP_SD_PIN_D1;
        slot_config.d2 = ESP_SD_PIN_D2;
        slot_config.d3 = ESP_SD_PIN_D3;
        slot_config.d4 = ESP_SD_PIN_D4;
        slot_config.d5 = ESP_SD_PIN_D5;
        slot_config.d6 = ESP_SD_PIN_D6;
        slot_config.d7 = ESP_SD_PIN_D7;
        slot_config.cd = ESP_SD_PIN_CD;
        slot_config.wp = ESP_SD_PIN_WP;
        ESP_LOGI(TAG, "Using 1-line SD mode");
#else
        gpio_set_pull_mode(GPIO_NUM_15, GPIO_PULLUP_ONLY);
        gpio_set_pull_mode(GPIO_NUM_2,  GPIO_PULLUP_ONLY);
        gpio_set_pull_mode(GPIO_NUM_13, GPIO_PULLUP_ONLY);

        if (mode == SD_MODE_4_LINE) {
            gpio_set_pull_mode(GPIO_NUM_4,  GPIO_PULLUP_ONLY);
            gpio_set_pull_mode(GPIO_NUM_12, GPIO_PULLUP_ONLY);
        }
#endif
        ret = esp_vfs_fat_sdmmc_mount(base_path, &host, &slot_config, &mount_config, &card);
    } else {
        ESP_LOGI(TAG, "Using SPI mode, base path=%s", base_path);
        sdmmc_host_t host = SDSPI_HOST_DEFAULT();
        sdspi_slot_config_t slot_config = SDSPI_SLOT_CONFIG_DEFAULT();
        slot_config.gpio_miso = PIN_NUM_MISO;
        slot_config.gpio_mosi = PIN_NUM_MOSI;
        slot_config.gpio_sck  = PIN_NUM_CLK;
        slot_config.gpio_cs   = PIN_NUM_CS;

        ret = esp_vfs_fat_sdmmc_mount(base_path, &host, &slot_config, &mount_config, &card);
    }

    switch (ret) {
        case ESP_OK:
            // Card has been initialized, print its properties
            sdmmc_card_print_info(card);
            ESP_LOGI(TAG, "CID name %s!\n", card->cid.name);
            break;

        case ESP_ERR_INVALID_STATE:
            ESP_LOGE(TAG, "File system already mounted");
            break;

        case ESP_FAIL:
            ESP_LOGE(TAG, "Failed to mount filesystem. If you want the card to be formatted, set format_if_mount_failed = true.");
            break;

        default:
            ESP_LOGE(TAG, "Failed to initialize the card (%d). Make sure SD card lines have pull-up resistors in place.", ret);
            break;
    }

    return ret;

}

esp_err_t sdcard_unmount(void)
{
    esp_err_t ret = esp_vfs_fat_sdmmc_unmount();

    if (ret == ESP_ERR_INVALID_STATE) {
        ESP_LOGE(TAG, "File system not mounted");
    }
    return ret;
}

bool sdcard_is_exist()
{
    if (g_gpio >= 0) {
        return (gpio_get_level(g_gpio) == 0x00);
    } else {
        return true;
    }
    return false;
}

int IRAM_ATTR sdcard_read_detect_pin(void)
{
    if (g_gpio >= 0) {
        return gpio_get_level(g_gpio);
    } else {
        return -1;
    }
    return 0;
}

esp_err_t sdcard_destroy()
{
    if (g_gpio >= 0) {
        return gpio_isr_handler_remove(g_gpio);
    }
    return ESP_OK;
}

esp_err_t sdcard_init(int card_detect_pin, void (*detect_intr_handler)(void *), void *isr_context)
{
    esp_err_t ret = ESP_OK;
    if (card_detect_pin >= 0) {
        gpio_set_direction(card_detect_pin, GPIO_MODE_INPUT);
        if (detect_intr_handler) {
            gpio_set_intr_type(card_detect_pin, GPIO_INTR_ANYEDGE);
            gpio_isr_handler_add(card_detect_pin, detect_intr_handler, isr_context);
            gpio_intr_enable(card_detect_pin);
        }
        gpio_pullup_en(card_detect_pin);
    }
    g_gpio = card_detect_pin;
    return ret;
}

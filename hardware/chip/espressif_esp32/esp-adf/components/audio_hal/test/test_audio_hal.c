/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2019 <ESPRESSIF SYSTEMS (SHANGHAI) CO., LTD>
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

#include "esp_log.h"
#include "audio_mem.h"
#include "board.h"
#include "audio_hal.h"
#include "driver/i2s.h"
#include "driver/gpio.h"
#include "unity.h"
#include "es8311.h"
#include "es8388.h"
#include "zl38063.h"


#define TEST_I2S_NUM  I2S_NUM_0

static const char *TAG = "TEST_AUDIO_HAL";

extern const uint8_t test_pcm_start[] asm("_binary_test_pcm_start");
extern const uint8_t test_pcm_end[]   asm("_binary_test_pcm_end");

static void i2s_init()
{
    i2s_config_t i2s_cfg = {
        .mode = I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_RX,
        .sample_rate = 16000,
        .bits_per_sample = 16,
        .channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT,
        .communication_format = I2S_COMM_FORMAT_I2S,
        .dma_buf_count = 3,
        .dma_buf_len = 300,
        .use_apll = 1,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL2,
    };

    i2s_driver_install(TEST_I2S_NUM, &i2s_cfg, 0, NULL);
    i2s_pin_config_t i2s_pin_cfg = {0};
    get_i2s_pins(TEST_I2S_NUM, &i2s_pin_cfg);
    i2s_set_pin(TEST_I2S_NUM, &i2s_pin_cfg);
    i2s_mclk_gpio_select(TEST_I2S_NUM, GPIO_NUM_0);
}

static void i2s_deinit()
{
    i2s_driver_uninstall(TEST_I2S_NUM);
}

TEST_CASE("Usage test", "[audio_hal]")
{
    ESP_LOGI(TAG, "Initialize i2s");
    i2s_init();

    ESP_LOGI(TAG, "Start codec chip");
    audio_board_handle_t board_handle = audio_board_init();
    TEST_ASSERT_NOT_NULL(board_handle);
    TEST_ASSERT_FALSE(audio_hal_ctrl_codec(board_handle->audio_hal, AUDIO_HAL_CODEC_MODE_DECODE, AUDIO_HAL_CTRL_START));

    ESP_LOGI(TAG, "Set codec volume");
    TEST_ASSERT_FALSE(audio_hal_set_volume(board_handle->audio_hal, 65));

    int volume = 0;
    TEST_ASSERT_FALSE(audio_hal_get_volume(board_handle->audio_hal, &volume));
    ESP_LOGI(TAG, "Get codec volume: %d", volume);

    size_t bytes_written = 0;
    ESP_LOGI(TAG, "Start to play music");
    TEST_ASSERT_FALSE(i2s_write(TEST_I2S_NUM, test_pcm_start, test_pcm_end - test_pcm_start, &bytes_written, portMAX_DELAY));

    ESP_LOGW(TAG, "Reach the end of music, release all resource");
    TEST_ASSERT_FALSE(audio_board_deinit(board_handle));
    i2c_driver_delete(I2C_NUM_0);
    i2s_deinit();
}

/*
 * To run this case, please choose Lyrat_v4.3 in menuconfig and run on lyrat_v4.3 board
 */
TEST_CASE("Test for es8388 driver", "[audio_hal]")
{
    ESP_LOGI(TAG, "Initialize i2s");
    i2s_init();

    ESP_LOGI(TAG, "Start es8388 codec chip");
    audio_hal_codec_config_t es8388_cfg = AUDIO_CODEC_DEFAULT_CONFIG();
    TEST_ASSERT_FALSE(es8388_init(&es8388_cfg));
    TEST_ASSERT_FALSE(es8388_config_i2s(es8388_cfg.codec_mode, &es8388_cfg.i2s_iface));
    TEST_ASSERT_FALSE(es8388_ctrl_state(AUDIO_HAL_CODEC_MODE_BOTH, AUDIO_HAL_CTRL_START));
    TEST_ASSERT_FALSE(es8388_set_voice_volume(50));

    size_t bytes_written = 0;
    ESP_LOGI(TAG, "Start to play music");
    TEST_ASSERT_FALSE(i2s_write(TEST_I2S_NUM, test_pcm_start, test_pcm_end - test_pcm_start, &bytes_written, portMAX_DELAY));

    ESP_LOGW(TAG, "Reach the end of music, release all resource");
    i2c_driver_delete(I2C_NUM_0);
    i2s_deinit();
}

/*
 * To run this case, please choose Lyrat_mini in menuconfig and run on lyrat_mini board
 */
TEST_CASE("Test for es8311 driver", "[audio_hal]")
{
    ESP_LOGI(TAG, "Initialize i2s");
    i2s_init();

    ESP_LOGI(TAG, "Start es8311 codec chip");
    audio_hal_codec_config_t es8311_cfg = AUDIO_CODEC_DEFAULT_CONFIG();
    TEST_ASSERT_FALSE(es8311_codec_init(&es8311_cfg));
    TEST_ASSERT_FALSE(es8311_codec_config_i2s(es8311_cfg.codec_mode, &es8311_cfg.i2s_iface));
    TEST_ASSERT_FALSE(es8311_codec_ctrl_state(AUDIO_HAL_CODEC_MODE_BOTH, AUDIO_HAL_CTRL_START));
    TEST_ASSERT_FALSE(es8311_codec_set_voice_volume(50));

    size_t bytes_written = 0;
    ESP_LOGI(TAG, "Start to play music");
    TEST_ASSERT_FALSE(i2s_write(TEST_I2S_NUM, test_pcm_start, test_pcm_end - test_pcm_start, &bytes_written, portMAX_DELAY));

    ESP_LOGW(TAG, "Reach the end of music, release all resource");
    i2c_driver_delete(I2C_NUM_0);
    i2s_deinit();
}

/*
 * To run this case, please choose LyratD_MSC in menuconfig and run on lyratD_MSC board
 */
TEST_CASE("Test for zl38063 driver", "[audio_hal]")
{
    ESP_LOGI(TAG, "Initialize i2s");
    i2s_init();

    ESP_LOGI(TAG, "Start zl38063 DSP");
    audio_hal_codec_config_t zl38063_cfg = AUDIO_CODEC_DEFAULT_CONFIG();
    TEST_ASSERT_FALSE(zl38063_codec_init(&zl38063_cfg));
    TEST_ASSERT_FALSE(zl38063_codec_config_i2s(zl38063_cfg.codec_mode, &zl38063_cfg.i2s_iface));
    TEST_ASSERT_FALSE(zl38063_codec_ctrl_state(AUDIO_HAL_CODEC_MODE_BOTH, AUDIO_HAL_CTRL_START));
    TEST_ASSERT_FALSE(zl38063_codec_set_voice_volume(50));

    size_t bytes_written = 0;
    ESP_LOGI(TAG, "Start to play music");
    TEST_ASSERT_FALSE(i2s_write(TEST_I2S_NUM, test_pcm_start, test_pcm_end - test_pcm_start, &bytes_written, portMAX_DELAY));

    ESP_LOGW(TAG, "Reach the end of music, release all resource");
    i2c_driver_delete(I2C_NUM_0);
    i2s_deinit();
}

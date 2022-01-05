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
#include "board.h"
#include "audio_mem.h"
#include "display_service.h"
#include "periph_service.h"
#include "led_bar_is31x.h"
#include "led_indicator.h"
#include "led_bar_ws2812.h"
#include "led_bar_aw2013.h"
#include "unity.h"

static const char *TAG = "test_display_service";

/*
 * Usage of display service
 */
TEST_CASE("Create a display service and set different pattern", "[display_service]")
{
    display_service_handle_t display_handle = audio_board_led_init();
    TEST_ASSERT_NOT_NULL(display_handle);

    ESP_LOGI(TAG, "wifi connected");
    TEST_ASSERT_FALSE(display_service_set_pattern(display_handle, DISPLAY_PATTERN_WIFI_CONNECTED, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "wifi setting");
    TEST_ASSERT_FALSE(display_service_set_pattern(display_handle, DISPLAY_PATTERN_WIFI_SETTING, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "wifi connectting");
    TEST_ASSERT_FALSE(display_service_set_pattern(display_handle, DISPLAY_PATTERN_WIFI_CONNECTTING, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "wifi disconnected");
    TEST_ASSERT_FALSE(display_service_set_pattern(display_handle, DISPLAY_PATTERN_WIFI_DISCONNECTED, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "wifi setting finished");
    TEST_ASSERT_FALSE(display_service_set_pattern(display_handle, DISPLAY_PATTERN_WIFI_SETTING_FINISHED, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "wakeup on");
    TEST_ASSERT_FALSE(display_service_set_pattern(display_handle, DISPLAY_PATTERN_WAKEUP_ON, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "wakeup finish");
    TEST_ASSERT_FALSE(display_service_set_pattern(display_handle, DISPLAY_PATTERN_WAKEUP_FINISHED, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "speech begin");
    TEST_ASSERT_FALSE(display_service_set_pattern(display_handle, DISPLAY_PATTERN_SPEECH_BEGIN, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "speech finish");
    TEST_ASSERT_FALSE(display_service_set_pattern(display_handle, DISPLAY_PATTERN_WAKEUP_FINISHED, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGW(TAG, "Display service will be destroyed");
    TEST_ASSERT_FALSE(display_destroy(display_handle));
}

/*
 * When there is no led display chip, such as Lyrat-4.3, Lyrat-mini, we use "led_indicator" to control leds on board.
 * To run this case, please choose Lyrat-4.3 or Lyrat-mini.
 */
TEST_CASE("Create a display service on board without is31flXXX chip", "[display_service]")
{
    led_indicator_handle_t led_handle = led_indicator_init((gpio_num_t)get_green_led_gpio());
    TEST_ASSERT_NOT_NULL(led_handle);

    ESP_LOGI(TAG, "wifi connected");
    TEST_ASSERT_FALSE(led_indicator_pattern(led_handle, DISPLAY_PATTERN_WIFI_CONNECTED, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "wifi setting");
    TEST_ASSERT_FALSE(led_indicator_pattern(led_handle, DISPLAY_PATTERN_WIFI_SETTING, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "wifi connectting");
    TEST_ASSERT_FALSE(led_indicator_pattern(led_handle, DISPLAY_PATTERN_WIFI_CONNECTTING, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "wifi disconnected");
    TEST_ASSERT_FALSE(led_indicator_pattern(led_handle, DISPLAY_PATTERN_WIFI_DISCONNECTED, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "wifi setting finished");
    TEST_ASSERT_FALSE(led_indicator_pattern(led_handle, DISPLAY_PATTERN_WIFI_SETTING_FINISHED, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGW(TAG, "Display service will be destroyed");
    led_indicator_deinit(led_handle);
}

/*
 * When there is a led display chip, such as MSC_V2.1, MSC_V2.2, we use "led_bar" to control the leds on board
 * To run this case, please choose MSC_V2.1 or MSC_V2.2
 */
TEST_CASE("Create a display service on board with an is31flXXX chip", "[display_service]")
{
    esp_periph_handle_t led_handle = led_bar_is31x_init();;
    TEST_ASSERT_NOT_NULL(led_handle);

    ESP_LOGI(TAG, "wifi connected");
    TEST_ASSERT_FALSE(led_bar_is31x_pattern(led_handle, DISPLAY_PATTERN_WIFI_CONNECTED, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "wifi setting");
    TEST_ASSERT_FALSE(led_bar_is31x_pattern(led_handle, DISPLAY_PATTERN_WIFI_SETTING, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "wifi connectting");
    TEST_ASSERT_FALSE(led_bar_is31x_pattern(led_handle, DISPLAY_PATTERN_WIFI_CONNECTTING, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "wifi disconnected");
    TEST_ASSERT_FALSE(led_bar_is31x_pattern(led_handle, DISPLAY_PATTERN_WIFI_DISCONNECTED, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "wifi setting finished");
    TEST_ASSERT_FALSE(led_bar_is31x_pattern(led_handle, DISPLAY_PATTERN_WIFI_SETTING_FINISHED, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGW(TAG, "Display service will be destroyed");
    led_bar_is31x_deinit(led_handle);
}

/*
 * When there are both led display chip and led controled by gpio on the board, run this case to check function.
 */
TEST_CASE("Use both led bar and led indicator", "[display_service]")
{
    esp_periph_handle_t led_bar_handle = led_bar_is31x_init();
    TEST_ASSERT_NOT_NULL(led_bar_handle);

    led_indicator_handle_t led_handle = led_indicator_init((gpio_num_t)get_green_led_gpio());
    TEST_ASSERT_NOT_NULL(led_handle);

    ESP_LOGI(TAG, "wifi connected");
    TEST_ASSERT_FALSE(led_bar_is31x_pattern(led_bar_handle, DISPLAY_PATTERN_WIFI_CONNECTED, 0));
    TEST_ASSERT_FALSE(led_indicator_pattern(led_handle, DISPLAY_PATTERN_WIFI_CONNECTED, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "wifi setting");
    TEST_ASSERT_FALSE(led_bar_is31x_pattern(led_bar_handle, DISPLAY_PATTERN_WIFI_SETTING, 0));
    TEST_ASSERT_FALSE(led_indicator_pattern(led_handle, DISPLAY_PATTERN_WIFI_SETTING, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "wifi connectting");
    TEST_ASSERT_FALSE(led_bar_is31x_pattern(led_bar_handle, DISPLAY_PATTERN_WIFI_CONNECTTING, 0));
    TEST_ASSERT_FALSE(led_indicator_pattern(led_handle, DISPLAY_PATTERN_WIFI_CONNECTTING, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "wifi disconnected");
    TEST_ASSERT_FALSE(led_bar_is31x_pattern(led_bar_handle, DISPLAY_PATTERN_WIFI_DISCONNECTED, 0));
    TEST_ASSERT_FALSE(led_indicator_pattern(led_handle, DISPLAY_PATTERN_WIFI_DISCONNECTED, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "wifi setting finished");
    TEST_ASSERT_FALSE(led_bar_is31x_pattern(led_bar_handle, DISPLAY_PATTERN_WIFI_SETTING_FINISHED, 0));
    TEST_ASSERT_FALSE(led_indicator_pattern(led_handle, DISPLAY_PATTERN_WIFI_SETTING_FINISHED, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGW(TAG, "Display service will be destroyed");
    led_bar_is31x_deinit(led_bar_handle);
    led_indicator_deinit(led_handle);
}

TEST_CASE("Test when memory is not enough", "[display_service]")
{
    void *pt = NULL;
    do {
        pt = audio_calloc(1, 1);
    } while (pt);
    ESP_LOGW(TAG, "The memory has been run out, enter test ...");

    esp_periph_handle_t led_bar_handle = led_bar_is31x_init();
    TEST_ASSERT_NULL(led_bar_handle);

    led_indicator_handle_t led_handle = led_indicator_init((gpio_num_t)get_green_led_gpio());
    TEST_ASSERT_NULL(led_handle);

    ESP_LOGI(TAG, "wifi connected");
    TEST_ASSERT_TRUE(led_bar_is31x_pattern(led_bar_handle, DISPLAY_PATTERN_WIFI_CONNECTED, 0));
    TEST_ASSERT_TRUE(led_indicator_pattern(led_handle, DISPLAY_PATTERN_WIFI_CONNECTED, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "wifi setting");
    TEST_ASSERT_TRUE(led_bar_is31x_pattern(led_bar_handle, DISPLAY_PATTERN_WIFI_SETTING, 0));
    TEST_ASSERT_TRUE(led_indicator_pattern(led_handle, DISPLAY_PATTERN_WIFI_SETTING, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "wifi connectting");
    TEST_ASSERT_TRUE(led_bar_is31x_pattern(led_bar_handle, DISPLAY_PATTERN_WIFI_CONNECTTING, 0));
    TEST_ASSERT_TRUE(led_indicator_pattern(led_handle, DISPLAY_PATTERN_WIFI_CONNECTTING, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "wifi disconnected");
    TEST_ASSERT_TRUE(led_bar_is31x_pattern(led_bar_handle, DISPLAY_PATTERN_WIFI_DISCONNECTED, 0));
    TEST_ASSERT_TRUE(led_indicator_pattern(led_handle, DISPLAY_PATTERN_WIFI_DISCONNECTED, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "wifi setting finished");
    TEST_ASSERT_TRUE(led_bar_is31x_pattern(led_bar_handle, DISPLAY_PATTERN_WIFI_SETTING_FINISHED, 0));
    TEST_ASSERT_TRUE(led_indicator_pattern(led_handle, DISPLAY_PATTERN_WIFI_SETTING_FINISHED, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGW(TAG, "Display service will be destroyed");
    led_bar_is31x_deinit(led_bar_handle);
    led_indicator_deinit(led_handle);
}

/*
 * When there is a ws2812 display chip
 * To run this case, please choose DUL1906_D v1.0
 */
TEST_CASE("Create a display service on board with an ws2812 chip", "[display_service]")
{
    led_bar_ws2812_handle_t ws2812_handle = led_bar_ws2812_init((gpio_num_t)get_ws2812_gpio_pin(), (int)2);
    TEST_ASSERT_NOT_NULL(ws2812_handle);

    ESP_LOGI(TAG, "wifi setting");
    TEST_ASSERT_FALSE(led_bar_ws2812_pattern(ws2812_handle, DISPLAY_PATTERN_WIFI_SETTING, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "wifi connectting");
    TEST_ASSERT_FALSE(led_bar_ws2812_pattern(ws2812_handle, DISPLAY_PATTERN_WIFI_CONNECTTING, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "wifi connected");
    TEST_ASSERT_FALSE(led_bar_ws2812_pattern(ws2812_handle, DISPLAY_PATTERN_WIFI_CONNECTED, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "wifi setting finished");
    TEST_ASSERT_FALSE(led_bar_ws2812_pattern(ws2812_handle, DISPLAY_PATTERN_WIFI_SETTING_FINISHED, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "bt connectting");
    TEST_ASSERT_FALSE(led_bar_ws2812_pattern(ws2812_handle, DISPLAY_PATTERN_BT_CONNECTTING, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "bt connected");
    TEST_ASSERT_FALSE(led_bar_ws2812_pattern(ws2812_handle, DISPLAY_PATTERN_BT_CONNECTED, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "bt disconnected");
    TEST_ASSERT_FALSE(led_bar_ws2812_pattern(ws2812_handle, DISPLAY_PATTERN_BT_DISCONNECTED, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "recording start");
    TEST_ASSERT_FALSE(led_bar_ws2812_pattern(ws2812_handle, DISPLAY_PATTERN_RECORDING_START, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "recording stop");
    TEST_ASSERT_FALSE(led_bar_ws2812_pattern(ws2812_handle, DISPLAY_PATTERN_RECORDING_STOP, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "recotnition start");
    TEST_ASSERT_FALSE(led_bar_ws2812_pattern(ws2812_handle, DISPLAY_PATTERN_RECOGNITION_START, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "recotnition stop");
    TEST_ASSERT_FALSE(led_bar_ws2812_pattern(ws2812_handle, DISPLAY_PATTERN_RECOGNITION_STOP, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "wake up on");
    TEST_ASSERT_FALSE(led_bar_ws2812_pattern(ws2812_handle, DISPLAY_PATTERN_WAKEUP_ON, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "wake up finished");
    TEST_ASSERT_FALSE(led_bar_ws2812_pattern(ws2812_handle, DISPLAY_PATTERN_WAKEUP_FINISHED, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "music on");
    TEST_ASSERT_FALSE(led_bar_ws2812_pattern(ws2812_handle, DISPLAY_PATTERN_MUSIC_ON, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "music finised");
    TEST_ASSERT_FALSE(led_bar_ws2812_pattern(ws2812_handle, DISPLAY_PATTERN_MUSIC_FINISHED, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "volume");
    TEST_ASSERT_FALSE(led_bar_ws2812_pattern(ws2812_handle, DISPLAY_PATTERN_VOLUME, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "mute off");
    TEST_ASSERT_FALSE(led_bar_ws2812_pattern(ws2812_handle, DISPLAY_PATTERN_MUTE_OFF , 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "mute on");
    TEST_ASSERT_FALSE(led_bar_ws2812_pattern(ws2812_handle, DISPLAY_PATTERN_MUTE_ON, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "turn on");
    TEST_ASSERT_FALSE(led_bar_ws2812_pattern(ws2812_handle, DISPLAY_PATTERN_TURN_ON, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "turn off");
    TEST_ASSERT_FALSE(led_bar_ws2812_pattern(ws2812_handle, DISPLAY_PATTERN_TURN_OFF, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "battery low");
    TEST_ASSERT_FALSE(led_bar_ws2812_pattern(ws2812_handle, DISPLAY_PATTERN_BATTERY_LOW, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "battery charging");
    TEST_ASSERT_FALSE(led_bar_ws2812_pattern(ws2812_handle, DISPLAY_PATTERN_BATTERY_CHARGING, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "battery full");
    TEST_ASSERT_FALSE(led_bar_ws2812_pattern(ws2812_handle, DISPLAY_PATTERN_BATTERY_FULL, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "poweron init");
    TEST_ASSERT_FALSE(led_bar_ws2812_pattern(ws2812_handle, DISPLAY_PATTERN_POWERON_INIT, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "wifi no cfg");
    TEST_ASSERT_FALSE(led_bar_ws2812_pattern(ws2812_handle, DISPLAY_PATTERN_WIFI_NO_CFG, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "speech begin");
    TEST_ASSERT_FALSE(led_bar_ws2812_pattern(ws2812_handle, DISPLAY_PATTERN_SPEECH_BEGIN, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "speech over");
    TEST_ASSERT_FALSE(led_bar_ws2812_pattern(ws2812_handle, DISPLAY_PATTERN_SPEECH_OVER, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGW(TAG, "Display service will be destroyed");
    led_bar_ws2812_deinit(ws2812_handle);
}

/*
 * When there is a aw2013 display chip
 * To run this case, please choose cpuid board
 */
TEST_CASE("Create a display service on board with an aw2013 chip", "[display_service]")
{
    esp_periph_handle_t aw2013_handle = led_bar_aw2013_init();
    TEST_ASSERT_NOT_NULL(aw2013_handle);
    TEST_ASSERT_FALSE(led_bar_aw2013_set_blink_time(aw2013_handle, 5, 300));

    display_service_config_t display = {
        .based_cfg = {
            .task_stack = 3 * 1024,
            .task_prio = 5,
            .task_core = 0,
            .task_func = aw2013_led_bar_task,
            .service_start = NULL,
            .service_stop = NULL,
            .service_destroy = NULL,
            .service_ioctl = led_bar_aw2013_pattern,
            .service_name = "aw2013 display server",
            .user_data = NULL
        },
        .instance = aw2013_handle,
    };
    display_service_handle_t disp_serv = display_service_create(&display);
    TEST_ASSERT_NOT_NULL(disp_serv);

    ESP_LOGI(TAG, "wifi setting");
    TEST_ASSERT_FALSE(display_service_set_pattern(disp_serv, DISPLAY_PATTERN_WIFI_SETTING, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "wifi connectting");
    TEST_ASSERT_FALSE(display_service_set_pattern(disp_serv, DISPLAY_PATTERN_WIFI_CONNECTTING, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "wifi connected");
    TEST_ASSERT_FALSE(display_service_set_pattern(disp_serv, DISPLAY_PATTERN_WIFI_CONNECTED, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "wifi setting finished");
    TEST_ASSERT_FALSE(display_service_set_pattern(disp_serv, DISPLAY_PATTERN_WIFI_SETTING_FINISHED, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "bt connectting");
    TEST_ASSERT_FALSE(display_service_set_pattern(disp_serv, DISPLAY_PATTERN_BT_CONNECTTING, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "bt connected");
    TEST_ASSERT_FALSE(display_service_set_pattern(disp_serv, DISPLAY_PATTERN_BT_CONNECTED, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "bt disconnected");
    TEST_ASSERT_FALSE(display_service_set_pattern(disp_serv, DISPLAY_PATTERN_BT_DISCONNECTED, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "recording start");
    TEST_ASSERT_FALSE(display_service_set_pattern(disp_serv, DISPLAY_PATTERN_RECORDING_START, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "recording stop");
    TEST_ASSERT_FALSE(display_service_set_pattern(disp_serv, DISPLAY_PATTERN_RECORDING_STOP, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "recotnition start");
    TEST_ASSERT_FALSE(display_service_set_pattern(disp_serv, DISPLAY_PATTERN_RECOGNITION_START, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "recotnition stop");
    TEST_ASSERT_FALSE(display_service_set_pattern(disp_serv, DISPLAY_PATTERN_RECOGNITION_STOP, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "wake up on");
    TEST_ASSERT_FALSE(display_service_set_pattern(disp_serv, DISPLAY_PATTERN_WAKEUP_ON, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "wake up finished");
    TEST_ASSERT_FALSE(display_service_set_pattern(disp_serv, DISPLAY_PATTERN_WAKEUP_FINISHED, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "music on");
    TEST_ASSERT_FALSE(display_service_set_pattern(disp_serv, DISPLAY_PATTERN_MUSIC_ON, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "music finised");
    TEST_ASSERT_FALSE(display_service_set_pattern(disp_serv, DISPLAY_PATTERN_MUSIC_FINISHED, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "volume");
    TEST_ASSERT_FALSE(display_service_set_pattern(disp_serv, DISPLAY_PATTERN_VOLUME, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "mute off");
    TEST_ASSERT_FALSE(display_service_set_pattern(disp_serv, DISPLAY_PATTERN_MUTE_OFF , 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "mute on");
    TEST_ASSERT_FALSE(display_service_set_pattern(disp_serv, DISPLAY_PATTERN_MUTE_ON, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "turn on");
    TEST_ASSERT_FALSE(display_service_set_pattern(disp_serv, DISPLAY_PATTERN_TURN_ON, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "turn off");
    TEST_ASSERT_FALSE(display_service_set_pattern(disp_serv, DISPLAY_PATTERN_TURN_OFF, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "battery low");
    TEST_ASSERT_FALSE(display_service_set_pattern(disp_serv, DISPLAY_PATTERN_BATTERY_LOW, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "battery charging");
    TEST_ASSERT_FALSE(display_service_set_pattern(disp_serv, DISPLAY_PATTERN_BATTERY_CHARGING, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "battery full");
    TEST_ASSERT_FALSE(display_service_set_pattern(disp_serv, DISPLAY_PATTERN_BATTERY_FULL, 0));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_LOGW(TAG, "Display service will be destroyed");
    TEST_ASSERT_FALSE(display_destroy(disp_serv));
    led_bar_aw2013_deinit(aw2013_handle);
}

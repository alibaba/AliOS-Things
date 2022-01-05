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
#include <unistd.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "audio_event_iface.h"
#include "esp_log.h"
#include "esp_err.h"

#include "unity.h"
#include "nvs_flash.h"
#include "tcpip_adapter.h"
#include "audio_mem.h"
#include "esp_peripherals.h"
#include "periph_sdcard.h"
#include "periph_button.h"
#include "periph_touch.h"
#include "periph_wifi.h"
#include "periph_console.h"
#include "periph_led.h"
#include "periph_adc_button.h"
#include "periph_gpio_isr.h"
#include "periph_is31fl3216.h"
#include "periph_spiffs.h"
#include "periph_ws2812.h"
#include "board.h"

static const char *TAG = "ESP_PERIPH_TEST";

#define TEST_PERIPHERALS_MEMORY_LEAK_TIMES  1000

static void periph_adc_button_test(void)
{
    ESP_LOGI(TAG, "Set up peripherals handle");
    esp_periph_config_t periph_cfg = DEFAULT_ESP_PERIPH_SET_CONFIG();
    esp_periph_set_handle_t set = esp_periph_set_init(&periph_cfg);
    TEST_ASSERT_NOT_NULL(set);

    ESP_LOGI(TAG, "Register ADC button to peripherals");
    periph_adc_button_cfg_t adc_btn_cfg = PERIPH_ADC_BUTTON_DEFAULT_CONFIG();
    adc_arr_t adc_btn_tag = ADC_DEFAULT_ARR();
    adc_btn_tag.total_steps = 6;
    int btn_array[7] = {190, 600, 1000, 1375, 1775, 2195, 3100};
    adc_btn_tag.adc_level_step = btn_array;
    adc_btn_cfg.arr = &adc_btn_tag;
    adc_btn_cfg.arr_size = 1;
    esp_periph_handle_t adc_btn_handle = periph_adc_button_init(&adc_btn_cfg);
    TEST_ASSERT_NOT_NULL(adc_btn_handle);
    TEST_ASSERT_FALSE(esp_periph_start(set, adc_btn_handle));

    ESP_LOGI(TAG, "Set up event listener");
    audio_event_iface_cfg_t evt_cfg = AUDIO_EVENT_IFACE_DEFAULT_CFG();
    audio_event_iface_handle_t evt = audio_event_iface_init(&evt_cfg);
    TEST_ASSERT_NOT_NULL(evt);

    ESP_LOGI(TAG, "Listening event from peripherals");
    TEST_ASSERT_FALSE(audio_event_iface_set_listener(esp_periph_set_get_event_iface(set), evt));

    ESP_LOGI(TAG, "Test start, please press buttons on the board ... ");
    while (1) {
        audio_event_iface_msg_t msg;
        TEST_ASSERT_FALSE(audio_event_iface_listen(evt, &msg, portMAX_DELAY));
        ESP_LOGI(TAG, "action: %d, act id: %d", msg.cmd, (int)msg.data);
        if ((int)msg.data == 0) {
            ESP_LOGW(TAG, "press id 0, quit test");
            break;
        }
    }

    ESP_LOGI(TAG, "Quit test, release all resources");
    TEST_ASSERT_FALSE(esp_periph_set_stop_all(set));
    TEST_ASSERT_FALSE(audio_event_iface_remove_listener(esp_periph_set_get_event_iface(set), evt));
    TEST_ASSERT_FALSE(audio_event_iface_destroy(evt));
    TEST_ASSERT_FALSE(esp_periph_set_destroy(set));
}

TEST_CASE("adc button test", "[peripherals]")
{
    periph_adc_button_test();
}

static void periph_gpio_button_test(void)
{
    ESP_LOGI(TAG, "Set up peripherals handle");
    esp_periph_config_t periph_cfg = DEFAULT_ESP_PERIPH_SET_CONFIG();
    esp_periph_set_handle_t set = esp_periph_set_init(&periph_cfg);
    TEST_ASSERT_NOT_NULL(set);

    ESP_LOGI(TAG, "Register gpio button to peripherals");
    periph_button_cfg_t btn_cfg = {
        .gpio_mask = (1ULL << get_input_rec_id()) | (1ULL << get_input_mode_id()), //REC BTN & MODE BTN
    };
    esp_periph_handle_t button_handle = periph_button_init(&btn_cfg);
    TEST_ASSERT_NOT_NULL(button_handle);
    TEST_ASSERT_FALSE(esp_periph_start(set, button_handle));

    ESP_LOGI(TAG, "Set up event listener");
    audio_event_iface_cfg_t evt_cfg = AUDIO_EVENT_IFACE_DEFAULT_CFG();
    audio_event_iface_handle_t evt = audio_event_iface_init(&evt_cfg);
    TEST_ASSERT_NOT_NULL(evt);

    ESP_LOGI(TAG, "Listening event from peripherals");
    TEST_ASSERT_FALSE(audio_event_iface_set_listener(esp_periph_set_get_event_iface(set), evt));

    ESP_LOGI(TAG, "Test start, please press buttons on the board ... ");
    while (1) {
        audio_event_iface_msg_t msg;
        TEST_ASSERT_FALSE(audio_event_iface_listen(evt, &msg, portMAX_DELAY));
        ESP_LOGI(TAG, "action: %d, act id: %d", msg.cmd, (int)msg.data);
        if ((int)msg.data == get_input_mode_id()) {
            ESP_LOGW(TAG, "press [mode] button, quit test");
            break;
        }
    }

    ESP_LOGI(TAG, "Quit test, release all resources");
    TEST_ASSERT_FALSE(esp_periph_set_stop_all(set));
    TEST_ASSERT_FALSE(audio_event_iface_remove_listener(esp_periph_set_get_event_iface(set), evt));
    TEST_ASSERT_FALSE(audio_event_iface_destroy(evt));
    TEST_ASSERT_FALSE(esp_periph_set_destroy(set));
}

TEST_CASE("gpio button test", "[peripherals]")
{
    periph_gpio_button_test();
}

static esp_err_t play_func(esp_periph_handle_t periph, int argc, char *argv[])
{
    if (argc == 1) {
        ESP_LOGI(TAG, "play muisc, url:%s", argv[0]);
    } else if (argc == 0) {
        ESP_LOGI(TAG, "play");
    } else {
        ESP_LOGE(TAG, "error input");
    }
    return ESP_OK;
}

static esp_err_t pause_func(esp_periph_handle_t periph, int argc, char *argv[])
{
    if (argc == 0) {
        ESP_LOGI(TAG, "pause");
    } else {
        ESP_LOGE(TAG, "error input");
    }
    return ESP_OK;
}

static bool task_flag;

static esp_err_t quit_func(esp_periph_handle_t periph, int argc, char *argv[])
{
    if (argc == 0) {
        ESP_LOGI(TAG, "quit console");
        task_flag = false;
    } else {
        ESP_LOGE(TAG, "error input");
    }
    return ESP_OK;
}

const periph_console_cmd_t cli_cmd[] = {
    {
        .cmd = "play",
        .id  = 0,
        .help = "play music",
        .func = play_func,
    },
    {
        .cmd = "pause",
        .id = 1,
        .help = "pause music",
        .func = pause_func,
    },
    {
        .cmd = "quit",
        .id = 2,
        .help = "quit command line",
        .func = quit_func
    }
};

static void periph_console_test(void)
{
    ESP_LOGI(TAG, "Set up peripherals handle");
    esp_periph_config_t periph_cfg = DEFAULT_ESP_PERIPH_SET_CONFIG();
    esp_periph_set_handle_t set = esp_periph_set_init(&periph_cfg);
    TEST_ASSERT_NOT_NULL(set);

    ESP_LOGI(TAG, "Register console to peripherals");
    periph_console_cfg_t console_cfg = {
        .command_num = sizeof(cli_cmd) / sizeof(periph_console_cmd_t),
        .commands = cli_cmd,
    };
    esp_periph_handle_t console_handle = periph_console_init(&console_cfg);
    TEST_ASSERT_NOT_NULL(console_handle);

    task_flag = true;
    ESP_LOGI(TAG, "Start console, please input ...");
    TEST_ASSERT_FALSE(esp_periph_start(set, console_handle));

    while (task_flag) {
        vTaskDelay(10 / portTICK_RATE_MS);
    }

    ESP_LOGI(TAG, "Quit test, release all resources");
    TEST_ASSERT_FALSE(esp_periph_set_stop_all(set));
    TEST_ASSERT_FALSE(esp_periph_set_destroy(set));
}

TEST_CASE("console test", "[peripherals]")
{
    periph_console_test();
}

static void periph_gpio_isr_test(void)
{
    ESP_LOGI(TAG, "Set up peripherals handle");
    esp_periph_config_t periph_cfg = DEFAULT_ESP_PERIPH_SET_CONFIG();
    esp_periph_set_handle_t set = esp_periph_set_init(&periph_cfg);
    TEST_ASSERT_NOT_NULL(set);

    ESP_LOGI(TAG, "Register gpio isr to peripherals");
    gpio_isr_info_t gpio_isr_info = {
        .gpio_num = SDCARD_INTR_GPIO,
        .type = GPIO_INTR_ANYEDGE
    };
    periph_gpio_isr_cfg_t gpio_isr_cfg = {
        .info_size = 1,
        .gpio_isr_info = &gpio_isr_info
    };
    esp_periph_handle_t gpio_isr_handle = periph_gpio_isr_init(&gpio_isr_cfg);
    TEST_ASSERT_NOT_NULL(gpio_isr_handle);
    TEST_ASSERT_FALSE(esp_periph_start(set, gpio_isr_handle));

    ESP_LOGI(TAG, "Set up event listener");
    audio_event_iface_cfg_t evt_cfg = AUDIO_EVENT_IFACE_DEFAULT_CFG();
    audio_event_iface_handle_t evt = audio_event_iface_init(&evt_cfg);
    TEST_ASSERT_NOT_NULL(evt);

    ESP_LOGI(TAG, "Listening event from peripherals, please insert a sdcard to the board ...");
    TEST_ASSERT_FALSE(audio_event_iface_set_listener(esp_periph_set_get_event_iface(set), evt));

    while (1) {
        audio_event_iface_msg_t msg;
        TEST_ASSERT_FALSE(audio_event_iface_listen(evt, &msg, portMAX_DELAY));

        if ((int)msg.data == SDCARD_INTR_GPIO) {
            ESP_LOGW(TAG, "Detect sdcard insertion, quit test");
            break;
        }
    }

    ESP_LOGI(TAG, "Quit test, release all resources");
    TEST_ASSERT_FALSE(esp_periph_set_stop_all(set));
    TEST_ASSERT_FALSE(audio_event_iface_remove_listener(esp_periph_set_get_event_iface(set), evt));
    TEST_ASSERT_FALSE(audio_event_iface_destroy(evt));
    TEST_ASSERT_FALSE(esp_periph_set_destroy(set));
}

TEST_CASE("gpio isr test", "[peripherals]")
{
    periph_gpio_isr_test();
}

static void periph_is31fl3216_test(void)
{
    ESP_LOGI(TAG, "Set up peripherals handle");
    esp_periph_config_t periph_cfg = DEFAULT_ESP_PERIPH_SET_CONFIG();
    esp_periph_set_handle_t set = esp_periph_set_init(&periph_cfg);
    TEST_ASSERT_NOT_NULL(set);
    periph_is31fl3216_cfg_t is31fl3216_cfg = {
        .state = IS31FL3216_STATE_ON
    };

    ESP_LOGI(TAG, "Register gpio isr to peripherals");
    esp_periph_handle_t is31fl3216_handle = periph_is31fl3216_init(&is31fl3216_cfg);
    TEST_ASSERT_FALSE(esp_periph_start(set, is31fl3216_handle));

    for (int i = 0; i < BLUE_LED_MAX_NUM; i++) {
        TEST_ASSERT_FALSE(periph_is31fl3216_set_duty(is31fl3216_handle, i, 255));
    }

    TEST_ASSERT_FALSE(periph_is31fl3216_set_light_on_num(is31fl3216_handle, 1, BLUE_LED_MAX_NUM));
    TEST_ASSERT_FALSE(periph_is31fl3216_set_interval(is31fl3216_handle, 100));
    TEST_ASSERT_FALSE(periph_is31fl3216_set_shift_mode(is31fl3216_handle, PERIPH_IS31_SHIFT_MODE_ACC));
    TEST_ASSERT_FALSE(periph_is31fl3216_set_state(is31fl3216_handle, IS31FL3216_STATE_SHIFT));
    ESP_LOGI(TAG, "Start testing for 5 seconds...");

    vTaskDelay(5000 / portTICK_RATE_MS);

    ESP_LOGI(TAG, "Quit test, release all resources");
    TEST_ASSERT_FALSE(esp_periph_set_stop_all(set));
    TEST_ASSERT_FALSE(esp_periph_set_destroy(set));
}

TEST_CASE("is31fl3216 test", "[peripherals]")
{
    periph_is31fl3216_test();
}

static void periph_led_test(void)
{
    ESP_LOGI(TAG, "Set up peripherals handle");
    esp_periph_config_t periph_cfg = DEFAULT_ESP_PERIPH_SET_CONFIG();
    esp_periph_set_handle_t set = esp_periph_set_init(&periph_cfg);
    TEST_ASSERT_NOT_NULL(set);

    periph_led_cfg_t led_cfg = {
        .led_speed_mode = LEDC_LOW_SPEED_MODE,
        .led_duty_resolution = LEDC_TIMER_10_BIT,
        .led_timer_num = LEDC_TIMER_0,
        .led_freq_hz = 5000,
    };

    esp_periph_handle_t led_handle = periph_led_init(&led_cfg);
    TEST_ASSERT_FALSE(esp_periph_start(set, led_handle));

    TEST_ASSERT_FALSE(periph_led_blink(led_handle, get_blue_led_gpio(), 1000, 1000, true, -1, 0));
    TEST_ASSERT_FALSE(periph_led_blink(led_handle, get_green_led_gpio(), 500, 500, false, 4, 0));

    ESP_LOGI(TAG, "running...");
    vTaskDelay(1000 / portTICK_RATE_MS);
    ESP_LOGI(TAG, "STOP BLUE LED");
    TEST_ASSERT_FALSE(periph_led_stop(led_handle, get_blue_led_gpio()));

    vTaskDelay(1000 / portTICK_RATE_MS);
    ESP_LOGI(TAG, "Changing blink preset...");
    TEST_ASSERT_FALSE(periph_led_blink(led_handle, get_blue_led_gpio(), 500, 200, false, -1, 0));
    TEST_ASSERT_FALSE(periph_led_blink(led_handle, get_green_led_gpio(), 500, 1000, true, -1, 0));

    ESP_LOGI(TAG, "Quit test, release all resources");
    TEST_ASSERT_FALSE(esp_periph_set_stop_all(set));
    TEST_ASSERT_FALSE(esp_periph_set_destroy(set));
}

TEST_CASE("led test", "[peripherals]")
{
    periph_led_test();
}

static void periph_sdcard_test(void)
{
    ESP_LOGI(TAG, "Set up peripherals handle");
    esp_periph_config_t periph_cfg = DEFAULT_ESP_PERIPH_SET_CONFIG();
    esp_periph_set_handle_t set = esp_periph_set_init(&periph_cfg);
    TEST_ASSERT_NOT_NULL(set);

#ifdef CONFIG_ESP_LYRAT_MINI_V1_1_BOARD
    gpio_config_t sdcard_pwr_pin_cfg = {
        .pin_bit_mask = 1UL << SDCARD_PWR_CTRL,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    gpio_config(&sdcard_pwr_pin_cfg);
    gpio_set_level(SDCARD_PWR_CTRL, 0);
#endif

    periph_sdcard_cfg_t sdcard_cfg = {
        .card_detect_pin = get_sdcard_intr_gpio(),
        .root = "/sdcard"
    };

    esp_periph_handle_t sdcard_handle = periph_sdcard_init(&sdcard_cfg);
    TEST_ASSERT_NOT_NULL(sdcard_handle);
    TEST_ASSERT_FALSE(esp_periph_start(set, sdcard_handle));

    while (!periph_sdcard_is_mounted(sdcard_handle)) {
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }

    ESP_LOGI(TAG, "Wirte a string to sdcard");
    FILE *fp = fopen("/sdcard/test", "w+");
    TEST_ASSERT_NOT_NULL(fp);
    char test_str[] = "hello, this is sdcard test";
    TEST_ASSERT_EQUAL_INT(strlen(test_str), fwrite(test_str, 1, strlen(test_str), fp));
    TEST_ASSERT_FALSE(fsync(fileno(fp)));
    TEST_ASSERT_FALSE(fclose(fp));

    ESP_LOGI(TAG, "Read a string from sdcard");
    fp = fopen("/sdcard/test", "r");
    TEST_ASSERT_NOT_NULL(fp);
    char *read_str = audio_calloc(1, strlen(test_str) + 1);
    TEST_ASSERT_NOT_NULL(read_str);
    TEST_ASSERT_EQUAL_INT(strlen(test_str), fread(read_str, 1, strlen(test_str), fp));
    ESP_LOGW(TAG, "read string from sdcard file: %s", read_str);
    TEST_ASSERT_EQUAL_INT(0, strncmp(read_str, test_str, strlen(test_str)));

    ESP_LOGI(TAG, "Quit test, release all resources");
    TEST_ASSERT_FALSE(esp_periph_set_stop_all(set));
    TEST_ASSERT_FALSE(esp_periph_set_destroy(set));
    free(read_str);
    fclose(fp);
}

TEST_CASE("sdcard test", "[peripherals]")
{
    periph_sdcard_test();
}

static void periph_spiffs_test(void)
{
    ESP_LOGI(TAG, "Set up peripherals handle");
    esp_periph_config_t periph_cfg = DEFAULT_ESP_PERIPH_SET_CONFIG();
    esp_periph_set_handle_t set = esp_periph_set_init(&periph_cfg);
    TEST_ASSERT_NOT_NULL(set);

    periph_spiffs_cfg_t spiffs_cfg = {
        .root = "/spiffs",
        .max_files = 5,
        .format_if_mount_failed = true,
        .partition_label = NULL
    };
    esp_periph_handle_t spiffs_handle = periph_spiffs_init(&spiffs_cfg);
    TEST_ASSERT_NOT_NULL(spiffs_handle);
    TEST_ASSERT_FALSE(esp_periph_start(set, spiffs_handle));

    while (!periph_spiffs_is_mounted(spiffs_handle)) {
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }

    FILE *fp = fopen("/spiffs/test", "w+");
    TEST_ASSERT_NOT_NULL(fp);
    char test_str[] = "hello, this is spiffs test";
    TEST_ASSERT_EQUAL_INT(strlen(test_str), fwrite(test_str, 1, strlen(test_str), fp));
    TEST_ASSERT_FALSE(fclose(fp));

    ESP_LOGI(TAG, "Read a string from spiffs");
    fp = fopen("/spiffs/test", "r");
    TEST_ASSERT_NOT_NULL(fp);
    char *read_str = audio_calloc(1, strlen(test_str) + 1);
    TEST_ASSERT_NOT_NULL(read_str);
    TEST_ASSERT_EQUAL_INT(strlen(test_str), fread(read_str, 1, strlen(test_str), fp));
    ESP_LOGW(TAG, "read string from spiffs file: %s", read_str);
    TEST_ASSERT_EQUAL_INT(0, strncmp(read_str, test_str, strlen(test_str)));
    fclose(fp);

    ESP_LOGI(TAG, "Quit test, release all resources");
    TEST_ASSERT_FALSE(esp_periph_set_stop_all(set));
    TEST_ASSERT_FALSE(esp_periph_set_destroy(set));
    free(read_str);
}

TEST_CASE("spiffs test", "[peripherals]")
{
    periph_spiffs_test();
}

static void periph_touch_pad_test(void)
{
    ESP_LOGI(TAG, "Set up peripherals handle");
    esp_periph_config_t periph_cfg = DEFAULT_ESP_PERIPH_SET_CONFIG();
    esp_periph_set_handle_t set = esp_periph_set_init(&periph_cfg);
    TEST_ASSERT_NOT_NULL(set);

    ESP_LOGI(TAG, "Register touch pad to peripherals");
    periph_touch_cfg_t touch_cfg = {
        .touch_mask = TOUCH_PAD_SEL4 | TOUCH_PAD_SEL7 | TOUCH_PAD_SEL8 | TOUCH_PAD_SEL9,
        .tap_threshold_percent = 70,
    };
    esp_periph_handle_t touch_handle = periph_touch_init(&touch_cfg);
    TEST_ASSERT_NOT_NULL(touch_handle);
    TEST_ASSERT_FALSE(esp_periph_start(set, touch_handle));

    ESP_LOGI(TAG, "Set up event listener");
    audio_event_iface_cfg_t evt_cfg = AUDIO_EVENT_IFACE_DEFAULT_CFG();
    audio_event_iface_handle_t evt = audio_event_iface_init(&evt_cfg);
    TEST_ASSERT_NOT_NULL(evt);

    ESP_LOGI(TAG, "Listening event from peripherals, please press the touch pad");
    TEST_ASSERT_FALSE(audio_event_iface_set_listener(esp_periph_set_get_event_iface(set), evt));

    while (1) {
        audio_event_iface_msg_t msg;
        TEST_ASSERT_FALSE(audio_event_iface_listen(evt, &msg, portMAX_DELAY));
        if ((int)msg.data == get_input_set_id()) {
            ESP_LOGI(TAG, "[set] touched, quit test");
            break;
        }

        if (msg.cmd == PERIPH_TOUCH_TAP) {
            if ((int)msg.data == get_input_play_id()) {
                ESP_LOGI(TAG, "[play] touched");
            }
            if ((int)msg.data == get_input_volup_id()) {
                ESP_LOGI(TAG, "[vol+] touched");
            }
            if ((int)msg.data == get_input_voldown_id()) {
                ESP_LOGI(TAG, "[vol-] touched");
            }
        }

        if (msg.cmd == PERIPH_TOUCH_RELEASE) {
            if ((int)msg.data == get_input_play_id()) {
                ESP_LOGI(TAG, "[play] released");
            }
            if ((int)msg.data == get_input_volup_id()) {
                ESP_LOGI(TAG, "[vol+] released");
            }
            if ((int)msg.data == get_input_voldown_id()) {
                ESP_LOGI(TAG, "[vol-] released");
            }
        }
    }

    ESP_LOGI(TAG, "Quit test, release all resources");
    TEST_ASSERT_FALSE(esp_periph_set_stop_all(set));
    TEST_ASSERT_FALSE(audio_event_iface_remove_listener(esp_periph_set_get_event_iface(set), evt));
    TEST_ASSERT_FALSE(audio_event_iface_destroy(evt));
    TEST_ASSERT_FALSE(esp_periph_set_destroy(set));
}

static void periph_ws2812_test(void)
{
    ESP_LOGI(TAG, "Set up peripherals handle");
    esp_periph_config_t periph_cfg = DEFAULT_ESP_PERIPH_SET_CONFIG();
    esp_periph_set_handle_t set = esp_periph_set_init(&periph_cfg);
    TEST_ASSERT_NOT_NULL(set);

    periph_ws2812_cfg_t cfg = {
        .gpio_num = GPIO_NUM_3,
        .led_num = 2,
    };
    esp_periph_handle_t handle = periph_ws2812_init(&cfg);
    TEST_ASSERT_FALSE(esp_periph_start(set, handle));

    periph_ws2812_ctrl_cfg_t *control_cfg = malloc(sizeof(periph_ws2812_ctrl_cfg_t) * cfg.led_num);

    control_cfg[0].color = LED2812_COLOR_RED;
    control_cfg[0].mode = PERIPH_WS2812_BLINK;
    control_cfg[0].loop = 50;
    control_cfg[0].time_off_ms = 100;
    control_cfg[0].time_on_ms = 1000;

    control_cfg[1].color = LED2812_COLOR_BLUE;
    control_cfg[1].mode = PERIPH_WS2812_ONE;
    control_cfg[1].loop = 50;
    control_cfg[1].time_off_ms = 2000;
    control_cfg[1].time_on_ms = 2000;

    TEST_ASSERT_FALSE(periph_ws2812_control(handle, control_cfg, NULL));
    vTaskDelay(5000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "Quit test, release all resources");
    TEST_ASSERT_FALSE(esp_periph_set_stop_all(set));
    TEST_ASSERT_FALSE(esp_periph_set_destroy(set));
    free(control_cfg);
}

TEST_CASE("touch pad test", "[peripherals]")
{
    periph_touch_pad_test();
}

static void periph_wifi_test(void)
{
    ESP_LOGI(TAG, "Set up peripherals handle");
    esp_periph_config_t periph_cfg = DEFAULT_ESP_PERIPH_SET_CONFIG();
    esp_periph_set_handle_t set = esp_periph_set_init(&periph_cfg);
    TEST_ASSERT_NOT_NULL(set);

    ESP_LOGI(TAG, "Initialize wifi environment");
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK( nvs_flash_erase() );
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );
    tcpip_adapter_init();

    periph_wifi_cfg_t wifi_cfg = {0};
    wifi_cfg.ssid = "ESP-Audio";
    wifi_cfg.password = "esp123456";

    esp_periph_handle_t wifi_handle = periph_wifi_init(&wifi_cfg);
    esp_periph_start(set, wifi_handle);
    periph_wifi_wait_for_connected(wifi_handle, portMAX_DELAY);

    vTaskDelay(100 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "Quit test, release all resources");
    TEST_ASSERT_FALSE(esp_periph_set_stop_all(set));
    TEST_ASSERT_FALSE(esp_periph_set_destroy(set));
    TEST_ASSERT_FALSE(nvs_flash_deinit());
}

TEST_CASE("wifi test", "[peripherals]")
{
    periph_wifi_test();
}


TEST_CASE("[memory leak test] [gpio isr]", "[peripherals]")
{
    int test_count = TEST_PERIPHERALS_MEMORY_LEAK_TIMES;
    while (test_count--) {
        printf("-------Residual times: %d -------\n", test_count);
        periph_gpio_isr_test();
    }
}

TEST_CASE("[memory leak test] [adc button]", "[peripherals]")
{
    int test_count = TEST_PERIPHERALS_MEMORY_LEAK_TIMES;
    while (test_count--) {
        printf("-------Residual times: %d -------\n", test_count);
        periph_adc_button_test();
    }
}

TEST_CASE("[memory leak test] [console]", "[peripherals]")
{
    int test_count = TEST_PERIPHERALS_MEMORY_LEAK_TIMES;
    while (test_count--) {
        printf("-------Residual times: %d -------\n", test_count);
        periph_console_test();
    }
}

TEST_CASE("[memory leak test] [led]", "[peripherals]")
{
    int test_count = TEST_PERIPHERALS_MEMORY_LEAK_TIMES;
    while (test_count--) {
        printf("-------Residual times: %d -------\n", test_count);
        periph_led_test();
    }
}

TEST_CASE("[memory leak test] [spiffs]", "[peripherals]")
{
    int test_count = TEST_PERIPHERALS_MEMORY_LEAK_TIMES;
    while (test_count--) {
        printf("-------Residual times: %d -------\n", test_count);
        periph_spiffs_test();
    }
}

TEST_CASE("[memory leak test] [wifi]", "[peripherals]")
{
    int test_count = TEST_PERIPHERALS_MEMORY_LEAK_TIMES;
    while (test_count--) {
        printf("-------Residual times: %d -------\n", test_count);
        periph_wifi_test();
    }
}

TEST_CASE("[memory leak test] [gpio button]", "[peripherals]")
{
    int test_count = TEST_PERIPHERALS_MEMORY_LEAK_TIMES;
    while (test_count--) {
        printf("-------Residual times: %d -------\n", test_count);
        periph_gpio_button_test();
    }
}

TEST_CASE("[memory leak test] [is31fl3216]", "[peripherals]")
{
    int test_count = TEST_PERIPHERALS_MEMORY_LEAK_TIMES;
    while (test_count--) {
        printf("-------Residual times: %d -------\n", test_count);
        periph_is31fl3216_test();
    }
}

TEST_CASE("[memory leak test] [sdcard]", "[peripherals]")
{
    int test_count = TEST_PERIPHERALS_MEMORY_LEAK_TIMES;
    while (test_count--) {
        printf("-------Residual times: %d -------\n", test_count);
        periph_sdcard_test();
    }
}

TEST_CASE("[memory leak test] [touch]", "[peripherals]")
{
    int test_count = TEST_PERIPHERALS_MEMORY_LEAK_TIMES;
    while (test_count--) {
        printf("-------Residual times: %d -------\n", test_count);
        periph_touch_pad_test();
    }
}

TEST_CASE("[memory leak test] [ws2812]", "[peripherals]")
{
    int test_count = TEST_PERIPHERALS_MEMORY_LEAK_TIMES;
    while (test_count--) {
        printf("-------Residual times: %d -------\n", test_count);
        periph_ws2812_test();
    }
}

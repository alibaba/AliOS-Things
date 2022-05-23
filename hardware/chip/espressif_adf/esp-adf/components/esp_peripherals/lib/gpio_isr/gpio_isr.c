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
#include "driver/gpio.h"

static const char *TAG = "GPIO_ISR";

esp_err_t gpio_isr_init(int gpio_num, gpio_int_type_t type, gpio_isr_t gpio_isr_handle_func, void *isr_param)
{
    esp_err_t ret = ESP_OK;
    if (gpio_num < 0 || NULL == gpio_isr_handle_func) {
        ESP_LOGE(TAG, "Please check the parameters!");
        return ESP_OK;
    }
    ret |= gpio_set_direction(gpio_num, GPIO_MODE_INPUT);
    ret |= gpio_set_intr_type(gpio_num, type);
    ret |= gpio_isr_handler_add(gpio_num, gpio_isr_handle_func, isr_param);
    ret |= gpio_intr_enable(gpio_num);
    return ret;
}

esp_err_t gpio_isr_deinit(int gpio_num)
{
    esp_err_t ret = ESP_OK;
    if (gpio_num < 0) {
        ESP_LOGE(TAG, "The gpio number should greater than or equal to 0");
        return ESP_FAIL;
    }
    ret |= gpio_isr_handler_remove(gpio_num);
    ret |= gpio_intr_disable(gpio_num);
    return ret;
}

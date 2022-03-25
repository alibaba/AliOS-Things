/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "aos_hal_adc.h"
#include "driver/gpio.h"
#include "driver/adc.h"

uint8_t adc_bit_width;
uint8_t adc_atten_value;

typedef enum {
    ESP32_GPIO_NUM_32 = 32, /*!< GPIO32, input and output */
    ESP32_GPIO_NUM_33 = 33, /*!< GPIO33, input and output */
    ESP32_GPIO_NUM_34 = 34, /*!< GPIO34, input mode only */
    ESP32_GPIO_NUM_35 = 35, /*!< GPIO35, input mode only */
    ESP32_GPIO_NUM_36 = 36, /*!< GPIO36, input mode only */
    ESP32_GPIO_NUM_37 = 37, /*!< GPIO37, input mode only */
    ESP32_GPIO_NUM_38 = 38, /*!< GPIO38, input mode only */
    ESP32_GPIO_NUM_39 = 39, /*!< GPIO39, input mode only */
} aos_gpio_num_t;

typedef enum {
    ESP32_ADC1_CHANNEL_0 = 0, /*!< ADC1 channel 0 is GPIO1  */
    ESP32_ADC1_CHANNEL_1, /*!< ADC1 channel 1 is GPIO2  */
    ESP32_ADC1_CHANNEL_2, /*!< ADC1 channel 2 is GPIO3  */
    ESP32_ADC1_CHANNEL_3, /*!< ADC1 channel 3 is GPIO4  */
    ESP32_ADC1_CHANNEL_4, /*!< ADC1 channel 4 is GPIO5  */
    ESP32_ADC1_CHANNEL_5, /*!< ADC1 channel 5 is GPIO6  */
    ESP32_ADC1_CHANNEL_6, /*!< ADC1 channel 6 is GPIO7  */
    ESP32_ADC1_CHANNEL_7, /*!< ADC1 channel 7 is GPIO8  */
    ESP32_ADC1_CHANNEL_8, /*!< ADC1 channel 8 is GPIO9  */
    ESP32_ADC1_CHANNEL_9, /*!< ADC1 channel 9 is GPIO10 */
} aos_adc1_channel_t;


uint8_t aos_hal_pin_port_change(uint8_t port)
{
    uint8_t gpio_num = 0;
    switch (port) {
    case ESP32_ADC1_CHANNEL_0:
        gpio_num = ESP32_GPIO_NUM_36;
        break;

    case ESP32_ADC1_CHANNEL_1:
        gpio_num = ESP32_GPIO_NUM_37;
        break;

    case ESP32_ADC1_CHANNEL_2:
        gpio_num = ESP32_GPIO_NUM_38;
        break;

    case ESP32_ADC1_CHANNEL_3:
        gpio_num = ESP32_GPIO_NUM_39;
        break;

    case ESP32_ADC1_CHANNEL_4:
        gpio_num = ESP32_GPIO_NUM_32;
        break;

    case ESP32_ADC1_CHANNEL_5:
        gpio_num = ESP32_GPIO_NUM_33;
        break;

    case ESP32_ADC1_CHANNEL_6:
        gpio_num = ESP32_GPIO_NUM_34;
        break;

    case ESP32_ADC1_CHANNEL_7:
        gpio_num = ESP32_GPIO_NUM_35;
        break;

    default:
        break;
    }

    return gpio_num;
}

uint8_t aos_hal_find_adc_channel(uint8_t gpio_num)
{
    uint8_t channel = 0;
    switch (gpio_num) {
    case ESP32_GPIO_NUM_32:
        channel = ESP32_ADC1_CHANNEL_4;
        break;

    case ESP32_GPIO_NUM_33:
        channel = ESP32_ADC1_CHANNEL_5;
        break;

    case ESP32_GPIO_NUM_34:
        channel = ESP32_ADC1_CHANNEL_6;
        break;

    case ESP32_GPIO_NUM_35:
        channel = ESP32_ADC1_CHANNEL_7;
        break;

    case ESP32_GPIO_NUM_36:
        channel = ESP32_ADC1_CHANNEL_0;
        break;

    case ESP32_GPIO_NUM_37:
        channel = ESP32_ADC1_CHANNEL_1;
        break;

    case ESP32_GPIO_NUM_38:
        channel = ESP32_ADC1_CHANNEL_2;
        break;

    case ESP32_GPIO_NUM_39:
        channel = ESP32_ADC1_CHANNEL_3;
        break;

    default:
        break;
    }

    return channel;
}

int32_t aos_hal_adc_init(adc_dev_t *adc)
{
    int initialized = 0;
    uint8_t channel = 0;
    if (!initialized) {
        // set adc config width
        if (adc->config.adc_width >= 0 && adc->config.adc_width <= 3) {
            adc1_config_width(adc->config.adc_width);
        } else {
            adc->config.adc_width = ADC_WIDTH_BIT_12;
            adc1_config_width(ADC_WIDTH_BIT_12);
        }
        adc_bit_width = adc->config.adc_width;
        initialized = 1;
    }

    // uint8_t pin_id = adc->port;
    // channel = aos_hal_find_adc_channel(pin_id);
    channel = adc->port;
    // use ADC atten 0db
    adc_atten_value = adc->config.adc_atten;
    esp_err_t ret = adc1_config_channel_atten(channel, adc_atten_value);
    if (ret != ESP_OK) {
        printf("parameter error");
    }

    return ret;
}

int32_t aos_hal_adc_voltage_value_get(adc_dev_t *adc, uint32_t *output, uint32_t timeout)
{
    int val = 0;
    int vout = 0;
    int vmax = 1100;
    int dmax = 4095;
    uint8_t channel = 0;

    // uint8_t pin_id = adc->port;
    // channel = aos_hal_find_adc_channel(pin_id);
    channel = adc->port;

    val = adc1_get_raw(channel);
    if (val == -1) {
        printf("parameter error\r\n");
    }

    // calc v value
    // Vout = Dout * Vmax /Dmax
    // https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/adc.html
    // atten value
    // ADC_ATTEN_DV_0      100mV ~ 950mV   MAX 1100mV
    // ADC_ATTEN_DV_2_5    100mV ~ 1250mV  MAX 1500mV
    // ADC_ATTEN_DV_6      150mV ~ 1750mV  MAX 2200mV
    // ADC_ATTEN_DV_11     150mV ~ 2450mV  MAX 3900mV
    // ESP32 Dmax = 4095
    switch (adc_atten_value) {
    case ADC_ATTEN_0db:
        vmax = 1100;
        break;
    case ADC_ATTEN_2_5db:
        vmax = 1500;
        break;
    case ADC_ATTEN_6db:
        vmax = 2200;
        break;
    case ADC_ATTEN_11db:
        vmax = 3900;
        break;
    default:
        vmax = 1100;
        break;
    }

    switch (adc_bit_width) {
#if CONFIG_IDF_TARGET_ESP32
    case ADC_WIDTH_BIT_9:
        dmax = 511;
        break;
    case ADC_WIDTH_BIT_10:
        dmax = 1023;
        break;
    case ADC_WIDTH_BIT_11:
        dmax = 2047;
        break;
    case ADC_WIDTH_BIT_12:
        dmax = 4095;
        break;
#endif
    default:
        dmax = 4095;
        break;
    }

    vout = val * vmax / dmax;
    *(uint32_t *)output = vout;

    return 0;
}

int32_t aos_hal_adc_raw_value_get(adc_dev_t *adc, uint32_t *output, uint32_t timeout)
{
    int val = 0;
    uint8_t channel = 0;

    // uint8_t pin_id = adc->port;
    // channel = aos_hal_find_adc_channel(pin_id);
    channel = adc->port;

    val = adc1_get_raw(channel);
    if (val == -1) {
        printf("parameter error\r\n");
    }

    *(uint32_t *)output = val;

    return 0;
}

// esp32 can set atten and width to change adc value
// use board config to setting

// typedef struct {
//     uint8_t atten;
//     uint8_t width;
// } adc_param_t;

// int32_t aos_hal_adc_para_chg(adc_dev_t *adc, adc_param_t para)
// {
//     esp_err_t ret = 0;
//     uint8_t channel = 0;

//     adc_atten_t atten = para.atten;
//     uint8_t pin_id = adc->port;
//     channel = aos_hal_find_adc_channel(pin_id);

//     ret = adc1_config_channel_atten(channel, atten);
//     if (ret != ESP_OK) {
//         printf("parameter error, adc config atten failed\r\n");
//         return ret;
//     }

//     adc_bits_width_t width = para.width;
//     ret = adc1_config_width(width);
//     if (ret != ESP_OK) {
//         printf("parameter error, adc config width failed\r\n");
//         return ret;
//     }

//     switch (width) {
//         case ADC_WIDTH_9Bit:
//             adc_bit_width = 9;
//             break;
//         case ADC_WIDTH_10Bit:
//             adc_bit_width = 10;
//             break;
//         case ADC_WIDTH_11Bit:
//             adc_bit_width = 11;
//             break;
//         case ADC_WIDTH_12Bit:
//             adc_bit_width = 12;
//             break;
//         default:
//             break;
//     }

//     return ret;
// }

int32_t aos_hal_adc_finalize(adc_dev_t *adc)
{
    int32_t ret = 0;
    return ret;
}

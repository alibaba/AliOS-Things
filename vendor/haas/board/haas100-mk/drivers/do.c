/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include "aos/kernel.h"
#include "ulog/ulog.h"
#include "do.h"
#include "hal_iomux_haas1000.h"

#define TAG "ex_do"

typedef struct {
    uint8_t         installed;
    gpio_dev_t      gpio_dev;
} gpio_dev_ouput_t;

/*digital input gpio dev list , the default value is high*/
static gpio_dev_ouput_t gpio_dev_output[DO_PORT_SIZE] = {
    {0, {HAL_IOMUX_PIN_P2_7, OUTPUT_PUSH_PULL, NULL}},
    {0, {HAL_IOMUX_PIN_P2_6, OUTPUT_PUSH_PULL, NULL}},
    {0, {HAL_IOMUX_PIN_P4_0, OUTPUT_PUSH_PULL, NULL}},
    {0, {HAL_IOMUX_PIN_P4_1, OUTPUT_PUSH_PULL, NULL}},
};

int32_t expansion_board_do_init(void)
{
    int32_t ret = 0;
    uint32_t i = 0;

    /*init digital input*/
    for (i = 0; i < DO_PORT_SIZE; i++) {
        ret = hal_gpio_init(&gpio_dev_output[i].gpio_dev);
        if (ret) {
            LOGE(TAG, "do %d pin %d init fail ret", i, gpio_dev_output[i].gpio_dev.port, ret);
            return -1;
        }

        /*init status do should output low, depends on pd requires */
        ret = hal_gpio_output_high(&gpio_dev_output[i].gpio_dev);
        if (ret) {
            LOGE(TAG, "%s %d port %d set low fail , ret %d", __FUNCTION__, __LINE__, i, ret);
            return -1;
        }
        gpio_dev_output[i].installed = 1;

    }

    return 0;
}

int32_t expansion_board_do_high(uint8_t port)
{
    int32_t ret = -1;

    if (port >= DO_PORT_SIZE) {
        LOGE(TAG, "%s %d invalid input port %d", __FUNCTION__, __LINE__, port);
        return -1;
    }

    if (gpio_dev_output[port].installed == 0) {
        LOGE(TAG, "%s %d port %d haven't init yet", __FUNCTION__, __LINE__, port);
        return -1;
    }

    /*do output high ,gpio should pull down */
    ret = hal_gpio_output_low(&gpio_dev_output[port].gpio_dev);
    if (ret) {
        LOGE(TAG, "%s %d port %d set high fail , ret %d", __FUNCTION__, __LINE__, port, ret);
        return -1;
    }

    return 0;
}

int32_t expansion_board_do_low(uint8_t port)
{
    int32_t ret = -1;

    if (port >= DO_PORT_SIZE) {
        LOGE(TAG, "%s %d invalid input port %d", __FUNCTION__, __LINE__, port);
        return -1;
    }

    if (gpio_dev_output[port].installed == 0) {
        LOGE(TAG, "%s %d port %d haven't init yet", __FUNCTION__, __LINE__, port);
        return -1;
    }

    /*do output high ,gpio should pull up */
    ret = hal_gpio_output_high(&gpio_dev_output[port].gpio_dev);
    if (ret) {
        LOGE(TAG, "%s %d port %d set low fail , ret %d", __FUNCTION__, __LINE__, port, ret);
        return -1;
    }

    return 0;
}
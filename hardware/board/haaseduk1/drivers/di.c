/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#include "di.h"
#include "aos/kernel.h"
#include "hal_iomux_haas1000.h"
#include "ulog/ulog.h"
#include <stdio.h>

#define TAG "ex_di"

#define TIMER_CHECK_INTERVAL 10
#define DI_STABLE_COUNT      5

digital_input_value_change_notify g_di_notify_cb = NULL;

typedef struct {
    uint8_t         installed;
    uint8_t         monitor_flag;
    uint8_t         check_count;
    gpio_pinstate_t exactly_level;
    gpio_dev_t      gpio_dev;
} gpio_dev_input_t;

/*digital input gpio dev list , the default value is high*/
static gpio_dev_input_t gpio_dev_input[DI_PORT_SIZE] = {
    {0, 0, 0, GPIO_PinState_Set, {HAL_IOMUX_PIN_P2_4, IRQ_MODE, NULL} },
    {0, 0, 0, GPIO_PinState_Set, {HAL_IOMUX_PIN_P2_5, IRQ_MODE, NULL} },
    {0, 0, 0, GPIO_PinState_Set, {HAL_IOMUX_PIN_P4_7, IRQ_MODE, NULL} },
    {0, 0, 0, GPIO_PinState_Set, {HAL_IOMUX_PIN_P4_6, IRQ_MODE, NULL} },
};

static aos_timer_t st_di_check_timer = {0};

static int32_t get_di_port_by_iomux(uint8_t iomux, uint32_t *diport)
{
    uint32_t i = 0;

    for (i = 0; i < DI_PORT_SIZE; i++) {
        if (gpio_dev_input[i].gpio_dev.port == iomux) {
            *diport = i;
            return 0;
        }
    }

    return -1;
}

static void di_interrup_proc(void *arg)
{
    uint8_t  port   = 0;
    uint32_t diport = 0;
    int32_t  ret    = 0;
    uint32_t value  = 0;

    if (NULL == arg) {
        LOGE(TAG, "Invalid input %s %d", __FILE__, __LINE__);
        return;
    }

    port = *(uint8_t *)arg;
    ret  = get_di_port_by_iomux(port, &diport);
    if (ret) {
        LOGE(TAG, "can not porc iomux %d interrupt", port);
        return;
    }

    if (gpio_dev_input[diport].installed == 0) {
        LOGE(TAG, "di port %d iomux %d haven't init yet", diport, port);
        return;
    }

    ret = hal_gpio_input_get(&(gpio_dev_input[diport].gpio_dev), &value);
    if (ret) {
        LOGE(TAG, "Fail to get di port %d input value ret %d", diport, value);
        return;
    }
    hal_gpio_clear_irq(&gpio_dev_input[diport].gpio_dev);
    hal_gpio_disable_irq(&gpio_dev_input[diport].gpio_dev);
    if (value != gpio_dev_input[diport].exactly_level) {
        /*add the port to the monitorlist list*/
        // LOGI(TAG,"add di %d to monitor list", diport);
        gpio_dev_input[diport].check_count  = 0;
        gpio_dev_input[diport].monitor_flag = 1;
    }

    if (value == GPIO_PinState_Set) {
        /*rising edge interrupt, so enable falling interrupt */
        // LOGI(TAG, "port %d recv a rising edge interrupt and enable the
        // falling edge", diport);
        ret = hal_gpio_enable_irq(&gpio_dev_input[diport].gpio_dev,
                                  IRQ_TRIGGER_FALLING_EDGE, di_interrup_proc,
                                  &gpio_dev_input[diport].gpio_dev.port);
    }

    if (value == GPIO_PinState_Reset) {
        /*falling edge interrupt*/
        // LOGI(TAG, "port %d recv a falling edge interrupt and enable the
        // rising edge", diport);
        ret = hal_gpio_enable_irq(&gpio_dev_input[diport].gpio_dev,
                                  IRQ_TRIGGER_RISING_EDGE, di_interrup_proc,
                                  &gpio_dev_input[diport].gpio_dev.port);
    }

    if (ret) {
        LOGE(TAG, "Fail to enable gpio interrupt , gpio value is %d, ret %d",
             value, ret);
    }
    return;
}

static void di_value_check(void *timer, void *arg)
{
    uint32_t i;
    int32_t  ret        = 0;
    uint32_t gpio_value = 0;

    for (i = 0; i < DI_PORT_SIZE; i++) {
        if (gpio_dev_input[i].installed == 0 ||
            gpio_dev_input[i].monitor_flag == 0) {
            continue;
        }
        ret = hal_gpio_input_get(&gpio_dev_input[i].gpio_dev, &gpio_value);
        if (ret) {
            LOGE(TAG, "Fail to get di %d port %d value at %s %d", i,
                 gpio_dev_input[i].gpio_dev.port, __FILE__, __LINE__);
            continue;
        }
        if (gpio_value == gpio_dev_input[i].exactly_level) {
            /*remove it from standout monitor list*/
            gpio_dev_input[i].monitor_flag = 0;
            gpio_dev_input[i].check_count  = 0;
        } else {
            gpio_dev_input[i].check_count++;
            if (gpio_dev_input[i].check_count >= DI_STABLE_COUNT) {
                gpio_dev_input[i].monitor_flag  = 0;
                gpio_dev_input[i].exactly_level = gpio_value;
                gpio_dev_input[i].check_count   = 0;
                LOGI(TAG, "di %d changes to value %d", i, gpio_value);
                /*notify the gpio value change info */
                if (NULL != g_di_notify_cb) {
                    ret = g_di_notify_cb(i, gpio_value);
                    if (ret) {
                        LOGE(TAG, "Fail to notify di %d value changes to %d", i,
                             gpio_value);
                    }
                }
            }
        }
    }
}

int32_t expansion_board_di_init()
{
    int32_t  ret        = 0;
    uint32_t i          = 0;
    uint32_t gpio_value = GPIO_PinState_Set;

    /*init digital input*/
    for (i = 0; i < DI_PORT_SIZE; i++) {
        ret = hal_gpio_init(&gpio_dev_input[i].gpio_dev);
        if (ret) {
            LOGE(TAG, "di %d pin %d init fail ret", i,
                 gpio_dev_input[i].gpio_dev.port, ret);
            return -1;
        }

        ret = hal_gpio_input_get(&gpio_dev_input[i].gpio_dev, &gpio_value);
        if (ret) {
            LOGE(TAG, "di %d pin %d fail to get value, ret %d", i,
                 gpio_dev_input[i].gpio_dev.port, ret);
            return -1;
        }
        LOGI(TAG, "di %d init value is %d", i, gpio_value);

        /*for haas1000 doesn't support both edge trigger*/
        if (gpio_value == GPIO_PinState_Set) {
            ret = hal_gpio_enable_irq(
                &gpio_dev_input[i].gpio_dev, IRQ_TRIGGER_FALLING_EDGE,
                di_interrup_proc, &gpio_dev_input[i].gpio_dev.port);
        } else {
            ret = hal_gpio_enable_irq(&gpio_dev_input[i].gpio_dev,
                                      IRQ_TRIGGER_RISING_EDGE, di_interrup_proc,
                                      &gpio_dev_input[i].gpio_dev.port);
        }

        if (ret) {
            LOGE(TAG, "di %d pin %d fail enable irq ret %d", i,
                 gpio_dev_input[i].gpio_dev.port, ret);
            return -1;
        }
        gpio_dev_input[i].installed     = 1;
        gpio_dev_input[i].exactly_level = gpio_value;
    }

    /*init the gpio check timer, check gpio value every 10ms */
    ret = aos_timer_new_ext(&st_di_check_timer, di_value_check, NULL,
                            TIMER_CHECK_INTERVAL, 1, 1);
    if (ret) {
        LOGE(TAG, "Fail to new gpio value check timer ret 0x%x", ret);
        for (i = 0; i < DI_PORT_SIZE; i++) {
            hal_gpio_disable_irq(&gpio_dev_input[i].gpio_dev);
            hal_gpio_finalize(&gpio_dev_input[i].gpio_dev);
            gpio_dev_input[i].installed = 0;
        }
        return -1;
    }

    return 0;
}

int32_t expansion_board_di_get_value(uint8_t port, gpio_pinstate_t *value)
{
    int32_t ret = 0;

    if (port >= DI_PORT_SIZE || NULL == value) {
        LOGE(TAG, " %s %d Invalid input port %d", __FILE__, __LINE__, port);
        return -1;
    }

    if (gpio_dev_input[port].installed == 0) {
        LOGE(TAG, "DI port %d haven't init yet", port);
        return -1;
    }

    *value = gpio_dev_input[port].exactly_level;
    return 0;
}

void expansion_board_di_change_notify_register(
    digital_input_value_change_notify cb)
{
    if (NULL == cb) {
        LOGE(TAG, " %s %d Invalid input", __FILE__, __LINE__);
        return;
    }
    g_di_notify_cb = cb;
}

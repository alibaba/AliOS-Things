/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "hal/soc/soc.h"
#include "mico_rtos.h"

typedef uint32_t UINT32;
#define VOID void
#define ASSERT(exp)

#include "gpio.h"
#include "gpio_pub.h"

typedef enum
{
    GIRQ_TRIGGER_LOW_LEVEL = 0x0,	/* Interrupt triggered at input signal's LOW LEVEL  */
    GIRQ_TRIGGER_HGIH_LEVEL = 0x1,	/* Interrupt triggered at input signal's HIGH LEVEL  */
    GIRQ_TRIGGER_RISING_EDGE  = 0x2, /* Interrupt triggered at input signal's rising edge  */
    GIRQ_TRIGGER_FALLING_EDGE = 0x3 /* Interrupt triggered at input signal's falling edge */
};

typedef struct
{
    void *arg;
    gpio_irq_handler_t handler;
} gpio_int_hdl;

static gpio_int_hdl gpio_int_hdls[ 22 ];

static const uint8_t gpio_map[] =
{
    [1] = 30,
    [2] = 29,
    [3] = 17,
    [4] = 16,
    [5] = 14,
    [6] = 15,
    [7] = 0,
    [8] = 1,
    [11] = 20,
    [12] = 4,
    [17] = 23,
    [18] = 22,
    [19] = 11,
    [20] = 10,
    [21] = 21,
};

static const uint8_t config_map[] =
{
    [INPUT_PULL_UP] = GMODE_INPUT_PULLUP,
    [INPUT_PULL_DOWN] = GMODE_INPUT_PULLDOWN,
    [INPUT_HIGH_IMPEDANCE] = GMODE_INPUT,
    [OUTPUT_PUSH_PULL] = GMODE_OUTPUT,
};

static const uint8_t int_edge_map[] =
{
    [IRQ_TRIGGER_RISING_EDGE] = GIRQ_TRIGGER_RISING_EDGE,
    [IRQ_TRIGGER_FALLING_EDGE] = GIRQ_TRIGGER_FALLING_EDGE,
};

int32_t hal_gpio_init(gpio_dev_t *gpio)
{
    uint32_t param;

    if((gpio->config == OUTPUT_OPEN_DRAIN_NO_PULL) || (gpio->config == OUTPUT_OPEN_DRAIN_PULL_UP))
        return -1;

    param = GPIO_CFG_PARAM(gpio_map[gpio->port], config_map[gpio->config]);
    gpio_ctrl(CMD_GPIO_CFG, &param);

    return 0;
}

int32_t hal_gpio_finalize(gpio_dev_t *gpio)
{
    uint32_t param;

    param = GPIO_CFG_PARAM(gpio_map[gpio->port], config_map[INPUT_HIGH_IMPEDANCE]);
    gpio_ctrl(CMD_GPIO_CFG, &param);

    return 0;
}

int32_t hal_gpio_output_high(gpio_dev_t *gpio)
{
    uint32_t param;

    param = GPIO_OUTPUT_PARAM(gpio_map[gpio->port], 1);
    gpio_ctrl(CMD_GPIO_OUTPUT, &param);

    return 0;
}

int32_t hal_gpio_output_low(gpio_dev_t *gpio)
{
    uint32_t param;

    param = GPIO_OUTPUT_PARAM(gpio_map[gpio->port], 0);
    gpio_ctrl(CMD_GPIO_OUTPUT, &param);

    return 0;
}

int32_t hal_gpio_output_toggle(gpio_dev_t *gpio)
{
    uint32_t param;

    param = gpio_map[gpio->port];
    gpio_ctrl(CMD_GPIO_OUTPUT_REVERSE, &param);

    return 0;
}

int32_t hal_gpio_input_get(gpio_dev_t *gpio, uint32_t *value)
{
    uint32_t param;

    param = gpio_map[gpio->port];
   *value = gpio_ctrl(CMD_GPIO_INPUT, &param);
    return 0;
}

static gpio_int_cb( uint8_t i )
{
    void *arg = gpio_int_hdls[i].arg;
    gpio_irq_handler_t cb_fn = gpio_int_hdls[i].handler;

    if(cb_fn != NULL)
        cb_fn(arg);
}

int32_t hal_gpio_enable_irq(gpio_dev_t *gpio, gpio_irq_trigger_t trigger,
                                     gpio_irq_handler_t handler, void *arg)
{
    GPIO_INT_ST int_struct;

    if(trigger == IRQ_TRIGGER_BOTH_EDGES)
        return -1;

    int_struct.id = gpio_map[gpio->port];
    int_struct.mode = int_edge_map[trigger];
    int_struct.phandler = gpio_int_cb;
    gpio_int_hdls[ int_struct.id ].arg = arg;
    gpio_int_hdls[ int_struct.id ].handler = handler;

    gpio_ctrl(CMD_GPIO_INT_ENABLE, &int_struct);

    return 0;
}

int32_t hal_gpio_disable_irq(gpio_dev_t *gpio)
{
    uint32_t param;

    param = gpio_map[gpio->port];

    gpio_ctrl(CMD_GPIO_INT_DISABLE, &param);

    return 0;
}

int32_t hal_gpio_clear_irq(gpio_dev_t *gpio)
{
    uint32_t param;

    param = gpio_map[gpio->port];

    gpio_ctrl(CMD_GPIO_INT_CLEAR, &param);

    return 0;
}

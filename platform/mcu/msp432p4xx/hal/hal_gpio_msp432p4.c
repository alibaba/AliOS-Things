/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>

#include "aos/hal/gpio.h"

#include <k_api.h>
#include <aos/kernel.h>

/* Board Header files */
#include "Board.h"

/* Driver Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/gpio/GPIOMSP432.h>



extern const GPIOMSP432_Config GPIOMSP432_config;


static int32_t hal_gpio_data_transform(gpio_dev_t *gpio, PinConfig *pinconfig)
{
    if (NULL == gpio || NULL == pinconfig) {
        return -1;
    }

    switch (gpio->config) {
        case ANALOG_MODE:
            printf("GPIO analog mode is not support on msp432p4\r\n");
            return -1;
        case IRQ_MODE:
            switch (*(gpio_irq_trigger_t *)gpio->priv) {
                case IRQ_TRIGGER_RISING_EDGE:
                    pinconfig->config = 0;
                    pinconfig->config |= GPIO_CFG_INPUT | GPIO_CFG_IN_INT_RISING;
                    break;
                case IRQ_TRIGGER_FALLING_EDGE:
                    pinconfig->config = 0;
                    pinconfig->config |= GPIO_CFG_INPUT | GPIO_CFG_IN_INT_FALLING;
                    break;
                case IRQ_TRIGGER_BOTH_EDGES:
                    pinconfig->config = 0;
                    pinconfig->config |= GPIO_CFG_INPUT | GPIO_CFG_IN_INT_BOTH_EDGES;
                    break;
                default:
                    printf("gpio invalid irq_mode %d \r\n", gpio->priv);
                    return -1;
            }
            break;
        case INPUT_PULL_UP:
            pinconfig->config = 0;
            pinconfig->config |= GPIO_CFG_IN_PU;
            break;
        case INPUT_PULL_DOWN:
            pinconfig->config = 0;
            pinconfig->config |= GPIO_CFG_IN_PD;
            break;
        case INPUT_HIGH_IMPEDANCE:
            printf("GPIO INPUT_HIGH_IMPEDANCE mode is not support on msp432p4\r\n");
            return -1;
        case OUTPUT_PUSH_PULL:
            pinconfig->config = 0;
            pinconfig->config |= GPIO_CFG_OUT_STD;
            break;
        case OUTPUT_OPEN_DRAIN_NO_PULL:
            pinconfig->config = 0;
            pinconfig->config |= GPIO_CFG_OUT_OD_NOPULL;
            break;
        case OUTPUT_OPEN_DRAIN_PULL_UP:
            pinconfig->config = 0;
            pinconfig->config |= GPIO_CFG_OUT_OD_PU;
            break;
        default:
            printf("invalid gpio config %d at %s %d \r\n",
                gpio->config, __func__, __LINE__);
            return -1;
    }
    return 0;
}

int32_t hal_gpio_output_high(gpio_dev_t *gpio)
{
    PinConfig *pinconfig;
    
    if (gpio == NULL) {
        printf("invalid input para at %s %d \r\n", __FILE__, __LINE__);
        return -1;
    }

    if (gpio->port >= GPIOMSP432_config.numberOfPinConfigs) {
        printf("gpio->port is invalid %d at %s %d \r\n", gpio->port, __FILE__, __LINE__);
        return -1;
    }

    pinconfig = (PinConfig *) &GPIOMSP432_config.pinConfigs[gpio->port];
    if ((pinconfig->config & GPIO_CFG_INPUT) == GPIO_CFG_INPUT) {
        printf("fail to set output low for the pin is input mode \r\n");
        return -1;
    }

    GPIO_write(gpio->port, 1);

    return 0;
}


int32_t hal_gpio_output_low(gpio_dev_t *gpio)
{
    PinConfig *pinconfig;
    
    if (gpio == NULL) {
        printf("invalid input para at %s %d \r\n", __FILE__, __LINE__);
        return -1;
    }

    if (gpio->port >= GPIOMSP432_config.numberOfPinConfigs) {
        printf("gpio->port is invalid %d at %s %d \r\n", gpio->port, __FILE__, __LINE__);
        return -1;
    }

    pinconfig = (PinConfig *) &GPIOMSP432_config.pinConfigs[gpio->port];
    if ((pinconfig->config & GPIO_CFG_INPUT) == GPIO_CFG_INPUT) {
        printf("fail to set output low for the pin is input mode \r\n");
        return -1;
    }

    GPIO_write(gpio->port, 0);

    return 0;
}

int32_t hal_gpio_output_toggle(gpio_dev_t *gpio)
{
    PinConfig *pinconfig;
    
    if (gpio == NULL) {
        printf("invalid input para at %s %d \r\n", __FILE__, __LINE__);
        return -1;
    }

    if (gpio->port >= GPIOMSP432_config.numberOfPinConfigs) {
        printf("gpio->port is invalid %d at %s %d \r\n", gpio->port, __FILE__, __LINE__);
        return -1;
    }

    pinconfig = (PinConfig *) &GPIOMSP432_config.pinConfigs[gpio->port];
    if ((pinconfig->config & GPIO_CFG_INPUT) == GPIO_CFG_INPUT) {
        printf("fail to set output toggle for the pin is input mode \r\n");
        return -1;
    }
    
    GPIO_toggle(gpio->port);

    return 0;
}


int32_t hal_gpio_input_get(gpio_dev_t *gpio, uint32_t *value)
{
    if (gpio == NULL || NULL == value) {
        printf("invalid input para at %s %d \r\n", __FILE__, __LINE__);
        return -1;
    }

    if (gpio->port >= GPIOMSP432_config.numberOfPinConfigs) {
        printf("gpio->port is invalid %d at %s %d \r\n", gpio->port, __FILE__, __LINE__);
        return -1;
    }

    *value = GPIO_read(gpio->port);

    return 0;
}

void hal_gpio_total_init()
{
    GPIO_init();
    return ;
}

int32_t hal_gpio_enable_irq(gpio_dev_t *gpio, gpio_irq_trigger_t trigger,gpio_irq_handler_t handler, void *arg)
{
    int32_t ret = -1;

    if (NULL == gpio) {
        printf("invalid input %s \r\n", __func__);
        return ret;
    }

    if (gpio->port >= GPIOMSP432_config.numberOfPinConfigs) {
        printf("gpio->port is invalid %d at %s %d \r\n", gpio->port, __FILE__, __LINE__);
        return -1;
    }

    GPIO_enableInt(gpio->port);
    return 0;
}

int32_t hal_gpio_disable_irq(gpio_dev_t *gpio)
{
    int32_t ret = -1;

    if (NULL == gpio) {
        printf("invalid input %s \r\n", __func__);
        return ret;
    }

    if (gpio->port >= GPIOMSP432_config.numberOfPinConfigs) {
        printf("gpio->port is invalid %d at %s %d \r\n", gpio->port, __FILE__, __LINE__);
        return -1;
    }

    GPIO_disableInt(gpio->port);
    return 0;
}

int32_t hal_gpio_clear_irq(gpio_dev_t *gpio)
{
    int32_t ret = -1;

    if (NULL == gpio) {
        printf("invalid input %s \r\n", __func__);
        return ret;
    }

    if (gpio->port >= GPIOMSP432_config.numberOfPinConfigs) {
        printf("gpio->port is invalid %d at %s %d \r\n", gpio->port, __FILE__, __LINE__);
        return -1;
    }

    GPIO_clearInt(gpio->port);
    return 0;

}


int32_t hal_gpio_init(gpio_dev_t *gpio)
{
    PinConfig     *config = NULL;
    int32_t       ret = 0;
    
    if (NULL == gpio) {
        printf("invalid input para at %s %d \r\n", __FILE__, __LINE__);
        return -1;
    }

    if (gpio->port >= GPIOMSP432_config.numberOfPinConfigs) {
        printf("gpio->port is invalid %d at %s %d \r\n", gpio->port, __FILE__, __LINE__);
        return -1;
    }

    config = (PinConfig *) &GPIOMSP432_config.pinConfigs[gpio->port];
    ret = hal_gpio_data_transform(gpio, config);
    if (ret) {
        printf("gpio init fail at %s %d ret is %d \r\n", __func__, __LINE__, ret);
        return ret;
    }

    ret = GPIO_setConfig(gpio->port, GPIOMSP432_config.pinConfigs[gpio->port]);
    if (ret) {
        printf("gpio init fail at %s %d ret is %d \r\n", __func__, __LINE__, ret);
        return ret;
    }

    return 0;
}



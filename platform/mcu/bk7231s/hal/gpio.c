/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "hal/soc/soc.h"
#include "rtos_pub.h"
#include "gpio_pub.h"

typedef struct
{
    void *arg;
    gpio_irq_handler_t handler;
} gpio_int_hdl;

static gpio_int_hdl gpio_int_hdls[GPIONUM];

int32_t hal_gpio_init(gpio_dev_t *gpio)
{
	int ret = 0;
    uint32_t param, mode;

	switch(gpio->config)
	{
		case ANALOG_MODE:
			mode = GMODE_SECOND_FUNC;
			break;
		case INPUT_PULL_UP:
			mode = GMODE_INPUT_PULLUP;
			break;
		case INPUT_PULL_DOWN:
			mode = GMODE_INPUT_PULLDOWN;
			break;
		case INPUT_HIGH_IMPEDANCE:
			mode = GMODE_INPUT;
			break;
		case OUTPUT_PUSH_PULL:
			mode = GMODE_OUTPUT;
			break;
		default:
			ret = -1;
			break;
	}

	if(ret == 0)
	{
		param = GPIO_CFG_PARAM(gpio->port, mode);
		sddev_control(GPIO_DEV_NAME, CMD_GPIO_CFG, &param);
	}
	
    return ret;
}

int32_t hal_gpio_finalize(gpio_dev_t *gpio)
{
    uint32_t param;

    param = GPIO_CFG_PARAM(gpio->port, GMODE_INPUT);
	sddev_control(GPIO_DEV_NAME, CMD_GPIO_CFG, &param);

    return 0;
}

int32_t hal_gpio_output_high(gpio_dev_t *gpio)
{
	bk_gpio_output(gpio->port, 1);
	
    return 0;
}

int32_t hal_gpio_output_low(gpio_dev_t *gpio)
{
	bk_gpio_output(gpio->port, 0);

    return 0;
}

int32_t hal_gpio_output_toggle(gpio_dev_t *gpio)
{
    uint32_t param;

    param = gpio->port;
	sddev_control(GPIO_DEV_NAME, CMD_GPIO_OUTPUT_REVERSE, &param);

    return 0;
}

int32_t hal_gpio_input_get(gpio_dev_t *gpio, uint32_t *value)
{
   *value = bk_gpio_input(gpio->port);
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
	int ret = 0;
    GPIO_INT_ST int_struct;

	if(trigger == IRQ_TRIGGER_RISING_EDGE)
	{
		int_struct.mode = GPIO_INT_LEVEL_RISING;
	}
	else if(trigger == IRQ_TRIGGER_FALLING_EDGE)
	{
		int_struct.mode = GPIO_INT_LEVEL_FALLING;
	}
	else
	{
		ret = -1;
	}

	if(ret == 0)
	{
	    int_struct.id = gpio->port;
	    int_struct.phandler = gpio_int_cb;
	    gpio_int_hdls[ int_struct.id ].arg = arg;
	    gpio_int_hdls[ int_struct.id ].handler = handler;

	    sddev_control(GPIO_DEV_NAME, CMD_GPIO_INT_ENABLE, &int_struct);
	}

    return ret;
}

int32_t hal_gpio_disable_irq(gpio_dev_t *gpio)
{
    uint32_t param;

    param = gpio->port;

    sddev_control(GPIO_DEV_NAME, CMD_GPIO_INT_DISABLE, &param);

    return 0;
}

int32_t hal_gpio_clear_irq(gpio_dev_t *gpio)
{
    uint32_t param;

    param = gpio->port;

    sddev_control(GPIO_DEV_NAME, CMD_GPIO_CLR_DPLL_UNLOOK_INT_BIT, &param);

    return 0;
}

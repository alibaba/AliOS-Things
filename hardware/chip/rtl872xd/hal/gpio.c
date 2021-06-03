/**
******************************************************************************
* @file    paltform_gpio.c 
* @author  William Xu
* @version V1.0.0
* @date    05-May-2014
* @brief   This file provide GPIO driver functions.
******************************************************************************
*
*  The MIT License
*  Copyright (c) 2014 MXCHIP Inc.
*
*  Permission is hereby granted, free of charge, to any person obtaining a copy 
*  of this software and associated documentation files (the "Software"), to deal
*  in the Software without restriction, including without limitation the rights 
*  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*  copies of the Software, and to permit persons to whom the Software is furnished
*  to do so, subject to the following conditions:
*
*  The above copyright notice and this permission notice shall be included in
*  all copies or substantial portions of the Software.
*
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
*  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
*  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
*  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR 
*  IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
******************************************************************************
*/ 

#include "aos/hal/gpio.h"
#include "PinNames.h"
#include "objects.h"
#include "gpio_irq_api.h"


const static uint8_t gpio_remap[] =
{
    [0] = PA_0,
    [1] = PA_1,
    [2] = PA_2,
    [3] = PA_3,
    [4] = PA_4,
    [5] = PA_5,
    [6] = PA_6,
    [7] = PA_7,
    [8] = PA_8,
    [9] = PA_9,
    [10] = PA_10,
    [11] = PA_11,
    [12] = PA_12,
    [13] = PA_13,
    [14] = PA_14,
    [15] = PA_15,
    [16] = PA_16,
    [17] = PA_17,
    [18] = PA_18,
    [19] = PA_19,
    [20] = PA_20,
    [21] = PA_21,
    [22] = PA_22,
    [23] = PA_23,
    [24] = PA_24,
    [25] = PA_25,
    [26] = PA_26,
    [27] = PA_27,
    [28] = PA_28,
    [29] = PA_29,
    [30] = PA_30,
    [31] = PA_31,
    [32] = PB_0,
    [33] = PB_1,
    [34] = PB_2,
    [35] = PB_3,
    [36] = PB_4,
    [37] = PB_5,
    [38] = PB_6,
    [39] = PB_7,
    [40] = PB_8,
    [41] = PB_9,
    [42] = PB_10,
    [43] = PB_11,
    [44] = PB_12,
    [45] = PB_13,
    [46] = PB_14,
    [47] = PB_15,
    [48] = PB_16,
    [49] = PB_17,
    [50] = PB_18,
    [51] = PB_19,
    [52] = PB_20,
    [53] = PB_21,
    [54] = PB_22,
    [55] = PB_23,
    [56] = PB_24,
    [57] = PB_25,
    [58] = PB_26,
    [59] = PB_27,
    [60] = PB_28,
    [61] = PB_29,
    [62] = PB_30,
    [63] = PB_31,
};

#define GPIO_NUM_PINS   (sizeof(gpio_remap) / sizeof(gpio_remap[0]))

typedef struct
{
	gpio_t gpio_obj;
	gpio_irq_t gpio_irq_obj;
} gpio_objs_t;

static gpio_objs_t gpio_objs[GPIO_NUM_PINS];

void platform_jtag_off(void)
{
  static uint8_t jtag_on = 1;
  if(jtag_on){
    sys_jtag_off();
    jtag_on = 0;
  }
}

int32_t hal_gpio_init( gpio_dev_t *gpio )
{
    if(!gpio || gpio->port >= GPIO_NUM_PINS)
        return -1;

	gpio_t *gpio_obj = &gpio_objs[gpio->port].gpio_obj;
	uint8_t pin = gpio_remap[gpio->port];

    if(pin == PA_27 || pin == PB_23){
        platform_jtag_off();
    }
	gpio_init(gpio_obj, pin);

	switch(gpio->config){
		case INPUT_PULL_UP:
			gpio_dir(gpio_obj, PIN_INPUT);     // Direction: Input
			gpio_mode(gpio_obj, PullUp);       // Pull-High
			break;
		case INPUT_PULL_DOWN:
			gpio_dir(gpio_obj, PIN_INPUT);     // Direction: Input
			gpio_mode(gpio_obj, PullDown);       // Pull-Down
			break;
		case INPUT_HIGH_IMPEDANCE:
			gpio_dir(gpio_obj, PIN_INPUT);     // Direction: Input
			gpio_mode(gpio_obj, PullNone);       // Pull-None
			break;
		case OUTPUT_PUSH_PULL:
			gpio_dir(gpio_obj, PIN_OUTPUT);     // Direction: Input
			gpio_mode(gpio_obj, PullUp);       // Pull-None
			break;
		case OUTPUT_OPEN_DRAIN_NO_PULL:
		case OUTPUT_OPEN_DRAIN_PULL_UP:
			gpio_dir(gpio_obj, PIN_OUTPUT);     // Direction: Input
			gpio_mode(gpio_obj, PullDown);       // Pull-None
			break;
	}

	return 0;
}

int32_t hal_gpio_deinit( gpio_dev_t *gpio )
{
    if(!gpio || gpio->port >= GPIO_NUM_PINS)
        return -1;

    gpio_deinit(&gpio_objs[gpio->port].gpio_obj);

    return 0;
}

int32_t hal_gpio_output_high( gpio_dev_t* gpio )
{
    if(!gpio || gpio->port >= GPIO_NUM_PINS)
        return -1;

    gpio_write(&gpio_objs[gpio->port].gpio_obj, 1);

    return 0;
}

int32_t hal_gpio_output_low( gpio_dev_t* gpio )
{
    if(!gpio || gpio->port >= GPIO_NUM_PINS)
        return -1;

    gpio_write(&gpio_objs[gpio->port].gpio_obj, 0);

    return 0;
}

int32_t hal_gpio_output_toggle( gpio_dev_t* gpio )
{
    if(!gpio || gpio->port >= GPIO_NUM_PINS)
        return -1;

    gpio_write(&gpio_objs[gpio->port].gpio_obj, !gpio_read(&gpio_objs[gpio->port].gpio_obj));

    return 0;
}

int32_t hal_gpio_input_get(gpio_dev_t *gpio, uint32_t *value)
{
    if(!gpio || gpio->port >= GPIO_NUM_PINS || !value)
        return -1;

    *value = gpio_read(&gpio_objs[gpio->port].gpio_obj) == 0 ? 0 : 1;

    return 0;
}

int32_t hal_gpio_get(gpio_dev_t *gpio, uint32_t *value)
{
    if(!gpio || gpio->port >= GPIO_NUM_PINS)
        return -1;

    return hal_gpio_input_get(gpio, value);
}

int32_t hal_gpio_enable_irq(gpio_dev_t *gpio, gpio_irq_trigger_t trigger, gpio_irq_handler_t handler, void *arg)
{
    if(!gpio || gpio->port >= GPIO_NUM_PINS)
        return -1;

    gpio_irq_t *gpio_irq_obj = &gpio_objs[gpio->port].gpio_irq_obj;
    uint8_t pin = gpio_remap[gpio->port];

    gpio_irq_init(gpio_irq_obj, pin, handler, (uint32_t)(arg));

    switch ( trigger )
    {
    case IRQ_TRIGGER_RISING_EDGE:
        gpio_irq_set(gpio_irq_obj, IRQ_RISE, 1);   // Rising Edge Trigger
        gpio_irq_enable(gpio_irq_obj);
        break;
    case IRQ_TRIGGER_FALLING_EDGE:
        gpio_irq_set(gpio_irq_obj, IRQ_FALL, 1);   // Falling Edge Trigger
        gpio_irq_enable(gpio_irq_obj);
        break;
    case IRQ_TRIGGER_BOTH_EDGES:
        return -1;
        break;
    default:
        return -1;
    }

    return 0;
}


int32_t hal_gpio_clear_irq( gpio_dev_t* gpio )
{
    if(!gpio || gpio->port >= GPIO_NUM_PINS)
        return -1;

    gpio_irq_deinit(&gpio_objs[gpio->port].gpio_irq_obj);

    return 0;
}


int32_t hal_gpio_finalize(gpio_dev_t *gpio)
{
    return 0;
}

int32_t hal_gpio_disable_irq(gpio_dev_t *gpio)
{
    return 0;
}

/*
 * Copyright (c) 2019 MediaTek Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "aos/hal/gpio.h"
#include "mt_gpio.h"
#include "eint.h"

int32_t hal_gpio_init( gpio_dev_t *gpio )
{
    return 0;
}

int32_t hal_gpio_deinit( gpio_dev_t *gpio )
{
    return 0;
}

int32_t hal_gpio_output_high( gpio_dev_t* gpio )
{
#if 0
    gpio_set_out(gpio->port, GPIO_OUT_ONE);
    gpio_set_dir(gpio->port, GPIO_DIR_OUT);
    gpio_set_mode(gpio->port, GPIO_MODE_GPIO);
#endif
    return 0;
}

int32_t hal_gpio_output_low( gpio_dev_t* gpio )
{
#if 0
    gpio_set_out(gpio->port, GPIO_OUT_ZERO);
    gpio_set_dir(gpio->port, GPIO_DIR_OUT);
    gpio_set_mode(gpio->port, GPIO_MODE_GPIO);
#endif

    return 0;
}

int32_t hal_gpio_output_toggle( gpio_dev_t* gpio )
{
    return 0;
}

int32_t hal_gpio_input_get(gpio_dev_t *gpio, uint32_t *value)
{
    int32_t ret = 0;
#if 0
    gpio_get_dir(gpio->port);
    if (0 == gpio_get_dir(gpio->port))
        *value = gpio_get_in(gpio->port);
    else
        ret = -1;
#endif
    return ret;
}

int32_t hal_gpio_enable_irq(gpio_dev_t *gpio, gpio_irq_trigger_t trigger, gpio_irq_handler_t handler, void *arg)
{
#if 0
	uint32_t pol = 0;
    gpio_set_mode(gpio->port, GPIO_MODE_GPIO);
    gpio_set_dir(gpio->port, GPIO_DIR_IN);

    switch (trigger)
    {
    	case IRQ_TRIGGER_RISING_EDGE:
		pol = 0;
		break;
	case IRQ_TRIGGER_FALLING_EDGE:
		pol = 1;
		break;
	case IRQ_TRIGGER_BOTH_EDGES:
		return -1;
		break;
	default:
		return -1;
    }
    mt_eint_registration(gpio->port, 0,  pol, handler, 1, 1);
#endif
    return 0;
}


int32_t hal_gpio_clear_irq( gpio_dev_t* gpio )
{
    return 0;
}


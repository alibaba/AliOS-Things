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

#include "PinNames.h"
#include "objects.h"
#include "gpio_irq_api.h"
#include <hal/soc/soc.h>

const static uint8_t gpio_remap[] =
{
    [0 ] = PA_14,
    [1 ] = PA_15,
    [2 ] = PA_22,
    [3 ] = PA_19,
    [4 ] = PA_23,
    [5 ] = PA_18,
    [6 ] = PA_0,
    [7 ] = PA_12,
    [8 ] = PA_5,
    [9 ] = PA_11,
    [10] = PA_30,
    [11] = PA_29,
    [12] = PA_8,
};

typedef struct 
{
	gpio_t gpio_obj;
	gpio_irq_t gpio_irq_obj;
} gpio_objs_t;

int32_t hal_gpio_init( gpio_dev_t *gpio )
{
	if(!gpio)
		return -1;

	gpio->priv = malloc(sizeof(gpio_objs_t));

	gpio_t *gpio_obj = &(((gpio_objs_t *)(gpio->priv))->gpio_obj);
	uint8_t pin = gpio_remap[gpio->port];

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
	if(!gpio || !gpio->priv)
		return -1;
	
	gpio_deinit(&(((gpio_objs_t *)(gpio->priv))->gpio_obj));
	free(gpio->priv);
	gpio->priv = NULL;
	
  	return 0;
}

int32_t hal_gpio_output_high( gpio_dev_t* gpio )
{
  	gpio_write(&(((gpio_objs_t *)(gpio->priv))->gpio_obj), 1);
  
  	return 0;
}

int32_t hal_gpio_output_low( gpio_dev_t* gpio )
{
  	gpio_write(&(((gpio_objs_t *)(gpio->priv))->gpio_obj), 0);
  
  	return 0;
}

int32_t hal_gpio_output_toggle( gpio_dev_t* gpio )
{
  	gpio_write(&(((gpio_objs_t *)(gpio->priv))->gpio_obj), !gpio_read(&(((gpio_objs_t *)(gpio->priv))->gpio_obj)));
  
  	return 0;
}

int32_t hal_gpio_input_get(gpio_dev_t *gpio, uint32_t *value)
{
  	*value = gpio_read(&(((gpio_objs_t *)(gpio->priv))->gpio_obj)) == 0 ? 0 : 1;
	return 0;
}

int32_t hal_gpio_enable_irq(gpio_dev_t *gpio, gpio_irq_trigger_t trigger, gpio_irq_handler_t handler, void *arg)
{
	gpio_irq_t *gpio_irq_obj = &(((gpio_objs_t *)(gpio->priv))->gpio_irq_obj);
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
	gpio_irq_deinit(&(((gpio_objs_t *)(gpio->priv))->gpio_irq_obj));
	return 0;
}

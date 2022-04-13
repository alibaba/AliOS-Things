/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>

#include "amp_hal_gpio.h"
#include "drv_gpio.h"
#include "fibo_common.h"
#include "fibo_gpio.h"
#include "osi_log.h"

int32_t amp_hal_gpio_init(gpio_dev_t *gpio)
{
    int ret = -1;
    uint16_t gpionum = gpio->port;
    ret = fibo_hal_gpioModeInit(gpionum);
    if(!ret)
    {
    	 ret = -1;
    	 return ret;
    }
    switch (gpio->config)
    {
       case     INPUT_PULL_UP:                               
       case     INPUT_PULL_DOWN:
       case     INPUT_HIGH_IMPEDANCE:
      	{
   		ret =  fibo_hal_gpioSetDir(gpionum, DRV_GPIO_INPUT);
	       if(!ret)
	       {
	           ret = -1;
	    	    return ret;
	       }
		 break;
	}
      case     OUTPUT_PUSH_PULL:   
      case     OUTPUT_OPEN_DRAIN_NO_PULL:
      case     OUTPUT_OPEN_DRAIN_PULL_UP:
      {
	  	OSI_PRINTFI(" gpio set %d DRV_GPIO_OUTPUT mode\n ", gpionum);
	  	 ret =  fibo_hal_gpioSetDir(gpionum, DRV_GPIO_OUTPUT);
	       if(!ret)
	       {
	           ret = -1;
	    	    return ret;
	       }
             break;
	}
	case    	IRQ_MODE:
	case    	ANALOG_MODE:
	{
		ret = fibo_hal_is_vaild_pin(gpionum);
	       if(!ret)
	       {
	           ret = -1;
	    	    return ret;
	       }
		break;
	}
	default:
	{
		break;
	}
    }
    ret = 0;
    return ret;
}

int32_t amp_hal_gpio_output_high(gpio_dev_t *gpio)
{
	int ret = -1;
	ret = fibo_hal_gpioSetLevel(gpio->port, HAL_GPIO_DATA_HIGH);
	if(!ret)
	{
	       ret = -1;
	    	return ret;
	}
    ret = 0;
    return ret;
}

int32_t amp_hal_gpio_output_low(gpio_dev_t *gpio)
{
	int ret = -1;
	ret = fibo_hal_gpioSetLevel(gpio->port, HAL_GPIO_DATA_LOW);
	if(!ret)
	{
	       ret = -1;
	    	return ret;
	}	
    ret = 0;
    return ret;
}

int32_t amp_hal_gpio_output_toggle(gpio_dev_t *gpio)
{
       int ret = -1;
	int value;
	ret = fibo_hal_gpioGetLevel(gpio->port, (hal_gpio_data_t *)(&value));
	if(ret)
	{
		if(value == HAL_GPIO_DATA_HIGH)
		{
			ret = fibo_hal_gpioSetLevel(gpio->port, HAL_GPIO_DATA_LOW);
			if(!ret)
			{
			       ret = -1;
			     	return ret;
			}	
		}
		else
		{
			ret = fibo_hal_gpioSetLevel(gpio->port, HAL_GPIO_DATA_HIGH);
			if(!ret)
			{
			       ret = -1;
			    	return ret;
			}				
		}
	}
	else
	{
	     ret = -1;
	     return ret;
	}
    ret = 0;
    return ret;
}

int32_t amp_hal_gpio_input_get(gpio_dev_t *gpio, uint32_t *value)
{
	int ret = -1;
	ret = fibo_hal_gpioGetLevel(gpio->port, (hal_gpio_data_t *)(value));
	if(!ret)
	{
	       ret = -1;
		return ret;
	}
      else
      	{
      	      ret = 0;
      	}
    return ret;
}

int32_t amp_hal_gpio_enable_irq(gpio_dev_t *gpio, gpio_irq_trigger_t trigger,
                            gpio_irq_handler_t handler, void *arg)
{
	int ret = fibo_hal_gpioModeInit(gpio->port);
	hal_gpioisr_t oc_isr;
	oc_isr.drv_gpio = NULL;
	oc_isr.drv_isr = (drvGpioConfig_t*)malloc(sizeof(drvGpioConfig_t));
	memset(oc_isr.drv_isr,0,sizeof(drvGpioConfig_t));
	oc_isr.drv_isr->mode= DRV_GPIO_INPUT;
	oc_isr.drv_isr->intr_enabled = true;
	oc_isr.drv_isr->intr_level = false;
	oc_isr.isr_callback= handler;	
	
	switch (trigger)
	{
		case IRQ_TRIGGER_RISING_EDGE:
		{
			oc_isr.drv_isr->debounce = false;
			oc_isr.drv_isr->rising = true;
		       oc_isr.drv_isr->falling = false;		
			break;
		}
		case IRQ_TRIGGER_FALLING_EDGE:
		{
			oc_isr.drv_isr->debounce = false;
			oc_isr.drv_isr->rising = false;
		       oc_isr.drv_isr->falling = true;		
			break;
		}			
		case IRQ_TRIGGER_BOTH_EDGES:
		default:
		{
			oc_isr.drv_isr->debounce = true;
			oc_isr.drv_isr->rising = true;
		       oc_isr.drv_isr->falling = true;		
			break;
		}
	}
	ret =  aliyun_hal_gpioIsrInit(gpio->port, &oc_isr, arg);
	if(ret == false)
	{
		ret = -1;
	}
	else
	{
		ret = 0;
	}
	free(oc_isr.drv_isr);
	return ret;
}

int32_t amp_hal_gpio_disable_irq(gpio_dev_t *gpio)
{
	int ret = -1;
	ret =  fibo_hal_gpioIsrDeInit(gpio->port);
	if(!ret)
	{
		ret =  -1;
	}
	else
	{
		ret  = 0;
	}
    return ret;
}

int32_t amp_hal_gpio_clear_irq(gpio_dev_t *gpio)
{
    return 0;
}

int32_t amp_hal_gpio_finalize(gpio_dev_t *gpio)
{
    int ret  = -1;
    ret = fibo_hal_gpioModerelease(gpio->port);
    if(!ret)
    {
    	ret = -1;
    }
    else
    {
    	ret = 0;
    }
    return ret;
}


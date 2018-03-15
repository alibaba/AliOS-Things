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


#include "mico_platform.h"
#include "mico_rtos.h"

#include "board.h"
#include "platform_peripheral.h"
#include "platform_logging.h"

#include "gpio_irq_api.h"

/******************************************************
*                    Constants
******************************************************/

/******************************************************
*                   Enumerations
******************************************************/

/******************************************************
*                 Type Definitions
******************************************************/

/******************************************************
*                    Structures
******************************************************/

/******************************************************
*               Variables Definitions
******************************************************/

/******************************************************
*               Function Declarations
******************************************************/

/******************************************************
*               Function Definitions
******************************************************/
OSStatus platform_gpio_init( const platform_gpio_t* gpio, platform_pin_config_t config )
{
	OSStatus          err = kNoErr;

	platform_mcu_powersave_disable();
  	require_action_quiet( gpio != NULL, exit, err = kParamErr);

  	gpio_init((gpio_t *)&gpio->gpio_obj, gpio->pin);

  	switch(config){
  		case INPUT_PULL_UP:
			gpio_dir((gpio_t *)&gpio->gpio_obj, PIN_INPUT);     // Direction: Input
			gpio_mode((gpio_t *)&gpio->gpio_obj, PullUp);       // Pull-High	 
			break;
		case INPUT_PULL_DOWN:
			gpio_dir((gpio_t *)&gpio->gpio_obj, PIN_INPUT);     // Direction: Input
			gpio_mode((gpio_t *)&gpio->gpio_obj, PullDown);       // Pull-Down	 
			break;
		case INPUT_HIGH_IMPEDANCE:
			gpio_dir((gpio_t *)&gpio->gpio_obj, PIN_INPUT);     // Direction: Input
			gpio_mode((gpio_t *)&gpio->gpio_obj, PullNone);       // Pull-None
			break;	
		case OUTPUT_PUSH_PULL:
			gpio_dir((gpio_t *)&gpio->gpio_obj, PIN_OUTPUT);     // Direction: Input
			gpio_mode((gpio_t *)&gpio->gpio_obj, PullUp);       // Pull-None
			break;	
		case OUTPUT_OPEN_DRAIN_NO_PULL:
		case OUTPUT_OPEN_DRAIN_PULL_UP:		
			gpio_dir((gpio_t *)&gpio->gpio_obj, PIN_OUTPUT);     // Direction: Input
			gpio_mode((gpio_t *)&gpio->gpio_obj, PullDown);       // Pull-None
			break;		
  	}
  
exit:
  	platform_mcu_powersave_enable();
  	return err;
}

OSStatus platform_gpio_deinit( const platform_gpio_t* gpio )
{
  	OSStatus          err = kNoErr;

  	platform_mcu_powersave_disable();
  	require_action_quiet( gpio != NULL, exit, err = kParamErr);

  	gpio_deinit((gpio_t *)&gpio->gpio_obj);

exit:
  	platform_mcu_powersave_enable();
  	return err;
}

#if 0
OSStatus platform_gpio_ip_deinit(void)
{
  OSStatus          err = kNoErr;

  platform_mcu_powersave_disable();

  HAL_GPIO_IP_DeInit();

exit:
  platform_mcu_powersave_enable();
  return err;
}
#endif

OSStatus platform_gpio_output_high( const platform_gpio_t* gpio )
{
  	OSStatus err = kNoErr;

  	platform_mcu_powersave_disable();

  	require_action_quiet( gpio != NULL, exit, err = kParamErr);

  	gpio_write((gpio_t *)&gpio->gpio_obj, 1);
  
exit:
  	platform_mcu_powersave_enable();
  	return err;
}

OSStatus platform_gpio_output_low( const platform_gpio_t* gpio )
{
  	OSStatus err = kNoErr;

  	platform_mcu_powersave_disable();

  	require_action_quiet( gpio != NULL, exit, err = kParamErr);

  	gpio_write((gpio_t *)&gpio->gpio_obj, 0);
  
exit:
  	platform_mcu_powersave_enable();
  	return err;
}

OSStatus platform_gpio_output_trigger( const platform_gpio_t* gpio )
{
  	OSStatus err = kNoErr;

  	platform_mcu_powersave_disable();

  	require_action_quiet( gpio != NULL, exit, err = kParamErr);

  	gpio_write((gpio_t *)&gpio->gpio_obj, !gpio_read((gpio_t *)&gpio->gpio_obj));
  
exit:
  	platform_mcu_powersave_enable();
  	return err;
}

bool platform_gpio_input_get( const platform_gpio_t* gpio )
{
  	bool result = false;

  	platform_mcu_powersave_disable();

  	require_quiet( gpio != NULL, exit);

  	if(gpio_read((gpio_t *)&gpio->gpio_obj)){
  		result = true;
  	}else{
  		result = false;
  	}
  
exit:
  	platform_mcu_powersave_enable();
  	return result;
}

OSStatus platform_gpio_irq_enable( const platform_gpio_t* gpio, platform_gpio_irq_trigger_t trigger, platform_gpio_irq_callback_t handler, void* arg )
{
  	OSStatus err = kNoErr;

  	platform_mcu_powersave_disable();
  	require_action_quiet( gpio != NULL, exit, err = kParamErr);

    	gpio_irq_init((gpio_irq_t *)&gpio->gpio_irq_obj, gpio->pin, handler, (uint32_t)(arg));

 	switch ( trigger )
  	{
    		case IRQ_TRIGGER_RISING_EDGE:
    			gpio_irq_set((gpio_irq_t *)&gpio->gpio_irq_obj, IRQ_RISE, 1);   // Rising Edge Trigger
    			gpio_irq_enable((gpio_irq_t *)&gpio->gpio_irq_obj);
      			break;
    		case IRQ_TRIGGER_FALLING_EDGE:
    			gpio_irq_set((gpio_irq_t *)&gpio->gpio_irq_obj, IRQ_FALL, 1);   // Falling Edge Trigger
    			gpio_irq_enable((gpio_irq_t *)&gpio->gpio_irq_obj);
      			break;
    		case IRQ_TRIGGER_BOTH_EDGES:
      			err =  kUnsupportedErr;
      			goto exit;
			break;
    		default:
      			err =  kParamErr;
      		goto exit;
  	}
  
exit:
  	platform_mcu_powersave_enable();
  	return err;
}


OSStatus platform_gpio_irq_disable( const platform_gpio_t* gpio )
{
  	OSStatus err = kNoErr;

  	platform_mcu_powersave_disable();
  	require_action_quiet( gpio != NULL, exit, err = kParamErr);

  	gpio_irq_deinit((gpio_irq_t *)&gpio->gpio_irq_obj);
  
exit:
  	platform_mcu_powersave_enable();
  	return err;
}


/******************************************************
 *      STM32F2xx Internal Function Definitions
 ******************************************************/

/******************************************************
 *               IRQ Handler Definitions
 ******************************************************/

/* Common IRQ handler for all GPIOs */


/******************************************************
 *               IRQ Handler Mapping
 ******************************************************/






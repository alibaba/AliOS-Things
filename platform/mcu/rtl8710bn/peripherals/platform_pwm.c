/**
******************************************************************************
* @file    paltform_pwm.c 
* @author  William Xu
* @version V1.0.0
* @date    05-May-2014
* @brief   This file provide PWM driver functions.
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


//#include "MICORTOS.h"
//#include "MICOPlatform.h"

#include "board.h"
#include "platform_peripheral.h"
#include "Platform_logging.h"

#include "pinmap.h"
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
OSStatus platform_pwm_init( const platform_pwm_t* pwm, uint32_t frequency, float duty_cycle )
{
  	OSStatus err = kNoErr;
	u32 period = (uint32_t)((float)1000000.0/frequency);
  	float value;  
  
  	require_action_quiet( pwm != NULL, exit, err = kParamErr);

  	platform_mcu_powersave_disable();

  	if (duty_cycle < (float)0.0) {
      		value = 0.0;
  	}else if (duty_cycle > (float)100.0) {
      		value = 1.0;
  	}else{
      		value = duty_cycle/100;
  	}

  	pwmout_init((pwmout_t*)&pwm->pwm_obj, pwm->pin);

	
	printf("pwm init period %d, duty_cycle %7.5f\r\n", period, value);
	pwmout_period_us((pwmout_t*)&pwm->pwm_obj, period);  
	
	pwmout_write((pwmout_t*)&pwm->pwm_obj, value);
exit:  
  	platform_mcu_powersave_enable();
  	return err;
}

OSStatus platform_pwm_start( const platform_pwm_t* pwm )
{
  	OSStatus err = kNoErr;
  
  	platform_mcu_powersave_disable();

  	require_action_quiet( pwm != NULL, exit, err = kParamErr);
  
  	pwmout_start((pwmout_t*)&pwm->pwm_obj); 

exit:  
  	platform_mcu_powersave_enable();
  	return err;
}

OSStatus platform_pwm_stop( const platform_pwm_t* pwm )
{
  	OSStatus err = kNoErr;
  
  	platform_mcu_powersave_disable();

  	require_action_quiet( pwm != NULL, exit, err = kParamErr);
  
  	pwmout_stop((pwmout_t*)&pwm->pwm_obj); 
  
exit:  
  	platform_mcu_powersave_enable();
  	return err;
}






#include "platform.h"
#include "platform_peripheral.h"
#include "debug.h"
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
  TIM_TimeBaseInitTypeDef tim_time_base_structure;
  TIM_OCInitTypeDef       tim_oc_init_structure;
  RCC_ClocksTypeDef       rcc_clock_frequencies;
  uint16_t                period              = 0;
  float                   adjusted_duty_cycle = ( ( duty_cycle > 100.0f ) ? 100.0f : duty_cycle );
  OSStatus                err                 = kNoErr;
  
  require_action_quiet( pwm != NULL, exit, err = kParamErr);

  platform_mcu_powersave_disable();

  RCC_GetClocksFreq( &rcc_clock_frequencies );

  if ( pwm->tim == TIM1 || pwm->tim == TIM8 || pwm->tim == TIM9 || pwm->tim == TIM10 || pwm->tim == TIM11 )
  {
    RCC_APB2PeriphClockCmd( pwm->tim_peripheral_clock, ENABLE );
    if( rcc_clock_frequencies.PCLK2_Frequency == rcc_clock_frequencies.HCLK_Frequency )
      period = (uint16_t)( rcc_clock_frequencies.PCLK2_Frequency / 20 / frequency - 1 ); /* Auto-reload value counts from 0; hence the minus 1 */
    else
      period = (uint16_t)( rcc_clock_frequencies.PCLK2_Frequency * 2 / 20 / frequency - 1 ); /* Auto-reload value counts from 0; hence the minus 1 */
  }
  else
  {
    RCC_APB1PeriphClockCmd( pwm->tim_peripheral_clock, ENABLE );
    if( rcc_clock_frequencies.PCLK1_Frequency == rcc_clock_frequencies.HCLK_Frequency )
      period = (uint16_t)( rcc_clock_frequencies.PCLK1_Frequency / 20 / frequency - 1 ); /* Auto-reload value counts from 0; hence the minus 1 */
    else
      period = (uint16_t)( rcc_clock_frequencies.PCLK1_Frequency * 2 / 20 / frequency - 1 ); /* Auto-reload value counts from 0; hence the minus 1 */
  }
    
  /* Set alternate function */
  platform_gpio_set_alternate_function( pwm->pin->port, pwm->pin->pin_number, GPIO_OType_PP, GPIO_PuPd_UP, pwm->gpio_af );
  
  /* Time base configuration */
  tim_time_base_structure.TIM_Period            = (uint32_t) period;
  tim_time_base_structure.TIM_Prescaler         = (uint16_t) 19;  /* Divide clock by 19 + 1 to enable a count of high cycle + low cycle = 1 PWM cycle */
  tim_time_base_structure.TIM_ClockDivision     = 0;
  tim_time_base_structure.TIM_CounterMode       = TIM_CounterMode_Up;
  tim_time_base_structure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit( pwm->tim, &tim_time_base_structure );
  
  /* PWM1 Mode configuration */
  tim_oc_init_structure.TIM_OCMode       = TIM_OCMode_PWM1;
  tim_oc_init_structure.TIM_OutputState  = TIM_OutputState_Enable;
  tim_oc_init_structure.TIM_OutputNState = TIM_OutputNState_Enable;
  tim_oc_init_structure.TIM_Pulse        = (uint16_t) ( adjusted_duty_cycle * (float) period / 100.0f );
  tim_oc_init_structure.TIM_OCPolarity   = TIM_OCPolarity_High;
  tim_oc_init_structure.TIM_OCNPolarity  = TIM_OCNPolarity_High;
  tim_oc_init_structure.TIM_OCIdleState  = TIM_OCIdleState_Reset;
  tim_oc_init_structure.TIM_OCNIdleState = TIM_OCIdleState_Set;
  
  switch ( pwm->channel )
  {
  case 1:
    {
      TIM_OC1Init( pwm->tim, &tim_oc_init_structure );
      TIM_OC1PreloadConfig( pwm->tim, TIM_OCPreload_Enable );
      break;
    }
  case 2:
    {
      TIM_OC2Init( pwm->tim, &tim_oc_init_structure );
      TIM_OC2PreloadConfig( pwm->tim, TIM_OCPreload_Enable );
      break;
    }
  case 3:
    {
      TIM_OC3Init( pwm->tim, &tim_oc_init_structure );
      TIM_OC3PreloadConfig( pwm->tim, TIM_OCPreload_Enable );
      break;
    }
  case 4:
    {
      TIM_OC4Init( pwm->tim, &tim_oc_init_structure );
      TIM_OC4PreloadConfig( pwm->tim, TIM_OCPreload_Enable );
      break;
    }
  default:
    {
      break;
    }
  }

exit:  
  platform_mcu_powersave_enable();
  return err;
}

OSStatus platform_pwm_start( const platform_pwm_t* pwm )
{
  OSStatus err = kNoErr;
  
  platform_mcu_powersave_disable();

  require_action_quiet( pwm != NULL, exit, err = kParamErr);
  
  TIM_Cmd( pwm->tim, ENABLE );
  TIM_CtrlPWMOutputs( pwm->tim, ENABLE );
  
exit:  
  platform_mcu_powersave_enable();
  return err;
}

OSStatus platform_pwm_stop( const platform_pwm_t* pwm )
{
  OSStatus err = kNoErr;
  
  platform_mcu_powersave_disable();

  require_action_quiet( pwm != NULL, exit, err = kParamErr);
  
  TIM_CtrlPWMOutputs( pwm->tim, DISABLE );
  TIM_Cmd( pwm->tim, DISABLE );
  
exit:  
  platform_mcu_powersave_enable();
  return err;
}





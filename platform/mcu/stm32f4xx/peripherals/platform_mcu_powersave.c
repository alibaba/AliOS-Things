
#include "platform_peripheral.h"
#include "platform.h"
#include "platform_config.h"
#include "platform_logging.h"
#include <string.h> // For memcmp
//#include "crt0.h"
#include "platform_init.h"

#if 0
/******************************************************
*                      Macros
******************************************************/

#define NUMBER_OF_LSE_TICKS_PER_MILLISECOND(scale_factor) ( 32768 / 1000 / scale_factor )
#define CONVERT_FROM_TICKS_TO_MS( n,s )                   ( n / NUMBER_OF_LSE_TICKS_PER_MILLISECOND(s) )

#define ENABLE_INTERRUPTS   __asm("CPSIE i")  /**< Enable interrupts to start task switching in MICO RTOS. */
#define DISABLE_INTERRUPTS  __asm("CPSID i")  /**< Disable interrupts to stop task switching in MICO RTOS. */

/******************************************************
*                    Constants
******************************************************/

#define RTC_INTERRUPT_EXTI_LINE       EXTI_Line22
#define WUT_COUNTER_MAX               0xFFFF
#define CK_SPRE_CLOCK_SOURCE_SELECTED 0xFFFF

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
*               Function Declarations
******************************************************/

/* Default RTC time. Set to 12:20:30 08/04/2013 Monday */
#ifndef MICO_DISABLE_MCU_POWERSAVE
static const platform_rtc_time_t default_rtc_time =
{
   .sec     = 30,
   .min     = 20,
   .hr      = 12,
   .weekday = 1,
   .date    = 8,
   .month   = 4,
   .year    = 13,
};
#endif

#ifndef MICO_DISABLE_MCU_POWERSAVE
static unsigned long  stop_mode_power_down_hook( unsigned long sleep_ms );
static OSStatus       select_wut_prescaler_calculate_wakeup_time( unsigned long* wakeup_time, unsigned long sleep_ms, unsigned long* scale_factor );
#else
static unsigned long  idle_power_down_hook( unsigned long sleep_ms );
#endif
//void wake_up_interrupt_notify( void );


/******************************************************
*               Variables Definitions
******************************************************/

#ifndef MICO_DISABLE_MCU_POWERSAVE
static bool          wake_up_interrupt_triggered  = false;
static unsigned long rtc_timeout_start_time       = 0;
static int32_t       stm32f2_clock_needed_counter = 0;
#endif /* #ifndef MICO_DISABLE_MCU_POWERSAVE */

/******************************************************
 *               Function Definitions
 ******************************************************/

OSStatus platform_mcu_powersave_init(void)
{
#ifndef MICO_DISABLE_MCU_POWERSAVE
  EXTI_InitTypeDef EXTI_InitStructure;
  RTC_InitTypeDef RTC_InitStruct;
  
  //RTC_DeInit( );

  RTC_InitStruct.RTC_HourFormat = RTC_HourFormat_24;
  
  /* RTC ticks every second */
  RTC_InitStruct.RTC_AsynchPrediv = 0x7F;
  RTC_InitStruct.RTC_SynchPrediv = 0xFF;
  
  RTC_Init( &RTC_InitStruct );
  
  /* Enable the PWR clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
  
  /* RTC clock source configuration ------------------------------------------*/
  /* Allow access to BKP Domain */
  PWR_BackupAccessCmd(ENABLE);
  PWR_BackupRegulatorCmd(ENABLE);
  
  /* Enable the LSE OSC */
  RCC_LSEConfig(RCC_LSE_ON);
  /* Wait till LSE is ready */
  while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
  {
  }
  
  /* Select the RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
  
  /* Enable the RTC Clock */
  RCC_RTCCLKCmd(ENABLE);
  
  
  /* RTC configuration -------------------------------------------------------*/
  /* Wait for RTC APB registers synchronisation */
  RTC_WaitForSynchro();
  
  RTC_WakeUpCmd( DISABLE );
  EXTI_ClearITPendingBit( RTC_INTERRUPT_EXTI_LINE );
  PWR_ClearFlag(PWR_FLAG_WU);
  RTC_ClearFlag(RTC_FLAG_WUTF);
  
  RTC_WakeUpClockConfig(RTC_WakeUpClock_RTCCLK_Div2);
  
  EXTI_ClearITPendingBit( RTC_INTERRUPT_EXTI_LINE );
  EXTI_InitStructure.EXTI_Line = RTC_INTERRUPT_EXTI_LINE;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  
  NVIC_EnableIRQ( RTC_WKUP_IRQn );

  RTC_ITConfig(RTC_IT_WUT, DISABLE);
  
  /* Prepare Stop-Mode but leave disabled */
  //PWR_ClearFlag(PWR_FLAG_WU);
  PWR->CR  |= PWR_CR_LPDS;
  PWR->CR  &= (unsigned long)(~(PWR_CR_PDDS));
  SCB->SCR |= ((unsigned long)SCB_SCR_SLEEPDEEP_Msk);
  
#ifdef USE_RTC_BKP
  if (RTC_ReadBackupRegister(RTC_BKP_DR0) != USE_RTC_BKP) {
    /* set it to 12:20:30 08/04/2013 monday */
    platform_rtc_set_time(&default_rtc_time);
    RTC_WriteBackupRegister(RTC_BKP_DR0, USE_RTC_BKP);
  }
#else
  //#ifdef RTC_ENABLED
  /* application must have mico_application_default_time structure declared somewhere, otherwise it wont compile */
  /* write default application time inside rtc */
  platform_rtc_set_time( &default_rtc_time );
  //#endif /* RTC_ENABLED */
#endif
  return kNoErr;
#else
  return kUnsupportedErr;
#endif
}

OSStatus platform_mcu_powersave_disable( void )
{
#ifndef MICO_DISABLE_MCU_POWERSAVE
    DISABLE_INTERRUPTS;
    if ( stm32f2_clock_needed_counter <= 0 )
    {
        SCB->SCR &= (~((unsigned long)SCB_SCR_SLEEPDEEP_Msk));
        stm32f2_clock_needed_counter = 0;
    }
    stm32f2_clock_needed_counter++;
    ENABLE_INTERRUPTS;

    return kNoErr;
#else
    return kUnsupportedErr;
#endif
}

OSStatus platform_mcu_powersave_enable( void )
{
#ifndef MICO_DISABLE_MCU_POWERSAVE
    DISABLE_INTERRUPTS;
    stm32f2_clock_needed_counter--;
    if ( stm32f2_clock_needed_counter <= 0 )
    {
        SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
        stm32f2_clock_needed_counter = 0;
    }
    ENABLE_INTERRUPTS;

    return kNoErr;
#else
    return kUnsupportedErr;
#endif
}

void platform_mcu_powersave_exit_notify( void )
{
#ifndef MICO_DISABLE_MCU_POWERSAVE
  UNUSED_PARAMETER( wake_up_interrupt_triggered );
  wake_up_interrupt_triggered = true;
#endif
}


#ifndef MICO_DISABLE_MCU_POWERSAVE
static OSStatus select_wut_prescaler_calculate_wakeup_time( unsigned long* wakeup_time, unsigned long sleep_ms, unsigned long* scale_factor )
{
  unsigned long temp;
  bool scale_factor_is_found = false;
  int i                              = 0;
  
  static unsigned long int available_wut_prescalers[] =
  {
    RTC_WakeUpClock_RTCCLK_Div2,
    RTC_WakeUpClock_RTCCLK_Div4,
    RTC_WakeUpClock_RTCCLK_Div8,
    RTC_WakeUpClock_RTCCLK_Div16
  };
  static unsigned long scale_factor_values[] = { 2, 4, 8, 16 };
  
  if ( sleep_ms == 0xFFFFFFFF )
  {
    /* wake up in a 100ms, since currently there may be no tasks to run, but after a few milliseconds */
    /* some of them can get unblocked( for example a task is blocked on mutex with unspecified ) */
    *scale_factor = 2;
    RTC_WakeUpClockConfig( RTC_WakeUpClock_RTCCLK_Div2 );
    *wakeup_time = NUMBER_OF_LSE_TICKS_PER_MILLISECOND( scale_factor_values[0] ) * 100;
  }
  else
  {
    for ( i = 0; i < 4; i++ )
    {
      temp = NUMBER_OF_LSE_TICKS_PER_MILLISECOND( scale_factor_values[i] ) * sleep_ms;
      if ( temp < WUT_COUNTER_MAX )
      {
        scale_factor_is_found = true;
        *wakeup_time = temp;
        *scale_factor = scale_factor_values[i];
        break;
      }
    }
    if ( scale_factor_is_found )
    {
      /* set new prescaler for wakeup timer */
      RTC_WakeUpClockConfig( available_wut_prescalers[i] );
    }
    else
    {
      /* scale factor can not be picked up for delays more that 32 seconds when RTCLK is selected as a clock source for the wakeup timer
      * for delays more than 32 seconds change from RTCCLK to 1Hz ck_spre clock source( used to update calendar registers ) */
      RTC_WakeUpClockConfig( RTC_WakeUpClock_CK_SPRE_16bits );
      
      /* with 1Hz ck_spre clock source the resolution changes to seconds  */
      *wakeup_time = ( sleep_ms / 1000 ) + 1;
      *scale_factor = CK_SPRE_CLOCK_SOURCE_SELECTED;
      
      return kGeneralErr;
    }
  }
  
  return kNoErr;
}
#endif


/******************************************************
 *               RTOS Powersave Hooks
 ******************************************************/

void platform_idle_hook( void )
{
    __asm("wfi");
}

uint32_t platform_power_down_hook( uint32_t sleep_ms )
{
#ifdef MICO_DISABLE_MCU_POWERSAVE
    /* If MCU powersave feature is disabled, enter idle mode when powerdown hook is called by the RTOS */
    return idle_power_down_hook( sleep_ms );

#else
    /* If MCU powersave feature is enabled, enter STOP mode when powerdown hook is called by the RTOS */
    return stop_mode_power_down_hook( sleep_ms );

#endif
}

#ifdef MICO_DISABLE_MCU_POWERSAVE
/* MCU Powersave is disabled */
static unsigned long idle_power_down_hook( unsigned long sleep_ms  )
{
    UNUSED_PARAMETER( sleep_ms );
    ENABLE_INTERRUPTS;
    __asm("wfi");
    return 0;
}
#else
static unsigned long stop_mode_power_down_hook( unsigned long sleep_ms )
{
  unsigned long retval;
  unsigned long wut_ticks_passed;
  unsigned long scale_factor = 0;
  UNUSED_PARAMETER(sleep_ms);
  UNUSED_PARAMETER(rtc_timeout_start_time);
  UNUSED_PARAMETER(scale_factor);
  
  if ( ( ( SCB->SCR & (unsigned long)SCB_SCR_SLEEPDEEP_Msk) != 0) && sleep_ms < 5 ){
    SCB->SCR &= (~((unsigned long)SCB_SCR_SLEEPDEEP_Msk));
    __asm("wfi");
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
    /* Note: We return 0 ticks passed because system tick is still going when wfi instruction gets executed */
    ENABLE_INTERRUPTS;
    return 0;
  }
  
  if ( ( ( SCB->SCR & (unsigned long)SCB_SCR_SLEEPDEEP_Msk) ) != 0 )
  {
    /* pick up the appropriate prescaler for a requested delay */
    select_wut_prescaler_calculate_wakeup_time(&rtc_timeout_start_time, sleep_ms, &scale_factor );

    DISABLE_INTERRUPTS;
    
    SysTick->CTRL &= (~(SysTick_CTRL_TICKINT_Msk|SysTick_CTRL_ENABLE_Msk)); /* systick IRQ off */
    RTC_ITConfig(RTC_IT_WUT, ENABLE);
    
    EXTI_ClearITPendingBit( RTC_INTERRUPT_EXTI_LINE );
    PWR_ClearFlag(PWR_FLAG_WU);
    RTC_ClearFlag(RTC_FLAG_WUTF);
    
    RTC_SetWakeUpCounter( rtc_timeout_start_time );
    RTC_WakeUpCmd( ENABLE );
    platform_rtc_enter_powersave();
    
    DBGMCU->CR |= 0x03; /* Enable debug in stop mode */
    
    /* This code will be running with BASEPRI register value set to 0, the main intention behind that is that */
    /* all interrupts must be allowed to wake the CPU from the power-down mode */
    /* the PRIMASK is set to 1( see DISABLE_INTERRUPTS), thus we disable all interrupts before entering the power-down mode */
    /* This may sound contradictory, however according to the ARM CM3 documentation power-management unit */
    /* takes into account only the contents of the BASEPRI register and it is an external from the CPU core unit */
    /* PRIMASK register value doesn't affect its operation. */
    /* So, if the interrupt has been triggered just before the wfi instruction */
    /* it remains pending and wfi instruction will be treated as a nop  */
    __asm("wfi");
    
    /* After CPU exits powerdown mode, the processer will not execute the interrupt handler(PRIMASK is set to 1) */
    /* Disable rtc for now */
    RTC_WakeUpCmd( DISABLE );
    RTC_ITConfig(RTC_IT_WUT, DISABLE);
    
    /* Initialise the clocks again */
    init_clocks( );
    
    /* Enable CPU ticks */
    SysTick->CTRL |= (SysTick_CTRL_TICKINT_Msk|SysTick_CTRL_ENABLE_Msk);
    
    /* Get the time of how long the sleep lasted */
    wut_ticks_passed = rtc_timeout_start_time - RTC_GetWakeUpCounter();
    UNUSED_VARIABLE(wut_ticks_passed);
    platform_rtc_exit_powersave( sleep_ms, (uint32_t *)&retval );
    /* as soon as interrupts are enabled, we will go and execute the interrupt handler */
    /* which triggered a wake up event */
    ENABLE_INTERRUPTS;
    wake_up_interrupt_triggered = false;
    return retval;
  }
  else
  {
    UNUSED_PARAMETER(wut_ticks_passed);
    ENABLE_INTERRUPTS;
    __asm("wfi");
    
    /* Note: We return 0 ticks passed because system tick is still going when wfi instruction gets executed */
    return 0;
  }
}
#endif /* MICO_DISABLE_MCU_POWERSAVE */


void platform_mcu_enter_standby(uint32_t secondsToWakeup)
{ 
  platform_rtc_time_t time;
  uint32_t currentSecond;
  RTC_AlarmTypeDef  RTC_AlarmStructure;

#if defined(STM32F410xx) || defined(STM32F412xG) ||defined(STM32F446xx)
  PWR_WakeUpPinCmd(PWR_WakeUp_Pin,ENABLE);
#endif
  
#if defined(STM32F40_41xxx) || defined(STM32F427_437xx) || defined(STM32F429_439xx) || defined(STM32F401xx) || defined(STM32F411xE)
  PWR_WakeUpPinCmd(ENABLE);
#endif

  if(secondsToWakeup == MICO_WAIT_FOREVER)
    PWR_EnterSTANDBYMode();

  platform_log("Wake up in %ld seconds", secondsToWakeup);
 
  platform_rtc_get_time(&time);
  currentSecond = time.hr*3600 + time.min*60 + time.sec;
  currentSecond += secondsToWakeup;
  RTC_AlarmStructure.RTC_AlarmTime.RTC_H12     = RTC_HourFormat_24;
  RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours   = currentSecond/3600%24;
  RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes = currentSecond/60%60;
  RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds = currentSecond%60;
  RTC_AlarmStructure.RTC_AlarmDateWeekDay = 0x31;
  RTC_AlarmStructure.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_Date;
  RTC_AlarmStructure.RTC_AlarmMask = RTC_AlarmMask_DateWeekDay ;

  RTC_AlarmCmd(RTC_Alarm_A, DISABLE);
  /* Disable the Alarm A */
  RTC_ITConfig(RTC_IT_ALRA, DISABLE);

  /* Clear RTC Alarm Flag */ 
  RTC_ClearFlag(RTC_FLAG_ALRAF);

  RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_AlarmStructure);

  /* Enable RTC Alarm A Interrupt: this Interrupt will wake-up the system from
     STANDBY mode (RTC Alarm IT not enabled in NVIC) */
  RTC_ITConfig(RTC_IT_ALRA, ENABLE);

  /* Enable the Alarm A */
  RTC_AlarmCmd(RTC_Alarm_A, ENABLE);

  PWR_EnterSTANDBYMode();
}


/******************************************************
 *         IRQ Handlers Definition & Mapping
 ******************************************************/

#ifndef MICO_DISABLE_MCU_POWERSAVE
MICO_RTOS_DEFINE_ISR( RTC_WKUP_irq )
{
    EXTI_ClearITPendingBit( RTC_INTERRUPT_EXTI_LINE );
}
#endif

#else
OSStatus platform_mcu_powersave_disable( void )
{
    return 0;
}

OSStatus platform_mcu_powersave_enable( void )
{
    return 0;
}

#endif


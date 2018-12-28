/** ****************************************************************************
 * @copyright Copyright (c) XXX
 * All rights reserved.
 *
 * @file    dbg_port.c
 * @brief   debug port
 * @version V1.0.0
 * @author  liucp
 * @date    20180408
 * @warning none
 * @note <b>history:</b>
 * 1. create file
 *
 */

/*******************************************************************************
 * INCLUDES
 */
#include "dbg_port.h"
#include "stm32l0xx_ll_bus.h"
#include "stm32l0xx_ll_system.h"
#include "stm32l0xx_ll_wwdg.h"
#include "gpio-board.h"
#include "system/timer.h"

/*******************************************************************************
 * DEBUG SWITCH MACROS
 */


/*******************************************************************************
 * MACROS
 */



/*******************************************************************************
 * TYPEDEFS
 */

/**
 * \defgroup GROUP_LOCAL_FUNCTIONS DRIVER:PORT:LOCAL_FUNCTIONS
 */


/*******************************************************************************
 * CONSTANTS
 */
/*******************************************************************************
 * LOCAL FUNCTIONS DECLEAR
 */
static void wwdg_feed_time_event__( void );


/*******************************************************************************
 * GLOBAL VARIABLES
 */


/*******************************************************************************
 * STATIC VARIABLES
 */
static TimerEvent_t s_stWwdgFeedTimer;


/*******************************************************************************
 * EXTERNAL VARIABLES
 */


/*******************************************************************************
 *  GLOBAL FUNCTIONS IMPLEMENTATION
 */
void dbg_swd_enable(void)
{
    // clock for swd is enabled default
    //LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);
    /* enable debug */
    LL_APB2_GRP1_EnableClock( LL_APB2_GRP1_PERIPH_DBGMCU );
    LL_DBGMCU_EnableDBGSleepMode( );
    LL_DBGMCU_EnableDBGStopMode( );
    LL_DBGMCU_EnableDBGStandbyMode( );
}

void dbg_swd_disable(void)
{
    GPIO_InitTypeDef initStruct = { 0 };

    LL_DBGMCU_DisableDBGSleepMode( );
    LL_DBGMCU_DisableDBGStopMode( );
    LL_DBGMCU_DisableDBGStandbyMode( );

    initStruct.Mode = GPIO_MODE_ANALOG;
    initStruct.Pull = GPIO_NOPULL;
    HW_GPIO_Init( GPIOA, GPIO_PIN_13, &initStruct );
    HW_GPIO_Init( GPIOA, GPIO_PIN_14, &initStruct );

    LL_APB2_GRP1_DisableClock( LL_APB2_GRP1_PERIPH_DBGMCU );
}

void wwdg_feed(void)
{
    if (LL_WWDG_IsEnabled(WWDG))
    {
        LL_WWDG_SetCounter(WWDG, 127);
    }
}

void wwdg_feed_check_timer(void)
{
    if (LL_WWDG_IsEnabled(WWDG))
    {
        if (LL_WWDG_GetCounter(WWDG) < 120)
        {

            LL_WWDG_SetCounter(WWDG, 127);
        }
    }
}

void WWDG_IRQHandler(void)
{
    volatile int i = 0;
    i ++;
}

void wwdg_feed_timer_on( void )
{
    if (LL_WWDG_IsEnabled(WWDG))
    {
        wwdg_feed_check_timer( );

        TimerInit( &s_stWwdgFeedTimer, wwdg_feed_time_event__ );
        TimerSetValue( &s_stWwdgFeedTimer, 50 );
        TimerStart( &s_stWwdgFeedTimer );
    }
}

void wwdg_feed_timer_off( void )
{
    if (LL_WWDG_IsEnabled(WWDG))
    {
        wwdg_feed_check_timer( );

        TimerStop( &s_stWwdgFeedTimer );
    }
}


/*******************************************************************************
 * LOCAL FUNCTIONS IMPLEMENTATION
 */
void wwdg_feed_time_event__( void )
{
    if (LL_WWDG_IsEnabled(WWDG))
    {
        wwdg_feed_check_timer( );
        TimerStart( &s_stWwdgFeedTimer );
    }
}

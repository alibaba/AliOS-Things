/** ****************************************************************************
 * @copyright Copyright (c) XXX
 * All rights reserved.
 *
 * @file    debug.c
 * @brief   debug
 * @version V1.0.0
 * @author  liucp
 * @date    2018-2-26
 * @warning none
 * @note <b>history:</b>
 * 1. create file
 *
 */

/*******************************************************************************
 * INCLUDES
 */
#include "debug.h"
#include "hw.h"
#include "system/timer.h"
#include "dbg_port.h"

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
 * \defgroup GROUP_LOCAL_FUNCTIONS MID:DEBUG:LOCAL_FUNCTIONS
 */


/*******************************************************************************
 * CONSTANTS
 */
/*******************************************************************************
 * LOCAL FUNCTIONS DECLEAR
 */
#ifdef MCU_SWD_DISABLE
/**
 * \brief disable swd debug
 * \return none
 */
static void dbg_disable_event( void );
#endif

/*******************************************************************************
 * GLOBAL VARIABLES
 */
uint32_t g_u32_debug_map = 0;

/*******************************************************************************
 * STATIC VARIABLES
 */
#ifdef MCU_SWD_DISABLE
static TimerEvent_t s_st_debug_timer;
#endif

/*******************************************************************************
 * EXTERNAL VARIABLES
 */


/*******************************************************************************
 *  GLOBAL FUNCTIONS IMPLEMENTATION
 */

void DBG_Init( void )
{
    g_u32_debug_map = DBG_LOG_DEFUALT_MASK;
    dbg_swd_enable();
#ifdef MCU_SWD_DISABLE
    TimerInit( &s_st_debug_timer, dbg_disable_event );
    TimerSetValue( &s_st_debug_timer, 10000 );
    TimerStart( &s_st_debug_timer );
#endif

}

void Error_Handler( void )
{
    DBG_PRINTF( "Error_Handler\n\r" );
    while ( 1 )
    {
        ;
    }
}



uint32_t dbg_get_debug_map(void)
{
    return g_u32_debug_map;
}

void dbg_set_debug_bit(uint32_t a_bit_mask)
{
    g_u32_debug_map |= a_bit_mask;
}

void dbg_clr_debug_bit(uint32_t a_bit_mask)
{
    g_u32_debug_map &= ~a_bit_mask;
}

/*******************************************************************************
 * LOCAL FUNCTIONS IMPLEMENTATION
 */
#ifdef MCU_SWD_DISABLE
void dbg_disable_event( void )
{
    dbg_swd_disable();
}
#endif

/** ****************************************************************************
 * @copyright Copyright (c) XXX
 * All rights reserved.
 *
 * @file    dbg_port.h
 * @brief   debug port
 * @version V1.0.0
 * @author  liucp
 * @date    20180408
 * @warning No Warnings
 * @note <b>history:</b>
 * 1.
 *
 */
#ifndef __DBG_PORT_H__
#define __DBG_PORT_H__

/*******************************************************************************
 * INCLUDES
 */
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"{
#endif

/**
 * \defgroup DBG_PORT DRIVER:PORT
 * @{
 */


/*******************************************************************************
 * DEBUG SWITCH MACROS
 */


/*******************************************************************************
 * MACROS
 */
//#define MCU_SWD_DISABLE

/*******************************************************************************
 * TYPEDEFS
 */

/*******************************************************************************
 * CONSTANTS
 */


/*******************************************************************************
 * GLOBAL VARIABLES DECLEAR
 */

/*******************************************************************************
 * GLOBAL FUNCTIONS DECLEAR
 */
/**
 * \brief enable debug using swd interface
 * \return none
 */
void dbg_swd_enable(void);


/**
 * \brief disable debug using swd interface, should be set to low power mode
 * \return none
 */
void dbg_swd_disable(void);

/**
  * \brief feed dog
  * \return none
  */
void wwdg_feed(void);

/**
  * \brief feed dog if check timer under the give time
  * \return none
  */
void wwdg_feed_check_timer(void);

/* start or stop dog feeding */
void wwdg_feed_timer_on( void );
void wwdg_feed_timer_off( void );

/**
 * end of group DBG_PORT
 * @}
 */

#ifdef __cplusplus
}
#endif


#endif

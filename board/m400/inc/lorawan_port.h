/** ****************************************************************************
 * @copyright Copyright (c) XXX
 * All rights reserved.
 *
 * @file    lorawan_port.h
 * @brief   lorawan port
 * @version V1.0.0
 * @author  liucp
 * @date    20180408
 * @warning No Warnings
 * @note <b>history:</b>
 * 1.
 *
 */
#ifndef __LORAWAN_PORT_H__
#define __LORAWAN_PORT_H__

/*******************************************************************************
 * INCLUDES
 */
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "hal/lorawan.h"
#include "utilities.h"


#ifdef __cplusplus
extern "C"{
#endif

/**
 * \defgroup LORAWAN_PORT DRIVER:PORT
 * @{
 */


/*******************************************************************************
 * DEBUG SWITCH MACROS
 */


/*******************************************************************************
 * MACROS
 */


/*******************************************************************************
 * TYPEDEFS
 */


/*******************************************************************************
 * CONSTANTS
 */


/*******************************************************************************
 * GLOBAL VARIABLES DECLEAR
 */

extern hal_lrwan_dev_chg_mode_t aos_lrwan_chg_mode;
extern hal_lrwan_sys_t aos_lrwan_sys;
extern hal_manufactory_itf_t aos_mft_itf;




/*******************************************************************************
 * GLOBAL FUNCTIONS DECLEAR
 */


/**
 * end of group LORAWAN_PORT
 * @}
 */

#ifdef __cplusplus
}
#endif


#endif

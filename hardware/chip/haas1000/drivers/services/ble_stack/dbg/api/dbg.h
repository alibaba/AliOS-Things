#ifndef DBG_H_
#define DBG_H_

/**
****************************************************************************************
* @addtogroup DBG
* @ingroup CONTROLLER
* @brief Debug
*
* @{
****************************************************************************************
*/


/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"     // stack configuration

#include "dbg_swdiag.h"        // sw profiling definitions

#include "dbg_trc.h"           // debug tracer definition

/*
 * FUNCTION DECLARATION
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialization of the BT Debug task
 *
 * This function initializes the the DBG task
 *
 ****************************************************************************************
 */
void dbg_init(bool reset);

/**
 ****************************************************************************************
 * @brief Send back to host status of platform reset request.
 *
 * @param status Reset error code
 *
 ****************************************************************************************
 */
void dbg_platform_reset_complete(uint32_t error);

///@} DBG

#endif // DBG_H_

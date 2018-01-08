
/** @file
 *
 * Bluetooth Management (BTM) Application Programming Interface
 *
 * The BTM consists of several management entities:
 *      1. Device Control - controls the local device
 *      2. Device Discovery - manages inquiries, discover database
 *      3. ACL Channels - manages ACL connections (BR/EDR and LE)
 *      4. SCO Channels - manages SCO connections
 *      5. Security - manages all security functionality
 *      6. Power Management - manages park, sniff, hold, etc.
 *
 * @defgroup micobt      Bluetooth
 *
 * MiCO Bluetooth Framework Functions
 */

#pragma once

#include "mico_bt_dev.h"
#include "mico_bt_cfg.h"

/******************************************************
 *               Function Declarations
 ******************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************/
/**
 * Framework Management Functions
 *
 * @addtogroup  micobt_Framework   Framework
 * @ingroup     micobt
 *
 * @{
 */
/****************************************************************************/

/**
 * Function         mico_bt_stack_init
 *
 *                  Initialize the Bluetooth controller and stack; register
 *                  callback for Bluetooth event notification.
 *
 * @param[in] p_bt_management_cback     : Callback for receiving Bluetooth management events
 * @param[in] p_bt_cfg_settings         : Bluetooth stack configuration
 * @param[in] mico_bt_cfg_buf_pools    : Buffer pool configuration
 *
 * @return    MICO_BT_SUCCESS : on success;
 *            MICO_BT_FAILED : if an error occurred
 */
mico_bt_result_t mico_bt_stack_init(mico_bt_management_cback_t *p_bt_management_cback,
                                    const mico_bt_cfg_settings_t     *p_bt_cfg_settings,
                                    const mico_bt_cfg_buf_pool_t     mico_bt_cfg_buf_pools[MICO_BT_CFG_NUM_BUF_POOLS]);



/**
 * Function         mico_bt_stack_deinit
 *
 *                  De-initialize the Bluetooth controller and stack.
 *
 * @return    MICO_BT_SUCCESS : on success;
 *            MICO_BT_ERROR   : if an error occurred
 */
OSStatus mico_bt_stack_deinit(void);


/**@} micobt_Framework */


#ifdef __cplusplus
}
#endif



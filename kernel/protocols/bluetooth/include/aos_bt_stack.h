/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _AOS_BT_STACK_H_
#define _AOS_BT_STACK_H_

#include "aos_bt_dev.h"
#include "smartbt_cfg.h"

/*****************************************************
 *               Function Declarations
 ******************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************/
/*
 * Framework Management Functions
 *
 * @addtogroup  aosbt_Framework   Framework
 * @ingroup     aosbt
 *
 * @{
 */
/***************************************************************************/

/*
 * Function         aos_bt_stack_init
 *
 *                  Initialize the Bluetooth controller and stack; register
 *                  callback for Bluetooth event notification.
 *
 * @param[in] p_bt_management_cback     : Callback for receiving Bluetooth management events
 * @param[in] p_bt_cfg_settings         : Bluetooth stack configuration
 * @param[in] aos_bt_cfg_buf_pools    : Buffer pool configuration
 *
 * @return    AOS_BT_SUCCESS : on success;
 *            AOS_BT_FAILED : if an error occurred
 */
aos_bt_result_t aos_bt_stack_init(aos_bt_management_cback_t *p_bt_management_cback,
                                  const aos_bt_cfg_settings_t     *p_bt_cfg_settings,
                                  const aos_bt_cfg_buf_pool_t     aos_bt_cfg_buf_pools[AOS_BT_CFG_NUM_BUF_POOLS]);



/*
 * Function         aos_bt_stack_deinit
 *
 *                  De-initialize the Bluetooth controller and stack.
 *
 * @return    AOS_BT_SUCCESS : on success;
 *            AOS_BT_ERROR   : if an error occurred
 */
aos_bt_result_t aos_bt_stack_deinit(void);


/*@} aosbt_Framework */


#ifdef __cplusplus
}
#endif

#endif

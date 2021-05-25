/**
 * Copyright (c) 2015, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _BTE_H_
#define _BTE_H_

#include <stdbool.h>
#include <bt_flags.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  Initialize BT Lib btgap.a.
 * @retval true Success.
 * @retval false Failed.
 *
 * <b>Example usage</b>
 * \code{.c}
    int ble_app_main(void)
    {

        bt_trace_init();
        bt_stack_config_init();
        bte_init();
        board_init();
        le_gap_init(APP_MAX_LINKS);
        app_le_gap_init();
        app_le_profile_init();
        pwr_mgr_init();
        task_init();

        return 0;
    }
 * \endcode
 */
bool bte_init(void);

/**
 * @brief  De initialize BT Lib btgap.a.
 * 
 * This interface will free all the resource of the btgap.a,
 * including the heap, message queues, timers, tasks and so on.
 * After calling the interface, application can't use any interface which supplyed by btgap.a.
 *
 * @retval void.
 *
 * <b>Example usage</b>
 * \code{.c}
    void bt_config_app_deinit(void)
    {
    	bt_config_task_deinit();
    	
    	le_get_gap_param(GAP_PARAM_DEV_STATE , &bt_config_gap_dev_state);
    	if (bt_config_gap_dev_state.gap_init_state != GAP_INIT_STATE_STACK_READY) {
    		BC_printf("BT Stack is not running\n\r");
    	}
#if F_BT_DEINIT
    	else {
    		bte_deinit();
    		bt_trace_uninit();
    		BC_printf("BT Stack deinitalized\n\r");
    	}
#endif
    }
 * \endcode
 */
#if F_BT_DEINIT
void bte_deinit(void);
#endif

#ifdef __cplusplus
}
#endif

#endif  /* _BTE_H_ */

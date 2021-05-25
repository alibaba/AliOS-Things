/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
   * @file      app_task.c
   * @brief     Routines to create App task and handle events & messages
   * @author    jane
   * @date      2017-06-02
   * @version   v1.0
   **************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
   **************************************************************************************
  */

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include <os_msg.h>
#include <os_task.h>
#include <gap.h>
#include <gap_le.h>
#include <trace_app.h>
#include <ble_central_app_task.h>
#include <app_msg.h>
#include <ble_central_app_task.h>
#include <ble_central_client_app.h>
#include <data_uart.h>
#include <user_cmd_parse.h>
#include <user_cmd.h>

/** @defgroup  CENTRAL_CLIENT_APP_TASK Central Client App Task
    * @brief This file handles the implementation of application task related functions.
    *
    * Create App task and handle events & messages
    * @{
    */
/*============================================================================*
 *                              Macros
 *============================================================================*/
#define BLE_CENRTAL_APP_TASK_PRIORITY             1         //!< Task priorities
#define BLE_CENRTAL_APP_TASK_STACK_SIZE           256 * 6   //!<  Task stack size
#define BLE_CENTRAL_MAX_NUMBER_OF_GAP_MESSAGE     0x20      //!<  GAP message queue size
#define BLE_CENTRAL_MAX_NUMBER_OF_IO_MESSAGE      0x20      //!<  IO message queue size
#define BLE_CENTRAL_MAX_NUMBER_OF_EVENT_MESSAGE   (BLE_CENTRAL_MAX_NUMBER_OF_GAP_MESSAGE + BLE_CENTRAL_MAX_NUMBER_OF_IO_MESSAGE)    //!< Event message queue size

/*============================================================================*
 *                              Variables
 *============================================================================*/
void *ble_central_app_task_handle;   //!< APP Task handle
void *ble_central_evt_queue_handle;  //!< Event queue handle
void *ble_central_io_queue_handle;   //!< IO queue handle

/*============================================================================*
 *                              Functions
 *============================================================================*/
void ble_central_app_main_task(void *p_param);

/**
 * @brief  Initialize App task
 * @return void
 */
void ble_central_app_task_init()
{
    os_task_create(&ble_central_app_task_handle, "ble_central_app", ble_central_app_main_task, 0, BLE_CENRTAL_APP_TASK_STACK_SIZE,
                   BLE_CENRTAL_APP_TASK_PRIORITY);
}

/**
 * @brief        App task to handle events & messages
 * @param[in]    p_param    Parameters sending to the task
 * @return       void
 */
void ble_central_app_main_task(void *p_param)
{
    uint8_t event;

    os_msg_queue_create(&ble_central_io_queue_handle, BLE_CENTRAL_MAX_NUMBER_OF_IO_MESSAGE, sizeof(T_IO_MSG));
    os_msg_queue_create(&ble_central_evt_queue_handle, BLE_CENTRAL_MAX_NUMBER_OF_EVENT_MESSAGE, sizeof(uint8_t));

    gap_start_bt_stack(ble_central_evt_queue_handle, ble_central_io_queue_handle, BLE_CENTRAL_MAX_NUMBER_OF_GAP_MESSAGE);

    data_uart_init(ble_central_evt_queue_handle, ble_central_io_queue_handle);
#if	CONFIG_BT_USER_COMMAND   
    user_cmd_init(&user_cmd_if, "central_client");
#endif

    while (true)
    {
        if (os_msg_recv(ble_central_evt_queue_handle, &event, 0xFFFFFFFF) == true)
        {
            if (event == EVENT_IO_TO_APP)
            {
                T_IO_MSG io_msg;
                if (os_msg_recv(ble_central_io_queue_handle, &io_msg, 0) == true)
                {
                    ble_central_app_handle_io_msg(io_msg);
                }
            }
            else
            {
                gap_handle_msg(event);
            }
        }
    }
}

/** @} */ /* End of group CENTRAL_CLIENT_APP_TASK */



/**
*********************************************************************************************************
*               Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     data_uart.h
* @brief    Data uart operations for testing multilink.
* @details  Data uart init and print data through data uart.
* @author   jane
* @date     2016-02-18
* @version  v0.1
*********************************************************************************************************
*/
#ifndef _DATA_UART_H_
#define  _DATA_UART_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

/** @defgroup DATA_UART_CMD Data Uart Command Module
  * @brief Application uses this module to receive user command and send infomation through data uart.
  * @{
  */
/** @defgroup DATA_UART_APIs Data Uart Interface
  * @{
  */
/*============================================================================*
 *                         Functions
 *============================================================================*/
/** @defgroup DATA_UART_APIs_Exported_Functions Data Uart Interface Exported Functions
    * @{
    */
/**
 * @brief  Initializes the Data UART.
 *
 * When data uart receives data, data uart will send an event IO_UART_MSG_TYPE to evt_queue_handle and send the data to io_queue_handle.
 * @param[in] event_queue_handle   Event queue handle which is created by APP.
 * @param[in] io_queue_handle      IO message queue handle which is created by APP.
 * @return void
 *
 * <b>Example usage</b>
 * \code{.c}
    void app_main_task(void *p_param)
    {
        char event;

        os_msg_queue_create(&io_queue_handle, MAX_NUMBER_OF_IO_MESSAGE, sizeof(T_IO_MSG));
        os_msg_queue_create(&evt_queue_handle, MAX_NUMBER_OF_EVENT_MESSAGE, sizeof(unsigned char));

        gap_start_bt_stack(evt_queue_handle, io_queue_handle, MAX_NUMBER_OF_GAP_MESSAGE);

        data_uart_init(evt_queue_handle, io_queue_handle);
        ......
    }
    void app_handle_io_msg(T_IO_MSG io_msg)
    {
        uint16_t msg_type = io_msg.type;
        uint8_t rx_char;

        switch (msg_type)
        {
        case IO_MSG_TYPE_UART:
            // We handle user command informations from Data UART in this branch.
            rx_char = (uint8_t)io_msg.subtype;
            user_cmd_collect(&user_cmd_if, &rx_char, sizeof(rx_char), user_cmd_table);
            break;
        default:
            break;
        }
    }
 * \endcode
 */
void data_uart_init(void *event_queue_handle, void *io_queue_handle);

/**
 * @brief  Print the trace information through data uart.
 * @param[in] fmt   Print parameters.
 * @return void
 *
 * <b>Example usage</b>
 * \code{.c}
    void test(void)
    {
        data_uart_print("GAP scan stop\r\n");
    }
 * \endcode
 */
void data_uart_print(char *fmt, ...);
/** End of DATA_UART_APIs_Exported_Functions
  * @}
  */

/** @} */ /* End of group DATA_UART_APIs */
/** @} */ /* End of group DATA_UART_CMD */
#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif


/**
*********************************************************************************************************
*               Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     user_cmd_parse.h
* @brief    Parse user command from lower Data UART data.
* @details  Structure of command parse results.
* @author
* @date     2016-02-18
* @version  v0.1
*********************************************************************************************************
*/
#ifndef __USER_CMD_PARSE_CMD_H
#define __USER_CMD_PARSE_CMD_H

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#include <stdint.h>
#include <stdbool.h>

/** @defgroup DATA_UART_CMD Data Uart Command Module
  * @{
  */
/** @defgroup USER_CMD_PARSE User Command Parse Module
  * @{
  */
/*============================================================================*
 *                         Micros
 *============================================================================*/
/** @defgroup USER_CMD_PARSE_Exported_Micros User Command Parse Module Exported Micros
    @brief  command parse related macros.
    * @{
    */

#define USER_CMD_MAX_COMMAND_LINE       70  /**< max. length of command line in bytes */
#define USER_CMD_MAX_HISTORY_LINE       3   /**< max. num of history command line */
#define USER_CMD_MAX_PARAMETERS         8   /**< max. number of parameters that the parser will scan */
/** End of USER_CMD_PARSE_Exported_Micros
  * @}
  */


/*============================================================================*
 *                         Types
 *============================================================================*/
/** @defgroup USER_CMD_PARSE_Exported_Types User Command Parse Module Exported Types
    * @{
    */

/**
* @brief Data UART command parse result.
*
* This is the structure where the command line parser puts its result.
*/
typedef enum
{
    RESULT_SUCESS,                             //!< Operation success.
    RESULT_GAP_CAUSE_ALREADY_IN_REQ     = 0x01,//!< Operation already in progress.
    RESULT_GAP_CAUSE_INVALID_STATE      = 0x02,//!< Invalid state.
    RESULT_GAP_CAUSE_INVALID_PARAM      = 0x03,//!< Invalid parameter.
    RESULT_GAP_CAUSE_NON_CONN           = 0x04,//!< No connection establishment.
    RESULT_GAP_CAUSE_NOT_FIND_IRK       = 0x05,//!< IRK not found.
    RESULT_GAP_CAUSE_ERROR_CREDITS      = 0x06,//!< Credits error.
    RESULT_GAP_CAUSE_SEND_REQ_FAILED    = 0x07,//!< Send Request failed.
    RESULT_GAP_CAUSE_NO_RESOURCE        = 0x08,//!< No resource.
    RESULT_GAP_CAUSE_INVALID_PDU_SIZE   = 0x09,//!< Invalid PDU size.
    RESULT_GAP_CAUSE_NOT_FIND           = 0x0a,//!< Not Found.
    RESULT_GAP_CAUSE_CONN_LIMIT         = 0x0b,//!< Connection reachs limited count.

    RESULT_ERR                          = 0x20,
    RESULT_CMD_EMPTY_LINE               = 0x21,
    RESULT_CMD_NOT_FOUND                = 0x22,
    RESULT_CMD_ERR_PARAM                = 0x23,
    RESULT_CMD_ERR_PARAM_NUM            = 0x24,
    RESULT_CMD_OUT_OF_RANGE             = 0x25,
    RESULT_CMD_NOT_SUPPORT              = 0x26,
    RESULT_GAP_CAUSE_ERROR_UNKNOWN      = 0xFF,//!< Unknown error.
} T_USER_CMD_PARSE_RESULT;

/**
 * @brief Data UART command parse value.
 *
 * This is the structure where the command line parser puts its values.
 */
typedef struct
{
    char       *p_cmd;                              /**< pointer to command */
    int32_t     param_count;                        /**< number of found parameters */
    uint32_t    dw_param[USER_CMD_MAX_PARAMETERS];  /**< automatically parsed parameters */
    char       *p_param[USER_CMD_MAX_PARAMETERS];   /**< automatically parsed parameters */
} T_USER_CMD_PARSED_VALUE;

/** @brief Command interface. */
typedef struct
{
    char      cmdline_buf[USER_CMD_MAX_COMMAND_LINE + 2];
    uint8_t   cmd_cur;
    uint8_t   cmd_history[USER_CMD_MAX_HISTORY_LINE][USER_CMD_MAX_COMMAND_LINE + 2];
    uint8_t   cmd_history_len[USER_CMD_MAX_HISTORY_LINE];
    uint8_t   history_head;
    uint8_t   history_tail;
    uint8_t   history_cur;
    int32_t   accum_cmd_len;            /**< accumulated length of command */
} T_USER_CMD_IF;

/** @brief Prototype of functions that can be called from command table. */
typedef T_USER_CMD_PARSE_RESULT(*T_USER_CMD_FUNC)(T_USER_CMD_PARSED_VALUE *p_parse_value);

/**
 * @brief Command table entry.
 *
 */
typedef struct
{
    char               *p_cmd;
    char               *p_option;
    char               *p_help;
    T_USER_CMD_FUNC     func;
} T_USER_CMD_TABLE_ENTRY;
/** End of USER_CMD_PARSE_Exported_Types
  * @}
  */

/*============================================================================*
 *                         Functions
 *============================================================================*/
/** @defgroup USER_CMD_PARSE_Exported_Functions User Command Parse Module Exported Functions
    * @{
    */

/**
 * @brief  Initiate command interface structure
 * @param[in] p_user_cmd_if   Store parsed commands.
 * @param[in] project_name    Initiate project name.
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
        user_cmd_init(&user_cmd_if, "central");
        ......
    }
 * \endcode
 */
void user_cmd_init(T_USER_CMD_IF *p_user_cmd_if, char *project_name);

/**
 * @brief  Collect command characters.
 *
 * @param[in] p_user_cmd_if   Store parsed commands.
 * @param[in] p_data          Data to be parsed.
 * @param[in] len             Length of data to be command parsed.
 * @param[in] p_cmd_table     Command table to execute function.
 * @return  Command collect result.
 * @retval 1 true.
 * @retval 0 false.
 *
 * <b>Example usage</b>
 * \code{.c}
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
bool user_cmd_collect(T_USER_CMD_IF *p_user_cmd_if, uint8_t *p_data, int32_t len,
                      const T_USER_CMD_TABLE_ENTRY *p_cmd_table);
/** End of USER_CMD_PARSE_Exported_Functions
  * @}
  */

/** @} */ /* End of group USER_CMD_PARSE */
/** @} */ /* End of group DATA_UART_CMD */
#ifdef  __cplusplus
}
#endif  /*  __cplusplus */

#endif  /* __USER_CMD_PARSE_CMD_H */


/**************************************************************************//**
 * @file     icc_api.c
 * @brief    This file defines the data types and macro for ICC API implementation.
 * 
 * @version  V1.00
 * @date     2017-06-05
 *
 * @note
 *
 ******************************************************************************
 *
 * Copyright(c) 2007 - 2016 Realtek Corporation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
 *
 ******************************************************************************/

#ifndef MBED_ICC_API_H
#define MBED_ICC_API_H
#if (defined(CONFIG_PLATFORM_8195BHP) && (CONFIG_PLATFORM_8195BHP == 1))  ||  (defined(CONFIG_PLATFORM_8195BLP) && (CONFIG_PLATFORM_8195BLP == 1))
///@name AmebaPro Only 
///@{

#include "device.h"

#ifdef __cplusplus
extern "C" {
#endif
/** @addtogroup icc ICC
 *  @ingroup    hal
 *  @brief      ICC(Inter CPU Communication) HAL APIs
 *  @{
 */

/**
  \brief  ICC message queue ID.
*/
typedef enum icc_api_msg_qid_e {
    ICC_MsgHQ           = 0,  ///<  higher priority message queue, data is this queue will be processed first
    ICC_MsgLQ           = 1   ///<   lower priority message queue
} icc_msg_qid_t, *picc_msg_qid_t;

/**
  \brief  Default type for ICC user application command word.
*/
typedef struct icc_user_cmd_s {
    union {
        uint32_t cmd_w;
        struct {
            uint32_t para0:24;  ///< command parameter 0. User application can re-define this field
            uint32_t cmd:8;     ///< command type, bit[31:24] is dedicated for the command type.
        } cmd_b;
    };
} icc_user_cmd_t, *picc_user_cmd_t;

#if 0
/**
  \brief  Default type for ICC user application message word.
*/
typedef struct icc_user_msg_s{
    union {
        uint32_t msg_w;
        struct {
            icc_msg_qid_t msg_q:8;  ///< message queue selection, bit[7:0] is dedicated for the message queue ID.
            uint32_t msg_op:8;      ///< message operland. User application can re-define this field.
            uint32_t msg_type:8;    ///< message type, bit[23:13] is dedicated for the message type.
            uint32_t cmd:8; ///< command type, bit[31:24] is reserved for the command type. User application should not use this field.
        } msg_b;
    };
} icc_user_msg_t, *picc_user_msg_t;
#endif

/**
  \brief  The function type of the ICC command callback function.
*/
typedef void (*icc_user_cmd_callback_t)(uint32_t icc_cmd, uint32_t icc_cmd_op, uint32_t cb_arg);
/**
  \brief  The function type of the ICC message callback function.
*/
typedef void (*icc_user_msg_callback_t)(uint8_t *pmsg_buf, uint32_t msg_size, uint32_t cb_arg);

/**
  \brief  The function type of the message data transfer complete callback function.
*/
typedef void (*icc_msg_tx_callback_t)(uint32_t cb_arg);

/** 
 *  @brief Initials the ICC (Inter CPU Communication) object. 
 *         The secure region ICC object should be initialed first.
 *
 *  @returns    void.
 */
void icc_init (void);

/** 
 *  @brief Disables the ICC interface.
 *
 *  @returns void.
 */
void icc_deinit (void);

/** 
 *  @brief Submits a message RX request with RX data buffer to the ICC HAL. The RX requests 
 *         are used to receive messages from HS platform. 
 *  @param[in]  rx_req  The message RX request will to be added to the message RX request pending queue.
 *  @param[in]  pbuf  The data buffer for ICC message data receiving.
 *  @param[in]  size  The size, in byte, of the data buffer.
 *
 *  @return     void.
 */
void icc_msg_rx_req_submit (icc_msg_rx_req_t *rx_req, uint8_t *pbuf, uint32_t size);

/** 
 *  @brief Registers(add) an ICC command to the user application ICC command table.
 *
 *  @param[in]  cmd  The user application ICC command type to be registered.
 *  @param[in]  callback  The handler function of this user application ICC command type.
 *  @param[in]  cb_arg  The application data will be passed back to the application with the callback function.
 *
 *  @return     void.
 */
void icc_cmd_register(uint8_t cmd, icc_user_cmd_callback_t callback, uint32_t cb_arg);

/** 
 *  @brief Unregisters(remove) an ICC command from the user application ICC command table.
 *
 *  @param[in]  cmd  The ICC command type to be removed from the user application ICC command table.
 *
 *  @returns    void.
 */
void icc_cmd_unregister(uint8_t cmd);

/** 
 *  @brief Sends an ICC command to the LS platform.
 *
 *  @param[in]  cmd  The ICC command word 0, it contains the command type and the data word0.
 *  @param[in]  cmd_para1  The ICC command parameter(data word1).
 *  @param[in]  timeout_us  The period, in micro-second, to wait the completion of
 *                          command sending.
 *                            -Value 0: no wait.
 *                            -Value 0xFFFFFFFF: wait forever.
 *  @param[in]  task_yield  The OS task yield function. The ICC HAL will call this function
 *                          while wait for the command sending is finished.
 *
 *  @return     void.
 */
void icc_cmd_send (uint32_t cmd_w, uint32_t cmd_para1, uint32_t timeout_us, void *task_yield);

/**
 *  @brief Registers(adds) an ICC user application message to the
 *         ICC user application message table.
 *
 *  @param[in]  msg_type  The ICC message type to be registered.
 *  @param[in]  frame  The frame type(category) of this ICC message.
 *  @param[in]  callback  The ICC message handler function of this new added message type.
 *  @param[in]  cb_arg  The application data will be passed back to the application
 *                      with the callback function.
 *
 *  @return     void.
 */
void icc_msg_register (uint8_t msg_type, uint8_t frame,
                          icc_user_msg_callback_t callback, uint32_t cb_arg);

/** 
 *  @brief Un-register(remove) an user application ICC message from the
 *         user application ICC message table.
 *
 *  @param[in]  msg  The user application ICC message type to be removed
 *                   from the user application ICC message table.
 *  @param[in]  frame  The frame type(category) of this ICC message.
 *
 *  @returns    void.
 */
void icc_msg_unregister (uint8_t msg, uint8_t frame);

/** 
 *  @brief Sends a ICC message (with message data) to peer.
 *  @param[in]  msg_type  The message type of the ICC message to be send.
 *  @param[in]  frame_type  The frame type(category) of the message type.
 *  @param[in]  q_id  The message queue ID. It assign the message queue to send this message(with its data).
 *  @param[in]  msg_data  The buffer of the message data to be send.
 *  @param[in]  msg_len  The length(in byte) of data in the buffer msg_data to be send.
 *  @param[in]  callback  The call-back function for the message data transfer is done.
 *  @param[in]  callback_arg  The argument of the call-back function.
 *
 *  @return     0: Message send OK. (the TX request adds to the TX pending queue OK).
 *  @return     < 0: Gor error on Message sending.
 */
int32_t icc_msg_tx_submit (uint8_t msg_type, uint8_t frame_type, uint8_t *msg_data, uint16_t msg_len,
                    icc_msg_tx_callback_t callback, uint32_t callback_arg);


/** @} */ /* End of group icc */

#ifdef __cplusplus
}
#endif

///@}
#endif  // end of "#if defined(CONFIG_PLATFORM_8195BHP) || defined(CONFIG_PLATFORM_8195BLP)"
#endif  // end of "#ifndef MBED_ICC_API_H"


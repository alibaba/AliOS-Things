/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#ifndef __HTTPD_H__
#define __HTTPD_H__
/**
 * @addtogroup HTTPD
 * @{
 * This section describes the HTTP daemon (HTTPD) APIs.\n
 * HTTPD is an information technology that processes requests using HTTP protocol, the basic network protocol used to distribute information on the World Wide Web.
 * The term can refer either to the entire computer system, an appliance,
 * or specifically to the software that accepts and supervises the HTTP requests.

 * @section httpd_api_usage How to use this module.
 *
 * - Step1: Mandatory, call #httpd_init() to initiate the HTTP server.\n
 * - Step2: Mandatory, call #httpd_start() to start the HTTP server.\n
 * - Step2: Optional, call #httpd_stop() to stop the HTTP server.\n
 *  - Sample code:
 *     @code
 *          #define CASE_SUCC 1
 *          #define CASE_FAIL -1
 *
 *          static xQueueHandle mbox = NULL;
 *          static int httpd_fb_handle(void);
 *
 *          int main(int argc, char* agrv[])
 *          {
 *              HTTPD_RESULT result = HTTPD_RESULT_UNINIT;
 *              int case_result = CASE_FAIL;
 *              httpd_para parameter;
 *
 *              result = httpd_init();
 *
 *              switch (result) {
 *                  case HTTPD_RESULT_SUCCESS:    //  The operation completed successfully.
 *                      break;
 *                  case HTTPD_RESULT_WAITING:    //  The user needs to wait for a feedback or polling status update.
 *                  case HTTPD_RESULT_UNINIT:     //  The server did not initiate.
 *                  case HTTPD_RESULT_QUEUE_FULL: //  The operation cannot be completed, the server queue is full.
 *                      return case_result;
 *              }
 *
 *              mbox = xQueueCreate(4, sizeof(httpd_fb));
 *              if (mbox == NULL) {
 *                  return case_result;
 *              }
 *
 *              memset(&parameter, 0, sizeof(httpd_para));
 *              parameter.fb_queue = mbox;
 *              result = httpd_start(&parameter);
 *
 *              switch (result) {
 *                  case HTTPD_RESULT_SUCCESS:    // The operation completed successfully.
 *                      case_result = CASE_SUCC;
 *                      break;
 *                  case HTTPD_RESULT_WAITING:    // The user needs to wait for a feedback or polling status update.
 *                      case_result = httpd_fb_handle();
 *                      break;
 *                  case HTTPD_RESULT_UNINIT:     // The server did not initiate.
 *                  case HTTPD_RESULT_QUEUE_FULL: // The operation cannot be completed, the server queue is full.
 *                      break;
 *              }
 *
 *              vQueueDelete(mbox);
 *              mbox = NULL;
 *
 *              return case_result;
 *          }
 *
 *          static int httpd_fb_handle(void)  // Waiting for a feedback.
 *          {
 *              int delay_num = 15;
 *              httpd_fb fb;
 *
 *              for ( ; delay_num > 0; delay_num--) {
 *                  vTaskDelay(1000); // a delay of 1000ms.
 *
 *                  if (xQueueReceive(mbox, (void *)&fb, 0) == pdPASS) {
 *                      switch (fb.status) {
 *                          case HTTPD_STATUS_RUN:
 *                              return CASE_SUCC;  // The server execution is in progress.
 *                          case HTTPD_STATUS_STOPPING:
 *                          case HTTPD_STATUS_STOP:
 *                              break;             // Waiting for a feedback.
 *                          case HTTPD_STATUS_UNINIT:
 *                              return CASE_FAIL;  // The server did not initiate.
 *                      }
 *                  }
 *              }
 *
 *              return CASE_FAIL;
 *          }
 *     @endcode
 */

#include <stdio.h>
#include "osal.h"

/** @defgroup httpd_enum Enum
  * @{
  */

/**
 *  @brief HTTPD result, defines the API return type.\n
 */
typedef enum
{
    HTTPD_RESULT_SUCCESS,   /**< The operation completed successfully. \n*/
    HTTPD_RESULT_WAITING,   /**< The user needs to wait for a feedback or polling status update. \n*/
    HTTPD_RESULT_UNINIT,    /**< The server did not initiate. \n*/
    HTTPD_RESULT_QUEUE_FULL /**< The operation cannot be completed, the server queue is full. \n*/
}HTTPD_RESULT;

/**
 *  @brief HTTPD status, indicates the status of the HTTP server.\n
 */
typedef enum
{
    HTTPD_STATUS_UNINIT,    /**< The server did not initiate. \n*/
    HTTPD_STATUS_RUN,       /**< The server execution is in progress. \n*/
    HTTPD_STATUS_STOPPING,  /**< The server execution is stopping. \n*/
    HTTPD_STATUS_STOP       /**< The server execution has stopped. \n*/
}HTTPD_STATUS;

/**
 *  @brief HTTPD stop reason, indicates the reason why the HTTP server has stopped. It is used only when #HTTPD_STATUS is #HTTPD_STATUS_STOP.\n
 */
typedef enum
{
    HTTPD_STOP_REASON_NO_MEMORY = -1,           /**< There is not enough memory available for the server operation. \n*/
    HTTPD_STOP_REASON_BEARER_CONN_FAIL = -2,    /**< The socket create or bind failed. \n*/
    HTTPD_STOP_REASON_BEARER_DISCONN = -3,      /**< The listen socket pipe is broken. \n*/
    HTTPD_STOP_REASON_BY_COMMAND = -4           /**< The user requested the server to stop. \n*/
}HTTPD_STOP_REASON;

/**
  * @}
  */

/** @defgroup httpd_struct Struct
  * @{
  */

/**
 *  @brief Configuration parameter details of the HTTP server.\n
 */
typedef struct
{
    char str_ip_addr[16];       /**< The IP address of the HTTP server's bind or listen operation. \n*/
    char str_port[6];           /**< The HTTP server's bind or listen port. \n*/
    OsBufQ fb_queue;     /**< The queue handle to receive the server's feedback. \n*/
}httpd_para;

/**
 *  @brief A feedback structure for the HTTP server, provides a feedback when the status is changed.\n
 */
typedef struct
{
    HTTPD_STATUS status;        /**< The HTTP server's status. \n*/
    HTTPD_STOP_REASON reason;   /**< The server has stopped with a status of #HTTPD_STATUS_STOP. \n*/
}httpd_fb;

/**
  * @}
  */

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief       This function initiates the HTTP server.
 * @return      Provides the function call result. \n
 *              If the return value is #HTTPD_RESULT_SUCCESS, the operation completed successfully. \n
 *              No other result is expected.
 * @par         Example
 * @code
 *          // initiate HTTP server
 *          HTTPD_RESULT result;
 *          result = httpd_init();
 *          switch (result) {
 *              case HTTPD_RESULT_SUCCESS:     // The operation completed successfully.
 *                  printf("\n\nHTTPD INIT - Success\n\n");
 *                  break;
 *              case HTTPD_RESULT_WAITING:     // The user needs to wait for a feedback or polling status update.
 *              case HTTPD_RESULT_UNINIT:      // The server did not initiate.
 *              case HTTPD_RESULT_QUEUE_FULL:  // The operation cannot be completed, the server queue is full.
 *                  break;
 *          }
 *          break;
 * @endcode
 */
HTTPD_RESULT httpd_init(void);

/**
 * @brief       This function starts the HTTP server.
 * @param[in]   parameter is configuration parameter to start the server. If it is NULL, the server starts with default setting.
 * @return      Provides the function call result. \n
 *              If the return value is #HTTPD_RESULT_SUCCESS, the operation completed successfully; \n
 *              If the return value is #HTTPD_RESULT_WAITING, the user needs to wait for a feedback or polling status update; \n
 *              If the return value is #HTTPD_RESULT_UNINIT, the server did not initiate; \n
 *              If the return value is #HTTPD_RESULT_QUEUE_FULL, the server queue is full, the operation cannot be completed.
 * @par         Example
 * @code
 *          #define CASE_SUCC 1
 *          #define CASE_FAIL -1
 *
 *          static xQueueHandle mbox = NULL;
 *          static int httpd_fb_handle(void);
 *
 *          int main(int argc, char* agrv[])
 *          {
 *              HTTPD_RESULT result = HTTPD_RESULT_UNINIT;
 *              int case_result = CASE_FAIL;
 *              httpd_para parameter;
 *
 *              result = httpd_init();
 *
 *              switch (result) {
 *                  case HTTPD_RESULT_SUCCESS:    //  The operation completed successfully.
 *                      break;
 *                  case HTTPD_RESULT_WAITING:    //  The user needs to wait for a feedback or polling status update.
 *                  case HTTPD_RESULT_UNINIT:     //  The server did not initiate.
 *                  case HTTPD_RESULT_QUEUE_FULL: //  The operation cannot be completed, the server queue is full.
 *                      return case_result;
 *              }
 *
 *              mbox = xQueueCreate(4, sizeof(httpd_fb));
 *              if (mbox == NULL) {
 *                  return case_result;
 *              }
 *
 *              memset(&parameter, 0, sizeof(httpd_para));
 *              parameter.fb_queue = mbox;
 *              result = httpd_start(&parameter);
 *
 *              switch (result) {
 *                  case HTTPD_RESULT_SUCCESS:    // The operation completed successfully.
 *                      case_result = CASE_SUCC;
 *                      break;
 *                  case HTTPD_RESULT_WAITING:    // The user needs to wait for a feedback or polling status update.
 *                      case_result = httpd_fb_handle();
 *                      break;
 *                  case HTTPD_RESULT_UNINIT:     // The server did not initiate.
 *                  case HTTPD_RESULT_QUEUE_FULL: // The operation cannot be completed, the server queue is full.
 *                      break;
 *              }
 *
 *              vQueueDelete(mbox);
 *              mbox = NULL;
 *
 *              return case_result;
 *          }
 *
 *          static int httpd_fb_handle(void)  // Waiting for a feedback.
 *          {
 *              int delay_num = 15;
 *              httpd_fb fb;
 *
 *              for ( ; delay_num > 0; delay_num--) {
 *                  vTaskDelay(1000); // a delay of 1000ms.
 *
 *                  if (xQueueReceive(mbox, (void *)&fb, 0) == pdPASS) {
 *                      switch (fb.status) {
 *                          case HTTPD_STATUS_RUN:
 *                              return CASE_SUCC;  // The server execution is in progress.
 *                          case HTTPD_STATUS_STOPPING:
 *                          case HTTPD_STATUS_STOP:
 *                              break;             // Waiting for a feedback.
 *                          case HTTPD_STATUS_UNINIT:
 *                              return CASE_FAIL;  // The server did not initiate.
 *                      }
 *                  }
 *              }
 *
 *              return CASE_FAIL;
 *          }
 * @endcode
 */
HTTPD_RESULT httpd_start(httpd_para *parameter);

/**
 * @brief       This function stops the HTTP server.
 * @return      Provides the function call result. \n
 *              If the return value is #HTTPD_RESULT_SUCCESS, the operation completed successfully; \n
 *              If the return value is #HTTPD_RESULT_WAITING, the user needs to wait for a feedback or polling status update; \n
 *              If the return value is #HTTPD_RESULT_UNINIT, the server did not initiate; \n
 *              If the return value is #HTTPD_RESULT_QUEUE_FULL, the server queue is full, the operation cannot be completed.
 * @par         Example
 * @code
 *          #define CASE_SUCC 1
 *          #define CASE_FAIL -1
 *
 *          static int httpd_poll_status(void);
 *
 *          int main(int argc, char* agrv[])
 *          {
 *              HTTPD_RESULT result = HTTPD_RESULT_UNINIT;
 *              int case_result = CASE_FAIL;
 *              httpd_para parameter;
 *
 *              result = httpd_init();
 *
 *              switch (result) {
 *                  case HTTPD_RESULT_SUCCESS:    //  The operation completed successfully.
 *                      break;
 *                  case HTTPD_RESULT_WAITING:    //  The user needs to wait for a feedback or poll the status.
 *                  case HTTPD_RESULT_UNINIT:     //  The server did not initiate.
 *                  case HTTPD_RESULT_QUEUE_FULL: //  The operation cannot be completed, the server queue is full.
 *                      return case_result;
 *              }
 *
 *              result = httpd_stop();
 *
 *              switch (result) {
 *                  case HTTPD_RESULT_SUCCESS:    // The operation completed successfully.
 *                      case_result = CASE_SUCC;
 *                      break;
 *                  case HTTPD_RESULT_WAITING:    // The user needs to wait for a feedback or polling status update.
 *                      case_result = httpd_poll_status();
 *                      break;
 *                  case HTTPD_RESULT_UNINIT:     // The server did not initiate.
 *                  case HTTPD_RESULT_QUEUE_FULL: // The operation cannot be completed, the server queue is full.
 *                      break;
 *              }
 *
 *              return case_result;
 *          }
 *
 *          static int httpd_poll_status(void)  // Polling status update.
 *          {
 *              int delay_num = 15;
 *              HTTPD_STATUS status = HTTPD_STATUS_UNINIT;
 *
 *              for ( ; delay_num > 0; delay_num--) {
 *                  vTaskDelay(1000); // a delay of 1000ms.
 *
 *                  status = httpd_get_status();
 *
 *                  switch (status) {
 *                      case HTTPD_STATUS_STOP:
 *                          return CASE_SUCC;   // The server execution has stopped.
 *                      case HTTPD_STATUS_STOPPING:
 *                      case HTTPD_STATUS_RUN:
 *                          break;              // Polling status is required.
 *                      case HTTPD_STATUS_UNINIT:
 *                          return CASE_FAIL;   // The server did not initiate.
 *                  }
 *              }
 *
 *              return CASE_FAIL;
 *          }
 * @endcode
 */
HTTPD_RESULT httpd_stop(void);

/**
 * @brief       This function queries the HTTP server's status. User can poll the server status instead of receiving a feedback.
 * @return      Provides the function call result. \n
 *              If the return value is #HTTPD_STATUS_UNINIT, the server did not initiate; \n
 *              If the return value is #HTTPD_STATUS_RUN, the server execution is in progress; \n
 *              If the return value is #HTTPD_STATUS_STOPPING, the server execution is stopping; \n
 *              If the return value is #HTTPD_STATUS_STOP, the server execution has stopped.
 * @par         Example
 * @code
 *          // query HTTP server status
 *          HTTPD_STATUS status;
 *          httpd_para para;
 *          status = HTTPD_STATUS();
 *          switch (status) {
 *              case HTTPD_STATUS_UNINIT:
 *                  printf("\n\n[HTTPDX] HTTPD GET STATUS - the server did not initiate\n\n");
 *                  break;
 *              case HTTPD_STATUS_RUN:
 *                  printf("\n\n[HTTPDX] HTTPD GET STATUS - the server execution is in progress\n\n");
 *                  break;
 *              case HTTPD_STATUS_STOP:
 *                  printf("\n\n[HTTPDX] HTTPD GET STATUS - the server execution has stopped.\n\n");
 *                  break;
 *              case HTTPD_STATUS_STOPPING:
 *                  printf("\n\n[HTTPDX] HTTPD GET STATUS - the server execution is stopping. Please wait for a  feedback or polling status update.\n\n");
 *                  break;
 *          }
 *          break;
 * @endcode
 */
HTTPD_STATUS httpd_get_status(void);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* __HTTPD_H__ */


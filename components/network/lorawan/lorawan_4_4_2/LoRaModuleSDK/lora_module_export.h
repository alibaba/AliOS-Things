/*
 * Copyright (c) 2018 Alibaba Group. All rights reserved.
 * License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#ifndef LORA_MODULE_EXPORT_H
#define LORA_MODULE_EXPORT_H

#include "connection.h"
#include "LoRaMac.h"

/*!
 * LoRa Module Status type
 */
typedef enum{
    LoRa_Initing = 0,
    LoRa_Init_Done,
    LoRa_Joining,
    LoRa_Join_Failed,
    LoRa_Join_Success,
    LoRa_Class_Switching,//valid only for switching to classB
    LoRa_Class_SwitchOK, //valid only for switching to classC/A
    LoRa_Class_SwitchNOK, 
    LoRa_Idle,           //being ready to send data
    LoRa_Sending
}LoRa_Status;

/*!
 * LoRa_Semaph_Release_Source type
 */
typedef enum{
    LoRa_Task = 0,
    RTC_IRQ,
    Radio_IRQ
}LoRaSemaphRelSource;

/*  function prototypes -----------------------------------------------*/

/*!
 * \brief   lorawan module initialization
 *
 * \details In addition to the initialization of the lorawan module, this
 *          function initializes the callback of APP services. Must be
 *          set to a valid callback function.
 * \param   [IN] .
 *
 * \retval  Possible returns are:
 *          -1:init failed.
 *           0:init succesful.
 */
extern int  lorawan_module_init(esl_connection_ops_cb_t* _esl_ops_cb, void* data);
/*!
 * \brief   lorawan module deinitialization
 *
 * \param   [IN] .
 *
 * \retval  Possible returns are:
 *          -1:deinit failed.
 *           0:deinit succesful.
 */
extern int  lorawan_module_deinit(void);
/*!
 * \brief   lorawan module send data, must loar module status be connected,see on_connect() CB funtion.
 *
 * \param   [IN] .
 *
 * \retval  Possible returns are:
 *          -1:send failed.
 *           0:send succesful.
 */
extern int  lorawan_module_send(int is_confirm, int app_port, const char* data, int len, handle_send_cb_fp_t cb);
/*!
 * \brief   get the latest downlink rssi and snr from lorawan module.
 *
 * \param   [IN] .
 *
 * \retval  Possible returns are:
 *          -1:get failed.
 *           0:get succesful.
 */
extern int  lorawan_module_get_rssi_snr(char* rssi, char* snr);
/*!
 * \brief   get the latest number of Uplink and Downlink package from lorawan module.
 *
 * \param   [IN] .
 *
 * \retval  Possible returns are:
 *          -1:get failed.
 *           0:get succesful.
 */
extern int lorawan_module_get_num_tx_rx(uint16_t* tx_num, uint16_t* rx_num);

/*!
 * \brief   trigger lorawan module to join.
 *          Note:
 *
 * \param   [IN] .
 *
 * \retval  Possible returns are:
 *          -1:trigger failed.
 *           0:trigger succesful, join result will be know later.
 */
extern int  lorawan_module_join(void);
/*!
 * \brief   trigger lorawan module to switch class type.
 *          Note:Should be Join successful and module status is LoRa_Join_Success or LoRa_Idle before starting to switch class
 *
 * \param   [IN] .
 *
 * \retval  Possible returns are:
 *          success:
 *          LoRa_Class_Switching,//valid only for switching to classB
 *          LoRa_Class_SwitchOK, //valid only for switching to classC/A
 *          failed:
 *          other status
 */
extern LoRa_Status lorawan_module_switch_class(DeviceClass_t newClass);

/*!
 * \brief   get current lorawan module status.
 *          Note:
 *
 * \param   [IN] .
 *
 * \retval  Possible returns are:
 *          see LoRa_Status.
 */
extern LoRa_Status lorawan_module_get_status(void);
/*!
 * \brief   get the join result from lorawan module.
 *
 * \param   [IN] .
 *
 * \retval  Possible returns are:
 *           0:join not ok.
 *           1:join successful.
 */
extern int lorawan_module_get_join_status(void);

/*!
 * \brief    lorawan module main process, get CPU to process lora event;
 *
 * \param   [IN] .
 *
 * \retval
 */
extern void lorawan_module_main(void const *arg);

/*!
 * \brief    [RTOS] other task activates lorawan module task
 *
 * \param   [IN] activate: 0: de-activate, non-0：activate
 *
 * \retval  Possible returns are:
 *          -1: fail.
 *           0: succesful.
 */
extern int lorawan_module_activate(int activate);


/*!
 * \brief    [RTOS] Radio/RTC interrupt trigger lorawan module task to work
 *
 * \param   [IN] interrupt Source
 *
 * \retval  Possible returns are:
 *          OS_OK.
 *          not OS_OK.
 */
extern int lora_interrupt_semaphore_release(LoRaSemaphRelSource releaseSource);

typedef int (*rx_cb_t)(uint8_t *data, int len);

/*!
 * \brief    register lorawan app protocol rx callback
 *
 * \param   [IN] port: port number.
 * \param   [IN] rx_cb_t: rx callback.
 *
 * \retval  Possible returns are:
 *          -1: fail.
 *           0: succesful.
 */
extern int lorawan_module_reg_app_rx_cb(uint8_t port, rx_cb_t rx_cb);

/*!
 * \brief   lorawan multicast initialization 
 *
 * \details  Initialize the multicast SDK, and set preset multicast channels and
 *          dynamic multicast channels.User can change the function to voluntarily 
 *          assign the multicast channel.
 *
 * \retval  Possible returns are:
 *          -1: fail.
 *           0: succesful.
 */
extern int lorawan_mc_setup(void);

#endif /* LORA_MODULE_EXPORT_H */

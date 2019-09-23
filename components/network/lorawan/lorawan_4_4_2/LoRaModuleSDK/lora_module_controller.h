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
#ifndef LORA_MODULE_CONTROLLER_H
#define LORA_MODULE_CONTROLLER_H

#include "connection.h"

#define osOK                    (1) //cmsis_os.h

/*!
 * Device states
 */
typedef enum eDeviceState
{
    DEVICE_STATE_REINIT,
    DEVICE_STATE_RESTORE,
    DEVICE_STATE_START,
    DEVICE_STATE_JOIN,
    DEVICE_STATE_REJOIN,
    DEVICE_STATE_SEND,
    DEVICE_STATE_REQ_DEVICE_TIME,
    DEVICE_STATE_REQ_PINGSLOT_ACK,
    DEVICE_STATE_REQ_BEACON_TIMING,
    DEVICE_STATE_BEACON_ACQUISITION,
    DEVICE_STATE_SWITCH_CLASS,
    DEVICE_STATE_CYCLE,
    DEVICE_STATE_SLEEP
}DeviceState_t;

int lorawan_init(esl_connection_ops_cb_t* _esl_cm_ops_cb, void* data);
int lorawan_deinit(void);
DeviceState_t LORA_DeviceProcess(void);

#endif /* LORA_MODULE_CONTROLLER_H */

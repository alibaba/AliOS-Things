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
#ifndef CONNECTION_H
#define CONNECTION_H

#include <stdint.h>
#include "lora_config.h"

typedef void (*handle_send_cb_fp_t)(int success, void* ctx);

typedef enum e_esl_data_type {
    /*!
     * unicast
     */
    ESL_UNICAST_DATA = 0,
    /*!
     * multicast
     */
    ESL_MULTICAST_DATA,
} esl_data_type_t;

/*!
 * \brief   Callback Function which lorawan module can notify the status change result to App layer.
 *          Register them in lorawan_module_init() if you care.
 */
typedef struct _esl_connection_ops_cb {
    int (*on_connect)(int success, void* ctx); //LoRa classA:join result;LoRa classB:join and switch result;
    int (*on_disconnect)(void* ctx);
    int (*lora_classbswitch_result)(int success,void* ctx);
    int (*data_arrived)(esl_data_type_t type, uint8_t port, uint8_t* data, int len, void* ctx);
} esl_connection_ops_cb_t;

#endif /* CONNECTION_H */

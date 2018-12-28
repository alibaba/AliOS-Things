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

#ifndef _LORA_MULTICAST_CONTROL_H_
#define _LORA_MULTICAST_CONTROL_H_

#include "LoRaMacMulticast.h"

/*!
 * \brief   lorawan multicast initialization
 *
 * \details This function initializes multicast app protocol.
 *
 * \param   mcCmdPort: dynamic mc command fport
 *                 mcGenAppKey: multicast Genappkey
 *
 * \retval  Possible returns are:
 *          -1:init failed.
 *           0:init succesful.
 */
int lorawan_mc_init(uint8_t mcCmdPort, uint8_t* mcGenAppKey);

/*!
 * \brief   lorawan create a preset multicast channel
 *
 * \param  groupId: multicaset channel id.
 *                mckey: multicaset channel mckey
 *
 * \retval  Possible returns are:
 *          -1:init failed.
 *           0:init succesful.
 */
int lorawan_mc_set_preset_channel(uint8_t groupId, mcChannel_t* channel, mcKey_t* mckey);

/*!
 * \brief   lorawan create a dynamic multicast channel
 *
 * \param   groupId: multicaset channel id.
 *
 * \retval  Possible returns are:
 *          -1:init failed.
 *           0:init succesful.
 */
int lorawan_mc_set_dynamic_channel(uint8_t groupId);

/*!
 * \brief   lorawan send dynamic multicast commands answer 
 *
 * \param   NULL.
 *
 * \retval  Possible returns are:
 *          -1:init failed.
 *           0:init succesful.
 */
int lorawan_mc_cmd_ans( void );

/*!
 * \brief   lorawan process  dynamic multicast commands received 
 *
 * \param   buff:mc command buffer.
 *                cmdSize:buffer length contain mc command
 *
 * \retval  Possible returns are:
 *          -1:init failed.
 *           0:init succesful.
 */
int lorawan_mc_process_cmd(uint8_t *buff, uint8_t cmdSize);

/*!
 * \brief   lorawan get the last multicast sdk error code
 *
 * \param   NULL.
 *
 * \retval  Possible returns are:
 *          -1:init failed.
 *           0:init succesful.
 */
int lorawan_mc_get_error( void );

#endif

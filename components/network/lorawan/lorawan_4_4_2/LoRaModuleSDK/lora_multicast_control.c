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

#include "LoRaMac.h"
#include "lora_multicast_control.h"

int lorawan_mc_init(uint8_t mcCmdPort, uint8_t* mcGenAppKey)
{
    if (mcGenAppKey == NULL)
    {
        return -1;
    }
    return McInitialization( mcCmdPort, mcGenAppKey );
}

int lorawan_mc_set_preset_channel(uint8_t groupId, mcChannel_t* channel, mcKey_t* mckey)
{
    if (channel == NULL || mckey == NULL)
    {
        return -1;
    }  
    return McCreatePresetChannel(groupId, channel, mckey);
}

int lorawan_mc_set_dynamic_channel(uint8_t groupId)
{
    return McCreateDynamicChannel(groupId);
}

int lorawan_mc_cmd_ans( void )
{
    return McSendCmdAns( );
}

int lorawan_mc_process_cmd(uint8_t *buff, uint8_t cmdSize)
{
    return McProcessCmd(buff, cmdSize);
}

int lorawan_mc_get_error( void )
{
    return McGetLastError( );
}

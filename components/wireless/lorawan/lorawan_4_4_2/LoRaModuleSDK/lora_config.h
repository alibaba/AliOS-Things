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

/* lora_config.h Define to prevent recursive inclusion -----------------------*/

#ifndef __LORA_CONFIG_H__
#define __LORA_CONFIG_H__

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "LoRaMac.h"

/* Exported constants --------------------------------------------------------*/

#ifndef ACTIVE_REGION
#define ACTIVE_REGION LORAMAC_REGION_CN470
#endif

/*!
 * LoRaWAN application port
 */
#define LORAWAN_APP_PORT                            (100)

/*!
 * dynamic multicast command port
 */
#define LORA_MULTICAST_CMD_PORT                     (200)

/*!
 * Uncomment to use the deprecated BeaconTiming MAC command
 */
//#define USE_BEACON_TIMING

/*!
 * Defines the application data transmission duty cycle. 10 minutes, value in [ms].
 */
#define APP_TX_DUTYCYCLE                            (60*1000*10) //APP send data in every 10 minutes, must be greater than beacon period 128s.

/*!
 * Defines a random delay for application data transmission duty cycle. 5s,
 * value in [ms].
 */
#define APP_TX_DUTYCYCLE_RND                        5000

/*!
 * Default datarate
 */
#define LORAWAN_DEFAULT_DATARATE                    DR_4

/*!
 * Default ping slots periodicity
 *
 * \remark periodicity is equal to 2^LORAWAN_DEFAULT_PING_SLOT_PERIODICITY seconds
 *         example: 2^3 = 8 seconds. The end-device will open an Rx slot every 8 seconds.
 */
#define LORAWAN_DEFAULT_PING_SLOT_PERIODICITY       2

/*!
 * LoRaWAN confirmed messages
 */
#define LORAWAN_CONFIRMED_MSG_ON                    true

/*!
 * LoRaWAN Adaptive Data Rate
 *
 * \remark Please note that when ADR is enabled the end-device should be static
 */
#define LORAWAN_ADR_ON                              true //default: false;true:just for LinkWAN Certification pass

#define LORAWAN_DUTY_CYCLE_RESTRICT_ON              true
//=====================Commissioning.h=========================

typedef enum node_freq_mode_s
{
    FREQ_MODE_INTRA = 1, // uplink and downlink use same frequencies
    FREQ_MODE_INTER = 2, // uplink and downlink use different frequencies
} node_freq_mode_t;

#define LORA_NODE_FREQ_TPYE                          FREQ_MODE_INTRA

//#define LORA_JOIN_FREQBAND_MASK                    (0x0003)//Default:0x0003(1A1+1A2),0x000C(2A1+2A2),0x3000(3B1+3B2),0xC000(4B1+4B2).
#define LORA_CH_MASK_CONTROL_DEFAULT                 (0)//Default:0:1A1+1A2,1:2A1+2A2,2:3B1+3B2,3:4B1+4B2
#define LORA_CHANNELS_MASK_DEFAULT                   (0xFF00)//chmask:0xFF00 or 0x00FF(0x00FF = FREQBAND_MASK's first FreqGroup,0x00FF = FREQBAND_MASK's second FreqGroup)
#define LORA_ENABLE_SCAN_BETWEEN_2_FREQGROUPS        (0)

//===========start radio parameters config==================
#define LORA_NODE_TARGET_CLASS_TPYE                  CLASS_B

#define LORA_JOIN_TIMEOUT_VALUE                      (35000) //35s

#define LORA_MODULE_SEND_HEARTBEAT                    1 //1: enable cycle data send, 0:disable

#define LORA_NODE_CONFIRM_RETX_NUM                   (2)

#define LORA_NODE_NUM_SEND_FAILED_TO_REJOIN          (3)
#define LORA_NODE_NUM_REJOIN_FAILED_TO_REINIT        (6)
#define LORA_NODE_NUM_CLASS_SWITCH_FAILED_TO_REINIT  (5)

#define LORA_NODE_NUM_PINGSLOTINFORACK_FAILED_TO_REINIT (5)
#define LORA_NODE_NUM_BEACONMISS_TO_SEND_DEVICETIMEREQ  (3)

#define LORA_NODE_DISABLE_JOIN_BACKOFF               (0)
#define LORA_NODE_DISABLE_CLASS_SWITCH_BACKOFF       (1)
#define LORA_MODULE_GET_FLASH_EUI_ENABLE             (0)

//===========start radio parameters config end==================

/* Exported types ------------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

#endif

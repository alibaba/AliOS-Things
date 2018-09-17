 /*
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
    (C)2013 Semtech

Description: LoRaMac classA device implementation

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Miguel Luis, Gregory Cristian and Wael Guibene
*/
/******************************************************************************
  * @file    lora.h
  * @author  MCD Application Team
  * @version V1.1.1
  * @date    01-June-2017
  * @brief   lora API to drive the lora state Machine
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V. 
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef __LORA_MAIN_H__
#define __LORA_MAIN_H__

#ifdef __cplusplus
 extern "C" {
#endif
   
/* Includes ------------------------------------------------------------------*/
#include "Comissioning.h"
#include "LoRaMac.h"
#include "region/Region.h"

/* Exported constants --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/

/*!
 * Application Data structure
 */
typedef struct
{
  /*point to the LoRa App data buffer*/
  uint8_t* Buff;
  /*LoRa App data buffer size*/
  uint8_t BuffSize;
  /*Port on which the LoRa App is data is sent/ received*/
  uint8_t Port;
  
} lora_AppData_t;

/*!
 * LoRa State Machine states 
 */
typedef enum eDevicState
{
    DEVICE_STATE_INIT,
    DEVICE_STATE_JOIN,
    DEVICE_STATE_JOINED,
    DEVICE_STATE_SEND,
    DEVICE_STATE_CYCLE,
    DEVICE_STATE_SLEEP
} DeviceState_t;

/*!
 * LoRa State Machine states 
 */
typedef enum eTxEventType
{
/*!
 * @brief AppdataTransmition issue based on timer every TxDutyCycleTime
 */
    TX_ON_TIMER,
/*!
 * @brief AppdataTransmition external event plugged on OnSendEvent( )
 */
    TX_ON_EVENT
} TxEventType_t;


/*!
 *LoRa Join Mode
 *
 * */

typedef enum eJoinModeType
{
    JOIN_MODE_OTAA,
    JOIN_MODE_ABP
} JoinMode_t;

/*!
 * LoRa State Machine states 
 */
typedef struct sLoRaParam
{
/*!
 * @brief Event type
 *
 * @retval value  battery level ( 0: very low, 254: fully charged )
 */
    TxEventType_t TxEvent;
/*!
 * @brief Application data transmission duty cycle in ms
 *
 * @note when TX_ON_TIMER Event type is selected
 */
    uint32_t TxDutyCycleTime;
/*!
 * @brief LoRaWAN device class
 */
    DeviceClass_t Class;
/*!
 * @brief Activation state of adaptativeDatarate
 */
    bool AdrEnable;
/*!
 * @brief Uplink datarate, if AdrEnable is off
 */
    int8_t TxDatarate;
/*!
 * @brief Enable or disable a public network
 *
 */
    bool EnablePublicNetwork;
/*!
 * @brief Number of trials for the join request.
 */
    uint8_t NbTrials;
/*!
 * @brief Join mode：0, OTAA; 1,ABP.
 */
    JoinMode_t JoinMode;
 
} LoRaParam_t;

/* Lora Main callbacks*/
typedef struct sLoRaMainCallback
{
/*!
 * @brief Get the current battery level
 *
 * @retval value  battery level ( 0: very low, 254: fully charged )
 */
    uint8_t ( *BoardGetBatteryLevel )( void );
  
/*!
 * @brief Gets the board 64 bits unique ID 
 *
 * @param [IN] id Pointer to an array that will contain the Unique ID
 */
    void    ( *BoardGetUniqueId ) ( uint8_t *id);
  /*!
 * Returns a pseudo random seed generated using the MCU Unique ID
 *
 * @retval seed Generated pseudo random seed
 */
    uint32_t ( *BoardGetRandomSeed ) (void);
/*!
 * @brief Prepares Tx Data to be sent on Lora network 
 *
 * @param [IN] AppData is a buffer to fill
 *
 * @param [IN] port is a Application port on wicth Appdata will be sent
 *
 * @param [IN] length of the AppDataBuffer to send
 *
 * @param [IN] requests a confirmed Frame from the Network
 */
  void ( *LoraTxData ) ( lora_AppData_t *AppData, FunctionalState* IsTxConfirmed);
/*!
 * @brief Process Rx Data received from Lora network 
 *
 * @param [IN] AppData is a buffer to process
 *
 * @param [IN] port is a Application port on wicth Appdata will be sent
 *
 * @param [IN] length is the number of recieved bytes
 */
    void ( *LoraRxData ) ( lora_AppData_t *AppData);
  
} LoRaMainCallback_t;



/* External variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */ 
/**
 * @brief Lora Initialisation
 * @param [IN] LoRaMainCallback_t
 * @param [IN] application parmaters
 * @retval none
 */
void lora_Init (LoRaMainCallback_t *callbacks, LoRaParam_t* LoRaParam );

/**
 * @brief run Lora classA state Machine 
 * @param [IN] none
 * @retval none
 */
void lora_fsm( void );

/**
 * @brief functionl requesting loRa state machine to send data 
 * @note function to link in mode TX_ON_EVENT 
 * @param  none
 * @retval none
 */
void OnSendEvent( void );


/**
 * @brief API returns the state of the lora state machine
 * @note return @DeviceState_t state
 * @param [IN] none
 * @retval return @FlagStatus
  */
DeviceState_t lora_getDeviceState( void );


#ifdef __cplusplus
}
#endif

#endif /*__LORA_MAIN_H__*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

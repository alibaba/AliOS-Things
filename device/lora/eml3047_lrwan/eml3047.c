/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

/*
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
    (C)2013 Semtech

Description: SX1276 driver specific target board functions implementation

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Miguel Luis and Gregory Cristian
*/
 /*******************************************************************************
  * @file    eml3047.c
  * @author  MCD Application Team
  * @version V1.0.2
  * @date    01-June-2017
  * @brief   driver LoRa module murata cmwx1zzabz-078
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
  
/* Includes ------------------------------------------------------------------*/

#include "hw.h"
#include "radio.h"
#include "sx1276.h"
#include "eml3047.h"
#include "delay.h"
#include "timeServer.h"

#define IRQ_HIGH_PRIORITY  0

/* Delay in ms between radio set in sleep mode and TCXO off*/
#define TCXO_OFF_DELAY 3

#define TCXO_ON()   //HW_GPIO_Write( RADIO_TCXO_VCC_PORT, RADIO_TCXO_VCC_PIN, 1) 

#define TCXO_OFF()  //HW_GPIO_Write( RADIO_TCXO_VCC_PORT, RADIO_TCXO_VCC_PIN, 0) 

static void SX1276AntSwInit( void );
  
static void SX1276AntSwDeInit( void );

void SX1276SetXO( uint8_t state );

uint32_t SX1276GetWakeTime( void );

void SX1276IoIrqInit( DioIrqHandler **irqHandlers );

void SX1276_SetAntSw( uint8_t opMode );

uint8_t SX1276GetPaSelect( uint32_t channel );

void SX1276SetAntSwLowPower( bool status );

void SX1276SetRfTxPower( int8_t power );

void SX1276SetAntSw( uint8_t opMode );
/*!
 * \brief Controls the antena switch if necessary.
 *
 * \remark see errata note
 *
 * \param [IN] opMode Current radio operating mode
 */
static LoRaBoardCallback_t BoardCallbacks = { SX1276SetXO,
                                              SX1276GetWakeTime,
                                              SX1276IoIrqInit,
                                              SX1276SetRfTxPower,
                                              SX1276SetAntSwLowPower,
                                              SX1276SetAntSw};

/*!
 * Radio driver structure initialization
 */
const struct Radio_s Radio =
{
  SX1276IoInit,
  SX1276IoDeInit,
  SX1276Init,
  SX1276GetStatus,
  SX1276SetModem,
  SX1276SetChannel,
  SX1276IsChannelFree,
  SX1276Random,
  SX1276SetRxConfig,
  SX1276SetTxConfig,
  SX1276CheckRfFrequency,
  SX1276GetTimeOnAir,
  SX1276Send,
  SX1276SetSleep,
  SX1276SetStby, 
  SX1276SetRx,
  SX1276StartCad,
  SX1276SetTxContinuousWave,
  SX1276ReadRssi,
  SX1276Write,
  SX1276Read,
  SX1276WriteBuffer,
  SX1276ReadBuffer,
  SX1276SetSyncWord,
  SX1276SetMaxPayloadLength,
  SX1276GetRadioWakeUpTime
};

uint32_t SX1276GetWakeTime( void )
{
  return  BOARD_WAKEUP_TIME;
}

void SX1276SetXO( uint8_t state )
{

  if (state == SET )
  {
    TCXO_ON(); 
    
    DelayMs( BOARD_WAKEUP_TIME ); //start up time of TCXO
  }
  else
  {
    TCXO_OFF(); 
  }
}
void SX1276IoInit( void )
{
  GPIO_InitTypeDef initStruct={0};
  
  SX1276BoardInit( &BoardCallbacks );
  
  initStruct.Mode =GPIO_MODE_IT_RISING;
  initStruct.Pull = GPIO_PULLUP;
  initStruct.Speed = GPIO_SPEED_HIGH;

  HW_GPIO_Init( RADIO_DIO_0_PORT, RADIO_DIO_0_PIN, &initStruct );
  HW_GPIO_Init( RADIO_DIO_1_PORT, RADIO_DIO_1_PIN, &initStruct );
  HW_GPIO_Init( RADIO_DIO_2_PORT, RADIO_DIO_2_PIN, &initStruct );
  HW_GPIO_Init( RADIO_DIO_3_PORT, RADIO_DIO_3_PIN, &initStruct );
  
  initStruct.Mode =GPIO_MODE_OUTPUT_PP;
  initStruct.Pull = GPIO_NOPULL;  
//  HW_GPIO_Init( RADIO_TCXO_VCC_PORT, RADIO_TCXO_VCC_PIN, &initStruct );
}

void SX1276IoIrqInit( DioIrqHandler **irqHandlers )
{
  HW_GPIO_SetIrq( RADIO_DIO_0_PORT, RADIO_DIO_0_PIN, IRQ_HIGH_PRIORITY, irqHandlers[0] );
  HW_GPIO_SetIrq( RADIO_DIO_1_PORT, RADIO_DIO_1_PIN, IRQ_HIGH_PRIORITY, irqHandlers[1] );
  HW_GPIO_SetIrq( RADIO_DIO_2_PORT, RADIO_DIO_2_PIN, IRQ_HIGH_PRIORITY, irqHandlers[2] );
  HW_GPIO_SetIrq( RADIO_DIO_3_PORT, RADIO_DIO_3_PIN, IRQ_HIGH_PRIORITY, irqHandlers[3] );
}


void SX1276IoDeInit( void )
{
  GPIO_InitTypeDef initStruct={0};

  initStruct.Mode = GPIO_MODE_IT_RISING ; //GPIO_MODE_ANALOG;
  initStruct.Pull = GPIO_NOPULL;
  
  HW_GPIO_Init( RADIO_DIO_0_PORT, RADIO_DIO_0_PIN, &initStruct );
  HW_GPIO_Init( RADIO_DIO_1_PORT, RADIO_DIO_1_PIN, &initStruct );
  HW_GPIO_Init( RADIO_DIO_2_PORT, RADIO_DIO_2_PIN, &initStruct );
  HW_GPIO_Init( RADIO_DIO_3_PORT, RADIO_DIO_3_PIN, &initStruct );
}

void SX1276SetRfTxPower( int8_t power )
{
    uint8_t paConfig = 0;
    uint8_t paDac = 0;

    paConfig = SX1276Read( REG_PACONFIG );
    paDac = SX1276Read( REG_PADAC );

    paConfig = ( paConfig & RF_PACONFIG_PASELECT_MASK ) | SX1276GetPaSelect( SX1276.Settings.Channel );
    paConfig = ( paConfig & RF_PACONFIG_MAX_POWER_MASK ) | 0x70;

    if( ( paConfig & RF_PACONFIG_PASELECT_PABOOST ) == RF_PACONFIG_PASELECT_PABOOST )
    {
        if( power > 17 )
        {
            paDac = ( paDac & RF_PADAC_20DBM_MASK ) | RF_PADAC_20DBM_ON;
        }
        else
        {
            paDac = ( paDac & RF_PADAC_20DBM_MASK ) | RF_PADAC_20DBM_OFF;
        }
        if( ( paDac & RF_PADAC_20DBM_ON ) == RF_PADAC_20DBM_ON )
        {
            if( power < 5 )
            {
                power = 5;
            }
            if( power > 20 )
            {
                power = 20;
            }
            paConfig = ( paConfig & RF_PACONFIG_OUTPUTPOWER_MASK ) | ( uint8_t )( ( uint16_t )( power - 5 ) & 0x0F );
        }
        else
        {
            if( power < 2 )
            {
                power = 2;
            }
            if( power > 17 )
            {
                power = 17;
            }
            paConfig = ( paConfig & RF_PACONFIG_OUTPUTPOWER_MASK ) | ( uint8_t )( ( uint16_t )( power - 2 ) & 0x0F );
        }
    }
    else
    {
        if( power < -1 )
        {
            power = -1;
        }
        if( power > 14 )
        {
            power = 14;
        }
        paConfig = ( paConfig & RF_PACONFIG_OUTPUTPOWER_MASK ) | ( uint8_t )( ( uint16_t )( power + 1 ) & 0x0F );
    }
    SX1276Write( REG_PACONFIG, paConfig );
    SX1276Write( REG_PADAC, paDac );
}
uint8_t SX1276GetPaSelect( uint32_t channel )
{
    return RF_PACONFIG_PASELECT_PABOOST;//RF_PACONFIG_PASELECT_RFO;
}



void SX1276SetAntSwLowPower( bool status )
{
    if( status == false )
    {
      SX1276AntSwInit( );
    }
    else 
    {
      SX1276AntSwDeInit( );
    }
}

static void SX1276AntSwInit( void )
{
  GPIO_InitTypeDef initStruct={0};

  initStruct.Mode =GPIO_MODE_OUTPUT_PP;
  initStruct.Pull = GPIO_NOPULL; 
  initStruct.Speed = GPIO_SPEED_HIGH;
  
  HW_GPIO_Init( RADIO_ANT_SWITCH_PORT, RADIO_ANT_SWITCH_PIN, &initStruct  ); 
  HW_GPIO_Write( RADIO_ANT_SWITCH_PORT, RADIO_ANT_SWITCH_PIN, 0);
//  
//  HW_GPIO_Init( RADIO_ANT_SWITCH_PORT_TX_BOOST, RADIO_ANT_SWITCH_PIN_TX_BOOST, &initStruct  ); 
//  HW_GPIO_Write( RADIO_ANT_SWITCH_PORT_TX_BOOST, RADIO_ANT_SWITCH_PIN_TX_BOOST, 0);
//  
//  HW_GPIO_Init( RADIO_ANT_SWITCH_PORT_TX_RFO, RADIO_ANT_SWITCH_PIN_TX_RFO, &initStruct  ); 
//  HW_GPIO_Write( RADIO_ANT_SWITCH_PORT_TX_RFO, RADIO_ANT_SWITCH_PIN_TX_RFO, 0);
}

static void SX1276AntSwDeInit( void )
{
  GPIO_InitTypeDef initStruct={0};

  initStruct.Mode = GPIO_MODE_ANALOG ;
  
  initStruct.Pull = GPIO_NOPULL;
  initStruct.Speed = GPIO_SPEED_HIGH;

  HW_GPIO_Init( RADIO_ANT_SWITCH_PORT, RADIO_ANT_SWITCH_PIN, &initStruct  ); 
  HW_GPIO_Write( RADIO_ANT_SWITCH_PORT, RADIO_ANT_SWITCH_PIN, 0);
  
//  HW_GPIO_Init( RADIO_ANT_SWITCH_PORT_TX_BOOST, RADIO_ANT_SWITCH_PIN_TX_BOOST, &initStruct  ); 
//  HW_GPIO_Write( RADIO_ANT_SWITCH_PORT_TX_BOOST, RADIO_ANT_SWITCH_PIN_TX_BOOST, 0);
//  
//  HW_GPIO_Init( RADIO_ANT_SWITCH_PORT_TX_RFO, RADIO_ANT_SWITCH_PIN_TX_RFO, &initStruct  ); 
//  HW_GPIO_Write( RADIO_ANT_SWITCH_PORT_TX_RFO, RADIO_ANT_SWITCH_PIN_TX_RFO, 0);
}

void SX1276SetAntSw( uint8_t opMode )
{
 uint8_t paConfig =  SX1276Read( REG_PACONFIG );
    switch( opMode )
    {
    case RFLR_OPMODE_TRANSMITTER:
      HW_GPIO_Write( RADIO_ANT_SWITCH_PORT, RADIO_ANT_SWITCH_PIN, 0 );
      SX1276.RxTx = 1;
    break;
    case RFLR_OPMODE_RECEIVER:
    case RFLR_OPMODE_RECEIVER_SINGLE:
    case RFLR_OPMODE_CAD:
    default:
     SX1276.RxTx = 0;
     HW_GPIO_Write( RADIO_ANT_SWITCH_PORT, RADIO_ANT_SWITCH_PIN, 1 );
     break;
    }
  
}





bool SX1276CheckRfFrequency( uint32_t frequency )
{
    // Implement check. Currently all frequencies are supported
    return true;
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

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
 * @file    lora.c
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

/* Includes ------------------------------------------------------------------*/
#include "hw.h"
#include "timeServer.h"
#include "LoRaMac.h"
#include "lora.h"

/*!
 * Join requests trials duty cycle.
 */
#define OVER_THE_AIR_ACTIVATION_DUTYCYCLE 10000 // 10 [s] value in ms

#if defined(REGION_EU868)

#include "LoRaMacTest.h"

/*!
 * LoRaWAN ETSI duty cycle control enable/disable
 *
 * \remark Please note that ETSI mandates duty cycled transmissions. Use only for test purposes
 */
#define LORAWAN_DUTYCYCLE_ON true

#define USE_SEMTECH_DEFAULT_CHANNEL_LINEUP 0

#if (USE_SEMTECH_DEFAULT_CHANNEL_LINEUP == 1)

#define LC4                                     \
    {                                           \
        867100000, 0, {((DR_5 << 4) | DR_0)}, 0 \
    }
#define LC5                                     \
    {                                           \
        867300000, 0, {((DR_5 << 4) | DR_0)}, 0 \
    }
#define LC6                                     \
    {                                           \
        867500000, 0, {((DR_5 << 4) | DR_0)}, 0 \
    }
#define LC7                                     \
    {                                           \
        867700000, 0, {((DR_5 << 4) | DR_0)}, 0 \
    }
#define LC8                                     \
    {                                           \
        867900000, 0, {((DR_5 << 4) | DR_0)}, 0 \
    }
#define LC9                                     \
    {                                           \
        868800000, 0, {((DR_7 << 4) | DR_7)}, 2 \
    }
#define LC10                                    \
    {                                           \
        868300000, 0, {((DR_6 << 4) | DR_6)}, 1 \
    }

#endif

#endif

static uint8_t DevEui[] = LORAWAN_DEVICE_EUI;
static uint8_t AppEui[] = LORAWAN_APPLICATION_EUI;
static uint8_t AppKey[] = LORAWAN_APPLICATION_KEY;

#if (OVER_THE_AIR_ACTIVATION == 0)

static uint8_t NwkSKey[] = LORAWAN_NWKSKEY;
static uint8_t AppSKey[] = LORAWAN_APPSKEY;

/*!
 * Device address
 */
static uint32_t DevAddr = LORAWAN_DEVICE_ADDRESS;

#endif

/*!
 * User application data buffer size
 */
#define LORAWAN_APP_DATA_BUFF_SIZE 242

/*!
 * User application data
 */
static uint8_t AppDataBuff[LORAWAN_APP_DATA_BUFF_SIZE];

/*!
 * User application data structure
 */
static lora_AppData_t AppData = { AppDataBuff, 0, 0 };

/*!
 * Indicates if the node is sending confirmed or unconfirmed messages
 */
static FunctionalState IsTxConfirmed;

/*!
 * Defines the LoRa parameters at Init
 */
static LoRaParam_t *LoRaParamInit;
/*!
 * Timer to handle the application data transmission duty cycle
 */
static TimerEvent_t TxNextPacketTimer;

static DeviceState_t DeviceState = DEVICE_STATE_INIT;

/*!
 * Timer to handle the state of LED1
 */

static LoRaMacPrimitives_t LoRaMacPrimitives;
static LoRaMacCallback_t LoRaMacCallbacks;
static MibRequestConfirm_t mibReq;

static LoRaMainCallback_t *LoRaMainCallbacks;

/*!
 * Indicates if a new packet can be sent
 */
static bool NextTx = true;

/*!
 * LoRaWAN compliance tests support data
 */
struct ComplianceTest_s
{
    bool Running;
    uint8_t State;
    FunctionalState IsTxConfirmed;
    uint8_t AppPort;
    uint8_t AppDataSize;
    uint8_t *AppDataBuffer;
    uint16_t DownLinkCounter;
    bool LinkCheck;
    uint8_t DemodMargin;
    uint8_t NbGateways;
} ComplianceTest;

/*!
 * \brief   Prepares the payload of the frame
 */
static void PrepareTxFrame( )
{
    if ( ComplianceTest.Running == true )
    {
        if ( ComplianceTest.LinkCheck == true )
        {
            ComplianceTest.LinkCheck = false;
            AppData.BuffSize = 3;
            AppData.Buff[0] = 5;
            AppData.Buff[1] = ComplianceTest.DemodMargin;
            AppData.Buff[2] = ComplianceTest.NbGateways;
            ComplianceTest.State = 1;
        }
        else
        {
            switch ( ComplianceTest.State )
            {
                case 4:
                    ComplianceTest.State = 1;
                    break;
                case 1:
                    AppData.BuffSize = 2;
                    AppData.Buff[0] = ComplianceTest.DownLinkCounter >> 8;
                    AppData.Buff[1] = ComplianceTest.DownLinkCounter;
                    break;
            }
        }
    }
    else
    {
        LoRaMainCallbacks->LoraTxData( &AppData, &IsTxConfirmed );
    }
}

/*!
 * \brief   Prepares the payload of the frame
 *
 * \retval  [0: frame could be send, 1: error]
 */
static bool SendFrame( void )
{
    McpsReq_t mcpsReq;
    LoRaMacTxInfo_t txInfo;

    if ( LoRaMacQueryTxPossible( AppData.BuffSize, &txInfo ) != LORAMAC_STATUS_OK )
    {
        // Send empty frame in order to flush MAC commands
        mcpsReq.Type = MCPS_UNCONFIRMED;
        mcpsReq.Req.Unconfirmed.fBuffer = NULL;
        mcpsReq.Req.Unconfirmed.fBufferSize = 0;
        mcpsReq.Req.Unconfirmed.Datarate = LoRaParamInit->TxDatarate;
    }
    else
    {
        if ( IsTxConfirmed == DISABLE )
        {
            mcpsReq.Type = MCPS_UNCONFIRMED;
            mcpsReq.Req.Unconfirmed.fPort = AppData.Port;
            mcpsReq.Req.Unconfirmed.fBuffer = AppData.Buff;
            mcpsReq.Req.Unconfirmed.fBufferSize = AppData.BuffSize;
            mcpsReq.Req.Unconfirmed.Datarate = LoRaParamInit->TxDatarate;
        }
        else
        {
            mcpsReq.Type = MCPS_CONFIRMED;
            mcpsReq.Req.Confirmed.fPort = AppData.Port;
            mcpsReq.Req.Confirmed.fBuffer = AppData.Buff;
            mcpsReq.Req.Confirmed.fBufferSize = AppData.BuffSize;
            mcpsReq.Req.Confirmed.NbTrials = 8;
            mcpsReq.Req.Confirmed.Datarate = LoRaParamInit->TxDatarate;
        }
    }
    if ( LoRaMacMcpsRequest( &mcpsReq ) == LORAMAC_STATUS_OK )
    {
        return false;
    }
    return true;
}

void OnSendEvent( void )
{
    MibRequestConfirm_t mibReq;
    LoRaMacStatus_t status;

    mibReq.Type = MIB_NETWORK_JOINED;
    status = LoRaMacMibGetRequestConfirm( &mibReq );

    if ( status == LORAMAC_STATUS_OK )
    {
        if ( mibReq.Param.IsNetworkJoined == true )
        {
            DeviceState = DEVICE_STATE_SEND;
            NextTx = true;
        }
        else
        {
            DeviceState = DEVICE_STATE_JOIN;
        }
    }
}
/*!
 * \brief Function executed on TxNextPacket Timeout event
 */
static void OnTxNextPacketTimerEvent( void )
{
    TimerStop( &TxNextPacketTimer );
    OnSendEvent( );
}

/*!
 * \brief   MCPS-Confirm event function
 *
 * \param   [IN] McpsConfirm - Pointer to the confirm structure,
 *               containing confirm attributes.
 */
static void McpsConfirm( McpsConfirm_t *mcpsConfirm )
{
    if ( mcpsConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK )
    {
        switch ( mcpsConfirm->McpsRequest )
        {
            case MCPS_UNCONFIRMED:
                {
                // Check Datarate
                // Check TxPower
                break;
            }
            case MCPS_CONFIRMED:
                {
                // Check Datarate
                // Check TxPower
                // Check AckReceived
                // Check NbTrials
                break;
            }
            case MCPS_PROPRIETARY:
                {
                break;
            }
            default:
                break;
        }
    }
    NextTx = true;
}

/*!
 * \brief   MCPS-Indication event function
 *
 * \param   [IN] mcpsIndication - Pointer to the indication structure,
 *               containing indication attributes.
 */
static void McpsIndication( McpsIndication_t *mcpsIndication )
{
    if ( mcpsIndication->Status != LORAMAC_EVENT_INFO_STATUS_OK )
    {
        return;
    }

    switch ( mcpsIndication->McpsIndication )
    {
        case MCPS_UNCONFIRMED:
            {
            break;
        }
        case MCPS_CONFIRMED:
            {
            break;
        }
        case MCPS_PROPRIETARY:
            {
            break;
        }
        case MCPS_MULTICAST:
            {
            DBG_PRINTF( "MCPS_MULTICAST\n" );
            break;
        }
        default:
            break;
    }

    // Check Multicast
    // Check Port
    // Check Datarate
    // Check FramePending
    // Check Buffer
    // Check BufferSize
    // Check Rssi
    // Check Snr
    // Check RxSlot
    DBG_PRINTF( "rssi = %d, snr = %d, datarate = %d\n", mcpsIndication->Rssi, mcpsIndication->Snr,
                mcpsIndication->RxDatarate );

    if ( ComplianceTest.Running == true )
    {
        ComplianceTest.DownLinkCounter++;
    }

    if ( mcpsIndication->RxData == true )
    {
        switch ( mcpsIndication->Port )
        {
            case 224:
                if ( ComplianceTest.Running == false )
                {
                    // Check compliance test enable command (i)
                    if ( (mcpsIndication->BufferSize == 4) &&
                         (mcpsIndication->Buffer[0] == 0x01)
                         &&
                         (mcpsIndication->Buffer[1] == 0x01)
                         &&
                         (mcpsIndication->Buffer[2] == 0x01)
                         &&
                         (mcpsIndication->Buffer[3] == 0x01) )
                    {
                        IsTxConfirmed = DISABLE;
                        AppData.Port = 224;
                        AppData.BuffSize = 2;
                        ComplianceTest.DownLinkCounter = 0;
                        ComplianceTest.LinkCheck = false;
                        ComplianceTest.DemodMargin = 0;
                        ComplianceTest.NbGateways = 0;
                        ComplianceTest.Running = true;
                        ComplianceTest.State = 1;

                        MibRequestConfirm_t mibReq;
                        mibReq.Type = MIB_ADR;
                        mibReq.Param.AdrEnable = true;
                        LoRaMacMibSetRequestConfirm( &mibReq );

#if defined(REGION_EU868)
                        LoRaMacTestSetDutyCycleOn(false);
#endif
                    }
                }
                else
                {
                    ComplianceTest.State = mcpsIndication->Buffer[0];
                    switch ( ComplianceTest.State )
                    {
                        case 0: // Check compliance test disable command (ii)
                            ComplianceTest.DownLinkCounter = 0;
                            ComplianceTest.Running = false;

                            MibRequestConfirm_t mibReq;
                            mibReq.Type = MIB_ADR;
                            mibReq.Param.AdrEnable = LoRaParamInit->AdrEnable;
                            LoRaMacMibSetRequestConfirm( &mibReq );
#if defined(REGION_EU868)
                            lora_config_duty_cycle_set(LORAWAN_DUTYCYCLE_ON ? ENABLE : DISABLE);
#endif
                            break;
                        case 1: // (iii, iv)
                            AppData.BuffSize = 2;
                            break;
                        case 2: // Enable confirmed messages (v)
                            IsTxConfirmed = ENABLE;
                            ComplianceTest.State = 1;
                            break;
                        case 3: // Disable confirmed messages (vi)
                            IsTxConfirmed = DISABLE;
                            ComplianceTest.State = 1;
                            break;
                        case 4: // (vii)
                            AppData.BuffSize = mcpsIndication->BufferSize;

                            AppData.Buff[0] = 4;
                            for ( uint8_t i = 1; i < AppData.BuffSize; i++ )
                            {
                                AppData.Buff[i] = mcpsIndication->Buffer[i] + 1;
                            }
                            break;
                        case 5: // (viii)
                        {
                            MlmeReq_t mlmeReq;
                            mlmeReq.Type = MLME_LINK_CHECK;
                            LoRaMacMlmeRequest( &mlmeReq );
                        }
                            break;
                        case 6: // (ix)
                        {
                            MlmeReq_t mlmeReq;

                            // Disable TestMode and revert back to normal operation

                            ComplianceTest.DownLinkCounter = 0;
                            ComplianceTest.Running = false;

                            MibRequestConfirm_t mibReq;
                            mibReq.Type = MIB_ADR;
                            mibReq.Param.AdrEnable = LoRaParamInit->AdrEnable;
                            LoRaMacMibSetRequestConfirm( &mibReq );
#if defined(REGION_EU868)
                            lora_config_duty_cycle_set(LORAWAN_DUTYCYCLE_ON ? ENABLE : DISABLE);
#endif

                            mlmeReq.Type = MLME_JOIN;

                            mlmeReq.Req.Join.DevEui = DevEui;
                            mlmeReq.Req.Join.AppEui = AppEui;
                            mlmeReq.Req.Join.AppKey = AppKey;
                            mlmeReq.Req.Join.NbTrials = 3;

                            LoRaMacMlmeRequest( &mlmeReq );
                            DeviceState = DEVICE_STATE_SLEEP;
                        }
                            break;
                        case 7: // (x)
                        {
                            if ( mcpsIndication->BufferSize == 3 )
                            {
                                MlmeReq_t mlmeReq;
                                mlmeReq.Type = MLME_TXCW;
                                mlmeReq.Req.TxCw.Timeout = (uint16_t) ((mcpsIndication->Buffer[1] << 8)
                                    | mcpsIndication->Buffer[2]);
                                LoRaMacMlmeRequest( &mlmeReq );
                            }
                            else if ( mcpsIndication->BufferSize == 7 )
                            {
                                MlmeReq_t mlmeReq;
                                mlmeReq.Type = MLME_TXCW_1;
                                mlmeReq.Req.TxCw.Timeout = (uint16_t) ((mcpsIndication->Buffer[1] << 8)
                                    | mcpsIndication->Buffer[2]);
                                mlmeReq.Req.TxCw.Frequency = (uint32_t) ((mcpsIndication->Buffer[3] << 16)
                                    | (mcpsIndication->Buffer[4] << 8) | mcpsIndication->Buffer[5])
                                                             * 100;
                                mlmeReq.Req.TxCw.Power = mcpsIndication->Buffer[6];
                                LoRaMacMlmeRequest( &mlmeReq );
                            }
                            ComplianceTest.State = 1;
                        }
                            break;
                        default:
                            break;
                    }
                }
                break;
            default:

                AppData.Port = mcpsIndication->Port;
                AppData.BuffSize = mcpsIndication->BufferSize;
                memcpy1( AppData.Buff, mcpsIndication->Buffer, AppData.BuffSize );

                LoRaMainCallbacks->LoraRxData( &AppData );
                break;
        }
    }
}

/*!
 * \brief   MLME-Confirm event function
 *
 * \param   [IN] MlmeConfirm - Pointer to the confirm structure,
 *               containing confirm attributes.
 */
static void MlmeConfirm( MlmeConfirm_t *mlmeConfirm )
{
    switch ( mlmeConfirm->MlmeRequest )
    {
        case MLME_JOIN:
            {
            if ( mlmeConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK )
            {
                SYS_LED_ON( );
                // Status is OK, node has joined the network
                DeviceState = DEVICE_STATE_JOINED;
            }
            else
            {
                SYS_LED_OFF( );
                // Join was not successful. Try to join again
                DeviceState = DEVICE_STATE_JOIN;
            }
            break;
        }
        case MLME_LINK_CHECK:
            {
            if ( mlmeConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK )
            {
                // Check DemodMargin
                // Check NbGateways
                if ( ComplianceTest.Running == true )
                {
                    ComplianceTest.LinkCheck = true;
                    ComplianceTest.DemodMargin = mlmeConfirm->DemodMargin;
                    ComplianceTest.NbGateways = mlmeConfirm->NbGateways;
                }
            }
            break;
        }
        default:
            break;
    }
    NextTx = true;
}
/**
 *  lora Init
 */
void lora_Init( LoRaMainCallback_t *callbacks, LoRaParam_t *LoRaParam )
{
    /* init the DeviceState*/
    DeviceState = DEVICE_STATE_INIT;

    /* init the Tx Duty Cycle*/
    LoRaParamInit = LoRaParam;

    /* init the main call backs*/
    LoRaMainCallbacks = callbacks;

#if (STATIC_DEVICE_EUI != 1)
    LoRaMainCallbacks->BoardGetUniqueId( DevEui );
#endif

#if (OVER_THE_AIR_ACTIVATION != 0)

    PRINTF_RAW( "OTAA\r\n" );
    PRINTF_RAW( "DevEui= %02X", DevEui[0] );
    for ( int i = 1; i < 8; i++ )
    {
        PRINTF_RAW( "-%02X", DevEui[i] );
    };
    PRINTF_RAW( "\r\n" );
    PRINTF_RAW( "AppEui= %02X", AppEui[0] );
    for ( int i = 1; i < 8; i++ )
    {
        PRINTF_RAW( "-%02X", AppEui[i] );
    };
    PRINTF_RAW( "\r\n" );
    PRINTF_RAW( "AppKey= %02X", AppKey[0] );
    for ( int i = 1; i < 16; i++ )
    {
        PRINTF_RAW( " %02X", AppKey[i] );
    };
    PRINTF_RAW( "\n\r\n" );
#else

#if (STATIC_DEVICE_ADDRESS != 1)
    // Random seed initialization
    srand1(LoRaMainCallbacks->BoardGetRandomSeed());
    // Choose a random device address
    DevAddr = randr(0, 0x01FFFFFF);
#endif
    PRINTF_RAW("ABP\r\n");
    PRINTF_RAW("DevEui= %02X", DevEui[0]);
    for (int i = 1; i < 8; i++)
    {
        PRINTF_RAW("-%02X", DevEui[i]);
    };
    PRINTF_RAW("\r\n");
    PRINTF_RAW("DevAdd=  %08X\n\r", DevAddr);
    PRINTF_RAW("NwkSKey= %02X", NwkSKey[0]);
    for (int i = 1; i < 16; i++)
    {
        PRINTF_RAW(" %02X", NwkSKey[i]);
    };
    PRINTF_RAW("\r\n");
    PRINTF_RAW("AppSKey= %02X", AppSKey[0]);
    for (int i = 1; i < 16; i++)
    {
        PRINTF_RAW(" %02X", AppSKey[i]);
    };
    PRINTF_RAW("\r\n");
#endif
}

/**
 *  lora class A state machine
 */

void lora_fsm( void )
{
    switch ( DeviceState )
    {
        case DEVICE_STATE_INIT:
            {
            LoRaMacPrimitives.MacMcpsConfirm = McpsConfirm;
            LoRaMacPrimitives.MacMcpsIndication = McpsIndication;
            LoRaMacPrimitives.MacMlmeConfirm = MlmeConfirm;
            LoRaMacCallbacks.GetBatteryLevel = LoRaMainCallbacks->BoardGetBatteryLevel;
#if defined(REGION_AS923)
            LoRaMacInitialization(&LoRaMacPrimitives, &LoRaMacCallbacks, LORAMAC_REGION_AS923);
#elif defined(REGION_AS923)
            LoRaMacInitialization(&LoRaMacPrimitives, &LoRaMacCallbacks, LORAMAC_REGION_AU915);
#elif defined(REGION_CN470)
            LoRaMacInitialization(&LoRaMacPrimitives, &LoRaMacCallbacks, LORAMAC_REGION_CN470);
#elif defined(REGION_CN779)
            LoRaMacInitialization(&LoRaMacPrimitives, &LoRaMacCallbacks, LORAMAC_REGION_CN779);
#elif defined(REGION_EU433)
            LoRaMacInitialization(&LoRaMacPrimitives, &LoRaMacCallbacks, LORAMAC_REGION_EU433);
#elif defined(REGION_IN865)
            LoRaMacInitialization(&LoRaMacPrimitives, &LoRaMacCallbacks, LORAMAC_REGION_IN865);
#elif defined(REGION_EU868)
            LoRaMacInitialization(&LoRaMacPrimitives, &LoRaMacCallbacks, LORAMAC_REGION_EU868);
#elif defined(REGION_KR920)
            LoRaMacInitialization(&LoRaMacPrimitives, &LoRaMacCallbacks, LORAMAC_REGION_KR920);
#elif defined(REGION_US915)
            LoRaMacInitialization(&LoRaMacPrimitives, &LoRaMacCallbacks, LORAMAC_REGION_US915);
#elif defined(REGION_US915_HYBRID)
            LoRaMacInitialization(&LoRaMacPrimitives, &LoRaMacCallbacks, LORAMAC_REGION_US915_HYBRID);
#else
#error "Please define a region in the compiler options."
#endif

            TimerInit( &TxNextPacketTimer, OnTxNextPacketTimerEvent );

            mibReq.Type = MIB_ADR;
            mibReq.Param.AdrEnable = LoRaParamInit->AdrEnable;
            LoRaMacMibSetRequestConfirm( &mibReq );

            mibReq.Type = MIB_PUBLIC_NETWORK;
            mibReq.Param.EnablePublicNetwork = LoRaParamInit->EnablePublicNetwork;
            LoRaMacMibSetRequestConfirm( &mibReq );

            mibReq.Type = MIB_DEVICE_CLASS;
            mibReq.Param.Class = LoRaParamInit->Class;
            LoRaMacMibSetRequestConfirm( &mibReq );

#if defined(REGION_EU868)
            lora_config_duty_cycle_set(LORAWAN_DUTYCYCLE_ON ? ENABLE : DISABLE);

#if (USE_SEMTECH_DEFAULT_CHANNEL_LINEUP == 1)
            LoRaMacChannelAdd(3, (ChannelParams_t)LC4);
            LoRaMacChannelAdd(4, (ChannelParams_t)LC5);
            LoRaMacChannelAdd(5, (ChannelParams_t)LC6);
            LoRaMacChannelAdd(6, (ChannelParams_t)LC7);
            LoRaMacChannelAdd(7, (ChannelParams_t)LC8);
            LoRaMacChannelAdd(8, (ChannelParams_t)LC9);
            LoRaMacChannelAdd(9, (ChannelParams_t)LC10);

            mibReq.Type = MIB_RX2_DEFAULT_CHANNEL;
            mibReq.Param.Rx2DefaultChannel = (Rx2ChannelParams_t)
            {   869525000, DR_3};
            LoRaMacMibSetRequestConfirm(&mibReq);

            mibReq.Type = MIB_RX2_CHANNEL;
            mibReq.Param.Rx2Channel = (Rx2ChannelParams_t)
            {   869525000, DR_3};
            LoRaMacMibSetRequestConfirm(&mibReq);
#endif

#endif
            DeviceState = DEVICE_STATE_JOIN;
            break;
        }
        case DEVICE_STATE_JOIN:
            {
#if (OVER_THE_AIR_ACTIVATION != 0)
            MlmeReq_t mlmeReq;

            mlmeReq.Type = MLME_JOIN;
            mlmeReq.Req.Join.DevEui = DevEui;
            mlmeReq.Req.Join.AppEui = AppEui;
            mlmeReq.Req.Join.AppKey = AppKey;
            mlmeReq.Req.Join.NbTrials = LoRaParamInit->NbTrials;

            if ( NextTx == true )
            {
                LoRaMacMlmeRequest( &mlmeReq );
            }

            DeviceState = DEVICE_STATE_SLEEP;
#else
            mibReq.Type = MIB_NET_ID;
            mibReq.Param.NetID = LORAWAN_NETWORK_ID;
            LoRaMacMibSetRequestConfirm(&mibReq);

            mibReq.Type = MIB_DEV_ADDR;
            mibReq.Param.DevAddr = DevAddr;
            LoRaMacMibSetRequestConfirm(&mibReq);

            mibReq.Type = MIB_NWK_SKEY;
            mibReq.Param.NwkSKey = NwkSKey;
            LoRaMacMibSetRequestConfirm(&mibReq);

            mibReq.Type = MIB_APP_SKEY;
            mibReq.Param.AppSKey = AppSKey;
            LoRaMacMibSetRequestConfirm(&mibReq);

            mibReq.Type = MIB_NETWORK_JOINED;
            mibReq.Param.IsNetworkJoined = true;
            LoRaMacMibSetRequestConfirm(&mibReq);

            DeviceState = DEVICE_STATE_SEND;
#endif
            break;
        }
        case DEVICE_STATE_JOINED:
            {
            PRINTF("JOINED\n\r")
            ;
            DeviceState = DEVICE_STATE_SEND;
            break;
        }
        case DEVICE_STATE_SEND:
            {
            if ( NextTx == true )
            {
                PrepareTxFrame( );

                NextTx = SendFrame( );
            }
            if ( ComplianceTest.Running == true )
            {
                // Schedule next packet transmission as soon as possible
                TimerSetValue( &TxNextPacketTimer, 5000 ); /* 5s */
                TimerStart( &TxNextPacketTimer );
            }
            else if ( LoRaParamInit->TxEvent == TX_ON_TIMER )
            {
                // Schedule next packet transmission
                TimerSetValue( &TxNextPacketTimer, LoRaParamInit->TxDutyCycleTime );
                TimerStart( &TxNextPacketTimer );
            }

            DeviceState = DEVICE_STATE_SLEEP;
            break;
        }
        case DEVICE_STATE_SLEEP:
            {
            // Wake up through events
            break;
        }
        default:
            {
            DeviceState = DEVICE_STATE_INIT;
            break;
        }
    }
}

DeviceState_t lora_getDeviceState( void )
{
    return DeviceState;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

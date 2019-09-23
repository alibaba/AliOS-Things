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

#include "stdlib.h"
#include "stdio.h"
#include <math.h>

//#include "debug.h"
//#include "hw.h"
//#include "low_power_manager.h"
//#include "bsp.h"
#include "time.h"
//#include "vcom.h"

#include "version.h"
#include "utilities.h"
#include "Commissioning.h"

#include "LoRaMac.h"
#include "lora_module_controller.h"
#include "lora_module_export.h"
#include "lora_config.h"
#include "connection.h"
#include "lora_multicast_control.h"
//#include "gpio.h"
//#include "cmsis_os.h"
//#include "lite-log.h"
//#include "battery.h"

//==================lora rx sensitive test====================================
#define LORA_BANDWIDTH                              0         // [0: 125 kHz]
#define LORA_SPREADING_FACTOR                       10        // [SF7..SF12]
#define LORA_CODINGRATE                             1         // [1: 4/5]
#define LORA_SYMBOL_TIMEOUT                         5         // Symbols
#define LORA_PREAMBLE_LENGTH                        8         // Same for Tx and Rx
#define LORA_FIX_LENGTH_PAYLOAD_ON                  false
#define LORA_IQ_INVERSION_ON                        false

#define TX_OUTPUT_POWER                             14        // dBm
//==================rx sensitive test End=====================================

#define NUM_MS_PER_SECOND       1000
#define NUM_SECOND_PER_MINUTE   60
#define NUM_MINUTE_PER_HOUR     60

#define TIMER_SWITCH_CLASSB_RESULT_CHECK (150000) //150s
#define TIMER_RADIO_STATUS_CHECK (180000) //180s

static int g_num_send_data_failed_to_rejoin =0;
static int g_num_of_join_failed             =0;
static uint16_t g_num_send_heartbeat        = 0;
static uint16_t g_num_received_data_ack     = 0;
static int g_num_of_class_switch_failed     = 0;
static int g_lora_class_switch_succes_flag  = true;
static unsigned long g_random_joindelay     = 0;

static int g_num_of_pingslotack_failed      = 0;
static int g_num_beacon_missed              = 0;

static DeviceClass_t g_current_target_lora_class_type = LORA_NODE_TARGET_CLASS_TPYE;
static DeviceClass_t g_current_lora_class_type        = CLASS_A;

static esl_connection_ops_cb_t* g_lora_esl_connection_ops_cb = NULL;
static void*                    g_lora_cm_data               = NULL;

static LoRa_Status sLoRa_Status = LoRa_Initing;

static int g_lora_rx_rssi = 0;
static int g_lora_rx_snr  = 0;

typedef struct {
    uint8_t port;
    rx_cb_t rx_cb;
} app_rx_cb_t;

#define MAX_APP_RX_CB_LIST (5)
static app_rx_cb_t g_app_rx_cb_list[MAX_APP_RX_CB_LIST];

//===========================================================================
#if LORA_MODULE_GET_FLASH_EUI_ENABLE
static uint8_t DevEui[8] = {0};//LORAWAN_DEVICE_EUI;
static uint8_t JoinEui[8] = {0};//LORAWAN_JOIN_EUI;
static uint8_t AppKey[16] = {0};//LORAWAN_APP_KEY;
static uint8_t NwkKey[16] = {0};//LORAWAN_NWK_KEY;
static uint8_t GenAppKey[16] = LORAWAN_MC_GEN_APP_KEY;
#else

/* preset multicast parameter */
static mcChannel_t g_mc_channels[LORAMAC_MAX_MC_CTX] = {
    { .AddrID = MULTICAST_0_ADDR, .Address = LORAWAN_MC_ADDR_0, .IsEnabled = 1, .Frequency = 0, .Datarate = LORAWAN_MC_DR_0, .Periodicity = LORAWAN_MC_PERIOD_0 },
    { .AddrID = MULTICAST_1_ADDR, .Address = LORAWAN_MC_ADDR_1, .IsEnabled = 1, .Frequency = 0, .Datarate = LORAWAN_MC_DR_1, .Periodicity = LORAWAN_MC_PERIOD_1 },
    { .AddrID = MULTICAST_2_ADDR, .Address = LORAWAN_MC_ADDR_2, .IsEnabled = 1, .Frequency = 0, .Datarate = LORAWAN_MC_DR_2, .Periodicity = LORAWAN_MC_PERIOD_2 },
    { .AddrID = MULTICAST_3_ADDR, .Address = LORAWAN_MC_ADDR_3, .IsEnabled = 1, .Frequency = 0, .Datarate = LORAWAN_MC_DR_3, .Periodicity = LORAWAN_MC_PERIOD_3 }
};

static mcKey_t g_mc_keys[LORAMAC_MAX_MC_CTX] = {
    { MIB_MC_KEY_0, LORAWAN_MC_KEY_0 },
    { MIB_MC_KEY_1, LORAWAN_MC_KEY_1 },
    { MIB_MC_KEY_2, LORAWAN_MC_KEY_2 },
    { MIB_MC_KEY_3, LORAWAN_MC_KEY_3 }
};

static uint8_t DevEui[] = LORAWAN_DEVICE_EUI;
static uint8_t JoinEui[] = LORAWAN_JOIN_EUI;
static uint8_t AppKey[] = LORAWAN_APP_KEY;
static uint8_t NwkKey[] = LORAWAN_NWK_KEY;

static uint8_t mcGenAppKey[] = LORAWAN_MC_GEN_APP_KEY;
#endif

#if( OVER_THE_AIR_ACTIVATION == 0 )

static uint8_t FNwkSIntKey[] = LORAWAN_F_NWK_S_INT_KEY;
static uint8_t SNwkSIntKey[] = LORAWAN_S_NWK_S_INT_KEY;
static uint8_t NwkSEncKey[] = LORAWAN_NWK_S_ENC_KEY;
static uint8_t AppSKey[] = LORAWAN_APP_S_KEY;

/*!
 * Device address
 */
static uint32_t DevAddr = LORAWAN_DEVICE_ADDRESS;

#endif

#define LORAWAN_APP_DATA_MAX_SIZE                           242
#define LORAWAN_CONTROLLER_PORT                             (2)
#define LORAWAN_CONTROLLER_DATA_LEN                         (0)

#define CHANNELS_MASK_SIZE                                  (6)

/*!
 * Application port
 */
static uint8_t AppPort = LORAWAN_CONTROLLER_PORT;

/*!
 * User application data size
 */
static uint8_t AppDataSize = 1;
static uint8_t AppDataSizeBackup = 1;

/*!
 * User application data
 */
static uint8_t AppDataBuffer[LORAWAN_APP_DATA_MAX_SIZE];

/*!
 * Indicates if the node is sending confirmed or unconfirmed messages
 */
static uint8_t IsTxConfirmed = LORAWAN_CONFIRMED_MSG_ON;

/*!
 * Defines the application data transmission duty cycle
 */
static uint32_t TxDutyCycleTime;

/*!
 * Timer to handle the application data transmission duty cycle
 */
static TimerEvent_t TxNextPacketTimer;

/*!
 * Specifies the state of the application LED
 */
static bool AppLedStateOn = false;

/*!
 * Indicates if a new packet can be sent
 */
static bool NextTx = true;

static DeviceState_t DeviceState, WakeUpState;

/*!
 * LoRaWAN compliance tests support data
 */
struct ComplianceTest_s
{
    bool Running;
    uint8_t State;
    bool IsTxConfirmed;
    uint8_t AppPort;
    uint8_t AppDataSize;
    uint8_t *AppDataBuffer;
    uint16_t DownLinkCounter;
    bool LinkCheck;
    uint8_t DemodMargin;
    uint8_t NbGateways;
    uint8_t uplink_cycle;
}ComplianceTest;

/*!
 *
 */
typedef enum
{
    LORAMAC_HANDLER_UNCONFIRMED_MSG = 0,
    LORAMAC_HANDLER_CONFIRMED_MSG = !LORAMAC_HANDLER_UNCONFIRMED_MSG
}LoRaMacHandlerMsgTypes_t;

typedef enum
{
  LORA_ERROR = -1,
  LORA_SUCCESS = 0
} LoraErrorStatus;

/*!
 * Application data structure
 */
typedef struct LoRaMacHandlerAppData_s
{
    LoRaMacHandlerMsgTypes_t MsgType;
    uint8_t Port;
    uint8_t BufferSize;
    uint8_t *Buffer;
}LoRaMacHandlerAppData_t;

static LoRaMacHandlerAppData_t AppData =
{
    .MsgType = LORAMAC_HANDLER_CONFIRMED_MSG,
    .Buffer = NULL,
    .BufferSize = 0,
    .Port = 0
};


#define HEX16(X)  X[0],X[1], X[2],X[3], X[4],X[5], X[6],X[7],X[8],X[9], X[10],X[11], X[12],X[13], X[14],X[15]
#define HEX8(X)   X[0],X[1], X[2],X[3], X[4],X[5], X[6],X[7]

/*!
 * MAC status strings
 */
static const char* MacStatusStrings[] =
{
    "OK", "Busy", "Service unknown", "Parameter invalid", "Frequency invalid",
    "Datarate invalid", "Frequency or datarate invalid", "No network joined",
    "Length error", "Device OFF", "Region not supported", "Skipped APP data",
    "Duty-cycle restricted", "No channel found", "No free channel found",
    "Busy beacon reserved time", "Busy ping-slot window time",
    "Busy uplink collision", "Crypto error", "FCnt handler error",
    "MAC command error", "ERROR"
};

/*!
 * MAC event info status strings.
 */
static const char* EventInfoStatusStrings[] =
{
    "OK", "Error", "Tx timeout", "Rx 1 timeout",
    "Rx 2 timeout", "Rx1 error", "Rx2 error",
    "Join failed", "Downlink repeated", "Tx DR payload size error",
    "Downlink too many frames loss", "dynamic multicast fcnt error",
    "Address fail", "MIC fail", "Multicast fail", "Beacon locked",
    "Beacon lost", "Beacon not found"
};

//==========================================================================
static LoRaMacPrimitives_t LoRaMacPrimitives;
static LoRaMacCallback_t LoRaMacCallbacks;
static TimerEvent_t JoinBackoffTimer;
static int JoinBackoffTimerInterval;
static TimerEvent_t ClassSwitchBackoffTimer;
static int ClassSwitchBackoffTimerInterval;
static TimerEvent_t ClassSwitchRespTimeoutTimer;
static TimerEvent_t RadioStatusTimer;

//==========================================================================
static void LORA_ResetParameters(void);
static int  LORA_SwitchClass(DeviceClass_t newClass);
static void LORA_InitFinished(void);
static void LORA_SwitchClassResult(int successful);
static void LORA_ClassBLostSync(void);
static void LORA_SendDataOkWithoutRx(void);
static void LORA_SendDataFailed(void);
static void LORA_SetRssiSnr(int rssi,int snr);

/* call back when LoRa endNode has received a frame*/
static void LORA_RxData(Mcps_t type, uint8_t* Buff, uint8_t BuffSize,uint8_t Port);

/* call back when LoRa endNode has just joined*/
static void LORA_JoinResult( int successful );

/* call back when need to set the device state */
static int LORA_SetDeviceState ( DeviceState_t state);
static void LORA_ReInit(void);
#if 0
static void LORA_ReJoin(void);
#endif

/* tx timer callback function*/
static void OnJoinBackoffTimerEvent(void);
static void OnClassSwitchRespTimeoutTimerEvent( void );

static int lorawan_module_get_app_rx_cb(uint8_t port, rx_cb_t *rx_cb);

/* load Main call backs structure*/
/* Lora Main callbacks*/
typedef struct sLoRaControllerCallback
{
    void (*LORA_InitFinished)(void);
    void (*LORA_SwitchClassResult)(int successful);
    void (*LORA_ClassBLostSync)(void);
    void (*LORA_SendDataOkWithoutRx)(void);
    void (*LORA_SendDataFailed)(void);
    void (*LORA_SetRssiSnr)(int rssi,int snr);
    void (*LORA_RxData ) (Mcps_t type, uint8_t* Buff, uint8_t BuffSize, uint8_t Port);
    void (*LORA_JoinResult)( int successful);
} LoRaControllerCallback_t;

static LoRaControllerCallback_t LoRaControllerCallbacks = {
                                                LORA_InitFinished,
                                                LORA_SwitchClassResult,
                                                LORA_ClassBLostSync,
                                                LORA_SendDataOkWithoutRx,
                                                LORA_SendDataFailed,
                                                LORA_SetRssiSnr,
                                                LORA_RxData,
                                                LORA_JoinResult,
                                              };

//==========================================================================

/*!
 * Prints the provided buffer in HEX
 *
 * \param buffer Buffer to be printed
 * \param size   Buffer size to be printed
 */
static void PrintHexBuffer( uint8_t *buffer, uint8_t size )
{
    uint8_t newline = 0;

    for( uint8_t i = 0; i < size; i++ )
    {
        if( newline != 0 )
        {
            PRINTF( "\r\n" );
            newline = 0;
        }

        PRINTF( "%02X ", buffer[i] );

        if( ( ( i + 1 ) % 16 ) == 0 )
        {
            newline = 1;
        }
    }
    PRINTF( "\r\n" );
}

/*!
 * Executes the network Join request
 */
static void JoinNetwork( void )
{
    MibRequestConfirm_t mibReq;
    LoRaMacStatus_t status;
    MlmeReq_t mlmeReq;
    mlmeReq.Type = MLME_JOIN;
    mlmeReq.Req.Join.DevEui = DevEui;
    mlmeReq.Req.Join.JoinEui = JoinEui;
    mlmeReq.Req.Join.Datarate = LORAWAN_DEFAULT_DATARATE;

    // Starts the join backoff timer
    JoinBackoffTimerInterval = (1<<g_num_of_join_failed)*NUM_MS_PER_SECOND*NUM_SECOND_PER_MINUTE +g_random_joindelay;//unit =ms; 1000*60s
    OnJoinBackoffTimerEvent();
    LoRaMacStart( );
    sLoRa_Status = LoRa_Joining;
    g_current_lora_class_type = CLASS_A;

    if(CLASS_C == g_current_target_lora_class_type)
    {
        mibReq.Type = MIB_DEVICE_CLASS;
        mibReq.Param.Class = CLASS_C;
        LoRaMacMibSetRequestConfirm( &mibReq );

        g_current_lora_class_type = CLASS_C;
    }
    // Starts the join procedure
    status = LoRaMacMlmeRequest( &mlmeReq );
    PRINTF( "\r\nLoRa:MLME-Request - MLME_JOIN,STATUS: %s\r\n", MacStatusStrings[status] );

    if( status == LORAMAC_STATUS_OK )
    {
        PRINTF( "LoRa:JOINING\r\n" );
		LORA_SetDeviceState( DEVICE_STATE_SLEEP );
    }
    else
    {
        LORA_SetDeviceState( DEVICE_STATE_CYCLE );
    }
}

/*!
 * \brief   Prepares the payload of the frame
 */
static void PrepareTxFrame( uint8_t port )
{
    #if 0
    /*!
     * Low battery threshold definition.
     */
    #define LOW_BAT_THRESHOLD   3450 // mV

    MibRequestConfirm_t mibReq;

    if( BoardGetBatteryVoltage( ) < LOW_BAT_THRESHOLD )
    {
        mibReq.Type = MIB_CHANNELS_TX_POWER;
        LoRaMacMibGetRequestConfirm( &mibReq );
        // 30 dBm = TX_POWER_0, 28 dBm = TX_POWER_1, ..., 20 dBm = TX_POWER_5, ..., 10 dBm = TX_POWER_10
        // The if condition is then "less than" to check if the power is greater than 20 dBm
        if( mibReq.Param.ChannelsTxPower < TX_POWER_5 )
        {
            mibReq.Param.ChannelsTxPower = TX_POWER_5;
            LoRaMacMibSetRequestConfirm( &mibReq );
        }
    }
    #endif

    switch( port )
    {
    case LORAWAN_APP_PORT:
        {
            AppDataSizeBackup = AppDataSize = 11;

            AppDataBuffer[0] = 0x00;
            AppDataBuffer[1] = 0x00; //id
            AppDataBuffer[2] = 0x00; //id
            AppDataBuffer[3] = 0x18; //method
            AppDataBuffer[4] = 0;    //HW_GetBatteryLevel();

            if(g_lora_rx_rssi> 127) g_lora_rx_rssi = 127;
            else if(g_lora_rx_rssi<-127) g_lora_rx_rssi = -127;
            if(g_lora_rx_snr> 127) g_lora_rx_snr = 127;
            else if(g_lora_rx_snr<-127) g_lora_rx_snr = -127;

            AppDataBuffer[5] = g_lora_rx_rssi;
            AppDataBuffer[6] = g_lora_rx_snr;

            //report g_num_send_heartbeat and  g_num_received_data_ack.
            AppDataBuffer[7] = (g_num_send_heartbeat & 0xFF00)>>8;
            AppDataBuffer[8] = (g_num_send_heartbeat & 0xFF);
            AppDataBuffer[9] = (g_num_received_data_ack & 0xFF00)>>8;
            AppDataBuffer[10] = (g_num_received_data_ack & 0xFF);

        }
        break;
    case LORAWAN_CONTROLLER_PORT:
        {
            AppDataSizeBackup = AppDataSize = LORAWAN_CONTROLLER_DATA_LEN;
            AppDataBuffer[0] = 0xDE;
        }
        break;
    case 224:
        if( ComplianceTest.LinkCheck == true )
        {
            ComplianceTest.LinkCheck = false;
            AppDataSize = 3;
            AppDataBuffer[0] = 5;
            AppDataBuffer[1] = ComplianceTest.DemodMargin;
            AppDataBuffer[2] = ComplianceTest.NbGateways;
            ComplianceTest.State = 1;
        }
        else
        {
            switch( ComplianceTest.State )
            {
            case 4:
                ComplianceTest.State = 1;
                break;
            case 1:
                AppDataSize = 2;
                AppDataBuffer[0] = ComplianceTest.DownLinkCounter >> 8;
                AppDataBuffer[1] = ComplianceTest.DownLinkCounter;
                break;
            }
        }
        break;
    default:
        break;
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

    if( LoRaMacQueryTxPossible( AppDataSize, &txInfo ) != LORAMAC_STATUS_OK )
    {
        // Send empty frame in order to flush MAC commands
        mcpsReq.Type = MCPS_UNCONFIRMED;
        mcpsReq.Req.Unconfirmed.fBuffer = NULL;
        mcpsReq.Req.Unconfirmed.fBufferSize = 0;
        mcpsReq.Req.Unconfirmed.Datarate = LORAWAN_DEFAULT_DATARATE;
    }
    else
    {
        if( IsTxConfirmed == false )
        {
            mcpsReq.Type = MCPS_UNCONFIRMED;
            mcpsReq.Req.Unconfirmed.fPort = AppPort;
            mcpsReq.Req.Unconfirmed.fBuffer = AppDataBuffer;
            mcpsReq.Req.Unconfirmed.fBufferSize = AppDataSize;
            mcpsReq.Req.Unconfirmed.Datarate = LORAWAN_DEFAULT_DATARATE;
        }
        else
        {
            mcpsReq.Type = MCPS_CONFIRMED;
            mcpsReq.Req.Confirmed.fPort = AppPort;
            mcpsReq.Req.Confirmed.fBuffer = AppDataBuffer;
            mcpsReq.Req.Confirmed.fBufferSize = AppDataSize;
            mcpsReq.Req.Confirmed.NbTrials = LORA_NODE_CONFIRM_RETX_NUM;
            mcpsReq.Req.Confirmed.Datarate = LORAWAN_DEFAULT_DATARATE;
        }
    }

    // Update global variable
    AppData.MsgType = ( mcpsReq.Type == MCPS_CONFIRMED ) ? LORAMAC_HANDLER_CONFIRMED_MSG : LORAMAC_HANDLER_UNCONFIRMED_MSG;
    AppData.Port = mcpsReq.Req.Unconfirmed.fPort;
    AppData.Buffer = mcpsReq.Req.Unconfirmed.fBuffer;
    AppData.BufferSize = mcpsReq.Req.Unconfirmed.fBufferSize;

    LoRaMacStatus_t status;
    status = LoRaMacMcpsRequest( &mcpsReq );
    PRINTF( "\r\nLoRa:MCPS-Request-DATA,STATUS:%s\r\n", MacStatusStrings[status] );
    sLoRa_Status = LoRa_Sending;

    if( status == LORAMAC_STATUS_OK )
    {
        return false;
    }
    return true;
}

#if 0
void SendHeartBeat(void)
{
    if ((true == lorawan_module_get_join_status())&&(g_lora_class_switch_succes_flag ==true))
    {
        PrepareTxFrame( LORA_ETAG_PORT);
        if(0 == lorawan_module_send(1, 19, (char*)AppDataBuffer, 10, NULL))
        {
          PRINTF("LoRa: HeartBeat Data Tx, Rssi=%d,SNR=%d,NumTx=%d,NumRxAck=%d.\r\n",
                g_lora_rx_rssi,g_lora_rx_snr,g_num_send_heartbeat,g_num_received_data_ack);

          g_num_send_heartbeat++;
        }
    }
}
#endif

static void OnJoinBackoffTimerEvent( void )
{
    /*1,set next join slot*/
    int JoinInterval = JoinBackoffTimerInterval;

    if(LORA_NODE_DISABLE_JOIN_BACKOFF) JoinInterval = 30*1000;  //30 seconds

    if(JoinInterval > (NUM_MS_PER_SECOND*NUM_SECOND_PER_MINUTE*NUM_MINUTE_PER_HOUR))
        JoinInterval = NUM_MS_PER_SECOND*NUM_SECOND_PER_MINUTE*NUM_MINUTE_PER_HOUR;
    TimerSetValue( &JoinBackoffTimer,JoinInterval);
    TimerStart( &JoinBackoffTimer);

    PRINTF("LoRa: Next Join Delay Seconds =%d,NumofJoin=%d.\r\n",JoinInterval/NUM_MS_PER_SECOND,g_num_of_join_failed);

    /*2,join*/
    LORA_SetDeviceState(DEVICE_STATE_JOIN);
}

/*!
 * \brief Function executed on TxNextPacket Timeout event
 */
static void OnTxNextPacketTimerEvent( void )
{
    MibRequestConfirm_t mibReq;
    LoRaMacStatus_t status;

    TimerStop( &TxNextPacketTimer );

    mibReq.Type = MIB_NETWORK_ACTIVATION;
    status = LoRaMacMibGetRequestConfirm( &mibReq );

    if( status == LORAMAC_STATUS_OK )
    {
        if( mibReq.Param.NetworkActivation == ACTIVATION_TYPE_NONE )
        {
            // Network not joined yet. Try to join again
            JoinNetwork( );
        }
        else
        {
            DeviceState = WakeUpState;
            NextTx = true;
        }
    }
}

/*!
 * \brief Function executed on StartNextClassSwitch Timeout event
 */
static void OnStartNextClassSwitchTimerEvent( void )
{
    TimerStop( &ClassSwitchBackoffTimer );

    //start to send
    NextTx = true;
    lorawan_module_switch_class(g_current_target_lora_class_type);

    PRINTF("LoRa: switching to ClassB from ClassA,waiting 2 minutes...\r\n");
}

/*!
 * \brief Function executed on ClassSwitchRespTimeoutTimer Timeout event
 */
static void OnClassSwitchRespTimeoutTimerEvent( void )
{
    //timer_set(&TimerSwitchClassBResultCheck, TIMER_SWITCH_CLASSB_RESULT_CHECK);
    PRINTF("LoRa: happen ClassBSwitchRespTimeout.\r\n");

    TimerStop( &ClassSwitchRespTimeoutTimer);

    //reint
    LORA_ReInit();
}

static void OnRadioStatusEvent(void)
{
    static int pre_cnt = 0;
    int cur_cnt = LoRaMacGetEventCnt();
    PRINTNOW();PRINTF("LoRa: Radio status checked.\r\n");
    if (cur_cnt == pre_cnt) {
        PRINTF("LoRa: Critical error, reinit lora\r\n");
        LORA_ReInit();
    }

    pre_cnt = cur_cnt;
    TimerStart(&RadioStatusTimer);
}

/*!
 * \brief   MCPS-Confirm event function
 *
 * \param   [IN] mcpsConfirm - Pointer to the confirm structure,
 *               containing confirm attributes.
 */
static void McpsConfirm( McpsConfirm_t *mcpsConfirm )
{
    PRINTF( "\r\nLoRa:MCPS-Confirm,STATUS: %s\r\n", EventInfoStatusStrings[mcpsConfirm->Status] );
    if( mcpsConfirm->Status != LORAMAC_EVENT_INFO_STATUS_OK )
    {
        /*notify upper layer */
        LoRaControllerCallbacks.LORA_SendDataFailed();
    }
    else
    {
        switch( mcpsConfirm->McpsRequest )
        {
            case MCPS_UNCONFIRMED:
            {
                // Check Datarate
                // Check TxPower

                /*notify upper layer */
                LoRaControllerCallbacks.LORA_SendDataOkWithoutRx();
                break;
            }
            case MCPS_CONFIRMED:
            {
                // Check Datarate
                // Check TxPower
                // Check AckReceived
                // Check NbTrials

                /*notify upper layer */
                if(mcpsConfirm->AckReceived)  LoRaControllerCallbacks.LORA_SendDataOkWithoutRx();
                else LoRaControllerCallbacks.LORA_SendDataFailed();

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

#if 0

    MibRequestConfirm_t mibGet;
    MibRequestConfirm_t mibReq;

    mibReq.Type = MIB_DEVICE_CLASS;
    LoRaMacMibGetRequestConfirm( &mibReq );

    PRINTF( "\r\n###### ===== UPLINK FRAME %lu ==== ######\r\n", mcpsConfirm->UpLinkCounter );
    PRINTF( "\r\n" );

    PRINTF( "CLASS       : %c\r\n", "ABC"[mibReq.Param.Class] );
    PRINTF( "\r\n" );
    PRINTF( "TX PORT     : %d\r\n", AppData.Port );

    if( AppData.BufferSize != 0 )
    {
        PRINTF( "TX DATA     : " );
        if( AppData.MsgType == LORAMAC_HANDLER_CONFIRMED_MSG )
        {
            PRINTF( "CONFIRMED - %s\r\n", ( mcpsConfirm->AckReceived != 0 ) ? "ACK" : "NACK" );
        }
        else
        {
            PRINTF( "UNCONFIRMED\r\n" );
        }
        PrintHexBuffer( AppData.Buffer, AppData.BufferSize );
    }

    PRINTF( "\r\n" );
    PRINTF( "DATA RATE   : DR_%d\r\n", mcpsConfirm->Datarate );

    mibGet.Type  = MIB_CHANNELS;
    if( LoRaMacMibGetRequestConfirm( &mibGet ) == LORAMAC_STATUS_OK )
    {
        PRINTF( "U/L FREQ    : %lu\r\n", mibGet.Param.ChannelList[mcpsConfirm->Channel].Frequency );
    }

    PRINTF( "TX POWER    : %d\r\n", mcpsConfirm->TxPower );

    mibGet.Type  = MIB_CHANNELS_MASK;
    if( LoRaMacMibGetRequestConfirm( &mibGet ) == LORAMAC_STATUS_OK )
    {
        PRINTF("CHANNEL MASK: ");
#if defined( REGION_AS923 ) || defined( REGION_CN779 ) || \
    defined( REGION_EU868 ) || defined( REGION_IN865 ) || \
    defined( REGION_KR920 ) || defined( REGION_EU433 ) || \
    defined( REGION_RU864 )

        for( uint8_t i = 0; i < 1; i++)

#elif defined( REGION_AU915 ) || defined( REGION_US915 ) || defined( REGION_CN470 )

        for( uint8_t i = 0; i < 5; i++)
#else

#error "Please define a region in the compiler options."

#endif
        {
            PRINTF("%04X ", mibGet.Param.ChannelsMask[i] );
        }
        PRINTF("\r\n");
    }

    PRINTF( "\r\n" );
#endif
}

/*!
 * \brief   MCPS-Indication event function
 *
 * \param   [IN] mcpsIndication - Pointer to the indication structure,
 *               containing indication attributes.
 */
static void McpsIndication( McpsIndication_t *mcpsIndication )
{
    PRINTF( "\r\nLoRa:FPending=%d,MCPS-Indication STATUS: %s\r\n", mcpsIndication->FramePending,EventInfoStatusStrings[mcpsIndication->Status] );
    if( mcpsIndication->Status != LORAMAC_EVENT_INFO_STATUS_OK )
    {
        return;
    }


    switch( mcpsIndication->McpsIndication )
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
            break;
        }
        default:
            break;
    }

    // Check Multicast
    // Check Port
    // Check Datarate
    // Check FramePending
    if(( mcpsIndication->FramePending == true )&&(CLASS_A == g_current_target_lora_class_type)&&(( MCPS_CONFIRMED == mcpsIndication->McpsIndication )))
    {
        // The server signals that it has pending data to be sent.
        // We schedule an uplink as soon as possible to flush the server.
        OnTxNextPacketTimerEvent( );
    }
    // Check Buffer
    // Check BufferSize
    // Check Rssi
    // Check Snr
    // Check RxSlot

    if( ComplianceTest.Running == true )
    {
        ComplianceTest.DownLinkCounter++;
    }

    if( mcpsIndication->RxData == true )
    {
        /*notify upper layer */
        LoRaControllerCallbacks.LORA_RxData(mcpsIndication->McpsIndication, mcpsIndication->Buffer, mcpsIndication->BufferSize, mcpsIndication->Port);

        switch( mcpsIndication->Port )
        {
        case 1: // The application LED can be controlled on port 1 or 2
        case 2:
            if( mcpsIndication->BufferSize == 1 )
            {
                AppLedStateOn = mcpsIndication->Buffer[0] & 0x01;
            }
            break;
        case 224:
            if( ComplianceTest.Running == false )
            {
                // Check compliance test enable command (i)
                if( ( mcpsIndication->BufferSize == 4 ) &&
                    ( mcpsIndication->Buffer[0] == 0x01 ) &&
                    ( mcpsIndication->Buffer[1] == 0x01 ) &&
                    ( mcpsIndication->Buffer[2] == 0x01 ) &&
                    ( mcpsIndication->Buffer[3] == 0x01 ) )
                {
                    IsTxConfirmed = false;
                    AppPort = 224;
                    AppDataSizeBackup = AppDataSize;
                    AppDataSize = 2;
                    ComplianceTest.DownLinkCounter = 0;
                    ComplianceTest.LinkCheck = false;
                    ComplianceTest.DemodMargin = 0;
                    ComplianceTest.NbGateways = 0;
                    ComplianceTest.Running = true;
                    ComplianceTest.uplink_cycle = 5;
                    ComplianceTest.State = 1;

                    MibRequestConfirm_t mibReq;
                    mibReq.Type = MIB_ADR;
                    mibReq.Param.AdrEnable = true;
                    LoRaMacMibSetRequestConfirm( &mibReq );

#if defined( REGION_EU868 )
                    LoRaMacTestSetDutyCycleOn( false );
#endif
                    WakeUpState = DEVICE_STATE_SEND;
                    DeviceState = DEVICE_STATE_SEND;
                    PRINTF( "\r\n###### ===== Enter Certification Mode! ==== ######\r\n" );
                }
            }
            else
            {
                ComplianceTest.State = mcpsIndication->Buffer[0];
                switch( ComplianceTest.State )
                {
                case 0: // Check compliance test disable command (ii)
                    IsTxConfirmed = LORAWAN_CONFIRMED_MSG_ON;
                    AppPort = LORAWAN_APP_PORT;
                    AppDataSize = AppDataSizeBackup;
                    ComplianceTest.DownLinkCounter = 0;
                    ComplianceTest.Running = false;

                    MibRequestConfirm_t mibReq;
                    mibReq.Type = MIB_ADR;
                    mibReq.Param.AdrEnable = LORAWAN_ADR_ON;
                    LoRaMacMibSetRequestConfirm( &mibReq );
#if defined( REGION_EU868 )
                    LoRaMacTestSetDutyCycleOn( LORAWAN_DUTYCYCLE_ON );
#endif
                    PRINTF( "\r\n###### ===== Exit Certification Mode! ==== ######\r\n" );
                    break;
                case 1: // (iii, iv)
                    AppDataSize = 2;
                    break;
                case 2: // Enable confirmed messages (v)
                    IsTxConfirmed = true;
                    ComplianceTest.State = 1;
                    break;
                case 3:  // Disable confirmed messages (vi)
                    IsTxConfirmed = false;
                    ComplianceTest.State = 1;
                    break;
                case 4: // (vii)
                    AppDataSize = mcpsIndication->BufferSize;

                    AppDataBuffer[0] = 4;
                    for( uint8_t i = 1; i < MIN( AppDataSize, LORAWAN_APP_DATA_MAX_SIZE ); i++ )
                    {
                        AppDataBuffer[i] = mcpsIndication->Buffer[i] + 1;
                    }
                    break;
                case 5: // (viii)
                    {
                        MlmeReq_t mlmeReq;
                        mlmeReq.Type = MLME_LINK_CHECK;
                        LoRaMacStatus_t status = LoRaMacMlmeRequest( &mlmeReq );
                        PRINTF( "\r\n###### ===== MLME-Request - MLME_LINK_CHECK ==== ######\r\n" );
                        PRINTF( "STATUS      : %s\r\n", MacStatusStrings[status] );
                    }
                    break;
                case 6: // (ix)
                    {
                        // Disable TestMode and revert back to normal operation
                        IsTxConfirmed = LORAWAN_CONFIRMED_MSG_ON;
                        AppPort = LORAWAN_APP_PORT;
                        AppDataSize = AppDataSizeBackup;
                        ComplianceTest.DownLinkCounter = 0;
                        ComplianceTest.Running = false;

                        MibRequestConfirm_t mibReq;
                        mibReq.Type = MIB_ADR;
                        mibReq.Param.AdrEnable = LORAWAN_ADR_ON;
                        LoRaMacMibSetRequestConfirm( &mibReq );
#if defined( REGION_EU868 )
                        LoRaMacTestSetDutyCycleOn( LORAWAN_DUTYCYCLE_ON );
#endif

                        JoinNetwork( );
                    }
                    break;
                case 7: // (x)
                    {
                        if( mcpsIndication->BufferSize == 3 )
                        {
                            MlmeReq_t mlmeReq;
                            mlmeReq.Type = MLME_TXCW;
                            mlmeReq.Req.TxCw.Timeout = ( uint16_t )( ( mcpsIndication->Buffer[1] << 8 ) | mcpsIndication->Buffer[2] );
                            LoRaMacStatus_t status = LoRaMacMlmeRequest( &mlmeReq );
                            PRINTF( "\r\n###### ===== MLME-Request - MLME_TXCW ==== ######\r\n" );
                            PRINTF( "STATUS      : %s\r\n", MacStatusStrings[status] );
                        }
                        else if( mcpsIndication->BufferSize == 7 )
                        {
                            MlmeReq_t mlmeReq;
                            mlmeReq.Type = MLME_TXCW_1;
                            mlmeReq.Req.TxCw.Timeout = ( uint16_t )( ( mcpsIndication->Buffer[1] << 8 ) | mcpsIndication->Buffer[2] );
                            mlmeReq.Req.TxCw.Frequency = ( uint32_t )( ( mcpsIndication->Buffer[3] << 16 ) | ( mcpsIndication->Buffer[4] << 8 ) | mcpsIndication->Buffer[5] ) * 100;
                            mlmeReq.Req.TxCw.Power = mcpsIndication->Buffer[6];
                            LoRaMacStatus_t status = LoRaMacMlmeRequest( &mlmeReq );
                            PRINTF( "\r\n###### ===== MLME-Request - MLME_TXCW1 ==== ######\r\n" );
                            PRINTF( "STATUS      : %s\r\n", MacStatusStrings[status] );
                        }
                        ComplianceTest.State = 1;
                    }
                    break;
                case 8: // Send DeviceTimeReq
                    {
                        MlmeReq_t mlmeReq;

                        mlmeReq.Type = MLME_DEVICE_TIME;

                        LoRaMacStatus_t status = LoRaMacMlmeRequest( &mlmeReq );
                        if (status != LORAMAC_STATUS_OK)
                        {
                            PRINTF("LoRa:STATUS error: %s\r\n", MacStatusStrings[status]);
                        }
                        else
                        {
                            WakeUpState = DEVICE_STATE_SEND;
                            DeviceState = DEVICE_STATE_SEND;
                        }
                    }
                    break;
                case 9: // Switch end device Class
                    {
                        // CLASS_A = 0, CLASS_B = 1, CLASS_C = 2
                        DeviceClass_t class;
                        class = ( DeviceClass_t )mcpsIndication->Buffer[1];
                        lorawan_module_switch_class(class);
                        if (class == CLASS_A || class == CLASS_C)
                        {
                            ComplianceTest.State = 1;
                        }
                        else if (class == CLASS_B)
                        {
                            // Disable TestMode and revert back to normal operation
                            IsTxConfirmed = LORAWAN_CONFIRMED_MSG_ON;
                            AppPort = LORAWAN_APP_PORT;
                            AppDataSize = AppDataSizeBackup;
                            ComplianceTest.DownLinkCounter = 0;
                            ComplianceTest.Running = false;

                            MibRequestConfirm_t mibReq;
                            mibReq.Type = MIB_ADR;
                            mibReq.Param.AdrEnable = LORAWAN_ADR_ON;
                            LoRaMacMibSetRequestConfirm( &mibReq );
#if defined( REGION_EU868 )
                            LoRaMacTestSetDutyCycleOn( LORAWAN_DUTYCYCLE_ON );
#endif
                            PRINTF( "\r\n###### ===== Exit Certification Mode! ==== ######\r\n" );
                        }
                    }
                    break;
                case 10: // Send PingSlotInfoReq
                    {
                        MlmeReq_t mlmeReq;

                        mlmeReq.Type = MLME_PING_SLOT_INFO;

                        mlmeReq.Req.PingSlotInfo.PingSlot.Value = mcpsIndication->Buffer[1];

                        LoRaMacStatus_t status = LoRaMacMlmeRequest( &mlmeReq );
                        if (status != LORAMAC_STATUS_OK)
                        {
                            PRINTF("LoRa:STATUS error: %s\r\n", MacStatusStrings[status]);
                        }
                        else
                        {
                            WakeUpState = DEVICE_STATE_SEND;
                            DeviceState = DEVICE_STATE_SEND;
                        }
                    }
                    break;
                case 11: // Send BeaconTimingReq
                    {
                        MlmeReq_t mlmeReq;

                        mlmeReq.Type = MLME_BEACON_TIMING;

                        LoRaMacStatus_t status = LoRaMacMlmeRequest( &mlmeReq );
                        if (status != LORAMAC_STATUS_OK)
                        {
                            PRINTF("LoRa:STATUS error: %s\r\n", MacStatusStrings[status]);
                        }
                        else
                        {
                            WakeUpState = DEVICE_STATE_SEND;
                            DeviceState = DEVICE_STATE_SEND;
                        }
                    }
                    break;
                case 0x81:
                    {
                        if (mcpsIndication->BufferSize == 2 && mcpsIndication->Buffer[1] != 0)
                        {
                            ComplianceTest.uplink_cycle = mcpsIndication->Buffer[1];
                            ComplianceTest.State = 1;
                        }
                        else
                        {
                            PRINTF("LoRa:param error\r\n");
                        }
                    }
                    break;
                case 0x82:
                    {
                        // Don't disable TestMode
                        MibRequestConfirm_t mibReq;
                        mibReq.Type = MIB_ADR;
                        mibReq.Param.AdrEnable = LORAWAN_ADR_ON;
                        LoRaMacMibSetRequestConfirm( &mibReq );
#if defined( REGION_EU868 )
                        LoRaMacTestSetDutyCycleOn( LORAWAN_DUTYCYCLE_ON );
#endif

                        JoinNetwork( );
                        ComplianceTest.State = 1;
                    }
                    break;
                default:
                    break;
                }
            }
            break;
        default:
            break;
        }
    }
    else
    {
        PRINTF("LoRa:McpsIndication: no RxData\r\n");
    }

    const char *slotStrings[] = { "1", "2", "C", "Ping-Slot", "Multicast Ping-Slot" };
    int8_t snr = mcpsIndication->Snr;

    /*notify upper layer */
    LoRaControllerCallbacks.LORA_SetRssiSnr(mcpsIndication->Rssi,snr);

    PRINTF( "\r\nLoRa:DOWNLINK FRAME %lu ==== ######\r\n", mcpsIndication->DownLinkCounter );

    PRINTNOW();PRINTF( "RX WINDOW   : %s\r\n", slotStrings[mcpsIndication->RxSlot] );

    PRINTF( "RX PORT     : %d\r\n", mcpsIndication->Port );

    if( mcpsIndication->BufferSize != 0 )
    {
        PRINTF( "RX DATA     : \r\n" );
        PrintHexBuffer( mcpsIndication->Buffer, mcpsIndication->BufferSize );
    }

    PRINTF( "\r\n" );
    PRINTF( "DATA RATE   : DR_%d\r\n", mcpsIndication->RxDatarate );
    PRINTF( "RX RSSI     : %d\r\n", mcpsIndication->Rssi );
    PRINTF( "RX SNR      : %d\r\n", snr );

    PRINTF( "\r\n" );
}

/*!
 * \brief   MLME-Confirm event function
 *
 * \param   [IN] mlmeConfirm - Pointer to the confirm structure,
 *               containing confirm attributes.
 */
static void MlmeConfirm( MlmeConfirm_t *mlmeConfirm )
{
    MibRequestConfirm_t mibReq;

    PRINTF( "\r\nLoRa:MLME-Confirm,STATUS: %s\r\n", EventInfoStatusStrings[mlmeConfirm->Status] );
    if( mlmeConfirm->Status != LORAMAC_EVENT_INFO_STATUS_OK )
    {
    }
    switch( mlmeConfirm->MlmeRequest )
    {
        case MLME_JOIN:
        {
            if( mlmeConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK )
            {
                MibRequestConfirm_t mibGet;
                //PRINTF( "LoRa:JOINED Successful,OTAA.\r\n\r\n" );

                mibGet.Type = MIB_DEV_ADDR;
                LoRaMacMibGetRequestConfirm( &mibGet );
                PRINTF( "DevAddr     : %08lX\r\n", mibGet.Param.DevAddr );

                PRINTF( "\n\r\n" );
                mibGet.Type = MIB_CHANNELS_DATARATE;
                LoRaMacMibGetRequestConfirm( &mibGet );
                PRINTF( "DATA RATE   : DR_%d\r\n", mibGet.Param.ChannelsDatarate );
                PRINTF( "\r\n" );
                // Status is OK, node has joined the network
/*
#if defined( USE_BEACON_TIMING )
                DeviceState = DEVICE_STATE_REQ_BEACON_TIMING;
#else
                DeviceState = DEVICE_STATE_REQ_DEVICE_TIME;
#endif
*/
                /*notify upper layer */
                LoRaControllerCallbacks.LORA_JoinResult(true);

            }
            else
            {
                // Join was not successful. Try to join again
                //PRINTF( "LoRa:JOIN failed,Try to join later ######\r\n" );
                //DelayMs(10*1000);
                //JoinNetwork( );

                /*notify upper layer */
                LoRaControllerCallbacks.LORA_JoinResult(false);
            }
            break;
        }
        case MLME_LINK_CHECK:
        {
            if( mlmeConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK )
            {
                // Check DemodMargin
                // Check NbGateways
                if( ComplianceTest.Running == true )
                {
                    ComplianceTest.LinkCheck = true;
                    ComplianceTest.DemodMargin = mlmeConfirm->DemodMargin;
                    ComplianceTest.NbGateways = mlmeConfirm->NbGateways;
                }
            }
            break;
        }
        case MLME_DEVICE_TIME:
        {
            if(g_current_lora_class_type != CLASS_B) //not need when under classB sending DEVICE_TIME_Req for Beacon Miss
            {
                if( mlmeConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK )
                {
                    // Setup the WakeUpState to DEVICE_STATE_SEND. This allows the
                    // application to initiate MCPS requests during a beacon acquisition
                    WakeUpState = DEVICE_STATE_SEND;
                    // Switch to the next state immediately
                    LORA_SetDeviceState( DEVICE_STATE_BEACON_ACQUISITION );
                    NextTx = true;
                    PRINTF( "LoRa:Received MLME_DEVICE_TIME_ANS,start BEACON_ACQUISITION. \n\r\n" );
                }
                else
                {
                    PRINTF( "LoRa:DEVICE_TIME_ANS Not Received.Switch Class failed.\n\r\n" );
                    /*notify upper layer */
                    NextTx = true;
                    LoRaControllerCallbacks.LORA_SwitchClassResult(false);
                }
            }
            else
            {
                g_num_beacon_missed = 0;
            }
            break;
        }
        case MLME_BEACON_TIMING:
        {
            // Setup the WakeUpState to DEVICE_STATE_SEND. This allows the
            // application to initiate MCPS requests during a beacon acquisition
            WakeUpState = DEVICE_STATE_SEND;
            // Switch to the next state immediately
			LORA_SetDeviceState( DEVICE_STATE_BEACON_ACQUISITION );
            NextTx = true;
            break;
        }
        case MLME_BEACON_ACQUISITION:
        {
            if( mlmeConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK )
            {
                if(g_current_lora_class_type != CLASS_B) //not need when under classB sending DEVICE_TIME_Req for Beacon Miss
                {
                    WakeUpState = DEVICE_STATE_SEND;
                    // Switch to the next state immediately
                    LORA_SetDeviceState( DEVICE_STATE_REQ_PINGSLOT_ACK );
                    NextTx = true;
                    PRINTF( "LoRa:Beacon Found,start PINGSLOT_REQ.\n\r\n" );
                }
            }
            else
            {
                PRINTF( "LoRa:Beacon Not Found.Switch Class failed.\n\r\n" );
/*
#if defined( USE_BEACON_TIMING )
                WakeUpState = DEVICE_STATE_REQ_BEACON_TIMING;
#else
                WakeUpState = DEVICE_STATE_REQ_DEVICE_TIME;
#endif
*/
                /*notify upper layer */
                NextTx = true;
                LoRaControllerCallbacks.LORA_SwitchClassResult(false);
            }
            break;
        }
        case MLME_PING_SLOT_INFO:
        {
            if( mlmeConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK )
            {
                mibReq.Type = MIB_DEVICE_CLASS;
                mibReq.Param.Class = CLASS_B;
                LoRaMacMibSetRequestConfirm( &mibReq );

                //PRINTF( "\r\n\r\n###### ===== Switch to Class B done. ==== ######\r\n\r\n" );

                WakeUpState = DEVICE_STATE_SEND;
                LORA_SetDeviceState( WakeUpState );
                NextTx = true;
                g_num_of_pingslotack_failed = 0;

                /*notify upper layer */
                LoRaControllerCallbacks.LORA_SwitchClassResult(true);
            }
            else
            {
                //WakeUpState = DEVICE_STATE_REQ_PINGSLOT_ACK;

                g_num_of_pingslotack_failed++;

                if (g_num_of_pingslotack_failed < LORA_NODE_NUM_PINGSLOTINFORACK_FAILED_TO_REINIT)
                {
                    WakeUpState = DEVICE_STATE_SEND;
                    // Switch to the next state immediately
                    LORA_SetDeviceState( DEVICE_STATE_REQ_PINGSLOT_ACK );
                    NextTx = true;
                    PRINTF( "LoRa:PINGSLOT_ACK Not Received,start PINGSLOT_REQ again.\n\r\n" );
                }
                else
                {
                    PRINTF( "LoRa:PINGSLOT_ACK Not Received. Switch Class failed.\n\r\n" );
                    /*notify upper layer */
                    NextTx = true;
                    g_num_of_pingslotack_failed = 0;

                    LoRaControllerCallbacks.LORA_SwitchClassResult(false);
                }
            }
            break;
        }
        default:
            break;
    }
}

/*!
 * \brief   MLME-Indication event function
 *
 * \param   [IN] mlmeIndication - Pointer to the indication structure.
 */
static void MlmeIndication( MlmeIndication_t *mlmeIndication )
{
    MibRequestConfirm_t mibReq;

    if( mlmeIndication->Status != LORAMAC_EVENT_INFO_STATUS_BEACON_LOCKED )
    {
        PRINTF( "\r\nLoRa:MLME-Indication,STATUS: %s\r\n", EventInfoStatusStrings[mlmeIndication->Status] );
    }
    if( mlmeIndication->Status != LORAMAC_EVENT_INFO_STATUS_OK )
    {
    }
    switch( mlmeIndication->MlmeIndication )
    {
        case MLME_JOIN:
        {// The MAC signals that we shall provide an join as soon as possible
            PRINTF("LoRa:LoRaMac trigger a MLME_JOIN.\n\r");
            //rejoin
            LORA_SetDeviceState(DEVICE_STATE_REJOIN);
            break;
        }
        case MLME_SCHEDULE_UPLINK:
        {// The MAC signals that we shall provide an uplink as soon as possible
            PRINTF("LoRa:LoRaMac trigger a MLME_SCHEDULE_UPLINK,DeviceState=%d.\n\r",DeviceState);
            NextTx = true;//OnTxNextPacketTimerEvent( );
            LORA_SetDeviceState(DEVICE_STATE_SEND);
            break;
        }
        case MLME_BEACON_LOST:
        {
            mibReq.Type = MIB_DEVICE_CLASS;
            mibReq.Param.Class = CLASS_A;
            LoRaMacMibSetRequestConfirm( &mibReq );

            PRINTF( "\r\n\r\nLoRa: Switch to Class A done. ==== ######\r\n\r\n" );

            // Switch to class A again
#if defined( USE_BEACON_TIMING )
            WakeUpState = DEVICE_STATE_REQ_BEACON_TIMING;
#else
            WakeUpState = DEVICE_STATE_REQ_DEVICE_TIME;
#endif
            PRINTF( "\r\nLoRa: BEACON LOST ==== ######\r\n" );
            /*notify upper layer */
            LoRaControllerCallbacks.LORA_ClassBLostSync();

            break;
        }
        case MLME_BEACON:
        {
            if( mlmeIndication->Status == LORAMAC_EVENT_INFO_STATUS_BEACON_LOCKED )
            {
                PRINTF( "LoRa:BEACON RECEIVED.\n\r");
                PRINTF( "\r\n###### ===== BEACON %lu ==== ######\r\n", mlmeIndication->BeaconInfo.Time );
                PRINTF( "GW DESC     : %d\r\n", mlmeIndication->BeaconInfo.GwSpecific.InfoDesc );
                PRINTF( "GW INFO     : " );
                PrintHexBuffer( mlmeIndication->BeaconInfo.GwSpecific.Info, 6 );
                PRINTF( "\r\n" );
                PRINTF( "FREQ        : %lu\r\n", mlmeIndication->BeaconInfo.Frequency );
                PRINTF( "DATA RATE   : DR_%d\r\n", mlmeIndication->BeaconInfo.Datarate );
                PRINTF( "RX RSSI     : %d\r\n", mlmeIndication->BeaconInfo.Rssi );

                int32_t snr = 0;
                if( mlmeIndication->BeaconInfo.Snr & 0x80 ) // The SNR sign bit is 1
                {
                    // Invert and divide by 4
                    snr = ( ( ~mlmeIndication->BeaconInfo.Snr + 1 ) & 0xFF ) >> 2;
                    snr = -snr;
                }
                else
                {
                    // Divide by 4
                    snr = ( mlmeIndication->BeaconInfo.Snr & 0xFF ) >> 2;
                }
                PRINTF( "RX SNR      : %ld\r\n", snr );
                PRINTF( "\r\n" );

                g_num_beacon_missed= 0;

            }
            else
            {
                PRINTF( "\r\nLoRa: BEACON NOT RECEIVED ==== ######\r\n" );
                g_num_beacon_missed++;
                if(g_num_beacon_missed >= LORA_NODE_NUM_BEACONMISS_TO_SEND_DEVICETIMEREQ)
                {
                    PRINTF("LoRa:BeaconMissedTimesIsGreaterThanDefined,willSendDeviceReq,currState=%d.\n\r",DeviceState);
                    NextTx = true;//OnTxNextPacketTimerEvent( );
                    LORA_SetDeviceState(DEVICE_STATE_REQ_DEVICE_TIME);
                }
            }
            break;
        }
        default:
            break;
    }
}

static void ConnectedProcess(void)
{
    bool cycleSendEnable = false;
    g_num_of_class_switch_failed = 0;
    g_lora_class_switch_succes_flag = true;
    sLoRa_Status = LoRa_Idle;

    //timer_stop(&TimerSwitchClassBResultCheck);

    g_num_received_data_ack = 0;
    g_num_send_heartbeat        = 0;

    #if LORA_MODULE_SEND_HEARTBEAT
    PRINTF( "\r\nLoRa: Enter Data Cycle Send. ==== ######\r\n" );
    cycleSendEnable = true;
    #endif

    if ((g_current_lora_class_type == CLASS_B) ||(cycleSendEnable))
    {
        //classB:ensure NS receive the first package with classB bit.
        NextTx = true;
        LORA_SetDeviceState(DEVICE_STATE_SEND);
        WakeUpState = DEVICE_STATE_SEND;

        TimerStart( &TxNextPacketTimer );
    }
    //call CB to notify App
    if (g_lora_esl_connection_ops_cb->on_connect) g_lora_esl_connection_ops_cb->on_connect(true, g_lora_cm_data);

}

void LORA_ResetParameters(void)
{
    //Board_LoRa_Module_Reset();
    ////osDelay(500);
    //set to join

    //reset counter
    g_num_send_heartbeat = 0;
    g_num_received_data_ack = 0;
    //join backoff
    sLoRa_Status = LoRa_Initing;

    g_num_of_join_failed =0;
    g_num_send_data_failed_to_rejoin = 0;
    g_num_of_class_switch_failed = 0;
    g_lora_class_switch_succes_flag = true;
    g_current_lora_class_type = CLASS_A;

    g_num_beacon_missed = 0;
    g_num_of_pingslotack_failed = 0;


    //Board_RTC_Wakeup_Timer_Stop();

    //timer_set(&TimerResponseCheck, 100);    // to check the response of the command from LoRaWAN module
    //timer_set(&TimerResultCheck, LORA_JOIN_TIMEOUT_VALUE);    // to check the result of the command to LoRaWAN module

}

static void LORA_ReInit(void)
{
    //call CB to notify App
    if (g_lora_esl_connection_ops_cb->on_disconnect) g_lora_esl_connection_ops_cb->on_disconnect(g_lora_cm_data);

    LORA_ResetParameters();
    //lorawan_module_join();
    LORA_SetDeviceState(DEVICE_STATE_REINIT);
}

#if 0
static void LORA_ReJoin(void)
{
    LORA_ResetParameters();
    //lorawan_module_join();
    LORA_SetDeviceState(DEVICE_STATE_JOIN);
}
#endif

DeviceState_t LORA_DeviceProcess(void) {
    MibRequestConfirm_t mibReq;
    LoRaMacStatus_t status;

    switch( DeviceState )
    {
        case DEVICE_STATE_REINIT:
        {
            PRINTF( "LoRa:start REINIT\n\r");
            LoRaMacInitialization( &LoRaMacPrimitives, &LoRaMacCallbacks, ACTIVE_REGION );
            lorawan_mc_setup();
            DeviceState = DEVICE_STATE_RESTORE;
            WakeUpState = DEVICE_STATE_START;
            TimerInit( &JoinBackoffTimer, OnJoinBackoffTimerEvent );
            TimerStop( &TxNextPacketTimer );
            NextTx = true;

            break;
        }
        case DEVICE_STATE_RESTORE:
        {
            // Try to restore from NVM and query the mac if possible.
            if (0)//( NvmCtxMgmtRestore( ) == NVMCTXMGMT_STATUS_SUCCESS )
            {
                PRINTF( "\r\nLoRa:###### ===== CTXS RESTORED ==== ######\r\n\r\n" );
            }
            else
            {
                mibReq.Type = MIB_APP_KEY;
                mibReq.Param.AppKey = AppKey;
                LoRaMacMibSetRequestConfirm( &mibReq );

                mibReq.Type = MIB_NWK_KEY;
                mibReq.Param.NwkKey = NwkKey;
                LoRaMacMibSetRequestConfirm( &mibReq );

                // Initialize LoRaMac device unique ID if not already defined in Commissioning.h
                if( ( DevEui[0] == 0 ) && ( DevEui[1] == 0 ) &&
                    ( DevEui[2] == 0 ) && ( DevEui[3] == 0 ) &&
                    ( DevEui[4] == 0 ) && ( DevEui[5] == 0 ) &&
                    ( DevEui[6] == 0 ) && ( DevEui[7] == 0 ) )
                {
                    //BoardGetUniqueId( DevEui );
                }

#if( OVER_THE_AIR_ACTIVATION == 0 )
                // Choose a random device address if not already defined in Commissioning.h
                if( DevAddr == 0 )
                {
                    // Random seed initialization
                    srand1( BoardGetRandomSeed( ) );

                    // Choose a random device address
                    DevAddr = randr( 0, 0x01FFFFFF );
                }

                mibReq.Type = MIB_NET_ID;
                mibReq.Param.NetID = LORAWAN_NETWORK_ID;
                LoRaMacMibSetRequestConfirm( &mibReq );

                mibReq.Type = MIB_DEV_ADDR;
                mibReq.Param.DevAddr = DevAddr;
                LoRaMacMibSetRequestConfirm( &mibReq );

                mibReq.Type = MIB_F_NWK_S_INT_KEY;
                mibReq.Param.FNwkSIntKey = FNwkSIntKey;
                LoRaMacMibSetRequestConfirm( &mibReq );

                mibReq.Type = MIB_S_NWK_S_INT_KEY;
                mibReq.Param.SNwkSIntKey = SNwkSIntKey;
                LoRaMacMibSetRequestConfirm( &mibReq );

                mibReq.Type = MIB_NWK_S_ENC_KEY;
                mibReq.Param.NwkSEncKey = NwkSEncKey;
                LoRaMacMibSetRequestConfirm( &mibReq );

                mibReq.Type = MIB_APP_S_KEY;
                mibReq.Param.AppSKey = AppSKey;
                LoRaMacMibSetRequestConfirm( &mibReq );
#endif
            }
            DeviceState = DEVICE_STATE_START;
            break;
        }

        case DEVICE_STATE_START:
        {
            TimerInit( &TxNextPacketTimer, OnTxNextPacketTimerEvent );

            mibReq.Type = MIB_PUBLIC_NETWORK;
            mibReq.Param.EnablePublicNetwork = LORAWAN_PUBLIC_NETWORK;
            LoRaMacMibSetRequestConfirm( &mibReq );

            mibReq.Type = MIB_ADR;
            mibReq.Param.AdrEnable = LORAWAN_ADR_ON;
            LoRaMacMibSetRequestConfirm( &mibReq );

#if defined( REGION_EU868 )
            LoRaMacTestSetDutyCycleOn( LORAWAN_DUTYCYCLE_ON );
#endif
            mibReq.Type = MIB_SYSTEM_MAX_RX_ERROR;
            mibReq.Param.SystemMaxRxError = 15;
            LoRaMacMibSetRequestConfirm( &mibReq );

            LoRaMacStart( );

            //set to Join
            mibReq.Type = MIB_NETWORK_ACTIVATION;
            status = LoRaMacMibGetRequestConfirm( &mibReq );

            if( status == LORAMAC_STATUS_OK )
            {
                if( mibReq.Param.NetworkActivation == ACTIVATION_TYPE_NONE )
                {
                    DeviceState = DEVICE_STATE_JOIN;
                }
                else
                {
                    DeviceState = DEVICE_STATE_SEND;
                    NextTx = true;
                }
            }
            break;
        }
        case DEVICE_STATE_JOIN:
        {
#if( OVER_THE_AIR_ACTIVATION != 0 )
			  PRINTF( "=======================================================\r\n" );
              PRINTF( "LoRa:OTAA\r\n");
              PRINTF( "LoRa:DevEui= %02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X\r\n", HEX8(DevEui));
              PRINTF( "LoRa:AppEui= %02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X\r\n", HEX8(JoinEui));
              //PRINTF( "LoRa:AppKey= %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\r\n", HEX16(AppKey));
			  PRINTF( "=======================================================\r\n" );

#endif

#if( OVER_THE_AIR_ACTIVATION == 0 )
            PRINTF( "LoRa:###### ===== JOINED ==== ######\r\n" );
            PRINTF( "\r\nABP\r\n\r\n" );
            PRINTF( "DevAddr     : %08lX\r\n", DevAddr );
            PRINTF( "NwkSKey     : %02X", FNwkSIntKey[0] );
            for( int i = 1; i < 16; i++ )
            {
                PRINTF( " %02X", FNwkSIntKey[i] );
            }
            PRINTF( "\r\n" );
            PRINTF( "AppSKey     : %02X", AppSKey[0] );
            for( int i = 1; i < 16; i++ )
            {
                PRINTF( " %02X", AppSKey[i] );
            }
            PRINTF( "\n\r\n" );

            // Tell the MAC layer which network server version are we connecting too.
            mibReq.Type = MIB_ABP_LORAWAN_VERSION;
            mibReq.Param.AbpLrWanVersion.Value = ABP_ACTIVATION_LRWAN_VERSION;
            LoRaMacMibSetRequestConfirm( &mibReq );

            mibReq.Type = MIB_NETWORK_ACTIVATION;
            mibReq.Param.NetworkActivation = ACTIVATION_TYPE_ABP;
            LoRaMacMibSetRequestConfirm( &mibReq );

#if defined( USE_BEACON_TIMING )
            DeviceState = DEVICE_STATE_REQ_BEACON_TIMING;
#else
            DeviceState = DEVICE_STATE_REQ_DEVICE_TIME;
#endif
#else
            JoinNetwork( );
#endif
            break;
        }

        case DEVICE_STATE_REJOIN:
        {
            PRINTF( "LoRa:start REJOIN\n\r");
            NextTx = true;
            TimerStop( &TxNextPacketTimer );
            JoinNetwork( );

            break;
        }

        case DEVICE_STATE_REQ_DEVICE_TIME:
        {
            MlmeReq_t mlmeReq;

            if( NextTx == true )
            {
                mlmeReq.Type = MLME_DEVICE_TIME;

                if( LoRaMacMlmeRequest( &mlmeReq ) == LORAMAC_STATUS_OK )
                {
                    PRINTF( "LoRa:Send MLME_DEVICE_TIME_Req\n\r\n" );
                    WakeUpState = DEVICE_STATE_SEND;
                }
            }
            DeviceState = DEVICE_STATE_SEND;
            break;
        }
        case DEVICE_STATE_REQ_BEACON_TIMING:
        {
            MlmeReq_t mlmeReq;

            if( NextTx == true )
            {
                mlmeReq.Type = MLME_BEACON_TIMING;

                if( LoRaMacMlmeRequest( &mlmeReq ) == LORAMAC_STATUS_OK )
                {
                    WakeUpState = DEVICE_STATE_SEND;
                }
            }
            DeviceState = DEVICE_STATE_SEND;
            break;
        }
        case DEVICE_STATE_BEACON_ACQUISITION:
        {
            MlmeReq_t mlmeReq;

            if( NextTx == true )
            {
                mlmeReq.Type = MLME_BEACON_ACQUISITION;

                LoRaMacMlmeRequest( &mlmeReq );
                NextTx = false;
                PRINTF( "LoRa:Send MLME_BEACON_ACQUISITION_Req\n\r\n" );
            }
            //DeviceState = DEVICE_STATE_SEND;
            //DeviceState = DEVICE_STATE_CYCLE;
            DeviceState = DEVICE_STATE_SLEEP; //TODO:tgl
            break;
        }
        case DEVICE_STATE_REQ_PINGSLOT_ACK:
        {
            MlmeReq_t mlmeReq;

            if( NextTx == true )
            {
                mlmeReq.Type = MLME_LINK_CHECK;
                LoRaMacMlmeRequest( &mlmeReq );

                mlmeReq.Type = MLME_PING_SLOT_INFO;
                mlmeReq.Req.PingSlotInfo.PingSlot.Fields.Periodicity = LORAWAN_DEFAULT_PING_SLOT_PERIODICITY;
                mlmeReq.Req.PingSlotInfo.PingSlot.Fields.RFU = 0;

                if( LoRaMacMlmeRequest( &mlmeReq ) == LORAMAC_STATUS_OK )
                {
                    WakeUpState = DEVICE_STATE_SEND;
                }
                PRINTF( "LoRa:Send MLME_PING_SLOT_INFO_Req\n\r\n" );
            }
            DeviceState = DEVICE_STATE_SEND;
            break;
        }
        case DEVICE_STATE_SEND:
        {
            if( NextTx == true )
            {
                PrepareTxFrame(AppPort);
                //NextTx = SendFrame( );
                lorawan_module_send(IsTxConfirmed, AppPort, (char *)AppDataBuffer, AppDataSize, NULL);
            }
            DeviceState = DEVICE_STATE_CYCLE;
            break;
        }
        case DEVICE_STATE_CYCLE:
        {
            int dutytime = 180*NUM_MS_PER_SECOND;//180s=3minutes
            int nextNeedTx = false;

            DeviceState = DEVICE_STATE_SLEEP;
            if( ComplianceTest.Running == true )
            {
                // Schedule next packet transmission
                TxDutyCycleTime = ComplianceTest.uplink_cycle * 1000; // default 5000 ms
                nextNeedTx = true;
            }
            else
            {
                // Schedule next packet transmission
                if(true == g_lora_class_switch_succes_flag) //ClassA/C or Enter ClassB
                {
                    if((2 >= g_num_received_data_ack)&&(g_current_lora_class_type == CLASS_B))
                    {
                        //after enter classB, make sure node receive PingSlotChannelReq from NS to change DR.
                        dutytime = 15*1000;//10s
                        nextNeedTx = true;
                        AppPort    = LORAWAN_CONTROLLER_PORT;
                    }
                    else
                    {
                        #if LORA_MODULE_SEND_HEARTBEAT
                        //after class switch succesfully, heatbeat will be send by lora module;
                        dutytime = APP_TX_DUTYCYCLE;
                        nextNeedTx = true;
                        AppPort    = LORAWAN_APP_PORT;
                        #endif
                    }
                }
                else//ClassB:before switch succesfully, MAC Cmd need be send to NS by lora module;
                {
                    dutytime = 180*NUM_MS_PER_SECOND;//180s=3minutes
                    nextNeedTx = true;
                    AppPort    = LORAWAN_CONTROLLER_PORT;
                }

                TxDutyCycleTime = dutytime + randr( -APP_TX_DUTYCYCLE_RND, APP_TX_DUTYCYCLE_RND );
            }

            if(nextNeedTx == true)
            {
                // Schedule next packet transmission
                TimerSetValue( &TxNextPacketTimer, TxDutyCycleTime );
                TimerStart( &TxNextPacketTimer );
            }
            break;
        }
        case DEVICE_STATE_SLEEP:
        {
            //osThreadYield();

            #if 0
            DISABLE_IRQ( );
            /* if an interrupt has occurred after DISABLE_IRQ, it is kept pending
             * and cortex will not enter low power anyway  */
            #ifndef LOW_POWER_DISABLE
            LPM_EnterLowPower( );
            #endif

            ENABLE_IRQ();
            #endif
            /* USER CODE BEGIN 2 */
            /* USER CODE END 2 */

            break;
        }
        default:
        {
            DeviceState = DEVICE_STATE_START;
            break;
        }
    }

    return DeviceState;

}

static void LORA_InitFinished(void)
{

}

static void LORA_SwitchClassResult(int successful) //only for switch to classB
{

    //support only classA->classB or classA->classC
    if(g_current_lora_class_type == CLASS_A)
    {
        TimerStop( &ClassSwitchRespTimeoutTimer);
        sLoRa_Status = LoRa_Join_Success;

        if (successful )
        {
            ConnectedProcess();
            TimerStart(&RadioStatusTimer);

            //call CB to notify App
            if (g_lora_esl_connection_ops_cb->lora_classbswitch_result) g_lora_esl_connection_ops_cb->lora_classbswitch_result(true, g_lora_cm_data);

            g_current_lora_class_type = CLASS_B;
            PRINTF( "=======================================================\r\n" );
            PRINTF("LoRa:Switch ClassB successful, Send data to infrom NS.\r\n");
            PRINTF( "=======================================================\r\n" );

        }
        else
        {
            PRINTF("LoRa: Switch ClassB failed,NumFailed=%d.\r\n",g_num_of_class_switch_failed);
            g_num_of_class_switch_failed++;
            if(g_num_of_class_switch_failed < LORA_NODE_NUM_CLASS_SWITCH_FAILED_TO_REINIT)
            {

                /*1,set next class switch slot*/
                ClassSwitchBackoffTimerInterval = (1<<g_num_of_class_switch_failed)*NUM_MS_PER_SECOND*NUM_SECOND_PER_MINUTE +g_random_joindelay;//unit =ms; 1000*60s

                if(LORA_NODE_DISABLE_CLASS_SWITCH_BACKOFF) ClassSwitchBackoffTimerInterval = 30*1000;  //30 seconds

                if(ClassSwitchBackoffTimerInterval > (NUM_MS_PER_SECOND*NUM_SECOND_PER_MINUTE*NUM_MINUTE_PER_HOUR/2))
                    ClassSwitchBackoffTimerInterval = NUM_MS_PER_SECOND*NUM_SECOND_PER_MINUTE*NUM_MINUTE_PER_HOUR/2;

                TimerSetValue( &ClassSwitchBackoffTimer,ClassSwitchBackoffTimerInterval);
                TimerStart( &ClassSwitchBackoffTimer);

                TimerStop( &TxNextPacketTimer );

                PRINTF("LoRa:Switch ClassB failed,Next switch Delay Seconds =%d,NumofSwitchFailed=%d.\r\n",ClassSwitchBackoffTimerInterval/NUM_MS_PER_SECOND,g_num_of_class_switch_failed);

            }
            else
            {
                PRINTF("LoRa: Class Switch failed times is more than defined, start reJoin.\r\n");
                g_num_of_class_switch_failed = 0;
                LORA_ReInit();
            }
        }
    }
    else
    {
        PRINTF("LoRa: Class Switch happend unexpected, start reJoin.\r\n");
        LORA_ReInit();
    }
}

static void LORA_ClassBLostSync(void)
{
    PRINTF("LoRa: ClassB Lost Sync(lost many Beacons), start reJoin.\r\n");
    LORA_ReInit();
}

static void LORA_SendDataOkWithoutRx(void)
{
    PRINTF("LoRa: SendDataOkWithoutRx.\r\n");
    if((true == g_lora_class_switch_succes_flag)) sLoRa_Status = LoRa_Idle;

    g_num_send_data_failed_to_rejoin = 0;
    g_num_received_data_ack++;

    #if 0 //Test code for class switch
    if(g_current_lora_class_type == CLASS_B)
      lorawan_module_switch_class(CLASS_A);

    if((g_current_lora_class_type == CLASS_A)&&(g_num_received_data_ack == 4))
      lorawan_module_switch_class(CLASS_C);

    if((g_current_lora_class_type == CLASS_C)&&(g_num_received_data_ack == 6))
      lorawan_module_switch_class(CLASS_A);

    if((g_current_lora_class_type == CLASS_A)&&(g_num_received_data_ack == 8))
      lorawan_module_switch_class(CLASS_B);
    #endif

}


static void LORA_SendDataFailed(void)
{
    int num2reint = LORA_NODE_NUM_REJOIN_FAILED_TO_REINIT;
    PRINTF("LoRa: SendDataFailed.\r\n");
    if(true == g_lora_class_switch_succes_flag) sLoRa_Status = LoRa_Idle;

    if((0 == g_num_received_data_ack)&&(true == g_lora_class_switch_succes_flag))
    {
        //osDelay(1000);
        //SendHeartBeat();
        NextTx = true;
        LORA_SetDeviceState(DEVICE_STATE_SEND);
    }
    else
    {
        num2reint = LORA_NODE_NUM_SEND_FAILED_TO_REJOIN;
    }

    g_num_send_data_failed_to_rejoin++;

    if(g_num_send_data_failed_to_rejoin >num2reint)
    {
      g_num_send_data_failed_to_rejoin = 0;

      //rejoin
      PRINTF("LoRa: SendDataFailed times is more than defined, start reJoin.\r\n");
      LORA_ReInit();
    }
}


static void LORA_SetRssiSnr(int rssi,int snr)
{
    g_lora_rx_rssi = rssi;
    g_lora_rx_snr = snr;
}

static void LORA_JoinResult( int successful )
{
    #if LORA_ENABLE_SCAN_BETWEEN_2_FREQGROUPS
    MibRequestConfirm_t mibReq;
    LoRaMacStatus_t status;
    uint16_t ChannelsMask[CHANNELS_MASK_SIZE] ={LORA_CHANNELS_MASK_DEFAULT,0,0,0,0,0};
    uint16_t ChannelsMaskBand;
    #endif

    if(!successful )
    {
        PRINTF("\r\nLoRa: JoinResult failed,NumJoin=%d,try another FreqGroup.\r\n",g_num_of_join_failed);
        sLoRa_Status = LoRa_Join_Failed;

        g_num_of_join_failed++;

        if(g_num_of_join_failed > LORA_NODE_NUM_REJOIN_FAILED_TO_REINIT)
        {
            g_num_of_join_failed = LORA_NODE_NUM_REJOIN_FAILED_TO_REINIT;

            // wait the join backoff timer timeout, it has been started in JoinNetwork()
        }

        //Scan freq group between 1A1+1A2 when join failed.Default:disable the scane of FreqGroup
        #if LORA_ENABLE_SCAN_BETWEEN_2_FREQGROUPS
        //step1：get current FreqGroup
        mibReq.Type = MIB_CHANNELS_MASK;
        status = LoRaMacMibGetRequestConfirm( &mibReq);

        //step2：set another FreqGroup
        if( status == LORAMAC_STATUS_OK )
        {
            if( mibReq.Param.ChannelsMask != NULL)
            {
                for( uint8_t i = 0; i < CHANNELS_MASK_SIZE; i++ )
                {
                    ChannelsMask[i] = mibReq.Param.ChannelsMask[i];
                }
            }
            ChannelsMaskBand = ChannelsMask[0];
            //change to another freqGroup
            ChannelsMask[0]= ((ChannelsMaskBand & 0x00FF) <<8) +((ChannelsMaskBand & 0xFF00) >>8);

            mibReq.Param.ChannelsMask = ChannelsMask;
            LoRaMacMibSetRequestConfirm( &mibReq );
        }
        #endif

    }
    else
    {
        PRINTF("\r\n===============================================\r\n");
        PRINTF("\r\nLoRa: JoinResult is Successful,current Class=%d.\r\n",g_current_lora_class_type);
        PRINTF("\r\n===============================================\r\n");
        sLoRa_Status = LoRa_Join_Success;

        g_num_of_join_failed =0;
        TimerStop( &JoinBackoffTimer);

        if ((CLASS_B == g_current_target_lora_class_type)) lorawan_module_switch_class(g_current_target_lora_class_type);
        else ConnectedProcess();
    }
}

static int LORA_SwitchClass(DeviceClass_t newClass)
{
    LoraErrorStatus Errorstatus = LORA_SUCCESS;
    MibRequestConfirm_t mibReq;
    DeviceClass_t currentClass;

    mibReq.Type = MIB_DEVICE_CLASS;
    LoRaMacMibGetRequestConfirm( &mibReq );

    currentClass = mibReq.Param.Class;
    /*attempt to swicth only if class update*/
    if (currentClass != newClass)
    {
        switch (newClass)
        {
            case CLASS_A:
            {
                mibReq.Param.Class = CLASS_A;
                if( LoRaMacMibSetRequestConfirm( &mibReq ) == LORAMAC_STATUS_OK )
                {
                /*switch is instantanuous*/
                }
                else
                {
                  Errorstatus = LORA_ERROR;
                }
                break;
            }
            case CLASS_B:
            {
                if (currentClass != CLASS_A)
                {
                  Errorstatus = LORA_ERROR;
                }
                else
                {
                    WakeUpState = DEVICE_STATE_REQ_DEVICE_TIME;
                    LORA_SetDeviceState(DEVICE_STATE_REQ_DEVICE_TIME);
                }
                break;
            }
            case CLASS_C:
            {
                if (currentClass != CLASS_A)
                {
                  Errorstatus = LORA_ERROR;
                }
                /*switch is instantanuous*/
                mibReq.Param.Class = CLASS_C;
                if( LoRaMacMibSetRequestConfirm( &mibReq ) == LORAMAC_STATUS_OK )
                {

                }
                else
                {
                    Errorstatus = LORA_ERROR;
                }

                //make a send to trigger classC Rx2 work
                NextTx = true;
                LORA_SetDeviceState(DEVICE_STATE_SEND);

                break;
            }
            default:
                break;
        }
    }
    return Errorstatus;
}

static void LORA_RxData(Mcps_t type, uint8_t* Buff, uint8_t BuffSize,uint8_t Port)
{
    int ret;

  /* USER CODE BEGIN 4 */
    if((true == g_lora_class_switch_succes_flag)) sLoRa_Status = LoRa_Idle;

    PRINTF("LoRa: Received Data, type=%d port=%d,len=%d,first data=0x%02x 0x%02x 0x%02x 0x%02x\r\n",
            type,
            Port,
            BuffSize,
            Buff[0],
            Buff[1],
            Buff[2],
            Buff[3]);

      g_num_send_data_failed_to_rejoin = 0;
      g_num_received_data_ack++;
      //1,parse Application's package protocol.

      //2,save the Eink data to Eink buffer
      switch (Port) {
          case LORAWAN_APP_PORT: {
              if (g_lora_esl_connection_ops_cb->data_arrived) {
                  if (type == MCPS_MULTICAST) {
                      g_lora_esl_connection_ops_cb->data_arrived(ESL_MULTICAST_DATA, Port, Buff, BuffSize, g_lora_cm_data);
                  } else {
                      g_lora_esl_connection_ops_cb->data_arrived(ESL_UNICAST_DATA, Port, Buff, BuffSize, g_lora_cm_data);
                  }
              }
              break;
          }
          case LORA_MULTICAST_CMD_PORT: {
              PRINTF("Dynamic multicast commands\r\n");
              lorawan_mc_process_cmd(Buff, BuffSize);
              lorawan_mc_cmd_ans();
              break;
         }
          default: {
              rx_cb_t app_rx_cb = NULL;
              ret = lorawan_module_get_app_rx_cb(Port, &app_rx_cb);
              if (ret != 0) {
                  PRINTF("LoRa: lorawan_module_get_app_rx_cb ret=%d\r\n", ret);
                  return;
              }

              if (app_rx_cb == NULL) {
                  PRINTF("LoRa: Port = %d not support!\r\n",Port);
              } else {
                  app_rx_cb(Buff, BuffSize);
              }
              break;
          }
      }
}

static int LORA_SetDeviceState ( DeviceState_t state) {
    LoraErrorStatus Errorstatus = LORA_SUCCESS;

    if (state == DEVICE_STATE_SEND) {
        if (sLoRa_Status == LoRa_Class_Switching || sLoRa_Status == LoRa_Joining)
        {
            Errorstatus = LORA_ERROR;
        }
        else
        {
            DeviceState = state;
        }
    } else if (state == DEVICE_STATE_REJOIN ) {
        if (sLoRa_Status == LoRa_Joining)
        {
            Errorstatus = LORA_ERROR;
        }
        else
        {
            DeviceState = state;
        }
    } else DeviceState = state;

    if(Errorstatus == LORA_ERROR)
    {
        PRINTF("\r\nLoRa: LORA_SetDeviceState() Error, target state=%d, current state=%d.\n\r",state,sLoRa_Status);
    }

    return Errorstatus;
}

int lorawan_init(esl_connection_ops_cb_t* _esl_cm_ops_cb, void* data)
{
    g_lora_esl_connection_ops_cb = _esl_cm_ops_cb;
    g_lora_cm_data = data;
    //assert_param(g_lora_esl_connection_ops_cb);

    #if LORA_MODULE_GET_FLASH_EUI_ENABLE
    device_status_get_devEUI(DevEui, sizeof(DevEui));
    device_status_get_appEUI(JoinEui, sizeof(JoinEui));
    device_status_get_appKey(AppKey, sizeof(AppKey));
    device_status_get_appKey(NwkKey, sizeof(NwkKey));

    PRINTF( "=======================================================\r\n" );
    PRINTF( "LoRa:get DevEui= %02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X\r\n", HEX8(DevEui));
    PRINTF( "LoRa:get AppEui= %02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X\r\n", HEX8(JoinEui));
    //PRINTF( "LoRa:get AppKey= %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\r\n", HEX16(AppKey));
    PRINTF( "=======================================================\r\n" );
    #endif

    LORA_ResetParameters();

    /* Configure and Init the LoRaWAN Stack*/
    LoRaMacPrimitives.MacMcpsConfirm = McpsConfirm;
    LoRaMacPrimitives.MacMcpsIndication = McpsIndication;
    LoRaMacPrimitives.MacMlmeConfirm = MlmeConfirm;
    LoRaMacPrimitives.MacMlmeIndication = MlmeIndication;
    LoRaMacCallbacks.GetBatteryLevel = NULL;
    LoRaMacCallbacks.GetTemperatureLevel = NULL;
    LoRaMacCallbacks.NvmContextChange = NULL;

    LoRaMacInitialization( &LoRaMacPrimitives, &LoRaMacCallbacks, ACTIVE_REGION );

    DeviceState = DEVICE_STATE_RESTORE;
    WakeUpState = DEVICE_STATE_START;
    g_random_joindelay = randr( 0, 2*APP_TX_DUTYCYCLE_RND );

    TimerInit( &JoinBackoffTimer, OnJoinBackoffTimerEvent );
    TimerInit( &ClassSwitchBackoffTimer, OnStartNextClassSwitchTimerEvent);

    TimerInit( &ClassSwitchRespTimeoutTimer, OnClassSwitchRespTimeoutTimerEvent);
    TimerSetValue( &ClassSwitchRespTimeoutTimer,TIMER_SWITCH_CLASSB_RESULT_CHECK);

    TimerInit(&RadioStatusTimer, OnRadioStatusEvent);
    TimerSetValue(&RadioStatusTimer, TIMER_RADIO_STATUS_CHECK);

    sLoRa_Status = LoRa_Init_Done;

    lorawan_mc_setup();

    PRINTF("LoRa:LoRa_Init_Done,LoRaWAN Protocol MAC VERSION: %lX\r\n", VERSION);

    return 0;
}

int lorawan_deinit(void)
{
    g_lora_esl_connection_ops_cb = NULL;
    g_lora_cm_data = NULL;
    sLoRa_Status = LoRa_Initing;

    return 0;
}

int lorawan_module_send(int is_confirm, int app_port, const char *data, int len, handle_send_cb_fp_t cb)
{
    if ( lorawan_module_get_join_status () != 1)
    {
        /*Not joined, try again later*/
        JoinNetwork();
        return -1;
    }

    if(len>LORAWAN_APP_DATA_MAX_SIZE) return -1;

    AppPort = app_port;
    AppDataSize = len;
    memcpy1(AppDataBuffer, (uint8_t *)data, len);
    IsTxConfirmed = is_confirm;

    if(false == SendFrame())
    {
        g_num_send_heartbeat++;
        return 0;
    }
    else
    {
        return -1;
    }
}

int lorawan_module_join(void) {

    PRINTF("Start lorawan_module_join\r\n");
    JoinNetwork();

    return 0;
}

LoRa_Status lorawan_module_switch_class(DeviceClass_t newClass)
{
    int switchStatus;
    if ((sLoRa_Status != LoRa_Join_Success)&&(sLoRa_Status != LoRa_Idle))
    {
        PRINTF("LoRa:failed to start lorawan_module_switch_class(),because sLoRa_Status=%d.\r\n",sLoRa_Status);
        return sLoRa_Status;
    }

    PRINTF("LoRa:lorawan_module_switch_class(),TargetClassType=%d,CurrentClass=%d,sLoRa_Status=%d.\r\n",newClass,g_current_lora_class_type,sLoRa_Status);

    if ((g_current_lora_class_type != newClass))
    {
        if(newClass == CLASS_B)
        {
            switchStatus = LORA_SwitchClass(newClass);

            if(switchStatus == LORA_SUCCESS)
            {
                TimerStart( &ClassSwitchRespTimeoutTimer);
                sLoRa_Status = LoRa_Class_Switching;
                g_lora_class_switch_succes_flag = false;
                g_current_target_lora_class_type = newClass;

                PRINTF("LoRa switching to ClassB from ClassA,waiting 2 minutes...\r\n");
                return LoRa_Class_Switching;
            }
        }
        else if ((newClass == CLASS_C)||(newClass == CLASS_A))
        {
            switchStatus = LORA_SwitchClass(newClass);

            sLoRa_Status = LoRa_Idle;
            g_lora_class_switch_succes_flag = true;

            PRINTF("LoRa switching to ClassC or ClassA, switch finished,switchStatus=%d.\r\n",switchStatus);

            if(switchStatus == LORA_SUCCESS)
            {
                g_current_target_lora_class_type = newClass;
                g_current_lora_class_type = newClass;
                return LoRa_Class_SwitchOK;
            }
        }
    }

    return LoRa_Class_SwitchNOK;
}

int lorawan_module_get_join_status(void)
{
    MibRequestConfirm_t mibReq;

    mibReq.Type = MIB_NETWORK_ACTIVATION;
    LoRaMacMibGetRequestConfirm( &mibReq );

    if( mibReq.Param.NetworkActivation == ACTIVATION_TYPE_NONE )
    {
        /*Not joined, try again later*/
        return 0;
    }
    else
    {
        return 1;
    }
}

LoRa_Status lorawan_module_get_status(void)
{
    return sLoRa_Status;
}

int lorawan_module_get_rssi_snr(char* rssi, char* snr)
{
    if((g_lora_rx_rssi ==0)&&(g_lora_rx_snr ==0))
    {
        return -1;
    }
    *rssi = g_lora_rx_rssi;
    *snr =  g_lora_rx_snr;

    return 0;
}

int lorawan_module_get_num_tx_rx(uint16_t* tx_num, uint16_t* rx_num)
{
    *tx_num = g_num_send_heartbeat;
    *rx_num = g_num_received_data_ack;

    return 0;
}

int lorawan_module_reg_app_rx_cb(uint8_t port, rx_cb_t rx_cb)
{
    int i;

    if (rx_cb == NULL) {
        PRINTF("LoRa: app_rx_cb is NULL!\r\n");
        return -1;
    }

    if (port == 0) {
        PRINTF("LoRa: port is 0!\r\n");
        return -1;
    }

    for (i = 0; i < MAX_APP_RX_CB_LIST; i++) {
        if (g_app_rx_cb_list[i].port == 0) {
            g_app_rx_cb_list[i].port = port;
            g_app_rx_cb_list[i].rx_cb = rx_cb;
            break;
        }
    }

    if (i == MAX_APP_RX_CB_LIST) {
        PRINTF("LoRa: no enough cb list space left!\r\n");
        return -1;
    }

    return 0;
}

static int lorawan_module_get_app_rx_cb(uint8_t port, rx_cb_t *rx_cb)
{
    int i;

    for (i = 0; i < MAX_APP_RX_CB_LIST; i++) {
        if (g_app_rx_cb_list[i].port == port) {
            *rx_cb = g_app_rx_cb_list[i].rx_cb;
            return 0;
        }
    }

    PRINTF("LoRa: no app rx cb found!\r\n");
    return -1;
}

int lorawan_mc_setup(void)
{
    uint8_t mcID;

    lorawan_mc_init(LORA_MULTICAST_CMD_PORT, mcGenAppKey);
    //mcID = 1;
    //lorawan_mc_set_dynamic_channel(mcID);
    mcID = 0;
    lorawan_mc_set_preset_channel(mcID, &g_mc_channels[mcID], &g_mc_keys[mcID]);
    mcID = 1;
    lorawan_mc_set_preset_channel(mcID, &g_mc_channels[mcID], &g_mc_keys[mcID]);
    mcID = 2;
    lorawan_mc_set_preset_channel(mcID, &g_mc_channels[mcID], &g_mc_keys[mcID]);
    mcID = 3;
    lorawan_mc_set_preset_channel(mcID, &g_mc_channels[mcID], &g_mc_keys[mcID]);
    return 0;
}

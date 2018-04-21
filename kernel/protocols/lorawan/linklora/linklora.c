/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "linklora.h"
#include "commissioning.h"
#include "utilities.h"
#include "LoRaMac.h"
#include "Region.h"
#include "timeServer.h"
#ifdef AOS_KV
#include <assert.h>
#include "kvmgr.h"
#endif

static uint8_t dev_eui[8] = LORAWAN_DEVICE_EUI;
static uint8_t app_eui[8] = LORAWAN_APPLICATION_EUI;
static uint8_t app_key[16] = LORAWAN_APPLICATION_KEY;

#define LORAWAN_APP_DATA_BUFF_SIZE 242

static uint8_t app_data_buff[LORAWAN_APP_DATA_BUFF_SIZE];
static lora_AppData_t app_data = { app_data_buff, 0, 0 };

static LoRaMacPrimitives_t LoRaMacPrimitives;
static LoRaMacCallback_t LoRaMacCallbacks;
static LoRaMainCallback_t *app_callbacks;
static MibRequestConfirm_t mibReq;

static int8_t is_tx_confirmed;
static bool next_tx = true;

static LoRaParam_t *LoRaParamInit;

static TimerEvent_t TxNextPacketTimer;
static DeviceState_t device_state = DEVICE_STATE_INIT;

lora_config_t g_lora_config = {CLASS_A, FREQ_TYPE_INTRA, 1, DR_5, NODE_MODE_NORMAL, INVALID_LORA_CONFIG};
join_method_t g_join_method;

static void prepare_tx_frame(void)
{
    app_callbacks->LoraTxData(&app_data, &is_tx_confirmed);
}

static bool send_frame(void)
{
    McpsReq_t mcpsReq;
    LoRaMacTxInfo_t txInfo;

    if (LoRaMacQueryTxPossible(app_data.BuffSize, &txInfo) != LORAMAC_STATUS_OK) {
        // Send empty frame in order to flush MAC commands
        mcpsReq.Type = MCPS_UNCONFIRMED;
        mcpsReq.Req.Unconfirmed.fBuffer = NULL;
        mcpsReq.Req.Unconfirmed.fBufferSize = 0;
        mcpsReq.Req.Unconfirmed.Datarate = LoRaParamInit->TxDatarate;
    } else {
        if (is_tx_confirmed == DISABLE) {
            mcpsReq.Type = MCPS_UNCONFIRMED;
            mcpsReq.Req.Unconfirmed.fPort = app_data.Port;
            mcpsReq.Req.Unconfirmed.fBuffer = app_data.Buff;
            mcpsReq.Req.Unconfirmed.fBufferSize = app_data.BuffSize;
            mcpsReq.Req.Unconfirmed.Datarate = LoRaParamInit->TxDatarate;
        } else {
            mcpsReq.Type = MCPS_CONFIRMED;
            mcpsReq.Req.Confirmed.fPort = app_data.Port;
            mcpsReq.Req.Confirmed.fBuffer = app_data.Buff;
            mcpsReq.Req.Confirmed.fBufferSize = app_data.BuffSize;
            mcpsReq.Req.Confirmed.NbTrials = 8;
            mcpsReq.Req.Confirmed.Datarate = LoRaParamInit->TxDatarate;
        }
    }

    if (LoRaMacMcpsRequest(&mcpsReq) == LORAMAC_STATUS_OK) {
        return false;
    }
    return true;
}

static void on_tx_next_packet_timer_event(void)
{
    MibRequestConfirm_t mibReq;
    LoRaMacStatus_t status;

    TimerStop(&TxNextPacketTimer);

    mibReq.Type = MIB_NETWORK_JOINED;
    status = LoRaMacMibGetRequestConfirm(&mibReq);

    if (status == LORAMAC_STATUS_OK) {
        if (mibReq.Param.IsNetworkJoined == true) {
            device_state = DEVICE_STATE_SEND;
            next_tx = true;
        }
    }
}

static void reset_join_state(void)
{
    if (g_lora_config.flag == VALID_LORA_CONFIG) {
        g_lora_config.flag = INVALID_LORA_CONFIG;
#ifdef AOS_KV
        aos_kv_set("lora", &g_lora_config, sizeof(g_lora_config));
#endif
    }
    device_state = DEVICE_STATE_JOIN;
}

static void store_lora_config(void)
{
    MibRequestConfirm_t mibReq;
    LoRaMacStatus_t status;
    uint32_t freqband;
    int8_t datarate;

    mibReq.Type = MIB_FREQ_BAND;
    status = LoRaMacMibGetRequestConfirm(&mibReq);
    if (status == LORAMAC_STATUS_OK) {
        freqband = mibReq.Param.freqband;
    } else {
        return;
    }

    mibReq.Type = MIB_CHANNELS_DATARATE;
    status = LoRaMacMibGetRequestConfirm(&mibReq);
    if (status == LORAMAC_STATUS_OK) {
        datarate = mibReq.Param.ChannelsDatarate;
    } else {
        return;
    }

    g_lora_config.freqband = freqband;
    g_lora_config.datarate = datarate;
    g_lora_config.flag = VALID_LORA_CONFIG;
#ifdef AOS_KV
    aos_kv_set("lora", &g_lora_config, sizeof(g_lora_config));
#endif
}

static void mcps_confirm(McpsConfirm_t *mcpsConfirm)
{
    if (mcpsConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK) {
        switch (mcpsConfirm->McpsRequest) {
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
    else
    {
        switch( mcpsConfirm->McpsRequest )
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

                reset_join_state();
                g_join_method = STORED_JOIN_METHOD;
                DBG_LINKLORA("Not receive Ack,Start to Join...\r\n");
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
    next_tx = true;
}

static void McpsIndication(McpsIndication_t *mcpsIndication)
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
            DBG_LINKLORA( "MCPS_MULTICAST\n" );
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
    DBG_LINKLORA( "rssi = %d, snr = %d, datarate = %d\r\n", mcpsIndication->Rssi, mcpsIndication->Snr,
                 mcpsIndication->RxDatarate);

    if ( mcpsIndication->RxData == true )
    {
        switch ( mcpsIndication->Port )
        {
            case 224:
                break;
            default:
                app_data.Port = mcpsIndication->Port;
                app_data.BuffSize = mcpsIndication->BufferSize;
                memcpy1(app_data.Buff, mcpsIndication->Buffer, app_data.BuffSize);
                app_callbacks->LoraRxData(&app_data);
                break;
        }
    }
}

static void MlmeConfirm( MlmeConfirm_t *mlmeConfirm )
{
    switch ( mlmeConfirm->MlmeRequest )
    {
        case MLME_JOIN:
        {
            if (mlmeConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK) {
                // Status is OK, node has joined the network
                device_state = DEVICE_STATE_JOINED;
            } else {
                // Join was not successful. Try to join again
                reset_join_state();
                if (g_join_method != SCAN_JOIN_METHOD) {
                    g_join_method = (g_join_method + 1) % JOIN_METHOD_NUM;
                }
                DBG_LINKLORA("Rejoin\r\n");
            }
            break;
        }
        case MLME_LINK_CHECK:
            {
            if ( mlmeConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK )
            {
                // Check DemodMargin
                // Check NbGateways
            }
            break;
        }
        default:
            break;
    }
    next_tx = true;
}

static char *get_class_name(int8_t class)
{
    if (g_lora_config.class == CLASS_B) {
        return "class_b";
    } else if (g_lora_config.class == CLASS_C) {
        return "class_c";
    } else {
        g_lora_config.class = CLASS_A;
        return "class_a";
    }
}

void lora_init(LoRaMainCallback_t *callbacks, LoRaParam_t *LoRaParam)
{
    device_state = DEVICE_STATE_INIT;
    LoRaParamInit = LoRaParam;
    app_callbacks = callbacks;

#ifdef AOS_KV
    assert(aos_kv_init() == 0);
#endif

#if (STATIC_DEVICE_EUI != 1)
    app_callbacks->BoardGetUniqueId(dev_eui);
#endif

    DBG_LINKLORA("class type %s\r\n", get_class_name(g_lora_config.class));
    DBG_LINKLORA("freq type %s\r\n", g_lora_config.freqtype == FREQ_TYPE_INTER? "inter": "intra");

#if (OVER_THE_AIR_ACTIVATION != 0)
    DBG_LINKLORA("OTAA\r\n" );
    DBG_LINKLORA("DevEui= %02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X\r\n",
                 dev_eui[0], dev_eui[1], dev_eui[2], dev_eui[3], dev_eui[4], dev_eui[5], dev_eui[6], dev_eui[7]);
    DBG_LINKLORA("AppEui= %02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X\r\n",
                 app_eui[0], app_eui[1], app_eui[2], app_eui[3], app_eui[4], app_eui[5], app_eui[6], app_eui[7]);
    DBG_LINKLORA("AppKey= %02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X\r\n",
                 app_key[0], app_key[1], app_key[2], app_key[3], app_key[4], app_key[5], app_key[6], app_key[7],
                 app_key[8], app_key[9], app_key[10], app_key[11], app_key[12], app_key[13], app_key[14], app_key[15]);
#else
    DBG_LINKLORA("ABP\r\n");
    DBG_LINKLORA("DevAdd=  %08X\n\r", DevAddr);
    DBG_LINKLORA("DevEui= %02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X\r\n",
                 dev_eui[0], dev_eui[1], dev_eui[2], dev_eui[3], dev_eui[4], dev_eui[5], dev_eui[6], dev_eui[7]);
    DBG_LINKLORA("NwkSKey= %02X", NwkSKey[0]);
    for (int i = 1; i < 16; i++)
    {
        DBG_LINKLORA(" %02X", NwkSKey[i]);
    };
    DBG_LINKLORA("\r\n");
    DBG_LINKLORA("AppSKey= %02X", AppSKey[0]);
    for (int i = 1; i < 16; i++)
    {
        DBG_LINKLORA(" %02X", AppSKey[i]);
    };
    DBG_LINKLORA("\r\n");
#endif
}

void lora_fsm( void )
{
#ifdef CONFIG_LINKLORA
    int len = sizeof(g_lora_config);
    int ret;
    lora_config_t lora_config;
#endif

    while (1) {
        switch (device_state) {
            case DEVICE_STATE_INIT:
            {
                LoRaMacPrimitives.MacMcpsConfirm = mcps_confirm;
                LoRaMacPrimitives.MacMcpsIndication = McpsIndication;
                LoRaMacPrimitives.MacMlmeConfirm = MlmeConfirm;
                LoRaMacCallbacks.GetBatteryLevel = app_callbacks->BoardGetBatteryLevel;
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
#elif defined( REGION_CN470S )
                LoRaMacInitialization( &LoRaMacPrimitives, &LoRaMacCallbacks, LORAMAC_REGION_CN470S);
#else
#error "Please define a region in the compiler options."
#endif

                TimerInit( &TxNextPacketTimer, on_tx_next_packet_timer_event );

#ifdef AOS_KV
                memset(&lora_config, 0, sizeof(lora_config));
                aos_kv_get("lora", &lora_config, &len);
                if (lora_config.flag == VALID_LORA_CONFIG) {
                    memcpy(&g_lora_config, &lora_config, sizeof(g_lora_config));
                }
#endif
                if (g_lora_config.flag == VALID_LORA_CONFIG) {
                    g_join_method = STORED_JOIN_METHOD;
                } else {
                    g_join_method = DEF_JOIN_METHOD;
                }

                mibReq.Type = MIB_ADR;
                mibReq.Param.AdrEnable = LoRaParamInit->AdrEnable;
                LoRaMacMibSetRequestConfirm( &mibReq );

                mibReq.Type = MIB_PUBLIC_NETWORK;
                mibReq.Param.EnablePublicNetwork = LoRaParamInit->EnablePublicNetwork;
                LoRaMacMibSetRequestConfirm( &mibReq );

                mibReq.Type = MIB_DEVICE_CLASS;
                mibReq.Param.Class = g_lora_config.class;
                LoRaMacMibSetRequestConfirm(&mibReq);

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
                device_state = DEVICE_STATE_JOIN;
                break;
            }

            case DEVICE_STATE_JOIN:
            {
#if (OVER_THE_AIR_ACTIVATION != 0)
                MlmeReq_t mlmeReq;

                mlmeReq.Type = MLME_JOIN;
                mlmeReq.Req.Join.DevEui = dev_eui;
                mlmeReq.Req.Join.AppEui = app_eui;
                mlmeReq.Req.Join.AppKey = app_key;

                mlmeReq.Req.Join.method = g_join_method;
                if (g_join_method == STORED_JOIN_METHOD) {
                    mlmeReq.Req.Join.freqband = g_lora_config.freqband;
                    mlmeReq.Req.Join.NbTrials = 3;
                    mlmeReq.Req.Join.datarate = g_lora_config.datarate;
                } else {
                    mlmeReq.Req.Join.NbTrials = 2;
                }

                if (next_tx == true) {
                    if (LoRaMacMlmeRequest(&mlmeReq) == LORAMAC_STATUS_OK) {
                        next_tx = false;
                    }
                    DBG_LINKLORA("Start to Join, method %d, nb_trials:%d\r\n",
                                 g_join_method, mlmeReq.Req.Join.NbTrials);
                }
                device_state = DEVICE_STATE_SLEEP;
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

                device_state = DEVICE_STATE_SEND;
#endif
                break;
            }
            case DEVICE_STATE_JOINED:
            {
                DBG_LINKLORA("Joined\n\r");
                store_lora_config();
                device_state = DEVICE_STATE_SEND;
                break;
            }
            case DEVICE_STATE_SEND:
                {
                if ( next_tx == true )
                {
                    prepare_tx_frame();
                    next_tx = send_frame( );
                }
                if ( LoRaParamInit->TxEvent == TX_ON_TIMER )
                {
                    // Schedule next packet transmission
                    TimerSetValue( &TxNextPacketTimer, LoRaParamInit->TxDutyCycleTime );
                    TimerStart( &TxNextPacketTimer );
                }
                device_state = DEVICE_STATE_SLEEP;
                break;
            }
            case DEVICE_STATE_SLEEP:
            {
                // Wake up through events
#ifndef LOW_POWER_DISABLE
                LowPower_Handler( );
#endif
                break;
            }
            default:
            {
                device_state = DEVICE_STATE_INIT;
                break;
            }
        }
    }
}

DeviceState_t lora_getDeviceState( void )
{
    return device_state;
}

lora_config_t *get_lora_config(void)
{
    return &g_lora_config;
}

/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "linkwan.h"
#include "commissioning.h"
#include "utilities.h"
#include "LoRaMac.h"
#include "Region.h"
#include "timeServer.h"
#include "radio.h"
#ifdef AOS_KV
#include <assert.h>
#include "kvmgr.h"
#endif

static uint8_t app_data_buff[LORAWAN_APP_DATA_BUFF_SIZE];
static lora_AppData_t app_data = {app_data_buff, 0, 0};

static LoRaMacPrimitives_t LoRaMacPrimitives;
static LoRaMacCallback_t LoRaMacCallbacks;
static LoRaMainCallback_t *app_callbacks;
static MibRequestConfirm_t mibReq;

static int8_t is_tx_confirmed = ENABLE;
static bool next_tx = true;
static uint8_t num_trials = 8;
static bool rejoin_flag = true;

static LoRaParam_t lora_param = {
    TX_ON_TIMER,
    APP_TX_DUTYCYCLE,
    LORAWAN_ADR_ON,
    DR_0,
    LORAWAN_PUBLIC_NETWORK,
    JOINREQ_NBTRIALS
};

static TimerEvent_t TxNextPacketTimer;
volatile static DeviceState_t device_state = DEVICE_STATE_INIT;

lora_config_t g_lora_config = {1, DR_5, INVALID_LORA_CONFIG};
lora_dev_t g_lora_dev = {LORAWAN_DEVICE_EUI, LORAWAN_APPLICATION_EUI, LORAWAN_APPLICATION_KEY, CLASS_A, NODE_MODE_NORMAL, VALID_LORA_CONFIG};
node_freq_type_t g_freq_type = FREQ_TYPE_INTRA;
join_method_t g_join_method;

static void prepare_tx_frame(void)
{
    app_callbacks->LoraTxData(&app_data);
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
        mcpsReq.Req.Unconfirmed.Datarate = lora_param.TxDatarate;
    } else {
        if (is_tx_confirmed == DISABLE) {
            mcpsReq.Type = MCPS_UNCONFIRMED;
            mcpsReq.Req.Unconfirmed.fPort = app_data.Port;
            mcpsReq.Req.Unconfirmed.fBuffer = app_data.Buff;
            mcpsReq.Req.Unconfirmed.fBufferSize = app_data.BuffSize;
            mcpsReq.Req.Unconfirmed.Datarate = lora_param.TxDatarate;
        } else {
            mcpsReq.Type = MCPS_CONFIRMED;
            mcpsReq.Req.Confirmed.fPort = app_data.Port;
            mcpsReq.Req.Confirmed.fBuffer = app_data.Buff;
            mcpsReq.Req.Confirmed.fBufferSize = app_data.BuffSize;
            mcpsReq.Req.Confirmed.NbTrials = num_trials;
            mcpsReq.Req.Confirmed.Datarate = lora_param.TxDatarate;
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
        } else {
            rejoin_flag = true;
            device_state = DEVICE_STATE_JOIN;
        }
        next_tx = true;
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
                g_join_method = DEF_JOIN_METHOD;
                DBG_LINKWAN("Not receive Ack,Start to Join...\r\n");
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
            DBG_LINKWAN( "MCPS_MULTICAST\n" );
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
    DBG_LINKWAN( "rssi = %d, snr = %d, datarate = %d\r\n", mcpsIndication->Rssi, mcpsIndication->Snr,
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

static uint32_t generate_rejoin_delay(void)
{
    uint32_t rejoin_delay = 0;

    while (rejoin_delay < 8000) {
        rejoin_delay += (Radio.Random() % 250);
    }

    return rejoin_delay;
}

static void MlmeConfirm( MlmeConfirm_t *mlmeConfirm )
{
    uint32_t rejoin_delay;

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
                    rejoin_delay = generate_rejoin_delay();
                } else {
                    g_join_method = DEF_JOIN_METHOD;
                    rejoin_delay = 60 * 60 * 1000;  // 1 hour
                }
                TimerSetValue(&TxNextPacketTimer, rejoin_delay);
                TimerStart(&TxNextPacketTimer);
                rejoin_flag = false;
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
    if (g_lora_dev.class == CLASS_B) {
        return "class_b";
    } else if (g_lora_dev.class == CLASS_C) {
        return "class_c";
    } else {
        g_lora_dev.class = CLASS_A;
        return "class_a";
    }
}

void lora_init(LoRaMainCallback_t *callbacks)
{
    device_state = DEVICE_STATE_INIT;
    app_callbacks = callbacks;

#ifdef AOS_KV
    assert(aos_kv_init() == 0);
#endif

}

static void print_dev_addr(void)
{
#if (OVER_THE_AIR_ACTIVATION != 0)
    DBG_LINKWAN("OTAA\r\n" );
    DBG_LINKWAN("DevEui= %02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X\r\n",
                 g_lora_dev.dev_eui[0], g_lora_dev.dev_eui[1], g_lora_dev.dev_eui[2], g_lora_dev.dev_eui[3], \
                 g_lora_dev.dev_eui[4], g_lora_dev.dev_eui[5], g_lora_dev.dev_eui[6], g_lora_dev.dev_eui[7]);
    DBG_LINKWAN("AppEui= %02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X\r\n",
                 g_lora_dev.app_eui[0], g_lora_dev.app_eui[1], g_lora_dev.app_eui[2], g_lora_dev.app_eui[3], \
                 g_lora_dev.app_eui[4], g_lora_dev.app_eui[5], g_lora_dev.app_eui[6], g_lora_dev.app_eui[7]);
    DBG_LINKWAN("AppKey= %02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X\r\n",
                 g_lora_dev.app_key[0], g_lora_dev.app_key[1], g_lora_dev.app_key[2], g_lora_dev.app_key[3], \
                 g_lora_dev.app_key[4], g_lora_dev.app_key[5], g_lora_dev.app_key[6], g_lora_dev.app_key[7], \
                 g_lora_dev.app_key[8], g_lora_dev.app_key[9], g_lora_dev.app_key[10], g_lora_dev.app_key[11], \
                 g_lora_dev.app_key[12], g_lora_dev.app_key[13], g_lora_dev.app_key[14], g_lora_dev.app_key[15]);
#else
    DBG_LINKWAN("ABP\r\n");
    DBG_LINKWAN("DevAdd=  %08X\n\r", DevAddr);
    DBG_LINKWAN("DevEui= %02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X\r\n",
                 g_lora_dev.dev_eui[0], g_lora_dev.dev_eui[1], g_lora_dev.dev_eui[2], g_lora_dev.dev_eui[3], \
                 g_lora_dev.dev_eui[4], g_lora_dev.dev_eui[5], g_lora_dev.dev_eui[6], g_lora_dev.dev_eui[7]);
    DBG_LINKWAN("NwkSKey= %02X", NwkSKey[0]);
    for (int i = 1; i < 16; i++)
    {
        DBG_LINKWAN(" %02X", NwkSKey[i]);
    };
    DBG_LINKWAN("\r\n");
    DBG_LINKWAN("AppSKey= %02X", AppSKey[0]);
    for (int i = 1; i < 16; i++)
    {
        DBG_LINKWAN(" %02X", AppSKey[i]);
    };
    DBG_LINKWAN("\r\n");
#endif

    DBG_LINKWAN("class type %s\r\n", get_class_name(g_lora_dev.class));
    DBG_LINKWAN("freq type %s\r\n", g_freq_type == FREQ_TYPE_INTER? "inter": "intra");
}

void lora_fsm( void )
{
#ifdef CONFIG_LINKWAN
    int len = sizeof(g_lora_config);
    int ret;
    lora_config_t lora_config;
    lora_dev_t lora_dev;
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
#elif defined( REGION_CN470A )
                LoRaMacInitialization( &LoRaMacPrimitives, &LoRaMacCallbacks, LORAMAC_REGION_CN470A);
#else
#error "Please define a region in the compiler options."
#endif
                TimerInit( &TxNextPacketTimer, on_tx_next_packet_timer_event );
#ifdef AOS_KV
                memset(&lora_config, 0, sizeof(lora_config));
                len = sizeof(g_lora_config);
                aos_kv_get("lora", &lora_config, &len);
                if (lora_config.flag == VALID_LORA_CONFIG) {
                    memcpy(&g_lora_config, &lora_config, sizeof(g_lora_config));
                }
                memset(&lora_dev, 0, sizeof(lora_dev));
                len = sizeof(g_lora_dev);
                aos_kv_get("lora_dev", &lora_dev, &len);
                if (lora_dev.flag == VALID_LORA_CONFIG) {
                    memcpy(&g_lora_dev, &lora_dev, sizeof(g_lora_dev));
                }
#endif
                if (g_lora_dev.dev_eui[5] & 0x1) {
                    g_freq_type = FREQ_TYPE_INTER;
                }
                print_dev_addr();

                if (g_lora_config.flag == VALID_LORA_CONFIG) {
                    g_join_method = STORED_JOIN_METHOD;
                } else {
                    g_join_method = DEF_JOIN_METHOD;
                }

                mibReq.Type = MIB_ADR;
                mibReq.Param.AdrEnable = lora_param.AdrEnable;
                LoRaMacMibSetRequestConfirm(&mibReq);

                mibReq.Type = MIB_PUBLIC_NETWORK;
                mibReq.Param.EnablePublicNetwork = lora_param.EnablePublicNetwork;
                LoRaMacMibSetRequestConfirm(&mibReq);

                mibReq.Type = MIB_DEVICE_CLASS;
                mibReq.Param.Class = g_lora_dev.class;
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
                mlmeReq.Req.Join.DevEui = g_lora_dev.dev_eui;
                mlmeReq.Req.Join.AppEui = g_lora_dev.app_eui;
                mlmeReq.Req.Join.AppKey = g_lora_dev.app_key;

                mlmeReq.Req.Join.method = g_join_method;
                if (g_join_method == STORED_JOIN_METHOD) {
                    mlmeReq.Req.Join.freqband = g_lora_config.freqband;
                    mlmeReq.Req.Join.NbTrials = 3;
                    mlmeReq.Req.Join.datarate = g_lora_config.datarate;
                } else {
                    mlmeReq.Req.Join.NbTrials = 2;
                }

                if (next_tx == true && rejoin_flag == true) {
                    if (LoRaMacMlmeRequest(&mlmeReq) == LORAMAC_STATUS_OK) {
                        next_tx = false;
                    }
                    DBG_LINKWAN("Start to Join, method %d, nb_trials:%d\r\n",
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
                DBG_LINKWAN("Joined\n\r");
                store_lora_config();
                device_state = DEVICE_STATE_SEND;
                break;
            }
            case DEVICE_STATE_SEND:
            {
                if (next_tx == true) {
                    prepare_tx_frame();
                    next_tx = send_frame();
                }
                if ( lora_param.TxEvent == TX_ON_TIMER )
                {
                    // Schedule next packet transmission
                    TimerSetValue(&TxNextPacketTimer, lora_param.TxDutyCycleTime);
                    TimerStart(&TxNextPacketTimer);
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

node_freq_type_t get_lora_freq_type(void)
{
    return g_freq_type;
}

bool set_lora_tx_datarate(int8_t datarate)
{
    if (datarate >= DR_0 && datarate <= DR_5) {
        lora_param.TxDatarate = datarate;
        return true;
    } else {
        return false;
    }
}

bool set_lora_tx_dutycycle(uint32_t dutycycle)
{
    lora_param.TxDutyCycleTime = dutycycle;
    return true;
}

bool set_lora_tx_len(uint16_t len)
{
    if (len <= LORAWAN_APP_DATA_BUFF_SIZE) {
        app_data.BuffSize = len;
        return true;
    } else {
        return false;
    }
}

bool set_lora_tx_confirmed_flag(int confirmed)
{
    is_tx_confirmed = confirmed;
    return true;
}

bool set_lora_tx_num_trials(uint8_t trials)
{
    num_trials = trials;
    return true;
}

bool set_lora_state(DeviceState_t state)
{
    if (device_state == DEVICE_STATE_SLEEP) {
        TimerStop(&TxNextPacketTimer);
    }
    device_state = state;
    return true;
}

bool send_lora_link_check(void)
{
    MlmeReq_t mlmeReq;

    mlmeReq.Type = MLME_LINK_CHECK;
    if (next_tx == true) {
        if (LoRaMacMlmeRequest(&mlmeReq) == LORAMAC_STATUS_OK) {
            next_tx = false;
            return true;
        }
    }
    return false;
}

bool set_lora_class(int8_t class)
{
    if (class >= CLASS_A && class <= CLASS_C) {
        g_lora_dev.class = class;
        g_lora_dev.flag = VALID_LORA_CONFIG;
#ifdef AOS_KV
        aos_kv_set("lora", &g_lora_config, sizeof(g_lora_config));
#endif
        return true;
    }
    return false;
}

bool set_lora_dev_eui(uint8_t *eui)
{
    memcpy(g_lora_dev.dev_eui, eui, 8);
    g_lora_dev.flag = VALID_LORA_CONFIG;
#ifdef AOS_KV
    aos_kv_set("lora_dev", &g_lora_dev, sizeof(g_lora_dev));
#endif
    return true;
}

bool set_lora_app_eui(uint8_t *eui)
{
    memcpy(g_lora_dev.app_eui, eui, 8);
    g_lora_dev.flag = VALID_LORA_CONFIG;
#ifdef AOS_KV
    aos_kv_set("lora_dev", &g_lora_dev, sizeof(g_lora_dev));
#endif
    return true;
}

bool set_lora_app_key(uint8_t *key)
{
    memcpy(g_lora_dev.app_key, key, 16);
    g_lora_dev.flag = VALID_LORA_CONFIG;
#ifdef AOS_KV
    aos_kv_set("lora_dev", &g_lora_dev, sizeof(g_lora_dev));
#endif
    return true;
}

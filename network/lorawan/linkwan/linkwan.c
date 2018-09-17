/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "linkwan.h"
#include "commissioning.h"
#include "utilities.h"
#include "LoRaMac.h"
#include "Region.h"
#include "RegionCN470A.h"
#include "timeServer.h"
#include "radio.h"
#ifdef AOS_KV
#include <assert.h>
#include "kvmgr.h"
#endif
#include <k_config.h>
#include <k_types.h>
#include <k_err.h>
#include <k_sys.h>
#include <k_list.h>
#include <k_obj.h>
#include <k_task.h>
#include <port.h>

static uint8_t        tx_buf[LORAWAN_APP_DATA_BUFF_SIZE];
static lora_AppData_t tx_data = { tx_buf, 1, 0 };
static uint8_t        rx_buf[LORAWAN_APP_DATA_BUFF_SIZE];
static lora_AppData_t rx_data = { rx_buf, 0, 0 };
static uint8_t        tx_size = 1;

static LoRaMacPrimitives_t LoRaMacPrimitives;
static LoRaMacCallback_t   LoRaMacCallbacks;
static LoRaMainCallback_t *app_callbacks;
static MibRequestConfirm_t mibReq;

static int8_t  is_tx_confirmed = 1;
static bool    next_tx         = true;
static uint8_t num_trials      = 8;
static bool    rejoin_flag     = true;

static uint8_t g_freqband_num = 0;

static LoRaParam_t lora_param = {
    TX_ON_NONE,    0, true, DR_0, LORAWAN_PUBLIC_NETWORK, JOINREQ_NBTRIALS,
    JOIN_MODE_OTAA
};

static TimerEvent_t           TxNextPacketTimer;
volatile static DeviceState_t device_state = DEVICE_STATE_INIT;
lora_dev_t                    g_lora_dev   = { LORAWAN_DEVICE_EUI,
                          LORAWAN_APPLICATION_EUI,
                          LORAWAN_APPLICATION_KEY,
                          0,
                          DR_5,
                          CLASS_A,
                          NODE_MODE_NORMAL,
                          0xffff,
                          VALID_LORA_CONFIG,
                          0 };
lora_abp_id_t g_lora_abp_id = { LORAWAN_DEVICE_ADDRESS, LORAWAN_NWKSKEY,
                                LORAWAN_APPSKEY, INVALID_LORA_CONFIG };

node_freq_mode_t g_freq_mode = FREQ_MODE_INTRA;
join_method_t    g_join_method;

static uint8_t  gJoinState    = 0;
static uint8_t  gAutoJoin     = 1;
static uint16_t gJoinInterval = 8;


static void start_dutycycle_timer(void);
static bool send_frame(void)
{
    McpsReq_t       mcpsReq;
    LoRaMacTxInfo_t txInfo;

    if (tx_data.BuffSize > LINKWAN_APP_DATA_SIZE) {
        tx_data.BuffSize = LINKWAN_APP_DATA_SIZE;
    }

    if (LoRaMacQueryTxPossible(tx_data.BuffSize, &txInfo) !=
        LORAMAC_STATUS_OK) {
        return true;
    }

    if (is_tx_confirmed == 0) {
        mcpsReq.Type                        = MCPS_UNCONFIRMED;
        mcpsReq.Req.Unconfirmed.fPort       = tx_data.Port;
        mcpsReq.Req.Unconfirmed.fBuffer     = tx_data.Buff;
        mcpsReq.Req.Unconfirmed.fBufferSize = tx_data.BuffSize;
        mcpsReq.Req.Unconfirmed.Datarate    = lora_param.TxDatarate;
    } else {
        mcpsReq.Type                      = MCPS_CONFIRMED;
        mcpsReq.Req.Confirmed.fPort       = tx_data.Port;
        mcpsReq.Req.Confirmed.fBuffer     = tx_data.Buff;
        mcpsReq.Req.Confirmed.fBufferSize = tx_data.BuffSize;
        mcpsReq.Req.Confirmed.NbTrials    = num_trials;
        mcpsReq.Req.Confirmed.Datarate    = lora_param.TxDatarate;
    }

    if (LoRaMacMcpsRequest(&mcpsReq) == LORAMAC_STATUS_OK) {
        return false;
    }

    return true;
}

static void prepare_tx_frame(void)
{
    if (lora_param.TxEvent == TX_ON_TIMER) {
        app_callbacks->LoraTxData(&tx_data);
    }
}

static void on_tx_next_packet_timer_event(void)
{
    MibRequestConfirm_t mib_req;
    LoRaMacStatus_t     status;


    TimerStop(&TxNextPacketTimer);

    mib_req.Type = MIB_NETWORK_JOINED;
    status       = LoRaMacMibGetRequestConfirm(&mib_req);

    if (status == LORAMAC_STATUS_OK) {
        if (mib_req.Param.IsNetworkJoined == true) {
            device_state = DEVICE_STATE_SEND;
        } else {
            rejoin_flag  = true;
            device_state = DEVICE_STATE_JOIN;
        }
    }
}

static uint16_t crc16(uint8_t *buffer, uint8_t length)
{
    const uint16_t polynom = 0x1021;
    uint16_t       crc     = 0x0000;

    for (uint8_t i = 0; i < length; ++i) {
        crc ^= (uint16_t)buffer[i] << 8;
        for (uint8_t j = 0; j < 8; ++j) {
            crc = (crc & 0x8000) ? (crc << 1) ^ polynom : (crc << 1);
        }
    }

    return crc;
}

static void read_lora_dev(lora_dev_t *lora_dev)
{
    int      len;
    uint16_t crc;

    memset(lora_dev, 0, sizeof(lora_dev_t));

    len = sizeof(lora_dev_t);
#ifdef AOS_KV
    aos_kv_get("lora_dev", lora_dev, &len);

    crc = crc16((uint8_t *)lora_dev, len - 2);
    if (crc != lora_dev->crc) {
        lora_dev->freqband = -1;
        lora_dev->class    = CLASS_A;
        lora_dev->mode     = NODE_MODE_NORMAL;
        lora_dev->mask     = 0xffff;
    }
#else
    memcpy(lora_dev, &g_lora_dev, sizeof(lora_dev_t));
#endif
}

static void write_lora_dev(lora_dev_t *lora_dev)
{
    lora_dev->crc = crc16((uint8_t *)lora_dev, sizeof(lora_dev_t) - 2);
#ifdef AOS_KV
    aos_kv_set("lora_dev", lora_dev, sizeof(lora_dev_t));
#else
    memcpy(&g_lora_dev, lora_dev, sizeof(lora_dev_t));
#endif
}

static void reset_join_state(void)
{
    lora_dev_t lora_dev;

    read_lora_dev(&lora_dev);
    lora_dev.freqband = -1;
    write_lora_dev(&lora_dev);
    device_state = DEVICE_STATE_JOIN;
}

static void store_lora_config(void)
{
    MibRequestConfirm_t mib_req;
    LoRaMacStatus_t     status;
    uint32_t            freqband;
    int8_t              datarate;
    lora_dev_t          lora_dev;

    mib_req.Type = MIB_FREQ_BAND;
    status       = LoRaMacMibGetRequestConfirm(&mib_req);
    if (status == LORAMAC_STATUS_OK) {
        freqband = mib_req.Param.freqband;
    } else {
        return;
    }

    mib_req.Type = MIB_CHANNELS_DATARATE;
    status       = LoRaMacMibGetRequestConfirm(&mib_req);
    if (status == LORAMAC_STATUS_OK) {
        datarate = mib_req.Param.ChannelsDatarate;
    } else {
        return;
    }

    read_lora_dev(&lora_dev);
    lora_dev.freqband = freqband;
    lora_dev.datarate = datarate;
    write_lora_dev(&lora_dev);
}

static void mcps_confirm(McpsConfirm_t *mcpsConfirm)
{
    if (mcpsConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK) {
        switch (mcpsConfirm->McpsRequest) {
            case MCPS_UNCONFIRMED: {
                // Check Datarate
                // Check TxPower
                break;
            }
            case MCPS_CONFIRMED: {
                // Check Datarate
                // Check TxPower
                // Check AckReceived
                // Check NbTrials
                break;
            }
            case MCPS_PROPRIETARY: {
                break;
            }
            default:
                break;
        }
    } else {
        switch (mcpsConfirm->McpsRequest) {
            case MCPS_UNCONFIRMED: {
                // Check Datarate
                // Check TxPower
                break;
            }
            case MCPS_CONFIRMED: {
                // Check Datarate
                // Check TxPower
                // Check AckReceived
                // Check NbTrials

                reset_join_state();
                g_join_method = DEF_JOIN_METHOD;
                DBG_LINKWAN("Not receive Ack,Start to Join...\r\n");
                break;
            }
            case MCPS_PROPRIETARY: {
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
    if (mcpsIndication->Status != LORAMAC_EVENT_INFO_STATUS_OK) {
        return;
    }

    switch (mcpsIndication->McpsIndication) {
        case MCPS_UNCONFIRMED: {
            break;
        }
        case MCPS_CONFIRMED: {
            break;
        }
        case MCPS_PROPRIETARY: {
            break;
        }
        case MCPS_MULTICAST: {
            DBG_LINKWAN("MCPS_MULTICAST\n");
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
    DBG_LINKWAN("rssi = %d, snr = %d, datarate = %d\r\n", mcpsIndication->Rssi,
                mcpsIndication->Snr, mcpsIndication->RxDatarate);

    if (mcpsIndication->RxData == true) {
        switch (mcpsIndication->Port) {
            case 224:
                break;
            default:
                rx_data.Port     = mcpsIndication->Port;
                rx_data.BuffSize = mcpsIndication->BufferSize;
                memcpy1(rx_data.Buff, mcpsIndication->Buffer, rx_data.BuffSize);
                app_callbacks->LoraRxData(&rx_data);
                break;
        }
#ifdef CONFIG_DEBUG_LINKWAN
    } else if (mcpsIndication->AckReceived) {
        DBG_LINKWAN("rx, ACK\r\n");
#endif
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

static uint8_t get_freqband_num(void)
{
    uint16_t mask = get_lora_freqband_mask();

    for (uint8_t i = 0; i < 16; i++) {
        if ((mask & (1 << i)) && i != 1) {
            g_freqband_num++;
        }
    }
}

static void MlmeConfirm(MlmeConfirm_t *mlmeConfirm)
{
    uint32_t rejoin_delay;

    switch (mlmeConfirm->MlmeRequest) {
        case MLME_JOIN: {
            if (mlmeConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK) {
                // Status is OK, node has joined the network
                device_state = DEVICE_STATE_JOINED;
                if (gJoinState) {
                    GBG_LINKWAN("+CJOIN:OK");
                }
            } else {
                // Join was not successful. Try to join again
                reset_join_state();
                if (g_join_method != SCAN_JOIN_METHOD) {
                    g_join_method = (g_join_method + 1) % JOIN_METHOD_NUM;
                    rejoin_delay  = generate_rejoin_delay();
                    if (g_join_method == SCAN_JOIN_METHOD) {
                        get_freqband_num();
                    }
                }

                if (g_freqband_num == 0) {
                    if (g_join_method == DEF_JOIN_METHOD) {
                        g_join_method = (g_join_method + 1) % JOIN_METHOD_NUM;
                        rejoin_delay  = generate_rejoin_delay();
                        get_freqband_num();
                    } else {
                        g_join_method = DEF_JOIN_METHOD;
                        rejoin_delay  = 60 * 60 * 1000; // 1 hour
                        DBG_LINKWAN("Wait 1 hour for new round of scan\r\n");
                    }
                } else {
                    g_freqband_num--;
                    rejoin_delay = gJoinInterval * 1000; // rejoin interval
                }
                TimerSetValue(&TxNextPacketTimer, rejoin_delay);
                TimerStart(&TxNextPacketTimer);
                rejoin_flag = false;
            }
            break;
        }
        case MLME_LINK_CHECK: {
#ifdef CONFIG_LINKWAN
            DBG_LINKWAN("+CLINKCHECK: %d, %d, %d, %d, %d\n",
                        mlmeConfirm->Status, mlmeConfirm->DemodMargin,
                        mlmeConfirm->NbGateways, mlmeConfirm->Rssi,
                        mlmeConfirm->Snr);
#endif
            if (mlmeConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK) {
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
    if (class == CLASS_B) {
        return "class_b";
    } else if (class == CLASS_C) {
        return "class_c";
    } else {
        return "class_a";
    }
}

void lora_reboot(int8_t mode)
{
    if (mode == 0) {
        HW_Reset();
    } else if (mode == 1) {
        // reset till current frame is sent
        prepare_tx_frame();
        next_tx = send_frame();
        HW_Reset();
    }
}


void lora_init(LoRaMainCallback_t *callbacks)
{
    device_state  = DEVICE_STATE_INIT;
    app_callbacks = callbacks;

#ifdef AOS_KV
    assert(aos_kv_init() == 0);
#endif
#ifdef CONFIG_LINKWAN_AT
    extern void linkwan_at_init(void);
    linkwan_at_init();
#endif
}

void lora_fsm(void)
{
#ifdef CONFIG_LINKWAN
    int           ret;
    int           len;
    lora_dev_t    lora_dev;
    lora_abp_id_t lora_abp_id;

#ifdef AOS_KV
    memset(&lora_dev, 0, sizeof(lora_dev));
    aos_kv_get("lora_dev", &lora_dev, &len);

    memset(&lora_abp_id, 0, sizeof(lora_abp_id));
    aos_kv_get("lora_abp", &lora_abp_id, &len);
#else
    memcpy(&lora_dev, &g_lora_dev, sizeof(lora_dev_t));
#endif
#endif

    while (1) {
#ifdef CONFIG_LINKWAN_AT
        extern void process_linkwan_at(void);
        process_linkwan_at();
#endif

        switch (device_state) {
            case DEVICE_STATE_INIT: {
                if (lora_dev.freqband >= 0) {
                    g_join_method = STORED_JOIN_METHOD;
                } else {
                    g_join_method = DEF_JOIN_METHOD;
                }

                LoRaMacPrimitives.MacMcpsConfirm    = mcps_confirm;
                LoRaMacPrimitives.MacMcpsIndication = McpsIndication;
                LoRaMacPrimitives.MacMlmeConfirm    = MlmeConfirm;
                LoRaMacCallbacks.GetBatteryLevel =
                  app_callbacks->BoardGetBatteryLevel;
#if defined(REGION_AS923)
                LoRaMacInitialization(&LoRaMacPrimitives, &LoRaMacCallbacks,
                                      LORAMAC_REGION_AS923);
#elif defined(REGION_AS923)
                LoRaMacInitialization(&LoRaMacPrimitives, &LoRaMacCallbacks,
                                      LORAMAC_REGION_AU915);
#elif defined(REGION_CN470)
                LoRaMacInitialization(&LoRaMacPrimitives, &LoRaMacCallbacks,
                                      LORAMAC_REGION_CN470);
#elif defined(REGION_CN779)
                LoRaMacInitialization(&LoRaMacPrimitives, &LoRaMacCallbacks,
                                      LORAMAC_REGION_CN779);
#elif defined(REGION_EU433)
                LoRaMacInitialization(&LoRaMacPrimitives, &LoRaMacCallbacks,
                                      LORAMAC_REGION_EU433);
#elif defined(REGION_IN865)
                LoRaMacInitialization(&LoRaMacPrimitives, &LoRaMacCallbacks,
                                      LORAMAC_REGION_IN865);
#elif defined(REGION_EU868)
                LoRaMacInitialization(&LoRaMacPrimitives, &LoRaMacCallbacks,
                                      LORAMAC_REGION_EU868);
#elif defined(REGION_KR920)
                LoRaMacInitialization(&LoRaMacPrimitives, &LoRaMacCallbacks,
                                      LORAMAC_REGION_KR920);
#elif defined(REGION_US915)
                LoRaMacInitialization(&LoRaMacPrimitives, &LoRaMacCallbacks,
                                      LORAMAC_REGION_US915);
#elif defined(REGION_US915_HYBRID)
                LoRaMacInitialization(&LoRaMacPrimitives, &LoRaMacCallbacks,
                                      LORAMAC_REGION_US915_HYBRID);
#elif defined(REGION_CN470A)
                LoRaMacInitialization(&LoRaMacPrimitives, &LoRaMacCallbacks,
                                      LORAMAC_REGION_CN470A);
#else
#error "Please define a region in the compiler options."
#endif
                TimerInit(&TxNextPacketTimer, on_tx_next_packet_timer_event);

                mibReq.Type            = MIB_ADR;
                mibReq.Param.AdrEnable = lora_param.AdrEnable;
                LoRaMacMibSetRequestConfirm(&mibReq);

                mibReq.Type = MIB_PUBLIC_NETWORK;
                mibReq.Param.EnablePublicNetwork =
                  lora_param.EnablePublicNetwork;
                LoRaMacMibSetRequestConfirm(&mibReq);

                mibReq.Type        = MIB_DEVICE_CLASS;
                mibReq.Param.Class = lora_dev.class;
                LoRaMacMibSetRequestConfirm(&mibReq);

#if defined(REGION_EU868)
                lora_config_duty_cycle_set(LORAWAN_DUTYCYCLE_ON ? 1 : 0);

#if (USE_SEMTECH_DEFAULT_CHANNEL_LINEUP == 1)
                LoRaMacChannelAdd(3, (ChannelParams_t)LC4);
                LoRaMacChannelAdd(4, (ChannelParams_t)LC5);
                LoRaMacChannelAdd(5, (ChannelParams_t)LC6);
                LoRaMacChannelAdd(6, (ChannelParams_t)LC7);
                LoRaMacChannelAdd(7, (ChannelParams_t)LC8);
                LoRaMacChannelAdd(8, (ChannelParams_t)LC9);
                LoRaMacChannelAdd(9, (ChannelParams_t)LC10);

                mibReq.Type = MIB_RX2_DEFAULT_CHANNEL;
                mibReq.Param.Rx2DefaultChannel =
                  (Rx2ChannelParams_t){ 869525000, DR_3 };
                LoRaMacMibSetRequestConfirm(&mibReq);

                mibReq.Type = MIB_RX2_CHANNEL;
                mibReq.Param.Rx2Channel =
                  (Rx2ChannelParams_t){ 869525000, DR_3 };
                LoRaMacMibSetRequestConfirm(&mibReq);
#endif

#endif
                device_state = DEVICE_STATE_JOIN;
                break;
            }

            case DEVICE_STATE_JOIN: {
                MlmeReq_t mlmeReq;
                if (lora_param.JoinMode == JOIN_MODE_OTAA) {
                    mlmeReq.Type            = MLME_JOIN;
                    mlmeReq.Req.Join.DevEui = lora_dev.dev_eui;
                    mlmeReq.Req.Join.AppEui = lora_dev.app_eui;
                    mlmeReq.Req.Join.AppKey = lora_dev.app_key;

                    mlmeReq.Req.Join.method = g_join_method;
                    if (g_join_method == STORED_JOIN_METHOD) {
                        mlmeReq.Req.Join.freqband = lora_dev.freqband;
                        mlmeReq.Req.Join.datarate = lora_dev.datarate;
                        mlmeReq.Req.Join.NbTrials = 3;
                    } else {
                        mlmeReq.Req.Join.NbTrials = num_trials;
                    }
                    if (next_tx == true && rejoin_flag == true) {
                        if (LoRaMacMlmeRequest(&mlmeReq) == LORAMAC_STATUS_OK) {
                            next_tx = false;
                        }
                        DBG_LINKWAN(
                          "Start to Join, method %d, nb_trials:%d\r\n",
                          g_join_method, mlmeReq.Req.Join.NbTrials);
                    }
                    device_state = DEVICE_STATE_SLEEP;
                } else if (lora_param.JoinMode == JOIN_MODE_ABP) {
                    mibReq.Type        = MIB_NET_ID;
                    mibReq.Param.NetID = LORAWAN_NETWORK_ID;
                    LoRaMacMibSetRequestConfirm(&mibReq);

                    mibReq.Type          = MIB_DEV_ADDR;
                    mibReq.Param.DevAddr = g_lora_abp_id.devaddr;
                    LoRaMacMibSetRequestConfirm(&mibReq);

                    mibReq.Type          = MIB_NWK_SKEY;
                    mibReq.Param.NwkSKey = g_lora_abp_id.nwkskey;
                    LoRaMacMibSetRequestConfirm(&mibReq);

                    mibReq.Type          = MIB_APP_SKEY;
                    mibReq.Param.AppSKey = g_lora_abp_id.appskey;
                    LoRaMacMibSetRequestConfirm(&mibReq);

                    mibReq.Type                  = MIB_NETWORK_JOINED;
                    mibReq.Param.IsNetworkJoined = true;
                    LoRaMacMibSetRequestConfirm(&mibReq);

                    device_state = DEVICE_STATE_SEND;
                }
                break;
            }
            case DEVICE_STATE_JOINED: {
                DBG_LINKWAN("Joined\n\r");
                store_lora_config();
                device_state = DEVICE_STATE_SEND;
                break;
            }
            case DEVICE_STATE_SEND: {
                if (next_tx == true) {
                    CPSR_ALLOC();
                    RHINO_CPU_INTRPT_DISABLE();

                    prepare_tx_frame();

                    next_tx = send_frame();

                    RHINO_CPU_INTRPT_ENABLE();
                }
                if (lora_param.TxEvent == TX_ON_TIMER) {
                    start_dutycycle_timer();
                } else if (lora_param.TxEvent == TX_ON_EVENT) {
                    lora_param.TxEvent = TX_ON_NONE;
                }
                device_state = DEVICE_STATE_SLEEP;
                break;
            }
            case DEVICE_STATE_SEND_MAC: {
                if (next_tx == true) {
                    tx_data.BuffSize = 0;
                    next_tx          = send_frame();
                }
                device_state = DEVICE_STATE_SLEEP;
                break;
            }
            case DEVICE_STATE_SLEEP: {
                // Wake up through events
#ifndef LOW_POWER_DISABLE
                LowPower_Handler();
#endif
                break;
            }
            default: {
                device_state = DEVICE_STATE_INIT;
                break;
            }
        }
    }
}

DeviceState_t lora_getDeviceState(void)
{
    return device_state;
}

JoinMode_t get_lora_join_mode(void)
{
    return lora_param.JoinMode;
}

bool set_lora_join_mode(JoinMode_t mode)
{
    lora_param.JoinMode = mode;
    return true;
}

node_freq_mode_t get_lora_freq_mode(void)
{
#ifdef AOS_KV
    int len;
    aos_kv_get("lora_uldl_mode", &g_freq_mode, &len);
#endif
    return g_freq_mode;
}

bool set_lora_freq_mode(node_freq_mode_t mode)
{
    if (mode != FREQ_MODE_INTRA && mode != FREQ_MODE_INTER) {
        return false;
    }
    g_freq_mode = mode;
#ifdef AOS_KV
    aos_kv_set("lora_uldl_mode", &g_freq_mode, sizeof(g_freq_mode));
#endif
    return true;
}

bool set_lora_tx_datarate(int8_t datarate)
{
    if (datarate >= CN470A_TX_MIN_DATARATE &&
        datarate <= CN470A_TX_MAX_DATARATE && get_lora_adr() == 0) {
        lora_param.TxDatarate = datarate;
        return true;
    } else {
        return false;
    }
}

int8_t get_lora_tx_datarate(void)
{
    return lora_param.TxDatarate;
}

bool set_lora_adr(int state)
{
    LoRaMacStatus_t     status;
    MibRequestConfirm_t mib_req;
    bool                ret = false;

    if (state == 0) {
        mib_req.Param.AdrEnable = false;
    } else {
        mib_req.Param.AdrEnable = true;
    }
    mib_req.Type = MIB_ADR;
    status       = LoRaMacMibSetRequestConfirm(&mib_req);
    if (status == LORAMAC_STATUS_OK) {
        ret = true;
    }
    return ret;
}

int get_lora_adr(void)
{
    MibRequestConfirm_t mib_req;

    mib_req.Type = MIB_ADR;
    LoRaMacMibGetRequestConfirm(&mib_req);
    if (mib_req.Param.AdrEnable == true) {
        return 1;
    }
    return 0;
}

static void start_dutycycle_timer(void)
{
    MibRequestConfirm_t mib_req;
    LoRaMacStatus_t     status;

    TimerStop(&TxNextPacketTimer);
    mib_req.Type = MIB_NETWORK_JOINED;
    status       = LoRaMacMibGetRequestConfirm(&mib_req);
    if (status == LORAMAC_STATUS_OK) {
        if (mib_req.Param.IsNetworkJoined == true &&
            lora_param.TxEvent == TX_ON_TIMER &&
            lora_param.TxDutyCycleTime != 0) {
            TimerSetValue(&TxNextPacketTimer, lora_param.TxDutyCycleTime);
            TimerStart(&TxNextPacketTimer);
            return;
        }
    }
    if (lora_param.TxDutyCycleTime == 0 && lora_param.TxEvent == TX_ON_TIMER) {
        lora_param.TxEvent = TX_ON_NONE;
    }
}

bool set_lora_tx_dutycycle(uint32_t dutycycle)
{
    if (dutycycle != 0 && dutycycle < 1000) {
        dutycycle = 1000;
    }

    lora_param.TxDutyCycleTime = dutycycle;
    TimerStop(&TxNextPacketTimer);
    if (dutycycle == 0) {
        lora_param.TxEvent = TX_ON_NONE;
    } else {
        lora_param.TxEvent = TX_ON_TIMER;
        start_dutycycle_timer();
    }
    return true;
}

uint32_t get_lora_tx_dutycycle(void)
{
    return lora_param.TxDutyCycleTime;
}

lora_AppData_t *get_lora_rx_data(void)
{
    return &rx_data;
}

lora_AppData_t *get_lora_tx_data(void)
{
    MibRequestConfirm_t mib_req;
    LoRaMacStatus_t     status;

    if (next_tx == false) {
        return NULL;
    }

    mib_req.Type = MIB_NETWORK_JOINED;
    status       = LoRaMacMibGetRequestConfirm(&mib_req);
    if (status == LORAMAC_STATUS_OK) {
        if (mib_req.Param.IsNetworkJoined == true) {
            return &tx_data;
        }
    }

    return NULL;
}

bool tx_lora_data(void)
{
    MibRequestConfirm_t mib_req;
    LoRaMacStatus_t     status;

    if (next_tx == false) {
        return false;
    }

    mib_req.Type = MIB_NETWORK_JOINED;
    status       = LoRaMacMibGetRequestConfirm(&mib_req);
    if (status == LORAMAC_STATUS_OK) {
        if (mib_req.Param.IsNetworkJoined == true) {
            TimerStop(&TxNextPacketTimer);
            lora_param.TxEvent = TX_ON_EVENT;
            device_state       = DEVICE_STATE_SEND;
            return true;
        }
    }

    return false;
}

bool set_lora_tx_cfm_flag(int confirmed)
{
    is_tx_confirmed = confirmed;
    return true;
}

int get_lora_tx_cfm_flag(void)
{
    return is_tx_confirmed;
}

bool set_lora_tx_cfm_trials(uint8_t trials)
{
    num_trials = trials;
    return true;
}

uint8_t get_lora_tx_cfm_trials(void)
{
    return num_trials;
}

bool set_lora_state(DeviceState_t state)
{
    if (device_state == DEVICE_STATE_SLEEP) {
        TimerStop(&TxNextPacketTimer);
    }
    device_state = state;
    return true;
}

bool set_lora_class(int8_t class)
{
    lora_dev_t lora_dev;

    if (class >= CLASS_A && class <= CLASS_C) {
        read_lora_dev(&lora_dev);
        lora_dev.class = class;
        write_lora_dev(&lora_dev);
        return true;
    }
    return false;
}

int8_t get_lora_class(void)
{
    lora_dev_t lora_dev;

    read_lora_dev(&lora_dev);
    return lora_dev.class;
}

bool set_lora_dev_eui(uint8_t *eui)
{
    lora_dev_t lora_dev;

    read_lora_dev(&lora_dev);
    memcpy(lora_dev.dev_eui, eui, 8);
    write_lora_dev(&lora_dev);
    return true;
}

void get_lora_dev_eui(uint8_t *eui)
{
    lora_dev_t lora_dev;

    read_lora_dev(&lora_dev);
    memcpy(eui, lora_dev.dev_eui, 8);
}

bool set_lora_app_eui(uint8_t *eui)
{
    lora_dev_t lora_dev;

    read_lora_dev(&lora_dev);
    memcpy(lora_dev.app_eui, eui, 8);
    write_lora_dev(&lora_dev);
    return true;
}

void get_lora_app_eui(uint8_t *eui)
{
    lora_dev_t lora_dev;
    read_lora_dev(&lora_dev);
    memcpy(eui, lora_dev.app_eui, 8);
}

bool set_lora_app_key(uint8_t *key)
{
    lora_dev_t lora_dev;

    read_lora_dev(&lora_dev);
    memcpy(lora_dev.app_key, key, 16);
    write_lora_dev(&lora_dev);
    return true;
}

void get_lora_app_key(uint8_t *key)
{
    lora_dev_t lora_dev;

    read_lora_dev(&lora_dev);
    memcpy(key, lora_dev.app_key, 16);
}

bool set_lora_freqband_mask(uint16_t mask)
{
    lora_dev_t lora_dev;

    read_lora_dev(&lora_dev);
    lora_dev.mask = mask;
    write_lora_dev(&lora_dev);
    return true;
}

uint16_t get_lora_freqband_mask(void)
{
    lora_dev_t lora_dev;

    read_lora_dev(&lora_dev);
    return lora_dev.mask;
}

void tx_lora_mac_req(void)
{
    if (device_state != DEVICE_STATE_SEND) {
        device_state = DEVICE_STATE_SEND_MAC;
    }
}

void get_lora_rssi(uint8_t band, int16_t *channel_rssi)
{
    for (uint8_t i = 0; i < 8; i++) {
        channel_rssi[i] = Radio.Rssi(MODEM_LORA);
    }
}

bool get_lora_report_mode(void)
{
    if (lora_param.TxEvent == TX_ON_TIMER) {
        return true;
    } else {
        return false;
    }
}

bool set_lora_report_mode(int8_t mode)
{
    if (mode != 0 || mode != 1) {
        return false;
    }
    if (mode == 1) {
        lora_param.TxEvent = TX_ON_TIMER;
    } else if (mode == 0) {
        lora_param.TxEvent = TX_ON_NONE;
    }
    return true;
}

int8_t get_lora_tx_power(void)
{
    MibRequestConfirm_t mib_req;
    LoRaMacStatus_t     status;

    mib_req.Type = MIB_CHANNELS_TX_POWER;
    status       = LoRaMacMibGetRequestConfirm(&mib_req);
    if (status == LORAMAC_STATUS_OK) {
        return mib_req.Param.ChannelsTxPower;
    } else {
        return -1;
    }
}

bool set_lora_tx_power(int8_t pwr)
{
    MibRequestConfirm_t mib_req;
    LoRaMacStatus_t     status;

    mib_req.Type                  = MIB_CHANNELS_TX_POWER;
    mib_req.Param.ChannelsTxPower = pwr;
    status                        = LoRaMacMibSetRequestConfirm(&mib_req);
    if (status == LORAMAC_STATUS_OK) {
        return true;
    } else {
        return false;
    }
}
// for linkWAN test
bool set_lora_tx_len(uint16_t len)
{
    if (len <= LORAWAN_APP_DATA_BUFF_SIZE) {
        tx_size = len;
        return true;
    }
    return false;
}

uint8_t get_lora_tx_len(void)
{
    return tx_data.BuffSize;
}


bool send_lora_link_check(void)
{
    MlmeReq_t mlmeReq;

    mlmeReq.Type = MLME_LINK_CHECK;
    if (next_tx == true) {
        if (LoRaMacMlmeRequest(&mlmeReq) == LORAMAC_STATUS_OK) {
            next_tx = false;
            return lora_tx_data_payload(1, get_lora_tx_cfm_trials(), NULL, 0);
        }
    }
    return false;
}

int get_device_status(void)
{
    return device_state;
}

bool get_lora_rx_window_params(uint8_t *RX1DRoffset, uint8_t *RX2DataRate,
                               uint32_t *RX2Frequency)
{
    MibRequestConfirm_t mib_req;
    LoRaMacStatus_t     status;

    if (next_tx == false) {
        return NULL;
    }

    mib_req.Type = MIB_RX1_DROFFSET;
    status       = LoRaMacMibGetRequestConfirm(&mib_req);
    if (status == LORAMAC_STATUS_OK) {
        *RX1DRoffset = mib_req.Param.Rx1DrOffset;
    } else {
        return false;
    }
    mib_req.Type = MIB_RX2_CHANNEL;
    status       = LoRaMacMibGetRequestConfirm(&mib_req);
    if (status == LORAMAC_STATUS_OK) {
        *RX2DataRate  = mib_req.Param.Rx2Channel.Datarate;
        *RX2Frequency = mib_req.Param.Rx2Channel.Frequency;
    } else {
        return false;
    }
    return true;
}
bool set_lora_rx_window_params(uint8_t RX1DRoffset, uint8_t RX2DR,
                               uint32_t RX2Freq)
{
    MibRequestConfirm_t mib_req;
    LoRaMacStatus_t     status;

    if (next_tx == false) {
        return NULL;
    }

    mib_req.Type              = MIB_RX1_DROFFSET;
    mib_req.Param.Rx1DrOffset = RX1DRoffset;
    status                    = LoRaMacMibSetRequestConfirm(&mib_req);
    if (status != LORAMAC_STATUS_OK) {
        return false;
    }

    mib_req.Type                       = MIB_RX2_CHANNEL;
    mib_req.Param.Rx2Channel.Datarate  = RX2DR;
    mib_req.Param.Rx2Channel.Frequency = RX2Freq;
    status                             = LoRaMacMibSetRequestConfirm(&mib_req);

    if (status != LORAMAC_STATUS_OK) {
        return false;
    }

    return true;
}
uint32_t get_lora_mac_rx1_delay(void)
{
    MibRequestConfirm_t mib_req;
    LoRaMacStatus_t     status;

    mib_req.Type = MIB_RECEIVE_DELAY_1;
    status       = LoRaMacMibGetRequestConfirm(&mib_req);
    if (status == LORAMAC_STATUS_OK) {
        return mib_req.Param.ReceiveDelay1;
    } else {
        return false;
    }
}
bool set_lora_mac_rx1_delay(long rx1delay)
{
    MibRequestConfirm_t mib_req;
    LoRaMacStatus_t     status;

    if (next_tx == false) {
        return NULL;
    }

    mib_req.Type                = MIB_RECEIVE_DELAY_1;
    mib_req.Param.ReceiveDelay1 = rx1delay;
    status                      = LoRaMacMibSetRequestConfirm(&mib_req);
    if (status == LORAMAC_STATUS_OK) {
        return true;
    } else {
        return false;
    }
}

bool set_lora_devaddr(uint8_t *devaddr)
{
    memcpy(g_lora_abp_id.devaddr, devaddr, 4);
    g_lora_abp_id.flag = VALID_LORA_CONFIG;
#ifdef AOS_KV
    aos_kv_set("lora_abp", &g_lora_abp_id, sizeof(g_lora_abp_id));
#endif
    return true;
}

uint8_t *get_lora_devaddr(void)
{
    return g_lora_abp_id.devaddr;
}

bool set_lora_appskey(uint8_t *buf)
{
    memcpy(g_lora_abp_id.appskey, buf, 16);
    g_lora_abp_id.flag = VALID_LORA_CONFIG;
#ifdef AOS_KV
    aos_kv_set("lora_abp", &g_lora_abp_id, sizeof(g_lora_abp_id));
#endif
    return true;
}
uint8_t *get_lora_appskey(void)
{
    return g_lora_abp_id.appskey;
}


bool set_lora_nwkskey(uint8_t *buf)
{
    memcpy(g_lora_abp_id.nwkskey, buf, 16);
    g_lora_abp_id.flag = VALID_LORA_CONFIG;
#ifdef AOS_KV
    aos_kv_set("lora_abp", &g_lora_abp_id, sizeof(g_lora_abp_id));
#endif
    return true;
}
uint8_t *get_lora_nwkskey(void)
{
    return g_lora_abp_id.nwkskey;
}

bool set_lora_app_port(uint8_t port)
{
    if (port < 1 || port > 223) {
        return false;
    }
    tx_data.Port = port;
    return true;
}

uint8_t get_lora_app_port(void)
{
    return tx_data.Port;
}


MulticastParams_t *get_lora_cur_multicast(void)
{
    MibRequestConfirm_t mib_req;
    LoRaMacStatus_t     status;

    mib_req.Type = MIB_MULTICAST_CHANNEL;
    status       = LoRaMacMibGetRequestConfirm(&mib_req);
    if (status == LORAMAC_STATUS_OK) {
        return mib_req.Param.MulticastList;
    } else {
        return NULL;
    }
}

bool set_lora_multicast(MulticastParams_t *multicastInfo)
{
    MibRequestConfirm_t mib_req;
    LoRaMacStatus_t     status;

    mib_req.Type                = MIB_MULTICAST_CHANNEL;
    mib_req.Param.MulticastList = multicastInfo;
    status                      = LoRaMacMibSetRequestConfirm(&mib_req);
    if (status == LORAMAC_STATUS_OK) {
        return true;
    } else {
        return false;
    }
}

bool lora_del_multicast(uint32_t dev_addr)
{
    MulticastParams_t *multiCastNode = get_lora_cur_multicast();

    if (multiCastNode == NULL) {
        return false;
    }
    while (multiCastNode != NULL) {
        if (dev_addr == multiCastNode->Address) {
            LoraMacDelMultCastNode(multiCastNode);
            return true;
        }
        multiCastNode = multiCastNode->Next;
    }
}

uint8_t get_lora_mulitcast_num(void)
{

    MulticastParams_t *multiCastNode = get_lora_cur_multicast();
    if (multiCastNode == NULL) {
        return 0;
    }
    uint8_t num = 0;
    while (multiCastNode != NULL) {
        num++;
        multiCastNode = multiCastNode->Next;
    }
    return num;
}

uint8_t get_device_battery()
{
    return app_callbacks->BoardGetBatteryLevel();
}

bool get_lora_join_params(uint8_t *bJoin, uint8_t *bAuto,
                          uint16_t *joinInterval, uint16_t *joinRetryCnt)
{
    *bJoin        = gAutoJoin;
    *bAuto        = gAutoJoin;
    *joinInterval = gJoinInterval;
    *joinRetryCnt = get_lora_tx_cfm_trials();
    return true;
}
bool init_lora_join(uint8_t bJoin, uint8_t bAutoJoin, uint16_t joinInterval,
                    uint16_t joinRetryCnt)
{

    bool ret = false;
    if (bJoin == 0) { // stop join
        TimerStop(&TxNextPacketTimer);
        MibRequestConfirm_t mib_req;
        LoRaMacStatus_t     status;
        mib_req.Type                  = MIB_NETWORK_JOINED;
        mib_req.Param.IsNetworkJoined = false;
        status                        = LoRaMacMibSetRequestConfirm(&mib_req);

        if (status == LORAMAC_STATUS_OK) {
            device_state = DEVICE_STATE_INIT;
            rejoin_flag  = bAutoJoin;
            ret          = true;
        }
    } else if (bJoin == 1) {
        set_lora_tx_cfm_trials(joinRetryCnt);
        rejoin_flag = bAutoJoin;
        MibRequestConfirm_t mib_req;
        mib_req.Type           = MIB_NETWORK_JOINED;
        LoRaMacStatus_t status = LoRaMacMibGetRequestConfirm(&mib_req);
        if (status == LORAMAC_STATUS_OK) {
            if (mib_req.Param.IsNetworkJoined == true) {
                mib_req.Type                  = MIB_NETWORK_JOINED;
                mib_req.Param.IsNetworkJoined = false;
                LoRaMacMibSetRequestConfirm(&mib_req);
            }
            DBG_LINKWAN("Rejoin again...\r");
            gJoinInterval = joinInterval;
            reset_join_state();
            g_join_method = DEF_JOIN_METHOD;
            TimerStop(&TxNextPacketTimer);
            TimerSetValue(&TxNextPacketTimer, joinInterval);
            TimerStart(&TxNextPacketTimer);
            return true;
        }
    } else {
        return false;
    }
}

bool lora_tx_data_payload(uint8_t confirm, uint8_t Nbtrials, uint8_t *payload,
                          uint8_t len)
{
    MibRequestConfirm_t mib_req;
    LoRaMacStatus_t     status;

    TimerStop(&TxNextPacketTimer);

    mib_req.Type = MIB_NETWORK_JOINED;
    status       = LoRaMacMibGetRequestConfirm(&mib_req);

    if (status == LORAMAC_STATUS_OK) {
        if (mib_req.Param.IsNetworkJoined == true) {
            is_tx_confirmed = confirm;
            memcpy(tx_data.Buff, payload, len);
            tx_data.BuffSize = len;
            set_lora_tx_cfm_trials(Nbtrials);
            device_state = DEVICE_STATE_SEND;
            return true;

        } else {
            rejoin_flag  = true;
            device_state = DEVICE_STATE_JOIN;
            return false;
        }
    }
    return false;
}

/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "linkwan.h"
#include "linkwan/include/commissioning.h"
#include "utilities.h"
#include "LoRaMac.h"
#include "Region.h"
#include "RegionCN470.h"
#include "system/timer.h"
#include "radio.h"
#ifdef AOS_COMP_KV
#include <assert.h>
#include <aos/kv.h>
#endif
#include <k_config.h>
#include <k_types.h>
#include <k_err.h>
#include <k_sys.h>
#include <k_list.h>
#include <k_obj.h>
#include <k_task.h>
#include <port.h>

#define LORAWAN_DEFAULT_PING_SLOT_PERIODICITY       3

#define LORAWAN_SEND_FAIL_MAX_RETRY                 3

static uint8_t        tx_buf[LORAWAN_APP_DATA_BUFF_SIZE];
static lora_AppData_t tx_data = { tx_buf, 1, 0 };
static uint8_t        rx_buf[LORAWAN_APP_DATA_BUFF_SIZE];
static lora_AppData_t rx_data = { rx_buf, 0, 0 };
static uint8_t        tx_size = 1;

static LoRaMacPrimitives_t LoRaMacPrimitives;
static LoRaMacCallback_t   LoRaMacCallbacks;
static LoRaMainCallback_t *app_callbacks;
static MibRequestConfirm_t mibReq;
static app_class_type_t    app_classType;

static int8_t  is_tx_confirmed      = 1;
static bool    next_tx              = true;
static uint8_t num_trials           = 8;
static uint8_t join_trials_counter  = 0;
static bool    rejoin_flag          = true;

static uint8_t g_freqband_num = 0;
static uint8_t g_sendfail_count = 0;

static LoRaParam_t lora_param = {
    TX_ON_NONE,    0, true, DR_0, LORAWAN_PUBLIC_NETWORK, JOINREQ_NBTRIALS,
    JOIN_MODE_OTAA
};

/*!
 * LoRaWAN compliance tests support data
 */
struct ComplianceTest_s {
    bool Running;
    uint8_t State;
    bool IsTxConfirmed;
    uint16_t DownLinkCounter;
    bool LinkCheck;
    uint8_t DemodMargin;
    uint8_t NbGateways;
    uint8_t uplink_cycle;
} ComplianceTest;

static TimerEvent_t           TxNextPacketTimer;
volatile static DeviceState_t device_state = DEVICE_STATE_INIT;
lora_dev_t                    g_lora_dev   = { LORAWAN_DEVICE_EUI,
                          LORAWAN_APPLICATION_EUI,
                          LORAWAN_APPLICATION_KEY,
                          1,
                          DR_2,
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
static uint8_t  gClassBPingPeriod = LORAWAN_DEFAULT_PING_SLOT_PERIODICITY;

static uint8_t  g_multicast_channels_mask = 0;

static int16_t  g_channel_rssi[8];
static int8_t   g_channel_snr[8];


#define LORA_BANDWIDTH                              0         // [0: 125 kHz]
#define LORA_SPREADING_FACTOR                       10        // [SF7..SF12]
#define LORA_CODINGRATE                             1         // [1: 4/5]
#define LORA_SYMBOL_TIMEOUT                         5         // Symbols
#define LORA_PREAMBLE_LENGTH                        8         // Same for Tx and Rx
#define LORA_FIX_LENGTH_PAYLOAD_ON                  false
#define LORA_IQ_INVERSION_ON                        false

#define TX_OUTPUT_POWER                             14        // dBm


static void start_dutycycle_timer(void);

static int8_t set_multicast_channel(MulticastChannel_t *mc_channel,
                                    uint8_t *mc_key,
                                    uint8_t mc_id)
{
    MibRequestConfirm_t mibReq;
    AddressIdentifier_t mc_addrid;

    switch (mc_id) {
        case 0:
            mibReq.Type = MIB_MC_KEY_0;
            mc_addrid   = MULTICAST_0_ADDR;
            break;
        case 1:
            mibReq.Type = MIB_MC_KEY_1;
            mc_addrid   = MULTICAST_1_ADDR;
            break;
        case 2:
            mibReq.Type = MIB_MC_KEY_2;
            mc_addrid   = MULTICAST_2_ADDR;
            break;
        case 3:
            mibReq.Type = MIB_MC_KEY_3;
            mc_addrid   = MULTICAST_3_ADDR;
            break;
        default:
            mibReq.Type = MIB_MC_KEY_0;
            mc_addrid   = MULTICAST_0_ADDR;
            break;
    }

    mibReq.Param.McKey1 = mc_key;

    if (LORAMAC_STATUS_OK != LoRaMacMibSetRequestConfirm(&mibReq)) {
        DBG_LINKWAN("set mib mc error\r\n");

        return -1;
    }

    if (LORAMAC_STATUS_OK != LoRaMacCryptoDeriveMcSessionKeyPair(mc_addrid, mc_channel->Address)) {
        DBG_LINKWAN("derive mc skey error\r\n");

        return -1;
    }

    if (LORAMAC_STATUS_OK != LoRaMacMulticastChannelSet( *mc_channel )) {
        DBG_LINKWAN("set multicast channel error\r\n");

        return -1;
    }

    return 0;
}

int8_t multicast_add(uint32_t dev_addr,
                     uint32_t frequency,
                     uint8_t  data_rate,
                     uint16_t periodicity,
                     uint8_t *mc_key )
{
    int8_t              i;
    int8_t              empty_pos = -1;
    MulticastChannel_t  mc_channel;

    multicast_delete(dev_addr);

    for (i = LORAMAC_MAX_MC_CTX; i >= 0; i--) {
        if (0 == (g_multicast_channels_mask & (1 << i))) {
            empty_pos = i;
        }
    }

    if (empty_pos == -1) {
        return -1;
    }

    mc_channel.Address      = dev_addr;
    mc_channel.AddrID       = (AddressIdentifier_t)empty_pos;
    mc_channel.Datarate     = data_rate;
    mc_channel.Frequency    = frequency;
    mc_channel.IsEnabled    = true;
    mc_channel.Periodicity  = periodicity;

    if (0 == set_multicast_channel(&mc_channel, mc_key, empty_pos)) {
        g_multicast_channels_mask |= (1 << empty_pos);

        return empty_pos;
    }

    return -1;
}

int8_t multicast_delete(uint32_t dev_addr)
{
    uint8_t             i;
    MulticastChannel_t  multicast_channel;

    if (g_multicast_channels_mask == 0) {
        return -1;
    } else {
        for (i = 0; i < LORAMAC_MAX_MC_CTX; i++) {
            if (g_multicast_channels_mask & (1 << i)) {

                if (LORAMAC_STATUS_OK == LoRaMacMulticastChannelGet(i, &multicast_channel)) {

                    if (multicast_channel.Address == dev_addr) {

                        multicast_channel.IsEnabled = false;

                        if (LORAMAC_STATUS_OK == LoRaMacMulticastChannelSet(multicast_channel)) {
                            g_multicast_channels_mask &= ~(1 << i);

                            return 0;
                        }
                    }
                }
            }
        }

        return -1;
    }
}

uint8_t multicast_get_num(void)
{
    uint8_t   i;
    uint8_t   multicast_num = 0;

    for (i = 0; i < LORAMAC_MAX_MC_CTX; i++) {
        if (g_multicast_channels_mask & (1 << i)) {
            multicast_num++;
        }
    }

    return multicast_num;
}

static bool send_frame(void)
{
    McpsReq_t       mcpsReq;
    LoRaMacTxInfo_t txInfo;

    if (tx_data.BuffSize > LINKWAN_APP_DATA_SIZE) {
        tx_data.BuffSize = LINKWAN_APP_DATA_SIZE;
    }

    if (LoRaMacQueryTxPossible(tx_data.BuffSize, &txInfo) !=
        LORAMAC_STATUS_OK) {
        // Send empty frame in order to flush MAC commands
        mcpsReq.Type = MCPS_UNCONFIRMED;
        mcpsReq.Req.Unconfirmed.fBuffer = NULL;
        mcpsReq.Req.Unconfirmed.fBufferSize = 0;
        mcpsReq.Req.Unconfirmed.Datarate = lora_param.TxDatarate;
    } else {
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
    }

    if (LoRaMacMcpsRequest(&mcpsReq) == LORAMAC_STATUS_OK) {
        return false;
    }

    return true;
}

static void prepare_tx_frame(void)
{
    if ( ComplianceTest.Running == true ) {
        if ( ComplianceTest.LinkCheck == true ) {
            ComplianceTest.LinkCheck = false;
            tx_data.BuffSize = 3;
            tx_data.Buff[0] = 5;
            tx_data.Buff[1] = ComplianceTest.DemodMargin;
            tx_data.Buff[2] = ComplianceTest.NbGateways;
            ComplianceTest.State = 1;
        } else {
            switch ( ComplianceTest.State ) {
                case 4:
                    ComplianceTest.State = 1;
                    break;
                case 1:
                    tx_data.BuffSize = 2;
                    tx_data.Buff[0] = ComplianceTest.DownLinkCounter >> 8;
                    tx_data.Buff[1] = ComplianceTest.DownLinkCounter;
                    break;
            }
        }
    } else {
        if (lora_param.TxEvent == TX_ON_TIMER) {
            app_callbacks->LoraTxData(&tx_data);
        }
    }
}

static void on_tx_next_packet_timer_event(void)
{
    MibRequestConfirm_t mib_req;
    LoRaMacStatus_t     status;


    TimerStop(&TxNextPacketTimer);

    mib_req.Type = MIB_NETWORK_ACTIVATION;
    status       = LoRaMacMibGetRequestConfirm(&mib_req);

    if (status == LORAMAC_STATUS_OK) {
        if (mib_req.Param.NetworkActivation != ACTIVATION_TYPE_NONE) {
            device_state = DEVICE_STATE_SEND;
            next_tx = true;
        } else {
            rejoin_flag  = true;
            device_state = DEVICE_STATE_JOIN;
            next_tx      = true;
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
#ifdef AOS_COMP_KV
    aos_kv_get("lora_dev", lora_dev, &len);

    crc = crc16((uint8_t *)lora_dev, len - 2);
    if (crc != lora_dev->crc) {
        lora_dev->freqband = -1;
        lora_dev->class    = CLASS_A;
        lora_dev->mode     = NODE_MODE_NORMAL;
        lora_dev->mask     = 0xffff;
    }
#else
    memcpy1(lora_dev, &g_lora_dev, sizeof(lora_dev_t));
#endif
}

static void write_lora_dev(lora_dev_t *lora_dev)
{
    lora_dev->crc = crc16((uint8_t *)lora_dev, sizeof(lora_dev_t) - 2);
#ifdef AOS_COMP_KV
    aos_kv_set("lora_dev", lora_dev, sizeof(lora_dev_t), 1);
#else
    memcpy1(&g_lora_dev, lora_dev, sizeof(lora_dev_t));
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

    mib_req.Type = MIB_CHANNELS_DATARATE;
    status       = LoRaMacMibGetRequestConfirm(&mib_req);
    if (status == LORAMAC_STATUS_OK) {
        datarate = mib_req.Param.ChannelsDatarate;
    } else {
        return;
    }

    read_lora_dev(&lora_dev);

    if ((lora_dev.freqband != freqband)||
        (lora_dev.datarate != datarate)) {
        lora_dev.freqband = freqband;
        lora_dev.datarate = datarate;
    }

    write_lora_dev(&lora_dev);
}

static void mcps_confirm(McpsConfirm_t *mcpsConfirm)
{
    if (mcpsConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK) {

        g_sendfail_count = 0;

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

                g_sendfail_count++;

                if (g_sendfail_count > LORAWAN_SEND_FAIL_MAX_RETRY) {
                    g_sendfail_count = 0;

                    reset_join_state();
                    g_join_method = DEF_JOIN_METHOD;

                    DBG_LINKWAN("Not receive Ack,Start to Join...\r\n");
                } else {
                    device_state = DEVICE_STATE_SEND;

                    DBG_LINKWAN("Not receive Ack,Start to resend...\r\n");
                }

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

static void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr)
{
    DBG_LINKWAN( "rssi = %d, snr = %d\r\n", rssi, snr);
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
    if (mcpsIndication->Multicast == true) {
        DBG_LINKWAN("get the multicast\r\n");
    }
    // Check Port
    // Check Datarate
    // Check FramePending
    if (mcpsIndication->FramePending == true) {
        tx_data.BuffSize = 0;
        on_tx_next_packet_timer_event();
    }
    // Check Buffer
    // Check BufferSize
    // Check Rssi
    // Check Snr
    // Check RxSlot
    DBG_LINKWAN("rssi = %d, snr = %d, datarate = %d\r\n", mcpsIndication->Rssi,
                mcpsIndication->Snr, mcpsIndication->RxDatarate);

    // save rssi and snr for at command AT+CRSSI ?
    uint8_t rssi_index;

    if (mcpsIndication->RxSlot == RX_SLOT_WIN_1) {
        rssi_index = LoRaMacGetCurrentChannel();

        if (rssi_index > 7) {
            rssi_index &= 0x07;
        }

        set_lora_rssi(rssi_index, mcpsIndication->Rssi, mcpsIndication->Snr);
    }

    if ( ComplianceTest.Running == true ) {
        ComplianceTest.DownLinkCounter++;
    }

    if (mcpsIndication->RxData == true) {
        switch (mcpsIndication->Port) {
            case 224:
                if ( ComplianceTest.Running == false ) {
                    // Check compliance test enable command (i)
                    if ( (mcpsIndication->BufferSize == 4) &&
                         (mcpsIndication->Buffer[0] == 0x01) &&
                         (mcpsIndication->Buffer[1] == 0x01) &&
                         (mcpsIndication->Buffer[2] == 0x01) &&
                         (mcpsIndication->Buffer[3] == 0x01) ) {
                        ComplianceTest.IsTxConfirmed = false;
                        set_lora_tx_cfm_flag(ComplianceTest.IsTxConfirmed);
                        tx_data.Port = 224;
                        tx_data.BuffSize = 2;
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

#if defined(REGION_EU868)
                        LoRaMacTestSetDutyCycleOn(false);
#endif
                        set_lora_tx_dutycycle(ComplianceTest.uplink_cycle * 1000);
                        device_state = DEVICE_STATE_SEND;
                    }
                } else {
                    ComplianceTest.State = mcpsIndication->Buffer[0];
                    switch ( ComplianceTest.State ) {
                        case 0: // Check compliance test disable command (ii)
                            ComplianceTest.DownLinkCounter = 0;
                            ComplianceTest.Running = false;
                            tx_data.Port = 0;
                            set_lora_tx_dutycycle(0);

                            MibRequestConfirm_t mibReq;
                            mibReq.Type = MIB_ADR;
                            mibReq.Param.AdrEnable = lora_param.AdrEnable;
                            LoRaMacMibSetRequestConfirm( &mibReq );
#if defined(REGION_EU868)
                            lora_config_duty_cycle_set(LORAWAN_DUTYCYCLE_ON ? ENABLE : DISABLE);
#endif
                            break;
                        case 1: // (iii, iv)
                            tx_data.BuffSize = 2;
                            break;
                        case 2: // Enable confirmed messages (v)
                            ComplianceTest.IsTxConfirmed = true;
                            set_lora_tx_cfm_flag(ComplianceTest.IsTxConfirmed);
                            ComplianceTest.State = 1;
                            break;
                        case 3: // Disable confirmed messages (vi)
                            ComplianceTest.IsTxConfirmed = false;
                            set_lora_tx_cfm_flag(ComplianceTest.IsTxConfirmed);
                            ComplianceTest.State = 1;
                            break;
                        case 4: // (vii)
                            tx_data.BuffSize = mcpsIndication->BufferSize;

                            tx_data.Buff[0] = 4;
                            for ( uint8_t i = 1; i < tx_data.BuffSize; i++ ) {
                                tx_data.Buff[i] = (mcpsIndication->Buffer[i] + 1) % 256;
                            }
                            break;
                        case 5: { // (viii)
                            MlmeReq_t mlmeReq;
                            mlmeReq.Type = MLME_LINK_CHECK;
                            if (next_tx == true && LoRaMacMlmeRequest( &mlmeReq ) == LORAMAC_STATUS_OK) {
                                DBG_LINKWAN("start to send LinkCheckReq\r\n");
                            } else {
                                DBG_LINKWAN("fail to send LinkCheckReq\r\n");
                            }
                        }
                        break;
                        case 6: { // (ix)
                            // Disable TestMode and revert back to normal operation
                            ComplianceTest.DownLinkCounter = 0;
                            ComplianceTest.Running = false;
                            set_lora_tx_cfm_flag(1);
                            set_lora_tx_dutycycle(0);

                            MibRequestConfirm_t mibReq;
                            mibReq.Type = MIB_ADR;
                            mibReq.Param.AdrEnable = lora_param.AdrEnable;
                            LoRaMacMibSetRequestConfirm( &mibReq );
#if defined(REGION_EU868)
                            lora_config_duty_cycle_set(LORAWAN_DUTYCYCLE_ON ? ENABLE : DISABLE);
#endif
                            MibRequestConfirm_t mib_req;
                            mib_req.Type = MIB_NETWORK_ACTIVATION;
                            LoRaMacStatus_t status = LoRaMacMibGetRequestConfirm(&mib_req);
                            if (status == LORAMAC_STATUS_OK) {
                                if (mib_req.Param.NetworkActivation != ACTIVATION_TYPE_NONE) {
                                    mib_req.Type = MIB_NETWORK_ACTIVATION;
                                    mib_req.Param.NetworkActivation = ACTIVATION_TYPE_NONE;
                                    LoRaMacMibSetRequestConfirm(&mib_req);
                                }
                                DBG_LINKWAN("Rejoin again...\r");
                                reset_join_state();
                                g_join_method = DEF_JOIN_METHOD;
                                TimerStop(&TxNextPacketTimer);
                                TimerSetValue(&TxNextPacketTimer, gJoinInterval * 1000);
                                TimerStart(&TxNextPacketTimer);
                            }
                        }
                        break;
                        case 7: { // (x)

                            if (next_tx != true) {
                                DBG_LINKWAN("tx running, txcw can't be set!\r\n");
                                break;
                            }

                            if ( mcpsIndication->BufferSize == 3 ) {
                                MlmeReq_t mlmeReq;
                                mlmeReq.Type = MLME_TXCW;
                                mlmeReq.Req.TxCw.Timeout = (uint16_t)
                                                           ((mcpsIndication->Buffer[1] << 8) |
                                                            mcpsIndication->Buffer[2]);
                                LoRaMacMlmeRequest( &mlmeReq );
                            } else if ( mcpsIndication->BufferSize == 7 ) {
                                MlmeReq_t mlmeReq;
                                mlmeReq.Type = MLME_TXCW_1;
                                mlmeReq.Req.TxCw.Timeout = (uint16_t)
                                                           ((mcpsIndication->Buffer[1] << 8) |
                                                            mcpsIndication->Buffer[2]);
                                mlmeReq.Req.TxCw.Frequency =
                                    (uint32_t) ((mcpsIndication->Buffer[3] << 16) |
                                                (mcpsIndication->Buffer[4] << 8) |
                                                mcpsIndication->Buffer[5]) * 100;
                                mlmeReq.Req.TxCw.Power = mcpsIndication->Buffer[6];
                                LoRaMacMlmeRequest( &mlmeReq );
                            }
                            ComplianceTest.State = 1;
                        }
                        break;
                        case 9: { // Switch end device Class
                            DeviceClass_t class;

                            MibRequestConfirm_t mibReq;
                            mibReq.Type = MIB_DEVICE_CLASS;
                            // CLASS_A = 0, CLASS_B = 1, CLASS_C = 2
                            class = ( DeviceClass_t )mcpsIndication->Buffer[1];
                            mibReq.Param.Class = class;
                            set_lora_class(class);

                            LoRaMacStatus_t status = LoRaMacMibSetRequestConfirm(&mibReq);
                            if (status != LORAMAC_STATUS_OK) {
                                DBG_LINKWAN("STATUS error: %d\r\n", status);
                                break;
                            }

                            device_state = DEVICE_STATE_SEND;
                        }
                        break;
                        case 0x81: {
                            if (mcpsIndication->BufferSize == 2 && mcpsIndication->Buffer[1] != 0) {
                                ComplianceTest.uplink_cycle = mcpsIndication->Buffer[1];
                                set_lora_tx_dutycycle(ComplianceTest.uplink_cycle * 1000);
                                ComplianceTest.State = 1;
                            } else {
                                DBG_LINKWAN("param error\r\n");
                            }
                        }
                        break;
                        case 0x82: {
                            // Don't Disable TestMode
                            MibRequestConfirm_t mibReq;
                            mibReq.Type = MIB_ADR;
                            mibReq.Param.AdrEnable = lora_param.AdrEnable;
                            LoRaMacMibSetRequestConfirm( &mibReq );
#if defined(REGION_EU868)
                            lora_config_duty_cycle_set(LORAWAN_DUTYCYCLE_ON ? ENABLE : DISABLE);
#endif
                            MibRequestConfirm_t mib_req;
                            mib_req.Type = MIB_NETWORK_ACTIVATION;
                            LoRaMacStatus_t status = LoRaMacMibGetRequestConfirm(&mib_req);
                            if (status == LORAMAC_STATUS_OK) {
                                if (mib_req.Param.NetworkActivation != ACTIVATION_TYPE_NONE) {
                                    mib_req.Type = MIB_NETWORK_ACTIVATION;
                                    mib_req.Param.NetworkActivation = ACTIVATION_TYPE_NONE;
                                    LoRaMacMibSetRequestConfirm(&mib_req);
                                }
                                DBG_LINKWAN("Rejoin again...\r");
                                reset_join_state();
                                g_join_method = DEF_JOIN_METHOD;
                                TimerStop(&TxNextPacketTimer);
                                TimerSetValue(&TxNextPacketTimer, gJoinInterval * 1000);
                                TimerStart(&TxNextPacketTimer);
                                ComplianceTest.State = 1;
                            }
                        }
                        break;
                        case 0xf1: {
                            uint32_t rf_frequency;
                            if (device_state != DEVICE_STATE_SLEEP) {
                                DBG_LINKWAN("device state not sleep\r\n");
                                break;
                            }

                            if (mcpsIndication->BufferSize == 4) {
                                rf_frequency =
                                    (uint32_t) ((mcpsIndication->Buffer[1] << 16) |
                                                (mcpsIndication->Buffer[2] << 8) |
                                                mcpsIndication->Buffer[3]) * 100;
                            } else {
                                DBG_LINKWAN("param error: no rf frequency\r\n");
                                break;
                            }

                            extern RadioEvents_t *RadioEventsTest;

                            next_tx = false;
                            set_lora_tx_dutycycle(0);
                            RadioEventsTest->RxDone = OnRxDone;

                            Radio.SetChannel(rf_frequency);
                            DBG_LINKWAN("radio rx freq %d\r\n", (int)rf_frequency);
                            Radio.SetRxConfig(MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                                              LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                                              LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                                              0, true, 0, 0, LORA_IQ_INVERSION_ON, true);
                            Radio.Rx(0); // Continuous Rx
                        }

                        break;
                        case 0xf2: {
                            uint32_t rf_frequency;
                            if (mcpsIndication->BufferSize == 4) {
                                rf_frequency =
                                    (uint32_t) ((mcpsIndication->Buffer[1] << 16) |
                                                (mcpsIndication->Buffer[2] << 8) |
                                                mcpsIndication->Buffer[3]) * 100;
                            } else {
                                DBG_LINKWAN("param error: no rf frequency\r\n");
                                break;
                            }

                            set_lora_tx_dutycycle(0);
                            Radio.SetChannel(rf_frequency);
                            DBG_LINKWAN("radio tx freq %d\r\n", (int)rf_frequency);

                            Radio.SetTxConfig( MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
                                               LORA_SPREADING_FACTOR, LORA_CODINGRATE,
                                               LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                                               true, 0, 0, LORA_IQ_INVERSION_ON, 3000 );
                            Radio.Send( "PING", 4 );

                        }
                        break;
                        default:
                            break;
                    }
                }
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

    next_tx = true;
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
    uint8_t  freqband_num = 0;

    for (uint8_t i = 0; i < 16; i++) {
        if ((mask & (1 << i)) && i != 1) {
            freqband_num++;
        }
    }

    return freqband_num;
}

static void MlmeConfirm(MlmeConfirm_t *mlmeConfirm)
{
    uint32_t rejoin_delay;

    switch (mlmeConfirm->MlmeRequest) {
        case MLME_JOIN: {
            if (mlmeConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK) {
                // Status is OK, node has joined the network
                device_state = DEVICE_STATE_JOINED;

                join_trials_counter = 0;

                g_freqband_num      = 0;
            } else {
                // Join was not successful. Try to join again
                //reset_join_state();
                join_trials_counter++;

                // try to num_trials times
                if (join_trials_counter < num_trials) {
                    rejoin_delay = generate_rejoin_delay();
                } else {
                    join_trials_counter = 0;

                    if (g_join_method == STORED_JOIN_METHOD) {
                        g_join_method = DEF_JOIN_METHOD;

                        rejoin_delay = generate_rejoin_delay();
                    } else if (g_join_method == DEF_JOIN_METHOD) {
                        g_join_method = SCAN_JOIN_METHOD;

                        g_freqband_num = get_freqband_num();
                    }

                    if (g_join_method == SCAN_JOIN_METHOD) {
                        if (g_freqband_num > 0) {
                            g_freqband_num--;

                            rejoin_delay = generate_rejoin_delay();
                        } else {
                            g_join_method = DEF_JOIN_METHOD;
                            rejoin_delay  = 60 * 60 * 1000; // 1 hour

                            DBG_LINKWAN("Wait 1 hour for new round of scan\r\n");
                        }
                    }
                }

                TimerSetValue(&TxNextPacketTimer, rejoin_delay);
                TimerStart(&TxNextPacketTimer);
                rejoin_flag = false;
            }
            break;
        }
        case MLME_LINK_CHECK: {
            if (mlmeConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK) {
                // Check DemodMargin
                // Check NbGateways
                if ( ComplianceTest.Running == true ) {
                    ComplianceTest.LinkCheck = true;
                    ComplianceTest.DemodMargin = mlmeConfirm->DemodMargin;
                    ComplianceTest.NbGateways = mlmeConfirm->NbGateways;
                }
            }
            break;
        }
#ifdef LORAMAC_CLASSB_ENABLED
        case MLME_DEVICE_TIME: {
            device_state = DEVICE_STATE_BEACON_ACQUISITION;

            next_tx = true;
            break;
        }
        case MLME_BEACON_ACQUISITION:
        {
            if( mlmeConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK )
            {
                device_state = DEVICE_STATE_REQ_PINGSLOT_ACK;

                next_tx = true;
            }

            break;
        }
        case MLME_PING_SLOT_INFO:
        {
            MibRequestConfirm_t mibReq;

            if( mlmeConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK )
            {
                mibReq.Type = MIB_DEVICE_CLASS;
                mibReq.Param.Class = CLASS_B;
                LoRaMacMibSetRequestConfirm( &mibReq );

                DBG_LINKWAN( "\r\n\r\n###### ===== Switch to Class B done. ==== ######\r\n\r\n" );

                device_state = DEVICE_STATE_SEND;

                is_tx_confirmed = 0;
            }
            break;
        }
#endif
        default:
            break;
    }
    next_tx = true;
}

static void MlmeIndication(MlmeIndication_t *mlmeIndication)
{
    if (mlmeIndication->Status != LORAMAC_EVENT_INFO_STATUS_BEACON_LOCKED) {
        DBG_LINKWAN("STATUS:  %d\r\n", mlmeIndication->Status);
    }

    if (mlmeIndication->Status != LORAMAC_EVENT_INFO_STATUS_OK) {

    }

    switch (mlmeIndication->MlmeIndication) {
        case MLME_SCHEDULE_UPLINK: {
            on_tx_next_packet_timer_event();
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

int lora_compliance_test(uint8_t idx)
{
    McpsIndication_t mcpsIndication;
    char buffer[10] = {'\0'};

    memset(&mcpsIndication, '\0', sizeof(mcpsIndication));

    mcpsIndication.Status = LORAMAC_EVENT_INFO_STATUS_OK;
    mcpsIndication.McpsIndication = MCPS_UNCONFIRMED;
    mcpsIndication.RxData = true;
    mcpsIndication.Port = 224;
    mcpsIndication.Buffer = buffer;

    switch (idx) {
        case 0x0:
            mcpsIndication.BufferSize = 1;
            mcpsIndication.Buffer[0] = 0x0;
            break;

        case 0x1:
            mcpsIndication.BufferSize = 4;
            mcpsIndication.Buffer[0] = 0x01;
            mcpsIndication.Buffer[1] = 0x01;
            mcpsIndication.Buffer[2] = 0x01;
            mcpsIndication.Buffer[3] = 0x01;
            break;

        case 0x2:
            mcpsIndication.BufferSize = 1;
            mcpsIndication.Buffer[0] = 0x2;
            break;

        case 0x3:
            mcpsIndication.BufferSize = 1;
            mcpsIndication.Buffer[0] = 0x3;
            break;

        case 0x4:
            mcpsIndication.BufferSize = 4;
            mcpsIndication.Buffer[0] = 4;
            mcpsIndication.Buffer[1] = 256;
            mcpsIndication.Buffer[2] = 0x2;
            mcpsIndication.Buffer[3] = 0x3;
            break;

        case 0x5:
            mcpsIndication.BufferSize = 1;
            mcpsIndication.Buffer[0] = 5;
            break;

        case 0x6:
            mcpsIndication.BufferSize = 1;
            mcpsIndication.Buffer[0] = 6;
            break;

        case 0x7:
            mcpsIndication.BufferSize = 3;
            mcpsIndication.Buffer[0] = 7;
            mcpsIndication.Buffer[1] = 0;
            mcpsIndication.Buffer[2] = 0x8;
            break;

        case 0x80:
            mcpsIndication.BufferSize = 2;
            mcpsIndication.Buffer[0] = 0x80;
            mcpsIndication.Buffer[1] = 2;
            break;

        case 0xf0:
            mcpsIndication.BufferSize = 2;
            mcpsIndication.Buffer[0] = 0x80;
            mcpsIndication.Buffer[1] = 0;
            break;

        case 0x81:
            mcpsIndication.BufferSize = 2;
            mcpsIndication.Buffer[0] = 0x81;
            mcpsIndication.Buffer[1] = 15;
            break;

        case 0x82:
            break;

        case 0x83:
            mcpsIndication.BufferSize = 4;
            mcpsIndication.Buffer[0] = 0x83;
            mcpsIndication.Buffer[1] = 0x48;
            mcpsIndication.Buffer[2] = 0x01;
            mcpsIndication.Buffer[3] = 0x98;
            break;

        case 0x84:
            mcpsIndication.BufferSize = 4;
            mcpsIndication.Buffer[0] = 0x84;
            mcpsIndication.Buffer[1] = 0x48;
            mcpsIndication.Buffer[2] = 0x01;
            mcpsIndication.Buffer[3] = 0x98;
            break;

        default:
            return false;
    }

    McpsIndication(&mcpsIndication);
    return true;
}

void lora_init(LoRaMainCallback_t *callbacks, app_class_type_t class_type)
{
    device_state  = DEVICE_STATE_INIT;
    app_callbacks = callbacks;

    app_classType = class_type;

#ifdef AOS_COMP_KV
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

#ifdef AOS_COMP_KV
    len = sizeof(lora_dev);
    memset(&lora_dev, 0, sizeof(lora_dev));
    aos_kv_get("lora_dev", &lora_dev, &len);

    if (lora_dev.freqband >= 0) {
        if (0 == (lora_dev.mask & (1 << lora_dev.freqband))) {
            lora_dev.freqband = -1;
        }
    }

    len = sizeof(lora_abp_id);
    memset(&lora_abp_id, 0, sizeof(lora_abp_id));
    aos_kv_get("lora_abp", &lora_abp_id, &len);
#else
    memcpy1(&lora_dev, &g_lora_dev, sizeof(lora_dev_t));
#endif
#endif

    while (1) {
#ifdef CONFIG_LINKWAN_AT
        extern void process_linkwan_at(void);
        process_linkwan_at();
#endif

        if (Radio.IrqProcess != NULL) {
            Radio.IrqProcess();
        }

        LoRaMacProcess();

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
                LoRaMacPrimitives.MacMlmeIndication = MlmeIndication;
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

                app_classType = lora_dev.class;

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
                set_lora_app_port(2);

                LoRaMacStart();

                device_state = DEVICE_STATE_JOIN;
                break;
            }

            case DEVICE_STATE_JOIN: {
                MlmeReq_t mlmeReq;
                if (lora_param.JoinMode == JOIN_MODE_OTAA) {
                    mlmeReq.Type            = MLME_JOIN;
                    mlmeReq.Req.Join.DevEui = lora_dev.dev_eui;
                    mlmeReq.Req.Join.JoinEui = lora_dev.app_eui;

                    mibReq.Type         = MIB_APP_KEY;
                    mibReq.Param.AppKey = lora_dev.app_key;
                    LoRaMacMibSetRequestConfirm(&mibReq);

                    if (g_join_method == STORED_JOIN_METHOD) {
                        mlmeReq.Req.Join.Datarate = lora_dev.datarate;
                    } else {
                        mibReq.Type = MIB_CHANNELS_DEFAULT_DATARATE;

                        if (LORAMAC_STATUS_OK == LoRaMacMibGetRequestConfirm(&mibReq)) {
                            mlmeReq.Req.Join.Datarate = mibReq.Param.ChannelsDefaultDatarate;
                        }
                    }
                    if (next_tx == true && rejoin_flag == true) {
                        if (LoRaMacMlmeRequest(&mlmeReq) == LORAMAC_STATUS_OK) {
                            next_tx = false;
                        }
                        DBG_LINKWAN(
                          "Start to Join, method %d\r\n", g_join_method);
                    }
                    device_state = DEVICE_STATE_SLEEP;
                } else if (lora_param.JoinMode == JOIN_MODE_ABP) {
                    mibReq.Type        = MIB_NET_ID;
                    mibReq.Param.NetID = LORAWAN_NETWORK_ID;
                    LoRaMacMibSetRequestConfirm(&mibReq);

                    mibReq.Type          = MIB_DEV_ADDR;
                    mibReq.Param.DevAddr = g_lora_abp_id.devaddr;
                    LoRaMacMibSetRequestConfirm(&mibReq);
#if 0
                    mibReq.Type          = MIB_NWK_SKEY;
                    mibReq.Param.NwkSKey = g_lora_abp_id.nwkskey;
                    LoRaMacMibSetRequestConfirm(&mibReq);

                    mibReq.Type          = MIB_APP_SKEY;
                    mibReq.Param.AppSKey = g_lora_abp_id.appskey;
                    LoRaMacMibSetRequestConfirm(&mibReq);

                    mibReq.Type                  = MIB_NETWORK_JOINED;
                    mibReq.Param.IsNetworkJoined = true;
                    LoRaMacMibSetRequestConfirm(&mibReq);
#endif
                    device_state = DEVICE_STATE_SEND;
                }
                break;
            }
            case DEVICE_STATE_JOINED: {
                DBG_LINKWAN("Joined\n\r");
                store_lora_config();
#ifdef LORAMAC_CLASSB_ENABLED
                if (app_classType == LORA_APP_CLASS_B) {
                    device_state = DEVICE_STATE_REQ_DEVICE_TIME;
                } else {
                    device_state = DEVICE_STATE_SEND;
                }

                break;
            }
            case DEVICE_STATE_REQ_DEVICE_TIME: {
                MlmeReq_t mlmeReq;

                if( next_tx == true )
                {
                    mlmeReq.Type = MLME_DEVICE_TIME;

                    if( LoRaMacMlmeRequest( &mlmeReq ) == LORAMAC_STATUS_OK )
                    {
                        device_state = DEVICE_STATE_SEND;
                    }
                }
                break;
            }
            case DEVICE_STATE_BEACON_ACQUISITION: {
                MlmeReq_t mlmeReq;

                if( next_tx == true )
                {
                    mlmeReq.Type = MLME_BEACON_ACQUISITION;

                    LoRaMacMlmeRequest( &mlmeReq );

                    DBG_LINKWAN("Searching Beacon, wait ......");

                    next_tx = false;
                }
                device_state = DEVICE_STATE_SEND;
                break;
            }
            case DEVICE_STATE_REQ_PINGSLOT_ACK: {
                MlmeReq_t mlmeReq;

                if (true == next_tx) {
                    mlmeReq.Type = MLME_LINK_CHECK;
                    LoRaMacMlmeRequest(&mlmeReq);

                    mlmeReq.Type = MLME_PING_SLOT_INFO;
                    mlmeReq.Req.PingSlotInfo.PingSlot.Fields.Periodicity = gClassBPingPeriod;
                    mlmeReq.Req.PingSlotInfo.PingSlot.Fields.RFU = 0;

                    if (LORAMAC_STATUS_OK == LoRaMacMlmeRequest(&mlmeReq)) {
                        device_state = DEVICE_STATE_SEND;
                    }
                }
#else
            device_state = DEVICE_STATE_SEND;
#endif
                break;
            }
            case DEVICE_STATE_SEND: {
                DBG_LINKWAN("send state: %d\r\n", next_tx);
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
#ifdef AOS_COMP_KV
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
#ifdef AOS_COMP_KV
    aos_kv_set("lora_uldl_mode", &g_freq_mode, sizeof(g_freq_mode), 1);
#endif
    return true;
}

bool set_lora_tx_datarate(int8_t datarate)
{
    if (datarate >= CN470_TX_MIN_DATARATE &&
        datarate <= CN470_TX_MAX_DATARATE && get_lora_adr() == 0) {
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
    LoRaMacStatus_t     status;
    MibRequestConfirm_t mib_req;

    mib_req.Type = MIB_ADR;
    status = LoRaMacMibGetRequestConfirm(&mib_req);
    if ((status == LORAMAC_STATUS_OK) && (mib_req.Param.AdrEnable == true)) {
        return 1;
    }
    return 0;
}

static void start_dutycycle_timer(void)
{
    MibRequestConfirm_t mib_req;
    LoRaMacStatus_t     status;

    TimerStop(&TxNextPacketTimer);
    mib_req.Type = MIB_NETWORK_ACTIVATION;
    status       = LoRaMacMibGetRequestConfirm(&mib_req);
    if (status == LORAMAC_STATUS_OK) {
        if (mib_req.Param.NetworkActivation != ACTIVATION_TYPE_NONE &&
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

    mib_req.Type = MIB_NETWORK_ACTIVATION;
    status       = LoRaMacMibGetRequestConfirm(&mib_req);
    if (status == LORAMAC_STATUS_OK) {
        if (mib_req.Param.NetworkActivation != ACTIVATION_TYPE_NONE) {
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

    mib_req.Type = MIB_NETWORK_ACTIVATION;
    status       = LoRaMacMibGetRequestConfirm(&mib_req);
    if (status == LORAMAC_STATUS_OK) {
        if (mib_req.Param.NetworkActivation != ACTIVATION_TYPE_NONE) {
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

bool set_lora_freq_band(int8_t freqband)
{
    lora_dev_t lora_dev;

    read_lora_dev(&lora_dev);
    lora_dev.freqband = freqband;
    write_lora_dev(&lora_dev);
    return true;
}

void get_lora_freq_band(int8_t *freqband)
{
    lora_dev_t lora_dev;

    read_lora_dev(&lora_dev);
    *freqband = lora_dev.freqband;
}

bool set_lora_dev_eui(uint8_t *eui)
{
    lora_dev_t lora_dev;

    read_lora_dev(&lora_dev);
    memcpy1(lora_dev.dev_eui, eui, 8);
    write_lora_dev(&lora_dev);
    return true;
}

void get_lora_dev_eui(uint8_t *eui)
{
    lora_dev_t lora_dev;

    read_lora_dev(&lora_dev);
    memcpy1(eui, lora_dev.dev_eui, 8);
}

bool set_lora_app_eui(uint8_t *eui)
{
    lora_dev_t lora_dev;

    read_lora_dev(&lora_dev);
    memcpy1(lora_dev.app_eui, eui, 8);
    write_lora_dev(&lora_dev);
    return true;
}

void get_lora_app_eui(uint8_t *eui)
{
    lora_dev_t lora_dev;
    read_lora_dev(&lora_dev);
    memcpy1(eui, lora_dev.app_eui, 8);
}

bool set_lora_app_key(uint8_t *key)
{
    lora_dev_t lora_dev;

    read_lora_dev(&lora_dev);
    memcpy1(lora_dev.app_key, key, 16);
    write_lora_dev(&lora_dev);
    return true;
}

void get_lora_app_key(uint8_t *key)
{
    lora_dev_t lora_dev;

    read_lora_dev(&lora_dev);
    memcpy1(key, lora_dev.app_key, 16);
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

void get_lora_rssi(uint8_t band, int16_t *channel_rssi, int8_t *channel_snr)
{
    uint8_t    i;

    for (i = 0; i < 8; i++) {
        channel_rssi[i] = g_channel_rssi[i];
        channel_snr[i]  = g_channel_snr[i];
    }
}

void set_lora_rssi(uint8_t index, int16_t channel_rssi, int8_t channel_snr)
{
    g_channel_rssi[index] = channel_rssi;
    g_channel_snr[index]  = channel_snr;
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
    if ((mode != 0) && (mode != 1)) {
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

            device_state = DEVICE_STATE_SEND;

            return true;
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

void set_lora_devaddr(uint8_t *devaddr)
{
    memcpy1(g_lora_abp_id.devaddr, devaddr, 4);
    g_lora_abp_id.flag = VALID_LORA_CONFIG;
#ifdef AOS_COMP_KV
    aos_kv_set("lora_abp", &g_lora_abp_id, sizeof(g_lora_abp_id), 1);
#endif
}

void get_lora_devaddr(uint8_t *devaddr)
{
    int    len;
#ifdef AOS_COMP_KV
    aos_kv_get("lora_abp", &g_lora_abp_id, &len);
#endif
    memcpy1(devaddr, g_lora_abp_id.devaddr, 4);
}

void set_lora_appskey(uint8_t *buf)
{
    memcpy1(g_lora_abp_id.appskey, buf, 16);
    g_lora_abp_id.flag = VALID_LORA_CONFIG;
#ifdef AOS_COMP_KV
    aos_kv_set("lora_abp", &g_lora_abp_id, sizeof(g_lora_abp_id), 1);
#endif
}

void get_lora_appskey(uint8_t *buf)
{
    int    len;
#ifdef AOS_COMP_KV
    aos_kv_get("lora_abp", &g_lora_abp_id, &len);
#endif
    memcpy1(buf, g_lora_abp_id.appskey, 16);
}


void set_lora_nwkskey(uint8_t *buf)
{
    memcpy1(g_lora_abp_id.nwkskey, buf, 16);
    g_lora_abp_id.flag = VALID_LORA_CONFIG;
#ifdef AOS_COMP_KV
    aos_kv_set("lora_abp", &g_lora_abp_id, sizeof(g_lora_abp_id), 1);
#endif
}
void get_lora_nwkskey(uint8_t *buf)
{
    int      len;
#ifdef AOS_COMP_KV
    aos_kv_get("lora_abp", &g_lora_abp_id, &len);
#endif

    memcpy1(buf, g_lora_abp_id.nwkskey, 16);
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
        mib_req.Type                    = MIB_NETWORK_ACTIVATION;
        mib_req.Param.NetworkActivation = ACTIVATION_TYPE_NONE;
        status                          = LoRaMacMibSetRequestConfirm(&mib_req);

        if (status == LORAMAC_STATUS_OK) {
            device_state = DEVICE_STATE_INIT;
            rejoin_flag  = bAutoJoin;
            ret          = true;
        }
    } else if (bJoin == 1) {
        set_lora_tx_cfm_trials(joinRetryCnt);
        rejoin_flag = bAutoJoin;
        MibRequestConfirm_t mib_req;
        mib_req.Type           = MIB_NETWORK_ACTIVATION;
        LoRaMacStatus_t status = LoRaMacMibGetRequestConfirm(&mib_req);
        if (status == LORAMAC_STATUS_OK) {
            if (mib_req.Param.NetworkActivation != ACTIVATION_TYPE_NONE) {
                mib_req.Type                  = MIB_NETWORK_ACTIVATION;
                mib_req.Param.NetworkActivation = ACTIVATION_TYPE_NONE;
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

    mib_req.Type = MIB_NETWORK_ACTIVATION;
    status       = LoRaMacMibGetRequestConfirm(&mib_req);

    if (status == LORAMAC_STATUS_OK) {
        if (mib_req.Param.NetworkActivation != ACTIVATION_TYPE_NONE) {
            is_tx_confirmed = confirm;

            int i;
            char tmp_str[3] = {'\0'};
            for (i = 0; i < len / 2; i++) {
                memcpy1(tmp_str, payload + i * 2, 2);
                tx_data.Buff[i] = (int)strtol(tmp_str, NULL, 16);
            }

            tx_data.BuffSize = len / 2;

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

void set_classb_ping_period(uint8_t ping_period)
{
    device_state = DEVICE_STATE_REQ_PINGSLOT_ACK;
    gClassBPingPeriod = ping_period;

    next_tx = true;
}

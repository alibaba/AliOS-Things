#ifndef _SDIO_INTF_H_
#define _SDIO_INTF_H_

#include "tx_swdesc.h"

#define SDIO_INTF_DEBUG

#ifdef SDIO_INTF_DEBUG
#define SDIO_INTF_PRT       os_printf
#define SDIO_INTF_WPRT      warning_prf
#else
#define SDIO_INTF_PRT       os_null_printf
#define SDIO_INTF_WPRT      warning_prf
#endif

#define H2E_MSG_BUF_SIZE             31

#define WIFI_2_4_G_BAND              (0)
#define WIFI_5_0_G_BAND              (1)
#define WIFI_2_4_G_CHANNEL_NUM       (14)

#if CFG_SDIO
struct h2e_msg
{
    uint32_t dummy_word;                // used to cope with kernel message structure
    uint32_t msg[H2E_MSG_BUF_SIZE]; // body of the msg
};

#define E2A_MSG_PARAM_SIZE      256 // size in 4-byte words

/// Message structure for MSGs from Emb to App
typedef struct e2a_msg
{
    UINT16 id;                          ///< Message id.
    UINT16 dummy_dest_id;               ///<
    UINT16 dummy_src_id;                ///<
    UINT16 param_len;                   ///< Parameter embedded struct length.
    UINT16 param[E2A_MSG_PARAM_SIZE];  ///< Parameter embedded struct. Must be word-aligned.
    UINT16 pattern;                     ///< Used to stamp a valid MSG buffer
} E2A_MSG_S, *E2A_MSG_PTR;

#define SCANU_RET_PAYLOAD_LEN      128

typedef struct scanu_result_ind
{
    /// Length of the frame
    UINT16 length;
    /// Frame control field of the frame.
    UINT16 framectrl;
    /// Center frequency on which we received the packet
    UINT16 center_freq;
    /// PHY band
    UINT8 band;
    /// Index of the station that sent the frame. 0xFF if unknown.
    UINT8 sta_idx;
    /// Index of the VIF that received the frame. 0xFF if unknown.
    UINT8 inst_nbr;
    /// RSSI of the received frame.
    INT8 rssi;
    /// Frame payload.
    UINT32 payload[SCANU_RET_PAYLOAD_LEN];
} SCANU_RET_IND_S, *SCANU_RET_IND_PTR;


typedef struct probe_resp
{
    UINT8 timestamp[8];
    UINT16 beacon_int;
    UINT16 capab_info;

    /* followed by some of SSID, Supported rates,
     * FH Params, DS Params, CF Params, IBSS Params */
    UINT8 variable[1];
} __attribute__ ((packed)) probe_resp_s;

typedef struct ieee80211_probe_rsp
{
    UINT16 frame_control;
    UINT16 duration;
    UINT8 da[6];
    UINT8 sa[6];
    UINT8 bssid[6];
    UINT16 seq_ctrl;
    struct probe_resp rsp;
} __attribute__ ((packed)) IEEE80211_PROBE_RSP_S, *IEEE80211_PROBE_RSP_PTR;

#define STM32_PLATFORM

#ifdef STM32_PLATFORM

#define CMD_RET(cmd)			        (0x8000 | cmd)

/* Command codes */
#define SCMD_GET_HW_SPEC				0x0003
#define	SCMD_EEPROM_UPDATE				0x0004
#define SCMD_802_11_RESET				0x0005
#define	SCMD_802_11_SCAN				0x0006
#define SCMD_802_11_GET_LOG				0x000b
#define SCMD_MAC_MULTICAST_ADR			0x0010
#define SCMD_802_11_AUTHENTICATE		0x0011
#define SCMD_802_11_EEPROM_ACCESS		0x0059
#define SCMD_802_11_ASSOCIATE			0x0050
#define SCMD_802_11_SET_WEP				0x0013
#define SCMD_802_11_GET_STAT			0x0014
#define SCMD_802_3_GET_STAT				0x0015
#define SCMD_802_11_SNMP_MIB			0x0016
#define SCMD_MAC_REG_MAP				0x0017
#define SCMD_BBP_REG_MAP				0x0018
#define SCMD_MAC_REG_ACCESS				0x0019
#define SCMD_BBP_REG_ACCESS				0x001a
#define SCMD_RF_REG_ACCESS				0x001b
#define SCMD_802_11_RADIO_CONTROL		0x001c
#define SCMD_802_11_RF_CHANNEL			0x001d
#define SCMD_802_11_RF_TX_POWER			0x001e
#define SCMD_802_11_RSSI				0x001f
#define SCMD_802_11_RF_ANTENNA			0x0020
#define SCMD_802_11_PS_MODE				0x0021
#define SCMD_802_11_DATA_RATE			0x0022
#define SCMD_RF_REG_MAP					0x0023
#define SCMD_802_11_DEAUTHENTICATE		0x0024
#define SCMD_802_11_REASSOCIATE			0x0025
#define SCMD_MAC_CONTROL				0x0028
#define SCMD_802_11_AD_HOC_START		0x002b
#define SCMD_802_11_AD_HOC_JOIN			0x002c
#define SCMD_802_11_QUERY_TKIP_REPLY_CNTRS	0x002e
#define SCMD_802_11_ENABLE_RSN			0x002f
#define SCMD_802_11_SET_AFC				0x003c
#define SCMD_802_11_GET_AFC				0x003d
#define SCMD_802_11_AD_HOC_STOP			0x0040
#define SCMD_802_11_HOST_SLEEP_CFG		0x0043
#define SCMD_802_11_WAKEUP_CONFIRM		0x0044
#define SCMD_802_11_HOST_SLEEP_ACTIVATE	0x0045
#define SCMD_802_11_BEACON_STOP			0x0049
#define SCMD_802_11_MAC_ADDRESS			0x004d
#define SCMD_802_11_LED_GPIO_CTRL		0x004e
#define SCMD_802_11_EEPROM_ACCESS		0x0059
#define SCMD_802_11_BAND_CONFIG			0x0058
#define SCMD_GSPI_BUS_CONFIG			0x005a
#define SCMD_802_11D_DOMAIN_INFO		0x005b
#define SCMD_802_11_KEY_MATERIAL		0x005e
#define SCMD_802_11_SLEEP_PARAMS		0x0066
#define SCMD_802_11_INACTIVITY_TIMEOUT	0x0067
#define SCMD_802_11_SLEEP_PERIOD		0x0068
#define SCMD_802_11_TPC_CFG				0x0072
#define SCMD_802_11_PA_CFG				0x0073
#define SCMD_802_11_FW_WAKE_METHOD		0x0074
#define SCMD_802_11_SUBSCRIBE_EVENT		0x0075
#define SCMD_802_11_RATE_ADAPT_RATESET	0x0076
#define SCMD_802_11_TX_RATE_QUERY		0x007f
#define	SCMD_GET_TSF					0x0080
#define SCMD_BT_ACCESS					0x0087
#define SCMD_FWT_ACCESS					0x0095
#define SCMD_802_11_MONITOR_MODE		0x0098
#define SCMD_MESH_ACCESS				0x009b
#define SCMD_MESH_CONFIG_OLD			0x00a3
#define SCMD_MESH_CONFIG				0x00ac
#define	SCMD_SET_BOOT2_VER				0x00a5
#define	SCMD_FUNC_INIT					0x00a9
#define	SCMD_FUNC_SHUTDOWN				0x00aa
#define SCMD_802_11_BEACON_CTRL			0x00b0

#define SCMD_RESET_REQ                  0x00b1
#define SCMD_ME_CONFIG_REQ              0x00b2
#define SCMD_ME_CHAN_CONFIG_REQ         0x00b3
#define SCMD_START_REQ                  0x00b4
#define SCMD_ADD_IF_REQ                 0x00b5
#define SCMD_SET_PORT_REQ               0x00b6

#define SCMD_APM_START                  0x00b7
#define SCMD_BCN_CHANGE                 0x00b8
#define SCMD_MGMT_TX_REQ                0x00b9
#define SCMD_ADD_STA_REQ                0x00ba
#define SCMD_ADD_WEP_KEY                0x00bb

/* For the IEEE Power Save */
#define SCMD_SUBSCMD_ENTER_PS			0x0030
#define SCMD_SUBSCMD_EXIT_PS			0x0031
#define SCMD_SUBSCMD_SLEEP_CONFIRMED	0x0034
#define SCMD_SUBSCMD_FULL_POWERDOWN		0x0035
#define SCMD_SUBSCMD_FULL_POWERUP		0x0036

#define SCMD_ENABLE_RSN					0x0001
#define SCMD_DISABLE_RSN				0x0000

#define SCMD_ACT_GET					0x0000
#define SCMD_ACT_SET					0x0001
#define SCMD_ACT_GET_AES				0x0002
#define SCMD_ACT_SET_AES				0x0003
#define SCMD_ACT_REMOVE_AES				0x0004

/* Define action or option for SCMD_802_11_SET_WEP */
#define SCMD_ACT_ADD					0x0002
#define SCMD_ACT_REMOVE					0x0004
#define SCMD_ACT_USE_DEFAULT			0x0008

#define SCMD_TYPE_WEP_40_BIT			0x01
#define SCMD_TYPE_WEP_104_BIT			0x02

#define SCMD_NUM_OF_WEP_KEYS			4

#define SCMD_WEP_KEY_INDEX_MASK			0x3fff

/* Define action or option for SCMD_802_11_RESET */
#define SCMD_ACT_HALT					0x0003

/* Define action or option for SCMD_802_11_SCAN */
#define SCMD_BSS_TYPE_BSS				0x0001
#define SCMD_BSS_TYPE_IBSS				0x0002
#define SCMD_BSS_TYPE_ANY				0x0003

/* Define action or option for SCMD_802_11_SCAN */
#define SCMD_SCAN_TYPE_ACTIVE			0x0000
#define SCMD_SCAN_TYPE_PASSIVE			0x0001

#define SCMD_SCAN_RADIO_TYPE_BG			0

#define	SCMD_SCAN_PROBE_DELAY_TIME		0

/* Define action or option for SCMD_MAC_CONTROL */
#define SCMD_ACT_MAC_RX_ON				0x0001
#define SCMD_ACT_MAC_TX_ON				0x0002
#define SCMD_ACT_MAC_LOOPBACK_ON		0x0004
#define SCMD_ACT_MAC_WEP_ENABLE			0x0008
#define SCMD_ACT_MAC_INT_ENABLE			0x0010
#define SCMD_ACT_MAC_MULTICAST_ENABLE	0x0020
#define SCMD_ACT_MAC_BROADCAST_ENABLE			0x0040
#define SCMD_ACT_MAC_PROMISCUOUS_ENABLE			0x0080
#define SCMD_ACT_MAC_ALL_MULTICAST_ENABLE		0x0100
#define SCMD_ACT_MAC_STRICT_PROTECTION_ENABLE	0x0400

/* Event flags for SCMD_802_11_SUBSCRIBE_EVENT */
#define SCMD_SUBSCRIBE_RSSI_LOW		0x0001
#define SCMD_SUBSCRIBE_SNR_LOW		0x0002
#define SCMD_SUBSCRIBE_FAILCOUNT	0x0004
#define SCMD_SUBSCRIBE_BCNMISS		0x0008
#define SCMD_SUBSCRIBE_RSSI_HIGH	0x0010
#define SCMD_SUBSCRIBE_SNR_HIGH		0x0020

#pragma pack(1)

#define PROPRIETARY_TLV_BASE_ID		0x0100
#define MRVL_TERMINATE_TLV_ID		0xffff

#define TLV_TYPE_SSID				0x0000
#define TLV_TYPE_RATES				0x0001
#define TLV_TYPE_PHY_FH				0x0002
#define TLV_TYPE_PHY_DS				0x0003
#define TLV_TYPE_CF				    0x0004
#define TLV_TYPE_IBSS				0x0006

#define TLV_TYPE_DOMAIN				0x0007

#define TLV_TYPE_POWER_CAPABILITY	0x0021

#define TLV_TYPE_KEY_MATERIAL       (PROPRIETARY_TLV_BASE_ID + 0)
#define TLV_TYPE_CHANLIST           (PROPRIETARY_TLV_BASE_ID + 1)
#define TLV_TYPE_NUMPROBES          (PROPRIETARY_TLV_BASE_ID + 2)
#define TLV_TYPE_RSSI_LOW           (PROPRIETARY_TLV_BASE_ID + 4)
#define TLV_TYPE_SNR_LOW            (PROPRIETARY_TLV_BASE_ID + 5)
#define TLV_TYPE_FAILCOUNT          (PROPRIETARY_TLV_BASE_ID + 6)
#define TLV_TYPE_BCNMISS            (PROPRIETARY_TLV_BASE_ID + 7)
#define TLV_TYPE_LED_GPIO           (PROPRIETARY_TLV_BASE_ID + 8)
#define TLV_TYPE_LEDBEHAVIOR        (PROPRIETARY_TLV_BASE_ID + 9)
#define TLV_TYPE_PASSTHROUGH        (PROPRIETARY_TLV_BASE_ID + 10)
#define TLV_TYPE_REASSOCAP          (PROPRIETARY_TLV_BASE_ID + 11)
#define TLV_TYPE_POWER_TBL_2_4GHZ   (PROPRIETARY_TLV_BASE_ID + 12)
#define TLV_TYPE_POWER_TBL_5GHZ     (PROPRIETARY_TLV_BASE_ID + 13)
#define TLV_TYPE_BCASTPROBE	    (PROPRIETARY_TLV_BASE_ID + 14)
#define TLV_TYPE_NUMSSID_PROBE	    (PROPRIETARY_TLV_BASE_ID + 15)
#define TLV_TYPE_WMMQSTATUS   	    (PROPRIETARY_TLV_BASE_ID + 16)
#define TLV_TYPE_CRYPTO_DATA	    (PROPRIETARY_TLV_BASE_ID + 17)
#define TLV_TYPE_WILDCARDSSID	    (PROPRIETARY_TLV_BASE_ID + 18)
#define TLV_TYPE_TSFTIMESTAMP	    (PROPRIETARY_TLV_BASE_ID + 19)
#define TLV_TYPE_RSSI_HIGH          (PROPRIETARY_TLV_BASE_ID + 22)
#define TLV_TYPE_SNR_HIGH           (PROPRIETARY_TLV_BASE_ID + 23)
#define TLV_TYPE_AUTH_TYPE          (PROPRIETARY_TLV_BASE_ID + 31)
#define TLV_TYPE_MESH_ID            (PROPRIETARY_TLV_BASE_ID + 37)
#define TLV_TYPE_OLD_MESH_ID        (PROPRIETARY_TLV_BASE_ID + 291)

typedef struct mrvl_ie_header
{
    UINT16 type;
    UINT16 len;
} __attribute__((packed)) MRVL_IE_HDR_S, *MRVL_IE_HDR_PTR;


typedef struct mrvl_ie_ssid_param_set
{
    struct mrvl_ie_header header;
    UINT8 ssid[1];
} __attribute__((packed)) MRVL_IE_SSID_PARAM_S, *MRVL_IE_SSID_PARAM_PTR;

typedef struct chanscanmode
{
    UINT8 passivescan: 1;
    UINT8 disablechanfilt: 1;
    UINT8 reserved_2_7: 6;
} __attribute__((packed))CHAN_SCAN_MODE_S, *CHAN_SCAN_MODE_PTR;

typedef struct chanscanparamset
{
    UINT8 radiotype;
    UINT8 channumber;
    CHAN_SCAN_MODE_S chanscanmode;
    UINT16 minscantime;
    UINT16 maxscantime;
} __attribute__((packed)) CHAN_SCAN_PARAM_S, *CHAN_SCAN_PARAM_PTR;

typedef struct stm32_cmd_hdr
{
    UINT16 command;
    UINT16 size;
    UINT16 seqnum;
    UINT16 result;
} __attribute__((packed)) STM32_CMD_HDR_S, *STM32_CMD_HDR_PTR;

/* 802.11-related definitions */
/* TxPD descriptor */
typedef struct stm32_txpd
{
    /* union to cope up with later FW revisions */
    union
    {
        /* Current Tx packet status */
        UINT32 tx_status;
        struct
        {
            /* BSS type: client, AP, etc. */
            UINT8 bss_type;
            /* BSS number */
            UINT8 bss_num;
            /* Reserved */
            UINT16 reserved;
        } __attribute__((packed)) bss;

    } __attribute__((packed)) u ;

    /* Tx control */
    UINT32 tx_control;
    UINT32 tx_packet_location;
    /* Tx packet length */
    UINT16 tx_packet_length;
    /* First 2 byte of destination MAC address */
    UINT8 tx_dest_addr_high[2];
    /* Last 4 byte of destination MAC address */
    UINT8 tx_dest_addr_low[4];
    /* Pkt Priority */
    UINT8 priority;
    /* Pkt Trasnit Power control */
    UINT8 powermgmt;
    /* Amount of time the packet has been queued in the driver (units = 2ms) */
    UINT8 pktdelay_2ms;
    /* reserved */
    UINT8 reserved1;
} __attribute__((packed)) STM32_TXPD_S, *STM32_TXPD_PTR;

/* RxPD Descriptor */
typedef struct stm32_rxpd
{
    /* union to cope up with later FW revisions */
    union
    {
        /* Current Rx packet status */
        UINT16 status;
        struct
        {
            /* BSS type: client, AP, etc. */
            UINT8 bss_type;
            /* BSS number */
            UINT8 bss_num;
        } __attribute__((packed)) bss;
    } __attribute__((packed)) u;

    /* SNR */
    UINT8 snr;
    /* Tx control */
    UINT8 rx_control;
    /* Pkt length */
    UINT16 pkt_len;
    /* Noise Floor */
    UINT8 nf;
    /* Rx Packet Rate */
    UINT8 rx_rate;
    /* Pkt addr */
    UINT32 pkt_ptr;
    /* Next Rx RxPD addr */
    UINT32 next_rxpd_ptr;
    /* Pkt Priority */
    UINT8 priority;
    UINT8 reserved[3];
} __attribute__((packed)) STM32_RXPD_S, *STM32_RXPD_PTR;

enum stm32_ms_type
{
    MVMS_DAT = 0,
    MVMS_CMD = 1,
    MVMS_TXDONE = 2,
    MVMS_EVENT
};

#define BEACON_TIMESTAMP_LEN       8
#define ETH_ALEN	               6		/* Octets in one ethernet addr	 */

typedef struct ethhdr
{
    UINT8 h_dest[ETH_ALEN];
    UINT8 h_src[ETH_ALEN];
    UINT16 h_proto;
} __attribute__((packed)) ETHHDR_S, *ETHHDR_PTR;

typedef struct beacon_info
{
    UINT16 len;
    UINT8 bssid[ETH_ALEN];
    INT8 rssi;
    UINT8 timestamp[BEACON_TIMESTAMP_LEN];
    UINT16 beacon_interval;
    UINT16 capability;
} __attribute__((packed)) BEACON_INFO_S, *BEACON_INFO_PTR;

typedef struct cmd_ds_802_11_scan_rsp
{
    STM32_CMD_HDR_S hdr;

    UINT16 bssdescriptsize;
    UINT8 nr_sets;
    BEACON_INFO_S beacon_info;
    UINT8 tlvbuffer[1];
} __attribute__((packed)) STM32_SCAN_RSP_S, *STM32_SCAN_RSP_PTR;


typedef struct _stm32_frame_hdr
{
    UINT16 len;
#if FOR_SDIO_BLK_512
    UINT8 type;
    UINT8 seq;
#else
    UINT16 type;
#endif


} STM32_FRAME_HDR;

typedef struct _stm32_tx_frame_
{
    UINT16 len;
#if FOR_SDIO_BLK_512
    UINT8 type;
    UINT8 seq;
#else
    UINT16 type;
#endif

    STM32_TXPD_S frm;

    void *private;
} STM32_TX_FRAME;

typedef struct
{
    UINT16 len;
    UINT16 type;

    STM32_CMD_HDR_S hdr;

    UINT32 resv;
} STM32_CMD_S, STM32_RSP_S;

typedef struct
{
    UINT16 len;
    UINT16 type;

    STM32_CMD_HDR_S hdr;

    UINT16 private;
} STM32_TX_MGMT_S, *STM32_TX_MGMT_PTR;

typedef struct stm32_cmd_802_11_scan
{
    STM32_CMD_HDR_S hdr;

    UINT8 bsstype;
    UINT8 bssid[ETH_ALEN];
    UINT8 tlvbuffer[1];
} __attribute__((packed)) STM32_CMD_802_11_SCAN_S, *STM32_CMD_802_11_SCAN_PTR;

struct MrvlIEtype_keyParamSet
{
    /* type ID */
    UINT16 type;

    /* length of Payload */
    UINT16 length;

    /* type of key: WEP=0, TKIP=1, AES=2 */
    UINT16 keytypeid;

    /* key control Info specific to a keytypeid */
    UINT16 keyinfo;

    /* length of key */
    UINT16 keylen;

    /* key material of size keylen */
    UINT8 key[32];
} __attribute__((packed));

typedef struct cmd_ds_802_11_key_material
{
    STM32_CMD_HDR_S hdr;

    UINT16 action;
    struct MrvlIEtype_keyParamSet keyParamSet[2];
} __attribute__((packed)) STM32_KEY_MATERIAL_S, *STM32_KEY_MATERIAL_PTR;

typedef struct cmd_ds_802_11_associate_response
{
    STM32_CMD_HDR_S hdr;

    UINT16 capability;
    UINT16 statuscode;
    UINT16 aid;
    UINT8 iebuf[512];
} __attribute__((packed)) ASSOC_RESP_S, *ASSOC_RESP_PTR;

typedef struct cmd_ds_802_11_associate
{
    STM32_CMD_HDR_S hdr;

    UINT8 bssid[6];
    UINT16 capability;
    UINT16 listeninterval;
    UINT16 bcnperiod;
    UINT8 dtimperiod;
    UINT8 iebuf[512];    /* Enough for required and most optional IEs */
} __attribute__((packed)) ASSOC_REQ_S, *ASSOC_REQ_PTR;

#pragma pack()
#endif // STM32_PLATFORM


/*******************************************************************************
* Function Declarations
*******************************************************************************/
extern void sdio_emb_txdesc_copy(struct txdesc *dst_local, ETHHDR_PTR eth_hdr_ptr);
extern void sdio_rxed_trigger_evt(void);
#endif // CFG_SDIO

#endif // _SDIO_INTF_H_


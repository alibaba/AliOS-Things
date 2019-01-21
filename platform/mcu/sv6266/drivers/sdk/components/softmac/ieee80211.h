/*
 * Copyright (c) 2007, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 */

/**
 * \file
 *         A MAC protocol implementation that does not do anything.
 * \author
 *         Adam Dunkels <adam@sics.se>
 */

#ifndef __IEEE80211_H__
#define __IEEE80211_H__

#include "soc_types.h"

#define MAC_ADDR_LEN            6
#define ETH_HLEN	14		/* Total octets in header.	 */
#define ETH_ZLEN	60		/* Min. octets in frame sans FCS */
#define ETH_DATA_LEN	1500		/* Max. octets in payload	 */
#define ETH_FRAME_LEN	1514		/* Max. octets in frame sans FCS */
#define ETH_FCS_LEN	4		/* Octets in the FCS		 */


#define UIP_ETHTYPE_ARP  0x0806
#define UIP_ETHTYPE_IP   0x0800
#define UIP_ETHTYPE_IPV6 0x86dd
#define UIP_ETHTYPE_EAPOL 0x888E	/* 802.1X EAP over LANs */
/*
 * DS bit usage
 *
 * TA = transmitter address
 * RA = receiver address
 * DA = destination address
 * SA = source address
 *
 * ToDS    FromDS  A1(RA)  A2(TA)  A3      A4      Use
 * -----------------------------------------------------------------
 *  0       0       DA      SA      BSSID   -       IBSS/DLS
 *  0       1       DA      BSSID   SA      -       AP -> STA
 *  1       0       BSSID   SA      DA      -       AP <- STA
 *  1       1       RA      TA      DA      SA      unspecified (WDS)
 */

#define IEEE80211_LEN	24
#define ETHERNET_OFFSET	18
#define FCS_LEN 			4

#define IEEE80211_FCTL_VERS		0x0003
#define IEEE80211_FCTL_FTYPE		0x000c
#define IEEE80211_FCTL_STYPE		0x00f0
#define IEEE80211_FCTL_TODS		0x0100
#define IEEE80211_FCTL_FROMDS		0x0200
#define IEEE80211_FCTL_MOREFRAGS	0x0400
#define IEEE80211_FCTL_RETRY		0x0800
#define IEEE80211_FCTL_PM		0x1000
#define IEEE80211_FCTL_MOREDATA		0x2000
#define IEEE80211_FCTL_PROTECTED	0x4000
#define IEEE80211_FCTL_ORDER		0x8000
#define IEEE80211_FCTL_CTL_EXT		0x0f00

#define IEEE80211_SCTL_FRAG		0x000F
#define IEEE80211_SCTL_SEQ		0xFFF0

#define IEEE80211_FTYPE_MGMT		0x0000
#define IEEE80211_FTYPE_CTL		0x0004
#define IEEE80211_FTYPE_DATA		0x0008
#define IEEE80211_FTYPE_EXT		0x000c

/* management */
#define IEEE80211_STYPE_ASSOC_REQ	0x0000
#define IEEE80211_STYPE_ASSOC_RESP	0x0010
#define IEEE80211_STYPE_REASSOC_REQ	0x0020
#define IEEE80211_STYPE_REASSOC_RESP	0x0030
#define IEEE80211_STYPE_PROBE_REQ	0x0040
#define IEEE80211_STYPE_PROBE_RESP	0x0050
#define IEEE80211_STYPE_BEACON		0x0080
#define IEEE80211_STYPE_ATIM		0x0090
#define IEEE80211_STYPE_DISASSOC	0x00A0
#define IEEE80211_STYPE_AUTH		0x00B0
#define IEEE80211_STYPE_DEAUTH		0x00C0
#define IEEE80211_STYPE_ACTION		0x00D0

/* control */
#define IEEE80211_STYPE_CTL_EXT		0x0060
#define IEEE80211_STYPE_BACK_REQ	0x0080
#define IEEE80211_STYPE_BACK		0x0090
#define IEEE80211_STYPE_PSPOLL		0x00A0
#define IEEE80211_STYPE_RTS		0x00B0
#define IEEE80211_STYPE_CTS		0x00C0
#define IEEE80211_STYPE_ACK		0x00D0
#define IEEE80211_STYPE_CFEND		0x00E0
#define IEEE80211_STYPE_CFENDACK	0x00F0

/* data */
#define IEEE80211_STYPE_DATA			0x0000
#define IEEE80211_STYPE_DATA_CFACK		0x0010
#define IEEE80211_STYPE_DATA_CFPOLL		0x0020
#define IEEE80211_STYPE_DATA_CFACKPOLL		0x0030
#define IEEE80211_STYPE_NULLFUNC		0x0040
#define IEEE80211_STYPE_CFACK			0x0050
#define IEEE80211_STYPE_CFPOLL			0x0060
#define IEEE80211_STYPE_CFACKPOLL		0x0070
#define IEEE80211_STYPE_QOS_DATA		0x0080
#define IEEE80211_STYPE_QOS_DATA_CFACK		0x0090
#define IEEE80211_STYPE_QOS_DATA_CFPOLL		0x00A0
#define IEEE80211_STYPE_QOS_DATA_CFACKPOLL	0x00B0
#define IEEE80211_STYPE_QOS_NULLFUNC		0x00C0
#define IEEE80211_STYPE_QOS_CFACK		0x00D0
#define IEEE80211_STYPE_QOS_CFPOLL		0x00E0
#define IEEE80211_STYPE_QOS_CFACKPOLL		0x00F0

/* extension, added by 802.11ad */
#define IEEE80211_STYPE_DMG_BEACON		0x0000

/* control extension - for IEEE80211_FTYPE_CTL | IEEE80211_STYPE_CTL_EXT */
#define IEEE80211_CTL_EXT_POLL		0x2000
#define IEEE80211_CTL_EXT_SPR		0x3000
#define IEEE80211_CTL_EXT_GRANT	0x4000
#define IEEE80211_CTL_EXT_DMG_CTS	0x5000
#define IEEE80211_CTL_EXT_DMG_DTS	0x6000
#define IEEE80211_CTL_EXT_SSW		0x8000
#define IEEE80211_CTL_EXT_SSW_FBACK	0x9000
#define IEEE80211_CTL_EXT_SSW_ACK	0xa000

/* miscellaneous IEEE 802.11 constants */
#define IEEE80211_MAX_FRAG_THRESHOLD	2352
#define IEEE80211_MAX_RTS_THRESHOLD	2353
#define IEEE80211_MAX_AID		2007
#define IEEE80211_MAX_TIM_LEN		251
/* Maximum size for the MA-UNITDATA primitive, 802.11 standard section
   6.2.1.1.2.

   802.11e clarifies the figure in section 7.1.2. The frame body is
   up to 2304 octets long (maximum MSDU size) plus any crypt overhead. */
#define IEEE80211_MAX_DATA_LEN		2304
/* 30 byte 4 addr hdr, 2 byte QoS, 2304 byte MSDU, 12 byte crypt, 4 byte FCS */
#define IEEE80211_MAX_FRAME_LEN		2352

#define IEEE80211_MAX_SSID_LEN		32

#define IEEE80211_MAX_MESH_ID_LEN	32

#define IEEE80211_QOS_CTL_LEN		2
/* 1d tag mask */
#define IEEE80211_QOS_CTL_TAG1D_MASK		0x0007
/* TID mask */
#define IEEE80211_QOS_CTL_TID_MASK		0x000f
/* EOSP */
#define IEEE80211_QOS_CTL_EOSP			0x0010
/* ACK policy */
#define IEEE80211_QOS_CTL_ACK_POLICY_NORMAL	0x0000
#define IEEE80211_QOS_CTL_ACK_POLICY_NOACK	0x0020
#define IEEE80211_QOS_CTL_ACK_POLICY_NO_EXPL	0x0040
#define IEEE80211_QOS_CTL_ACK_POLICY_BLOCKACK	0x0060
#define IEEE80211_QOS_CTL_ACK_POLICY_MASK	0x0060
/* A-MSDU 802.11n */
#define IEEE80211_QOS_CTL_A_MSDU_PRESENT	0x0080
/* Mesh Control 802.11s */
#define IEEE80211_QOS_CTL_MESH_CONTROL_PRESENT  0x0100

/* U-APSD queue for WMM IEs sent by AP */
#define IEEE80211_WMM_IE_AP_QOSINFO_UAPSD	(1<<7)
#define IEEE80211_WMM_IE_AP_QOSINFO_PARAM_SET_CNT_MASK	0x0f

/* U-APSD queues for WMM IEs sent by STA */
#define IEEE80211_WMM_IE_STA_QOSINFO_AC_VO	(1<<0)
#define IEEE80211_WMM_IE_STA_QOSINFO_AC_VI	(1<<1)
#define IEEE80211_WMM_IE_STA_QOSINFO_AC_BK	(1<<2)
#define IEEE80211_WMM_IE_STA_QOSINFO_AC_BE	(1<<3)
#define IEEE80211_WMM_IE_STA_QOSINFO_AC_MASK	0x0f

/* U-APSD max SP length for WMM IEs sent by STA */
#define IEEE80211_WMM_IE_STA_QOSINFO_SP_ALL	0x00
#define IEEE80211_WMM_IE_STA_QOSINFO_SP_2	0x01
#define IEEE80211_WMM_IE_STA_QOSINFO_SP_4	0x02
#define IEEE80211_WMM_IE_STA_QOSINFO_SP_6	0x03
#define IEEE80211_WMM_IE_STA_QOSINFO_SP_MASK	0x03
#define IEEE80211_WMM_IE_STA_QOSINFO_SP_SHIFT	5

#define IEEE80211_HT_CTL_LEN		4



/* 802.11n HT capability MSC set */
#define IEEE80211_HT_MCS_RX_HIGHEST_MASK	0x3ff
#define IEEE80211_HT_MCS_TX_DEFINED		0x01
#define IEEE80211_HT_MCS_TX_RX_DIFF		0x02
/* value 0 == 1 stream etc */
#define IEEE80211_HT_MCS_TX_MAX_STREAMS_MASK	0x0C
#define IEEE80211_HT_MCS_TX_MAX_STREAMS_SHIFT	2
#define	IEEE80211_HT_MCS_TX_MAX_STREAMS	4
#define IEEE80211_HT_MCS_TX_UNEQUAL_MODULATION	0x10


/* 802.11 BAR control masks */
#define IEEE80211_BAR_CTRL_ACK_POLICY_NORMAL	0x0000
#define IEEE80211_BAR_CTRL_MULTI_TID		0x0002
#define IEEE80211_BAR_CTRL_CBMTID_COMPRESSED_BA	0x0004
#define IEEE80211_BAR_CTRL_TID_INFO_MASK	0xf000
#define IEEE80211_BAR_CTRL_TID_INFO_SHIFT	12

#define IEEE80211_HT_MCS_MASK_LEN		10

#define IEEE80211_MAX_SSID_LEN		32

#define IEEE80211_MAX_SUPP_RATES 32

#define HT_CAP_INFO_LDPC_CODING_CAP             (0x1 << 0)
#define HT_CAP_INFO_SUPP_CHANNEL_WIDTH_SET      (0x1 << 1)
#define HT_CAP_INFO_SMPS_MASK                   (0x3 << 2)
#define HT_CAP_INFO_SMPS_DISABLED               (0x3 << 2)
#define HT_CAP_INFO_GREEN_FIELD                 (0x1 << 4)
#define HT_CAP_INFO_SHORT_GI20MHZ               (0x1 << 5)
#define HT_CAP_INFO_SHORT_GI40MHZ               (0x1 << 6)
#define HT_CAP_INFO_TX_STBC                     (0x1 << 7)
#define HT_CAP_INFO_RX_STBC_MASK                (0x3 << 8)
#define HT_CAP_INFO_DELAYED_BA                  (0x1 << 10)
#define HT_CAP_INFO_MAX_AMSDU_SIZE              (0x1 << 11)
#define HT_CAP_INFO_DSSS_CCK40MHZ               (0x1 << 12)
/* B13 - Reserved (was PSMP support during P802.11n development) */
#define HT_CAP_INFO_40MHZ_INTOLERANT            (0x1 << 14)
#define HT_CAP_INFO_LSIG_TXOP_PROTECT_SUPPORT   (0x1 << 15)
#define HT_CAP_INFO_RX_STBC_ONESTR              (0x1 << 8)

#define MIN_AVAIL_CHANNEL	1
#define MAX_AVAIL_2GCHANNEL	14
#define MAX_AVAIL_CHANNEL	38
#define CHANNELAVALIBLE 0x1FFF //China available channel
//#define CHANNELAVAILIBLE 0x7WFF //Taiwan available channel
#define MAX_SSID_LEN	32
#define RSSI_LEVEL		5
#define MIN_RSSI		(-100)
#define MAX_RSSI		(-55)
#define RSSI_CHALEV_CNT 3
#define IEEE80211_MAX_PKTLEN (1600)

typedef enum RSSILEVEL_MIN
{
    RSSILEVEL_1 = -100,
    RSSILEVEL_2 = -85,
    RSSILEVEL_3 = -70,
    RSSILEVEL_4 = -55,
} RSSILEVEL_MIN;

#if 0
#define WLAN_CAPABILITY_ESS (1<<0)
#define WLAN_CAPABILITY_IBSS (1<<1)
#define WLAN_CAPABILITY_CF_POLLABLE (1<<2)
#define WLAN_CAPABILITY_CF_POLL_REQUEST (1<<3)
#define WLAN_CAPABILITY_PRIVACY (1<<4)
#define WLAN_CAPABILITY_SHORT_PREAMBLE (1<<5)
#define WLAN_CAPABILITY_PBCC (1<<6)
#define WLAN_CAPABILITY_CHANNEL_AGILITY (1<<7)
#define WLAN_CAPABILITY_SPECTRUM_MGMT (1<<8)
#define WLAN_CAPABILITY_QOS (1<<9)
#define WLAN_CAPABILITY_SHORT_SLOT_TIME (1<<10)
#define WLAN_CAPABILITY_DSSS_OFDM (1<<13)
#endif

/**
 * struct ieee80211_mcs_info - MCS information
 * @rx_mask: RX mask
 * @rx_highest: highest supported RX rate. If set represents
 *	the highest supported RX data rate in units of 1 Mbps.
 *	If this field is 0 this value should not be used to
 *	consider the highest RX data rate supported.
 * @tx_params: TX parameters
 */
struct ieee80211_mcs_info {
	u8 rx_mask[IEEE80211_HT_MCS_MASK_LEN];
	u16 rx_highest;
	u8 tx_params;
	u8 reserved[3];
} __attribute__((packed));

/**
 * struct ieee80211_vht_mcs_info - VHT MCS information
 * @rx_mcs_map: RX MCS map 2 bits for each stream, total 8 streams
 * @rx_highest: Indicates highest long GI VHT PPDU data rate
 *	STA can receive. Rate expressed in units of 1 Mbps.
 *	If this field is 0 this value should not be used to
 *	consider the highest RX data rate supported.
 * @tx_mcs_map: TX MCS map 2 bits for each stream, total 8 streams
 * @tx_highest: Indicates highest long GI VHT PPDU data rate
 *	STA can transmit. Rate expressed in units of 1 Mbps.
 *	If this field is 0 this value should not be used to
 *	consider the highest TX data rate supported.
 */
struct ieee80211_vht_mcs_info {
	u16 rx_mcs_map;
	u16 rx_highest;
	u16 tx_mcs_map;
	u16 tx_highest;
} __attribute__((packed));


/**
 * struct ieee80211_ht_cap - HT capabilities
 *
 * This structure is the "HT capabilities element" as
 * described in 802.11n D5.0 7.3.2.57
 */
struct ieee80211_ht_cap {
	u16 cap_info;
	u8 ampdu_params_info;

	/* 16 bytes MCS information */
	struct ieee80211_mcs_info mcs;

	u16 extended_ht_cap_info;
	u32 tx_BF_cap_info;
	u8 antenna_selection_info;
} __attribute__ ((packed));



/* 802.11n HT capabilities masks (for cap_info) */
#define IEEE80211_HT_CAP_LDPC_CODING		0x0001
#define IEEE80211_HT_CAP_SUP_WIDTH_20_40	0x0002
#define IEEE80211_HT_CAP_SM_PS			0x000C
#define		IEEE80211_HT_CAP_SM_PS_SHIFT	2
#define IEEE80211_HT_CAP_GRN_FLD		0x0010
#define IEEE80211_HT_CAP_SGI_20			0x0020
#define IEEE80211_HT_CAP_SGI_40			0x0040
#define IEEE80211_HT_CAP_TX_STBC		0x0080
#define IEEE80211_HT_CAP_RX_STBC		0x0300
#define		IEEE80211_HT_CAP_RX_STBC_SHIFT	8
#define IEEE80211_HT_CAP_DELAY_BA		0x0400
#define IEEE80211_HT_CAP_MAX_AMSDU		0x0800
#define IEEE80211_HT_CAP_DSSSCCK40		0x1000
#define IEEE80211_HT_CAP_RESERVED		0x2000
#define IEEE80211_HT_CAP_40MHZ_INTOLERANT	0x4000
#define IEEE80211_HT_CAP_LSIG_TXOP_PROT		0x8000

/* 802.11n HT extended capabilities masks (for extended_ht_cap_info) */
#define IEEE80211_HT_EXT_CAP_PCO		0x0001
#define IEEE80211_HT_EXT_CAP_PCO_TIME		0x0006
#define		IEEE80211_HT_EXT_CAP_PCO_TIME_SHIFT	1
#define IEEE80211_HT_EXT_CAP_MCS_FB		0x0300
#define		IEEE80211_HT_EXT_CAP_MCS_FB_SHIFT	8
#define IEEE80211_HT_EXT_CAP_HTC_SUP		0x0400
#define IEEE80211_HT_EXT_CAP_RD_RESPONDER	0x0800

/* 802.11n HT capability AMPDU settings (for ampdu_params_info) */
#define IEEE80211_HT_AMPDU_PARM_FACTOR		0x03
#define IEEE80211_HT_AMPDU_PARM_DENSITY		0x1C
#define		IEEE80211_HT_AMPDU_PARM_DENSITY_SHIFT	2

/*
 * Maximum length of AMPDU that the STA can receive.
 * Length = 2 ^ (13 + max_ampdu_length_exp) - 1 (octets)
 */
enum ieee80211_max_ampdu_length_exp {
	IEEE80211_HT_MAX_AMPDU_8K = 0,
	IEEE80211_HT_MAX_AMPDU_16K = 1,
	IEEE80211_HT_MAX_AMPDU_32K = 2,
	IEEE80211_HT_MAX_AMPDU_64K = 3
};

#define IEEE80211_HT_MAX_AMPDU_FACTOR 13

/* Minimum MPDU start spacing */
enum ieee80211_min_mpdu_spacing {
	IEEE80211_HT_MPDU_DENSITY_NONE = 0,	/* No restriction */
	IEEE80211_HT_MPDU_DENSITY_0_25 = 1,	/* 1/4 usec */
	IEEE80211_HT_MPDU_DENSITY_0_5 = 2,	/* 1/2 usec */
	IEEE80211_HT_MPDU_DENSITY_1 = 3,	/* 1 usec */
	IEEE80211_HT_MPDU_DENSITY_2 = 4,	/* 2 usec */
	IEEE80211_HT_MPDU_DENSITY_4 = 5,	/* 4 usec */
	IEEE80211_HT_MPDU_DENSITY_8 = 6,	/* 8 usec */
	IEEE80211_HT_MPDU_DENSITY_16 = 7	/* 16 usec */
};

/* Information Element IDs */
enum ieee80211_eid {
	WLAN_EID_SSID = 0,
	WLAN_EID_SUPP_RATES = 1,
	WLAN_EID_FH_PARAMS = 2,
	WLAN_EID_DS_PARAMS = 3,
	WLAN_EID_CF_PARAMS = 4,
	WLAN_EID_TIM = 5,
	WLAN_EID_IBSS_PARAMS = 6,
	WLAN_EID_CHALLENGE = 16,

	WLAN_EID_COUNTRY = 7,
	WLAN_EID_HP_PARAMS = 8,
	WLAN_EID_HP_TABLE = 9,
	WLAN_EID_REQUEST = 10,

	WLAN_EID_QBSS_LOAD = 11,
	WLAN_EID_EDCA_PARAM_SET = 12,
	WLAN_EID_TSPEC = 13,
	WLAN_EID_TCLAS = 14,
	WLAN_EID_SCHEDULE = 15,
	WLAN_EID_TS_DELAY = 43,
	WLAN_EID_TCLAS_PROCESSING = 44,
	WLAN_EID_QOS_CAPA = 46,
	/* 802.11z */
	WLAN_EID_LINK_ID = 101,
	/* 802.11s */
	WLAN_EID_MESH_CONFIG = 113,
	WLAN_EID_MESH_ID = 114,
	WLAN_EID_LINK_METRIC_REPORT = 115,
	WLAN_EID_CONGESTION_NOTIFICATION = 116,
	WLAN_EID_PEER_MGMT = 117,
	WLAN_EID_CHAN_SWITCH_PARAM = 118,
	WLAN_EID_MESH_AWAKE_WINDOW = 119,
	WLAN_EID_BEACON_TIMING = 120,
	WLAN_EID_MCCAOP_SETUP_REQ = 121,
	WLAN_EID_MCCAOP_SETUP_RESP = 122,
	WLAN_EID_MCCAOP_ADVERT = 123,
	WLAN_EID_MCCAOP_TEARDOWN = 124,
	WLAN_EID_GANN = 125,
	WLAN_EID_RANN = 126,
	WLAN_EID_PREQ = 130,
	WLAN_EID_PREP = 131,
	WLAN_EID_PERR = 132,
	WLAN_EID_PXU = 137,
	WLAN_EID_PXUC = 138,
	WLAN_EID_AUTH_MESH_PEER_EXCH = 139,
	WLAN_EID_MIC = 140,

	WLAN_EID_PWR_CONSTRAINT = 32,
	WLAN_EID_PWR_CAPABILITY = 33,
	WLAN_EID_TPC_REQUEST = 34,
	WLAN_EID_TPC_REPORT = 35,
	WLAN_EID_SUPPORTED_CHANNELS = 36,
	WLAN_EID_CHANNEL_SWITCH = 37,
	WLAN_EID_MEASURE_REQUEST = 38,
	WLAN_EID_MEASURE_REPORT = 39,
	WLAN_EID_QUIET = 40,
	WLAN_EID_IBSS_DFS = 41,

	WLAN_EID_ERP_INFO = 42,
	WLAN_EID_EXT_SUPP_RATES = 50,

	WLAN_EID_HT_CAPABILITY = 45,
	WLAN_EID_HT_OPERATION = 61,

	WLAN_EID_RSN = 48,
	WLAN_EID_MMIE = 76,
	WLAN_EID_WPA = 221,
	WLAN_EID_GENERIC = 221,
	WLAN_EID_VENDOR_SPECIFIC = 221,
	WLAN_EID_QOS_PARAMETER = 222,

	WLAN_EID_AP_CHAN_REPORT = 51,
	WLAN_EID_NEIGHBOR_REPORT = 52,
	WLAN_EID_RCPI = 53,
	WLAN_EID_BSS_AVG_ACCESS_DELAY = 63,
	WLAN_EID_ANTENNA_INFO = 64,
	WLAN_EID_RSNI = 65,
	WLAN_EID_MEASUREMENT_PILOT_TX_INFO = 66,
	WLAN_EID_BSS_AVAILABLE_CAPACITY = 67,
	WLAN_EID_BSS_AC_ACCESS_DELAY = 68,
	WLAN_EID_RRM_ENABLED_CAPABILITIES = 70,
	WLAN_EID_MULTIPLE_BSSID = 71,
	WLAN_EID_BSS_COEX_2040 = 72,
	WLAN_EID_OVERLAP_BSS_SCAN_PARAM = 74,
	WLAN_EID_EXT_CAPABILITY = 127,

	WLAN_EID_MOBILITY_DOMAIN = 54,
	WLAN_EID_FAST_BSS_TRANSITION = 55,
	WLAN_EID_TIMEOUT_INTERVAL = 56,
	WLAN_EID_RIC_DATA = 57,
	WLAN_EID_RIC_DESCRIPTOR = 75,

	WLAN_EID_DSE_REGISTERED_LOCATION = 58,
	WLAN_EID_SUPPORTED_REGULATORY_CLASSES = 59,
	WLAN_EID_EXT_CHANSWITCH_ANN = 60,

	WLAN_EID_VHT_CAPABILITY = 191,
	WLAN_EID_VHT_OPERATION = 192,

	/* 802.11ad */
	WLAN_EID_NON_TX_BSSID_CAP =  83,
	WLAN_EID_WAKEUP_SCHEDULE = 143,
	WLAN_EID_EXT_SCHEDULE = 144,
	WLAN_EID_STA_AVAILABILITY = 145,
	WLAN_EID_DMG_TSPEC = 146,
	WLAN_EID_DMG_AT = 147,
	WLAN_EID_DMG_CAP = 148,
	WLAN_EID_DMG_OPERATION = 151,
	WLAN_EID_DMG_BSS_PARAM_CHANGE = 152,
	WLAN_EID_DMG_BEAM_REFINEMENT = 153,
	WLAN_EID_CHANNEL_MEASURE_FEEDBACK = 154,
	WLAN_EID_AWAKE_WINDOW = 157,
	WLAN_EID_MULTI_BAND = 158,
	WLAN_EID_ADDBA_EXT = 159,
	WLAN_EID_NEXT_PCP_LIST = 160,
	WLAN_EID_PCP_HANDOVER = 161,
	WLAN_EID_DMG_LINK_MARGIN = 162,
	WLAN_EID_SWITCHING_STREAM = 163,
	WLAN_EID_SESSION_TRANSITION = 164,
	WLAN_EID_DYN_TONE_PAIRING_REPORT = 165,
	WLAN_EID_CLUSTER_REPORT = 166,
	WLAN_EID_RELAY_CAP = 167,
	WLAN_EID_RELAY_XFER_PARAM_SET = 168,
	WLAN_EID_BEAM_LINK_MAINT = 169,
	WLAN_EID_MULTIPLE_MAC_ADDR = 170,
	WLAN_EID_U_PID = 171,
	WLAN_EID_DMG_LINK_ADAPT_ACK = 172,
	WLAN_EID_QUIET_PERIOD_REQ = 175,
	WLAN_EID_QUIET_PERIOD_RESP = 177,
	WLAN_EID_EPAC_POLICY = 182,
	WLAN_EID_CLISTER_TIME_OFF = 183,
	WLAN_EID_ANTENNA_SECTOR_ID_PATTERN = 190,
};

#define WLAN_SA_QUERY_TR_ID_LEN 2

#define WLAN_CAPABILITY_MASK	(WLAN_CAPABILITY_GMODE_SHORT_SLOT | WLAN_CAPABILITY_SHORT_PREAMBLE | WLAN_CAPABILITY_PRIVACY | WLAN_CAPABILITY_ESS)

#define WLAN_CAPABILITY_ESS		(1<<0)
#define WLAN_CAPABILITY_IBSS		(1<<1)

/*
 * A mesh STA sets the ESS and IBSS capability bits to zero.
 * however, this holds true for p2p probe responses (in the p2p_find
 * phase) as well.
 */
#define WLAN_CAPABILITY_IS_STA_BSS(cap)	\
	(!((cap) & (WLAN_CAPABILITY_ESS | WLAN_CAPABILITY_IBSS)))

#define WLAN_CAPABILITY_CF_POLLABLE	(1<<2)
#define WLAN_CAPABILITY_CF_POLL_REQUEST	(1<<3)
#define WLAN_CAPABILITY_PRIVACY		(1<<4)
#define WLAN_CAPABILITY_SHORT_PREAMBLE	(1<<5)
#define WLAN_CAPABILITY_PBCC		(1<<6)
#define WLAN_CAPABILITY_CHANNEL_AGILITY	(1<<7)
#define WLAN_CAPABILITY_GMODE_SHORT_SLOT	(1<<10)

// FIXME XXX XXX XXX
typedef unsigned long long uint64_t;
typedef uint64_t __le64; 

/**
 * struct ieee80211_msrment_ie
 *
 * This structure refers to "Measurement Request/Report information element"
 */
struct ieee80211_msrment_ie {
	u8 token;
	u8 mode;
	u8 type;
	u8 request[0];
} __attribute__ ((packed));



/**
 * struct ieee80211_channel_sw_ie
 *
 * This structure refers to "Channel Switch Announcement information element"
 */
struct ieee80211_channel_sw_ie {
	u8 mode;
	u8 new_ch_num;
	u8 count;
} __attribute__ ((packed));

struct ieee80211_mgmt {
	u16 frame_control;
	u16 duration;
	u8 da[6];
	u8 sa[6];
	u8 bssid[6];
	u16 seq_ctrl;
	union {
		struct {
			u16 auth_alg;
			u16 auth_transaction;
			u16 status_code;
			/* possibly followed by Challenge text */
			u8 variable[0];
		} __attribute__ ((packed)) auth;
		struct {
			u16 reason_code;
		} __attribute__ ((packed)) deauth;
		struct {
			u16 capab_info;
			u16 listen_interval;
			/* followed by SSID and Supported rates */
			u8 variable[0];
		} __attribute__ ((packed)) assoc_req;
		struct {
			u16 capab_info;
			u16 status_code;
			u16 aid;
			/* followed by Supported rates */
			u8 variable[0];
		} __attribute__ ((packed)) assoc_resp, reassoc_resp;
		struct {
			u16 capab_info;
			u16 listen_interval;
			u8 current_ap[6];
			/* followed by SSID and Supported rates */
			u8 variable[0];
		} __attribute__ ((packed)) reassoc_req;
		struct {
			u16 reason_code;
		} __attribute__ ((packed)) disassoc;
		struct {
			u64 timestamp;
			u16 beacon_int;
			u16 capab_info;
			/* followed by some of SSID, Supported rates,
			 * FH Params, DS Params, CF Params, IBSS Params, TIM */
			u8 variable[0];
		} __attribute__ ((packed)) beacon;
		struct {
			/* only variable items: SSID, Supported rates */
			u8 variable[0];
		} __attribute__ ((packed)) probe_req;
		struct {
			u64 timestamp;
			u16 beacon_int;
			u16 capab_info;
			/* followed by some of SSID, Supported rates,
			 * FH Params, DS Params, CF Params, IBSS Params */
			u8 variable[0];
		} __attribute__ ((packed)) probe_resp;
		struct {
			u8 category;
			union {
				struct {
					u8 action_code;
					u8 dialog_token;
					u8 status_code;
					u8 variable[0];
				} __attribute__ ((packed)) wme_action;
				struct{
					u8 action_code;
					u8 element_id;
					u8 length;
					struct ieee80211_channel_sw_ie sw_elem;
				} __attribute__((packed)) chan_switch;
				struct{
					u8 action_code;
					u8 dialog_token;
					u8 element_id;
					u8 length;
					struct ieee80211_msrment_ie msr_elem;
				} __attribute__((packed)) measurement;
				struct{
					u8 action_code;
					u8 dialog_token;
					u16 capab;
					u16 timeout;
					u16 start_seq_num;
				} __attribute__((packed)) addba_req;
				struct{
					u8 action_code;
					u8 dialog_token;
					u16 status;
					u16 capab;
					u16 timeout;
				} __attribute__((packed)) addba_resp;
				struct{
					u8 action_code;
					u16 params;
					u16 reason_code;
				} __attribute__((packed)) delba;
				struct {
					u8 action_code;
					u8 variable[0];
				} __attribute__((packed)) self_prot;
				struct{
					u8 action_code;
					u8 variable[0];
				} __attribute__((packed)) mesh_action;
				struct {
					u8 action;
					u8 trans_id[WLAN_SA_QUERY_TR_ID_LEN];
				} __attribute__ ((packed)) sa_query;
				struct {
					u8 action;
					u8 smps_control;
				} __attribute__ ((packed)) ht_smps;
				struct {
					u8 action_code;
					u8 dialog_token;
					u16 capability;
					u8 variable[0];
				} __attribute__ ((packed)) tdls_discover_resp;
			} u;
		} __attribute__ ((packed)) action;
	} u;
} __attribute__ ((packed));

struct ieee80211_hdr {
	u16 frame_control;
	u16 duration_id;
	u8 addr1[6];
	u8 addr2[6];
	u8 addr3[6];
	u16 seq_ctrl;
	u8 addr4[6];
} __attribute__ ((packed));

struct ieee80211_hdr_3addr {
	u16 frame_control;
	u16 duration_id;
	u8 addr1[6];
	u8 addr2[6];
	u8 addr3[6];
	u16 seq_ctrl;
	u8 data[0];		/**< Beginning of frame data */
} __attribute__ ((packed));

struct ieee80211_qos_hdr {
	u16 frame_control;
	u16 duration_id;
	u8 addr1[6];
	u8 addr2[6];
	u8 addr3[6];
	u16 seq_ctrl;
	u16 qos_ctrl;
	u8 data[0];		/**< Beginning of frame data */
} __attribute__ ((packed));

struct ieee80211_pspoll {
	u16 frame_control;
    u16 aid;
	u8 bssid[6];
	u8 transmitmac[6];
} __attribute__ ((packed));


#if 1
//#ifndef IEEE80211_MACRO
//#define IEEE80211_MACRO

/**
 * ieee80211_has_tods - check if IEEE80211_FCTL_TODS is set
 * @fc: frame control bytes in little-endian byteorder
 */
#define ieee80211_has_tods(fc)  ((fc & IEEE80211_FCTL_TODS) != 0)

/**
 * ieee80211_has_fromds - check if IEEE80211_FCTL_FROMDS is set
 * @fc: frame control bytes in little-endian byteorder
 */
#define ieee80211_has_fromds(fc) ((fc & IEEE80211_FCTL_FROMDS) != 0)

/**
 * ieee80211_has_morefrags - check if IEEE80211_FCTL_MOREFRAGS is set
 * @fc: frame control bytes in little-endian byteorder
 */
 #define ieee80211_has_morefrags(fc) ((fc & IEEE80211_FCTL_MOREFRAGS) != 0)
 
/**
 * ieee80211_has_retry - check if IEEE80211_FCTL_RETRY is set
 * @fc: frame control bytes in little-endian byteorder
 */
   #define ieee80211_has_retry(fc) ((fc & IEEE80211_FCTL_RETRY) != 0)

/**
 * ieee80211_has_pm - check if IEEE80211_FCTL_PM is set
 * @fc: frame control bytes in little-endian byteorder
 */
#define ieee80211_has_pm(fc) ((fc & IEEE80211_FCTL_PM) != 0;)

/**
 * ieee80211_has_moredata - check if IEEE80211_FCTL_MOREDATA is set
 * @fc: frame control bytes in little-endian byteorder
 */
#define ieee80211_has_moredata(fc) ((fc & IEEE80211_FCTL_MOREDATA) != 0)

/**
 * ieee80211_has_protected - check if IEEE80211_FCTL_PROTECTED is set
 * @fc: frame control bytes in little-endian byteorder
 */
#define ieee80211_has_protected(fc) ((fc & IEEE80211_FCTL_PROTECTED) != 0)

/**
 * ieee80211_has_order - check if IEEE80211_FCTL_ORDER is set
 * @fc: frame control bytes in little-endian byteorder
 */
#define ieee80211_has_order(fc) ((fc & IEEE80211_FCTL_ORDER) != 0)

/**
 * ieee80211_is_mgmt - check if type is IEEE80211_FTYPE_MGMT
 * @fc: frame control bytes in little-endian byteorder
 */
#define ieee80211_is_mgmt(fc) ((fc & IEEE80211_FCTL_FTYPE) == IEEE80211_FTYPE_MGMT)

/**
 * ieee80211_is_ctl - check if type is IEEE80211_FTYPE_CTL
 * @fc: frame control bytes in little-endian byteorder
 */
#define ieee80211_is_ctl(fc) ((fc & IEEE80211_FCTL_FTYPE) == IEEE80211_FTYPE_CTL)

/**
 * ieee80211_is_data - check if type is IEEE80211_FTYPE_DATA
 * @fc: frame control bytes in little-endian byteorder
 */
#define ieee80211_is_data(fc) ((fc & IEEE80211_FCTL_FTYPE) == IEEE80211_FTYPE_DATA)

/**
 * ieee80211_is_data_qos - check if type is IEEE80211_FTYPE_DATA and IEEE80211_STYPE_QOS_DATA is set
 * @fc: frame control bytes in little-endian byteorder
 */
#define ieee80211_is_data_qos(fc) ((fc & (IEEE80211_FCTL_FTYPE | IEEE80211_STYPE_QOS_DATA)) == (IEEE80211_FTYPE_DATA | IEEE80211_STYPE_QOS_DATA))

/**
 * ieee80211_is_data_present - check if type is IEEE80211_FTYPE_DATA and has data
 * @fc: frame control bytes in little-endian byteorder
 */
#define ieee80211_is_data_present(fc) ((fc & (IEEE80211_FCTL_FTYPE | 0x40)) == (IEEE80211_FTYPE_DATA))

/**
 * ieee80211_is_assoc_req - check if IEEE80211_FTYPE_MGMT && IEEE80211_STYPE_ASSOC_REQ
 * @fc: frame control bytes in little-endian byteorder
 */
#define ieee80211_is_assoc_req(fc) ((fc & (IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) == (IEEE80211_FTYPE_MGMT | IEEE80211_STYPE_ASSOC_REQ))

/**
 * ieee80211_is_assoc_resp - check if IEEE80211_FTYPE_MGMT && IEEE80211_STYPE_ASSOC_RESP
 * @fc: frame control bytes in little-endian byteorder
 */
#define ieee80211_is_assoc_resp(fc) ((fc & (IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) == (IEEE80211_FTYPE_MGMT | IEEE80211_STYPE_ASSOC_RESP))

/**
 * ieee80211_is_reassoc_req - check if IEEE80211_FTYPE_MGMT && IEEE80211_STYPE_REASSOC_REQ
 * @fc: frame control bytes in little-endian byteorder
 */
#define ieee80211_is_reassoc_req(fc) ((fc & (IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) == (IEEE80211_FTYPE_MGMT | IEEE80211_STYPE_REASSOC_REQ))

/**
 * ieee80211_is_reassoc_resp - check if IEEE80211_FTYPE_MGMT && IEEE80211_STYPE_REASSOC_RESP
 * @fc: frame control bytes in little-endian byteorder
 */
#define ieee80211_is_reassoc_resp(fc) ((fc & (IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) == (IEEE80211_FTYPE_MGMT | IEEE80211_STYPE_REASSOC_RESP))

/**
 * ieee80211_is_probe_req - check if IEEE80211_FTYPE_MGMT && IEEE80211_STYPE_PROBE_REQ
 * @fc: frame control bytes in little-endian byteorder
 */
#define ieee80211_is_probe_req(fc) ((fc & (IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) == (IEEE80211_FTYPE_MGMT | IEEE80211_STYPE_PROBE_REQ))

/**
 * ieee80211_is_probe_resp - check if IEEE80211_FTYPE_MGMT && IEEE80211_STYPE_PROBE_RESP
 * @fc: frame control bytes in little-endian byteorder
 */
#define ieee80211_is_probe_resp(fc) ((fc & (IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) == (IEEE80211_FTYPE_MGMT | IEEE80211_STYPE_PROBE_RESP))

/**
 * ieee80211_is_beacon - check if IEEE80211_FTYPE_MGMT && IEEE80211_STYPE_BEACON
 * @fc: frame control bytes in little-endian byteorder
 */
#define ieee80211_is_beacon(fc) ((fc & (IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) == (IEEE80211_FTYPE_MGMT | IEEE80211_STYPE_BEACON))

/**
 * ieee80211_is_atim - check if IEEE80211_FTYPE_MGMT && IEEE80211_STYPE_ATIM
 * @fc: frame control bytes in little-endian byteorder
 */
#define ieee80211_is_atim(fc) ((fc & (IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) == (IEEE80211_FTYPE_MGMT | IEEE80211_STYPE_ATIM))

/**
 * ieee80211_is_disassoc - check if IEEE80211_FTYPE_MGMT && IEEE80211_STYPE_DISASSOC
 * @fc: frame control bytes in little-endian byteorder
 */
#define ieee80211_is_disassoc(fc) ((fc & (IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) == (IEEE80211_FTYPE_MGMT | IEEE80211_STYPE_DISASSOC))

/**
 * ieee80211_is_auth - check if IEEE80211_FTYPE_MGMT && IEEE80211_STYPE_AUTH
 * @fc: frame control bytes in little-endian byteorder
 */
#define ieee80211_is_auth(fc) ((fc & (IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) == (IEEE80211_FTYPE_MGMT | IEEE80211_STYPE_AUTH))

/**
 * ieee80211_is_deauth - check if IEEE80211_FTYPE_MGMT && IEEE80211_STYPE_DEAUTH
 * @fc: frame control bytes in little-endian byteorder
 */
#define ieee80211_is_deauth(fc) ((fc & (IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) == (IEEE80211_FTYPE_MGMT | IEEE80211_STYPE_DEAUTH))

/**
 * ieee80211_is_action - check if IEEE80211_FTYPE_MGMT && IEEE80211_STYPE_ACTION
 * @fc: frame control bytes in little-endian byteorder
 */
#define ieee80211_is_action(fc) ((fc & (IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) == (IEEE80211_FTYPE_MGMT | IEEE80211_STYPE_ACTION))

/**
 * ieee80211_is_back_req - check if IEEE80211_FTYPE_CTL && IEEE80211_STYPE_BACK_REQ
 * @fc: frame control bytes in little-endian byteorder
 */
#define ieee80211_is_back_req(fc) ((fc & (IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) == (IEEE80211_FTYPE_CTL | IEEE80211_STYPE_BACK_REQ))

/**
 * ieee80211_is_back - check if IEEE80211_FTYPE_CTL && IEEE80211_STYPE_BACK
 * @fc: frame control bytes in little-endian byteorder
 */
#define ieee80211_is_back(fc) ((fc & (IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) == (IEEE80211_FTYPE_CTL | IEEE80211_STYPE_BACK))

/**
 * ieee80211_is_pspoll - check if IEEE80211_FTYPE_CTL && IEEE80211_STYPE_PSPOLL
 * @fc: frame control bytes in little-endian byteorder
 */
#define ieee80211_is_pspoll(fc) ((fc & (IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) == (IEEE80211_FTYPE_CTL | IEEE80211_STYPE_PSPOLL))

/**
 * ieee80211_is_rts - check if IEEE80211_FTYPE_CTL && IEEE80211_STYPE_RTS
 * @fc: frame control bytes in little-endian byteorder
 */
#define ieee80211_is_rts(fc) ((fc & (IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) == (IEEE80211_FTYPE_CTL | IEEE80211_STYPE_RTS))

/**
 * ieee80211_is_cts - check if IEEE80211_FTYPE_CTL && IEEE80211_STYPE_CTS
 * @fc: frame control bytes in little-endian byteorder
 */
#define ieee80211_is_cts(fc) ((fc & (IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) == (IEEE80211_FTYPE_CTL | IEEE80211_STYPE_CTS))

/**
 * ieee80211_is_ack - check if IEEE80211_FTYPE_CTL && IEEE80211_STYPE_ACK
 * @fc: frame control bytes in little-endian byteorder
 */
#define ieee80211_is_ack(fc) ((fc & (IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) == (IEEE80211_FTYPE_CTL | IEEE80211_STYPE_ACK))

/**
 * ieee80211_is_cfend - check if IEEE80211_FTYPE_CTL && IEEE80211_STYPE_CFEND
 * @fc: frame control bytes in little-endian byteorder
 */
#define ieee80211_is_cfend(fc) ((fc & (IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) == (IEEE80211_FTYPE_CTL | IEEE80211_STYPE_CFEND))

/**
 * ieee80211_is_cfendack - check if IEEE80211_FTYPE_CTL && IEEE80211_STYPE_CFENDACK
 * @fc: frame control bytes in little-endian byteorder
 */
#define ieee80211_is_cfendack(fc) ((fc & (IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) == (IEEE80211_FTYPE_CTL | IEEE80211_STYPE_CFENDACK))

/**
 * ieee80211_is_nullfunc - check if frame is a regular (non-QoS) nullfunc frame
 * @fc: frame control bytes in little-endian byteorder
 */
#define ieee80211_is_nullfunc(fc) ((fc & (IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) == (IEEE80211_FTYPE_DATA | IEEE80211_STYPE_NULLFUNC))

/**
 * ieee80211_is_qos_nullfunc - check if frame is a QoS nullfunc frame
 * @fc: frame control bytes in little-endian byteorder
 */
#define ieee80211_is_qos_nullfunc(fc) ((fc & (IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) == (IEEE80211_FTYPE_DATA | IEEE80211_STYPE_QOS_NULLFUNC))

/**
 * ieee80211_is_first_frag - check if IEEE80211_SCTL_FRAG is not set
 * @seq_ctrl: frame sequence control bytes in little-endian byteorder
 */
#define ieee80211_is_first_frag(seq_ctrl) ((seq_ctrl & IEEE80211_SCTL_FRAG) == 0)

/**
 * ieee80211_get_qos_ctl - get pointer to qos control bytes
 * @hdr: the frame
 *
 * The qos ctrl bytes come after the frame_control, duration, seq_num
 * and 3 or 4 addresses of length ETH_ALEN.
 * 3 addr: 2 + 2 + 2 + 3*6 = 24
 * 4 addr: 2 + 2 + 2 + 4*6 = 30
 */
/*inline U8 *ieee80211_get_qos_ctl(struct ieee80211_hdr *hdr)
{
	if (ieee80211_has_a4(hdr->frame_control))
		return (U8 *)hdr + 30;
	else
		return (U8 *)hdr + 24;
}*/
#endif

struct ieee80211_element {
	u8 id;
	u8 len;
	u8 data[0];
} __attribute__ ((packed));

/**
 * struct ieee80211_meshconf_ie
 *
 * This structure refers to "Mesh Configuration information element"
 */
struct ieee80211_meshconf_ie {
	u8 meshconf_psel;
	u8 meshconf_pmetric;
	u8 meshconf_congest;
	u8 meshconf_synch;
	u8 meshconf_auth;
	u8 meshconf_form;
	u8 meshconf_cap;
} __attribute__ ((packed));


/**
 * struct ieee80211_tim
 *
 * This structure refers to "Traffic Indication Map information element"
 */
struct ieee80211_tim_ie {
	u8 dtim_count;
	u8 dtim_period;
	u8 bitmap_ctrl;
	/* variable size: 1 - 251 bytes */
	u8 virtual_map[1];
} __attribute__ ((packed));



/**
 * struct ieee80211_quiet_ie
 *
 * This structure refers to "Quiet information element"
 */
struct ieee80211_quiet_ie {
	u8 count;
	u8 period;
	u16 duration;
	u16 offset;
} __attribute__ ((packed));

/**
 * struct ieee80211_ht_operation - HT operation IE
 *
 * This structure is the "HT operation element" as
 * described in 802.11n-2009 7.3.2.57
 */
struct ieee80211_ht_operation {
	u8 primary_chan;
	u8 ht_param;
	u16 operation_mode;
	u16 stbc_param;
	u8 basic_set[16];
} __attribute__ ((packed));

/**
 * struct ieee80211_rann_ie
 *
 * This structure refers to "Root Announcement information element"
 */
struct ieee80211_rann_ie {
	u8 rann_flags;
	u8 rann_hopcount;
	u8 rann_ttl;
	u8 rann_addr[6];
	u32 rann_seq;
	u32 rann_interval;
	u32 rann_metric;
} __attribute__ ((packed));

enum ieee80211_rann_flags {
	RANN_FLAG_IS_GATE = 1 << 0,
};


/* this struct represents 802.11n's RA/TID combination */
struct ieee80211_ra_tid {
	u8 ra[ETH_ALEN];
	u16 tid;
};

/* Parsed Information Elements */
struct ieee802_11_elems {
	u8 *ie_start;
	size_t total_len;

	/* pointers to IEs */
	u8 *ssid;
	u8 *supp_rates;
	u8 *fh_params;
	u8 *ds_params;
	u8 *cf_params;
	struct ieee80211_tim_ie *tim;
	u8 *ibss_params;
	u8 *challenge;
	u8 *wpa;
	u8 *rsn;
	u8 *erp_info;
	u8 *ext_supp_rates;
	u8 *wmm_info;
	u8 *wmm_param;
	struct ieee80211_ht_cap *ht_cap_elem;
	struct ieee80211_ht_operation *ht_operation;
	struct ieee80211_meshconf_ie *mesh_config;
	u8 *mesh_id;
	u8 *peering;
	u8 *preq;
	u8 *prep;
	u8 *perr;
	struct ieee80211_rann_ie *rann;
	u8 *ch_switch_elem;
	u8 *country_elem;
	u8 *pwr_constr_elem;
	u8 *quiet_elem;	/* first quite element */
	u8 *timeout_int;

	/* length of them, respectively */
	u8 ssid_len;
	u8 supp_rates_len;
	u8 fh_params_len;
	u8 ds_params_len;
	u8 cf_params_len;
	u8 tim_len;
	u8 ibss_params_len;
	u8 challenge_len;
	u8 wpa_len;
	u8 rsn_len;
	u8 erp_info_len;
	u8 ext_supp_rates_len;
	u8 wmm_info_len;
	u8 wmm_param_len;
	u8 mesh_id_len;
	u8 peering_len;
	u8 preq_len;
	u8 prep_len;
	u8 perr_len;
	u8 ch_switch_elem_len;
	u8 country_elem_len;
	u8 pwr_constr_elem_len;
	u8 quiet_elem_len;
	u8 num_of_quiet_elem;	/* can be more the one */
	u8 timeout_int_len;

	/* whether a parse error occurred while retrieving these elements */
	bool parse_error;
};

struct ieee80211_probe_request {
	struct ieee80211_hdr_3addr header;
	/* SSID, supported rates */
	struct ieee802_11_elems info_element[0];
} __attribute__ ((packed));

#if 0
struct ieee80211_auth {
	struct ieee80211_hdr_3addr header;
	__le16 algorithm;
	__le16 transaction;
	__le16 status;
	/* challenge */
	struct ieee80211_info_element info_element[0];
} __attribute__ ((packed));
#endif
#if 1
struct ieee80211_auth {
	struct ieee80211_hdr_3addr header;
	u8 iv[3];
	u8 key;
	u8 data[512];
	/* challenge */
} __attribute__ ((packed));
#endif
struct ieee80211_assoc_request {
	struct ieee80211_hdr_3addr header;
	u16 capability;
	u16 listen_interval;
	/* SSID, supported rates, RSN */
	struct ieee802_11_elems info_element[0];
} __attribute__ ((packed));

struct ieee80211_reassoc_request {
	struct ieee80211_hdr_3addr header;
	u16 capability;
	u16 listen_interval;
	u8 curr_ap_addr[6];
	/* SSID, supported rates, RSN */
	struct ieee802_11_elems info_element[0];
} __attribute__ ((packed));

struct ieee80211_deauth_disasso_request {
	struct ieee80211_hdr_3addr header;
	u16 reason_code;
} __attribute__ ((packed));

struct ieee80211_probe_response {
	u8 		time_stamp[8];
	u16	beacon_interval;
	u16	capability;
} __attribute__ ((packed));

struct ieee80211_auth_response {
	u16 	auth_algorithm;
	u16	auth_seq;
	u16	status_code;
} __attribute__ ((packed));

struct ieee80211_association_response {
	u16 	capability_info;
	u16	    status_code;
	u8		association_id;
} __attribute__ ((packed));

struct ieee80211_beacon_response {
	u8 		time_stamp[8];
	u16	    beacon_interval;
	u16	    capability;
} __attribute__ ((packed));

struct ieee80211_ht_capabilities {
     u16 ht_capabilities_info;
     u8 a_mpdu_params;
     u8 supported_mcs_set[16];
     u16 ht_extended_capabilities;
     u32 tx_bf_capability_info;
     u8 asel_capabilities;
} __attribute__ ((packed));

typedef struct t_eth_addr {
  u8 addr[MAC_ADDR_LEN];
} eth_addr;

struct uip_eth_hdr {
  eth_addr dest;
  eth_addr src;
  u16 type;
};

#define cpu_to_le16(x)	Le16(x)
#define le16_to_cpu(x)	Le16(x)

// Endian converters
#define Le16(b)                        \
   (  ((u16)(     (b) &   0xFF) << 8)  \
   |  (     ((u16)(b) & 0xFF00) >> 8)  \
   )

#define Le64(b)                             \
   (  ((b &       0xFF) << 56)  \
   |  ((b &     0xFF00) << 40)  \
   |  ((b &   0xFF0000) << 24)  \
   |  ((b & 0xFF000000) << 8)  \
   |  ((b & 0xFF00000000) >> 8)  \
   |  ((b & 0xFF0000000000) >> 24)  \
   |  ((b & 0xFF000000000000) >> 40)  \
   |  ((b & 0xFF00000000000000) >> 56)  \
   )

#endif /* __IEEE80211_H__ */

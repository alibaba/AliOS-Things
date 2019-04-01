#ifndef _HDR80211_H_
#define _HDR80211_H_

#include <net_types.h>

#ifdef __GNUC__
#define STRUCT_PACKED __attribute__ ((packed))
#else
#define STRUCT_PACKED
#endif



enum ieee80211_category {
	WLAN_CATEGORY_SPECTRUM_MGMT = 0,
	WLAN_CATEGORY_QOS = 1,
	WLAN_CATEGORY_DLS = 2,
	WLAN_CATEGORY_BACK = 3,
	WLAN_CATEGORY_PUBLIC = 4,
	WLAN_CATEGORY_HT = 7,
	WLAN_CATEGORY_SA_QUERY = 8,
	WLAN_CATEGORY_PROTECTED_DUAL_OF_ACTION = 9,
	WLAN_CATEGORY_TDLS = 12,
	WLAN_CATEGORY_MESH_ACTION = 13,
	WLAN_CATEGORY_MULTIHOP_ACTION = 14,
	WLAN_CATEGORY_SELF_PROTECTED = 15,
	WLAN_CATEGORY_WMM = 17,
	WLAN_CATEGORY_VENDOR_SPECIFIC_PROTECTED = 126,
	WLAN_CATEGORY_VENDOR_SPECIFIC = 127,
};
/* BACK action code */
enum ieee80211_back_actioncode {
	WLAN_ACTION_ADDBA_REQ = 0,
	WLAN_ACTION_ADDBA_RESP = 1,
	WLAN_ACTION_DELBA = 2,
};

/* BACK (block-ack) parties */
enum ieee80211_back_parties {
	WLAN_BACK_RECIPIENT = 0,
	WLAN_BACK_INITIATOR = 1,
};



/* Status codes (IEEE 802.11-2007, 7.3.1.9, Table 7-23) */
#define WLAN_STATUS_SUCCESS 0
#define WLAN_STATUS_UNSPECIFIED_FAILURE 1
#define WLAN_STATUS_TDLS_WAKEUP_ALTERNATE 2
#define WLAN_STATUS_TDLS_WAKEUP_REJECT 3
#define WLAN_STATUS_SECURITY_DISABLED 5
#define WLAN_STATUS_UNACCEPTABLE_LIFETIME 6
#define WLAN_STATUS_NOT_IN_SAME_BSS 7
#define WLAN_STATUS_CAPS_UNSUPPORTED 10
#define WLAN_STATUS_REASSOC_NO_ASSOC 11
#define WLAN_STATUS_ASSOC_DENIED_UNSPEC 12
#define WLAN_STATUS_NOT_SUPPORTED_AUTH_ALG 13
#define WLAN_STATUS_UNKNOWN_AUTH_TRANSACTION 14
#define WLAN_STATUS_CHALLENGE_FAIL 15
#define WLAN_STATUS_AUTH_TIMEOUT 16
#define WLAN_STATUS_AP_UNABLE_TO_HANDLE_NEW_STA 17
#define WLAN_STATUS_ASSOC_DENIED_RATES 18
/* IEEE 802.11b */
#define WLAN_STATUS_ASSOC_DENIED_NOSHORT 19
#define WLAN_STATUS_ASSOC_DENIED_NOPBCC 20
#define WLAN_STATUS_ASSOC_DENIED_NOAGILITY 21
/* IEEE 802.11h */
#define WLAN_STATUS_SPEC_MGMT_REQUIRED 22
#define WLAN_STATUS_PWR_CAPABILITY_NOT_VALID 23
#define WLAN_STATUS_SUPPORTED_CHANNEL_NOT_VALID 24
/* IEEE 802.11g */
#define WLAN_STATUS_ASSOC_DENIED_NO_SHORT_SLOT_TIME 25
#define WLAN_STATUS_ASSOC_DENIED_NO_DSSS_OFDM 26
#define WLAN_STATUS_ASSOC_DENIED_NO_HT 27
#define WLAN_STATUS_R0KH_UNREACHABLE 28
#define WLAN_STATUS_ASSOC_DENIED_NO_PCO 29
/* IEEE 802.11w */
#define WLAN_STATUS_ASSOC_REJECTED_TEMPORARILY 30
#define WLAN_STATUS_ROBUST_MGMT_FRAME_POLICY_VIOLATION 31
#define WLAN_STATUS_UNSPECIFIED_QOS_FAILURE 32
#define WLAN_STATUS_REQUEST_DECLINED 37
#define WLAN_STATUS_INVALID_PARAMETERS 38
/* IEEE 802.11i */
#define WLAN_STATUS_INVALID_IE 40
#define WLAN_STATUS_GROUP_CIPHER_NOT_VALID 41
#define WLAN_STATUS_PAIRWISE_CIPHER_NOT_VALID 42
#define WLAN_STATUS_AKMP_NOT_VALID 43
#define WLAN_STATUS_UNSUPPORTED_RSN_IE_VERSION 44
#define WLAN_STATUS_INVALID_RSN_IE_CAPAB 45
#define WLAN_STATUS_CIPHER_REJECTED_PER_POLICY 46
#define WLAN_STATUS_TS_NOT_CREATED 47
#define WLAN_STATUS_DIRECT_LINK_NOT_ALLOWED 48
#define WLAN_STATUS_DEST_STA_NOT_PRESENT 49
#define WLAN_STATUS_DEST_STA_NOT_QOS_STA 50
#define WLAN_STATUS_ASSOC_DENIED_LISTEN_INT_TOO_LARGE 51
/* IEEE 802.11r */
#define WLAN_STATUS_INVALID_FT_ACTION_FRAME_COUNT 52
#define WLAN_STATUS_INVALID_PMKID 53
#define WLAN_STATUS_INVALID_MDIE 54
#define WLAN_STATUS_INVALID_FTIE 55
#define WLAN_STATUS_GAS_ADV_PROTO_NOT_SUPPORTED 59
#define WLAN_STATUS_NO_OUTSTANDING_GAS_REQ 60
#define WLAN_STATUS_GAS_RESP_NOT_RECEIVED 61
#define WLAN_STATUS_STA_TIMED_OUT_WAITING_FOR_GAS_RESP 62
#define WLAN_STATUS_GAS_RESP_LARGER_THAN_LIMIT 63
#define WLAN_STATUS_REQ_REFUSED_HOME 64
#define WLAN_STATUS_ADV_SRV_UNREACHABLE 65
#define WLAN_STATUS_REQ_REFUSED_SSPN 67
#define WLAN_STATUS_REQ_REFUSED_UNAUTH_ACCESS 68
#define WLAN_STATUS_INVALID_RSNIE 72
#define WLAN_STATUS_TRANSMISSION_FAILURE 79





/* block-ack parameters */
#define IEEE80211_ADDBA_PARAM_POLICY_MASK 0x0002
#define IEEE80211_ADDBA_PARAM_TID_MASK 0x003C
#define IEEE80211_ADDBA_PARAM_BUF_SIZE_MASK 0xFFC0
#define IEEE80211_DELBA_PARAM_TID_MASK 0xF000
#define IEEE80211_DELBA_PARAM_INITIATOR_MASK 0x0800


#define	BA_POLICY_DELAYED		0
#define	BA_POLICY_IMMEDIATE	    1

#define	ADDBA_STATUS_SUCCESS			0
#define	ADDBA_STATUS_REFUSED		37
#define	ADDBA_STATUS_INVALID_PARAM	38


#define	DELBA_REASON_QSTA_LEAVING	36
#define	DELBA_REASON_END_BA			37
#define	DELBA_REASON_UNKNOWN_BA	38
#define	DELBA_REASON_TIMEOUT			39




/* 802.11 BAR control masks */
#define IEEE80211_BAR_CTRL_ACK_POLICY_NORMAL	0x0000
#define IEEE80211_BAR_CTRL_MULTI_TID		0x0002
#define IEEE80211_BAR_CTRL_CBMTID_COMPRESSED_BA	0x0004
#define IEEE80211_BAR_CTRL_TID_INFO_MASK	0xf000
#define IEEE80211_BAR_CTRL_TID_INFO_SHIFT	12

#define IEEE80211_SN_SHIFT							4



#define MAC_ADDR_LEN				6

#define FCS_LEN                     4
#define HDR80211_MGMT_LEN           24
#define IEEE80211_QOS_CTL_LEN       2
#define IEEE80211_HT_CTL_LEN        4

/**
 * IEEE 802.11 Address offset position. This defines the offset of Address1,
 * Address2, Address3 and Address4 if present.
 */
#define OFFSET_ADDR1                        4
#define OFFSET_ADDR2                        10
#define OFFSET_ADDR3                        16
#define OFFSET_ADDR4                        24


/* IEEE 802.11 Frame Control: */
#define M_FC_VER		            0x0003
#define M_FC_FTYPE		            0x000c
#define M_FC_STYPE		            0x00f0
#define M_FC_TODS		            0x0100
#define M_FC_FROMDS		            0x0200
#define M_FC_MOREFRAGS	            0x0400
#define M_FC_RETRY		            0x0800
#define M_FC_PWRMGMT	            0x1000
#define M_FC_MOREDATA		        0x2000
#define M_FC_PROTECTED	            0x4000
#define M_FC_ORDER		            0x8000

#define M_SC_FRAG		            0x000F
#define M_SC_SEQ		            0xFFF0

/* Frame Type: */
#define FT_MGMT		                0x0000
#define FT_CTRL		                0x0004
#define FT_DATA		                0x0008



/* Frame SubType: Management Frames */
#define FST_ASSOC_REQ	            0x0000
#define FST_ASSOC_RESP	            0x0010
#define FST_REASSOC_REQ	            0x0020
#define FST_REASSOC_RESP	        0x0030
#define FST_PROBE_REQ	            0x0040
#define FST_PROBE_RESP	            0x0050
#define FST_BEACON		            0x0080
#define FST_ATIM		            0x0090
#define FST_DISASSOC	            0x00A0
#define FST_AUTH		            0x00B0
#define FST_DEAUTH		            0x00C0
#define FST_ACTION		            0x00D0

/* Frame SubType: Control Frames */
#define FST_CONTROL_WRAPPER         0x0070
#define FST_BA_REQ	                0x0080
#define FST_BA	                    0x0090
#define FST_PSPOLL		            0x00A0
#define FST_RTS		                0x00B0
#define FST_CTS		                0x00C0
#define FST_ACK		                0x00D0
#define FST_CFEND		            0x00E0
#define FST_CFENDACK	            0x00F0

/* Frame SubType: Data Frames */
#define FST_DATA			        0x0000
#define FST_DATA_CFACK		        0x0010
#define FST_DATA_CFPOLL		        0x0020
#define FST_DATA_CFACKPOLL		    0x0030
#define FST_NULLFUNC		        0x0040
#define FST_CFACK			        0x0050
#define FST_CFPOLL			        0x0060
#define FST_CFACKPOLL		        0x0070
#define FST_QOS_DATA		        0x0080
#define FST_QOS_DATA_CFACK		    0x0090
#define FST_QOS_DATA_CFPOLL		    0x00A0
#define FST_QOS_DATA_CFACKPOLL	    0x00B0
#define FST_QOS_NULLFUNC		    0x00C0
#define FST_QOS_CFACK		        0x00D0
#define FST_QOS_CFPOLL		        0x00E0
#define FST_QOS_CFACKPOLL		    0x00F0


#define FC_TYPE(fc)           ((fc) & (M_FC_FTYPE|M_FC_STYPE))
#define FC_FTYPE(fc)          ((fc) & M_FC_FTYPE)
#define FC_STYPE(fc)          ((fc) & M_FC_STYPE)


#define IS_TODS_SET(fc)       ((fc) & M_FC_TODS)
#define IS_FROMDS_SET(fc)     ((fc) & M_FC_FROMDS)
#define IS_MOREFLAG_SET(fc)   ((fc) & M_FC_MOREFRAGS)
#define IS_RETRY_SET(fc)      ((fc) & M_FC_RETRY)
#define IS_PM_SET(fc)         ((fc) & M_FC_PWRMGMT)
#define IS_MOREDATA_SET(fc)   ((fc) & M_FC_MOREDATA)
#define IS_PROTECT_SET(fc)    ((fc) & M_FC_PROTECTED)
#define IS_ORDER_SET(fc)      ((fc) & M_FC_ORDER)
#define IS_4ADDR_FORMAT(fc)   IS_EQUAL(((fc)&(M_FC_TODS|M_FC_FROMDS)), (M_FC_TODS|M_FC_FROMDS))
#define IS_MGMT_FRAME(fc)     IS_EQUAL(FC_FTYPE(fc), FT_MGMT)
#define IS_CTRL_FRAME(fc)     IS_EQUAL(FC_FTYPE(fc), FT_CTRL)
#define IS_DATA_FRAME(fc)     IS_EQUAL(FC_FTYPE(fc), FT_DATA)
#define IS_QOS_DATA(fc)       IS_EQUAL(((fc)&(M_FC_FTYPE|FST_QOS_DATA)), (FT_DATA|FST_QOS_DATA))
#define IS_NULL_DATA(fc)      IS_EQUAL((fc)&(M_FC_FTYPE|FST_NULLFUNC), (FT_DATA|FST_NULLFUNC))
#define IS_QOS_NULL_DATA(fc)  IS_EQUAL((fc)&(M_FC_FTYPE|FST_NULLFUNC|FST_QOS_DATA), (FT_DATA|FST_NULLFUNC|FST_QOS_DATA)) 


#define IS_SUBTYPE_DATA(fc)   IS_EQUAL(FC_STYPE(fc), FST_DATA)
#define IS_SUBTYPE_OOSDATA(fc)   IS_EQUAL(FC_STYPE(fc), FST_QOS_DATA)


#define IS_ASSOC_REQ(fc)      IS_EQUAL(FC_TYPE(fc), (FT_MGMT|FST_ASSOC_REQ))
#define IS_ASSOC_RESP(fc)     IS_EQUAL(FC_TYPE(fc), (FT_MGMT|FST_ASSOC_RESP))
#define IS_REASSOC_REQ(fc)    IS_EQUAL(FC_TYPE(fc), (FT_MGMT|FST_REASSOC_REQ))
#define IS_REASSOC_RESP(fc)   IS_EQUAL(FC_TYPE(fc), (FT_MGMT|FST_REASSOC_RESP))
#define IS_PROBE_REQ(fc)      IS_EQUAL(FC_TYPE(fc), (FT_MGMT|FST_PROBE_REQ))
#define IS_PROBE_RESP(fc)     IS_EQUAL(FC_TYPE(fc), (FT_MGMT|FST_PROBE_RESP))
#define IS_BEACON(fc)         IS_EQUAL(FC_TYPE(fc), (FT_MGMT|FST_BEACON))
#define IS_ATIM(fc)           IS_EQUAL(FC_TYPE(fc), (FT_MGMT|FST_ATIM))
#define IS_DISASSOC(fc)       IS_EQUAL(FC_TYPE(fc), (FT_MGMT|FST_DISASSOC))
#define IS_AUTH(fc)           IS_EQUAL(FC_TYPE(fc), (FT_MGMT|FST_AUTH))
#define IS_DEAUTH(fc)         IS_EQUAL(FC_TYPE(fc), (FT_MGMT|FST_DEAUTH))
#define IS_ACTION(fc)         IS_EQUAL(FC_TYPE(fc), (FT_MGMT|FST_ACTION))
#define IS_PSPOLL(fc)         IS_EQUAL(FC_TYPE(fc), (FT_CTRL|FST_PSPOLL))
#define IS_RTS(fc)            IS_EQUAL(FC_TYPE(fc), (FT_MGMT|FST_RTS))
#define IS_CTS(fc)            IS_EQUAL(FC_TYPE(fc), (FT_MGMT|FST_CTS))
#define IS_ACK(fc)            IS_EQUAL(FC_TYPE(fc), (FT_MGMT|FST_ACK))
#define IS_BAR(fc)            IS_EQUAL(FC_TYPE(fc), (FT_CTRL|FST_BA_REQ))
#define IS_BA(fc)             IS_EQUAL(FC_TYPE(fc), (FT_CTRL|FST_BA))


struct ieee80211_bar {
	u16 frame_control;
	u16 duration;
	u8 ra[6];
	u8 ta[6];
	u16 control;
	u16 start_seq_num;
} ;



#define GET_FC_MOREFRAG(fc)	  (((fc) & 0x0400) >> 10)

typedef struct FC_Field_st {
    u16                 ver:2;
    u16                 type:2;
    u16                 subtype:4;
    u16                 toDS:1;
    u16                 fromDS:1;
    u16                 MoreFlag:1;
    u16                 Retry:1;
    u16                 PwrMgmt:1;
    u16                 MoreData:1;
    u16                 Protected:1;
    u16                 order:1;    

} FC_Field, *PFC_Field;

typedef struct QOS_Ctrl_st {
    u16                 tid:4;
    u16                 bit4:1;
    u16                 ack_policy:2;
    u16                 rsvd:1;
    u16                 bit8_15:8;

} QOSCtrl_Field, *PQOSCtrl_Field;

typedef struct HDR80211_Data_st {
	FC_Field            fc;
	u16                 dur;
	ETHER_ADDR          addr1;
	ETHER_ADDR          addr2;
	ETHER_ADDR          addr3;
	u16                 seq_ctrl;    
    
} HDR80211_Data, *PHDR80211_Data;

typedef struct HDR80211_Data_4addr_st {
	FC_Field            fc;
	u16                 dur;
	ETHER_ADDR          addr1;
	ETHER_ADDR          addr2;
	ETHER_ADDR          addr3;
	u16                 seq_ctrl;        
    ETHER_ADDR          addr4;

} HDR80211_Data_4addr, *PHDR80211_Data_4addr;

#define BASE_LEN(PTR2variable,PTR)			(PTR2variable - (u8 *) PTR)


typedef struct HDR80211_Mgmt_st {
    FC_Field            fc;
    u16                 dur;
    ETHER_ADDR          da;
    ETHER_ADDR          sa;
    ETHER_ADDR          bssid;
    u16                 seq_ctrl;

    union {
        struct {
            u16         auth_algo;
            u16         trans_id;
            u16         status_code;
            /* possibly followed by Challenge text */
            u8          variable[0];    
        } auth;
        struct {
            u16         reason_code;
        } deauth;
        struct {
            u16         capab_info;
            u16         listen_interval;
            /* followed by SSID and Supported rates */
            u8          variable[0];
        } assoc_req;
        struct {
			u16         capab_info;
			u16         status_code;
			u16         aid;
			/* followed by Supported rates */
			u8 variable[0];
		} assoc_resp, reassoc_resp;
        struct {
			u16         capab_info;
			u16         listen_interval;
			u8          current_ap[6];
			/* followed by SSID and Supported rates */
			u8 variable[0];
		} reassoc_req;
        struct {
			u16         reason_code;
		} disassoc;
        struct {
			u64         timestamp;
			u16         beacon_int;
			u16         capab_info;
			/* followed by some of SSID, Supported rates,
			 * FH Params, DS Params, CF Params, IBSS Params, TIM */
			u8 variable[0];
		} beacon;
		struct { /*lint -save -e43 */
			/* only variable items: SSID, Supported rates */
			u8 variable[0];
		} probe_req; /*lint -restore */
		struct {
			u64         timestamp;
			u16         beacon_int;
			u16         capab_info;
			/* followed by some of SSID, Supported rates,
			 * FH Params, DS Params, CF Params, IBSS Params */
			u8 variable[0];
		} probe_resp;
        struct {
            u8          category;
            union {
                struct {
					u8  action_code;
					u8  dialog_token;
					u8  status_code;
					u8  variable[0];
				} wme_action;
				struct {
					u8  action_code;
					u8  element_id;
					u8  length;
//					struct ieee80211_channel_sw_ie sw_elem;
				} chan_switch;
                struct {
					u8  action_code;
					u8  dialog_token;
					u8  element_id;
					u8  length;
//					struct ieee80211_msrment_ie msr_elem;
				} measurement;
				struct{
					u8  action_code;
					u8  dialog_token;
					u16 capab;
					u16 timeout;
					u16 start_seq_num;
				} STRUCT_PACKED addba_req;
				struct{
					u8  action_code;
					u8  dialog_token;
					u16 status;
					u16 capab;
					u16 timeout;
				} STRUCT_PACKED addba_resp;
                struct {
					u8  action_code;
					u16 params;
					u16 reason_code;
				} STRUCT_PACKED delba;
				struct {
					u8  action_code;
					/* capab_info for open and confirm,
					 * reason for close
					 */
					u16 aux;
					/* Followed in plink_confirm by status
					 * code, AID and supported rates,
					 * and directly by supported rates in
					 * plink_open and plink_close
					 */
					u8  variable[0];
				} plink_action;
                struct{
					u8  action_code;
					u8  variable[0];
				} mesh_action;
				struct {
					u8  action;
//					u8  trans_id[WLAN_SA_QUERY_TR_ID_LEN];
				} sa_query;
				struct {
					u8  action;
					u8  smps_control;
				} ht_smps;
            } u;           
        } STRUCT_PACKED action;
    } u;
    
} STRUCT_PACKED HDR80211_Mgmt, *PHDR80211_Mgmt;

// 1 byte category code
#define IEEE80211_ACTION_CATEGORY_SIZE 1
/* mgmt header + 1 byte category code */
#define IEEE80211_MIN_ACTION_SIZE OFFSETOF(HDR80211_Mgmt, u.action.u)

typedef struct HDR80211_Ctrl_st 
{
    FC_Field            fc;
    u16                 dur;
    ETHER_ADDR          ra;
    ETHER_ADDR          ta;

} HDR80211_Ctrl, *PHDR80211_Ctrl;


typedef struct HDR8023_Data_st {
	ETHER_ADDR			dest;
	ETHER_ADDR			src;
	u16					protocol;
} HDR8023_Data, *PHDR8023_Data;


#define HDR80211_MGMT(x)    /*lint -save -e740 */ (HDR80211_Mgmt *)((u8 *)(x)+((PKT_Info *)(x))->hdr_offset) /*lint -restore */
#define HDR80211_CTRL(x)
#define HDR80211_DATA(x)





/* Define WLAN Cipher Suite */
#define CIPHER_SUITE_NONE                   0
#define CIPHER_SUITE_WEP40                  1
#define CIPHER_SUITE_WEP104                 2
#define CIPHER_SUITE_TKIP                   3
#define CIPHER_SUITE_CCMP                   4



/**
 * WLAN Operation Mode Definition (for wlan_mode field):
 *
 * @ WLAN_STA: operate as STA (infrastructure) mode
 * @ WLAN_AP: operate as AP (infrastructure) mode
 * @ WLAN_IBSS: operate as IBSS (ad-hoc) mode
 * @ WLAN_WDS: Wireless Distribution System mode (Wireless Bridge)
 */
#define WLAN_STA                            0
#define WLAN_AP                             1
#define WLAN_IBSS                           2
#define WLAN_WDS                            3




/**
 * HT Mode Definition (for ht_mode field):
 * 
 * @ HT_NONE
 * @ HT_MF
 * @ HT_GF
 */
#define HT_NONE                         0
#define HT_MF                           1
#define HT_GF                           2


#if 0
static u8 *GET_QOS_CTRL(HDR80211_Data_4addr *hdr)
{
	if (hdr->fc.fromDS && hdr->fc.toDS)
		return (u8 *)hdr + 30;
	else
		return (u8 *)hdr + 24;
}
#endif

#endif /* _HDR80211_H_ */




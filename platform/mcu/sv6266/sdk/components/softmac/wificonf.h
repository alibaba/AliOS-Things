#ifndef __SYSTEMCONF_H__
#define __SYSTEMCONF_H__

#include "soc_types.h"
#include "osal.h"
#include "osal.h"
#include "wpa.h"

#define MAX_APRECORD 	1
#define DEFAULT_AP_LIST_AMOUNT		(50)
#define MAX_CONN_RTYCNT	10
#define CFG_SOFTAP_SSID_LENGTH (32)
#define IF0_NAME       "et0"
#define IF1_NAME       "et1"
#define ENABLE_HK (1)

typedef enum {
    EN_DRATE_IDX_BMODE_LONG_10M = 0,       //LONG PREAMBLE, 1M
    EN_DRATE_IDX_BMODE_LONG_20M = 1,       //LONG PREAMBLE, 2M
    EN_DRATE_IDX_BMODE_LONG_55M = 2,       //LONG PREAMBLE, 5.5M 
    EN_DRATE_IDX_BMODE_LONG_110M= 3,       //LONG PREAMBLE, 11M
    EN_DRATE_IDX_BMODE_SHORT_20M= 4,       //SHORT PREAMBLE, 2M
    EN_DRATE_IDX_BMODE_SHORT_55M= 5,       //SHORT PREAMBLE, 5.5M	
    EN_DRATE_IDX_BMODE_SHORT_110M=6,       //SHORT PREAMBLE, 11M			
	
    EN_DRATE_IDX_GMODE_60M=7,              //6M		
    EN_DRATE_IDX_GMODE_90M=8, 		       //9M,	
    EN_DRATE_IDX_GMODE_120M=9,             //12M		
    EN_DRATE_IDX_GMODE_180M=10,            //18M			
    EN_DRATE_IDX_GMODE_240M=11,            //24M		
    EN_DRATE_IDX_GMODE_360M=12,            //36M			
    EN_DRATE_IDX_GMODE_480M=13,	           //48M	
    EN_DRATE_IDX_GMODE_540M=14,            //54M
    
    EN_DRATE_IDX_NMODE_LONG_GI_65=15,		//6.5M
    EN_DRATE_IDX_NMODE_LONG_GI_130=16,	    //13M		
    EN_DRATE_IDX_NMODE_LONG_GI_195=17,		//19.5M
    EN_DRATE_IDX_NMODE_LONG_GI_260=18,		//26M	
    EN_DRATE_IDX_NMODE_LONG_GI_390=19,		//39M
    EN_DRATE_IDX_NMODE_LONG_GI_520=20,		//52M	
    EN_DRATE_IDX_NMODE_LONG_GI_585=21,		//58.5M
    EN_DRATE_IDX_NMODE_LONG_GI_650=22,     //65M
		
    EN_DRATE_IDX_INVALID=0xFF,	
	
} EN_DATA_RATE_IDX;

typedef enum {
	WIFI_DISCONNECTION = 0,
	WIFI_CONNECTION,
} ieee80211_action;

typedef enum {
	IEEE80211_DISCONNECTED = 0,
	IEEE80211_PROBING_REQ,
	IEEE80211_AUTHENTICATING_SEQ1,
	IEEE80211_AUTHENTICATING_SEQ3,
	IEEE80211_ASSOCIATING_REQ,
	IEEE80211_REASSOCIATING_REQ,
	IEEE80211_EAPOL_HANDSHAKE1,
    IEEE80211_EAPOL_HANDSHAKE2,
	IEEE80211_EAPOL_HANDSHAKE3,
    IEEE80211_EAPOL_HANDSHAKE4,
	IEEE80211_GETIP,
	IEEE80211_CONNECTED,
    IEEE80211_SCANCHANNEL,
    IEEE80211_ACTIVEPROBE,
    IEEE80211_ACTIVETRANSITION,
} ieee80211_state;

typedef enum {
	MGMT_CONN_PASSIVE = 0,
	MGMT_CONN_ACTIVE,
	MGMT_GETIP,
	MGMT_CONNECTED,
    MGMT_DISCONNECTED,
    MGMT_RESETTIMER,
    MGMT_RECONNECT,
    MGMT_DELTASK,
} MGMT_ACTION;

typedef enum {
	DIS_TIMEOUT = 0,
	DIS_DEAUTH,
	DIS_DISASSOC,
    DIS_ASSOCERR,
    DIS_SELFDIS,
} DISCONN_TYPE;

/* 802.11 authentication and  association Status code, refer to the following URL
https://supportforums.cisco.com/document/141136/80211-association-status-80211-deauth-reason-codes*/
typedef enum {
	IEEE80211_RSP_SUCCESS = 0,
	IEEE80211_RSP_FAILURE = 1,
	IEEE80211_RSP_CAPAB_UNSUPP = 10,
	IEEE80211_RSP_REASSOC_INVALID = 11,
	IEEE80211_RSP_ASSOC_DENIED = 12,
	IEEE80211_RSP_AUTH_ALGO_UNSUPP = 13,
	IEEE80211_RSP_AUTH_SEQ_INVALID = 14,
	IEEE80211_RSP_AUTH_CHALL_INVALID = 15,
	IEEE80211_RSP_AUTH_TIMEOUT = 16,
	IEEE80211_RSP_ASSOC_NO_ROOM = 17,
	IEEE80211_RSP_ASSOC_NEED_RATE = 18,
	IEEE80211_RSP_ASSOC_NEED_SHORT_PMBL = 19,
	IEEE80211_RSP_ASSOC_NEED_PBCC = 20,
	IEEE80211_RSP_ASSOC_NEED_CHAN_AGILITY = 21,
	IEEE80211_RSP_ASSOC_NEED_SPECTRUM_MGMT = 22,
	IEEE80211_RSP_ASSOC_BAD_POWER = 23,
	IEEE80211_RSP_ASSOC_BAD_CHANNELS = 24,
	IEEE80211_RSP_ASSOC_NEED_SHORT_SLOT = 25,
	IEEE80211_RSP_ASSOC_NEED_DSSS_OFDM = 26,
	IEEE80211_RSP_QOS_FAILURE = 32,
	IEEE80211_RSP_QOS_NO_ROOM = 33,
	IEEE80211_RSP_LINK_IS_HORRIBLE = 34,
	IEEE80211_RSP_ASSOC_NEED_QOS = 35,
	IEEE80211_RSP_REQUEST_DECLINED = 37,
	IEEE80211_RSP_REQUEST_INVALID = 38,
	IEEE80211_RSP_TS_NOT_CREATED_AGAIN = 39,
	IEEE80211_RSP_INVALID_IE = 40,
	IEEE80211_RSP_GROUP_CIPHER_INVALID = 41,
	IEEE80211_RSP_PAIR_CIPHER_INVALID  = 42,
	IEEE80211_RSP_AKMP_INVALID = 43,
	IEEE80211_RSP_RSN_VERSION_UNSUPP = 44,
	IEEE80211_RSP_RSN_CAPAB_INVALID = 45,
	IEEE80211_RSP_CIPHER_REJECTED = 46,
	IEEE80211_RSP_TS_NOT_CREATED_WAIT = 47,
	IEEE80211_RSP_DIRECT_LINK_FORBIDDEN = 48,
	IEEE80211_RSP_DEST_NOT_PRESENT = 49,
	IEEE80211_RSP_DEST_NOT_QOS = 50,
	IEEE80211_RSP_ASSOC_LISTEN_TOO_HIGH = 51,
} ieee80211_rsp_state;

enum {   
    NL80211_CHAN_NO_HT = 0,
    NL80211_CHAN_HT20,
    NL80211_CHAN_HT40MINUS,
    NL80211_CHAN_HT40PLUS,
};

typedef enum _MP_ID
{
    MP_MAC_ADDR=0,
    MP_RF_TABLE,
    MP_HT_TABLE,
    MP_LT_TABLE,
    MP_TEMP_BOUNDARY,
    MP_SERIAL_NO,
    MP_NUM_MAX
} MP_ID;

typedef struct t_WIFI_STATUS
{
	ieee80211_state status;
	DISCONN_TYPE	type;
	u16		        reasoncode;
} WIFI_STATUS;


typedef struct t_TAG_AP_INFO
{
	u8 		name[32];
	u8		name_len;
	u8 		mac[6];
	u8		channel;
	u8		security_type;
	u8		security_subType;
	u8		rssi;
} TAG_AP_INFO;
extern TAG_AP_INFO *ap_list;


typedef enum t_TAG_SECURITY
{
	NONE = 0,
	WEP,
	WPA,
	WPA2,
	WPAWPA2,
} TAG_SECURITY;

typedef enum t_POWER_MODE
{
	PSACTIVE_MODE = 0,
	PSSAVING_MODE,
    TEMP_ACTIVE, //There is data to send or receive, go to active mode temporarily.
    TRANS_TO_PS, //Means there is a packet send to AP with the power management bit is power save mode.
    TRANS_TO_AC, //Means there is a packet send to AP with the power management bit is active mode.
    TRANS_TO_ACTMP, //Means there is a packet send to AP with the power management bit is active mode.
} POWER_MODE;

typedef enum t_TAG_SECURITY_TYPE
{
	NOSECTYPE = 0,
	TKIP,
	CCMP,
	TKIP_CCMP,
} TAG_SECURITY_TYPE;

typedef union uip_ip4addr_t {
  u8  u8[4];			
  u16 u16[2];
  u32 u32;
} uip_ip4addr_t;

typedef struct packetinfo {
  u8  *data;			
  u16 len;
  s16 rssi;
} packetinfo;

typedef enum 
{
	STA_CONN = 0,
	STA_DISCONN,
} STA_STATE;

typedef struct stainfo {
  STA_STATE  stastatus;           
  u8  mac[6];			
} STAINFO;

typedef struct t_IP_CONFIGURATION
{
	uip_ip4addr_t	local_ip_addr;
	uip_ip4addr_t	net_mask;
	uip_ip4addr_t	gateway_ip_addr;
	uip_ip4addr_t	dns_server;
	u32				dhcp_enable;		// 1: Enable 0: Fix IP
} IP_CONFIGURATION;

typedef enum _ssv_rc_rate_type {
    RC_TYPE_B_ONLY = 0,
    RC_TYPE_LEGACY_GB,
    RC_TYPE_HT_LGI_20,
    RC_TYPE_HT_SGI_20,
    RC_TYPE_LEGACY_A,
    RC_TYPE_HT_LGI_20_5G,
    RC_TYPE_HT_SGI_20_5G,
    RC_TYPE_HT_GF,
    RC_TYPE_CUSTOM,
    RC_TYPE_MAX,
} ssv_rc_rate_type;

typedef struct t_DATARATE_INFO
{
	ssv_rc_rate_type ratetype;
    u8 datarate;
    u8 m1datarate;
    u8 m2datarate;
	u8 index;
	u8 succfulcnt;
    u8 failcnt;
    u8 tryup;
    u8 mcs0_7;
    u8 ratetbl[12];
    u8 ratetblnum;
}DATARATE_INFO;

typedef enum {
   eSOFTAP_STA_ID0=0,      //client 0
   eSOFTAP_STA_ID1,        //client 1
   eSOFTAP_STA_ID2,        //client 2 
   eSOFTAP_STA_ID3,        //client 3 
   eSOFTAP_STA_MAX_ID        //client 1
} eSOFTAP_STA_ID;

typedef struct _STA_INFO    //0:use peermac0,1:use peermac1
{
    u8 connected;    
    u8 mac[6];    //only one sta available now!!
    u8 ipaddr[4];
    s32 idle_timer;
    s32 softap_tx_null_data_idle_time;
    u32 filteraddr;
	u8 ht_support;
	u8 wmm_support;
} STA_INFO;

typedef enum WIFI_DIS_REASON{
    UNSUPPORT_ENCRYPT = 0,
    WRONG_PASSPHRASE,
    DEAUTHENTICATION,
    DISASSOCIATION,
    AP_MISSING,
    CANNOT_GET_IP,
    DISCONNECT,
} WIFI_DIS_REASON;

typedef struct _AT_SCAN_CUSTOM_ARGS {
    void (*callbackfn)(void *);
    u32 channel5Gindex;
    u16 channel2Gindex;
    u16 scantime_in_ms;
    /*
     * Specific a SSID during scanning
     */
    char  ssid[32];
    u8 ssid_len;
} AT_SCAN_CUSTOM_ARGS;

typedef struct _WIFI_RSP {
    u8  wifistatus;
    u8  id;
    u16 reason;
    u16 code;
} WIFI_RSP;

typedef struct _CUSTOM_CONFIG {
    u16 APlist_amount;
    u16 available_2Gchannel;
    u32 available_5Gchannel;
}CUSTOM_CONFIG;

typedef enum {
    RECV_BEACON             = 0x1,
    RECV_MGMT               = 0x2,
    RECV_BROADCAST_DATA     = 0x4,
    RECV_DATA               = 0x8,
} SNIFFER_RECVINDEX;

typedef enum {
    DUT_NONE = 0,
    DUT_STA,
    DUT_AP,
    DUT_CONCURRENT,
    DUT_TWOSTA,
    DUT_SNIFFER,
} WIFI_OPMODE;

typedef enum {
    IF_STA = 0,
    IF_AP,
    IF_NONE,
} IF_MODE;

typedef enum {
    IF_0 = 0,
    IF_1,
    IF_MAX,
} IF_NUM;

enum {
    NO_SECONDCH = 0,
    UPPER_SECONDCH,
    LOWER_SECONDCH,
};

typedef enum {
	TW = 0,
    CN,
	HK,
    US,
    JP,
    COUNTRY_MAX,
}COUNTRY_CODE;

typedef enum {
    RADIO_BAND_2G=0,
    RADIO_BAND_5100=1,
    RADIO_BAND_5500=2,
    RADIO_BAND_5700=3,
    RADIO_BAND_5900=4,
}RADIO_BAND;   

typedef enum {
	SOFTAP_EAPOL_INITIAL = 0,
	SOFTAP_EAPOL_STEP1,	// tx1_of_4
	SOFTAP_EAPOL_STEP3,	// tx3_of_4
	SOFTAP_EAPOL_DONE,
    SOFTAP_EAPOL_MAX
} SOFTAP_EAPOL_ACTION;

typedef enum {
	SOFTAP_TX_DEAUTH = 0,
	SOFTAP_NOTIFY_CONN,	
	SOFTAP_NOTIFY_DISCONN,
    SOFTAP_ACT_MAX
} SOFTAP_MGMT_ACTION;

typedef struct _SOFTAP_EAPOL_MSG {
    SOFTAP_EAPOL_ACTION state;
} SOFTAP_EAPOL_MSG;

typedef struct 
{
	u8 index;
	u8 macaddr[6];
} SOFTAP_MGMT_MSG;

typedef struct t_RSSI_CAL
{
    u8 rssival[6];
    u8 rssiindex;
}RSSI_CAL;

typedef struct t_AP_DETAIL_INFO
{
	u8 		              mac[6];
	u8		              channel;
	u8		              ssid_len;
	u8 		              ssid[32];
	u8		              key[64];
	u8		              key_len;
	u8		              security_type;
	u8		              security_subType;
	u8                    	      cci_start;
	u8		              cci_gate;    
	u8		              rssi;
	u8                            txnullcnt;
        u8                            configen;
	u8		              pmk[32];
//	u8		              ratetbl[16];
//	u8		              ratetblnum;
	u8		              rssiLevelChangCnt;
    u8					  retrycnt;
    u8 					  beaconmisscnt;

    IP_CONFIGURATION      ipconf;
    struct wpa_common_ctx wpactx;
    RSSI_CAL              rssical;
    void                  (*wificbfun)(WIFI_RSP*);
    u32					  sta_txengflow;
    u16                   mgmtseq;
    u16                   dataseq;
    u16					  capability_info;
    u16  				  ht_capab;
	u16 				  beacon_interval;
    u16 				  association_id;
    u16                   unicastseq;
    u8 	                  action;
    u8 	                  status;
    u8	                  wifi_security;
    u8	                  priority;
    u8 					  idle_count;
    u8 					  custom_crypt;
    u8 					  ht_support;
    u8 					  wmm_support;
    u8                    dtim_period;
    u8                    secondchinfo;
} AP_DETAIL_INFO;

typedef struct t_IEEE80211STATUS
{
	u8  				local_mac[2][6];
    char 	            ifname[2][6];
    u8                  ifmode[2];
    u8                  mac0_en;
    u8                  mac1_en;
    u8                  radioprocack;
    u8                  available_index;
    CUSTOM_CONFIG       customconf;
	AP_DETAIL_INFO		connAP[2];

    u32	 		 	 	scantime;

    //mac_rx_handler execute counter!! used to measure input packets counter!!! If it didn't change any more,it means something wrong!!
    u32 mac_rx_cnt;
    WIFI_OPMODE opmode;

    //added in 2015/11/27
    u32 region_code;
    u32 watch_dog_count; //unit: (mini-seconds)    

    u8	recordAP;      
    u8  support_5G;
    u8  countrycode;
    u8  atuartid;
    u8  atuartrate;
    u8  baserate;
    
    //softap status!!
    OsBufQ        dhcps_que;
    OsBufQ        softap_mgm_que;
	OsBufQ        softap_eapol_que;
	OsBufQ        softap_txdeauth_que;
    u8 softap_state;
//	u8 softap_gw_digit[3];	//gateway setting!!

	u32 softap_start_ip;	//ex:172.31.254.240
	u32 softap_end_ip;	//ex:172.31.254.250
	u32 softap_gw;	    //ex:172.31.254.250
	u32 softap_subnet;	//ex:255.255.255.0

	u8 softap_hk_support;	//beacon will have special information for HOMEKIT
	const uint8_t *softap_hk_inIE;
	size_t softap_hk_inIELen;

    s16 softap_sta_connected_number;                //0 or 1 or 2
    u8 softap_sta_mac[6];    	//eapol 
	u8 softap_sta_mac_assoc[6];	//assoc+reassoc
	u8 softap_sta_mac_auth[6];	//auth
	u8 softap_sta_mac_prob[6];	//prob
    u8 softap_tmp_htsupp;
    u8 softap_tmp_wmmsupp;
    u8 softap_ht40info;
	
    STA_INFO softap_sta_info[4];

    u8 softap_key[64];
    u8 softap_keylen;
    u8 softap_pmk[32];
    u8 softap_max_sta_num;
    u8 softap_encryt_mode;  //0: open, 1:WPA2-tkip 2:WPA2:CCMP
    u8 softap_nmode_support;
    u8 softap_dhcpd_init;
    s32 softap_reset_cnt;

//<<config start
    s8 softap_ssid[CFG_SOFTAP_SSID_LENGTH+1];   //+1 to prevent printf core dump!!
    s8 softap_ssid_length;   //+1 to prevent printf core dump!!
    u8 run_channel;
    u8 softap_beaconpkt_en;
    u32 *softap_beacon_addr;
    u32 softap_ack_timeout;
    u32 softap_data_timeout;
//>>config end
    
    u16 softap_seq_no;    //sequence number!!
    u16 softap_freg_no;   //fregment number!!
    u16 softap_seq_and_freg_no;
    u64 softap_timestamp;   //time stamp
    s16 softap_beacon_interval;
#if 1
	s8 softap_dump_enable;
	u32 softap_pktbuf_tmp;
	s32 softap_mem_cnt;
	s32 softap_eapol_mcnt;
#endif
	
    u32	softap_txengflow;
    void (*sofatpnotifycbfn)(STAINFO *);

    //sniffer mode settings
    void (*mgmtcbfn)(packetinfo *);
    void (*sniffercb)(packetinfo *);
    u32 snifferfilter;   
    
    DATARATE_INFO		  rateinfo[5];
    u16                   rc_mask;

    bool mac_debug_en;
    void (*mac_send_debug_cb)(void *data);
    void (*radio_send_debug_cb)(void *data);

    /* mutex. protect the member of gwifistatus */
    OsMutex w_mutex;
#if 1
    bool LowRateRTSCTS;
#endif
    bool ICMPFixLowRate;
    bool rate_2m_enable;

    /*
        bit0:RADIO_BAND_2G,
        bit1:RADIO_BAND_5100,
        bit2:RADIO_BAND_5500,
        bit3:RADIO_BAND_5700,
        bit4:RADIO_BAND_5900,
    */
    u8 dpd_disable_mask;
    u8 dcdc_disable;
}IEEE80211STATUS;

/*int remove_sysconfig(void);
void reset_global_conf();
void init_global_conf();

S32 gconfig_set_softap_ssid(char *p_ssid);
S32 gconfig_set_enable_smart(SMARK_LINK_TYPE val, U8 change_now);
S32 gconfig_set_auto_connect(U8 val);
S32 gconfig_save_config();
void clear_ap_lists();*/
#endif

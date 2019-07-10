#ifndef _RDA59XX_WIFI_INCLUDE_H_
#define _RDA59XX_WIFI_INCLUDE_H_

#include "rda_def.h"
#include "rda59xx_wifi_common.h"

/*************************************common******************************************/
#define WIFI_TXP_NUM                2
#define WIFI_CHANNEL_NUM            14
#define WIFI_TX_POWER_PHY_GN_REG  0x11F
#define WIFI_TX_POWER_PHY_B_REG   0x120
#define WIFI_TX_POWER_RF_REG      0x8A
#define WIFI_XTAL_CAL_REG         0xDA

extern r_void maclib_init(r_void);
extern r_void rda59xx_txip_data(r_void* data, r_u32 len, r_u8 mode);
extern r_void *rda59xx_get_databuf(r_void);
extern r_void rda59xx_set_cb_queue(r_void* queue);
/*
 * mode    0 - not 11n   1 - 11n
 *
 * -----------------11n(mode 1)(Mbps)-----------------
 * rate          HT20                   HT40
 *       GI(800ns)  GI(400ns)   GI(800ns)  GI(400ns)
 *  0       6.5       7.2          13.5       15
 *  1       13       14.2           27        30
 *  2      19.5      21.7          40.5       45
 *  3       26       28.9           54        60
 *  4       39       43.3           81        90
 *  5       52       57.8           108       120
 *  6      58.5      65            121.5      135  
 *  7       65       72             135       150  
 *
 * --------------not 11n(mode 0)(Mbps)-----------------
 * rate   data rate     rate   data rate
 *  0     autorate        9        9        
 *  1        1           12       12       
 *  2        2           18       18       
 *  5       5.5          24       24       
 *  11       11          36       36       
 *  6        6           48       48
 *
 */  
extern r_s32 rda59xx_set_data_rate(r_u8 mode, r_u8 rate);
extern void rda59xx_set_ARPoffload(r_u32 enable, r_u32 ip);
extern r_void rda59xx_send_rawdata(r_u8* data, r_u32 len);
extern r_s32 rda59xx_send_nulldata(r_s32 power_save);
extern r_s32 rda59xx_set_retrans_policy(r_u8 count);
extern r_s32 rda59xx_set_channel(r_u8 channel);
extern r_u8 rda59xx_get_channel();
extern r_s32 rda59xx_filter_multicast(r_u8 enable);
extern r_void rda59xx_wland_task(r_void *arg);

extern r_void *wland_queue;


/********************************************sniffer************************************************/

/* Enable filtering ACK frames (no support)*/
//#define RDA_RX_FILTER_DROP_ACK                  BIT0

/* Enable filtering CTS frames (no support)*/
//#define RDA_RX_FILTER_DROP_CTS                  BIT1

/* Enable filtering RTS frames (no support)*/
//#define RDA_RX_FILTER_DROP_RTS                  BIT2

/* Enable filtering beacon frames */
#define RDA_RX_FILTER_DROP_BEACON               BIT3

/* Enable filtering ATIM frames (no support)*/
//#define RDA_RX_FILTER_DROP_ATIM                 BIT4

/* Enable filtering CF_END frames (no support)*/
//#define RDA_RX_FILTER_DROP_CF_END               BIT5

/* Enable filtering QCF_POLL frames (no support)*/
//#define RDA_RX_FILTER_DROP_QCF_POLL             BIT6

/* Filter Management frames which are not directed to current STA */
#define RDA_RX_FILTER_DROP_ND_MGMT              BIT7

/* Filter BC/MC MGMT frames belonging to other BSS */
#define RDA_RX_FILTER_DROP_BC_MC_MGMT_OTHER_BSS BIT8

/* Enable filtering of duplicate frames */
#define RDA_RX_FILTER_DROP_DUPLICATE            BIT9

/* Enable filtering of frames whose FCS has failed */
#define RDA_RX_FILTER_DROP_FCS_FAILED           BIT10

/* Enable filtering of De-authentication frame */
#define RDA_RX_FILTER_DROP_DEAUTH               BIT11

/* Filter BA frames which are not received as SIFS response (no support)*/
//#define RDA_RX_FILTER_DROP_NSIFS_RESP_BA        BIT12

/* Filter BA frames which are received as SIFS response (no support)*/
//#define RDA_RX_FILTER_DROP_SIFS_RESP_BA         BIT13

/* Filter frames which are received in secondary channel (20 MHz PPDU from Secondary channel) */
#define RDA_RX_FILTER_DROP_SEC_CHANNEL          BIT14

/* Filter BC/MC DATA frames belonging to other BSS */
#define RDA_RX_FILTER_DROP_BC_MC_DATA_OTHER_BSS BIT15

/* Filter DATA frames not directed to this station */
#define RDA_RX_FILTER_DROP_ND_DATA              BIT16

/* Filter Control frames which are not directed to current STA (no support)*/
//#define RDA_RX_FILTER_DROP_ND_CONTROL           BIT17

/* Filter Beacon frames (in IBSS mode) which are not used for adoption because the timestamp field is lower than TSF timer */
#define RDA_RX_FILTER_DROP_IBSS_BEACON          BIT18

typedef int (*sniffer_handler_t)(void *data, unsigned short data_len);

extern r_s32 rda59xx_sniffer_set_filter(r_u8 to_ds, r_u8 from_ds, r_u32 mgm_filter);
extern r_s32 rda59xx_sniffer_enable_internal(sniffer_handler_t handler);
extern r_s32 rda59xx_sniffer_disable_internal();




/********************************************scan**************************************************/
typedef struct {
    r_u8 BSSID[ETH_ALEN];
    r_u8 SSID[MAX_SSID_LEN];
    r_u8 SSID_len;
    r_u8 channel;
    r_u8 secure_type;//0~none, BIT(0)~wpa2, BIT(1)~wpa, BIT(2)~wep
    r_u8 wmm;
    r_s8 RSSI;
    r_u8 *ie;
    r_u16 capability;
    r_u32 ie_length;  /* byte length of Information Elements */
}rda59xx_scan_result;

typedef struct {
    r_u8 SSID[MAX_SSID_LEN];
    r_u8 SSID_len;
    r_u8 channel;
    r_u8 scan_mode;
    r_u8 scan_time;
    r_u8 country_code;
}rda59xx_scan_info;

extern r_void rda59xx_set_country_code(r_u8 c_code);
extern r_void rda59xx_set_expired_time(r_u32 expired_time);
extern r_s32 rda59xx_del_scan_result(rda59xx_scan_result *del_result);
extern r_s32 rda59xx_del_scan_all_result(void);
extern r_s32 rda59xx_check_scan_result(const r_u8 *ssid, const r_u8 *bssid, const r_u8 channel);
extern r_s32 rda59xx_get_scan_result_special(rda59xx_scan_result *buf, const r_u8 *ssid, const r_u8 *bssid, const r_u8 channel);
extern r_s32 rda59xx_get_scan_result_index(rda59xx_scan_result *buf, const r_u8 index);
extern r_u32 rda59xx_get_scan_result(rda59xx_scan_result *buf, const r_u8 num);
extern r_u32 rda59xx_get_scan_num();
extern r_u32 rda59xx_scan_internal(rda59xx_scan_info *scan_info);



/*****************************************sta**********************************************/
typedef struct{
    r_u8 ssid[MAX_SSID_LEN];
    r_u8 pw[MAX_PW_LEN];
    r_u8 bssid[ETH_ALEN];
    r_u8 channel;
    r_u8 dhcp;
    r_u32 ip;
    r_u32 netmask;
    r_u32 gateway;
}rda59xx_sta_info;

typedef struct {
    r_u8 bssid[ETH_ALEN];
    r_u8 ssid[MAX_SSID_LEN];
    r_u8 ssid_len;
    /* Refer to dna_wlan_sec_type_e */
    r_u8 channel;
    r_u8 secure;
    r_s8 rssi;
    r_u32 ipaddr;
    r_u32 mask;
    r_u32 gateway;
    r_u32 dns1;
    r_u32 dns2;
} rda59xx_bss_info;

extern r_s32 rda59xx_get_joined_AP(rda59xx_scan_result *bss);
extern void rda5981_set_auth_timeout(r_u8 timeout_enable);
extern void rda5981_set_sta_listen_interval(unsigned char interval);
/****************************************ap************************************************/
typedef struct {
    r_u8 mac[ETH_ALEN];
    r_u32 ip;
}rda59xx_apsta_info_t;

typedef struct{
    r_u8 ssid[MAX_SSID_LEN];
    r_u8 pw[MAX_PW_LEN];
    r_u8 channel;
    r_u8 secure;
    r_u8 hidden;
    r_u16 beacon;
    r_u32 ip;
    r_u32 netmask;
    r_u32 gateway;
    r_u32 dhcps;
    r_u32 dhcpe;
}rda59xx_ap_info;

extern r_u32 rda59xx_get_ap_join_info(rda59xx_apsta_info_t *buf, const r_u32 len);

extern r_void rda59xx_set_AP_white_list(r_u32 flag, r_u8 *mac);

#endif /* _RDA59XX_WIFI_INCLUDE_H_ */


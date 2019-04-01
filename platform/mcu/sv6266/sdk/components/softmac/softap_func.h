#ifndef __SOFTAP_FUNC_H__
#define __SOFTAP_FUNC_H__

#include "soc_types.h"
#include "ieee80211.h"
#include "wificonf.h"

typedef struct 
{
	//u8 a_gw_digit[3];

	u32 start_ip;	//ex:172.31.254.240
	u32 end_ip;	//ex:172.31.254.250
	u32 gw;	    //ex:172.31.254.250
	u32 subnet;	//ex:255.255.255.0
	
	s8 max_sta_num;
	s8 encryt_mode;	
	s8 keylen;
	u8 key[64];
	u8 channel;
	s16 beacon_interval;

	s8 ssid_length;
	s8 ssid[32];
} SOFTAP_CUSTOM_CONFIG;


//void set_mrx_filter_softap();
void init_softap_conf();
s32 softap_start();
int32_t softap_exit();
void softap_rx_authenticate(struct ieee80211_hdr_3addr * mgmt);
void softap_rx_disassoc(struct ieee80211_hdr_3addr * mgmt);
void softap_rx_deauth(struct ieee80211_hdr_3addr * mgmt);

#if (ENABLE_HK==1) 
void softap_enable_hk_support();
void softap_disable_hk_support();
void softap_set_hkie(const uint8_t *inIE, size_t inIELen);
#endif

#endif


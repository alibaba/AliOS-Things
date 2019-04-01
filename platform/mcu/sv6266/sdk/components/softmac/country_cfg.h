#ifndef _COUNTRY_CFG_H
#define _COUNTRY_CFG_H

#include "soc_types.h"

#define MAX_REG_RULES 6
#define WIFI_2G_BASEFREQ 2412
#define WIFI_5G_BASEFREQ 5005

enum nl80211_reg_rule_flags {
	NL80211_RRF_NO_OFDM		    = 1<<0,
	NL80211_RRF_NO_CCK		    = 1<<1,
	NL80211_RRF_NO_INDOOR		= 1<<2,
	NL80211_RRF_NO_OUTDOOR		= 1<<3,
	NL80211_RRF_DFS			    = 1<<4,
	NL80211_RRF_PTP_ONLY		= 1<<5,
	NL80211_RRF_PTMP_ONLY		= 1<<6,
	NL80211_RRF_PASSIVE_SCAN	= 1<<7,
	NL80211_RRF_NO_IBSS		    = 1<<8,
};

typedef struct {
    u32 ch;
    u32 flag;
}CHANNEL_INFO;

typedef struct {
    u32 start_freq_mhz;
    u32 end_freq_mhz;
    u32 max_bandwidth_mhz;
	u32 max_antenna_gain;
    u32 max_eirp;
	u32 flags;
}FREQ_INFO;

typedef struct {
    u32 country;
    FREQ_INFO freq[MAX_REG_RULES];
}COUNTRY_INFO;

typedef struct {
    u32 ch;
    u32 freq_mhz;
}CHANNEL_FREQ;

int config_channel(COUNTRY_CODE index);

#endif

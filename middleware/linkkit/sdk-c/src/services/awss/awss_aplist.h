/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __AWSS_APLIST__
#define __AWSS_APLIST__

#include <stdint.h>
#include "os.h"
#include "zconfig_protocol.h"

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif

struct ap_info {
	uint8_t auth;
	uint8_t channel;
	uint8_t encry[2];
	uint8_t mac[ETH_ALEN];
	char ssid[ZC_MAX_SSID_LEN];
	signed char rssi;
};

int awss_clear_aplist(void);
int awss_is_ready_clr_aplist(void);
int awss_open_aplist_monitor(void);
int awss_close_aplist_monitor(void);
int awss_init_ieee80211_aplist(void);
int awss_deinit_ieee80211_aplist(void);
int awss_ieee80211_aplist_process(uint8_t *mgmt_header, int len, int link_type,
                                  struct parser_res *res, signed char rssi);

/* storage to store apinfo */
extern struct ap_info *zconfig_aplist;
/* aplist num, less than MAX_APLIST_NUM */
extern uint8_t zconfig_aplist_num;

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif
#endif

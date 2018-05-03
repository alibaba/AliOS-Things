#ifndef _SMART_CONFIG_H
#define _SMART_CONFIG_H

#include "soc_types.h"
#include "idmanage/pbuf.h"
#include "wificonf.h"
#include "wifinetstack.h"
#include "wifi_api.h"
#include "ieee80211.h"
#include "osal/osal.h"

#define SC_OK   1
#define SC_NG   0
#define RECV_ALL (RECV_BEACON | RECV_MGMT | RECV_BROADCAST_DATA | RECV_DATA)
#define RECV_DATA_BCN (RECV_BEACON | RECV_BROADCAST_DATA | RECV_DATA)
#define RECV_DEFAULT (RECV_MGMT | RECV_BROADCAST_DATA)

extern IEEE80211STATUS gwifistatus;

u8 attach_sniffer_cb(SNIFFER_RECVINDEX recv_filter, void (*cb_fn)(packetinfo *), u16 stack_size);
u8 deattach_sniffer_cb(void);
//void smart_config_start(void);
//void smart_config_stop(void);
void start_sniffer_mode(void);
void stop_sniffer_mode(void);
u8 auto_ch_switch_start(u8 time_interval);
u8 auto_ch_switch_stop(void);
void attach_channel_change_cb(void (*ch_cb)(void));
void deattach_channel_change_cb(void);
inline void set_channel(u8 ch);
inline u8 get_current_channel(void);


#endif //_SMART_CONFIG_H

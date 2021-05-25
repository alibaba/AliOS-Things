#ifndef __WIFI_PERFORMANCE_MONITOR_H__
#define __WIFI_PERFORMANCE_MONITOR_H__

#include <platform_stdlib.h>

#ifdef CONFIG_PLATFORM_8721D
#include "rtl8721d.h"
#endif

struct WIFI_TIME {
	//	RX related
	u32 rx_mpdu_time;
	u32 rx_mpdu_time1;
	u32 rx_mpdu_time2;
	
	u32 recv_entry_time;
	
	u32 recv_func_time;
	
	u32 recv_func_prehandle_time;
	
	u32 validate_recv_frame_time;
	u32 validate_recv_frame_time1;
	
	u32 recv_func_posthandle_time;
	u32 recv_func_posthandle_time1;
	u32 recv_func_posthandle_time2;
	u32 recv_func_posthandle_time3;

	u32 process_recv_indicatepkts_time;
		
	u32 rtw_recv_indicatept_time;
	
	u32 rltk_netif_rx_time;
	
	u32 netif_rx_time;
	
	u32 ethernetif_recv_time;	
	u32 rltk_wlan_recv_time;
	u32 netif_input_time;

	//TX related
	u32 wlan_send_time;
	u32 wlan_send_time1;
	u32 wlan_send_time2;
	
	u32 wlan_send_skb_time;
	
	u32 xmit_entry_time;
	
	u32 xmit_time;
	u32 xmit_time1;
	
	u32 xmit_data_time;
	
	u32 pre_xmitframe_time;
	u32 pre_xmitframe_time1;
	u32 pre_xmitframe_time2;
	
	u32 xmitframe_direct_time;
	
	u32 xmitframe_coalesce_time;
	
	u32 dump_xframe_time;
};

#ifdef WIFI_PERFORMANCE_MONITOR
	extern struct WIFI_TIME wifi_time_test;
	void wifi_performance_print();

#define WIFI_MONITOR_TIMER_START(x)	do {x = RTIM_TestTimer_GetCount(); \
										} while(0);

#define WIFI_MONITOR_TIMER_END(x, len)	do { if (len > 500) \
												x = x - RTIM_TestTimer_GetCount(); \
											else \
												x = 0; \
										}while(0);
#else
#define WIFI_MONITOR_TIMER_START(x) 
#define WIFI_MONITOR_TIMER_END(x, len) 
#endif

#endif //__WIFI_PERFORMANCE_MONITOR_H__

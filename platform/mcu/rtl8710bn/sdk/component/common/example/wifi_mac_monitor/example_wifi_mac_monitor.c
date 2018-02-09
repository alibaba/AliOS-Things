/******************************************************************************
 *
 * Copyright(c) 2007 - 2015 Realtek Corporation. All rights reserved.
 *
 *
 ******************************************************************************/
#include <platform_opts.h>
#include <wifi_mac_monitor/example_wifi_mac_monitor.h>
#include <platform/platform_stdlib.h>

typedef int (*mac_monitor_ptr)(unsigned char *phone_macaddr,char real_rssi);
extern mac_monitor_ptr mac_monitor_callback;

int get_station_mac(unsigned char *sta_mac,char rssi)
{
	printf("\n\r%s: %02x:%02x:%02x:%02x:%02x:%02x, Rssi = %d\n",__func__,*(sta_mac)\
			,*(sta_mac+1),*(sta_mac+2),*(sta_mac+3),*(sta_mac+4),*(sta_mac+5),rssi);
	return 0;
}

void example_wifi_mac_monitor(void)
{
	mac_monitor_callback = get_station_mac;
	return;
}


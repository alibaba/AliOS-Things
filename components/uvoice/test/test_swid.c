/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifdef AOS_NETMGR
#include "netmgr.h"
#endif

#include "uvoice_swid.h"


#define SOUND_SSID_FLOW_DISCONNECTED	0
#define SOUND_SSID_FLOW_MONITOR			1
#define SOUND_SSID_FLOW_CONNECTING		2
#define SOUND_SSID_FLOW_CONNECTED		3

static int sound_ssid_flow = SOUND_SSID_FLOW_DISCONNECTED;
static long long ssid_conn_time;
static bool uvoice_swid_initialized = false;

static int lisen_freq_result(char *result)
{
	char *ssid;
	char *passwd;

	//printf("%s: %s\n", __func__, result);

	if (sound_ssid_flow != SOUND_SSID_FLOW_MONITOR) {
		if (sound_ssid_flow == SOUND_SSID_FLOW_CONNECTING) {
			if (aos_now_ms() - ssid_conn_time > 10000)
				sound_ssid_flow = SOUND_SSID_FLOW_MONITOR;
			else
				return 0;
		} else {
			return 0;
		}
	}

	ssid = result;
	passwd = strchr(result, '\n');

	if (!passwd) {
		printf("%s: no enter char\n", __func__);
		return -1;
	}

	//sound_ssid_flow = SOUND_SSID_FLOW_CONNECTING;
	//ssid_conn_time = aos_now_ms();

	*passwd = '\0';
	passwd++;
	printf("ssid: %s, passwd: %s\n", ssid, passwd);
	return 0;

	netmgr_ap_config_t config;
	memcpy(config.ssid, ssid, strlen(ssid) + 1);
	memcpy(config.pwd, passwd, strlen(passwd) + 1); 
	netmgr_set_ap_config(&config);
	printf("connect to '%s'\n", config.ssid);
	netmgr_start(false);

	return 0;
}

static void lisen_freq_start(void)
{
	sound_ssid_flow = SOUND_SSID_FLOW_MONITOR;
	uvoice_swid_start(lisen_freq_result);
}

static void lisen_freq_stop(void)
{
	if (sound_ssid_flow == SOUND_SSID_FLOW_CONNECTING)
		sound_ssid_flow = SOUND_SSID_FLOW_CONNECTED;
	uvoice_swid_stop();
}

void test_swid_handle(int argc, char **argv)
{
	if (!uvoice_swid_initialized) {
		uvoice_swid_init();
		uvoice_swid_initialized = true;
	}

	if (argc >= 2) {
		if (!strcmp(argv[1], "off")) {
			lisen_freq_stop();
		} else {
			lisen_freq_start();
		}
		return;
	}
	lisen_freq_start();
}


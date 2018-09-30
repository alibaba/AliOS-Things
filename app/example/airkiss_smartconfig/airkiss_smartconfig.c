/*
 * Copyright (C) 2018 shaoguoji <752147916@qq.com>
 */

#include <aos/aos.h>
#include <hal/wifi.h>
#include <stdio.h>

#include "airkiss.h"
#include "espressif/esp_libc.h"

hal_wifi_init_type_t sta_info;
extern hal_wifi_module_t aos_wifi_esp8266;

void airkiss_done(AIR_KISS_STATE state, void *pdata)
{
    switch(state) {
        case AIRKISS_STATE_WAIT:
            printf("AIRKISS_STATE_WAIT\n");
            break;
        case AIRKISS_STATE_FIND_CHANNEL:
            printf("AIRKISS_STATE_FIND_CHANNEL\n");
            break;
        case AIRKISS_STATE_GETTING_SSID_PSWD:
            printf("AIRKISS_STATE_GETTING_SSID_PSWD\n");
            hal_wifi_init_type_t *result = pdata;
            
            sta_info.wifi_mode = STATION;
            sta_info.dhcp_mode = DHCP_CLIENT;
            strncpy(sta_info.wifi_ssid, result->wifi_ssid, sizeof(sta_info.wifi_ssid) - 1);
            strncpy(sta_info.wifi_key, result->wifi_key, sizeof(sta_info.wifi_key) - 1);
            hal_wifi_start(&aos_wifi_esp8266, &sta_info);
            
            break;
        case AIRKISS_STATE_COMPLETE:
            printf("AIRKISS_STATE_COMPLETE\n");
            break;
    }
    
}

int application_start(int argc, char *argv[])
{
    printf("AliOS Airkiss Test Start...\r\n");
    start_airkiss(airkiss_done);
    aos_loop_run();

    return 0;
}


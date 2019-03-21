/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "stdio.h"
#include <hal/wifi.h>

#include <ulocation/ulocation.h>
#include <uloc_hal.h>

#if defined(DEV_SAL_MK3060)
#define ULOC_WIFI
#endif

#define ULOCATION_TRACE(...)                                     \
    do {                                                         \
        HAL_Printf("\033[1;32;40m%s.%d: ", __func__, __LINE__);  \
        HAL_Printf(__VA_ARGS__);                                 \
        HAL_Printf("\033[0m\r\n");                               \
    } while (0)

static char digit2char(uint8_t u8_arg)
{
    uint8_t v = u8_arg & 0x0f;

    if (v >=0 && v <= 9){
        return (char)('0' + v);
    }else{
        return (char)('a' + v - 10);
    }

    return 0;
}

static void conver_ap_info_fmt(char *out_str, const hal_wifi_link_stat_t *p_ap_info)
{
    ULOCATION_TRACE("conver_ap_info_fmt \n");

    if (!out_str || !p_ap_info){
        ULOCATION_TRACE("%s %d fails", __func__, __LINE__);
        return;
    }

    /* 4c:48:da:26:ea:d9,-56,alibaba-inc */

    uint8_t *bssid = p_ap_info->bssid;
    char *p_out = out_str;

    for (int idx = 0; idx < 6; idx++){
        *p_out++ = digit2char((bssid[idx]) >> 4);
        *p_out++ = digit2char((bssid[idx]) & 0x0f);

        if (idx < 5){
            *p_out++ = ':';
        }else{
            *p_out++ = ',';
        }
    }

    sprintf(p_out, "%d,%s", p_ap_info->wifi_strength, p_ap_info->ssid);
}


int uloc_hal_get_wifi(char **mmac, char **macs)
{
    int rlt = -1;
    *mmac = NULL;
    *macs = NULL;

#ifdef ULOC_WIFI
    static char mmac_buf[128];
    memset(mmac_buf, 0, sizeof(mmac_buf));

    hal_wifi_link_stat_t current_ap;
    hal_wifi_get_link_stat(NULL, &current_ap);

    conver_ap_info_fmt(mmac_buf, &current_ap);

    *mmac = mmac_buf;

    static char macs_buf[512];
    memset(macs_buf, 0, sizeof(macs_buf));

    hal_wifi_link_stat_t out_stat[10];
    int ap_num = 10;
    hal_wifi_scan_ap_list(NULL, out_stat, &ap_num);

    ULOCATION_TRACE("hal_wifi_scan_ap_list returns %d aps info\n", ap_num);

    for (int idx = 0; idx < ap_num; idx++){
        ULOCATION_TRACE("test_wifi result for idx=%d: wifi_strength: %d, ssid: %s, channel %d, bssid: %d %d %d %d %d %d",
                       idx,
                       out_stat[idx].wifi_strength,
                       (char *)out_stat[idx].ssid,
                       out_stat[idx].channel,
                       out_stat[idx].bssid[0],
                       out_stat[idx].bssid[1],
                       out_stat[idx].bssid[2],
                       out_stat[idx].bssid[3],
                       out_stat[idx].bssid[4],
                       out_stat[idx].bssid[5]);

        if (memcmp(out_stat[idx].bssid, current_ap.bssid, sizeof(current_ap.bssid)) != 0){
            if (idx != 0){
                snprintf(macs_buf + strlen(macs_buf), sizeof(macs_buf) - strlen(macs_buf), "%s", "|");
            }

            char single_ap_buf[128];
            memset(single_ap_buf, 0, sizeof(single_ap_buf));

            conver_ap_info_fmt(single_ap_buf, &out_stat[idx]);
            ULOCATION_TRACE("MAIN AP info for ap %d: %s", idx, single_ap_buf);

            snprintf(macs_buf + strlen(macs_buf), sizeof(macs_buf) - strlen(macs_buf), "%s", single_ap_buf);
        }

    }

    *macs = macs_buf;

    rlt = 0;
#endif

    return rlt;
}


/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "blecfg_pub.h"

#include "aos/kv.h"

#include "lwip/icmp.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "lwip/prot/ip.h"
#include "lwip/inet_chksum.h"

#define LIMIT_WIFI_SSID         256
#define LIMIT_WIFI_PASSWD       256

typedef struct {
    uint8_t ssid[LIMIT_WIFI_SSID];
    uint8_t passwd[LIMIT_WIFI_PASSWD];
} wifi_info;

static wifi_info s_wifi_info;

static int32_t BleCfg_wifi_kv_save(char *ssid, char *passwd)
{
    int32_t ret;
    int len;
    char wifi_ssid[LIMIT_WIFI_SSID];
    char wifi_passwd[LIMIT_WIFI_PASSWD];

    if ( ssid == NULL || strlen(ssid) >= LIMIT_WIFI_SSID
      || passwd == NULL || strlen(passwd) >= LIMIT_WIFI_PASSWD ) {
        BLECFG_LOG_ERROR("[%s]: input para error!\r\n", __func__);
        return -1;
    }

    len = LIMIT_WIFI_SSID;
    ret = aos_kv_get("wifi_ssid", wifi_ssid, &len);
    if(ret){
        wifi_ssid[0] = 0;
    }
    len = LIMIT_WIFI_PASSWD;
    ret = aos_kv_get("wifi_passwd", wifi_passwd, &len);
    if(ret){
        wifi_passwd[0] = 0;
    }

    if ( strcmp(wifi_ssid, ssid) == 0
      && strcmp(wifi_passwd, passwd) == 0 ) {
        /* No need to save */
        BLECFG_LOG_INFO("[%s]: kv already saved.\r\n", __func__);
        return 0;
    }

    ret = aos_kv_set("wifi_ssid", ssid, strlen(ssid)+1, 1);
    if ( ret ){
        BLECFG_LOG_ERROR("[%s]: set 'wifi_ssid' fail! ret = %d\r\n", __func__, ret);
        return ret;
    }

    ret = aos_kv_set("wifi_passwd", passwd, strlen(passwd)+1, 1);
    if ( ret ) {
        BLECFG_LOG_ERROR("[%s]: set 'wifi_passwd' fail! ret = %d\r\n", __func__, ret);
        return ret;
    }

    return 0;
}

static int32_t BleCfg_wifi_kv_load(char *ssid, char *passwd)
{
    int32_t ret;
    int len;

    len = LIMIT_WIFI_SSID;
    ret = aos_kv_get("wifi_ssid", ssid, &len);
    if(ret){
        //BLECFG_LOG_INFO("%s: wifi_ssid not find", __func__);
        return ret;
    }
    BLECFG_LOG_INFO("%s: wifi_ssid '%s'\r\n", __func__, ssid);

    len = LIMIT_WIFI_PASSWD;
    ret = aos_kv_get("wifi_passwd", passwd, &len);
    if(ret){
        //BLECFG_LOG_INFO("%s: wifi_passwd not find", __func__);
        return ret;
    }
    BLECFG_LOG_INFO("%s: wifi_passwd '%s'\r\n", __func__, passwd);

    return 0;
}

/* wifi_info_stat_t: WIFI_INFO_NULL/WIFI_INFO_SAVED -> WIFI_INFO_AVAILABLE */
BLECFG_STATE BleCfg_wifi_set(char *ssid, char *passwd)
{
    wifi_info *wifi_info = &s_wifi_info;

    if ( strlen(ssid) + 1 >= sizeof(wifi_info->ssid)
      || strlen(passwd) + 1 >= sizeof(wifi_info->passwd) ) {
        return BLECFG_COMMON_FAILED;
    }

    strncpy(wifi_info->ssid, ssid, sizeof(wifi_info->ssid));
    strncpy(wifi_info->passwd, passwd, sizeof(wifi_info->passwd));

    return BLECFG_SUCCESS;
}

BLECFG_STATE BleCfg_wifi_connect()
{
    int32_t netmgr_handle, ret;
    wifi_info *wifi_info = &s_wifi_info;
    netmgr_connect_params_t netmgr_params;

    memset(&netmgr_params, 0, sizeof(netmgr_connect_params_t));
    strncpy(netmgr_params.params.wifi_params.ssid, wifi_info->ssid, NETMGR_SSID_MAX_LEN);
    strncpy(netmgr_params.params.wifi_params.pwd, wifi_info->passwd, NETMGR_PWD_MAX_LEN);
    netmgr_params.params.wifi_params.timeout = 18000;

    netmgr_handle = netmgr_get_dev("/dev/wifi0");
    ret = netmgr_connect(netmgr_handle, &netmgr_params);
    BLECFG_LOG_INFO("%s: netmgr_connect return %d\r\n", __func__, ret);
    if ( ret != 0 ) {
        return BLECFG_COMMON_FAILED;
    }

    return BLECFG_SUCCESS;
}

/* wifi_info_stat_t: WIFI_INFO_AVAILABLE -> WIFI_INFO_SAVED */
BLECFG_STATE BleCfg_wifi_save()
{
    int32_t ret;
    wifi_info *wifi_info = &s_wifi_info;

    ret = BleCfg_wifi_kv_save(wifi_info->ssid, wifi_info->passwd);
    if ( ret != 0 ) {
        return BLECFG_COMMON_FAILED;
    }

    return BLECFG_SUCCESS;
}


/* wifi_info_stat_t: WIFI_INFO_NULL -> WIFI_INFO_AVAILABLE */
BLECFG_STATE BleCfg_wifi_restore()
{
    int32_t ret;
    wifi_info *wifi_info = &s_wifi_info;

    ret = BleCfg_wifi_kv_load(wifi_info->ssid, wifi_info->passwd);
    if ( ret != 0 ) {
        return BLECFG_COMMON_FAILED;
    }

    return BLECFG_SUCCESS;
}

BLECFG_STATE BleCfg_netmgr_init(BleCfg_dhcp_cb dhcp_callback)
{
    int32_t ret;

    memset(&s_wifi_info, 0, sizeof(s_wifi_info));

    /* netmgr init */
    event_service_init(NULL);
    ret = netmgr_service_init(NULL);
    if(ret) {
        BLECFG_LOG_ERROR("%s: net mgr init, ret = %x\r\n", __func__, ret);
        return BLECFG_COMMON_FAILED;
    }

    event_subscribe(EVENT_NETMGR_DHCP_SUCCESS, dhcp_callback, NULL);

    return BLECFG_SUCCESS;
}


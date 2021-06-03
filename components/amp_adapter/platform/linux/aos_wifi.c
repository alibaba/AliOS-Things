/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <netmgr.h>
#include <netmgr_wifi.h>
#include <aos/list.h>
#include <uservice/uservice.h>


#define KV_VAL_LENGTH       32

struct hdl_info;
typedef struct hdl_info {
    slist_t next;
    char* file_name;
    netmgr_hdl_t hdl;
    netmgr_type_t type;
} hdl_info_t;

#define TAG "netmgr_service"

static slist_t g_hdl_list_head;

int netmgr_wifi_set_ip_mode(netmgr_wifi_ip_mode_t mode);
netmgr_wifi_ip_mode_t netmgr_wifi_get_ip_mode(void);
int netmgr_wifi_get_ip_stat(char* ip_addr, char* mask, char* gw, char* dns_server);
void netmgr_wifi_auto_reconnect(int enable);
int  netmgr_wifi_get_wifi_state();
int netmgr_wifi_set_static_ip_stat(const char* ip_addr, const char* mask, const char* gw, const char* dns_server);

int event_service_init(utask_t *task)
{
    return 0;
}

static int add_hdl_info(int fd, char* file_name, netmgr_type_t type)
{
    hdl_info_t* cur;

    printf("%s:%d\n", __func__, __LINE__);
    cur = malloc(sizeof(hdl_info_t));
    if(cur == NULL) {
        return -1;
    }
    memset(cur, 0, sizeof(hdl_info_t));
    printf("%s:%d\n", __func__, __LINE__);

    cur->hdl = fd;
    cur->file_name = strdup(file_name);
    cur->type = type;

    slist_add_tail(&cur->next, &g_hdl_list_head);

    return 0;
}

static int del_hdl_by_name(char* file_name)
{
    hdl_info_t* cur;
    int found = 0;

    slist_for_each_entry(&g_hdl_list_head, cur, hdl_info_t, next) {
        if(strcmp(cur->file_name, file_name) == 0) {
            found = 1;
            break;
        }
    }

    if(1 == found) {
        free(cur->file_name);
        slist_del(&cur->next, &g_hdl_list_head);
        return 0;
    } else {
        return -1;
    }
}

static netmgr_hdl_t  get_hdl_by_name(char* file_name)
{
    hdl_info_t* cur;
    int found = 0;

    slist_for_each_entry(&g_hdl_list_head, cur, hdl_info_t, next) {
        if(strcmp(cur->file_name, file_name) == 0) {
            found = 1;
            break;
        }
    }

    if(1 == found) {
        return cur->hdl;
    } else {
        return -1;
    }
}

/**
 * @brief  create & init the netmgr uservice
 * @param  [in] task
 * @return
 */
int netmgr_service_init(utask_t *task)
{
    return 0;
}

/**
 * @brief  destroy & uninit the netmgr uservice
 * @return
 */
void netmgr_service_deinit()
{

}

int netmgr_add_dev(const char* name)
{
    return 0;
}

netmgr_hdl_t netmgr_get_dev(const char* name)
{
    return get_hdl_by_name((char *)name);
}

int netmgr_set_ifconfig(netmgr_hdl_t hdl, netmgr_ifconfig_info_t* info)
{
    return 0;
}

int netmgr_get_ifconfig(netmgr_hdl_t hdl, netmgr_ifconfig_info_t* info)
{
    if(info != NULL) {
        info->dhcp_en = true;
        //return netmgr_wifi_get_ip_stat(info->ip_addr, info->mask, info->gw, info->dns_server);
        info->ip_addr[0] = 1;
        info->ip_addr[2] = 2;
        return 0;
    } else {
        return -1;
    }
    return 0;
}

void netmgr_set_auto_reconnect(netmgr_hdl_t hdl, bool enable)
{

}


int netmgr_get_config(netmgr_hdl_t hdl, netmgr_config_t* config)
{

    return 0;
}

int netmgr_del_config(netmgr_hdl_t hdl, netmgr_del_config_t* config)
{
    return 0;
}


int netmgr_get_state(netmgr_hdl_t hdl)
{
    return 0;
}

int netmgr_connect(netmgr_hdl_t hdl, netmgr_connect_params_t* params)
{
    aos_printf("wifi connect begin...\n");
    aos_printf("ssid %s, password %s\n", params->params.wifi_params.ssid, params->params.wifi_params.pwd);
    aos_msleep(5000);
    aos_printf("wifi connected\n");
    return 0;
}

int netmgr_disconnect(netmgr_hdl_t hdl)
{
    return 0;
}

int netmgr_save_config(netmgr_hdl_t hdl)
{
    return 0;
}

int netmgr_set_connect_params(netmgr_hdl_t hdl, netmgr_connect_params_t* params)
{
    //TODO
    return 0;
}

int netmgr_set_msg_cb(netmgr_hdl_t hdl, netmgr_msg_cb_t cb)
{

    return 0;
}

int netmgr_del_msg_cb(netmgr_hdl_t hdl, netmgr_msg_cb_t cb)
{

    return 0;
}

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "py/obj.h"
#include "py/runtime.h"
#include "py/builtin.h"
#include "py/stackctrl.h"

#include "netmgr.h"
#include <uservice/uservice.h>
#include <uservice/eventid.h>

#include "lwip/icmp.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "lwip/prot/ip.h"
#include "lwip/inet_chksum.h"
#include <aos/ble.h>
#include <atomic.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/gatt.h>
#include <bluetooth/uuid.h>
#include <netmgr.h>
#include <netmgr_wifi.h>
#include <ble_netconfig.h>

#include "HaasLog.h"

#define DAEMON_STACK_SIZE 4096

#define NETCFG_PING_ID 0xAFAF
#define NETCFG_PING_DATA_SIZE 32

// BLE_NetCfg_init set WIFI DEV PATH
#define WIFI_DEV_PATH "/dev/wifi0"

typedef struct amp_wifi_info {
    char ssid[128];
    char mac[6];
    char ip[16];
    int rssi;
} amp_wifi_info_t;

static void ble_ping_prepare_echo(struct icmp_echo_hdr *iecho, uint16_t len)
{
    int32_t i;
    int32_t data_len = len - sizeof(struct icmp_echo_hdr);
    static uint16_t ping_seq_num = 0;

    ICMPH_TYPE_SET(iecho, ICMP_ECHO);
    ICMPH_CODE_SET(iecho, 0);
    iecho->chksum = 0;
    iecho->id = NETCFG_PING_ID;
    iecho->seqno = lwip_htons(++ping_seq_num);

    for (i = 0; i < data_len; i++)
        ((char *)iecho)[sizeof(struct icmp_echo_hdr) + i] = (char)i;

    iecho->chksum = inet_chksum(iecho, len);
}

static void ping_thread(void *args)
{
    int sock;
    int len;
    ip_addr_t ip_addr;
    struct sockaddr_storage to;
    uint8_t res = BLE_NETCFG_PING_SUCCESS;
    int32_t temp;
    char host_name[15] = "www.aliyun.com";
    struct addrinfo hints;
    struct addrinfo *addr;
    struct sockaddr_in *to4;
    struct icmp_echo_hdr *iecho;
    int32_t ping_size = sizeof(struct icmp_echo_hdr) + NETCFG_PING_DATA_SIZE;
    uint8_t buf[32] = {0};
    int32_t from_len = 0;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_CANONNAME;
    hints.ai_protocol = 0;

    temp = lwip_getaddrinfo(host_name, NULL, &hints, &addr);
    if (temp != 0)
        return;

    if (((struct sockaddr *)(addr->ai_addr))->sa_family == AF_INET) {
        struct sockaddr_in *addr4_in = (struct sockaddr_in *)(addr->ai_addr);
        inet_addr_to_ipaddr(ip_2_ip4(&ip_addr), &addr4_in->sin_addr);
        IP_SET_TYPE_VAL(ip_addr, IPADDR_TYPE_V4);
    }

    freeaddrinfo(addr);

    if (IP_IS_V4(ip_addr)) {
        to4 = (struct sockaddr_in *)&to;
        to4->sin_len = sizeof(to4);
        to4->sin_family = AF_INET;
        inet_addr_from_ipaddr(&to4->sin_addr, ip_2_ip4(&ip_addr));
    }

    sock = lwip_socket(AF_INET, SOCK_RAW, IP_PROTO_ICMP);

    iecho = (struct icmp_echo_hdr *)aos_malloc(ping_size);
    ble_ping_prepare_echo(iecho, (uint16_t)ping_size);

    len = lwip_sendto(sock, iecho, ping_size, 0, (struct sockaddr *)&to, sizeof(to));
    aos_free(iecho);

    if (len > 0)
        len = lwip_recvfrom(sock, buf, sizeof(buf), 0, (struct sockaddr *)&to, (socklen_t *)&from_len);

    if (len <= 0)
        res = BLE_NETCFG_PING_FAIL;

    LOG_D("%s: res %d\r\n", __func__, res);

    lwip_close(sock);
    BLE_NetCfg_notificate(&res, 1);

    // res = BLE_NetCfg_wifi_info(&wifi_status.wifi_ssid, &wifi_status.wifi_password);
    // if (res == BLE_NETCFG_SUCCESS) {
        /* 需要记录WiFi信息 */
        // demo_wifi_info_set(wifi_ssid, wifi_password);
        // LOG_D("%s:%d BLE Net Config Get Wifi SSID = %s, PASSWD=%s\n", __func__, __LINE__, wifi_status.wifi_ssid, wifi_status.wifi_password);
    // }

    BLE_NetCfg_stop();
}

int ble_netcfg_callback_function(BLE_NETCFG_EVENT event, BLE_NETCFG_STATE state, void *event_data)
{
    uint8_t res = BLE_NETCFG_DHCP_SUCCESS;
    uint8_t ipaddr[64] = {'0'};

    switch (event) {
        case BLE_NETCFG_EVENT_GOT_IP:
            printf("Got IP\r\n");
            BLE_NetCfg_notificate(&res, 1);

            netmgr_wifi_get_ip_addr(ipaddr);
            BLE_NetCfg_notificate(ipaddr, strlen((char *)ipaddr));

            aos_task_new("ping_daemon", ping_thread, NULL, DAEMON_STACK_SIZE);
            break;
        default:
            return -1;
    }

    return 0;
}

STATIC mp_obj_t ble_netconfig_init(void)
{
    LOG_D("%s:%d function entry\n", __func__, __LINE__);
    int ret;
    ret = BLE_NetCfg_init(ble_netcfg_callback_function);
    if (ret) {
        LOG_E("error: BLE Net Config initial! ret %x\r\n", ret);
        return mp_const_false;
    } else {
        LOG_D("%s:%d BLE Net Config Init OK!\n", __func__, __LINE__);
        return mp_const_true;
    }
}
MP_DEFINE_CONST_FUN_OBJ_0(ble_netconfig_obj_init, ble_netconfig_init);

STATIC mp_obj_t ble_netconfig_start(void)
{
    LOG_D("%s:%d function entry\n", __func__, __LINE__);
    int ret;
    ret = BLE_NetCfg_start();
    if (ret) {
        LOG_E("error: BLE Net Config start! ret %x\r\n", ret);
        return mp_const_false;
    } else {
        LOG_D("%s:%d BLE Net Config Start OK!\n", __func__, __LINE__);
        return mp_const_true;
    }
}
MP_DEFINE_CONST_FUN_OBJ_0(ble_netconfig_obj_start, ble_netconfig_start);

STATIC mp_obj_t ble_netconfig_stop(void)
{
    LOG_D("%s:%d function entry\n", __func__, __LINE__);
    int ret;
    ret = BLE_NetCfg_stop();
    if (ret) {
        LOG_E("error: BLE Net Config stop! ret %x\r\n", ret);
        return mp_const_false;
    } else {
        LOG_D("%s:%d BLE Net Config Stop OK!\n", __func__, __LINE__);
        return mp_const_true;
    }
}
MP_DEFINE_CONST_FUN_OBJ_0(ble_netconfig_obj_stop, ble_netconfig_stop);


STATIC mp_obj_t ble_netconfig_notificate(void)
{
    LOG_D("%s:%d function entry\n", __func__, __LINE__);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(ble_netconfig_obj_notificate, ble_netconfig_notificate);

// get wifi info
int ble_netconfig_get_wifi_info(amp_wifi_info_t *wifi_info)
{
    LOG_D("%s:%d function entry\n", __func__, __LINE__);
    netmgr_hdl_t hdl;
    netmgr_config_t config;
    netmgr_ifconfig_info_t info;
    int ap_num;
    int used_ap;                                         /**< ap that is used in the array */

    if((hdl = netmgr_get_dev(WIFI_DEV_PATH)) == -1) {
        printf(" %s :  %d \r\n",__func__,__LINE__);
        return -1;
    }

	memset(&info, 0, sizeof(info));
    if(netmgr_get_ifconfig(hdl, &info) == -1) {
        printf(" %s :  %d \r\n",__func__,__LINE__);
        return -1;
    }

	memset(&config, 0, sizeof(config));
    if(netmgr_get_config(hdl, &config) == -1) {
        printf(" %s :  %d \r\n",__func__,__LINE__);
        return -1;
    }
    ap_num = config.config.wifi_config.ap_num;
    used_ap = config.config.wifi_config.used_ap;

    if((ap_num < MAX_AP_CONFIG_NUM) && (used_ap < ap_num)) {
	    memset(wifi_info->ssid, 0, sizeof(wifi_info->ssid));
	    strncpy(wifi_info->ssid, config.config.wifi_config.config[used_ap].ssid, sizeof(wifi_info->ssid) - 1);
    } else {
        printf(" %s :  %d \r\n",__func__,__LINE__);
        return -1;
    }

    snprintf(wifi_info->ip, sizeof(wifi_info->ip), "%s", info.ip_addr);
	memcpy(wifi_info->mac, info.mac, sizeof(wifi_info->mac));
	wifi_info->rssi = info.rssi;

    return 0;
}

STATIC mp_obj_t ble_netconfig_get_wifi_status(void) {
    amp_wifi_info_t wifi_info ;
    int ret = ble_netconfig_get_wifi_info(&wifi_info);
    if(ret != 0){
        printf("modnetmgr: amp_get_wifi_info failed \n ");
    }

    mp_obj_t dict = mp_obj_new_dict(4);
    mp_obj_dict_store(MP_OBJ_FROM_PTR(dict),mp_obj_new_str("SSID",4),mp_obj_new_str(wifi_info.ssid,strlen(wifi_info.ssid)));
    mp_obj_dict_store(MP_OBJ_FROM_PTR(dict),mp_obj_new_str("MAC",3),mp_obj_new_str(wifi_info.mac,6));
    mp_obj_dict_store(MP_OBJ_FROM_PTR(dict),mp_obj_new_str("IP",2),mp_obj_new_str(wifi_info.ip,strlen(wifi_info.ip)));
    mp_obj_dict_store(MP_OBJ_FROM_PTR(dict),mp_obj_new_str("RSSI",4),mp_obj_new_int(wifi_info.rssi));

    return dict;
}
MP_DEFINE_CONST_FUN_OBJ_0(ble_netconfig_obj_get_wifi_info, ble_netconfig_get_wifi_status);

STATIC mp_obj_t ble_netconfig_get_status(void)
{
    LOG_D("%s:%d function entry\n", __func__, __LINE__);
    netmgr_hdl_t hdl;
    hdl = netmgr_get_dev(WIFI_DEV_PATH);

    int status = netmgr_get_state(hdl);
    printf(" status is %d \n",status);
    if(status == 5){
        return mp_const_true ;
    }else{
        return mp_const_false ;
    }

    return mp_const_false ;

}
MP_DEFINE_CONST_FUN_OBJ_0(ble_netconfig_obj_get_status, ble_netconfig_get_status);

STATIC mp_obj_t ble_netconfig_device_info(void)
{
    LOG_D("%s:%d function entry\n", __func__, __LINE__);
    // BLE_NETCFG_STATE ret;
    // char *pk;
    // char *dn;
    // char *ds;
    // ret = BLE_NetCfg_dev_info(&pk, &dn, &ds);
    // if(ret == BLE_NETCFG_SUCCESS) {
    //     LOG_D("%s:%d function BLE_NETCFG_SUCCESS\n", __func__, __LINE__);
    // } else {
    //     LOG_D("%s:%d function BLE_NETCFG_COMMON_FAILED\n", __func__, __LINE__);
    // }
}
MP_DEFINE_CONST_FUN_OBJ_0(ble_netconfig_obj_device_info, ble_netconfig_device_info);

STATIC const mp_rom_map_elem_t blenetconfig_module_globals_table[] = {
    {MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_ble_netconfig)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_init), MP_ROM_PTR(&ble_netconfig_obj_init)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_start), MP_ROM_PTR(&ble_netconfig_obj_start)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_stop), MP_ROM_PTR(&ble_netconfig_obj_stop)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_notificate), MP_ROM_PTR(&ble_netconfig_obj_notificate)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_getInfo), MP_ROM_PTR(&ble_netconfig_obj_get_wifi_info)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_getStatus), MP_ROM_PTR(&ble_netconfig_obj_get_status)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_device), MP_ROM_PTR(&ble_netconfig_obj_device_info)},
};

STATIC MP_DEFINE_CONST_DICT(blenetconfig_module_globals, blenetconfig_module_globals_table);

const mp_obj_module_t blenetconfig_module = {
    .base = {&mp_type_module},
    .globals = (mp_obj_dict_t *)&blenetconfig_module_globals,
};



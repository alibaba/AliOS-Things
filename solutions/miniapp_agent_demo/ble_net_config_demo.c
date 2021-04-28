/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
/**
 * @file main.c
 *
 * This file includes the entry code of link sdk related demo
 *
 */

#include <string.h>
#include <stdio.h>
#include <aos/kernel.h>
#include "netmgr.h"
#include <uservice/uservice.h>
#include <uservice/eventid.h>

#include "lwip/icmp.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "lwip/prot/ip.h"
#include "lwip/inet_chksum.h"
#include "ble_netconfig.h"

#define DAEMON_STACK_SIZE 4096

#define NETCFG_PING_ID 0xAFAF
#define NETCFG_PING_DATA_SIZE 32

extern int netmgr_wifi_get_ip_addr(char *ip_address);
extern void linksdk_thread(void *args);
extern int32_t demo_wifi_info_set(char *ssid, char *password);
extern int32_t demo_wifi_info_get(char **ssid, char **password);

static void ping_prepare_echo(struct icmp_echo_hdr *iecho, uint16_t len)
{
    int32_t i;
    int32_t data_len = len - sizeof(struct icmp_echo_hdr);
    static uint16_t ping_seq_num = 0;

    ICMPH_TYPE_SET(iecho, ICMP_ECHO);
    ICMPH_CODE_SET(iecho, 0);
    iecho->chksum = 0;
    iecho->id = NETCFG_PING_ID;
    iecho->seqno = lwip_htons(++ping_seq_num);

    for (i = 0; i < data_len; i++) {
        ((char *)iecho)[sizeof(struct icmp_echo_hdr) + i] = (char)i;
    }

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
    aos_status_t ret;
    aos_task_t task_main;
    char *wifi_ssid;
    char *wifi_password;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_CANONNAME;
    hints.ai_protocol = 0;

    temp = lwip_getaddrinfo(host_name, NULL, &hints, &addr);
    if (temp != 0) {
        return;
    }

    if(((struct sockaddr*)(addr->ai_addr))->sa_family == AF_INET) {
        struct sockaddr_in *addr4_in = (struct sockaddr_in*)(addr->ai_addr);
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
    ping_prepare_echo(iecho, (uint16_t)ping_size);

    len = lwip_sendto(sock, iecho, ping_size, 0, (struct sockaddr *)&to, sizeof(to));
    aos_free(iecho);

    if (len > 0) {
        len = lwip_recvfrom(sock, buf, sizeof(buf), 0, (struct sockaddr *)&to, (socklen_t *)&from_len);
    }

    if (len <= 0) {
        res = BLE_NETCFG_PING_FAIL;
    }

    printf("%s: res %d\r\n", __func__, res);

    lwip_close(sock);
    BLE_NetCfg_notificate(&res, 1);
    //BLE_NetCfg_stop();

    res = BLE_NetCfg_wifi_info(&wifi_ssid, &wifi_password);
    if (res == BLE_NETCFG_SUCCESS) {            
        /* 信息记录到kv中，固化信息 */
        demo_wifi_info_set(wifi_ssid, wifi_password);
        printf("Wifi info set to kv\n");
    }

    /* 配网流程结束，开启linkSDK连云流程 */
    ret = aos_task_create(&task_main, "linksdk_demo", linksdk_thread,
                    NULL, NULL, 6048, AOS_DEFAULT_APP_PRI, AOS_TASK_AUTORUN);
    if (ret < 0) {
        printf("create 'linksdk_demo' task failed, ret = %ld \r\n", ret);
    }
}

int ble_netcfg_callback(BLE_NETCFG_EVENT event, BLE_NETCFG_STATE state, void *event_data)
{
    aos_status_t ret;
    uint8_t res = BLE_NETCFG_DHCP_SUCCESS;
    uint8_t ipaddr[64] = {'0'};

    printf("'ble_netcfg_callback' event = %d \r\n", event);

    switch (event) {
        case BLE_NETCFG_EVENT_GOT_IP:
            printf("Got IP\r\n");
            BLE_NetCfg_notificate(&res, 1);

            netmgr_wifi_get_ip_addr(ipaddr);
            BLE_NetCfg_notificate(ipaddr, strlen((char *)ipaddr));

            ret = aos_task_new("ping_daemon", ping_thread, NULL, DAEMON_STACK_SIZE);
            if (ret < 0) {
                printf("create 'ping_daemon' task failed, ret = %ld \r\n", ret);
            }

            break;
        default:
            return -1;
    }

    return 0;
}

int application_start(int argc, char *argv[])
{
    int ret;
    char *wifi_ssid;
    char *wifi_password;

    aos_set_log_level(AOS_LL_DEBUG);
    
    ret = BLE_NetCfg_init(ble_netcfg_callback);
    if (ret) {
        printf("error: BLE Net Config initial! ret %x\r\n", ret);
    }
    ret = BLE_NetCfg_start();
    if (ret) {
        printf("error: BLE Net Config start! ret %x\r\n", ret);
    }

    aos_msleep(100);

    ret = demo_wifi_info_get(&wifi_ssid, &wifi_password);
    if (ret == 0) {            
         printf("Wifi info get from kv\n");
         BLE_NetCfg_wifi_set(wifi_ssid, wifi_password);
    }

    while (1) {
        aos_msleep(1000);
    };

    return 0;
}

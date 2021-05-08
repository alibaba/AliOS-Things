/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#if AOS_COMP_CLI
#include "aos/cli.h"
#endif
#include "uservice/uservice.h"
#include "uservice/eventid.h"

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
    BLE_NetCfg_stop();
}

int ble_netcfg_callback(BLE_NETCFG_EVENT event, BLE_NETCFG_STATE state, void *event_data)
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

static void netcfg_comp_example(int argc, char **argv)
{
    int ret;

    ret = BLE_NetCfg_init(ble_netcfg_callback);
    if (ret) {
        printf("error: BLE Net Config initial! ret %x\r\n", ret);
    }
    ret = BLE_NetCfg_start();
    if (ret) {
        printf("error: BLE Net Config start! ret %x\r\n", ret);
    }
}

#if AOS_COMP_CLI
ALIOS_CLI_CMD_REGISTER(netcfg_comp_example, netcfg_example, netcfg component base example);
#endif

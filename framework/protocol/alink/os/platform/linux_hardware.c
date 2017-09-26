/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>

#include <net/if.h>       // struct ifreq
//inet_ntoa
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>

#include "platform.h"
#include "platform_config.h"

#define ALINK_CONFIG_FILE_NAME      "alinkconfig.db"

#define IP_MULTICAST_NET_ADDR   "224.0.0.0"
#define IP_MULTICAST_NET_MASK   "240.0.0.0"
#define ROUTER_INFO_PATH        "/proc/net/route"
#define ROUTER_RECORD_SIZE      256

/*
route table format:
#cat /proc/net/route
Iface   Destination Gateway     Flags   RefCnt  Use Metric  Mask        MTU Window  IRTT
eth0    00000000    010AA8C0    0003    0   0   0   00000000    0   0   0
eth1    0000FEA9    00000000    0001    0   0   1000    0000FFFF    0   0   0
*/
char *platform_get_multicast_ifname(char *ifname, int ifname_size)
{
    FILE *fp = NULL;
    char line[ROUTER_RECORD_SIZE] = {0};
    char iface[IFNAMSIZ] = {0};
    char best_route_iface[IFNAMSIZ] = {0};
    char *result = NULL;
    unsigned int best_route_mask = 0;
    unsigned int multicast_net, multicast_mask;
    unsigned int destination, gateway, flags, mask;
    unsigned int refCnt, use, metric, mtu, window, irtt;

    fp = fopen(ROUTER_INFO_PATH, "r");
    if (fp == NULL) {
        perror("fopen");
        return result;
    }

    char *buff = fgets(line, sizeof(line), fp);
    if (buff == NULL) {
        perror("fgets");
        goto out;
    }

    inet_pton(AF_INET, IP_MULTICAST_NET_ADDR, &multicast_net);
    inet_pton(AF_INET, IP_MULTICAST_NET_MASK, &multicast_mask);
    while (fgets(line, sizeof(line), fp)) {
        if (11 !=
            sscanf(line, "%s %08x %08x %x %d %d %d %08x %d %d %d",
                   iface, &destination, &gateway, &flags, &refCnt, &use,
                   &metric, &mask, &mtu, &window, &irtt)) {
            perror("sscanf");
            continue;
        }

        /*default route */
        if ((destination == 0) && (mask == 0)
            && (best_route_iface[0] == '\0')) {
            best_route_mask = mask;
            strncpy(best_route_iface, iface,
                    sizeof(best_route_iface) - 1);
            continue;
        }

        /*default route & multicast route */
        if ((multicast_net & mask) == (destination & mask) &&
            best_route_mask < mask) {
            best_route_mask = mask;
            strncpy(best_route_iface, iface,
                    sizeof(best_route_iface) - 1);
        }
    }

    if (best_route_iface[0] != '\0') {
        strncpy(ifname, best_route_iface, ifname_size - 1);
        result = ifname;
    }

out:
    if (fp) {
        fclose(fp);
    }
    return result;
}

char *platform_wifi_get_mac(char mac_str[PLATFORM_MAC_LEN])
{
    struct ifreq ifreq;
    int sock;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        return NULL;
    }

    strcpy(ifreq.ifr_name, IFNAME);
    if (ioctl(sock, SIOCGIFHWADDR, &ifreq) == 0) {
        goto get_mac;
    }

    int i;
    for (i = 0; i < 10; i++) {
        ifreq.ifr_ifindex = i;
        if (ioctl(sock, SIOCGIFNAME, &ifreq) < 0) {
            continue;
        }

        if (strcmp(ifreq.ifr_name, "lo") == 0) {
            continue;
        }

        if (ioctl(sock, SIOCGIFHWADDR, &ifreq) < 0) {
            continue;
        }

        goto get_mac;
    }
    return NULL;

get_mac:
    snprintf(mac_str, PLATFORM_MAC_LEN, "%02X:%02X:%02X:%02X:%02X:%02X",
             (unsigned char)ifreq.ifr_hwaddr.sa_data[0],
             (unsigned char)ifreq.ifr_hwaddr.sa_data[1],
             (unsigned char)ifreq.ifr_hwaddr.sa_data[2],
             (unsigned char)ifreq.ifr_hwaddr.sa_data[3],
             (unsigned char)ifreq.ifr_hwaddr.sa_data[4],
             (unsigned char)ifreq.ifr_hwaddr.sa_data[5]);

    return mac_str;
}

int platform_wifi_get_rssi_dbm(void)
{
    return 0;
}

uint32_t platform_wifi_get_ip(char ip_str[PLATFORM_IP_LEN])
{
    struct ifreq ifreq;
    int sock;
    char ifname[IFNAMSIZ] = {0};

    if (NULL == platform_get_multicast_ifname(ifname, sizeof(ifname))) {
        perror("get multicast ifname");
        return -1;
    }

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        return -1;
    }

    ifreq.ifr_addr.sa_family = AF_INET; //ipv4 address
    strncpy(ifreq.ifr_name, ifname, IFNAMSIZ - 1);

    if (ioctl(sock, SIOCGIFADDR, &ifreq) < 0) {
        perror("ioctl");
        return -1;
    }

    close(sock);

    strncpy(ip_str,
            inet_ntoa(((struct sockaddr_in *)&ifreq.ifr_addr)->sin_addr),
            PLATFORM_IP_LEN);

    return ((struct sockaddr_in *)&ifreq.ifr_addr)->sin_addr.s_addr;
}

char *platform_get_chipid(char chipid[PLATFORM_CID_LEN])
{
    //strncpy(chipid, "unknown chipid", PLATFORM_CID_LEN);
    strncpy(chipid, "rtl8188eu 12345678", PLATFORM_CID_LEN);
    chipid[PLATFORM_CID_LEN - 1] = '\0';

    return chipid;
}

char *platform_get_os_version(char os_ver[STR_SHORT_LEN])
{
    FILE *fp = fopen("/proc/version", "r");
    size_t count = 0;

    if (fp) {
        count = fread(os_ver, STR_SHORT_LEN, 1, fp);
        fclose(fp);
    }

    if (!count) {
        strncpy(os_ver, "unknown version", STR_SHORT_LEN);
    }

    os_ver[STR_SHORT_LEN - 1] = '\0';

    return os_ver;
}

int platform_config_write(const char *buffer, int length)
{
    FILE *fp;
    size_t written_len;
    char filepath[128] = {0};

    if (!buffer || length <= 0) {
        return -1;
    }

    snprintf(filepath, sizeof(filepath),
             "%s%s", platform_get_storage_directory(),
             ALINK_CONFIG_FILE_NAME);
    fp = fopen(filepath, "w");
    if (!fp) {
        return -1;
    }

    written_len = fwrite(buffer, 1, length, fp);

    fclose(fp);

    return ((written_len != length) ? -1 : 0);
}

int platform_config_read(char *buffer, int length)
{
    FILE *fp;
    size_t read_len;
    char filepath[128] = {0};

    if (!buffer || length <= 0) {
        return -1;
    }

    snprintf(filepath, sizeof(filepath), "%s%s",
             platform_get_storage_directory(), ALINK_CONFIG_FILE_NAME);
    fp = fopen(filepath, "r");
    if (!fp) {
        return -1;
    }

    read_len = fread(buffer, 1, length, fp);
    fclose(fp);

    return ((read_len != length) ? -1 : 0);
}

int platform_sys_net_is_ready(void)
{
    //TODO: network ready?
    return 1;
}

void platform_sys_reboot(void)
{
    int ret = 0;
    ret = system(REBOOT_CMD);
}

char *platform_get_module_name(char name_str[STR_SHORT_LEN])
{
    strncpy(name_str, MODULE_NAME, STR_SHORT_LEN);
    name_str[STR_SHORT_LEN - 1] = '\0';
    return name_str;
}

const char *platform_get_storage_directory(void)
{
    return ALINK_STORAGE_PATH;
}

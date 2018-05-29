/*
 * Copyright (c) 2014-2016 Alibaba Group. All rights reserved.
 *
 * Alibaba Group retains all right, title and interest (including all
 * intellectual property rights) in and to this computer program, which is
 * protected by applicable intellectual property laws.  Unless you have
 * obtained a separate written license from Alibaba Group., you are not
 * authorized to utilize all or a part of this computer program for any
 * purpose (including reproduction, distribution, modification, and
 * compilation into object code), and you must immediately destroy or
 * return to Alibaba Group all copies of this computer program.  If you
 * are licensed by Alibaba Group, your rights to utilize this computer
 * program are limited by the terms of that license.  To obtain a license,
 * please contact Alibaba Group.
 *
 * This computer program contains trade secrets owned by Alibaba Group.
 * and, unless unauthorized by Alibaba Group in writing, you agree to
 * maintain the confidentiality of this computer program and related
 * information and to not disclose this computer program and related
 * information to any other person or entity.
 *
 * THIS COMPUTER PROGRAM IS PROVIDED AS IS WITHOUT ANY WARRANTIES, AND
 * Alibaba Group EXPRESSLY DISCLAIMS ALL WARRANTIES, EXPRESS OR IMPLIED,
 * INCLUDING THE WARRANTIES OF MERCHANTIBILITY, FITNESS FOR A PARTICULAR
 * PURPOSE, TITLE, AND NONINFRINGEMENT.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>

#include <net/if.h>	      // struct ifreq
//inet_ntoa
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>

#include "platform.h"
#include "platform_config.h"

#define ALINK_CONFIG_FILE_NAME      "alinkconfig.db"
#define ROUTER_INFO_PATH        "/proc/net/route"
#define ROUTER_RECORD_SIZE      256


/*
route table format:
#cat /proc/net/route
Iface   Destination Gateway     Flags   RefCnt  Use Metric  Mask        MTU Window  IRTT
eth0    00000000    010AA8C0    0003    0   0   0   00000000    0   0   0
eth1    0000FEA9    00000000    0001    0   0   1000    0000FFFF    0   0   0
*/
char *platform_get_default_routing_ifname(char *ifname, int ifname_size)
{
    FILE *fp = NULL;
    char line[ROUTER_RECORD_SIZE] = {0};
    char iface[IFNAMSIZ] = {0};
    char *result = NULL;
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

    while (fgets(line, sizeof(line), fp)) {
        if (11 !=
            sscanf(line, "%s %08x %08x %x %d %d %d %08x %d %d %d",
                   iface, &destination, &gateway, &flags, &refCnt, &use,
                   &metric, &mask, &mtu, &window, &irtt)) {
            perror("sscanf");
            continue;
        }

        /*default route */
        if ((destination == 0) && (mask == 0)) {
            strncpy(ifname, iface, ifname_size - 1);
            result = ifname;
            break;
        }
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
    int sock = -1;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        return NULL;
    }
    strcpy(ifreq.ifr_name, IFNAME);

    if (ioctl(sock, SIOCGIFHWADDR, &ifreq) < 0) {
        close(sock);
        perror("ioctl");
        return NULL;
    }

    snprintf(mac_str, PLATFORM_MAC_LEN, "%02X:%02X:%02X:%02X:%02X:%02X",
             (unsigned char)ifreq.ifr_hwaddr.sa_data[0],
             (unsigned char)ifreq.ifr_hwaddr.sa_data[1],
             (unsigned char)ifreq.ifr_hwaddr.sa_data[2],
             (unsigned char)ifreq.ifr_hwaddr.sa_data[3],
             (unsigned char)ifreq.ifr_hwaddr.sa_data[4],
             (unsigned char)ifreq.ifr_hwaddr.sa_data[5]);

    close(sock);
    return mac_str;
}

int platform_wifi_get_rssi_dbm(void)
{
    return 0;
}

uint32_t platform_wifi_get_ip(char ip_str[PLATFORM_IP_LEN], const char *ifname)
{
    struct ifreq ifreq;
    int sock = -1;
    char ifname_buff[IFNAMSIZ] = {0};

    if((NULL == ifname || strlen(ifname) == 0) &&
        NULL == (ifname = platform_get_default_routing_ifname(ifname_buff, sizeof(ifname_buff)))){
        perror("get default routeing ifname");
        return -1;
    }

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        return -1;
    }

    ifreq.ifr_addr.sa_family = AF_INET; //ipv4 address
    strncpy(ifreq.ifr_name, ifname, IFNAMSIZ - 1);

    if (ioctl(sock, SIOCGIFADDR, &ifreq) < 0) {
        close(sock);
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

    if (!count)
        strncpy(os_ver, "unknown version", STR_SHORT_LEN);

    os_ver[STR_SHORT_LEN - 1] = '\0';

    return os_ver;
}

int platform_config_write(const char *buffer, int length)
{
    FILE *fp;
    size_t written_len;
    char filepath[128] = {0};

    if (!buffer || length <= 0)
        return -1;

    snprintf(filepath, sizeof(filepath),
             "%s%s", platform_get_storage_directory(),
             ALINK_CONFIG_FILE_NAME);
    fp = fopen(filepath, "w");
    if (!fp)
        return -1;

    written_len = fwrite(buffer, 1, length, fp);

    fclose(fp);

    return ((written_len != length) ? -1: 0);
}

int platform_config_read(char *buffer, int length)
{
    FILE *fp;
    size_t read_len;
    char filepath[128] = {0};

    if (!buffer || length <= 0)
        return -1;

    snprintf(filepath, sizeof(filepath), "%s%s",
             platform_get_storage_directory(), ALINK_CONFIG_FILE_NAME);
    fp = fopen(filepath, "r");
    if (!fp)
        return -1;

    read_len = fread(buffer, 1, length, fp);
    fclose(fp);

    return ((read_len != length)? -1: 0);
}

int platform_sys_net_is_ready(void)
{
    //TODO: network ready?
    return 1;
}

void platform_sys_reboot(void)
{
    system(REBOOT_CMD);
}

char *platform_get_module_name(char name_str[STR_SHORT_LEN])
{
    strncpy(name_str, MODULE_NAME, STR_SHORT_LEN);
    name_str[STR_SHORT_LEN - 1] ='\0';
    return name_str;
}

const char *platform_get_storage_directory(void)
{
    return ALINK_STORAGE_PATH;
}

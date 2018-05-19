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
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <pthread.h>
#include <net/if.h>	      // struct ifreq
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>       // IP_MAXPACKET (65535)
#include <net/ethernet.h>     // ETH_P_ALL
#include <linux/if_packet.h>


#include "platform.h"
#include "platform_config.h"

//一键配置超时时间, 建议超时时间1-3min, APP侧一键配置1min超时
int platform_awss_get_timeout_interval_ms(void)
{
    return 3 * 60 * 1000;
}

//默认热点配网超时时间
int platform_awss_get_connect_default_ssid_timeout_interval_ms( void )
{
    return 0;
}

//一键配置每个信道停留时间, 建议200ms-400ms
int platform_awss_get_channelscan_interval_ms(void)
{
    return 200;
}

//wifi信道切换，信道1-13
void platform_awss_switch_channel(char primary_channel,
		char secondary_channel, uint8_t bssid[ETH_ALEN])
{
#define HAL_PRIME_CHNL_OFFSET_DONT_CARE	0
#define HAL_PRIME_CHNL_OFFSET_LOWER	1
#define HAL_PRIME_CHNL_OFFSET_UPPER	2
	int flag = (primary_channel <= 4
			? HAL_PRIME_CHNL_OFFSET_LOWER : HAL_PRIME_CHNL_OFFSET_UPPER);
	char buf[256];

	snprintf(buf, sizeof(buf), "echo \"%d %d 0\" > /proc/net/rtl8188eu/%s/monitor",
			primary_channel, flag, WLAN_IFNAME);
	system(buf);
}

int open_socket(void)
{
	int fd;
#if 0
	if (getuid() != 0)
		err("root privilege needed!\n");
#endif
	//create a raw socket that shall sniff
	fd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	assert(fd >= 0);

	struct ifreq ifr;
	int sockopt = 1;

	memset(&ifr, 0, sizeof(ifr));

	/* set interface to promiscuous mode */
	strncpy(ifr.ifr_name, WLAN_IFNAME, sizeof(ifr.ifr_name));
	if (ioctl(fd, SIOCGIFFLAGS, &ifr) < 0) {
		perror("ioctl(SIOCGIFFLAGS)");
		goto exit;
	}
	ifr.ifr_flags |= IFF_PROMISC;
	if (ioctl(fd, SIOCSIFFLAGS, &ifr) < 0) {
		perror("ioctl(SIOCSIFFLAGS)");
		goto exit;
	}

	/* allow the socket to be reused */
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR,
                &sockopt, sizeof(sockopt)) < 0) {
		perror("setsockopt(SO_REUSEADDR)");
		goto exit;
	}

	/* bind to device */
	struct sockaddr_ll ll;

	memset(&ll, 0, sizeof(ll));
	ll.sll_family = PF_PACKET;
	ll.sll_protocol = htons(ETH_P_ALL);
	ll.sll_ifindex = if_nametoindex(WLAN_IFNAME);
	if (bind(fd, (struct sockaddr *)&ll, sizeof(ll)) < 0) {
		perror("bind[PF_PACKET] failed");
		goto exit;
	}

	return fd;
exit:
	close(fd);
	exit(EXIT_FAILURE);
}

pthread_t monitor_thread;
char monitor_running;

void *monitor_thread_func(void *arg)
{
    platform_awss_recv_80211_frame_cb_t ieee80211_handler = arg;
    /* buffer to hold the 80211 frame */
    char *ether_frame = malloc(IP_MAXPACKET);
	assert(ether_frame);

    int fd = open_socket();
    int len, ret;
    fd_set rfds;
    struct timeval tv;

    while (monitor_running) {
        FD_ZERO(&rfds);
        FD_SET(fd, &rfds);

        tv.tv_sec = 0;
        tv.tv_usec = 100 * 1000;//100ms

        ret = select(fd + 1, &rfds, NULL, NULL, &tv);
        assert(ret >= 0);

        if (!ret)
            continue;

        //memset(ether_frame, 0, IP_MAXPACKET);
        len = recv(fd, ether_frame, IP_MAXPACKET, 0);
        if (len < 0) {
            perror ("recv() failed:");
            //Something weird happened
            continue;
        }

        /*
         * Note: use tcpdump -i wlan0 -w file.pacp to check link type and FCS
         */

        /* rtl8188: include 80211 FCS field(4 byte) */
        int with_fcs = 1;
        /* rtl8188: link-type IEEE802_11_RADIO (802.11 plus radiotap header) */
        int link_type = AWSS_LINK_TYPE_80211_RADIO;

        (*ieee80211_handler)(ether_frame, len, link_type, with_fcs);
    }

    free(ether_frame);
    close(fd);

    return NULL;
}

//进入monitor模式, 并做好一些准备工作，如
//设置wifi工作在默认信道6
//若是linux平台，初始化socket句柄，绑定网卡，准备收包
//若是rtos的平台，注册收包回调函数aws_80211_frame_handler()到系统接口
void platform_awss_open_monitor(platform_awss_recv_80211_frame_cb_t cb)
{
	char buf[256];
	int ret;

	snprintf(buf, sizeof(buf), "ifconfig %s up", WLAN_IFNAME);
	ret = system(buf);

	snprintf(buf, sizeof(buf), "iwconfig %s mode monitor", WLAN_IFNAME);
	ret = system(buf);

    monitor_running = 1;

    ret = pthread_create(&monitor_thread, NULL, monitor_thread_func, cb);
    assert(!ret);
}

//退出monitor模式，回到station模式, 其他资源回收
void platform_awss_close_monitor(void)
{
	char buf[256];

    monitor_running = 0;

    pthread_join(monitor_thread, NULL);

	snprintf(buf, sizeof(buf), "ifconfig %s up", WLAN_IFNAME);
	system(buf);

	snprintf(buf, sizeof(buf), "iwconfig %s mode managed", WLAN_IFNAME);
	system(buf);
}

int platform_awss_connect_ap(
        _IN_ uint32_t connection_timeout_ms,
        _IN_ char ssid[PLATFORM_MAX_SSID_LEN],
        _IN_ char passwd[PLATFORM_MAX_PASSWD_LEN],
        _IN_OPT_ enum AWSS_AUTH_TYPE auth,
        _IN_OPT_ enum AWSS_ENC_TYPE encry,
        _IN_OPT_ uint8_t bssid[ETH_ALEN],
        _IN_OPT_ uint8_t channel)
{
	char buf[256];
	int ret;

	snprintf(buf, sizeof(buf), "ifconfig %s up", WLAN_IFNAME);
	ret = system(buf);

	snprintf(buf, sizeof(buf), "iwconfig %s mode managed", WLAN_IFNAME);
	ret = system(buf);

	snprintf(buf, sizeof(buf), "wpa_cli -i %s status | grep 'wpa_state=COMPLETED'", WLAN_IFNAME);
	do {
		ret = system(buf);
		usleep(100 * 1000);
	} while (ret);

	snprintf(buf, sizeof(buf), "udhcpc -i %s", WLAN_IFNAME);
	ret = system(buf);

	//TODO: wait dhcp ready here
    return 0;
}

int platform_wifi_scan(platform_wifi_scan_result_cb_t cb)
{
	return 0;
}

p_aes128_t platform_aes128_init(
    const uint8_t* key,
    const uint8_t* iv,
    AES_DIR_t dir)
{
	return 0;
}

int platform_aes128_destroy(
    p_aes128_t aes)
{
	return 0;
}

int platform_aes128_cbc_encrypt(
    p_aes128_t aes,
    const void *src,
    size_t blockNum,
    void *dst )
{
	return 0;
}

int platform_aes128_cbc_decrypt(
    p_aes128_t aes,
    const void *src,
    size_t blockNum,
    void *dst )
{
	return 0;
}

int platform_wifi_get_ap_info(
    char ssid[PLATFORM_MAX_SSID_LEN],
    char passwd[PLATFORM_MAX_PASSWD_LEN],
    uint8_t bssid[ETH_ALEN])
{
	if (NULL != ssid){
		strcpy(ssid, "NO_WIFI");
	}
	return 0;
}


int platform_wifi_low_power(int timeout_ms)
{
    //wifi_enter_power_saving_mode();
    usleep(timeout_ms);
    //wifi_exit_power_saving_mode();

    return 0;
}

int platform_wifi_enable_mgnt_frame_filter(
            _IN_ uint32_t filter_mask,
            _IN_OPT_ uint8_t vendor_oui[3],
            _IN_ platform_wifi_mgnt_frame_cb_t callback)
{
    return -2;
}

int platform_wifi_send_80211_raw_frame(_IN_ enum platform_awss_frame_type type,
        _IN_ uint8_t *buffer, _IN_ int len)
{
    return -2;
}

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

#ifndef __ZCONFIG_LIB_H
#define __ZCONFIG_LIB_H

#ifndef u8
#define u8                  unsigned char
#define u16                 unsigned short
#define u32                 unsigned int
#endif

#ifndef ETH_ALEN
#define ETH_ALEN            (6)
#endif

#define ZC_MAX_SSID_LEN     (32 + 1)/* ssid: 32 octets at most, include the NULL-terminated */
#define ZC_MAX_PASSWD_LEN   (64 + 1)/* 8-63 ascii */
#define MAX_APLIST_NUM      (100)

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif

enum _ZC_LOGLEVEL_ {
    LOGLEVEL_NONE,
    LOGLEVEL_ERROR,
    LOGLEVEL_WARN,
    LOGLEVEL_INFO,
    LOGLEVEL_DEBUG
};

enum _ZC_AUTH_TYPE_ {
    ZC_AUTH_TYPE_OPEN,
    ZC_AUTH_TYPE_SHARED,
    ZC_AUTH_TYPE_WPAPSK,
    ZC_AUTH_TYPE_WPA8021X,
    ZC_AUTH_TYPE_WPA2PSK,
    ZC_AUTH_TYPE_WPA28021X,
    ZC_AUTH_TYPE_WPAPSKWPA2PSK,
    ZC_AUTH_TYPE_MAX = ZC_AUTH_TYPE_WPAPSKWPA2PSK,
    ZC_AUTH_TYPE_INVALID = 0xff,
};

enum _ZC_ENC_TYPE_ {
    ZC_ENC_TYPE_NONE,
    ZC_ENC_TYPE_WEP,
    ZC_ENC_TYPE_TKIP,
    ZC_ENC_TYPE_AES,
    ZC_ENC_TYPE_TKIPAES,
    ZC_ENC_TYPE_MAX = ZC_ENC_TYPE_TKIPAES,
    ZC_ENC_TYPE_INVALID = 0xff,
};

enum _ZC_PKG_TYPE_ {
    PKG_INVALID,       // invalid pkg, --无效包
    PKG_BC_FRAME,      // broadcast frame, --信道扫描阶段，收到收到该返回值建议延长在当前信道停留时间，可以延长T1
    PKG_START_FRAME,   // start frame, --信道扫描阶段，收到该返回值用于锁定信道
    PKG_DATA_FRAME,    // data frame, --数据包，锁定信道后长时间T2收不到数据包，需重新进入扫描阶段
    PKG_ALINK_ROUTER,  // alink router
    PKG_GROUP_FRAME,   // group frame
    PKG_END            // --配网结束事件，已拿到ssid和passwd，通过回调函数去获取ssid和passwd
    /*
     * 参考值：
     * T1:             400ms >= T2 >= 100ms
     * T2:             3s
     */
};


//进入monitor模式前后调用该函数
void zconfig_init();
//配网成功后，调用该函数，释放内存资源
void zconfig_destroy(void);
/*
    进入monitor/sniffer模式后，将收到的包传给该函数进行处理
    若进入monitor时进行包过滤配置，以下几种包不能过滤：
    1) 数据包，目的地址为广播地址
    2) 长度>40的管理帧
    厂家需要自行切换信道，切信道时间按照自身平台需要进行调整，建议取值范围[100ms - 300ms]
    其中，6号信道需作为固定信道放在信道列表里！！！
    input:
    pkt_data: 80211 wireless raw package, include data frame & management frame
    pkt_length:    radio_hdr + 80211 hdr + payload, without fcs(4B)
    return:
    见enum _PKG_TYPE_结构体说明
*/
int zconfig_recv_callback(void *pkt_data, u32 pkt_length, u8 channel,
                          int link_type, int with_fcs, signed char rssi);

/*
 * save apinfo
 * 0 -- success, otherwise, failed.
 */
int zconfig_set_apinfo(u8 *ssid, u8* bssid, u8 channel, u8 auth,
                       u8 pairwise_cipher, u8 group_cipher, signed char rssi);

/* helper function, auth/encry type to string */
const char *zconfig_auth_str(u8 auth);
const char *zconfig_encry_str(u8 encry);

const char *zconfig_lib_version(void);

/* add channel to global scanning channel list */
int zconfig_add_active_channel(int channel);
/* channel locked callback */
void zconfig_channel_locked_callback(u8 primary_channel,
                                     u8 secondary_channel, u8 *bssid);
/* got ssid&passwd callback */
void zconfig_got_ssid_passwd_callback(u8 *ssid, u8 *passwd, u8 *bssid,
                                      u8 auth, u8 encry, u8 channel);

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif

#endif

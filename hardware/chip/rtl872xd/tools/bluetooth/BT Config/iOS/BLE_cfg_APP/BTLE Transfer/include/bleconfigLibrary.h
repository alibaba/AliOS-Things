//
//  bleconfigLibrary.h
//  bleconfigLibrary
//
//  Created by CN3_SD9 on 2016/9/28.
//  Copyright © 2016年 Realtek. All rights reserved.
//

#import <Foundation/Foundation.h>

#define MAC_ADDR_LEN                (6)
#define MAX_INFO_LEN                (64)
#define RTK_BTCONFIG_MAX_BSS_NUM    (64)

struct dev_info{
    unsigned char  status;                  //BIT(0):connected BIT(1):profile saved
    unsigned short dev_type;
    unsigned char  mac[MAC_ADDR_LEN];
    unsigned int   ip;
    unsigned char  extra_info[MAX_INFO_LEN];//name
    unsigned char  require_pin;             //1-require PIN, 0-no need for PIN
    unsigned char  band;
    unsigned char  connection_status;
    unsigned char  channel;
    unsigned char  rssi;
    unsigned char  configure_status;
};

typedef enum _wlan_mac_state {
    STATE_DISABLED  =0x00,
    STATE_IDLE      =0x01,
    STATE_SCANNING  =0x02,
    STATE_STARTED   =0x03,
    STATE_CONNECTED =0x04,
    STATE_WAITFORKEY=0x05,
    STATE_WRONG_PASSWORD=0x0f
} wlan_mac_state;

typedef enum _blecfg_state{
    STATE_CFG_IDLE,
    STATE_CAPABILITY,
    STATE_SCAN_5G,
    STATE_SCAN_2G,
    STATE_SCAN_25G,
    STATE_CONNECTION,
    STATE_CONNECTION_STATUS
}blecfg_state;

struct rtk_btconfig_bss_info
{
    unsigned char authAlg;/*0:open,1:WPA,2:WEP*/
    unsigned char bdBssId[6];
    unsigned char bdSsIdBuf[32];
    unsigned char ChannelNumber;
    unsigned char rssi;
    unsigned char TTL;
} __attribute__((packed));
struct rtk_btconfig_ss_result
{
    unsigned char band;/*0 for 2.4G,1 for 5G*/
    unsigned int number;/*total number of results*/
    struct rtk_btconfig_bss_info bss_info[RTK_BTCONFIG_MAX_BSS_NUM];
}__attribute__((packed));

@interface bleconfigLibrary : NSObject

-(int)gen_cmd_capability:(uint8_t*)val;
-(int)gen_cmd_sitesurvey_5G:(uint8_t*)val;
-(int)gen_cmd_sitesurvey_2G:(uint8_t*)val;
-(int)gen_cmd_connection_status:(uint8_t*)val;
-(int)gen_cmd_connection_request:(uint8_t*)val :(uint8_t)ch :(uint8_t)authAlg :(uint8_t*)ssid :(uint8_t*)bssid :(uint8_t*)pwd :(int)pwdLen;

-(BOOL)is_cmd_capability:(const uint8_t*)bytes;
-(BOOL)is_cmd_support_5G:(const uint8_t*)bytes;
-(BOOL)is_cmd_support_2G:(const uint8_t*)bytes;
-(Byte)get_product_type:(const uint8_t*)bytes;
-(BOOL)is_cmd_ss_5G:(const uint8_t*)bytes;
-(BOOL)is_cmd_ss_2G:(const uint8_t*)bytes;
-(BOOL)is_cmd_connection:(const uint8_t*)bytes;
-(BOOL)is_cmd_status:(const uint8_t*)bytes;

-(uint8_t)getDevice_connectedStatus:(const uint8_t*)bytes;
-(BOOL)isDevice_connectedEncrypt:(const uint8_t*)bytes;
-(NSString*)getDevice_connectedSSID:(const uint8_t*)bytes;
-(NSString*)getDevice_connectedBSSID:(const uint8_t*)bytes;
-(uint8_t)getDevice_connectedRSSI:(const uint8_t*)bytes;

-(void)setSiteSurveyResult:(const uint8_t*)bytes :(struct rtk_btconfig_ss_result*)ss_result :(int)ss_sectionNum :(int)unit;
-(BOOL)dump_device_information:(const uint8_t*)bytes :(struct dev_info*)new_dev;

@end

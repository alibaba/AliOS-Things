/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __DEVMGR__H
#define __DEVMGR__H

#include <stdint.h>
#include <unistd.h>
#include "aos/aos.h"
#include "service.h"
#include "devmgr_common.h"
#include "alink_export_internal.h"

//#define __DEVMGR_UT__   1
#ifdef __DEVMGR_UT__
#include "devmgr_ut.h"
#endif

#if defined(__cplusplus) /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif

#define STR_SIGN_LEN        (16)
#define STR_DEVID_LEN       (17)
#define STR_TOKEN_LEN       (6)
#define STR_UUID_LEN        (32)
#define ETHER_ADDR_BYTES    (6)

// add by wukong 2017-4-17
#define STR_MODEL_LEN   (80 + 1)
#ifndef STR_NAME_LEN
#define STR_NAME_LEN    (32 + 1)
#endif
// add by wukong end 2017-4-17

#define MODULE_NAME_DEVMGR "devmgr"

/*
* 设备类型定义
*/
typedef enum {
    DEV_TYPE_GATEWAY        = 0,
    DEV_TYPE_WIFI           = 1,
    DEV_TYPE_ZIGBEE         = 2,
    DEV_TYPE_BLE            = 3,
    DEV_TYPE_WIFI_SUB       = 4,
    DEV_TYPE_MAX
}
devtype_t;

/*
* 设备本地连接状态定义
*/
typedef enum {
    LINK_STATE_OFFLINE        = 0,
    LINK_STATE_ONLINE         = 1,
} link_state_t;

/*
* 本地和云端链接云端状态事件定义
*/
typedef enum {
    NETWORK_EVENT_UP        = 0,
    NETWORK_EVENT_DOWN      = 1,
    LINK_EVENT_ONLINE       = 2,
    LINK_EVENT_OFFLINE      = 3,
    LINK_EVENT_LEAVE        = 4
} network_event_t;


/*
* 设备连接云端状态定义
*/
typedef enum {
    DEVICE_STATE_INITIAL        = 0,
    DEVICE_STATE_AUTHORISED     = 1,
    DEVICE_STATE_REGISTERED     = 2,
    DEVICE_STATE_LOGINED        = 3,
    DEVICE_STATE_LOGOUTED       = 4
} device_state_t;

// add by wukong 2017-4-17 for router sub devices
typedef struct router_base_s {
    char name[STR_NAME_LEN];          // hostname, for example, my iphone's hostname is "wujy"
    char ostype[STR_NAME_LEN];        // router's sub device type,  e.g "Android"
    char category[STR_NAME_LEN];      // e.g "unknown"
    char manufacturer[STR_NAME_LEN];  // e.g "HUAWEI","HTC", "unknown"
    char model[STR_MODEL_LEN];        // e.g "PANDORABOX_NETWORKING_ROUTER_NEWIFI_PB1", "alink", "unknown"
} router_base_t;
// add by wukong end 2017-4-17

typedef struct dev_base_s {
    uint8_t dev_type;
    uint32_t model_id;
    union {
        char ether_mac[ETHER_ADDR_BYTES];
        char ieee_addr[IEEE_ADDR_BYTES];
    } u;
    char rand[SUBDEV_RAND_BYTES];
    char sign[STR_SIGN_LEN + 1];
    char dev_id[STR_DEVID_LEN + 1];
    char uuid[STR_UUID_LEN + 1];
    char token[STR_TOKEN_LEN + 1];
    uint16_t crc;
} dev_base_t;


typedef struct dev_info_s {
    dlist_t list_node;
    dlist_t attr_head;
    dev_base_t dev_base;
    // add by wukong 2017-4-17
    router_base_t router_base;
    // add by wukong end 2017-4-17
    uint8_t device_idx; //0xff为无效值
    uint8_t cloud_state;
    uint8_t link_state;
    void *dev_mutex;
    uint8_t ref_cnt;
} dev_info_t;

typedef int32_t (*zigbee_remove_device_cb_t)(uint8_t ieee_addr[IEEE_ADDR_BYTES]);

int devmgr_get_dev_type(const char *devid_or_uuid, uint8_t *dev_type);

dev_info_t *devmgr_get_devinfo(const char *devid_or_uuid_or_mac);

#ifdef GATEWAY_SDK
//dev_info_t *devmgr_get_devinfo(const char *devid_or_uuid);
dev_info_t *devmgr_get_unknown_devinfo(const char *devid_or_uuid);
dev_info_t *devmgr_get_devinfo_by_ieeeaddr(unsigned char
                                           ieee_addr[IEEE_ADDR_BYTES]);

int devmgr_sync_device_profile(uint8_t dev_type, unsigned int model_id);

int devmgr_authorise_device(char *devid);

/*获取全部设备model_id,提供给文件下载管理模块使用*/
int devmgr_get_all_device_modelid(uint32_t model_id[], int *num);

/*释放devinfo对象引用计数*/
void devmgr_put_devinfo_ref(dev_info_t *devinfo);

int devmgr_update_device_online_status(const char *devid, link_state_t state);

int devmgr_update_zigbee_device_online_status(uint8_t
                                              ieee_addr[IEEE_ADDR_BYTES], uint8_t online);

int devmgr_relogin_device(const char *devid);

int devmgr_join_zigbee_device(unsigned char ieee_addr[IEEE_ADDR_BYTES],
                              uint32_t model_id, const char rand[SUBDEV_RAND_BYTES], const char *sign);

int devmgr_leave_zigbee_device(unsigned char ieee_addr[IEEE_ADDR_BYTES]);

void __dump_devinfo(dev_info_t *devinfo);

int devmgr_delete_device_by_devinfo(dev_info_t *devinfo);

void __dump_device_list();

//lmns依赖接口
//device -> gw,wifi设备上下线接口
//payload:{"uuid":"","mac":"","modelId":"","model":"","name":""}
int devmgr_update_wifi_device_online_status(const char *devid, uint8_t online,
                                            const char *payload);

//app -> gw,扫码添加和删除设备
int devmgr_delete_device(const char *devid);
//payload:{"mac":"","ieeeAddr":"","modelId":"","dev_type":""}
int devmgr_add_device(const char *devid, const char *payload);

void devmgr_free_unknown_devlist(void);

//主动从设备端获取设备静态信息
int devmgr_zbnet_get_devinfo(unsigned char ieee_addr[IEEE_ADDR_BYTES]);

void devmgr_reset_all_device(void);
#endif

// add by wukong 2017-4-17
int get_mac_by_uuid(const char *uuid, char *mac);
int get_uuid_by_mac(const char *mac, char *uuid);
// add by wukong end 2017-17

int devmgr_init();
void devmgr_exit();

#if defined(__cplusplus) /* If this is a C++ compiler, use C linkage */
}
#endif

#endif


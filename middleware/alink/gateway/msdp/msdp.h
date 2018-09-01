/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __MSDP_H__
#define __MSDP_H__

/******************************************/
#include <stdint.h>
#include <unistd.h>
#include <stdarg.h>
#include "aos/aos.h"
#include "alink_export_internal.h"
#include "alink_protocol.h"
#include "json_parser.h"

//#define __MSDP_UT__     1
#ifdef __MSDP_UT__
#include "msdp_ut.h"
#endif
#if defined(__cplusplus) /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif

/******************************************/
#define MODULE_NAME_MSDP "msdp"

#define MAX_PARAMS_LEN                  3600
#define MAX_VENDOR_CB_REMALLOC_COUNT    5
#define MAX_UUID_LEN                    33
#define MAX_ATTR_NAME_LEN               80
#define MAX_SERVICE_NAME_LEN            80
#define MAX_SERVICE_COUNT               32

#define JSON_KEY_UUID       "uuid"
#define JSON_KEY_ATTRSET    "attrSet"
#define JSON_KEY_SERVICE    "service"
#define JSON_KEY_VALUE      "value"
#define JSON_KEY_WHEN       "when"
#define JSON_KEY_RESULT     "result"
#define JSON_KEY_CODE       "code"
#define JSON_KEY_ARGS       "args"
#define JSON_KEY_SET        "set"
#define JSON_KEY_EXTRA      "extra"
#define JSON_KEY_DEVID      "devId"

#define REQUEST_SERVICE_STRING_FMT      "{\"uuid\":\"%s\",\"service\":\"%s\",\"args\":%s}"
#define RPC_RESULT_STRING_FMT           "{\"uuid\":\"%s\",\"service\":\"%s\",\"code\":\"%d\",\"result\":[%s]}"
#define REGISTER_SERVICES_STRING_FMT    "[{\"uuid\":\"%s\",\"service\":%s}]"
#define GET_DEVICE_ATTR_STRING_FMT      "{\"uuid\":\"%s\",\"attrSet\":%s}"
#define SET_DEVICE_ATTR_STRING_FMT      "{\"uuid\":\"%s\",\"attrSet\":%s,%s}"
#define ATTR_NAME_VALUE_STRING_FMT      "\"%s\":{\"value\":\"%s\",\"when\":\"%s\"}"
#define ATTR_NAME_OBJECT_STRING_FMT     "\"%s\":{\"value\":%s,\"when\":\"%s\"}"
#define POST_ATTR_OBJECT_STRING_FMT     "{\"uuid\":\"%s\",\"%s\":{\"value\":%s,\"when\":\"%s\"}}"
#define POST_ATTR_VALUE_STRING_FMT      "{\"uuid\":\"%s\",\"%s\":{\"value\":\"%s\",\"when\":\"%s\"}}"

#define METHOD_REGISTER_REMOTE_SERVICE  "registerRemoteService"
#define METHOD_GET_DEVICE_STATUS        "getDeviceStatus"
#define METHOD_SET_DEVICE_STATUS        "setDeviceStatus"
#define METHOD_SET_DEVICE_STATUS_ARRAY  "setDeviceStatusArray"
#define METHOD_POST_DEVICE_DATA         "postDeviceData"
#define METHOD_POST_DEVICE_DATA_ARRAY   "postDeviceDataArray"
#define METHOD_REQUEST_REMOTE_SERVICE   "requestRemoteService"
#define METHOD_POST_REMOTE_SERVICE_RSP  "postRemoteServieRsp"

/******************************************/

typedef int (*dev_option_cb)(char *params);
typedef int (*get_attr_cb)(char *value_buff, int buff_size);
typedef int (*set_attr_cb)(const char *attr_value);
typedef int (*get_subdev_attr_cb)(const char *mac, char *value_buff,
                                  int buff_size);
typedef int (*set_subdev_attr_cb)(const char *mac, const char *value_buff);
typedef int (*get_dev_attr_cb)(const char *uuid, const char *attr_name,
                               char **attr_value);
typedef int (*set_dev_attr_cb)(const char *uuid, const char *attr_name,
                               const char *attr_value);

enum ALINK_COMMAND {
    ACB_GET_DEVICE_STATUS = 0,
    ACB_SET_DEVICE_STATUS = 1,
    ACB_SET_DEVICE_STATUS_ARRAY = 2,
    ACB_REQUEST_REMOTE_SERVICE = 3,
    ACB_GET_DEVICE_STATUS_BY_RAWDATA = 4,
    ACB_SET_DEVICE_STATUS_BY_RAWDATA = 5,
    ACB_REQUEST_DEVICE_UPGRADE = 6,
    ACB_REQUEST_DEVICE_UNUPGRADE = 7,
    MAX_CALLBACK_NUM
};

typedef struct {
    dlist_t list_node;             //链表节点
    char name[MAX_SERVICE_NAME_LEN];        //service方法名称
    ALINK_SERVICE_EXECUTE_CB service_cb;    //设置属性回调函数
} service_handler_t, *service_handler_ptr;


typedef struct {
    dlist_t list_node;             //当前属性handler链接节点
    char name[MAX_ATTR_NAME_LEN];           //属性名称
    void *get_cb;                           //获取属性回调函数
    void *set_cb;                           //设置属性回调函数
} attr_hanler_t, *attr_hanler_ptr;


typedef struct {
    uint8_t dev_type;                       //设备model
    dlist_t list_node;             //设备model链表节点(只对subdevice有意义)
    dlist_t attr_head;             //属性链表头
    dlist_t srv_head;              //服务链表头
    void *mutex_lock;                       //service和attribute链表读写操作锁
    dev_option_cb option_callback[MAX_CALLBACK_NUM];//alink method处理接口
} device_helper_t, *device_helper_ptr;

typedef struct post_node {
    dlist_t list_node; /* post_list */
    char *params;
} post_node_t;


/******************************************/
int msdp_add_asyncpost_task(char *params);

int msdp_get_all_attrname(const char *uuid, char *attrset, int buff_size);

int msdp_get_all_rpcname(uint8_t dev_type, char *name_array[], int array_size);

int msdp_register_attr_cb(uint8_t dev_type,
                          const char *name,
                          void *pf_get,
                          void *pf_set);

int msdp_register_service_cb(uint8_t dev_type,
                             const char *name,
                             void *pf_exec);

int msdp_register_remote_service(char *uuid, const char *service);

void msdp_unregister_service_cb(uint8_t dev_type, const char *name);

int msdp_register_device_helper(uint8_t dev_type,
                                dev_option_cb callback[MAX_CALLBACK_NUM]);

void msdp_unregister_device_helper(uint8_t dev_type);

int msdp_report_device_status(const char *uuid, const char *attr_name,
                              const char *attr_value);

int msdp_report_device_attrs(const char *uuid, const char *attrs[]);

int msdp_set_device_status(const char *uuid, const char *attr_name,
                           const char *attr_value);

int msdp_request_remote_service(const char *uuid, const char *service_name,
                                const char *args);

int msdp_dispatch_event(const char *uuid, const char *method, char *params);

void msdp_dump_helper();

attr_hanler_ptr msdp_get_attr_handler(const device_helper_ptr helper,
                                      const char *name);

service_handler_ptr msdp_get_service_handler(const device_helper_ptr helper,
                                             const char *name);

device_helper_ptr msdp_get_helper(const char *devid_or_uuid);

int lmns_post_device_data(const char *devid, char *payload);

int lmns_post_remote_service_rsp(const char *devid, char *payload);

int msdp_post_device_data_array(char *params);

int msdp_get_gw_attr(const char *gw_uuid, const char *attr_name,
                     char *attr_value_buff, int buff_size);

void *msdp_dup_string(const char *src);

void *msdp_dup_buff(const void *buff, unsigned buff_size);

void *msdp_new_buff(unsigned int buff_size);

void msdp_free_buff(void *buff);

int msdp_gw_init();

int msdp_zbnet_init();

int msdp_init();

void msdp_exit();
#if defined(__cplusplus) /* If this is a C++ compiler, use C linkage */
}
#endif

#endif


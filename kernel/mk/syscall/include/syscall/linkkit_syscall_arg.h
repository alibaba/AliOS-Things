/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef LINKKIT_SYSCALL_ARG_H
#define LINKKIT_SYSCALL_ARG_H

#include "dev_model_api.h"
#include "infra_compat.h"

typedef struct {
    iotx_linkkit_dev_type_t       dev_type;
    iotx_linkkit_dev_meta_info_t *meta_info;
} IOT_Linkkit_Open_syscall_arg_t;

typedef struct {
    int devid;
} IOT_Linkkit_Connect_syscall_arg_t;

typedef struct {
    int timeout_ms;
} IOT_Linkkit_Yield_syscall_arg_t;

typedef struct {
    int devid;
} IOT_Linkkit_Close_syscall_arg_t;

typedef struct {
    int                      devid;
    iotx_linkkit_msg_type_t  msg_type;
    unsigned char           *payload;
    int                      payload_len;
} IOT_Linkkit_Report_syscall_arg_t;

typedef struct {
    int                      devid;
    iotx_linkkit_msg_type_t  msg_type;
    unsigned char           *payload;
    int                      payload_len;
} IOT_Linkkit_Query_syscall_arg_t;

typedef struct {
    int   devid;
    char *eventid;
    int   eventid_len;
    char *payload;
    int   payload_len;
} IOT_Linkkit_TriggerEvent_syscall_arg_t;

typedef struct {
    IOT_LogLevel  level;
    void         *arg;
} IOT_SetLogLevel_syscall_arg_t;


typedef struct {
    IOT_LogLevel level;
} IOT_DumpMemoryStats_syscall_arg_t;


typedef struct {
    const char  *product_key;
    const char  *device_name;
    const char  *device_secret;
    void       **info;
} IOT_SetupConnInfo_syscall_arg_t;

typedef struct {
    int   option;
    void *data;
} IOT_Ioctl_syscall_arg_t;

#define IOT_HAL_SET_PRODUCT_KEY     (0x00u)
#define IOT_HAL_GET_PRODUCT_KEY     (0x01u)
#define IOT_HAL_SET_PRODUCT_SECRET  (0x02u)
#define IOT_HAL_GET_PRODUCT_SECRET  (0x03u)
#define IOT_HAL_SET_DEVICE_NAME     (0x04u)
#define IOT_HAL_GET_DEVICE_NAME     (0x05u)
#define IOT_HAL_SET_DEVICE_SECRET   (0x06u)
#define IOT_HAL_GET_DEVICE_SECRET   (0x07u)

// for HAL_Set secret keys
typedef struct {
    unsigned int  cmd;
    void         *arg;
} IOT_HAL_Ioctl_syscall_arg_t;

typedef struct {
    iotx_ioctl_event_t  event;
    void               *callback;
} IOT_RegisterCallback_syscall_arg_t;

#endif /* LINKKIT_SYSCALL_ARG_H */


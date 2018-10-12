/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#ifndef _IOT_EXPORT_LINKKIT_H_
#define _IOT_EXPORT_LINKKIT_H_

#include "iot_export.h"

typedef enum {
    IOTX_LINKKIT_DEV_TYPE_MASTER,
    IOTX_LINKKIT_DEV_TYPE_SLAVE,
    IOTX_LINKKIT_DEV_TYPE_MAX
} iotx_linkkit_dev_type_t;

typedef struct {
    char product_key[PRODUCT_KEY_MAXLEN];
    char product_secret[PRODUCT_SECRET_MAXLEN];
    char device_name[DEVICE_NAME_MAXLEN];
    char device_secret[DEVICE_SECRET_MAXLEN];
} iotx_linkkit_dev_meta_info_t;

typedef enum {
    /* post property value to cloud */
    ITM_MSG_POST_PROPERTY,

    /* post device info update message to cloud */
    ITM_MSG_DEVICEINFO_UPDATE,

    /* post device info delete message to cloud */
    ITM_MSG_DEVICEINFO_DELETE,

    /* post raw data to cloud */
    ITM_MSG_POST_RAW_DATA,

    /* only for slave device, send login request to cloud */
    ITM_MSG_LOGIN,

    /* only for slave device, send logout request to cloud */
    ITM_MSG_LOGOUT,

    /* query ntp time from cloud */
    ITM_MSG_QUERY_TIMESTAMP,

    /* only for master device, query topo list */
    ITM_MSG_QUERY_TOPOLIST,

    IOTX_LINKKIT_MSG_MAX
} iotx_linkkit_msg_type_t;

/**
 * @brief create a new device
 *
 * @param dev_type. type of device which will be created. see iotx_linkkit_dev_type_t
 * @param meta_info. The product key, product secret, device name and device secret of new device.
 *
 * @return success: device id (>=0), fail: -1.
 *
 */
int IOT_Linkkit_Open(iotx_linkkit_dev_type_t dev_type, iotx_linkkit_dev_meta_info_t *meta_info);

/**
 * @brief start device network connection.
 *        for master device, start to connect aliyun server.
 *        for slave device, send message to cloud for register new device and add topo with master device
 *
 * @param devid. device identifier.
 * @param timeout_ms. connect timeout, only for master device.
 *
 * @return success: device id (>=0), fail: -1.
 *
 */
int IOT_Linkkit_Connect(int devid, int timeout_ms);

/**
 * @brief try to receive message from cloud and dispatch these message to user event callback
 *
 * @param timeout_ms. timeout for waiting new message arrived
 *
 * @return void.
 *
 */
void IOT_Linkkit_Yield(int timeout_ms);

/**
 * @brief close device network connection and release resources.
 *        for master device, disconnect with aliyun server and release all local resources.
 *        for slave device, send message to cloud for delete topo with master device and unregister itself, then release device's resources.
 *
 * @param devid. device identifier.
 *
 * @return success: 0, fail: -1.
 *
 */
int IOT_Linkkit_Close(int devid);

/**
 * @brief post message to cloud
 *
 * @param devid. device identifier.
 * @param msg_type. message type. see iotx_linkkit_msg_type_t
 *
 * @param payload. message payload.
 * @param payload_len. message payload length.
 *
 * @return success: 0 or message id (>=1), fail: -1.
 *
 */
int IOT_Linkkit_Report(int devid, iotx_linkkit_msg_type_t msg_type, unsigned char *payload, int payload_len);

/**
 * @brief post event to cloud
 *
 * @param devid. device identifier.
 * @param eventid. tsl event id.
 * @param eventid_len. length of tsl event id.
 * @param payload. event payload.
 * @param payload_len. event payload length.
 *
 * @return success: message id (>=1), fail: -1.
 *
 */
int IOT_Linkkit_TriggerEvent(int devid, char *eventid, int eventid_len, char *payload, int payload_len);

#endif

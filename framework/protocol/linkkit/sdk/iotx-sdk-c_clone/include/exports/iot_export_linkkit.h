/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef _IOT_EXPORT_LINKKIT_H_
#define _IOT_EXPORT_LINKKIT_H_

#if defined(__cplusplus)
extern "C" {
#endif

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

    /* only for master device, qurey firmware ota data */
    ITM_MSG_QUERY_FOTA_DATA,

    /* only for master device, qurey config ota data */
    ITM_MSG_QUERY_COTA_DATA,

    /* only for master device, request config ota data from cloud */
    ITM_MSG_REQUEST_COTA,

    /* only for master device, request fota image from cloud */
    ITM_MSG_REQUEST_FOTA_IMAGE,

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
DLL_IOT_API int IOT_Linkkit_Open(iotx_linkkit_dev_type_t dev_type, iotx_linkkit_dev_meta_info_t *meta_info);

/**
 * @brief start device network connection.
 *        for master device, start to connect aliyun server.
 *        for slave device, send message to cloud for register new device and add topo with master device
 *
 * @param devid. device identifier.
 *
 * @return success: device id (>=0), fail: -1.
 *
 */
DLL_IOT_API int IOT_Linkkit_Connect(int devid);

/**
 * @brief try to receive message from cloud and dispatch these message to user event callback
 *
 * @param timeout_ms. timeout for waiting new message arrived
 *
 * @return void.
 *
 */
DLL_IOT_API void IOT_Linkkit_Yield(int timeout_ms);

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
DLL_IOT_API int IOT_Linkkit_Close(int devid);

/**
 * @brief Report message to cloud
 *
 * @param devid. device identifier.
 * @param msg_type. message type. see iotx_linkkit_msg_type_t, as follows:
 *        ITM_MSG_POST_PROPERTY
 *        ITM_MSG_DEVICEINFO_UPDATE
 *        ITM_MSG_DEVICEINFO_DELETE
 *        ITM_MSG_POST_RAW_DATA
 *        ITM_MSG_LOGIN
 *        ITM_MSG_LOGOUT
 *
 * @param payload. message payload.
 * @param payload_len. message payload length.
 *
 * @return success: 0 or message id (>=1), fail: -1.
 *
 */
DLL_IOT_API int IOT_Linkkit_Report(int devid, iotx_linkkit_msg_type_t msg_type, unsigned char *payload,
                                   int payload_len);

/**
 * @brief post message to cloud
 *
 * @param devid. device identifier.
 * @param msg_type. message type. see iotx_linkkit_msg_type_t, as follows:
 *        ITM_MSG_QUERY_TIMESTAMP
 *        ITM_MSG_QUERY_TOPOLIST
 *        ITM_MSG_QUERY_FOTA_DATA
 *        ITM_MSG_QUERY_COTA_DATA
 *        ITM_MSG_REQUEST_COTA
 *        ITM_MSG_REQUEST_FOTA_IMAGE
 *
 * @param payload. message payload.
 * @param payload_len. message payload length.
 *
 * @return success: 0 or message id (>=1), fail: -1.
 *
 */
DLL_IOT_API int IOT_Linkkit_Query(int devid, iotx_linkkit_msg_type_t msg_type, unsigned char *payload,
                                  int payload_len);

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
DLL_IOT_API int IOT_Linkkit_TriggerEvent(int devid, char *eventid, int eventid_len, char *payload, int payload_len);

#ifdef LOG_REPORT_TO_CLOUD
    int check_target_msg(const char *input, int len);
    void get_msgid(char *payload, int is_cloud);
    void send_permance_info(char *input, int input_len, char *comments, int report_format);
#endif



#if defined(__cplusplus)
}
#endif
#endif

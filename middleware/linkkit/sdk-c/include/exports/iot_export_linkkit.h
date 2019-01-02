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
    IOTX_LINKKIT_CMD_OPTION_PROPERTY_POST_REPLY,           /* only for master device, choose whether you need receive property post reply message */
    IOTX_LINKKIT_CMD_OPTION_EVENT_POST_REPLY,              /* only for master device, choose whether you need receive event post reply message */
    IOTX_LINKKIT_CMD_OPTION_PROPERTY_SET_REPLY,            /* only for master device, choose whether you need send property set reply message */
    IOTX_LINKKIT_CMD_MAX
} iotx_linkkit_ioctl_cmd_t;

typedef enum {
    /* post property value to cloud */
    IOTX_LINKKIT_MSG_POST_PROPERTY,

    /* post device info update message to cloud */
    IOTX_LINKKIT_MSG_DEVICEINFO_UPDATE,

    /* post device info delete message to cloud */
    IOTX_LINKKIT_MSG_DEVICEINFO_DELETE,

    /* post raw data to cloud */
    IOTX_LINKKIT_MSG_POST_RAW_DATA,

    IOTX_LINKKIT_MSG_MAX
} iotx_linkkit_msg_type_t;

typedef struct {
    /**
     * @brief connected cloud event
     *
     * @return service request success: 0, fail: -1.
     *
     */
    int (* connected)(void);

    /**
     * @brief disconnected cloud event
     *
     * @return service request success: 0, fail: -1.
     *
     */
    int (* disconnected)(void);

    /**
     * @brief raw data received from cloud
     *
     * @param devid. device identifier
     * @param payload. raw data payload
     * @param payload_len. length of raw data payload
     *
     * @return service request success: 0, fail: -1.
     *
     */
    int (* down_raw)(const int devid, const unsigned char *payload, const int payload_len);

    /**
     * @brief post raw data reply from cloud
     *
     * @param devid. device identifier
     * @param payload. raw data payload
     * @param payload_len. length of raw data payload
     *
     * @return service request success: 0, fail: -1.
     *
     */
    int (* up_raw_reply)(const int devid, const unsigned char *payload, const int payload_len);

    /**
     * @brief async service request message from cloud
     *
     * @param devid. device identifier
     * @param serviceid. service id in TSL
     * @param serviceid_len. length of service id
     * @param request. service request payload
     * @param request_len. length of service request payload
     * @param response. user service response payload, should use HAL_Malloc to malloc memory for *response if response exist
     * @param response_len. length of user service response payload.
     *
     * @return service request success: 0, fail: -1.
     *
     */
    int (* async_service_request)(const int devid, const char *serviceid, const int serviceid_len, const char *request,
                                  const int request_len,
                                  char **response, int *response_len);

    /**
    * @brief sync service request message from cloud
    *
    * @param devid. device identifier
    * @param serviceid. service id in TSL
    * @param serviceid_len. length of service id
    * @param request. service request payload
    * @param request_len. length of service request payload
    * @param response. user service response payload, should use HAL_Malloc to malloc memory for *response if response exist
    * @param response_len. length of user service response payload.
    *
    * @return service request success: 0, fail: -1.
    *
    */
    int (* sync_service_request)(const int devid, const char *serviceid, const int serviceid_len, const char *request,
                                 const int request_len,
                                 char **response, int *response_len);

    /**
     * @brief property set message from cloud
     *
     * @param devid. device identifier
     * @param request. property set payload
     * @param request_len. length of property set payload
     *
     * @return service request success: 0, fail: -1.
     *
     */
    int (* property_set)(const int devid, const char *request, const int request_len);

    /**
     * @brief property get message from cloud
     *
     * @param devid. device identifier
     * @param request. property get payload
     * @param request_len. length of property get payload
     * @param response. user property get response payload, should use HAL_Malloc to malloc memory for *response if response exist
     * @param response_len. length of user property get response payload.
     *
     * @return property get response success: 0, fail: -1.
     *
     */
    int (* property_get)(const int devid, const char *request, const int request_len, char **response, int *response_len);

    /**
     * @brief response message from cloud
     *
     * @param devid. device identifier
     * @param msgid. message id, same with return value of IOT_Linkkit_Post
     * @param code. reply code from cloud, success: 200
     * @param payload. reply payload, it's different from different message
     * @param payload_len. length of reply payload
     *
     * @return service request success: 0, fail: -1.
     *
     */
    int (* post_reply)(const int devid, const int msgid, const int code, const char *reply, const int reply_len);

    /**
     * @brief UTC timestamp from cloud
     *
     * @param utc. UTC timestamp
     *
     * @return service request success: 0, fail: -1.
     *
     */
    int (* ntp_response)(const char *utc);
    int (* permit_join)(void);
    int (* initialized)(const int devid);
} iotx_linkkit_event_handler_t;

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
 * @brief configuration runtime parameter before network established.
 *
 * @param devid. device identifier.
 * @param cmd. command. see iotx_ioctl_cmd_t
 * @param arg. command argument.
 *
 * @return success: device id (>=0), fail: -1.
 *
 */
int IOT_Linkkit_Ioctl(int devid, iotx_linkkit_ioctl_cmd_t cmd, void *arg);

/**
 * @brief start device network connection.
 *        for master device, start to connect aliyun server.
 *        for slave device, send message to cloud for register new device and add topo with master device
 *
 * @param devid. device identifier.
 * @param iotx_linkkit_event_handler_t. event callback, only for master device. see iotx_linkkit_event_handler_t
 *
 * @return success: device id (>=0), fail: -1.
 *
 */
int IOT_Linkkit_Connect(int devid, iotx_linkkit_event_handler_t *hdlrs);

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
 * @param msg_type. message type. see iotx_linkkit_msg_type_t.
 * @param identifier. optional parameter accroding to iotx_linkkit_msg_type_t.
 * @param payload. message payload.
 * @param payload_len. message payload length.
 *
 * @return success: 0 or message id (>=1), fail: -1.
 *
 */
int IOT_Linkkit_Post(int devid, iotx_linkkit_msg_type_t msg_type, unsigned char *payload, int payload_len);

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
#ifndef __OTA_PRIVATE_H__
#define __OTA_PRIVATE_H__

#if defined(__cplusplus)
extern "C" {
#endif

#include "core_stdinc.h"

#include "core_http.h"
#include "aiot_ota_api.h"

#define OTA_VERSION_TOPIC_PREFIX             "/ota/device/inform"
#define OTA_PROGRESS_TOPIC_PREFIX            "/ota/device/progress"

#define OTA_MODULE_NAME                      "OTA"
#define DOWNLOAD_MODULE_NAME                 "DOWNLOAD"

#define OTA_DEFAULT_DOWNLOAD_BUFLEN          (2 * 1024)
#define OTA_DEFAULT_DOWNLOAD_TIMEOUT_MS      (5 * 1000)

#define OTA_FOTA_TOPIC                       "/ota/device/upgrade/+/+"
#define OTA_FOTA_TOPIC_PREFIX                "/ota/device/upgrade"
#define OTA_COTA_PUSH_TOPIC                  "/sys/+/+/thing/config/push"
#define OTA_COTA_PUSH_TOPIC_POSTFIX          "/thing/config/push"
#define OTA_COTA_GET_REPLY_TOPIC             "/sys/+/+/thing/config/get_reply"
#define OTA_COTA_GET_REPLY_TOPIC_POSTFIX     "/thing/config/get_reply"
#define OTA_COTA_TOPIC_PREFIX                "/sys/"
#define OTA_GET_TOPIC_PREFIX                 "/sys"
#define OTA_GET_TOPIC_SUFFIX                 "thing/ota/firmware/get"
#define OTA_GET_REPLY_TOPIC_SUFFIX           "thing/ota/firmware/get_reply"
#define OTA_OTA_GET_REPLY_TOPIC              "/sys/+/+/thing/ota/firmware/get_reply"

#define OTA_HTTPCLIENT_MAX_URL_LEN           (256)
#define OTA_MAX_DIGIT_NUM_OF_UINT32          (20)
#define OTA_RESPONSE_PARTIAL                 (206)
#define OTA_RESPONSE_OK                      (200)
#define OTA_TOPIC_NUM                        (4)
#define OTA_MD5_LEN                          (32)
#define OTA_SHA256_LEN                       (64)

typedef enum {
    DOWNLOAD_STATUS_START,
    DOWNLOAD_STATUS_FETCH,
    DOWNLOAD_STATUS_RENEWAL,
} download_status_t;

typedef enum {
    OTA_TYPE_FOTA,
    OTA_TYPE_CONFIG_PUSH,
    OTA_TYPE_CONFIG_GET,
} ota_type_t;

/**
 * @brief OTA过程中处理mqtt消息的句柄, 该句柄主要用于通过mqtt协议从云端收取固件升级消息, 包括固件的url等
 *
 */
typedef struct {
    void                        *userdata;            /* 组件调用recv_handler的入参之一, 传入用户数据 */
    aiot_ota_recv_handler_t     recv_handler;         /* OTA的mqtt消息到达设备端时, 通知用户的回调 */
    aiot_sysdep_portfile_t      *sysdep;

    /*---- 以上都是用户在API可配 ----*/
    /*---- 以下都是OTA内部使用, 用户无感知 ----*/

    void            *mqtt_handle;
    void            *module;
    void            *data_mutex;
} ota_handle_t;

/**
 * @brief 处理下载任务的句柄, 该句柄主要用于通过http协议从指定的url下载固件
 *
 */
typedef struct {
    void
    *userdata;       /* 组件调用recv_handler 时的入参之一, 传入用户数据 */
    aiot_download_recv_handler_t       recv_handler;    /* 设备收到分段的固件报文时的回调函数 */
    aiot_download_task_desc_t          *task_desc;      /* 某次下载活动的目标描述信息, 如URL等 */
    aiot_sysdep_portfile_t             *sysdep;
    uint32_t                           range_start;
    uint32_t                           range_end;

    /*---- 以上都是用户在API可配 ----*/
    /*---- 以下都是downloader内部使用, 用户无感知 ----*/

    uint8_t         download_status;
    void            *http_handle;
    uint32_t        size_fetched;
    uint32_t        range_size_fetched;
    uint32_t        content_len;
    int32_t         percent;
    int32_t         http_rsp_status_code;
    void            *digest_ctx;
    void            *data_mutex;
    void            *recv_mutex;
} download_handle_t;

#if defined(__cplusplus)
}
#endif

#endif  /* __OTA_PRIVATE_H__ */


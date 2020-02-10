/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "linkkit/infra/infra_types.h"
#include "linkkit/infra/infra_defs.h"
#include "linkkit/infra/infra_compat.h"
#include "linkkit/dev_model_api.h"
#include "linkkit/wrappers/wrappers.h"

#ifdef INFRA_MEM_STATS
    #include "linkkit/infra/infra_mem_stats.h"
#endif
#include "cJSON.h"
#include "app_entry.h"

// for demo only
#define PRODUCT_KEY      "a1ikrQdGiG8"
#define PRODUCT_SECRET   "UgqWfzL5B2zNvglF"
#define DEVICE_NAME      "test_cd_02"
#define DEVICE_SECRET    "O0wxZBV3AEL5SKCpXmc6vGr2B6eMFxdS"

#define EXAMPLE_TRACE(fmt, ...)  \
    do { \
        HAL_Printf("%s|%03d :: ", __func__, __LINE__); \
        HAL_Printf(fmt, ##__VA_ARGS__); \
        HAL_Printf("%s", "\r\n"); \
    } while(0)

/* json data of property "CountDown", used for debugging in AliCloud console
{
  "CountDown": {"IsRunning":1,"TimeLeft":10,"PowerSwitch":1,"Timestamp":"1534314100000"}
}
*/

/* Properties used define */
#define PROPERTY_POWERSWITCH            "PowerSwitch"
#define PROPERTY_COUNTDOWN              "CountDown"
#define PROPERTY_ITEM_TIMELEFT          "TimeLeft"
#define PROPERTY_ITEM_ISRUNNING         "IsRunning"
#define PROPERTY_ITEM_POWERSWITCH       "PowerSwitch"
#define PROPERTY_ITEM_TIMESTAMP         "Timestamp"

/* app context type define */
typedef struct _app_context {
    int         devid;
    int         inited;
    int         connected;

    void        *timerHandle;
    char        timestamp[20];
    int         powerSwitch_Actual;
    int         powerSwitch_Target;
} app_context_t;

static app_context_t *app_context;

static void app_set_context(app_context_t *ctx)
{
    app_context = ctx;
}

static void *app_get_context(void)
{
    return app_context;
}

/*
 * Property CountDown paylaod construction and post
 */
static int app_post_countdown(int isrun, int timelf, int pwrsw, char *timestamp, app_context_t *app_ctx)
{
    int ret = -1;
    char *payload = NULL;
    cJSON *root, *prop;

    root = cJSON_CreateObject();
    if (root == NULL) {
        return ret;
    }

    prop = cJSON_CreateObject();
    if (prop == NULL) {
        cJSON_Delete(root);
        return ret;
    }

    cJSON_AddItemToObject(root, PROPERTY_COUNTDOWN, prop);
    cJSON_AddNumberToObject(prop, PROPERTY_ITEM_ISRUNNING, isrun);
    cJSON_AddNumberToObject(prop, PROPERTY_ITEM_TIMELEFT, timelf);
    cJSON_AddNumberToObject(prop, PROPERTY_ITEM_POWERSWITCH, pwrsw);
    cJSON_AddStringToObject(prop, PROPERTY_ITEM_TIMESTAMP, timestamp);

    payload = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);

    /* Post the CountDown property */
    if (payload == NULL) {
        return ret;
    }
    ret = IOT_Linkkit_Report(app_ctx->devid, ITM_MSG_POST_PROPERTY, (unsigned char *)payload, strlen(payload));
    HAL_Free(payload);
    if (ret < 0) {
        EXAMPLE_TRACE("app post property \"CountDown\" failed");
        return ret;
    }
    EXAMPLE_TRACE("app post property \"CountDown\" succeed, msgID = %d\r\n", ret);

    return ret;
}

/*
 * Property PowerSwitch paylaod construction and post
 */
static int app_post_powerswitch(int pwrsw, app_context_t *app_ctx)
{
    int ret = -1;
    char *payload = NULL;
    cJSON *root = cJSON_CreateObject();
    if (root == NULL) {
        return ret;
    }

    cJSON_AddNumberToObject(root, PROPERTY_POWERSWITCH, pwrsw);
    payload = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);

    /* Post the PowerSwitch property */
    if (payload == NULL) {
        return ret;
    }

    ret = IOT_Linkkit_Report(app_ctx->devid, ITM_MSG_POST_PROPERTY, (unsigned char *)payload, strlen(payload));
    HAL_Free(payload);
    if (ret < 0) {
        EXAMPLE_TRACE("app post property \"PowerSwitch\" failed");
        return ret;
    }
    EXAMPLE_TRACE("app post property \"PowerSwitch\" secceed, msgID = %d\r\n", ret);

    return ret;
}

/*
 * Timer expired handle funciton, trigger powerswitch action and post specific property here
 */
static void app_timer_expired_handle(void *ctx)
{
    app_context_t *app_ctx = app_get_context();
    if (NULL == app_ctx) {
        EXAMPLE_TRACE("can't get app context, just return");
        return;
    }

    EXAMPLE_TRACE("app timer expired!\r\n");

    /* Trigger powerswitch action, just set value here */
    app_ctx->powerSwitch_Actual = app_ctx->powerSwitch_Target;

    /* Set PowerSwitch property then post */
    app_post_powerswitch(app_ctx->powerSwitch_Actual, app_ctx);

    /* Set CountDown property then post */
    app_post_countdown(0, 0, app_ctx->powerSwitch_Target, app_ctx->timestamp, app_ctx);
}

static void *app_timer_open(void *ctx)
{
    return HAL_Timer_Create("App_CountDown", app_timer_expired_handle, ctx);
}

static void app_timer_close(void *timer)
{
    HAL_Timer_Delete(timer);
}

static void app_timer_stop(void *timer)
{
    HAL_Timer_Stop(timer);
    EXAMPLE_TRACE("app timer stop");
}

static void app_timer_start(void *timer, int s)
{
    HAL_Timer_Start(timer, s * 1000);
    EXAMPLE_TRACE("app timer start");
}

/*
 * cloud connected
 */
static int on_connect(void)
{
    app_context_t *app_ctx = app_get_context();
    if (app_ctx == NULL) {
        return -1;
    }

    app_ctx->connected = 1;
    EXAMPLE_TRACE("device is connected\n");

    /*
     * implement user process routine here
     */

    return 0;
}

/*
 * cloud disconnected
 */
static int on_disconnect(void)
{
    app_context_t *app_ctx = app_get_context();
    if (app_ctx == NULL) {
        return -1;
    }

    app_ctx->connected = 0;
    EXAMPLE_TRACE("device is disconnect\n");

    /*
     * implement user process routine here
     */

    return 0;
}

/*
 * Property set from cloud handler
 */
static int property_set_handle(const int devid, const char *payload, const int payload_len)
{
    int ret = -1;
    int powerSwitch = 0;
    int timeLeft = 0;
    int isRunning = 0;
    cJSON *root, *prop, *item;
    app_context_t *app_ctx = app_get_context();
    if (app_ctx == NULL) {
        return ret;
    }

    EXAMPLE_TRACE("property set, payload: \"%s\"", payload);

    root = cJSON_Parse(payload);
    if (root == NULL) {
        EXAMPLE_TRACE("property set payload is not JSON format");
        return ret;
    }

    prop = cJSON_GetObjectItem(root, PROPERTY_COUNTDOWN);
    if (prop == NULL || !cJSON_IsObject(prop)) {
        prop = cJSON_GetObjectItem(root, PROPERTY_POWERSWITCH);
        if (prop == NULL || !cJSON_IsNumber(prop)) {
            cJSON_Delete(root);
            return ret;
        }
        EXAMPLE_TRACE("property is PowerSwitch");

        app_ctx->powerSwitch_Actual = prop->valueint;
        EXAMPLE_TRACE("PowerSwitch actual value set to %d\r\n", app_ctx->powerSwitch_Actual);
        cJSON_Delete(root);

        /* Post PowerSwitch value */
        app_post_powerswitch(app_ctx->powerSwitch_Actual, app_ctx);

        return ret;
    }

    EXAMPLE_TRACE("property is CountDown");

    /* Get powerSwitch value */
    item = cJSON_GetObjectItem(prop, PROPERTY_ITEM_POWERSWITCH);
    if (item != NULL && cJSON_IsNumber(item)) {
        powerSwitch = item->valueint;
        EXAMPLE_TRACE("PowerSwitch target value is %d", powerSwitch);
    } else {
        cJSON_Delete(root);
        return ret;
    }

    /* Get timeLeft value */
    item = cJSON_GetObjectItem(prop, PROPERTY_ITEM_TIMELEFT);
    if (item != NULL && cJSON_IsNumber(item)) {
        timeLeft = item->valueint;
        EXAMPLE_TRACE("TimeLeft is %d", timeLeft);
    } else {
        cJSON_Delete(root);
        return ret;
    }

    /* Get isRunning value */
    item = cJSON_GetObjectItem(prop, PROPERTY_ITEM_ISRUNNING);
    if (item != NULL && cJSON_IsNumber(item)) {
        isRunning = item->valueint;
        EXAMPLE_TRACE("IsRunning is %d", isRunning);
    } else {
        cJSON_Delete(root);
        return ret;
    }

    /* Get timeStamp value */
    item = cJSON_GetObjectItem(prop, PROPERTY_ITEM_TIMESTAMP);
    if (item != NULL && cJSON_IsString(item)) {
        int len = strlen(item->valuestring);
        if (len < 20) {
            memset(app_ctx->timestamp, 0, sizeof(app_ctx->timestamp));
            memcpy(app_ctx->timestamp, item->valuestring, len);
            EXAMPLE_TRACE("Timestamp is %s", app_ctx->timestamp);
        } else {
            EXAMPLE_TRACE("Timestamp string error");
            cJSON_Delete(root);
            return ret;
        }
    } else {
        cJSON_Delete(root);
        return ret;
    }

    /* Delete Json */
    cJSON_Delete(root);

    /* Start or stop timer according to "IsRunning" */
    if (isRunning == 1) {
        app_timer_start(app_ctx->timerHandle, timeLeft);
        /* temp powerswitch value to app context */
        app_ctx->powerSwitch_Target = powerSwitch;
    } else if (isRunning == 0) {
        app_timer_stop(app_ctx->timerHandle);
    }

    /* Just echo the CountDown property */
    ret = IOT_Linkkit_Report(app_ctx->devid, ITM_MSG_POST_PROPERTY, (unsigned char *)payload, payload_len);
    if (ret < 0) {
        EXAMPLE_TRACE("app post property \"CountDown\" failed");
        return ret;
    }
    EXAMPLE_TRACE("app post property \"CountDown\" succeed, msgID = %d\r\n", ret);

    /* Post the PowerSwitch property, powerSwitch_Actual used */
    /* Post PowerSwitch value */
    app_post_powerswitch(app_ctx->powerSwitch_Actual, app_ctx);

    return 0;
}

static int report_reply_handle(const int devid, const int msgid, const int code, const char *payload,
                               const int payload_len)
{
    EXAMPLE_TRACE("thing@%p: response arrived: {id:%d, code:%d, message:%s}\n", devid, msgid, code,
                  payload == NULL ? "NULL" : payload);

    /*
     * implement user process routine here
     */

    return 0;
}

/*
 * Device inited handle
 */
static int initialized_handle(const int devid)
{
    app_context_t *app_ctx = app_get_context();

    app_ctx->inited = 1;
    EXAMPLE_TRACE("device %d inited\n", devid);

    /*
     * implement user process routine here
     */

    return 0;
}

static uint64_t user_update_sec(void)
{
    uint64_t start_time = 0;

    if (start_time == 0) {
        start_time = HAL_UptimeMs();
    }

    return (HAL_UptimeMs() - start_time) / 1000;
}

/*
 * linkkit timer countdown example routine
 */
int linkkit_example()
{
    int ret = 0;
    uint64_t time_prev_sec = 0, time_now_sec = 0;
    app_context_t app_ctx = {0};

    /* init app context */
    app_ctx.timerHandle = app_timer_open(NULL);
    app_ctx.powerSwitch_Actual = 0;     /* assume initial status of PowerSwitch is 0 */
    app_ctx.powerSwitch_Target = 0;
    app_set_context(&app_ctx);

    /* init linkkit device metadata */
    iotx_linkkit_dev_meta_info_t linkkit_meta_info;

    memset(&linkkit_meta_info, 0, sizeof(iotx_linkkit_dev_meta_info_t));
    HAL_GetProductKey(linkkit_meta_info.product_key);
    HAL_GetDeviceName(linkkit_meta_info.device_name);
    HAL_GetProductSecret(linkkit_meta_info.product_secret);
    HAL_GetDeviceSecret(linkkit_meta_info.device_secret);

    /* Register Callback */
    IOT_RegisterCallback(ITE_CONNECT_SUCC, on_connect);
    IOT_RegisterCallback(ITE_DISCONNECTED, on_disconnect);
    IOT_RegisterCallback(ITE_PROPERTY_SET, property_set_handle);
    IOT_RegisterCallback(ITE_REPORT_REPLY, report_reply_handle);
    IOT_RegisterCallback(ITE_INITIALIZE_COMPLETED, initialized_handle);

    /*
     * Open linkkit device
     */
    app_ctx.devid = IOT_Linkkit_Open(IOTX_LINKKIT_DEV_TYPE_MASTER, &linkkit_meta_info);
    if (ret < 0) {
        EXAMPLE_TRACE("linkkit device open fail");
        return -1;
    }

    /*
     * Config linkkit device option
     */
    int post_property_reply = 1;
    IOT_Ioctl(IOTX_IOCTL_RECV_PROP_REPLY, (void *)&post_property_reply);

    /*
     * Start device network connection
     */
    ret = IOT_Linkkit_Connect(app_ctx.devid);
    if (ret < 0) {
        EXAMPLE_TRACE("linkkit connect fail");
        return -1;
    }

    EXAMPLE_TRACE("linkkit started, enter loop");
    while (1) {
        /* Do linkkit yeild first */
        IOT_Linkkit_Yield(200);

        time_now_sec = user_update_sec();
        if (time_prev_sec == time_now_sec) {
            continue;
        }


        if (time_now_sec % 10) {
            /*
            * implement user process routine here
            */
        }

        time_prev_sec = time_now_sec;
    }

    /* app end */
    IOT_Linkkit_Close(app_ctx.devid);
    app_timer_stop(app_ctx.timerHandle);
    app_timer_close(app_ctx.timerHandle);

    return 0;
}

void set_iotx_info()
{
    char _device_name[IOTX_DEVICE_NAME_LEN + 1] = {0};
    HAL_GetDeviceName(_device_name);
    if (strlen(_device_name) == 0) {
        HAL_SetProductKey(PRODUCT_KEY);
        HAL_SetProductSecret(PRODUCT_SECRET);
        HAL_SetDeviceName(DEVICE_NAME);
        HAL_SetDeviceSecret(DEVICE_SECRET);
    }
}

int linkkit_main(void *paras)
{
#if !defined(WIFI_PROVISION_ENABLED) || !defined(BUILD_AOS)
    set_iotx_info();
#endif
    IOT_SetLogLevel(IOT_LOG_INFO);

    EXAMPLE_TRACE("start!\n");
    /*
     * linkkit timer countdown demo
     * please check document: https://help.aliyun.com/document_detail/73708.html?spm=a2c4g.11174283.6.560.zfcQ3y
     *         API introduce:  https://help.aliyun.com/document_detail/68687.html?spm=a2c4g.11186623.6.627.RJcT3F
     */
    linkkit_example();

    IOT_DumpMemoryStats(IOT_LOG_DEBUG);

    EXAMPLE_TRACE("out of example!\n");

    IOT_SetLogLevel(IOT_LOG_NONE);

    return 0;
}
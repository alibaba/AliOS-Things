/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "iot_import.h"
#include "iot_export.h"
#include "linkkit_export.h"
#include "app_entry.h"

/*
 * please modify this string follow as product's TSL.
 */
#include "data/cntdown_tsl.data"

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

/* Properties used define */
#define PROPERTY_POWERSWITCH            "PowerSwitch"
#define PROPERTY_COUNTDOWN              "CountDown"
#define PROPERTY_COUNTDOWN_POWERSWITCH  "CountDown.PowerSwitch"
#define PROPERTY_COUNTDOWN_TIMELEFT     "CountDown.TimeLeft"
#define PROPERTY_COUNTDOWN_ISRUNNING    "CountDown.IsRunning"

/* app context type define */
typedef struct _app_context {
    const void *thing;
    const char *prop_powerswitch;
    const char *prop_countdown;
    const char *prop_countdown_pwrsw;
    const char *prop_countdown_timelf;
    const char *prop_countdown_isrun;
    void       *timerHandle;
    int        powerSwitch_Actual;
    int        powerSwitch_Target;
    int        cloud_connected;
    int        local_connected;
    int        thing_enabled;
} app_context_t;

/* json data of property "CountDown", used for debugging in AliCloud console
{
  "CountDown": {"IsRunning":1,"TimeLeft":10,"PowerSwitch":1,"Timestamp":"1534314100000"}
}
*/

/*
 * the callback of linkkit_post_property.
 * response_id is compare with the result of linkkit_post_property.
 *
 */
void post_property_cb(const void *thing_id, int response_id, int code, const char *response_message, void *ctx)
{
    EXAMPLE_TRACE("thing@%p: response arrived: {id:%d, code:%d, message:%s}\n", thing_id, response_id, code,
              response_message == NULL ? "NULL" : response_message);

    /* do user's post property callback process logical here. */

    /* ............................... */

    /* user's post property callback process logical complete */
}

/*
 * Timer expired handle funciton, trigger powerswitch action and post specific property here
 */
static void app_timer_expired_handle(void *ctx)
{
    int ret = -1;
    app_context_t *app_ctx = (app_context_t *)ctx;
    int timeLeft = 0;
    int isrunning = 0;

    EXAMPLE_TRACE("app timer expired!\r\n");

    if (NULL == app_ctx) {
        EXAMPLE_TRACE("can't get app context, just return\r\n");
        return;
    }
    /* Trigger powerswitch action, just set value here */
    app_ctx->powerSwitch_Actual = app_ctx->powerSwitch_Target;

    /* Set PowerSwitch property then post */
    ret = linkkit_set_value(linkkit_method_set_property_value, app_ctx->thing, app_ctx->prop_powerswitch,
                            &app_ctx->powerSwitch_Target, NULL);
    if (ret != SUCCESS_RETURN) {
        EXAMPLE_TRACE("app set property \"%s\" failed", app_ctx->prop_powerswitch);
        return;
    }

    ret = linkkit_post_property(app_ctx->thing, app_ctx->prop_powerswitch, post_property_cb);
    if (ret != SUCCESS_RETURN) {
        EXAMPLE_TRACE("app post property \"%s\" failed", app_ctx->prop_powerswitch);
        return;
    }
    EXAMPLE_TRACE("app post property \"%s\" succeed", app_ctx->prop_powerswitch);

    /* Set CountDown property then post */
    ret = linkkit_set_value(linkkit_method_set_property_value, app_ctx->thing, app_ctx->prop_countdown_timelf, &timeLeft,
                            NULL);
    if (ret != SUCCESS_RETURN) {
        EXAMPLE_TRACE("app set property \"%s\" failed", app_ctx->prop_countdown);
        return;
    }

    ret = linkkit_set_value(linkkit_method_set_property_value, app_ctx->thing, app_ctx->prop_countdown_isrun, &isrunning,
                            NULL);
    if (ret != SUCCESS_RETURN) {
        EXAMPLE_TRACE("app set property \"%s\" failed", app_ctx->prop_countdown);
        return;
    }

    ret = linkkit_post_property(app_ctx->thing, app_ctx->prop_countdown, post_property_cb);
    if (ret != SUCCESS_RETURN) {
        EXAMPLE_TRACE("app post property \"%s\" failed", app_ctx->prop_countdown);
        return;
    }
    EXAMPLE_TRACE("app post property \"%s\" succeed", app_ctx->prop_countdown);
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

/* connect handle
 * cloud and local
 */
#ifdef LOCAL_CONN_ENABLE
    static int on_connect(void *ctx, int cloud)
#else
    static int on_connect(void *ctx)
#endif
{
    app_context_t *app_ctx = ctx;

#ifdef LOCAL_CONN_ENABLE
    if (cloud) {
        app_ctx->cloud_connected = 1;
    } else {
        app_ctx->local_connected = 1;
    }
    EXAMPLE_TRACE("%s is connected\n", cloud ? "cloud" : "local");
#else
    app_ctx->cloud_connected = 1;
    EXAMPLE_TRACE("%s is connected\n", "cloud");
#endif
    /* do user's connect process logical here. */

    /* ............................... */

    /* user's connect process logical complete */

    return 0;
}


/* disconnect handle
 * cloud and local
 */
#ifdef LOCAL_CONN_ENABLE
    static int on_disconnect(void *ctx, int cloud)
#else
    static int on_disconnect(void *ctx)
#endif
{
    app_context_t *app_ctx = ctx;

#ifdef LOCAL_CONN_ENABLE
    if (cloud) {
        app_ctx->cloud_connected = 0;
    } else {
        app_ctx->local_connected = 0;
    }
    EXAMPLE_TRACE("%s is disconnect\n", cloud ? "cloud" : "local");
#else
    app_ctx->cloud_connected = 0;
    EXAMPLE_TRACE("%s is disconnect\n", "cloud");
#endif

    /* do user's disconnect process logical here. */

    /* ............................... */

    /* user's disconnect process logical complete */
    return 0;
}


/*
 * receive raw data handler
 */
static int raw_data_arrived(const void *thing_id, const void *data, int len, void *ctx)
{
    char raw_data[128] = {0};

    EXAMPLE_TRACE("raw data arrived,len:%d\n", len);

    /* do user's raw data process logical here. */

    /* ............................... */

    /* user's raw data process logical complete */

    /* send result to cloud
     * please send your data via raw_data
     * example rule: just reply a string to check
     */
    snprintf(raw_data, sizeof(raw_data), "test down raw reply data %lld", (long long)HAL_UptimeMs());
    /* answer raw data handle result */
    linkkit_invoke_raw_service(thing_id, 0, raw_data, strlen(raw_data));

    return 0;
}


/* thing create succuss */
static int thing_create(const void *thing_id, void *ctx)
{
    app_context_t *app_ctx = ctx;

    EXAMPLE_TRACE("new thing@%p created.\n", thing_id);
    app_ctx->thing = thing_id;

    /* do user's thing create process logical here. */

    /* ............................... */

    /* user's thing create process logical complete */

    return 0;
}


/* thing enable
 * thing is enabled, than it can be communicated
 */
static int thing_enable(const void *thing_id, void *ctx)
{
    app_context_t *app_ctx = ctx;

    app_ctx->thing_enabled = 1;

    /* do user's thing enable process logical here. */

    /* ............................... */

    /* user's thing enable process logical complete */

    return 0;
}


/* thing disable
 * thing is disable, than it can not be communicated
 */
static int thing_disable(const void *thing_id, void *ctx)
{
    app_context_t *app_ctx = ctx;

    app_ctx->thing_enabled = 0;

    /* do user's thing disable process logical here. */

    /* ............................... */

    /* user's thing disable process logical complete */

    return 0;
}


/*
 * this is the "custom" service handler
 * alink method: thing.service.Custom
 * please follow TSL modify the idendifier
 */
#ifdef RRPC_ENABLED
static int handle_service_custom(app_context_t *_app_ctx, const void *thing, const char *service_identifier,
                                 int request_id, int rrpc)
#else
static int handle_service_custom(app_context_t *_app_ctx, const void *thing, const char *service_identifier,
                                 int request_id)
#endif /* RRPC_ENABLED */
{
    char identifier[128] = {0};
    /*
     * please follow TSL modify the value type
     */
    int transparency_value;
    int contrastratio_value;

    /*
     * get iutput value.
     * compare the service identifier
     * please follow user's TSL modify the "transparency".
     */
    snprintf(identifier, sizeof(identifier), "%s.%s", service_identifier, "transparency");
    linkkit_get_value(linkkit_method_get_service_input_value, thing, identifier, &transparency_value, NULL);
    EXAMPLE_TRACE("identifier: %s value is %d.\n", identifier, transparency_value);

    /*
     * set output value according to user's process result.
     * example rule: Contrastratio will changed by transparency.
     */

    /* do user's service process logical here. */

    /* ............................... */

    /* user's service process logical complete */


    /*
     * please follow user's TSL modify the "transparency".
     */
    snprintf(identifier, sizeof(identifier), "%s.%s", service_identifier, "Contrastratio");
    contrastratio_value = transparency_value + 1;
    linkkit_set_value(linkkit_method_set_service_output_value, thing, identifier, &contrastratio_value, NULL);
#ifdef RRPC_ENABLED
    linkkit_answer_service(thing, service_identifier, request_id, 200, rrpc);
#else
    linkkit_answer_service(thing, service_identifier, request_id, 200);
#endif /* RRPC_ENABLED */

    return 0;
}


/*
 * the handler of service which is defined by identifier, not property
 * alink method: thing.service.{tsl.service.identifier}
 */
#ifdef RRPC_ENABLED
    static int thing_call_service(const void *thing_id, const char *service, int request_id, int rrpc, void *ctx)
#else
    static int thing_call_service(const void *thing_id, const char *service, int request_id, void *ctx)
#endif /* RRPC_ENABLED */
{
    app_context_t *app_ctx = ctx;

    EXAMPLE_TRACE("service(%s) requested, id: thing@%p, request id:%d\n",
              service, thing_id, request_id);

    /* please follow TSL modify the idendifier --- Custom */
    if (strcmp(service, "Custom") == 0) {
#ifdef RRPC_ENABLED
        handle_service_custom(app_ctx, thing_id, service, request_id, rrpc);
#else
        handle_service_custom(app_ctx, thing_id, service, request_id);
#endif /* RRPC_ENABLED */
    }

    return 0;
}


/*
 * the handler of property changed
 * alink method: thing.service.property.set
 */
static int thing_prop_changed(const void *thing_id, const char *property, void *ctx)
{
    int ret = -1;
    app_context_t *app_ctx = (app_context_t *)ctx;
    EXAMPLE_TRACE("property \"%s\" changed", property);

    if (strstr(property, app_ctx->prop_countdown) != 0) {
        /* post property
        * result is response_id; if response_id = -1, it is fail, else it is success.
        * response_id by be compare in post_property_cb.
        */
        int powerSwitch[1] = {0};
        int timeLeft[1] = {0};
        int isRunning[1] = {0};

        /* Get property value of "CountDown" */
        linkkit_get_value(linkkit_method_get_property_value, thing_id, app_ctx->prop_countdown_pwrsw, powerSwitch, NULL);
        EXAMPLE_TRACE("app get property value, PowerSwitch = %d", powerSwitch[0]);

        linkkit_get_value(linkkit_method_get_property_value, thing_id, app_ctx->prop_countdown_timelf, timeLeft, NULL);
        EXAMPLE_TRACE("app get property value, TimeLeft = %d", timeLeft[0]);

        linkkit_get_value(linkkit_method_get_property_value, thing_id, app_ctx->prop_countdown_isrun, isRunning, NULL);
        EXAMPLE_TRACE("app get property value, IsRunning = %d", isRunning[0]);

        /* Start or stop timer according to "IsRunning" */
        if (isRunning[0] == 1) {
            app_timer_start(app_ctx->timerHandle, timeLeft[0]);
            /* temp powerswitch value to app context */
            app_ctx->powerSwitch_Target = powerSwitch[0];
        } else if (isRunning[0] == 0) {
            app_timer_stop(app_ctx->timerHandle);
        }

        /* Post property "CountDown" to response request from cloud*/
        ret = linkkit_post_property(thing_id, property, post_property_cb);
        if (ret != SUCCESS_RETURN) {
            EXAMPLE_TRACE("app post property \"%s\" failed", property);
            return ret;
        }
        EXAMPLE_TRACE("app post property \"%s\" succeed", property);

        /* Set property "PowerSwitch" then post */
        ret = linkkit_set_value(linkkit_method_set_property_value, app_ctx->thing, app_ctx->prop_powerswitch,
                                &app_ctx->powerSwitch_Actual, NULL);
        if (ret != SUCCESS_RETURN) {
            EXAMPLE_TRACE("app set property \"%s\" failed", app_ctx->prop_powerswitch);
            return ret;
        }

        ret = linkkit_post_property(app_ctx->thing, app_ctx->prop_powerswitch, post_property_cb);
        if (ret != SUCCESS_RETURN) {
            EXAMPLE_TRACE("app post property \"%s\" failed", app_ctx->prop_powerswitch);
            return ret;
        }
        EXAMPLE_TRACE("app post property \"%s\" succeed", app_ctx->prop_powerswitch);
    } else if (strstr(property, app_ctx->prop_powerswitch) != 0) {
        int powerSwitch[1] = {0};

        /* Get property value of "PowerSwitch" */
        linkkit_get_value(linkkit_method_get_property_value, thing_id, app_ctx->prop_powerswitch, powerSwitch, NULL);
        EXAMPLE_TRACE("app get property value, PowerSwitch = %d", powerSwitch[0]);

        /* Trigger powerswitch action, just set value here */
        app_ctx->powerSwitch_Actual = powerSwitch[0];

        /* Set property "PowerSwitch" then post */
        ret = linkkit_set_value(linkkit_method_set_property_value, app_ctx->thing, app_ctx->prop_powerswitch,
                                &app_ctx->powerSwitch_Actual, NULL);
        if (ret != SUCCESS_RETURN) {
            EXAMPLE_TRACE("app set property \"%s\" failed", app_ctx->prop_powerswitch);
            return ret;
        }

        ret = linkkit_post_property(app_ctx->thing, app_ctx->prop_powerswitch, post_property_cb);
        if (ret != SUCCESS_RETURN) {
            EXAMPLE_TRACE("app post property \"%s\" failed", app_ctx->prop_powerswitch);
            return ret;
        }
        EXAMPLE_TRACE("app post property \"%s\" succeed", app_ctx->prop_powerswitch);
    }

    return 0;
}

/* there is some data transparent transmission by linkkit */
static int linkit_data_arrived(const void *thing_id, const void *params, int len, void *ctx)
{
    EXAMPLE_TRACE("thing@%p: masterdev_linkkit_data(%d byte): %s\n", thing_id, len, (const char *)params);

    /* do user's data arrived process logical here. */

    /* ............................... */

    /* user's data arrived process logical complete */
    return 0;
}


#ifdef POST_WIFI_STATUS

static int is_active(app_context_t *sample_ctx)
{
#ifdef LOCAL_CONN_ENABLE
    return (sample_ctx->cloud_connected /* && sample_ctx->thing_enabled*/) ||
           (sample_ctx->local_connected /* && sample_ctx->thing_enabled*/);
#else
    return sample_ctx->cloud_connected /* && sample_ctx->thing_enabled*/;
#endif
}

typedef struct {
    char *band;
    int channel;
    int rssi;
    int snr;
    char mac[6];
    int tx_rate;
    int rx_rate;
} user_wireless_info_t;

static user_wireless_info_t example_wireless_info = {
    .band = 0,
    .channel = 1,
    .rssi = -30,
    .snr = 30,
    .mac = {0x18, 0xFE, 0x34, 0x12, 0x34, 0x56},
    .tx_rate = 1,
    .rx_rate = 1,
};

static int get_wireless_info(user_wireless_info_t *wireless_info)
{
    if (wireless_info) {
        memcpy(wireless_info, &example_wireless_info, sizeof(user_wireless_info_t));
    }

    return 0;
}

static int post_property_wifi_status_once(app_context_t *sample_ctx)
{
    int                 ret     = -1;
    int                 i       = 0;
    static int          is_post = 0;
    char                val_buf[32];
    uint8_t             bssid[ETH_ALEN];
    user_wireless_info_t wireless_info;

    char *band    = NULL;
    int   channel = 0;
    int   rssi    = 0;
    int   snr     = 0;
    int   tx_rate = 0;
    int   rx_rate = 0;

    if (is_active(sample_ctx) && 0 == is_post) {
        get_wireless_info(&wireless_info);

#ifdef WIFI_PROVISION_ENABLED              
        HAL_Wifi_Get_Ap_Info(NULL, NULL, bssid);
#endif

        band    = wireless_info.band == 0 ? "2.4G" : "5G";
        channel = wireless_info.channel;
        rssi    = wireless_info.rssi;
        snr     = wireless_info.snr;
        tx_rate = wireless_info.tx_rate;
        rx_rate = wireless_info.rx_rate;

        linkkit_set_value(linkkit_method_set_property_value, sample_ctx->thing,
                          "WIFI_Band", band, NULL);
        linkkit_post_property(sample_ctx->thing, "WIFI_Band", post_property_cb);

        linkkit_set_value(linkkit_method_set_property_value, sample_ctx->thing,
                          "WIFI_Channel", &channel, NULL);
        linkkit_post_property(sample_ctx->thing, "WIFI_Channel",
                              post_property_cb);

        linkkit_set_value(linkkit_method_set_property_value, sample_ctx->thing,
                          "WiFI_RSSI", &rssi, NULL);
        linkkit_post_property(sample_ctx->thing, "WiFI_RSSI", post_property_cb);

        linkkit_set_value(linkkit_method_set_property_value, sample_ctx->thing,
                          "WiFI_SNR", &snr, NULL);
        linkkit_post_property(sample_ctx->thing, "WiFI_SNR", post_property_cb);

        memset(val_buf, 0, sizeof(val_buf));
        for (i = 0; i < ETH_ALEN; i++) {
            snprintf(val_buf + strlen(val_buf),
                     sizeof(val_buf) - strlen(val_buf), "%c:", bssid[i]);
        }
        if (strlen(val_buf) > 0 && val_buf[strlen(val_buf) - 1] == ':') {
            val_buf[strlen(val_buf) - 1] = '\0';
        }

        linkkit_set_value(linkkit_method_set_property_value, sample_ctx->thing,
                          "WIFI_AP_BSSID", val_buf, NULL);
        linkkit_post_property(sample_ctx->thing, "WIFI_AP_BSSID",
                              post_property_cb);

        linkkit_set_value(linkkit_method_set_property_value, sample_ctx->thing,
                          "WIFI_Tx_Rate", &tx_rate, NULL);
        linkkit_post_property(sample_ctx->thing, "WIFI_Tx_Rate",
                              post_property_cb);

        linkkit_set_value(linkkit_method_set_property_value, sample_ctx->thing,
                          "WIFI_Rx_Rate", &rx_rate, NULL);
        linkkit_post_property(sample_ctx->thing, "WIFI_Rx_Rate",
                              post_property_cb);

        is_post = 1;
        ret     = 0;
    }
    return ret;
}
#endif


static unsigned long long uptime_sec(void)
{
    static unsigned long long start_time = 0;

    if (start_time == 0) {
        start_time = HAL_UptimeMs();
    }

    return (HAL_UptimeMs() - start_time) / 1000;
}


int linkkit_example()
{
    app_context_t app_ctx = {0};
    int execution_time = 20;
    int exit = 0;
    unsigned long long now = 0;
    unsigned long long prev_sec = 0;
    int get_tsl_from_cloud = 0;                        /* the param of whether it is get tsl from cloud */
    int option = 1;
    linkkit_ops_t linkkit_ops = {
        .on_connect           = on_connect,            /* connect handler */
        .on_disconnect        = on_disconnect,         /* disconnect handler */
        .raw_data_arrived     = raw_data_arrived,      /* receive raw data handler */
        .thing_create         = thing_create,          /* thing created handler */
        .thing_enable         = thing_enable,          /* thing enabled handler */
        .thing_disable        = thing_disable,         /* thing disabled handler */
        .thing_call_service   = thing_call_service,    /* self-defined service handler */
        .thing_prop_changed   = thing_prop_changed,    /* property set handler */
        .linkit_data_arrived  = linkit_data_arrived,   /* transparent transmission data handler */
    };


    /* init app context */
    app_ctx.prop_powerswitch = PROPERTY_POWERSWITCH;
    app_ctx.prop_countdown = PROPERTY_COUNTDOWN;
    app_ctx.prop_countdown_pwrsw = PROPERTY_COUNTDOWN_POWERSWITCH;
    app_ctx.prop_countdown_timelf = PROPERTY_COUNTDOWN_TIMELEFT;
    app_ctx.prop_countdown_isrun = PROPERTY_COUNTDOWN_ISRUNNING;
    app_ctx.timerHandle = app_timer_open(&app_ctx);
    app_ctx.powerSwitch_Actual = 0;     /* assume initial status of PowerSwitch is 0 */

    EXAMPLE_TRACE("linkkit start");

    /*
     * set linkkit_opt_property_post_reply option to ture, so we can obtain set property reply through callback function.
     */
    linkkit_set_opt(linkkit_opt_property_post_reply, &option);

    /*
     * linkkit start
     * max_buffered_msg = 16, set the handle msg max numbers.
     *     if it is enough memory, this number can be set bigger.
     * if get_tsl_from_cloud = 0, it will use the default tsl [TSL_STRING]; if get_tsl_from_cloud =1, it will get tsl from cloud.
     */
    if (-1 == linkkit_start(16, get_tsl_from_cloud, linkkit_loglevel_debug, &linkkit_ops, linkkit_cloud_domain_shanghai,
                            &app_ctx)) {
        EXAMPLE_TRACE("linkkit start fail");
        return -1;
    }

    if (!get_tsl_from_cloud) {
        /*
         * if get_tsl_from_cloud = 0, set default tsl [TSL_STRING]
         * please modify TSL_STRING by the TSL's defined.
         */
        linkkit_set_tsl(TSL_STRING, strlen(TSL_STRING));
    }

    EXAMPLE_TRACE("linkkit enter loop");
    while (1) {
        /*
         * if linkkit is support Multi-thread, the linkkit_dispatch and linkkit_yield with callback by linkkit,
         * else it need user to call these function to received data.
         */
#if (CONFIG_SDK_THREAD_COST == 0)
        linkkit_yield(100);
        linkkit_dispatch();
#else
        HAL_SleepMs(100);
#endif /* CONFIG_SDK_THREAD_COST */
        now = uptime_sec();
        if (prev_sec == now) {
            continue;
        }

        /*
         * do user's process logical here.
         * example rule:
         *    about 10 seconds, assume trigger post wifi property event about every 10s.
         *    about 30 seconds, assume trigger post property event about every 30s.
         *
         * please follow user's rule to modify these code.
         */

#ifdef POST_WIFI_STATUS
        if (now % 10 == 0) {
            post_property_wifi_status_once(&app_ctx);
        }
#endif
        if (exit) {
            break;
        }

        /* after all, this is an sample, give a chance to return... */
        /* modify this value for this sample executaion time period */
        if (now > 60 * execution_time) {
            exit = 1;
        }

        prev_sec = now;
    }

    /* app end */
    linkkit_end();
    app_timer_stop(app_ctx.timerHandle);
    app_timer_close(app_ctx.timerHandle);

    return 0;
}

void set_iotx_info()
{
    HAL_SetProductKey(PRODUCT_KEY);
    HAL_SetProductSecret(PRODUCT_SECRET);
    HAL_SetDeviceName(DEVICE_NAME);
    HAL_SetDeviceSecret(DEVICE_SECRET);
}

int linkkit_main(void *paras)
{
#if !defined(WIFI_PROVISION_ENABLED) || !defined(BUILD_AOS)
    set_iotx_info();
#endif
    EXAMPLE_TRACE("start!\n");
    /*
     * linkkit timer countdown demo
     * please check document: https://help.aliyun.com/document_detail/73708.html?spm=a2c4g.11174283.6.560.zfcQ3y
     *         API introduce:  https://help.aliyun.com/document_detail/68687.html?spm=a2c4g.11186623.6.627.RJcT3F
     */
    linkkit_example();

    IOT_DumpMemoryStats(IOT_LOG_DEBUG);

    EXAMPLE_TRACE("out of example!\n");

    return 0;
}

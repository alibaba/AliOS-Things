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
#include "data/solo_tsl.data"

#define EVENT_ERROR_IDENTIFIER "Error"
#define EVENT_ERROR_OUTPUT_INFO_IDENTIFIER "ErrorCode"

// for demo only
#define PRODUCT_KEY     "a1X2bEnP82z"
#define PRODUCT_SECRET  "7jluWm1zql7bt8qK"
#define DEVICE_NAME     "test_06"
#define DEVICE_SECRET   "wQ1xOzFH3kLdjCTLfi8Xbw4otRz0lHoq"


#define EXAMPLE_TRACE(fmt, ...)                        \
    do {                                               \
        HAL_Printf("%s|%03d :: ", __func__, __LINE__); \
        HAL_Printf(fmt, ##__VA_ARGS__);                \
        HAL_Printf("%s", "\r\n");                      \
    } while (0)

typedef struct _sample_context {
    const void *thing;
    int         cloud_connected;
    int         local_connected;
    int         thing_enabled;
} sample_context_t;


/*
 * the callback of linkkit_post_property.
 * response_id is compare with the result of linkkit_post_property.
 *
 */
void post_property_cb(const void *thing_id, int response_id, int code,
                      const char *response_message, void *ctx)
{
    EXAMPLE_TRACE("thing@%p: response arrived:\nid:%d\tcode:%d\tmessage:%s\n",
                  thing_id, response_id, code,
                  response_message == NULL ? "NULL" : response_message);

    /* do user's post property callback process logical here. */

    /* ............................... */

    /* user's post property callback process logical complete */
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
    sample_context_t *sample_ctx = ctx;
#ifdef LOCAL_CONN_ENABLE
    if (cloud) {
        sample_ctx->cloud_connected = 1;
    } else {
        sample_ctx->local_connected = 1;
    }
    EXAMPLE_TRACE("%s is connected\n", cloud ? "cloud" : "local");
#else
    sample_ctx->cloud_connected = 1;
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
    sample_context_t *sample_ctx = ctx;

#ifdef LOCAL_CONN_ENABLE
    if (cloud) {
        sample_ctx->cloud_connected = 0;
    } else {
        sample_ctx->local_connected = 0;
    }
    EXAMPLE_TRACE("%s is disconnect\n", cloud ? "cloud" : "local");
#else
    sample_ctx->cloud_connected = 0;
    EXAMPLE_TRACE("%s is disconnect\n", "cloud");
#endif

    /* do user's disconnect process logical here. */

    /* ............................... */

    /* user's disconnect process logical complete */
    return 0;
}

// TODO:
/*
 * receive raw data handler
 */
static int raw_data_arrived(const void *thing_id, const void *data, int len,
                            void *ctx)
{
    char raw_data[128] = { 0 };

    EXAMPLE_TRACE("raw data arrived,len:%d\n", len);

    /* do user's raw data process logical here. */

    /* ............................... */

    /* user's raw data process logical complete */

    /* send result to cloud
     * please send your data via raw_data
     * example rule: just reply a string to check
     */
    snprintf(raw_data, sizeof(raw_data), "test down raw reply data %lld",
             (long long)HAL_UptimeMs());
    /* answer raw data handle result */
    linkkit_invoke_raw_service(thing_id, 0, raw_data, strlen(raw_data));

    return 0;
}


/* thing create succuss */
static int thing_create(const void *thing_id, void *ctx)
{
    sample_context_t *sample_ctx = ctx;

    EXAMPLE_TRACE("new thing@%p created.\n", thing_id);
    sample_ctx->thing = thing_id;

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
    sample_context_t *sample_ctx = ctx;

    sample_ctx->thing_enabled = 1;

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
    sample_context_t *sample_ctx = ctx;

    sample_ctx->thing_enabled = 0;

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
static int handle_service_custom(sample_context_t *_sample_ctx,
                                 const void       *thing,
                                 const char *service_identifier, int request_id,
                                 int rrpc)
#else
static int handle_service_custom(sample_context_t *_sample_ctx,
                                 const void       *thing,
                                 const char *service_identifier, int request_id)
#endif /* RRPC_ENABLED */
{
    char identifier[128] = { 0 };
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
    snprintf(identifier, sizeof(identifier), "%s.%s", service_identifier,
             "transparency");
    linkkit_get_value(linkkit_method_get_service_input_value, thing, identifier,
                      &transparency_value, NULL);

    EXAMPLE_TRACE("identifier: %s value is %d.\n", identifier,
                  transparency_value);

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
    snprintf(identifier, sizeof(identifier), "%s.%s", service_identifier,
             "Contrastratio");
    contrastratio_value = transparency_value + 1;
    linkkit_set_value(linkkit_method_set_service_output_value, thing,
                      identifier, &contrastratio_value, NULL);
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
static int thing_call_service(const void *thing_id, const char *service,
                              int request_id, int rrpc, void *ctx)
#else
static int thing_call_service(const void *thing_id, const char *service,
                              int request_id, void *ctx)
#endif /* RRPC_ENABLED */
{
    sample_context_t *sample_ctx = ctx;

    EXAMPLE_TRACE("service(%s) requested, id: thing@%p, request id:%d\n",
                  service, thing_id, request_id);

    /* please follow TSL modify the idendifier --- Custom */
    if (strcmp(service, "Custom") == 0) {
#ifdef RRPC_ENABLED
        handle_service_custom(sample_ctx, thing_id, service, request_id, rrpc);
#else
        handle_service_custom(sample_ctx, thing_id, service, request_id);
#endif /* RRPC_ENABLED */
    }

    return 0;
}


/*
 * the handler of property changed
 * alink method: thing.service.property.set
 */
static int thing_prop_changed(const void *thing_id, const char *property,
                              void *ctx)
{
    char *value_str        = NULL;
    char  property_buf[64] = { 0 };
    int   response_id      = -1;

    /* do user's property changed process logical here. */

    /* ............................... */

    /* user's property changed process logical complete */


    /*
     * example:
     *     property identifier:
     *               IndoorTemperature
     *               TemperatureModelStatus
     *               CurrentTemperature
     *
     * please follow TSL modify this property identifier
     */

    /* if the proprety id is %s.%s, please follow this code */
    /* get new property value */
    if (strstr(property, "HSVColor") != 0) {
        double hue, saturation, value;

        /* generate property identifier HSVColor.Hue */
        snprintf(property_buf, sizeof(property_buf), "%s.%s", property, "Hue");
        /* get value by linkkit_get_value */
        linkkit_get_value(linkkit_method_get_property_value, thing_id,
                          property_buf, &hue, &value_str);
        if (value_str) {
            HAL_Free(value_str);
            value_str = NULL;
        }

        /* generate property identifier HSVColor.Saturation */
        snprintf(property_buf, sizeof(property_buf), "%s.%s", property,
                 "Saturation");
        /* get value by linkkit_get_value */
        linkkit_get_value(linkkit_method_get_property_value, thing_id,
                          property_buf, &saturation, &value_str);
        if (value_str) {
            HAL_Free(value_str);
            value_str = NULL;
        }

        /* generate property identifier HSVColor.Value */
        snprintf(property_buf, sizeof(property_buf), "%s.%s", property,
                 "Value");
        /* get value by linkkit_get_value */
        linkkit_get_value(linkkit_method_get_property_value, thing_id,
                          property_buf, &value, &value_str);
        if (value_str) {
            HAL_Free(value_str);
            value_str = NULL;
        }

        EXAMPLE_TRACE("property(%s), Hue:%f, Saturation:%f, Value:%f\n",
                      property, hue, saturation, value);
    } else if (strstr(property, "HSLColor") != 0) {
        double hue, saturation, lightness;

        /* generate property identifier HSLColor.Hue */
        snprintf(property_buf, sizeof(property_buf), "%s.%s", property, "Hue");
        /* get value by linkkit_get_value */
        linkkit_get_value(linkkit_method_get_property_value, thing_id,
                          property_buf, &hue, &value_str);
        if (value_str) {
            HAL_Free(value_str);
            value_str = NULL;
        }

        /* generate property identifier HSLColor.Saturation */
        snprintf(property_buf, sizeof(property_buf), "%s.%s", property,
                 "Saturation");
        /* get value by linkkit_get_value */
        linkkit_get_value(linkkit_method_get_property_value, thing_id,
                          property_buf, &saturation, &value_str);
        if (value_str) {
            HAL_Free(value_str);
            value_str = NULL;
        }

        /* generate property identifier HSLColor.Lightness */
        snprintf(property_buf, sizeof(property_buf), "%s.%s", property,
                 "Lightness");
        /* get value by linkkit_get_value */
        linkkit_get_value(linkkit_method_get_property_value, thing_id,
                          property_buf, &lightness, &value_str);
        if (value_str) {
            HAL_Free(value_str);
            value_str = NULL;
        }

        EXAMPLE_TRACE("property(%s), Hue:%f, Saturation:%f, Lightness:%f\n",
                      property, hue, saturation, lightness);
    } else if (strstr(property, "RGBColor") != 0) {
        int red, green, blue;

        /* generate property identifier RGBColor.Red */
        snprintf(property_buf, sizeof(property_buf), "%s.%s", property, "Red");
        /* get value by linkkit_get_value */
        linkkit_get_value(linkkit_method_get_property_value, thing_id,
                          property_buf, &red, &value_str);
        if (value_str) {
            HAL_Free(value_str);
            value_str = NULL;
        }

        /* generate property identifier RGBColor.Green */
        snprintf(property_buf, sizeof(property_buf), "%s.%s", property,
                 "Green");
        /* get value by linkkit_get_value */
        linkkit_get_value(linkkit_method_get_property_value, thing_id,
                          property_buf, &green, &value_str);
        if (value_str) {
            HAL_Free(value_str);
            value_str = NULL;
        }

        /* generate property identifier RGBColor.Blue */
        snprintf(property_buf, sizeof(property_buf), "%s.%s", property, "Blue");
        /* get value by linkkit_get_value */
        linkkit_get_value(linkkit_method_get_property_value, thing_id,
                          property_buf, &blue, &value_str);
        if (value_str) {
            HAL_Free(value_str);
            value_str = NULL;
        }

        EXAMPLE_TRACE("property(%s), Red:%d, Green:%d, Blue:%d\n", property,
                      red, green, blue);
    }

    /* post property
     * result is response_id; if response_id = -1, it is fail, else it is
     * success. response_id by be compare in post_property_cb.
     */
    response_id = linkkit_post_property(thing_id, property, post_property_cb);

    EXAMPLE_TRACE("post property(%s) response id: %d\n", property, response_id);

    return 0;
}


/* there is some data transparent transmission by linkkit */
static int linkit_data_arrived(const void *thing_id, const void *params,
                               int len, void *ctx)
{
    EXAMPLE_TRACE("thing@%p: masterdev_linkkit_data(%d byte): %s\n", thing_id,
                  len, (const char *)params);

    /* do user's data arrived process logical here. */

    /* ............................... */

    /* user's data arrived process logical complete */
    return 0;
}

static int is_active(sample_context_t *sample_ctx)
{
#ifdef LOCAL_CONN_ENABLE
    return (sample_ctx->cloud_connected /* && sample_ctx->thing_enabled*/) ||
           (sample_ctx->local_connected /* && sample_ctx->thing_enabled*/);
#else
    return sample_ctx->cloud_connected /* && sample_ctx->thing_enabled*/;
#endif
}

#ifdef POST_WIFI_STATUS

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

static int post_property_wifi_status_once(sample_context_t *sample_ctx)
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


int post_all_prop(sample_context_t *sample)
{
    /* demo for post all property */
    return linkkit_post_property(sample->thing, NULL, post_property_cb);
}


int trigger_event(sample_context_t *sample)
{
    char event_output_identifier[64];
    snprintf(event_output_identifier, sizeof(event_output_identifier), "%s.%s",
             EVENT_ERROR_IDENTIFIER, EVENT_ERROR_OUTPUT_INFO_IDENTIFIER);

    int errorCode = 0;
    linkkit_set_value(linkkit_method_set_event_output_value, sample->thing,
                      event_output_identifier, &errorCode, NULL);

    return linkkit_trigger_event(sample->thing, EVENT_ERROR_IDENTIFIER,
                                 post_property_cb);
}

#ifdef EXTENDED_INFO_ENABLED
int trigger_deviceinfo(sample_context_t *sample)
{
    /* please modify the parameter */
    return linkkit_trigger_extended_info_operate(
                       sample->thing, "[{device_info : 21}]",
                       linkkit_extended_info_operate_update);
}
#endif

void ntp_time_reply(const char *offset_time)
{
    EXAMPLE_TRACE("ntp time: %s\n", offset_time);
}


int linkkit_example()
{
    sample_context_t    sample_ctx = { 0 };
    int                 exit = 0;
    int                 cnt = 0;
    unsigned long long  now = 0;
    unsigned long long  prev_sec = 0;
    int                 get_tsl_from_cloud = 0; /* the param of whether it is get tsl from cloud */

    linkkit_ops_t linkkit_ops = {
        .on_connect       = on_connect,       /* connect handler */
        .on_disconnect    = on_disconnect,    /* disconnect handler */
        .raw_data_arrived = raw_data_arrived, /* receive raw data handler */
        .thing_create     = thing_create,     /* thing created handler */
        .thing_enable     = thing_enable,     /* thing enabled handler */
        .thing_disable    = thing_disable,    /* thing disabled handler */
        .thing_call_service = thing_call_service, /* self-defined service handler */
        .thing_prop_changed = thing_prop_changed, /* property set handler */
        .linkit_data_arrived = linkit_data_arrived, /* transparent transmission data handler */
    };

    EXAMPLE_TRACE("linkkit start");

    /*
     * linkkit start
     * max_buffered_msg = 16, set the handle msg max numbers.
     *     if it is enough memory, this number can be set bigger.
     * if get_tsl_from_cloud = 0, it will use the default tsl [TSL_STRING]; if
     * get_tsl_from_cloud =1, it will get tsl from cloud.
     */
    if (-1 == linkkit_start(16, get_tsl_from_cloud, linkkit_loglevel_debug,
                            &linkkit_ops, linkkit_cloud_domain_shanghai,
                            &sample_ctx)) {
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
    while (!linkkit_is_try_leave()) {
        /*
         * if linkkit is support Multi-thread, the linkkit_dispatch and
         * linkkit_yield with callback by linkkit, else it need user to call
         * these function to received data.
         */
#if (CONFIG_SDK_THREAD_COST == 0)
        linkkit_yield(100);
        if (++cnt % 10 == 0) {
            EXAMPLE_TRACE(".");
            cnt = 0;
        }
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
         *    about 10 seconds, assume trigger post wifi property event about
         * every 10s. about 30 seconds, assume trigger post property event about
         * every 30s.
         *
         * please follow user's rule to modify these code.
         */

        /* Manually Trigger Config OTA */
        /* if (now % 10 == 0) {
            linkkit_invoke_cota_get_config("product","file","",NULL);
        } */

#ifdef POST_WIFI_STATUS
        if (now % 10 == 0) {
            linkkit_ntp_time_request(ntp_time_reply);
            post_property_wifi_status_once(&sample_ctx);
        }
#endif
        if (now % 30 == 0 && is_active(&sample_ctx)) {
            linkkit_ntp_time_request(ntp_time_reply);
            post_all_prop(&sample_ctx);
        }

        if (now % 45 == 0 && is_active(&sample_ctx)) {
            linkkit_ntp_time_request(ntp_time_reply);
            trigger_event(&sample_ctx);
        }

#ifdef EXTENDED_INFO_ENABLED
        if (now % 50 == 0 && is_active(&sample_ctx)) {
            linkkit_ntp_time_request(ntp_time_reply);
            trigger_deviceinfo(&sample_ctx);
        }
#endif

        if (exit) {
            break;
        }

        /* after all, this is an sample, give a chance to return... */
        /* modify this value for this sample executaion time period */
        // if (now > 60 * execution_time) {
        //     exit = 1;
        // }

        prev_sec = now;
    }

    /* linkkit end */
    linkkit_end();
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
     * linkkit demo
     * please check document: https://help.aliyun.com/document_detail/73708.html
     *         API introduce: https://help.aliyun.com/document_detail/68687.html
     */
    linkkit_example();

    IOT_DumpMemoryStats(IOT_LOG_DEBUG);

    EXAMPLE_TRACE("out of sample!\n");

    return 0;
}

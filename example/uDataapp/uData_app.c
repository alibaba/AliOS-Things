/** USER NOTIFICATION
 *  this sample code is only used for evaluation or test of the iLop project.
 *  Users should modify this sample code freely according to the product/device TSL, like
 *  property/event/service identifiers, and the item value type(type, length, etc...).
 *  Create user's own execution logic for specific products.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>
#include <unistd.h>
#include <aos/aos.h>

#include "linkkit_export.h"

#include "iot_import.h"
#include "aos/uData.h"


/*
 * example for product "灯-Demo"
 */


#define LINKKIT_PRINTF(...)  \
    do {                                                     \
        printf("\e[0;32m%s@line%d:\t", __FUNCTION__, __LINE__);  \
        printf(__VA_ARGS__);                                 \
        printf("\e[0m");                                   \
    } while (0)

/* identifier of property/service/event, users should modify this macros according to your own product TSL. */
#define EVENT_ERROR_IDENTIFIER                 "Error"
#define EVENT_ERROR_OUTPUT_INFO_IDENTIFIER     "ErrorCode"
#define EVENT_CUSTOM_IDENTIFIER                "Custom"
extern int service_dtc_publish_cycle_set(udata_type_e type, uint32_t cycle);

extern int service_dtc_name_set(udata_type_e type, char* src[], int num);

/* specify ota buffer size for ota service, ota service will use this buffer for bin download. */
static void ota_init();


/* PLEASE set RIGHT tsl string according to your product. */
extern const char TSL_STRING[];


void  *g_thing_id = NULL;

int dtc_set_value(const void *thing_id, const char *identifier,const void* value,  const char *value_str)
{
    int ret = linkkit_set_value(linkkit_method_set_property_value, thing_id, identifier, value, value_str);
    if(0 != ret){
        return ret;
    }
    return 0;
}

int dtc_post_property(const void *thing_id, const char *identifier)
{
    int ret = linkkit_post_property(thing_id, identifier);
    if( ret < 0 ){
        return ret;
    }
    return 0;
}


/* user sample context struct. */
typedef struct _sample_context {
    const void *thing;

    int cloud_connected;
    int thing_enabled;

    int service_custom_input_transparency;
    int service_custom_output_contrastratio;
#ifdef SERVICE_OTA_ENABLED
    char ota_buffer[OTA_BUFFER_SIZE];
#endif /* SERVICE_OTA_ENABLED */
} sample_context_t;

#ifdef SUBDEV_ENABLE
#define DEMO_SUBDEV1_PRODUCT_KEY       "a1jwrlrp3f5"
#define DEMO_SUBDEV1_DEVICE_NAME       "olin_subdev_test"
#define DEMO_SUBDEV1_DEVICE_SECRET     "7d99rEma1ttOwulwuzpRyAuv4HLKjF36"
extern const char SUBDEV1_TSL_STRING[];

typedef struct {

} subdev_sample_context_t;

subdev_sample_context_t g_subdev_sample_context;

#endif



sample_context_t g_sample_context;

#ifdef LOCAL_CONN_ENABLE
static int on_connect(void *ctx, int cloud)
#else
static int on_connect(void *ctx)
#endif
{
    sample_context_t *sample = ctx;

    sample->cloud_connected = 1;

    ota_init();

#ifdef LOCAL_CONN_ENABLE
    LINKKIT_PRINTF("%s is connected\n", cloud ? "cloud" : "local");
#else
    LINKKIT_PRINTF("%s is connected\n", "cloud");
#endif

    aos_post_event(EV_YUNIO, CODE_YUNIO_ON_CONNECTED, 0);

    return 0;
}

#ifdef LOCAL_CONN_ENABLE
static int on_disconnect(void *ctx, int cloud)
#else
static int on_disconnect(void *ctx)
#endif
{
    sample_context_t *sample = ctx;

    sample->cloud_connected = 0;

#ifdef LOCAL_CONN_ENABLE
    LINKKIT_PRINTF("%s is disconnect\n", cloud ? "cloud" : "local");
#else
    LINKKIT_PRINTF("%s is disconnect\n", "cloud");
#endif

    aos_post_event(EV_YUNIO, CODE_YUNIO_ON_DISCONNECTED, 0);

    return 0;
}

static int raw_data_arrived(const void *thing_id, const void *data, int len, void *ctx)
{
    char raw_data[128] = {0};

    LINKKIT_PRINTF("raw data arrived,len:%d\n", len);

    /* do user's raw data process logical here. */

    /* ............................... */

    /* user's raw data process logical complete */

    snprintf(raw_data, sizeof(raw_data), "test down raw reply data %lld", (long long)HAL_UptimeMs());

    linkkit_invoke_raw_service(thing_id, 0, raw_data, strlen(raw_data));

    return 0;
}

static int thing_create(const void *thing_id, void *ctx)
{
    sample_context_t *sample = ctx;

    LINKKIT_PRINTF("new thing@%p created.\n", thing_id);

    sample->thing = thing_id;
#ifdef SUBDEV_ENABLE
    linkkit_add_subdev(sample->thing, DEMO_SUBDEV1_PRODUCT_KEY, DEMO_SUBDEV1_DEVICE_NAME, DEMO_SUBDEV1_DEVICE_SECRET,
                       SUBDEV1_TSL_STRING, strlen(SUBDEV1_TSL_STRING));
#endif
    return 0;
}

static int thing_enable(const void *thing_id, void *ctx)
{
    sample_context_t *sample = ctx;

    sample->thing_enabled = 1;

    return 0;
}

static int thing_disable(const void *thing, void *ctx)
{
    sample_context_t *sample = ctx;

    sample->thing_enabled = 0;

    return 0;
}
#ifdef RRPC_ENABLED
static int handle_service_custom(sample_context_t *sample, const void *thing, const char *service_identifier,
                                 int request_id, int rrpc)
#else
static int handle_service_custom(sample_context_t *sample, const void *thing, const char *service_identifier,
                                 int request_id)
#endif /* RRPC_ENABLED */
{
    char identifier[128] = {0};

    /*
     * get iutput value.
     */
    snprintf(identifier, sizeof(identifier), "%s.%s", service_identifier, "transparency");
    linkkit_get_value(linkkit_method_get_service_input_value, thing, identifier, &sample->service_custom_input_transparency,
                      NULL);

    /*
     * set output value according to user's process result.
     */

    snprintf(identifier, sizeof(identifier), "%s.%s", service_identifier, "Contrastratio");

    sample->service_custom_output_contrastratio = sample->service_custom_input_transparency >= 0 ?
                                                  sample->service_custom_input_transparency : sample->service_custom_input_transparency * -1;

    linkkit_set_value(linkkit_method_set_service_output_value, thing, identifier,
                      &sample->service_custom_output_contrastratio, NULL);
#ifdef RRPC_ENABLED
    linkkit_answer_service(thing, service_identifier, request_id, 200, rrpc);
#else
    linkkit_answer_service(thing, service_identifier, request_id, 200);
#endif /* RRPC_ENABLED */

    return 0;
}
#ifdef RRPC_ENABLED
static int thing_call_service(const void *thing_id, const char *service, int request_id, int rrpc, void *ctx)
#else
static int thing_call_service(const void *thing_id, const char *service, int request_id, void *ctx)
#endif /* RRPC_ENABLED */
{
    sample_context_t *sample = ctx;

    LINKKIT_PRINTF("service(%s) requested, id: thing@%p, request id:%d\n",
                   service, thing_id, request_id);

    if (strcmp(service, "Custom") == 0) {
#ifdef RRPC_ENABLED
        handle_service_custom(sample, thing_id, service, request_id, rrpc);
#else
        handle_service_custom(sample, thing_id, service, request_id);
#endif /* RRPC_ENABLED */
    }

    return 0;
}

static int thing_prop_changed(const void *thing_id, const char *property, void *ctx)
{
    char *value_str = NULL;
    char property_buf[64] = {0};



    return 0;
}

int on_response_arrived(const void *thing_id, int respons_id, int code, char *message, void *ctx)
{
    LINKKIT_PRINTF("response arrived:\nid:%d\tcode:%d\tmessage:%s\n", respons_id, code, message == NULL ? "NULL" : message);

    return 0;
}

int masterdev_linkit_data_arrived(const void *thing_id, const void *data, int len, void *ctx)
{
    LINKKIT_PRINTF("#### masterdev_linkkit_data(%d byte): %s####\n", len, (char *)data);
    return 0;
}

static linkkit_ops_t alinkops = {
    .on_connect         = on_connect,
    .on_disconnect      = on_disconnect,
    .raw_data_arrived   = raw_data_arrived,
    .thing_create       = thing_create,
    .thing_enable       = thing_enable,
    .thing_disable      = thing_disable,
    .thing_call_service = thing_call_service,
    .thing_prop_changed = thing_prop_changed,
    .response_arrived   = on_response_arrived,
    .linkit_data_arrived = masterdev_linkit_data_arrived,
};

#ifdef SUBDEV_ENABLE
int subdev_topo_add_success(const void *sub_thing_id, void *ctx)
{
    LINKKIT_PRINTF("#### subthing(@%p) topo add success####\n", sub_thing_id);
    return 0;
}
int subdev_topo_add_fail(const void *sub_thing_id, int code, const char *message, void *ctx)
{
    LINKKIT_PRINTF("#### subthing(@%p) code(%d) message(%s) topo add fail####\n", sub_thing_id, code,
                   message ? message : "NULL");
    return 0;
}

int subdev_login_success(const void *sub_thing_id, void *ctx)
{
    LINKKIT_PRINTF("#### subthing(@%p) login success####\n", sub_thing_id);
    return 0;
}

int subdev_login_fail(const void *sub_thing_id, int code, const char *message, void *ctx)
{
    LINKKIT_PRINTF("#### subthing(@%p) code(%d) message(%s) topo add fail####\n", sub_thing_id, code,
                   message ? message : "NULL");
    return 0;
}

int subdev_raw_data_arrived(const void *sub_thing_id, const void *data, int len, void *ctx)
{

    LINKKIT_PRINTF("subdev(@%p) raw data arrived,len:%d\n", sub_thing_id, len);

    /* do user's raw data process logical here. */

    /* ............................... */

    /* user's raw data process logical complete */
    return 0;
}

int subdev_thing_create(const void *sub_thing_id, void *ctx)
{
    LINKKIT_PRINTF("new thing@%p created.\n", sub_thing_id);
    return 0;
}

int subdev_thing_enable(const void *sub_thing_id, void *ctx)
{
    LINKKIT_PRINTF("thing@%p has been enabled.\n", sub_thing_id);
    return 0;
}

int subdev_thing_disable(const void *sub_thing_id, void *ctx)
{
    LINKKIT_PRINTF("thing@%p has been disabled.\n", sub_thing_id);
    return 0;
}

int subdev_linkit_data_arrived(const void *sub_thing_id, const void *data, int len, void *ctx)
{
    LINKKIT_PRINTF("#### subdev_linkkit_data(%d byte): %s####\n", len, (char *)data);
    return 0;
}

static linkkit_subdev_ops_t subdev_ops = {
    .topo_add_success = subdev_topo_add_success,
    .topo_add_fail = subdev_topo_add_fail,
    .login_success = subdev_login_success,
    .login_fail = subdev_login_fail,
    .raw_data_arrived = subdev_raw_data_arrived,
    .thing_create = subdev_thing_create,
    .thing_enable = subdev_thing_enable,
    .thing_disable = subdev_thing_disable,
    .linkit_data_arrived = subdev_linkit_data_arrived,
};
#endif

static unsigned long long uptime_sec(void)
{
    static unsigned long long start_time = 0;

    if (start_time == 0) {
        start_time = HAL_UptimeMs();
    }

    return (HAL_UptimeMs() - start_time) / 1000;
}

static int post_event_error(sample_context_t *sample)
{
    char event_output_identifier[64];
    snprintf(event_output_identifier, sizeof(event_output_identifier), "%s.%s", EVENT_ERROR_IDENTIFIER,
             EVENT_ERROR_OUTPUT_INFO_IDENTIFIER);

    int errorCode = 0;
    linkkit_set_value(linkkit_method_set_event_output_value,
                      sample->thing,
                      event_output_identifier,
                      &errorCode, NULL);

    return linkkit_trigger_event(sample->thing, EVENT_ERROR_IDENTIFIER);
}

static int is_active(sample_context_t *sample)
{
    return sample->cloud_connected && sample->thing_enabled;
}

int g_dtc_reg_flag = 0;

void linkkit_action(void *params)
{
    static unsigned long long now = 0;

    static int now_size = 0;
    static int last_size = 0;

    sample_context_t *sample_ctx = params;

    linkkit_dispatch();

    if (0 == g_dtc_reg_flag) {
        service_dtc_register(sample_ctx->thing, dtc_set_value,dtc_post_property);
        g_dtc_reg_flag = 1;
    }
    now += 1;

#if 1
    /* about 60 seconds, assume trigger event about every 60s. */
    if (now % 600 == 0 && is_active(sample_ctx)) {
        int id_send = 0;
        int ret;
        ret = post_event_error(sample_ctx);
#if 0
        LINKKIT_PRINTF("####%s, ret=%d####\n", "post_event_error", ret);
        ret = linkkit_post_property(sample_ctx->thing, NULL);
        LINKKIT_PRINTF("####%s, ret=%d####\n", "linkkit_post_property", ret);
#endif
        if (ret > 0) {
            id_send = ret;
            LINKKIT_PRINTF("send id:%d\n", id_send);
        }
    }
#endif

#if 0
    now_size = system_get_free_heap_size();
    if (now_size != last_size) {
        last_size = now_size;
        if ((now_size - last_size) > 256 ||  (last_size - now_size) > 256) {
            LINKKIT_PRINTF("[heap check task] free heap size:%d\n", now_size);
        }
    } else if (now % 600 == 0) {
        LINKKIT_PRINTF("[heap check task] free heap size:%d Bytes(now time:%d)\n", now_size, now);
    }
#endif

    aos_post_delayed_action(100, linkkit_action, sample_ctx);
}


int linkkit_main()
{
    sample_context_t *sample_ctx = &g_sample_context;
    int execution_time = 0;
    int get_tsl_from_cloud = 0;

    execution_time = execution_time < 1 ? 1 : execution_time;
    LINKKIT_PRINTF("sample execution time: %d minutes\n", execution_time);
    LINKKIT_PRINTF("%s tsl from cloud\n", get_tsl_from_cloud == 0 ? "Not get" : "get");

    memset(sample_ctx, 0, sizeof(sample_context_t));
    sample_ctx->thing_enabled = 1;

    linkkit_start(16, get_tsl_from_cloud, linkkit_loglevel_debug, &alinkops, linkkit_cloud_domain_sh, sample_ctx);
    if (!get_tsl_from_cloud) {
        linkkit_set_tsl(TSL_STRING, strlen(TSL_STRING));
    }
#ifdef SUBDEV_ENABLE
    linkkit_subdev_init(&subdev_ops, &g_subdev_sample_context);
#endif
    aos_post_delayed_action(100, linkkit_action, sample_ctx);

    return 0;
}

int linkkit_app(void)
{
    linkkit_main();

    return 0;
}

static void ota_init()
{
    static int init = 0;
    if (init) {
        return;
    }
    init = 1;

    aos_post_event(EV_SYS, CODE_SYS_ON_START_FOTA, 0);
}


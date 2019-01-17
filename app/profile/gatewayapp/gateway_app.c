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
#include "aos/kernel.h"
#include <time.h>

#include "iot_export.h"

#include "iot_import.h"
#include "udata/udata.h"
#include "aos/hal/gpio.h"
#include "aos/hal/uart.h"


#ifdef GW_LINKEDGE_WS
#include "linkedge_ws.h"
#include <cJson.h>
static  cJSON * jRoot = NULL;
#endif

#include <linkkit_export.h>
#include "device.h"

/*
 * example for product gateway
 */


#define GW_PRINTF(...)  \
    do {                                                     \
        printf(__VA_ARGS__);                                 \
    } while (0)

/* identifier of property/service/event, users should modify this macros according to your own product TSL. */
#define EVENT_ERROR_IDENTIFIER                 "Error"
#define EVENT_ERROR_OUTPUT_INFO_IDENTIFIER     "ErrorCode"
#define EVENT_CUSTOM_IDENTIFIER                "Custom"
extern int service_dtc_publish_cycle_set(udata_type_e type, uint32_t cycle);

extern int service_dtc_name_set(udata_type_e type, char* src[], int num);

extern gpio_dev_t brd_gpio_table[];

/* PLEASE set RIGHT tsl string according to your product. */
extern const char TSL_STRING[];


void  *g_thing_id = NULL;
int cloud_started = 0;

void post_property_cb(const void *thing_id, int response_id, int code,
                      const char *response_message, void *ctx)
{
}

int dtc_set_value(const void *thing_id, const char *identifier,const void* value,  const char *value_str)
{
#ifdef GW_LINKEDGE_WS
    const char * id = strchr(identifier, '.');
    if(!id){
        return  websocket_report_prop(identifier, value_str);
    }

    if(!jRoot)
        jRoot = cJSON_CreateObject();
    if(!jRoot) return -1;
    cJSON_AddNumberToObject(jRoot, id+1, atof(value_str));
    hal_gpio_output_toggle(&brd_gpio_table[1]);
    return 0;
#else
    int ret = linkkit_set_value(linkkit_method_set_property_value, thing_id, identifier, value, value_str);
    if(0 != ret){
        return ret;
    }
    hal_gpio_output_toggle(&brd_gpio_table[1]);
    return 0;
#endif
}

int dtc_post_property(const void *thing_id, const char *identifier)
{
#ifdef GW_LINKEDGE_WS
    if(!jRoot) return 0;

    char * v = cJSON_PrintUnformatted(jRoot);
    cJSON_Delete(jRoot);
    jRoot = NULL;
    if(!v) return -1;
    int ret = websocket_report_prop(identifier, v);
    free(v);
    return ret;
#else
    int ret = linkkit_post_property(thing_id, identifier, post_property_cb);
    if( ret < 0 ){
        return ret;
    }
    return 0;
#endif
}


/* user sample context struct. */
typedef struct _sample_context {
    const void *thing;

    int cloud_connected;
    int thing_enabled;

    int service_custom_input_transparency;
    int service_custom_output_contrastratio;
} sample_context_t;



sample_context_t g_sample_context;

#ifdef LOCAL_CONN_ENABLE
static int on_connect(void *ctx, int cloud)
#else
static int on_connect(void *ctx)
#endif
{
    sample_context_t *sample = ctx;

    sample->cloud_connected = 1;

#ifdef LOCAL_CONN_ENABLE
    GW_PRINTF("%s is connected\n", cloud ? "cloud" : "local");
#else
    GW_PRINTF("%s is connected\n", "cloud");
#endif

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
    GW_PRINTF("%s is disconnect\n", cloud ? "cloud" : "local");
#else
    GW_PRINTF("%s is disconnect\n", "cloud");
#endif

    return 0;
}

static int raw_data_arrived(const void *thing_id, const void *data, int len, void *ctx)
{
    char raw_data[128] = {0};

    GW_PRINTF("raw data arrived,len:%d\n", len);

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

    GW_PRINTF("new thing@%p created.\n", thing_id);

    sample->thing = thing_id;

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

    GW_PRINTF("service(%s) requested, id: thing@%p, request id:%d\n",
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
    GW_PRINTF("response arrived:\nid:%d\tcode:%d\tmessage:%s\n", respons_id, code, message == NULL ? "NULL" : message);

    return 0;
}

int masterdev_linkit_data_arrived(const void *thing_id, const void *data, int len, void *ctx)
{
    GW_PRINTF("#### masterdev_linkkit_data(%d byte): %s####\n", len, (char *)data);
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
    .linkit_data_arrived = masterdev_linkit_data_arrived,
};

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

    return linkkit_trigger_event(sample->thing, EVENT_ERROR_IDENTIFIER, post_property_cb);
}

static int is_active(sample_context_t *sample)
{
    return sample->cloud_connected && sample->thing_enabled;
}

int g_dtc_reg_flag = 0;

void ptgw_action(void *params)
{
    static unsigned long long now = 0;

    static int now_size = 0;
    static int last_size = 0;

    sample_context_t *sample_ctx = params;

#ifdef GW_LINKEDGE_WS
    websocket_loop();
    if ((0 == g_dtc_reg_flag)&&!websocket_local_offline()) {
        int ret = service_dtc_register(sample_ctx, dtc_set_value,dtc_post_property);
		if (!ret)
		{
			g_dtc_reg_flag = 1;
            hal_gpio_output_high(&brd_gpio_table[0]);
		}
				
    }
#else
    if(cloud_started){
        
#if (CONFIG_SDK_THREAD_COST == 0)
        linkkit_yield(100);
        linkkit_dispatch();
#endif
        if (0 == g_dtc_reg_flag) {
            int ret = service_dtc_register(sample_ctx->thing, dtc_set_value,dtc_post_property);
			if (!ret)
				{
					g_dtc_reg_flag = 1;
					hal_gpio_output_high(&brd_gpio_table[0]);
				}
				
        }
        now += 1;
    }
#endif
    aos_post_delayed_action(50, ptgw_action, sample_ctx);
}


int ptgw_main()
{
    sample_context_t *sample_ctx = &g_sample_context;
    aos_post_delayed_action(100, ptgw_action, sample_ctx);

    return 0;
}



#ifdef PT_SCANNER

/*Scanner Device defines */
static aos_task_t scan_task;
static uart_dev_t uart4;
#define BUF_MAX 512
static uint8_t device_buf[BUF_MAX];
static uint32_t dev_buf_len = 0;
/*Event parameter define*/
#define FMT_EVENT_SCAN          "\"DeviceNumber\":\"%s\",\"Timestamp\":\"%d\",\"Contain\":\"%s\""


int dev_post_scan_event(uint8_t * msg){

    if(!cloud_started || !g_sample_context.thing) return -1;

    char timest[128];
    snprintf(timest, 128, "%d", time(0));

    linkkit_set_value(linkkit_method_set_event_output_value,
                      g_sample_context.thing,
                      "scanevent.DeviceNumber",
                      NULL, "00001");
    linkkit_set_value(linkkit_method_set_event_output_value,
                      g_sample_context.thing,
                      "scanevent.Timestamp",
                      NULL, timest);
    linkkit_set_value(linkkit_method_set_event_output_value,
                      g_sample_context.thing,
                      "scanevent.Contain",
                      NULL, msg);

    return linkkit_trigger_event(g_sample_context.thing, "scanevent", post_property_cb);
}

static void init_scanner(){

    uart4.port = 3;
    uart4.config.baud_rate = 9600;
    uart4.config.data_width = DATA_WIDTH_8BIT;
    uart4.config.parity = NO_PARITY;
    uart4.config.stop_bits = STOP_BITS_1;
    uart4.config.flow_control = FLOW_CONTROL_DISABLED;
    uart4.config.mode = MODE_TX_RX;

    int32_t ret = hal_uart_init(&uart4);
    GW_PRINTF("Init UART %d\n", ret);
}

static void scanner_srv(void * p){
    while(1){
    #ifdef GW_LINKEDGE_WS
        if(websocket_local_offline()){
             aos_msleep(1000);
             continue;
        }
    #endif

        int32_t ret = hal_uart_recv_II(&uart4, device_buf, BUF_MAX, &dev_buf_len, 50);
        if(!ret){
            GW_PRINTF("Scanner get data %u\n", dev_buf_len);
            device_buf[dev_buf_len] = 0;

    #ifdef GW_LINKEDGE_WS
            for(int i=0; i<dev_buf_len; i++){

                //Prepare for JSON '"' and '\'
                if(device_buf[i]==0x5c || device_buf[i]==0x22){
                    memmove(device_buf+i+2, device_buf+i+1, dev_buf_len-i-1);
                    device_buf[i+1] = device_buf[i];
                    device_buf[i] = 0x5c;
                    dev_buf_len++;
                    i++;
                }
            }

            static uint8_t param[BUF_MAX+128];
                
            int elen = snprintf(param, BUF_MAX+128, FMT_EVENT_SCAN, "00001", time(0), device_buf);
            
            while(websocket_report_event("scanevent", param)){
                GW_PRINTF("Retry report event %d\n");
                aos_msleep(50);
            }
    #else
            
            dev_post_scan_event(device_buf);   
    #endif
        }
    }
}

#endif

#ifdef GW_LINKEDGE_WS
static aos_task_t ws_task;
static void ws_srv(void * p){
    while(1){
        websocket_loop();
    }
}
#endif

int gw_app(void)
{
    GW_PRINTF("Gateway cloud/local connection starting.\n");
#ifdef GW_LINKEDGE_WS
    websocket_local_link();
    //aos_task_new_ext(&ws_task, "linkedge", ws_srv, NULL, 4096, 28);
#else
    HAL_SetProductKey(PRODUCT_KEY);
    HAL_SetProductSecret(PRODUCT_SECRET);
    HAL_SetDeviceName(DEVICE_NAME);
    HAL_SetDeviceSecret(DEVICE_SECRET);


    sample_context_t *sample_ctx = &g_sample_context;
    int execution_time = 0;
    int get_tsl_from_cloud = 0;
#ifdef PT_SCANNER
    //get_tsl_from_cloud = 1;
#endif
    execution_time = execution_time < 1 ? 1 : execution_time;
    GW_PRINTF("sample execution time: %d minutes\n", execution_time);
    GW_PRINTF("%s tsl from cloud\n", get_tsl_from_cloud == 0 ? "Not get" : "get");

    memset(sample_ctx, 0, sizeof(sample_context_t));
    sample_ctx->thing_enabled = 1;

    linkkit_start(16, get_tsl_from_cloud, linkkit_loglevel_debug, &alinkops, linkkit_cloud_domain_shanghai, sample_ctx);
    if (!get_tsl_from_cloud)
    {
        linkkit_set_tsl(TSL_STRING, strlen(TSL_STRING));
    }

    cloud_started = 1;
#endif

#ifdef PT_SCANNER
    init_scanner();
    aos_task_new_ext(&scan_task, "scanner", scanner_srv, NULL, 1024*4, 41);
#endif
    ptgw_main();

    return 0;
}

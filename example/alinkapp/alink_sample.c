/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <sys/time.h>
#include "alink_export.h"
#include "config.h"
#include "json_parser.h"
#include "aos/aos.h"
#include "aos/network.h"
#include "kvmgr.h"
#include <netmgr.h>
#include <accs.h>

#ifdef AOS_ATCMD
#include <atparser.h>
#endif
#ifdef AOS_AT_ADAPTER
#include <at_adapter.h>
#undef CONFIG_YWSS
#endif

#ifdef CONFIG_YWSS
#include <enrollee.h>
#endif

/* raw data device means device post byte stream to cloud,
 * cloud translate byte stream to json value by lua script
 * for each product model, vendor need to sumbit a lua script
 * doing this job
 *
 * undefine RAW_DATA_DEVICE, sample will post json string to cloud
 */
//#define RAW_DATA_DEVICE

#define Method_PostData    "postDeviceData"
#define Method_PostRawData "postDeviceRawData"
#define Method_GetAlinkTime "getAlinkTime"

#define PostDataFormat      "{\"ErrorCode\":{\"value\":\"%d\"},\"Hue\":{\"value\":\"%d\"},\"Luminance\":{\"value\":\"%d\"},\"Switch\":{\"value\":\"%d\"},\"WorkMode\":{\"value\":\"%d\"}}"
#define post_data_buffer_size    (512)
static void do_report(void);
#ifndef RAW_DATA_DEVICE
static char post_data_buffer[post_data_buffer_size];
#else
static char raw_data_buffer[post_data_buffer_size];

/* rawdata byte order
 *
 * rawdata[0] = 0xaa;
 * rawdata[1] = 0x07;
 * rawdata[2] = device.power;
 * rawdata[3] = device.work_mode;
 * rawdata[4] = device.temp_value;
 * rawdata[5] = device.light_value;
 * rawdata[6] = device.time_delay;
 * rawdata[7] = 0x55;
 */
#define RAW_DATA_SIZE           (8)
uint8_t device_state_raw_data[RAW_DATA_SIZE] = {
    0xaa, 0x07, 1, 8, 10, 50, 10, 0x55
};
#endif

enum {
    ATTR_ERRORCODE_INDEX,
    ATTR_HUE_INDEX,
    ATTR_LUMINANCE_INDEX,
    ATTR_SWITCH_INDEX,
    ATTR_WORKMODE_INDEX,
    ATTR_MAX_NUMS
};
#define DEVICE_ATTRS_NUM   (ATTR_MAX_NUMS)

int device_state[] = {0, 10, 50, 1, 2};/* default value */
char *device_attr[] = {
    "ErrorCode",
    "Hue",
    "Luminance",
    "Switch",
    "WorkMode",
    NULL
};

static bool auto_netmgr;

void helper_api_test(void);
void activate_button_pressed(void *arg);

static void cloud_connected(int cb_type, const char *json_buffer) {
    //do_report();
}

static void cloud_disconnected(int cb_type, const char *json_buffer) { LOG("alink cloud disconnected!"); }

int callback_upgrade_device(const char *params)
{
    LOG("alink device start to upgrade.");
    return 0;
}

int callback_cancel_upgrade_device(const char *params)
{
    LOG("alink device stop to upgrade.");
    return 0;
}

#ifndef RAW_DATA_DEVICE
static void cloud_get_device_status(int cb_type, const char *json_buffer)
{
    do_report();
    LOG("---> get device status :  %s",json_buffer);
}

static void cloud_set_device_status(int cb_type, const char *json_buffer)
{
    int attr_len = 0, value_len = 0, value = 0, i;
    char *value_str = NULL, *attr_str = NULL;

    LOG("---> set device status :  %s",json_buffer);
    for (i = 0; device_attr[i]; i++) {
        attr_str = json_get_value_by_name((char *)json_buffer, strlen(json_buffer),
                device_attr[i], &attr_len, NULL);
        value_str = json_get_value_by_name(attr_str, attr_len,
                "value", &value_len, NULL);

        if (value_str && value_len > 0) {
            char last_char = *(value_str+value_len);
            *(value_str + value_len) = 0;
            value = atoi(value_str);
            *(value_str + value_len) = last_char;
            device_state[i] = value;
        }
    }
    do_report();
}

#else
static char a2x(char ch)
{
    switch (ch) {
        case '1':
            return 1;
        case '2':
            return 2;
        case '3':
            return 3;
        case '4':
            return 4;
        case '5':
            return 5;
        case '6':
            return 6;
        case '7':
            return 7;
        case '8':
            return 8;
        case '9':
            return 9;
        case 'A':
        case 'a':
            return 10;
        case 'B':
        case 'b':
            return 11;
        case 'C':
        case 'c':
            return 12;
        case 'D':
        case 'd':
            return 13;
        case 'E':
        case 'e':
            return 14;
        case 'F':
        case 'f':
            return 15;
        default:
            break;;
    }

    return 0;
}

int uart_send(const uint8_t *raw_data, int raw_data_len)
{
    //TODO: implement uart send here
    int i;
    for (i = 0; i < raw_data_len; i++)
        printf("%02x ", raw_data[i]);

    printf("\n");
    return 0;
}

int raw_data_unserialize(char *json_buffer, uint8_t *raw_data, int *raw_data_len)
{
    int attr_len = 0, i = 0;
    char *attr_str = NULL;

    assert(json_buffer && raw_data && raw_data_len);

    attr_str = json_get_value_by_name(json_buffer, strlen(json_buffer),
            "rawData", &attr_len, NULL);

    if (!attr_str || !attr_len || attr_len > *raw_data_len * 2)
        return -1;

    for (i = 0; i < attr_len; i += 2) {
        raw_data[i / 2] = a2x(attr_str[i]) << 4;
        raw_data[i / 2] += a2x(attr_str[i + 1]);
    }

    raw_data[i / 2] = '\0';
    *raw_data_len = i / 2;

    return 0;
}

static void cloud_get_device_raw_data(int cb_type, const char *json_buffer)
{
    int ret = 0, raw_data_len = RAW_DATA_SIZE;
    uint8_t raw_data[RAW_DATA_SIZE] = { 0 };

    ret = raw_data_unserialize(json_buffer, raw_data, &raw_data_len);
    if (!ret)
        uart_send(raw_data, raw_data_len);
}

static void cloud_set_device_raw_data(int cb_type, const char *json_buffer)
{
    int ret = 0, raw_data_len = RAW_DATA_SIZE;
    uint8_t raw_data[RAW_DATA_SIZE] = { 0 };

    ret = raw_data_unserialize(json_buffer, raw_data, &raw_data_len);
    if (!ret) {
        /* update device state */
        memcpy(device_state_raw_data, raw_data, raw_data_len);
        uart_send(raw_data, raw_data_len);
    }
}

int alink_post_raw_data(uint8_t *byte_stream, int len)
{
        int i, size;
#define RawDataHeader   "{\"rawData\":\""
#define RawDataTail     "\", \"length\":\"%d\"}"

        size = strlen(RawDataHeader);
        strncpy(raw_data_buffer, RawDataHeader, post_data_buffer_size);
        for (i = 0; i < len; i++) {
           size += snprintf(raw_data_buffer + size,
                   post_data_buffer_size - size, "%02X", byte_stream[i]);
        }

        size += snprintf(raw_data_buffer + size,
                post_data_buffer_size - size, RawDataTail, len * 2);

        return alink_report(Method_PostRawData, raw_data_buffer);
}
#endif

static void do_report(void)
{
    //TODO: async
    //aos_loop_schedule_work(1000, activate_button_pressed, NULL, NULL, NULL);
    //helper_api_test();
#ifdef RAW_DATA_DEVICE
    /*
     * Note: post data to cloud,
     * use sample alink_post_raw_data()
     * or alink_post()
     */
    /* sample for raw data device */
    alink_post_raw_data(device_state_raw_data, RAW_DATA_SIZE);

#else
    /* sample for json data device */
    snprintf(post_data_buffer, post_data_buffer_size, PostDataFormat,
            device_state[ATTR_ERRORCODE_INDEX],
            device_state[ATTR_HUE_INDEX],
            device_state[ATTR_LUMINANCE_INDEX],
            device_state[ATTR_SWITCH_INDEX],
            device_state[ATTR_WORKMODE_INDEX]);
    LOG("start report async");
    aos_cloud_report(Method_PostData, post_data_buffer, NULL, NULL);
#endif
}

int alink_get_time(unsigned int *utc_time)
{
#define TIME_STR_LEN    (32)
    char buf[TIME_STR_LEN] = { 0 }, *attr_str;
    int size = TIME_STR_LEN, attr_len = 0;
    int ret;

    ret = alink_query(Method_GetAlinkTime, "{}", buf, &size);
    if (!ret) {
        attr_str = json_get_value_by_name(buf, size, "time", &attr_len, NULL);
        if (attr_str && utc_time) {
            sscanf(attr_str, "%u", utc_time);
        }
    }

    return ret;
}

void helper_api_test(void)
{
    unsigned int time;
    int ret = alink_get_time(&time);
    assert(!ret);

    LOG("get alink utc time: %d", time);
}

void awss_demo(void)
{
#if 0
    char ssid[PLATFORM_MAX_SSID_LEN] = { 0 };
    char passwd[PLATFORM_MAX_PASSWD_LEN] = { 0 };
#define WLAN_CONNECTION_TIMEOUT     (30 * 1000) //30 seconds

    /* after system booting, read ssid & passwd from flash */
    vendor_read_ssid_passwd_from_flash(ssid, passwd);

    if (ssid is empty)
        awss_start(); /* Note: awss_start() will block until success */
#endif

    /* call alink_start() after system got IP address */

    /* activate device after alink connnected, see activate_button_pressed */
}

/* activate sample */
char active_data_tx_buffer[128];
#define ActivateDataFormat    "{\"ErrorCode\": { \"value\": \"%d\" }}"
void alink_activate(void* arg)
{
    snprintf(active_data_tx_buffer, sizeof(active_data_tx_buffer)-1, ActivateDataFormat, 1);
    aos_cli_printf("active send:%s\r\n", active_data_tx_buffer);
    alink_report_async(Method_PostData, (char *)active_data_tx_buffer, NULL, NULL);

    snprintf(active_data_tx_buffer, sizeof(active_data_tx_buffer)-1, ActivateDataFormat, 0);
    aos_cli_printf("send:%s\r\n", active_data_tx_buffer);
    alink_report_async(Method_PostData, (char *)active_data_tx_buffer, NULL, NULL);
}

void alink_key_process(input_event_t *eventinfo, void *priv_data)
{
    if (eventinfo->type != EV_KEY) {
        return;
    }

    if (eventinfo->code == CODE_BOOT) {
        if (eventinfo->value == VALUE_KEY_CLICK) {
            if (cloud_is_connected() == false) {
                netmgr_start(true);
            } else {
                alink_activate(NULL);
            }
        } else if(eventinfo->value == VALUE_KEY_LTCLICK) {
            netmgr_clear_ap_config();
            aos_reboot();
        } else if(eventinfo->value == VALUE_KEY_LLTCLICK) {
            netmgr_clear_ap_config();
            alink_factory_reset();
        }
    }
}

static void handle_reset_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    alink_factory_reset();
}

static struct cli_command resetcmd = {
    .name = "reset",
    .help = "factory reset",
    .function = handle_reset_cmd
};

static void handle_active_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    aos_schedule_call(alink_activate, NULL);
}

static struct cli_command ncmd = {
    .name = "active_alink",
    .help = "active_alink [start]",
    .function = handle_active_cmd
};

static void handle_model_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    #define MAX_MODEL_LENGTH 30
    char model[MAX_MODEL_LENGTH] = "light";
    int  model_len = sizeof(model);
    aos_kv_get("model", model, &model_len);

    if (argc == 1) {
        aos_cli_printf("Usage: model light/gateway. Model is currently %s\r\n", model);
        return;
    }

    if (strcmp(argv[1], "gateway") == 0) {
        if (strcmp(model, argv[1])) {
            aos_kv_del("alink");
            aos_kv_set("model", "gateway", sizeof("gateway"), 1);
            aos_cli_printf("Swith model to gateway, please reboot\r\n");
        } else {
            aos_cli_printf("Current model is already gateway\r\n");
        }
    } else {
        if (strcmp(model, argv[1])) {
            aos_kv_del("alink");
            aos_kv_set("model", "light", sizeof("light"), 1);
            aos_cli_printf("Swith model to light, please reboot\r\n");
        } else {
            aos_cli_printf("Current model is already light\r\n");
        }
    }
}

static struct cli_command modelcmd = {
    .name = "model",
    .help = "model light/gateway",
    .function = handle_model_cmd
};


static void handle_uuid_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    extern int cloud_is_connected(void);
    extern bool gateway_is_connected(void);
    extern const char *gateway_get_uuid(void);
    if (cloud_is_connected()) {
        aos_cli_printf("uuid: %s\r\n", config_get_main_uuid());
#ifdef MESH_GATEWAY_SERVICE
    } else if (gateway_is_connected()) {
        aos_cli_printf("uuid: %s\r\n", gateway_get_uuid());
#endif
    } else {
        aos_cli_printf("alink is not connected\r\n");
    }
}

static struct cli_command uuidcmd = {
    .name = "uuid",
    .help = "uuid",
    .function = handle_uuid_cmd
};

enum SERVER_ENV {
    DAILY = 0,
    SANDBOX,
    ONLINE,
    DEFAULT
};
const char *env_str[] = {"daily", "sandbox", "online", "default"};

void usage(void)
{
    LOG("\nalink_sample -e enviroment -t work_time -l log_level");
    LOG("\t -e alink server environment, 'daily', 'sandbox' or 'online'(default)");
    LOG("\t -t work time, unit is s");
    LOG("\t -l log level, trace/debug/info/warn/error/fatal/none");
    LOG("\t -h show help text");
}
enum MESH_ROLE {
    MESH_MASTER = 0,
    MESH_NODE = 1,
    MESH_GATEWAY
};

static int env = DEFAULT;
static int mesh_mode = MESH_GATEWAY;
extern char *optarg;
extern char *g_sn;

static int is_alink_started = 0;

static void alink_service_event(input_event_t *event, void *priv_data) {
#ifndef AOS_AT_ADAPTER
    if (event->type != EV_WIFI) {
#else
    if (event->type != EV_AT) {
#endif
        return;
    }

#ifndef AOS_AT_ADAPTER
    if (event->code != CODE_WIFI_ON_GOT_IP) {
#else
    if (event->code != CODE_AT_IF_READY) {
#endif
        return;
    }

    if(is_alink_started == 0) {
        is_alink_started = 1;
        alink_start();
    }
}
typedef struct ota_device_info {
    const char *product_key;
    const char *device_name;
    const char *uuid;
} OTA_device_info_t;

OTA_device_info_t ota_device_info;

static void alink_connect_event(input_event_t *event, void *priv_data)
{
    if (event->type != EV_SYS) {
        return;
    }

    if (event->code == CODE_SYS_ON_ALINK_ONLINE ) {

#ifdef CONFIG_YWSS
        awss_registrar_init();
#endif
        ota_device_info.uuid = config_get_main_uuid();
        aos_post_event(EV_SYS, CODE_SYS_ON_START_FOTA, (long unsigned int)&ota_device_info);
        do_report();
        return;
    }
}
static int alink_cloud_report(const char *method, const char *json_buffer)
{
    return alink_report_async(method, json_buffer, NULL, NULL);
}

static void alink_cloud_connected(void) {
    aos_post_event(EV_YUNIO, CODE_YUNIO_ON_CONNECTED, 0);
    LOG("alink cloud connected!");

    aos_cloud_register_backend(&alink_cloud_report);
    aos_cloud_trigger(CLOUD_CONNECTED, NULL);
}

static void alink_cloud_disconnected(void) {
    aos_post_event(EV_YUNIO, CODE_YUNIO_ON_DISCONNECTED, 0);
    aos_cloud_trigger(CLOUD_DISCONNECTED, NULL);
}

static void alink_cloud_get_device_status(char *json_buffer)
{
    aos_cloud_trigger(GET_DEVICE_STATUS, json_buffer);
}

static void alink_cloud_set_device_status(char *json_buffer)
{
    aos_cloud_trigger(SET_DEVICE_STATUS, json_buffer);
}

static void alink_cloud_get_device_raw_data(char *json_buffer)
{
    aos_cloud_trigger(GET_DEVICE_RAWDATA, json_buffer);
}

static void alink_cloud_set_device_raw_data(char *json_buffer)
{
    aos_cloud_trigger(SET_DEVICE_RAWDATA, json_buffer);
}

static void alink_cloud_init(void)
{
    alink_register_callback(ALINK_CLOUD_CONNECTED, &alink_cloud_connected);
    alink_register_callback(ALINK_CLOUD_DISCONNECTED, &alink_cloud_disconnected);
    alink_register_callback(ALINK_GET_DEVICE_RAWDATA, &alink_cloud_get_device_raw_data);
    alink_register_callback(ALINK_SET_DEVICE_RAWDATA, &alink_cloud_set_device_raw_data);
    alink_register_callback(ALINK_GET_DEVICE_STATUS, &alink_cloud_get_device_status);
    alink_register_callback(ALINK_SET_DEVICE_STATUS, &alink_cloud_set_device_status);

    aos_cloud_register_callback(CLOUD_CONNECTED, &cloud_connected);
    aos_cloud_register_callback(CLOUD_DISCONNECTED, &cloud_disconnected);
    /*
     * NOTE: register ALINK_GET/SET_DEVICE_STATUS or ALINK_GET/SET_DEVICE_RAWDATA
     */
#ifdef RAW_DATA_DEVICE
    /*
     * TODO: before using callback ALINK_GET/SET_DEVICE_RAWDATA,
     * submit product_model_xxx.lua script to ALINK cloud.
     * ALINKTEST_LIVING_LIGHT_SMARTLED_LUA is done with it.
     */
    aos_cloud_register_callback(GET_DEVICE_RAWDATA, &cloud_get_device_raw_data);
    aos_cloud_register_callback(SET_DEVICE_RAWDATA, &cloud_set_device_raw_data);
#else
    aos_cloud_register_callback(GET_DEVICE_STATUS, &cloud_get_device_status);
    aos_cloud_register_callback(SET_DEVICE_STATUS, &cloud_set_device_status);
#endif
}

/***************************auto_netmgr code start*****************************/
#ifndef AOS_AT_ADAPTER
static void clear_kv_and_reboot()
{
    aos_kv_del(NETMGR_WIFI_KEY);
    LOG("KV cleared, will reboot now.");
    aos_reboot();
}

#define AUTO_HOTSPOT_TIMEOUT_S (2*60) // 2 min
#define HOTSPOT_TIMEOUT_KV_NAME "hotspot_timeout"
static int get_hotspot_timeout()
{
    char timeout_s[10];
    int len = sizeof(timeout_s);
    int timeout;

    if (aos_kv_get(HOTSPOT_TIMEOUT_KV_NAME, (void *)timeout_s, &len) == 0) {
        LOG("hotspot_timeout KV value will be used: %s seconds", timeout_s);
        timeout = atoi(timeout_s);
    } else {
        timeout = AUTO_HOTSPOT_TIMEOUT_S;
        LOG("Default hotspot timeout (%s) will be used.", timeout);
    }

    return timeout * 1000; // s -> ms
}

static void auto_netmgr_timer_fn(void *arg1, void *arg2)
{
    LOG("Info: Auto netmgr timeout hit.");
    clear_kv_and_reboot();
}

#define AUTO_NETMGR_TIMEOUT (10*60*1000) // 10 mins
static aos_timer_t auto_netmgr_timer = {NULL};
static aos_timer_t hotspot_timer = {NULL};
static void start_auto_netmgr_timer()
{
    int ms = AUTO_NETMGR_TIMEOUT;

    LOG("%s", __func__);
    if (auto_netmgr == false) return;
    if (aos_timer_new(&auto_netmgr_timer, auto_netmgr_timer_fn, NULL, ms, 0) != 0) {
        LOG("Error: aos_timer_new failed in %s", __func__);
        return;
    }
    if (aos_timer_start(&hotspot_timer) != 0) {
        LOG("Error: aos_timer_start failed in %s", __func__);
        return;
    }
}

static void stop_timer(aos_timer_t *timer)
{
    LOG("%s", __func__);
    if (!timer) {LOG("Timer is not set"); return;}
    if (aos_timer_stop(timer) != 0) {
        LOG("Error: aos_timer_stop failed in ");
        return;
    }
}

static void hotspot_timer_fn(void *arg1, void *arg2)
{
    LOG("Info: Hotspot timeout hit.");
    clear_kv_and_reboot();
}

static void awss_hotspot_connected_handler()
{
    int ms;

    LOG("%s", __func__);
    if (auto_netmgr == false) return;
    ms = get_hotspot_timeout();
    stop_timer(&auto_netmgr_timer);
    if (aos_timer_new(&hotspot_timer, hotspot_timer_fn, NULL, ms, 0) != 0) {
        LOG("Error: aos_timer_new failed");
        return;
    }
    if (aos_timer_start(&hotspot_timer) != 0) {
        LOG("Error: aos_timer_start failed");
        return;
    }
}

static void awss_hotspot_switch_ap_done_handler()
{
    LOG("%s", __func__);
    if (auto_netmgr == false) return;
    stop_timer(&hotspot_timer);
}

#define ACTIVE_DELAY (5*1000)
static void do_auto_active()
{
    alink_activate(NULL);
    LOG("--------auto active done------");
    aos_msleep(ACTIVE_DELAY);
    clear_kv_and_reboot();
}

static void auto_active_handler(input_event_t *event, void *priv_data)
{
    if (auto_netmgr == false) return;
    if (event->type != EV_YUNIO) return;
    if (event->code != CODE_YUNIO_ON_CONNECTED) return;
    aos_post_delayed_action(ACTIVE_DELAY, do_auto_active, NULL);
}

#define AUTO_NETMGR_KEY "auto_netmgr"
static bool get_auto_netmgr_config()
{
    char c[5];
    int len = sizeof(c);
    bool ret;

    if (aos_kv_get(AUTO_NETMGR_KEY, (void *)c, &len) != 0) {
        ret = false;
        LOGI("alink", "kv(%s) not set, auto_netmgr will be disabled",
          AUTO_NETMGR_KEY);
    } else {
        ret = true;
        LOGD("alink", "kv(%s) found, auto_netmgr will be enabled",
          AUTO_NETMGR_KEY);
    }

    return ret;
}
#endif

/***************************auto_netmgr code end*******************************/

int application_start(int argc, char *argv[])
{
#if AOS_ATCMD
    at.set_mode(ASYN);
    at.init(AT_RECV_PREFIX, AT_RECV_SUCCESS_POSTFIX, 
            AT_RECV_FAIL_POSTFIX, AT_SEND_DELIMITER, 1000);
#endif

#ifdef WITH_SAL
    sal_init();
#endif

    aos_set_log_level(AOS_LL_DEBUG);

#ifndef AOS_AT_ADAPTER
    if (mesh_mode == MESH_MASTER) {
#ifdef CONFIG_AOS_DDM
        ddm_run(argc, argv);
#endif
        return 0;
    }

#ifdef CONFIG_AOS_DDA
    dda_enable(atoi(mesh_num));
    dda_service_init();
#endif
#endif // #ifndef AOS_AT_ADAPTER

    aos_cli_register_command(&uuidcmd);
    alink_cloud_init();

    if (mesh_mode == MESH_GATEWAY) {
        aos_cli_register_command(&ncmd);
        aos_cli_register_command(&modelcmd);
        aos_cli_register_command(&resetcmd);

        if (env == SANDBOX)
            alink_enable_sandbox_mode();
        else if (env == DAILY)
            alink_enable_daily_mode(NULL, 0);

#ifndef AOS_AT_ADAPTER
        aos_register_event_filter(EV_WIFI, alink_service_event, NULL);
#else
        aos_register_event_filter(EV_AT, alink_service_event, NULL);
#endif
        aos_register_event_filter(EV_SYS, alink_connect_event, NULL);
        aos_register_event_filter(EV_KEY, alink_key_process, NULL);
#ifndef AOS_AT_ADAPTER
        aos_register_event_filter(EV_YUNIO, auto_active_handler, NULL);

        auto_netmgr = get_auto_netmgr_config();
        awss_register_callback(AWSS_HOTSPOT_CONNECTED,
          &awss_hotspot_connected_handler);
        awss_register_callback(AWSS_HOTSPOT_SWITCH_AP_DONE,
          &awss_hotspot_switch_ap_done_handler);
        if (auto_netmgr) start_auto_netmgr_timer();
#else
        auto_netmgr = false;
        at_adapter_init();
#endif

        netmgr_init();
        netmgr_start(auto_netmgr);
    }

#if defined(CONFIG_AOS_DDA) && !defined(AOS_AT_ADAPTER)
    dda_service_start();
#else
    aos_loop_run();
    LOG("alink end.");
    alink_end();
#endif

    return 0;
}

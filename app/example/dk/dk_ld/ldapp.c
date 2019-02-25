/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "iot_import.h"
#include "iot_export.h"
#include "iot_export_mqtt.h"
#include "ulog/ulog.h"
#include "aos/yloop.h"
#include "network/network.h"
#include <netmgr.h>
#include <aos/kernel.h>
#include <k_err.h>
#include <netmgr.h>
#include <aos/cli.h>


#include "sensor/sensor.h"

#ifdef LITTLEVGL_DISPLAY
#include "sensor_display.h"
#endif


#define PRODUCT_KEY   "a1W1ZBPEeM9"
#define DEVICE_NAME   "stm32f412zg-nucleo"
#define DEVICE_SECRET "WaqjbXXX3vHbOhWFIoeS7QOsmdlQakJH"

typedef void (*task_fun)(void *);

char __product_key[PRODUCT_KEY_LEN + 1];
char __device_name[DEVICE_NAME_LEN + 1];
char __device_secret[DEVICE_SECRET_LEN + 1];

#define ALINK_BODY_FORMAT \
    "{\"id\":\"%d\",\"version\":\"1.0\",\"method\":\"%s\",\"params\":%s}"
#define ALINK_TOPIC_PROP_POST \
    "/sys/" PRODUCT_KEY "/" DEVICE_NAME "/thing/event/property/post"
#define ALINK_TOPIC_PROP_POSTRSP \
    "/sys/" PRODUCT_KEY "/" DEVICE_NAME "/thing/event/property/post_reply"
#define ALINK_TOPIC_PROP_SET \
    "/sys/" PRODUCT_KEY "/" DEVICE_NAME "/thing/service/property/set"
#define ALINK_METHOD_PROP_POST "thing.event.property.post"
/*
* Please check below item which in feature self-definition from "https://linkdevelop.aliyun.com/"
*/
#define PROP_POST_FORMAT_ACC         "{\"Accelerometer\":{\"X\":%f,\"Y\":%f, \"Z\":%f}}"
#define PROP_POST_FORMAT_HUMITEMP    "{\"report_sensor\":{\"Humi\":%f,\"Temp\":%f}}"
#define PROP_SET_FORMAT_CMDLED       "\"cmd_led\":"

#define MQTT_MSGLEN             (1024)

uint32_t        cnt           = 0;

void *gpclient;

static int             fd_acc  = -1;
static int             fd_als  = -1;
static int             fd_humi = -1;
static int             fd_temp = -1;
static char            linkkit_started = 0;

#define EXAMPLE_TRACE(fmt, ...)  \
    do { \
        HAL_Printf("%s|%03d :: ", __func__, __LINE__); \
        HAL_Printf(fmt, ##__VA_ARGS__); \
        HAL_Printf("%s", "\r\n"); \
    } while(0)


int mqtt_client_example(void);
int linkkit_main(void *paras);


static int sensor_all_open(void)
{
    int fd = -1;
    char name[SENSOR_NAME_LEN];
    int index = 0;

    index = 0;
    snprintf(name,SENSOR_NAME_LEN,"%s/%d",dev_acc_path,index);
    fd = aos_open(name, O_RDWR);
    if (fd < 0) {
        printf("Off-line: aos_open %s fd = %d.\n", name, fd);
    }
    fd_acc = fd;


    index = 0;
    snprintf(name,SENSOR_NAME_LEN,"%s/%d",dev_als_path,index);
    fd = aos_open(name, O_RDWR);
    if (fd < 0) {
        printf("Off-line: aos_open %s fd = %d.\n", name, fd);
    }
    fd_als = fd;

    index = 0;
    snprintf(name,SENSOR_NAME_LEN,"%s/%d",dev_humi_path,index);

    fd = aos_open(name, O_RDWR);
    if (fd < 0) {
        printf("Off-line: aos_open %s fd = %d.\n", name, fd);
    }
    fd_humi = fd;

    index = 0;
    snprintf(name,SENSOR_NAME_LEN,"%s/%d",dev_temp_path,index);
    fd = aos_open(name, O_RDWR);
    if (fd < 0) {
        printf("Off-line: aos_open %s fd = %d.\n", name, fd);
    }
    fd_temp = fd;

    return 0;
}

static int get_acc_data(int32_t *x, int32_t *y, int32_t *z)
{
    accel_data_t data = { 0 };
    ssize_t      size = 0;
    if (fd_acc < 0)
        return 0;
    size = aos_read(fd_acc, &data, sizeof(data));
    if (size != sizeof(data)) {
        printf("aos_read return error.\n");
    }

    *x = data.data[0];
    *y = data.data[1];
    *z = data.data[2];

    return 0;
}

static int get_humi_data(uint32_t *humi, uint64_t *timestamp)
{
    humidity_data_t data = { 0 };
    ssize_t size = 0;
    if (fd_humi < 0)
        return 0;
    size = aos_read(fd_humi, &data, sizeof(data));
    if (size != sizeof(data)) {
        printf("aos_read humi sensor return error.\n");
    }

    *humi      = data.h;
    *timestamp = data.timestamp;
    return 0;
}

static int get_temp_data(uint32_t *temp, uint64_t *timestamp)
{
    temperature_data_t data = { 0 };
    ssize_t            size = 0;
    if (fd_temp < 0)
        return 0;
    size = aos_read(fd_temp, &data, sizeof(data));
    if (size != sizeof(data)) {
        printf("aos_read humi sensor return error.\n");
    }

    *temp      = data.t;
    *timestamp = data.timestamp;
    return 0;
}

/*
 * MQTT Subscribe handler
 * topic: ALINK_TOPIC_PROP_SET
 */
static void handle_prop_set(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg)
{
#ifdef CLD_CMD_LED_REMOTE_CTRL_SUPPORT
    iotx_mqtt_topic_info_pt ptopic_info = (iotx_mqtt_topic_info_pt)msg->msg;
    char *p_serch = NULL;
    uint8_t led_cmd = 0;
    bool gpio_level = 0;
    p_serch = strstr(ptopic_info->payload, PROP_SET_FORMAT_CMDLED);
    if (p_serch != NULL) {
      led_cmd = *(p_serch + strlen(PROP_SET_FORMAT_CMDLED));
    } else {
      LOG("----");
      LOG("Topic: '%.*s' (Length: %d)", ptopic_info->topic_len,
                  ptopic_info->ptopic, ptopic_info->topic_len);
      LOG("Payload: '%.*s' (Length: %d)", ptopic_info->payload_len,
                  ptopic_info->payload, ptopic_info->payload_len);
       LOG("----");
    }
    if (led_cmd == '1' || led_cmd == '0')
      gpio_level = led_cmd - '0';
    board_drv_led_ctrl(gpio_level);
#endif

}


/*
* MQTT Subscribe handler
* topic: ALINK_TOPIC_PROP_POSTRSP
*/
static void handle_prop_postrsp(void *pcontext, void *pclient,
                                iotx_mqtt_event_msg_pt msg)
{
    iotx_mqtt_topic_info_pt ptopic_info = (iotx_mqtt_topic_info_pt)msg->msg;

#if 0
    // print topic name and topic message
    LOG("----");
    LOG("Topic: '%.*s' (Length: %d)", ptopic_info->topic_len,
        ptopic_info->ptopic, ptopic_info->topic_len);
    LOG("Payload: '%.*s' (Length: %d)", ptopic_info->payload_len,
        ptopic_info->payload, ptopic_info->payload_len);
    LOG("----");
#endif
}

static void wifi_service_event(input_event_t *event, void *priv_data)
{
    if (event->type != EV_WIFI) {
        return;
    }

    if (event->code != CODE_WIFI_ON_GOT_IP) {
        return;
    }
    LOG("wifi_service_event!");
    if (!linkkit_started) {
        aos_task_new("iotx_mqtt",(task_fun)linkkit_main, NULL,1024*6);
        linkkit_started = 1;
    }
}

static void mqtt_publish(void *psttimer, void *pclient)
{
    int      rc         = -1;
    char     param[128] = { 0 };
    char     msg_pub[256] = { 0 };
    int      x, y, z;
    uint32_t humi_data      = 0;
    uint32_t temp_data      = 0;
    uint64_t humi_timestamp = 0;
    uint64_t temp_timestamp = 0;
    float    acc_nkg[3]     = { 0 };
    iotx_mqtt_topic_info_t topic_msg;

    /* Initialize topic information */
    memset(&topic_msg, 0x0, sizeof(iotx_mqtt_topic_info_t));

    topic_msg.qos    = IOTX_MQTT_QOS0;
    topic_msg.retain = 0;
    topic_msg.dup    = 0;
    memset(param, 0, sizeof(param));
    memset(msg_pub, 0, sizeof(msg_pub));
#ifdef DEV_BOARD_DEVELOPERKIT
    /* read sensor data */
    get_acc_data(&x, &y, &z);
    acc_nkg[0] = (float)x * 9.8 / 1024;
    acc_nkg[1] = (float)y * 9.8 / 1024;
    acc_nkg[2] = (float)z * 9.8 / 1024;
    sprintf(param, PROP_POST_FORMAT_ACC,
            acc_nkg[0], acc_nkg[1], acc_nkg[2]);

#elif defined DEV_HUMI_TEMP_SUPPORT
    get_humi_data(&humi_data, &humi_timestamp);
    get_temp_data(&temp_data, &temp_timestamp);
    sprintf(param, PROP_POST_FORMAT_HUMITEMP,
            (float)humi_data, (float)temp_data * 0.1);

#endif
    int msg_len = sprintf(msg_pub, ALINK_BODY_FORMAT, cnt,
                          ALINK_METHOD_PROP_POST, param);
    if (msg_len < 0) {
        LOG("Error occur! Exit program");
    }

    topic_msg.payload     = (void *)msg_pub;
    topic_msg.payload_len = msg_len;

    rc = IOT_MQTT_Publish(pclient, ALINK_TOPIC_PROP_POST, &topic_msg);
    if (rc < 0) {
        LOG("error occur when publish. %d", rc);
    }

    LOG("packet-id=%u, publish topic msg=%s", (uint32_t)rc, msg_pub);

    ++cnt;
}

void event_handle(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg)
{
    uintptr_t packet_id = (uintptr_t)msg->msg;
    iotx_mqtt_topic_info_pt topic_info = (iotx_mqtt_topic_info_pt)msg->msg;

    switch (msg->event_type) {
        case IOTX_MQTT_EVENT_UNDEF:
            EXAMPLE_TRACE("undefined event occur.");
            break;

        case IOTX_MQTT_EVENT_DISCONNECT:
            EXAMPLE_TRACE("MQTT disconnect.");
            break;

        case IOTX_MQTT_EVENT_RECONNECT:
            EXAMPLE_TRACE("MQTT reconnect.");
            break;

        case IOTX_MQTT_EVENT_SUBCRIBE_SUCCESS:
            EXAMPLE_TRACE("subscribe success, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_SUBCRIBE_TIMEOUT:
            EXAMPLE_TRACE("subscribe wait ack timeout, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_SUBCRIBE_NACK:
            EXAMPLE_TRACE("subscribe nack, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_UNSUBCRIBE_SUCCESS:
            EXAMPLE_TRACE("unsubscribe success, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_UNSUBCRIBE_TIMEOUT:
            EXAMPLE_TRACE("unsubscribe timeout, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_UNSUBCRIBE_NACK:
            EXAMPLE_TRACE("unsubscribe nack, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_PUBLISH_SUCCESS:
            EXAMPLE_TRACE("publish success, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_PUBLISH_TIMEOUT:
            EXAMPLE_TRACE("publish timeout, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_PUBLISH_NACK:
            EXAMPLE_TRACE("publish nack, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_PUBLISH_RECEIVED:
            EXAMPLE_TRACE("topic message arrived but without any related handle: topic=%.*s, topic_msg=%.*s",
                          topic_info->topic_len,
                          topic_info->ptopic,
                          topic_info->payload_len,
                          topic_info->payload);
            break;

        case IOTX_MQTT_EVENT_BUFFER_OVERFLOW:
            EXAMPLE_TRACE("buffer overflow, %s", msg->msg);
            break;

        default:
            EXAMPLE_TRACE("Should NOT arrive here.");
            break;
    }
}

int mqtt_client(void)
{
    int rc, msg_len, cnt = 0;
    iotx_conn_info_pt pconn_info;
    iotx_mqtt_param_t mqtt_params;
    aos_timer_t  publish_timer;

    HAL_GetProductKey(__product_key);
    HAL_GetDeviceName(__device_name);
    HAL_GetDeviceSecret(__device_secret);

    /* Device AUTH */
    if (0 != IOT_SetupConnInfo(__product_key, __device_name, __device_secret, (void **)&pconn_info)) {
        EXAMPLE_TRACE("AUTH request failed!");
        return -1;
    }

    /* Initialize MQTT parameter */
    memset(&mqtt_params, 0x0, sizeof(mqtt_params));

    mqtt_params.port = pconn_info->port;
    mqtt_params.host = pconn_info->host_name;
    mqtt_params.client_id = pconn_info->client_id;
    mqtt_params.username = pconn_info->username;
    mqtt_params.password = pconn_info->password;
    mqtt_params.pub_key = pconn_info->pub_key;

    mqtt_params.request_timeout_ms = 2000;
    mqtt_params.clean_session = 0;
    mqtt_params.keepalive_interval_ms = 60000;
    mqtt_params.read_buf_size = MQTT_MSGLEN;
    mqtt_params.write_buf_size = MQTT_MSGLEN;

    mqtt_params.handle_event.h_fp = event_handle;
    mqtt_params.handle_event.pcontext = NULL;


    /* Construct a MQTT client with specify parameter */
    gpclient = IOT_MQTT_Construct(&mqtt_params);
    if (NULL == gpclient) {
        EXAMPLE_TRACE("MQTT construct failed");
        return -1;
    }

    IOT_MQTT_Yield(gpclient, 200);

    /* Subscribe the specific topic */
    rc = IOT_MQTT_Subscribe(gpclient, ALINK_TOPIC_PROP_POSTRSP,
                            IOTX_MQTT_QOS0, handle_prop_postrsp, NULL);
    if (rc < 0) {
        IOT_MQTT_Destroy(&gpclient);
        EXAMPLE_TRACE("IOT_MQTT_Subscribe() failed, rc = %d", rc);
        return -1;
    }

    /* Subscribe the specific topic */
    rc = IOT_MQTT_Subscribe(gpclient, ALINK_TOPIC_PROP_SET,
                             IOTX_MQTT_QOS0, handle_prop_set, NULL);
    if (rc < 0) {
        IOT_MQTT_Destroy(&gpclient);
        EXAMPLE_TRACE("IOT_MQTT_Subscribe() failed, rc = %d", rc);
        return -1;
    }
    IOT_MQTT_Yield(gpclient, 200);
    HAL_SleepMs(1000);
    rc = aos_timer_new(&publish_timer, mqtt_publish, gpclient, 2000, 1);
    if (rc < 0) {
        IOT_MQTT_Destroy(&gpclient);
        EXAMPLE_TRACE("ldapp creat timer failed, rc = %d", rc);
        return -1;
    }

    while (1) {
        IOT_MQTT_Yield(gpclient, 200);
    }
    IOT_MQTT_Unsubscribe(gpclient, ALINK_TOPIC_PROP_POSTRSP);
    IOT_MQTT_Yield(gpclient, 200);
    IOT_MQTT_Unsubscribe(gpclient, ALINK_TOPIC_PROP_SET);
    IOT_MQTT_Yield(gpclient, 200);

    IOT_MQTT_Destroy(&gpclient);
    return 0;
}

int linkkit_main(void *paras)
{
    IOT_OpenLog("mqtt");
    IOT_SetLogLevel(IOT_LOG_DEBUG);

    HAL_SetProductKey(PRODUCT_KEY);
    HAL_SetDeviceName(DEVICE_NAME);
    HAL_SetDeviceSecret(DEVICE_SECRET);
#if defined(SUPPORT_ITLS)
    HAL_SetProductSecret(PRODUCT_SECRET);
#endif
    /* Choose Login Server */
    int domain_type = IOTX_CLOUD_REGION_SHANGHAI;
    IOT_Ioctl(IOTX_IOCTL_SET_DOMAIN, (void *)&domain_type);

    /* Choose Login  Method */
    int dynamic_register = 0;
    IOT_Ioctl(IOTX_IOCTL_SET_DYNAMIC_REGISTER, (void *)&dynamic_register);

    mqtt_client();
    IOT_DumpMemoryStats(IOT_LOG_DEBUG);
    IOT_CloseLog();

    EXAMPLE_TRACE("out of sample!");

    return 0;
}


int application_start(int argc, char *argv[])
{
    netmgr_ap_config_t apconfig;

#ifdef CSP_LINUXHOST
    signal(SIGPIPE, SIG_IGN);
#endif

#ifdef WITH_SAL
    sal_init();
#endif

    printf("== Build on: %s %s ===\n", __DATE__, __TIME__);
    aos_set_log_level(AOS_LL_DEBUG);

    aos_register_event_filter(EV_WIFI, wifi_service_event, NULL);

    sensor_all_open();

    netmgr_init();
#if 0
    memset(&apconfig, 0, sizeof(apconfig));
    strcpy(apconfig.ssid, "LinkDevelop-Workshop");
    strcpy(apconfig.pwd, "linkdevelop");
    netmgr_set_ap_config(&apconfig);
#endif
    netmgr_start(false);

#ifdef LITTLEVGL_DISPLAY
    sensor_display_init();
#endif

    aos_loop_run();

    return 0;
}

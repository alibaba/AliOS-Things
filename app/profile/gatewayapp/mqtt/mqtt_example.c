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
#include "udata/udata.h"
#include "device.h"

#ifdef LITTLEVGL_DISPLAY
#include "sensor_display.h"
#endif

#define MQTT_APP_MSG_LEN     (256)
#define MQTT_APP_PARAMS_LEN  (128)


/* #define PRODUCT_KEY   "a1EFEUaGi0K" */
/* #define DEVICE_NAME   "H1QT0N3oSMENYjHIoDvI" */
/* #define DEVICE_SECRET "WgwmLPWjm1k7z4m1MkCVpGlKTN0zxGeT" */

typedef void (*task_fun)(void *);

char __product_key[PRODUCT_KEY_LEN + 1];
char __device_name[DEVICE_NAME_LEN + 1];
char __device_secret[DEVICE_SECRET_LEN + 1];

#define ALINK_BODY_FORMAT \
    "{\"id\":\"%u\",\"version\":\"1.0\",\"method\":\"%s\",\"params\":%s}"
#define ALINK_TOPIC_PROP_POST \
    "/sys/" PRODUCT_KEY "/" DEVICE_NAME "/thing/event/property/post"
#define ALINK_TOPIC_PROP_POSTRSP \
    "/sys/" PRODUCT_KEY "/" DEVICE_NAME "/thing/event/property/post_reply"
#define ALINK_TOPIC_PROP_SET \
    "/sys/" PRODUCT_KEY "/" DEVICE_NAME "/thing/service/property/set"
#define ALINK_METHOD_PROP_POST "thing.event.property.post"

#define PROP_SET_FORMAT_UDATA      "\"dtc_config\":"


#define DATA_CONVERT_FLOAT(a,b)  (((float)(a))/((float)(b)))
#define DATA_CONVERT_INT(a,b)    (((int32_t)(a))/((int32_t)(b)))


#define MQTT_DTC_ACC_ON     "\"acc=1\""
#define MQTT_DTC_ACC_OFF    "\"acc=0\""

#define MQTT_DTC_GYRO_ON    "\"gyro=1\""
#define MQTT_DTC_GYRO_OFF   "\"gyro=0\""

#define MQTT_DTC_MAG_ON     "\"mag=1\""
#define MQTT_DTC_MAG_OFF    "\"mag=0\""

#define MQTT_DTC_TEMP_ON    "\"temp=1\""
#define MQTT_DTC_TEMP_OFF   "\"temp=0\""

#define MQTT_DTC_HUMI_ON    "\"humi=1\""
#define MQTT_DTC_HUMI_OFF   "\"humi=0\""

#define MQTT_DTC_ALS_ON     "\"als=1\""
#define MQTT_DTC_ALS_OFF    "\"als=0\""

#define MQTT_DTC_PS_ON      "\"ps=1\""
#define MQTT_DTC_PS_OFF     "\"ps=0\""

#define MQTT_DTC_BARO_ON    "\"baro=1\""
#define MQTT_DTC_BARO_OFF   "\"baro=0\""
#define MQTT_MSGLEN                 (1024)

uint32_t cnt  = 0;
void *gpclient = NULL;
static char linkkit_started = 0;
static int g_mqtt_con_flag = 0;

static udata_type_e g_service_type[UDATA_MAX_CNT] = {0};
static bool g_service_flag[UDATA_MAX_CNT] = {0};


#define EXAMPLE_TRACE(fmt, ...)  \
    do { \
        HAL_Printf("%s|%03d :: ", __func__, __LINE__); \
        HAL_Printf(fmt, ##__VA_ARGS__); \
        HAL_Printf("%s", "\r\n"); \
    } while(0)


int mqtt_client_example(void);
int linkkit_main(void *paras);

/*
 * MQTT Subscribe handler
 * topic: ALINK_TOPIC_PROP_SET
 */
static void handle_prop_set(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg)
{
    int ret;
    udata_type_e type;
    iotx_mqtt_topic_info_pt ptopic_info = (iotx_mqtt_topic_info_pt)msg->msg;
    char* p_serch = NULL;
    char* str = 0;
    p_serch = strstr(ptopic_info->payload, PROP_SET_FORMAT_UDATA);
    if (p_serch != NULL) {
        str = (p_serch + strlen(PROP_SET_FORMAT_UDATA));
        printf("str==%s\n",str);
    } else {
      LOG("----");
      LOG("Topic: '%.*s' (Length: %d)", ptopic_info->topic_len,
                  ptopic_info->ptopic, ptopic_info->topic_len);
      LOG("Payload: '%.*s' (Length: %d)", ptopic_info->payload_len,
                  ptopic_info->payload, ptopic_info->payload_len);
       LOG("----");
    }
    if (0 == strncmp(str, MQTT_DTC_ACC_ON, strlen(MQTT_DTC_ACC_ON))){
        type = UDATA_SERVICE_ACC;
        g_service_flag[type] = true;
    }
    else if (0 == strncmp(str, MQTT_DTC_ACC_OFF, strlen(MQTT_DTC_ACC_OFF))){
        type = UDATA_SERVICE_ACC;
        g_service_flag[type] = false;
    }
    else if (0 == strncmp(str, MQTT_DTC_MAG_ON, strlen(MQTT_DTC_MAG_ON))){
        type = UDATA_SERVICE_MAG;
        g_service_flag[type] = true;
    }
    else if (0 == strncmp(str, MQTT_DTC_MAG_OFF, strlen(MQTT_DTC_MAG_OFF))){
        type = UDATA_SERVICE_MAG;
        g_service_flag[type] = false;
    }
    else if (0 == strncmp(str, MQTT_DTC_GYRO_ON, strlen(MQTT_DTC_GYRO_ON))){
        type = UDATA_SERVICE_GYRO;
        g_service_flag[type] = true;
    }
    else if (0 == strncmp(str, MQTT_DTC_GYRO_OFF, strlen(MQTT_DTC_GYRO_OFF))){
        type = UDATA_SERVICE_GYRO;
        g_service_flag[type] = false;
    }
    else if (0 == strncmp(str, MQTT_DTC_TEMP_ON, strlen(MQTT_DTC_TEMP_ON))){
        type = UDATA_SERVICE_TEMP;
        g_service_flag[type] = true;
    }
    else if (0 == strncmp(str, MQTT_DTC_TEMP_OFF, strlen(MQTT_DTC_TEMP_OFF))){
        type = UDATA_SERVICE_TEMP;
        g_service_flag[type] = false;
    }
    else if (0 == strncmp(str, MQTT_DTC_HUMI_ON, strlen(MQTT_DTC_HUMI_ON))){
        type = UDATA_SERVICE_HUMI;
        g_service_flag[type] = true;
    }
    else if (0 == strncmp(str, MQTT_DTC_HUMI_OFF, strlen(MQTT_DTC_HUMI_OFF))){
        type = UDATA_SERVICE_HUMI;
        g_service_flag[type] = false;
    }
    else if (0 == strncmp(str, MQTT_DTC_ALS_ON, strlen(MQTT_DTC_ALS_ON))){
        type = UDATA_SERVICE_ALS;
        g_service_flag[type] = true;
    }
    else if (0 == strncmp(str, MQTT_DTC_ALS_OFF, strlen(MQTT_DTC_ALS_OFF))){
        type = UDATA_SERVICE_ALS;
        g_service_flag[type] = false;
    }
    else if (0 == strncmp(str, MQTT_DTC_PS_ON, strlen(MQTT_DTC_PS_ON))){
        type = UDATA_SERVICE_PS;
        g_service_flag[type] = true;
    }
    else if (0 == strncmp(str, MQTT_DTC_PS_OFF, strlen(MQTT_DTC_PS_OFF))){
        type = UDATA_SERVICE_PS;
        g_service_flag[type] = false;
    }
    else if (0 == strncmp(str, MQTT_DTC_BARO_ON, strlen(MQTT_DTC_BARO_ON))){
        type = UDATA_SERVICE_BARO;
        g_service_flag[type] = true;
    }
    else if (0 == strncmp(str, MQTT_DTC_BARO_OFF, strlen(MQTT_DTC_BARO_OFF))){
        type = UDATA_SERVICE_BARO;
        g_service_flag[type] = false;
    }

    else{
        return;
    }

    ret = service_dtc_publish_set(type, g_service_flag[type]);
    if(unlikely(ret)){
        return;
    }
}


/*
* MQTT Subscribe handler
* topic: ALINK_TOPIC_PROP_POSTRSP
*/
static void handle_prop_postrsp(void *pcontext, void *pclient,
                                iotx_mqtt_event_msg_pt msg)
{
#if 0

    iotx_mqtt_topic_info_pt ptopic_info = (iotx_mqtt_topic_info_pt)msg->msg;

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
    int rc;
    iotx_conn_info_pt pconn_info;
    iotx_mqtt_param_t mqtt_params;

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
#if 0
    rc = aos_timer_new(&publish_timer, mqtt_publish, gpclient, 2000, 1);
    if (rc < 0) {
        IOT_MQTT_Destroy(&gpclient);
        EXAMPLE_TRACE("ldapp creat timer failed, rc = %d", rc);
        return -1;
    }
#endif

    if(g_mqtt_con_flag == 0){
        rc = service_dtc_connect_set(true);
        if(0 == rc){
            g_mqtt_con_flag = 1;
        }
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

int mqtt_sample_start(void)
{
    int i;

    for(i = 0 ; i < UDATA_MAX_CNT; i++){
        g_service_type[i] = UDATA_MAX_CNT;
        g_service_flag[i] = false;
    }
#ifdef CSP_LINUXHOST
    signal(SIGPIPE, SIG_IGN);
#endif

#ifdef WITH_SAL
    sal_init();
#endif

    printf("== Build on: %s %s ===\n", __DATE__, __TIME__);
    aos_set_log_level(AOS_LL_DEBUG);

    aos_register_event_filter(EV_WIFI, wifi_service_event, NULL);

    netmgr_init();
    netmgr_start(false);

    return 0;
}

int gateway_cloud_report(void* pdata, uint32_t len)
{
    int      rc         = -1;
    char     msg_pub[MQTT_APP_MSG_LEN]  = {0};
    iotx_mqtt_topic_info_t topic_msg;

    if(pdata == NULL){
        return -1;
    }

    if(strlen(pdata) >= MQTT_APP_PARAMS_LEN){
        return -1;
    }

    LOG("mqtt = %s\n",(char*)pdata);


    if(NULL == gpclient){
        return -1;
    }
    /* Initialize topic information */
    memset(&topic_msg, 0x0, sizeof(iotx_mqtt_topic_info_t));

    topic_msg.qos    = IOTX_MQTT_QOS0;
    topic_msg.retain = 0;
    topic_msg.dup    = 0;
    memset(msg_pub, 0, sizeof(msg_pub));


    int msg_len = snprintf(msg_pub, MQTT_APP_MSG_LEN,ALINK_BODY_FORMAT, (unsigned int)cnt,
                          ALINK_METHOD_PROP_POST, (char*)pdata);
    if (msg_len < 0) {
        LOG("Error occur! Exit program");
        return -1;
    }

    topic_msg.payload     = (void *)msg_pub;
    topic_msg.payload_len = msg_len;

    rc = IOT_MQTT_Publish(gpclient, ALINK_TOPIC_PROP_POST, &topic_msg);
    if (rc < 0) {
        LOG("error occur when publish. %d", rc);
        return -1;
    }

    LOG("packet-id=%u, publish topic msg=%s", (uint32_t)rc, msg_pub);
    cnt++;

    return 0;
}



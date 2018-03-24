/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <netmgr.h>
#include <aos/yloop.h>
#include <hal/sensor.h>

#include <sys/time.h>
#include "aos/aos.h"
#include "aos/network.h"
#include "kvmgr.h"
#include "aos/uData.h"
#include "iot_import.h"
#include "iot_export.h"


#ifdef AOS_ATCMD
#include <atparser.h>
#endif


#define PRODUCT_KEY             "gM4V4jrB0no"
#define DEVICE_NAME             "zwj_mk3060"
#define DEVICE_SECRET           "XQ8rRJntoCmX890RrlBpa3EGBRVgiE7W"

#define TOPIC_UPDATE            "/"PRODUCT_KEY"/"DEVICE_NAME"/update"
#define TOPIC_ERROR             "/"PRODUCT_KEY"/"DEVICE_NAME"/update/error"
#define TOPIC_GET               "/"PRODUCT_KEY"/"DEVICE_NAME"/get"
#define TOPIC_DATA              "/"PRODUCT_KEY"/"DEVICE_NAME"/data"

#define GPS_PUBLISH_SIZE        (256)
#define GPS_MSG_LEN_MAX         (2048)

static void *pclientgps;
static int  is_demo_started = 0;
int         g_gpsmqttinitflag = 0;

static int      g_publishflag = 0;
static uint32_t g_skippubcnt = 0; 

static void test_gps_report_demo(input_event_t *event, void *priv_data);
static void test_gps_handle_mqtt(char *pwbuf, int blen, int argc, char **argv);
static struct cli_command gpstcmd = {
    .name = "gps",
    .help = "report gps",
    .function = test_gps_handle_mqtt
};

static void test_gps_event_handle_mqtt(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg)
{
    uintptr_t packet_id = (uintptr_t)msg->msg;
    iotx_mqtt_topic_info_pt topic_info = (iotx_mqtt_topic_info_pt)msg->msg;

    switch (msg->event_type) {
        case IOTX_MQTT_EVENT_UNDEF:
            LOG("undefined event occur.\n");
            break;

        case IOTX_MQTT_EVENT_DISCONNECT:
            LOG("MQTT disconnect.\n");
            break;

        case IOTX_MQTT_EVENT_RECONNECT:
            LOG("MQTT reconnect.\n");
            break;

        case IOTX_MQTT_EVENT_SUBCRIBE_SUCCESS:
            LOG("subscribe success, packet-id=%u\n", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_SUBCRIBE_TIMEOUT:
            printf("subscribe wait ack timeout, packet-id=%u\n", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_SUBCRIBE_NACK:
            LOG("subscribe nack, packet-id=%u\n", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_UNSUBCRIBE_SUCCESS:
            LOG("unsubscribe success, packet-id=%u\n", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_UNSUBCRIBE_TIMEOUT:
            LOG("unsubscribe timeout, packet-id=%u\n", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_UNSUBCRIBE_NACK:
            LOG("unsubscribe nack, packet-id=%u\n", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_PUBLISH_SUCCESS:
            LOG("publish success, packet-id=%u\n", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_PUBLISH_TIMEOUT:
            LOG("publish timeout, packet-id=%u\n", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_PUBLISH_NACK:
            LOG("publish nack, packet-id=%u\n", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_PUBLISH_RECVEIVED:
            LOG("topic message arrived but without any related handle: topic=%.*s, topic_msg=%.*s\n",
                          topic_info->topic_len,
                          topic_info->ptopic,
                          topic_info->payload_len,
                          topic_info->payload);
            break;

        default:
            LOG("Should NOT arrive here.\n");
            break;
    }
}


static int test_gps_mqtt_client_init(void)
{
    int ret;
    int rc = 0, cnt = 0;
    iotx_conn_info_pt pconn_info;
    iotx_mqtt_param_t mqtt_params;
    char *msg_buf = NULL, *msg_readbuf = NULL;

    if (NULL == (msg_buf = (char *)malloc(GPS_MSG_LEN_MAX))) {
        LOG("%s fail   line %d\n",__func__,__LINE__);
        rc = -1;
        goto do_exit;
    }

    if (NULL == (msg_readbuf = (char *)malloc(GPS_MSG_LEN_MAX))) {
        LOG("%s fail   line %d\n",__func__,__LINE__);
        rc = -1;
        goto do_exit;
    }

    if (0 != IOT_SetupConnInfo(PRODUCT_KEY, DEVICE_NAME, DEVICE_SECRET, (void **)&pconn_info)) {
        LOG("%s fail   line %d\n",__func__,__LINE__);
        rc = -1;
        goto do_exit;
    }

    memset(&mqtt_params, 0x0, sizeof(mqtt_params));

    mqtt_params.port = pconn_info->port;
    mqtt_params.host = pconn_info->host_name;
    mqtt_params.client_id = pconn_info->client_id;
    mqtt_params.username = pconn_info->username;
    mqtt_params.password = pconn_info->password;
    mqtt_params.pub_key = pconn_info->pub_key;

    mqtt_params.request_timeout_ms = 500;
    mqtt_params.clean_session = 0;
    mqtt_params.keepalive_interval_ms = 10000;
    mqtt_params.pread_buf = msg_readbuf;
    mqtt_params.read_buf_size = GPS_MSG_LEN_MAX;
    mqtt_params.pwrite_buf = msg_buf;
    mqtt_params.write_buf_size = GPS_MSG_LEN_MAX;

    mqtt_params.handle_event.h_fp = test_gps_event_handle_mqtt;
    mqtt_params.handle_event.pcontext = NULL;

    pclientgps = IOT_MQTT_Construct(&mqtt_params);
    if (NULL == pclientgps) {
        
        LOG("%s fail   line %d\n",__func__,__LINE__);
        rc = -1;
        goto do_exit;
    }

    aos_msleep(1000);
    
    g_gpsmqttinitflag = 1;
    LOG("test_gps_mqtt_client_init success\n");

    return 0;

do_exit:
    if (NULL != msg_buf) {
        free(msg_buf);
    }

    if (NULL != msg_readbuf) {
        free(msg_readbuf);
    }

    return rc;
}


static int test_gps_mqtt_publish(void* addr)
{
    int ret = 0;
    int i;
    iotx_mqtt_topic_info_t topic_msg;
    char buff[GPS_PUBLISH_SIZE];
    int  size = GPS_PUBLISH_SIZE;
    gps_data_t* pdata = (gps_data_t*)addr;

    if(0 == pdata){
        return -1;
    }
   
    ret = snprintf( buff, size, "{\n"
        "\"lat\":  %f,\n"
        "\"lon\":  %f,\n"
        "\"alt\":  %f\n"
        "}",
        pdata->lat, pdata->lon,pdata->elv);

    if((ret < 0) && (ret > size)){
        return -1;
    }
    
    topic_msg.qos = IOTX_MQTT_QOS0;
    topic_msg.retain = 0;
    topic_msg.dup = 0;
    topic_msg.payload = (void *)buff;
    topic_msg.payload_len = ret;
    ret = IOT_MQTT_Publish(pclientgps, TOPIC_DATA, &topic_msg);

    if (ret < 0) {
        LOG("%s fail   line %d\n",__func__,__LINE__);
        return -1;
    }
    LOG("%s sucsss>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> %d\n",__func__,__LINE__);

    return 0;
}

static void test_gps_report_demo(input_event_t *event, void *priv_data)
{
    int ret;
    int i;
    udata_pkg_t buf;
    
    if ((event == NULL)||(event->type != EV_UDATA)) {
        return;
    }
    
    if(event->code != CODE_UDATA_REPORT_PUBLISH){
        return;
    }
    
    ret = uData_report_publish(event,&buf);
    if((0 == ret) && (1 == g_gpsmqttinitflag)){
        gps_data_t* data = buf.payload;
        printf("uData::::::::::::::>type = (%d)\n", buf.type);
        printf("uData::::::::::::::>time= (%d:%d:%d.%d)\n", data->utc.hour,data->utc.min,data->utc.sec,data->utc.hsec);
        printf("uData::::::::::::::>lat = (%f )\n", data->lat);
        printf("uData::::::::::::::>lon = (%f )\n", data->lon);

        if(0 == g_publishflag)
        {
            g_publishflag = 1;
            ret = test_gps_mqtt_publish(data);
            g_publishflag = 0;
            if(0 != ret){
                LOG("%s fail   line %d\n",__func__,__LINE__);
                return;
            }
        }
        else{
            g_skippubcnt++;
            LOG("skip test_gps_mqtt_publish % times\n");
        }

    }

}

static void test_gps_handle_mqtt(char *pwbuf, int blen, int argc, char **argv)
{
    int ret;
    if (is_demo_started == 0){
        is_demo_started = 1;
        ret = test_gps_mqtt_client_init();
        if(0 != ret){
            LOG("%s fail   line %d\n",__func__,__LINE__);
            return;
        }
    }
}

static void test_gps_udata_service_event(input_event_t *event, void *priv_data) 
{
    int ret;

    if (event->type != EV_WIFI) {
        return;
    }

    if (event->code != CODE_WIFI_ON_GOT_IP) {
        return;
    }

    if (is_demo_started == 0) {
        is_demo_started = 1;
        ret = test_gps_mqtt_client_init();
        if(0 != ret){
            LOG("%s fail   line %d\n",__func__,__LINE__);
            return;
        }
    }
}


int application_start(int argc, char *argv[])
{
    int ret = 0;

#ifdef WITH_SAL
    ret = sal_init();
    if(0 != ret){
        LOG("%s fail   line %d\n",__func__,__LINE__);
        return -1;
    }
#endif

    ret = aos_register_event_filter(EV_WIFI, test_gps_udata_service_event, NULL);
    if(0 != ret){
        LOG("%s fail   line %d\n",__func__,__LINE__);
        return -1;
    }

    ret = aos_cli_register_command(&gpstcmd);
    if(0 != ret){
        LOG("%s fail   line %d\n",__func__,__LINE__);
        return -1;
    }

    ret = aos_register_event_filter(EV_UDATA, test_gps_report_demo, NULL);
    if(0 != ret){
        LOG("%s fail   line %d\n",__func__,__LINE__);
        return -1;
    }
    
    ret = uData_subscribe(UDATA_SERVICE_GPS);
    if(0 != ret){
        LOG("%s fail   line %d\n",__func__,__LINE__);
        return -1;
    }
    
    aos_loop_run();

    return 0;
}

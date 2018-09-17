/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "iot_import.h"
#include "iot_export.h"
#include "iot_export_mqtt.h"
#include "aos/log.h"
#include "aos/yloop.h"
#include "aos/network.h"
#include <netmgr.h>
#include <aos/kernel.h>
#include <k_err.h>
#include <netmgr.h>
#include <aos/cli.h>
#include <aos/cloud.h>

#include "hal/hal.h"
#include "sensor.h"

#ifdef LITTLEVGL_DISPLAY
#include "sensor_display.h"
#endif

#ifdef AOS_ATCMD
#include <atparser.h>
#endif


#define PRODUCT_KEY   "a1E31Zmhcxo"
#define DEVICE_NAME   "QSUvUO7V5lxwJsOHgyHc"
#define DEVICE_SECRET "O6iyf0lnZXJQEyHdyMGPASkEamb5cDEi"


#define ALINK_BODY_FORMAT \
    "{\"id\":\"%d\",\"version\":\"1.0\",\"method\":\"%s\",\"params\":%s}"
#define ALINK_TOPIC_PROP_POST \
    "/sys/" PRODUCT_KEY "/" DEVICE_NAME "/thing/event/property/post"
#define ALINK_TOPIC_PROP_POSTRSP \
    "/sys/" PRODUCT_KEY "/" DEVICE_NAME "/thing/event/property/post_reply"
#define ALINK_TOPIC_PROP_SET \
    "/sys/" PRODUCT_KEY "/" DEVICE_NAME "/thing/service/property/set"
#define ALINK_METHOD_PROP_POST "thing.event.property.post"

#define MSG_LEN_MAX (1024)
/*
* Please check below item which in feature self-definition from "https://linkdevelop.aliyun.com/"
*/
#define PROP_POST_FORMAT_ACC         "{\"Accelerometer\":{\"X\":%f,\"Y\":%f, \"Z\":%f}}"
#define PROP_POST_FORMAT_HUMITEMP    "{\"report_sensor\":{\"Humi\":%f,\"Temp\":%f}}"
#define PROP_SET_FORMAT_CMDLED       "\"cmd_led\":"

int        cnt           = 0;
static int is_subscribed = 0;

void *gpclient;
char  msg_pub[512];

static int             fd_acc  = -1;
static int             fd_als  = -1;
static int             fd_humi = -1;
static int             fd_temp = -1;
iotx_mqtt_topic_info_t topic_msg;
char                  *msg_buf = NULL, *msg_readbuf = NULL;


int mqtt_client_example(void);

static int sensor_all_open(void)
{
    int fd = -1;
    fd     = aos_open(dev_acc_path, O_RDWR);
    if (fd < 0) {
        printf("Off-line: aos_open %s fd = %d.\n", dev_acc_path, fd);
    }
    fd_acc = fd;

    fd = aos_open(dev_als_path, O_RDWR);
    if (fd < 0) {
        printf("Off-line: aos_open %s fd = %d.\n", dev_als_path, fd);
    }
    fd_als = fd;

    fd = aos_open(dev_humi_path, O_RDWR);
    if (fd < 0) {
        printf("Off-line: aos_open %s fd = %d.\n", dev_humi_path, fd);
    }
    fd_humi = fd;

    fd = aos_open(dev_temp_path, O_RDWR);
    if (fd < 0) {
        printf("Off-line: aos_open %s fd = %d.\n", dev_temp_path, fd);
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
    mqtt_client_example();
}

static void mqtt_publish(void *pclient)
{
    int      rc         = -1;
    char     param[256] = { 0 };
    int      x, y, z;
    uint32_t humi_data      = 0;
    uint32_t temp_data      = 0;
    uint64_t humi_timestamp = 0;
    uint64_t temp_timestamp = 0;
    float    acc_nkg[3]     = { 0 };

    if (is_subscribed == 0) {
        /* Subscribe the specific topic */
        rc = IOT_MQTT_Subscribe(pclient, ALINK_TOPIC_PROP_POSTRSP,
                                IOTX_MQTT_QOS0, handle_prop_postrsp, NULL);
        if (rc < 0) {
            // IOT_MQTT_Destroy(&pclient);
            LOG("IOT_MQTT_Subscribe() failed, rc = %d", rc);
        }

		/* Subscribe the specific topic */
        rc = IOT_MQTT_Subscribe(pclient, ALINK_TOPIC_PROP_SET,
                                 IOTX_MQTT_QOS0, handle_prop_set, NULL);
        if (rc < 0) {
            // IOT_MQTT_Destroy(&pclient);
            LOG("IOT_MQTT_Subscribe() failed, rc = %d", rc);
        }
        is_subscribed = 1;
    } else {
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
    }

    if (++cnt < 20000) {
        aos_post_delayed_action(1000, mqtt_publish, pclient);
    } else {
        IOT_MQTT_Unsubscribe(pclient, ALINK_TOPIC_PROP_POSTRSP);
        aos_msleep(200);
        IOT_MQTT_Destroy(&pclient);
        release_buff();
        is_subscribed = 0;
        cnt           = 0;
    }
}

static void mqtt_service_event(input_event_t *event, void *priv_data)
{

    if (event->type != EV_SYS) {
        return;
    }

    if (event->code != CODE_SYS_ON_MQTT_READ) {
        return;
    }
    LOG("mqtt_service_event!");
    mqtt_publish(priv_data);
}

void event_handle_mqtt(void *pcontext, void *pclient,
                       iotx_mqtt_event_msg_pt msg)
{
    uintptr_t               packet_id  = (uintptr_t)msg->msg;
    iotx_mqtt_topic_info_pt topic_info = (iotx_mqtt_topic_info_pt)msg->msg;

    switch (msg->event_type) {
        case IOTX_MQTT_EVENT_UNDEF:
            LOG("undefined event occur.");
            break;

        case IOTX_MQTT_EVENT_DISCONNECT:
            LOG("MQTT disconnect.");
            break;

        case IOTX_MQTT_EVENT_RECONNECT:
            LOG("MQTT reconnect.");
            break;

        case IOTX_MQTT_EVENT_SUBCRIBE_SUCCESS:
            LOG("subscribe success, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_SUBCRIBE_TIMEOUT:
            LOG("subscribe wait ack timeout, packet-id=%u",
                (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_SUBCRIBE_NACK:
            LOG("subscribe nack, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_UNSUBCRIBE_SUCCESS:
            LOG("unsubscribe success, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_UNSUBCRIBE_TIMEOUT:
            LOG("unsubscribe timeout, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_UNSUBCRIBE_NACK:
            LOG("unsubscribe nack, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_PUBLISH_SUCCESS:
            LOG("publish success, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_PUBLISH_TIMEOUT:
            LOG("publish timeout, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_PUBLISH_NACK:
            LOG("publish nack, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_PUBLISH_RECVEIVED:
            LOG("topic message arrived but without any related handle: "
                "topic=%.*s, topic_msg=%.*s",
                topic_info->topic_len, topic_info->ptopic,
                topic_info->payload_len, topic_info->payload);
            break;

        default:
            LOG("Should NOT arrive here.");
            break;
    }
}

void release_buff()
{
    if (NULL != msg_buf) {
        aos_free(msg_buf);
    }

    if (NULL != msg_readbuf) {
        aos_free(msg_readbuf);
    }
}

int mqtt_client_example(void)
{
    int               rc = 0;
    iotx_conn_info_pt pconn_info;
    iotx_mqtt_param_t mqtt_params;

    if (msg_buf != NULL) {
        return rc;
    }

    if (NULL == (msg_buf = (char *)aos_malloc(MSG_LEN_MAX))) {
        LOG("not enough memory");
        rc = -1;
        release_buff();
        return rc;
    }

    if (NULL == (msg_readbuf = (char *)aos_malloc(MSG_LEN_MAX))) {
        LOG("not enough memory");
        rc = -1;
        release_buff();
        return rc;
    }

    /* Device AUTH */
    if (0 != IOT_SetupConnInfo(PRODUCT_KEY, DEVICE_NAME, DEVICE_SECRET,
                               (void **)&pconn_info)) {
        LOG("AUTH request failed!");
        rc = -1;
        release_buff();
        return rc;
    }

    /* Initialize MQTT parameter */
    memset(&mqtt_params, 0x0, sizeof(mqtt_params));
    mqtt_params.port                  = pconn_info->port;
    mqtt_params.host                  = pconn_info->host_name;
    mqtt_params.client_id             = pconn_info->client_id;
    mqtt_params.username              = pconn_info->username;
    mqtt_params.password              = pconn_info->password;
    mqtt_params.pub_key               = pconn_info->pub_key;
    mqtt_params.request_timeout_ms    = 2000;
    mqtt_params.clean_session         = 0;
    mqtt_params.keepalive_interval_ms = 60000;
    mqtt_params.pread_buf             = msg_readbuf;
    mqtt_params.read_buf_size         = MSG_LEN_MAX;
    mqtt_params.pwrite_buf            = msg_buf;
    mqtt_params.write_buf_size        = MSG_LEN_MAX;
    mqtt_params.handle_event.h_fp     = event_handle_mqtt;
    mqtt_params.handle_event.pcontext = NULL;

    /* Construct a MQTT client with specify parameter */
    gpclient = IOT_MQTT_Construct(&mqtt_params);
    if (NULL == gpclient) {
        LOG("MQTT construct failed");
        rc = -1;
        release_buff();
        // aos_unregister_event_filter(EV_SYS,  mqtt_service_event, gpclient);
    } else {
        aos_register_event_filter(EV_SYS, mqtt_service_event, gpclient);
    }

    return rc;
}

static void handle_mqtt(char *pwbuf, int blen, int argc, char **argv)
{
    mqtt_client_example();
}

static struct cli_command mqttcmd = { .name     = "mqtt",
                                      .help     = "factory mqtt",
                                      .function = handle_mqtt };

int application_start(int argc, char *argv[])
{
    netmgr_ap_config_t apconfig;

#if AOS_ATCMD
    at.set_mode(ASYN);
    at.init(AT_RECV_PREFIX, AT_RECV_SUCCESS_POSTFIX, AT_RECV_FAIL_POSTFIX,
            AT_SEND_DELIMITER, 1000);
#endif

#ifdef WITH_SAL
    sal_init();
#endif

    printf("== Build on: %s %s ===\n", __DATE__, __TIME__);
    aos_set_log_level(AOS_LL_DEBUG);
    sensor_all_open();
    aos_register_event_filter(EV_WIFI, wifi_service_event, NULL);

    netmgr_init();
#if 0
    memset(&apconfig, 0, sizeof(apconfig));
    strcpy(apconfig.ssid, "LinkDevelop-Workshop");
    strcpy(apconfig.pwd, "linkdevelop");
    netmgr_set_ap_config(&apconfig);
#endif
    netmgr_start(false);

    aos_cli_register_command(&mqttcmd);
#ifdef CSP_LINUXHOST
    mqtt_client_example();
#endif

#ifdef LITTLEVGL_DISPLAY
    sensor_display_init();
#endif

    aos_loop_run();

    return 0;
}

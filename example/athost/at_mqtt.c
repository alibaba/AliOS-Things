/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <aos/aos.h>
#include <atparser.h>
#include <aos/network.h>
#include <hal/wifi.h>

#include "iot_import.h"
#include "iot_export.h"
#include "iot_export_mqtt.h"
#include "athost.h"
#include "at_util.h"

#define TAG "atmqtt"

#define MQTT_DEFAULT_TLS_MODE 0
#define MQTT_DEFAULT_REQ_TIMOUT_MS 2000
#define MQTT_DEFAULT_CLEAN_SESSION 0
#define MQTT_DEFAULT_KEEPALIVE_MS 60000
#define MQTT_DEFAULT_VERSION "1.0.0"
#define MQTT_DEFAULT_AUTH_MODE 1

#define MQTT_DEFAULT_TOPIC_UPDATE "/" PRODUCT_KEY "/" DEVICE_NAME "/update"
#define MQTT_DEFAULT_TOPIC_ERROR "/" PRODUCT_KEY "/" DEVICE_NAME "/update/error"
#define MQTT_DEFAULT_TOPIC_GET "/" PRODUCT_KEY "/" DEVICE_NAME "/get"

#define MQTT_PRODUCT_KEY_MAX_LEN 50
#define MQTT_DEVICE_NAME_MAX_LEN 50
#define MQTT_DEVICE_SECRET_MAX_LEN 50
#define MQTT_VERSION_MAX_LEN 10
#define MQTT_MSG_MAX_LEN 1024
#define MQTT_TOPIC_MAX_LEN 200

enum
{
    CME_ERROR_OP_NOT_ALLOWED       = 3,
    CME_ERROR_OP_NOT_SUPPORT       = 4,
    CME_ERROR_TXT_STR_TOO_LONG     = 24,
    CME_ERROR_TXT_STR_INVALID_CHAR = 25,
    CME_ERROR_NET_TIMEOUT          = 31,
    CME_ERROR_INCORRECT_PARA       = 50,
    CME_ERROR_UNKNOWN              = 100,
};

typedef struct
{
    int  mode; // 0: non-TLS 1: TLS
    char hostname[MAX_ATCMD_DOMAIN_LEN];
    int  port;

    char productkey[MQTT_PRODUCT_KEY_MAX_LEN];
    char devicename[MQTT_DEVICE_NAME_MAX_LEN];
    char devicesecret[MQTT_DEVICE_SECRET_MAX_LEN];

    uint32_t timeout;   // sec
    uint32_t keepalive; // sec
    int      clean;     // 0: not clean session 1: clean session
    char     version[MQTT_VERSION_MAX_LEN];
    int      authmode; // 0: only set para without auth 1: set with auth
} mqtt_config_t;

typedef struct
{
    bool valid;
    int  packet_id; // used by recvpub

    int qos;        // used by pub
    int msg_format; // 0: ACII 1: Hex
    int fragment_id;

    char topic_pub[MQTT_TOPIC_MAX_LEN];
    char msg_pub[MQTT_MSG_MAX_LEN];
} mqtt_pub_msg_t;

typedef struct
{
    bool valid;
    char topic[MQTT_TOPIC_MAX_LEN];
    int  qos;
} mqtt_sub_topic_t;

static mqtt_config_t     m_mqtt_config;
static iotx_conn_info_pt m_mqtt_conn_info = NULL;
static void *            m_pclient        = NULL;

static char *m_writebuf = NULL;
static char *m_readbuf  = NULL;

static mqtt_pub_msg_t   m_last_pub_msg;
static mqtt_pub_msg_t   m_last_recvpub_msg;
static mqtt_sub_topic_t m_last_sub_topic;
static mqtt_sub_topic_t m_last_unsub_topic;

static mqtt_pub_msg_t   m_attempt_pub_msg;
static mqtt_sub_topic_t m_attempt_sub_topic;
static mqtt_sub_topic_t m_attempt_unsub_topic;

#define MAX_ATCMD_PUB_RESULT_RSP_LEN 25
static int notify_mqtt_pub_result(int packet_id, int result)
{
    const char *pub_result_preifx                      = "+IMQTTPUB:";
    int         offset                                 = 0;
    char        response[MAX_ATCMD_PUB_RESULT_RSP_LEN] = { 0 };

    if (result < 0 || result > 2) {
        LOGE(TAG, "invalid pub result %d\n", result);
        goto err;
    }

    // prefix
    if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_PUB_RESULT_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_PUB_RESULT_RSP_LEN - offset,
                   "%s", AT_RECV_PREFIX);
    } else {
        LOGE(TAG, "at string too long %s\n", response);
        goto err;
    }

    // result
    if (offset + strlen(pub_result_preifx) + 5 + 1 + 1 + 2 <
        MAX_ATCMD_PUB_RESULT_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_PUB_RESULT_RSP_LEN - offset,
                   "%s%d,%d\r\n", pub_result_preifx, packet_id, result);
    } else {
        LOGE(TAG, "at string too long %s\n", response);
        goto err;
    }

    if (insert_uart_send_msg(response, NULL, strlen(response), 0) != 0) {
        LOGE(TAG, "Error insert uart send msg fail\r\n");
        goto err;
    }

    return 0;
err:
    return -1;
}

#define MAX_ATCMD_SUB_RESULT_RSP_LEN 25
static int notify_mqtt_sub_result(int packet_id, int result)
{
    const char *sub_result_preifx                      = "+IMQTTSUB:";
    int         offset                                 = 0;
    char        response[MAX_ATCMD_SUB_RESULT_RSP_LEN] = { 0 };

    if ((result < 0 || result > 2) && result != 128) {
        LOGE(TAG, "invalid sub result %d\n", result);
        goto err;
    }

    // prefix
    if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_SUB_RESULT_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_SUB_RESULT_RSP_LEN - offset,
                   "%s", AT_RECV_PREFIX);
    } else {
        LOGE(TAG, "at string too long %s\n", response);
        goto err;
    }

    // result
    if (offset + strlen(sub_result_preifx) + 5 + 1 + 3 + 2 <
        MAX_ATCMD_SUB_RESULT_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_SUB_RESULT_RSP_LEN - offset,
                   "%s%d,%d\r\n", sub_result_preifx, packet_id, result);
    } else {
        LOGE(TAG, "at string too long %s\n", response);
        goto err;
    }

    if (insert_uart_send_msg(response, NULL, strlen(response), 0) != 0) {
        LOGE(TAG, "Error insert uart send msg fail\r\n");
        goto err;
    }

    return 0;
err:
    return -1;
}

#define MAX_ATCMD_UNSUB_RESULT_RSP_LEN 25
static int notify_mqtt_unsub_result(int packet_id, int result)
{
    const char *unsub_result_preifx                      = "+IMQTTUNSUB:";
    int         offset                                   = 0;
    char        response[MAX_ATCMD_UNSUB_RESULT_RSP_LEN] = { 0 };

    if (result < 0 || result > 2) {
        LOGE(TAG, "invalid unsub result %d\n", result);
        goto err;
    }

    // prefix
    if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_UNSUB_RESULT_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_UNSUB_RESULT_RSP_LEN - offset,
                   "%s", AT_RECV_PREFIX);
    } else {
        LOGE(TAG, "at string too long %s\n", response);
        goto err;
    }

    // result
    if (offset + strlen(unsub_result_preifx) + 5 + 1 + 1 + 2 <
        MAX_ATCMD_UNSUB_RESULT_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_UNSUB_RESULT_RSP_LEN - offset,
                   "%s%d,%d\r\n", unsub_result_preifx, packet_id, result);
    } else {
        LOGE(TAG, "at string too long %s\n", response);
        goto err;
    }

    if (insert_uart_send_msg(response, NULL, strlen(response), 0) != 0) {
        LOGE(TAG, "Error insert uart send msg fail\r\n");
        goto err;
    }

    return 0;
err:
    return -1;
}

#define MAX_ATCMD_MQTT_STATE_RSP_LEN 25
static int notify_mqtt_state(int state)
{
    const char *mqtt_state_preifx                      = "+IMQTTSTATE:";
    int         offset                                 = 0;
    char        response[MAX_ATCMD_MQTT_STATE_RSP_LEN] = { 0 };

    if (state < 0 || state > 2) {
        LOGE(TAG, "invalid mqtt state %d\n", state);
        goto err;
    }

    // prefix
    if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_MQTT_STATE_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_STATE_RSP_LEN - offset,
                   "%s", AT_RECV_PREFIX);
    } else {
        LOGE(TAG, "at string too long %s\n", response);
        goto err;
    }

    // state
    if (offset + strlen(mqtt_state_preifx) + 1 < MAX_ATCMD_MQTT_STATE_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_STATE_RSP_LEN - offset,
                   "%s%d", mqtt_state_preifx, state);
    } else {
        LOGE(TAG, "at string too long %s\n", response);
        goto err;
    }

    // AT_RECV_PREFIX
    if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_MQTT_STATE_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_STATE_RSP_LEN - offset,
                   "%s", AT_RECV_PREFIX);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
             response);
        goto err;
    }

    // AT_RECV_SUCCESS_POSTFIX
    if (offset + strlen(AT_RECV_SUCCESS_POSTFIX) <
        MAX_ATCMD_MQTT_STATE_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_STATE_RSP_LEN - offset,
                   "%s", AT_RECV_SUCCESS_POSTFIX);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
             response);
        goto err;
    }

    if (insert_uart_send_msg(response, NULL, strlen(response), 0) != 0) {
        LOGE(TAG, "Error insert uart send msg fail\r\n");
        goto err;
    }

    return 0;
err:
    return -1;
}

enum
{
    MQTT_AUTH_FAIL    = 0,
    MQTT_AUTH_SUCCESS = 1,

    // 0: disconnected 1: reserved 2: connected
    MQTT_STATE_DISCON = 0,
    MQTT_STATE_RESERV = 1,
    MQTT_STATE_CONNEC = 2,

    MQTT_SUB_SUCCESS_QOS0 = 0,
    MQTT_SUB_SUCCESS_QOS1 = 1,
    MQTT_SUB_SUCCESS_QOS2 = 2,
    MQTT_SUB_FAIL         = 128,

    MQTT_UNSUB_SUCCESS   = 0,
    MQTT_UNSUB_TIMEOUT   = 1,
    MQTT_UNSUB_OTHER_ERR = 2,

    MQTT_PUB_SUCCESS   = 0,
    MQTT_PUB_TIMEOUT   = 1,
    MQTT_PUB_OTHER_ERR = 2,
};

static void mqtt_handle_event(void *pcontext, void *pclient,
                              iotx_mqtt_event_msg_pt pmsg)
{
    iotx_mqtt_topic_info_pt topic_info;
    unsigned int            packet_id;

    if (NULL == pmsg) {
        LOGE(TAG, "mqtt handle event error NULL");
    }

    switch (pmsg->event_type) {
        case IOTX_MQTT_EVENT_UNDEF:
            LOG("undefined event occur.");
            break;

        case IOTX_MQTT_EVENT_DISCONNECT:
            LOG("MQTT disconnect.");
            notify_mqtt_state(MQTT_STATE_DISCON);
            break;

        case IOTX_MQTT_EVENT_RECONNECT:
            LOG("MQTT reconnect.");
            notify_mqtt_state(MQTT_STATE_CONNEC);
            break;

        case IOTX_MQTT_EVENT_SUBCRIBE_SUCCESS:
            packet_id = (unsigned int)pmsg->msg;

            // TODO: QoS value pending
            if (m_last_sub_topic.valid) {
                notify_mqtt_sub_result(packet_id, m_last_sub_topic.qos);
                LOG("subscribe success, packet-id=%u", packet_id);
            } else {
                LOGE(TAG, "Invalid subsribe topic!\n", m_last_sub_topic);
            }
            break;

        case IOTX_MQTT_EVENT_SUBCRIBE_TIMEOUT:
            packet_id = (unsigned int)pmsg->msg;

            notify_mqtt_sub_result(packet_id, MQTT_SUB_FAIL);
            LOG("subscribe wait ack timeout, packet-id=%u", packet_id);
            break;

        case IOTX_MQTT_EVENT_SUBCRIBE_NACK:
            packet_id = (unsigned int)pmsg->msg;

            notify_mqtt_sub_result(packet_id, MQTT_SUB_FAIL);
            LOG("subscribe nack, packet-id=%u", packet_id);
            break;

        case IOTX_MQTT_EVENT_UNSUBCRIBE_SUCCESS:
            packet_id = (unsigned int)pmsg->msg;

            notify_mqtt_unsub_result(packet_id, MQTT_UNSUB_SUCCESS);
            LOG("unsubscribe success, packet-id=%u", packet_id);
            break;

        case IOTX_MQTT_EVENT_UNSUBCRIBE_TIMEOUT:
            packet_id = (unsigned int)pmsg->msg;

            notify_mqtt_unsub_result(packet_id, MQTT_UNSUB_TIMEOUT);
            LOG("unsubscribe timeout, packet-id=%u", packet_id);
            break;

        case IOTX_MQTT_EVENT_UNSUBCRIBE_NACK:
            packet_id = (unsigned int)pmsg->msg;

            notify_mqtt_unsub_result(packet_id, MQTT_UNSUB_OTHER_ERR);
            LOG("unsubscribe nack, packet-id=%u", packet_id);
            break;

        case IOTX_MQTT_EVENT_PUBLISH_SUCCESS:
            packet_id = (unsigned int)pmsg->msg;

            notify_mqtt_pub_result(packet_id, MQTT_PUB_SUCCESS);
            LOG("publish success, packet-id=%u", packet_id);
            break;

        case IOTX_MQTT_EVENT_PUBLISH_TIMEOUT:
            packet_id = (unsigned int)pmsg->msg;

            notify_mqtt_pub_result(packet_id, MQTT_PUB_TIMEOUT);
            LOG("publish timeout, packet-id=%u", packet_id);
            break;

        case IOTX_MQTT_EVENT_PUBLISH_NACK:
            packet_id = (unsigned int)pmsg->msg;

            notify_mqtt_pub_result(packet_id, MQTT_PUB_OTHER_ERR);
            LOG("publish nack, packet-id=%u", packet_id);
            break;

        case IOTX_MQTT_EVENT_PUBLISH_RECVEIVED:
            topic_info = (iotx_mqtt_topic_info_pt)pmsg->msg;

            LOG("topic message arrived but without any related handle: "
                "topic=%.*s, topic_msg=%.*s ",
                topic_info->topic_len, topic_info->ptopic,
                topic_info->payload_len, topic_info->payload);

            printf("\nqos=%d\n", topic_info->qos);
            break;

        default:
            LOGE(TAG, "MQTT even handle: should NOT arrive here.");
            break;
    }
}

#define MAX_ATCMD_MQTT_RCVPUB_RSP_LEN \
    (20 + 10 + MQTT_MSG_MAX_LEN + 5 + MQTT_TOPIC_MAX_LEN + 1 + 5 + 5)
static int notify_mqtt_rcvpub_msg()
{
    const char *mqtt_rcvpub_prefix                      = "+IMQTTRCVPUB:";
    char        response[MAX_ATCMD_MQTT_RCVPUB_RSP_LEN] = { 0 };
    int         ret;
    int         offset = 0;

    // AT_RECV_PREFIX
    if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_MQTT_RCVPUB_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_RCVPUB_RSP_LEN - offset,
                   "%s", AT_RECV_PREFIX);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
             response);
        goto err;
    }

    // prefix
    if (offset + strlen(mqtt_rcvpub_prefix) < MAX_ATCMD_MQTT_RCVPUB_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_RCVPUB_RSP_LEN - offset,
                   "%s", mqtt_rcvpub_prefix);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
             response);
        goto err;
    }

    // PACKETID + TOPIC + MSG_LEN + MESSAGE + [FORMAT] + [FRAGMENT_ID]
    if (offset + 10 + strlen(m_last_recvpub_msg.topic_pub) + 2 + 5 +
          strlen(m_last_pub_msg.msg_pub) + 2 + 1 + 5 + 5 <
        MAX_ATCMD_MQTT_RCVPUB_RSP_LEN) {
        if (m_last_recvpub_msg.valid) {
            offset += snprintf(
              response + offset, MAX_ATCMD_MQTT_RCVPUB_RSP_LEN - offset,
              "%d,\"%s\",%d,\"%s\",%d,%d", m_last_recvpub_msg.packet_id,
              m_last_recvpub_msg.topic_pub, strlen(m_last_recvpub_msg.msg_pub),
              m_last_recvpub_msg.msg_pub, m_last_recvpub_msg.msg_format,
              m_last_recvpub_msg.fragment_id);
        } else {
            LOGE(TAG, "last recvpub msg invalid!\n");
            goto err;
        }
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
             response);
        goto err;
    }

    // AT_RECV_PREFIX
    if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_MQTT_RCVPUB_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_RCVPUB_RSP_LEN - offset,
                   "%s", AT_RECV_PREFIX);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
             response);
        goto err;
    }

    // AT_RECV_SUCCESS_POSTFIX
    if (offset + strlen(AT_RECV_SUCCESS_POSTFIX) <
        MAX_ATCMD_MQTT_RCVPUB_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_RCVPUB_RSP_LEN - offset,
                   "%s", AT_RECV_SUCCESS_POSTFIX);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
             response);
        goto err;
    }

    if (insert_uart_send_msg(response, NULL, strlen(response), 0) != 0) {
        LOGE(TAG, "%s %d insert uart send msg fail\r\n", __func__, __LINE__);
        goto err;
    }

    return 0;

err:
    return -1;
}

static void subscribed_msg_arrive_handle(void *pcontext, void *pclient,
                                         iotx_mqtt_event_msg_pt pmsg)
{
    iotx_mqtt_topic_info_pt ptopic_info = (iotx_mqtt_topic_info_pt)pmsg->msg;

    if (NULL == ptopic_info) {
        LOGE(TAG, "invalid topic info ptr\n");
        return;
    }

    if (ptopic_info->topic_len > MQTT_TOPIC_MAX_LEN ||
        ptopic_info->payload_len > MQTT_MSG_MAX_LEN) {
        LOGE(TAG, "topic len %d msg len %d exceed max limit!",
             ptopic_info->topic_len, ptopic_info->payload_len);
        return;
    }

    memset(&m_last_recvpub_msg, 0, sizeof(m_last_recvpub_msg));
    m_last_recvpub_msg.packet_id = ptopic_info->packet_id;
    memcpy(m_last_recvpub_msg.topic_pub, ptopic_info->ptopic,
           ptopic_info->topic_len);
    memcpy(m_last_recvpub_msg.msg_pub, ptopic_info->payload,
           ptopic_info->payload_len);
    m_last_recvpub_msg.msg_format  = 0;
    m_last_recvpub_msg.fragment_id = 0;
    m_last_recvpub_msg.valid       = true;

    LOG("----");
    LOG("Topic: '%.*s' (Length: %d)", ptopic_info->topic_len,
        ptopic_info->ptopic, ptopic_info->topic_len);
    LOG("Payload: '%.*s' (Length: %d)", ptopic_info->payload_len,
        ptopic_info->payload, ptopic_info->payload_len);
    LOG("----");

    // notfiy
    notify_mqtt_rcvpub_msg();

    return;
}

static int mqtt_subscribe(mqtt_sub_topic_t *sub_topic)
{
    int ret = 0;

    if (NULL == sub_topic) {
        return -1;
    }

    /* Subscribe the specific topic */
    ret = IOT_MQTT_Subscribe(m_pclient, sub_topic->topic, sub_topic->qos,
                             subscribed_msg_arrive_handle, NULL);
    if (ret >= 0) {
        memcpy(&m_last_sub_topic, sub_topic, sizeof(mqtt_sub_topic_t));
    }

    return ret;
}

static int mqtt_unsubscribe(mqtt_sub_topic_t *unsub_topic)
{
    int ret = 0;

    if (NULL == unsub_topic) {
        return -1;
    }

    ret = IOT_MQTT_Unsubscribe(m_pclient, unsub_topic->topic);
    if (ret >= 0) {
        memcpy(&m_last_unsub_topic, unsub_topic, sizeof(m_last_unsub_topic));
    }

    return ret;
}

static int mqtt_publish(mqtt_pub_msg_t *pub_msg)
{
    int                    ret = 0;
    iotx_mqtt_topic_info_t topic_msg;

    if (NULL == pub_msg) {
        return -1;
    }

    memset(&topic_msg, 0, sizeof(topic_msg));
    topic_msg.qos         = pub_msg->qos;
    topic_msg.payload     = (void *)pub_msg->msg_pub;
    topic_msg.payload_len = strlen(pub_msg->msg_pub);

    ret = IOT_MQTT_Publish(m_pclient, pub_msg->topic_pub, &topic_msg);
    if (ret >= 0) {
        memcpy(&m_last_pub_msg, pub_msg, sizeof(mqtt_pub_msg_t));
    }

    return ret;
}

static void mqtt_service_event(input_event_t *event, void *priv_data)
{
    if (event->type != EV_SYS) {
        return;
    }

    if (event->code != CODE_SYS_ON_MQTT_READ) {
        return;
    }

    LOGD(TAG, "mqtt_service_event!");
    // ready to subsribe
    // mqtt_publis()
}

#define MAX_ATCMD_AUTH_RESULT_RSP_LEN 20
static int notify_mqtt_auth_result(int result)
{
    const char *auth_result_preifx = "+IMQTTAUTH:";
    char *      auth_result;
    int         offset                                  = 0;
    char        response[MAX_ATCMD_AUTH_RESULT_RSP_LEN] = { 0 };

    if (result == MQTT_AUTH_FAIL) {
        auth_result = "FAIL";
    } else if (result == MQTT_AUTH_SUCCESS) {
        auth_result = "OK";
    } else {
        LOGE(TAG, "invalid auth result %d\n", result);
        goto err;
    }

    // prefix
    if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_AUTH_RESULT_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_AUTH_RESULT_RSP_LEN - offset,
                   "%s", AT_RECV_PREFIX);
    } else {
        LOGE(TAG, "at string too long %s\n", response);
        goto err;
    }

    // result
    if (offset + strlen(auth_result_preifx) + strlen(auth_result) + 2 <
        MAX_ATCMD_AUTH_RESULT_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_AUTH_RESULT_RSP_LEN - offset,
                   "%s%s\r\n", auth_result_preifx, auth_result);
    } else {
        LOGE(TAG, "at string too long %s\n", response);
        goto err;
    }

    if (insert_uart_send_msg(response, NULL, strlen(response), 0) != 0) {
        LOGE(TAG, "Error insert uart send msg fail\r\n");
        goto err;
    }

    return 0;
err:
    return -1;
}

static int mqtt_authenticate()
{
    int ret    = 0;
    int notify = MQTT_AUTH_SUCCESS;

    if (0 != IOT_SetupConnInfo(
               m_mqtt_config.productkey, m_mqtt_config.devicename,
               m_mqtt_config.devicesecret, (void **)&m_mqtt_conn_info)) {
        LOGE(TAG, "authenticate request failed!");
        ret = -1;
    }

    if (ret != 0)
        notify = MQTT_AUTH_FAIL;

    notify_mqtt_auth_result(notify);

    return ret;
}

static int post_mqtt_authenticate_task()
{
    if (aos_task_new("mqtt_authenticate_task", mqtt_authenticate, NULL, 2048) !=
        0) {
        LOGE(TAG, "Fail to create mqtt authenticate task\r\n");
        return -1;
    }

    return 0;
}

static void release_buff()
{
    aos_free(m_writebuf);
    m_writebuf = NULL;

    aos_free(m_readbuf);
    m_readbuf = NULL;
}

static int mqtt_connect()
{
    iotx_mqtt_param_t mqtt_params;

    if (m_mqtt_conn_info == NULL) {
        LOGE(TAG, "Conn info not ready yet!\n");
        goto err;
    }

    if (m_writebuf != NULL) {
        LOGE(TAG, "m_writebuf has been allocated\n");
        goto err;
    }

    if ((NULL == (m_writebuf = (char *)aos_malloc(MQTT_MSG_MAX_LEN))) ||
        (NULL == (m_readbuf = (char *)aos_malloc(MQTT_MSG_MAX_LEN)))) {
        LOGE(TAG, "not enough memory");
        goto err;
    }

    /* Initialize MQTT parameter */
    memset(&mqtt_params, 0, sizeof(mqtt_params));
    mqtt_params.port      = m_mqtt_conn_info->port;
    mqtt_params.host      = m_mqtt_conn_info->host_name;
    mqtt_params.client_id = m_mqtt_conn_info->client_id;
    mqtt_params.username  = m_mqtt_conn_info->username;
    mqtt_params.password  = m_mqtt_conn_info->password;
    mqtt_params.pub_key   = m_mqtt_conn_info->pub_key;

    mqtt_params.request_timeout_ms    = m_mqtt_config.timeout;
    mqtt_params.clean_session         = m_mqtt_config.clean;
    mqtt_params.keepalive_interval_ms = m_mqtt_config.keepalive;

    mqtt_params.pread_buf         = m_readbuf;
    mqtt_params.read_buf_size     = MQTT_MSG_MAX_LEN;
    mqtt_params.pwrite_buf        = m_writebuf;
    mqtt_params.write_buf_size    = MQTT_MSG_MAX_LEN;
    mqtt_params.handle_event.h_fp = mqtt_handle_event;

    mqtt_params.handle_event.pcontext = NULL;

    /* Construct a MQTT client with specify parameter */
    if ((m_pclient = IOT_MQTT_Construct(&mqtt_params)) == NULL) {
        LOGE(TAG, "MQTT construct failed!\n");
        goto err;
    }

    aos_register_event_filter(EV_SYS, mqtt_service_event, m_pclient);

    return 0;

err:
    release_buff();
    return -1;
}

static int post_mqtt_connect_task()
{
    if (aos_task_new("mqtt_connect_task", mqtt_connect, NULL, 5120) != 0) {
        LOGE(TAG, "Fail to create mqtt connect task\r\n");
        return -1;
    }

    return 0;
}

#define MAX_ATCMD_MQTT_MODE_RSP_LEN 30
static int atcmd_imqtt_mode()
{
    const char *mqtt_mode_prefix = "+IMQTTMODE:";
    char        single, mode;
    char        response[MAX_ATCMD_MQTT_MODE_RSP_LEN] = { 0 };
    int         ret;
    int         offset = 0;

    at.parse(&single, 1);
    if ('=' != single && '?' != single) {
        LOGE(TAG, "Invalid mqtt mode prefix %c!\n", single);
        goto err;
    }

    // AT_RECV_PREFIX
    if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_MQTT_MODE_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_MODE_RSP_LEN - offset,
                   "%s", AT_RECV_PREFIX);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
             response);
        goto err;
    }

    if ('?' == single) {
        mode = m_mqtt_config.mode ? '1' : '0';

        // MQTTMODE prefix + mode
        if (offset + strlen(mqtt_mode_prefix) + 1 <
            MAX_ATCMD_MQTT_MODE_RSP_LEN) {
            offset +=
              snprintf(response + offset, MAX_ATCMD_MQTT_MODE_RSP_LEN - offset,
                       "%s%c", mqtt_mode_prefix, mode);
        } else {
            LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                 response);
            goto err;
        }

        // AT_RECV_PREFIX
        if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_MQTT_MODE_RSP_LEN) {
            offset +=
              snprintf(response + offset, MAX_ATCMD_MQTT_MODE_RSP_LEN - offset,
                       "%s", AT_RECV_PREFIX);
        } else {
            LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                 response);
            goto err;
        }
    } else if ('=' == single) {
        at.parse(&mode, 1);

        if ('?' == mode) {
            // MQTTMODE prefix + \"mode\"
            if (offset + strlen(mqtt_mode_prefix) + 6 <
                MAX_ATCMD_MQTT_MODE_RSP_LEN) {
                offset += snprintf(response + offset,
                                   MAX_ATCMD_MQTT_MODE_RSP_LEN - offset,
                                   "%s\"mode\"", mqtt_mode_prefix);
            } else {
                LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                     response);
                goto err;
            }

            if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_MQTT_MODE_RSP_LEN) {
                offset += snprintf(response + offset,
                                   MAX_ATCMD_MQTT_MODE_RSP_LEN - offset, "%s",
                                   AT_RECV_PREFIX);
            } else {
                LOGE(TAG, "at string too long %s\n", response);
                goto err;
            }
        } else if ('0' == mode || '1' == mode) {
            m_mqtt_config.mode = mode - '0';
        } else {
            LOGE(TAG, "invalid mqtt mode %c\n", mode);
            goto err;
        }
    } else {
        LOGE(TAG, "Invalid mqtt mode prefix %c!\n", single);
        goto err;
    }

    // AT_RECV_SUCCESS_POSTFIX
    if (offset + strlen(AT_RECV_SUCCESS_POSTFIX) <
        MAX_ATCMD_MQTT_MODE_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_MODE_RSP_LEN - offset,
                   "%s", AT_RECV_SUCCESS_POSTFIX);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
             response);
        goto err;
    }

    if (insert_uart_send_msg(response, NULL, strlen(response), 0) != 0) {
        LOGE(TAG, "%s %d insert uart send msg fail\r\n", __func__, __LINE__);
        goto err;
    }

    return 0;

err:
    return -1;
}

#define MAX_ATCMD_MQTT_OPEN_RSP_LEN \
    (20 + MAX_ATCMD_DOMAIN_LEN + MAX_ATCMD_PORT_LEN)
static int atcmd_imqtt_open()
{
    const char *mqtt_open_prefix = "+IMQTTOPEN:";
    char        single;
    char        hostname[MAX_ATCMD_DOMAIN_LEN] = { 0 };
    int         port;
    char        port_str[MAX_ATCMD_PORT_LEN]          = { 0 };
    char        response[MAX_ATCMD_MQTT_OPEN_RSP_LEN] = { 0 };
    int         ret;
    int         offset = 0;

    at.parse(&single, 1);
    if ('=' != single && '?' != single) {
        LOGE(TAG, "Invalid mqtt open prefix %c!\n", single);
        goto err;
    }

    // AT_RECV_PREFIX
    if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_MQTT_OPEN_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_OPEN_RSP_LEN - offset,
                   "%s", AT_RECV_PREFIX);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
             response);
        goto err;
    }

    if ('?' == single) {
        // MQTTOPEN prefix + hostname + port
        if (offset + strlen(mqtt_open_prefix) + MAX_ATCMD_DOMAIN_LEN +
              MAX_ATCMD_PORT_LEN + 1 <
            MAX_ATCMD_MQTT_OPEN_RSP_LEN) {
            offset +=
              snprintf(response + offset, MAX_ATCMD_MQTT_OPEN_RSP_LEN - offset,
                       "%s%s,%d", mqtt_open_prefix, m_mqtt_config.hostname,
                       m_mqtt_config.port);
        } else {
            LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                 response);
            goto err;
        }

        // AT_RECV_PREFIX
        if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_MQTT_OPEN_RSP_LEN) {
            offset +=
              snprintf(response + offset, MAX_ATCMD_MQTT_OPEN_RSP_LEN - offset,
                       "%s", AT_RECV_PREFIX);
        } else {
            LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                 response);
            goto err;
        }
    } else if ('=' == single) {
        at.parse(&single, 1);

        if ('?' == single) {
            // MQTTOPEN prefix + \"hostname\" + \"port\"
            if (offset + strlen(mqtt_open_prefix) + 17 <
                MAX_ATCMD_MQTT_OPEN_RSP_LEN) {
                offset += snprintf(response + offset,
                                   MAX_ATCMD_MQTT_OPEN_RSP_LEN - offset,
                                   "%s\"hostname\",\"port\"", mqtt_open_prefix);
            } else {
                LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                     response);
                goto err;
            }

            if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_MQTT_OPEN_RSP_LEN) {
                offset += snprintf(response + offset,
                                   MAX_ATCMD_MQTT_OPEN_RSP_LEN - offset, "%s",
                                   AT_RECV_PREFIX);
            } else {
                LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                     response);
                goto err;
            }
        } else {
            hostname[0] = single;

            ret = atcmd_socket_data_info_get(&hostname[1], sizeof(hostname) - 1,
                                             NULL);
            if (ret < 0) {
                LOGE(TAG, "Invalid hostname %s !\n", hostname);
                goto err;
            }

            ret = atcmd_socket_data_info_get(port_str, sizeof(port_str),
                                             &atcmd_socket_data_len_check);
            if (ret < 0 || (port = atoi(port_str)) < 0) {
                LOGE(TAG, "Invalid port %s !\n", port_str);
                goto err;
            }

            // set hostname, port
            memcpy(m_mqtt_config.hostname, hostname, strlen(hostname));
            m_mqtt_config.port = port;
        }
    } else {
        LOGE(TAG, "Invalid mqtt mode prefix %c!\n", single);
        goto err;
    }

    // AT_RECV_SUCCESS_POSTFIX
    if (offset + strlen(AT_RECV_SUCCESS_POSTFIX) <
        MAX_ATCMD_MQTT_OPEN_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_OPEN_RSP_LEN - offset,
                   "%s", AT_RECV_SUCCESS_POSTFIX);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
             response);
        goto err;
    }

    if (insert_uart_send_msg(response, NULL, strlen(response), 0) != 0) {
        LOGE(TAG, "%s %d insert uart send msg fail\r\n", __func__, __LINE__);
        goto err;
    }

    return 0;

err:
    return -1;
}

#define MAX_ATCMD_MQTT_AUTH_RSP_LEN                             \
    (50 + MQTT_PRODUCT_KEY_MAX_LEN + MQTT_DEVICE_NAME_MAX_LEN + \
     MQTT_DEVICE_SECRET_MAX_LEN)

const static char *AUTH_PARA_TAG_KEY    = "PRODUCT_KEY";
const static char *AUTH_PARA_TAG_NAME   = "DEVICE_NAME";
const static char *AUTH_PARA_TAG_SECRET = "DEVICE_SECRET";

static int atcmd_imqtt_auth()
{
    const char *mqtt_auth_prefix = "+IMQTTAUTH:";
    char        single;
    char        productkey[MQTT_PRODUCT_KEY_MAX_LEN]     = { 0 };
    char        devicename[MQTT_DEVICE_NAME_MAX_LEN]     = { 0 };
    char        devicesecret[MQTT_DEVICE_SECRET_MAX_LEN] = { 0 };
    char        response[MAX_ATCMD_MQTT_AUTH_RSP_LEN]    = { 0 };
    int         ret;
    int         error_no = CME_ERROR_UNKNOWN;
    int         offset   = 0;

    at.parse(&single, 1);
    if ('=' != single && '?' != single) {
        LOGE(TAG, "Invalid mqtt auth prefix %c!\n", single);
        error_no = CME_ERROR_TXT_STR_INVALID_CHAR;
        goto err;
    }

    // AT_RECV_PREFIX
    if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_MQTT_AUTH_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_AUTH_RSP_LEN - offset,
                   "%s", AT_RECV_PREFIX);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
             response);
        error_no = CME_ERROR_TXT_STR_TOO_LONG;
        goto err;
    }

    if ('?' == single) {
        // MQTTAUTH prefix + productkey + productkey + devicesecret
        if (offset + strlen(mqtt_auth_prefix) + 2 + MQTT_PRODUCT_KEY_MAX_LEN +
              MQTT_DEVICE_NAME_MAX_LEN + MQTT_DEVICE_SECRET_MAX_LEN <
            MAX_ATCMD_MQTT_AUTH_RSP_LEN) {
            offset +=
              snprintf(response + offset, MAX_ATCMD_MQTT_AUTH_RSP_LEN - offset,
                       "%s%s,%s,%s", mqtt_auth_prefix, m_mqtt_config.productkey,
                       m_mqtt_config.devicename, m_mqtt_config.devicesecret);
        } else {
            LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                 response);
            error_no = CME_ERROR_TXT_STR_TOO_LONG;
            goto err;
        }

        // AT_RECV_PREFIX
        if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_MQTT_AUTH_RSP_LEN) {
            offset +=
              snprintf(response + offset, MAX_ATCMD_MQTT_AUTH_RSP_LEN - offset,
                       "%s", AT_RECV_PREFIX);
        } else {
            LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                 response);
            error_no = CME_ERROR_TXT_STR_TOO_LONG;
            goto err;
        }
    } else if ('=' == single) {
        at.parse(&single, 1);

        if ('?' == single) {
            // MQTTAUTH prefix + \"PRODUCT_KEY\" + \"DEVICE_NAME\" +
            // \"DEVICE_SECRET\"
            if (offset + strlen(mqtt_auth_prefix) + strlen(AUTH_PARA_TAG_KEY) +
                  3 + strlen(AUTH_PARA_TAG_NAME) + 3 +
                  strlen(AUTH_PARA_TAG_SECRET) + 2 <
                MAX_ATCMD_MQTT_AUTH_RSP_LEN) {
                offset += snprintf(
                  response + offset, MAX_ATCMD_MQTT_AUTH_RSP_LEN - offset,
                  "%s\"%s\",\"%s\",\"%s\"", mqtt_auth_prefix, AUTH_PARA_TAG_KEY,
                  AUTH_PARA_TAG_NAME, AUTH_PARA_TAG_SECRET);
            } else {
                LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                     response);
                error_no = CME_ERROR_TXT_STR_TOO_LONG;
                goto err;
            }

            if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_MQTT_AUTH_RSP_LEN) {
                offset += snprintf(response + offset,
                                   MAX_ATCMD_MQTT_AUTH_RSP_LEN - offset, "%s",
                                   AT_RECV_PREFIX);
            } else {
                LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                     response);
                error_no = CME_ERROR_TXT_STR_TOO_LONG;
                goto err;
            }
        } else {
            productkey[0] = single;

            ret = atcmd_socket_data_info_get(&productkey[1],
                                             sizeof(productkey) - 1, NULL);
            if (ret < 0) {
                LOGE(TAG, "Invalid productkey %s !\n", productkey);
                error_no = CME_ERROR_INCORRECT_PARA;
                goto err;
            }

            ret =
              atcmd_socket_data_info_get(devicename, sizeof(devicename), NULL);
            if (ret < 0) {
                LOGE(TAG, "Invalid devicename %s !\n", devicename);
                error_no = CME_ERROR_INCORRECT_PARA;
                goto err;
            }

            ret = atcmd_socket_data_info_get(devicesecret, sizeof(devicesecret),
                                             NULL);
            if (ret < 0) {
                LOGE(TAG, "Invalid devicesecret %s !\n", devicesecret);
                error_no = CME_ERROR_INCORRECT_PARA;
                goto err;
            }

            strcpy(m_mqtt_config.productkey, productkey);
            strcpy(m_mqtt_config.devicename, devicename);
            strcpy(m_mqtt_config.devicesecret, devicesecret);

            // trigger auth
            post_mqtt_authenticate_task();
        }
    } else {
        LOGE(TAG, "Invalid mqtt auth prefix %c!\n", single);
        error_no = CME_ERROR_TXT_STR_INVALID_CHAR;
        goto err;
    }

    // AT_RECV_SUCCESS_POSTFIX
    if (offset + strlen(AT_RECV_SUCCESS_POSTFIX) <
        MAX_ATCMD_MQTT_AUTH_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_AUTH_RSP_LEN - offset,
                   "%s", AT_RECV_SUCCESS_POSTFIX);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
             response);
        error_no = CME_ERROR_TXT_STR_TOO_LONG;
        goto err;
    }

    if (insert_uart_send_msg(response, NULL, strlen(response), 0) != 0) {
        LOGE(TAG, "%s %d insert uart send msg fail\r\n", __func__, __LINE__);
        goto err;
    }

    return 0;

err:
    notify_atcmd_cme_error(error_no);

    return -1;
}

#define MAX_ATCMD_MQTT_PARA_RSP_LEN 150
const static char *PARA_TAG_TIMEOUT   = "TIMEOUT";
const static char *PARA_TAG_CLEAN     = "CLEAN";
const static char *PARA_TAG_KEEPALIVE = "KEEPALIVE";
const static char *PARA_TAG_VERSION   = "VERSION";
const static char *PARA_TAG_AUTHMODE  = "AUTHMODE";

static int atcmd_imqtt_para()
{
    const char *mqtt_para_prefix = "+IMQTTPARA:";
    char        single;
    char        body[20] = { 0 };
    int         timeout;
    int         keepalive;
    int         clean;
    char        version[MQTT_VERSION_MAX_LEN] = { 0 };
    int         authmode;
    char        response[MAX_ATCMD_MQTT_PARA_RSP_LEN] = { 0 };
    int         error_no                              = CME_ERROR_UNKNOWN;
    int         ret;
    int         offset = 0;

    at.parse(&single, 1);
    if ('=' != single && '?' != single) {
        LOGE(TAG, "Invalid mqtt para prefix %c!\n", single);
        goto err;
    }

    // AT_RECV_PREFIX
    if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_MQTT_PARA_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_PARA_RSP_LEN - offset,
                   "%s", AT_RECV_PREFIX);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
             response);
        goto err;
    }

    if ('?' == single) {
        // MQTTPARA + TIMEOUT + value + CLEAN + value + KEEEPALIVE + value +
        // VESRION + value + AUTHMODE + value
        if (offset + strlen(mqtt_para_prefix) + strlen(PARA_TAG_TIMEOUT) + 4 +
              10 + strlen(PARA_TAG_CLEAN) + 4 + 1 + strlen(PARA_TAG_KEEPALIVE) +
              4 + 10 + strlen(PARA_TAG_VERSION) + 4 + MQTT_VERSION_MAX_LEN +
              strlen(PARA_TAG_AUTHMODE) + 3 + 1 <
            MAX_ATCMD_MQTT_PARA_RSP_LEN) {
            offset += snprintf(
              response + offset, MAX_ATCMD_MQTT_PARA_RSP_LEN - offset,
              "%s\"%s\",%d,\"%s\",%d,\"%s\",%d,\"%s\",\"%s\",\"%s\",%d",
              mqtt_para_prefix, PARA_TAG_TIMEOUT, m_mqtt_config.timeout,
              PARA_TAG_CLEAN, m_mqtt_config.clean, PARA_TAG_KEEPALIVE,
              m_mqtt_config.keepalive, PARA_TAG_VERSION, m_mqtt_config.version,
              PARA_TAG_AUTHMODE, m_mqtt_config.authmode);
        } else {
            LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                 response);
            error_no = CME_ERROR_TXT_STR_TOO_LONG;
            goto err;
        }

        // AT_RECV_PREFIX
        if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_MQTT_PARA_RSP_LEN) {
            offset +=
              snprintf(response + offset, MAX_ATCMD_MQTT_PARA_RSP_LEN - offset,
                       "%s", AT_RECV_PREFIX);
        } else {
            LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                 response);
            error_no = CME_ERROR_TXT_STR_TOO_LONG;
            goto err;
        }
    } else if ('=' == single) {
        at.parse(&single, 1);

        if ('?' == single) {
            // MQTTPARA + \"ParaTag\" + \"ParaValue\"
            if (offset + strlen(mqtt_para_prefix) + 25 <
                MAX_ATCMD_MQTT_PARA_RSP_LEN) {
                offset += snprintf(
                  response + offset, MAX_ATCMD_MQTT_PARA_RSP_LEN - offset,
                  "%s\"ParaTag\",\"ParaValue\"", mqtt_para_prefix);
            } else {
                LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                     response);
                error_no = CME_ERROR_TXT_STR_TOO_LONG;
                goto err;
            }

            if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_MQTT_PARA_RSP_LEN) {
                offset += snprintf(response + offset,
                                   MAX_ATCMD_MQTT_PARA_RSP_LEN - offset, "%s",
                                   AT_RECV_PREFIX);
            } else {
                LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                     response);
                error_no = CME_ERROR_TXT_STR_TOO_LONG;
                goto err;
            }
        } else {
            at.parse(body, strlen(PARA_TAG_TIMEOUT) + 2);
            if (memcmp(body, "TIMEOUT\",", strlen(PARA_TAG_TIMEOUT) + 2) != 0) {
                LOGE(TAG, "Invalid para tag %s !\n", body);
                error_no = CME_ERROR_INCORRECT_PARA;
                goto err;
            }

            memset(body, 0, sizeof(body));
            ret = atcmd_socket_data_info_get(body, sizeof(body),
                                             &atcmd_socket_data_len_check);
            if (ret < 0 || (timeout = atoi(body)) < 0) {
                LOGE(TAG, "Invalid timeout %s !\n", body);
                error_no = CME_ERROR_INCORRECT_PARA;
                goto err;
            }

            at.parse(body, strlen(PARA_TAG_CLEAN) + 3);
            if (memcmp(body, "\"CLEAN\",", strlen(PARA_TAG_CLEAN) + 3) != 0) {
                LOGE(TAG, "Invalid para tag %s !\n", body);
                error_no = CME_ERROR_INCORRECT_PARA;
                goto err;
            }

            memset(body, 0, sizeof(body));
            ret = atcmd_socket_data_info_get(body, sizeof(body),
                                             &atcmd_socket_data_len_check);
            if (ret < 0 || (clean = atoi(body)) < 0) {
                LOGE(TAG, "Invalid clean %s !\n", body);
                error_no = CME_ERROR_INCORRECT_PARA;
                goto err;
            }

            at.parse(body, strlen(PARA_TAG_KEEPALIVE) + 3);
            if (memcmp(body, "\"KEEPALIVE\",",
                       strlen(PARA_TAG_KEEPALIVE) + 3) != 0) {
                LOGE(TAG, "Invalid para tag %s !\n", body);
                error_no = CME_ERROR_INCORRECT_PARA;
                goto err;
            }

            memset(body, 0, sizeof(body));
            ret = atcmd_socket_data_info_get(body, sizeof(body),
                                             &atcmd_socket_data_len_check);
            if (ret < 0 || (keepalive = atoi(body)) < 0) {
                LOGE(TAG, "Invalid keepalive %s !\n", body);
                error_no = CME_ERROR_INCORRECT_PARA;
                goto err;
            }

            at.parse(body, strlen(PARA_TAG_VERSION) + 3);
            if (memcmp(body, "\"VERSION\",", strlen(PARA_TAG_VERSION) + 3) !=
                0) {
                LOGE(TAG, "Invalid para tag %s !\n", body);
                error_no = CME_ERROR_INCORRECT_PARA;
                goto err;
            }

            memset(body, 0, sizeof(body));
            ret = atcmd_socket_data_info_get(body, sizeof(body), NULL);
            if (ret < 0 || strlen(body) - 2 > MQTT_VERSION_MAX_LEN) {
                LOGE(TAG, "Invalid keepalive %s !\n", body);
                error_no = CME_ERROR_INCORRECT_PARA;
                goto err;
            }
            memcpy(version, body + 1, strlen(body) - 2);

            at.parse(body, strlen(PARA_TAG_AUTHMODE) + 3);
            if (memcmp(body, "\"AUTHMODE\",", strlen(PARA_TAG_AUTHMODE) + 3) !=
                0) {
                LOGE(TAG, "Invalid para tag %s !\n", body);
                error_no = CME_ERROR_INCORRECT_PARA;
                goto err;
            }

            memset(body, 0, sizeof(body));
            ret = atcmd_socket_data_info_get(body, sizeof(body),
                                             &atcmd_socket_data_len_check);
            if (ret < 0 || (authmode = atoi(body)) < 0) {
                LOGE(TAG, "Invalid keepalive %s !\n", body);
                error_no = CME_ERROR_INCORRECT_PARA;
                goto err;
            }

            m_mqtt_config.timeout   = timeout;
            m_mqtt_config.clean     = clean;
            m_mqtt_config.keepalive = keepalive;
            strcpy(m_mqtt_config.version, version);
            m_mqtt_config.authmode = authmode;
        }
    } else {
        LOGE(TAG, "Invalid mqtt para prefix %c!\n", single);
        error_no = CME_ERROR_TXT_STR_INVALID_CHAR;
        goto err;
    }

    // AT_RECV_SUCCESS_POSTFIX
    if (offset + strlen(AT_RECV_SUCCESS_POSTFIX) <
        MAX_ATCMD_MQTT_PARA_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_PARA_RSP_LEN - offset,
                   "%s", AT_RECV_SUCCESS_POSTFIX);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
             response);
        error_no = CME_ERROR_TXT_STR_TOO_LONG;
        goto err;
    }

    if (insert_uart_send_msg(response, NULL, strlen(response), 0) != 0) {
        LOGE(TAG, "%s %d insert uart send msg fail\r\n", __func__, __LINE__);
        goto err;
    }

    return 0;

err:
    notify_atcmd_cme_error(error_no);

    return -1;
}

#define MAX_ATCMD_MQTT_CONN_RSP_LEN 10
static int atcmd_imqtt_conn()
{
    char single;
    char response[MAX_ATCMD_MQTT_CONN_RSP_LEN] = { 0 };
    int  ret;
    int  error_no = CME_ERROR_UNKNOWN;
    int  offset   = 0;

    at.parse(&single, 1);
    if ('=' != single && '\r' != single) {
        LOGE(TAG, "Invalid mqtt conn prefix %c!\n", single);
        error_no = CME_ERROR_TXT_STR_INVALID_CHAR;
        goto err;
    }

    if ('=' == single) {
        at.parse(&single, 1);

        if ('?' != single) {
            LOGE(TAG, "Invalid mqtt conn prefix %c!\n", single);
            error_no = CME_ERROR_TXT_STR_INVALID_CHAR;
            goto err;
        }
    } else {
        // connect
        post_mqtt_connect_task();
    }

    // AT_RECV_PREFIX
    if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_MQTT_CONN_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_CONN_RSP_LEN - offset,
                   "%s", AT_RECV_PREFIX);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
             response);
        error_no = CME_ERROR_TXT_STR_TOO_LONG;
        goto err;
    }

    // AT_RECV_SUCCESS_POSTFIX
    if (offset + strlen(AT_RECV_SUCCESS_POSTFIX) <
        MAX_ATCMD_MQTT_CONN_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_CONN_RSP_LEN - offset,
                   "%s", AT_RECV_SUCCESS_POSTFIX);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
             response);
        error_no = CME_ERROR_TXT_STR_TOO_LONG;
        goto err;
    }

    if (insert_uart_send_msg(response, NULL, strlen(response), 0) != 0) {
        LOGE(TAG, "%s %d insert uart send msg fail\r\n", __func__, __LINE__);
        goto err;
    }

    return 0;

err:
    notify_atcmd_cme_error(error_no);

    return -1;
}

#define MAX_ATCMD_MQTT_PUB_RSP_LEN \
    (20 + MQTT_MSG_MAX_LEN + 1 + MQTT_TOPIC_MAX_LEN + 1 + 5)

static const char *PARA_TAG_TOPIC   = "topic";
static const char *PARA_TAG_QOS     = "qos";
static const char *PARA_TAG_MSG     = "message";
static const char *PARA_TAG_FORMAT  = "format";
static const char *PARA_TAG_FRAG_ID = "fragment_id";

static int atcmd_imqtt_pub()
{
    const char *mqtt_pub_prefix = "+IMQTTPUB:";
    char        single;
    char        body[20]                             = { 0 };
    char        response[MAX_ATCMD_MQTT_PUB_RSP_LEN] = { 0 };
    int         ret;
    int         error_no = CME_ERROR_UNKNOWN;
    int         offset   = 0;

    at.parse(&single, 1);
    if ('=' != single && '?' != single) {
        LOGE(TAG, "Invalid mqtt pub prefix %c!\n", single);
        error_no = CME_ERROR_TXT_STR_INVALID_CHAR;
        goto err;
    }

    // AT_RECV_PREFIX
    if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_MQTT_PUB_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_PUB_RSP_LEN - offset, "%s",
                   AT_RECV_PREFIX);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
             response);
        error_no = CME_ERROR_TXT_STR_TOO_LONG;
        goto err;
    }

    // prefix
    if (offset + strlen(mqtt_pub_prefix) < MAX_ATCMD_MQTT_PUB_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_PUB_RSP_LEN - offset, "%s",
                   mqtt_pub_prefix);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
             response);
        error_no = CME_ERROR_TXT_STR_TOO_LONG;
        goto err;
    }

    if ('?' == single) {
        // TOPIC + QOS + MESSAGE + [FORMAT] + [FRAGMENT_ID]
        if (offset + strlen(m_last_pub_msg.topic_pub) +
              strlen(m_last_pub_msg.msg_pub) + 1 + 1 + 5 + 4 + 4 <
            MAX_ATCMD_MQTT_PUB_RSP_LEN) {
            if (m_last_pub_msg.valid)
                offset += snprintf(
                  response + offset, MAX_ATCMD_MQTT_PUB_RSP_LEN - offset,
                  "\"%s\",%d,\"%s\",%d,%d", m_last_pub_msg.topic_pub,
                  m_last_pub_msg.qos, m_last_pub_msg.msg_pub,
                  m_last_pub_msg.msg_format, m_last_pub_msg.fragment_id);
            else
                offset += snprintf(response + offset,
                                   MAX_ATCMD_MQTT_PUB_RSP_LEN - offset, ",,,,");
        } else {
            LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                 response);
            error_no = CME_ERROR_TXT_STR_TOO_LONG;
            goto err;
        }
    } else if ('=' == single) {
        at.parse(&single, 1);

        if ('?' == single) {
            // \"topic\" + \"qos\" + \"message\" + \"format\" + \"fragment_id\"
            if (offset + strlen(PARA_TAG_TOPIC) + 3 + strlen(PARA_TAG_QOS) + 3 +
                  strlen(PARA_TAG_MSG) + 3 + strlen(PARA_TAG_FORMAT) + 3 +
                  strlen(PARA_TAG_FRAG_ID) + 2 <
                MAX_ATCMD_MQTT_PUB_RSP_LEN) {
                offset += snprintf(response + offset,
                                   MAX_ATCMD_MQTT_PUB_RSP_LEN - offset,
                                   "\"%s\",\"%s\",\"%s\",\"%s\",\"%s\"",
                                   PARA_TAG_TOPIC, PARA_TAG_QOS, PARA_TAG_MSG,
                                   PARA_TAG_FORMAT, PARA_TAG_FRAG_ID);
            } else {
                LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                     response);
                error_no = CME_ERROR_TXT_STR_TOO_LONG;
                goto err;
            }
        } else {
            memset(&m_attempt_pub_msg, 0, sizeof(m_attempt_pub_msg));

            m_attempt_pub_msg.topic_pub[0] = single;
            ret                            = atcmd_socket_data_info_get(
              m_attempt_pub_msg.topic_pub, sizeof(m_attempt_pub_msg.topic_pub),
              NULL);
            if (ret < 0) {
                LOGE(TAG, "Invalid topic %s !\n", m_attempt_pub_msg.topic_pub);
                error_no = CME_ERROR_INCORRECT_PARA;
                goto err;
            }

            // ignore "
            if (strlen(m_attempt_pub_msg.topic_pub) > 0 &&
                m_attempt_pub_msg
                    .topic_pub[strlen(m_attempt_pub_msg.topic_pub) - 1] ==
                  '\"') {
                m_attempt_pub_msg
                  .topic_pub[strlen(m_attempt_pub_msg.topic_pub) - 1] = '\0';
            } else {
                LOGE(TAG, "Invalid topic %s !\n", m_attempt_pub_msg.topic_pub);
                error_no = CME_ERROR_INCORRECT_PARA;
                goto err;
            }

            memset(body, 0, sizeof(body));
            ret = atcmd_socket_data_info_get(body, sizeof(body),
                                             &atcmd_socket_data_len_check);
            if (ret < 0 || (m_attempt_pub_msg.qos = atoi(body)) < 0 ||
                m_attempt_pub_msg.qos > 2) {
                LOGE(TAG, "Invalid qos %s !\n", body);
                error_no = CME_ERROR_INCORRECT_PARA;
                goto err;
            }

            // eat "
            at.parse(&single, 1);
            ret = atcmd_socket_data_info_get(m_attempt_pub_msg.msg_pub,
                                             sizeof(m_attempt_pub_msg.msg_pub),
                                             NULL);
            if (ret < 0) {
                LOGE(TAG, "Invalid message %s !\n", m_attempt_pub_msg.msg_pub);
                error_no = CME_ERROR_INCORRECT_PARA;
                goto err;
            }

            // ignore "
            if (strlen(m_attempt_pub_msg.msg_pub) > 0) {
                m_attempt_pub_msg
                  .msg_pub[strlen(m_attempt_pub_msg.msg_pub) - 1] = '\0';
            }

            memset(body, 0, sizeof(body));
            ret = atcmd_socket_data_info_get(body, sizeof(body),
                                             &atcmd_socket_data_len_check);
            if (ret < 0 || (m_attempt_pub_msg.msg_format = atoi(body)) < 0 ||
                m_attempt_pub_msg.msg_format > 1) {
                LOGE(TAG, "Invalid format %s !\n", body);
                error_no = CME_ERROR_INCORRECT_PARA;
                goto err;
            }

            memset(body, 0, sizeof(body));
            ret = atcmd_socket_data_info_get(body, sizeof(body),
                                             &atcmd_socket_data_len_check);
            if (ret < 0 || (m_attempt_pub_msg.fragment_id = atoi(body)) < 0) {
                LOGE(TAG, "Invalid frag_id %s !\n", body);
                error_no = CME_ERROR_INCORRECT_PARA;
                goto err;
            }

            m_attempt_pub_msg.valid = true;
            ret                     = mqtt_publish(&m_attempt_pub_msg);
            if (ret < 0) {
                if (ret == MQTT_NETWORK_ERROR)
                    error_no = CME_ERROR_NET_TIMEOUT;
                else if (ret == FAIL_RETURN || ret == NULL_VALUE_ERROR)
                    error_no = CME_ERROR_INCORRECT_PARA;
                else {
                    LOGE(TAG, "mqtt publish unknown err! ret %d\n", ret);
                }

                goto err;
            } else {
                // packetid
                if (offset + 5 < MAX_ATCMD_MQTT_PUB_RSP_LEN) {
                    offset +=
                      snprintf(response + offset,
                               MAX_ATCMD_MQTT_PUB_RSP_LEN - offset, "%d", ret);
                } else {
                    LOGE(TAG, "%s %d at string too long %s\n", __func__,
                         __LINE__, response);
                    error_no = CME_ERROR_TXT_STR_TOO_LONG;
                    goto err;
                }
            }
        }
    } else {
        LOGE(TAG, "Invalid mqtt pub prefix %c!\n", single);
        error_no = CME_ERROR_TXT_STR_INVALID_CHAR;
        goto err;
    }

    // AT_RECV_PREFIX
    if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_MQTT_PUB_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_PUB_RSP_LEN - offset, "%s",
                   AT_RECV_PREFIX);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
             response);
        error_no = CME_ERROR_TXT_STR_TOO_LONG;
        goto err;
    }

    // AT_RECV_SUCCESS_POSTFIX
    if (offset + strlen(AT_RECV_SUCCESS_POSTFIX) < MAX_ATCMD_MQTT_PUB_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_PUB_RSP_LEN - offset, "%s",
                   AT_RECV_SUCCESS_POSTFIX);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
             response);
        error_no = CME_ERROR_TXT_STR_TOO_LONG;
        goto err;
    }

    if (insert_uart_send_msg(response, NULL, strlen(response), 0) != 0) {
        LOGE(TAG, "%s %d insert uart send msg fail\r\n", __func__, __LINE__);
        goto err;
    }

    return 0;

err:
    notify_atcmd_cme_error(error_no);

    return -1;
}

#define MAX_ATCMD_MQTT_SUB_RSP_LEN (20 + MQTT_TOPIC_MAX_LEN)

static int atcmd_imqtt_sub()
{
    const char *mqtt_sub_prefix = "+IMQTTSUB:";
    char        single;
    char        body[20]                             = { 0 };
    char        response[MAX_ATCMD_MQTT_SUB_RSP_LEN] = { 0 };
    int         ret;
    int         error_no = CME_ERROR_UNKNOWN;
    int         offset   = 0;

    at.parse(&single, 1);
    if ('=' != single && '?' != single) {
        LOGE(TAG, "Invalid mqtt sub prefix %c!\n", single);
        error_no = CME_ERROR_TXT_STR_INVALID_CHAR;
        goto err;
    }

    // AT_RECV_PREFIX
    if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_MQTT_SUB_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_SUB_RSP_LEN - offset, "%s",
                   AT_RECV_PREFIX);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
             response);
        error_no = CME_ERROR_TXT_STR_TOO_LONG;
        goto err;
    }

    // prefix
    if (offset + strlen(mqtt_sub_prefix) < MAX_ATCMD_MQTT_SUB_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_SUB_RSP_LEN - offset, "%s",
                   mqtt_sub_prefix);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
             response);
        error_no = CME_ERROR_TXT_STR_TOO_LONG;
        goto err;
    }

    if ('?' == single) {
        // TOPIC + QOS
        if (offset + MQTT_TOPIC_MAX_LEN + 2 + 2 < MAX_ATCMD_MQTT_SUB_RSP_LEN) {
            if (m_last_sub_topic.valid)
                offset += snprintf(
                  response + offset, MAX_ATCMD_MQTT_SUB_RSP_LEN - offset,
                  "\"%s\",%d", m_last_sub_topic.topic, m_last_sub_topic.qos);
            else
                offset += snprintf(response + offset,
                                   MAX_ATCMD_MQTT_SUB_RSP_LEN - offset, ",");
        } else {
            LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                 response);
            error_no = CME_ERROR_TXT_STR_TOO_LONG;
            goto err;
        }
    } else if ('=' == single) {
        at.parse(&single, 1);

        if ('?' == single) {
            // \"topic\" + \"qos\"
            if (offset + strlen(PARA_TAG_TOPIC) + 3 + strlen(PARA_TAG_QOS) + 2 <
                MAX_ATCMD_MQTT_SUB_RSP_LEN) {
                offset += snprintf(
                  response + offset, MAX_ATCMD_MQTT_SUB_RSP_LEN - offset,
                  "\"%s\",\"%s\"", PARA_TAG_TOPIC, PARA_TAG_QOS);
            } else {
                LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                     response);
                error_no = CME_ERROR_TXT_STR_TOO_LONG;
                goto err;
            }
        } else {
            memset(&m_attempt_sub_topic, 0, sizeof(m_attempt_sub_topic));

            ret = atcmd_socket_data_info_get(m_attempt_sub_topic.topic,
                                             sizeof(m_attempt_sub_topic.topic),
                                             NULL);
            if (ret < 0) {
                LOGE(TAG, "Invalid topic %s !\n", m_attempt_sub_topic.topic);
                error_no = CME_ERROR_INCORRECT_PARA;
                goto err;
            }

            // ignore "
            if (strlen(m_attempt_sub_topic.topic) > 0 &&
                m_attempt_sub_topic
                    .topic[strlen(m_attempt_sub_topic.topic) - 1] == '\"') {
                m_attempt_sub_topic
                  .topic[strlen(m_attempt_sub_topic.topic) - 1] = '\0';
            } else {
                LOGE(TAG, "Invalid topic %s !\n", m_attempt_sub_topic.topic);
                error_no = CME_ERROR_INCORRECT_PARA;
                goto err;
            }

            memset(body, 0, sizeof(body));
            ret = atcmd_socket_data_info_get(body, sizeof(body),
                                             &atcmd_socket_data_len_check);
            if (ret < 0 || (m_attempt_sub_topic.qos = atoi(body)) < 0 ||
                m_attempt_sub_topic.qos > 2) {
                LOGE(TAG, "Invalid qos %s !\n", body);
                error_no = CME_ERROR_INCORRECT_PARA;
                goto err;
            }

            m_attempt_sub_topic.valid = true;
            ret                       = mqtt_subscribe(&m_attempt_sub_topic);
            if (ret < 0) {
                if (ret == MQTT_NETWORK_ERROR)
                    error_no = CME_ERROR_NET_TIMEOUT;
                else if (ret == FAIL_RETURN || ret == NULL_VALUE_ERROR)
                    error_no = CME_ERROR_INCORRECT_PARA;

                goto err;
            } else {
                if (offset + 5 < MAX_ATCMD_MQTT_SUB_RSP_LEN) {
                    offset +=
                      snprintf(response + offset,
                               MAX_ATCMD_MQTT_SUB_RSP_LEN - offset, "%d", ret);
                } else {
                    LOGE(TAG, "%s %d at string too long %s\n", __func__,
                         __LINE__, response);
                    error_no = CME_ERROR_TXT_STR_TOO_LONG;
                    goto err;
                }
            }
        }
    } else {
        LOGE(TAG, "Invalid mqtt sub prefix %c!\n", single);
        error_no = CME_ERROR_TXT_STR_INVALID_CHAR;
        goto err;
    }

    // AT_RECV_PREFIX
    if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_MQTT_SUB_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_SUB_RSP_LEN - offset, "%s",
                   AT_RECV_PREFIX);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
             response);
        error_no = CME_ERROR_TXT_STR_TOO_LONG;
        goto err;
    }

    // AT_RECV_SUCCESS_POSTFIX
    if (offset + strlen(AT_RECV_SUCCESS_POSTFIX) < MAX_ATCMD_MQTT_SUB_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_SUB_RSP_LEN - offset, "%s",
                   AT_RECV_SUCCESS_POSTFIX);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
             response);
        error_no = CME_ERROR_TXT_STR_TOO_LONG;
        goto err;
    }

    if (insert_uart_send_msg(response, NULL, strlen(response), 0) != 0) {
        LOGE(TAG, "%s %d insert uart send msg fail\r\n", __func__, __LINE__);
        goto err;
    }

    return 0;

err:
    notify_atcmd_cme_error(error_no);
    return -1;
}

#define MAX_ATCMD_MQTT_UNSUB_RSP_LEN (25 + MQTT_TOPIC_MAX_LEN)

static int atcmd_imqtt_unsub()
{
    const char *mqtt_unsub_prefix = "+IMQTTUNSUB:";
    char        single;
    char        body[20]                               = { 0 };
    char        response[MAX_ATCMD_MQTT_UNSUB_RSP_LEN] = { 0 };
    int         error_no                               = CME_ERROR_UNKNOWN;
    int         ret;
    int         offset = 0;

    at.parse(&single, 1);
    if ('=' != single && '?' != single) {
        LOGE(TAG, "Invalid mqtt unsub prefix %c!\n", single);
        error_no = CME_ERROR_TXT_STR_INVALID_CHAR;
        goto err;
    }

    // AT_RECV_PREFIX
    if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_MQTT_UNSUB_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_UNSUB_RSP_LEN - offset,
                   "%s", AT_RECV_PREFIX);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
             response);
        error_no = CME_ERROR_TXT_STR_TOO_LONG;
        goto err;
    }

    // prefix
    if (offset + strlen(mqtt_unsub_prefix) < MAX_ATCMD_MQTT_UNSUB_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_UNSUB_RSP_LEN - offset,
                   "%s", mqtt_unsub_prefix);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
             response);
        error_no = CME_ERROR_TXT_STR_TOO_LONG;
        goto err;
    }

    if ('?' == single) {
        // TOPIC
        if (offset + strlen(m_last_unsub_topic.topic) <
            MAX_ATCMD_MQTT_UNSUB_RSP_LEN) {
            if (m_last_unsub_topic.valid)
                offset += snprintf(response + offset,
                                   MAX_ATCMD_MQTT_UNSUB_RSP_LEN - offset,
                                   "\"%s\"", m_last_unsub_topic.topic);
        } else {
            LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                 response);
            error_no = CME_ERROR_TXT_STR_TOO_LONG;
            goto err;
        }
    } else if ('=' == single) {
        at.parse(&single, 1);

        if ('?' == single) {
            // \"topic\"
            if (offset + strlen(PARA_TAG_TOPIC) + 2 <
                MAX_ATCMD_MQTT_UNSUB_RSP_LEN) {
                offset += snprintf(response + offset,
                                   MAX_ATCMD_MQTT_UNSUB_RSP_LEN - offset,
                                   "\"%s\"", PARA_TAG_TOPIC, PARA_TAG_QOS);
            } else {
                LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                     response);
                error_no = CME_ERROR_TXT_STR_TOO_LONG;
                goto err;
            }
        } else {
            memset(&m_attempt_unsub_topic, 0, sizeof(m_attempt_unsub_topic));

            ret = atcmd_socket_data_info_get(
              m_attempt_unsub_topic.topic, sizeof(m_attempt_unsub_topic.topic),
              NULL);
            if (ret < 0) {
                LOGE(TAG, "Invalid topic %s !\n", m_attempt_unsub_topic.topic);
                error_no = CME_ERROR_INCORRECT_PARA;
                goto err;
            }

            if (strlen(m_attempt_unsub_topic.topic) > 0 &&
                m_attempt_unsub_topic
                    .topic[strlen(m_attempt_unsub_topic.topic) - 1] == '\"') {
                m_attempt_unsub_topic
                  .topic[strlen(m_attempt_unsub_topic.topic) - 1] = '\0';
            } else {
                LOGE(TAG, "Invalid topic %s !\n", m_attempt_unsub_topic.topic);
                error_no = CME_ERROR_INCORRECT_PARA;
                goto err;
            }

            m_attempt_unsub_topic.valid = true;
            ret = mqtt_unsubscribe(&m_attempt_unsub_topic);
            if (ret < 0) {
                if (ret == MQTT_NETWORK_ERROR)
                    error_no = CME_ERROR_NET_TIMEOUT;
                else if (ret == FAIL_RETURN)
                    error_no = CME_ERROR_INCORRECT_PARA;

                goto err;
            } else {
                if (offset + 5 < MAX_ATCMD_MQTT_UNSUB_RSP_LEN) {
                    offset += snprintf(response + offset,
                                       MAX_ATCMD_MQTT_UNSUB_RSP_LEN - offset,
                                       "%d", ret);
                } else {
                    LOGE(TAG, "%s %d at string too long %s\n", __func__,
                         __LINE__, response);
                    error_no = CME_ERROR_TXT_STR_TOO_LONG;
                    goto err;
                }
            }
        }
    } else {
        LOGE(TAG, "Invalid mqtt unsub prefix %c!\n", single);
        error_no = CME_ERROR_TXT_STR_INVALID_CHAR;
        goto err;
    }

    // AT_RECV_PREFIX
    if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_MQTT_UNSUB_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_UNSUB_RSP_LEN - offset,
                   "%s", AT_RECV_PREFIX);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
             response);
        error_no = CME_ERROR_TXT_STR_TOO_LONG;
        goto err;
    }

    // AT_RECV_SUCCESS_POSTFIX
    if (offset + strlen(AT_RECV_SUCCESS_POSTFIX) <
        MAX_ATCMD_MQTT_UNSUB_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_UNSUB_RSP_LEN - offset,
                   "%s", AT_RECV_SUCCESS_POSTFIX);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
             response);
        error_no = CME_ERROR_TXT_STR_TOO_LONG;
        goto err;
    }

    if (insert_uart_send_msg(response, NULL, strlen(response), 0) != 0) {
        LOGE(TAG, "%s %d insert uart send msg fail\r\n", __func__, __LINE__);
        goto err;
    }

    return 0;

err:
    notify_atcmd_cme_error(error_no);
    return -1;
}

static int atcmd_imqtt_rcvpub()
{
    const char *mqtt_rcvpub_prefix = "+IMQTTRCVPUB:";
    char        single;
    char        response[MAX_ATCMD_MQTT_RCVPUB_RSP_LEN] = { 0 };
    int         ret;
    int         offset = 0;


    at.parse(&single, 1);
    if ('=' != single && '?' != single) {
        LOGE(TAG, "Invalid mqtt recvpub prefix %c!\n", single);
        goto err;
    }

    if ('=' == single) {
        at.parse(&single, 1);

        if ('?' != single) {
            LOGE(TAG, "Invalid mqtt recvpub prefix %c!\n", single);
            goto err;
        }
    } else if ('?' == single) {
        // AT_RECV_PREFIX
        if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_MQTT_RCVPUB_RSP_LEN) {
            offset += snprintf(response + offset,
                               MAX_ATCMD_MQTT_RCVPUB_RSP_LEN - offset, "%s",
                               AT_RECV_PREFIX);
        } else {
            LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                 response);
            goto err;
        }

        // prefix
        if (offset + strlen(mqtt_rcvpub_prefix) <
            MAX_ATCMD_MQTT_RCVPUB_RSP_LEN) {
            offset += snprintf(response + offset,
                               MAX_ATCMD_MQTT_RCVPUB_RSP_LEN - offset, "%s",
                               mqtt_rcvpub_prefix);
        } else {
            LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                 response);
            goto err;
        }

        // PACKETID + TOPIC + MSG_LEN + MESSAGE + [FORMAT] + [FRAGMENT_ID]
        if (offset + 10 + strlen(m_last_recvpub_msg.topic_pub) + 5 +
              strlen(m_last_pub_msg.msg_pub) + 1 + 5 + 5 <
            MAX_ATCMD_MQTT_RCVPUB_RSP_LEN) {
            if (m_last_recvpub_msg.valid)
                offset += snprintf(
                  response + offset, MAX_ATCMD_MQTT_PUB_RSP_LEN - offset,
                  "%d,%s,%d,%s,%d,%d", m_last_recvpub_msg.packet_id,
                  m_last_recvpub_msg.topic_pub,
                  strlen(m_last_recvpub_msg.msg_pub),
                  m_last_recvpub_msg.msg_pub, m_last_recvpub_msg.msg_format,
                  m_last_recvpub_msg.fragment_id);
            else
                offset +=
                  snprintf(response + offset,
                           MAX_ATCMD_MQTT_PUB_RSP_LEN - offset, ",,,,,");
        } else {
            LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                 response);
            goto err;
        }


    } else {
        LOGE(TAG, "Invalid mqtt recvpub prefix %c!\n", single);
        goto err;
    }

    // AT_RECV_PREFIX
    if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_MQTT_RCVPUB_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_RCVPUB_RSP_LEN - offset,
                   "%s", AT_RECV_PREFIX);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
             response);
        goto err;
    }

    // AT_RECV_SUCCESS_POSTFIX
    if (offset + strlen(AT_RECV_SUCCESS_POSTFIX) <
        MAX_ATCMD_MQTT_RCVPUB_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_RCVPUB_RSP_LEN - offset,
                   "%s", AT_RECV_SUCCESS_POSTFIX);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
             response);
        goto err;
    }

    if (insert_uart_send_msg(response, NULL, strlen(response), 0) != 0) {
        LOGE(TAG, "%s %d insert uart send msg fail\r\n", __func__, __LINE__);
        goto err;
    }

    return 0;

err:
    return -1;
}

#define MAX_ATCMD_MQTT_STATE_RSP_LEN 30

static int atcmd_imqtt_state()
{
    const char *mqtt_state_prefix = "+IMQTTSTATE:";
    char        single;
    char        response[MAX_ATCMD_MQTT_STATE_RSP_LEN] = { 0 };
    int         ret;
    int         offset = 0;

    at.parse(&single, 1);
    if ('=' != single && '?' != single) {
        LOGE(TAG, "Invalid mqtt state prefix %c!\n", single);
        goto err;
    }

    // AT_RECV_PREFIX
    if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_MQTT_STATE_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_STATE_RSP_LEN - offset,
                   "%s", AT_RECV_PREFIX);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
             response);
        goto err;
    }

    if ('=' == single) {
        at.parse(&single, 1);

        if ('?' != single) {
            LOGE(TAG, "Invalid mqtt state prefix %c!\n", single);
            goto err;
        }
    } else if ('?' == single) {
        int state;

        if (IOT_MQTT_CheckStateNormal(m_pclient) > 0)
            state = MQTT_STATE_CONNEC;
        else
            state = MQTT_STATE_DISCON;

        // IMQTTSTATE + state
        if (offset + strlen(mqtt_state_prefix) + 1 <
            MAX_ATCMD_MQTT_STATE_RSP_LEN) {
            offset +=
              snprintf(response + offset, MAX_ATCMD_MQTT_STATE_RSP_LEN - offset,
                       "%s%d", mqtt_state_prefix, state);
        } else {
            LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                 response);
            goto err;
        }

        // AT_RECV_PREFIX
        if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_MQTT_STATE_RSP_LEN) {
            offset +=
              snprintf(response + offset, MAX_ATCMD_MQTT_STATE_RSP_LEN - offset,
                       "%s", AT_RECV_PREFIX);
        } else {
            LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                 response);
            goto err;
        }
    }

    // AT_RECV_SUCCESS_POSTFIX
    if (offset + strlen(AT_RECV_SUCCESS_POSTFIX) <
        MAX_ATCMD_MQTT_STATE_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_STATE_RSP_LEN - offset,
                   "%s", AT_RECV_SUCCESS_POSTFIX);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
             response);
        goto err;
    }

    if (insert_uart_send_msg(response, NULL, strlen(response), 0) != 0) {
        LOGE(TAG, "%s %d insert uart send msg fail\r\n", __func__, __LINE__);
        goto err;
    }

    return 0;

err:
    return -1;
}

#define MAX_ATCMD_MQTT_DISCONN_RSP_LEN 20
static int atcmd_imqtt_disconn()
{
    char single;
    char response[MAX_ATCMD_MQTT_DISCONN_RSP_LEN] = { 0 };
    int  error_no                                 = CME_ERROR_UNKNOWN;
    int  ret;
    int  offset = 0;

    at.parse(&single, 1);
    if ('=' != single && '\r' != single) {
        LOGE(TAG, "Invalid mqtt disconn prefix %c!\n", single);
        error_no = CME_ERROR_TXT_STR_INVALID_CHAR;
        goto err;
    }

    if ('=' == single) {
        at.parse(&single, 1);

        if ('?' != single) {
            LOGE(TAG, "Invalid mqtt disconn prefix %c!\n", single);
            error_no = CME_ERROR_TXT_STR_INVALID_CHAR;
            goto err;
        }
    } else {
        if (IOT_MQTT_Destroy(&m_pclient) < 0) {
            LOGE(TAG, "mqtt disconn fail!\n");
            error_no = CME_ERROR_OP_NOT_ALLOWED;
            goto err;
        }
    }

    // AT_RECV_PREFIX
    if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_MQTT_DISCONN_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_DISCONN_RSP_LEN - offset,
                   "%s", AT_RECV_PREFIX);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
             response);
        error_no = CME_ERROR_TXT_STR_TOO_LONG;
        goto err;
    }

    // AT_RECV_SUCCESS_POSTFIX
    if (offset + strlen(AT_RECV_SUCCESS_POSTFIX) <
        MAX_ATCMD_MQTT_DISCONN_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_DISCONN_RSP_LEN - offset,
                   "%s", AT_RECV_SUCCESS_POSTFIX);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
             response);
        error_no = CME_ERROR_TXT_STR_TOO_LONG;
        goto err;
    }

    if (insert_uart_send_msg(response, NULL, strlen(response), 0) != 0) {
        LOGE(TAG, "%s %d insert uart send msg fail\r\n", __func__, __LINE__);
        goto err;
    }

    return 0;

err:
    notify_atcmd_cme_error(error_no);
    return -1;
}

static int atcmd_imqtt_pubin()
{
    return -1;
}

static int atcmd_imqtt_rcvpubin()
{
    return -1;
}

static int atcmd_imqtt_dbgpara()
{
    return -1;
}

static int atcmd_imqtt_pingrsp()
{
    return -1;
}

enum
{
    ATCMD_IMQTT_MODE = 0,
    ATCMD_IMQTT_OPEN,
    ATCMD_IMQTT_AUTH,
    ATCMD_IMQTT_PARA,
    ATCMD_IMQTT_CONN,
    ATCMD_IMQTT_PUB,
    ATCMD_IMQTT_PUBIN,
    ATCMD_IMQTT_SUB,
    ATCMD_IMQTT_UNSUB,
    ATCMD_IMQTT_RCVPUB,
    ATCMD_IMQTT_RCVPUBIN,
    ATCMD_IMQTT_STATE,
    ATCMD_IMQTT_DISCONN,
    ATCMD_IMQTT_DBGPARA,
    ATCMD_IMQTT_PINGRSP,
};

static const atcmd_hdl_t at_mqtt_cmds_table[] = {
    { .name     = "AT+IMQTTMODE",
      .help     = "AT+IMQTTMODE=<mode>",
      .function = atcmd_imqtt_mode },
    { .name     = "AT+IMQTTOPEN",
      .help     = "AT+IMQTTOPEN=<hostname>,<port>",
      .function = atcmd_imqtt_open },
    { .name     = "AT+IMQTTAUTH",
      .help     = "AT+IMQTTAUTH=<ParaValue1>,<ParaValue2>,...<ParaValuen>",
      .function = atcmd_imqtt_auth },
    { .name     = "AT+IMQTTPARA",
      .help     = "AT+IMQTTPARA=<ParaTag>,<ParaValue>",
      .function = atcmd_imqtt_para },
    { .name     = "AT+IMQTTCONN",
      .help     = "AT+IMQTTCONN",
      .function = atcmd_imqtt_conn },
    { .name     = "AT+IMQTTPUB",
      .help     = "AT+IMQTTPUB=<topic>,<qos>,<message>,[format],[fragment_id]",
      .function = atcmd_imqtt_pub },
    { .name     = "AT+IMQTTPUBIN",
      .help     = "AT+IMQTTPUBIN=<topic>,<qos>,<msg_len>",
      .function = atcmd_imqtt_pubin },
    { .name     = "AT+IMQTTSUB",
      .help     = "AT+IMQTTSUB=<topic>,<qos>",
      .function = atcmd_imqtt_sub },
    { .name     = "AT+IMQTTUNSUB",
      .help     = "AT+IMQTTUNSUB=<topic>",
      .function = atcmd_imqtt_unsub },
    { .name     = "AT+IMQTTRCVPUB",
      .help     = "AT+IMQTTRCVPUB?",
      .function = atcmd_imqtt_rcvpub },
    { .name     = "AT+IMQTTRCVPUBIN",
      .help     = "AT+IMQTTRCVPUBIN?",
      .function = atcmd_imqtt_rcvpubin },
    { .name     = "AT+IMQTTSTATE",
      .help     = "AT+IMQTTSTATE?",
      .function = atcmd_imqtt_state },
    { .name     = "AT+IMQTTDISCONN",
      .help     = "AT+IMQTTDISCONN",
      .function = atcmd_imqtt_disconn },
    { .name     = "AT+IMQTTDBGPARA",
      .help     = "AT+IMQTTDBGPARA=<ParaTag>,<ParaValue>",
      .function = atcmd_imqtt_dbgpara },
    { .name = "AT+IMQTTPINGRSP", .help = "", .function = atcmd_imqtt_pingrsp },
};

static atcmd_hdl_ptr_t get_atcmd_mqtt_handler()
{
    const char *atcmd_prefix                 = "QTT";
    char        prefix[MAX_ATCMD_PREFIX_LEN] = { 0 };
    char        single;
    int         index = -1;

    // Eat "QTT"
    at.parse(prefix, strlen(atcmd_prefix));
    if (memcmp(prefix, atcmd_prefix, strlen(atcmd_prefix)) != 0) {
        goto err;
    }

    at.parse(&single, 1);
    memset(prefix, 0, sizeof(prefix));

    switch (single) {
        case 'M':
            at.parse(prefix, 3);

            if (memcmp(prefix, "ODE", 3) != 0)
                goto err;

            index = ATCMD_IMQTT_MODE;
            break;
        case 'O':
            at.parse(prefix, 3);

            if (memcmp(prefix, "PEN", 3) != 0)
                goto err;

            index = ATCMD_IMQTT_OPEN;
            break;
        case 'A':
            at.parse(prefix, 3);

            if (memcmp(prefix, "UTH", 3) != 0)
                goto err;

            index = ATCMD_IMQTT_AUTH;
            break;
        case 'P':
            at.parse(prefix, 2);

            if (memcmp(prefix, "AR", 2) == 0) {
                at.parse(&single, 1);

                if (single != 'A')
                    goto err;

                index = ATCMD_IMQTT_PARA;
            } else if (memcmp(prefix, "UB", 2) == 0) {
                index = ATCMD_IMQTT_PUB;
            } else if (memcmp(prefix, "IN", 2) == 0) {
                index = ATCMD_IMQTT_PINGRSP;
            } else {
                goto err;
            }

            break;
        case 'C':
            at.parse(prefix, 3);

            if (memcmp(prefix, "ONN", 3) != 0)
                goto err;

            index = ATCMD_IMQTT_CONN;
            break;
        case 'S':
            at.parse(prefix, 2);

            if (memcmp(prefix, "UB", 2) == 0) {
                index = ATCMD_IMQTT_SUB;
            } else if (memcmp(prefix, "TA", 2) == 0) {
                memset(prefix, 0, sizeof(prefix));
                at.parse(prefix, 2);

                if (memcmp(prefix, "TE", 2) != 0)
                    goto err;

                index = ATCMD_IMQTT_STATE;
            } else {
                goto err;
            }

            break;
        case 'U': //"UNSUB":
            at.parse(prefix, 4);

            if (memcmp(prefix, "NSUB", 4) != 0)
                goto err;

            index = ATCMD_IMQTT_UNSUB;
            break;
        case 'R': //"CVPUB":
            at.parse(prefix, 5);

            if (memcmp(prefix, "CVPUB", 5) != 0)
                goto err;

            index = ATCMD_IMQTT_RCVPUB;
            break;
        case 'D': //"DIS": "DBG":
            at.parse(prefix, 6);

            if (memcmp(prefix, "ISCONN", 6) == 0) {
                index = ATCMD_IMQTT_DISCONN;
            } else if (memcmp(prefix, "BGPARA", 6) == 0) {
                index = ATCMD_IMQTT_DBGPARA;
            } else {
                goto err;
            }

            break;
        default:
            goto err;
    }

    if (index >= 0 && index < sizeof(at_mqtt_cmds_table)) {
        return &at_mqtt_cmds_table[index];
    }

err:
    LOGE(TAG, "Invalid mqtt prefix %s\n", prefix);
    return NULL;
}

static int mqtt_init()
{
    memset(&m_mqtt_config, 0, sizeof(m_mqtt_config));
    m_mqtt_config.mode = MQTT_DEFAULT_TLS_MODE;
    strcpy(m_mqtt_config.productkey, PRODUCT_KEY);
    strcpy(m_mqtt_config.devicename, DEVICE_NAME);
    strcpy(m_mqtt_config.devicesecret, DEVICE_SECRET);

    m_mqtt_config.timeout   = MQTT_DEFAULT_REQ_TIMOUT_MS;
    m_mqtt_config.clean     = MQTT_DEFAULT_CLEAN_SESSION;
    m_mqtt_config.keepalive = MQTT_DEFAULT_KEEPALIVE_MS;
    strcpy(m_mqtt_config.version, MQTT_DEFAULT_VERSION);
    m_mqtt_config.authmode = MQTT_DEFAULT_AUTH_MODE;

    memset(&m_last_pub_msg, 0, sizeof(m_last_pub_msg));
    memset(&m_last_sub_topic, 0, sizeof(m_last_sub_topic));
    memset(&m_last_unsub_topic, 0, sizeof(m_last_unsub_topic));

    return 0;
}

static void mqtt_deinit()
{
    return;
}

atcmd_op_t mqtt_op = {
    .name              = "mqtt",
    .prefix            = "IM",
    .init              = mqtt_init,
    .deinit            = mqtt_deinit,
    .get_atcmd_handler = get_atcmd_mqtt_handler,
};

int register_atcmd_mqtt_opt()
{
    return atcmd_register_handlers(&mqtt_op);
}

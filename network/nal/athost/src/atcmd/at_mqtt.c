/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aos/kernel.h"
#include "ulog/ulog.h"

#include "dev_sign_api.h"
#include "mqtt_api.h"
#include "wrappers.h"
#include "infra_compat.h"

#include "at_util.h"

#define TAG "atmqtt"

#define MQTT_DEFUALT_YIELD_TIMEOUT_MS 200
#define MQTT_DEFAULT_TLS_MODE      1
#define MQTT_DEFAULT_REQ_TIMOUT_MS 2000
#define MQTT_DEFAULT_CLEAN_SESSION 0
#define MQTT_DEFAULT_KEEPALIVE_MS  60000
#define MQTT_DEFAULT_VERSION       "3.1"
#define MQTT_DEFAULT_VERSION_ALT   "3.1.1"
#define MQTT_DEFAULT_AUTH_MODE     1

#define MQTT_DEFAULT_TOPIC_UPDATE "/" PRODUCT_KEY "/" DEVICE_NAME "/update"
#define MQTT_DEFAULT_TOPIC_ERROR "/" PRODUCT_KEY "/" DEVICE_NAME "/update/error"
#define MQTT_DEFAULT_TOPIC_GET   "/" PRODUCT_KEY "/" DEVICE_NAME "/get"

#define MQTT_VERSION_MAX_LEN 10
#define MQTT_MSG_MAX_LEN     1024
#define MQTT_TOPIC_MAX_LEN   200
#define MQTT_PREFIX_MAX_LEN  100
#define MAX_ATCMD_MQTT_MSG_RSP_LEN \
    (MQTT_PREFIX_MAX_LEN + MQTT_MSG_MAX_LEN + MQTT_TOPIC_MAX_LEN)
static char mqtt_msg_rsp[MAX_ATCMD_MQTT_MSG_RSP_LEN] = { 0 };
static char mqtt_msg_notify[MAX_ATCMD_MQTT_MSG_RSP_LEN] = { 0 };

typedef struct
{
    int  mode; // 0: non-TLS 1: TLS
    char hostname[MAX_ATCMD_DOMAIN_LEN];
    int  port;

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
static void             *m_pclient        = NULL;

static mqtt_pub_msg_t   m_last_pub_msg;
static mqtt_pub_msg_t   m_frag_pub_msg;
static mqtt_pub_msg_t   m_last_recvpub_msg;
static mqtt_sub_topic_t m_last_sub_topic;
static mqtt_sub_topic_t m_last_unsub_topic;

static mqtt_pub_msg_t   m_attempt_pub_msg;
static mqtt_sub_topic_t m_attempt_sub_topic;
static mqtt_sub_topic_t m_attempt_unsub_topic;

static bool m_yield_task_start =  false;
static aos_mutex_t m_comm_mutex;

#define MIN_INTERVAL_CON_DISCON_MS 1000
static long long m_connect_time = -1;

/*
 *  Common const string
 */
#define MAX_INTEGER_STR_LEN 10
#define ONE_DIGIT_STR_LEN   1
const static char *PARA_TAG_STR  = "\"ParaTag\"";
const static char *PARA_VAL_STR  = "\"ParaValue\"";
const static char *SEPARATOR_STR = ",";
const static char *QUOTE_STR     = "\"";

static int notify_mqtt_pub_result(int packet_id, int result)
{
    const char *pub_result_preifx = "+IMQTTPUB:";
    int         offset            = 0;
    char       *response          = mqtt_msg_notify;

    memset(response, 0, MAX_ATCMD_MQTT_MSG_RSP_LEN);

    if (result < 0 || result > 2) {
        LOGE(TAG, "invalid pub result %d\n", result);
        goto err;
    }

    // prefix
    if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_MQTT_MSG_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_MSG_RSP_LEN - offset, "%s",
                   AT_RECV_PREFIX);
    } else {
        LOGE(TAG, "at string too long %s\n", response);
        goto err;
    }

    // result
    if (offset + strlen(pub_result_preifx) + MAX_INTEGER_STR_LEN * 2 +
          strlen(SEPARATOR_STR) + strlen(AT_RECV_PREFIX) <
        MAX_ATCMD_MQTT_MSG_RSP_LEN) {
        offset += snprintf(
          response + offset, MAX_ATCMD_MQTT_MSG_RSP_LEN - offset, "%s%d%s%d%s",
          pub_result_preifx, packet_id, SEPARATOR_STR, result, AT_RECV_PREFIX);
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

static int notify_mqtt_sub_result(int packet_id, int result)
{
    const char *sub_result_preifx = "+IMQTTSUB:";
    int         offset            = 0;
    char       *response          = mqtt_msg_notify;

    memset(response, 0, MAX_ATCMD_MQTT_MSG_RSP_LEN);

    if ((result < 0 || result > 2) && result != 128) {
        LOGE(TAG, "invalid sub result %d\n", result);
        goto err;
    }

    // prefix
    if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_MQTT_MSG_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_MSG_RSP_LEN - offset, "%s",
                   AT_RECV_PREFIX);
    } else {
        LOGE(TAG, "at string too long %s\n", response);
        goto err;
    }

    // result
    if (offset + strlen(sub_result_preifx) + MAX_INTEGER_STR_LEN * 2 +
          strlen(SEPARATOR_STR) + strlen(AT_RECV_PREFIX) <
        MAX_ATCMD_MQTT_MSG_RSP_LEN) {
        offset += snprintf(
          response + offset, MAX_ATCMD_MQTT_MSG_RSP_LEN - offset, "%s%d%s%d%s",
          sub_result_preifx, packet_id, SEPARATOR_STR, result, AT_RECV_PREFIX);
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

static int notify_mqtt_unsub_result(int packet_id, int result)
{
    const char *unsub_result_preifx = "+IMQTTUNSUB:";
    int         offset              = 0;
    char       *response            = mqtt_msg_notify;

    memset(response, 0, MAX_ATCMD_MQTT_MSG_RSP_LEN);

    if (result < 0 || result > 2) {
        LOGE(TAG, "invalid unsub result %d\n", result);
        goto err;
    }

    // prefix
    if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_MQTT_MSG_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_MSG_RSP_LEN - offset, "%s",
                   AT_RECV_PREFIX);
    } else {
        LOGE(TAG, "at string too long %s\n", response);
        goto err;
    }

    // result
    if (offset + strlen(unsub_result_preifx) + MAX_INTEGER_STR_LEN * 2 +
          strlen(SEPARATOR_STR) + strlen(AT_RECV_PREFIX) <
        MAX_ATCMD_MQTT_MSG_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_MSG_RSP_LEN - offset,
                   "%s%d%s%d%s", unsub_result_preifx, packet_id, SEPARATOR_STR,
                   result, AT_RECV_PREFIX);
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

static int notify_mqtt_state(int state)
{
    const char *mqtt_state_preifx = "+IMQTTSTATE:";
    int         offset            = 0;
    char       *response          = mqtt_msg_notify;

    memset(response, 0, MAX_ATCMD_MQTT_MSG_RSP_LEN);

    if (state < 0 || state > 2) {
        LOGE(TAG, "invalid mqtt state %d\n", state);
        goto err;
    }

    // prefix
    if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_MQTT_MSG_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_MSG_RSP_LEN - offset, "%s",
                   AT_RECV_PREFIX);
    } else {
        LOGE(TAG, "at string too long %s\n", response);
        goto err;
    }

    // state
    if (offset + strlen(mqtt_state_preifx) + MAX_INTEGER_STR_LEN <
        MAX_ATCMD_MQTT_MSG_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_MSG_RSP_LEN - offset,
                   "%s%d", mqtt_state_preifx, state);
    } else {
        LOGE(TAG, "at string too long %s\n", response);
        goto err;
    }

    // AT_RECV_PREFIX
    if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_MQTT_MSG_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_MSG_RSP_LEN - offset, "%s",
                   AT_RECV_PREFIX);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
             response);
        goto err;
    }

    // AT_RECV_SUCCESS_POSTFIX
    if (offset + strlen(AT_RECV_SUCCESS_POSTFIX) < MAX_ATCMD_MQTT_MSG_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_MSG_RSP_LEN - offset, "%s",
                   AT_RECV_SUCCESS_POSTFIX);
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

static void subscribed_msg_arrive_handle(void *pcontext, void *pclient,
                                         iotx_mqtt_event_msg_pt pmsg);

static void mqtt_handle_event(void *pcontext, void *pclient,
                              iotx_mqtt_event_msg_pt pmsg)
{
    iotx_mqtt_topic_info_pt topic_info;
    unsigned int            packet_id;

    if (NULL == pmsg) {
        LOGE(TAG, "mqtt handle event error NULL");
        return;
    }

    switch (pmsg->event_type) {
        case IOTX_MQTT_EVENT_UNDEF:
            LOGD(TAG, "undefined event occur.");
            break;

        case IOTX_MQTT_EVENT_DISCONNECT:
            LOGD(TAG, "MQTT disconnect.");
            notify_mqtt_state(MQTT_STATE_DISCON);
            break;

        case IOTX_MQTT_EVENT_RECONNECT:
            LOGD(TAG, "MQTT reconnect.");
            notify_mqtt_state(MQTT_STATE_CONNEC);
            break;

        case IOTX_MQTT_EVENT_SUBCRIBE_SUCCESS:
            packet_id = (unsigned int)pmsg->msg;

            // TODO: QoS value pending
            if (m_last_sub_topic.valid) {
                notify_mqtt_sub_result(packet_id, m_last_sub_topic.qos);
                LOGD(TAG, "subscribe success, packet-id=%u", packet_id);
            } else {
                LOGE(TAG, "Invalid subsribe topic!\n", m_last_sub_topic);
            }
            break;

        case IOTX_MQTT_EVENT_SUBCRIBE_TIMEOUT:
            packet_id = (unsigned int)pmsg->msg;

            notify_mqtt_sub_result(packet_id, MQTT_SUB_FAIL);
            LOGD(TAG, "subscribe wait ack timeout, packet-id=%u", packet_id);
            break;

        case IOTX_MQTT_EVENT_SUBCRIBE_NACK:
            packet_id = (unsigned int)pmsg->msg;

            notify_mqtt_sub_result(packet_id, MQTT_SUB_FAIL);
            LOGD(TAG, "subscribe nack, packet-id=%u", packet_id);
            break;

        case IOTX_MQTT_EVENT_UNSUBCRIBE_SUCCESS:
            packet_id = (unsigned int)pmsg->msg;

            notify_mqtt_unsub_result(packet_id, MQTT_UNSUB_SUCCESS);
            LOGD(TAG, "unsubscribe success, packet-id=%u", packet_id);
            break;

        case IOTX_MQTT_EVENT_UNSUBCRIBE_TIMEOUT:
            packet_id = (unsigned int)pmsg->msg;

            notify_mqtt_unsub_result(packet_id, MQTT_UNSUB_TIMEOUT);
            LOGD(TAG, "unsubscribe timeout, packet-id=%u", packet_id);
            break;

        case IOTX_MQTT_EVENT_UNSUBCRIBE_NACK:
            packet_id = (unsigned int)pmsg->msg;

            notify_mqtt_unsub_result(packet_id, MQTT_UNSUB_OTHER_ERR);
            LOGD(TAG, "unsubscribe nack, packet-id=%u", packet_id);
            break;

        case IOTX_MQTT_EVENT_PUBLISH_SUCCESS:
            packet_id = (unsigned int)pmsg->msg;

            notify_mqtt_pub_result(packet_id, MQTT_PUB_SUCCESS);
            LOGD(TAG, "publish success, packet-id=%u", packet_id);
            break;

        case IOTX_MQTT_EVENT_PUBLISH_TIMEOUT:
            packet_id = (unsigned int)pmsg->msg;

            notify_mqtt_pub_result(packet_id, MQTT_PUB_TIMEOUT);
            LOGD(TAG, "publish timeout, packet-id=%u", packet_id);
            break;

        case IOTX_MQTT_EVENT_PUBLISH_NACK:
            packet_id = (unsigned int)pmsg->msg;

            notify_mqtt_pub_result(packet_id, MQTT_PUB_OTHER_ERR);
            LOGD(TAG, "publish nack, packet-id=%u", packet_id);
            break;

        case IOTX_MQTT_EVENT_PUBLISH_RECEIVED:
            topic_info = (iotx_mqtt_topic_info_pt)pmsg->msg;

            if (memcmp(topic_info->ptopic, m_last_sub_topic.topic,
                       topic_info->topic_len) == 0) {
                subscribed_msg_arrive_handle(NULL, NULL, pmsg);
            } else {
                LOGD(TAG, "topic message arrived but without any related handle: "
                "topic=%.*s, topic_msg=%.*s ",
                (int)topic_info->topic_len, topic_info->ptopic,
                (int)topic_info->payload_len, topic_info->payload);
            }

            break;

        default:
            LOGE(TAG, "MQTT even handle: should NOT arrive here.");
            break;
    }
}

static void mqtt_yield_task()
{
    if (m_yield_task_start) {
        LOGW(TAG, "Yield task already run!\n");
        return;
    }

    if (!m_pclient) {
        LOGE(TAG, "Invalid mqtt pclient\n");
        goto exit;
    }

    m_yield_task_start = true;
    m_connect_time = aos_now_ms();
    LOGD(TAG, "New mqtt yield task starts\n");
    notify_mqtt_state(MQTT_STATE_CONNEC);

    aos_mutex_lock(&m_comm_mutex, AOS_WAIT_FOREVER);
    while (1) {
        if (!m_yield_task_start) {
            break;
        }

        if (!IOT_MQTT_CheckStateNormal(m_pclient)) {
            LOGD(TAG, "mqtt disconnect!\n");
            notify_mqtt_state(MQTT_STATE_DISCON);
        }

        IOT_MQTT_Yield(m_pclient, MQTT_DEFUALT_YIELD_TIMEOUT_MS);
    }
    aos_mutex_unlock(&m_comm_mutex);

exit:
    m_yield_task_start = false;

    LOGD(TAG, "Mqtt yield task exits\n");
}

static int post_mqtt_yield_task()
{
    aos_task_t  task;

    if (aos_task_new_ext(&task, "mqtt_yield_task", mqtt_yield_task,
                         NULL, 6144, AOS_DEFAULT_APP_PRI - 1) != 0) {
        LOGE(TAG, "Fail to create mqtt yield task\r\n");
        return -1;
    }

    return 0;
}

static int notify_mqtt_rcvpub_msg()
{
    const char *mqtt_rcvpub_prefix = "+IMQTTRCVPUB:";
    int         offset   = 0;
    char        *response = mqtt_msg_notify;

    memset(response, 0, MAX_ATCMD_MQTT_MSG_RSP_LEN);

    // AT_RECV_PREFIX
    if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_MQTT_MSG_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_MSG_RSP_LEN - offset, "%s",
                   AT_RECV_PREFIX);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
             response);
        goto err;
    }

    // prefix
    if (offset + strlen(mqtt_rcvpub_prefix) < MAX_ATCMD_MQTT_MSG_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_MSG_RSP_LEN - offset, "%s",
                   mqtt_rcvpub_prefix);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
             response);
        goto err;
    }

    // PACKETID + TOPIC + MSG_LEN + MESSAGE + [FORMAT] + [FRAGMENT_ID]
    if (offset + strlen(m_last_recvpub_msg.topic_pub) +
          strlen(m_last_pub_msg.msg_pub) + MAX_INTEGER_STR_LEN * 3 +
          ONE_DIGIT_STR_LEN + strlen(SEPARATOR_STR) * 5 +
          strlen(QUOTE_STR) * 4 <
        MAX_ATCMD_MQTT_MSG_RSP_LEN) {
        if (m_last_recvpub_msg.valid) {
            offset += snprintf(
              response + offset, MAX_ATCMD_MQTT_MSG_RSP_LEN - offset,
              "%d%s\"%s\"%s%d%s\"%s\"", m_last_recvpub_msg.packet_id,
              SEPARATOR_STR, m_last_recvpub_msg.topic_pub, SEPARATOR_STR,
              strlen(m_last_recvpub_msg.msg_pub), SEPARATOR_STR,
              m_last_recvpub_msg.msg_pub);
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
    if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_MQTT_MSG_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_MSG_RSP_LEN - offset, "%s",
                   AT_RECV_PREFIX);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
             response);
        goto err;
    }

    // AT_RECV_SUCCESS_POSTFIX
    if (offset + strlen(AT_RECV_SUCCESS_POSTFIX) < MAX_ATCMD_MQTT_MSG_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_MSG_RSP_LEN - offset, "%s",
                   AT_RECV_SUCCESS_POSTFIX);
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
             (int)ptopic_info->topic_len, (int)ptopic_info->payload_len);
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
    LOG("Topic: '%.*s' (Length: %d)", (int)ptopic_info->topic_len,
        ptopic_info->ptopic, (int)ptopic_info->topic_len);
    LOG("Payload: '%.*s' (Length: %d)", (int)ptopic_info->payload_len,
        ptopic_info->payload, (int)ptopic_info->payload_len);
    LOG("----");

    // notfiy
    notify_mqtt_rcvpub_msg();

    return;
}

static int mqtt_subscribe(mqtt_sub_topic_t *sub_topic)
{
    int ret = 0;

    if (NULL == sub_topic || !sub_topic->valid) {
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

    if (NULL == unsub_topic || !unsub_topic->valid) {
        return -1;
    }

    ret = IOT_MQTT_Unsubscribe(m_pclient, unsub_topic->topic);
    if (ret >= 0) {
        memcpy(&m_last_unsub_topic, unsub_topic, sizeof(m_last_unsub_topic));
    }

    return ret;
}

static int mqtt_publish_msg(mqtt_pub_msg_t *pub_msg)
{
    int                    ret = 0;
    iotx_mqtt_topic_info_t topic_msg;

    if (NULL == pub_msg || !pub_msg->valid) {
        return -1;
    }

    if (pub_msg->fragment_id != 0 ||
        m_frag_pub_msg.fragment_id != 0) {
        int offset;
        // first time
        if (m_frag_pub_msg.fragment_id == 0) {
            memcpy(&m_frag_pub_msg, pub_msg, sizeof(mqtt_pub_msg_t));
        } else if (m_frag_pub_msg.fragment_id - 1 != pub_msg->fragment_id) {
            LOGE(TAG, "expect frag id %d but recv %d \n",
                    m_frag_pub_msg.fragment_id - 1,
                    pub_msg->fragment_id);
            goto err;
        }

        if (m_frag_pub_msg.fragment_id - 1 == pub_msg->fragment_id) {
            // compare topic, qos, format
            if (memcmp(m_frag_pub_msg.topic_pub, pub_msg->topic_pub,
                strlen(m_frag_pub_msg.topic_pub)) != 0) {
                LOGE(TAG, "unmatched frag topic %s\n", pub_msg->topic_pub);
                goto err;
            }

            if (m_frag_pub_msg.qos != pub_msg->qos ||
                m_frag_pub_msg.msg_format != pub_msg->msg_format) {
                LOGE(TAG, "unmatched frag qos %d format %d\n",
                        pub_msg->qos, pub_msg->msg_format);
                goto err;
            }

            offset = strlen(m_frag_pub_msg.msg_pub);
            if (offset + strlen(pub_msg->msg_pub) >= sizeof(m_frag_pub_msg.msg_pub)) {
                LOGE(TAG, "frag message exceeds max len %d\n", sizeof(m_frag_pub_msg.msg_pub));
                goto err;
            }

            memcpy(m_frag_pub_msg.msg_pub + offset, pub_msg->msg_pub, strlen(pub_msg->msg_pub));
            m_frag_pub_msg.fragment_id = pub_msg->fragment_id;
        }
    } else {
        memcpy(&m_frag_pub_msg, pub_msg, sizeof(mqtt_pub_msg_t));
    }

    // more fragment
    if (m_frag_pub_msg.fragment_id != 0) {
        return 0;
    }

    memset(&topic_msg, 0, sizeof(topic_msg));
    topic_msg.qos         = m_frag_pub_msg.qos;
    topic_msg.payload     = (void *)m_frag_pub_msg.msg_pub;
    topic_msg.payload_len = strlen(m_frag_pub_msg.msg_pub);

    ret = IOT_MQTT_Publish(m_pclient, m_frag_pub_msg.topic_pub, &topic_msg);
    if (ret >= 0) {
        memcpy(&m_last_pub_msg, &m_frag_pub_msg, sizeof(mqtt_pub_msg_t));
    } else {
        static int publish_fail = 0;
        LOGE(TAG, "publish fail count %d\n", ++publish_fail);
        HAL_SleepMs(100);
    }

    memset(&m_frag_pub_msg, 0, sizeof(mqtt_pub_msg_t));
    return ret;

err:
    memset(&m_frag_pub_msg, 0, sizeof(mqtt_pub_msg_t));
    return -1;
}

static int notify_mqtt_auth_result(int result)
{
    const char *auth_result_preifx = "+IMQTTAUTH:";
    char       *auth_result;
    int         offset   = 0;
    char       *response = mqtt_msg_notify;

    memset(response, 0, MAX_ATCMD_MQTT_MSG_RSP_LEN);

    if (result == MQTT_AUTH_FAIL) {
        auth_result = "FAIL";
    } else if (result == MQTT_AUTH_SUCCESS) {
        auth_result = "OK";
    } else {
        LOGE(TAG, "invalid auth result %d\n", result);
        goto err;
    }

    // prefix
    if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_MQTT_MSG_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_MSG_RSP_LEN - offset, "%s",
                   AT_RECV_PREFIX);
    } else {
        LOGE(TAG, "at string too long %s\n", response);
        goto err;
    }

    // result
    if (offset + strlen(auth_result_preifx) + strlen(auth_result) +
          strlen(AT_RECV_PREFIX) <
        MAX_ATCMD_MQTT_MSG_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_MSG_RSP_LEN - offset,
                   "%s%s%s", auth_result_preifx, auth_result, AT_RECV_PREFIX);
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

static void mqtt_authenticate()
{
    int ret    = 0;
    int notify = MQTT_AUTH_SUCCESS;
    char productkey[IOTX_PRODUCT_KEY_LEN]     = { 0 };
    char devicename[IOTX_DEVICE_NAME_LEN]     = { 0 };
    char devicesecret[IOTX_DEVICE_SECRET_LEN] = { 0 };

    HAL_GetProductKey(productkey);
    HAL_GetDeviceName(devicename);
    HAL_GetDeviceSecret(devicesecret);

    if (0 != IOT_SetupConnInfo(
               productkey, devicename,
               devicesecret, (void **)&m_mqtt_conn_info)) {
        LOGE(TAG, "authenticate request failed!");
        ret = -1;
    }

    if (ret != 0)
        notify = MQTT_AUTH_FAIL;

    notify_mqtt_auth_result(notify);

    return;
}

static int post_mqtt_authenticate_task()
{
    if (aos_task_new("mqtt_authenticate_task", mqtt_authenticate, NULL, 5120) !=
        0) {
        LOGE(TAG, "Fail to create mqtt authenticate task\r\n");
        return -1;
    }

    return 0;
}

static void mqtt_connect()
{
    iotx_mqtt_param_t mqtt_params;
    int error_no = CME_ERROR_UNKNOWN;

    if (m_mqtt_conn_info == NULL) {
        LOGE(TAG, "Conn info not ready yet!\n");
        error_no = CME_ERROR_INCORRECT_PARA;
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

    mqtt_params.read_buf_size     = MQTT_MSG_MAX_LEN;
    mqtt_params.write_buf_size    = MQTT_MSG_MAX_LEN;
    mqtt_params.handle_event.h_fp = mqtt_handle_event;

    mqtt_params.handle_event.pcontext = NULL;

    /* Construct a MQTT client with specify parameter */
    if ((m_pclient = IOT_MQTT_Construct(&mqtt_params)) == NULL) {
        LOGE(TAG, "MQTT construct failed!\n");
        error_no = CME_ERROR_NET_TIMEOUT;
        goto err;
    }

    post_mqtt_yield_task();

    return;

err:
    notify_atcmd_cme_error(error_no);
    return;
}

static int post_mqtt_connect_task()
{
    if (aos_task_new("mqtt_connect_task", mqtt_connect, NULL, 5120) != 0) {
        LOGE(TAG, "Fail to create mqtt connect task\r\n");
        return -1;
    }

    return 0;
}


const static char *TAG_MODE_STR = "\"mode\"";
static int         atcmd_imqtt_mode()
{
    const char *mqtt_mode_prefix = "+IMQTTMODE:";
    char        single, mode;
    int         offset   = 0;
    char       *response = mqtt_msg_rsp;
    int         error_no = CME_ERROR_UNKNOWN;

    memset(response, 0, MAX_ATCMD_MQTT_MSG_RSP_LEN);

    atcmd_read(&single, 1);
    if ('=' != single && '?' != single) {
        LOGE(TAG, "Invalid mqtt mode prefix %c!\n", single);
        error_no = CME_ERROR_TXT_STR_INVALID_CHAR;
        goto err;
    }

    // AT_RECV_PREFIX
    if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_MQTT_MSG_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_MSG_RSP_LEN - offset, "%s",
                   AT_RECV_PREFIX);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
             response);
        error_no = CME_ERROR_TXT_STR_TOO_LONG;
        goto err;
    }

    if ('?' == single) {
        mode = m_mqtt_config.mode ? '1' : '0';

        // MQTTMODE prefix + mode
        if (offset + strlen(mqtt_mode_prefix) + ONE_DIGIT_STR_LEN <
            MAX_ATCMD_MQTT_MSG_RSP_LEN) {
            offset +=
              snprintf(response + offset, MAX_ATCMD_MQTT_MSG_RSP_LEN - offset,
                       "%s%c", mqtt_mode_prefix, mode);
        } else {
            LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                 response);
            error_no = CME_ERROR_TXT_STR_TOO_LONG;
            goto err;
        }

        // AT_RECV_PREFIX
        if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_MQTT_MSG_RSP_LEN) {
            offset +=
              snprintf(response + offset, MAX_ATCMD_MQTT_MSG_RSP_LEN - offset,
                       "%s", AT_RECV_PREFIX);
        } else {
            LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                 response);
            error_no = CME_ERROR_TXT_STR_TOO_LONG;
            goto err;
        }
    } else if ('=' == single) {
        atcmd_read(&mode, 1);

        if ('?' == mode) {
            // MQTTMODE prefix + \"mode\"
            if (offset + strlen(mqtt_mode_prefix) + strlen(TAG_MODE_STR) <
                MAX_ATCMD_MQTT_MSG_RSP_LEN) {
                offset += snprintf(response + offset,
                                   MAX_ATCMD_MQTT_MSG_RSP_LEN - offset, "%s%s",
                                   mqtt_mode_prefix, TAG_MODE_STR);
            } else {
                LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                     response);
                error_no = CME_ERROR_TXT_STR_TOO_LONG;
                goto err;
            }

            if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_MQTT_MSG_RSP_LEN) {
                offset += snprintf(response + offset,
                                   MAX_ATCMD_MQTT_MSG_RSP_LEN - offset, "%s",
                                   AT_RECV_PREFIX);
            } else {
                LOGE(TAG, "at string too long %s\n", response);
                error_no = CME_ERROR_TXT_STR_TOO_LONG;
                goto err;
            }
        } else if ('0' == mode || '1' == mode) {
            atcmd_read(&single, 1);
            if (single == '\r') {
                m_mqtt_config.mode = mode - '0';
            } else {
                LOGE(TAG, "invalid mqtt mode %c%c\n", mode, single);
                error_no = CME_ERROR_INCORRECT_PARA;
                goto err;
            }
        } else {
            LOGE(TAG, "invalid mqtt mode %c\n", mode);
            error_no = CME_ERROR_INCORRECT_PARA;
            goto err;
        }
    } else {
        LOGE(TAG, "Invalid mqtt mode prefix %c!\n", single);
        error_no = CME_ERROR_INCORRECT_PARA;
        goto err;
    }

    // AT_RECV_SUCCESS_POSTFIX
    if (offset + strlen(AT_RECV_SUCCESS_POSTFIX) < MAX_ATCMD_MQTT_MSG_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_MSG_RSP_LEN - offset, "%s",
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

const static char *TAG_HOSTNAME_STR = "\"hostname\"";
const static char *TAG_PORT_STR     = "\"port\"";
static int         atcmd_imqtt_open()
{
    const char *mqtt_open_prefix = "+IMQTTOPEN:";
    char        single;
    char        hostname[MAX_ATCMD_DOMAIN_LEN] = { 0 };
    int         port;
    char        port_str[MAX_ATCMD_PORT_LEN] = { 0 };
    int         ret;
    int         offset   = 0;
    char       *response = mqtt_msg_rsp;

    memset(response, 0, MAX_ATCMD_MQTT_MSG_RSP_LEN);

    atcmd_read(&single, 1);
    if ('=' != single && '?' != single) {
        LOGE(TAG, "Invalid mqtt open prefix %c!\n", single);
        goto err;
    }

    // AT_RECV_PREFIX
    if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_MQTT_MSG_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_MSG_RSP_LEN - offset, "%s",
                   AT_RECV_PREFIX);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
             response);
        goto err;
    }

    if ('?' == single) {
        // MQTTOPEN prefix + hostname + port
        if (offset + strlen(mqtt_open_prefix) + MAX_ATCMD_DOMAIN_LEN +
              MAX_ATCMD_PORT_LEN + strlen(QUOTE_STR) * 2 +
              strlen(SEPARATOR_STR) <
            MAX_ATCMD_MQTT_MSG_RSP_LEN) {
            offset +=
              snprintf(response + offset, MAX_ATCMD_MQTT_MSG_RSP_LEN - offset,
                       "%s\"%s\"%s%d", mqtt_open_prefix, m_mqtt_config.hostname,
                       SEPARATOR_STR, m_mqtt_config.port);
        } else {
            LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                 response);
            goto err;
        }

        // AT_RECV_PREFIX
        if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_MQTT_MSG_RSP_LEN) {
            offset +=
              snprintf(response + offset, MAX_ATCMD_MQTT_MSG_RSP_LEN - offset,
                       "%s", AT_RECV_PREFIX);
        } else {
            LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                 response);
            goto err;
        }
    } else if ('=' == single) {
        atcmd_read(&single, 1);

        if ('?' == single) {
            // MQTTOPEN prefix + \"hostname\" + \"port\"
            if (offset + strlen(mqtt_open_prefix) + strlen(TAG_HOSTNAME_STR) +
                  strlen(TAG_PORT_STR) + strlen(SEPARATOR_STR) <
                MAX_ATCMD_MQTT_MSG_RSP_LEN) {
                offset += snprintf(
                  response + offset, MAX_ATCMD_MQTT_MSG_RSP_LEN - offset,
                  "%s%s%s%s", mqtt_open_prefix, TAG_HOSTNAME_STR, SEPARATOR_STR,
                  TAG_PORT_STR);
            } else {
                LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                     response);
                goto err;
            }

            if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_MQTT_MSG_RSP_LEN) {
                offset += snprintf(response + offset,
                                   MAX_ATCMD_MQTT_MSG_RSP_LEN - offset, "%s",
                                   AT_RECV_PREFIX);
            } else {
                LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                     response);
                goto err;
            }
        } else {
            ret = atcmd_socket_data_info_get(hostname, sizeof(hostname), NULL);
            if (ret < 0) {
                LOGE(TAG, "Invalid hostname %s !\n", hostname);
                goto err;
            }

            // ignore "
            if (strlen(hostname) > 0 &&
                hostname[strlen(hostname) - 1] == '\"') {
                hostname[strlen(hostname) - 1] = '\0';
            } else {
                LOGE(TAG, "Invalid hostname %s !\n", hostname);
                goto err;
            }

            ret = atcmd_socket_data_info_get(port_str, sizeof(port_str),
                                             &atcmd_socket_data_len_check);
            if (ret < 0 || strlen(port_str) <= 0 || (port = atoi(port_str)) < 0) {
                LOGE(TAG, "Invalid port %s !\n", port_str);
                goto err;
            }

            LOGW(TAG, "imqtt open is currently not supported!");
            goto err;

#if IMQTT_OPEN_SUPPORT
            // set hostname, port
            memset(m_mqtt_config.hostname, 0, sizeof(m_mqtt_config.hostname));
            memcpy(m_mqtt_config.hostname, hostname, strlen(hostname));
            m_mqtt_config.port = port;
#endif
        }
    } else {
        LOGE(TAG, "Invalid mqtt mode prefix %c!\n", single);
        goto err;
    }

    // AT_RECV_SUCCESS_POSTFIX
    if (offset + strlen(AT_RECV_SUCCESS_POSTFIX) < MAX_ATCMD_MQTT_MSG_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_MSG_RSP_LEN - offset, "%s",
                   AT_RECV_SUCCESS_POSTFIX);
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

const static char *TAG_PRODUCT_KEY_STR   = "\"PRODUCT_KEY\"";
const static char *TAG_DEVICE_NAME_STR   = "\"DEVICE_NAME\"";
const static char *TAG_DEVICE_SECRET_STR = "\"DEVICE_SECRET\"";

static int atcmd_imqtt_auth()
{
    const char *mqtt_auth_prefix = "+IMQTTAUTH:";
    char        single;
    char        productkey[IOTX_PRODUCT_KEY_LEN + 2]     = { 0 };
    char        devicename[IOTX_DEVICE_NAME_LEN + 2]     = { 0 };
    char        devicesecret[IOTX_DEVICE_SECRET_LEN + 2] = { 0 };
    int         ret;
    int         error_no = CME_ERROR_UNKNOWN;
    int         offset   = 0;
    char       *response = mqtt_msg_rsp;

    memset(response, 0, MAX_ATCMD_MQTT_MSG_RSP_LEN);

    atcmd_read(&single, 1);
    if ('=' != single && '?' != single) {
        LOGE(TAG, "Invalid mqtt auth prefix %c!\n", single);
        error_no = CME_ERROR_TXT_STR_INVALID_CHAR;
        goto err;
    }

    // AT_RECV_PREFIX
    if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_MQTT_MSG_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_MSG_RSP_LEN - offset, "%s",
                   AT_RECV_PREFIX);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
             response);
        error_no = CME_ERROR_TXT_STR_TOO_LONG;
        goto err;
    }

    if ('?' == single) {
        // MQTTAUTH prefix + productkey + productkey + devicesecret
        if (offset + strlen(mqtt_auth_prefix) +
              IOTX_PRODUCT_KEY_LEN +
              IOTX_DEVICE_NAME_LEN +
              IOTX_DEVICE_SECRET_LEN + strlen(QUOTE_STR) * 6 +
              strlen(SEPARATOR_STR) * 2 <
            MAX_ATCMD_MQTT_MSG_RSP_LEN) {
            HAL_GetProductKey(productkey);
            HAL_GetDeviceName(devicename);
            HAL_GetDeviceSecret(devicesecret);

            offset += snprintf(
              response + offset, MAX_ATCMD_MQTT_MSG_RSP_LEN - offset,
              "%s\"%s\"%s\"%s\"%s\"%s\"", mqtt_auth_prefix,
              productkey, SEPARATOR_STR, devicename,
              SEPARATOR_STR, devicesecret);
        } else {
            LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                 response);
            error_no = CME_ERROR_TXT_STR_TOO_LONG;
            goto err;
        }

        // AT_RECV_PREFIX
        if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_MQTT_MSG_RSP_LEN) {
            offset +=
              snprintf(response + offset, MAX_ATCMD_MQTT_MSG_RSP_LEN - offset,
                       "%s", AT_RECV_PREFIX);
        } else {
            LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                 response);
            error_no = CME_ERROR_TXT_STR_TOO_LONG;
            goto err;
        }
    } else if ('=' == single) {
        atcmd_read(&single, 1);

        if ('?' == single) {
            // MQTTAUTH prefix + \"PRODUCT_KEY\" + \"DEVICE_NAME\" +
            // \"DEVICE_SECRET\"
            if (offset + strlen(mqtt_auth_prefix) +
                  strlen(TAG_PRODUCT_KEY_STR) + strlen(TAG_DEVICE_NAME_STR) +
                  strlen(TAG_DEVICE_SECRET_STR) + strlen(SEPARATOR_STR) * 2 <
                MAX_ATCMD_MQTT_MSG_RSP_LEN) {
                offset += snprintf(
                  response + offset, MAX_ATCMD_MQTT_MSG_RSP_LEN - offset,
                  "%s%s%s%s%s%s", mqtt_auth_prefix, TAG_PRODUCT_KEY_STR,
                  SEPARATOR_STR, TAG_DEVICE_NAME_STR, SEPARATOR_STR,
                  TAG_DEVICE_SECRET_STR);
            } else {
                LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                     response);
                error_no = CME_ERROR_TXT_STR_TOO_LONG;
                goto err;
            }

            if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_MQTT_MSG_RSP_LEN) {
                offset += snprintf(response + offset,
                                   MAX_ATCMD_MQTT_MSG_RSP_LEN - offset, "%s",
                                   AT_RECV_PREFIX);
            } else {
                LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                     response);
                error_no = CME_ERROR_TXT_STR_TOO_LONG;
                goto err;
            }
        } else {
            ret =
              atcmd_socket_data_info_get(productkey, sizeof(productkey), NULL);
            if (ret < 0) {
                LOGE(TAG, "Invalid productkey %s !\n", productkey);
                error_no = CME_ERROR_INCORRECT_PARA;
                goto err;
            }

            // ignore \"
            if (strlen(productkey) > 0 &&
                productkey[strlen(productkey) - 1] == '\"') {
                productkey[strlen(productkey) - 1] = '\0';
            } else {
                LOGE(TAG, "Invalid productkey %s !\n", productkey);
                error_no = CME_ERROR_INCORRECT_PARA;
                goto err;
            }

            // eat \"
            atcmd_read(&single, 1);
            ret =
              atcmd_socket_data_info_get(devicename, sizeof(devicename), NULL);
            if (ret < 0) {
                LOGE(TAG, "Invalid devicename %s !\n", devicename);
                error_no = CME_ERROR_INCORRECT_PARA;
                goto err;
            }

            // ignore \"
            if (strlen(devicename) > 0 &&
                devicename[strlen(devicename) - 1] == '\"') {
                devicename[strlen(devicename) - 1] = '\0';
            } else {
                LOGE(TAG, "Invalid devicename %s !\n", devicename);
                error_no = CME_ERROR_INCORRECT_PARA;
                goto err;
            }

            // eat \"
            atcmd_read(&single, 1);
            ret = atcmd_socket_data_info_get(devicesecret, sizeof(devicesecret),
                                             NULL);
            if (ret <= 0) {
                LOGE(TAG, "Invalid devicesecret %s !\n", devicesecret);
                error_no = CME_ERROR_INCORRECT_PARA;
                goto err;
            }

            // ignore \"
            if (strlen(devicesecret) > 0 &&
                devicesecret[strlen(devicesecret) - 1] == '\"') {
                devicesecret[strlen(devicesecret) - 1] = '\0';
            } else {
                LOGE(TAG, "Invalid devicename %s !\n", devicesecret);
                error_no = CME_ERROR_INCORRECT_PARA;
                goto err;
            }

            HAL_SetProductKey(productkey);
            HAL_SetDeviceName(devicename);
            HAL_SetDeviceSecret(devicesecret);

            // trigger auth
            post_mqtt_authenticate_task();
        }
    } else {
        LOGE(TAG, "Invalid mqtt auth prefix %c!\n", single);
        error_no = CME_ERROR_TXT_STR_INVALID_CHAR;
        goto err;
    }

    // AT_RECV_SUCCESS_POSTFIX
    if (offset + strlen(AT_RECV_SUCCESS_POSTFIX) < MAX_ATCMD_MQTT_MSG_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_MSG_RSP_LEN - offset, "%s",
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

#define TAG_PARA_ARRAY_SIZE 5
#define MAX_KEEPALIVE_VAL 180
#define MIN_KEEPALIVE_VAL 60
enum {
   TAG_TIMEOUT   = 0,
   TAG_CLEAN     = 1,
   TAG_KEEPALIVE = 2,
   TAG_VERSION   = 3,
   TAG_AUTHMODE  = 4,
};

const static char *TAG_PARA_ARRAY[TAG_PARA_ARRAY_SIZE] = {
                    "\"TIMEOUT\"",
                    "\"CLEAN\"",
                    "\"KEEPALIVE\"",
                    "\"VERSION\"",
                    "\"AUTHMODE\"",
};

int update_imqtt_para()
{
    int         i, j;
    int         ret;
    char        body[20] = { 0 };
    char        version[MQTT_VERSION_MAX_LEN] = { 0 };
    int         val[TAG_PARA_ARRAY_SIZE];

    for (int i = 0; i < TAG_PARA_ARRAY_SIZE; i++) {
        val[i] = -1;
    }

    for (i = 0; i < TAG_PARA_ARRAY_SIZE; i++) {
        memset(body, 0, sizeof(body));
        ret = atcmd_socket_data_info_get(body, sizeof(body), NULL);
        if (ret != 0) {
            LOGE(TAG, "Invalid para tag %s !\n", body);
            goto err;
        }

        for (j = 0; j < TAG_PARA_ARRAY_SIZE; j++) {
            if (memcmp(&TAG_PARA_ARRAY[j][1], body, strlen(body)) == 0 ||
                memcmp(&TAG_PARA_ARRAY[j][0], body, strlen(body)) == 0) {
                if (val[j] >= 0) {
                    LOGE(TAG, "repeat para %s !\n", body);
                    goto err;
                }

                if (j == TAG_VERSION) {  // VERSION
                    ret = atcmd_socket_data_info_get(body, sizeof(body), NULL);
                    if (ret < 0 || strlen(body) - 2 > MQTT_VERSION_MAX_LEN) {
                       LOGE(TAG, "Invalid version %s !\n", body);
                       goto err;
                    }

                    val[j] = 1;
                    memcpy(version, body + 1, strlen(body) - 2);

                    if (strcmp(version, MQTT_DEFAULT_VERSION) != 0 &&
                        strcmp(version, MQTT_DEFAULT_VERSION_ALT) != 0) {
                        LOGE(TAG, "Invalid version %s !\n", version);
                        goto err;
                    }
                } else {
                    memset(body, 0, sizeof(body));
                    ret = atcmd_socket_data_info_get(body, sizeof(body), &atcmd_socket_data_len_check);
                    if (ret < 0 || strlen(body) <= 0 || (val[j] = atoi(body)) < 0){
                       LOGE(TAG, "Invalid %s: %s !\n", TAG_PARA_ARRAY[j], body);
                       goto err;
                    }

                    // check val range
                    if (j == TAG_CLEAN) {
                        if (val[j] != 0 && val[j] != 1) {
                            LOGE(TAG, "Invalid clean val %d !\n", val[j]);
                            goto err;
                        }
                    } else if (j == TAG_KEEPALIVE) {
                        if (val[j] > MAX_KEEPALIVE_VAL)
                            val[j] = MAX_KEEPALIVE_VAL;
                        else if (val[j] < MIN_KEEPALIVE_VAL)
                            val[j] = MIN_KEEPALIVE_VAL;
                    } else if (j == TAG_AUTHMODE) {
                        if (val[j] != 0 && val[j] != 1) {
                            LOGE(TAG, "Invalid authmode val %d !\n", val[j]);
                            goto err;
                        }
                    }
                }

                break;
            }
        }

        // no extra para
        if (ret == 1)
            break;

        if (j == TAG_PARA_ARRAY_SIZE) {
            LOGE(TAG, "no para tag match!\n");
            return -1;
        }
    }

    for (i = 0; i < TAG_PARA_ARRAY_SIZE; i++) {
        if (val[i] < 0)
            continue;

        if (i == TAG_TIMEOUT) {
            m_mqtt_config.timeout = val[i] * 1000;
        } else if (i == TAG_CLEAN) {
            m_mqtt_config.clean   = val[i];
        } else if (i == TAG_KEEPALIVE) {
            m_mqtt_config.keepalive = val[i] * 1000;
        } else if (i == TAG_VERSION) {
            memset(m_mqtt_config.version, 0, sizeof(m_mqtt_config.version));
            memcpy(m_mqtt_config.version, version, strlen(version));
        } else if (i == TAG_AUTHMODE) {
            m_mqtt_config.authmode = val[i];
        }
    }

    return 0;

err:
    return -1;
}

static int atcmd_imqtt_para()
{
    const char *mqtt_para_prefix = "+IMQTTPARA:";
    char        single;
    int         error_no = CME_ERROR_UNKNOWN;
    int         offset   = 0;
    char       *response = mqtt_msg_rsp;

    memset(response, 0, MAX_ATCMD_MQTT_MSG_RSP_LEN);

    atcmd_read(&single, 1);
    if ('=' != single && '?' != single) {
        LOGE(TAG, "Invalid mqtt para prefix %c!\n", single);
        goto err;
    }

    // AT_RECV_PREFIX
    if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_MQTT_MSG_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_MSG_RSP_LEN - offset, "%s",
                   AT_RECV_PREFIX);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
             response);
        goto err;
    }

    if ('?' == single) {
        // MQTTPARA + TIMEOUT + value + CLEAN + value + KEEEPALIVE + value +
        // VESRION + value + AUTHMODE + value
        if (offset + strlen(mqtt_para_prefix) + strlen(TAG_PARA_ARRAY[TAG_TIMEOUT]) +
              strlen(TAG_PARA_ARRAY[TAG_CLEAN]) + strlen(TAG_PARA_ARRAY[TAG_KEEPALIVE]) +
              strlen(TAG_PARA_ARRAY[TAG_VERSION]) + strlen(TAG_PARA_ARRAY[TAG_AUTHMODE]) +
              MAX_INTEGER_STR_LEN * 2 + MQTT_VERSION_MAX_LEN +
              strlen(QUOTE_STR) * 2 + ONE_DIGIT_STR_LEN * 2 +
              strlen(SEPARATOR_STR) * 4 <
            MAX_ATCMD_MQTT_MSG_RSP_LEN) {
            offset += snprintf(
              response + offset, MAX_ATCMD_MQTT_MSG_RSP_LEN - offset,
              "%s%s%s%d%s%s%s%d%s%s%s%d%s%s%s\"%s\"%s%s%s%d", mqtt_para_prefix,
              TAG_PARA_ARRAY[TAG_TIMEOUT], SEPARATOR_STR, (int)m_mqtt_config.timeout / 1000,
              SEPARATOR_STR, TAG_PARA_ARRAY[TAG_CLEAN], SEPARATOR_STR, m_mqtt_config.clean,
              SEPARATOR_STR, TAG_PARA_ARRAY[TAG_KEEPALIVE], SEPARATOR_STR,
              (int)m_mqtt_config.keepalive / 1000, SEPARATOR_STR, TAG_PARA_ARRAY[TAG_VERSION],
              SEPARATOR_STR, m_mqtt_config.version, SEPARATOR_STR,
              TAG_PARA_ARRAY[TAG_AUTHMODE], SEPARATOR_STR, m_mqtt_config.authmode);
        } else {
            LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                 response);
            error_no = CME_ERROR_TXT_STR_TOO_LONG;
            goto err;
        }

        // AT_RECV_PREFIX
        if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_MQTT_MSG_RSP_LEN) {
            offset +=
              snprintf(response + offset, MAX_ATCMD_MQTT_MSG_RSP_LEN - offset,
                       "%s", AT_RECV_PREFIX);
        } else {
            LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                 response);
            error_no = CME_ERROR_TXT_STR_TOO_LONG;
            goto err;
        }
    } else if ('=' == single) {
        atcmd_read(&single, 1);

        if ('?' == single) {
            // MQTTPARA + \"ParaTag\" + \"ParaValue\"
            if (offset + strlen(mqtt_para_prefix) + strlen(PARA_TAG_STR) +
                  strlen(SEPARATOR_STR) + strlen(PARA_VAL_STR) <
                MAX_ATCMD_MQTT_MSG_RSP_LEN) {
                offset += snprintf(response + offset,
                                   MAX_ATCMD_MQTT_MSG_RSP_LEN - offset,
                                   "%s%s%s%s", mqtt_para_prefix, PARA_TAG_STR,
                                   SEPARATOR_STR, PARA_VAL_STR);
            } else {
                LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                     response);
                error_no = CME_ERROR_TXT_STR_TOO_LONG;
                goto err;
            }

            if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_MQTT_MSG_RSP_LEN) {
                offset += snprintf(response + offset,
                                   MAX_ATCMD_MQTT_MSG_RSP_LEN - offset, "%s",
                                   AT_RECV_PREFIX);
            } else {
                LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                     response);
                error_no = CME_ERROR_TXT_STR_TOO_LONG;
                goto err;
            }
        } else {
            if (update_imqtt_para() < 0) {
                error_no = CME_ERROR_TXT_STR_INVALID_CHAR;
                goto err;
            }
        }
    } else {
        LOGE(TAG, "Invalid mqtt para prefix %c!\n", single);
        error_no = CME_ERROR_TXT_STR_INVALID_CHAR;
        goto err;
    }

    // AT_RECV_SUCCESS_POSTFIX
    if (offset + strlen(AT_RECV_SUCCESS_POSTFIX) < MAX_ATCMD_MQTT_MSG_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_MSG_RSP_LEN - offset, "%s",
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

static int atcmd_imqtt_conn()
{
    char  single;
    int   error_no = CME_ERROR_UNKNOWN;
    int   offset   = 0;
    char *response = mqtt_msg_rsp;

    memset(response, 0, MAX_ATCMD_MQTT_MSG_RSP_LEN);

    atcmd_read(&single, 1);
    if ('=' != single && '\r' != single) {
        LOGE(TAG, "Invalid mqtt conn prefix %c!\n", single);
        error_no = CME_ERROR_TXT_STR_INVALID_CHAR;
        goto err;
    }

    if ('=' == single) {
        atcmd_read(&single, 1);

        if ('?' != single) {
            LOGE(TAG, "Invalid mqtt conn prefix %c!\n", single);
            error_no = CME_ERROR_TXT_STR_INVALID_CHAR;
            goto err;
        }
    } else {
        if (m_yield_task_start) {
            LOGE(TAG, "Already connected, operation not allowed!\n");
            error_no = CME_ERROR_OP_NOT_ALLOWED;
            goto err;
        }

        // connect
        post_mqtt_connect_task();
    }

    // AT_RECV_PREFIX
    if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_MQTT_MSG_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_MSG_RSP_LEN - offset, "%s",
                   AT_RECV_PREFIX);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
             response);
        error_no = CME_ERROR_TXT_STR_TOO_LONG;
        goto err;
    }

    // AT_RECV_SUCCESS_POSTFIX
    if (offset + strlen(AT_RECV_SUCCESS_POSTFIX) < MAX_ATCMD_MQTT_MSG_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_MSG_RSP_LEN - offset, "%s",
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

static const char *TAG_TOPIC_STR   = "\"topic\"";
static const char *TAG_QOS_STR     = "\"qos\"";
static const char *TAG_MSG_STR     = "\"message\"";
static const char *TAG_FORMAT_STR  = "\"format\"";
static const char *TAG_FRAG_ID_STR = "\"fragment_id\"";

static int atcmd_imqtt_pub()
{
    const char *mqtt_pub_prefix = "+IMQTTPUB:";
    char        single;
    char        body[20] = { 0 };
    int         ret;
    int         error_no = CME_ERROR_UNKNOWN;
    int         offset   = 0;
    char        *response = mqtt_msg_rsp;

    memset(response, 0, MAX_ATCMD_MQTT_MSG_RSP_LEN);

    atcmd_read(&single, 1);
    if ('=' != single && '?' != single) {
        LOGE(TAG, "Invalid mqtt pub prefix %c!\n", single);
        error_no = CME_ERROR_TXT_STR_INVALID_CHAR;
        goto err;
    }

    // AT_RECV_PREFIX
    if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_MQTT_MSG_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_MSG_RSP_LEN - offset, "%s",
                   AT_RECV_PREFIX);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
             response);
        error_no = CME_ERROR_TXT_STR_TOO_LONG;
        goto err;
    }

    if ('?' == single) {
        // prefix
        if (offset + strlen(mqtt_pub_prefix) < MAX_ATCMD_MQTT_MSG_RSP_LEN) {
            offset +=
              snprintf(response + offset, MAX_ATCMD_MQTT_MSG_RSP_LEN - offset, "%s",
                       mqtt_pub_prefix);
        } else {
            LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                 response);
            error_no = CME_ERROR_TXT_STR_TOO_LONG;
            goto err;
        }

        // TOPIC + QOS + MESSAGE + [FORMAT] + [FRAGMENT_ID]
        if (offset + strlen(m_last_pub_msg.topic_pub) +
              strlen(m_last_pub_msg.msg_pub) + ONE_DIGIT_STR_LEN * 2 +
              MAX_INTEGER_STR_LEN + strlen(SEPARATOR_STR) * 4 +
              strlen(QUOTE_STR) * 4 <
            MAX_ATCMD_MQTT_MSG_RSP_LEN) {
            if (m_last_pub_msg.valid)
                offset += snprintf(
                  response + offset, MAX_ATCMD_MQTT_MSG_RSP_LEN - offset,
                  "\"%s\"%s%d%s\"%s\"%s%d%s%d", m_last_pub_msg.topic_pub,
                  SEPARATOR_STR, m_last_pub_msg.qos, SEPARATOR_STR,
                  m_last_pub_msg.msg_pub, SEPARATOR_STR,
                  m_last_pub_msg.msg_format, SEPARATOR_STR,
                  m_last_pub_msg.fragment_id);
            else
                offset += snprintf(response + offset,
                                   MAX_ATCMD_MQTT_MSG_RSP_LEN - offset, ",,,,");
        } else {
            LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                 response);
            error_no = CME_ERROR_TXT_STR_TOO_LONG;
            goto err;
        }
    } else if ('=' == single) {
        atcmd_read(&single, 1);

        if ('?' == single) {
            // prefix
            if (offset + strlen(mqtt_pub_prefix) < MAX_ATCMD_MQTT_MSG_RSP_LEN) {
                offset +=
                  snprintf(response + offset, MAX_ATCMD_MQTT_MSG_RSP_LEN - offset, "%s",
                           mqtt_pub_prefix);
            } else {
                LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                     response);
                error_no = CME_ERROR_TXT_STR_TOO_LONG;
                goto err;
            }

            // \"topic\" + \"qos\" + \"message\" + \"format\" + \"fragment_id\"
            if (offset + strlen(TAG_TOPIC_STR) + strlen(TAG_QOS_STR) +
                  strlen(TAG_MSG_STR) + strlen(TAG_FORMAT_STR) +
                  strlen(TAG_FRAG_ID_STR) + strlen(SEPARATOR_STR) * 4 <
                MAX_ATCMD_MQTT_MSG_RSP_LEN) {
                offset += snprintf(
                  response + offset, MAX_ATCMD_MQTT_MSG_RSP_LEN - offset,
                  "%s%s%s%s%s%s%s%s%s", TAG_TOPIC_STR, SEPARATOR_STR,
                  TAG_QOS_STR, SEPARATOR_STR, TAG_MSG_STR, SEPARATOR_STR,
                  TAG_FORMAT_STR, SEPARATOR_STR, TAG_FRAG_ID_STR);
            } else {
                LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                     response);
                error_no = CME_ERROR_TXT_STR_TOO_LONG;
                goto err;
            }
        } else {
            memset(&m_attempt_pub_msg, 0, sizeof(m_attempt_pub_msg));

            ret = atcmd_socket_data_info_get(
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
            if (ret < 0 || strlen(body) <= 0 || (m_attempt_pub_msg.qos = atoi(body)) < 0 ||
                m_attempt_pub_msg.qos > 2) {
                LOGE(TAG, "Invalid qos %s !\n", body);
                error_no = CME_ERROR_INCORRECT_PARA;
                goto err;
            }

            // eat "
            atcmd_read(&single, 1);
            ret = atcmd_socket_text_info_get(m_attempt_pub_msg.msg_pub,
                                             sizeof(m_attempt_pub_msg.msg_pub),
                                             NULL);
            if (ret < 0) {
                LOGE(TAG, "Invalid message %s !\n", m_attempt_pub_msg.msg_pub);
                error_no = CME_ERROR_INCORRECT_PARA;
                goto err;
            }

            // ignore "
            if (strlen(m_attempt_pub_msg.msg_pub) > 0 &&
                m_attempt_pub_msg
                    .msg_pub[strlen(m_attempt_pub_msg.msg_pub) - 1] == '\"') {
                m_attempt_pub_msg
                  .msg_pub[strlen(m_attempt_pub_msg.msg_pub) - 1] = '\0';
            } else {
                LOGE(TAG, "Invalid pub msg %s !\n", m_attempt_pub_msg.msg_pub);
                error_no = CME_ERROR_INCORRECT_PARA;
                goto err;
            }

            if (ret == 0) {
                memset(body, 0, sizeof(body));
                ret = atcmd_socket_data_info_get(body, sizeof(body),
                                                 &atcmd_socket_data_len_check);
                if (ret < 0 || strlen(body) <= 0 ||
                    (m_attempt_pub_msg.msg_format = atoi(body)) < 0 ||
                    m_attempt_pub_msg.msg_format > 1) {
                    LOGE(TAG, "Invalid format %s !\n", body);
                    error_no = CME_ERROR_INCORRECT_PARA;
                    goto err;
                }
            }

            if (ret == 0) {
                memset(body, 0, sizeof(body));
                ret = atcmd_socket_data_info_get(body, sizeof(body),
                                                 &atcmd_socket_data_len_check);
                if (ret <= 0 ||  strlen(body) <= 0 ||
                    (m_attempt_pub_msg.fragment_id = atoi(body)) < 0) {
                    LOGE(TAG, "Invalid frag_id %s !\n", body);
                    error_no = CME_ERROR_INCORRECT_PARA;
                    goto err;
                }
            }

            m_attempt_pub_msg.valid = true;
            ret                     = mqtt_publish_msg(&m_attempt_pub_msg);
            if (ret < 0) {
                if (ret == MQTT_NETWORK_ERROR)
                    error_no = CME_ERROR_NET_TIMEOUT;
                else if (ret == FAIL_RETURN || ret == NULL_VALUE_ERROR)
                    error_no = CME_ERROR_INCORRECT_PARA;
                else {
                    LOGE(TAG, "mqtt publish unknown err! ret %d\n", ret);
                }
                goto err;
            } else if (m_attempt_pub_msg.fragment_id == 0) {
                // prefix
                if (offset + strlen(mqtt_pub_prefix) < MAX_ATCMD_MQTT_MSG_RSP_LEN) {
                    offset +=
                      snprintf(response + offset, MAX_ATCMD_MQTT_MSG_RSP_LEN - offset, "%s",
                               mqtt_pub_prefix);
                } else {
                    LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                         response);
                    error_no = CME_ERROR_TXT_STR_TOO_LONG;
                    goto err;
                }

                // packetid
                if (offset + MAX_INTEGER_STR_LEN < MAX_ATCMD_MQTT_MSG_RSP_LEN) {
                    offset +=
                      snprintf(response + offset,
                               MAX_ATCMD_MQTT_MSG_RSP_LEN - offset, "%d", ret);
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
    if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_MQTT_MSG_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_MSG_RSP_LEN - offset, "%s",
                   AT_RECV_PREFIX);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
             response);
        error_no = CME_ERROR_TXT_STR_TOO_LONG;
        goto err;
    }

    // AT_RECV_SUCCESS_POSTFIX
    if (offset + strlen(AT_RECV_SUCCESS_POSTFIX) < MAX_ATCMD_MQTT_MSG_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_MSG_RSP_LEN - offset, "%s",
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

static int atcmd_imqtt_sub()
{
    const char *mqtt_sub_prefix = "+IMQTTSUB:";
    char        single;
    char        body[20] = { 0 };
    int         ret;
    int         error_no = CME_ERROR_UNKNOWN;
    int         offset   = 0;
    char       *response = mqtt_msg_rsp;

    memset(response, 0, MAX_ATCMD_MQTT_MSG_RSP_LEN);

    atcmd_read(&single, 1);
    if ('=' != single && '?' != single) {
        LOGE(TAG, "Invalid mqtt sub prefix %c!\n", single);
        error_no = CME_ERROR_TXT_STR_INVALID_CHAR;
        goto err;
    }

    // AT_RECV_PREFIX
    if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_MQTT_MSG_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_MSG_RSP_LEN - offset, "%s",
                   AT_RECV_PREFIX);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
             response);
        error_no = CME_ERROR_TXT_STR_TOO_LONG;
        goto err;
    }

    // prefix
    if (offset + strlen(mqtt_sub_prefix) < MAX_ATCMD_MQTT_MSG_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_MSG_RSP_LEN - offset, "%s",
                   mqtt_sub_prefix);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
             response);
        error_no = CME_ERROR_TXT_STR_TOO_LONG;
        goto err;
    }

    if ('?' == single) {
        // TOPIC + QOS
        if (offset + strlen(m_last_sub_topic.topic) + ONE_DIGIT_STR_LEN +
              strlen(SEPARATOR_STR) + strlen(QUOTE_STR) * 2 <
            MAX_ATCMD_MQTT_MSG_RSP_LEN) {
            if (m_last_sub_topic.valid)
                offset += snprintf(response + offset,
                                   MAX_ATCMD_MQTT_MSG_RSP_LEN - offset,
                                   "\"%s\"%s%d", m_last_sub_topic.topic,
                                   SEPARATOR_STR, m_last_sub_topic.qos);
            else
                offset += snprintf(response + offset,
                                   MAX_ATCMD_MQTT_MSG_RSP_LEN - offset, ",");
        } else {
            LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                 response);
            error_no = CME_ERROR_TXT_STR_TOO_LONG;
            goto err;
        }
    } else if ('=' == single) {
        atcmd_read(&single, 1);

        if ('?' == single) {
            // \"topic\" + \"qos\"
            if (offset + strlen(TAG_TOPIC_STR) + strlen(TAG_QOS_STR) +
                  strlen(SEPARATOR_STR) <
                MAX_ATCMD_MQTT_MSG_RSP_LEN) {
                offset += snprintf(
                  response + offset, MAX_ATCMD_MQTT_MSG_RSP_LEN - offset,
                  "%s%s%s", TAG_TOPIC_STR, SEPARATOR_STR, TAG_QOS_STR);
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
            if (ret <= 0 || strlen(body) <= 0 ||
                (m_attempt_sub_topic.qos = atoi(body)) < 0 ||
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
                if (offset + MAX_INTEGER_STR_LEN < MAX_ATCMD_MQTT_MSG_RSP_LEN) {
                    offset +=
                      snprintf(response + offset,
                               MAX_ATCMD_MQTT_MSG_RSP_LEN - offset, "%d", ret);
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
    if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_MQTT_MSG_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_MSG_RSP_LEN - offset, "%s",
                   AT_RECV_PREFIX);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
             response);
        error_no = CME_ERROR_TXT_STR_TOO_LONG;
        goto err;
    }

    // AT_RECV_SUCCESS_POSTFIX
    if (offset + strlen(AT_RECV_SUCCESS_POSTFIX) < MAX_ATCMD_MQTT_MSG_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_MSG_RSP_LEN - offset, "%s",
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

static int atcmd_imqtt_unsub()
{
    const char *mqtt_unsub_prefix = "+IMQTTUNSUB:";
    char        single;
    int         error_no = CME_ERROR_UNKNOWN;
    int         ret;
    int         offset   = 0;
    char       *response = mqtt_msg_rsp;

    memset(response, 0, MAX_ATCMD_MQTT_MSG_RSP_LEN);

    atcmd_read(&single, 1);
    if ('=' != single && '?' != single) {
        LOGE(TAG, "Invalid mqtt unsub prefix %c!\n", single);
        error_no = CME_ERROR_TXT_STR_INVALID_CHAR;
        goto err;
    }

    // AT_RECV_PREFIX
    if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_MQTT_MSG_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_MSG_RSP_LEN - offset, "%s",
                   AT_RECV_PREFIX);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
             response);
        error_no = CME_ERROR_TXT_STR_TOO_LONG;
        goto err;
    }

    // prefix
    if (offset + strlen(mqtt_unsub_prefix) < MAX_ATCMD_MQTT_MSG_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_MSG_RSP_LEN - offset, "%s",
                   mqtt_unsub_prefix);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
             response);
        error_no = CME_ERROR_TXT_STR_TOO_LONG;
        goto err;
    }

    if ('?' == single) {
        // TOPIC
        if (offset + strlen(m_last_unsub_topic.topic) + strlen(QUOTE_STR) * 2 <
            MAX_ATCMD_MQTT_MSG_RSP_LEN) {
            if (m_last_unsub_topic.valid)
                offset += snprintf(response + offset,
                                   MAX_ATCMD_MQTT_MSG_RSP_LEN - offset,
                                   "\"%s\"", m_last_unsub_topic.topic);
        } else {
            LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                 response);
            error_no = CME_ERROR_TXT_STR_TOO_LONG;
            goto err;
        }
    } else if ('=' == single) {
        atcmd_read(&single, 1);

        if ('?' == single) {
            // \"topic\"
            if (offset + strlen(TAG_TOPIC_STR) < MAX_ATCMD_MQTT_MSG_RSP_LEN) {
                offset += snprintf(response + offset,
                                   MAX_ATCMD_MQTT_MSG_RSP_LEN - offset, "%s",
                                   TAG_TOPIC_STR);
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
            if (ret <= 0) {
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
                if (offset + MAX_INTEGER_STR_LEN < MAX_ATCMD_MQTT_MSG_RSP_LEN) {
                    offset +=
                      snprintf(response + offset,
                               MAX_ATCMD_MQTT_MSG_RSP_LEN - offset, "%d", ret);
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
    if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_MQTT_MSG_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_MSG_RSP_LEN - offset, "%s",
                   AT_RECV_PREFIX);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
             response);
        error_no = CME_ERROR_TXT_STR_TOO_LONG;
        goto err;
    }

    // AT_RECV_SUCCESS_POSTFIX
    if (offset + strlen(AT_RECV_SUCCESS_POSTFIX) < MAX_ATCMD_MQTT_MSG_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_MSG_RSP_LEN - offset, "%s",
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

static int atcmd_imqtt_rcvpub()
{
    const char *mqtt_rcvpub_prefix = "+IMQTTRCVPUB:";
    char        single;
    int         offset   = 0;
    char       *response = mqtt_msg_rsp;

    memset(response, 0, MAX_ATCMD_MQTT_MSG_RSP_LEN);

    atcmd_read(&single, 1);
    if ('=' != single && '?' != single) {
        LOGE(TAG, "Invalid mqtt recvpub prefix %c!\n", single);
        goto err;
    }

    if ('=' == single) {
        atcmd_read(&single, 1);

        if ('?' != single) {
            LOGE(TAG, "Invalid mqtt recvpub prefix %c!\n", single);
            goto err;
        }
    } else if ('?' == single) {
        // AT_RECV_PREFIX
        if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_MQTT_MSG_RSP_LEN) {
            offset +=
              snprintf(response + offset, MAX_ATCMD_MQTT_MSG_RSP_LEN - offset,
                       "%s", AT_RECV_PREFIX);
        } else {
            LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                 response);
            goto err;
        }

        // prefix
        if (offset + strlen(mqtt_rcvpub_prefix) < MAX_ATCMD_MQTT_MSG_RSP_LEN) {
            offset +=
              snprintf(response + offset, MAX_ATCMD_MQTT_MSG_RSP_LEN - offset,
                       "%s", mqtt_rcvpub_prefix);
        } else {
            LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                 response);
            goto err;
        }

        // PACKETID + TOPIC + MSG_LEN + MESSAGE + [FORMAT] + [FRAGMENT_ID]
        if (offset + MAX_INTEGER_STR_LEN * 3 +
              strlen(m_last_recvpub_msg.topic_pub) +
              strlen(m_last_pub_msg.msg_pub) + ONE_DIGIT_STR_LEN +
              strlen(SEPARATOR_STR) * 5 + strlen(QUOTE_STR) * 4 <
            MAX_ATCMD_MQTT_MSG_RSP_LEN) {
            if (m_last_recvpub_msg.valid) {
                offset += snprintf(
                  response + offset, MAX_ATCMD_MQTT_MSG_RSP_LEN - offset,
                  "%d%s\"%s\"%s%d%s\"%s\"",
                  m_last_recvpub_msg.packet_id, SEPARATOR_STR,
                  m_last_recvpub_msg.topic_pub, SEPARATOR_STR,
                  strlen(m_last_recvpub_msg.msg_pub), SEPARATOR_STR,
                  m_last_recvpub_msg.msg_pub);
            } else {
                offset +=
                  snprintf(response + offset,
                           MAX_ATCMD_MQTT_MSG_RSP_LEN - offset, ",,,,,");
            }
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
    if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_MQTT_MSG_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_MSG_RSP_LEN - offset, "%s",
                   AT_RECV_PREFIX);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
             response);
        goto err;
    }

    // AT_RECV_SUCCESS_POSTFIX
    if (offset + strlen(AT_RECV_SUCCESS_POSTFIX) < MAX_ATCMD_MQTT_MSG_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_MSG_RSP_LEN - offset, "%s",
                   AT_RECV_SUCCESS_POSTFIX);
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

static int atcmd_imqtt_state()
{
    const char *mqtt_state_prefix = "+IMQTTSTATE:";
    char        single;
    int         offset   = 0;
    char       *response = mqtt_msg_rsp;

    memset(response, 0, MAX_ATCMD_MQTT_MSG_RSP_LEN);

    atcmd_read(&single, 1);
    if ('=' != single && '?' != single) {
        LOGE(TAG, "Invalid mqtt state prefix %c!\n", single);
        goto err;
    }

    // AT_RECV_PREFIX
    if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_MQTT_MSG_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_MSG_RSP_LEN - offset, "%s",
                   AT_RECV_PREFIX);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
             response);
        goto err;
    }

    if ('=' == single) {
        atcmd_read(&single, 1);

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
            MAX_ATCMD_MQTT_MSG_RSP_LEN) {
            offset +=
              snprintf(response + offset, MAX_ATCMD_MQTT_MSG_RSP_LEN - offset,
                       "%s%d", mqtt_state_prefix, state);
        } else {
            LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                 response);
            goto err;
        }

        // AT_RECV_PREFIX
        if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_MQTT_MSG_RSP_LEN) {
            offset +=
              snprintf(response + offset, MAX_ATCMD_MQTT_MSG_RSP_LEN - offset,
                       "%s", AT_RECV_PREFIX);
        } else {
            LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
                 response);
            goto err;
        }
    }

    // AT_RECV_SUCCESS_POSTFIX
    if (offset + strlen(AT_RECV_SUCCESS_POSTFIX) < MAX_ATCMD_MQTT_MSG_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_MSG_RSP_LEN - offset, "%s",
                   AT_RECV_SUCCESS_POSTFIX);
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

static int atcmd_imqtt_disconn()
{
    char  single;
    int   error_no = CME_ERROR_UNKNOWN;
    int   offset   = 0;
    char *response = mqtt_msg_rsp;

    memset(response, 0, MAX_ATCMD_MQTT_MSG_RSP_LEN);

    atcmd_read(&single, 1);
    if ('=' != single && '\r' != single) {
        LOGE(TAG, "Invalid mqtt disconn prefix %c!\n", single);
        error_no = CME_ERROR_TXT_STR_INVALID_CHAR;
        goto err;
    }

    if ('=' == single) {
        atcmd_read(&single, 1);

        if ('?' != single) {
            LOGE(TAG, "Invalid mqtt disconn prefix %c!\n", single);
            error_no = CME_ERROR_TXT_STR_INVALID_CHAR;
            goto err;
        }
    } else {
        if (m_connect_time < 0 ||
            aos_now_ms() - m_connect_time < MIN_INTERVAL_CON_DISCON_MS) {
            LOGE(TAG, "Frequent disconn operation not allowed!\n");
            error_no = CME_ERROR_OP_NOT_ALLOWED;
            goto err;
        }

        m_yield_task_start = false;
        aos_mutex_lock(&m_comm_mutex, AOS_WAIT_FOREVER);
        IOT_MQTT_Destroy(&m_pclient);
        aos_mutex_unlock(&m_comm_mutex);
        m_connect_time = -1;
    }

    // AT_RECV_PREFIX
    if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_MQTT_MSG_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_MSG_RSP_LEN - offset, "%s",
                   AT_RECV_PREFIX);
    } else {
        LOGE(TAG, "%s %d at string too long %s\n", __func__, __LINE__,
             response);
        error_no = CME_ERROR_TXT_STR_TOO_LONG;
        goto err;
    }

    // AT_RECV_SUCCESS_POSTFIX
    if (offset + strlen(AT_RECV_SUCCESS_POSTFIX) < MAX_ATCMD_MQTT_MSG_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_MQTT_MSG_RSP_LEN - offset, "%s",
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

    notify_mqtt_state(MQTT_STATE_DISCON);

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
    atcmd_read(prefix, strlen(atcmd_prefix));
    if (memcmp(prefix, atcmd_prefix, strlen(atcmd_prefix)) != 0) {
        goto err;
    }

    atcmd_read(&single, 1);
    memset(prefix, 0, sizeof(prefix));

    switch (single) {
        case 'M':
            atcmd_read(prefix, 3);

            if (memcmp(prefix, "ODE", 3) != 0)
                goto err;

            index = ATCMD_IMQTT_MODE;
            break;
        case 'O':
            atcmd_read(prefix, 3);

            if (memcmp(prefix, "PEN", 3) != 0)
                goto err;

            index = ATCMD_IMQTT_OPEN;
            break;
        case 'A':
            atcmd_read(prefix, 3);

            if (memcmp(prefix, "UTH", 3) != 0)
                goto err;

            index = ATCMD_IMQTT_AUTH;
            break;
        case 'P':
            atcmd_read(prefix, 2);

            if (memcmp(prefix, "AR", 2) == 0) {
                atcmd_read(&single, 1);

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
            atcmd_read(prefix, 3);

            if (memcmp(prefix, "ONN", 3) != 0)
                goto err;

            index = ATCMD_IMQTT_CONN;
            break;
        case 'S':
            atcmd_read(prefix, 2);

            if (memcmp(prefix, "UB", 2) == 0) {
                index = ATCMD_IMQTT_SUB;
            } else if (memcmp(prefix, "TA", 2) == 0) {
                memset(prefix, 0, sizeof(prefix));
                atcmd_read(prefix, 2);

                if (memcmp(prefix, "TE", 2) != 0)
                    goto err;

                index = ATCMD_IMQTT_STATE;
            } else {
                goto err;
            }

            break;
        case 'U': //"UNSUB":
            atcmd_read(prefix, 4);

            if (memcmp(prefix, "NSUB", 4) != 0)
                goto err;

            index = ATCMD_IMQTT_UNSUB;
            break;
        case 'R': //"CVPUB":
            atcmd_read(prefix, 5);

            if (memcmp(prefix, "CVPUB", 5) != 0)
                goto err;

            index = ATCMD_IMQTT_RCVPUB;
            break;
        case 'D': //"DIS": "DBG":
            atcmd_read(prefix, 6);

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
        return (atcmd_hdl_ptr_t) &at_mqtt_cmds_table[index];
    }

err:
    LOGE(TAG, "Invalid mqtt prefix %s\n", prefix);
    return NULL;
}

static int mqtt_init()
{
    memset(&m_mqtt_config, 0, sizeof(m_mqtt_config));
    m_mqtt_config.mode = MQTT_DEFAULT_TLS_MODE;

    m_mqtt_config.timeout   = MQTT_DEFAULT_REQ_TIMOUT_MS;
    m_mqtt_config.clean     = MQTT_DEFAULT_CLEAN_SESSION;
    m_mqtt_config.keepalive = MQTT_DEFAULT_KEEPALIVE_MS;
    strcpy(m_mqtt_config.version, MQTT_DEFAULT_VERSION);
    m_mqtt_config.authmode = MQTT_DEFAULT_AUTH_MODE;

    memset(&m_last_pub_msg, 0, sizeof(m_last_pub_msg));
    memset(&m_last_sub_topic, 0, sizeof(m_last_sub_topic));
    memset(&m_last_unsub_topic, 0, sizeof(m_last_unsub_topic));
    memset(&m_frag_pub_msg, 0, sizeof(m_frag_pub_msg));

    if (aos_mutex_new(&m_comm_mutex) != 0) {
        LOGE(TAG, "Creating mutex failed\r\n");
        return -1;
    }

    return 0;
}

static void mqtt_deinit()
{
    if (aos_mutex_is_valid(&m_comm_mutex)) {
        aos_mutex_free(&m_comm_mutex);
    }

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

/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <stddef.h>
#include "iot_import.h"
#include "iot_export.h"
#include "iotx_system.h"
#include "iotx_utils.h"
#include "utils_hmac.h"
#include "string_utils.h"
#include "utils_sysinfo.h"

#include "MQTTPacket/MQTTPacket.h"
#include "iotx_mqtt_internal.h"
#include "utils_md5.h"

#define MQTT_DEFAULT_MSG_LEN 1280

#ifdef VERSION_REPORT_DEBUG
    #define VERSION_DEBUG(...) log_debug("MQTT", __VA_ARGS__)
#else
    #define VERSION_DEBUG(...)
#endif
#define DEBUG_REPORT_MID_DEVINFO_FIRMWARE (1)

static int iotx_mc_send_packet(iotx_mc_client_t *c, char *buf, int length, iotx_time_t *time);
static int iotx_mc_read_packet(iotx_mc_client_t *c, iotx_time_t *timer, unsigned int *packet_type);
static int iotx_mc_keepalive_sub(iotx_mc_client_t *pClient);
static void iotx_mc_disconnect_callback(iotx_mc_client_t *pClient) ;
static int iotx_mc_check_state_normal(iotx_mc_client_t *c);
static void iotx_mc_reconnect_callback(iotx_mc_client_t *pClient);
static int iotx_mc_push_pubInfo_to(iotx_mc_client_t *c, int len, unsigned short msgId, list_node_t **node);
static int iotx_mc_push_subInfo_to(iotx_mc_client_t *c, int len, unsigned short msgId, enum msgTypes type,
                                   iotx_mc_topic_handle_t *handler,
                                   list_node_t **node);
static int iotx_mc_check_handle_is_identical(iotx_mc_topic_handle_t *messageHandlers1,
        iotx_mc_topic_handle_t *messageHandler2);
static int iotx_mc_check_handle_is_identical_ex(iotx_mc_topic_handle_t *messageHandlers1,
        iotx_mc_topic_handle_t *messageHandler2);

static iotx_mc_state_t iotx_mc_get_client_state(iotx_mc_client_t *pClient);
static void iotx_mc_set_client_state(iotx_mc_client_t *pClient, iotx_mc_state_t newState);

static int _dump_wait_list(iotx_mc_client_t *c, const char *type);

static void _iotx_mqtt_event_handle_sub(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg);

static void *g_mqtt_client = NULL;

#if WITH_MQTT_ZIP_TOPIC
#define MQTT_MD5_PATH_DEFAULT_LEN (16)

static int iotx_mc_get_md5_topic(const char *path, int len, char outbuf[], int outlen)
{
    unsigned char md5[16] = {0};
    if (!path || !len || !outbuf || !outlen) {
        return -1;
    }

    utils_md5((unsigned char *)path, (size_t)len, md5);
    memcpy(outbuf, md5, outlen > 16 ? 16 : outlen);
    return 0;
}
#endif
static int _in_yield_cb;

static int _handle_event(iotx_mqtt_event_handle_pt handle, iotx_mc_client_t *c, iotx_mqtt_event_msg_pt msg)
{
    ARGUMENT_SANITY_CHECK(handle != NULL, FAIL_RETURN);
    ARGUMENT_SANITY_CHECK(handle->h_fp != NULL, FAIL_RETURN);
    _in_yield_cb = 1;
    handle->h_fp(handle->pcontext, c, msg);
    _in_yield_cb = 0;
    return 0;
}

static inline int _is_in_yield_cb()
{
    return _in_yield_cb;
}

static int iotx_mc_check_rule(char *iterm, iotx_mc_topic_type_t type)
{
    int i = 0;
    int len = 0;

    if (NULL == iterm) {
        mqtt_err("iterm is NULL");
        return FAIL_RETURN;
    }

    len = strlen(iterm);

    for (i = 0; i < len; i++) {
        if (TOPIC_FILTER_TYPE == type) {
            if ('+' == iterm[i] || '#' == iterm[i]) {
                if (1 != len) {
                    mqtt_err("the character # and + is error");
                    return FAIL_RETURN;
                }
            }
        } else {
            if ('+' == iterm[i] || '#' == iterm[i]) {
                mqtt_err("has character # and + is error");
                return FAIL_RETURN;
            }
        }

        if (iterm[i] < 32 || iterm[i] >= 127) {
            return FAIL_RETURN;
        }
    }
    return SUCCESS_RETURN;
}

/* check whether the topic is matched or not */
static char iotx_mc_is_topic_matched(char *topicFilter, MQTTString *topicName)
{
    if (!topicFilter || !topicName) {
        return 0;
    }
    char *curf = topicFilter;
    char *curn = topicName->lenstring.data;
    char *curn_end = curn + topicName->lenstring.len;

    while (*curf && curn < curn_end) {
        if (*curn == '/' && *curf != '/') {
            break;
        }

        if (*curf != '+' && *curf != '#' && *curf != *curn) {
            break;
        }

        if (*curf == '+') {
            /* skip until we meet the next separator, or end of string */
            char *nextpos = curn + 1;
            while (nextpos < curn_end && *nextpos != '/') {
                nextpos = ++curn + 1;
            }
        } else if (*curf == '#') {
            curn = curn_end - 1;    /* skip until end of string */
        }
        curf++;
        curn++;
    }

    return (curn == curn_end) && (*curf == '\0');
}
/* Check topic name */
/* 0, topic name is valid; NOT 0, topic name is invalid */
static int iotx_mc_check_topic(const char *topicName, iotx_mc_topic_type_t type)
{
    int mask = 0;
    char *delim = "/";
    char *iterm = NULL;
    char topicString[IOTX_MC_TOPIC_NAME_MAX_LEN];
    if (NULL == topicName || '/' != topicName[0]) {
        return FAIL_RETURN;
    }

    if (strlen(topicName) > IOTX_MC_TOPIC_NAME_MAX_LEN) {
        mqtt_err("len of topicName exceeds 64");
        return FAIL_RETURN;
    }

    memset(topicString, 0x0, IOTX_MC_TOPIC_NAME_MAX_LEN);
    strncpy(topicString, topicName, IOTX_MC_TOPIC_NAME_MAX_LEN - 1);

    iterm = strtok(topicString, delim);

    if (SUCCESS_RETURN != iotx_mc_check_rule(iterm, type)) {
        mqtt_err("run iotx_check_rule error");
        return FAIL_RETURN;
    }

    for (;;) {
        iterm = strtok(NULL, delim);

        if (iterm == NULL) {
            break;
        }

        /* The character '#' is not in the last */
        if (1 == mask) {
            mqtt_err("the character # is error");
            return FAIL_RETURN;
        }

        if (SUCCESS_RETURN != iotx_mc_check_rule(iterm, type)) {
            mqtt_err("run iotx_check_rule error");
            return FAIL_RETURN;
        }

        if (iterm[0] == '#') {
            mask = 1;
        }
    }

    return SUCCESS_RETURN;
}


/* Send keepalive packet */
static int MQTTKeepalive(iotx_mc_client_t *pClient)
{
    int len = 0;
    int rc = 0;
    /* there is no ping outstanding - send ping packet */
    iotx_time_t timer;

    if (!pClient) {
        return FAIL_RETURN;
    }

    iotx_time_init(&timer);
    utils_time_countdown_ms(&timer, 1000);

    HAL_MutexLock(pClient->lock_write_buf);

    ALLOC_SERIALIZE_BUF(pClient, buf_send, buf_size_send, buf_size_send_max, 0, 0);
    if (!pClient->buf_send) {
        HAL_MutexUnlock(pClient->lock_write_buf);
        return FAIL_RETURN;
    }

    len = MQTTSerialize_pingreq((unsigned char *)pClient->buf_send, pClient->buf_size_send);
    mqtt_debug("len = MQTTSerialize_pingreq() = %d", len);

    if (len <= 0) {
        mqtt_err("Serialize ping request is error");

        RESET_SERIALIZE_BUF(pClient, buf_send, buf_size_send);
        HAL_MutexUnlock(pClient->lock_write_buf);
        return MQTT_PING_PACKET_ERROR;
    }

    rc = iotx_mc_send_packet(pClient, pClient->buf_send, len, &timer);
    if (SUCCESS_RETURN != rc) {
        /* ping outstanding, then close socket unsubscribe topic and handle callback function */
        mqtt_err("ping outstanding is error,result = %d", rc);

        RESET_SERIALIZE_BUF(pClient, buf_send, buf_size_send);
        HAL_MutexUnlock(pClient->lock_write_buf);
        return MQTT_NETWORK_ERROR;
    }

    RESET_SERIALIZE_BUF(pClient, buf_send, buf_size_send);
    HAL_MutexUnlock(pClient->lock_write_buf);
    return SUCCESS_RETURN;
}

/* MQTT send connect packet */
int MQTTConnect(iotx_mc_client_t *pClient)
{
    MQTTPacket_connectData *pConnectParams;
    iotx_time_t connectTimer;
    int len = 0;

    if (!pClient) {
        return FAIL_RETURN;
    }

    pConnectParams = &pClient->connect_data;
    HAL_MutexLock(pClient->lock_write_buf);

    ALLOC_SERIALIZE_BUF(pClient, buf_send, buf_size_send, buf_size_send_max, MQTT_CONNECT_REQUIRED_BUFLEN, 0);
    if (!pClient->buf_send) {
        HAL_MutexUnlock(pClient->lock_write_buf);
        return FAIL_RETURN;
    }

    if ((len = MQTTSerialize_connect((unsigned char *)pClient->buf_send, pClient->buf_size_send, pConnectParams)) <= 0) {
        mqtt_err("Serialize connect packet failed,len = %d", len);

        RESET_SERIALIZE_BUF(pClient, buf_send, buf_size_send);
        HAL_MutexUnlock(pClient->lock_write_buf);
        return MQTT_CONNECT_PACKET_ERROR;
    }

    /* send the connect packet */
    iotx_time_init(&connectTimer);
    utils_time_countdown_ms(&connectTimer, pClient->request_timeout_ms);
    if ((iotx_mc_send_packet(pClient, pClient->buf_send, len, &connectTimer)) != SUCCESS_RETURN) {
        mqtt_err("send connect packet failed");

        RESET_SERIALIZE_BUF(pClient, buf_send, buf_size_send);
        HAL_MutexUnlock(pClient->lock_write_buf);
        return MQTT_NETWORK_ERROR;
    }

    RESET_SERIALIZE_BUF(pClient, buf_send, buf_size_send);
    HAL_MutexUnlock(pClient->lock_write_buf);
    return SUCCESS_RETURN;
}

int MQTTPublish(iotx_mc_client_t *c, const char *topicName, iotx_mqtt_topic_info_pt topic_msg)

{
    list_node_t         *node = NULL;
    iotx_time_t         timer;
    MQTTString          topic = MQTTString_initializer;
    int                 len = 0;

    if (!c || !topicName || !topic_msg) {
        return FAIL_RETURN;
    }

    topic.cstring = (char *)topicName;
    iotx_time_init(&timer);
    utils_time_countdown_ms(&timer, c->request_timeout_ms);

    HAL_MutexLock(c->lock_list_pub);
    HAL_MutexLock(c->lock_write_buf);

    ALLOC_SERIALIZE_BUF(c, buf_send, buf_size_send, buf_size_send_max, strlen(topicName) + topic_msg->payload_len,
                        topicName);
    if (!c->buf_send) {
        HAL_MutexUnlock(c->lock_write_buf);
        HAL_MutexUnlock(c->lock_list_pub);
        return FAIL_RETURN;
    }

    len = MQTTSerialize_publish((unsigned char *)c->buf_send,
                                c->buf_size_send,
                                0,
                                topic_msg->qos,
                                topic_msg->retain,
                                topic_msg->packet_id,
                                topic,
                                (unsigned char *)topic_msg->payload,
                                topic_msg->payload_len);
    if (len <= 0) {
        mqtt_err("MQTTSerialize_publish is error, len=%d, buf_size_send=%u, payloadlen=%u",
                 len,
                 c->buf_size_send,
                 topic_msg->payload_len);

        RESET_SERIALIZE_BUF(c, buf_send, buf_size_send);
        HAL_MutexUnlock(c->lock_write_buf);
        HAL_MutexUnlock(c->lock_list_pub);
        return MQTT_PUBLISH_PACKET_ERROR;
    }


    /* If the QOS >1, push the information into list of wait publish ACK */
    if (topic_msg->qos > IOTX_MQTT_QOS0) {
        /* push into list */
        if (SUCCESS_RETURN != iotx_mc_push_pubInfo_to(c, len, topic_msg->packet_id, &node)) {
            mqtt_err("push publish into to pubInfolist failed!");

            RESET_SERIALIZE_BUF(c, buf_send, buf_size_send);
            HAL_MutexUnlock(c->lock_write_buf);
            HAL_MutexUnlock(c->lock_list_pub);
            return MQTT_PUSH_TO_LIST_ERROR;
        }
    }

    /* send the publish packet */
    if (iotx_mc_send_packet(c, c->buf_send, len, &timer) != SUCCESS_RETURN) {
        if (topic_msg->qos > IOTX_MQTT_QOS0) {
            /* If not even successfully sent to IP stack, meaningless to wait QOS1 ack, give up waiting */
            list_remove(c->list_pub_wait_ack, node);
        }

        RESET_SERIALIZE_BUF(c, buf_send, buf_size_send);
        HAL_MutexUnlock(c->lock_write_buf);
        HAL_MutexUnlock(c->lock_list_pub);
        return MQTT_NETWORK_ERROR;
    }

#if WITH_MQTT_JSON_FLOW

    const char     *json_payload = (const char *)topic_msg->payload;

    mqtt_info("Upstream Topic: '%s'", topicName);
    mqtt_info("Upstream Payload:");
    iotx_facility_json_print(json_payload, LOG_INFO_LEVEL, '>');

#endif  /* #if WITH_MQTT_JSON_FLOW */

    RESET_SERIALIZE_BUF(c, buf_send, buf_size_send);
    HAL_MutexUnlock(c->lock_write_buf);
    HAL_MutexUnlock(c->lock_list_pub);

    return SUCCESS_RETURN;
}


/* MQTT send publish ACK */
static int MQTTPuback(iotx_mc_client_t *c, unsigned int msgId, enum msgTypes type)
{
    int rc = 0;
    int len = 0;
    iotx_time_t timer;

    if (!c) {
        return FAIL_RETURN;
    }

    iotx_time_init(&timer);
    utils_time_countdown_ms(&timer, c->request_timeout_ms);

    HAL_MutexLock(c->lock_write_buf);
    if (type == PUBACK) {
        ALLOC_SERIALIZE_BUF(c, buf_send, buf_size_send, buf_size_send_max, 0, 0);
        if (!c->buf_send) {
            HAL_MutexUnlock(c->lock_write_buf);
            return FAIL_RETURN;
        }

        len = MQTTSerialize_ack((unsigned char *)c->buf_send, c->buf_size_send, PUBACK, 0, msgId);
#if WITH_MQTT_QOS2_PACKET
    } else if (type == PUBREC) {
        ALLOC_SERIALIZE_BUF(c, buf_send, buf_size_send, buf_size_send_max, 0, 0);
        if (!c->buf_send) {
            HAL_MutexUnlock(c->lock_write_buf);
            return FAIL_RETURN;
        }
        len = MQTTSerialize_ack((unsigned char *)c->buf_send, c->buf_size_send, PUBREC, 0, msgId);
    } else if (type == PUBREL) {
        ALLOC_SERIALIZE_BUF(c, buf_send, buf_size_send, buf_size_send_max, 0, 0);
        if (!c->buf_send) {
            HAL_MutexUnlock(c->lock_write_buf);
            return FAIL_RETURN;
        }
        len = MQTTSerialize_ack((unsigned char *)c->buf_send, c->buf_size_send, PUBREL, 0, msgId);
#endif  /* #if WITH_MQTT_QOS2_PACKET */
    } else {
        HAL_MutexUnlock(c->lock_write_buf);
        return MQTT_PUBLISH_ACK_TYPE_ERROR;
    }

    if (len <= 0) {
        RESET_SERIALIZE_BUF(c, buf_send, buf_size_send);
        HAL_MutexUnlock(c->lock_write_buf);
        return MQTT_PUBLISH_ACK_PACKET_ERROR;
    }

    rc = iotx_mc_send_packet(c, c->buf_send, len, &timer);
    if (rc != SUCCESS_RETURN) {
        RESET_SERIALIZE_BUF(c, buf_send, buf_size_send);
        HAL_MutexUnlock(c->lock_write_buf);
        return MQTT_NETWORK_ERROR;
    }

    RESET_SERIALIZE_BUF(c, buf_send, buf_size_send);
    HAL_MutexUnlock(c->lock_write_buf);
    return SUCCESS_RETURN;
}

static int remove_handle_from_list(iotx_mc_client_t *c, iotx_mc_topic_handle_t *h)
{
    iotx_mc_topic_handle_t **hp, *h1;

    hp = &c->first_sub_handle;
    while ((*hp) != NULL) {
        h1 = *hp;
        if (h1 == h) {
            *hp = h->next;
        } else {
            hp = &h1->next;
        }
    }

    return 0;
}

/* MQTT send subscribe packet */
static int MQTTSubscribe(iotx_mc_client_t *c, const char *topicFilter, iotx_mqtt_qos_t qos, unsigned int msgId,
                         iotx_mqtt_event_handle_func_fpt messageHandler, void *pcontext)
{
    int                         len = 0;
    iotx_time_t                 timer;
    MQTTString                  topic = MQTTString_initializer;
    /*iotx_mc_topic_handle_t handler = {topicFilter, {messageHandler, pcontext}};*/
    iotx_mc_topic_handle_t     *handler = NULL;
    list_node_t                *node = NULL;

    if (!c || !topicFilter || !messageHandler) {
        return FAIL_RETURN;
    }
#if !(WITH_MQTT_DYN_TXBUF)
    if (!c->buf_send) {
        return FAIL_RETURN;
    }
#endif

    topic.cstring = (char *)topicFilter;
    iotx_time_init(&timer);
    utils_time_countdown_ms(&timer, c->request_timeout_ms);

    handler = mqtt_malloc(sizeof(iotx_mc_topic_handle_t));
    if (NULL == handler) {
        return FAIL_RETURN;
    }

#if !(WITH_MQTT_ZIP_TOPIC)
    handler->topic_filter = mqtt_malloc(strlen(topicFilter) + 1);
    if (NULL == handler->topic_filter) {
        mqtt_free(handler);
        return FAIL_RETURN;
    }
    memcpy((char *)handler->topic_filter, topicFilter, strlen(topicFilter) + 1);
#else
    if (strstr(topicFilter, "/+") != NULL || strstr(topicFilter, "/#") != NULL) {
        handler->topic_filter = mqtt_malloc(strlen(topicFilter) + 1);
        if (NULL == handler->topic_filter) {
            mqtt_free(handler);
            return FAIL_RETURN;
        }
        handler->topic_type = TOPIC_FILTER_TYPE;
        memcpy((char *)handler->topic_filter, topicFilter, strlen(topicFilter) + 1);
    } else {
        handler->topic_filter = mqtt_malloc(MQTT_MD5_PATH_DEFAULT_LEN);
        if (NULL == handler->topic_filter) {
            mqtt_free(handler);
            return FAIL_RETURN;
        }
        handler->topic_type = TOPIC_NAME_TYPE;
        if (iotx_mc_get_md5_topic(topicFilter, strlen(topicFilter), (char *)handler->topic_filter,
                                  MQTT_MD5_PATH_DEFAULT_LEN) != 0) {
            mqtt_free(handler->topic_filter);
            mqtt_free(handler);
            return FAIL_RETURN;
        }
    }
#endif
    handler->handle.h_fp = messageHandler;
    handler->handle.pcontext = pcontext;

    HAL_MutexLock(c->lock_write_buf);
    ALLOC_SERIALIZE_BUF(c, buf_send, buf_size_send, buf_size_send_max, strlen(topicFilter), topicFilter);
    if (!c->buf_send) {
        HAL_MutexUnlock(c->lock_write_buf);
        mqtt_free(handler->topic_filter);
        mqtt_free(handler);
        return FAIL_RETURN;
    }

    len = MQTTSerialize_subscribe((unsigned char *)c->buf_send, c->buf_size_send, 0, (unsigned short)msgId, 1, &topic,
                                  (int *)&qos);
    if (len <= 0) {
        mqtt_free(handler->topic_filter);
        mqtt_free(handler);

        RESET_SERIALIZE_BUF(c, buf_send, buf_size_send);
        HAL_MutexUnlock(c->lock_write_buf);
        return MQTT_SUBSCRIBE_PACKET_ERROR;
    }

    /*
     * NOTE: It prefer to push the element into list and then remove it when send failed,
     *       because some of extreme cases
     * */

    /* push the element to list of wait subscribe ACK */
    if (SUCCESS_RETURN != iotx_mc_push_subInfo_to(c, len, msgId, SUBSCRIBE, handler, &node)) {
        mqtt_err("push publish into to pubInfolist failed!");
        mqtt_free(handler->topic_filter);
        mqtt_free(handler);

        RESET_SERIALIZE_BUF(c, buf_send, buf_size_send);
        HAL_MutexUnlock(c->lock_write_buf);
        return MQTT_PUSH_TO_LIST_ERROR;
    }

    mqtt_debug("%20s : %08d", "Packet Ident", msgId);
    mqtt_debug("%20s : %s", "Topic", topicFilter);
    mqtt_debug("%20s : %d", "QoS", (int)qos);
    mqtt_debug("%20s : %d", "Packet Length", len);
#if defined(INSPECT_MQTT_FLOW)
    HEXDUMP_DEBUG(c->buf_send, len);
#endif

    if ((iotx_mc_send_packet(c, c->buf_send, len, &timer)) != SUCCESS_RETURN) { /* send the subscribe packet */
        /* If send failed, remove it */
        HAL_MutexLock(c->lock_list_sub);
        list_remove(c->list_sub_wait_ack, node);
        HAL_MutexUnlock(c->lock_list_sub);
        mqtt_err("run sendPacket error!");
        mqtt_free(handler->topic_filter);
        mqtt_free(handler);

        RESET_SERIALIZE_BUF(c, buf_send, buf_size_send);
        HAL_MutexUnlock(c->lock_write_buf);
        return MQTT_NETWORK_ERROR;
    }

    RESET_SERIALIZE_BUF(c, buf_send, buf_size_send);
    HAL_MutexUnlock(c->lock_write_buf);

#if (WITH_MQTT_SUB_SHORTCUT)
    iotx_mc_topic_handle_t *h = mqtt_malloc(sizeof(iotx_mc_topic_handle_t));
    if (h == NULL) {
        /* If send failed, remove it */
        HAL_MutexLock(c->lock_list_sub);
        list_remove(c->list_sub_wait_ack, node);
        HAL_MutexUnlock(c->lock_list_sub);
        mqtt_err("run sendPacket error!");
        mqtt_free(handler->topic_filter);
        mqtt_free(handler);
        RESET_SERIALIZE_BUF(c, buf_send, buf_size_send);
        mqtt_err("maloc failed!");
        return FAIL_RETURN;
    }

    memset(h, 0, sizeof(iotx_mc_topic_handle_t));
    h->topic_filter = handler->topic_filter;
    h->handle.h_fp =  handler->handle.h_fp;
    h->handle.pcontext = handler->handle.pcontext;
    h->topic_type =  handler->topic_type;

    HAL_MutexLock(c->lock_generic);
    h->next = c->first_sub_handle;
    c->first_sub_handle = h;
    HAL_MutexUnlock(c->lock_generic);
#endif
    _dump_wait_list(c, "sub");

    return SUCCESS_RETURN;
}


/* MQTT send unsubscribe packet */
static int MQTTUnsubscribe(iotx_mc_client_t *c, const char *topicFilter, unsigned int msgId)
{
    iotx_time_t timer;
    MQTTString topic = MQTTString_initializer;
    int len = 0;
    /*iotx_mc_topic_handle_t handler = {topicFilter, {NULL, NULL}};*/
    iotx_mc_topic_handle_t *handler = NULL;

    /* push into list */
    list_node_t *node = NULL;

    if (!c || !topicFilter) {
        return FAIL_RETURN;
    }

    topic.cstring = (char *)topicFilter;
    iotx_time_init(&timer);
    utils_time_countdown_ms(&timer, c->request_timeout_ms);

    handler = mqtt_malloc(sizeof(iotx_mc_topic_handle_t));
    if (NULL == handler) {
        return FAIL_RETURN;
    }

#if !(WITH_MQTT_ZIP_TOPIC)
    handler->topic_filter = mqtt_malloc(strlen(topicFilter) + 1);
    if (NULL == handler->topic_filter) {
        mqtt_free(handler);
        return FAIL_RETURN;
    }
    memcpy((char *)handler->topic_filter, topicFilter, strlen(topicFilter) + 1);
#else
    if (strstr(topicFilter, "/+") != NULL || strstr(topicFilter, "/#") != NULL) {
        handler->topic_filter = mqtt_malloc(strlen(topicFilter) + 1);
        if (NULL == handler->topic_filter) {
            mqtt_free(handler);
            return FAIL_RETURN;
        }
        handler->topic_type = TOPIC_FILTER_TYPE;
        memcpy((char *)handler->topic_filter, topicFilter, strlen(topicFilter) + 1);
    } else {
        handler->topic_filter = mqtt_malloc(MQTT_MD5_PATH_DEFAULT_LEN);
        if (NULL == handler->topic_filter) {
            mqtt_free(handler);
            return FAIL_RETURN;
        }
        handler->topic_type = TOPIC_NAME_TYPE;
        if (iotx_mc_get_md5_topic(topicFilter, strlen(topicFilter), (char *)handler->topic_filter,
                                  MQTT_MD5_PATH_DEFAULT_LEN) != 0) {
            mqtt_free(handler->topic_filter);
            mqtt_free(handler);
            return FAIL_RETURN;
        }
    }
#endif

    HAL_MutexLock(c->lock_write_buf);

    ALLOC_SERIALIZE_BUF(c, buf_send, buf_size_send, buf_size_send_max, strlen(topicFilter), topicFilter);
    if (NULL == c->buf_send) {
        HAL_MutexUnlock(c->lock_write_buf);
        mqtt_free(handler->topic_filter);
        mqtt_free(handler);
        return FAIL_RETURN;
    }

    if ((len = MQTTSerialize_unsubscribe((unsigned char *)c->buf_send, c->buf_size_send, 0, (unsigned short)msgId, 1,
                                         &topic)) <= 0) {
        mqtt_free(handler->topic_filter);
        mqtt_free(handler);

        RESET_SERIALIZE_BUF(c, buf_send, buf_size_send);
        HAL_MutexUnlock(c->lock_write_buf);
        return MQTT_UNSUBSCRIBE_PACKET_ERROR;
    }

    if (SUCCESS_RETURN != iotx_mc_push_subInfo_to(c, len, msgId, UNSUBSCRIBE, handler, &node)) {
        mqtt_err("push publish into to pubInfolist failed!");
        mqtt_free(handler->topic_filter);
        mqtt_free(handler);

        RESET_SERIALIZE_BUF(c, buf_send, buf_size_send);
        HAL_MutexUnlock(c->lock_write_buf);
        return MQTT_PUSH_TO_LIST_ERROR;
    }

    if ((iotx_mc_send_packet(c, c->buf_send, len, &timer)) != SUCCESS_RETURN) { /* send the subscribe packet */
        /* remove from list */
        HAL_MutexLock(c->lock_list_sub);
        list_remove(c->list_sub_wait_ack, node);
        HAL_MutexUnlock(c->lock_list_sub);
        mqtt_free(handler->topic_filter);
        mqtt_free(handler);

        RESET_SERIALIZE_BUF(c, buf_send, buf_size_send);
        HAL_MutexUnlock(c->lock_write_buf);
        return MQTT_NETWORK_ERROR;
    }

    MQTTString cur_topic;
    cur_topic.cstring = NULL;
    cur_topic.lenstring.data = (char *)handler->topic_filter;

#if !(WITH_MQTT_ZIP_TOPIC)
    cur_topic.lenstring.len = strlen(handler->topic_filter) + 1;
#else
    if (handler->topic_type == TOPIC_FILTER_TYPE) {
        cur_topic.lenstring.len = strlen(handler->topic_filter) + 1;
    } else {
        cur_topic.lenstring.len = MQTT_MD5_PATH_DEFAULT_LEN;
    }
#endif


    /* we have to find the right message handler - indexed by topic */
    HAL_MutexLock(c->lock_generic);
    iotx_mc_topic_handle_t *h, *h2;
    for (h = c->first_sub_handle; h != NULL; h = h2) {
        h2 = h->next;
        if (MQTTPacket_equals(&cur_topic, (char *)h->topic_filter)
            || iotx_mc_is_topic_matched((char *)h->topic_filter, &cur_topic)) {
            mqtt_debug("topic be matched");
            remove_handle_from_list(c, h);
            mqtt_free(h->topic_filter);
            mqtt_free(h);
        }

    }
    HAL_MutexUnlock(c->lock_generic);

    RESET_SERIALIZE_BUF(c, buf_send, buf_size_send);
    HAL_MutexUnlock(c->lock_write_buf);
    return SUCCESS_RETURN;
}


/* MQTT send disconnect packet */
static int MQTTDisconnect(iotx_mc_client_t *c)
{
    int             rc = FAIL_RETURN;
    int             len = 0;
    iotx_time_t     timer;     /* we might wait for incomplete incoming publishes to complete */

    if (!c) {
        return FAIL_RETURN;
    }

    HAL_MutexLock(c->lock_write_buf);

    ALLOC_SERIALIZE_BUF(c, buf_send, buf_size_send, buf_size_send_max, 0, 0);
    if (!c->buf_send) {
        HAL_MutexUnlock(c->lock_write_buf);
        return FAIL_RETURN;
    }

    len = MQTTSerialize_disconnect((unsigned char *)c->buf_send, c->buf_size_send);

    iotx_time_init(&timer);
    utils_time_countdown_ms(&timer, c->request_timeout_ms);

    if (len > 0) {
        rc = iotx_mc_send_packet(c, c->buf_send, len, &timer);           /* send the disconnect packet */
    }

    RESET_SERIALIZE_BUF(c, buf_send, buf_size_send);
    HAL_MutexUnlock(c->lock_write_buf);
    return rc;
}

/* remove the list element specified by @msgId from list of wait publish ACK */
/* return: 0, success; NOT 0, fail; */
static int iotx_mc_mask_pubInfo_from(iotx_mc_client_t *c, uint16_t msgId)
{
    if (!c) {
        return FAIL_RETURN;
    }

    HAL_MutexLock(c->lock_list_pub);
    if (c->list_pub_wait_ack->len) {
        list_iterator_t *iter;
        list_node_t *node = NULL;
        iotx_mc_pub_info_t *repubInfo = NULL;

        if (NULL == (iter = list_iterator_new(c->list_pub_wait_ack, LIST_TAIL))) {
            HAL_MutexUnlock(c->lock_list_pub);
            return SUCCESS_RETURN;
        }


        for (;;) {
            node = list_iterator_next(iter);

            if (NULL == node) {
                break;
            }

            repubInfo = (iotx_mc_pub_info_t *) node->val;
            if (NULL == repubInfo) {
                mqtt_err("node's value is invalid!");
                continue;
            }

            if (repubInfo->msg_id == msgId) {
                repubInfo->node_state = IOTX_MC_NODE_STATE_INVALID; /* mark as invalid node */
            }
        }

        list_iterator_destroy(iter);
    }
    HAL_MutexUnlock(c->lock_list_pub);

    return SUCCESS_RETURN;
}


/* push the wait element into list of wait publish ACK */
/* return: 0, success; NOT 0, fail; */
static int iotx_mc_push_pubInfo_to(iotx_mc_client_t *c, int len, unsigned short msgId, list_node_t **node)
{
    if (!c || !node) {
        mqtt_err("the param of c is error!");
        return FAIL_RETURN;
    }

    if ((len < 0) || (len > c->buf_size_send)) {
        mqtt_err("the param of len is error!");
        return FAIL_RETURN;
    }

    if (c->list_pub_wait_ack->len >= IOTX_MC_REPUB_NUM_MAX) {
        mqtt_err("more than %u elements in republish list. List overflow!", c->list_pub_wait_ack->len);
        return FAIL_RETURN;
    }

    iotx_mc_pub_info_t *repubInfo = (iotx_mc_pub_info_t *)mqtt_malloc(sizeof(iotx_mc_pub_info_t) + len);
    if (NULL == repubInfo) {
        mqtt_err("run iotx_memory_malloc is error!");
        return FAIL_RETURN;
    }

    repubInfo->node_state = IOTX_MC_NODE_STATE_NORMANL;
    repubInfo->msg_id = msgId;
    repubInfo->len = len;
    iotx_time_start(&repubInfo->pub_start_time);
    repubInfo->buf = (unsigned char *)repubInfo + sizeof(iotx_mc_pub_info_t);

    memcpy(repubInfo->buf, c->buf_send, len);

    *node = list_node_new(repubInfo);
    if (NULL == *node) {
        mqtt_err("run list_node_new is error!");
        return FAIL_RETURN;
    }

    list_rpush(c->list_pub_wait_ack, *node);

    return SUCCESS_RETURN;
}


/* push the wait element into list of wait subscribe(unsubscribe) ACK */
/* return: 0, success; NOT 0, fail; */
static int iotx_mc_push_subInfo_to(iotx_mc_client_t *c, int len, unsigned short msgId, enum msgTypes type,
                                   iotx_mc_topic_handle_t *handler,
                                   list_node_t **node)
{
    if (!c || !handler || !node) {
        return FAIL_RETURN;
    }

    HAL_MutexLock(c->lock_list_sub);

    mqtt_debug("c->list_sub_wait_ack->len:IOTX_MC_SUB_REQUEST_NUM_MAX = %d:%d",
               c->list_sub_wait_ack->len,
               IOTX_MC_SUB_REQUEST_NUM_MAX);
    _dump_wait_list(c, "sub");

    if (c->list_sub_wait_ack->len >= IOTX_MC_SUB_REQUEST_NUM_MAX) {
        HAL_MutexUnlock(c->lock_list_sub);
        mqtt_err("number of subInfo more than max!,size = %d", c->list_sub_wait_ack->len);
        return FAIL_RETURN;
    }

    iotx_mc_subsribe_info_t *subInfo = (iotx_mc_subsribe_info_t *)mqtt_malloc(sizeof(
            iotx_mc_subsribe_info_t) + len);
    if (NULL == subInfo) {
        HAL_MutexUnlock(c->lock_list_sub);
        mqtt_err("run iotx_memory_malloc is error!");
        return FAIL_RETURN;
    }

    subInfo->node_state = IOTX_MC_NODE_STATE_NORMANL;
    subInfo->msg_id = msgId;
    subInfo->len = len;
    iotx_time_start(&subInfo->sub_start_time);
    subInfo->type = type;
    subInfo->handler = handler;

#if 0
    subInfo->buf = (unsigned char *)subInfo + sizeof(iotx_mc_subsribe_info_t);
    memcpy(subInfo->buf, c->buf_send, len);
#endif

    *node = list_node_new(subInfo);
    if (NULL == *node) {
        HAL_MutexUnlock(c->lock_list_sub);
        mqtt_free(subInfo);
        mqtt_err("run list_node_new is error!");
        return FAIL_RETURN;
    }

    list_rpush(c->list_sub_wait_ack, *node);

    HAL_MutexUnlock(c->lock_list_sub);

    return SUCCESS_RETURN;
}

/* Required to run in lock protection setup by caller */
static int _dump_wait_list(iotx_mc_client_t *c, const char *type)
{
#ifdef INSPECT_MQTT_LIST
    list_t          *dump_list = NULL;

    ARGUMENT_SANITY_CHECK(c != NULL, FAIL_RETURN);
    ARGUMENT_SANITY_CHECK(type != NULL, FAIL_RETURN);

    if (strlen(type) && !strcmp(type, "sub")) {
        dump_list = c->list_sub_wait_ack;
    } else if (strlen(type) && !strcmp(type, "pub")) {
        dump_list = c->list_pub_wait_ack;
    } else {
        mqtt_err("Invalid argument of type = '%s', abort", type);
    }

    mqtt_debug("LIST type = %s, LIST len = %d", type, dump_list->len);
    mqtt_debug("********");
    if (dump_list->len) {
        list_iterator_t             *iter;
        list_node_t                 *node = NULL;
        iotx_mc_subsribe_info_t     *subInfo = NULL;
        iotx_mc_pub_info_t          *pubInfo = NULL;

        if (NULL == (iter = list_iterator_new(dump_list, LIST_TAIL))) {
            return SUCCESS_RETURN;
        }

        iotx_mc_topic_handle_t *debug_handler = NULL;
        char *topic_filter = NULL;

        for (;;) {
            node = list_iterator_next(iter);
            if (NULL == node) {
                break;
            }

            if (strlen(type) && !strcmp(type, "sub")) {
                subInfo = (iotx_mc_subsribe_info_t *) node->val;
                if (!subInfo) {
                    mqtt_err("sub node's value is invalid!");
                    continue;
                }
                debug_handler = subInfo->handler;
                topic_filter = (char *)((debug_handler) ? (debug_handler->topic_filter) : ("NULL"));
#if 0
                HEXDUMP_DEBUG(subInfo->handler->topic_filter, 32);
                mqtt_debug("[%d] %-32s(%d) | %p | %-8s | %-6s |",
                           subInfo->msg_id,
                           (subInfo->handler->topic_filter[0] == '/') ? subInfo->handler->topic_filter : "+ N/A +",
#else
                mqtt_debug("[%d] %s(%d) | %p | %-8s | %-6s |",
                           subInfo->msg_id,
                           topic_filter,
#endif
                           subInfo->len,
                           debug_handler,
                           (subInfo->node_state == IOTX_MC_NODE_STATE_INVALID) ? "INVALID" : "NORMAL",
                           (subInfo->type == SUBSCRIBE) ? "SUB" : "UNSUB"
                          );
            } else {
                pubInfo = (iotx_mc_pub_info_t *) node->val;
                if (!pubInfo) {
                    mqtt_err("pub node's value is invalid!");
                    continue;
                }
            }
        }

        list_iterator_destroy(iter);
    }
    mqtt_debug("********");

#endif
    return SUCCESS_RETURN;
}

/* remove the list element specified by @msgId from list of wait subscribe(unsubscribe) ACK */
/* and return message handle by @messageHandler */
/* return: 0, success; NOT 0, fail; */
static int iotx_mc_mask_subInfo_from(iotx_mc_client_t *c, unsigned int msgId, iotx_mc_topic_handle_t **messageHandler)
{
    if (!c/* || !messageHandler*/) {
        return FAIL_RETURN;
    }

    if (c->list_sub_wait_ack->len) {
        list_iterator_t *iter;
        list_node_t *node = NULL;
        iotx_mc_subsribe_info_t *subInfo = NULL;

        if (NULL == (iter = list_iterator_new(c->list_sub_wait_ack, LIST_TAIL))) {
            return SUCCESS_RETURN;
        }

        for (;;) {
            node = list_iterator_next(iter);
            if (NULL == node) {
                break;
            }

            subInfo = (iotx_mc_subsribe_info_t *) node->val;
            if (NULL == subInfo) {
                mqtt_err("node's value is invalid!");
                continue;
            }

            if (subInfo->msg_id == msgId) {
                *messageHandler = subInfo->handler;
                subInfo->handler = NULL;
                subInfo->node_state = IOTX_MC_NODE_STATE_INVALID; /* mark as invalid node */
            }
        }

        list_iterator_destroy(iter);
    }
    _dump_wait_list(c, "sub");

    return SUCCESS_RETURN;
}


/* get next packet-id */
static int iotx_mc_get_next_packetid(iotx_mc_client_t *c)
{
    unsigned int id = 0;

    if (!c) {
        return FAIL_RETURN;
    }

    HAL_MutexLock(c->lock_generic);
    c->packet_id = (c->packet_id == IOTX_MC_PACKET_ID_MAX) ? 1 : c->packet_id + 1;
    id = c->packet_id;
    HAL_MutexUnlock(c->lock_generic);

    return id;
}


/* send packet */
static int iotx_mc_send_packet(iotx_mc_client_t *c, char *buf, int length, iotx_time_t *time)
{
    int rc = FAIL_RETURN;
    int sent = 0;
    unsigned int left_t = 0;

    if (!c || !buf || !time) {
        return rc;
    }

    while (sent < length && !utils_time_is_expired(time)) {
        left_t = iotx_time_left(time);
        left_t = (left_t == 0) ? 1 : left_t;
        rc = c->ipstack->write(c->ipstack, &buf[sent], length, left_t);
        if (rc < 0) { /* there was an error writing the data */
            break;
        }
        sent += rc;
    }

    if (sent == length) {
        rc = SUCCESS_RETURN;
    } else {
        rc = MQTT_NETWORK_ERROR;
    }
    return rc;
}


/* decode packet */
static int iotx_mc_decode_packet(iotx_mc_client_t *c, int *value, int timeout)
{
    char i;
    int multiplier = 1;
    int len = 0;
    const int MAX_NO_OF_REMAINING_LENGTH_BYTES = 4;

    if (!c || !value) {
        return FAIL_RETURN;
    }

    *value = 0;
    do {
        int rc = MQTTPACKET_READ_ERROR;

        if (++len > MAX_NO_OF_REMAINING_LENGTH_BYTES) {
            return MQTTPACKET_READ_ERROR; /* bad data */
        }

        rc = c->ipstack->read(c->ipstack, &i, 1, timeout == 0 ? 1 : timeout);
        if (rc != 1) {
            return MQTT_NETWORK_ERROR;
        }

        *value += (i & 127) * multiplier;
        multiplier *= 128;
    } while ((i & 128) != 0);

    return len;
}


/* read packet */
static int iotx_mc_read_packet(iotx_mc_client_t *c, iotx_time_t *timer, unsigned int *packet_type)
{
    MQTTHeader header = {0};
    int len = 0;
    int rem_len = 0;
    int rc = 0;
    unsigned int left_t = 0;

    if (!c || !timer || !packet_type) {
        return FAIL_RETURN;
    }

    /* 1. read the header byte.  This has the packet type in it */
    left_t = iotx_time_left(timer);
    left_t = (left_t == 0) ? 1 : left_t;
    rc = c->ipstack->read(c->ipstack, c->buf_read, 1, left_t);
    if (0 == rc) { /* timeout */
        *packet_type = 0;
        return SUCCESS_RETURN;
    } else if (1 != rc) {
        mqtt_debug("mqtt read error, rc=%d", rc);
        return FAIL_RETURN;
    }

    len = 1;

    /* 2. read the remaining length.  This is variable in itself */
    left_t = iotx_time_left(timer);
    left_t = (left_t == 0) ? 1 : left_t;
    if ((rc = iotx_mc_decode_packet(c, &rem_len, left_t)) < 0) {
        mqtt_err("decodePacket error,rc = %d", rc);
        return rc;
    }

    len += MQTTPacket_encode((unsigned char *)c->buf_read + 1,
                             rem_len); /* put the original remaining length back into the buffer */

    /* Check if the received data length exceeds mqtt read buffer length */
    if ((rem_len > 0) && ((rem_len + len) > c->buf_size_read)) {
        mqtt_err("mqtt read buffer is too short, mqttReadBufLen : %u, remainDataLen : %d", c->buf_size_read, rem_len);
        int needReadLen = c->buf_size_read - len;
        left_t = iotx_time_left(timer);
        left_t = (left_t == 0) ? 1 : left_t;
        if (c->ipstack->read(c->ipstack, c->buf_read + len, needReadLen, left_t) != needReadLen) {
            mqtt_err("mqtt read error");
            return FAIL_RETURN;
        }

        /* drop data whitch over the length of mqtt buffer */
        int remainDataLen = rem_len - needReadLen;
        char *remainDataBuf = mqtt_malloc(remainDataLen + 1);
        if (!remainDataBuf) {
            mqtt_err("allocate remain buffer failed");
            return FAIL_RETURN;
        }

        left_t = iotx_time_left(timer);
        left_t = (left_t == 0) ? 1 : left_t;
        if (c->ipstack->read(c->ipstack, remainDataBuf, remainDataLen, left_t) != remainDataLen) {
            mqtt_err("mqtt read error");
            mqtt_free(remainDataBuf);
            remainDataBuf = NULL;
            return FAIL_RETURN;
        }

        mqtt_free(remainDataBuf);
        remainDataBuf = NULL;

        if (NULL != c->handle_event.h_fp) {
            iotx_mqtt_event_msg_t msg;

            msg.event_type = IOTX_MQTT_EVENT_BUFFER_OVERFLOW;
            msg.msg = "mqtt read buffer is too short";
            _handle_event(&c->handle_event, c, &msg);
        }

        return SUCCESS_RETURN;

    }

    /* 3. read the rest of the buffer using a callback to supply the rest of the data */
    left_t = iotx_time_left(timer);
    left_t = (left_t == 0) ? 1 : left_t;
    if (rem_len > 0 && (c->ipstack->read(c->ipstack, c->buf_read + len, rem_len, left_t) != rem_len)) {
        mqtt_err("mqtt read error");
        return FAIL_RETURN;
    }

    header.byte = c->buf_read[0];
    *packet_type = header.bits.type;
    if ((len + rem_len) < c->buf_size_read) {
        c->buf_read[len + rem_len] = '\0';
    }
    return SUCCESS_RETURN;
}

/* deliver message */
static void iotx_mc_deliver_message(iotx_mc_client_t *c, MQTTString *topicName, iotx_mqtt_topic_info_pt topic_msg)
{
    int flag_matched = 0;

    if (!c || !topicName || !topic_msg) {
        return;
    }

    topic_msg->ptopic = topicName->lenstring.data;
    topic_msg->topic_len = topicName->lenstring.len;

#if WITH_MQTT_ZIP_TOPIC

    MQTTString      md5_topic;
    char            md5_topic_data[MQTT_MD5_PATH_DEFAULT_LEN] = {0};
    char           *net_topic;
    uint32_t        net_topic_len;

    if (topicName->cstring) {
        net_topic = topicName->cstring;
        net_topic_len = strlen(topicName->cstring);
    } else {
        net_topic = topicName->lenstring.data;
        net_topic_len = topicName->lenstring.len;
    }
    md5_topic.cstring = NULL;
    md5_topic.lenstring.data = md5_topic_data;
    md5_topic.lenstring.len = MQTT_MD5_PATH_DEFAULT_LEN;
    iotx_mc_get_md5_topic(net_topic, net_topic_len, md5_topic_data, MQTT_MD5_PATH_DEFAULT_LEN);
    /* we have to find the right message handler - indexed by topic */
    HAL_MutexLock(c->lock_generic);
    iotx_mc_topic_handle_t *h;
    for (h = c->first_sub_handle; h != NULL; h = h->next) {
        if (MQTTPacket_equals(&md5_topic, (char *)h->topic_filter)
            || iotx_mc_is_topic_matched((char *)h->topic_filter, topicName)) {
#else
    /* we have to find the right message handler - indexed by topic */
    HAL_MutexLock(c->lock_generic);
    iotx_mc_topic_handle_t *h;
    for (h = c->first_sub_handle; h != NULL; h = h->next) {
        if (MQTTPacket_equals(topicName, (char *)h->topic_filter)
            || iotx_mc_is_topic_matched((char *)h->topic_filter, topicName)) {
#endif
            mqtt_debug("topic be matched");

            iotx_mc_topic_handle_t *msg_handle = h;
            HAL_MutexUnlock(c->lock_generic);

            if (NULL != msg_handle->handle.h_fp) {
                iotx_mqtt_event_msg_t msg;
                msg.event_type = IOTX_MQTT_EVENT_PUBLISH_RECEIVED;
                msg.msg = (void *)topic_msg;
                _handle_event(&msg_handle->handle, c, &msg);
                flag_matched = 1;
            }

            HAL_MutexLock(c->lock_generic);
        }
    }

    HAL_MutexUnlock(c->lock_generic);

    if (0 == flag_matched) {
        mqtt_debug("NO matching any topic, call default handle function");

        if (NULL != c->handle_event.h_fp) {
            iotx_mqtt_event_msg_t msg;

            msg.event_type = IOTX_MQTT_EVENT_PUBLISH_RECEIVED;
            msg.msg = topic_msg;
            _handle_event(&c->handle_event, c, &msg);
        }
    }
}

/* handle CONNACK packet received from remote MQTT broker */
static int iotx_mc_handle_recv_CONNACK(iotx_mc_client_t *c)
{
    int rc = SUCCESS_RETURN;
    unsigned char connack_rc = 255;
    char sessionPresent = 0;

    if (!c) {
        return FAIL_RETURN;
    }

    if (MQTTDeserialize_connack((unsigned char *)&sessionPresent, &connack_rc, (unsigned char *)c->buf_read,
                                c->buf_size_read) != 1) {
        mqtt_err("connect ack is error");
        return MQTT_CONNECT_ACK_PACKET_ERROR;
    }

    switch (connack_rc) {
        case IOTX_MC_CONNECTION_ACCEPTED:
            rc = SUCCESS_RETURN;
            break;
        case IOTX_MC_CONNECTION_REFUSED_UNACCEPTABLE_PROTOCOL_VERSION:
            rc = MQTT_CONANCK_UNACCEPTABLE_PROTOCOL_VERSION_ERROR;
            break;
        case IOTX_MC_CONNECTION_REFUSED_IDENTIFIER_REJECTED:
            rc = MQTT_CONNACK_IDENTIFIER_REJECTED_ERROR;
            break;
        case IOTX_MC_CONNECTION_REFUSED_SERVER_UNAVAILABLE:
            rc = MQTT_CONNACK_SERVER_UNAVAILABLE_ERROR;
            break;
        case IOTX_MC_CONNECTION_REFUSED_BAD_USERDATA:
            rc = MQTT_CONNACK_BAD_USERDATA_ERROR;
            break;
        case IOTX_MC_CONNECTION_REFUSED_NOT_AUTHORIZED:
            rc = MQTT_CONNACK_NOT_AUTHORIZED_ERROR;
            break;
        default:
            rc = MQTT_CONNACK_UNKNOWN_ERROR;
            break;
    }

    return rc;
}


/* handle PUBACK packet received from remote MQTT broker */
static int iotx_mc_handle_recv_PUBACK(iotx_mc_client_t *c)
{
    unsigned short mypacketid;
    unsigned char dup = 0;
    unsigned char type = 0;

    if (!c) {
        return FAIL_RETURN;
    }

    if (MQTTDeserialize_ack(&type, &dup, &mypacketid, (unsigned char *)c->buf_read, c->buf_size_read) != 1) {
        return MQTT_PUBLISH_ACK_PACKET_ERROR;
    }

    (void)iotx_mc_mask_pubInfo_from(c, mypacketid);

    /* call callback function to notify that PUBLISH is successful */
    if (NULL != c->handle_event.h_fp) {
        iotx_mqtt_event_msg_t msg;
        msg.event_type = IOTX_MQTT_EVENT_PUBLISH_SUCCESS;
        msg.msg = (void *)(uintptr_t)mypacketid;
        _handle_event(&c->handle_event, c, &msg);
    }

    return SUCCESS_RETURN;
}


/* handle SUBACK packet received from remote MQTT broker */
static int iotx_mc_handle_recv_SUBACK(iotx_mc_client_t *c)
{
    unsigned short mypacketid;
    int i = 0, count = 0, fail_flag = -1, j = 0;
    int grantedQoS[MUTLI_SUBSCIRBE_MAX];
    int flag_dup = 0, rc;

    iotx_mc_topic_handle_t *messagehandler = NULL;

    if (!c) {
        return FAIL_RETURN;
    }

    rc = MQTTDeserialize_suback(&mypacketid, MUTLI_SUBSCIRBE_MAX, &count, grantedQoS, (unsigned char *)c->buf_read,
                                c->buf_size_read);

    if (rc < 0) {
        mqtt_err("Sub ack packet error, rc = MQTTDeserialize_suback() = %d", rc);
        return MQTT_SUBSCRIBE_ACK_PACKET_ERROR;
    }

    mqtt_debug("%20s : %d", "Return Value", rc);
    mqtt_debug("%20s : %d", "Packet ID", mypacketid);
    mqtt_debug("%20s : %d", "Count", count);
    for (i = 0; i < count; ++i) {
        mqtt_debug("%16s[%02d] : %d", "Granted QoS", i, grantedQoS[i]);
    }
#ifdef INSPECT_MQTT_FLOW
#endif

#ifdef INSPECT_MQTT_LIST
    mqtt_debug("receivce SUBACK, packetid: %d", mypacketid);
#endif
    HAL_MutexLock(c->lock_list_sub);
    (void)iotx_mc_mask_subInfo_from(c, mypacketid, &messagehandler);
    HAL_MutexUnlock(c->lock_list_sub);
    if ((NULL == messagehandler)) {
        return MQTT_SUB_INFO_NOT_FOUND_ERROR;
    }

    if ((NULL == messagehandler->handle.h_fp) || (NULL == messagehandler->topic_filter)) {
        mqtt_free(messagehandler);
        return MQTT_SUB_INFO_NOT_FOUND_ERROR;
    }


    for (j = 0; j <  count; j++) {
        flag_dup = 0;
        fail_flag = 0;
        /* In negative case, grantedQoS will be 0xFFFF FF80, which means -128 */
        if ((uint8_t)grantedQoS[j] == 0x80) {
            fail_flag = 1;
            mqtt_err("MQTT SUBSCRIBE failed, ack code is 0x80");
        }
        HAL_MutexLock(c->lock_generic);
        iotx_mc_topic_handle_t *h;
        for (h = c->first_sub_handle; h; h = h->next) {
            /* If subscribe the same topic and callback function, then ignore */
            if (0 == iotx_mc_check_handle_is_identical(h, messagehandler)) {
                /* if subscribe a identical topic and relate callback function, then ignore this subscribe */
                flag_dup = 1;
                mqtt_warning("There is identical topic and related handle in list");
#if (WITH_MQTT_SUB_SHORTCUT)
                if (fail_flag == 1) {
                    remove_handle_from_list(c, h);
                    mqtt_free(h->topic_filter);
                    mqtt_free(h);
                }
#endif
                break;
            }
        }
        HAL_MutexUnlock(c->lock_generic);
        if (fail_flag == 0 && flag_dup == 0) {
            iotx_mc_topic_handle_t *handle = mqtt_malloc(sizeof(iotx_mc_topic_handle_t));
            if (!handle) {
                mqtt_free(messagehandler[j].topic_filter);
                return FAIL_RETURN;
            }

            memset(handle, 0, sizeof(iotx_mc_topic_handle_t));

            handle->topic_filter = messagehandler[j].topic_filter;
            handle->handle.h_fp = messagehandler[j].handle.h_fp;
            handle->handle.pcontext = messagehandler[j].handle.pcontext;
            handle->topic_type =  messagehandler[j].topic_type;

            HAL_MutexLock(c->lock_generic);
            handle->next = c->first_sub_handle;
            c->first_sub_handle = handle;
            HAL_MutexUnlock(c->lock_generic);
        } else {
#if !(WITH_MQTT_SUB_SHORTCUT)
            mqtt_free(messagehandler[j].topic_filter);
#endif
        }
    }

    mqtt_free(messagehandler);
    /* call callback function to notify that SUBSCRIBE is successful */
    iotx_mqtt_event_msg_t msg;
    msg.msg = (void *)(uintptr_t)mypacketid;
    if (fail_flag == 1) {
        msg.event_type = IOTX_MQTT_EVENT_SUBCRIBE_NACK;
    } else {
        msg.event_type = IOTX_MQTT_EVENT_SUBCRIBE_SUCCESS;
    }

    _iotx_mqtt_event_handle_sub(c->handle_event.pcontext, c, &msg);

    if (NULL != c->handle_event.h_fp) {
        _handle_event(&c->handle_event, c, &msg);
    }

    return SUCCESS_RETURN;
}

/* handle PUBLISH packet received from remote MQTT broker */
static int iotx_mc_handle_recv_PUBLISH(iotx_mc_client_t *c)
{
    int result = 0;
    MQTTString topicName;
    iotx_mqtt_topic_info_t topic_msg;
    int qos = 0;
    uint32_t payload_len = 0;

    if (!c) {
        return FAIL_RETURN;
    }

    memset(&topic_msg, 0x0, sizeof(iotx_mqtt_topic_info_t));
    memset(&topicName, 0x0, sizeof(MQTTString));

    if (1 != MQTTDeserialize_publish((unsigned char *)&topic_msg.dup,
                                     (int *)&qos,
                                     (unsigned char *)&topic_msg.retain,
                                     (unsigned short *)&topic_msg.packet_id,
                                     &topicName,
                                     (unsigned char **)&topic_msg.payload,
                                     (int *)&payload_len,
                                     (unsigned char *)c->buf_read,
                                     c->buf_size_read)) {
        return MQTT_PUBLISH_PACKET_ERROR;
    }
    topic_msg.qos = (unsigned char)qos;
    topic_msg.payload_len = payload_len;

#if WITH_MQTT_JSON_FLOW

    const char     *json_payload = (const char *)topic_msg.payload;

    mqtt_info("Downstream Topic: '%.*s'", topicName.lenstring.len, topicName.lenstring.data);
    mqtt_info("Downstream Payload:");
    iotx_facility_json_print(json_payload, LOG_INFO_LEVEL, '<');

#endif  /* #if WITH_MQTT_JSON_FLOW */

    mqtt_debug("%20s : %08d", "Packet Ident", topic_msg.packet_id);
    mqtt_debug("%20s : %d", "Topic Length", topicName.lenstring.len);
    mqtt_debug("%20s : %.*s",
               "Topic Name",
               topicName.lenstring.len,
               topicName.lenstring.data);
    mqtt_debug("%20s : %d / %d", "Payload Len/Room",
               topic_msg.payload_len,
               c->buf_read + c->buf_size_read - topic_msg.payload);
    mqtt_debug("%20s : %d", "Receive Buflen", c->buf_size_read);

#if defined(INSPECT_MQTT_FLOW)
    mqtt_debug("%20s : %p", "Payload Buffer", topic_msg.payload);
    mqtt_debug("%20s : %p", "Receive Buffer", c->buf_read);
    HEXDUMP_DEBUG(topic_msg.payload, topic_msg.payload_len);
#endif

    topic_msg.ptopic = NULL;
    topic_msg.topic_len = 0;

    mqtt_debug("delivering msg ...");

    /* flowControl for specific topic */
    static uint64_t time_prev = 0;
    uint64_t time_curr = 0;
    char *filterStr = "{\"method\":\"thing.service.property.set\"";
    int filterLen = strlen(filterStr);

    if (0 == memcmp(topic_msg.payload, filterStr, filterLen)) {
        time_curr = HAL_UptimeMs();
        if (time_curr < time_prev) {
            time_curr = time_prev;
        }
        if ((time_curr - time_prev) <= (uint64_t)50) {
            mqtt_info("MQTT over threshould");
            return SUCCESS_RETURN;
        } else {
            time_prev = time_curr;
        }
    }

    iotx_mc_deliver_message(c, &topicName, &topic_msg);

    if (topic_msg.qos == IOTX_MQTT_QOS0) {
        return SUCCESS_RETURN;
    } else if (topic_msg.qos == IOTX_MQTT_QOS1) {
        result = MQTTPuback(c, topic_msg.packet_id, PUBACK);
    } else if (topic_msg.qos == IOTX_MQTT_QOS2) {
        result = MQTTPuback(c, topic_msg.packet_id, PUBREC);
    } else {
        mqtt_err("Invalid QOS, QOSvalue = %d", topic_msg.qos);
        return MQTT_PUBLISH_QOS_ERROR;
    }

    return result;
}

/* handle UNSUBACK packet received from remote MQTT broker */
static int iotx_mc_handle_recv_UNSUBACK(iotx_mc_client_t *c)
{
    unsigned short mypacketid = 0;  /* should be the same as the packetid above */

    if (!c) {
        return FAIL_RETURN;
    }

    if (MQTTDeserialize_unsuback(&mypacketid, (unsigned char *)c->buf_read, c->buf_size_read) != 1) {
        return MQTT_UNSUBSCRIBE_ACK_PACKET_ERROR;
    }

    iotx_mc_topic_handle_t *messageHandler = NULL;
#ifdef INSPECT_MQTT_LIST
    mqtt_debug("receivce UNSUBACK, packetid: %d", mypacketid);
#endif
    HAL_MutexLock(c->lock_list_sub);
    (void)iotx_mc_mask_subInfo_from(c, mypacketid, &messageHandler);
    HAL_MutexUnlock(c->lock_list_sub);

    if (NULL == messageHandler || NULL == messageHandler->topic_filter) {
        return MQTT_SUB_INFO_NOT_FOUND_ERROR;
    }

    /* Remove from message handler array */
    HAL_MutexLock(c->lock_generic);
    iotx_mc_topic_handle_t *h, *h_next;
    for (h = c->first_sub_handle; h != NULL; h = h_next) {
        h_next = h->next;

        if (0 == iotx_mc_check_handle_is_identical_ex(h, messageHandler)) {
            remove_handle_from_list(c, h);
            if (h->topic_filter != NULL) {
                mqtt_free(h->topic_filter);
                h->topic_filter = NULL;
            }
            mqtt_free(h);
            /* NOTE: in case of more than one register(subscribe) with different callback function,
             *       so we must keep continuously searching related message handle */
        }
    }

    if (NULL != c->handle_event.h_fp) {
        iotx_mqtt_event_msg_t msg;
        msg.event_type = IOTX_MQTT_EVENT_UNSUBCRIBE_SUCCESS;
        msg.msg = (void *)(uintptr_t)mypacketid;
        _handle_event(&c->handle_event, c, &msg);
    }
    mqtt_free(messageHandler->topic_filter);
    mqtt_free(messageHandler);

    HAL_MutexUnlock(c->lock_generic);
    return SUCCESS_RETURN;
}


/* wait CONNACK packet from remote MQTT broker */
static int iotx_mc_wait_CONNACK(iotx_mc_client_t *c)
{

#define WAIT_CONNACK_MAX (10)
    unsigned char wait_connack = 0;
    unsigned int packetType = 0;
    int rc = 0;
    iotx_time_t timer;

    if (!c) {
        return FAIL_RETURN;
    }

    iotx_time_init(&timer);
    utils_time_countdown_ms(&timer, c->request_timeout_ms);

    do {
        /* read the socket, see what work is due */
        rc = iotx_mc_read_packet(c, &timer, &packetType);
        if (rc != SUCCESS_RETURN) {
            mqtt_err("readPacket error,result = %d", rc);
            return MQTT_NETWORK_ERROR;
        }

        if (++wait_connack > WAIT_CONNACK_MAX) {
            mqtt_err("wait connack timeout");
            return MQTT_NETWORK_ERROR;
        }

    } while (packetType != CONNACK);

    rc = iotx_mc_handle_recv_CONNACK(c);
    if (SUCCESS_RETURN != rc) {
        mqtt_err("recvConnackProc error,result = %d", rc);
    }

    return rc;
}


/* MQTT cycle to handle packet from remote broker */
static int iotx_mc_cycle(iotx_mc_client_t *c, iotx_time_t *timer)
{
    unsigned int packetType;
    int rc = SUCCESS_RETURN;

    if (!c) {
        return FAIL_RETURN;
    }

    iotx_mc_state_t state = iotx_mc_get_client_state(c);
    if (state != IOTX_MC_STATE_CONNECTED) {
        mqtt_debug("state = %d", state);
        return MQTT_STATE_ERROR;
    }

    if (IOTX_MC_KEEPALIVE_PROBE_MAX < c->keepalive_probes) {
        iotx_mc_set_client_state(c, IOTX_MC_STATE_DISCONNECTED);
        c->keepalive_probes = 0;
        mqtt_debug("keepalive_probes more than %u, disconnected\n", IOTX_MC_KEEPALIVE_PROBE_MAX);
    }

    /* read the socket, see what work is due */
    rc = iotx_mc_read_packet(c, timer, &packetType);
    if (rc != SUCCESS_RETURN) {
        iotx_mc_set_client_state(c, IOTX_MC_STATE_DISCONNECTED);
        mqtt_debug("readPacket error,result = %d", rc);
        return MQTT_NETWORK_ERROR;
    }

    if (MQTT_CPT_RESERVED == packetType) {
        /* mqtt_debug("wait data timeout"); */
        return SUCCESS_RETURN;
    }

    /* clear ping mark when any data received from MQTT broker */
    HAL_MutexLock(c->lock_generic);
    c->ping_mark = 0;
    c->keepalive_probes = 0;
    HAL_MutexUnlock(c->lock_generic);

    switch (packetType) {
        case CONNACK: {
            mqtt_debug("CONNACK");
            break;
        }
        case PUBACK: {
            mqtt_debug("PUBACK");
            rc = iotx_mc_handle_recv_PUBACK(c);
            if (SUCCESS_RETURN != rc) {
                mqtt_err("recvPubackProc error,result = %d", rc);
            }

            break;
        }
        case SUBACK: {
            mqtt_debug("SUBACK");
            rc = iotx_mc_handle_recv_SUBACK(c);
            if (SUCCESS_RETURN != rc) {
                mqtt_err("recvSubAckProc error,result = %d", rc);
            }
            break;
        }
        case PUBLISH: {
            mqtt_debug("PUBLISH");
            /* HEXDUMP_DEBUG(c->buf_read, 32); */

            rc = iotx_mc_handle_recv_PUBLISH(c);
            if (SUCCESS_RETURN != rc) {
                mqtt_err("recvPublishProc error,result = %d", rc);
            }
            break;
        }
        case UNSUBACK: {
            mqtt_debug("UNSUBACK");
            rc = iotx_mc_handle_recv_UNSUBACK(c);
            if (SUCCESS_RETURN != rc) {
                mqtt_err("recvUnsubAckProc error,result = %d", rc);
            }
            break;
        }
        case PINGRESP: {
            rc = SUCCESS_RETURN;
            mqtt_info("receive ping response!");
            break;
        }
        default:
            mqtt_err("INVALID TYPE");
            return FAIL_RETURN;
    }

    return rc;
}


/* check MQTT client is in normal state */
/* 0, in abnormal state; 1, in normal state */
static int iotx_mc_check_state_normal(iotx_mc_client_t *c)
{
    if (!c) {
        return 0;
    }

    if (iotx_mc_get_client_state(c) == IOTX_MC_STATE_CONNECTED) {
        return 1;
    }

    return 0;
}


/* return: 0, identical; NOT 0, different */
static int iotx_mc_check_handle_is_identical(iotx_mc_topic_handle_t *messageHandlers1,
        iotx_mc_topic_handle_t *messageHandler2)
{
    if (!messageHandlers1 || !messageHandler2) {
        return 1;
    }

    if (!(messageHandlers1 ->topic_filter) || !(messageHandler2->topic_filter)) {
        return 1;
    }

#if !(WITH_MQTT_ZIP_TOPIC)
    int topicNameLen = strlen(messageHandlers1->topic_filter);

    if (topicNameLen != strlen(messageHandler2->topic_filter)) {
        return 1;
    }

    if (0 != strncmp(messageHandlers1->topic_filter, messageHandler2->topic_filter, topicNameLen)) {
        return 1;
    }
#else
    int i;

    if (messageHandlers1->topic_type != messageHandler2->topic_type) {
        return 1;
    }

    if (messageHandlers1->topic_type == TOPIC_NAME_TYPE) {
        for (i = 0; i < MQTT_MD5_PATH_DEFAULT_LEN; i++) {
            if (messageHandler2->topic_filter[i] != messageHandlers1->topic_filter[i]) {
                return 1;
            }
        }
    } else {
        int topicNameLen = strlen(messageHandlers1->topic_filter);

        if (topicNameLen != strlen(messageHandler2->topic_filter)) {
            return 1;
        }

        if (0 != strncmp(messageHandlers1->topic_filter, messageHandler2->topic_filter, topicNameLen)) {
            return 1;
        }
    }
#endif
    if (messageHandlers1->handle.h_fp != messageHandler2->handle.h_fp) {
        return 1;
    }

    /* context must be identical also */
    if (messageHandlers1->handle.pcontext != messageHandler2->handle.pcontext) {
        return 1;
    }

    return 0;
}

/* return: 0, identical; NOT 0, different */
static int iotx_mc_check_handle_is_identical_ex(iotx_mc_topic_handle_t *messageHandlers1,
        iotx_mc_topic_handle_t *messageHandler2)
{
    if (!messageHandlers1 || !messageHandler2) {
        return 1;
    }

    if (!(messageHandlers1 ->topic_filter) || !(messageHandler2->topic_filter)) {
        return 1;
    }

#if !(WITH_MQTT_ZIP_TOPIC)
    int topicNameLen = strlen(messageHandlers1->topic_filter);

    if (topicNameLen != strlen(messageHandler2->topic_filter)) {
        return 1;
    }

    if (0 != strncmp(messageHandlers1->topic_filter, messageHandler2->topic_filter, topicNameLen)) {
        return 1;
    }
#else
    int i;

    if (messageHandlers1->topic_type != messageHandler2->topic_type) {
        return 1;
    }

    if (messageHandlers1->topic_type == TOPIC_NAME_TYPE) {
        for (i = 0; i < MQTT_MD5_PATH_DEFAULT_LEN; i++) {
            if (messageHandler2->topic_filter[i] != messageHandlers1->topic_filter[1]) {
                return 1;
            }
        }
    } else {
        int topicNameLen = strlen(messageHandlers1->topic_filter);

        if (topicNameLen != strlen(messageHandler2->topic_filter)) {
            return 1;
        }

        if (0 != strncmp(messageHandlers1->topic_filter, messageHandler2->topic_filter, topicNameLen)) {
            return 1;
        }
    }
#endif

    return 0;
}

/* subscribe */
int iotx_mc_subscribe(iotx_mc_client_t *c,
                      const char *topicFilter,
                      iotx_mqtt_qos_t qos,
                      iotx_mqtt_event_handle_func_fpt topic_handle_func,
                      void *pcontext)
{
    int rc = FAIL_RETURN;
    unsigned int msgId = iotx_mc_get_next_packetid(c);

    if (NULL == c || NULL == topicFilter || !topic_handle_func) {
        return NULL_VALUE_ERROR;
    }


    if (!iotx_mc_check_state_normal(c)) {
        mqtt_err("mqtt client state is error,state = %d", iotx_mc_get_client_state(c));
        return MQTT_STATE_ERROR;
    }

    if (0 != iotx_mc_check_topic(topicFilter, TOPIC_FILTER_TYPE)) {
        mqtt_err("topic format is error,topicFilter = %s", topicFilter);
        return MQTT_TOPIC_FORMAT_ERROR;
    }

    mqtt_debug("PERFORM subscribe to '%s' (msgId=%d)", topicFilter, msgId);
    rc = MQTTSubscribe(c, topicFilter, qos, msgId, topic_handle_func, pcontext);
    if (rc != SUCCESS_RETURN) {
        if (rc == MQTT_NETWORK_ERROR) {
            iotx_mc_set_client_state(c, IOTX_MC_STATE_DISCONNECTED);
        }

        mqtt_err("run MQTTSubscribe error, rc = %d", rc);
        return rc;
    }

    mqtt_info("mqtt subscribe packet sent,topic = %s!", topicFilter);
    return msgId;
}


/* unsubscribe */
int iotx_mc_unsubscribe(iotx_mc_client_t *c, const char *topicFilter)
{
    int rc = FAIL_RETURN;
    unsigned int msgId = iotx_mc_get_next_packetid(c);

    if (NULL == c || NULL == topicFilter) {
        return NULL_VALUE_ERROR;
    }

    if (0 != iotx_mc_check_topic(topicFilter, TOPIC_FILTER_TYPE)) {
        mqtt_err("topic format is error,topicFilter = %s", topicFilter);
        return MQTT_TOPIC_FORMAT_ERROR;
    }

    if (!iotx_mc_check_state_normal(c)) {
        mqtt_err("mqtt client state is error,state = %d", iotx_mc_get_client_state(c));
        return MQTT_STATE_ERROR;
    }

    rc = MQTTUnsubscribe(c, topicFilter, msgId);
    if (rc != SUCCESS_RETURN) {
        if (rc == MQTT_NETWORK_ERROR) { /* send the subscribe packet */
            iotx_mc_set_client_state(c, IOTX_MC_STATE_DISCONNECTED);
        }

        mqtt_err("run MQTTUnsubscribe error!");
        return rc;
    }

    mqtt_info("mqtt unsubscribe packet sent,topic = %s!", topicFilter);
    return (int)msgId;
}

/* publish */
int iotx_mc_publish(iotx_mc_client_t *c, const char *topicName, iotx_mqtt_topic_info_pt topic_msg)
{
    uint16_t msg_id = 0;
    int rc = FAIL_RETURN;

    if (NULL == c || NULL == topicName || NULL == topic_msg) {
        return NULL_VALUE_ERROR;
    }

    if (0 != iotx_mc_check_topic(topicName, TOPIC_NAME_TYPE)) {
        mqtt_err("topic format is error,topicFilter = %s", topicName);
        return MQTT_TOPIC_FORMAT_ERROR;
    }

    if (!iotx_mc_check_state_normal(c)) {
        mqtt_err("mqtt client state is error,state = %d", iotx_mc_get_client_state(c));
        return MQTT_STATE_ERROR;
    }

    if (topic_msg->qos == IOTX_MQTT_QOS1 || topic_msg->qos == IOTX_MQTT_QOS2) {
        msg_id = iotx_mc_get_next_packetid(c);
        topic_msg->packet_id = msg_id;
    }

    if (topic_msg->qos == IOTX_MQTT_QOS2) {
        mqtt_err("MQTTPublish return error,MQTT_QOS2 is now not supported.");
        return MQTT_PUBLISH_QOS_ERROR;
    }

#if defined(INSPECT_MQTT_FLOW)
    HEXDUMP_DEBUG(topic_msg->payload, topic_msg->payload_len);
#endif

    rc = MQTTPublish(c, topicName, topic_msg);
    if (rc != SUCCESS_RETURN) { /* send the subscribe packet */
        if (rc == MQTT_NETWORK_ERROR) {
            iotx_mc_set_client_state(c, IOTX_MC_STATE_DISCONNECTED);
        }
        mqtt_err("MQTTPublish is error, rc = %d", rc);
        return rc;
    }

    return (int)msg_id;
}


/* get state of MQTT client */
static iotx_mc_state_t iotx_mc_get_client_state(iotx_mc_client_t *pClient)
{


    iotx_mc_state_t state;
    HAL_MutexLock(pClient->lock_generic);
    state = pClient->client_state;
    HAL_MutexUnlock(pClient->lock_generic);

    return state;
}


/* set state of MQTT client */
static void iotx_mc_set_client_state(iotx_mc_client_t *pClient, iotx_mc_state_t newState)
{

    HAL_MutexLock(pClient->lock_generic);
    pClient->client_state = newState;
    HAL_MutexUnlock(pClient->lock_generic);
}


/* set MQTT connection parameter */
static int iotx_mc_set_connect_params(iotx_mc_client_t *pClient, MQTTPacket_connectData *pConnectParams)
{

    if (NULL == pClient || NULL == pConnectParams) {
        return NULL_VALUE_ERROR;
    }

    memcpy(pClient->connect_data.struct_id, pConnectParams->struct_id, 4);
    pClient->connect_data.struct_version = pConnectParams->struct_version;
    pClient->connect_data.MQTTVersion = pConnectParams->MQTTVersion;
    pClient->connect_data.clientID = pConnectParams->clientID;
    pClient->connect_data.cleansession = pConnectParams->cleansession;
    pClient->connect_data.willFlag = pConnectParams->willFlag;
    pClient->connect_data.username = pConnectParams->username;
    pClient->connect_data.password = pConnectParams->password;
    memcpy(pClient->connect_data.will.struct_id, pConnectParams->will.struct_id, 4);
    pClient->connect_data.will.struct_version = pConnectParams->will.struct_version;
    pClient->connect_data.will.topicName = pConnectParams->will.topicName;
    pClient->connect_data.will.message = pConnectParams->will.message;
    pClient->connect_data.will.qos = pConnectParams->will.qos;
    pClient->connect_data.will.retained = pConnectParams->will.retained;

    if (pConnectParams->keepAliveInterval < KEEP_ALIVE_INTERVAL_DEFAULT_MIN) {
        mqtt_warning("Input heartbeat interval(%d ms) < Allowed minimum(%d ms)",
                     (pConnectParams->keepAliveInterval * 1000),
                     (KEEP_ALIVE_INTERVAL_DEFAULT_MIN * 1000)
                    );
        mqtt_warning("Reset heartbeat interval => %d Millisecond",
                     (KEEP_ALIVE_INTERVAL_DEFAULT_MIN * 1000)
                    );
        pClient->connect_data.keepAliveInterval = KEEP_ALIVE_INTERVAL_DEFAULT_MIN;
    } else if (pConnectParams->keepAliveInterval > KEEP_ALIVE_INTERVAL_DEFAULT_MAX) {
        mqtt_warning("Input heartbeat interval(%d ms) > Allowed maximum(%d ms)",
                     (pConnectParams->keepAliveInterval * 1000),
                     (KEEP_ALIVE_INTERVAL_DEFAULT_MAX * 1000)
                    );
        mqtt_warning("Reset heartbeat interval => %d Millisecond",
                     (KEEP_ALIVE_INTERVAL_DEFAULT_MAX * 1000)
                    );
        pClient->connect_data.keepAliveInterval = KEEP_ALIVE_INTERVAL_DEFAULT_MAX;
    } else {
        pClient->connect_data.keepAliveInterval = pConnectParams->keepAliveInterval;
    }

    return SUCCESS_RETURN;
}

static int32_t iotx_mc_calc_seed(uint32_t *p_seed)
{
    char                   *device_name;
    iotx_device_info_pt     pdev = iotx_device_info_get();
    uint32_t                seed = 0;

    POINTER_SANITY_CHECK(p_seed, NULL_VALUE_ERROR);
    POINTER_SANITY_CHECK(pdev, NULL_VALUE_ERROR);

    device_name = pdev->device_name;
    STRING_PTR_SANITY_CHECK(device_name, NULL_VALUE_ERROR);

    while ('\0' != *device_name) {
        seed += *device_name;
        device_name++;
    }
    seed += (HAL_UptimeMs() / 1000);
    seed %= UINT32_MAX;
    *p_seed = seed;
    return SUCCESS_RETURN;
}

static int32_t iotx_mc_calc_random_init()
{
    uint32_t                seed;

    if (SUCCESS_RETURN != iotx_mc_calc_seed(&seed)) {
        return FAIL_RETURN;
    }

    HAL_Srandom(seed);
    return SUCCESS_RETURN;
}

/* Initialize MQTT client */
int iotx_mc_init(iotx_mc_client_t *pClient, iotx_mqtt_param_t *pInitParams)
{
    int rc = FAIL_RETURN;
    iotx_mc_state_t mc_state = IOTX_MC_STATE_INVALID;

    ARGUMENT_SANITY_CHECK(pClient, NULL_VALUE_ERROR);
    ARGUMENT_SANITY_CHECK(pInitParams, NULL_VALUE_ERROR);
    ARGUMENT_SANITY_CHECK(pInitParams->write_buf_size, NULL_VALUE_ERROR);
    ARGUMENT_SANITY_CHECK(pInitParams->read_buf_size, NULL_VALUE_ERROR);

    memset(pClient, 0x0, sizeof(iotx_mc_client_t));

    pClient->lock_generic = HAL_MutexCreate();
    if (!pClient->lock_generic) {
        return FAIL_RETURN;
    }

    pClient->lock_list_sub = HAL_MutexCreate();
    if (!pClient->lock_list_sub) {
        goto RETURN;
    }

    pClient->lock_list_pub = HAL_MutexCreate();
    if (!pClient->lock_list_pub) {
        goto RETURN;
    }

    pClient->lock_yield = HAL_MutexCreate();
    if (!pClient->lock_yield) {
        goto RETURN;
    }

    MQTTPacket_connectData connectdata = MQTTPacket_connectData_initializer;

    connectdata.MQTTVersion = IOTX_MC_MQTT_VERSION;
    connectdata.keepAliveInterval = pInitParams->keepalive_interval_ms / 1000;

    connectdata.clientID.cstring = (char *)pInitParams->client_id;
    connectdata.username.cstring = (char *)pInitParams->username;
    connectdata.password.cstring = (char *)pInitParams->password;
    connectdata.cleansession = pInitParams->clean_session;

    pClient->packet_id = 0;
    if (pInitParams->request_timeout_ms < IOTX_MC_REQUEST_TIMEOUT_MIN_MS
        || pInitParams->request_timeout_ms > IOTX_MC_REQUEST_TIMEOUT_MAX_MS) {

        pClient->request_timeout_ms = IOTX_MC_REQUEST_TIMEOUT_DEFAULT_MS;
    } else {
        pClient->request_timeout_ms = pInitParams->request_timeout_ms;
    }


#if !(WITH_MQTT_DYN_TXBUF)
    pClient->buf_send = mqtt_malloc(pInitParams->write_buf_size);
    if (pClient->buf_send == NULL) {
        goto RETURN;
    }
    pClient->buf_size_send = pInitParams->write_buf_size;
#else
    pClient->buf_size_send_max = pInitParams->write_buf_size;
#endif
    pClient->buf_read = mqtt_malloc(pInitParams->read_buf_size);
    if (pClient->buf_read == NULL) {
        goto RETURN;
    }
    pClient->buf_size_read = pInitParams->read_buf_size;

    pClient->keepalive_probes = 0;

    pClient->handle_event.h_fp = pInitParams->handle_event.h_fp;
    pClient->handle_event.pcontext = pInitParams->handle_event.pcontext;

    /* Initialize reconnect parameter */
    pClient->reconnect_param.reconnect_time_interval_ms = IOTX_MC_RECONNECT_INTERVAL_MIN_MS;

    pClient->list_pub_wait_ack = list_new();
    if (pClient->list_pub_wait_ack) {
        pClient->list_pub_wait_ack->free = LITE_free_routine;
    }
    pClient->list_sub_wait_ack = list_new();
    if (pClient->list_sub_wait_ack) {
        pClient->list_sub_wait_ack->free = LITE_free_routine;
    }

    pClient->lock_write_buf = HAL_MutexCreate();


    /* Initialize MQTT connect parameter */
    rc = iotx_mc_set_connect_params(pClient, &connectdata);
    if (SUCCESS_RETURN != rc) {
        mc_state = IOTX_MC_STATE_INVALID;
        goto RETURN;
    }

    iotx_time_init(&pClient->next_ping_time);
    iotx_time_init(&pClient->reconnect_param.reconnect_next_time);

    pClient->ipstack = (utils_network_pt)mqtt_malloc(sizeof(utils_network_t));
    if (NULL == pClient->ipstack) {
        mqtt_err("allocate Network struct failed");
        rc = FAIL_RETURN;
        goto RETURN;
    }
    memset(pClient->ipstack, 0x0, sizeof(utils_network_t));

    rc = iotx_net_init(pClient->ipstack, pInitParams->host, pInitParams->port, pInitParams->pub_key,
                       iotx_device_info_get()->product_key);

    if (SUCCESS_RETURN != rc) {
        mc_state = IOTX_MC_STATE_INVALID;
        goto RETURN;
    }
    if (SUCCESS_RETURN != iotx_mc_calc_random_init()) {
        mqtt_err("iotx_mc_calc_random_init failed");
        rc = FAIL_RETURN;
        goto RETURN;
    }

    mc_state = IOTX_MC_STATE_INITIALIZED;
    rc = SUCCESS_RETURN;
    mqtt_info("MQTT init success!");

RETURN :
    iotx_mc_set_client_state(pClient, mc_state);
    if (rc != SUCCESS_RETURN) {
        if (pClient->list_pub_wait_ack) {
            pClient->list_pub_wait_ack->free(pClient->list_pub_wait_ack);
            pClient->list_pub_wait_ack = NULL;
        }
        if (pClient->list_sub_wait_ack) {
            pClient->list_sub_wait_ack->free(pClient->list_sub_wait_ack);
            pClient->list_sub_wait_ack = NULL;
        }
        if (pClient->buf_send != NULL) {
            mqtt_free(pClient->buf_send);
            pClient->buf_send = NULL;
        }
        if (pClient->buf_read != NULL) {
            mqtt_free(pClient->buf_read);
            pClient->buf_read = NULL;
        }
        if (pClient->ipstack) {
            mqtt_free(pClient->ipstack);
            pClient->ipstack = NULL;
        }
        if (pClient->lock_list_sub) {
            HAL_MutexDestroy(pClient->lock_list_sub);
            pClient->lock_list_sub = NULL;
        }
        if (pClient->lock_list_pub) {
            HAL_MutexDestroy(pClient->lock_list_pub);
            pClient->lock_list_pub = NULL;
        }
        if (pClient->lock_write_buf) {
            HAL_MutexDestroy(pClient->lock_write_buf);
            pClient->lock_write_buf = NULL;
        }
        if (pClient->lock_yield) {
            HAL_MutexDestroy(pClient->lock_yield);
            pClient->lock_yield = NULL;
        }
    }

    return rc;
}

#define CONFIG_SUBINFO_LIFE         (10)

/* remove node of list of wait subscribe ACK, which is in invalid state or timeout */
static int MQTTSubInfoProc(iotx_mc_client_t *pClient)
{
    int rc = SUCCESS_RETURN;

    if (!pClient) {
        return FAIL_RETURN;
    }

    HAL_MutexLock(pClient->lock_list_sub);
    do {
        if (0 == pClient->list_sub_wait_ack->len) {
            break;
        }

        list_iterator_t *iter;
        list_node_t *node = NULL;
        list_node_t *tempNode = NULL;
        uint16_t packet_id = 0;
        enum msgTypes msg_type;

        if (NULL == (iter = list_iterator_new(pClient->list_sub_wait_ack, LIST_TAIL))) {
            mqtt_err("new list failed");
            HAL_MutexUnlock(pClient->lock_list_sub);
            return SUCCESS_RETURN;
        }

        for (;;) {
            node = list_iterator_next(iter);

            if (NULL != tempNode) {
#ifdef INSPECT_MQTT_LIST
                mqtt_debug("remove list_sub_wait_ack before");
#endif
                _dump_wait_list(pClient, "sub");
                list_remove(pClient->list_sub_wait_ack, tempNode);
                tempNode = NULL;
#ifdef INSPECT_MQTT_LIST
                _dump_wait_list(pClient, "sub");
#endif
                mqtt_debug("remove list_sub_wait_ack after");
            }

            if (NULL == node) {
                break; /* end of list */
            }

            iotx_mc_subsribe_info_t *subInfo = (iotx_mc_subsribe_info_t *) node->val;
            if (NULL == subInfo) {
                mqtt_err("node's value is invalid!");
                tempNode = node;
                continue;
            }

            /* remove invalid node */
            if (IOTX_MC_NODE_STATE_INVALID == subInfo->node_state) {
                tempNode = node;
                continue;
            }

            if (iotx_mc_get_client_state(pClient) != IOTX_MC_STATE_CONNECTED) {
                continue;
            }

            /* check the request if timeout or not */
            if (utils_time_spend(&subInfo->sub_start_time) <= (pClient->request_timeout_ms * CONFIG_SUBINFO_LIFE)) {
                /* continue to check the next node */
                continue;
            }

            iotx_mc_topic_handle_t *messageHandler = NULL;
#ifdef INSPECT_MQTT_LIST
            mqtt_debug("MQTTSubInfoProc Timeout, packetid: %d", subInfo->msg_id);
#endif
            (void)iotx_mc_mask_subInfo_from(pClient, packet_id, &messageHandler);

            /* When arrive here, it means timeout to wait ACK */
            packet_id = subInfo->msg_id;
            msg_type = subInfo->type;

            /* Wait MQTT SUBSCRIBE ACK timeout */

            iotx_mqtt_event_msg_t msg;

            if (SUBSCRIBE == msg_type) {
                /* subscribe timeout */
#if (WITH_MQTT_SUB_SHORTCUT)
                HAL_MutexLock(pClient->lock_generic);
                iotx_mc_topic_handle_t *h, *h_next;
                for (h = pClient->first_sub_handle; h != NULL; h = h_next) {
                    h_next = h->next;

                    if (0 == iotx_mc_check_handle_is_identical(h, messageHandler)) {
                        remove_handle_from_list(pClient, h);
                        mqtt_free(h);
                        break;
                    }
                }
                HAL_MutexUnlock(pClient->lock_generic);
#endif
                msg.event_type = IOTX_MQTT_EVENT_SUBCRIBE_TIMEOUT;
                msg.msg = (void *)(uintptr_t)packet_id;
                _iotx_mqtt_event_handle_sub(pClient->handle_event.pcontext, pClient, &msg);
            } else { /* if (UNSUBSCRIBE == msg_type) */
                /* unsubscribe timeout */
                msg.event_type = IOTX_MQTT_EVENT_UNSUBCRIBE_TIMEOUT;
                msg.msg = (void *)(uintptr_t)packet_id;
            }


            if (NULL != pClient->handle_event.h_fp) {
                pClient->handle_event.h_fp(pClient->handle_event.pcontext, pClient, &msg);
            }

            if (messageHandler) {
                mqtt_free(messageHandler->topic_filter);
            }
            mqtt_free(messageHandler);

            tempNode = node;
        }

        list_iterator_destroy(iter);

    } while (0);

    HAL_MutexUnlock(pClient->lock_list_sub);
    return rc;
}


static void iotx_mc_keepalive(iotx_mc_client_t *pClient)
{
    int rc = 0;

    if (!pClient) {
        return;
    }

    /* Periodic sending ping packet to detect whether the network is connected */
    iotx_mc_keepalive_sub(pClient);

    iotx_mc_state_t currentState = iotx_mc_get_client_state(pClient);
    do {
        /* if Exceeds the maximum delay time, then return reconnect timeout */
        if (IOTX_MC_STATE_DISCONNECTED_RECONNECTING == currentState) {
            /* Reconnection is successful, Resume regularly ping packets */
            HAL_MutexLock(pClient->lock_generic);
            pClient->ping_mark = 0;
            HAL_MutexUnlock(pClient->lock_generic);
            rc = iotx_mc_handle_reconnect(pClient);
            if (SUCCESS_RETURN != rc) {
                mqtt_debug("reconnect network fail, rc = %d", rc);
            } else {
                mqtt_info("network is reconnected!");
                iotx_mc_reconnect_callback(pClient);
                pClient->reconnect_param.reconnect_time_interval_ms = IOTX_MC_RECONNECT_INTERVAL_MIN_MS;
            }

            break;
        }

        /* If network suddenly interrupted, stop pinging packet, try to reconnect network immediately */
        if (IOTX_MC_STATE_DISCONNECTED == currentState) {
            mqtt_err("network is disconnected!");
            iotx_mc_disconnect_callback(pClient);

            pClient->reconnect_param.reconnect_time_interval_ms = IOTX_MC_RECONNECT_INTERVAL_MIN_MS;
            utils_time_countdown_ms(&(pClient->reconnect_param.reconnect_next_time),
                                    pClient->reconnect_param.reconnect_time_interval_ms);

            pClient->ipstack->disconnect(pClient->ipstack);
            iotx_mc_set_client_state(pClient, IOTX_MC_STATE_DISCONNECTED_RECONNECTING);
            break;
        }

    } while (0);
}


/* republish */
static int MQTTRePublish(iotx_mc_client_t *c, char *buf, int len)
{
    iotx_time_t timer;
    iotx_time_init(&timer);
    utils_time_countdown_ms(&timer, c->request_timeout_ms);

    HAL_MutexLock(c->lock_write_buf);

    if (iotx_mc_send_packet(c, buf, len, &timer) != SUCCESS_RETURN) {
        HAL_MutexUnlock(c->lock_write_buf);
        return MQTT_NETWORK_ERROR;
    }

    HAL_MutexUnlock(c->lock_write_buf);
    return SUCCESS_RETURN;
}


/* remove node of list of wait publish ACK, which is in invalid state or timeout */
static int MQTTPubInfoProc(iotx_mc_client_t *pClient)
{
    int rc = 0;
    iotx_mc_state_t state = IOTX_MC_STATE_INVALID;

    if (!pClient) {
        return FAIL_RETURN;
    }

    HAL_MutexLock(pClient->lock_list_pub);
    do {
        if (0 == pClient->list_pub_wait_ack->len) {
            break;
        }

        list_iterator_t *iter;
        list_node_t *node = NULL;
        list_node_t *tempNode = NULL;

        if (NULL == (iter = list_iterator_new(pClient->list_pub_wait_ack, LIST_TAIL))) {
            mqtt_err("new list failed");
            break;
        }

        for (;;) {

            node = list_iterator_next(iter);

            if (NULL != tempNode) {
                list_remove(pClient->list_pub_wait_ack, tempNode);
                tempNode = NULL;
            }

            if (NULL == node) {
                break; /* end of list */
            }

            iotx_mc_pub_info_t *repubInfo = (iotx_mc_pub_info_t *) node->val;
            if (NULL == repubInfo) {
                mqtt_err("node's value is invalid!");
                tempNode = node;
                continue;
            }

            /* remove invalid node */
            if (IOTX_MC_NODE_STATE_INVALID == repubInfo->node_state) {
                tempNode = node;
                continue;
            }

            state = iotx_mc_get_client_state(pClient);
            if (state != IOTX_MC_STATE_CONNECTED) {
                continue;
            }

            /* check the request if timeout or not */
            if (utils_time_spend(&repubInfo->pub_start_time) <= (pClient->request_timeout_ms * 2)) {
                continue;
            }

            /* If wait ACK timeout, republish */
            rc = MQTTRePublish(pClient, (char *)repubInfo->buf, repubInfo->len);
            iotx_time_start(&repubInfo->pub_start_time);

            if (MQTT_NETWORK_ERROR == rc) {
                iotx_mc_set_client_state(pClient, IOTX_MC_STATE_DISCONNECTED);
                break;
            }
        }

        list_iterator_destroy(iter);

    } while (0);

    HAL_MutexUnlock(pClient->lock_list_pub);

    return SUCCESS_RETURN;
}


/* connect */
int iotx_mc_connect(iotx_mc_client_t *pClient)
{
    int rc = FAIL_RETURN;

    if (NULL == pClient) {
        return NULL_VALUE_ERROR;
    }

    /* Establish TCP or TLS connection */
    rc = pClient->ipstack->connect(pClient->ipstack);
    if (SUCCESS_RETURN != rc) {
        pClient->ipstack->disconnect(pClient->ipstack);
        mqtt_err("TCP or TLS Connection failed");

        if (ERROR_CERTIFICATE_EXPIRED == rc) {
            mqtt_err("certificate is expired!");
            return ERROR_CERT_VERIFY_FAIL;
        } else {
            return MQTT_NETWORK_CONNECT_ERROR;
        }
    }

    /* remove */
    /*mqtt_debug("start MQTT connection with parameters: clientid=%s, username=%s, password=%s",
              pClient->connect_data.clientID.cstring,
              pClient->connect_data.username.cstring,
              pClient->connect_data.password.cstring);*/

    rc = MQTTConnect(pClient);
    if (rc  != SUCCESS_RETURN) {
        pClient->ipstack->disconnect(pClient->ipstack);
        mqtt_err("send connect packet failed");
        return  rc;
    }

    if (SUCCESS_RETURN != iotx_mc_wait_CONNACK(pClient)) {
        (void)MQTTDisconnect(pClient);
        pClient->ipstack->disconnect(pClient->ipstack);
        mqtt_err("wait connect ACK timeout, or receive a ACK indicating error!");
        return MQTT_CONNECT_ERROR;
    }

    iotx_mc_set_client_state(pClient, IOTX_MC_STATE_CONNECTED);

    utils_time_countdown_ms(&pClient->next_ping_time, pClient->connect_data.keepAliveInterval * 1000);

    mqtt_info("mqtt connect success!");
    return SUCCESS_RETURN;
}


int iotx_mc_attempt_reconnect(iotx_mc_client_t *pClient)
{

    int rc;

    mqtt_info("reconnect params: MQTTVersion=%d, clientID=%s, keepAliveInterval=%d, username=%s",
              pClient->connect_data.MQTTVersion,
              pClient->connect_data.clientID.cstring,
              pClient->connect_data.keepAliveInterval,
              pClient->connect_data.username.cstring);
    pClient->ipstack->disconnect(pClient->ipstack);
    /* Ignoring return code. failures expected if network is disconnected */
    rc = iotx_mc_connect(pClient);

    if (SUCCESS_RETURN != rc) {
        mqtt_err("run iotx_mqtt_connect() error!");
        return rc;
    }

    return SUCCESS_RETURN;
}


/* reconnect */
int iotx_mc_handle_reconnect(iotx_mc_client_t *pClient)
{
    int             rc = FAIL_RETURN;
    uint32_t        interval_ms = 0;

    if (NULL == pClient) {
        return NULL_VALUE_ERROR;
    }
    mqtt_info("Waiting to reconnect...");
    if (!utils_time_is_expired(&(pClient->reconnect_param.reconnect_next_time))) {
        /* Timer has not expired. Not time to attempt reconnect yet. Return attempting reconnect */
        HAL_SleepMs(100);
        return FAIL_RETURN;
    }

    mqtt_info("start to reconnect");
    /* REDO AUTH before each reconnection */
    if (NULL != pClient->mqtt_auth && SUCCESS_RETURN != pClient->mqtt_auth()) {
        mqtt_err("redo authentication error!");
        return -1;
    }

    rc = iotx_mc_attempt_reconnect(pClient);
    if (SUCCESS_RETURN == rc) {
        iotx_mc_set_client_state(pClient, IOTX_MC_STATE_CONNECTED);
        return SUCCESS_RETURN;
    } else {
        /* if reconnect network failed, then increase currentReconnectWaitInterval */
        /* e.g. init currentReconnectWaitInterval=1s, reconnect failed, then 2s..4s..8s */
        if (IOTX_MC_RECONNECT_INTERVAL_MAX_MS > pClient->reconnect_param.reconnect_time_interval_ms) {
            pClient->reconnect_param.reconnect_time_interval_ms *= 2;
        } else {
            pClient->reconnect_param.reconnect_time_interval_ms = IOTX_MC_RECONNECT_INTERVAL_MAX_MS;
        }
    }

    interval_ms = pClient->reconnect_param.reconnect_time_interval_ms;
    interval_ms += HAL_Random(pClient->reconnect_param.reconnect_time_interval_ms);
    if (IOTX_MC_RECONNECT_INTERVAL_MAX_MS < interval_ms) {
        interval_ms = IOTX_MC_RECONNECT_INTERVAL_MAX_MS;
    }
    utils_time_countdown_ms(&(pClient->reconnect_param.reconnect_next_time), interval_ms);

    mqtt_err("mqtt reconnect failed rc = %d", rc);

    return rc;
}

int iotx_mc_disconnect(iotx_mc_client_t *pClient)
{
    int             rc = -1;

    if (NULL == pClient) {
        return NULL_VALUE_ERROR;
    }

    if (iotx_mc_check_state_normal(pClient)) {
        rc = MQTTDisconnect(pClient);
        mqtt_debug("rc = MQTTDisconnect() = %d", rc);
    }

    /* close tcp/ip socket or free tls resources */
    pClient->ipstack->disconnect(pClient->ipstack);

    iotx_mc_set_client_state(pClient, IOTX_MC_STATE_INITIALIZED);

    mqtt_info("mqtt disconnect!");
    return SUCCESS_RETURN;
}

static void iotx_mc_disconnect_callback(iotx_mc_client_t *pClient)
{

    if (NULL != pClient->handle_event.h_fp) {
        iotx_mqtt_event_msg_t msg;
        msg.event_type = IOTX_MQTT_EVENT_DISCONNECT;
        msg.msg = NULL;

        pClient->handle_event.h_fp(pClient->handle_event.pcontext,
                                   pClient,
                                   &msg);
    }
}


/* release MQTT resource */
static int iotx_mc_release(iotx_mc_client_t *pClient)
{
    if (NULL == pClient) {
        return NULL_VALUE_ERROR;
    }

    /* iotx_delete_thread(pClient); */
    HAL_SleepMs(100);

    iotx_mc_disconnect(pClient);
    iotx_mc_set_client_state(pClient, IOTX_MC_STATE_INVALID);
    HAL_SleepMs(100);

    if (pClient->first_sub_handle != NULL) {
        iotx_mc_topic_handle_t *handler = pClient->first_sub_handle;
        iotx_mc_topic_handle_t *next_handler = pClient->first_sub_handle;
        while (handler) {
            next_handler = handler->next;
            if (handler->topic_filter != NULL) {
                mqtt_free(handler->topic_filter);
                handler->topic_filter = NULL;
            }
            mqtt_free(handler);
            handler = next_handler;
        }
    }

    HAL_MutexDestroy(pClient->lock_generic);
    HAL_MutexDestroy(pClient->lock_list_sub);
    HAL_MutexDestroy(pClient->lock_list_pub);
    HAL_MutexDestroy(pClient->lock_write_buf);

    list_destroy(pClient->list_pub_wait_ack);
    list_destroy(pClient->list_sub_wait_ack);

    if (pClient->buf_send != NULL) {
        mqtt_free(pClient->buf_send);
        pClient->buf_send = NULL;
    }
    if (pClient->buf_read != NULL) {
        mqtt_free(pClient->buf_read);
        pClient->buf_read = NULL;
    }
    if (NULL != pClient->ipstack) {
        mqtt_free(pClient->ipstack);
    }

    mqtt_info("mqtt release!");
    return SUCCESS_RETURN;
}


static void iotx_mc_reconnect_callback(iotx_mc_client_t *pClient)
{

    /* handle callback function */
    if (NULL != pClient->handle_event.h_fp) {
        iotx_mqtt_event_msg_t msg;
        msg.event_type = IOTX_MQTT_EVENT_RECONNECT;
        msg.msg = NULL;

        pClient->handle_event.h_fp(pClient->handle_event.pcontext,
                                   pClient,
                                   &msg);
    }
}

static int iotx_mc_keepalive_sub(iotx_mc_client_t *pClient)
{

    int rc = SUCCESS_RETURN;

    if (NULL == pClient) {
        return NULL_VALUE_ERROR;
    }

    /* if in disabled state, without having to send ping packets */
    if (!iotx_mc_check_state_normal(pClient)) {
        return SUCCESS_RETURN;
    }

    /* if there is no ping_timer timeout, then return success */
    if (!utils_time_is_expired(&pClient->next_ping_time)) {
        return SUCCESS_RETURN;
    }

    /* update to next time sending MQTT keep-alive */
    utils_time_countdown_ms(&pClient->next_ping_time, pClient->connect_data.keepAliveInterval * 1000);

    rc = MQTTKeepalive(pClient);
    if (SUCCESS_RETURN != rc) {
        if (rc == MQTT_NETWORK_ERROR) {
            iotx_mc_set_client_state(pClient, IOTX_MC_STATE_DISCONNECTED);
        }
        mqtt_err("ping outstanding is error,result = %d", rc);
        return rc;
    }

    mqtt_info("send MQTT ping...");

    HAL_MutexLock(pClient->lock_generic);
    pClient->ping_mark = 1;
    pClient->keepalive_probes++;
    HAL_MutexUnlock(pClient->lock_generic);

    return SUCCESS_RETURN;
}

// aos will implement this function
#if defined(BUILD_AOS)
extern void aos_get_version_hex(unsigned char version[VERSION_NUM_SIZE]);
#else
void aos_get_version_hex(unsigned char version[VERSION_NUM_SIZE])
{
    const char *p_version = LINKKIT_VERSION;
    int i = 0, j = 0;
    unsigned char res = 0;

    for (j = 0; j < 3; j++) {
        for (res = 0; p_version[i] <= '9' && p_version[i] >= '0'; i++) {
            res = res * 10 + p_version[i] - '0';
        }
        version[j] = res;
        i++;
    }
    version[3] = 0x00;
}
#endif

// aos will implement this function
#if defined(BUILD_AOS)
extern void aos_get_mac_hex(unsigned char mac[MAC_ADDRESS_SIZE]);
#else
void aos_get_mac_hex(unsigned char mac[MAC_ADDRESS_SIZE])
{
    memcpy(mac, "\x01\x02\x03\x04\x05\x06\x07\x08", MAC_ADDRESS_SIZE);
}
#endif

// aos will implement this function
#if defined(BUILD_AOS)
extern void aos_get_chip_code(unsigned char chip_code[CHIP_CODE_SIZE]);
#else
void aos_get_chip_code(unsigned char chip_code[CHIP_CODE_SIZE])
{
    memcpy(chip_code, "\x01\x02\x03\x04", CHIP_CODE_SIZE);
}
#endif

const char *DEVICE_INFO_UPDATE_FMT = "{\"id\":\"2\",\"version\":\"1.0\",\"params\":["
                                     "{\"attrKey\":\"SYS_ALIOS_ACTIVATION\",\"attrValue\":\"%s\",\"domain\":\"SYSTEM\"},"
                                     "{\"attrKey\":\"SYS_LP_SDK_VERSION\",\"attrValue\":\"%s\",\"domain\":\"SYSTEM\"},"
                                     "{\"attrKey\":\"SYS_SDK_LANGUAGE\",\"attrValue\":\"C\",\"domain\":\"SYSTEM\"},"
                                     "{\"attrKey\":\"SYS_SDK_IF_INFO\",\"attrValue\":\"%s\",\"domain\":\"SYSTEM\"}"
                                     "],\"method\":\"thing.deviceinfo.update\"}";

static int iotx_mc_report_devinfo(iotx_mc_client_t *pclient)
{
    int ret = 0;
    int i;
    char mac[MAC_ADDRESS_SIZE] = {0};
    char version[VERSION_NUM_SIZE] = {0};
    char random_num[RANDOM_NUM_SIZE];
    char chip_code[CHIP_CODE_SIZE] = {0};
    char output[AOS_ACTIVE_INFO_LEN] = {0};
    char topic_name[IOTX_URI_MAX_LEN + 1] = {0};
    char network_interfaces[NIF_STRLEN_MAX] = {0};
    char *msg = NULL;
    int  msg_len = 0;
    iotx_mqtt_topic_info_t topic_info;
    iotx_device_info_pt dev = iotx_device_info_get();

    VERSION_DEBUG("devinfo report in MQTT");

    /* Construct aos activation data */
    aos_get_version_hex((unsigned char *)version);
    aos_get_mac_hex((unsigned char *)mac);
    mac[6] = ACTIVE_SINGLE_GW;
#ifdef BUILD_AOS
    mac[7] = ACTIVE_LINKKIT_AOS;
#else
    mac[7] = ACTIVE_LINKKIT_ONLY;
#endif

    HAL_Srandom(HAL_UptimeMs());
    for (i = 0; i < 4; i ++) {
        random_num[i] = (char)HAL_Random(0xFF);
    }
    aos_get_chip_code((unsigned char *)chip_code);
    /*
    input: version 4byte + random 4 byte + mac 4byte + chip_code 4byte
    output: output_buffer store the version info process. length at least OUTPUT_SPACE_SIZE
    return: 0 success, 1 failed
    */
    ret = aos_get_version_info((unsigned char *)version, (unsigned char *)random_num, (unsigned char *)mac,
                               (unsigned char *)chip_code, (unsigned char *)output, AOS_ACTIVE_INFO_LEN);
    if (ret) {
        mqtt_err("aos_get_version_info failed");
        return FAIL_RETURN;
    }
    VERSION_DEBUG("get aos avtive info: %s", output);

    /* devinfo update topic name */
    ret = HAL_Snprintf(topic_name,
                       IOTX_URI_MAX_LEN,
                       "/sys/%s/%s/thing/deviceinfo/update",
                       dev->product_key,
                       dev->device_name);
    if (ret <= 0) {
        mqtt_err("topic generate err");
        return FAIL_RETURN;
    }
    VERSION_DEBUG("devinfo report topic: %s", topic_name);

    ret = HAL_GetNetifInfo(network_interfaces);
    if (ret <= 0 || ret >= NIF_STRLEN_MAX) {
        mqtt_err("the network interface info set failed or not set, writen len is %d", ret);
        const char *default_network_info = "invalid network interface info";
        strncpy(network_interfaces, default_network_info, strlen(default_network_info));
    }

    msg_len = strlen(DEVICE_INFO_UPDATE_FMT) + strlen(LINKKIT_VERSION) + AOS_ACTIVE_INFO_LEN + \
              + strlen(network_interfaces) + 1;
    msg = (char *)mqtt_malloc(msg_len);
    if (msg == NULL) {
        mqtt_err("malloc err");
        return FAIL_RETURN;
    }
    memset(msg, 0, msg_len);

    /* devinfo update message */
    ret = HAL_Snprintf(msg,
                       msg_len,
                       DEVICE_INFO_UPDATE_FMT,
                       output,
                       LINKKIT_VERSION,
                       network_interfaces
                      );
    if (ret <= 0) {
        mqtt_err("topic msg generate err");
        mqtt_free(msg);
        return FAIL_RETURN;
    }
    VERSION_DEBUG("devinfo report data: %s", msg);

    topic_info.qos = IOTX_MQTT_QOS1;
    topic_info.payload = (void *)msg;
    topic_info.payload_len = strlen(msg);
    topic_info.retain = 0;
    topic_info.dup = 0;

    ret = iotx_mc_publish(pclient, topic_name, &topic_info);
    mqtt_free(msg);
    if (ret < 0) {
        mqtt_err("publish failed");
        return FAIL_RETURN;
    }
    VERSION_DEBUG("devinfo report succeed");

    return SUCCESS_RETURN;
}

/* report Firmware version */
static int iotx_mc_report_firmware_version(iotx_mc_client_t *pclient)
{
#if defined(BUILD_AOS)
    return SUCCESS_RETURN;
#else
    int ret;
    char topic_name[IOTX_URI_MAX_LEN + 1] = {0};
    char msg[FIRMWARE_VERSION_MSG_LEN] = {0};
    iotx_mqtt_topic_info_t topic_info;
    iotx_device_info_pt dev = iotx_device_info_get();
    char version[FIRMWARE_VERSION_MAXLEN] = {0};

    ret = HAL_GetFirmwareVesion(version);
    if (ret <= 0) {
        mqtt_err("firmware version does not implement");
        return FAIL_RETURN;
    }

    VERSION_DEBUG("firmware version report start in MQTT");

    /* firmware report topic name generate */
    ret = HAL_Snprintf(topic_name,
                       IOTX_URI_MAX_LEN,
                       "/ota/device/inform/%s/%s",
                       dev->product_key,
                       dev->device_name
                      );
    if (ret <= 0) {
        mqtt_err("firmware report topic generate err");
        return FAIL_RETURN;
    }
    VERSION_DEBUG("firmware report topic: %s", topic_name);

    /* firmware report message json data generate */
    ret = HAL_Snprintf(msg,
                       FIRMWARE_VERSION_MSG_LEN,
                       "{\"id\":\"%d\",\"params\":{\"version\":\"%s\"}}",
                       3,
                       version
                      );
    if (ret <= 0) {
        mqtt_err("firmware report message json data generate err");
        return FAIL_RETURN;
    }
    VERSION_DEBUG("firmware report data: %s", msg);

    topic_info.qos = IOTX_MQTT_QOS1;
    topic_info.payload = (void *)msg;
    topic_info.payload_len = strlen(msg);
    topic_info.retain = 0;
    topic_info.dup = 0;

    // publish message
    ret = iotx_mc_publish(pclient, topic_name, &topic_info);
    if (ret < 0) {
        mqtt_err("publish failed");
        return FAIL_RETURN;
    }

    VERSION_DEBUG("firmware version report finished, iotx_mc_publish() = %d", ret);
    return SUCCESS_RETURN;
#endif
}

/* report ModuleID */
static int iotx_mc_report_mid(iotx_mc_client_t *pclient)
{
    int                         ret;
    char                        topic_name[IOTX_URI_MAX_LEN + 1];
    iotx_mqtt_topic_info_t      topic_info;
    char                        requestId[MIDREPORT_REQID_LEN + 1] = {0};
    iotx_device_info_pt         dev  = iotx_device_info_get();
    char                        pid[PID_STRLEN_MAX + 1] = {0};
    char                        mid[MID_STRLEN_MAX + 1] = {0};

    memset(pid, 0, sizeof(pid));
    memset(mid, 0, sizeof(mid));

    if (0 == HAL_GetPartnerID(pid)) {
        mqtt_debug("PartnerID is Null");
        return SUCCESS_RETURN;
    }
    if (0 == HAL_GetModuleID(mid)) {
        mqtt_debug("ModuleID is Null");
        return SUCCESS_RETURN;
    }

    mqtt_debug("MID Report: started in MQTT");

    iotx_midreport_reqid(requestId,
                         dev->product_key,
                         dev->device_name);
    /* 1,generate json data */
    char *msg = mqtt_malloc(MIDREPORT_PAYLOAD_LEN);
    if (NULL == msg) {
        mqtt_err("allocate mem failed");
        return FAIL_RETURN;
    }

    iotx_midreport_payload(msg,
                           requestId,
                           mid,
                           pid);

    mqtt_debug("MID Report: json data = '%s'", msg);

    memset(&topic_info, 0, sizeof(iotx_mqtt_topic_info_t));

    topic_info.qos = IOTX_MQTT_QOS0;
    topic_info.payload = (void *)msg;
    topic_info.payload_len = strlen(msg);
    topic_info.retain = 0;
    topic_info.dup = 0;

    /* 2,generate topic name */
    ret = iotx_midreport_topic(topic_name,
                               "",
                               dev->product_key,
                               dev->device_name);

    mqtt_debug("MID Report: topic name = '%s'", topic_name);

    if (ret < 0) {
        mqtt_err("generate topic name of info failed");
        mqtt_free(msg);
        return FAIL_RETURN;
    }

    ret = IOT_MQTT_Publish(pclient, topic_name, &topic_info);
    if (ret < 0) {
        mqtt_err("publish failed");
        mqtt_free(msg);
        return FAIL_RETURN;
    }

    mqtt_free(msg);

    mqtt_debug("MID Report: finished, IOT_MQTT_Publish() = %d", ret);
    return SUCCESS_RETURN;
}


typedef struct {
    uintptr_t packet_id;
    uint8_t ack_type;
    iotx_mqtt_event_handle_func_fpt sub_state_cb;
    struct list_head linked_list;
} mqtt_sub_node_t;

static struct list_head g_mqtt_sub_list = LIST_HEAD_INIT(g_mqtt_sub_list);

static void _iotx_mqtt_event_handle_sub(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg)
{
    if (pclient == NULL || msg == NULL) {
        return;
    }

    iotx_mc_client_t *client = (iotx_mc_client_t *)pclient;
    uintptr_t packet_id = (uintptr_t) msg->msg;

    mqtt_debug("packet_id = %d, event_type=%d", packet_id, msg->event_type);
    mqtt_sub_node_t *node = NULL;
    mqtt_sub_node_t *next = NULL;

    HAL_MutexLock(client->lock_generic);
    list_for_each_entry_safe(node, next, &g_mqtt_sub_list, linked_list, mqtt_sub_node_t) {
        if (node->packet_id == packet_id) {
            node->ack_type = msg->event_type;
        }
    }
    HAL_MutexUnlock(client->lock_generic);
}


/************************  Public Interface ************************/
void *IOT_MQTT_Construct(iotx_mqtt_param_t *pInitParams)
{
    int                 err;
    iotx_mc_client_t   *pclient;
    iotx_mqtt_param_t *mqtt_params = NULL;

    if (pInitParams != NULL) {
        if (g_mqtt_client != NULL) {
            IOT_MQTT_Destroy(&g_mqtt_client);
        }

    } else {
        if (g_mqtt_client != NULL) {
            return NULL;
        }

        mqtt_params = (iotx_mqtt_param_t *)mqtt_malloc(sizeof(iotx_mqtt_param_t));
        if (mqtt_params == NULL) {
            return NULL;
        }

        iotx_conn_info_pt pconn_info;

        char product_key[PRODUCT_KEY_LEN + 1];
        char device_name[DEVICE_NAME_LEN + 1];
        char device_secret[DEVICE_SECRET_LEN + 1];
        HAL_GetProductKey(product_key);
        HAL_GetDeviceName(device_name);
        HAL_GetDeviceSecret(device_secret);

        int ret = IOT_SetupConnInfo(product_key, device_name, device_secret, (void **)&pconn_info);
        if (ret != SUCCESS_RETURN) {
            mqtt_free(mqtt_params);
            return NULL;
        }
        /* Initialize MQTT parameter */
        memset(mqtt_params, 0x0, sizeof(iotx_mqtt_param_t));

        mqtt_params->port      = pconn_info->port;
        mqtt_params->host      = pconn_info->host_name;
        mqtt_params->client_id = pconn_info->client_id;
        mqtt_params->username  = pconn_info->username;
        mqtt_params->password  = pconn_info->password;
        mqtt_params->pub_key   = pconn_info->pub_key;

        mqtt_params->request_timeout_ms    = 2000;
        mqtt_params->clean_session         = 0;
        mqtt_params->keepalive_interval_ms = 60000;
        mqtt_params->read_buf_size         = MQTT_DEFAULT_MSG_LEN;
        mqtt_params->write_buf_size        = MQTT_DEFAULT_MSG_LEN;
        mqtt_params->handle_event.h_fp     = NULL;
        mqtt_params->handle_event.pcontext = NULL;
        pInitParams = mqtt_params;
    }

    if (pInitParams->host == NULL || pInitParams->client_id == NULL ||
        pInitParams->username == NULL || pInitParams->password == NULL) {
        mqtt_err("init params is not complete");
        if (mqtt_params != NULL) {
            mqtt_free(mqtt_params);
        }
        return NULL;
    }

    pclient = (iotx_mc_client_t *)mqtt_malloc(sizeof(iotx_mc_client_t));
    if (NULL == pclient) {
        mqtt_err("not enough memory.");
        if (mqtt_params != NULL) {
            mqtt_free(mqtt_params);
        }
        return NULL;
    }

    err = iotx_mc_init(pclient, pInitParams);
    if (SUCCESS_RETURN != err) {
        mqtt_free(pclient);
        if (mqtt_params != NULL) {
            mqtt_free(mqtt_params);
        }
        return NULL;
    }

    if (mqtt_params != NULL) {
        mqtt_free(mqtt_params);
    }

    err = iotx_mc_connect(pclient);
    if (SUCCESS_RETURN != err) {
        iotx_mc_release(pclient);
        mqtt_free(pclient);
        return NULL;
    }

    pclient->mqtt_auth = iotx_guider_authenticate;

    /* report module id */
    err = iotx_mc_report_mid(pclient);
    if (SUCCESS_RETURN != err) {
#ifdef DEBUG_REPORT_MID_DEVINFO_FIRMWARE
        mqtt_err("failed to report mid");
#endif
    }

    /* report device info */
    err = iotx_mc_report_devinfo(pclient);
    if (SUCCESS_RETURN != err) {
#ifdef DEBUG_REPORT_MID_DEVINFO_FIRMWARE
        mqtt_err("failed to report devinfo");
#endif
    }

    /* report firmware version */
    err = iotx_mc_report_firmware_version(pclient);
    if (SUCCESS_RETURN != err) {
#ifdef DEBUG_REPORT_MID_DEVINFO_FIRMWARE
        mqtt_err("failed to report firmware version");
#endif
    }

    g_mqtt_client = pclient;
    return pclient;
}

int IOT_MQTT_Destroy(void **phandler)
{
    void *client;
    if (phandler != NULL) {
        client = *phandler;
        *phandler = NULL;
    } else {
        client = g_mqtt_client;
    }

    POINTER_SANITY_CHECK(client, NULL_VALUE_ERROR);

    iotx_mc_release((iotx_mc_client_t *)client);
    mqtt_free(client);
    g_mqtt_client = NULL;

    return SUCCESS_RETURN;
}


int IOT_MQTT_Yield(void *handle, int timeout_ms)
{
    int                 rc = SUCCESS_RETURN;
    iotx_time_t         time;

    iotx_mc_client_t *pClient = (iotx_mc_client_t *)(handle ? handle : g_mqtt_client);

    POINTER_SANITY_CHECK(pClient, NULL_VALUE_ERROR);
    if (timeout_ms < 0) {
        mqtt_err("Invalid argument, timeout_ms = %d", timeout_ms);
        return -1;
    }
    if (timeout_ms == 0) {
        timeout_ms = 10;
    }

    iotx_time_init(&time);
    utils_time_countdown_ms(&time, timeout_ms);


    do {
        if (SUCCESS_RETURN != rc) {
            mqtt_err("error occur rc=%d", rc);
        }

        HAL_MutexLock(pClient->lock_yield);
        /* Keep MQTT alive or reconnect if connection abort */
        iotx_mc_keepalive(pClient);

        /* acquire package in cycle, such as PINGRESP or PUBLISH */
        rc = iotx_mc_cycle(pClient, &time);
        if (SUCCESS_RETURN == rc) {
            /* check list of wait publish ACK to remove node that is ACKED or timeout */
            MQTTPubInfoProc(pClient);

            /* check list of wait subscribe(or unsubscribe) ACK to remove node that is ACKED or timeout */
            MQTTSubInfoProc(pClient);
        }
        HAL_MutexUnlock(pClient->lock_yield);
        /*  */
        unsigned int left_t = iotx_time_left(&time);
        if (left_t < 10) {
            HAL_SleepMs(left_t);
        } else {
            HAL_SleepMs(10);
        }
    } while (!utils_time_is_expired(&time));

    return 0;
}

/* check whether MQTT connection is established or not */
int IOT_MQTT_CheckStateNormal(void *handle)
{
    POINTER_SANITY_CHECK(handle, NULL_VALUE_ERROR);
    return iotx_mc_check_state_normal((iotx_mc_client_t *)handle);
}

int IOT_MQTT_Subscribe(void *handle,
                       const char *topic_filter,
                       iotx_mqtt_qos_t qos,
                       iotx_mqtt_event_handle_func_fpt topic_handle_func,
                       void *pcontext)
{
    iotx_mc_client_t *client = (iotx_mc_client_t *)(handle ? handle : g_mqtt_client);

    POINTER_SANITY_CHECK(client, NULL_VALUE_ERROR);
    POINTER_SANITY_CHECK(topic_handle_func, NULL_VALUE_ERROR);
    STRING_PTR_SANITY_CHECK(topic_filter, NULL_VALUE_ERROR);

    if (qos > IOTX_MQTT_QOS2) {
        mqtt_warning("Invalid qos(%d) out of [%d, %d], using %d",
                     qos,
                     IOTX_MQTT_QOS0, IOTX_MQTT_QOS2, IOTX_MQTT_QOS0);
        qos = IOTX_MQTT_QOS0;
    }
#ifdef INSPECT_MQTT_LIST
    mqtt_debug("iotx_mc_subscribe before");
#endif
    _dump_wait_list(client, "sub");
    return iotx_mc_subscribe(client, topic_filter, qos, topic_handle_func, pcontext);
}

#define SUBSCRIBE_SYNC_TIMEOUT_MAX 10000
int IOT_MQTT_Subscribe_Sync(void *handle,
                            const char *topic_filter,
                            iotx_mqtt_qos_t qos,
                            iotx_mqtt_event_handle_func_fpt topic_handle_func,
                            void *pcontext,
                            int timeout_ms)
{
    int             subed;
    int             ret;
    iotx_time_t     timer;

    iotx_mc_client_t *client = (iotx_mc_client_t *)(handle ? handle : g_mqtt_client);

    POINTER_SANITY_CHECK(client, NULL_VALUE_ERROR);
    STRING_PTR_SANITY_CHECK(topic_filter, NULL_VALUE_ERROR);

    if (timeout_ms > SUBSCRIBE_SYNC_TIMEOUT_MAX) {
        timeout_ms = SUBSCRIBE_SYNC_TIMEOUT_MAX;
    }
    if (qos > IOTX_MQTT_QOS2) {
        mqtt_warning("Invalid qos(%d) out of [%d, %d], using %d",
                     qos,
                     IOTX_MQTT_QOS0, IOTX_MQTT_QOS2, IOTX_MQTT_QOS0);
        qos = IOTX_MQTT_QOS0;
    }
#ifdef INSPECT_MQTT_LIST
    mqtt_debug("iotx_mc_subscribe before");
#endif
    _dump_wait_list(client, "sub");

    iotx_time_init(&timer);
    utils_time_countdown_ms(&timer, timeout_ms);

    ret = -1;
    subed = 0;
    int cnt = 0;
    do {
        if (ret < 0) {
            ret = iotx_mc_subscribe(client, topic_filter, qos, topic_handle_func, pcontext);
            if (_is_in_yield_cb() != 0) {
                return ret;
            }
        }

        if (!subed && ret >= 0) {
            mqtt_sub_node_t *node = (mqtt_sub_node_t *)mqtt_malloc(sizeof(mqtt_sub_node_t));
            if (node != NULL) {
                mqtt_debug("packet_id = %d", ret);
                node->packet_id = ret;
                node->ack_type = IOTX_MQTT_EVENT_UNDEF;
                HAL_MutexLock(client->lock_generic);
                list_add_tail(&node->linked_list, &g_mqtt_sub_list);
                HAL_MutexUnlock(client->lock_generic);
                subed = 1;
            }

        }
        IOT_MQTT_Yield(client, 100);

        mqtt_sub_node_t *node = NULL;
        mqtt_sub_node_t *next = NULL;

        HAL_MutexLock(client->lock_generic);
        list_for_each_entry_safe(node, next, &g_mqtt_sub_list, linked_list, mqtt_sub_node_t) {
            if (node->packet_id == ret) {
                mqtt_debug("node->ack_type=%d cnt=%d", node->ack_type, cnt++);
                if (node->ack_type == IOTX_MQTT_EVENT_SUBCRIBE_SUCCESS) {
                    list_del(&node->linked_list);
                    mqtt_free(node);
                    mqtt_debug("success!!");
                    HAL_MutexUnlock(client->lock_generic);
                    return ret;
                } else if (node->ack_type == IOTX_MQTT_EVENT_SUBCRIBE_NACK) {
                    list_del(&node->linked_list);
                    mqtt_free(node);
                    ret = -1; //resub
                    subed = 0;
                } else if (node->ack_type == IOTX_MQTT_EVENT_SUBCRIBE_TIMEOUT) {
                    list_del(&node->linked_list);
                    mqtt_free(node);
                    ret = -1; //resub
                    subed = 0;
                }
            }
            break;
        }
        HAL_MutexUnlock(client->lock_generic);
    } while (!utils_time_is_expired(&timer));
    mqtt_debug("time out!!");
    mqtt_sub_node_t *node = NULL;
    mqtt_sub_node_t *next = NULL;

    HAL_MutexLock(client->lock_generic);
    list_for_each_entry_safe(node, next, &g_mqtt_sub_list, linked_list, mqtt_sub_node_t) {
        if (node->packet_id == ret) {
            list_del(&node->linked_list);
            mqtt_free(node);
            break;
        }
    }
    HAL_MutexUnlock(client->lock_generic);

    return -1;
}


int IOT_MQTT_Unsubscribe(void *handle, const char *topic_filter)
{
    iotx_mc_client_t *client = (iotx_mc_client_t *)(handle ? handle : g_mqtt_client);

    POINTER_SANITY_CHECK(client, NULL_VALUE_ERROR);
    STRING_PTR_SANITY_CHECK(topic_filter, NULL_VALUE_ERROR);

    return iotx_mc_unsubscribe(client, topic_filter);
}

int IOT_MQTT_Publish(void *handle, const char *topic_name, iotx_mqtt_topic_info_pt topic_msg)
{
    iotx_mc_client_t *client = (iotx_mc_client_t *)(handle ? handle : g_mqtt_client);

    POINTER_SANITY_CHECK(client, NULL_VALUE_ERROR);
    STRING_PTR_SANITY_CHECK(topic_name, NULL_VALUE_ERROR);

    return iotx_mc_publish(client, topic_name, topic_msg);
}

int IOT_MQTT_Publish_Simple(void *handle, const char *topic_name, int qos, void *data, int len)
{
    iotx_mqtt_topic_info_t mqtt_msg;
    iotx_mc_client_t *client = (iotx_mc_client_t *)(handle ? handle : g_mqtt_client);
    POINTER_SANITY_CHECK(client, NULL_VALUE_ERROR);

    memset(&mqtt_msg, 0x0, sizeof(iotx_mqtt_topic_info_t));

    mqtt_msg.qos         = qos;
    mqtt_msg.retain      = 0;
    mqtt_msg.dup         = 0;
    mqtt_msg.payload     = (void *)data;
    mqtt_msg.payload_len = len;

    if (IOT_MQTT_Publish(client, topic_name, &mqtt_msg) < 0) {
        mqtt_err("IOT_MQTT_Publish failed\n");
        return -1;
    }

    return 0;
}


/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <stddef.h>
#include "iot_import.h"
#include "iot_export.h"
#include "guider.h"
#include "lite-log.h"
#include "lite-utils.h"
#include "utils_net.h"
#include "utils_hmac.h"
#include "utils_list.h"
#include "utils_timer.h"
//#include "security.h"
#include "sdk-impl_internal.h"

#include "MQTTPacket/MQTTPacket.h"
#include "mqtt_client.h"

#include <aos/aos.h>

#ifdef MQTT_ID2_CRYPTO
/* Macros for accessing the most offset byte of a number */
#define MQTT_MS(A, OFFSET) ((A << (sizeof(A) - (OFFSET))*8) >> (sizeof(A) - 1) * 8)

void writeUint16(unsigned char **pptr, unsigned short anUint)
{
    if (!pptr) {
        return;
    }

    **pptr = MQTT_MS(anUint, 2);
    (*pptr)++;
    **pptr = MQTT_MS(anUint, 1);
    (*pptr)++;
}

/**
 * Writes an integer as 4 bytes to an output buffer.
 * @param pptr pointer to the output buffer - incremented by the number of bytes used & returned
 * @param anInt the integer to write
 */
void writeUint32(unsigned char **pptr, unsigned int anUint)
{
    if (pptr) {
        return;
    }

    **pptr = MQTT_MS(anUint, 4);
    (*pptr)++;
    **pptr = MQTT_MS(anUint, 3);
    (*pptr)++;
    **pptr = MQTT_MS(anUint, 2);
    (*pptr)++;
    **pptr = MQTT_MS(anUint, 1);
    (*pptr)++;
}

#endif  /* #ifdef MQTT_ID2_CRYPTO */

/* MQTT client version number */
#define IOTX_MC_MQTT_VERSION                    (4)

/* maximum length of topic name in byte */
#define IOTX_MC_TOPIC_NAME_MAX_LEN              (64)

/* maximum MQTT packet-id */
#define IOTX_MC_PACKET_ID_MAX                   (65535)

/* maximum number of simultaneously invoke subscribe request */
#define IOTX_MC_SUB_REQUEST_NUM_MAX             (10)

/* Minimum interval of MQTT reconnect in millisecond */
#define IOTX_MC_RECONNECT_INTERVAL_MIN_MS       (1000)

/* Maximum interval of MQTT reconnect in millisecond */
#define IOTX_MC_RECONNECT_INTERVAL_MAX_MS       (60000)

/* Minimum timeout interval of MQTT request in millisecond */
#define IOTX_MC_REQUEST_TIMEOUT_MIN_MS          (500)

/* Maximum timeout interval of MQTT request in millisecond */
#define IOTX_MC_REQUEST_TIMEOUT_MAX_MS          (5000)

/* Default timeout interval of MQTT request in millisecond */
#define IOTX_MC_REQUEST_TIMEOUT_DEFAULT_MS      (2000)


typedef enum {
    IOTX_MC_CONNECTION_ACCEPTED = 0,
    IOTX_MC_CONNECTION_REFUSED_UNACCEPTABLE_PROTOCOL_VERSION = 1,
    IOTX_MC_CONNECTION_REFUSED_IDENTIFIER_REJECTED = 2,
    IOTX_MC_CONNECTION_REFUSED_SERVER_UNAVAILABLE = 3,
    IOTX_MC_CONNECTION_REFUSED_BAD_USERDATA = 4,
    IOTX_MC_CONNECTION_REFUSED_NOT_AUTHORIZED = 5
} iotx_mc_connect_ack_code_t;


/* State of MQTT client */
typedef enum {
    IOTX_MC_STATE_INVALID = 0,                    // MQTT in invalid state
    IOTX_MC_STATE_INITIALIZED = 1,                // MQTT in initializing state
    IOTX_MC_STATE_CONNECTED = 2,                  // MQTT in connected state
    IOTX_MC_STATE_DISCONNECTED = 3,               // MQTT in disconnected state
    IOTX_MC_STATE_DISCONNECTED_RECONNECTING = 4,  // MQTT in reconnecting state
} iotx_mc_state_t;


typedef enum MQTT_NODE_STATE {
    IOTX_MC_NODE_STATE_NORMANL = 0,
    IOTX_MC_NODE_STATE_INVALID,
} iotx_mc_node_t;


/* Handle structure of subscribed topic */
typedef struct {
    const char *topic_filter;
    iotx_mqtt_event_handle_t handle;
} iotx_mc_topic_handle_t;


/* Information structure of subscribed topic */
typedef struct SUBSCRIBE_INFO {
    enum msgTypes           type;           // type, (sub or unsub)
    uint16_t                msg_id;         // packet id of subscribe(unsubcribe)
    iotx_time_t             sub_start_time; // start time of subscribe request
    iotx_mc_node_t          node_state;     // state of this node
    iotx_mc_topic_handle_t  handler;        // handle of topic subscribed(unsubcribed)
    uint16_t                len;            // length of subscribe message
    unsigned char          *buf;            // subscribe message
} iotx_mc_subsribe_info_t, *iotx_mc_subsribe_info_pt;


/* Information structure of published topic */
typedef struct REPUBLISH_INFO {
    iotx_time_t             pub_start_time;     // start time of publish request
    iotx_mc_node_t          node_state;         // state of this node
    uint16_t                msg_id;             // packet id of publish
    uint32_t                len;                // length of publish message
    unsigned char          *buf;                // publish message
} iotx_mc_pub_info_t, *iotx_mc_pub_info_pt;


/* Reconnected parameter of MQTT client */
typedef struct {
    iotx_time_t         reconnect_next_time;         // the next time point of reconnect
    uint32_t            reconnect_time_interval_ms;  // time interval of this reconnect
} iotx_mc_reconnect_param_t;


/* structure of MQTT client */
typedef struct Client {
    void                           *lock_generic;                            // generic lock
    uint32_t                        packet_id;                               // packet id
    uint32_t                        request_timeout_ms;                      // request timeout in millisecond
    uint32_t                        buf_size_send;                           // send buffer size in byte
    uint32_t                        buf_size_read;                           // read buffer size in byte
    char                           *buf_send;                                // pointer of send buffer
    char                           *buf_read;                                // pointer of read buffer
    iotx_mc_topic_handle_t          sub_handle[IOTX_MC_SUB_NUM_MAX];         // array of subscribe handle
    utils_network_pt                ipstack;                                 // network parameter
    iotx_time_t                     next_ping_time;                          // next ping time
    int                             ping_mark;                               // flag of ping
    iotx_mc_state_t                 client_state;                            // state of MQTT client
    iotx_mc_reconnect_param_t       reconnect_param;                         // reconnect parameter
    MQTTPacket_connectData          connect_data;                            // connection parameter
    list_t                         *list_pub_wait_ack;                       // list of wait publish ack
    list_t                         *list_sub_wait_ack;                       // list of subscribe or unsubscribe ack
    void                           *lock_list_pub;                           // lock of list of wait publish ack
    void                           *lock_list_sub;                           // lock of list of subscribe or unsubscribe ack
    void                           *lock_write_buf;                          // lock of write
    iotx_mqtt_event_handle_t        handle_event;                            // event handle
} iotx_mc_client_t, *iotx_mc_client_pt;

static int iotx_mc_send_packet(iotx_mc_client_t *c, char *buf, int length, iotx_time_t *timer);
static int iotx_mc_get_client_state(iotx_mc_client_t *pClient);
static void iotx_mc_set_client_state(iotx_mc_client_t *pClient, iotx_mc_state_t newState);
static int iotx_mc_keepalive_sub(iotx_mc_client_t *pClient);
static void iotx_mc_disconnect_callback(iotx_mc_client_t *pClient) ;
static int iotx_mc_check_state_normal(iotx_mc_client_t *c);
static int iotx_mc_handle_reconnect(iotx_mc_client_t *pClient);
static void iotx_mc_reconnect_callback(iotx_mc_client_t *pClient);
static int iotx_mc_push_pubInfo_to(iotx_mc_client_t *c, int len, unsigned short msgId, list_node_t **node);
static int iotx_mc_push_subInfo_to(iotx_mc_client_t *c, int len, unsigned short msgId, enum msgTypes type,
                                   iotx_mc_topic_handle_t *handler,
                                   list_node_t **node);
static int iotx_mc_check_handle_is_identical(iotx_mc_topic_handle_t *messageHandlers1,
                                             iotx_mc_topic_handle_t *messageHandler2);

typedef enum {
    TOPIC_NAME_TYPE = 0,
    TOPIC_FILTER_TYPE
} iotx_mc_topic_type_t;


// check rule whether is valid or not
static int iotx_mc_check_rule(char *iterm, iotx_mc_topic_type_t type)
{
    if (NULL == iterm) {
        log_err("iterm is NULL");
        return FAIL_RETURN;
    }

    int i = 0;
    int len = strlen(iterm);
    for (i = 0; i < len; i++) {
        if (TOPIC_FILTER_TYPE == type) {
            if ('+' == iterm[i] || '#' == iterm[i]) {
                if (1 != len) {
                    log_err("the character # and + is error");
                    return FAIL_RETURN;
                }
            }
        } else {
            if ('+' == iterm[i] || '#' == iterm[i]) {
                log_err("has character # and + is error");
                return FAIL_RETURN;
            }
        }

        if (iterm[i] < 32 || iterm[i] >= 127) {
            return FAIL_RETURN;
        }
    }
    return SUCCESS_RETURN;
}


// Check topic name
// 0, topic name is valid; NOT 0, topic name is invalid
static int iotx_mc_check_topic(const char *topicName, iotx_mc_topic_type_t type)
{
    if (NULL == topicName) {
        return FAIL_RETURN;
    }

    if (strlen(topicName) > IOTX_MC_TOPIC_NAME_MAX_LEN) {
        log_err("len of topicName exceeds 64");
        return FAIL_RETURN;
    }

    int mask = 0;
    char topicString[IOTX_MC_TOPIC_NAME_MAX_LEN];
    memset(topicString, 0x0, IOTX_MC_TOPIC_NAME_MAX_LEN);
    strncpy(topicString, topicName, IOTX_MC_TOPIC_NAME_MAX_LEN);

    char *delim = "/";
    char *iterm = NULL;
    iterm = strtok(topicString, delim);

    if (SUCCESS_RETURN != iotx_mc_check_rule(iterm, type)) {
        log_err("run iotx_check_rule error");
        return FAIL_RETURN;
    }

    for (;;) {
        iterm = strtok(NULL, delim);

        if (iterm == NULL) {
            break;
        }

        // The character '#' is not in the last
        if (1 == mask) {
            log_err("the character # is error");
            return FAIL_RETURN;
        }

        if (SUCCESS_RETURN != iotx_mc_check_rule(iterm, type)) {
            log_err("run iotx_check_rule error");
            return FAIL_RETURN;
        }

        if (iterm[0] == '#') {
            mask = 1;
        }
    }

    return SUCCESS_RETURN;
}


// Send keepalive packet
static int MQTTKeepalive(iotx_mc_client_t *pClient)
{
    /* there is no ping outstanding - send ping packet */
    iotx_time_t timer;

    if (!pClient) {
        return FAIL_RETURN;
    }

    iotx_time_init(&timer);
    utils_time_countdown_ms(&timer, 1000);
    int len = 0;
    int rc = 0;

    HAL_MutexLock(pClient->lock_write_buf);
    len = MQTTSerialize_pingreq((unsigned char *)pClient->buf_send, pClient->buf_size_send);
    if (len <= 0) {
        HAL_MutexUnlock(pClient->lock_write_buf);
        log_err("Serialize ping request is error");
        return MQTT_PING_PACKET_ERROR;
    }

    rc = iotx_mc_send_packet(pClient, pClient->buf_send, len, &timer);
    if (SUCCESS_RETURN != rc) {
        HAL_MutexUnlock(pClient->lock_write_buf);
        /*ping outstanding, then close socket unsubscribe topic and handle callback function*/
        log_err("ping outstanding is error,result = %d", rc);
        return MQTT_NETWORK_ERROR;
    }
    HAL_MutexUnlock(pClient->lock_write_buf);

    return SUCCESS_RETURN;
}


// MQTT send connect packet
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
    if ((len = MQTTSerialize_connect((unsigned char *)pClient->buf_send, pClient->buf_size_send, pConnectParams)) <= 0) {
        HAL_MutexUnlock(pClient->lock_write_buf);
        log_err("Serialize connect packet failed,len = %d", len);
        return MQTT_CONNECT_PACKET_ERROR;
    }

    /* send the connect packet*/
    iotx_time_init(&connectTimer);
    utils_time_countdown_ms(&connectTimer, pClient->request_timeout_ms);
    if ((iotx_mc_send_packet(pClient, pClient->buf_send, len, &connectTimer)) != SUCCESS_RETURN) {
        HAL_MutexUnlock(pClient->lock_write_buf);
        log_err("send connect packet failed");
        return MQTT_NETWORK_ERROR;
    }
    HAL_MutexUnlock(pClient->lock_write_buf);

    return SUCCESS_RETURN;
}


// MQTT send publish packet
int MQTTPublish(iotx_mc_client_t *c, const char *topicName, iotx_mqtt_topic_info_pt topic_msg)

{
    iotx_time_t timer;
    MQTTString topic = MQTTString_initializer;
    int len = 0;

    if (!c || !topicName || !topic_msg) {
        return FAIL_RETURN;
    }

    topic.cstring = (char *)topicName;
    iotx_time_init(&timer);
    utils_time_countdown_ms(&timer, c->request_timeout_ms);

    HAL_MutexLock(c->lock_write_buf);
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
        HAL_MutexUnlock(c->lock_write_buf);
        log_err("MQTTSerialize_publish is error, len=%d, buf_size=%u, payloadlen=%u",
                len,
                c->buf_size_send,
                topic_msg->payload_len);
        return MQTT_PUBLISH_PACKET_ERROR;
    }

    list_node_t *node = NULL;

    // If the QOS >1, push the information into list of wait publish ACK
    if (topic_msg->qos > IOTX_MQTT_QOS0) {
        // push into list
        if (SUCCESS_RETURN != iotx_mc_push_pubInfo_to(c, len, topic_msg->packet_id, &node)) {
            log_err("push publish into to pubInfolist failed!");
            HAL_MutexUnlock(c->lock_write_buf);
            return MQTT_PUSH_TO_LIST_ERROR;
        }
    }

    // send the publish packet
    if (iotx_mc_send_packet(c, c->buf_send, len, &timer) != SUCCESS_RETURN) {
        if (topic_msg->qos > IOTX_MQTT_QOS0) {
            // If failed, remove from list
            HAL_MutexLock(c->lock_list_pub);
            list_remove(c->list_pub_wait_ack, node);
            HAL_MutexUnlock(c->lock_list_pub);
        }

        HAL_MutexUnlock(c->lock_write_buf);
        return MQTT_NETWORK_ERROR;
    }

    HAL_MutexUnlock(c->lock_write_buf);
    return SUCCESS_RETURN;
}


// MQTT send publish ACK
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
        len = MQTTSerialize_ack((unsigned char *)c->buf_send, c->buf_size_send, PUBACK, 0, msgId);
    } else if (type == PUBREC) {
        len = MQTTSerialize_ack((unsigned char *)c->buf_send, c->buf_size_send, PUBREC, 0, msgId);
    } else if (type == PUBREL) {
        len = MQTTSerialize_ack((unsigned char *)c->buf_send, c->buf_size_send, PUBREL, 0, msgId);
    } else {
        HAL_MutexUnlock(c->lock_write_buf);
        return MQTT_PUBLISH_ACK_TYPE_ERROR;
    }

    if (len <= 0) {
        HAL_MutexUnlock(c->lock_write_buf);
        return MQTT_PUBLISH_ACK_PACKET_ERROR;
    }

    rc = iotx_mc_send_packet(c, c->buf_send, len, &timer);
    if (rc != SUCCESS_RETURN) {
        HAL_MutexUnlock(c->lock_write_buf);
        return MQTT_NETWORK_ERROR;
    }

    HAL_MutexUnlock(c->lock_write_buf);
    return SUCCESS_RETURN;
}


// MQTT send subscribe packet
static int MQTTSubscribe(iotx_mc_client_t *c, const char *topicFilter, iotx_mqtt_qos_t qos, unsigned int msgId,
                         iotx_mqtt_event_handle_func_fpt messageHandler, void *pcontext)
{
    int len = 0;
    iotx_time_t timer;
    MQTTString topic = MQTTString_initializer;

    if (!c || !topicFilter || !messageHandler) {
        return FAIL_RETURN;
    }

    topic.cstring = (char *)topicFilter;
    iotx_time_init(&timer);
    utils_time_countdown_ms(&timer, c->request_timeout_ms);

    HAL_MutexLock(c->lock_write_buf);

    len = MQTTSerialize_subscribe((unsigned char *)c->buf_send, c->buf_size_send, 0, (unsigned short)msgId, 1, &topic,
                                  (int *)&qos);
    if (len <= 0) {
        HAL_MutexUnlock(c->lock_write_buf);
        return MQTT_SUBSCRIBE_PACKET_ERROR;
    }

    iotx_mc_topic_handle_t handler = {topicFilter, {messageHandler, pcontext}};

    list_node_t *node = NULL;


    /*
     * NOTE: It prefer to push the element into list and then remove it when send failed,
     *       because some of extreme cases
     * */

    // push the element to list of wait subscribe ACK
    if (SUCCESS_RETURN != iotx_mc_push_subInfo_to(c, len, msgId, SUBSCRIBE, &handler, &node)) {
        log_err("push publish into to pubInfolist failed!");
        HAL_MutexUnlock(c->lock_write_buf);
        return MQTT_PUSH_TO_LIST_ERROR;
    }

    if ((iotx_mc_send_packet(c, c->buf_send, len, &timer)) != SUCCESS_RETURN) { // send the subscribe packet
        // If send failed, remove it
        HAL_MutexLock(c->lock_list_sub);
        list_remove(c->list_sub_wait_ack, node);
        HAL_MutexUnlock(c->lock_list_sub);
        HAL_MutexUnlock(c->lock_write_buf);
        log_err("run sendPacket error!");
        return MQTT_NETWORK_ERROR;
    }

    HAL_MutexUnlock(c->lock_write_buf);
    return SUCCESS_RETURN;
}


// MQTT send unsubscribe packet
static int MQTTUnsubscribe(iotx_mc_client_t *c, const char *topicFilter, unsigned int msgId)
{
    iotx_time_t timer;
    MQTTString topic = MQTTString_initializer;
    int len = 0;

    if (!c || !topicFilter) {
        return FAIL_RETURN;
    }

    topic.cstring = (char *)topicFilter;
    iotx_time_init(&timer);
    utils_time_countdown_ms(&timer, c->request_timeout_ms);

    HAL_MutexLock(c->lock_write_buf);

    if ((len = MQTTSerialize_unsubscribe((unsigned char *)c->buf_send, c->buf_size_send, 0, (unsigned short)msgId, 1,
                                         &topic)) <= 0) {
        HAL_MutexUnlock(c->lock_write_buf);
        return MQTT_UNSUBSCRIBE_PACKET_ERROR;
    }

    iotx_mc_topic_handle_t handler = {topicFilter, {NULL, NULL}};

    // push into list
    list_node_t *node = NULL;
    if (SUCCESS_RETURN != iotx_mc_push_subInfo_to(c, len, msgId, UNSUBSCRIBE, &handler, &node)) {
        log_err("push publish into to pubInfolist failed!");
        HAL_MutexUnlock(c->lock_write_buf);
        return MQTT_PUSH_TO_LIST_ERROR;
    }

    if ((iotx_mc_send_packet(c, c->buf_send, len, &timer)) != SUCCESS_RETURN) { // send the subscribe packet
        // remove from list
        HAL_MutexLock(c->lock_list_sub);
        list_remove(c->list_sub_wait_ack, node);
        HAL_MutexUnlock(c->lock_list_sub);
        HAL_MutexUnlock(c->lock_write_buf);
        return MQTT_NETWORK_ERROR;
    }

    HAL_MutexUnlock(c->lock_write_buf);

    return SUCCESS_RETURN;
}


// MQTT send disconnect packet
static int MQTTDisconnect(iotx_mc_client_t *c)
{
    int rc = FAIL_RETURN;
    iotx_time_t timer;     // we might wait for incomplete incoming publishes to complete

    if (!c) {
        return FAIL_RETURN;
    }

    HAL_MutexLock(c->lock_write_buf);
    int len = MQTTSerialize_disconnect((unsigned char *)c->buf_send, c->buf_size_send);

    iotx_time_init(&timer);
    utils_time_countdown_ms(&timer, c->request_timeout_ms);

    if (len > 0) {
        rc = iotx_mc_send_packet(c, c->buf_send, len, &timer);           // send the disconnect packet
    }

    HAL_MutexUnlock(c->lock_write_buf);

    return rc;
}

#ifdef MQTT_ID2_CRYPTO
#define HASH_SHA1_LEN           (20)
#define HMAC_MD5_LEN            (16)
#define MQTT_MAX_TOPIC_LEN      (64)

typedef struct _MQTT_ReplayFender {
    uint32_t        sequence;
    uint16_t        hmac_len;
#ifdef HASH_SHA1
    char            hmac_str[HASH_SHA1_LEN * 2];
#else
    char            hmac_str[HMAC_MD5_LEN * 2];
#endif
} __attribute__((packed)) MQTT_ReplayFender;

#if 0
static void _to_lower(char *upper, int len)
{
    int             i;

    for (i = 0; i < len; i ++) {
        if (upper[i] >= 'A' && upper[i] <= 'Z') {
            upper[i] = upper[i] - 'A' + 'a';
        }
    }
}
#endif

static int _fill_replay_fender(
    iotx_mc_client_t *c,
    MQTT_ReplayFender *f,
    const char *topic,
    uint16_t msg_id,
    int enc_payloadlen)
{
    char                hmac_source[MQTT_MAX_TOPIC_LEN + 32] = {0};
    int                 hmac_srclen = 0;
    iotx_conn_info_pt   conn;

    if (!c || !f || !topic) {
        return -1;
    }

    if (!(conn = iotx_conn_info_get())) {
        return -1;
    }

    uint8_t *ptr = (uint8_t *) & (f->sequence);
    writeUint32(&ptr, (uint32_t)msg_id);
    ptr = (uint8_t *) & (f->hmac_len);
    writeUint16(&ptr, HMAC_MD5_LEN * 2);

    snprintf((char *)hmac_source, sizeof(hmac_source),
             "%s%d%d",
             topic, msg_id, enc_payloadlen);
    hmac_srclen = strlen(hmac_source);
    log_debug("hmac_source(%d) = '%s'", hmac_srclen, hmac_source);

    memset(f->hmac_str, 0, sizeof(f->hmac_str));
#ifdef HASH_SHA1
    uint8_t             sha1_hexbuf[HASH_SHA1_LEN] = {0};

    utils_sha1(hmac_source, hmac_srclen, sha1_hexbuf);
    LITE_hexbuf_convert(sha1_hexbuf, f->hmac_str, HASH_SHA1_LEN, 0);
    log_debug("sha1_str(%d) = '%s'", HASH_SHA1_LEN * 2, f->hmac_str);
#else
    utils_hmac_md5((const char *)hmac_source,
                   hmac_srclen,
                   f->hmac_str,
                   (const char *)conn->aeskey_hex,
                   (int)sizeof(conn->aeskey_hex));

    HEXDUMP_DEBUG(f->hmac_str, sizeof(f->hmac_str));
    //_to_lower(f->hmac_str, sizeof(f->hmac_str));
    HEXDUMP_DEBUG(f->hmac_str, sizeof(f->hmac_str));
#endif

    return 0;
}

static int _create_encoded_payload(
    iotx_mc_client_t *c,
    char *topic,
    iotx_mqtt_topic_info_pt topic_msg)
{
    MQTT_ReplayFender       fender;
    int                     ret = -1;
    int                     out_buf_len = 0;
    uint8_t                *out_buf = NULL;
    uint8_t                 iv[16] = {0};
    int                     enc_len = 0;
    iotx_conn_info_pt       conn;

    if (!c || !topic || !topic_msg) {
        return -1;
    }

    if (!(conn = iotx_conn_info_get())) {
        return -1;
    }

    out_buf_len = (topic_msg->payload_len / 16) * 16;
    if (topic_msg->payload_len % 16 != 0) {
        out_buf_len += 16;
    }

    /* get fender (fender is between mqtt header and mqtt message) */
    memset(&fender, 0, sizeof(MQTT_ReplayFender));
    _fill_replay_fender(c,
                        &fender,
                        topic,
                        topic_msg->packet_id,
                        out_buf_len);

    out_buf_len += sizeof(MQTT_ReplayFender);
    out_buf = (uint8_t *)LITE_malloc(out_buf_len + 1);
    memset(out_buf, 0, out_buf_len + 1);
    memcpy(out_buf, &fender, sizeof(MQTT_ReplayFender));

    /* encode MQTT message */
    ret = tfs_aes128_cbc_enc(conn->aeskey_hex,
                             iv,
                             topic_msg->payload_len,
                             (const uint8_t *)topic_msg->payload,
                             &enc_len,
                             out_buf + sizeof(MQTT_ReplayFender),
                             TFS_AES_ZERO_PADDING);
    log_debug("rc = tfs_aes128_cbc_enc() = %d, enc_len = %d", ret, enc_len);

    if (ret != 0) {
        log_err("tfs_aes128_cbc_enc error!");
        ret = -1;
        goto exit;
    }
    memset((void *)topic_msg->payload, 0, topic_msg->payload_len);
    memcpy((void *)topic_msg->payload, out_buf, out_buf_len);
    topic_msg->payload_len = out_buf_len;

    HEXDUMP_DEBUG(topic_msg->payload, topic_msg->payload_len);

exit:
    LITE_free(out_buf);

    return ret;
}
#endif  /* #ifdef MQTT_ID2_CRYPTO */

// remove the list element specified by @msgId from list of wait publish ACK
// return: 0, success; NOT 0, fail;
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
                log_err("node's value is invalid!");
                continue;
            }

            if (repubInfo->msg_id == msgId) {
                repubInfo->node_state = IOTX_MC_NODE_STATE_INVALID; // mark as invalid node
            }
        }

        list_iterator_destroy(iter);
    }
    HAL_MutexUnlock(c->lock_list_pub);

    return SUCCESS_RETURN;
}


// push the wait element into list of wait publish ACK
// return: 0, success; NOT 0, fail;
static int iotx_mc_push_pubInfo_to(iotx_mc_client_t *c, int len, unsigned short msgId, list_node_t **node)
{
    if (!c || !node) {
        log_err("the param of c is error!");
        return FAIL_RETURN;
    }

    if ((len < 0) || (len > c->buf_size_send)) {
        log_err("the param of len is error!");
        return FAIL_RETURN;
    }

    HAL_MutexLock(c->lock_list_pub);

    if (c->list_pub_wait_ack->len >= IOTX_MC_REPUB_NUM_MAX) {
        HAL_MutexUnlock(c->lock_list_pub);
        log_err("more than %u elements in republish list. List overflow!", c->list_pub_wait_ack->len);
        return FAIL_RETURN;
    }

    iotx_mc_pub_info_t *repubInfo = (iotx_mc_pub_info_t *)LITE_malloc(sizeof(iotx_mc_pub_info_t) + len);
    if (NULL == repubInfo) {
        HAL_MutexUnlock(c->lock_list_pub);
        log_err("run iotx_memory_malloc is error!");
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
        HAL_MutexUnlock(c->lock_list_pub);
        log_err("run list_node_new is error!");
        return FAIL_RETURN;
    }

    list_rpush(c->list_pub_wait_ack, *node);

    HAL_MutexUnlock(c->lock_list_pub);

    return SUCCESS_RETURN;
}


// push the wait element into list of wait subscribe(unsubscribe) ACK
// return: 0, success; NOT 0, fail;
static int iotx_mc_push_subInfo_to(iotx_mc_client_t *c, int len, unsigned short msgId, enum msgTypes type,
                                   iotx_mc_topic_handle_t *handler,
                                   list_node_t **node)
{
    if (!c || !handler || !node) {
        return FAIL_RETURN;
    }

    HAL_MutexLock(c->lock_list_sub);

    if (c->list_sub_wait_ack->len >= IOTX_MC_SUB_REQUEST_NUM_MAX) {
        HAL_MutexUnlock(c->lock_list_sub);
        log_err("number of subInfo more than max!,size = %d", c->list_sub_wait_ack->len);
        return FAIL_RETURN;
    }

    iotx_mc_subsribe_info_t *subInfo = (iotx_mc_subsribe_info_t *)LITE_malloc(sizeof(
                                                                                  iotx_mc_subsribe_info_t) + len);
    if (NULL == subInfo) {
        HAL_MutexUnlock(c->lock_list_sub);
        log_err("run iotx_memory_malloc is error!");
        return FAIL_RETURN;
    }

    subInfo->node_state = IOTX_MC_NODE_STATE_NORMANL;
    subInfo->msg_id = msgId;
    subInfo->len = len;
    iotx_time_start(&subInfo->sub_start_time);
    subInfo->type = type;
    subInfo->handler = *handler;
    subInfo->buf = (unsigned char *)subInfo + sizeof(iotx_mc_subsribe_info_t);

    memcpy(subInfo->buf, c->buf_send, len);

    *node = list_node_new(subInfo);
    if (NULL == *node) {
        HAL_MutexUnlock(c->lock_list_sub);
        log_err("run list_node_new is error!");
        return FAIL_RETURN;
    }

    list_rpush(c->list_sub_wait_ack, *node);

    HAL_MutexUnlock(c->lock_list_sub);

    return SUCCESS_RETURN;
}


// remove the list element specified by @msgId from list of wait subscribe(unsubscribe) ACK
// and return message handle by @messageHandler
// return: 0, success; NOT 0, fail;
static int iotx_mc_mask_subInfo_from(iotx_mc_client_t *c, unsigned int msgId, iotx_mc_topic_handle_t *messageHandler)
{
    if (!c || !messageHandler) {
        return FAIL_RETURN;
    }

    HAL_MutexLock(c->lock_list_sub);
    if (c->list_sub_wait_ack->len) {
        list_iterator_t *iter;
        list_node_t *node = NULL;
        iotx_mc_subsribe_info_t *subInfo = NULL;

        if (NULL == (iter = list_iterator_new(c->list_sub_wait_ack, LIST_TAIL))) {
            HAL_MutexLock(c->lock_list_sub);
            return SUCCESS_RETURN;
        }

        for (;;) {
            node = list_iterator_next(iter);
            if (NULL == node) {
                break;
            }

            subInfo = (iotx_mc_subsribe_info_t *) node->val;
            if (NULL == subInfo) {
                log_err("node's value is invalid!");
                continue;
            }

            if (subInfo->msg_id == msgId) {
                *messageHandler = subInfo->handler; // return handle
                subInfo->node_state = IOTX_MC_NODE_STATE_INVALID; // mark as invalid node
            }
        }

        list_iterator_destroy(iter);
    }
    HAL_MutexUnlock(c->lock_list_sub);

    return SUCCESS_RETURN;
}


// get next packet-id
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


// send packet
static int iotx_mc_send_packet(iotx_mc_client_t *c, char *buf, int length, iotx_time_t *time)
{
    int rc = FAIL_RETURN;
    int sent = 0;

    if (!c || !buf || !time) {
        return rc;
    }

    while (sent < length && !utils_time_is_expired(time)) {
        rc = c->ipstack->write(c->ipstack, &buf[sent], length, iotx_time_left(time));
        if (rc < 0) { // there was an error writing the data
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


// decode packet
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

        rc = c->ipstack->read(c->ipstack, &i, 1, timeout);
        if (rc != 1) {
            return MQTT_NETWORK_ERROR;
        }

        *value += (i & 127) * multiplier;
        multiplier *= 128;
    } while ((i & 128) != 0);

    return len;
}


// read packet
static int iotx_mc_read_packet(iotx_mc_client_t *c, iotx_time_t *timer, unsigned int *packet_type)
{
    MQTTHeader header = {0};
    int len = 0;
    int rem_len = 0;
    int rc = 0;

    if (!c || !timer || !packet_type) {
        return FAIL_RETURN;
    }

    /* 1. read the header byte.  This has the packet type in it */
    rc = c->ipstack->read(c->ipstack, c->buf_read, 1, iotx_time_left(timer));
    if (0 == rc) { // timeout
        *packet_type = 0;
        return SUCCESS_RETURN;
    } else if (1 != rc) {
        log_debug("mqtt read error, rc=%d", rc);
        return FAIL_RETURN;
    }

    len = 1;

    /* 2. read the remaining length.  This is variable in itself */
    if ((rc = iotx_mc_decode_packet(c, &rem_len, iotx_time_left(timer))) < 0) {
        log_err("decodePacket error,rc = %d", rc);
        return rc;
    }

    len += MQTTPacket_encode((unsigned char *)c->buf_read + 1,
                             rem_len); /* put the original remaining length back into the buffer */

    /*Check if the received data length exceeds mqtt read buffer length*/
    if ((rem_len > 0) && ((rem_len + len) > c->buf_size_read)) {
        log_err("mqtt read buffer is too short, mqttReadBufLen : %u, remainDataLen : %d", c->buf_size_read, rem_len);
        int needReadLen = c->buf_size_read - len;
        if (c->ipstack->read(c->ipstack, c->buf_read + len, needReadLen, iotx_time_left(timer)) != needReadLen) {
            log_err("mqtt read error");
            return FAIL_RETURN;
        }

        /* drop data whitch over the length of mqtt buffer*/
        int remainDataLen = rem_len - needReadLen;
        char *remainDataBuf = LITE_malloc(remainDataLen + 1);
        if (!remainDataBuf) {
            log_err("malloc remain buffer failed");
            return FAIL_RETURN;
        }

        if (c->ipstack->read(c->ipstack, remainDataBuf, remainDataLen, iotx_time_left(timer)) != remainDataLen) {
            log_err("mqtt read error");
            LITE_free(remainDataBuf);
            remainDataBuf = NULL;
            return FAIL_RETURN;
        }

        LITE_free(remainDataBuf);
        remainDataBuf = NULL;

        return FAIL_RETURN;

    }

    /* 3. read the rest of the buffer using a callback to supply the rest of the data */
    if (rem_len > 0 && (c->ipstack->read(c->ipstack, c->buf_read + len, rem_len, iotx_time_left(timer)) != rem_len)) {
        log_err("mqtt read error");
        return FAIL_RETURN;
    }

    header.byte = c->buf_read[0];
    *packet_type = header.bits.type;
    return SUCCESS_RETURN;
}


// check whether the topic is matched or not
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
            // skip until we meet the next separator, or end of string
            char *nextpos = curn + 1;
            while (nextpos < curn_end && *nextpos != '/') {
                nextpos = ++curn + 1;
            }
        } else if (*curf == '#') {
            curn = curn_end - 1;    // skip until end of string
        }
        curf++;
        curn++;
    }

    return (curn == curn_end) && (*curf == '\0');
}


// deliver message
static void iotx_mc_deliver_message(iotx_mc_client_t *c, MQTTString *topicName, iotx_mqtt_topic_info_pt topic_msg)
{
    int i, flag_matched = 0;

    if (!c || !topicName || !topic_msg) {
        return;
    }

    topic_msg->ptopic = topicName->lenstring.data;
    topic_msg->topic_len = topicName->lenstring.len;

    // we have to find the right message handler - indexed by topic
    HAL_MutexLock(c->lock_generic);
    for (i = 0; i < IOTX_MC_SUB_NUM_MAX; ++i) {

        if ((c->sub_handle[i].topic_filter != 0)
            && (MQTTPacket_equals(topicName, (char *)c->sub_handle[i].topic_filter)
                || iotx_mc_is_topic_matched((char *)c->sub_handle[i].topic_filter, topicName))) {
            log_debug("topic be matched");

            iotx_mc_topic_handle_t msg_handle = c->sub_handle[i];
            HAL_MutexUnlock(c->lock_generic);

            if (NULL != msg_handle.handle.h_fp) {
                iotx_mqtt_event_msg_t msg;
                msg.event_type = IOTX_MQTT_EVENT_PUBLISH_RECVEIVED;
                msg.msg = (void *)topic_msg;

                msg_handle.handle.h_fp(msg_handle.handle.pcontext, c, &msg);
                flag_matched = 1;
            }

            HAL_MutexLock(c->lock_generic);
        }
    }

    HAL_MutexUnlock(c->lock_generic);

    if (0 == flag_matched) {
        log_debug("NO matching any topic, call default handle function");

        if (NULL != c->handle_event.h_fp) {
            iotx_mqtt_event_msg_t msg;

            msg.event_type = IOTX_MQTT_EVENT_PUBLISH_RECVEIVED;
            msg.msg = topic_msg;

            c->handle_event.h_fp(c->handle_event.pcontext, c, &msg);
        }
    }
}


// handle CONNACK packet received from remote MQTT broker
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
        log_err("connect ack is error");
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


// handle PUBACK packet received from remote MQTT broker
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

    // call callback function to notify that PUBLISH is successful.
    if (NULL != c->handle_event.h_fp) {
        iotx_mqtt_event_msg_t msg;
        msg.event_type = IOTX_MQTT_EVENT_PUBLISH_SUCCESS;
        msg.msg = (void *)(uintptr_t)mypacketid;
        c->handle_event.h_fp(c->handle_event.pcontext, c, &msg);
    }

    return SUCCESS_RETURN;
}


// handle SUBACK packet received from remote MQTT broker
static int iotx_mc_handle_recv_SUBACK(iotx_mc_client_t *c)
{
    unsigned short mypacketid;
    int i, count = 0, grantedQoS = -1;
    int i_free = -1, flag_dup = 0;

    if (!c) {
        return FAIL_RETURN;
    }

    if (MQTTDeserialize_suback(&mypacketid, 1, &count, &grantedQoS, (unsigned char *)c->buf_read, c->buf_size_read) != 1) {
        log_err("Sub ack packet error");
        return MQTT_SUBSCRIBE_ACK_PACKET_ERROR;
    }

    /* In negative case, grantedQoS will be 0xFFFF FF80, which means -128 */
    if ((uint8_t)grantedQoS == 0x80) {
        iotx_mqtt_event_msg_t msg;

        log_err("MQTT SUBSCRIBE failed, ack code is 0x80");

        msg.event_type = IOTX_MQTT_EVENT_SUBCRIBE_NACK;
        msg.msg = (void *)(uintptr_t)mypacketid;
        c->handle_event.h_fp(c->handle_event.pcontext, c, &msg);

        return MQTT_SUBSCRIBE_ACK_FAILURE;
    }

    iotx_mc_topic_handle_t messagehandler;
    memset(&messagehandler, 0, sizeof(iotx_mc_topic_handle_t));
    (void)iotx_mc_mask_subInfo_from(c, mypacketid, &messagehandler);

    if ((NULL == messagehandler.handle.h_fp) || (NULL == messagehandler.topic_filter)) {
        return MQTT_SUB_INFO_NOT_FOUND_ERROR;
    }

    HAL_MutexLock(c->lock_generic);

    for (i = 0; i < IOTX_MC_SUB_NUM_MAX; ++i) {
        /*If subscribe the same topic and callback function, then ignore*/
        if ((NULL != c->sub_handle[i].topic_filter)) {
            if (0 == iotx_mc_check_handle_is_identical(&c->sub_handle[i], &messagehandler)) {
                // if subscribe a identical topic and relate callback function, then ignore this subscribe.
                flag_dup = 1;
                log_err("There is a identical topic and related handle in list!");
                break;
            }
        } else {
            if (-1 == i_free) {
                i_free = i; // record free element
            }
        }
    }

    if (0 == flag_dup) {
        if (-1 == i_free) {
            log_err("NOT more @sub_handle space!");
            return FAIL_RETURN;
        } else {
            c->sub_handle[i_free].topic_filter = messagehandler.topic_filter;
            c->sub_handle[i_free].handle.h_fp = messagehandler.handle.h_fp;
            c->sub_handle[i_free].handle.pcontext = messagehandler.handle.pcontext;
        }
    }

    HAL_MutexUnlock(c->lock_generic);

    // call callback function to notify that SUBSCRIBE is successful.
    if (NULL != c->handle_event.h_fp) {
        iotx_mqtt_event_msg_t msg;
        msg.event_type = IOTX_MQTT_EVENT_SUBCRIBE_SUCCESS;
        msg.msg = (void *)(uintptr_t)mypacketid;
        c->handle_event.h_fp(c->handle_event.pcontext, c, &msg);
    }

    return SUCCESS_RETURN;
}


// handle PUBLISH packet received from remote MQTT broker
static int iotx_mc_handle_recv_PUBLISH(iotx_mc_client_t *c)
{
    int result = 0;
    MQTTString topicName;
    iotx_mqtt_topic_info_t topic_msg;

    if (!c) {
        return FAIL_RETURN;
    }
    int qos=0;
    memset(&topic_msg, 0x0, sizeof(iotx_mqtt_topic_info_t));
    memset(&topicName, 0x0, sizeof(MQTTString));

    if (1 != MQTTDeserialize_publish((unsigned char *)&topic_msg.dup,
                                     (int *)&qos,
                                     (unsigned char *)&topic_msg.retain,
                                     (unsigned short *)&topic_msg.packet_id,
                                     &topicName,
                                     (unsigned char **)&topic_msg.payload,
                                     (int *)&topic_msg.payload_len,
                                     (unsigned char *)c->buf_read,
                                     c->buf_size_read)) {
        return MQTT_PUBLISH_PACKET_ERROR;
    }
    topic_msg.qos=(unsigned char)qos;
#ifdef MQTT_ID2_CRYPTO
    /* aes decrypt for MQTT payload */
    int                 ret = -1;
    uint8_t             iv[16] = {0};
    int32_t             dec_len = 0;
    int                 offset = sizeof(MQTT_ReplayFender);
    uint8_t            *dec_out = NULL;
    iotx_conn_info_pt   conn = NULL;

    conn = iotx_conn_info_get();
    if (!conn) {
        return -1;
    }
    dec_out = LITE_malloc(topic_msg.payload_len);
    if (!dec_out) {
        return -1;
    }

    memset(dec_out, 0, topic_msg.payload_len);
    ret = tfs_aes128_cbc_dec(conn->aeskey_hex,
                             iv,
                             topic_msg.payload_len - offset,
                             (const uint8_t *)topic_msg.payload + offset,
                             &dec_len,
                             dec_out,
                             TFS_AES_ZERO_PADDING);
    log_debug("rc = tfs_aes128_cbc_dec() = %d, dec_len = %d", ret, dec_len);

    if (ret != 0) {
        log_err("tfs_aes128_cbc_dec error!");
        LITE_free(dec_out);
        return -1;
    }

    memset((void *)topic_msg.payload, 0, topic_msg.payload_len);
    memcpy((void *)topic_msg.payload, dec_out, dec_len);
    topic_msg.payload_len = dec_len;

    log_debug("ID2 decrypt publish[%d]: %s", topic_msg.payload_len, topic_msg.payload);
    LITE_free(dec_out);
#endif

    char       *tmp_topic_name = NULL;

    tmp_topic_name = LITE_malloc(topicName.lenstring.len + 1);
    memset(tmp_topic_name, 0, topicName.lenstring.len + 1);
    memcpy(tmp_topic_name, topicName.lenstring.data, topicName.lenstring.len);
    log_debug("msg.id = | %d |", topic_msg.packet_id);
    log_debug("topicName = | %s |", tmp_topic_name);
    LITE_free(tmp_topic_name);

#if defined(INSPECT_MQTT_FLOW)
    HEXDUMP_DEBUG(topic_msg.payload, topic_msg.payload_len);
#endif

    topic_msg.ptopic = NULL;
    topic_msg.topic_len = 0;

    log_debug("delivering msg ...");

    iotx_mc_deliver_message(c, &topicName, &topic_msg);

    if (topic_msg.qos == IOTX_MQTT_QOS0) {
        return SUCCESS_RETURN;
    } else if (topic_msg.qos == IOTX_MQTT_QOS1) {
        result = MQTTPuback(c, topic_msg.packet_id, PUBACK);
    } else if (topic_msg.qos == IOTX_MQTT_QOS2) {
        result = MQTTPuback(c, topic_msg.packet_id, PUBREC);
    } else {
        log_err("Invalid QOS, QOSvalue = %d", topic_msg.qos);
        return MQTT_PUBLISH_QOS_ERROR;
    }

    return result;
}


// handle UNSUBACK packet received from remote MQTT broker
static int iotx_mc_handle_recv_UNSUBACK(iotx_mc_client_t *c)
{
    unsigned short i, mypacketid = 0;  // should be the same as the packetid above

    if (!c) {
        return FAIL_RETURN;
    }

    if (MQTTDeserialize_unsuback(&mypacketid, (unsigned char *)c->buf_read, c->buf_size_read) != 1) {

        return MQTT_UNSUBSCRIBE_ACK_PACKET_ERROR;
    }

    iotx_mc_topic_handle_t messageHandler;
    (void)iotx_mc_mask_subInfo_from(c, mypacketid, &messageHandler);

    /* Remove from message handler array */
    HAL_MutexLock(c->lock_generic);
    for (i = 0; i < IOTX_MC_SUB_NUM_MAX; ++i) {
        if ((c->sub_handle[i].topic_filter != NULL)
            && (0 == iotx_mc_check_handle_is_identical(&c->sub_handle[i], &messageHandler))) {
            memset(&c->sub_handle[i], 0, sizeof(iotx_mc_topic_handle_t));

            /* NOTE: in case of more than one register(subscribe) with different callback function,
             *       so we must keep continuously searching related message handle. */
        }
    }

    if (NULL != c->handle_event.h_fp) {
        iotx_mqtt_event_msg_t msg;
        msg.event_type = IOTX_MQTT_EVENT_UNSUBCRIBE_SUCCESS;
        msg.msg = (void *)(uintptr_t)mypacketid;

        c->handle_event.h_fp(c->handle_event.pcontext, c, &msg);
    }

    HAL_MutexUnlock(c->lock_generic);
    return SUCCESS_RETURN;
}


// wait CONNACK packet from remote MQTT broker
static int iotx_mc_wait_CONNACK(iotx_mc_client_t *c)
{
    unsigned int packetType = 0;
    int rc = 0;
    iotx_time_t timer;

    if (!c) {
        return FAIL_RETURN;
    }

    iotx_time_init(&timer);
    utils_time_countdown_ms(&timer, c->connect_data.keepAliveInterval * 1000);

    do {
        // read the socket, see what work is due
        rc = iotx_mc_read_packet(c, &timer, &packetType);
        if (rc != SUCCESS_RETURN) {
            log_err("readPacket error,result = %d", rc);
            return MQTT_NETWORK_ERROR;
        }

    } while (packetType != CONNACK);

    rc = iotx_mc_handle_recv_CONNACK(c);
    if (SUCCESS_RETURN != rc) {
        log_err("recvConnackProc error,result = %d", rc);
    }

    return rc;
}


// MQTT cycle to handle packet from remote broker
static int iotx_mc_cycle(iotx_mc_client_t *c, iotx_time_t *timer)
{
    unsigned int packetType;
    int rc = SUCCESS_RETURN;

    if (!c) {
        return FAIL_RETURN;
    }

    int state = iotx_mc_get_client_state(c);
    if (state != IOTX_MC_STATE_CONNECTED) {
        log_debug("state = %d", state);
        return MQTT_STATE_ERROR;
    }

    // read the socket, see what work is due
    rc = iotx_mc_read_packet(c, timer, &packetType);
    if (rc != SUCCESS_RETURN) {
        iotx_mc_set_client_state(c, IOTX_MC_STATE_DISCONNECTED);
        log_debug("readPacket error,result = %d", rc);
        return MQTT_NETWORK_ERROR;
    }

    if (MQTT_CPT_RESERVED == packetType) {
        // log_debug("wait data timeout");
        return SUCCESS_RETURN;
    }

    // receive any data to renew ping_timer
    utils_time_countdown_ms(&c->next_ping_time, c->connect_data.keepAliveInterval * 1000);

    // clear ping mark when any data received from MQTT broker
    HAL_MutexLock(c->lock_generic);
    c->ping_mark = 0;
    HAL_MutexUnlock(c->lock_generic);

    switch (packetType) {
        case CONNACK: {
            log_debug("CONNACK");
            break;
        }
        case PUBACK: {
            log_debug("PUBACK");
            rc = iotx_mc_handle_recv_PUBACK(c);
            if (SUCCESS_RETURN != rc) {
                log_err("recvPubackProc error,result = %d", rc);
            }

            break;
        }
        case SUBACK: {
            log_debug("SUBACK");
            rc = iotx_mc_handle_recv_SUBACK(c);
            if (SUCCESS_RETURN != rc) {
                log_err("recvSubAckProc error,result = %d", rc);
            }
            break;
        }
        case PUBLISH: {
            log_debug("PUBLISH");
            // HEXDUMP_DEBUG(c->buf_read, 32);

            rc = iotx_mc_handle_recv_PUBLISH(c);
            if (SUCCESS_RETURN != rc) {
                log_err("recvPublishProc error,result = %d", rc);
            }
            break;
        }
        case UNSUBACK: {
            rc = iotx_mc_handle_recv_UNSUBACK(c);
            if (SUCCESS_RETURN != rc) {
                log_err("recvUnsubAckProc error,result = %d", rc);
            }
            break;
        }
        case PINGRESP: {
            rc = SUCCESS_RETURN;
            log_info("receive ping response!");
            break;
        }
        default:
            log_err("INVALID TYPE");
            return FAIL_RETURN;
    }

    return rc;
}


// check MQTT client is in normal state.
// 0, in abnormal state; 1, in normal state.
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


// return: 0, identical; NOT 0, different.
static int iotx_mc_check_handle_is_identical(iotx_mc_topic_handle_t *messageHandlers1,
                                             iotx_mc_topic_handle_t *messageHandler2)
{
    if (!messageHandlers1 || !messageHandler2) {
        return 1;
    }

    int topicNameLen = strlen(messageHandlers1->topic_filter);

    if (topicNameLen != strlen(messageHandler2->topic_filter)) {
        return 1;
    }

    if (0 != strncmp(messageHandlers1->topic_filter, messageHandler2->topic_filter, topicNameLen)) {
        return 1;
    }

    if (messageHandlers1->handle.h_fp != messageHandler2->handle.h_fp) {
        return 1;
    }

    // context must be identical also.
    if (messageHandlers1->handle.pcontext != messageHandler2->handle.pcontext) {
        return 1;
    }

    return 0;
}


// subscribe
static int iotx_mc_subscribe(iotx_mc_client_t *c,
                                    const char *topicFilter,
                                    iotx_mqtt_qos_t qos,
                                    iotx_mqtt_event_handle_func_fpt topic_handle_func,
                                    void *pcontext)
{
    if (NULL == c || NULL == topicFilter || !topic_handle_func) {
        return NULL_VALUE_ERROR;
    }

    int rc = FAIL_RETURN;

    if ((int)qos < IOTX_MQTT_QOS0 || (int)qos > IOTX_MQTT_QOS2) {
        return rc;
    }

    if (!iotx_mc_check_state_normal(c)) {
        log_err("mqtt client state is error,state = %d", iotx_mc_get_client_state(c));
        return MQTT_STATE_ERROR;
    }

    if (0 != iotx_mc_check_topic(topicFilter, TOPIC_FILTER_TYPE)) {
        log_err("topic format is error,topicFilter = %s", topicFilter);
        return MQTT_TOPIC_FORMAT_ERROR;
    }

    unsigned int msgId = iotx_mc_get_next_packetid(c);
    rc = MQTTSubscribe(c, topicFilter, qos, msgId, topic_handle_func, pcontext);
    if (rc != SUCCESS_RETURN) {
        if (rc == MQTT_NETWORK_ERROR) {
            iotx_mc_set_client_state(c, IOTX_MC_STATE_DISCONNECTED);
        }

        log_err("run MQTTSubscribe error");
        return rc;
    }

    log_info("mqtt subscribe success,topic = %s!", topicFilter);
    return msgId;
}


// unsubscribe
static int iotx_mc_unsubscribe(iotx_mc_client_t *c, const char *topicFilter)
{
    if (NULL == c || NULL == topicFilter) {
        return NULL_VALUE_ERROR;
    }

    if (0 != iotx_mc_check_topic(topicFilter, TOPIC_FILTER_TYPE)) {
        log_err("topic format is error,topicFilter = %s", topicFilter);
        return MQTT_TOPIC_FORMAT_ERROR;
    }

    int rc = FAIL_RETURN;

    if (!iotx_mc_check_state_normal(c)) {
        log_err("mqtt client state is error,state = %d", iotx_mc_get_client_state(c));
        return MQTT_STATE_ERROR;
    }

    unsigned int msgId = iotx_mc_get_next_packetid(c);

    rc = MQTTUnsubscribe(c, topicFilter, msgId);
    if (rc != SUCCESS_RETURN) {
        if (rc == MQTT_NETWORK_ERROR) { // send the subscribe packet
            iotx_mc_set_client_state(c, IOTX_MC_STATE_DISCONNECTED);
        }

        log_err("run MQTTUnsubscribe error!");
        return rc;
    }

    log_info("mqtt unsubscribe success,topic = %s!", topicFilter);
    return msgId;
}

// publish
static int iotx_mc_publish(iotx_mc_client_t *c, const char *topicName, iotx_mqtt_topic_info_pt topic_msg)
{
    uint16_t msg_id = 0;

    if (NULL == c || NULL == topicName || NULL == topic_msg) {
        return NULL_VALUE_ERROR;
    }

    if (0 != iotx_mc_check_topic(topicName, TOPIC_NAME_TYPE)) {
        log_err("topic format is error,topicFilter = %s", topicName);
        return MQTT_TOPIC_FORMAT_ERROR;
    }

    int rc = FAIL_RETURN;

    if (!iotx_mc_check_state_normal(c)) {
        log_err("mqtt client state is error,state = %d", iotx_mc_get_client_state(c));
        return MQTT_STATE_ERROR;
    }

    if (topic_msg->qos == IOTX_MQTT_QOS1 || topic_msg->qos == IOTX_MQTT_QOS2) {
        msg_id = iotx_mc_get_next_packetid(c);
        topic_msg->packet_id = msg_id;
    }

#ifdef MQTT_ID2_CRYPTO
    _create_encoded_payload(c, (char *)topicName, topic_msg);
#endif

#if defined(INSPECT_MQTT_FLOW)
    HEXDUMP_DEBUG(topic_msg->payload, topic_msg->payload_len);
#endif

    rc = MQTTPublish(c, topicName, topic_msg);
    if (rc != SUCCESS_RETURN) { // send the subscribe packet
        if (rc == MQTT_NETWORK_ERROR) {
            iotx_mc_set_client_state(c, IOTX_MC_STATE_DISCONNECTED);
        }
        log_err("MQTTPublish is error, rc = %d", rc);
        return rc;
    }

    return msg_id;
}


// get state of MQTT client
static int iotx_mc_get_client_state(iotx_mc_client_t *pClient)
{

    if (!pClient) {
        return -1;
    }

    int state;
    HAL_MutexLock(pClient->lock_generic);
    state = pClient->client_state;
    HAL_MutexUnlock(pClient->lock_generic);

    return state;
}


// set state of MQTT client
static void iotx_mc_set_client_state(iotx_mc_client_t *pClient, iotx_mc_state_t newState)
{

    HAL_MutexLock(pClient->lock_generic);
    pClient->client_state = newState;
    HAL_MutexUnlock(pClient->lock_generic);
}


// set MQTT connection parameter
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
        log_warning("Input heartbeat interval(%d ms) < Allowed minimum(%d ms)",
                    (pConnectParams->keepAliveInterval * 1000),
                    (KEEP_ALIVE_INTERVAL_DEFAULT_MIN * 1000)
                   );
        log_warning("Reset heartbeat interval => %d Millisecond",
                    (KEEP_ALIVE_INTERVAL_DEFAULT_MIN * 1000)
                   );
        pClient->connect_data.keepAliveInterval = KEEP_ALIVE_INTERVAL_DEFAULT_MIN;
    } else if (pConnectParams->keepAliveInterval > KEEP_ALIVE_INTERVAL_DEFAULT_MAX) {
        log_warning("Input heartbeat interval(%d ms) > Allowed maximum(%d ms)",
                    (pConnectParams->keepAliveInterval * 1000),
                    (KEEP_ALIVE_INTERVAL_DEFAULT_MAX * 1000)
                   );
        log_warning("Reset heartbeat interval => %d Millisecond",
                    (KEEP_ALIVE_INTERVAL_DEFAULT_MAX * 1000)
                   );
        pClient->connect_data.keepAliveInterval = KEEP_ALIVE_INTERVAL_DEFAULT_MAX;
    } else {
        pClient->connect_data.keepAliveInterval = pConnectParams->keepAliveInterval;
    }

    return SUCCESS_RETURN;
}


// Initialize MQTT client
static int iotx_mc_init(iotx_mc_client_t *pClient, iotx_mqtt_param_t *pInitParams)
{
    int rc = FAIL_RETURN;
    int mc_state = IOTX_MC_STATE_INVALID;

    if ((NULL == pClient) || (NULL == pInitParams)) {
        return NULL_VALUE_ERROR;
    }

    memset(pClient, 0x0, sizeof(iotx_mc_client_t));

    MQTTPacket_connectData connectdata = MQTTPacket_connectData_initializer;

    connectdata.MQTTVersion = IOTX_MC_MQTT_VERSION;
    connectdata.keepAliveInterval = pInitParams->keepalive_interval_ms / 1000;

    connectdata.clientID.cstring = (char *)pInitParams->client_id;
    connectdata.username.cstring = (char *)pInitParams->username;
    connectdata.password.cstring = (char *)pInitParams->password;


    memset(pClient->sub_handle, 0, IOTX_MC_SUB_NUM_MAX * sizeof(iotx_mc_topic_handle_t));

    pClient->packet_id = 0;
    pClient->lock_generic = HAL_MutexCreate();
    pClient->lock_list_sub = HAL_MutexCreate();
    pClient->lock_list_pub = HAL_MutexCreate();

    if (pInitParams->request_timeout_ms < IOTX_MC_REQUEST_TIMEOUT_MIN_MS
        || pInitParams->request_timeout_ms > IOTX_MC_REQUEST_TIMEOUT_MAX_MS) {

        pClient->request_timeout_ms = IOTX_MC_REQUEST_TIMEOUT_DEFAULT_MS;
    } else {
        pClient->request_timeout_ms = pInitParams->request_timeout_ms;
    }

    pClient->buf_send = pInitParams->pwrite_buf;
    pClient->buf_size_send = pInitParams->write_buf_size;
    pClient->buf_read = pInitParams->pread_buf;
    pClient->buf_size_read = pInitParams->read_buf_size;

    pClient->handle_event.h_fp = pInitParams->handle_event.h_fp;
    pClient->handle_event.pcontext = pInitParams->handle_event.pcontext;

    /* Initialize reconnect parameter */
    pClient->reconnect_param.reconnect_time_interval_ms = IOTX_MC_RECONNECT_INTERVAL_MIN_MS;

    pClient->list_pub_wait_ack = list_new();
    pClient->list_pub_wait_ack->free = LITE_free_routine;
    pClient->list_sub_wait_ack = list_new();
    pClient->list_sub_wait_ack->free = LITE_free_routine;

    pClient->lock_write_buf = HAL_MutexCreate();


    /* Initialize MQTT connect parameter */
    rc = iotx_mc_set_connect_params(pClient, &connectdata);
    if (SUCCESS_RETURN != rc) {
        mc_state = IOTX_MC_STATE_INVALID;
        goto RETURN;
    }

    iotx_time_init(&pClient->next_ping_time);
    iotx_time_init(&pClient->reconnect_param.reconnect_next_time);

    pClient->ipstack = (utils_network_pt)LITE_malloc(sizeof(utils_network_t));
    if (NULL == pClient->ipstack) {
        log_err("malloc Network failed");
        rc = FAIL_RETURN;
        goto RETURN;
    }
    memset(pClient->ipstack, 0x0, sizeof(utils_network_t));

    rc = iotx_net_init(pClient->ipstack, pInitParams->host, pInitParams->port, pInitParams->pub_key);
    if (SUCCESS_RETURN != rc) {
        mc_state = IOTX_MC_STATE_INVALID;
        goto RETURN;
    }
#if defined(MQTT_ID2_CRYPTO)
    pClient->ipstack->ca_crt = NULL;
    pClient->ipstack->ca_crt_len = 0;
#endif
    mc_state = IOTX_MC_STATE_INITIALIZED;
    rc = SUCCESS_RETURN;
    log_info("MQTT init success!");

RETURN :
    if (rc != SUCCESS_RETURN) {
        if (pClient->list_pub_wait_ack) {
            pClient->list_pub_wait_ack->free(pClient->list_pub_wait_ack);
            pClient->list_pub_wait_ack = NULL;
        }
        if (pClient->list_sub_wait_ack) {
            pClient->list_sub_wait_ack->free(pClient->list_sub_wait_ack);
            pClient->list_sub_wait_ack = NULL;
        }
        if (pClient->ipstack) {
            LITE_free(pClient->ipstack);
            pClient->ipstack = NULL;
        }
        if (pClient->lock_generic) {
            HAL_MutexDestroy(pClient->lock_generic);
            pClient->lock_generic = NULL;
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
    }
    iotx_mc_set_client_state(pClient, (iotx_mc_state_t)mc_state);

    return rc;
}


// remove node of list of wait subscribe ACK, which is in invalid state or timeout
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
            log_err("new list failed");
            HAL_MutexLock(pClient->lock_list_sub);
            return SUCCESS_RETURN;
        }

        for (;;) {
            node = list_iterator_next(iter);

            if (NULL != tempNode) {
                list_remove(pClient->list_sub_wait_ack, tempNode);
                tempNode = NULL;
            }

            if (NULL == node) {
                break; // end of list
            }

            iotx_mc_subsribe_info_t *subInfo = (iotx_mc_subsribe_info_t *) node->val;
            if (NULL == subInfo) {
                log_err("node's value is invalid!");
                tempNode = node;
                continue;
            }

            // remove invalid node
            if (IOTX_MC_NODE_STATE_INVALID == subInfo->node_state) {
                tempNode = node;
                continue;
            }

            if (iotx_mc_get_client_state(pClient) != IOTX_MC_STATE_CONNECTED) {
                continue;
            }

            // check the request if timeout or not
            if (utils_time_spend(&subInfo->sub_start_time) <= (pClient->request_timeout_ms * 2)) {
                // continue to check the next node
                continue;
            }

            /* When arrive here, it means timeout to wait ACK */
            packet_id = subInfo->msg_id;
            msg_type = subInfo->type;

            HAL_MutexUnlock(pClient->lock_list_sub);

            // Wait MQTT SUBSCRIBE ACK timeout
            if (NULL != pClient->handle_event.h_fp) {
                iotx_mqtt_event_msg_t msg;

                if (SUBSCRIBE == msg_type) {
                    // subscribe timeout
                    msg.event_type = IOTX_MQTT_EVENT_SUBCRIBE_TIMEOUT;
                    msg.msg = (void *)(uintptr_t)packet_id;
                } else { /*if (UNSUBSCRIBE == msg_type)*/
                    // unsubscribe timeout
                    msg.event_type = IOTX_MQTT_EVENT_UNSUBCRIBE_TIMEOUT;
                    msg.msg = (void *)(uintptr_t)packet_id;
                }

                pClient->handle_event.h_fp(pClient->handle_event.pcontext, pClient, &msg);
            }

            HAL_MutexLock(pClient->lock_list_sub);

            tempNode = node;
        }

        list_iterator_destroy(iter);

    } while (0);

    HAL_MutexUnlock(pClient->lock_list_sub);

    return rc;
}

#ifndef STM32_USE_SPI_WIFI
int is_connected = 0;

static void cb_recv(int fd, void *arg)
{
    iotx_mc_client_t *pClient = (iotx_mc_client_t *)arg;

    if (!is_connected) {
        if (SUCCESS_RETURN != iotx_mc_wait_CONNACK(pClient)) {
            (void)MQTTDisconnect(pClient);
            pClient->ipstack->disconnect(pClient->ipstack);
            log_err("wait connect ACK timeout, or receive a ACK indicating error!");
            return;
        }
        iotx_mc_set_client_state(pClient, IOTX_MC_STATE_CONNECTED);
        utils_time_countdown_ms(&pClient->next_ping_time, pClient->connect_data.keepAliveInterval * 1000);
        aos_post_event(EV_SYS, CODE_SYS_ON_MQTT_READ, 0u);
        is_connected = 1;
        return;
    }

    IOT_MQTT_Yield(pClient, 100);
}
extern int get_iotx_fd();
//extern int get_ssl_fd();
#endif

static void iotx_mc_keepalive(iotx_mc_client_t *pClient)
{
    int rc = 0;

    if (!pClient) {
        return;
    }
    /*Periodic sending ping packet to detect whether the network is connected*/
    iotx_mc_keepalive_sub(pClient);

    int currentState = iotx_mc_get_client_state(pClient);
    do {
        /*if Exceeds the maximum delay time, then return reconnect timeout*/
        if (IOTX_MC_STATE_DISCONNECTED_RECONNECTING == currentState) {
            /*Reconnection is successful, Resume regularly ping packets*/
            HAL_MutexLock(pClient->lock_generic);
            pClient->ping_mark = 0;
            HAL_MutexUnlock(pClient->lock_generic);
            rc = iotx_mc_handle_reconnect(pClient);
            if (SUCCESS_RETURN != rc) {
                log_debug("reconnect network fail, rc = %d", rc);
            } else {
                log_info("network is reconnected!");
                iotx_mc_reconnect_callback(pClient);
                pClient->reconnect_param.reconnect_time_interval_ms = IOTX_MC_RECONNECT_INTERVAL_MIN_MS;
            }

            break;
        }

        /*If network suddenly interrupted, stop pinging packet, try to reconnect network immediately*/
        if (IOTX_MC_STATE_DISCONNECTED == currentState) {
            log_err("network is disconnected!");
            iotx_mc_disconnect_callback(pClient);

            pClient->reconnect_param.reconnect_time_interval_ms = IOTX_MC_RECONNECT_INTERVAL_MIN_MS;
            utils_time_countdown_ms(&(pClient->reconnect_param.reconnect_next_time),
                                    pClient->reconnect_param.reconnect_time_interval_ms);
#ifndef STM32_USE_SPI_WIFI
            if (is_connected) {
                aos_cancel_poll_read_fd(get_iotx_fd(), cb_recv, pClient);
                is_connected = 0;
            }
#endif
            pClient->ipstack->disconnect(pClient->ipstack);
            iotx_mc_set_client_state(pClient, IOTX_MC_STATE_DISCONNECTED_RECONNECTING);
            break;
        }

    } while (0);
}


// republish
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


// remove node of list of wait publish ACK, which is in invalid state or timeout
static int MQTTPubInfoProc(iotx_mc_client_t *pClient)
{
    int rc = 0;
    int state = IOTX_MC_STATE_INVALID;

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
            log_err("new list failed");
            break;
        }

        for (;;) {
            node = list_iterator_next(iter);

            if (NULL != tempNode) {
                list_remove(pClient->list_pub_wait_ack, tempNode);
                tempNode = NULL;
            }

            if (NULL == node) {
                break; // end of list
            }

            iotx_mc_pub_info_t *repubInfo = (iotx_mc_pub_info_t *) node->val;
            if (NULL == repubInfo) {
                log_err("node's value is invalid!");
                tempNode = node;
                continue;
            }

            // remove invalid node
            if (IOTX_MC_NODE_STATE_INVALID == repubInfo->node_state) {
                tempNode = node;
                continue;
            }

            state = iotx_mc_get_client_state(pClient);
            if (state != IOTX_MC_STATE_CONNECTED) {
                continue;
            }

            // check the request if timeout or not
            if (utils_time_spend(&repubInfo->pub_start_time) <= (pClient->request_timeout_ms * 2)) {
                continue;
            }

            // If wait ACK timeout, republish
            HAL_MutexUnlock(pClient->lock_list_pub);
            rc = MQTTRePublish(pClient, (char *)repubInfo->buf, repubInfo->len);
            iotx_time_start(&repubInfo->pub_start_time);
            HAL_MutexLock(pClient->lock_list_pub);

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


// connect
static int iotx_mc_connect(iotx_mc_client_t *pClient)
{
    int rc = FAIL_RETURN;

    if (NULL == pClient) {
        return NULL_VALUE_ERROR;
    }

    /*Establish TCP or TLS connection*/
    rc = pClient->ipstack->connect(pClient->ipstack);
    if (SUCCESS_RETURN != rc) {
        pClient->ipstack->disconnect(pClient->ipstack);
        log_err("TCP or TLS Connection failed");

        if (ERROR_CERTIFICATE_EXPIRED == rc) {
            log_err("certificate is expired!");
            return ERROR_CERT_VERIFY_FAIL;
        } else {
            return MQTT_NETWORK_CONNECT_ERROR;
        }
    }

    log_debug("start MQTT connection with parameters: clientid=%s, username=%s, password=%s",
              pClient->connect_data.clientID.cstring,
              pClient->connect_data.username.cstring,
              pClient->connect_data.password.cstring);

    rc = MQTTConnect(pClient);
    if (rc  != SUCCESS_RETURN) {
        pClient->ipstack->disconnect(pClient->ipstack);
        log_err("send connect packet failed");
        return rc;
    }
#ifdef STM32_USE_SPI_WIFI
    if (SUCCESS_RETURN != iotx_mc_wait_CONNACK(pClient)) {
        (void)MQTTDisconnect(pClient);
        pClient->ipstack->disconnect(pClient->ipstack);
        log_err("wait connect ACK timeout, or receive a ACK indicating error!");
        return MQTT_CONNECT_ERROR;
    }

    iotx_mc_set_client_state(pClient, IOTX_MC_STATE_CONNECTED);

    utils_time_countdown_ms(&pClient->next_ping_time, pClient->connect_data.keepAliveInterval * 1000);

    log_info("mqtt connect success!");
#else
    aos_poll_read_fd(get_iotx_fd(), cb_recv, pClient);
#endif
    return rc;
}


static int iotx_mc_attempt_reconnect(iotx_mc_client_t *pClient)
{

    int rc;

    log_info("reconnect params:MQTTVersion =%d clientID =%s keepAliveInterval =%d username = %s",
             pClient->connect_data.MQTTVersion,
             pClient->connect_data.clientID.cstring,
             pClient->connect_data.keepAliveInterval,
             pClient->connect_data.username.cstring);

    /* Ignoring return code. failures expected if network is disconnected */
    rc = iotx_mc_connect(pClient);

    if (SUCCESS_RETURN != rc) {
        log_err("run iotx_mqtt_connect() error!");
        return rc;
    }

    return SUCCESS_RETURN;
}


// reconnect
static int iotx_mc_handle_reconnect(iotx_mc_client_t *pClient)
{

    if (NULL == pClient) {
        return NULL_VALUE_ERROR;
    }

    if (!utils_time_is_expired(&(pClient->reconnect_param.reconnect_next_time))) {
        /* Timer has not expired. Not time to attempt reconnect yet. Return attempting reconnect */
        return FAIL_RETURN;
    }

    log_info("start reconnect");

    // REDO AUTH before each reconnection
    if (0 != iotx_guider_authenticate()) {
        log_err("run iotx_guider_authenticate() error!\n");
        return -1;
    }

    int rc = FAIL_RETURN;
    rc = iotx_mc_attempt_reconnect(pClient);
    if (SUCCESS_RETURN == rc) {
        iotx_mc_set_client_state(pClient, IOTX_MC_STATE_CONNECTED);
        return SUCCESS_RETURN;
    } else {
        /*if reconnect network failed, then increase currentReconnectWaitInterval,
        ex: init currentReconnectWaitInterval=1s,  reconnect failed then 2s .4s. 8s*/
        if (IOTX_MC_RECONNECT_INTERVAL_MAX_MS > pClient->reconnect_param.reconnect_time_interval_ms) {
            pClient->reconnect_param.reconnect_time_interval_ms *= 2;
        } else {
            pClient->reconnect_param.reconnect_time_interval_ms = IOTX_MC_RECONNECT_INTERVAL_MAX_MS;
        }
    }

    utils_time_countdown_ms(&(pClient->reconnect_param.reconnect_next_time),
                            pClient->reconnect_param.reconnect_time_interval_ms);

    log_err("mqtt reconnect failed rc = %d", rc);

    return rc;
}


// disconnect
static int iotx_mc_disconnect(iotx_mc_client_t *pClient)
{
    int             rc = -1;
    if (NULL == pClient) {
        return NULL_VALUE_ERROR;
    }

    if (iotx_mc_check_state_normal(pClient)) {
        rc = MQTTDisconnect(pClient);
        log_debug("rc = MQTTDisconnect() = %d", rc);
    }
#ifndef STM32_USE_SPI_WIFI
    if (is_connected) {
        aos_cancel_poll_read_fd(get_iotx_fd(), cb_recv, pClient);
        is_connected = 0;
    }
#endif

    /*close tcp/ip socket or free tls resources*/
    pClient->ipstack->disconnect(pClient->ipstack);

    iotx_mc_set_client_state(pClient, IOTX_MC_STATE_INITIALIZED);

    log_info("mqtt disconnect!");
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


// release MQTT resource
static int iotx_mc_release(iotx_mc_client_t *pClient)
{

    if (NULL == pClient) {
        return NULL_VALUE_ERROR;
    }

    // iotx_delete_thread(pClient);
    HAL_SleepMs(100);

    iotx_mc_disconnect(pClient);
    iotx_mc_set_client_state(pClient, IOTX_MC_STATE_INVALID);
    HAL_SleepMs(100);

    HAL_MutexDestroy(pClient->lock_generic);
    HAL_MutexDestroy(pClient->lock_list_sub);
    HAL_MutexDestroy(pClient->lock_list_pub);
    HAL_MutexDestroy(pClient->lock_write_buf);

    list_destroy(pClient->list_pub_wait_ack);
    list_destroy(pClient->list_sub_wait_ack);

    if (NULL != pClient->ipstack) {
        LITE_free(pClient->ipstack);
    }

    log_info("mqtt release!");
    return SUCCESS_RETURN;
}


static void iotx_mc_reconnect_callback(iotx_mc_client_t *pClient)
{

    /*handle callback function*/
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

    /*if in disabled state, without having to send ping packets*/
    if (!iotx_mc_check_state_normal(pClient)) {
        return SUCCESS_RETURN;
    }

    /*if there is no ping_timer timeout, then return success*/
    if (!utils_time_is_expired(&pClient->next_ping_time)) {
        return SUCCESS_RETURN;
    }

    // update to next time sending MQTT keep-alive
    utils_time_countdown_ms(&pClient->next_ping_time, pClient->connect_data.keepAliveInterval * 1000);

    rc = MQTTKeepalive(pClient);
    if (SUCCESS_RETURN != rc) {
        if (rc == MQTT_NETWORK_ERROR) {
            iotx_mc_set_client_state(pClient, IOTX_MC_STATE_DISCONNECTED);
        }
        log_err("ping outstanding is error,result = %d", rc);
        return rc;
    }

    log_info("send MQTT ping...");

    HAL_MutexLock(pClient->lock_generic);
    pClient->ping_mark = 1;
    HAL_MutexUnlock(pClient->lock_generic);

    return SUCCESS_RETURN;
}



/************************  Public Interface ************************/
void *IOT_MQTT_Construct(iotx_mqtt_param_t *pInitParams)
{
    int err;
    iotx_mc_client_t   *pclient;

    POINTER_SANITY_CHECK(pInitParams, NULL);
    POINTER_SANITY_CHECK(pInitParams->pwrite_buf, NULL);
    POINTER_SANITY_CHECK(pInitParams->pread_buf, NULL);

    STRING_PTR_SANITY_CHECK(pInitParams->host, NULL);
    STRING_PTR_SANITY_CHECK(pInitParams->client_id, NULL);
    STRING_PTR_SANITY_CHECK(pInitParams->username, NULL);
    STRING_PTR_SANITY_CHECK(pInitParams->password, NULL);

    pclient = (iotx_mc_client_t *)LITE_malloc(sizeof(iotx_mc_client_t));
    if (NULL == pclient) {
        log_err("not enough memory.");
        return NULL;
    }

    err = iotx_mc_init(pclient, pInitParams);
    if (SUCCESS_RETURN != err) {
        LITE_free(pclient);
        return NULL;
    }

    err = iotx_mc_connect(pclient);
    if (SUCCESS_RETURN != err) {
        iotx_mc_release(pclient);
        LITE_free(pclient);
        return NULL;
    }

    return pclient;
}

int IOT_MQTT_Destroy(void **phandler)
{
    POINTER_SANITY_CHECK(phandler, NULL_VALUE_ERROR);
    POINTER_SANITY_CHECK(*phandler, NULL_VALUE_ERROR);

    iotx_mc_release((iotx_mc_client_t *)(*phandler));
    LITE_free(*phandler);
    *phandler = NULL;

    return SUCCESS_RETURN;
}

int IOT_MQTT_Yield(void *handle, int timeout_ms)
{
    int                 rc = SUCCESS_RETURN;
    iotx_mc_client_t   *pClient = (iotx_mc_client_t *)handle;
    iotx_time_t         time;

    POINTER_SANITY_CHECK(handle, NULL_VALUE_ERROR);
    if (timeout_ms < 0) {
        log_err("Invalid argument, timeout_ms = %d", timeout_ms);
        return -1;
    }
    if (timeout_ms == 0) {
        timeout_ms = 10;
    }

    iotx_time_init(&time);
    utils_time_countdown_ms(&time, timeout_ms);
#ifdef STM32_USE_SPI_WIFI
    do {
#endif
        // acquire package in cycle, such as PINGRESP or PUBLISH
        rc = iotx_mc_cycle(pClient, &time);
        if (SUCCESS_RETURN == rc) {
            // check list of wait publish ACK to remove node that is ACKED or timeout
            MQTTPubInfoProc(pClient);

            // check list of wait subscribe(or unsubscribe) ACK to remove node that is ACKED or timeout
            MQTTSubInfoProc(pClient);
#ifdef STM32_USE_SPI_WIFI
        } else {
            return rc;
#endif
        }

        // Keep MQTT alive or reconnect if connection abort.
        iotx_mc_keepalive(pClient);
#ifdef STM32_USE_SPI_WIFI
    } while (!utils_time_is_expired(&time) && (SUCCESS_RETURN == rc));
#endif

    return 0;
}

// check whether MQTT connection is established or not.
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
    POINTER_SANITY_CHECK(handle, NULL_VALUE_ERROR);
    POINTER_SANITY_CHECK(topic_handle_func, NULL_VALUE_ERROR);
    STRING_PTR_SANITY_CHECK(topic_filter, NULL_VALUE_ERROR);

    if ((int)qos < IOTX_MQTT_QOS0 || (int)qos > IOTX_MQTT_QOS2) {
        log_warning("Invalid qos(%d) out of [%d, %d], using %d",
                    qos,
                    IOTX_MQTT_QOS0, IOTX_MQTT_QOS2, IOTX_MQTT_QOS0);
        qos = IOTX_MQTT_QOS0;
    }

    return iotx_mc_subscribe((iotx_mc_client_t *)handle, topic_filter, qos, topic_handle_func, pcontext);
}

int IOT_MQTT_Unsubscribe(void *handle, const char *topic_filter)
{
    POINTER_SANITY_CHECK(handle, NULL_VALUE_ERROR);
    STRING_PTR_SANITY_CHECK(topic_filter, NULL_VALUE_ERROR);

    return iotx_mc_unsubscribe((iotx_mc_client_t *)handle, topic_filter);
}

int IOT_MQTT_Publish(void *handle, const char *topic_name, iotx_mqtt_topic_info_pt topic_msg)
{
    POINTER_SANITY_CHECK(handle, NULL_VALUE_ERROR);
    STRING_PTR_SANITY_CHECK(topic_name, NULL_VALUE_ERROR);

    return iotx_mc_publish((iotx_mc_client_t *)handle, topic_name, topic_msg);
}

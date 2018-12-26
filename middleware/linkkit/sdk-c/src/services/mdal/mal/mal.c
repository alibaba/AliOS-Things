/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <stddef.h>
#include "iot_export.h"
#include "iotx_system.h"
#include "iotx_utils.h"
#include "iotx_system_internal.h"
#include "utils_sysinfo.h"
#include "utils_hmac.h"

#include "mdal_mal_import.h"
#include "mdal_mal_export.h"
#include "mal.h"

#define MQTT_DEFAULT_MSG_LEN 1280
#define MAL_TIMEOUT_FOREVER -1
#define MAL_MC_PACKET_ID_MAX (65535)
#define MAL_MC_TOPIC_NAME_MAX_LEN (128)
#define MAL_MC_DEFAULT_BUFFER_NUM  1
#define MAL_MC_MAX_BUFFER_NUM  14
#define MAL_MC_MAX_TOPIC_LEN   128
#define MAL_MC_MAX_MSG_LEN     512

#define MAL_MC_DEAFULT_TIMEOUT   (8000)
#define GUIDER_SIGN_LEN             (66)
#define GUIDER_TS_LEN               (16)

#define mal_emerg(...)             log_emerg("MAL", __VA_ARGS__)
#define mal_crit(...)              log_crit("MAL", __VA_ARGS__)
#define mal_err(...)               log_err("MAL", __VA_ARGS__)
#define mal_warning(...)           log_warning("MAL", __VA_ARGS__)
#define mal_info(...)              log_info("MAL", __VA_ARGS__)
#define mal_debug(...)             log_debug("MAL", __VA_ARGS__)

#define mal_malloc(...)            LITE_malloc(__VA_ARGS__, MEM_MAGIC, "mal")
#define mal_free                   LITE_free

typedef struct at_mqtt_msg_buff_s {
    uint8_t  write_index;
    uint8_t  read_index;
    uint8_t  last_write_index;
    uint8_t  valid_flag[MAL_MC_MAX_BUFFER_NUM];
    uint8_t  buffer_num;
    char    *topic[MAL_MC_MAX_BUFFER_NUM];
    char    *msg_data[MAL_MC_MAX_BUFFER_NUM];
    void    *buffer_mutex;
} at_mqtt_msg_buff_t;
static at_mqtt_msg_buff_t    g_at_mqtt_buff_mgr;
static char g_at_mqtt_topic[MAL_MC_DEFAULT_BUFFER_NUM][MAL_MC_TOPIC_NAME_MAX_LEN];
static char g_at_mqtt_msg_data[MAL_MC_DEFAULT_BUFFER_NUM][MAL_MC_MAX_MSG_LEN];

static int mal_mc_check_state_normal(iotx_mc_client_t *c);
static int mal_mc_release(iotx_mc_client_t *c);
static iotx_mc_state_t mal_mc_get_client_state(iotx_mc_client_t *pClient);
static void mal_mc_set_client_state(iotx_mc_client_t *pClient, iotx_mc_state_t newState);
int mal_mc_data_copy_from_buf(char *topic, char *message);

static void *g_mqtt_client = NULL;

static int mal_mc_check_rule(char *iterm, iotx_mc_topic_type_t type)
{
    int i = 0;
    int len = 0;

    if (NULL == iterm) {
        mal_err("iterm is NULL");
        return FAIL_RETURN;
    }

    len = strlen(iterm);

    for (i = 0; i < len; i++) {
        if (TOPIC_FILTER_TYPE == type) {
            if ('+' == iterm[i] || '#' == iterm[i]) {
                if (1 != len) {
                    mal_err("the character # and + is error");
                    return FAIL_RETURN;
                }
            }
        } else {
            if ('+' == iterm[i] || '#' == iterm[i]) {
                mal_err("has character # and + is error");
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
static char mal_mc_is_topic_matched(char *topicFilter, const char *topicName)
{
    if (!topicFilter || !topicName) {
        return 0;
    }
    char *curf = topicFilter;
    const char *curn = topicName;
    const char *curn_end = curn + strlen(topicName);

    while (*curf && curn < curn_end) {
        if (*curn == '/' && *curf != '/') {
            break;
        }

        if (*curf != '+' && *curf != '#' && *curf != *curn) {
            break;
        }

        if (*curf == '+') {
            /* skip until we meet the next separator, or end of string */
            const char *nextpos = curn + 1;
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
static int mal_mc_check_topic(const char *topicName, iotx_mc_topic_type_t type)
{
    int mask = 0;
    char *delim = "/";
    char *iterm = NULL;
    char topicString[MAL_MC_TOPIC_NAME_MAX_LEN];
    if (NULL == topicName || '/' != topicName[0]) {
        return FAIL_RETURN;
    }

    if (strlen(topicName) > MAL_MC_TOPIC_NAME_MAX_LEN) {
        mal_err("len of topicName(%d) exceeds 64", strlen(topicName));
        return FAIL_RETURN;
    }

    memset(topicString, 0x0, MAL_MC_TOPIC_NAME_MAX_LEN);
    strncpy(topicString, topicName, MAL_MC_TOPIC_NAME_MAX_LEN - 1);

    iterm = strtok(topicString, delim);

    if (SUCCESS_RETURN != mal_mc_check_rule(iterm, type)) {
        mal_err("run iotx_check_rule error");
        return FAIL_RETURN;
    }

    for (;;) {
        iterm = strtok(NULL, delim);

        if (iterm == NULL) {
            break;
        }

        /* The character '#' is not in the last */
        if (1 == mask) {
            mal_err("the character # is error");
            return FAIL_RETURN;
        }

        if (SUCCESS_RETURN != mal_mc_check_rule(iterm, type)) {
            mal_err("run iotx_check_rule error");
            return FAIL_RETURN;
        }

        if (iterm[0] == '#') {
            mask = 1;
        }
    }

    return SUCCESS_RETURN;
}

/* MQTT send connect packet */
int MALMQTTConnect(iotx_mc_client_t *pClient)
{
    char product_key[PRODUCT_KEY_LEN + 1] = {0};
    char device_name[DEVICE_NAME_LEN + 1] = {0};
    char device_secret[DEVICE_SECRET_LEN + 1] = {0};

    HAL_GetProductKey(product_key);
    HAL_GetDeviceName(device_name);
    HAL_GetDeviceSecret(device_secret);

    if (0 != HAL_MDAL_MAL_Connect(product_key, device_name, device_secret)) {
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

int MALMQTTPublish(iotx_mc_client_t *c, const char *topicName, iotx_mqtt_topic_info_pt topic_msg)

{
    if (!c || !topicName || !topic_msg) {
        mal_err("MALMQTTPublish invalid parms\n");
        return FAIL_RETURN;
    }

    if (0 != HAL_MDAL_MAL_Publish(topicName, topic_msg->qos, topic_msg->payload)) {
        mal_err("MALMQTTPublish publish failed\n");
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

int MAL_MQTT_Publish_Simple(void *handle, const char *topic_name, int qos, void *data, int len)
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

    if (MAL_MQTT_Publish(client, topic_name, &mqtt_msg) < 0) {
        mal_err("IOT_MQTT_Publish failed\n");
        return -1;
    }

    return 0;
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
static int MALMQTTSubscribe(iotx_mc_client_t *c, const char *topicFilter, iotx_mqtt_qos_t qos, unsigned int msgId,
                            iotx_mqtt_event_handle_func_fpt messageHandler, void *pcontext, int timeout_ms)
{
    int status;

    if (!c || !topicFilter || !messageHandler) {
        return FAIL_RETURN;
    }
    iotx_mc_topic_handle_t *h = mal_malloc(sizeof(iotx_mc_topic_handle_t));
    if (h == NULL) {
        mal_err("maloc failed!");
        return FAIL_RETURN;
    }

    memset(h, 0, sizeof(iotx_mc_topic_handle_t));

    h->topic_filter = mal_malloc(strlen(topicFilter) + 1);
    if (NULL == h->topic_filter) {
        mal_free(h);
        return FAIL_RETURN;
    }
    memcpy((char *)h->topic_filter, topicFilter, strlen(topicFilter) + 1);

    h->handle.h_fp = messageHandler;
    h->handle.pcontext = pcontext;
    h->topic_type =  TOPIC_NAME_TYPE;

    HAL_MutexLock(c->lock_generic);
    h->next = c->first_sub_handle;
    c->first_sub_handle = h;
    HAL_MutexUnlock(c->lock_generic);
    if (HAL_MDAL_MAL_Subscribe(topicFilter, qos, &msgId, &status, timeout_ms) != 0) {
        return FAIL_RETURN;
    }
    return SUCCESS_RETURN;
}


/* MQTT send unsubscribe packet */
static int MALMQTTUnsubscribe(iotx_mc_client_t *c, const char *topicFilter, unsigned int msgId)
{
    int status;
    int ret;

    ret = HAL_MDAL_MAL_Unsubscribe(topicFilter, &msgId, &status);
    if (ret != 0) {
        return -1;
    }

    iotx_mc_topic_handle_t *h;
    for (h = c->first_sub_handle; h != NULL; h = h->next) {
        if (((strlen(topicFilter) == strlen(h->topic_filter))
             && (strcmp(topicFilter, (char *)h->topic_filter) == 0))
            || (mal_mc_is_topic_matched((char *)h->topic_filter, topicFilter))) {
            remove_handle_from_list(c, h);
        }
    }

    return 0;
}

/* MQTT send disconnect packet */
static int MALMQTTDisconnect(iotx_mc_client_t *c)
{
    return HAL_MDAL_MAL_Disconnect();
}

/* get next packet-id */
static int mal_mc_get_next_packetid(iotx_mc_client_t *c)
{
    unsigned int id = 0;

    if (!c) {
        return FAIL_RETURN;
    }

    HAL_MutexLock(c->lock_generic);
    c->packet_id = (c->packet_id == MAL_MC_PACKET_ID_MAX) ? 1 : c->packet_id + 1;
    id = c->packet_id;
    HAL_MutexUnlock(c->lock_generic);

    return id;
}

/* handle PUBLISH packet received from remote MQTT broker */
static int iotx_mc_handle_recv_PUBLISH(iotx_mc_client_t *c, char *topic, char *msg)
{
    iotx_mqtt_topic_info_t topic_msg = {0};
    int flag_matched = 0;

    if (!c || !topic || !msg) {
        return FAIL_RETURN;
    }
    mal_debug("recv pub topic=%s msg=%s", topic, msg);
    /* flowControl for specific topic */
    static uint64_t time_prev = 0;
    uint64_t time_curr = 0;
    char *filterStr = "{\"method\":\"thing.service.property.set\"";
    int filterLen = strlen(filterStr);

    if (0 == memcmp(msg, filterStr, filterLen)) {
        /* mal_debug("iotx_mc_handle_recv_PUBLISH match filterstring"); */
        time_curr = HAL_UptimeMs();
        if (time_curr < time_prev) {
            time_curr = time_prev;
        }
        if ((time_curr - time_prev) <= (uint64_t)50) {
            mal_debug("pub over threshould");
            return SUCCESS_RETURN;
        } else {
            time_prev = time_curr;
        }
    }

    /* we have to find the right message handler - indexed by topic */
    HAL_MutexLock(c->lock_generic);
    iotx_mc_topic_handle_t *h;
    for (h = c->first_sub_handle; h != NULL; h = h->next) {
        if (((strlen(topic) == strlen(h->topic_filter))
             && (strcmp(topic, (char *)h->topic_filter) == 0))
            || (mal_mc_is_topic_matched((char *)h->topic_filter, topic))) {
            mal_debug("pub topic is matched");

            iotx_mc_topic_handle_t *msg_handle = h;
            HAL_MutexUnlock(c->lock_generic);

            if (NULL != msg_handle->handle.h_fp) {
                iotx_mqtt_event_msg_t event_msg = {0};
                topic_msg.payload = msg;
                topic_msg.payload_len = strlen(msg);
                topic_msg.ptopic = topic;
                topic_msg.topic_len = strlen(topic);
                event_msg.event_type = IOTX_MQTT_EVENT_PUBLISH_RECEIVED;
                event_msg.msg = &topic_msg;
                msg_handle->handle.h_fp(msg_handle->handle.pcontext, c, &event_msg);
                flag_matched = 1;
            }

            HAL_MutexLock(c->lock_generic);
        }
    }

    HAL_MutexUnlock(c->lock_generic);

    if (0 == flag_matched) {
        mal_debug("NO matching any topic, call default handle function");

        if (NULL != c->handle_event.h_fp) {
            iotx_mqtt_event_msg_t event_msg = {0};

            topic_msg.payload = msg;
            topic_msg.payload_len = strlen(msg);
            topic_msg.ptopic = topic;
            topic_msg.topic_len = strlen(topic);
            event_msg.event_type = IOTX_MQTT_EVENT_PUBLISH_RECEIVED;
            event_msg.msg = &topic_msg;

            c->handle_event.h_fp(c->handle_event.pcontext, c, &event_msg);
        }
    }

    return SUCCESS_RETURN;
}

/* MQTT cycle to handle packet from remote broker */
static int mal_mc_cycle(iotx_mc_client_t *c, iotx_time_t *timer)
{
    int rc = SUCCESS_RETURN;
    char msg[MAL_MC_MAX_MSG_LEN] = {0};
    char topic[MAL_MC_MAX_MSG_LEN] = {0};

    if (!c) {
        return FAIL_RETURN;
    }

    if (HAL_MDAL_MAL_State() == IOTX_MC_STATE_CONNECTED) {
        mal_mc_set_client_state(c, IOTX_MC_STATE_CONNECTED);
    }

    if (mal_mc_get_client_state(c) != IOTX_MC_STATE_CONNECTED) {
        mal_err("mal state = %d error", mal_mc_get_client_state(c));
        return MQTT_STATE_ERROR;
    }

    if (HAL_MDAL_MAL_State() != IOTX_MC_STATE_CONNECTED) {
        mal_err("hal mal state = %d error", HAL_MDAL_MAL_State());
        mal_mc_set_client_state(c, IOTX_MC_STATE_DISCONNECTED);
        return MQTT_NETWORK_ERROR;
    }

    /* read the buf, see what work is due */
    rc = mal_mc_data_copy_from_buf(topic, msg);
    if (rc != SUCCESS_RETURN) {
        /* mal_debug("wait data timeout"); */
        return rc;
    }

    rc = iotx_mc_handle_recv_PUBLISH(c, topic, msg);
    if (SUCCESS_RETURN != rc) {
        mal_err("recvPublishProc error,result = %d", rc);
    }

    return rc;
}


/* check MQTT client is in normal state */
/* 0, in abnormal state; 1, in normal state */
static int mal_mc_check_state_normal(iotx_mc_client_t *c)
{
    if (!c) {
        return 0;
    }

    if (mal_mc_get_client_state(c) == IOTX_MC_STATE_CONNECTED) {
        return 1;
    }

    return 0;
}

/* subscribe */
int mal_mc_subscribe(iotx_mc_client_t *c,
                     const char *topicFilter,
                     iotx_mqtt_qos_t qos,
                     iotx_mqtt_event_handle_func_fpt topic_handle_func,
                     void *pcontext,
                     int timeout_ms)
{
    int rc = FAIL_RETURN;
    unsigned int msgId = mal_mc_get_next_packetid(c);

    if (NULL == c || NULL == topicFilter || !topic_handle_func) {
        return NULL_VALUE_ERROR;
    }


    if (!mal_mc_check_state_normal(c)) {
        mal_err("mqtt client state is error,state = %d", mal_mc_get_client_state(c));
        return MQTT_STATE_ERROR;
    }

    if (0 != mal_mc_check_topic(topicFilter, TOPIC_FILTER_TYPE)) {
        mal_err("topic format is error,topicFilter = %s", topicFilter);
        return MQTT_TOPIC_FORMAT_ERROR;
    }

    mal_debug("PERFORM subscribe to '%s' (msgId=%d)", topicFilter, msgId);
    rc = MALMQTTSubscribe(c, topicFilter, qos, msgId, topic_handle_func, pcontext, timeout_ms);
    if (rc != SUCCESS_RETURN) {
        if (rc == MQTT_NETWORK_ERROR) {
            mal_mc_set_client_state(c, IOTX_MC_STATE_DISCONNECTED);
        }

        mal_err("run MQTTSubscribe error, rc = %d", rc);
        return rc;
    }

    mal_info("mqtt subscribe packet sent,topic = %s!", topicFilter);
    return msgId;
}


/* unsubscribe */
int mal_mc_unsubscribe(iotx_mc_client_t *c, const char *topicFilter)
{
    int rc = FAIL_RETURN;
    unsigned int msgId = mal_mc_get_next_packetid(c);

    if (NULL == c || NULL == topicFilter) {
        return NULL_VALUE_ERROR;
    }

    if (0 != mal_mc_check_topic(topicFilter, TOPIC_FILTER_TYPE)) {
        mal_err("topic format is error,topicFilter = %s", topicFilter);
        return MQTT_TOPIC_FORMAT_ERROR;
    }

    if (!mal_mc_check_state_normal(c)) {
        mal_err("mqtt client state is error,state = %d", mal_mc_get_client_state(c));
        return MQTT_STATE_ERROR;
    }

    rc = MALMQTTUnsubscribe(c, topicFilter, msgId);
    if (rc != SUCCESS_RETURN) {
        if (rc == MQTT_NETWORK_ERROR) { /* send the subscribe packet */
            mal_mc_set_client_state(c, IOTX_MC_STATE_DISCONNECTED);
        }

        mal_err("run MALMQTTUnsubscribe error!");
        return rc;
    }

    mal_info("mqtt unsubscribe packet sent,topic = %s!", topicFilter);
    return (int)msgId;
}

/* publish */
int mal_mc_publish(iotx_mc_client_t *c, const char *topicName, iotx_mqtt_topic_info_pt topic_msg)
{
    uint16_t msg_id = 0;
    int rc = FAIL_RETURN;

    if (NULL == c || NULL == topicName || NULL == topic_msg) {
        return NULL_VALUE_ERROR;
    }

    if (0 != mal_mc_check_topic(topicName, TOPIC_NAME_TYPE)) {
        mal_err("topic format is error,topicFilter = %s", topicName);
        return MQTT_TOPIC_FORMAT_ERROR;
    }

    if (!mal_mc_check_state_normal(c)) {
        mal_err("mqtt client state is error,state = %d", mal_mc_get_client_state(c));
        return MQTT_STATE_ERROR;
    }

    if (topic_msg->qos == IOTX_MQTT_QOS1 || topic_msg->qos == IOTX_MQTT_QOS2) {
        msg_id = mal_mc_get_next_packetid(c);
        topic_msg->packet_id = msg_id;
    }

    if (topic_msg->qos == IOTX_MQTT_QOS2) {
        mal_err("MALMQTTPublish return error,MQTT_QOS2 is now not supported.");
        return MQTT_PUBLISH_QOS_ERROR;
    }

    rc = MALMQTTPublish(c, topicName, topic_msg);
    if (rc != SUCCESS_RETURN) { /* send the subscribe packet */
        if (rc == MQTT_NETWORK_ERROR) {
            mal_mc_set_client_state(c, IOTX_MC_STATE_DISCONNECTED);
        }
        mal_err("MALMQTTPublish is error, rc = %d", rc);
        return rc;
    }

    return (int)msg_id;
}


/* get state of MQTT client */
static iotx_mc_state_t mal_mc_get_client_state(iotx_mc_client_t *pClient)
{
    iotx_mc_state_t state;
    HAL_MutexLock(pClient->lock_generic);
    state = pClient->client_state;
    HAL_MutexUnlock(pClient->lock_generic);

    return state;
}


/* set state of MQTT client */
static void mal_mc_set_client_state(iotx_mc_client_t *pClient, iotx_mc_state_t newState)
{

    HAL_MutexLock(pClient->lock_generic);
    pClient->client_state = newState;
    HAL_MutexUnlock(pClient->lock_generic);
}


int mal_mc_recv_buf_init()
{
    g_at_mqtt_buff_mgr.read_index  = 0;
    g_at_mqtt_buff_mgr.write_index = 0;
    g_at_mqtt_buff_mgr.last_write_index = 0;
    g_at_mqtt_buff_mgr.buffer_num = MAL_MC_DEFAULT_BUFFER_NUM;

    int i;
    for (i = 0; i < MAL_MC_MAX_BUFFER_NUM; i++) {
        g_at_mqtt_buff_mgr.valid_flag[i] = 0;
        if (i < MAL_MC_DEFAULT_BUFFER_NUM) {
            g_at_mqtt_buff_mgr.topic[i] = g_at_mqtt_topic[i];
            g_at_mqtt_buff_mgr.msg_data[i] = g_at_mqtt_msg_data[i];
            memset(g_at_mqtt_buff_mgr.topic[i], 0, MAL_MC_MAX_TOPIC_LEN);
            memset(g_at_mqtt_buff_mgr.msg_data[i], 0, MAL_MC_MAX_MSG_LEN);
        } else {
            g_at_mqtt_buff_mgr.topic[i] = NULL;
            g_at_mqtt_buff_mgr.msg_data[i] = NULL;
        }
    }

    if (NULL == (g_at_mqtt_buff_mgr.buffer_mutex = HAL_MutexCreate())) {
        mal_err("create buffer mutex error");
        return -1;
    }

    return 0;
}

void mal_mc_recv_buf_deinit()
{
    g_at_mqtt_buff_mgr.read_index  = 0;
    g_at_mqtt_buff_mgr.write_index = 0;
    g_at_mqtt_buff_mgr.last_write_index = 0;

    int i;
    for (i = 0; i < MAL_MC_MAX_BUFFER_NUM; i++) {
        g_at_mqtt_buff_mgr.valid_flag[i] = 0;
        if (i < MAL_MC_DEFAULT_BUFFER_NUM) {
            memset(g_at_mqtt_buff_mgr.topic[i], 0, MAL_MC_MAX_TOPIC_LEN);
            memset(g_at_mqtt_buff_mgr.msg_data[i], 0, MAL_MC_MAX_MSG_LEN);
        } else {
            if (i < g_at_mqtt_buff_mgr.buffer_num) {
                if (g_at_mqtt_buff_mgr.topic[i] != NULL) {
                    mal_free(g_at_mqtt_buff_mgr.topic[i]);
                }
                if (g_at_mqtt_buff_mgr.msg_data[i] != NULL) {
                    mal_free(g_at_mqtt_buff_mgr.msg_data[i]);
                }
            }
        }
    }

    HAL_MutexDestroy(g_at_mqtt_buff_mgr.buffer_mutex);
}

int mal_mc_wait_for_result()
{
    iotx_time_t         time;
    int state = 0;
    int timeout_ms = MAL_MC_DEAFULT_TIMEOUT;
    iotx_time_init(&time);
    utils_time_countdown_ms(&time, timeout_ms);
    do {
        unsigned int left_t;
        left_t = iotx_time_left(&time);
        if (left_t < 100) {
            HAL_SleepMs(left_t);
        } else {
            HAL_SleepMs(100);
        }

        state = HAL_MDAL_MAL_State();
    } while (!utils_time_is_expired(&time) && (state != IOTX_MC_STATE_CONNECTED));

    if (state == IOTX_MC_STATE_CONNECTED) {
        return SUCCESS_RETURN;
    } else {
        return FAIL_RETURN;
    }
}
/* connect */
int mal_mc_connect(iotx_mc_client_t *pClient)
{
    int rc = FAIL_RETURN;

    if (NULL == pClient) {
        return NULL_VALUE_ERROR;
    }

    rc = MALMQTTConnect(pClient);
    if (rc  != SUCCESS_RETURN) {
        mal_err("send connect packet failed");
        return  rc;
    }
    if (SUCCESS_RETURN != mal_mc_wait_for_result()) {
        mal_err("current state is not connected");
        return FAIL_RETURN;
    }

    mal_mc_set_client_state(pClient, IOTX_MC_STATE_CONNECTED);

    mal_info("mqtt connect success!");
    return SUCCESS_RETURN;
}

int mal_mc_disconnect(iotx_mc_client_t *pClient)
{
    int             rc = -1;

    if (NULL == pClient) {
        return NULL_VALUE_ERROR;
    }

    if (mal_mc_check_state_normal(pClient)) {
        rc = MALMQTTDisconnect(pClient);
        mal_debug("rc = MALMQTTDisconnect() = %d", rc);
    }

    mal_mc_set_client_state(pClient, IOTX_MC_STATE_INITIALIZED);

    mal_info("mqtt disconnect!");
    return SUCCESS_RETURN;
}

/* release MQTT resource */
static int mal_mc_release(iotx_mc_client_t *pClient)
{
    if (NULL == pClient) {
        return NULL_VALUE_ERROR;
    }

    /* iotx_delete_thread(pClient); */
    HAL_SleepMs(100);

    mal_mc_disconnect(pClient);
    mal_mc_set_client_state(pClient, IOTX_MC_STATE_INVALID);
    HAL_SleepMs(100);
    if (pClient->first_sub_handle != NULL) {
        iotx_mc_topic_handle_t *handler = pClient->first_sub_handle;
        iotx_mc_topic_handle_t *next_handler = pClient->first_sub_handle;
        while (handler) {
            next_handler = handler->next;
            if (handler->topic_filter != NULL) {
                mal_free(handler->topic_filter);
                handler->topic_filter = NULL;
            }
            mal_free(handler);
            handler = next_handler;
        }
    }
    HAL_MutexDestroy(pClient->lock_generic);
    HAL_MutexDestroy(pClient->lock_list_sub);

    mal_mc_recv_buf_deinit();

    mal_info("mqtt release!");
    return SUCCESS_RETURN;
}

#ifndef MAL_ICA_ENABLED

#define GUIDER_DEFAULT_TS_STR       "2524608000000"
static void guider_get_timestamp_str(char *buf, int len)
{
    HAL_Snprintf(buf, len, "%s", GUIDER_DEFAULT_TS_STR);

    return;
}

static int _calc_hmac_signature(
            char *product_key,
            char *device_name,
            char *device_secret,
            char *device_id,
            char *hmac_sigbuf,
            const int hmac_buflen,
            const char *timestamp_str)
{
    char                    signature[64];
    char                    hmac_source[512];
    int                     rc = -1;

    memset(signature, 0, sizeof(signature));
    memset(hmac_source, 0, sizeof(hmac_source));
    rc = HAL_Snprintf(hmac_source,
                      sizeof(hmac_source),
                      "clientId%s" "deviceName%s" "productKey%s" "timestamp%s",
                      device_id,
                      device_name,
                      product_key,
                      timestamp_str);
    LITE_ASSERT(rc < sizeof(hmac_source));

    utils_hmac_sha1(hmac_source, strlen(hmac_source),
                    signature,
                    device_secret,
                    strlen(device_secret));

    memcpy(hmac_sigbuf, signature, hmac_buflen);
    return 0;
}
#endif /* MAL_ICA_ENABLED */

int iotx_mdal_get_auth_username_passwd(char *out_username, char *out_password)
{
#ifndef MAL_ICA_ENABLED
    char                guider_sign[GUIDER_SIGN_LEN] = {0};
    char                timestamp_str[GUIDER_TS_LEN] = {0};

    char product_key[PRODUCT_KEY_LEN + 1] = {0};
    char device_name[DEVICE_NAME_LEN + 1] = {0};
    char device_id[DEVICE_ID_LEN + 1] = {0};
    char device_secret[DEVICE_SECRET_LEN + 1] = {0};

    HAL_GetProductKey(product_key);
    HAL_GetDeviceName(device_name);
    HAL_GetDeviceSecret(device_secret);
    HAL_Snprintf(device_id, DEVICE_ID_LEN, "%s.%s", product_key, device_name);
    device_id[DEVICE_ID_LEN-1] = '\0';
    if ((out_username == NULL) || (out_password == NULL)) {
        mal_err("username or passwd is null");
        return -1;
    }

    guider_get_timestamp_str(timestamp_str, sizeof(timestamp_str));

    /* calculate the sign */
    _calc_hmac_signature(product_key, device_name, device_secret, device_id, guider_sign,
                         sizeof(guider_sign), timestamp_str);

    /* fill up username and password */
    _fill_conn_string(out_username, 64,
                      "%s&%s",
                      device_name,
                      product_key);
    _fill_conn_string(out_password, 64,
                      "%s",
                      guider_sign);
    return 0;
#else
    mal_err("Get username and passwd feature is not supported, when MAL_ICA_ENABLED is y");
    return -1;
#endif
}

int mal_mc_data_copy_to_buf(char *topic, char *message)
{
    uint8_t       write_index;
    char         *copy_ptr;

    if ((topic == NULL) || (message == NULL)) {
        mal_err("write buffer is NULL");
        return -1;
    }

    if ((strlen(topic) >= MAL_MC_MAX_TOPIC_LEN) ||
        (strlen(message) >= MAL_MC_MAX_MSG_LEN)) {
        mal_err("topic(%d) or message(%d) too large", strlen(topic), strlen(message));
        return -1;
    }

    HAL_MutexLock(g_at_mqtt_buff_mgr.buffer_mutex);
    write_index     = g_at_mqtt_buff_mgr.write_index;

    if ((g_at_mqtt_buff_mgr.valid_flag[write_index])
        && (g_at_mqtt_buff_mgr.buffer_num == MAL_MC_MAX_BUFFER_NUM)) {
        mal_err("buffer is full");

        HAL_MutexUnlock(g_at_mqtt_buff_mgr.buffer_mutex);
        return -1;
    }
    if (g_at_mqtt_buff_mgr.valid_flag[write_index]) {
        int last_write_index = write_index;
        g_at_mqtt_buff_mgr.last_write_index = last_write_index;
        write_index = g_at_mqtt_buff_mgr.buffer_num;
        mal_err("increase buffer to %d", g_at_mqtt_buff_mgr.buffer_num);
        g_at_mqtt_buff_mgr.topic[write_index] = mal_malloc(MAL_MC_MAX_TOPIC_LEN);
        if (g_at_mqtt_buff_mgr.topic[write_index] == NULL) {
            mal_err("increase buffer failed, drop it");
            return -1;
        }
        g_at_mqtt_buff_mgr.msg_data[write_index] = mal_malloc(MAL_MC_MAX_MSG_LEN);
        if (g_at_mqtt_buff_mgr.msg_data[write_index] == NULL) {
            mal_err("increase buffer failed, drop it");
            mal_free(g_at_mqtt_buff_mgr.topic[write_index]);
            return -1;
        }
        g_at_mqtt_buff_mgr.buffer_num ++;
    } else {
        g_at_mqtt_buff_mgr.last_write_index = 0;
    }

    copy_ptr = g_at_mqtt_buff_mgr.topic[write_index];
    memcpy(copy_ptr, topic, strlen(topic));
    copy_ptr = g_at_mqtt_buff_mgr.msg_data[write_index];
    memcpy(copy_ptr, message, strlen(message));

    g_at_mqtt_buff_mgr.valid_flag[write_index] = 1;
    write_index++;

    if (write_index >= g_at_mqtt_buff_mgr.buffer_num) {
        write_index = 0;
    }

    if (g_at_mqtt_buff_mgr.last_write_index != 0) {
        g_at_mqtt_buff_mgr.write_index = g_at_mqtt_buff_mgr.last_write_index;
    } else {
        g_at_mqtt_buff_mgr.write_index  = write_index;
    }
    HAL_MutexUnlock(g_at_mqtt_buff_mgr.buffer_mutex);

    return 0;
}

int mal_mc_data_copy_from_buf(char *topic, char *message)
{
    uint8_t         read_index;
    char           *copy_ptr;

    if ((topic == NULL) || (message == NULL)) {
        mal_err("read buffer is NULL");
        return -1;
    }

    HAL_MutexLock(g_at_mqtt_buff_mgr.buffer_mutex);
    read_index = g_at_mqtt_buff_mgr.read_index;

    if (g_at_mqtt_buff_mgr.valid_flag[read_index] == 0) {
        HAL_MutexUnlock(g_at_mqtt_buff_mgr.buffer_mutex);
        return -1;
    }

    copy_ptr = g_at_mqtt_buff_mgr.topic[read_index];
    memcpy(topic, copy_ptr, strlen(copy_ptr));
    copy_ptr = g_at_mqtt_buff_mgr.msg_data[read_index];
    memcpy(message, copy_ptr, strlen(copy_ptr));

    memset(g_at_mqtt_buff_mgr.topic[read_index], 0, MAL_MC_MAX_TOPIC_LEN);
    memset(g_at_mqtt_buff_mgr.msg_data[read_index], 0, MAL_MC_MAX_MSG_LEN);
    g_at_mqtt_buff_mgr.valid_flag[read_index] = 0;
    read_index++;

    if (read_index >= g_at_mqtt_buff_mgr.buffer_num) {

        read_index = 0;

    }

    g_at_mqtt_buff_mgr.read_index = read_index;

    HAL_MutexUnlock(g_at_mqtt_buff_mgr.buffer_mutex);

    return 0;

}

static struct list_head g_mqtt_sub_list = LIST_HEAD_INIT(g_mqtt_sub_list);

static int yielding = 0;
static inline void _yield_lock(iotx_mc_client_t *pClient)
{
    yielding = 1;
    HAL_MutexLock(pClient->lock_yield);
}
static inline void _yield_unlock(iotx_mc_client_t *pClient)
{
    HAL_MutexUnlock(pClient->lock_yield);
    yielding = 0;
}

static inline int _get_yield_lock_state()
{
    return yielding;
}

/************************  Public Interface ************************/
void *MAL_MQTT_Construct(iotx_mqtt_param_t *pInitParams)
{
    int                 err;
    iotx_mc_client_t   *pclient;

    pclient = (iotx_mc_client_t *)mal_malloc(sizeof(iotx_mc_client_t));
    if (NULL == pclient) {
        mal_err("not enough memory.");
        return NULL;
    }

    memset(pclient, 0, sizeof(iotx_mc_client_t));

    if (pInitParams != NULL) {
        pclient->handle_event.h_fp = pInitParams->handle_event.h_fp;
        pclient->handle_event.pcontext = pInitParams->handle_event.pcontext;
    }

    mal_mc_recv_buf_init();
    HAL_MDAL_MAL_RegRecvCb(mal_mc_data_copy_to_buf);

    mal_mc_set_client_state(pclient, IOTX_MC_STATE_INITIALIZED);
    err = mal_mc_connect(pclient);
    if (SUCCESS_RETURN != err) {
        mal_mc_release(pclient);
        mal_free(pclient);
        return NULL;
    }

#if 1
    iotx_set_report_func(MAL_MQTT_Publish_Simple);

    err = iotx_report_mid(pclient);
    if (SUCCESS_RETURN != err) {
        mal_mc_release(pclient);
        mal_free(pclient);
        return NULL;
    }

    /* report device info */
    err = iotx_report_devinfo(pclient);
    if (SUCCESS_RETURN != err) {
        mal_mc_release(pclient);
        mal_free(pclient);
        return NULL;
    }

    err = iotx_report_firmware_version(pclient);
    if (SUCCESS_RETURN != err) {
        mal_mc_release(pclient);
        mal_free(pclient);
        return NULL;
    }
#endif
    g_mqtt_client = pclient;
    return pclient;
}

int MAL_MQTT_Destroy(void **phandler)
{
    void *client;
    if (phandler != NULL) {
        client = *phandler;
        *phandler = NULL;
    } else {
        client = g_mqtt_client;
    }

    POINTER_SANITY_CHECK(client, NULL_VALUE_ERROR);

    mal_mc_release((iotx_mc_client_t *)client);
    mal_free(client);
    g_mqtt_client = NULL;

    return SUCCESS_RETURN;
}


int MAL_MQTT_Yield(void *handle, int timeout_ms)
{
    int                 rc = SUCCESS_RETURN;
    iotx_time_t         time;
    iotx_mc_client_t *pClient = (iotx_mc_client_t *)(handle ? handle : g_mqtt_client);

    POINTER_SANITY_CHECK(pClient, NULL_VALUE_ERROR);
    if (timeout_ms < 0) {
        mal_err("Invalid argument, timeout_ms = %d", timeout_ms);
        return -1;
    }
    if (timeout_ms == 0) {
        timeout_ms = 10;
    }

    iotx_time_init(&time);
    utils_time_countdown_ms(&time, timeout_ms);

    do {
        if (SUCCESS_RETURN != rc) {
            unsigned int left_t = iotx_time_left(&time);
            /*mal_info("error occur or no data");*/
            if (left_t < 20) {
                HAL_SleepMs(left_t);
            } else {
                HAL_SleepMs(20);
            }
        }
        _yield_lock(pClient);

        /* acquire package in cycle, such as PUBLISH */
        rc = mal_mc_cycle(pClient, &time);
        _yield_unlock(pClient);
    } while (!utils_time_is_expired(&time));

    return 0;
}

/* check whether MQTT connection is established or not */
int MAL_MQTT_CheckStateNormal(void *handle)
{
    POINTER_SANITY_CHECK(handle, NULL_VALUE_ERROR);
    return mal_mc_check_state_normal((iotx_mc_client_t *)handle);
}

int MAL_MQTT_Subscribe(void *handle,
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
        mal_warning("Invalid qos(%d) out of [%d, %d], using %d",
                    qos,
                    IOTX_MQTT_QOS0, IOTX_MQTT_QOS2, IOTX_MQTT_QOS0);
        qos = IOTX_MQTT_QOS0;
    }
    return mal_mc_subscribe(client, topic_filter, qos, topic_handle_func, pcontext, MAL_TIMEOUT_FOREVER);
}

int MAL_MQTT_Subscribe_Sync(void *handle,
                            const char *topic_filter,
                            iotx_mqtt_qos_t qos,
                            iotx_mqtt_event_handle_func_fpt topic_handle_func,
                            void *pcontext,
                            int timeout_ms)
{
    iotx_mc_client_t *client = (iotx_mc_client_t *)(handle ? handle : g_mqtt_client);

    POINTER_SANITY_CHECK(client, NULL_VALUE_ERROR);
    POINTER_SANITY_CHECK(topic_handle_func, NULL_VALUE_ERROR);
    STRING_PTR_SANITY_CHECK(topic_filter, NULL_VALUE_ERROR);

    if (qos > IOTX_MQTT_QOS2) {
        mal_warning("Invalid qos(%d) out of [%d, %d], using %d",
                    qos,
                    IOTX_MQTT_QOS0, IOTX_MQTT_QOS2, IOTX_MQTT_QOS0);
        qos = IOTX_MQTT_QOS0;
    }
    return mal_mc_subscribe(client, topic_filter, qos, topic_handle_func, pcontext, timeout_ms);
}

int MAL_MQTT_Unsubscribe(void *handle, const char *topic_filter)
{
    iotx_mc_client_t *client = (iotx_mc_client_t *)(handle ? handle : g_mqtt_client);

    POINTER_SANITY_CHECK(client, NULL_VALUE_ERROR);
    STRING_PTR_SANITY_CHECK(topic_filter, NULL_VALUE_ERROR);

    return mal_mc_unsubscribe(client, topic_filter);
}

int MAL_MQTT_Publish(void *handle, const char *topic_name, iotx_mqtt_topic_info_pt topic_msg)
{
    iotx_mc_client_t *client = (iotx_mc_client_t *)(handle ? handle : g_mqtt_client);

    POINTER_SANITY_CHECK(client, NULL_VALUE_ERROR);
    STRING_PTR_SANITY_CHECK(topic_name, NULL_VALUE_ERROR);

    return mal_mc_publish(client, topic_name, topic_msg);
}

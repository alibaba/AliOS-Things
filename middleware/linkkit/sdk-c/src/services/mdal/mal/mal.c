/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <stddef.h>
#include "iot_export.h"
#include "iotx_system.h"
#include "iotx_utils.h"
#include "utils_sysinfo.h"

#include "mdal_at_mqtt_client.h"
#include "mal.h"

#define MQTT_DEFAULT_MSG_LEN 1280
#define MAL_TIMEOUT_FOREVER -1
#define MAL_MC_PACKET_ID_MAX (65535)
#define MAL_MC_TOPIC_NAME_MAX_LEN (128)

#define mal_emerg(...)             log_emerg("MAL", __VA_ARGS__)
#define mal_crit(...)              log_crit("MAL", __VA_ARGS__)
#define mal_err(...)               log_err("MAL", __VA_ARGS__)
#define mal_warning(...)           log_warning("MAL", __VA_ARGS__)
#define mal_info(...)              log_info("MAL", __VA_ARGS__)
#define mal_debug(...)             log_debug("MAL", __VA_ARGS__)

#define mal_malloc(...)            LITE_malloc(__VA_ARGS__, MEM_MAGIC, "mal")
#define mal_free                   LITE_free

static int mal_mc_check_state_normal(iotx_mc_client_t *c);

static iotx_mc_state_t mal_mc_get_client_state(iotx_mc_client_t *pClient);
static void mal_mc_set_client_state(iotx_mc_client_t *pClient, iotx_mc_state_t newState);

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
        mal_err("len of topicName exceeds 64");
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

    HAL_AT_MQTT_Connect(product_key, device_name, device_secret, 0);
    return 0;
}

int MALMQTTPublish(iotx_mc_client_t *c, const char *topicName, iotx_mqtt_topic_info_pt topic_msg)

{
    if (!c || !topicName || !topic_msg) {
        return FAIL_RETURN;
    }

    if (0 != HAL_AT_MQTT_Publish(topicName, topic_msg->qos, topic_msg->payload)) {
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
    if (HAL_AT_MQTT_Subscribe(topicFilter, qos, &msgId, &status, timeout_ms) != 0) {
        return FAIL_RETURN;
    }
    return SUCCESS_RETURN;
}


/* MQTT send unsubscribe packet */
static int MALMQTTUnsubscribe(iotx_mc_client_t *c, const char *topicFilter, unsigned int msgId)
{
    int status;
    return HAL_AT_MQTT_Unsubscribe(topicFilter, msgId, &status);
}


/* MQTT send disconnect packet */
static int MALMQTTDisconnect(iotx_mc_client_t *c)
{
    return HAL_AT_MQTT_Disconnect();
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
    iotx_mqtt_topic_info_t topic_msg;
    int flag_matched = 0;

    if (!c) {
        return FAIL_RETURN;
    }

    char *filterStr = "{\"method\":\"thing.service.property.set\"";
    int filterLen = strlen(filterStr);
    if (0 == memcmp(msg, filterStr, filterLen)) {
        return SUCCESS_RETURN;
    }

    /* we have to find the right message handler - indexed by topic */
    HAL_MutexLock(c->lock_generic);
    iotx_mc_topic_handle_t *h;
    for (h = c->first_sub_handle; h != NULL; h = h->next) {
        if ((strlen(topic) == strlen(h->topic_filter)) && (strcmp(topic, (char *)h->topic_filter) == 0)) {
            mal_debug("topic be matched");

            iotx_mc_topic_handle_t *msg_handle = h;
            HAL_MutexUnlock(c->lock_generic);

            if (NULL != msg_handle->handle.h_fp) {
                iotx_mqtt_event_msg_t event_msg;
                topic_msg.payload = msg;
                topic_msg.ptopic = topic;
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
            iotx_mqtt_event_msg_t msg;

            msg.event_type = IOTX_MQTT_EVENT_PUBLISH_RECEIVED;
            msg.msg = &topic_msg;

            c->handle_event.h_fp(c->handle_event.pcontext, c, &msg);
        }
    }

    return SUCCESS_RETURN;
}

#define AT_MQTT_MAX_MSG_LEN 1200
/* MQTT cycle to handle packet from remote broker */
static int mal_mc_cycle(iotx_mc_client_t *c, iotx_time_t *timer)
{
    int rc = SUCCESS_RETURN;
    char msg[AT_MQTT_MAX_MSG_LEN] = {0};
    char topic[AT_MQTT_MAX_MSG_LEN] = {0};

    if (!c) {
        return FAIL_RETURN;
    }

    iotx_mc_state_t state = mal_mc_get_client_state(c);
    if (state != IOTX_MC_STATE_CONNECTED) {
        return MQTT_STATE_ERROR;
    }

#if 0
    if ((state = HAL_AT_MQTT_State()) != IOTX_MC_STATE_CONNECTED) {
        mal_mc_set_client_state(c, state);
        return MQTT_STATE_ERROR;
    }
#endif

    /* read the socket, see what work is due */
    rc = HAL_AT_MQTT_Readmsg(&topic, &msg);
    if (rc != SUCCESS_RETURN) {
        /* mal_debug("wait data timeout"); */
        return SUCCESS_RETURN;
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


/* Initialize MQTT client */
int mal_mc_init(iotx_mc_client_t *pClient, iotx_mqtt_param_t *pInitParams)
{
    int rc = FAIL_RETURN;
    iotx_mc_state_t mc_state = IOTX_MC_STATE_INVALID;

    if ((NULL == pClient) || (NULL == pInitParams)) {
        return NULL_VALUE_ERROR;
    }

    memset(pClient, 0x0, sizeof(iotx_mc_client_t));

    pClient->lock_generic = HAL_MutexCreate();
    if (!pClient->lock_generic) {
        return FAIL_RETURN;
    }

    pClient->lock_list_sub = HAL_MutexCreate();
    if (!pClient->lock_list_sub) {
        goto RETURN;
    }
    pClient->lock_yield = HAL_MutexCreate();
    if (!pClient->lock_yield) {
        goto RETURN;
    }

    mc_state = IOTX_MC_STATE_INITIALIZED;
    rc = SUCCESS_RETURN;
    mal_info("MQTT init success!");

RETURN :
    mal_mc_set_client_state(pClient, mc_state);
    if (rc != SUCCESS_RETURN) {
        if (pClient->lock_list_sub) {
            HAL_MutexDestroy(pClient->lock_list_sub);
            pClient->lock_list_sub = NULL;
        }
        if (pClient->lock_yield) {
            HAL_MutexDestroy(pClient->lock_yield);
            pClient->lock_yield = NULL;
        }
    }

    return rc;
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

    mal_info("mqtt release!");
    return SUCCESS_RETURN;
}

/* AOS activation data report */
// aos will implement this function
#ifndef BUILD_AOS
unsigned int aos_get_version_info(unsigned char version_num[VERSION_NUM_SIZE],
                                  unsigned char random_num[RANDOM_NUM_SIZE], unsigned char mac_address[MAC_ADDRESS_SIZE],
                                  unsigned char chip_code[CHIP_CODE_SIZE], unsigned char *output_buffer, unsigned int output_buffer_size)
{
    char *p = (char *)output_buffer;

    if (output_buffer_size < AOS_ACTIVE_INFO_LEN) {
        return 1;
    }

    memset(p, 0, output_buffer_size);

    LITE_hexbuf_convert(version_num, p, VERSION_NUM_SIZE, 1);
    p += VERSION_NUM_SIZE * 2;
    LITE_hexbuf_convert(random_num, p, RANDOM_NUM_SIZE, 1);
    p += RANDOM_NUM_SIZE * 2;
    LITE_hexbuf_convert(mac_address, p, MAC_ADDRESS_SIZE, 1);
    p += MAC_ADDRESS_SIZE * 2;
    LITE_hexbuf_convert(chip_code, p, CHIP_CODE_SIZE, 1);
    p += CHIP_CODE_SIZE * 2;
    strcat(p, "1111111111222222222233333333334444444444");

    return 0;
}
#endif

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
/* Report AOS Version */
const char *MAL_DEVICE_INFO_UPDATE_FMT = "{\"id\":\"2\",\"version\":\"1.0\",\"params\":["
        "{\"attrKey\":\"SYS_ALIOS_ACTIVATION\",\"attrValue\":\"%s\",\"domain\":\"SYSTEM\"},"
        "{\"attrKey\":\"SYS_LP_SDK_VERSION\",\"attrValue\":\"%s\",\"domain\":\"SYSTEM\"},"
        "{\"attrKey\":\"SYS_SDK_LANGUAGE\",\"attrValue\":\"C\",\"domain\":\"SYSTEM\"},"
        "{\"attrKey\":\"SYS_SDK_IF_INFO\",\"attrValue\":\"%s\",\"domain\":\"SYSTEM\"}"
        "],\"method\":\"thing.deviceinfo.update\"}";
static int mal_mc_report_devinfo(iotx_mc_client_t *pclient)
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
    char product_key[PRODUCT_KEY_LEN + 1] = {0};
    char device_name[DEVICE_NAME_LEN + 1] = {0};

    mal_debug("devinfo report in MQTT");

    // Get AOS kernel version: AOS-R-1.3.0, transform to hex format
    aos_get_version_hex((unsigned char *)version);

    // Get Mac address
    aos_get_mac_hex((unsigned char *)mac);

    mac[6] = ACTIVE_SINGLE_GW;
#ifdef BUILD_AOS
    mac[7] = ACTIVE_LINKKIT_AOS;
#else
    mac[7] = ACTIVE_LINKKIT_ONLY;
#endif
    // Get Random
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
    show_mm();
    // Get aos active info
    ret = aos_get_version_info((unsigned char *)version, (unsigned char *)random_num, (unsigned char *)mac,
                               (unsigned char *)chip_code, (unsigned char *)output, AOS_ACTIVE_INFO_LEN);
    if (ret) {
        mal_err("aos_get_version_info failed");
        return FAIL_RETURN;
    }
    mal_debug("get aos avtive info: %s", output);

    HAL_GetProductKey(product_key);
    HAL_GetDeviceName(device_name);

    /* linkkit version topic name */
    ret = HAL_Snprintf(topic_name,
                       IOTX_URI_MAX_LEN,
                       "/sys/%s/%s/thing/deviceinfo/update",
                       product_key,
                       device_name);
    if (ret <= 0) {
        mal_err("topic generate err");
        return FAIL_RETURN;
    }
    mal_debug("devinfo report topic: %s", topic_name);

    ret = HAL_GetNetifInfo(network_interfaces);
    if (ret <= 0 || ret >= NIF_STRLEN_MAX) {
        mal_err("the network interface info set failed or not set, writen len is %d", ret);
        const char *default_network_info = "invalid network interface info";
        strncpy(network_interfaces, default_network_info, strlen(default_network_info));
    }
    msg_len = strlen(MAL_DEVICE_INFO_UPDATE_FMT) + strlen(LINKKIT_VERSION) + AOS_ACTIVE_INFO_LEN + \
              + strlen(network_interfaces) + 1;
    msg = (char *)mal_malloc(msg_len);
    if (msg == NULL) {
        mal_err("malloc err");
        return FAIL_RETURN;
    }
    memset(msg, 0, msg_len);
    /* generate linkkit version json data */
    ret = HAL_Snprintf(msg,
                       msg_len,
                       MAL_DEVICE_INFO_UPDATE_FMT,
                       output,
                       LINKKIT_VERSION,
                       network_interfaces
                      );
    if (ret <= 0) {
        mal_err("topic msg generate err");
        mal_free(msg);
        return FAIL_RETURN;
    }
    mal_debug("devinfo report data: %s", msg);

    topic_info.qos = IOTX_MQTT_QOS1;
    topic_info.payload = (void *)msg;
    topic_info.payload_len = strlen(msg);
    topic_info.retain = 0;
    topic_info.dup = 0;

    // publish message
    ret = mal_mc_publish(pclient, topic_name, &topic_info);
    mal_free(msg);
    if (ret < 0) {
        mal_err("publish failed");
        return FAIL_RETURN;
    }
    mal_debug("devinfo report succeed");

    return SUCCESS_RETURN;
}

/* report Firmware version */
static int mal_mc_report_firmware_version(iotx_mc_client_t *pclient)
{
#if defined(BUILD_AOS)
    return SUCCESS_RETURN;
#else
    int ret;
    char topic_name[IOTX_URI_MAX_LEN + 1] = {0};
    char msg[FIRMWARE_VERSION_MSG_LEN] = {0};
    iotx_mqtt_topic_info_t topic_info;
    char product_key[PRODUCT_KEY_LEN + 1] = {0};
    char device_name[DEVICE_NAME_LEN + 1] = {0};
    char version[FIRMWARE_VERSION_MAXLEN] = {0};

    ret = HAL_GetFirmwareVesion(version);
    if (ret <= 0) {
        mal_err("firmware version does not implement");
        return FAIL_RETURN;
    }

    mal_debug("firmware version report start in MQTT");

    HAL_GetProductKey(product_key);
    HAL_GetDeviceName(device_name);

    /* firmware report topic name generate */
    ret = HAL_Snprintf(topic_name,
                       IOTX_URI_MAX_LEN,
                       "/ota/device/inform/%s/%s",
                       product_key,
                       device_name
                      );
    if (ret <= 0) {
        mal_err("firmware report topic generate err");
        return FAIL_RETURN;
    }
    mal_debug("firmware report topic: %s", topic_name);

    /* firmware report message json data generate */
    ret = HAL_Snprintf(msg,
                       FIRMWARE_VERSION_MSG_LEN,
                       "{\"id\":\"%d\",\"params\":{\"version\":\"%s\"}}",
                       3,
                       version
                      );
    if (ret <= 0) {
        mal_err("firmware report message json data generate err");
        return FAIL_RETURN;
    }
    mal_debug("firmware report data: %s", msg);

    topic_info.qos = IOTX_MQTT_QOS1;
    topic_info.payload = (void *)msg;
    topic_info.payload_len = strlen(msg);
    topic_info.retain = 0;
    topic_info.dup = 0;

    // publish message
    ret = mal_mc_publish(pclient, topic_name, &topic_info);
    if (ret < 0) {
        mal_err("publish failed");
        return FAIL_RETURN;
    }

    mal_debug("firmware version report finished, mal_mc_publish() = %d", ret);
    return SUCCESS_RETURN;
#endif
}

/* report ModuleID */
static int mal_mc_report_mid(iotx_mc_client_t *pclient)
{
    int                         ret;
    char                        topic_name[IOTX_URI_MAX_LEN + 1];
    iotx_mqtt_topic_info_t      topic_info;
    char                        requestId[MIDREPORT_REQID_LEN + 1] = {0};
    char product_key[PRODUCT_KEY_LEN + 1] = {0};
    char device_name[DEVICE_NAME_LEN + 1] = {0};
    char                        pid[PID_STRLEN_MAX + 1] = {0};
    char                        mid[MID_STRLEN_MAX + 1] = {0};

    memset(pid, 0, sizeof(pid));
    memset(mid, 0, sizeof(mid));

    if (0 == HAL_GetPartnerID(pid)) {
        mal_debug("PartnerID is Null");
        return SUCCESS_RETURN;
    }
    if (0 == HAL_GetModuleID(mid)) {
        mal_debug("ModuleID is Null");
        return SUCCESS_RETURN;
    }

    mal_debug("MID Report: started in MQTT");

    HAL_GetProductKey(product_key);
    HAL_GetDeviceName(device_name);

    iotx_midreport_reqid(requestId,
                         product_key,
                         device_name);
    /* 1,generate json data */
    char *msg = mal_malloc(MIDREPORT_PAYLOAD_LEN);
    if (NULL == msg) {
        mal_err("allocate mem failed");
        return FAIL_RETURN;
    }

    iotx_midreport_payload(msg,
                           requestId,
                           mid,
                           pid);

    mal_debug("MID Report: json data = '%s'", msg);

    memset(&topic_info, 0, sizeof(iotx_mqtt_topic_info_t));

    topic_info.qos = IOTX_MQTT_QOS0;
    topic_info.payload = (void *)msg;
    topic_info.payload_len = strlen(msg);
    topic_info.retain = 0;
    topic_info.dup = 0;

    /* 2,generate topic name */
    ret = iotx_midreport_topic(topic_name,
                               "",
                               product_key,
                               device_name);

    mal_debug("MID Report: topic name = '%s'", topic_name);

    if (ret < 0) {
        mal_err("generate topic name of info failed");
        mal_free(msg);
        return FAIL_RETURN;
    }

    ret = MAL_MQTT_Publish(pclient, topic_name, &topic_info);
    if (ret < 0) {
        mal_err("publish failed");
        mal_free(msg);
        return FAIL_RETURN;
    }

    mal_free(msg);

    mal_debug("MID Report: finished, IOT_MQTT_Publish() = %d", ret);
    return SUCCESS_RETURN;
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

    err = mal_mc_connect(pclient);
    if (SUCCESS_RETURN != err) {
        mal_mc_release(pclient);
        mal_free(pclient);
        return NULL;
    }

#if 1
    err = mal_mc_report_mid(pclient);
    if (SUCCESS_RETURN != err) {
        mal_mc_release(pclient);
        mal_free(pclient);
        return NULL;
    }

    /* report device info */
    err = mal_mc_report_devinfo(pclient);
    if (SUCCESS_RETURN != err) {
        mal_mc_release(pclient);
        mal_free(pclient);
        return NULL;
    }

    err = mal_mc_report_firmware_version(pclient);
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
            mal_info("error occur ");
            if (left_t < 20) {
                HAL_SleepMs(left_t);
            } else {
                HAL_SleepMs(20);
            }
        }
        _yield_lock(pClient);

        /* acquire package in cycle, such as PUBLISH */
        rc = mal_mc_cycle(pClient, &time);
        if (SUCCESS_RETURN == rc) {
        }
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


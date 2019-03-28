/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "mdal_ica_at_client.h"
#include "mdal_mal_import.h"
#include "iotx_utils.h"

#define MDAL_ICA_MALLOC(size) LITE_malloc(size, MEM_MAGIC, "mdal.ica")
#define MDAL_ICA_FREE(ptr)    LITE_free(ptr)

typedef enum {
    AT_MQTT_IDLE = 0,
    AT_MQTT_SEND_TYPE_SIMPLE,
    AT_MQTT_AUTH,
    AT_MQTT_SUB,
    AT_MQTT_UNSUB,
    AT_MQTT_PUB,
} at_mqtt_send_type_t;

recv_cb g_recv_cb;

int at_ica_mqtt_atsend(char *at_cmd, int timeout_ms);
int at_ica_mqtt_client_deinit(void);
int at_ica_mqtt_client_init(void);
int at_ica_mqtt_client_state(void);
int at_ica_mqtt_client_publish(const char *topic, int qos, const char *message);
int at_ica_mqtt_client_unsubscribe(const char *topic,
                                   unsigned int *mqtt_packet_id,
                                   int *mqtt_status);
int at_ica_mqtt_client_subscribe(const char *topic,
                                 int qos,
                                 unsigned int *mqtt_packet_id,
                                 int *mqtt_status,
                                 int timeout_ms);
int at_ica_mqtt_client_conn(char *proKey, char *devName, char *devSecret, int tlsEnable);
int at_ica_mqtt_client_auth(char *proKey, char *devName, char *devSecret, int tlsEnable);
int at_ica_mqtt_client_disconn(void);

static char mqtt_cmd_buffer[AT_MQTT_CMD_MAX_LEN];

int HAL_MDAL_MAL_Init()
{
#ifdef MAL_ICA_ENABLED
    g_recv_cb = NULL;
    return at_ica_mqtt_client_init();
#endif
    return -1;
}

int HAL_MDAL_MAL_Deinit()
{
#ifdef MAL_ICA_ENABLED
    g_recv_cb = NULL;
    return at_ica_mqtt_client_deinit();
#endif
    return -1;
}

int HAL_MDAL_MAL_Connect(char *proKey, char *devName, char *devSecret)
{
#ifdef MAL_ICA_ENABLED
    return at_ica_mqtt_client_conn(proKey, devName, devSecret, 0);
#endif
    return -1;
}

int HAL_MDAL_MAL_Disconnect(void)
{
#ifdef MAL_ICA_ENABLED
    return at_ica_mqtt_client_disconn();
#endif
    return -1;
}

int HAL_MDAL_MAL_Subscribe(const char *topic, int qos, unsigned int *mqtt_packet_id, int *mqtt_status, int timeout_ms)
{
#ifdef MAL_ICA_ENABLED
    return at_ica_mqtt_client_subscribe(topic, qos, mqtt_packet_id, mqtt_status, timeout_ms);
#endif
    return -1;
}

int HAL_MDAL_MAL_Unsubscribe(const char *topic, unsigned int *mqtt_packet_id, int *mqtt_status)
{
#ifdef MAL_ICA_ENABLED
    return at_ica_mqtt_client_unsubscribe(topic, mqtt_packet_id, mqtt_status);
#endif
    return -1;
}

int HAL_MDAL_MAL_Publish(const char *topic, int qos, const char *message)
{
#ifdef MAL_ICA_ENABLED
    return at_ica_mqtt_client_publish(topic, qos, message);
#endif
    return -1;
}


int HAL_MDAL_MAL_State(void)
{
#ifdef MAL_ICA_ENABLED
    return at_ica_mqtt_client_state();
#endif
    return -1;
}

void HAL_MDAL_MAL_RegRecvCb(recv_cb cb)
{
    g_recv_cb = cb;
}

int HAL_MDAL_MAL_Connectwifi(char *at_conn_wifi)
{
#ifdef MAL_ICA_ENABLED
    char  at_cmd[64];
    /* disconnect before connect to the network */
    if (at_ica_mqtt_client_disconn() != 0) {
        return -1;
    }

    memcpy(at_cmd, at_conn_wifi, 64);
    /* connect to the network */
    if (at_ica_mqtt_atsend(at_cmd, AT_MQTT_WAIT_FOREVER) != 0) {
        return -1;
    }

    return 0;
#endif
    return -1;
}

static char              *g_ica_rsp_buff = NULL;
static volatile int       g_mqtt_connect_state = 0;
static volatile at_mqtt_send_type_t   g_ica_at_response = AT_MQTT_IDLE;
static volatile int       g_at_response_result = 0;
static void              *g_sem_response;
static volatile int       g_response_msg_number = 0;
static int                g_response_packetid = 0;
static int                g_response_status = 0;
static int                g_public_qos = 0;

int at_ica_mqtt_atsend(char *at_cmd, int timeout_ms);

static void at_err_callback(char *at_rsp)
{
    char *temp;
    int    data;

    temp            = strtok(at_rsp, ":");
    temp            = strtok(NULL, ":");
    if ((data = strtol(temp, NULL, 0)) == 3) {
        g_at_response_result = 0;
    } else {
        g_at_response_result = -1;
    }

    /* notify the sender error; */
    HAL_SemaphorePost(g_sem_response);

    return;
}

static void at_succ_callback(void)
{
    g_at_response_result = 0;
    /* notify the sender ok; */
    HAL_SemaphorePost(g_sem_response);

    return;
}

static void sub_callback(char *at_rsp)
{
    char  *temp;

    if (strstr(at_rsp, AT_MQTT_CMD_ERROR_RSP)) {
        g_at_response_result = -1;

        /* notify the sender fail; */
        HAL_SemaphorePost(g_sem_response);

        return;
    } else if (NULL != strstr(at_rsp, AT_ICA_MQTT_MQTTSUBRSP)) {
        /* get status/packet_id */
        if (NULL != strstr(at_rsp, ",")) {
            g_at_response_result = 0;

            temp            = strtok(at_rsp, ":");

            if (temp != NULL) {
                temp            = strtok(NULL, ",");
            } else {
                mdal_err("subscribe rsp param invalid 1");
                g_at_response_result = -1;

                HAL_SemaphorePost(g_sem_response);
                return;
            }

            if (temp != NULL) {
                g_response_packetid = strtol(temp, NULL, 0);

                temp            = strtok(NULL, "\r\n");
            } else {
                mdal_err("subscribe rsp param invalid 2");
                g_at_response_result = -1;

                HAL_SemaphorePost(g_sem_response);
                return;
            }

            if (temp != NULL) {
                g_response_status = strtol(temp, NULL, 0);
            } else {
                mdal_err("subscribe rsp param invalid 3");
                g_at_response_result = -1;

                HAL_SemaphorePost(g_sem_response);
                return;
            }

            /* notify the sender ok; */
            HAL_SemaphorePost(g_sem_response);
        }
    }

    return;
}

static void unsub_callback(char *at_rsp)
{
    char  *temp;
    if (strstr(at_rsp, AT_MQTT_CMD_ERROR_RSP)) {
        g_at_response_result = -1;

        /* notify the sender fail; */
        HAL_SemaphorePost(g_sem_response);

        return;
    } else if (NULL != strstr(at_rsp, AT_ICA_MQTT_MQTTUNSUBRSP)) {
        /* get status/packet_id */
        if (NULL != strstr(at_rsp, ",")) {
            g_at_response_result = 0;

            temp            = strtok(at_rsp, ":");

            if (temp != NULL) {
                temp            = strtok(NULL, ",");
            } else {
                mdal_err("subscribe rsp param invalid 1");

                return;
            }

            if (temp != NULL) {
                g_response_packetid = strtol(temp, NULL, 0);

                temp            = strtok(NULL, "\r\n");
            } else {
                mdal_err("subscribe rsp param invalid 2");

                return;
            }

            if (temp != NULL) {
                g_response_status = strtol(temp, NULL, 0);
            } else {
                mdal_err("subscribe rsp param invalid 3");

                return;
            }

            mdal_err("unsub: %s", g_ica_rsp_buff);
            mdal_err("unsub packetid: %d, sta: %d", g_response_packetid, g_response_status);

            /* notify the sender ok; */
            HAL_SemaphorePost(g_sem_response);
        }
    }

    return;
}

static void pub_callback(char *at_rsp)
{
    char  *temp;
    if (strstr(at_rsp, AT_MQTT_CMD_ERROR_RSP)) {
        g_at_response_result = -1;

        /* notify the sender fail; */
        HAL_SemaphorePost(g_sem_response);

        return;
    } else if (NULL != strstr(at_rsp, AT_ICA_MQTT_MQTTPUBRSP)) {
        /* get status/packet_id */
        if ((NULL != strstr(at_rsp, ","))
            || (0 == g_public_qos)) {

            temp            = strtok(at_rsp, ":");

            if (temp != NULL) {
                if (0 == g_public_qos) {
                    temp    = strtok(NULL, "\r\n");
                } else {
                    temp    = strtok(NULL, ",");
                }
            } else {
                mdal_err("public get packetid error");

                return;
            }

            if (temp != NULL) {
                g_response_packetid = strtol(temp, NULL, 0);
            } else {
                mdal_err("public parse packetid error");

                return;
            }

            if (0 != g_public_qos) {
                temp            = strtok(NULL, "\r\n");

                if (temp != NULL) {
                    g_response_status = strtol(temp, NULL, 0);
                } else {
                    mdal_err("public parse status error");

                    return;
                }
            }

            g_at_response_result = 0;

            /* notify the sender ok; */
            HAL_SemaphorePost(g_sem_response);
        }
    }

    return;
}

static void state_change_callback(char *at_rsp)
{
    char *temp;

    if (NULL == at_rsp) {
        return;
    }

    temp = strtok(at_rsp, ":");

    if (temp != NULL) {
        temp = strtok(NULL, "\r\n");

        if (temp != NULL) {
            g_mqtt_connect_state = strtol(temp, NULL, 0);
        }
    }
    return;
}

static void recv_data_callback(char *at_rsp)
{
    char     *temp = NULL;
    char     *topic_ptr = NULL;
    char     *msg_ptr = NULL;
    unsigned int  msg_len = 0;
    /* unsinged int  packet_id = 0; */

    if (NULL == at_rsp) {
        return;
    }

    /* try to get msg id */
    temp = strtok(g_ica_rsp_buff, ":");

    if (temp != NULL) {
        temp  = strtok(NULL, ",");

        if (temp != NULL) {
            /* packet_id = strtol(temp, NULL, 0); */
        } else {
            mdal_err("packet id error");

            return;
        }
    } else {
        mdal_err("packet id not found");

        return;
    }

    /* try to get topic string */
    temp = strtok(NULL, "\"");

    if (temp != NULL) {
        temp[strlen(temp)] = '\0';

        topic_ptr = temp;
    } else {
        mdal_err("publish topic not found");

        return;
    }

    /* try to get payload string */
    temp = strtok(NULL, ",");

    if (temp != NULL) {
        msg_len = strtol(temp, NULL, 0);

        while (*temp++ != '\"');

        msg_ptr = temp;

        msg_ptr[msg_len] = '\0';

        g_recv_cb(topic_ptr, msg_ptr);

        return;
    } else {
        mdal_err("publish data not found");

        return;
    }

}

static void at_ica_mqtt_client_rsp_callback(void *arg, char *rspinfo, int rsplen)
{
    if (NULL == rspinfo || rsplen == 0) {
        mdal_err("invalid input of rsp callback");
        return;
    }
    if (NULL == g_ica_rsp_buff) {
        mdal_err("g_ica_rsp_buff rsp is NULL");
        return;
    }

    if (rsplen > AT_MQTT_RSP_MAX_LEN) {
        mdal_err("rsp len(%d) exceed max len", rsplen);
        return;
    }

    memcpy(g_ica_rsp_buff, rspinfo, rsplen);
    g_ica_rsp_buff[rsplen] = '\0';

    if (0 == memcmp(g_ica_rsp_buff,
                    AT_ICA_MQTT_MQTTERROR,
                    strlen(AT_ICA_MQTT_MQTTERROR))) {

        at_err_callback(g_ica_rsp_buff);
    } else if (0 == memcmp(g_ica_rsp_buff,
                           AT_ICA_MQTT_MQTTRCVPUB,
                           strlen(AT_ICA_MQTT_MQTTRCVPUB))) { /* Receive Publish Data */

        recv_data_callback(g_ica_rsp_buff);
    } else if (0 == memcmp(g_ica_rsp_buff,
                           AT_ICA_MQTT_MQTTSTATERSP,
                           strlen(AT_ICA_MQTT_MQTTSTATERSP))) {  /* Receive Mqtt Status Change */

        state_change_callback(g_ica_rsp_buff);
    } else {
        switch (g_ica_at_response) {  /* nothing to process */

            case AT_MQTT_IDLE:

                break;

            case AT_MQTT_SEND_TYPE_SIMPLE:

                if (0 == memcmp(g_ica_rsp_buff,
                                AT_MQTT_CMD_SUCCESS_RSP,
                                strlen(AT_MQTT_CMD_SUCCESS_RSP))) {

                    at_succ_callback();
                } else {

                    mdal_err("invalid success type %s", g_ica_rsp_buff);
                }

                break;

            case AT_MQTT_AUTH:

                if (0 == memcmp(g_ica_rsp_buff,
                                AT_ICA_MQTT_MQTTAUTHRSP,
                                strlen(AT_ICA_MQTT_MQTTAUTHRSP))) {

                    if (NULL != strstr(g_ica_rsp_buff, AT_MQTT_CMD_SUCCESS_RSP)) {
                        at_succ_callback();
                    }
                }

                break;

            case AT_MQTT_SUB:
                sub_callback(g_ica_rsp_buff);
                break;

            case AT_MQTT_UNSUB:
                unsub_callback(g_ica_rsp_buff);
                break;

            case AT_MQTT_PUB:
                pub_callback(g_ica_rsp_buff);
                break;

            default:
                break;

        }
    }

    return;
}

int at_ica_mqtt_client_disconn(void)
{
    char   at_cmd[64];

    memset(at_cmd, 0, 64);

    /* connect to the network */
    HAL_Snprintf(at_cmd,
                 64,
                 "%s\r\n",
                 AT_ICA_MQTT_MQTTDISCONN);

    /* disconnect from server */
    if (0 != at_ica_mqtt_atsend(at_cmd, AT_MQTT_WAIT_TIMEOUT)) {
        mdal_err("disconnect at command fail");

        return -1;
    }

    return 0;
}

int at_ica_mqtt_client_auth(char *proKey, char *devName, char *devSecret, int tlsEnable)
{
    char        at_cmd[AT_MQTT_CMD_MAX_LEN];

    if ((proKey == NULL) || (devName == NULL) || (devSecret == NULL)) {

        mdal_err("auth param should not be NULL");

        return -1;
    }

    /* set tls mode before auth */
    if (tlsEnable) {
        memset(at_cmd, 0, AT_MQTT_CMD_MAX_LEN);

        HAL_Snprintf(at_cmd,
                     AT_MQTT_CMD_MAX_LEN - 1,
                     "%s=%d\r\n",
                     AT_ICA_MQTT_MQTTMODE,
                     1);

        if (0 != at_ica_mqtt_atsend(at_cmd, AT_MQTT_WAIT_TIMEOUT)) {

            mdal_err("tls at command fail");

            return -1;
        }
    }

    /* submit auth */
    memset(at_cmd, 0, AT_MQTT_CMD_MAX_LEN);

    HAL_Snprintf(at_cmd,
                 AT_MQTT_CMD_MAX_LEN - 1,
                 "%s=\"%s\",\"%s\",\"%s\"\r\n",
                 AT_ICA_MQTT_MQTTAUTH,
                 proKey, devName, devSecret);

    if (0 != at_ica_mqtt_atsend(at_cmd, AT_MQTT_WAIT_TIMEOUT)) {

        mdal_err("auth at command fail");

        return -1;
    }

    return 0;
}

int at_ica_mqtt_client_conn(char *proKey, char *devName, char *devSecret, int tlsEnable)
{
    char  at_cmd[64];

    if ((proKey == NULL) || (devName == NULL) || (devSecret == NULL)) {

        mdal_err("conn param should not be NULL");

        return -1;
    }

    if (0 != at_ica_mqtt_client_auth(proKey, devName, devSecret, tlsEnable)) {

        mdal_err("authen fail");

        return -1;
    }

    HAL_SleepMs(500);

    /* connect to mqtt server */
    memset(at_cmd, 0, 64);

    HAL_Snprintf(at_cmd,
                 64,
                 "%s\r\n",
                 AT_ICA_MQTT_MQTTCONN);

    if (0 != at_ica_mqtt_atsend(at_cmd, AT_MQTT_WAIT_TIMEOUT)) {

        mdal_err("conn at command fail");

        return -1;
    }

    return 0;
}

int at_ica_mqtt_client_subscribe(const char *topic,
                                 int qos,
                                 unsigned int *mqtt_packet_id,
                                 int *mqtt_status,
                                 int timeout_ms)
{
    char    at_cmd[AT_MQTT_CMD_MAX_LEN];

    if ((topic == NULL) || (mqtt_packet_id == NULL) || (mqtt_status == NULL)) {

        mdal_err("subscribe param should not be NULL");

        return -1;
    }

    memset(at_cmd, 0, AT_MQTT_CMD_MAX_LEN);

    HAL_Snprintf(at_cmd,
                 AT_MQTT_CMD_MAX_LEN - 1,
                 "%s=\"%s\",%d\r\n",
                 AT_ICA_MQTT_MQTTSUB,
                 topic,
                 qos);

    if (0 != at_ica_mqtt_atsend(at_cmd, timeout_ms)) {
        mdal_err("sub at command fail");

        return -1;
    }

    return 0;
}

int at_ica_mqtt_client_unsubscribe(const char *topic,
                                   unsigned int *mqtt_packet_id,
                                   int *mqtt_status)
{
    char    at_cmd[AT_MQTT_CMD_MAX_LEN];
    if ((topic == NULL) || (mqtt_packet_id == NULL) || (mqtt_status == NULL)) {

        mdal_err("unsubscribe param should not be NULL");

        return -1;
    }

    memset(at_cmd, 0, AT_MQTT_CMD_MAX_LEN);

    HAL_Snprintf(at_cmd,
                 AT_MQTT_CMD_MAX_LEN - 1,
                 "%s=\"%s\"\r\n",
                 AT_ICA_MQTT_MQTTUNSUB,
                 topic);

    if (0 != at_ica_mqtt_atsend(at_cmd, AT_MQTT_WAIT_TIMEOUT)) {

        mdal_err("unsub at command fail");

        return -1;
    }

    return 0;
}

int at_ica_mqtt_client_publish(const char *topic, int qos, const char *message)
{
    char    at_cmd[AT_MQTT_CMD_MAX_LEN] = {0};
    char    msg_convert[AT_MQTT_CMD_MAX_LEN] = {0};
    char   *temp;
    if ((topic == NULL) || (message == NULL)) {

        mdal_err("publish param should not be NULL");

        return -1;
    }

    temp = msg_convert;

    /* for the case of " appeared in the string */
    while (*message) {
        if (*message == '\"') {
            *temp++ = '\\';
        }

        *temp++ = *message++;
    }

    HAL_Snprintf(at_cmd,
                 AT_MQTT_CMD_MAX_LEN - 1,
                 "%s=\"%s\",%d,\"%s\"\r\n",
                 AT_ICA_MQTT_MQTTPUB,
                 topic,
                 qos,
                 msg_convert);

    g_public_qos = qos;
    if (0 != at_ica_mqtt_atsend(at_cmd, AT_MQTT_WAIT_TIMEOUT)) {

        mdal_err("pub at command fail");

        return -1;
    }
    return 0;
}


int at_ica_mqtt_client_state(void)
{
    return (int)g_mqtt_connect_state;
}

int at_ica_mqtt_client_init(void)
{
    g_ica_rsp_buff = MDAL_ICA_MALLOC(AT_MQTT_RSP_MAX_LEN);
    if (NULL == g_ica_rsp_buff) {
        mdal_err("at ica mqtt client malloc buff failed");
        return -1;
    }

    if (NULL == (g_sem_response = HAL_SemaphoreCreate())) {
        if (NULL != g_ica_rsp_buff) {
            MDAL_ICA_FREE(g_ica_rsp_buff);

            g_ica_rsp_buff = NULL;
        }
        mdal_err("at ica mqtt client create sem failed");

        return -1;
    }

    g_mqtt_connect_state = 0;

    HAL_MDAL_MAL_ICA_Init();

    HAL_MDAL_MAL_ICA_InputCb(AT_ICA_MQTT_MQTTRCV,
                             AT_ICA_MQTT_POSTFIX,
                             mqtt_cmd_buffer,
                             AT_MQTT_CMD_MAX_LEN,
                             at_ica_mqtt_client_rsp_callback,
                             NULL);

    HAL_MDAL_MAL_ICA_InputCb(AT_ICA_MQTT_MQTTERROR,
                             AT_ICA_MQTT_POSTFIX,
                             mqtt_cmd_buffer,
                             AT_MQTT_CMD_MAX_LEN,
                             at_ica_mqtt_client_rsp_callback,
                             NULL);

    HAL_MDAL_MAL_ICA_InputCb(AT_ICA_MQTT_MQTTOK,
                             AT_ICA_MQTT_POSTFIX,
                             mqtt_cmd_buffer,
                             AT_MQTT_CMD_MAX_LEN,
                             at_ica_mqtt_client_rsp_callback,
                             NULL);

    return 0;
}

int at_ica_mqtt_client_deinit(void)
{
    if (NULL != g_ica_rsp_buff) {
        MDAL_ICA_FREE(g_ica_rsp_buff);
        g_ica_rsp_buff = NULL;
    }

    HAL_SemaphoreDestroy(g_sem_response);

    g_mqtt_connect_state = 0;

    return 0;
}

int at_ica_mqtt_atsend(char *at_cmd, int timeout_ms)
{
    if (at_cmd == NULL) {
        return -1;
    }
    /* state error */
    if (g_ica_at_response != AT_MQTT_IDLE) {

        mdal_err("at send state is not idle (%d)", g_ica_at_response);

        return -1;
    }

    mdal_err("send: %s", at_cmd);

    if (NULL != strstr(at_cmd, AT_ICA_MQTT_MQTTAUTH)) {
        g_ica_at_response = AT_MQTT_AUTH;
    } else if (NULL != strstr(at_cmd, AT_ICA_MQTT_MQTTSUB)) {
        g_ica_at_response = AT_MQTT_SUB;
    } else if (NULL != strstr(at_cmd, AT_ICA_MQTT_MQTTUNSUB)) {
        g_ica_at_response = AT_MQTT_UNSUB;
    } else if (NULL != strstr(at_cmd, AT_ICA_MQTT_MQTTPUB)) {
        g_ica_at_response = AT_MQTT_PUB;
    } else {
        g_ica_at_response = AT_MQTT_SEND_TYPE_SIMPLE;
    }

    if (0 != HAL_MDAL_MAL_ICA_Write(at_cmd)) {

        mdal_err("at send raw api fail");

        g_ica_at_response = AT_MQTT_IDLE;

        return -1;
    }
    HAL_SemaphoreWait(g_sem_response, timeout_ms);
    g_ica_at_response = AT_MQTT_IDLE;

    return g_at_response_result;
}





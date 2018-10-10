/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <aos/aos.h>
#include "at_mqtt_client.h"
#include "at_mqtt_ica_client.h"
#include "atparser.h"


#define AT_MQTT_CMD_MAX_LEN             512
#define AT_MQTT_CMD_SUCCESS_RSP         "OK"
#define AT_MQTT_CMD_FAIL_RSP            "FAIL"
#define AT_MQTT_CMD_ERROR_RSP           "ERROR"
#define AT_MQTT_SUBSCRIBE_FAIL          128
#define AT_MQTT_RSP_MAX_LEN             1500

typedef enum {
    AT_MQTT_IDLE = 0,
    AT_MQTT_SEND_TYPE_SIMPLE,
    AT_MQTT_AUTH,
    AT_MQTT_SUB,
    AT_MQTT_UNSUB,
    AT_MQTT_PUB,
} at_mqtt_send_type_t;

static char        *g_ica_rsp_buff = NULL;
static volatile uint8_t   g_mqtt_connect_state = 0;
static volatile at_mqtt_send_type_t   g_ica_at_response = AT_MQTT_IDLE;
static volatile int       g_at_response_result = 0;
static aos_sem_t          g_sem_response;
static volatile uint8_t   g_response_msg_number = 0;
static int                g_response_packetid = 0;
static int                g_response_status = 0;
static int                g_public_qos = 0;

static int at_mqtt_ica_atsend(char *at_cmd);

static void at_err_callback(void)
{
    g_at_response_result = -1;

    // notify the sender error;
    aos_sem_signal(&g_sem_response);

    return;
}

static void at_succ_callback(void)
{
    g_at_response_result = 0;

    // notify the sender ok;
    aos_sem_signal(&g_sem_response);

    return;
}

static void sub_callback(char *at_rsp)
{
    char  *temp;

    if (strstr(at_rsp, AT_MQTT_CMD_ERROR_RSP)) {
        g_at_response_result = -1;

        // notify the sender fail;
        aos_sem_signal(&g_sem_response);

        return;
    } else if (NULL != strstr(at_rsp, AT_MQTT_ICA_MQTTSUBRSP)) {
        // get status/packet_id
        if (NULL != strstr(at_rsp, ",")) {
            g_at_response_result = 0;

            temp            = strtok(at_rsp, ":");

            if (temp != NULL) {
                temp            = strtok(NULL, ",");
            } else {
                printf("subscribe rsp param invalid 1\r\n");

                return;
            }

            if (temp != NULL) {
                g_response_packetid = strtol(temp, NULL, 0);

                temp            = strtok(NULL, "\r\n");
            } else {
                printf("subscribe rsp param invalid 2\r\n");

                return;
            }

            if (temp != NULL) {
                g_response_status = strtol(temp, NULL, 0);
            } else {
                printf("subscribe rsp param invalid 3\r\n");

                return;
            }

            // notify the sender ok;
            aos_sem_signal(&g_sem_response);
        }
    }

    return;
}

static void unsub_callback(char *at_rsp)
{
    char  *temp;

    if (strstr(at_rsp, AT_MQTT_CMD_ERROR_RSP)) {
        g_at_response_result = -1;

        // notify the sender fail;
        aos_sem_signal(&g_sem_response);

        return;
    } else if (NULL != strstr(at_rsp, AT_MQTT_ICA_MQTTUNSUBRSP)) {
        // get status/packet_id
        if (NULL != strstr(at_rsp, ",")) {
            g_at_response_result = 0;

            temp            = strtok(at_rsp, ":");

            if (temp != NULL) {
                temp            = strtok(NULL, ",");
            } else {
                printf("subscribe rsp param invalid 1\r\n");

                return;
            }

            if (temp != NULL) {
                g_response_packetid = strtol(temp, NULL, 0);

                temp            = strtok(NULL, "\r\n");
            } else {
                printf("subscribe rsp param invalid 2\r\n");

                return;
            }

            if (temp != NULL) {
                g_response_status = strtol(temp, NULL, 0);
            } else {
                printf("subscribe rsp param invalid 3\r\n");

                return;
            }

            printf("unsub: %s", g_ica_rsp_buff);
            printf("unsub packetid: %d, sta: %d\r\n", g_response_packetid, g_response_status);

            // notify the sender ok;
            aos_sem_signal(&g_sem_response);
        }
    }

    return;
}

static void pub_callback(char *at_rsp)
{
    char  *temp;

    if (strstr(at_rsp, AT_MQTT_CMD_ERROR_RSP)) {
        g_at_response_result = -1;

        // notify the sender fail;
        aos_sem_signal(&g_sem_response);

        return;
    } else if (NULL != strstr(at_rsp, AT_MQTT_ICA_MQTTPUBRSP)) {
        // get status/packet_id
        if ((NULL != strstr(at_rsp, ","))
          ||(0 == g_public_qos)) {

            temp            = strtok(at_rsp, ":");

            if (temp != NULL) {
                if (0 == g_public_qos) {
                    temp    = strtok(NULL, "\r\n");
                } else {
                    temp    = strtok(NULL, ",");
                }
            } else {
                printf("public get packetid error\r\n");

                return;
            }

            if (temp != NULL) {
                g_response_packetid = strtol(temp, NULL, 0);
            } else {
                printf("public parse packetid error\r\n");

                return;
            }

            if (0 != g_public_qos) {
                temp            = strtok(NULL, "\r\n");

                if (temp != NULL) {
                    g_response_status = strtol(temp, NULL, 0);
                } else {
                    printf("public parse status error\r\n");

                    return;
                }
            }

            g_at_response_result = 0;

            // notify the sender ok;
            aos_sem_signal(&g_sem_response);
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
    uint32_t  msg_len = 0;
    uint32_t  packet_id = 0;

    if (NULL == at_rsp) {
        return;
    }

    // try to get msg id
    temp = strtok(g_ica_rsp_buff, ":");

    if (temp != NULL) {
        temp  = strtok(NULL, ",");

        if (temp != NULL) {
            packet_id = strtol(temp, NULL, 0);
        } else {
            printf("packet id error\r\n");

            return;
        }
    } else {
        printf("packet id not found\r\n");

        return;
    }

    // try to get topic string
    temp = strtok(NULL, "\"");

    if (temp != NULL) {
        temp[strlen(temp)] = '\0';

        topic_ptr = temp;
    } else {
        printf("publish topic not found\r\n");

        return;
    }

    // try to get payload string
    temp = strtok(NULL, ",");

    if (temp != NULL) {
        msg_len = strtol(temp, NULL, 0);

        while (*temp++ != '\"');

        msg_ptr = temp;

        msg_ptr[msg_len] = '\0';

        at_mqtt_save_msg(topic_ptr, msg_ptr);

        return;
    } else {
        printf("publish data not found\r\n");

        return;
    }

}

static void at_mqtt_ica_client_rsp_callback(void *arg, char *rspinfo, int rsplen)
{
    if (NULL == rspinfo || rsplen == 0) {
        printf("invalid input of rsp callback\r\n");
        return;
    }

    if (NULL == g_ica_rsp_buff) {
        printf("g_ica_rsp_buff rsp is NULL\r\n");
        return;
    }

    if (rsplen > AT_MQTT_RSP_MAX_LEN) {
        printf("rsp len exceed max len\r\n");
        return;
    }

    memcpy(g_ica_rsp_buff, rspinfo, rsplen);
    g_ica_rsp_buff[rsplen] = '\0';

    if (0 == memcmp(g_ica_rsp_buff,
                    AT_MQTT_ICA_MQTTERROR,
                    strlen(AT_MQTT_ICA_MQTTERROR))) {

        at_err_callback();
    } else if (0 == memcmp(g_ica_rsp_buff,
                           AT_MQTT_ICA_MQTTRCVPUB,
                           strlen(AT_MQTT_ICA_MQTTRCVPUB))) { // Receive Publish Data

        recv_data_callback(g_ica_rsp_buff);
    } else if (0 == memcmp(g_ica_rsp_buff,
                           AT_MQTT_ICA_MQTTSTATERSP,
                           sizeof(AT_MQTT_ICA_MQTTSTATERSP))) {  // Receive Mqtt Status Change

        state_change_callback(g_ica_rsp_buff);
    } else {
        switch (g_ica_at_response) {  // nothing to process

            case AT_MQTT_IDLE:

                break;

            case AT_MQTT_SEND_TYPE_SIMPLE:

                if (0 == memcmp(g_ica_rsp_buff,
                                AT_MQTT_CMD_SUCCESS_RSP,
                                strlen(AT_MQTT_CMD_SUCCESS_RSP))) {

                    at_succ_callback();
                } else {

                    printf("invalid success type %s", g_ica_rsp_buff);
                }

                break;

            case AT_MQTT_AUTH:

                if (0 == memcmp(g_ica_rsp_buff,
                                AT_MQTT_ICA_MQTTAUTHRSP,
                                strlen(AT_MQTT_ICA_MQTTAUTHRSP))) {

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

static int at_mqtt_ica_client_disconn(void)
{
    char   at_cmd[64];

    memset(at_cmd, 0, 64);

    // connect to the network
    snprintf(at_cmd,
             64,
             "%s\r\n",
             AT_MQTT_ICA_MQTTDISCONN);

    /* disconnect from server */
    if (0 != at_mqtt_ica_atsend(at_cmd)) {
        printf("disconnect at command fail\r\n");

        return -1;
    }

    return 0;
}

static int at_mqtt_ica_client_auth(char *proKey, char *devName, char *devSecret, uint8_t tlsEnable)
{
    char        at_cmd[AT_MQTT_CMD_MAX_LEN];

    if ((proKey == NULL)||(devName == NULL)||(devSecret == NULL)) {

        printf("auth param should not be NULL\r\n");

        return -1;
    }

    /* set tls mode before auth */
    if (tlsEnable) {
        memset(at_cmd, 0, AT_MQTT_CMD_MAX_LEN);

        snprintf(at_cmd,
                 AT_MQTT_CMD_MAX_LEN - 1,
                 "%s=%d",
                 AT_MQTT_ICA_MQTTMODE,
                 1);

        if (0 != at_mqtt_ica_atsend(at_cmd)) {

            printf("tls at command fail\r\n");

            return -1;
        }
    }

    /* submit auth */
    memset(at_cmd, 0, AT_MQTT_CMD_MAX_LEN);

    snprintf(at_cmd,
             AT_MQTT_CMD_MAX_LEN - 1,
             "%s=\"%s\",\"%s\",\"%s\"\r\n",
             AT_MQTT_ICA_MQTTAUTH,
             proKey, devName, devSecret);

    if (0 != at_mqtt_ica_atsend(at_cmd)) {

        printf("auth at command fail\r\n");

        return -1;
    }

    return 0;
}

static int at_mqtt_ica_client_conn(char *proKey, char *devName, char *devSecret, uint8_t tlsEnable)
{
    char  at_cmd[64];

    if ((proKey == NULL)||(devName == NULL)||(devSecret == NULL)) {

        printf("conn param should not be NULL\r\n");

        return -1;
    }

    if (0 != at_mqtt_ica_client_auth(proKey, devName, devSecret, tlsEnable)) {

        printf("authen fail\r\n");

        return -1;
    }

    aos_msleep(500);

    /* connect to mqtt server */
    memset(at_cmd, 0, 64);

    snprintf(at_cmd,
             64,
             "%s\r\n",
             AT_MQTT_ICA_MQTTCONN);

    if (0 != at_mqtt_ica_atsend(at_cmd)) {

        printf("conn at command fail\r\n");

        return -1;
    }

    return 0;
}

static int at_mqtt_ica_client_subscribe(char *topic,
                                        uint8_t qos,
                                        int *mqtt_packet_id,
                                        int *mqtt_status)
{
    char    at_cmd[AT_MQTT_CMD_MAX_LEN];

    if ((topic == NULL)||(mqtt_packet_id == NULL)||(mqtt_status == NULL)) {

        printf("subscribe param should not be NULL\r\n");

        return -1;
    }

    memset(at_cmd, 0, AT_MQTT_CMD_MAX_LEN);

    snprintf(at_cmd,
             AT_MQTT_CMD_MAX_LEN - 1,
             "%s=\"%s\",%d\r\n",
             AT_MQTT_ICA_MQTTSUB,
             topic,
             qos);

    if (0 != at_mqtt_ica_atsend(at_cmd)) {
        printf("sub at command fail\r\n");

        return -1;
    }

    return 0;
}

static int at_mqtt_ica_client_ubsubscribe(char *topic,
                                          int *mqtt_packet_id,
                                          int *mqtt_status)
{
    char    at_cmd[AT_MQTT_CMD_MAX_LEN];

    if ((topic == NULL)||(mqtt_packet_id == NULL)||(mqtt_status == NULL)) {

        printf("unsubscribe param should not be NULL\r\n");

        return -1;
    }

    memset(at_cmd, 0, AT_MQTT_CMD_MAX_LEN);

    snprintf(at_cmd,
             AT_MQTT_CMD_MAX_LEN - 1,
             "%s=\"%s\"",
             AT_MQTT_ICA_MQTTUNSUB,
             topic);

    if (0 != at_mqtt_ica_atsend(at_cmd)) {

        printf("unsub at command fail\r\n");

        return -1;
    }

    return 0;
}

static int at_mqtt_ica_client_publish(char *topic, uint8_t qos, char *message)
{
//    int     packet_id;
//    int     status;
    char    at_cmd[AT_MQTT_CMD_MAX_LEN];
    char    msg_convert[AT_MQTT_CMD_MAX_LEN];
    char   *temp;

    if ((topic == NULL)||(message == NULL)) {

        printf("publish param should not be NULL\r\n");

        return -1;
    }

    memset(at_cmd, 0, AT_MQTT_CMD_MAX_LEN);
    memset(msg_convert, 0, AT_MQTT_CMD_MAX_LEN);

    temp = msg_convert;

    // for the case of " appeared in the string
    while (*message) {
        if (*message == '\"') {
            *temp++ = '\\';
        }

        *temp++ = *message++;
    }

    snprintf(at_cmd,
             AT_MQTT_CMD_MAX_LEN - 1,
             "%s=\"%s\",%d,\"%s\"\r\n",
             AT_MQTT_ICA_MQTTPUB,
             topic,
             qos,
             msg_convert);

    g_public_qos = qos;

    if (0 != at_mqtt_ica_atsend(at_cmd)) {

        printf("pub at command fail\r\n");

        return -1;
    }

    return 0;
}


static int at_mqtt_ica_client_state(void)
{
    return (int)g_mqtt_connect_state;
}

static int at_mqtt_ica_client_settings(void)
{
    return 0;
}

static at_mqtt_client_op_t g_at_mqtt_ica_client = {
    .conn           = at_mqtt_ica_client_conn,
    .disconn        = at_mqtt_ica_client_disconn,
    .subscribe      = at_mqtt_ica_client_subscribe,
    .unsubscribe    = at_mqtt_ica_client_ubsubscribe,
    .publish        = at_mqtt_ica_client_publish,
    .state          = at_mqtt_ica_client_state,
    .settings       = at_mqtt_ica_client_settings,
    .sendat         = at_mqtt_ica_atsend,
};


int at_mqtt_ica_client_init(void)
{
    g_ica_rsp_buff = aos_malloc(AT_MQTT_RSP_MAX_LEN);

    if (NULL == g_ica_rsp_buff) {
        return -1;
    }

    if (0 != aos_sem_new(&g_sem_response, 0)) {
        if (NULL == g_ica_rsp_buff) {
            aos_free(g_ica_rsp_buff);

            g_ica_rsp_buff = NULL;
        }

        return -1;
    }

    g_mqtt_connect_state = 0;

    at.init(AT_RECV_PREFIX,
            AT_RECV_SUCCESS_POSTFIX,
            AT_RECV_FAIL_POSTFIX,
            AT_SEND_DELIMITER,
            1000);

    at.set_mode(ASYN);

    at.oob(AT_MQTT_ICA_MQTTRCV,
           AT_MQTT_ICA_POSTFIX,
           AT_MQTT_CMD_MAX_LEN,
           at_mqtt_ica_client_rsp_callback,
           NULL);

    at.oob(AT_MQTT_ICA_MQTTERROR,
           AT_MQTT_ICA_POSTFIX,
           AT_MQTT_CMD_MAX_LEN,
           at_mqtt_ica_client_rsp_callback,
           NULL);

    at.oob(AT_MQTT_ICA_MQTTOK,
           AT_MQTT_ICA_POSTFIX,
           AT_MQTT_CMD_MAX_LEN,
           at_mqtt_ica_client_rsp_callback,
           NULL);

    return at_mqtt_register(&g_at_mqtt_ica_client);
}

int at_mqtt_ica_client_deinit(void)
{
    if (NULL != g_ica_rsp_buff) {

        aos_free(g_ica_rsp_buff);
        g_ica_rsp_buff = NULL;
    }

    if (aos_sem_is_valid(&g_sem_response)) {
        aos_sem_free(&g_sem_response);
    }

    g_mqtt_connect_state = 0;

    return at_mqtt_unregister();
}

static int at_mqtt_ica_atsend(char *at_cmd)
{
    if (at_cmd == NULL) {
        return -1;
    }

    // state error
    if (g_ica_at_response != AT_MQTT_IDLE) {

        printf("at send state is not idle\r\n");

        return -1;
    }

    printf("send: %s", at_cmd);

    if (NULL != strstr(at_cmd, AT_MQTT_ICA_MQTTAUTH)) {
        g_ica_at_response = AT_MQTT_AUTH;
    } else if (NULL != strstr(at_cmd, AT_MQTT_ICA_MQTTSUB)) {
        g_ica_at_response = AT_MQTT_SUB;
    } else if (NULL != strstr(at_cmd, AT_MQTT_ICA_MQTTUNSUB)) {
        g_ica_at_response = AT_MQTT_UNSUB;
    } else if (NULL != strstr(at_cmd, AT_MQTT_ICA_MQTTPUB)) {
        g_ica_at_response = AT_MQTT_PUB;
    } else {
        g_ica_at_response = AT_MQTT_SEND_TYPE_SIMPLE;
    }

    if (0 != at.send_raw_no_rsp(at_cmd)) {

        printf("at send raw api fail\r\n");

        g_ica_at_response = AT_MQTT_IDLE;

        return -1;
    }

    aos_sem_wait(&g_sem_response, AOS_WAIT_FOREVER);

    g_ica_at_response = AT_MQTT_IDLE;

    return g_at_response_result;
}




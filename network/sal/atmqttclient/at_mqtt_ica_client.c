/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdint.h>
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

static aos_sem_t    g_ica_rsp_sem;
static char        *g_ica_rsp_buff = NULL;
static mqtt_state_t g_mqtt_state;

static at_mqtt_state_type_t  g_at_mqtt_state = AT_MQTT_STATE_IDLE;

static void at_mqtt_gotip_callback(void *arg, char *rspinfo, int rsplen)
{
    g_at_mqtt_state = AT_MQTT_STATE_GOTIP;
}

static void at_mqtt_ica_client_rsp_callback(void *arg, char *rspinfo, int rsplen)
{
    char *temp;

    if (NULL == rspinfo || rsplen == 0) {
        printf("invalid input of rsp callback, %d, %d\r\n", rspinfo, rsplen);
        return;
    }

    if (NULL == g_ica_rsp_buff) {
        printf("g_ica_rsp_buff rsp is NULL\r\n");
        return;
    }

    if (rsplen > AT_MQTT_RSP_MAX_LEN) {
        printf("rsp len exceed max len\r\n");
    }

    memcpy(g_ica_rsp_buff, rspinfo, rsplen);
    g_ica_rsp_buff[rsplen] = '\0';

    if (strstr(g_ica_rsp_buff, AT_MQTT_ICA_MQTTAUTHRSP) != NULL) { // Authentication Response

        if (g_at_mqtt_state == AT_MQTT_STATE_AUTH) {
            aos_sem_signal(&g_ica_rsp_sem);
        }
    } else if (strstr(g_ica_rsp_buff, AT_MQTT_ICA_MQTTPUBRSP) != NULL) { // Publish Response

        if (g_at_mqtt_state == AT_MQTT_STATE_PUBLISH) {
            aos_sem_signal(&g_ica_rsp_sem);
        }
    } else if (strstr(g_ica_rsp_buff, AT_MQTT_ICA_MQTTSUBRSP) != NULL) { // Subscribe Response

        if  (g_at_mqtt_state == AT_MQTT_STATE_SUBSCRIBE) {
            aos_sem_signal(&g_ica_rsp_sem);
        }
    } else if (strstr(g_ica_rsp_buff, AT_MQTT_ICA_MQTTUNSUBRSP) != NULL) { // Unsubscribe Response

        if  (g_at_mqtt_state == AT_MQTT_STATE_UNSUBSCRIBE) {
            aos_sem_signal(&g_ica_rsp_sem);
        }
    } else if (strstr(g_ica_rsp_buff, AT_MQTT_ICA_MQTTRCVPUB) != NULL) { // Receive Publish Data

        temp = strtok(g_ica_rsp_buff, "=");

        //at_mqtt_write_buffer( temp, rsplen - 12);
        printf("receive pub: %s\r\n", temp);
    } else if (strstr(g_ica_rsp_buff, AT_MQTT_ICA_MQTTSTATERSP) != NULL) {  // Receive Mqtt Status Change

        temp = strtok(g_ica_rsp_buff, ":");
        temp = strtok(NULL, "\r\n");

        g_mqtt_state.mqtt_state       = strtol(temp, NULL, 0);
        g_mqtt_state.auto_report_flag = 1;
    }

    return;
}

static int at_mqtt_ica_client_disconn(void)
{
    char at_rsp[AT_MQTT_CMD_MAX_LEN];

    /* disconnect from server */
    memset(at_rsp, 0, AT_MQTT_CMD_MAX_LEN);

    at.send_raw(AT_MQTT_ICA_MQTTDISCONN, at_rsp, AT_MQTT_CMD_MAX_LEN);

    if (strstr(at_rsp, AT_MQTT_CMD_SUCCESS_RSP) == NULL) {
        return -1;
    }

    return 0;
}

static int at_mqtt_ica_client_auth(char *proKey, char *devName, char *devSecret, uint8_t tlsEnable)
{
    uint8_t     i;
    char        at_cmd[AT_MQTT_CMD_MAX_LEN];
    char        at_rsp[AT_MQTT_CMD_MAX_LEN];

    /* set tls mode before auth */
    if (tlsEnable) {
        memset(at_cmd, 0, AT_MQTT_CMD_MAX_LEN);
        memset(at_rsp, 0, AT_MQTT_CMD_MAX_LEN);

        snprintf(at_cmd,
                 AT_MQTT_CMD_MAX_LEN - 1,
                 "%s=%d",
                 AT_MQTT_ICA_MQTTMODE,
                 1);

        at.send_raw(at_cmd, at_rsp, AT_MQTT_CMD_MAX_LEN);

        if (strstr(at_rsp, AT_MQTT_CMD_SUCCESS_RSP) == NULL) {
            return -1;
        }
    }

    /* submit auth */

    memset(at_cmd, 0, AT_MQTT_CMD_MAX_LEN);
    memset(at_rsp, 0, AT_MQTT_CMD_MAX_LEN);

    snprintf(at_cmd,
             AT_MQTT_CMD_MAX_LEN - 1,
             "%s=\"%s\",\"%s\",\"%s\"\r\n",
             AT_MQTT_ICA_MQTTAUTH,
             proKey, devName, devSecret);

    at.send_raw(at_cmd, at_rsp, AT_MQTT_CMD_MAX_LEN);

    if (strstr(at_rsp, AT_MQTT_CMD_SUCCESS_RSP) == NULL) {        
        return -1;
    }

    g_at_mqtt_state = AT_MQTT_STATE_AUTH;

    aos_sem_wait(&g_ica_rsp_sem, AOS_WAIT_FOREVER);

    g_at_mqtt_state = AT_MQTT_STATE_AUTH_FINISH;

    if (strstr(g_ica_rsp_buff, AT_MQTT_CMD_SUCCESS_RSP) == NULL) {
        return -1;
    }

    return 0;
}

static int at_mqtt_ica_client_conn(char *proKey, char *devName, char *devSecret, uint8_t tlsEnable)
{
    char at_rsp[64];
    char at_cmd[64];
    int  conn_ret;
    int  wifi_timeout = 0;

    // disconnect before connect to the network
    at_mqtt_ica_client_disconn();

    memset(at_rsp, 0, 64);
    memset(at_cmd, 0, 64);

    // connect to the network
    snprintf(at_cmd,
             64,
             "%s\r\n",
             AT_MQTT_CONNECT_NETWORK);

    g_at_mqtt_state = AT_MQTT_STATE_JOIN_WIFI;

    at.send_raw(at_cmd, at_rsp, AT_MQTT_CMD_MAX_LEN);

    if (strstr(at_rsp, AT_MQTT_CMD_SUCCESS_RSP) == NULL) {
        return -1;
    }

    while (g_at_mqtt_state != AT_MQTT_STATE_GOTIP) {

        wifi_timeout++;

        // timeout return
        if (wifi_timeout >= 5) {
            return -1;
        }

        // must be 1000ms, or some error may appear, need to resolve later
        aos_msleep(1000);
    };

    conn_ret = at_mqtt_ica_client_auth(proKey, devName, devSecret, tlsEnable);

    if (conn_ret != 0) {
        return -1;
    }

    aos_msleep(500);

    /* connect to mqtt server */
    memset(at_rsp, 0, 64);
    memset(at_cmd, 0, 64);

    snprintf(at_cmd,
             64,
             "%s\r\n",
             AT_MQTT_ICA_MQTTCONN);

    at.send_raw(at_cmd, at_rsp, AT_MQTT_CMD_MAX_LEN);

    if (strstr(at_rsp, AT_MQTT_CMD_SUCCESS_RSP) == NULL) {
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
    char    at_rsp[AT_MQTT_CMD_MAX_LEN];
    char   *temp;

    memset(at_cmd, 0, AT_MQTT_CMD_MAX_LEN);
    memset(at_rsp, 0, AT_MQTT_CMD_MAX_LEN);

    // TODO: check the return value
    snprintf(at_cmd,
             AT_MQTT_CMD_MAX_LEN - 1,
             "%s=%s,%d\r\n",
             AT_MQTT_ICA_MQTTSUB,
             topic,
             qos);

    at.send_raw(at_cmd, at_rsp, AT_MQTT_CMD_MAX_LEN);

    if (strstr(at_rsp, AT_MQTT_CMD_SUCCESS_RSP) == NULL) {
        return -1;
    }

    g_at_mqtt_state = AT_MQTT_STATE_SUBSCRIBE;

    aos_sem_wait(&g_ica_rsp_sem, AOS_WAIT_FOREVER);

    g_at_mqtt_state = AT_MQTT_STATE_SUBSCRIBE_FINISH;

    temp            = strtok(g_ica_rsp_buff, ":");
    temp            = strtok(NULL, ",");
    *mqtt_packet_id = strtol(temp, NULL, 0);
    temp            = strtok(NULL, "\r\n");
    *mqtt_status    = strtol(temp, NULL, 0);

    return 0;
}

static int at_mqtt_ica_client_ubsubscribe(char *topic,
                                          int *mqtt_packet_id,
                                          int *mqtt_status)
{
    char    at_cmd[AT_MQTT_CMD_MAX_LEN];
    char    at_rsp[AT_MQTT_CMD_MAX_LEN];
    char   *temp;

    memset(at_cmd, 0, AT_MQTT_CMD_MAX_LEN);
    memset(at_rsp, 0, AT_MQTT_CMD_MAX_LEN);

    // TODO: check the return value
    snprintf(at_cmd,
             AT_MQTT_CMD_MAX_LEN - 1,
             "%s=%s",
             AT_MQTT_ICA_MQTTUNSUB,
             topic);

    at.send_raw(at_cmd, at_rsp, AT_MQTT_CMD_MAX_LEN);

    if (strstr(at_rsp, AT_MQTT_CMD_SUCCESS_RSP) == NULL) {
        return -1;
    }

    g_at_mqtt_state = AT_MQTT_STATE_UNSUBSCRIBE;

    aos_sem_wait(&g_ica_rsp_sem, AOS_WAIT_FOREVER);

    g_at_mqtt_state = AT_MQTT_STATE_UNSUBSCRIBE_FINISH;

    temp            = strtok(g_ica_rsp_buff, ":");
    temp            = strtok(NULL, ",");
    *mqtt_packet_id = strtol(temp, NULL, 0);
    temp            = strtok(NULL, "\r\n");
    *mqtt_status    = strtol(temp, NULL, 0);

    return 0;
}

static int at_mqtt_ica_client_publish(char *topic, uint8_t qos, int *message)
{
    int     packet_id;
    int     status;
    char    at_cmd[AT_MQTT_CMD_MAX_LEN];
    char    at_rsp[AT_MQTT_CMD_MAX_LEN];
    char   *temp;

    memset(at_cmd, 0, AT_MQTT_CMD_MAX_LEN);
    memset(at_rsp, 0, AT_MQTT_CMD_MAX_LEN);

    // TODO: check the return value, fragment check
    snprintf(at_cmd,
             AT_MQTT_CMD_MAX_LEN - 1,
             "%s=%s,%d,\"%s\"\r\n",
             AT_MQTT_ICA_MQTTPUB,
             topic,
             qos,
             message);

    at.send_raw(at_cmd, at_rsp, AT_MQTT_CMD_MAX_LEN);

    if (strstr(at_rsp, AT_MQTT_CMD_SUCCESS_RSP) == NULL) {
        return -1;
    }

    g_at_mqtt_state = AT_MQTT_STATE_PUBLISH;

    aos_sem_wait(&g_ica_rsp_sem, AOS_WAIT_FOREVER);

    g_at_mqtt_state = AT_MQTT_STATE_PUBLISH_FINISH;

    temp        = strtok(g_ica_rsp_buff, ":");
    temp        = strtok(NULL, ",");
    packet_id   = strtol(temp, NULL, 0);
    temp        = strtok(NULL, "\r\n");
    status      = strtol(temp, NULL, 0);

    return 0;
}


static int at_mqtt_ica_client_state(void)
{
    char        at_cmd[AT_MQTT_CMD_MAX_LEN];
    char        at_rsp[AT_MQTT_CMD_MAX_LEN];
    uint8_t     state;
    char       *temp;

    // if the ICA module auto report the state, return this value directly.
    if (g_mqtt_state.auto_report_flag) {

        return g_mqtt_state.mqtt_state;
    }

    memset(at_cmd, 0, AT_MQTT_CMD_MAX_LEN);
    memset(at_rsp, 0, AT_MQTT_CMD_MAX_LEN);

    // TODO: check the return value, fragment check
    snprintf(at_cmd,
             AT_MQTT_CMD_MAX_LEN - 1,
             "%s?",
             AT_MQTT_ICA_MQTTSTATE);

    at.send_raw(at_cmd, at_rsp, AT_MQTT_CMD_MAX_LEN);

    if (strstr(at_rsp, AT_MQTT_CMD_SUCCESS_RSP) == NULL) {
        return -1;
    }

    temp  = strtok(at_rsp, ":");

    temp  = strtok(NULL, "\r\n");
    state = strtol(temp, NULL, 0);

    return 0;
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
};


int at_mqtt_ica_client_init(void)
{
    g_ica_rsp_buff = aos_malloc(AT_MQTT_RSP_MAX_LEN);

    if (NULL == g_ica_rsp_buff) {
        return -1;
    }

    if (0 != aos_sem_new(&g_ica_rsp_sem, 0)) {
        if (g_ica_rsp_buff != NULL) {
            aos_free(g_ica_rsp_buff);
            g_ica_rsp_buff = NULL;
        }

        return -1;
    }

    g_mqtt_state.auto_report_flag = 0;
    g_mqtt_state.mqtt_state       = 0;

    g_at_mqtt_state = AT_MQTT_STATE_IDLE;

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

    at.oob(AT_MQTT_CONNECT_GOTIP,
           AT_MQTT_ICA_POSTFIX,
           AT_MQTT_CMD_MAX_LEN,
           at_mqtt_gotip_callback,
           NULL);

    return at_mqtt_register(&g_at_mqtt_ica_client);
}

int at_mqtt_ica_client_deinit(void)
{
    if (aos_sem_is_valid(&g_ica_rsp_sem)) {
        aos_sem_free(&g_ica_rsp_sem);
    }

    aos_free(g_ica_rsp_buff);
    g_ica_rsp_buff = NULL;

    g_mqtt_state.auto_report_flag = 0;
    g_mqtt_state.mqtt_state       = 0;

    g_at_mqtt_state = AT_MQTT_STATE_IDLE;

    return at_mqtt_unregister();
}


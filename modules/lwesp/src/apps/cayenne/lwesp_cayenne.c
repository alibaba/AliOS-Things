/**
 * \file            lwesp_cayenne.c
 * \brief           MQTT client for Cayenne
 */

/*
 * Copyright (c) 2022 Tilen MAJERLE
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * This file is part of LwESP - Lightweight ESP-AT parser library.
 *
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
 * Version:         v1.1.2-dev
 */
#include "lwesp/apps/lwesp_cayenne.h"
#include "lwesp/apps/lwesp_cayenne_evt.h"
#include "lwesp/apps/lwesp_mqtt_client.h"
#include "lwesp/apps/lwesp_mqtt_client_evt.h"
#include "lwesp/lwesp_mem.h"
#include "lwesp/lwesp_pbuf.h"

#if !defined(LWESP_DEV)
/* This part is not fixed and may change in the future, hence only available when LWESP_DEV development macro is enabled */
#error "This app is under development and shall not be used in real application. Its API may change in the future"
#endif /* !defined(LWESP_DEV) */

#if !LWESP_CFG_NETCONN || !LWESP_CFG_MODE_STATION
#error "Netconn and station mode must be enabled!"
#endif /* !LWESP_CFG_NETCONN || !LWESP_CFG_MODE_STATION */

/* Tracing debug message */
#define LWESP_CFG_DBG_CAYENNE_TRACE             (LWESP_CFG_DBG_CAYENNE | LWESP_DBG_TYPE_TRACE)
#define LWESP_CFG_DBG_CAYENNE_TRACE_WARNING     (LWESP_CFG_DBG_CAYENNE | LWESP_DBG_TYPE_TRACE | LWESP_DBG_LVL_WARNING)
#define LWESP_CFG_DBG_CAYENNE_TRACE_SEVERE      (LWESP_CFG_DBG_CAYENNE | LWESP_DBG_TYPE_TRACE | LWESP_DBG_LVL_SEVERE)

#define LWESP_CAYENNE_API_VERSION_LEN           (sizeof(LWESP_CAYENNE_API_VERSION) - 1)
#define CHECK_FORWARDSLASH_AND_FORWARD(str)     do { if ((str) == NULL || *(str) != '/') { return lwespERR; } ++(str); } while (0)

/**
 * \brief           Data type and unit descriptor information
 */
typedef struct {
    const char* tname;                          /*!< Data type name */
    const char* tvalue;                         /*!< Data type cayenne value*/
    const char* uname;                          /*!< Data unit name */
    const char* uvalue;                         /*!< Data unit value */
} cayenne_data_type_unit_desc_t;

/**
 * \brief           Data type and unit descriptor array
 */
static const cayenne_data_type_unit_desc_t data_type_unit_descs[] = {
#define LWESP_CAYENNE_DATA_TYPE_DEFINE(tname, tconst, tvalue, uname, uconst, uvalue)    {tname, tvalue, uname, uvalue},
#include "lwesp/apps/lwesp_cayenne_macros.h"
};

/**
 * \brief           Topic type and string key-value pair structure
 */
typedef struct {
    lwesp_cayenne_topic_t topic;                /*!< Topic name */
    const char* str;                            /*!< Topic string */
} topic_cmd_str_pair_t;

/**
 * \brief           List of key-value pairs for topic type and string
 */
static const topic_cmd_str_pair_t
topic_cmd_str_pairs[] = {
    { LWESP_CAYENNE_TOPIC_DATA, "data" },
    { LWESP_CAYENNE_TOPIC_COMMAND, "cmd" },
    { LWESP_CAYENNE_TOPIC_CONFIG, "conf" },
    { LWESP_CAYENNE_TOPIC_RESPONSE, "response" },
    { LWESP_CAYENNE_TOPIC_SYS_MODEL, "sys/model" },
    { LWESP_CAYENNE_TOPIC_SYS_VERSION, "sys/version" },
    { LWESP_CAYENNE_TOPIC_SYS_CPU_MODEL, "sys/cpu/model" },
    { LWESP_CAYENNE_TOPIC_SYS_CPU_SPEED, "sys/cpu/speed" },
    { LWESP_CAYENNE_TOPIC_DIGITAL, "digital" },
    { LWESP_CAYENNE_TOPIC_DIGITAL_COMMAND, "digital-cmd" },
    { LWESP_CAYENNE_TOPIC_DIGITAL_CONFIG, "digital-conf" },
    { LWESP_CAYENNE_TOPIC_ANALOG, "analog" },
    { LWESP_CAYENNE_TOPIC_ANALOG_COMMAND, "analog-cmd" },
    { LWESP_CAYENNE_TOPIC_ANALOG_CONFIG, "analog-conf" }
};
static char topic_name[256];                    /*!< Topic name for publish/subscribe */
static char payload_data[128];                  /*!< Payload data */
static lwesp_cayenne_t* first_cayenne;          /*!< Linked list of all cayenne objects */

/**
 * \brief           Parse received topic string and verify if received string format is expected by the application
 * \param[in]       c: Cayenne handle object
 * \param[in]       topic: Topic string received over MQTT
 * \param[in]       topic_len: Length of topic string in units of bytes
 * \return          \ref lwespOK on success, member of \ref lwespr_t otherwise
 */
lwespr_t
lwesp_cayenne_parse_topic(lwesp_cayenne_t* c, const char* topic, size_t topic_len) {
    lwesp_cayenne_msg_t* msg;
    size_t len;

    LWESP_ASSERT("c != NULL", c != NULL);
    LWESP_ASSERT("topic != NULL", topic != NULL);
    LWESP_ASSERT("topic_len > 0", topic_len > 0);

    LWESP_DEBUGF(LWESP_CFG_DBG_CAYENNE_TRACE, "[LWESP CAYENNE] Parsing received topic: %.*s\r\n", (int)topic_len, topic);
    msg = &c->msg;                              /* Get message handle */

    /* Topic starts with API version */
    if (strncmp(topic, LWESP_CAYENNE_API_VERSION, LWESP_CAYENNE_API_VERSION_LEN) != 0) {
        return lwespERR;
    }
    topic += LWESP_CAYENNE_API_VERSION_LEN;
    CHECK_FORWARDSLASH_AND_FORWARD(topic);

    /* Check username */
    len = strlen(c->info_c->user);
    if (strncmp(topic, c->info_c->user, len) != 0) {
        return lwespERR;
    }
    topic += len;

    /* Check for /things/ */
    len = sizeof("/things/") - 1;
    if (strncmp(topic, "/things/", len) != 0) {
        return lwespERR;
    }
    topic += len;

    /* Check client ID */
    len = strlen(c->info_c->id);
    if (strncmp(topic, c->info_c->id, len) != 0) {
        return lwespERR;
    }
    topic += len;
    CHECK_FORWARDSLASH_AND_FORWARD(topic);

    /* Now parse topic string */
    msg->topic = LWESP_CAYENNE_TOPIC_END;
    for (size_t i = 0; i < LWESP_ARRAYSIZE(topic_cmd_str_pairs); ++i) {
        len = strlen(topic_cmd_str_pairs[i].str);
        if (strncmp(topic_cmd_str_pairs[i].str, topic, len) == 0) {
            msg->topic = topic_cmd_str_pairs[i].topic;
            topic += len;
            break;
        }
    }

    /* This means error, no topic found */
    if (msg->topic == LWESP_CAYENNE_TOPIC_END) {
        return lwespERR;
    }

    /* Parse channel */
    msg->channel = LWESP_CAYENNE_NO_CHANNEL;
    if (*topic == '/') {
        ++topic;
        if (*topic == '+') {
            msg->channel = LWESP_CAYENNE_ALL_CHANNELS;
        } else if (*topic == '#') {
            msg->channel = LWESP_CAYENNE_ALL_CHANNELS;
        } else if (*topic >= '0' && *topic <= '9') {
            msg->channel = 0;
            while (*topic >= '0' && *topic <= '9') {
                msg->channel = (10 * msg->channel) + (*topic - '0');
                ++topic;
            }
        } else {
            return lwespERR;
        }
    }
    return lwespOK;
}

/**
 * \brief           Parse received data from MQTT channel
 * \param[in]       c: Cayenne handle object
 * \param[in]       payload: Payload string received over MQTT
 * \param[in]       payload_len: Length of payload string in units of bytes
 * \return          \ref lwespOK on success, member of \ref lwespr_t otherwise
 */
static lwespr_t
lwesp_cayenne_parse_payload(lwesp_cayenne_t* c, const char* payload, size_t payload_len) {
    lwesp_cayenne_msg_t* msg;

    LWESP_ASSERT("c != NULL", c != NULL);
    LWESP_ASSERT("payload != NULL", payload != NULL);
    LWESP_ASSERT("payload_len > 0", payload_len > 0);

    LWESP_DEBUGF(LWESP_CFG_DBG_CAYENNE_TRACE, "[LWESP CAYENNE] Parsing received payload: %.*s\r\n", (int)payload_len, payload);

    msg = &c->msg;                              /* Get message handle */
    msg->seq = NULL;                            /* Reset sequence string */

    /* Parse topic format here */
    switch (msg->topic) {
        case LWESP_CAYENNE_TOPIC_DATA: {
            LWESP_DEBUGF(LWESP_CFG_DBG_CAYENNE_TRACE, "[LWESP CAYENNE] Topic is data\r\n");
            /* Parse data with '=' separator */
            break;
        }
        case LWESP_CAYENNE_TOPIC_COMMAND:
        case LWESP_CAYENNE_TOPIC_ANALOG_COMMAND:
        case LWESP_CAYENNE_TOPIC_DIGITAL_COMMAND: {
            /* Parsing "sequence,value" */
            char* comm = strchr(payload, ',');
            if (comm != NULL) {
                /* Fill the buffer */
                msg->seq = payload;
                msg->values[0].key = NULL;
                msg->values[0].key_len = 0;
                msg->values[0].value = comm + 1;
                msg->values[0].value_len = payload_len - (comm - payload + 1);
                msg->values_count = 1;
            } else {
                return lwespERR;
            }
            break;
        }
        case LWESP_CAYENNE_TOPIC_ANALOG: {
            LWESP_DEBUGF(LWESP_CFG_DBG_CAYENNE_TRACE, "[LWESP CAYENNE] Topic is analog\r\n");
            /* Here parse type,value */
        }
        default:
            break;
    }
    return lwespOK;
}

/**
 * \brief           Build topic string based on input parameters
 * \param[in]       c: Cayenne handle object
 * \param[in]       topic_str: Output variable for created topic
 * \param[in]       topic_str_len: Length of topic_str param including NULL termination
 * \param[in]       topic: Cayenne topic
 * \param[in]       channel: Cayenne channel
 * \return          \ref lwespOK on success, member of \ref lwespr_t otherwise
 */
static lwespr_t
lwesp_cayenne_build_topic(lwesp_cayenne_t* c, char* topic_str, size_t topic_str_len,
                          lwesp_cayenne_topic_t topic, uint16_t channel) {
    size_t rem_len;
    char ch_token[6];

    LWESP_ASSERT("c != NULL", c != NULL);
    LWESP_ASSERT("topic_str != NULL", topic_str != NULL);
    LWESP_ASSERT("topic_str_len > 0", topic_str_len > 0);
    LWESP_ASSERT("topic < LWESP_CAYENNE_TOPIC_END", topic < LWESP_CAYENNE_TOPIC_END);

    /*
     * Assert for basic part without topic
     *
     * Min required size of buffer should contain space at least for basic parts of the topic in format
     *
     * "v1/user_id/things/client_id/"
     *
     * Number 11 contains: "/" + "/things/" + "/" + NULL termination
     */
    LWESP_ASSERT("topic_str_len > min_string_length", topic_str_len > (strlen(LWESP_CAYENNE_API_VERSION) + strlen(c->info_c->user) + strlen(c->info_c->id) + 11));
    topic_str[0] = 0;

    /* Base part */
    strcat(topic_str, LWESP_CAYENNE_API_VERSION);
    strcat(topic_str, "/");
    strcat(topic_str, c->info_c->user);
    strcat(topic_str, "/things/");
    strcat(topic_str, c->info_c->id);
    strcat(topic_str, "/");
    rem_len = topic_str_len - strlen(topic_str) - 1;

    /* Topic string */
    for (size_t i = 0; i < LWESP_ARRAYSIZE(topic_cmd_str_pairs); ++i) {
        if (topic == topic_cmd_str_pairs[i].topic) {
            LWESP_ASSERT("strlen(topic_cmd_str_pairs[i].str) <= rem_len", strlen(topic_cmd_str_pairs[i].str) <= rem_len);
            strcat(topic_str, topic_cmd_str_pairs[i].str);
            break;
        }
    }
    rem_len = topic_str_len - strlen(topic_str) - 1;

    /* Append channel if necessary */
    if (channel != LWESP_CAYENNE_NO_CHANNEL) {
        if (channel == LWESP_CAYENNE_ALL_CHANNELS) {
            LWESP_ASSERT("rem_len >= 2", rem_len >= 2);
            strcat(topic_str, "/+");
        } else {
            LWESP_ASSERT("rem_len >= 1", rem_len >= 1);
            lwesp_u16_to_str(channel, ch_token);
            strcat(topic_str, "/");
            LWESP_ASSERT("strlen(ch_token) <= rem_len", strlen(ch_token) <= rem_len - 1);
            strcat(topic_str, ch_token);
        }
    }
    LWESP_DEBUGF(LWESP_CFG_DBG_CAYENNE_TRACE, "[LWESP CAYENNE] Final topic string: %s\r\n", topic_name);
    return lwespOK;
}

/**
 * \brief           Try to send TX data to Cayenne cloud.
 * Take the data from internal TX message buffer, build topic and go to cloud
 * \param[in]       c: Cayenne handle object
 */
static void
prv_try_send_data(lwesp_cayenne_t* c) {
    const lwesp_cayenne_tx_msg_t* msg;
    lwespr_t res;
    uint8_t try_again = 1, should_skip = 1;

    if (!lwesp_mqtt_client_is_connected(c->mqtt_client)) {
        return;
    }

    /* Try to send as many messages as possible in one shot */
    while (try_again && lwesp_buff_get_linear_block_read_length(&c->tx_buff) >= sizeof(*msg)) {
        msg = lwesp_buff_get_linear_block_read_address(&c->tx_buff);

        /* Setup topic first, then follow with data setup */
        should_skip = 1;
        try_again = 0;
        if ((res = lwesp_cayenne_build_topic(c, topic_name, sizeof(topic_name), msg->topic, msg->channel)) == lwespOK) {
            const cayenne_data_type_unit_desc_t* dtu = NULL;
            size_t len;

            /* Get data type and unit descriptor */
            if (msg->data_type_unit < LWESP_CAYENNE_DATA_TYPE_END_UNIT_END) {
                dtu = &data_type_unit_descs[(size_t)msg->data_type_unit];
            }
            payload_data[0] = 0;

            /* Describe data with data type and unit information */
            if (dtu != NULL) {
                if (dtu->tvalue != NULL) {
                    strcat(payload_data, dtu->tvalue);
                    if (dtu->uvalue != NULL) {
                        strcat(payload_data, ",");
                    }
                }
                if (dtu->uvalue != NULL) {
                    strcat(payload_data, dtu->uvalue);
                }
            }
            if ((len = strlen(payload_data)) > 0) {
                strcat(payload_data, "=");
                ++len;
            }

            /* Format data to string */
            switch (msg->data_format) {
                case LWESP_CAYENNE_DATA_FORMAT_UINT32: {
                    sprintf(&payload_data[len], "%lu", (unsigned long)msg->data.u32);
                    break;
                }
                case LWESP_CAYENNE_DATA_FORMAT_INT32: {
                    sprintf(&payload_data[len], "%ld", (long)msg->data.i32);
                    break;
                }
                case LWESP_CAYENNE_DATA_FORMAT_FLOAT:
                case LWESP_CAYENNE_DATA_FORMAT_DOUBLE: {
                    sprintf(&payload_data[len], "%.5f", msg->data.flt);
                    break;
                }
                case LWESP_CAYENNE_DATA_FORMAT_STRING: {
                    sprintf(&payload_data[len], "%s", msg->data.str);
                    break;
                }
                default: {
                    strcpy(payload_data, "unknown");
                    break;
                }
            }
            LWESP_DEBUGF(LWESP_CFG_DBG_CAYENNE_TRACE, "[LWESP CAYENNE] Trying to publish. Topic: %s, payload: %s\r\n", topic_name, payload_data);

            /* Start protocol transmission */
            if ((res = lwesp_mqtt_client_publish(c->mqtt_client, topic_name, payload_data, strlen(payload_data), LWESP_MQTT_QOS_AT_LEAST_ONCE, 0, NULL)) == lwespOK) {
                try_again = 1;
                should_skip = 1;
            } else if (res == lwespERRMEM) {
                try_again = 0;
                should_skip = 0;
            } else {
                try_again = 0;
                should_skip = 1;
            }
            LWESP_DEBUGW(LWESP_CFG_DBG_CAYENNE_TRACE, res == lwespOK,
                         "[LWESP CAYENNE] Publishing to Cayenne started successfully\r\n");
            LWESP_DEBUGW(LWESP_CFG_DBG_CAYENNE_TRACE_WARNING, res != lwespOK,
                         "[LWESP CAYENNE] Failed to start data publish, error code: %d\r\n", (int)res);
        } else {
            LWESP_DEBUGW(LWESP_CFG_DBG_CAYENNE_TRACE_WARNING, res != lwespOK,
                         "[LWESP CAYENNE] Failed to build topic to publish data, error code: %d\r\n", (int)res);
        }
        if (should_skip) {
            lwesp_buff_skip(&c->tx_buff, sizeof(*msg));
        }
    }
}

/**
 * \brief           Global MQTT client event callback function
 * \param[in]       client: MQTT client object handle
 * \param[in]       evt: Structure with event information
 */
static void
prv_mqtt_client_evt_cb(lwesp_mqtt_client_p client, lwesp_mqtt_evt_t* evt) {
    lwesp_cayenne_t* c = lwesp_mqtt_client_get_arg(client);
    switch (lwesp_mqtt_client_evt_get_type(client, evt)) {
        /*
         * Connect event at MQTT connection level.
         *
         * Called if user successfully connected to MQTT server (and ready to subscribe/publish)
         * or if connection failed for some reason.
         */
        case LWESP_MQTT_EVT_CONNECT: {          /* MQTT connect event occurred */
            lwesp_mqtt_conn_status_t status = lwesp_mqtt_client_evt_connect_get_status(client, evt);
            if (status == LWESP_MQTT_CONN_STATUS_ACCEPTED) {
                lwespr_t res;
                LWESP_DEBUGF(LWESP_CFG_DBG_CAYENNE_TRACE,
                             "[LWESP CAYENNE] Connection accepted, time to subscribe\r\n");
                lwesp_cayenne_build_topic(c, topic_name, sizeof(topic_name), LWESP_CAYENNE_TOPIC_COMMAND, LWESP_CAYENNE_ALL_CHANNELS);
                if ((res = lwesp_mqtt_client_subscribe(client, topic_name, LWESP_MQTT_QOS_AT_LEAST_ONCE, (void*)LWESP_CAYENNE_TOPIC_COMMAND)) != lwespOK) {
                    LWESP_DEBUGF(LWESP_CFG_DBG_CAYENNE_TRACE_WARNING,
                                 "[LWESP CAYENNE] Could not subscribe to first topic, error code: %d\r\n", (int)res);
                }
            } else {
                LWESP_DEBUGF(LWESP_CFG_DBG_CAYENNE_TRACE_WARNING,
                             "[LWESP CAYENNE] Could not establish connection with server, status code: %d\r\n", (int)status);
                lwesp_cayenne_connect(c);       /* Try to connect again */
            }
            break;
        }

        /* Subscribe event completed */
        case LWESP_MQTT_EVT_SUBSCRIBE: {
            lwespr_t res;
            LWESP_DEBUGF(LWESP_CFG_DBG_CAYENNE_TRACE, "[LWESP CAYENNE] Subscribe event\r\n");
            if ((res = lwesp_mqtt_client_evt_subscribe_get_result(client, evt)) == lwespOK) {
                lwesp_cayenne_topic_t next_topic = (lwesp_cayenne_topic_t)lwesp_mqtt_client_evt_subscribe_get_argument(client, evt);

                /* Get the new topic to subscribe to, based on previous one, from argument */
                switch (next_topic) {
                    case LWESP_CAYENNE_TOPIC_COMMAND: {
                        next_topic = LWESP_CAYENNE_TOPIC_CONFIG;
                        break;
                    }
                    default: {
                        next_topic = LWESP_CAYENNE_TOPIC_END;
                        break;
                    }
                }

                /* Subscribe only if necessary, otherwise end at this point */
                if (next_topic < LWESP_CAYENNE_TOPIC_END) {
                    lwesp_cayenne_build_topic(c, topic_name, sizeof(topic_name), next_topic, LWESP_CAYENNE_ALL_CHANNELS);
                    if ((res = lwesp_mqtt_client_subscribe(client, topic_name, LWESP_MQTT_QOS_AT_LEAST_ONCE, (void*)next_topic)) != lwespOK) {
                        LWESP_DEBUGF(LWESP_CFG_DBG_CAYENNE_TRACE_WARNING,
                                     "[LWESP CAYENNE] Could not subscribe to first topic, error code: %d\r\n", (int)res);
                    }
                } else {
                    LWESP_DEBUGF(LWESP_CFG_DBG_CAYENNE_TRACE,
                                 "[LWESP CAYENNE] Successfully subscribed to all necessary channels\r\n");

                    /* Send notification to user as being successfully connected */
                    c->evt.type = LWESP_CAYENNE_EVT_CONNECT;
                    c->evt_fn(c, &c->evt);

                    prv_try_send_data(c);       /* Try to send data, if anything */
                }
            } else {
                LWESP_DEBUGF(LWESP_CFG_DBG_CAYENNE_TRACE_WARNING,
                             "[LWESP CAYENNE] Failed to subscribe to topic with, error code: %d\r\n", (int)res);
            }
            break;
        }

        /* Message published event occurred from client to server */
        case LWESP_MQTT_EVT_PUBLISH: {
            prv_try_send_data(c);               /* Try to send more data */
            break;
        }

        /* Server has published a message to client */
        case LWESP_MQTT_EVT_PUBLISH_RECV: {
            const char* topic = lwesp_mqtt_client_evt_publish_recv_get_topic(client, evt);
            size_t topic_len = lwesp_mqtt_client_evt_publish_recv_get_topic_len(client, evt);
            const uint8_t* payload = lwesp_mqtt_client_evt_publish_recv_get_payload(client, evt);
            size_t payload_len = lwesp_mqtt_client_evt_publish_recv_get_payload_len(client, evt);

            LWESP_DEBUGF(LWESP_CFG_DBG_CAYENNE_TRACE, "[LWESP CAYENNE] Publish rcved\r\n");
            LWESP_DEBUGF(LWESP_CFG_DBG_CAYENNE_TRACE, "[LWESP CAYENNE] Publish rcv topic: %.*s\r\n", (int)topic_len, (const void*)topic);
            LWESP_DEBUGF(LWESP_CFG_DBG_CAYENNE_TRACE, "[LWESP CAYENNE] Publish rcv data : %.*s\r\n", (int)payload_len, (const void*)payload);

            /* Try to parse received topic and respective payload */
            if (lwesp_cayenne_parse_topic(c, topic, topic_len) == lwespOK
                && lwesp_cayenne_parse_payload(c, (const void*)payload, payload_len) == lwespOK) {

                LWESP_DEBUGF(LWESP_CFG_DBG_CAYENNE_TRACE, "[LWESP CAYENNE] Topic and payload parsed. Channel: %d, Sequence: %s, Key: %s, Value: %s\r\n",
                             (int)c->msg.channel, c->msg.seq, c->msg.values[0].key, c->msg.values[0].value);

                /* Send notification to user */
                c->evt.type = LWESP_CAYENNE_EVT_DATA;
                c->evt.evt.data.msg = &c->msg;
                c->evt_fn(c, &c->evt);
            }
            break;
        }

        /* Client is fully disconnected from MQTT server */
        case LWESP_MQTT_EVT_DISCONNECT: {
            LWESP_DEBUGF(LWESP_CFG_DBG_CAYENNE_TRACE,
                         "[LWESP CAYENNE] MQTT client disconnected. Trying to reconnect\r\n");

            /* Send notification to user as being successfully connected */
            c->evt.type = LWESP_CAYENNE_EVT_DISCONNECT;
            c->evt_fn(c, &c->evt);
            lwesp_cayenne_connect(c);       /* Try to re-connect */
            break;
        }

        /* Keep-alive event at MQTT communication level */
        case LWESP_MQTT_EVT_KEEP_ALIVE: {
            break;
        }

        /* Internal connection poll event */
        case LWESP_MQTT_EVT_CONN_POLL: {
            prv_try_send_data(c);           /* Try to send data */
            break;
        }
        default:
            break;
    }
}

/**
 * \brief           Global stack event callback
 * \param[in]       evt: Structure with event information
 */
static lwespr_t
prv_global_evt_cb(lwesp_evt_t* evt) {
    switch (lwesp_evt_get_type(evt)) {
        case LWESP_EVT_WIFI_GOT_IP: {
            LWESP_DEBUGF(LWESP_CFG_DBG_CAYENNE_TRACE,
                         "[LWESP CAYENNE] Wifi got IP, let's try to connect to devices\r\n");
            for (lwesp_cayenne_t* t = first_cayenne; t != NULL; t = t->next) {
                lwesp_cayenne_connect(t);
            }
            break;
        }
        default:
            break;
    }
    return lwespOK;
}

/**
 * \brief           Initialize cayenne module.
 * Call this only once in the application lifetime
 * \return          \ref lwespOK on success, member of \ref lwespr_t otherwise
 */
lwespr_t
lwesp_cayenne_init(void) {
    first_cayenne = NULL;
    lwesp_evt_register(prv_global_evt_cb);
    return lwespOK;
}

/**
 * \brief           Create new instance of cayenne MQTT connection
 * \note            Each call to this functions starts new thread for async receive processing.
 *                  Function will block until thread is created and successfully started
 * \param[in]       c: Cayenne handle object
 * \param[in]       client_info: MQTT client info with username, password and client ID.
 *                      This is used for read-only purpose and can be reused by different clients
 * \param[in]       evt_fn: Event function to send information to user upon different activity
 * \return          \ref lwespOK on success, member of \ref lwespr_t otherwise
 */
lwespr_t
lwesp_cayenne_create(lwesp_cayenne_t* c, const lwesp_mqtt_client_info_t* client_info, lwesp_cayenne_evt_fn evt_fn) {
    LWESP_ASSERT("c != NULL", c != NULL);
    LWESP_ASSERT("client_info != NULL", client_info != NULL);
    LWESP_ASSERT("evt_fn != NULL", evt_fn != NULL);

    c->next = NULL;
    c->info_c = client_info;
    c->evt_fn = evt_fn;
    c->client_buff_tx_len = c->client_buff_tx_len > 0 ? c->client_buff_tx_len : LWESP_CAYENNE_BUFF_TX_LEN;
    c->client_buff_rx_len = c->client_buff_rx_len > 0 ? c->client_buff_rx_len : LWESP_CAYENNE_BUFF_RX_LEN;
    c->tx_buff_count = c->tx_buff_count > 0 ? c->tx_buff_count : LWESP_CAYENNE_BUFF_TX_COUNT;

    /*
     * Initialize the TX data buffer, in charge to store
     * messages being ready for transmission to Cayenne cloud
     */
    if (!lwesp_buff_init(&c->tx_buff, sizeof(lwesp_cayenne_tx_msg_t) * c->tx_buff_count)) {
        LWESP_DEBUGF(LWESP_CFG_DBG_CAYENNE_TRACE_SEVERE, "[LWESP CAYENNE] Could not allocate memory TX messages buffer\r\n");
        return lwespERRMEM;
    }
    /* Create new MQTT client object */
    if ((c->mqtt_client = lwesp_mqtt_client_new(c->client_buff_tx_len, c->client_buff_rx_len)) == NULL) {
        LWESP_DEBUGF(LWESP_CFG_DBG_CAYENNE_TRACE_SEVERE, "[LWESP CAYENNE] Could not allocate memory for new MQTT client object\r\n");
        lwesp_buff_free(&c->tx_buff);
        return lwespERRMEM;
    }
    lwesp_mqtt_client_set_arg(c->mqtt_client, c);

    /* Add object to linked list */
    lwesp_sys_protect();
    if (first_cayenne == NULL) {
        first_cayenne = c;
    } else {
        lwesp_cayenne_t* t;
        for (t = first_cayenne; t->next != NULL; t = t->next) {}
        t->next = c;
    }
    lwesp_sys_unprotect();

    /* Connect to server */
    lwesp_cayenne_connect(c);
    return lwespOK;
}

/**
 * \brief           Try to connect to Cayenne server over asynchronous MQTT module
 * Function does not block until connected, rather it only starts the connection to server
 *
 * \param[in]       c: Cayenne handle object
 * \return          \ref lwespOK on success, member of \ref lwespr_t otherwise
 */
lwespr_t
lwesp_cayenne_connect(lwesp_cayenne_t* c) {
    lwespr_t res;
    LWESP_ASSERT("c != NULL", c != NULL);

    lwesp_sys_protect();
    if (lwesp_mqtt_client_is_connected(c->mqtt_client)) {
        res = lwespOK;
    } else {
        res = lwesp_mqtt_client_connect(c->mqtt_client, LWESP_CAYENNE_HOST, LWESP_CAYENNE_PORT, prv_mqtt_client_evt_cb, c->info_c);
    }
    lwesp_sys_unprotect();
    return res;
}

/**
 * \brief           Publish data to Cayenne cloud (extended mode), with sensor info or actuator data.
 * It writes data to TX message buffer and later tries to start transmission from it.
 * If transmission is not succesful, it will re-try from the poll event.
 * \param[in]       c: Cayenne handle object
 * \param[in]       tx_msg: Pointer to data object to transmit
 * \return          \ref lwespOK on success, member of \ref lwespr_t otherwise
 */
lwespr_t
lwesp_cayenne_publish_ex(lwesp_cayenne_t* c, const lwesp_cayenne_tx_msg_t* tx_msg) {
    lwespr_t res = lwespOK;

    LWESP_ASSERT("c != NULL", c != NULL);
    LWESP_ASSERT("tx_msg != NULL", tx_msg != NULL);

    lwesp_sys_protect();
    if (lwesp_buff_get_free(&c->tx_buff) >= sizeof(*tx_msg)) {
        lwesp_buff_write(&c->tx_buff, tx_msg, sizeof(*tx_msg));
        prv_try_send_data(c);
    } else {
        res = lwespERRMEM;
    }
    lwesp_sys_unprotect();
    LWESP_DEBUGW(LWESP_CFG_DBG_CAYENNE_TRACE_WARNING, res != lwespOK,
                 "[LWESP CAYENNE] Failed to write message to TX buffer, error code: %d\r\n", (int)res);
    return res;
}

/**
 * \brief           Publish response message to command received from cloud
 * \param[in]       c: Cayenne handle
 * \param[in]       msg: Received message with command topic
 * \param[in]       resp: Response type, either `OK` or `ERROR`. Member of \ref lwesp_cayenne_resp_t enumeration
 * \param[in]       message: Message text in case of error to be displayed to Cayenne dashboard
 * \return          \ref lwespOK on success, member of \ref lwespr_t otherwise
 */
lwespr_t
lwesp_cayenne_publish_response(lwesp_cayenne_t* c, lwesp_cayenne_msg_t* msg, lwesp_cayenne_resp_t resp, const char* message) {
    lwespr_t res;

    LWESP_ASSERT("c != NULL", c != NULL);
    LWESP_ASSERT("msg != NULL && msg->seq != NULL", msg != NULL && msg->seq != NULL);

    lwesp_sys_protect();
    if ((res = lwesp_cayenne_build_topic(c, topic_name, sizeof(topic_name), LWESP_CAYENNE_TOPIC_RESPONSE, LWESP_CAYENNE_NO_CHANNEL)) == lwespOK) {
        size_t len, msg_len;

        payload_data[0] = 0;
        sprintf(payload_data, "%s,%s", resp == LWESP_CAYENNE_RESP_OK ? "ok" : "error", msg->seq);
        if (resp != LWESP_CAYENNE_RESP_OK) {
            strcat(payload_data, "=");
            len = strlen(payload_data);
            msg_len = strlen(message);
            if (msg_len > (sizeof(payload_data) - 1 - len)) {
                msg_len = sizeof(payload_data) - 1 - len;
            }
            strncpy(&payload_data[len], message, msg_len);
            payload_data[len + msg_len] = 0;
        }
        if ((res = lwesp_mqtt_client_publish(c->mqtt_client, topic_name, payload_data, strlen(payload_data), LWESP_MQTT_QOS_AT_LEAST_ONCE, 0, NULL)) != lwespOK) {
            LWESP_DEBUGW(LWESP_CFG_DBG_CAYENNE_TRACE_WARNING, res != lwespOK,
                         "[LWESP CAYENNE] Failed to publish response to, error code: %d\r\n", (int)res);
        }
    } else {
        LWESP_DEBUGW(LWESP_CFG_DBG_CAYENNE_TRACE_WARNING, res != lwespOK,
                     "[LWESP CAYENNE] Failed to build topic to publish response, error code: %d\r\n", (int)res);
    }
    lwesp_sys_unprotect();
    return res;
}
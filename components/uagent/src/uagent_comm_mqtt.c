/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "uagent_config.h"
#include "uagent_inner_api.h"
#include "aos/kernel.h"
#include "aos/errno.h"
#include "core_mqtt.h"

static char mqtt_topic_up[UAGENT_TOPIC_MAX_LEN];
static char mqtt_topic_dn[UAGENT_TOPIC_MAX_LEN];
static char mqtt_topic_up_reply[UAGENT_TOPIC_MAX_LEN];
static on_recv_handler recv_handler = NULL;
static char mqtt_comm_init = 0;
static void *mqtt_client = NULL;


static void uagent_up_reply_handler(void *handle, const aiot_mqtt_recv_t *packet, void *userdata)
{

}

static void uagent_handler(void *handle, const aiot_mqtt_recv_t *packet, void *userdata)
{
     switch (packet->type) {
        case AIOT_MQTTRECV_PUB: {
            /* print topic name and topic message */
            UAGENT_DEBUG("Topic: '%.*s' (Length: %d)",
                packet->data.pub.topic_len,
                packet->data.pub.topic,
                packet->data.pub.topic_len);
            UAGENT_DEBUG("Payload: '%.*s' (Length: %d)",
                packet->data.pub.payload_len,
                packet->data.pub.payload,
                packet->data.pub.payload_len);
                if (NULL != packet->data.pub.payload) {
                    recv_handler(packet->data.pub.payload_len , packet->data.pub.payload);
                }
        }
        break;
        default: {
        }
        break;
    }

}

int uagent_ext_comm_init(const char pk[12], const char dn[36], on_recv_handler handler)
{
    int rc = -EINVAL;
    /* todo retry if construct fail or subscribe fail */
    if (0==mqtt_comm_init && NULL != pk && NULL != dn) {
        UAGENT_INFO("[uA]prepare start uagent comm %s %s\n", pk, dn);
        /* Construct a MQTT client with default parameter */
        if (mqtt_client != NULL) {
            snprintf(mqtt_topic_up, UAGENT_TOPIC_MAX_LEN, TOPIC_AGENT_INFO, pk, dn);
            snprintf(mqtt_topic_dn, UAGENT_TOPIC_MAX_LEN, TOPIC_AGENT_MAN, pk, dn);
            snprintf(mqtt_topic_up_reply, UAGENT_TOPIC_MAX_LEN, TOPIC_AGENT_POST_REPLY, pk, dn);
#ifdef SP_BROADCAST_TOPIC
            char broadcast_topic_dn[UAGENT_TOPIC_MAX_LEN];
            snprintf(broadcast_topic_dn, UAGENT_TOPIC_MAX_LEN, "/broadcast/%s/deviceman", pk);
#endif
            recv_handler = handler;
            UAGENT_INFO("[uA]Subsrcibe TOPIC %s\r\n", mqtt_topic_dn);
            /* Subscribe the specific topic */
            rc = aiot_mqtt_sub(mqtt_client, mqtt_topic_dn , uagent_handler, CORE_MQTT_QOS0, NULL);
            if (rc < 0) {
                UAGENT_ERR("[uA]IOT_MQTT_Subscribe(%s) failed, rc = %d\r\n", mqtt_topic_dn, rc);
            } else {
                UAGENT_INFO("[uA]IOT_MQTT_Subscribe(%s) success\r\n", mqtt_topic_dn);
                rc = 0;
            }

            /* Subscribe the specific topic */
            rc = aiot_mqtt_sub(mqtt_client, mqtt_topic_up_reply, uagent_up_reply_handler, CORE_MQTT_QOS0 , NULL);
            if (rc < 0) {
                UAGENT_ERR("[uA]IOT_MQTT_Subscribe(%s) failed, rc = %d\r\n", mqtt_topic_up_reply, rc);
            } else {
                UAGENT_INFO("[uA]IOT_MQTT_Subscribe(%s) success\r\n", mqtt_topic_up_reply);
                rc = 0;
            }
#ifdef SP_BROADCAST_TOPIC
            UAGENT_INFO("[uA]Subsrcibe TOPIC %s", broadcast_topic_dn);
         //   rc = IOT_MQTT_Subscribe(NULL, broadcast_topic_dn, IOTX_MQTT_QOS1, uagent_handler, NULL);
            rc = aiot_mqtt_sub(mqtt_client, broadcast_topic_dn, uagent_handler, CORE_MQTT_QOS1 , NULL);
            if (rc < 0) {
                UAGENT_ERR("[uA]IOT_MQTT_Subscribe() failed, rc = %d\r\n", rc);
            } else {
                UAGENT_INFO("[uA]IOT_MQTT_Subscribe(%s) success\r\n", broadcast_topic_dn);
            }
#endif
            mqtt_comm_init = 1;
        } else {
            UAGENT_ERR("[uA]### MQTT construct Fail");
        }

    } else {
        UAGENT_ERR("[uA]invalid argument or uagent_comm already build");
    }
    return rc;

}

int uagent_ext_comm_publish(const unsigned short len, void* data, const bool conn_orient)
{

    int rc = 0;
    if (mqtt_client != NULL) {
        rc = aiot_mqtt_pub(mqtt_client, mqtt_topic_up, data, len, conn_orient ? CORE_MQTT_QOS1 : CORE_MQTT_QOS0);
    } else {
        UAGENT_ERR("[uA]mqtt client not init");
        rc = -1;
    }
    return rc;
}

char check_connect()
{
    char rc = 0;
    if (NULL != mqtt_client) {
        core_mqtt_handle_t *mqtt_handle = mqtt_client;
        if (mqtt_handle->disconnected == 0 && mqtt_handle->has_connected == 1) {
            rc = 1;
        } else {
            rc = 0;
        }
    }
    return rc;
}

int uagent_mqtt_client_set(void *mqtt_client_handle)
{
    int rc = 0;
    if (NULL != mqtt_client_handle) {
        mqtt_client = mqtt_client_handle;
        rc = 0;
    } else {
        UAGENT_ERR("[uA] mqtt client is NULL");
        rc = -1;
    }
    return rc;
}

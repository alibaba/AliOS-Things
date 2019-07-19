/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mqtt_api.h"
#include "uagent_config.h"
#include "uagent_inner_api.h"
#include "aos/kernel.h"
#include "aos/errno.h"

static char mqtt_topic_up[UAGENT_TOPIC_MAX_LEN];
static char mqtt_topic_dn[UAGENT_TOPIC_MAX_LEN];
static on_recv_handler recv_handler = NULL;
static char mqtt_comm_init = 0;
static void *mqtt_client = NULL;

static void uagent_handler(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg)
{
    iotx_mqtt_topic_info_pt     ptopic_info = (iotx_mqtt_topic_info_pt)msg->msg;
    UAGENT_DEBUG("---- UAGENT INFO RCV SUB TOPIC -----");
    switch (msg->event_type) {
    case IOTX_MQTT_EVENT_PUBLISH_RECEIVED:
        /* print topic name and topic message */
        UAGENT_DEBUG( "Topic: '%.*s' (Length: %d)",
            ptopic_info->topic_len,
            ptopic_info->ptopic,
            ptopic_info->topic_len);
        UAGENT_DEBUG( "Payload: '%.*s' (Length: %d)",
            ptopic_info->payload_len,
            ptopic_info->payload,
            ptopic_info->payload_len);
        if (ptopic_info->payload != NULL) {
            recv_handler(ptopic_info->payload_len, ptopic_info->payload);
        }
        break;

    case IOTX_MQTT_EVENT_DISCONNECT:
        UAGENT_ERR("[uA]### MQTT DISCONNECT");
        break;

    case IOTX_MQTT_EVENT_RECONNECT:
        UAGENT_INFO(">>> MQTT RECONNECT");
        break;
    default:
        UAGENT_INFO("Unknown Event Type %d", msg->event_type);
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
        if ((mqtt_client = IOT_MQTT_Construct(NULL)) != NULL) {
            snprintf(mqtt_topic_up, UAGENT_TOPIC_MAX_LEN, TOPIC_AGENT_INFO, pk, dn);
            snprintf(mqtt_topic_dn, UAGENT_TOPIC_MAX_LEN, TOPIC_AGENT_MAN, pk, dn);
#ifdef SP_BROADCAST_TOPIC
            char broadcast_topic_dn[UAGENT_TOPIC_MAX_LEN];
            snprintf(broadcast_topic_dn, UAGENT_TOPIC_MAX_LEN, "/broadcast/%s/deviceman", pk);
#endif
            recv_handler = handler;
            UAGENT_INFO("[uA]Subsrcibe TOPIC %s\r\n", mqtt_topic_dn);
            /* Subscribe the specific topic */
            rc = IOT_MQTT_Subscribe(NULL, mqtt_topic_dn, IOTX_MQTT_QOS0, uagent_handler, NULL);
            if (rc < 0) {
                UAGENT_ERR("[uA]IOT_MQTT_Subscribe(%s) failed, rc = %d\r\n", mqtt_topic_dn, rc);
            }
#ifdef SP_BROADCAST_TOPIC
            UAGENT_INFO("[uA]Subsrcibe TOPIC %s", broadcast_topic_dn);
            rc = IOT_MQTT_Subscribe(NULL, broadcast_topic_dn, IOTX_MQTT_QOS1, uagent_handler, NULL);
            if (rc < 0) {
                UAGENT_ERR("[uA]IOT_MQTT_Subscribe() failed, rc = %d\r\n", rc);
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
    return IOT_MQTT_Publish_Simple(NULL, mqtt_topic_up, conn_orient?IOTX_MQTT_QOS1:IOTX_MQTT_QOS0, data, len);
}

char check_connect()
{
    char rc = 0;
    if(NULL!=mqtt_client){
        rc = (1==IOT_MQTT_CheckStateNormal(mqtt_client))?1:0;
    }
    return rc;
}


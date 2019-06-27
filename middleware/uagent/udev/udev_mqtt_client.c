/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "aos/kernel.h"
#include "ulog/ulog.h"
#include "infra_compat.h"
#include "udev.h"
#include "udev_net.h"
#include "udev_mqtt_client.h"

static uint8_t mqtt_rbuf[MQTT_RBUF_SIZE] = {0};
static uint8_t mqtt_wbuf[MQTT_WBUF_SIZE] = {0};
static mc_message_handler_t message_handlers[MAX_MESSAGE_HANDLERS];
static unsigned int request_timeout_ms = 2000;
static int package_id = 1;
static int keepalive_interval = UDEV_KEEPALIVE;
static int ping_outtime = 0;
static char mqtt_token[48] = {0};

int keepalive(int fd)
{
    int rc = 0;
    static unsigned int now = 0;

    if (keepalive_interval == 0)
        return 0;

    if (aos_now_ms() >= now + keepalive_interval){
        now = aos_now_ms();

        if (ping_outtime)
            return -1; /* PINGRESP not received in keepalive interval */

        /* LOGI("udev", "send ping request\n"); */
        int len = MQTTSerialize_pingreq(mqtt_rbuf, MQTT_RBUF_SIZE);
        if (len > 0 && (rc = udev_send_packet(fd, mqtt_rbuf, len, 1000)) == 0) /* send the ping packet */
            ping_outtime = 0;
    }

    return rc;
}

/**
 * assume topic filter and name is in correct format
 * # can only be at end
 * + and # can only be next to separator
 */
static char isTopicMatched(char* topic_filter, mc_string_t* topic_name)
{
    char* curf = topic_filter;
    char* curn = topic_name->lenstring.data;
    char* curn_end = curn + topic_name->lenstring.len;

    while (*curf && curn < curn_end){
        if (*curn == '/' && *curf != '/')
            break;
        if (*curf != '+' && *curf != '#' && *curf != *curn)
            break;
        if (*curf == '+'){
            /* skip until we meet the next separator, or end of string */
            char* nextpos = curn + 1;
            while (nextpos < curn_end && *nextpos != '/')
                nextpos = ++curn + 1;
        }
        else if (*curf == '#')
            curn = curn_end - 1;    /* skip until end of string */
        curf++;
        curn++;
    };

    return (curn == curn_end) && (*curf == '\0');
}

int deliverMessage(int fd, mc_string_t* topic_name, mc_message_t* message)
{
    int i;
    int rc = -1;

    if(topic_name ==NULL || message == NULL){
        return rc;
    }

    /* we have to find the right message handler - indexed by topic */
    for (i = 0; i < MAX_MESSAGE_HANDLERS; ++i){
        /* LOGI("udev", "i:%d, topic:%s", i, (char*)message_handlers[i].topicFilter); */
        if (message_handlers[i].topic_filter != 0 && (MQTTPacket_equals(topic_name, (char*)message_handlers[i].topic_filter) ||
                isTopicMatched((char*)message_handlers[i].topic_filter, topic_name))){
            if (message_handlers[i].handler != NULL){
                message_handlers[i].handler(topic_name, message);
                rc = 0;
            }
        }
    }

    return rc;
}

static int decodePacket(int fd, int* value, int timeout)
{
    unsigned char i;
    int multiplier = 1;
    int len = 0;
    const int MAX_NO_OF_REMAINING_LENGTH_BYTES = 4;

    *value = 0;
    do{
        int rc = MQTTPACKET_READ_ERROR;

        if (++len > MAX_NO_OF_REMAINING_LENGTH_BYTES){
            rc = MQTTPACKET_READ_ERROR; /* bad data */
            goto exit;
        }
        rc = udev_net_read(fd, &i, 1, timeout);
        if (rc != 1)
            goto exit;
        *value += (i & 127) * multiplier;
        multiplier *= 128;
    } while ((i & 128) != 0);
exit:
    return len;
}

static int readPacket(int fd, unsigned int timeout)
{
    MQTTHeader header = {0};
    int len = 0;
    int rem_len = 0;

    /* 1. read the header byte.  This has the packet type in it */
    int rc = udev_net_read(fd, mqtt_rbuf, 1, timeout);
    if (rc != 1)
        goto exit;

    len = 1;
    /* 2. read the remaining length.  This is variable in itself */
    decodePacket(fd, &rem_len, timeout);
    len += MQTTPacket_encode(mqtt_rbuf + 1, rem_len); /* put the original remaining length back into the buffer */

    if (rem_len > (MQTT_RBUF_SIZE - len)){
        rc = -1;
        goto exit;
    }

    /* 3. read the rest of the buffer using a callback to supply the rest of the data */
    if (rem_len > 0 && (rc = udev_net_read(fd, mqtt_rbuf + len, rem_len, timeout) != rem_len)) {
        rc = 0;
        goto exit;
    }

    header.byte = mqtt_rbuf[0];
    rc =  MQTT_HEADER_GET_TYPE(header.byte);
    /*  rc = header.bits.type;*/
exit:
    return rc;
}

int cycle(int fd, unsigned int timeout)
{
    int len = 0,
        rc = 0;

    int packet_type = readPacket(fd, timeout);     /* read the socket, see what work is due */

    switch (packet_type){
        default:
            /* no more data to read, unrecoverable. Or read packet fails due to unexpected network error */
            rc = packet_type;
            goto exit;
        case 0: /* timed out reading packet */
            break;
        case CONNACK:
        case PUBACK:
        case SUBACK:
            break;
        case PUBLISH:
        {
            mc_string_t topicName;
            mc_message_t msg;
            int intQoS;
            msg.payloadlen = 0; /* this is a size_t, but deserialize publish sets this as int */
            /* LOGI("udev", "PUBLISH\n"); */
            if (MQTTDeserialize_publish(&msg.dup, &intQoS, &msg.retained, &msg.id, &topicName,
               (unsigned char**)&msg.payload, (int*)&msg.payloadlen, mqtt_rbuf, MQTT_RBUF_SIZE) != 1){
                    goto exit;
               }
            msg.qos = (mc_qos_e)intQoS;
            /* LOGE("udev", "deliverMessage"); */
            deliverMessage(fd, &topicName, &msg);
            if (msg.qos != QOS0){
                if (msg.qos == QOS1)
                    len = MQTTSerialize_ack(mqtt_wbuf, MQTT_WBUF_SIZE, PUBACK, 0, msg.id);
                else if (msg.qos == QOS2)
                    len = MQTTSerialize_ack(mqtt_wbuf, MQTT_WBUF_SIZE, PUBREC, 0, msg.id);
                if (len <= 0)
                    rc = -1;
                else
                    rc = udev_send_packet(fd, mqtt_wbuf, len, timeout);
                if (rc == -1)
                     goto exit; /* there was a problem */
            }
            break;
        }
        case PUBREC:
        case PUBREL:
        {
            unsigned short mypacketid;
            unsigned char dup, type;
            /* LOGI("udev", "PUBREL\n"); */
            if (MQTTDeserialize_ack(&type, &dup, &mypacketid, mqtt_rbuf, MQTT_RBUF_SIZE) != 1)
                rc = -1;
            else if ((len = MQTTSerialize_ack(mqtt_wbuf, MQTT_WBUF_SIZE,
                (packet_type == PUBREC) ? PUBREL : PUBCOMP, 0, mypacketid)) <= 0)
                rc = -1;
            /* send the PUBREL packet */
            else if ((rc = udev_send_packet(fd, mqtt_wbuf, len, timeout)) != 0)
                rc = -1; /* there was a problem */
            if (rc == -1)
                goto exit; /* there was a problem */
            break;
        }

        case PUBCOMP:
            break;
        case PINGRESP:
            /* c->ping_outstanding = 0; */
            break;
    }

    /* if (keepalive(c) != SUCCESS) {
     *     check only keepalive FAILURE status so that previous FAILURE status can be considered as FAULT
     *     rc = FAILURE;
     * }
     */

exit:
    if (rc == 0)
        rc = packet_type;
    /* else if (c->isconnected)
     *     MQTTCloseSession(c);
     */
    return rc;
}

int waitfor(int fd, int packet_type, unsigned int timeout)
{
    int rc = -1;
    unsigned int now = aos_now_ms() + timeout;

    do{
        rc = cycle(fd, timeout);
    }
    while (aos_now_ms() < now && rc != packet_type && rc >= 0);

    return rc;
}

int MQTTSetMessageHandler(const char* topic_filter, mc_handler_cb handler)
{
    int rc = 0;
    int i = -1;

    if (handler == NULL){
        return -1;
    }

    for (i = 0; i < MAX_MESSAGE_HANDLERS; ++i){
        if (message_handlers[i].topic_filter == NULL){
            rc = 0;
            break;
        }
    }

    if (i < MAX_MESSAGE_HANDLERS){
        message_handlers[i].topic_filter = topic_filter;
        message_handlers[i].handler = handler;
    }

    return rc;
}

int udev_mqtt_yield(int fd, int timeout)
{
    int rc = 0;
    unsigned int now = aos_now_ms() + timeout;

    do{
        /* LOGI("udev", "aos_now_ms:%d\n", aos_now_ms); */
        if((rc = cycle(fd, timeout)) < 0)
            break;
        if((rc = keepalive(fd)) < 0)
            break;
  	} while (aos_now_ms() < now);

    return rc;
}

int udev_mqtt_subscript(int fd, const char* topic_filter, mc_qos_e qos, mc_handler_cb handler)
{
    int rc = -1;
    int len = 0;
    mc_string_t topic = MQTTString_initializer;
    topic.cstring = (char *)topic_filter;
    unsigned int timeout = request_timeout_ms;
/** #if defined(MQTT_TASK)
 *      MutexLock(&c->mutex);
 * #endif
 */
    if (fd < 0)
        goto exit;
    int qos_int = (int)qos;
    len = MQTTSerialize_subscribe(mqtt_rbuf, MQTT_RBUF_SIZE, 0, package_id++, 1, &topic, (int*)&qos_int);
    if (len <= 0)
        goto exit;
    if ((rc = udev_send_packet(fd, mqtt_rbuf, len, timeout)) != 0) /* send the subscribe packet */
        goto exit;             /* there was a problem */

    /* wait for suback */
    if (waitfor(fd, SUBACK, timeout) == SUBACK){
        int count = 0;
        unsigned short mypacketid;
        int grantedQoS = QOS0;
        if (MQTTDeserialize_suback(&mypacketid, 1, &count, &grantedQoS, mqtt_rbuf, MQTT_RBUF_SIZE) == 1){
            if (grantedQoS != 0x80)
                rc = MQTTSetMessageHandler(topic_filter, handler);
        }
    }
    else
        rc = -1;

exit:
/**    if (rc == FAILURE)
 *         MQTTCloseSession(c);
 * #if defined(MQTT_TASK)
 *     MutexUnlock(&c->mutex);
 * #endif
 */
    return rc;
}


int udev_mqtt_publish(int fd, const char* topic_name, mc_message_t* message)
{
    int rc = -1;
    mc_string_t topic = MQTTString_initializer;
    topic.cstring = (char *)topic_name;
    int len = 0;

#if defined(MQTT_TASK)
	  MutexLock(&c->mutex);
#endif /* defined(MQTT_TASK) */
    if (fd < 0)
        goto exit;

    if (message->qos == QOS1 || message->qos == QOS2)
        message->id = package_id++;

    len = MQTTSerialize_publish(mqtt_wbuf, MQTT_WBUF_SIZE, 0, message->qos, message->retained, message->id,
              topic, (unsigned char*)message->payload, message->payloadlen);
    if (len <= 0)
        goto exit;
    if ((rc = udev_send_packet(fd, mqtt_wbuf, len, request_timeout_ms)) != 0) /* send the subscribe packet */
        goto exit; /* there was a problem */

    if (message->qos == QOS1){
        if (waitfor(fd, PUBACK, request_timeout_ms) == PUBACK){
            unsigned short mypacketid;
            unsigned char dup, type;
            if (MQTTDeserialize_ack(&type, &dup, &mypacketid, mqtt_rbuf, MQTT_RBUF_SIZE) != 1)
                rc = -1;
        }
        else
            rc = -1;
    }
    else if (message->qos == QOS2){
        if (waitfor(fd, PUBCOMP, request_timeout_ms) == PUBCOMP){
            unsigned short mypacketid;
            unsigned char dup, type;
            if (MQTTDeserialize_ack(&type, &dup, &mypacketid, mqtt_rbuf, MQTT_RBUF_SIZE) != 1)
                rc = -1;
        }
        else
            rc = -1;
    }

exit:
/**    if (rc == -1)
 *         MQTTCloseSession(c);
 * #if defined(MQTT_TASK)
 *       MutexUnlock(&c->mutex);
 * #endif
 */
    return rc;
}

int udev_mqtt_connect(const char *host, unsigned short port, const char *client_id, const char *token, unsigned int request_timeout)
{
    int fd = -1, len = 0;
    MQTTPacket_connectData connectdata = MQTTPacket_connectData_initializer;

    connectdata.MQTTVersion = 4;
    connectdata.keepAliveInterval = UDEV_KEEPALIVE / 1000;
    connectdata.clientID.cstring = (char *)client_id;
    connectdata.cleansession = 0;

    strncpy(mqtt_token, token, sizeof(mqtt_token));
    char *delims = strchr(mqtt_token, '|');
    *delims = '\0';
    connectdata.username.cstring = mqtt_token;
    connectdata.password.cstring = delims + 1;
    /* LOGE("udev", "user:%s[%d], passwd:%s[%d]", connectdata.username.cstring, strlen(connectdata.username.cstring),
                                                connectdata.password.cstring, strlen(connectdata.password.cstring)); */
    request_timeout_ms = request_timeout;
    if((fd = udev_net_connect(host, port)) < 0){
        LOGE("udev", "udev_net_connect failed");
        return -1;
    }

    if ((len = MQTTSerialize_connect((unsigned char *)mqtt_wbuf, MQTT_WBUF_SIZE, &connectdata)) <= 0) {
        LOGE("udev", "Serialize connect packet failed");
        return -1;
    }

    if(udev_send_packet(fd, mqtt_wbuf, len, request_timeout_ms) != 0){
        LOGE("udev", "udev_send_packet failed");
        return -1;
    }

    if (waitfor(fd, CONNACK, request_timeout_ms) != CONNACK){
        LOGE("udev", "waitfor failed");
        return -1;
    }
    unsigned char rc = 0;
    unsigned char sessionPresent = 0;
    if (MQTTDeserialize_connack(&sessionPresent, &rc, mqtt_rbuf, MQTT_RBUF_SIZE) != 1){
        LOGE("udev", "MQTTDeserialize_connack failed");
        return -1;
    }
    LOGI("udev", "mqtt connect success\n");

    return fd;
}
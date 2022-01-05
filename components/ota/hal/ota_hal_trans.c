/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include "ota_hal_trans.h"
#include "aiot_mqtt_api.h"

/*MQTT API*/
int ota_hal_mqtt_publish(void *mqtt_client, char *topic, int qos, void *data, int len)
{
    return aiot_mqtt_pub(mqtt_client, topic, data, len, qos);
}

int ota_hal_mqtt_subscribe(void *mqtt_client, char *topic, void* cb, void *ctx)
{
    return aiot_mqtt_sub(mqtt_client, topic, cb, 0, ctx);
}

int ota_hal_mqtt_type_is_pub(void *msg)
{
    aiot_mqtt_recv_t *mqtt_msg = (aiot_mqtt_recv_t *)msg;
    if (mqtt_msg->type == AIOT_MQTTRECV_PUB) {
        return 1;
    }
}

void *ota_hal_mqtt_get_payload(void *msg)
{
    void *payload = NULL;
    aiot_mqtt_recv_t *mqtt_msg = (aiot_mqtt_recv_t *)msg;
    payload = (char *)mqtt_msg->data.pub.payload;
    return payload;
}

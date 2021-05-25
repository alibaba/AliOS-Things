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

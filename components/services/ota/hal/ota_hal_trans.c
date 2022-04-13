/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include "ota_log.h"
#include "ota_hal_trans.h"

#ifndef OTA_SIGNAL_CHANNEL
#define OTA_SIGNAL_CHANNEL 1
#endif


#if (OTA_SIGNAL_CHANNEL) == 2
#include "linkkit/coap_api.h"
#elif (OTA_SIGNAL_CHANNEL) == 1
#include "aiot_state_api.h"
#include "aiot_sysdep_api.h"
#include "aiot_mqtt_api.h"
#endif

#if defined OTA_CONFIG_BLE
void breeze_disconnect_ble(void);
uint32_t breeze_post_ext(uint8_t cmd, uint8_t *buffer, uint32_t length);
int ota_ble_disconnect(void)
{
    breeze_disconnect_ble();
    return 0;
}

int ota_ble_transport(unsigned char cmd, unsigned char* buf, unsigned int len)
{
    return (int)breeze_post_ext(cmd, buf, len);
}
#endif

/*MQTT API*/
int ota_hal_mqtt_publish(void *mqtt_client, char *topic, int qos, void *data, int len)
{
    return aiot_mqtt_pub(mqtt_client, topic, data, len, qos);
}

int ota_hal_mqtt_subscribe(void *mqtt_client, char *topic, void* cb, void *ctx)
{
    return aiot_mqtt_sub(mqtt_client, topic, cb, 0, ctx);
}

/*CoAP API*/
int ota_coap_send(char *topic, void *message)
{
#if (OTA_SIGNAL_CHANNEL) == 2
    return IOT_CoAP_SendMessage(NULL, topic, message);
#else
    return 0;
#endif
}

int ota_coap_parse_block(void *message, int type, int *num, int *more, int *size)
{
#if (OTA_SIGNAL_CHANNEL) == 2
    return IOT_CoAP_ParseOption_block(message, type, num, more, size);
#else
    return 0;
#endif
}

int ota_coap_send_block(char *topic, void *message, int block_type, int num, int more, int size)
{
#if (OTA_SIGNAL_CHANNEL) == 2
    return IOT_CoAP_SendMessage_block(NULL, topic, message, block_type,num, more, size);
#else
    return 0;
#endif
}

int ota_coap_get_payload(void *message, const char **payload, int *len)
{
#if (OTA_SIGNAL_CHANNEL) == 2
    return IOT_CoAP_GetMessagePayload(message, payload, len);
#else
    return 0;
#endif
}

int ota_coap_get_code(void *message, void *code)
{
#if (OTA_SIGNAL_CHANNEL) == 2
    return IOT_CoAP_GetMessageCode(message, code);
#else
    return 0;
#endif
}

int ota_coap_init(void)
{
#if (OTA_SIGNAL_CHANNEL) == 2
    int ret = 0;
    void *h_ch = NULL;
    h_ch = (void*)ota_IOT_CoAP_Init(NULL);
    if (h_ch) {
        ret = ota_IOT_CoAP_DeviceNameAuth(h_ch);
        if (ret < 0) {
            OTA_LOG_E("COAP error");
            return ret;
        }
    }
#else
    return 0;
#endif
}

int ota_coap_deinit(void)
{
#if (OTA_SIGNAL_CHANNEL) == 2
    IOT_CoAP_Deinit(NULL);
#endif
    return 0;
}

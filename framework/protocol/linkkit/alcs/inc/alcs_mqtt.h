#ifndef _ALCS_MQTT_H_
#define _ALCS_MQTT_H_

#include "iot_export.h"

typedef enum {
	ALCS_MQTT_STATUS_SUCCESS,
	ALCS_MQTT_STATUS_ERROR
}alcs_mqtt_status_e;

typedef struct {
	CoAPContext *coap_ctx;
	char product_key[PRODUCT_KEY_LEN];
	char device_name[DEVICE_ID_LEN];
	uint32_t send_id;
}alcs_mqtt_ctx_t;

#define ALCS_MQTT_PREFIX                                       "/sys/%s/%s"

#define ALCS_MQTT_THING_LAN_PREFIX_GET_REPLY_FMT               "/thing/lan/prefix/get_reply"
#define ALCS_MQTT_THING_LAN_PREFIX_GET_FMT                     "/thing/lan/prefix/get"
#define ALCS_MQTT_THING_LAN_PREFIX_UPDATE_FMT                  "/thing/lan/prefix/update"
#define ALCS_MQTT_THING_LAN_PREFIX_UPDATE_REPLY_FMT            "/thing/lan/prefix/update_reply"
#define ALCS_MQTT_THING_LAN_PREFIX_BLACKLIST_UPDATE_FMT        "/thing/lan/blacklist/update"
#define ALCS_MQTT_THING_LAN_PREFIX_BLACKLIST_UPDATE_REPLY_FMT  "/thing/lan/blacklist/update_reply"

#define ALCS_MQTT_THING_ALCS_REQUEST              "{\"id\":\"%d\",\"version\":\"1.0\",\"params\":\"{}\",\"method\":\"thing.lan.prefix.get\"}"
#define ALCS_MQTT_THING_LAN_PREFIX_RESPONSE_FMT   "{\"id\": \"%d\", \"code\": %d, \"data\": %s}"

#define ALCS_MQTT_TOPIC_MAX_LEN (128)

#define ALCS_MQTT_JSON_KEY_PREFIX   "prefix" 
#define ALCS_MQTT_JSON_KEY_SECRET   "deviceSecret" 
#define ALCS_MQTT_JSON_KEY_BLACK    "blacklist"

#define ALCS_MQTT_PREFIX_MAX_LEN (40)
#define ALCS_MQTT_SECRET_MAX_LEN (40)
#define ALCS_MQTT_BLACK_MAX_LEN  (100)

alcs_mqtt_status_e alcs_mqtt_init(void *handle,char *product_key,char *device_name);
alcs_mqtt_status_e alcs_mqtt_deinit(void *handle,char *product_key,char *device_name);
alcs_mqtt_status_e alcs_mqtt_blacklist_update(void* ctx);
alcs_mqtt_status_e alcs_mqtt_prefixkey_update(void* ctx);
alcs_mqtt_status_e alcs_prefixkey_get(char *product_key,char *device_name);

#endif

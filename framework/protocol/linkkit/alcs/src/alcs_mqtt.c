#include <stdio.h>
#include "iot_export.h"

#include "json_parser.h"
#include "lite-utils.h"
#include "mqtt_instance.h"
#include "CoAPExport.h"
#include "alcs_api.h"
#include "alcs_mqtt.h"

static alcs_mqtt_ctx_t g_alcs_mqtt_ctx;

static alcs_mqtt_ctx_t* __alcs_mqtt_get_ctx(void)
{
	return &g_alcs_mqtt_ctx;
}

static alcs_mqtt_status_e __alcs_mqtt_publish(char *topic, int qos, void *data, int len)
{
    return (mqtt_publish(topic,qos,data,len) != 0) ? ALCS_MQTT_STATUS_ERROR : ALCS_MQTT_STATUS_SUCCESS;
}

static alcs_mqtt_status_e __alcs_mqtt_send_response(char *topic, int id, int code, char *data)
{
	char *msg_pub = NULL;
	uint16_t msg_len = 0;
	alcs_mqtt_status_e status = ALCS_MQTT_STATUS_SUCCESS;
		
	if (data == NULL || strlen(data) == 0)
        data = "{}";

	msg_len = strlen(ALCS_MQTT_THING_LAN_PREFIX_RESPONSE_FMT) + 20 + strlen(data) + 1;
	
	if ((msg_pub = LITE_malloc(msg_len)) == NULL)
		return ALCS_MQTT_STATUS_ERROR;
	
	snprintf(msg_pub,msg_len,ALCS_MQTT_THING_LAN_PREFIX_RESPONSE_FMT,id,code,data);

    status =  __alcs_mqtt_publish(topic,1,msg_pub,strlen(msg_pub));

	LITE_free(msg_pub);

	return status;
}

static alcs_mqtt_status_e __alcs_mqtt_kv_set(const char *key, const void *val, int len, int sync)
{
	//if (aos_kv_set(key,val,len,sync) != 0)
	//	return ALCS_MQTT_STATUS_ERROR;
	
	log_info("ALCS KV Set, Key: %s, Val: %s, Len: %d",key,val,len);
	return ALCS_MQTT_STATUS_SUCCESS;
}

static alcs_mqtt_status_e __alcs_mqtt_kv_get(const char *key, void *buffer, int *buffer_len)
{
	//if (aos_kv_get(key,buffer,buffer_len) != 0)
	//	return ALCS_MQTT_STATUS_ERROR;

	log_info("ALCS KV Get, Key: %s",key);
	
	return ALCS_MQTT_STATUS_SUCCESS;
}

static alcs_mqtt_status_e __alcs_mqtt_kv_del(const char *key)
{
	//if (aos_kv_del(key) != 0)
	//	return ALCS_MQTT_STATUS_ERROR;

	log_info("ALCS KV Del, Key: %s",key);
	
	return ALCS_MQTT_STATUS_SUCCESS;
}

static void __alcs_mqtt_subscribe_callback(char *topic, int topic_len, void *payload, int payload_len, void *ctx)
{
	alcs_mqtt_ctx_t *alcs_mqtt_ctx = (alcs_mqtt_ctx_t*)ctx;
	char topic_compare[ALCS_MQTT_TOPIC_MAX_LEN] = {0};
	char reqid[16]   = {0};
	
    if (topic == NULL || payload == NULL || topic_len == 0 || payload_len == 0) return;

	memset(topic_compare,0,ALCS_MQTT_TOPIC_MAX_LEN);
	snprintf(topic_compare,ALCS_MQTT_TOPIC_MAX_LEN,ALCS_MQTT_PREFIX ALCS_MQTT_THING_LAN_PREFIX_GET_REPLY_FMT,
					alcs_mqtt_ctx->product_key,alcs_mqtt_ctx->device_name);

	log_info("Receivce Message, Topic: %s",topic_compare);
	
	if ((strlen(topic_compare) == topic_len) && (strncmp(topic_compare,topic,topic_len) == 0))
	{
        int data_len = 0, prefix_len = 0, secret_len = 0;
        char *data = NULL, *prefix = NULL, *secret = NULL;
        data = json_get_value_by_name((char *)payload, payload_len, "data", &data_len, NULL);

        if(NULL != data && 0 != data_len){
            char back1, back2;
            prefix = json_get_value_by_name(data, data_len, ALCS_MQTT_JSON_KEY_PREFIX, &prefix_len, NULL);
            secret = json_get_value_by_name(data, data_len, ALCS_MQTT_JSON_KEY_SECRET, &secret_len, NULL);
           
            if(NULL != alcs_mqtt_ctx->coap_ctx && prefix && secret) {
                back1 = prefix[prefix_len];
                prefix[prefix_len] = 0;
                back2 = secret[secret_len];
                secret[secret_len] = 0;
                alcs_add_svr_key(alcs_mqtt_ctx->coap_ctx, prefix, secret);
                prefix[prefix_len] = back1;
                secret[secret_len] = back2;                
                
                if (ALCS_MQTT_STATUS_SUCCESS != __alcs_mqtt_kv_set(ALCS_MQTT_JSON_KEY_PREFIX, prefix, prefix_len, 1)) {
                    log_err("ALCS KV Set Prefix Fail");
                    ;
                }
                if (ALCS_MQTT_STATUS_SUCCESS != __alcs_mqtt_kv_set(ALCS_MQTT_JSON_KEY_SECRET, secret, secret_len, 1)) {
                    log_err("ALCS KV Set Secret Fail");
                    ;
                }
            }
        } else {
            if (ALCS_MQTT_STATUS_SUCCESS == __alcs_mqtt_kv_get(ALCS_MQTT_JSON_KEY_PREFIX, prefix, &prefix_len) &&
                ALCS_MQTT_STATUS_SUCCESS == __alcs_mqtt_kv_get(ALCS_MQTT_JSON_KEY_SECRET, secret, &secret_len)) {
                if(NULL != alcs_mqtt_ctx->coap_ctx && prefix_len && secret_len) {
                    alcs_add_svr_key(alcs_mqtt_ctx->coap_ctx, prefix, secret);
                }
            }
        }
        return;
    }

	memset(topic_compare,0,ALCS_MQTT_TOPIC_MAX_LEN);
	snprintf(topic_compare,ALCS_MQTT_TOPIC_MAX_LEN,ALCS_MQTT_PREFIX ALCS_MQTT_THING_LAN_PREFIX_UPDATE_FMT,
					alcs_mqtt_ctx->product_key,alcs_mqtt_ctx->device_name);

	if ((strlen(topic_compare) == topic_len) && (strncmp(topic_compare,topic,topic_len) == 0))
	{
        int param_len = 0, prefix_len = 0, id_len = 0;
        char *param = NULL, *prefix = NULL, *id = NULL;
        id = json_get_value_by_name((char*)payload, payload_len, "id", &id_len, NULL);

        if(NULL != id && 0 != id_len)
            strncpy(reqid, id, sizeof(reqid)-1);
        param = json_get_value_by_name((char *)payload, payload_len, "params", &param_len, NULL);
        if(NULL != param && 0 != param_len){
            prefix = json_get_value_by_name(param, param_len, ALCS_MQTT_JSON_KEY_PREFIX, &prefix_len, NULL);

        if(NULL != alcs_mqtt_ctx->coap_ctx && prefix)
            if (0 != alcs_remove_svr_key(alcs_mqtt_ctx->coap_ctx, prefix)) {
            }
            if (ALCS_MQTT_STATUS_SUCCESS != __alcs_mqtt_kv_del(ALCS_MQTT_JSON_KEY_PREFIX)) {
                log_err("Remove the keyprefix from aos_kv fail");
                ;
            }

			char reply_topic[ALCS_MQTT_TOPIC_MAX_LEN] = {0};
			snprintf(reply_topic,ALCS_MQTT_TOPIC_MAX_LEN,ALCS_MQTT_PREFIX ALCS_MQTT_THING_LAN_PREFIX_UPDATE_REPLY_FMT,
					alcs_mqtt_ctx->product_key,alcs_mqtt_ctx->device_name);
            __alcs_mqtt_send_response(reply_topic, atoi(reqid), 200, NULL);
        }
        return;
    }

	memset(topic_compare,0,ALCS_MQTT_TOPIC_MAX_LEN);
	snprintf(topic_compare,ALCS_MQTT_TOPIC_MAX_LEN,ALCS_MQTT_PREFIX ALCS_MQTT_THING_LAN_PREFIX_BLACKLIST_UPDATE_FMT,
					alcs_mqtt_ctx->product_key,alcs_mqtt_ctx->device_name);

	if ((strlen(topic_compare) == topic_len) && (strncmp(topic_compare,topic,topic_len) == 0))
	{
        int param_len = 0, blacklist_len = 0, id_len = 0;
        char *param = NULL, *blacklist = NULL, *id = NULL;
        id = json_get_value_by_name((char*)payload, payload_len, "id", &id_len, NULL);

        if(NULL != id && 0 != id_len)
            strncpy(reqid, id, sizeof(reqid)-1);
        param = json_get_value_by_name((char *)payload, payload_len, "params", &param_len, NULL);
        if(NULL != param && 0 != param_len){
            blacklist = json_get_value_by_name(param, param_len, ALCS_MQTT_JSON_KEY_BLACK, &blacklist_len, NULL);
            if(NULL != alcs_mqtt_ctx->coap_ctx && blacklist) {
                alcs_set_revocation(alcs_mqtt_ctx->coap_ctx, blacklist);
                if (ALCS_MQTT_STATUS_SUCCESS != __alcs_mqtt_kv_set(ALCS_MQTT_JSON_KEY_BLACK, blacklist, blacklist_len, 1)) {
                    log_err("aos_kv_set set blacklist fail"); 
                    ;
                }
            }

			char reply_topic[ALCS_MQTT_TOPIC_MAX_LEN] = {0};
			snprintf(reply_topic,ALCS_MQTT_TOPIC_MAX_LEN,ALCS_MQTT_PREFIX ALCS_MQTT_THING_LAN_PREFIX_BLACKLIST_UPDATE_REPLY_FMT,
					alcs_mqtt_ctx->product_key,alcs_mqtt_ctx->device_name);
            __alcs_mqtt_send_response(reply_topic, atoi(reqid), 200, NULL);
        } else {
            if (ALCS_MQTT_STATUS_SUCCESS == __alcs_mqtt_kv_get(ALCS_MQTT_JSON_KEY_BLACK, blacklist, &blacklist_len)) {
                if (NULL != alcs_mqtt_ctx->coap_ctx)
                    alcs_set_revocation(alcs_mqtt_ctx->coap_ctx, blacklist);
            }
        }
        return;
    }
}

static alcs_mqtt_status_e __alcs_mqtt_subscribe(void *ctx,char *topic)
{
    return (mqtt_subscribe(topic,__alcs_mqtt_subscribe_callback,ctx) != 0) ? ALCS_MQTT_STATUS_ERROR : ALCS_MQTT_STATUS_SUCCESS;
}


static alcs_mqtt_status_e __alcs_mqtt_unsubscribe(void *ctx,char *topic)
{
    return (mqtt_unsubscribe(topic) != 0) ? ALCS_MQTT_STATUS_ERROR : ALCS_MQTT_STATUS_SUCCESS;
}


alcs_mqtt_status_e alcs_mqtt_init(void *handle,char *product_key,char *device_name)
{
	char topic[ALCS_MQTT_TOPIC_MAX_LEN] = {0};
	alcs_mqtt_status_e status = ALCS_MQTT_STATUS_SUCCESS;
	alcs_mqtt_ctx_t* ctx =  __alcs_mqtt_get_ctx();

	if (handle == NULL || product_key == NULL || strlen(product_key) > PRODUCT_KEY_LEN ||
		device_name == NULL || strlen(device_name) > DEVICE_NAME_LEN)
		return ALCS_MQTT_STATUS_ERROR;
	
	memset(ctx,0,sizeof(alcs_mqtt_ctx_t));
	ctx->coap_ctx = (CoAPContext*)handle;
	memcpy(ctx->product_key,product_key,strlen(product_key));
	memcpy(ctx->device_name,device_name,strlen(device_name));
	
	memset(topic,0,ALCS_MQTT_TOPIC_MAX_LEN);
	snprintf(topic,ALCS_MQTT_TOPIC_MAX_LEN,ALCS_MQTT_PREFIX ALCS_MQTT_THING_LAN_PREFIX_GET_REPLY_FMT,
					ctx->product_key,ctx->device_name);
    if (__alcs_mqtt_subscribe((void*)ctx,topic) != ALCS_MQTT_STATUS_SUCCESS)
	{
		log_err("ALCS Subscribe Failed, Topic: %s",topic);
		status = ALCS_MQTT_STATUS_ERROR;
	}

	memset(topic,0,ALCS_MQTT_TOPIC_MAX_LEN);
	snprintf(topic,ALCS_MQTT_TOPIC_MAX_LEN,ALCS_MQTT_PREFIX ALCS_MQTT_THING_LAN_PREFIX_UPDATE_FMT,
					ctx->product_key,ctx->device_name);
    if (__alcs_mqtt_subscribe((void*)ctx,topic) != ALCS_MQTT_STATUS_SUCCESS)
	{
		log_err("ALCS Subscribe Failed, Topic: %s",topic);
		status = ALCS_MQTT_STATUS_ERROR;
	}

	memset(topic,0,ALCS_MQTT_TOPIC_MAX_LEN);
	snprintf(topic,ALCS_MQTT_TOPIC_MAX_LEN,ALCS_MQTT_PREFIX ALCS_MQTT_THING_LAN_PREFIX_BLACKLIST_UPDATE_FMT,
					ctx->product_key,ctx->device_name);
    if (__alcs_mqtt_subscribe((void*)ctx,topic) != ALCS_MQTT_STATUS_SUCCESS)
	{
		log_err("ALCS Subscribe Failed, Topic: %s",topic);
		status = ALCS_MQTT_STATUS_ERROR;
	}

	alcs_mqtt_prefixkey_update((void*)ctx->coap_ctx);
	alcs_mqtt_blacklist_update((void*)ctx->coap_ctx);

	alcs_prefixkey_get(ctx->product_key,ctx->device_name);
	
	return status;
}


alcs_mqtt_status_e alcs_mqtt_deinit(void *handle,char *product_key,char *device_name)
{
    char topic[ALCS_MQTT_TOPIC_MAX_LEN] = {0};
    alcs_mqtt_status_e status = ALCS_MQTT_STATUS_SUCCESS;
    alcs_mqtt_ctx_t* ctx =  __alcs_mqtt_get_ctx();

    if (handle == NULL || product_key == NULL || strlen(product_key) > PRODUCT_KEY_LEN ||
        device_name == NULL || strlen(device_name) > DEVICE_NAME_LEN || ctx == NULL)
        return ALCS_MQTT_STATUS_ERROR;

    memset(topic,0,ALCS_MQTT_TOPIC_MAX_LEN);
    snprintf(topic,ALCS_MQTT_TOPIC_MAX_LEN,ALCS_MQTT_PREFIX ALCS_MQTT_THING_LAN_PREFIX_GET_REPLY_FMT,
                    ctx->product_key,ctx->device_name);
    if (__alcs_mqtt_unsubscribe((void*)ctx,topic) != ALCS_MQTT_STATUS_SUCCESS)
    {
        log_err("ALCS Subscribe Failed, Topic: %s",topic);
        status = ALCS_MQTT_STATUS_ERROR;
    }

    memset(topic,0,ALCS_MQTT_TOPIC_MAX_LEN);
    snprintf(topic,ALCS_MQTT_TOPIC_MAX_LEN,ALCS_MQTT_PREFIX ALCS_MQTT_THING_LAN_PREFIX_UPDATE_FMT,
                    ctx->product_key,ctx->device_name);
    if (__alcs_mqtt_unsubscribe((void*)ctx,topic) != ALCS_MQTT_STATUS_SUCCESS)
    {
        log_err("ALCS Subscribe Failed, Topic: %s",topic);
        status = ALCS_MQTT_STATUS_ERROR;
    }

    memset(topic,0,ALCS_MQTT_TOPIC_MAX_LEN);
    snprintf(topic,ALCS_MQTT_TOPIC_MAX_LEN,ALCS_MQTT_PREFIX ALCS_MQTT_THING_LAN_PREFIX_BLACKLIST_UPDATE_FMT,
                    ctx->product_key,ctx->device_name);
    if (__alcs_mqtt_unsubscribe((void*)ctx,topic) != ALCS_MQTT_STATUS_SUCCESS)
    {
        log_err("ALCS Subscribe Failed, Topic: %s",topic);
        status = ALCS_MQTT_STATUS_ERROR;
    }

    return status;
}

alcs_mqtt_status_e alcs_mqtt_blacklist_update(void* ctx)
{
    CoAPContext *context = (CoAPContext *)ctx;
    char blacklist[ALCS_MQTT_BLACK_MAX_LEN] = {0};
    int blacklist_len = ALCS_MQTT_BLACK_MAX_LEN;

    if (NULL == context) return -1;
    
    if (ALCS_MQTT_STATUS_SUCCESS == __alcs_mqtt_kv_get(ALCS_MQTT_JSON_KEY_BLACK, blacklist, &blacklist_len)) {        
        log_info("The blacklist is %.*s", blacklist_len, blacklist);
        if(blacklist_len) {
            alcs_set_revocation(context, blacklist);
            return ALCS_MQTT_STATUS_SUCCESS;
        }
    }
    
    return ALCS_MQTT_STATUS_ERROR;
}


alcs_mqtt_status_e alcs_mqtt_prefixkey_update(void* ctx)
{
    CoAPContext *context = (CoAPContext *)ctx;
    char prefix[ALCS_MQTT_PREFIX_MAX_LEN] = {0};
    char secret[ALCS_MQTT_SECRET_MAX_LEN] = {0};
    int prefix_len = ALCS_MQTT_PREFIX_MAX_LEN, secret_len = ALCS_MQTT_SECRET_MAX_LEN;

    if (NULL == context) return ALCS_MQTT_STATUS_ERROR;
         
    log_info("start alcs_prefixkey_update\n");
   
    if (ALCS_MQTT_STATUS_SUCCESS == __alcs_mqtt_kv_get(ALCS_MQTT_JSON_KEY_PREFIX, prefix, &prefix_len) &&
        ALCS_MQTT_STATUS_SUCCESS == __alcs_mqtt_kv_get(ALCS_MQTT_JSON_KEY_SECRET, secret, &secret_len)) {
        log_info("The prefix is  %.*s, deviceSecret is %.*s", prefix_len, prefix, secret_len, secret);
        if(prefix_len && secret_len) {
            alcs_add_svr_key(context, prefix, secret);
            return ALCS_MQTT_STATUS_SUCCESS;
        }
    }
    
    return ALCS_MQTT_STATUS_ERROR;
}

alcs_mqtt_status_e alcs_prefixkey_get(char *product_key,char *device_name)
{
    //int ret = 0;
	char *msg_pub = NULL;
	uint16_t msg_len = 0;
	char topic[ALCS_MQTT_TOPIC_MAX_LEN] = {0};
	alcs_mqtt_ctx_t* ctx =  __alcs_mqtt_get_ctx();
	alcs_mqtt_status_e status = ALCS_MQTT_STATUS_SUCCESS;
	int id = ctx->send_id++;
	
	if (product_key == NULL || strlen(product_key) > PRODUCT_KEY_LEN ||
		device_name == NULL || strlen(device_name) > DEVICE_NAME_LEN)
		return ALCS_MQTT_STATUS_ERROR;
	snprintf(topic,ALCS_MQTT_TOPIC_MAX_LEN,ALCS_MQTT_PREFIX ALCS_MQTT_THING_LAN_PREFIX_GET_FMT,
		product_key,device_name);

	msg_len = strlen(ALCS_MQTT_THING_ALCS_REQUEST) + 10 + 1;
	if ((msg_pub = LITE_malloc(msg_len)) == NULL)
		return ALCS_MQTT_STATUS_ERROR;
	
	snprintf(msg_pub, msg_len, ALCS_MQTT_THING_ALCS_REQUEST,id);

	log_err("ALCS Prefix Get, Topic: %s, Payload: %s",topic,msg_pub);
    status = __alcs_mqtt_publish(topic,1,msg_pub,strlen(msg_pub));

	LITE_free(msg_pub);

	return status;
}


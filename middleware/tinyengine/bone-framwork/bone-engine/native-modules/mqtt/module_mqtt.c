/*
 *  Copyright © 2018 alibaba. All rights reserved.
 */

#include <be_osal.h>
#include <be_jse_module.h>


#ifdef BE_OS_AOS
#include "iot_export.h"
#include "iot_import.h"
#include "aos/network.h"
#include <mbedtls/sha1.h>
#include "mqtt_instance.h"
#include "iot_export_mqtt.h"
#include "ali_crypto.h"
#endif
#include "module_mqtt.h"

#define JS_MQTT_TAG "MQTT"
#define CONFIG_LOGMACRO_DETAILS
#define IOT_MQTT_TOPIC_NAME_SIZE (128 + 1)

typedef struct
{
    struct list_head list;
    be_jse_symbol_t *func;
    char topic_name[IOT_MQTT_TOPIC_NAME_SIZE];
} topic_process_t;

#define MSG_LEN_MAX (2048)

typedef struct
{
    char productKey[16];
    char deviceName[32];
    char deviceSecret[48];
    int max_msg_size;
} MqttContext;

static MqttContext mqtt;
static struct list_head topic_list = LIST_HEAD_INIT(topic_list);
static be_jse_symbol_t *start_up_func = NULL;

static int smartled_event_handler(int event_type, void *ctx)
{
    be_warn(JS_MQTT_TAG, "event_type %d\n", event_type);
    switch (event_type)
    {
        default:
        break;
    }

    return 0;
}

static void mqtt_service_event(be_osal_input_event_t *event, void *priv_data)
{
    int ret;

    if (event->type != EV_SYS)
    {
        return;
    }

    if (event->code != CODE_SYS_ON_MQTT_READ)
    {
        return;
    }

    be_warn(JS_MQTT_TAG, "mqtt_service_event!");

    // 通知 MQTT success
    if (start_up_func && symbol_is_function(start_up_func))
    {
        BE_ASYNC_S *async = (BE_ASYNC_S *)calloc(1, sizeof(BE_ASYNC_S));
        async->func = start_up_func;
        async->param_count = 0;
        async->params = NULL;

        ret = be_osal_schedule_call(be_jse_async_event_cb, async);
        if (ret >= 0)
        {
            INC_SYMBL_REF(async->func);
        }
        else
        {
            be_error(JS_MQTT_TAG, "fatal error");
            free(async);
        }
    }

    symbol_unlock(start_up_func);
    start_up_func = NULL;
}

static void mqtt_sub_callback(char *topic, int topic_len, void *payload, int payload_len, void *ctx)
{

    int ret;
    be_jse_symbol_t *func = (be_jse_symbol_t *)ctx;
    char ch;

    BE_ASYNC_S *async = (BE_ASYNC_S *)calloc(1, sizeof(BE_ASYNC_S));
    async->func = func;
    async->param_count = 2;
    async->params = (be_jse_symbol_t **)calloc(1, sizeof(be_jse_symbol_t *) * async->param_count);
    async->params[0] = new_json_symbol((char *)payload, payload_len);

    ch = topic[topic_len];
    if (ch != 0)
        topic[topic_len] = 0;
    async->params[1] = new_str_symbol(topic);
    topic[topic_len] = ch;

    ret = be_osal_schedule_call(be_jse_async_event_cb, async);
    if (ret >= 0)
    {
        INC_SYMBL_REF(async->func);
    }
    else
    {
        be_error(JS_MQTT_TAG, "fatal error");
        free(async->params);
        free(async);
    }
}

static be_jse_symbol_t *module_handle_cb(be_jse_vm_ctx_t *execInfo, be_jse_symbol_t *var, const char *name)
{
    int ret;
    be_jse_symbol_t *arg0 = NULL;
    be_jse_symbol_t *arg1 = NULL;

    char productKey[16] = {0};
    char deviceName[32] = {0};
    char deviceSecret[48] = {0};

    if (strcmp(name, "VERSION") == 0)
    {
        lexer_token_cmp(execInfo->lex, BE_TOKEN_ID);
        return new_str_symbol("0.0.1");
    }

    if (strcmp(name, "sign") == 0)
    {
        be_jse_handle_function(0, &arg0, NULL, NULL, NULL);
        if (symbol_is_object(arg0))
        {

            be_jse_symbol_t *productKeySymbol = unlock_symbol_value(lookup_named_child_symbol(get_symbol_node_id(arg0), "productKey", false));
            be_jse_symbol_t *deviceNameSymbol = unlock_symbol_value(lookup_named_child_symbol(get_symbol_node_id(arg0), "deviceName", false));
            be_jse_symbol_t *deviceSecretSymbol = unlock_symbol_value(lookup_named_child_symbol(get_symbol_node_id(arg0), "deviceSecret", false));
            symbol_unlock(arg0);

            if (productKeySymbol == NULL || deviceNameSymbol == NULL || deviceSecretSymbol == NULL)
            {
                be_error(JS_MQTT_TAG, "参数错误");
                return new_str_symbol(NULL);
            }

            symbol_to_str(productKeySymbol, productKey, sizeof(productKey));
            symbol_to_str(deviceNameSymbol, deviceName, sizeof(deviceName));
            symbol_to_str(deviceSecretSymbol, deviceSecret, sizeof(deviceSecret));

            symbol_unlock(productKeySymbol);
            symbol_unlock(deviceNameSymbol);
            symbol_unlock(deviceSecretSymbol);

            unsigned char content[256] = {0};
            snprintf((char *)content,sizeof(content),"clientId%sdeviceName%sproductKey%s", deviceName, deviceName, productKey);
            //printf("content=%s,deviceSecret=%d,siezof(%d)\n\r",content,strlen(deviceSecret),strlen((char *)content));
 
            unsigned char k_ipad[64] = {0};
            unsigned char k_opad[64] = {0};
            unsigned char out[20] = {0};

            memcpy(k_ipad, deviceSecret, strlen(deviceSecret));
            memcpy(k_opad, deviceSecret, strlen(deviceSecret));

            for (int i = 0; i < sizeof(k_ipad); i++)
            {
                k_ipad[i] ^= 0x36;
                k_opad[i] ^= 0x5c;
            }

#if 0

            mbedtls_sha1_context sha1_ctx;
            mbedtls_sha1_init(&sha1_ctx);
            mbedtls_sha1_starts(&sha1_ctx);
            mbedtls_sha1_update(&sha1_ctx, k_ipad, sizeof(k_ipad));
            mbedtls_sha1_update(&sha1_ctx, content, strlen((char *)content));
            mbedtls_sha1_finish(&sha1_ctx, out);
            mbedtls_sha1_free(&sha1_ctx);
          
            mbedtls_sha1_init(&sha1_ctx);
            mbedtls_sha1_starts(&sha1_ctx);
            mbedtls_sha1_update(&sha1_ctx, k_opad, sizeof(k_opad));
            mbedtls_sha1_update(&sha1_ctx, out, sizeof(out));
            mbedtls_sha1_finish(&sha1_ctx, out);
            mbedtls_sha1_free(&sha1_ctx);

#else

            void *context;
            size_t   context_size;

            ali_hash_get_ctx_size(SHA1, &context_size);
            context = calloc(1, context_size);

            ali_hash_init(SHA1, context);
            ali_hash_update(k_ipad, sizeof(k_ipad), context);
            ali_hash_update(content, strlen((char *)content), context);
            ali_hash_final(out, context);

            ali_hash_reset(context);
            ali_hash_init(SHA1, context);
            ali_hash_update(k_opad, sizeof(k_opad), context);
            ali_hash_update(out, sizeof(out), context);
            ali_hash_final(out, context);

            free(context);
#endif

            char sign[41] = {0};
            for (int i = 0; i < sizeof(out); ++i)
            {
                unsigned char byte[2] = {0};
                byte[0] = out[i] / 16;
                byte[1] = out[i] % 16;

                for (int j = 0; j < 2; ++j)
                {
                    if (byte[j] >= 0 && byte[j] <= 9)
                        sign[2 * i + j] = '0' + byte[j];
                    else
                        sign[2 * i + j] = 'a' + byte[j] - 10;
                }
            }

            return new_str_symbol(sign);
        }
    }

    if (strcmp(name, "start") == 0)
    {
        be_jse_handle_function(0, &arg0, &arg1, NULL, NULL);
        if (symbol_is_object(arg0))
        {

            be_jse_symbol_t *productKeySymbol = unlock_symbol_value(lookup_named_child_symbol(get_symbol_node_id(arg0), "productKey", false));
            be_jse_symbol_t *deviceNameSymbol = unlock_symbol_value(lookup_named_child_symbol(get_symbol_node_id(arg0), "deviceName", false));
            be_jse_symbol_t *deviceSecretSymbol = unlock_symbol_value(lookup_named_child_symbol(get_symbol_node_id(arg0), "deviceSecret", false));
            symbol_unlock(arg0);

            if (productKeySymbol == NULL || deviceNameSymbol == NULL || deviceSecretSymbol == NULL)
            {

                be_error(JS_MQTT_TAG, "参数错误");
                if (arg1 && symbol_is_function(arg1))
                {
                    BE_ASYNC_S *async = (BE_ASYNC_S *)calloc(1, sizeof(BE_ASYNC_S));
                    async->func = arg1;
                    async->param_count = 1;
                    async->params = (be_jse_symbol_t **)calloc(1, sizeof(be_jse_symbol_t *) * async->param_count);
                    async->params[0] = new_str_symbol("productKey or deviceName or deviceSecret is null");

                    ret = be_osal_schedule_call(be_jse_async_event_cb, async);
                    if (ret >= 0)
                    {
                        // success
                        INC_SYMBL_REF(async->func);
                    }
                    else
                    {
                        be_error(JS_MQTT_TAG, "fatal error");
                        free(async->params);
                        free(async);
                    }
                }

                symbol_unlock(arg1);

                return new_int_symbol(-1);
                ;
            }

            symbol_to_str(productKeySymbol, mqtt.productKey, sizeof(mqtt.productKey));
            symbol_to_str(deviceNameSymbol, mqtt.deviceName, sizeof(mqtt.deviceName));
            symbol_to_str(deviceSecretSymbol, mqtt.deviceSecret, sizeof(mqtt.deviceSecret));

            symbol_unlock(productKeySymbol);
            symbol_unlock(deviceNameSymbol);
            symbol_unlock(deviceSecretSymbol);

            mqtt.max_msg_size = MSG_LEN_MAX;

            if (mqtt_init_instance(mqtt.productKey, mqtt.deviceName, mqtt.deviceSecret, mqtt.max_msg_size) < 0)
            {
                be_warn(JS_MQTT_TAG, "mqtt_init_instance failed\n");

                if (arg1 && symbol_is_function(arg1))
                {
                    BE_ASYNC_S *async = (BE_ASYNC_S *)calloc(1, sizeof(BE_ASYNC_S));
                    async->func = arg1;
                    async->param_count = 1;
                    async->params = (be_jse_symbol_t **)calloc(1, sizeof(be_jse_symbol_t *) * async->param_count);
                    async->params[0] = new_str_symbol("mqtt_init_instance failed");

                    ret = be_osal_schedule_call(be_jse_async_event_cb, async);
                    if (ret >= 0)
                    {
                        // success
                        INC_SYMBL_REF(async->func);
                    }
                    else
                    {
                        be_warn(JS_MQTT_TAG, "fatal error");
                        free(async->params);
                        free(async);
                    }
                }

                symbol_unlock(arg1);

                return new_int_symbol(-1);
            }

            be_osal_register_event_filter(EV_SYS, mqtt_service_event, NULL);
            start_up_func = arg1;
        }
        return new_int_symbol(0);
    }

    if (strcmp(name, "subscribe") == 0)
    {

        be_jse_handle_function(0, &arg0, &arg1, NULL, NULL);

        char topic_name[IOT_MQTT_TOPIC_NAME_SIZE] = {0};
        symbol_to_str(arg0, topic_name, sizeof(topic_name));
        symbol_unlock(arg0);

        topic_process_t *item;
        list_for_each_entry(item, &topic_list, list)
        {
            // 判断是否重复
            if (strcmp(topic_name, item->topic_name) == 0)
            {
                symbol_unlock(arg1);
                return new_int_symbol(-1);
            }
        }

        ret = mqtt_subscribe(topic_name, mqtt_sub_callback, arg1);
        if (ret < 0)
        {
            be_warn(JS_MQTT_TAG, "IOT_MQTT_Subscribe() failed, rc = %d", ret);
            symbol_unlock(arg1);
        }
        else
        {
            // 加到list中
            topic_process_t *proc = (topic_process_t *)calloc(1, sizeof(topic_process_t));
            memcpy(proc->topic_name, topic_name, strlen(topic_name));
            proc->func = arg1;
            list_add_tail(&proc->list, &topic_list);
        }

        return new_int_symbol(ret);
    }

    if (strcmp(name, "unsubscribe") == 0)
    {
        arg0 = be_jse_handle_single_arg_function();

        int len = symbol_str_len(arg0);
        char *arg0Str = (char *)calloc(1, len);
        symbol_to_str(arg0, arg0Str, len);

        ret = mqtt_unsubscribe(arg0Str);

        topic_process_t *item = NULL;
        list_for_each_entry(item, &topic_list, list)
        {
            if (strcmp(arg0Str, item->topic_name) == 0)
            {
                break;
            }
        }

        if (item)
        {
            symbol_unlock(item->func);
            list_del(&item->list);
            free(item);
        }

        free(arg0Str);
        symbol_unlock(arg0);
        return new_int_symbol(ret);
    }

    if (strcmp(name, "publish") == 0)
    {

        be_jse_handle_function(0, &arg0, &arg1, NULL, NULL);

        if (arg0 == NULL || arg1 == NULL || !symbol_is_string(arg0) || !symbol_is_string(arg0))
        {
            symbol_unlock(arg0);
            symbol_unlock(arg1);
            return new_int_symbol(-1);
        }

        int msgLen = symbol_str_len(arg0);
        char *arg0Str = (char *)calloc(1, msgLen);
        symbol_to_str(arg0, arg0Str, msgLen);

        msgLen = symbol_str_len(arg1);
        char *arg1Str = (char *)calloc(1, msgLen);
        symbol_to_str(arg1, arg1Str, msgLen);

        ret = mqtt_publish(arg0Str, IOTX_MQTT_QOS1, arg1Str, msgLen);
        if (ret < 0)
        {
            be_warn(JS_MQTT_TAG, "mqtt_publish() failed, ret = %d", ret);
        }

        symbol_unlock(arg0);
        symbol_unlock(arg1);
        free(arg0Str);
        free(arg1Str);

        return new_int_symbol(ret);
    }

    return BE_JSE_FUNC_UNHANDLED;
}

void module_mqtt_load(void)
{
    mqtt_deinit_instance();

    memset(&mqtt, 0, sizeof(MqttContext));
    be_jse_module_load(JS_MQTT_TAG, module_handle_cb);
}

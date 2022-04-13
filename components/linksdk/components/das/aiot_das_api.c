#include "inc/das.h"
#include "das_private.h"

#include "core_string.h"
#include "core_log.h"
#include "core_global.h"
#include "core_mqtt.h"

#include "aiot_mqtt_api.h"

static void _das_core_mqtt_process_handler(void *context, aiot_mqtt_event_t *event, core_mqtt_event_t *core_event)
{
    das_handle_t *das_handle = (das_handle_t *)context;

    if (core_event != NULL) {
        switch (core_event->type) {
            case CORE_MQTTEVT_DEINIT: {
                das_handle->mqtt = NULL;
                return;
            }
            break;
            default: {

            }
            break;
        }
    }
}

static int32_t _das_core_mqtt_operate_process_handler(das_handle_t *das_handle, core_mqtt_option_t option)
{
    core_mqtt_process_data_t process_data;

    if (das_handle == NULL) {
        return STATE_USER_INPUT_NULL_POINTER;
    }

    memset(&process_data, 0, sizeof(core_mqtt_process_data_t));
    process_data.handler = _das_core_mqtt_process_handler;
    process_data.context = das_handle;

    return core_mqtt_setopt(das_handle->mqtt, option, &process_data);
}

static void _das_recv_handler(void *handle, const aiot_mqtt_recv_t *packet, void *userdata)
{
    das_handle_t *das_handle = (das_handle_t *)userdata;
    
    if (das_handle == NULL) {
        return;
    }

     switch (packet->type) {
        case AIOT_MQTTRECV_PUB: {
            das_on_message(das_handle->das,
                       (uint8_t *)packet->data.pub.payload,
                       (size_t)packet->data.pub.payload_len);
        }
        break;
        default: {
        }
        break;
    }
}

static int _das_bind_mqtt(das_handle_t *das_handle, void *mqtt_handle) 
{
    int ret = STATE_SUCCESS;
    const char *sub_topic = NULL;

    if (das_handle == NULL || mqtt_handle == NULL) {
        return STATE_USER_INPUT_NULL_POINTER;
    }

    das_handle->mqtt = mqtt_handle;
    das_handle->das = das_init(core_mqtt_get_product_key(mqtt_handle), core_mqtt_get_device_name(mqtt_handle));
    if (das_handle->das == NULL) {
        return STATE_DAS_INIT_ERROR;
    }

    sub_topic = das_sub_topic(das_handle->das, NULL);
    if (sub_topic == NULL) {
        return STATE_DAS_SUB_TOPIC_ERROR;
    }

    ret = aiot_mqtt_sub(mqtt_handle, (char *)sub_topic, _das_recv_handler, 0, (void *)das_handle);
    if (ret >= STATE_SUCCESS) {
        ret = _das_core_mqtt_operate_process_handler(das_handle, CORE_MQTTOPT_APPEND_PROCESS_HANDLER);
    }

    return ret;
}

int32_t _das_publish(const char *topic, const uint8_t *message, size_t msg_size, void *channel)
{
    int32_t res = STATE_SUCCESS;
    das_handle_t *das_handle = (das_handle_t *)channel;

    if (das_handle == NULL) {
        return STATE_USER_INPUT_NULL_POINTER;
    }

    if (das_handle->inited == 0) {
        return STATE_USER_INPUT_EXEC_DISABLED;
    }

    if (das_handle->mqtt == NULL) {
        return STATE_DAS_SET_MQTT_HANDLE_ERROR;
    }

    res = aiot_mqtt_pub(das_handle->mqtt, (char *)topic, (uint8_t *)message, msg_size, 0);
    return res;
}

void *aiot_das_init()
{
    das_handle_t *das_handle = NULL;
    aiot_sysdep_portfile_t *sysdep = NULL;

    sysdep = aiot_sysdep_get_portfile();
    if (sysdep == NULL) {
        return NULL;
    }

    das_handle = sysdep->core_sysdep_malloc(sizeof(das_handle_t), DAS_MODULE_NAME);
    if (das_handle == NULL) {
        return NULL;
    }
    memset(das_handle, 0, sizeof(das_handle_t));

    das_handle->sysdep = sysdep;

    das_handle->inited = 1;

    return das_handle;
}

int32_t aiot_das_setopt(void *handle, aiot_das_option_t option, void *data)
{
    int32_t res = STATE_SUCCESS;
    das_handle_t *das_handle = handle;

    if (handle == NULL || data == NULL) {
        return STATE_USER_INPUT_NULL_POINTER;
    }

    if (option >= AIOT_DASOPT_MAX) {
        return STATE_USER_INPUT_OUT_RANGE;
    }

    if (das_handle->inited == 0) { 
        return STATE_USER_INPUT_EXEC_DISABLED; 
    }

    switch (option) {
        case AIOT_DASOPT_MQTT_HANDLE: {
            void *mqtt_handle = data;
            res = _das_bind_mqtt(das_handle, mqtt_handle);
            if (res == STATE_SUCCESS) {
                das_connection(das_handle->das, _das_publish, das_handle);
                das_on_connected(das_handle->das);
            }
        }
        break;
        case AIOT_DASOPT_SET_FIRMWARE_VERSION: {
            int ret = das_set_firmware_version((char *)data);
            if (ret == -1) {
                res = STATE_DAS_SET_FIRVER_ERROR;
            }
        }
        break;
        default: {
            res = STATE_USER_INPUT_UNKNOWN_OPTION;
        }
        break;
    }

    return res;
}

int32_t aiot_das_stepping(void *handle) 
{
    int ret = STATE_SUCCESS;
    das_result_t das_ret = DAS_SUCCESS;
    das_handle_t *das_handle = (das_handle_t *)handle;
    uint64_t time = 0;

    if (handle == NULL) {
        return STATE_USER_INPUT_NULL_POINTER;
    }

    time = das_handle->sysdep->core_sysdep_time();
    das_ret = das_stepping(das_handle->das, time);
    if (das_ret != DAS_SUCCESS) {
        ret = STATE_DAS_STEPPING_ERROR;
    }

    return ret;
}

int32_t aiot_das_deinit(void **handle)
{
    das_handle_t *das_handle = NULL;
    const char *sub_topic = NULL;

    if (handle == NULL || *handle == NULL) {
        return STATE_USER_INPUT_NULL_POINTER;
    }

    das_handle = *(das_handle_t **)handle;

    if (das_handle->inited == 0) {
        return STATE_USER_INPUT_EXEC_DISABLED;
    }

    das_handle->inited = 0;

    sub_topic = das_sub_topic(das_handle->das, NULL);
    if (sub_topic == NULL) {
        return STATE_DAS_SUB_TOPIC_ERROR;
    }

    _das_core_mqtt_operate_process_handler(das_handle, CORE_MQTTOPT_REMOVE_PROCESS_HANDLER);
    aiot_mqtt_unsub(das_handle->mqtt, (char *)sub_topic);
    
    das_on_disconnected(das_handle->das);
    das_final(das_handle->das);

    das_handle->sysdep->core_sysdep_free(handle);
    *handle = NULL;
    
    return STATE_SUCCESS;
}
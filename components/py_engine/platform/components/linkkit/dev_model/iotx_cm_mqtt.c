#include "iotx_cm_internal.h"

#if defined(MQTT_COMM_ENABLED) || defined(MAL_ENABLED)

static iotx_cm_connection_t *_mqtt_conncection = NULL;
static void iotx_cloud_conn_mqtt_event_handle(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg);
static int  _mqtt_connect(uint32_t timeout);
static int _mqtt_publish(iotx_cm_ext_params_t *params, const char *topic, const char *payload,
                         unsigned int payload_len);
static int _mqtt_sub(iotx_cm_ext_params_t *params, const char *topic,
                     iotx_cm_data_handle_cb topic_handle_func, void *pcontext);
static iotx_mqtt_qos_t _get_mqtt_qos(iotx_cm_ack_types_t ack_type);
static int _mqtt_unsub(const char *topic);
static int _mqtt_close(void);
static void _set_common_handlers(void);

iotx_cm_connection_t *iotx_cm_open_mqtt(iotx_cm_init_param_t *params)
{
    iotx_mqtt_param_t *mqtt_param = NULL;

    if (_mqtt_conncection != NULL) {
        iotx_state_event(ITE_STATE_DEV_MODEL, STATE_DEV_MODEL_INTERNAL_MQTT_DUP_INIT, NULL);
        return _mqtt_conncection;
    }

    _mqtt_conncection = (iotx_cm_connection_t *)cm_malloc(sizeof(iotx_cm_connection_t));
    if (_mqtt_conncection == NULL) {
        iotx_state_event(ITE_STATE_DEV_MODEL, STATE_SYS_DEPEND_MALLOC, NULL);
        goto failed;
    }
    memset(_mqtt_conncection, 0, sizeof(iotx_cm_connection_t));

    mqtt_param = (iotx_mqtt_param_t *)cm_malloc(sizeof(iotx_mqtt_param_t));
    if (mqtt_param == NULL) {
        iotx_state_event(ITE_STATE_DEV_MODEL, STATE_SYS_DEPEND_MALLOC, NULL);
        goto failed;
    }
    memset(mqtt_param, 0, sizeof(iotx_mqtt_param_t));

    mqtt_param->request_timeout_ms = params->request_timeout_ms;
    mqtt_param->clean_session = 0;
    mqtt_param->keepalive_interval_ms = params->keepalive_interval_ms;
    mqtt_param->read_buf_size = params->read_buf_size;
    mqtt_param->write_buf_size = params->write_buf_size;
    mqtt_param->handle_event.h_fp = iotx_cloud_conn_mqtt_event_handle;
    mqtt_param->handle_event.pcontext = NULL;

    _mqtt_conncection->open_params = mqtt_param;
    _mqtt_conncection->event_handler = params->handle_event;
    _mqtt_conncection->cb_data = params->context;
    _set_common_handlers();

    return _mqtt_conncection;

failed:
    if (_mqtt_conncection != NULL) {
        cm_free(_mqtt_conncection);
        _mqtt_conncection = NULL;
    }

    if (mqtt_param != NULL) {
        cm_free(mqtt_param);
    }

    return NULL;
}

static void iotx_cloud_conn_mqtt_event_handle(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg)
{
    uintptr_t packet_id = (uintptr_t)msg->msg;
    if (_mqtt_conncection == NULL) {
        return;
    }

    switch (msg->event_type) {
        case IOTX_MQTT_EVENT_DISCONNECT: {
            iotx_cm_event_msg_t event;
            event.type = IOTX_CM_EVENT_CLOUD_DISCONNECT;
            event.msg = NULL;
            if (_mqtt_conncection->event_handler) {
                _mqtt_conncection->event_handler(_mqtt_conncection->fd, &event, _mqtt_conncection->cb_data);
            }
        }
        break;

        case IOTX_MQTT_EVENT_RECONNECT: {
            iotx_cm_event_msg_t event;
            event.type = IOTX_CM_EVENT_CLOUD_CONNECTED;
            event.msg = NULL;

            if (_mqtt_conncection->event_handler) {
                _mqtt_conncection->event_handler(_mqtt_conncection->fd, &event, _mqtt_conncection->cb_data);
            }
        }
        break;

        case IOTX_MQTT_EVENT_SUBCRIBE_SUCCESS: {
            iotx_cm_event_msg_t event;
            event.type = IOTX_CM_EVENT_SUBCRIBE_SUCCESS;
            event.msg = (void *)packet_id;

            if (_mqtt_conncection->event_handler) {
                _mqtt_conncection->event_handler(_mqtt_conncection->fd, &event, _mqtt_conncection->cb_data);
            }
        }
        break;

        case IOTX_MQTT_EVENT_SUBCRIBE_NACK:
        case IOTX_MQTT_EVENT_SUBCRIBE_TIMEOUT: {
            iotx_cm_event_msg_t event;
            event.type = IOTX_CM_EVENT_SUBCRIBE_FAILED;
            event.msg = (void *)packet_id;

            if (_mqtt_conncection->event_handler) {
                _mqtt_conncection->event_handler(_mqtt_conncection->fd, &event, _mqtt_conncection->cb_data);
            }
        }
        break;

        case IOTX_MQTT_EVENT_UNSUBCRIBE_SUCCESS: {
            iotx_cm_event_msg_t event;
            event.type = IOTX_CM_EVENT_UNSUB_SUCCESS;
            event.msg = (void *)packet_id;

            if (_mqtt_conncection->event_handler) {
                _mqtt_conncection->event_handler(_mqtt_conncection->fd, &event, _mqtt_conncection->cb_data);
            }
        }
        break;

        case IOTX_MQTT_EVENT_UNSUBCRIBE_NACK:
        case IOTX_MQTT_EVENT_UNSUBCRIBE_TIMEOUT: {
            iotx_cm_event_msg_t event;
            event.type = IOTX_CM_EVENT_UNSUB_FAILED;
            event.msg = (void *)packet_id;

            if (_mqtt_conncection->event_handler) {
                _mqtt_conncection->event_handler(_mqtt_conncection->fd, &event, _mqtt_conncection->cb_data);
            }
        }
        break;

        case IOTX_MQTT_EVENT_PUBLISH_SUCCESS: {
            iotx_cm_event_msg_t event;
            event.type = IOTX_CM_EVENT_PUBLISH_SUCCESS;
            event.msg = (void *)packet_id;

            if (_mqtt_conncection->event_handler) {
                _mqtt_conncection->event_handler(_mqtt_conncection->fd, &event, _mqtt_conncection->cb_data);
            }
        }
        break;

        case IOTX_MQTT_EVENT_PUBLISH_NACK:
        case IOTX_MQTT_EVENT_PUBLISH_TIMEOUT: {
            iotx_cm_event_msg_t event;
            event.type = IOTX_CM_EVENT_PUBLISH_FAILED;
            event.msg = (void *)packet_id;

            if (_mqtt_conncection->event_handler) {
                _mqtt_conncection->event_handler(_mqtt_conncection->fd, &event, _mqtt_conncection->cb_data);
            }
        }
        break;

        case IOTX_MQTT_EVENT_PUBLISH_RECEIVED: {
            iotx_mqtt_topic_info_pt topic_info = (iotx_mqtt_topic_info_pt)msg->msg;
            iotx_cm_data_handle_cb topic_handle_func = (iotx_cm_data_handle_cb)pcontext;
            char *topic = NULL;

            if (topic_handle_func == NULL) {
                iotx_state_event(ITE_STATE_DEV_MODEL, STATE_DEV_MODEL_RECV_UNEXP_MQTT_PUB, "unexpected pub message received");
                return;
            }

            topic = cm_malloc(topic_info->topic_len + 1);
            if (topic == NULL) {
                iotx_state_event(ITE_STATE_DEV_MODEL, STATE_SYS_DEPEND_MALLOC, NULL);
                return;
            }
            memset(topic, 0, topic_info->topic_len + 1);
            memcpy(topic, topic_info->ptopic, topic_info->topic_len);

            topic_handle_func(_mqtt_conncection->fd, topic, topic_info->payload, topic_info->payload_len, NULL);

            cm_free(topic);
        }
        break;

        case IOTX_MQTT_EVENT_BUFFER_OVERFLOW:
            iotx_state_event(ITE_STATE_DEV_MODEL, STATE_DEV_MODEL_INTERNAL_ERROR, "mqtt read buffer too small");
            break;

        default:
            break;
    }
}

extern sdk_impl_ctx_t g_sdk_impl_ctx;
static int _mqtt_connect(uint32_t timeout)
{
    void *pclient;
    iotx_time_t timer;
    iotx_cm_event_msg_t event;

    char product_key[IOTX_PRODUCT_KEY_LEN + 1] = {0};
    char device_name[IOTX_DEVICE_NAME_LEN + 1] = {0};
    char device_secret[IOTX_DEVICE_SECRET_LEN + 1] = {0};

    if (_mqtt_conncection == NULL) {
        return STATE_DEV_MODEL_INTERNAL_MQTT_NOT_INIT_YET;
    }

    IOT_Ioctl(IOTX_IOCTL_GET_PRODUCT_KEY, product_key);
    IOT_Ioctl(IOTX_IOCTL_GET_DEVICE_NAME, device_name);
    IOT_Ioctl(IOTX_IOCTL_GET_DEVICE_SECRET, device_secret);

    if (strlen(product_key) == 0) {
        return STATE_USER_INPUT_PK;
    }
    if (strlen(device_name) == 0) {
        return STATE_USER_INPUT_DN;
    }

    iotx_time_init(&timer);
    utils_time_countdown_ms(&timer, timeout);

    if (g_sdk_impl_ctx.mqtt_customzie_info[0] != '\0') {
        ((iotx_mqtt_param_t *)_mqtt_conncection->open_params)->customize_info = g_sdk_impl_ctx.mqtt_customzie_info;
    }
    if (g_sdk_impl_ctx.mqtt_port_num != 0) {
        ((iotx_mqtt_param_t *)_mqtt_conncection->open_params)->port = g_sdk_impl_ctx.mqtt_port_num;
    }

    do {
        pclient = IOT_MQTT_Construct((iotx_mqtt_param_t *)_mqtt_conncection->open_params);
        if (pclient != NULL) {
            iotx_cm_event_msg_t event;
            _mqtt_conncection->context = pclient;
            event.type = IOTX_CM_EVENT_CLOUD_CONNECTED;
            event.msg = NULL;

            if (_mqtt_conncection->event_handler) {
                _mqtt_conncection->event_handler(_mqtt_conncection->fd, &event, (void *)_mqtt_conncection);
            }
            return STATE_SUCCESS;
        }
        HAL_SleepMs(500);
    } while (!utils_time_is_expired(&timer));

    event.type = IOTX_CM_EVENT_CLOUD_CONNECT_FAILED;
    event.msg = NULL;

    if (_mqtt_conncection->event_handler) {
        _mqtt_conncection->event_handler(_mqtt_conncection->fd, &event, (void *)_mqtt_conncection);
    }

    return STATE_DEV_MODEL_MQTT_CONNECT_FAILED;
}

static int _mqtt_publish(iotx_cm_ext_params_t *ext, const char *topic, const char *payload, unsigned int payload_len)
{
    int qos = 0;

    if (_mqtt_conncection == NULL) {
        return STATE_DEV_MODEL_INTERNAL_MQTT_NOT_INIT_YET;
    }

    if (ext != NULL) {
        qos = (int)_get_mqtt_qos(ext->ack_type);
    }
    return IOT_MQTT_Publish_Simple(_mqtt_conncection->context, topic, qos, (void *)payload, payload_len);
}

static int _mqtt_yield(uint32_t timeout)
{
    if (_mqtt_conncection == NULL) {
        return STATE_DEV_MODEL_INTERNAL_MQTT_NOT_INIT_YET;
    }

    return IOT_MQTT_Yield(_mqtt_conncection->context, timeout);
}

static int _mqtt_sub(iotx_cm_ext_params_t *ext, const char *topic,
                     iotx_cm_data_handle_cb topic_handle_func, void *pcontext)
{
    int sync = 0;
    iotx_mqtt_qos_t qos = IOTX_MQTT_QOS0;
    int timeout = 0;
    int ret;

    if (_mqtt_conncection == NULL) {
        return STATE_DEV_MODEL_INTERNAL_MQTT_NOT_INIT_YET;
    }

    if (ext != NULL) {
        if (ext->sync_mode == IOTX_CM_ASYNC) {
            sync = 0;
        } else {
            sync = 1;
            timeout = ext->sync_timeout;
        }
        qos = (int)_get_mqtt_qos(ext->ack_type);
    }

    if (sync != 0) {
        ret = IOT_MQTT_Subscribe_Sync(_mqtt_conncection->context,
                                      topic,
                                      qos,
                                      iotx_cloud_conn_mqtt_event_handle,
                                      (void *)topic_handle_func,
                                      timeout);
    } else {
        ret = IOT_MQTT_Subscribe(_mqtt_conncection->context,
                                 topic,
                                 qos,
                                 iotx_cloud_conn_mqtt_event_handle,
                                 (void *)topic_handle_func);
    }

    return ret;
}

static int _mqtt_unsub(const char *topic)
{
    if (_mqtt_conncection == NULL) {
        return STATE_DEV_MODEL_INTERNAL_MQTT_NOT_INIT_YET;
    }

    return IOT_MQTT_Unsubscribe(_mqtt_conncection->context, topic);
}

static int _mqtt_close()
{
    if (_mqtt_conncection == NULL) {
        return STATE_DEV_MODEL_INTERNAL_MQTT_NOT_INIT_YET;
    }

    cm_free(_mqtt_conncection->open_params);
    IOT_MQTT_Destroy(&_mqtt_conncection->context);
    cm_free(_mqtt_conncection);
    _mqtt_conncection = NULL;
    return STATE_SUCCESS;
}

static iotx_mqtt_qos_t _get_mqtt_qos(iotx_cm_ack_types_t ack_type)
{
    switch (ack_type) {
        case IOTX_CM_MESSAGE_NO_ACK:
            return IOTX_MQTT_QOS0;

        case IOTX_CM_MESSAGE_NEED_ACK:
            return IOTX_MQTT_QOS1;

        case IOTX_CM_MESSAGE_SUB_LOCAL:
            return IOTX_MQTT_QOS3_SUB_LOCAL;

        default:
            return IOTX_MQTT_QOS0;
    }
}

static void _set_common_handlers()
{
    if (_mqtt_conncection != NULL) {
        _mqtt_conncection->connect_func = _mqtt_connect;
        _mqtt_conncection->sub_func = _mqtt_sub;
        _mqtt_conncection->unsub_func = _mqtt_unsub;
        _mqtt_conncection->pub_func = _mqtt_publish;
        _mqtt_conncection->yield_func = (iotx_cm_yield_fp)_mqtt_yield;
        _mqtt_conncection->close_func = _mqtt_close;
    }
}
#endif

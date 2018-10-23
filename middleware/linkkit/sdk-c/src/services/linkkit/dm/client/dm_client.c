#include "iotx_dm_internal.h"

static dm_client_uri_map_t g_dm_client_uri_map[] = {
    {DM_URI_THING_SERVICE_PROPERTY_SET,       DM_URI_SYS_PREFIX,         IOTX_DM_DEVICE_ALL,     dm_client_thing_service_property_set         },
    {DM_URI_THING_SERVICE_REQUEST_WILDCARD,   DM_URI_SYS_PREFIX,         IOTX_DM_DEVICE_ALL,     dm_client_thing_service_request              },
    {DM_URI_THING_EVENT_POST_REPLY_WILDCARD,  DM_URI_SYS_PREFIX,         IOTX_DM_DEVICE_ALL,     dm_client_thing_event_post_reply             },
    {DM_URI_THING_MODEL_DOWN_RAW,             DM_URI_SYS_PREFIX,         IOTX_DM_DEVICE_ALL,     dm_client_thing_model_down_raw               },
    {DM_URI_THING_DEVICEINFO_UPDATE_REPLY,    DM_URI_SYS_PREFIX,         IOTX_DM_DEVICE_ALL,     dm_client_thing_deviceinfo_update_reply      },
    {DM_URI_THING_DEVICEINFO_DELETE_REPLY,    DM_URI_SYS_PREFIX,         IOTX_DM_DEVICE_ALL,     dm_client_thing_deviceinfo_delete_reply      },
    {DM_URI_THING_DYNAMICTSL_GET_REPLY,       DM_URI_SYS_PREFIX,         IOTX_DM_DEVICE_ALL,     dm_client_thing_dynamictsl_get_reply         },
    {DM_URI_THING_MODEL_UP_RAW_REPLY,         DM_URI_SYS_PREFIX,         IOTX_DM_DEVICE_ALL,     dm_client_thing_model_up_raw_reply           },
    {DM_URI_RRPC_REQUEST_WILDCARD,            DM_URI_SYS_PREFIX,         IOTX_DM_DEVICE_ALL,     dm_client_rrpc_request_wildcard              },
    {DM_URI_NTP_RESPONSE,                     DM_URI_EXT_NTP_PREFIX,     IOTX_DM_DEVICE_ALL,     dm_client_ntp_response                       },
    {NULL,                                    DM_URI_EXT_ERROR_PREFIX,   IOTX_DM_DEVICE_ALL,     dm_client_ext_error                          },
#ifdef DEVICE_MODEL_GATEWAY
    {DM_URI_THING_TOPO_ADD_NOTIFY,            DM_URI_SYS_PREFIX,         IOTX_DM_DEVICE_GATEWAY, dm_client_thing_topo_add_notify              },
    {DM_URI_THING_GATEWAY_PERMIT,             DM_URI_SYS_PREFIX,         IOTX_DM_DEVICE_GATEWAY, dm_client_thing_gateway_permit               },
    {DM_URI_THING_SUB_REGISTER_REPLY,         DM_URI_SYS_PREFIX,         IOTX_DM_DEVICE_GATEWAY, dm_client_thing_sub_register_reply           },
    {DM_URI_THING_SUB_UNREGISTER_REPLY,       DM_URI_SYS_PREFIX,         IOTX_DM_DEVICE_GATEWAY, dm_client_thing_sub_unregister_reply         },
    {DM_URI_THING_TOPO_ADD_REPLY,             DM_URI_SYS_PREFIX,         IOTX_DM_DEVICE_GATEWAY, dm_client_thing_topo_add_reply               },
    {DM_URI_THING_TOPO_DELETE_REPLY,          DM_URI_SYS_PREFIX,         IOTX_DM_DEVICE_GATEWAY, dm_client_thing_topo_delete_reply            },
    {DM_URI_THING_TOPO_GET_REPLY,             DM_URI_SYS_PREFIX,         IOTX_DM_DEVICE_GATEWAY, dm_client_thing_topo_get_reply               },
    {DM_URI_THING_LIST_FOUND_REPLY,           DM_URI_SYS_PREFIX,         IOTX_DM_DEVICE_GATEWAY, dm_client_thing_list_found_reply             },
    {DM_URI_COMBINE_LOGIN_REPLY,              DM_URI_EXT_SESSION_PREFIX, IOTX_DM_DEVICE_GATEWAY, dm_client_combine_login_reply                },
    {DM_URI_COMBINE_LOGOUT_REPLY,             DM_URI_EXT_SESSION_PREFIX, IOTX_DM_DEVICE_GATEWAY, dm_client_combine_logout_reply               },
    {DM_URI_THING_DISABLE,                    DM_URI_SYS_PREFIX,         IOTX_DM_DEVICE_GATEWAY, dm_client_thing_disable                      },
    {DM_URI_THING_ENABLE,                     DM_URI_SYS_PREFIX,         IOTX_DM_DEVICE_GATEWAY, dm_client_thing_enable                       },
    {DM_URI_THING_DELETE,                     DM_URI_SYS_PREFIX,         IOTX_DM_DEVICE_GATEWAY, dm_client_thing_delete                       },
#endif
};

static int _dm_client_subscribe_filter(char *uri, char *uri_name, char product_key[PRODUCT_KEY_MAXLEN],
                                       char device_name[DEVICE_NAME_MAXLEN])
{
    int res = 0;

    if (uri_name == NULL) {
        return SUCCESS_RETURN;
    }

    if (strlen(uri_name) == strlen(DM_URI_THING_EVENT_POST_REPLY_WILDCARD) &&
        memcmp(uri_name, DM_URI_THING_EVENT_POST_REPLY_WILDCARD, strlen(uri_name)) == 0) {
        int event_post_reply_opt = 0;
        res = dm_opt_get(DM_OPT_DOWNSTREAM_EVENT_POST_REPLY, &event_post_reply_opt);
        if (res == SUCCESS_RETURN && event_post_reply_opt == 0) {
            dm_client_unsubscribe(uri);
            return FAIL_RETURN;
        }
    }

    return SUCCESS_RETURN;
}

int dm_client_subscribe_all(char product_key[PRODUCT_KEY_MAXLEN], char device_name[DEVICE_NAME_MAXLEN], int dev_type)
{
    int res = 0, index = 0, fail_count = 0;
    int number = sizeof(g_dm_client_uri_map) / sizeof(dm_client_uri_map_t);
    char *uri = NULL;

    for (index = 0; index < number; index++) {
        if ((g_dm_client_uri_map[index].dev_type & dev_type) == 0) {
            continue;
        }
        dm_log_info("index: %d", index);

        res = dm_utils_service_name((char *)g_dm_client_uri_map[index].uri_prefix, (char *)g_dm_client_uri_map[index].uri_name,
                                    product_key, device_name, &uri);
        if (res < SUCCESS_RETURN) {
            index--;
            continue;
        }
        res = _dm_client_subscribe_filter(uri, (char *)g_dm_client_uri_map[index].uri_name, product_key, device_name);
        if (res < SUCCESS_RETURN) {
            DM_free(uri);
            continue;
        }
        res = dm_client_subscribe(uri, (void *)g_dm_client_uri_map[index].callback, NULL);
        if (res < SUCCESS_RETURN) {
            if (fail_count < 2) {
                index--;
                fail_count++;
            } else {
                fail_count = 0;
            }
            DM_free(uri);
            continue;
        } else {
            fail_count = 0;
        }
        DM_free(uri);
    }

    return SUCCESS_RETURN;
}

static void _dm_client_event_cloud_connected_handle(void)
{
    dm_log_info("IOTX_CM_EVENT_CLOUD_CONNECTED");

#ifdef OTA_ENABLED
    int res = 0;

    /* DM OTA Module Init */
    res = dm_ota_init();
    if (res != SUCCESS_RETURN) {
        return;
    }

    /* DM Config OTA Module Init */
    dm_cota_init();

    /* DM Firmware OTA Mudule Init */
    dm_fota_init();
#endif

#ifdef DEV_BIND_ENABLED
    awss_report_cloud();
#endif
    dm_msg_cloud_connected();
}

static void _dm_client_event_cloud_disconnect_handle(void)
{
    dm_log_info("IOTX_CM_EVENT_CLOUD_DISCONNECT");

    dm_msg_cloud_disconnect();
}

void dm_client_event_handle(int fd, iotx_cm_event_msg_t *event, void *context)
{
    switch (event->type) {
        case IOTX_CM_EVENT_CLOUD_CONNECTED: {
            _dm_client_event_cloud_connected_handle();
        }
        break;
        case IOTX_CM_EVENT_CLOUD_CONNECT_FAILED: {

        }
        break;
        case IOTX_CM_EVENT_CLOUD_DISCONNECT: {
            _dm_client_event_cloud_disconnect_handle();
        }
        break;
        default:
            break;
    }
}

void dm_client_thing_service_property_set(int fd, const char *topic, const char *payload, unsigned int payload_len,
        void *context)
{
    int res = 0;
    dm_msg_source_t source;
    dm_msg_dest_t dest;
    dm_msg_request_payload_t request;
    dm_msg_response_t response;

    memset(&source, 0, sizeof(dm_msg_source_t));
    memset(&dest, 0, sizeof(dm_msg_dest_t));
    memset(&request, 0, sizeof(dm_msg_request_payload_t));
    memset(&response, 0, sizeof(dm_msg_response_t));

    source.uri = topic;
    source.payload = (unsigned char *)payload;
    source.payload_len = payload_len;
    source.context = NULL;

    dest.uri_name = DM_URI_THING_SERVICE_PROPERTY_SET_REPLY;

    res = dm_msg_proc_thing_service_property_set(&source, &dest, &request, &response);
    if (res < SUCCESS_RETURN) {
        return;
    }

    int prop_set_reply_opt = 0;
    res = dm_opt_get(DM_OPT_UPSTREAM_PROPERTY_SET_REPLY, &prop_set_reply_opt);
    if (res == SUCCESS_RETURN) {
        if (prop_set_reply_opt) {
            dm_msg_response(DM_MSG_DEST_CLOUD, &request, &response, "{}", strlen("{}"), NULL);
        }
    }
}

void dm_client_thing_service_request(int fd, const char *topic, const char *payload, unsigned int payload_len,
                                     void *context)
{
    dm_msg_source_t source;

    memset(&source, 0, sizeof(dm_msg_source_t));

    source.uri = topic;
    source.payload = (unsigned char *)payload;
    source.payload_len = payload_len;
    source.context = NULL;

    dm_msg_proc_thing_service_request(&source);
}

void dm_client_thing_event_post_reply(int fd, const char *topic, const char *payload, unsigned int payload_len,
                                      void *context)
{
    dm_msg_source_t source;

    memset(&source, 0, sizeof(dm_msg_source_t));

    source.uri = topic;
    source.payload = (unsigned char *)payload;
    source.payload_len = payload_len;
    source.context = NULL;

    dm_msg_proc_thing_event_post_reply(&source);
}

void dm_client_thing_model_down_raw(int fd, const char *topic, const char *payload, unsigned int payload_len,
                                    void *context)
{
    dm_msg_source_t source;

    memset(&source, 0, sizeof(dm_msg_source_t));

    source.uri = topic;
    source.payload = (unsigned char *)payload;
    source.payload_len = payload_len;
    source.context = NULL;

    dm_msg_proc_thing_model_down_raw(&source);
}

void dm_client_thing_deviceinfo_update_reply(int fd, const char *topic, const char *payload, unsigned int payload_len,
        void *context)
{
    dm_msg_source_t source;

    memset(&source, 0, sizeof(dm_msg_source_t));

    source.uri = topic;
    source.payload = (unsigned char *)payload;
    source.payload_len = payload_len;
    source.context = NULL;

    dm_msg_proc_thing_deviceinfo_update_reply(&source);
}

void dm_client_thing_deviceinfo_delete_reply(int fd, const char *topic, const char *payload, unsigned int payload_len,
        void *context)
{
    dm_msg_source_t source;

    memset(&source, 0, sizeof(dm_msg_source_t));

    source.uri = topic;
    source.payload = (unsigned char *)payload;
    source.payload_len = payload_len;
    source.context = NULL;

    dm_msg_proc_thing_deviceinfo_delete_reply(&source);
}

void dm_client_thing_dynamictsl_get_reply(int fd, const char *topic, const char *payload, unsigned int payload_len,
        void *context)
{
    dm_msg_source_t source;

    memset(&source, 0, sizeof(dm_msg_source_t));

    source.uri = topic;
    source.payload = (unsigned char *)payload;
    source.payload_len = payload_len;
    source.context = NULL;

    dm_msg_proc_thing_dynamictsl_get_reply(&source);
}

void dm_client_thing_model_up_raw_reply(int fd, const char *topic, const char *payload, unsigned int payload_len,
                                        void *context)
{
    dm_msg_source_t source;

    memset(&source, 0, sizeof(dm_msg_source_t));

    source.uri = topic;
    source.payload = (unsigned char *)payload;
    source.payload_len = payload_len;
    source.context = NULL;

    dm_msg_proc_thing_model_up_raw_reply(&source);
}

void dm_client_rrpc_request_wildcard(int fd, const char *topic, const char *payload, unsigned int payload_len,
                                     void *context)
{
    dm_msg_source_t source;

    memset(&source, 0, sizeof(dm_msg_source_t));

    source.uri = topic;
    source.payload = (unsigned char *)payload;
    source.payload_len = payload_len;
    source.context = NULL;

    dm_msg_proc_rrpc_request(&source);
}

void dm_client_ntp_response(int fd, const char *topic, const char *payload, unsigned int payload_len, void *context)
{
    dm_msg_source_t source;

    memset(&source, 0, sizeof(dm_msg_source_t));

    source.uri = topic;
    source.payload = (unsigned char *)payload;
    source.payload_len = payload_len;
    source.context = NULL;

    dm_disp_ntp_response(&source);
}

void dm_client_ext_error(int fd, const char *topic, const char *payload, unsigned int payload_len, void *context)
{
    dm_msg_source_t source;

    memset(&source, 0, sizeof(dm_msg_source_t));

    source.uri = topic;
    source.payload = (unsigned char *)payload;
    source.payload_len = payload_len;
    source.context = NULL;

    dm_disp_ext_error_response(&source);
}

#ifdef DEVICE_MODEL_GATEWAY
void dm_client_thing_topo_add_notify(int fd, const char *topic, const char *payload, unsigned int payload_len,
                                     void *context)
{
    int res = 0;
    dm_msg_source_t source;
    dm_msg_dest_t dest;
    dm_msg_request_payload_t request;
    dm_msg_response_t response;

    memset(&source, 0, sizeof(dm_msg_source_t));
    memset(&dest, 0, sizeof(dm_msg_dest_t));
    memset(&request, 0, sizeof(dm_msg_request_payload_t));
    memset(&response, 0, sizeof(dm_msg_response_t));

    source.uri = topic;
    source.payload = (unsigned char *)payload;
    source.payload_len = payload_len;
    source.context = NULL;

    dest.uri_name = DM_URI_THING_TOPO_ADD_NOTIFY_REPLY;

    res = dm_msg_proc_thing_topo_add_notify(&source, &dest, &request, &response);
    if (res < SUCCESS_RETURN) {
        return;
    }

    dm_msg_response(DM_MSG_DEST_CLOUD, &request, &response, "{}", strlen("{}"), NULL);
}

void dm_client_thing_disable(int fd, const char *topic, const char *payload, unsigned int payload_len, void *context)
{
    int res = 0;
    dm_msg_source_t source;
    dm_msg_dest_t dest;
    dm_msg_request_payload_t request;
    dm_msg_response_t response;

    memset(&source, 0, sizeof(dm_msg_source_t));
    memset(&dest, 0, sizeof(dm_msg_dest_t));
    memset(&request, 0, sizeof(dm_msg_request_payload_t));
    memset(&response, 0, sizeof(dm_msg_response_t));

    source.uri = topic;
    source.payload = (unsigned char *)payload;
    source.payload_len = payload_len;
    source.context = NULL;

    dest.uri_name = DM_URI_THING_DISABLE_REPLY;

    res = dm_msg_proc_thing_disable(&source, &dest, &request, &response);
    if (res < SUCCESS_RETURN) {
        return;
    }

    dm_msg_response(DM_MSG_DEST_CLOUD, &request, &response, "{}", strlen("{}"), NULL);
}

void dm_client_thing_enable(int fd, const char *topic, const char *payload, unsigned int payload_len, void *context)
{
    int res = 0;
    dm_msg_source_t source;
    dm_msg_dest_t dest;
    dm_msg_request_payload_t request;
    dm_msg_response_t response;

    memset(&source, 0, sizeof(dm_msg_source_t));
    memset(&dest, 0, sizeof(dm_msg_dest_t));
    memset(&request, 0, sizeof(dm_msg_request_payload_t));
    memset(&response, 0, sizeof(dm_msg_response_t));

    source.uri = topic;
    source.payload = (unsigned char *)payload;
    source.payload_len = payload_len;
    source.context = NULL;

    dest.uri_name = DM_URI_THING_ENABLE_REPLY;

    res = dm_msg_proc_thing_enable(&source, &dest, &request, &response);
    if (res < SUCCESS_RETURN) {
        return;
    }

    dm_msg_response(DM_MSG_DEST_CLOUD, &request, &response, "{}", strlen("{}"), NULL);
}

void dm_client_thing_delete(int fd, const char *topic, const char *payload, unsigned int payload_len, void *context)
{
    int res = 0;
    dm_msg_source_t source;
    dm_msg_dest_t dest;
    dm_msg_request_payload_t request;
    dm_msg_response_t response;

    memset(&source, 0, sizeof(dm_msg_source_t));
    memset(&dest, 0, sizeof(dm_msg_dest_t));
    memset(&request, 0, sizeof(dm_msg_request_payload_t));
    memset(&response, 0, sizeof(dm_msg_response_t));

    source.uri = topic;
    source.payload = (unsigned char *)payload;
    source.payload_len = payload_len;
    source.context = NULL;

    dest.uri_name = DM_URI_THING_DELETE_REPLY;

    res = dm_msg_proc_thing_delete(&source, &dest, &request, &response);
    if (res < SUCCESS_RETURN) {
        return;
    }

    dm_msg_response(DM_MSG_DEST_CLOUD, &request, &response, "{}", strlen("{}"), NULL);
}

void dm_client_thing_gateway_permit(int fd, const char *topic, const char *payload, unsigned int payload_len,
                                    void *context)
{
    int res = 0;
    dm_msg_source_t source;
    dm_msg_dest_t dest;
    dm_msg_request_payload_t request;
    dm_msg_response_t response;

    memset(&source, 0, sizeof(dm_msg_source_t));
    memset(&dest, 0, sizeof(dm_msg_dest_t));
    memset(&request, 0, sizeof(dm_msg_request_payload_t));
    memset(&response, 0, sizeof(dm_msg_response_t));

    source.uri = topic;
    source.payload = (unsigned char *)payload;
    source.payload_len = payload_len;
    source.context = NULL;

    dest.uri_name = DM_URI_THING_GATEWAY_PERMIT_REPLY;

    res = dm_msg_proc_thing_gateway_permit(&source, &dest, &request, &response);
    if (res < SUCCESS_RETURN) {
        return;
    }

    dm_msg_response(DM_MSG_DEST_CLOUD, &request, &response, "{}", strlen("{}"), NULL);
}

void dm_client_thing_sub_register_reply(int fd, const char *topic, const char *payload, unsigned int payload_len,
                                        void *context)
{
    dm_msg_source_t source;

    memset(&source, 0, sizeof(dm_msg_source_t));

    source.uri = topic;
    source.payload = (unsigned char *)payload;
    source.payload_len = payload_len;
    source.context = NULL;

    dm_msg_proc_thing_sub_register_reply(&source);
}

void dm_client_thing_sub_unregister_reply(int fd, const char *topic, const char *payload, unsigned int payload_len,
        void *context)
{
    dm_msg_source_t source;

    memset(&source, 0, sizeof(dm_msg_source_t));

    source.uri = topic;
    source.payload = (unsigned char *)payload;
    source.payload_len = payload_len;
    source.context = NULL;

    dm_msg_proc_thing_sub_unregister_reply(&source);
}

void dm_client_thing_topo_add_reply(int fd, const char *topic, const char *payload, unsigned int payload_len,
                                    void *context)
{
    dm_msg_source_t source;

    memset(&source, 0, sizeof(dm_msg_source_t));

    source.uri = topic;
    source.payload = (unsigned char *)payload;
    source.payload_len = payload_len;
    source.context = NULL;

    dm_msg_proc_thing_topo_add_reply(&source);
}

void dm_client_thing_topo_delete_reply(int fd, const char *topic, const char *payload, unsigned int payload_len,
                                       void *context)
{
    dm_msg_source_t source;

    memset(&source, 0, sizeof(dm_msg_source_t));

    source.uri = topic;
    source.payload = (unsigned char *)payload;
    source.payload_len = payload_len;
    source.context = NULL;

    dm_msg_proc_thing_topo_delete_reply(&source);
}

void dm_client_thing_topo_get_reply(int fd, const char *topic, const char *payload, unsigned int payload_len,
                                    void *context)
{
    dm_msg_source_t source;

    memset(&source, 0, sizeof(dm_msg_source_t));

    source.uri = topic;
    source.payload = (unsigned char *)payload;
    source.payload_len = payload_len;
    source.context = NULL;

    dm_msg_proc_thing_topo_get_reply(&source);
}

void dm_client_thing_list_found_reply(int fd, const char *topic, const char *payload, unsigned int payload_len,
                                      void *context)
{
    dm_msg_source_t source;

    memset(&source, 0, sizeof(dm_msg_source_t));

    source.uri = topic;
    source.payload = (unsigned char *)payload;
    source.payload_len = payload_len;
    source.context = NULL;

    dm_msg_proc_thing_list_found_reply(&source);
}

void dm_client_combine_login_reply(int fd, const char *topic, const char *payload, unsigned int payload_len,
                                   void *context)
{
    dm_msg_source_t source;

    memset(&source, 0, sizeof(dm_msg_source_t));

    source.uri = topic;
    source.payload = (unsigned char *)payload;
    source.payload_len = payload_len;
    source.context = NULL;

    dm_msg_proc_combine_login_reply(&source);
}

void dm_client_combine_logout_reply(int fd, const char *topic, const char *payload, unsigned int payload_len,
                                    void *context)
{
    dm_msg_source_t source;

    memset(&source, 0, sizeof(dm_msg_source_t));

    source.uri = topic;
    source.payload = (unsigned char *)payload;
    source.payload_len = payload_len;
    source.context = NULL;

    dm_msg_proc_combine_logout_reply(&source);
}
#endif
/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#if defined(SDK_ENHANCE)

#include "iot_export_linkkit.h"
#include "sdk-impl_internal.h"
#include "lite-cjson.h"
#include "iotx_dm.h"

#define IOTX_LINKKIT_KEY_ID          "id"
#define IOTX_LINKKIT_KEY_CODE        "code"
#define IOTX_LINKKIT_KEY_DEVID       "devid"
#define IOTX_LINKKIT_KEY_SERVICEID   "serviceid"
#define IOTX_LINKKIT_KEY_PROPERTYID  "propertyid"
#define IOTX_LINKKIT_KEY_EVENTID     "eventid"
#define IOTX_LINKKIT_KEY_PAYLOAD     "payload"
#define IOTX_LINKKIT_KEY_CONFIG_ID   "configId"
#define IOTX_LINKKIT_KEY_CONFIG_SIZE "configSize"
#define IOTX_LINKKIT_KEY_GET_TYPE    "getType"
#define IOTX_LINKKIT_KEY_SIGN        "sign"
#define IOTX_LINKKIT_KEY_SIGN_METHOD "signMethod"
#define IOTX_LINKKIT_KEY_URL         "url"
#define IOTX_LINKKIT_KEY_VERSION     "version"
#define IOTX_LINKKIT_KEY_UTC         "utc"
#define IOTX_LINKKIT_KEY_RRPCID      "rrpcid"
#define IOTX_LINKKIT_KEY_CTX         "ctx"

typedef struct {
    void *mutex;
    void *dispatch_thread;
    int is_opened;
    int is_started;
    iotx_linkkit_event_handler_t *user_event_handler;
} iotx_linkkit_ctx_t;

static iotx_linkkit_ctx_t g_iotx_linkkit_ctx = {0};

static iotx_linkkit_ctx_t *_iotx_linkkit_get_ctx(void)
{
    return &g_iotx_linkkit_ctx;
}

static void _iotx_linkkit_mutex_lock(void)
{
    iotx_linkkit_ctx_t *ctx = _iotx_linkkit_get_ctx();
    if (ctx->mutex) {
        HAL_MutexLock(ctx->mutex);
    }
}

static void _iotx_linkkit_mutex_unlock(void)
{
    iotx_linkkit_ctx_t *ctx = _iotx_linkkit_get_ctx();
    if (ctx->mutex) {
        HAL_MutexUnlock(ctx->mutex);
    }
}

static void _iotx_linkkit_event_callback(iotx_dm_event_types_t type, char *payload)
{
    iotx_linkkit_ctx_t *ctx = _iotx_linkkit_get_ctx();

    sdk_info("Receive Message Type: %d", type);
    if (payload) {
        sdk_info("Receive Message: %s", payload);
    }

    switch (type) {
        case IOTX_DM_EVENT_CLOUD_CONNECTED: {
            if (ctx->user_event_handler->connected) {
                ctx->user_event_handler->connected();
            }
        }
        break;
        case IOTX_DM_EVENT_CLOUD_DISCONNECT: {
            if (ctx->user_event_handler->disconnected) {
                ctx->user_event_handler->disconnected();
            }
        }
        break;
        case IOTX_DM_EVENT_REGISTER_COMPLETED: {
            int res = 0;
            lite_cjson_t lite, lite_item_devid;

            if (payload == NULL) {
                return;
            }

            /* Parse Payload */
            memset(&lite, 0, sizeof(lite_cjson_t));
            res = lite_cjson_parse(payload, strlen(payload), &lite);
            if (res != SUCCESS_RETURN) {
                return;
            }

            /* Parse Devid */
            memset(&lite_item_devid, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, IOTX_LINKKIT_KEY_DEVID, strlen(IOTX_LINKKIT_KEY_DEVID), &lite_item_devid);
            if (res != SUCCESS_RETURN) {
                return;
            }
            sdk_debug("Current Devid: %d", lite_item_devid.value_int);

            if (ctx->user_event_handler->initialized) {
                ctx->user_event_handler->initialized(lite_item_devid.value_int);
            }
        }
        break;
        case IOTX_DM_EVENT_THING_SERVICE_REQUEST: {
            int res = 0, response_len = 0;
            char *request = NULL, *response = NULL;
            lite_cjson_t lite, lite_item_id, lite_item_devid, lite_item_serviceid, lite_item_payload;

            if (payload == NULL) {
                return;
            }

            /* Parse Payload */
            memset(&lite, 0, sizeof(lite_cjson_t));
            res = lite_cjson_parse(payload, strlen(payload), &lite);
            if (res != SUCCESS_RETURN) {
                return;
            }

            /* Parse Id */
            memset(&lite_item_id, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, IOTX_LINKKIT_KEY_ID, strlen(IOTX_LINKKIT_KEY_ID), &lite_item_id);
            if (res != SUCCESS_RETURN) {
                return;
            }
            sdk_debug("Current Id: %.*s", lite_item_id.value_length, lite_item_id.value);

            /* Parse Devid */
            memset(&lite_item_devid, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, IOTX_LINKKIT_KEY_DEVID, strlen(IOTX_LINKKIT_KEY_DEVID), &lite_item_devid);
            if (res != SUCCESS_RETURN) {
                return;
            }
            sdk_debug("Current Devid: %d", lite_item_devid.value_int);

            /* Parse Serviceid */
            memset(&lite_item_serviceid, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, IOTX_LINKKIT_KEY_SERVICEID, strlen(IOTX_LINKKIT_KEY_SERVICEID),
                                         &lite_item_serviceid);
            if (res != SUCCESS_RETURN) {
                return;
            }
            sdk_debug("Current ServiceID: %.*s", lite_item_serviceid.value_length, lite_item_serviceid.value);

            /* Parse Payload */
            memset(&lite_item_payload, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, IOTX_LINKKIT_KEY_PAYLOAD, strlen(IOTX_LINKKIT_KEY_PAYLOAD), &lite_item_payload);
            if (res != SUCCESS_RETURN) {
                return;
            }
            sdk_debug("Current Payload: %.*s", lite_item_payload.value_length, lite_item_payload.value);

            request = HAL_Malloc(lite_item_payload.value_length + 1);
            if (request == NULL) {
                sdk_err("Not Enough Memory");
                return;
            }
            memset(request, 0, lite_item_payload.value_length + 1);
            memcpy(request, lite_item_payload.value, lite_item_payload.value_length);

            if (ctx->user_event_handler->async_service_request) {
                res = ctx->user_event_handler->async_service_request(lite_item_devid.value_int, lite_item_serviceid.value,
                        lite_item_serviceid.value_length,
                        request, lite_item_payload.value_length, &response, &response_len);
                if (response != NULL && response_len > 0) {
                    /* service response exist */
                    iotx_dm_error_code_t code = (res == 0) ? (IOTX_DM_ERR_CODE_SUCCESS) : (IOTX_DM_ERR_CODE_REQUEST_ERROR);
                    iotx_dm_send_service_response(lite_item_devid.value_int, lite_item_id.value, lite_item_id.value_length, code,
                                                  lite_item_serviceid.value,
                                                  lite_item_serviceid.value_length,
                                                  response, response_len);
                    HAL_Free(response);
                }
            }

            HAL_Free(request);
        }
        break;
        case IOTX_DM_EVENT_PROPERTY_SET: {
            int res = 0;
            char *property_payload = NULL;
            lite_cjson_t lite, lite_item_devid, lite_item_payload;

            if (payload == NULL) {
                return;
            }

            /* Parse Payload */
            memset(&lite, 0, sizeof(lite_cjson_t));
            res = lite_cjson_parse(payload, strlen(payload), &lite);
            if (res != SUCCESS_RETURN) {
                return;
            }

            /* Parse Devid */
            memset(&lite_item_devid, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, IOTX_LINKKIT_KEY_DEVID, strlen(IOTX_LINKKIT_KEY_DEVID), &lite_item_devid);
            if (res != SUCCESS_RETURN) {
                return;
            }
            sdk_debug("Current Devid: %d", lite_item_devid.value_int);

            /* Parse Payload */
            memset(&lite_item_payload, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, IOTX_LINKKIT_KEY_PAYLOAD, strlen(IOTX_LINKKIT_KEY_PAYLOAD),
                                         &lite_item_payload);
            if (res != SUCCESS_RETURN) {
                return;
            }
            sdk_debug("Current Payload: %.*s", lite_item_payload.value_length, lite_item_payload.value);

            property_payload = HAL_Malloc(lite_item_payload.value_length + 1);
            if (property_payload == NULL) {
                sdk_err("No Enough Memory");
                return;
            }
            memset(property_payload, 0, lite_item_payload.value_length + 1);
            memcpy(property_payload, lite_item_payload.value, lite_item_payload.value_length);

            if (ctx->user_event_handler->property_set) {
                ctx->user_event_handler->property_set(lite_item_devid.value_int, property_payload,
                                                      lite_item_payload.value_length);
            }

            HAL_Free(property_payload);
        }
        break;
        case IOTX_DM_EVENT_PROPERTY_GET: {
            int res = 0, response_len = 0;
            char *request = NULL, *response = NULL;
            uintptr_t property_get_ctx_num = 0;
            void *property_get_ctx = NULL;
            lite_cjson_t lite, lite_item_id, lite_item_devid, lite_item_payload, lite_item_ctx;

            if (payload == NULL) {
                return;
            }

            /* Parse Payload */
            memset(&lite, 0, sizeof(lite_cjson_t));
            res = lite_cjson_parse(payload, strlen(payload), &lite);
            if (res != SUCCESS_RETURN) {
                return;
            }

            /* Parse Id */
            memset(&lite_item_id, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, IOTX_LINKKIT_KEY_ID, strlen(IOTX_LINKKIT_KEY_ID), &lite_item_id);
            if (res != SUCCESS_RETURN) {
                return;
            }
            sdk_debug("Current Id: %.*s", lite_item_id.value_length, lite_item_id.value);

            /* Parse Devid */
            memset(&lite_item_devid, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, IOTX_LINKKIT_KEY_DEVID, strlen(IOTX_LINKKIT_KEY_DEVID), &lite_item_devid);
            if (res != SUCCESS_RETURN) {
                return;
            }
            sdk_debug("Current Devid: %d", lite_item_devid.value_int);

            /* Parse Payload */
            memset(&lite_item_payload, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, IOTX_LINKKIT_KEY_PAYLOAD, strlen(IOTX_LINKKIT_KEY_PAYLOAD),
                                         &lite_item_payload);
            if (res != SUCCESS_RETURN) {
                return;
            }
            sdk_debug("Current Payload: %.*s", lite_item_payload.value_length, lite_item_payload.value);

            /* Parse Ctx */
            memset(&lite_item_ctx, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, IOTX_LINKKIT_KEY_CTX, strlen(IOTX_LINKKIT_KEY_CTX),
                                         &lite_item_ctx);
            if (res != SUCCESS_RETURN) {
                return;
            }
            sdk_debug("Current Ctx: %.*s", lite_item_ctx.value_length, lite_item_ctx.value);

            LITE_hexstr_convert(lite_item_ctx.value, lite_item_ctx.value_length, (unsigned char *)&property_get_ctx_num,
                                sizeof(uintptr_t));
            property_get_ctx = (void *)property_get_ctx_num;
            sdk_debug("property_get_ctx_num: %0x016llX", property_get_ctx_num);
            sdk_debug("property_get_ctx: %p", property_get_ctx);

            request = HAL_Malloc(lite_item_payload.value_length + 1);
            if (request == NULL) {
                sdk_err("No Enough Memory");
                return;
            }
            memset(request, 0, lite_item_payload.value_length + 1);
            memcpy(request, lite_item_payload.value, lite_item_payload.value_length);

            if (ctx->user_event_handler->property_get) {
                res = ctx->user_event_handler->property_get(lite_item_devid.value_int, request,
                        lite_item_payload.value_length, &response, &response_len);

                if (response != NULL && response_len > 0) {
                    /* property get response exist */
                    iotx_dm_error_code_t code = (res == 0) ? (IOTX_DM_ERR_CODE_SUCCESS) : (IOTX_DM_ERR_CODE_REQUEST_ERROR);
                    iotx_dm_send_property_get_response(lite_item_devid.value_int, lite_item_id.value, lite_item_id.value_length, code,
                                                       response, response_len, property_get_ctx);
                    HAL_Free(response);
                }
            }

            HAL_Free(request);
        }
        break;
        case IOTX_DM_EVENT_MODEL_DOWN_RAW: {
            int res = 0, raw_data_len = 0;
            unsigned char *raw_data = NULL;
            lite_cjson_t lite, lite_item_devid, lite_item_rawdata;

            if (payload == NULL) {
                return;
            }

            /* Parse Payload */
            memset(&lite, 0, sizeof(lite_cjson_t));
            res = lite_cjson_parse(payload, strlen(payload), &lite);
            if (res != SUCCESS_RETURN) {
                return;
            }

            /* Parse Devid */
            memset(&lite_item_devid, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, IOTX_LINKKIT_KEY_DEVID, strlen(IOTX_LINKKIT_KEY_DEVID),
                                         &lite_item_devid);
            if (res != SUCCESS_RETURN) {
                return;
            }
            sdk_debug("Current Devid: %d", lite_item_devid.value_int);

            /* Parse Raw Data */
            memset(&lite_item_rawdata, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, IOTX_LINKKIT_KEY_PAYLOAD, strlen(IOTX_LINKKIT_KEY_PAYLOAD),
                                         &lite_item_rawdata);
            if (res != SUCCESS_RETURN) {
                return;
            }
            sdk_debug("Current Raw Data: %.*s", lite_item_rawdata.value_length, lite_item_rawdata.value);

            raw_data_len = lite_item_rawdata.value_length / 2;
            raw_data = HAL_Malloc(raw_data_len);
            if (raw_data == NULL) {
                sdk_err("No Enough Memory");
                return;
            }
            LITE_hexstr_convert(lite_item_rawdata.value, lite_item_rawdata.value_length, raw_data, raw_data_len);

            HEXDUMP_DEBUG(raw_data, raw_data_len);

            if (ctx->user_event_handler->down_raw) {
                ctx->user_event_handler->down_raw(lite_item_devid.value_int, raw_data, raw_data_len);
            }

            HAL_Free(raw_data);
        }
        break;
        case IOTX_DM_EVENT_MODEL_UP_RAW_REPLY: {
            int res = 0, raw_data_len = 0;
            unsigned char *raw_data = NULL;
            lite_cjson_t lite, lite_item_devid, lite_item_rawdata;

            if (payload == NULL) {
                return;
            }

            /* Parse Payload */
            memset(&lite, 0, sizeof(lite_cjson_t));
            res = lite_cjson_parse(payload, strlen(payload), &lite);
            if (res != SUCCESS_RETURN) {
                return;
            }

            /* Parse Devid */
            memset(&lite_item_devid, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, IOTX_LINKKIT_KEY_DEVID, strlen(IOTX_LINKKIT_KEY_DEVID),
                                         &lite_item_devid);
            if (res != SUCCESS_RETURN) {
                return;
            }
            sdk_debug("Current Devid: %d", lite_item_devid.value_int);

            /* Parse Raw Data */
            memset(&lite_item_rawdata, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, IOTX_LINKKIT_KEY_PAYLOAD, strlen(IOTX_LINKKIT_KEY_PAYLOAD),
                                         &lite_item_rawdata);
            if (res != SUCCESS_RETURN) {
                return;
            }
            sdk_debug("Current Raw Data: %.*s", lite_item_rawdata.value_length, lite_item_rawdata.value);

            raw_data_len = lite_item_rawdata.value_length / 2;
            raw_data = HAL_Malloc(raw_data_len);
            if (raw_data == NULL) {
                sdk_err("No Enough Memory");
                return;
            }
            LITE_hexstr_convert(lite_item_rawdata.value, lite_item_rawdata.value_length, raw_data, raw_data_len);

            HEXDUMP_DEBUG(raw_data, raw_data_len);

            if (ctx->user_event_handler->up_raw_reply) {
                ctx->user_event_handler->up_raw_reply(lite_item_devid.value_int, raw_data, raw_data_len);
            }

            HAL_Free(raw_data);
        }
        break;
        case IOTX_DM_EVENT_EVENT_PROPERTY_POST_REPLY:
        case IOTX_DM_EVENT_DEVICEINFO_UPDATE_REPLY:
        case IOTX_DM_EVENT_DEVICEINFO_DELETE_REPLY: {
            int res = 0;
            lite_cjson_t lite, lite_item_id, lite_item_code, lite_item_devid;

            if (payload == NULL) {
                return;
            }

            /* Parse Payload */
            memset(&lite, 0, sizeof(lite_cjson_t));
            res = lite_cjson_parse(payload, strlen(payload), &lite);
            if (res != SUCCESS_RETURN) {
                return;
            }

            /* Parse Id */
            memset(&lite_item_id, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, IOTX_LINKKIT_KEY_ID, strlen(IOTX_LINKKIT_KEY_ID), &lite_item_id);
            if (res != SUCCESS_RETURN) {
                return;
            }
            sdk_debug("Current Id: %d", lite_item_id.value_int);

            /* Parse Code */
            memset(&lite_item_code, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, IOTX_LINKKIT_KEY_CODE, strlen(IOTX_LINKKIT_KEY_CODE),
                                         &lite_item_code);
            if (res != SUCCESS_RETURN) {
                return;
            }
            sdk_debug("Current Code: %d", lite_item_code.value_int);

            /* Parse Devid */
            memset(&lite_item_devid, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, IOTX_LINKKIT_KEY_DEVID, strlen(IOTX_LINKKIT_KEY_DEVID),
                                         &lite_item_devid);
            if (res != SUCCESS_RETURN) {
                return;
            }
            sdk_debug("Current Devid: %d", lite_item_devid.value_int);

            if (ctx->user_event_handler->post_reply) {
                ctx->user_event_handler->post_reply(lite_item_devid.value_int, lite_item_id.value_int, lite_item_code.value_int, NULL,
                                                    0);
            }
        }
        break;
        case IOTX_DM_EVENT_EVENT_SPECIFIC_POST_REPLY: {
            int res = 0;
            char *user_payload = NULL;
            lite_cjson_t lite, lite_item_id, lite_item_code, lite_item_devid, lite_item_eventid;

            if (payload == NULL) {
                return;
            }

            /* Parse Payload */
            memset(&lite, 0, sizeof(lite_cjson_t));
            res = lite_cjson_parse(payload, strlen(payload), &lite);
            if (res != SUCCESS_RETURN) {
                return;
            }

            /* Parse Id */
            memset(&lite_item_id, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, IOTX_LINKKIT_KEY_ID, strlen(IOTX_LINKKIT_KEY_ID), &lite_item_id);
            if (res != SUCCESS_RETURN) {
                return;
            }
            sdk_debug("Current Id: %d", lite_item_id.value_int);

            /* Parse Code */
            memset(&lite_item_code, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, IOTX_LINKKIT_KEY_CODE, strlen(IOTX_LINKKIT_KEY_CODE),
                                         &lite_item_code);
            if (res != SUCCESS_RETURN) {
                return;
            }
            sdk_debug("Current Code: %d", lite_item_code.value_int);

            /* Parse Devid */
            memset(&lite_item_devid, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, IOTX_LINKKIT_KEY_DEVID, strlen(IOTX_LINKKIT_KEY_DEVID),
                                         &lite_item_devid);
            if (res != SUCCESS_RETURN) {
                return;
            }
            sdk_debug("Current Devid: %d", lite_item_devid.value_int);

            /* Parse Property ID */
            memset(&lite_item_eventid, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, IOTX_LINKKIT_KEY_EVENTID, strlen(IOTX_LINKKIT_KEY_EVENTID),
                                         &lite_item_eventid);
            if (res != SUCCESS_RETURN) {
                return;
            }
            sdk_debug("Current EventID: %.*s", lite_item_eventid.value_length, lite_item_eventid.value);

            user_payload = HAL_Malloc(lite_item_eventid.value_length + 1);
            if (user_payload == NULL) {
                sdk_err("Not Enough Memory");
                return;
            }
            memset(user_payload, 0, lite_item_eventid.value_length + 1);
            memcpy(user_payload, lite_item_eventid.value, lite_item_eventid.value_length);

            if (ctx->user_event_handler->post_reply) {
                ctx->user_event_handler->post_reply(lite_item_devid.value_int, lite_item_id.value_int, lite_item_code.value_int,
                                                    user_payload, lite_item_eventid.value_length);
            }

            HAL_Free(user_payload);
        }
        break;
        case IOTX_DM_EVENT_NTP_RESPONSE: {
            int res = 0;
            char *utc_payload = NULL;
            lite_cjson_t lite, lite_item_utc;

            if (payload == NULL) {
                return;
            }

            /* Parse Payload */
            memset(&lite, 0, sizeof(lite_cjson_t));
            res = lite_cjson_parse(payload, strlen(payload), &lite);
            if (res != SUCCESS_RETURN) {
                return;
            }

            /* Parse Utc */
            memset(&lite_item_utc, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, IOTX_LINKKIT_KEY_UTC, strlen(IOTX_LINKKIT_KEY_UTC), &lite_item_utc);
            if (res != SUCCESS_RETURN) {
                return;
            }
            sdk_debug("Current UTC: %.*s", lite_item_utc.value_length, lite_item_utc.value);

            utc_payload = HAL_Malloc(lite_item_utc.value_length + 1);
            if (utc_payload == NULL) {
                sdk_err("Not Enough Memory");
                return;
            }
            memset(utc_payload, 0, lite_item_utc.value_length + 1);
            memcpy(utc_payload, lite_item_utc.value, lite_item_utc.value_length);

            if (ctx->user_event_handler->ntp_response) {
                ctx->user_event_handler->ntp_response(utc_payload);
            }

            HAL_Free(utc_payload);
        }
        break;
        case IOTX_DM_EVENT_RRPC_REQUEST: {
            int res = 0, rrpc_response_len = 0;
            char *rrpc_request = NULL, *rrpc_response = NULL;
            lite_cjson_t lite, lite_item_id, lite_item_devid, lite_item_rrpcid, lite_item_serviceid, lite_item_payload;

            if (payload == NULL) {
                return;
            }

            /* Parse Payload */
            memset(&lite, 0, sizeof(lite_cjson_t));
            res = lite_cjson_parse(payload, strlen(payload), &lite);
            if (res != SUCCESS_RETURN) {
                return;
            }

            /* Parse ID */
            memset(&lite_item_id, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, IOTX_LINKKIT_KEY_ID, strlen(IOTX_LINKKIT_KEY_ID), &lite_item_id);
            if (res != SUCCESS_RETURN) {
                return;
            }
            sdk_debug("Current Id: %.*s", lite_item_id.value_length, lite_item_id.value);

            /* Parse Devid */
            memset(&lite_item_devid, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, IOTX_LINKKIT_KEY_DEVID, strlen(IOTX_LINKKIT_KEY_DEVID),
                                         &lite_item_devid);
            if (res != SUCCESS_RETURN) {
                return;
            }
            sdk_debug("Current Devid: %d", lite_item_devid.value_int);

            /* Parse Serviceid */
            memset(&lite_item_serviceid, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, IOTX_LINKKIT_KEY_SERVICEID, strlen(IOTX_LINKKIT_KEY_SERVICEID),
                                         &lite_item_serviceid);
            if (res != SUCCESS_RETURN) {
                return;
            }
            sdk_debug("Current ServiceID: %.*s", lite_item_serviceid.value_length, lite_item_serviceid.value);

            /* Parse RRPCID */
            memset(&lite_item_rrpcid, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, IOTX_LINKKIT_KEY_RRPCID, strlen(IOTX_LINKKIT_KEY_RRPCID),
                                         &lite_item_rrpcid);
            if (res != SUCCESS_RETURN) {
                return;
            }
            sdk_debug("Current RRPC ID: %.*s", lite_item_rrpcid.value_length, lite_item_rrpcid.value);

            /* Parse Payload */
            memset(&lite_item_payload, 0, sizeof(lite_cjson_t));
            res = lite_cjson_object_item(&lite, IOTX_LINKKIT_KEY_PAYLOAD, strlen(IOTX_LINKKIT_KEY_PAYLOAD),
                                         &lite_item_payload);
            if (res != SUCCESS_RETURN) {
                return;
            }
            sdk_debug("Current Payload: %.*s", lite_item_payload.value_length, lite_item_payload.value);

            rrpc_request = HAL_Malloc(lite_item_payload.value_length + 1);
            if (rrpc_request == NULL) {
                sdk_err("Not Enough Memory");
                return;
            }
            memset(rrpc_request, 0, lite_item_payload.value_length + 1);
            memcpy(rrpc_request, lite_item_payload.value, lite_item_payload.value_length);

            if (ctx->user_event_handler->sync_service_request) {
                res = ctx->user_event_handler->sync_service_request(lite_item_devid.value_int, lite_item_serviceid.value,
                        lite_item_serviceid.value_length,
                        rrpc_request, lite_item_payload.value_length, &rrpc_response, &rrpc_response_len);
                if (rrpc_response != NULL && rrpc_response_len > 0) {
                    iotx_dm_error_code_t code = (res == 0) ? (IOTX_DM_ERR_CODE_SUCCESS) : (IOTX_DM_ERR_CODE_REQUEST_ERROR);
                    iotx_dm_send_rrpc_response(lite_item_devid.value_int, lite_item_id.value, lite_item_id.value_length, code,
                                               lite_item_rrpcid.value,
                                               lite_item_rrpcid.value_length,
                                               rrpc_response, rrpc_response_len);
                    HAL_Free(rrpc_response);
                }
            }

            HAL_Free(rrpc_request);
        }
        break;
        default: {
        }
        break;
    }
}

#if (CONFIG_SDK_THREAD_COST == 1)
static void *_iotx_linkkit_dispatch(void *params)
{
    while (1) {
        iotx_dm_dispatch();
        HAL_SleepMs(20);
    }
    return NULL;
}
#endif

static int _iotx_linkkit_master_open(iotx_linkkit_dev_meta_info_t *meta_info)
{
    int res = 0;
    iotx_linkkit_ctx_t *ctx = _iotx_linkkit_get_ctx();

    if (ctx->is_opened) {
        return FAIL_RETURN;
    }
    ctx->is_opened = 1;

    HAL_SetProductKey(meta_info->product_key);
    HAL_SetProductSecret(meta_info->product_secret);
    HAL_SetDeviceName(meta_info->device_name);
    HAL_SetDeviceSecret(meta_info->device_secret);

    /* Create Mutex */
    ctx->mutex = HAL_MutexCreate();
    if (ctx->mutex == NULL) {
        sdk_err("Not Enough Memory");
        ctx->is_opened = 0;
        return FAIL_RETURN;
    }

    res = iotx_dm_open();
    if (res != SUCCESS_RETURN) {
        HAL_MutexDestroy(ctx->mutex);
        ctx->is_opened = 0;
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

static int _iotx_linkkit_master_start(void)
{
    int res = 0, domain_type = 0, dynamic_register = 0;
    iotx_linkkit_ctx_t *ctx = _iotx_linkkit_get_ctx();
    iotx_dm_init_params_t dm_init_params;

    if (ctx->is_started) {
        return FAIL_RETURN;
    }
    ctx->is_started = 1;

    memset(&dm_init_params, 0, sizeof(iotx_dm_init_params_t));
    IOT_Ioctl(IOTX_IOCTL_GET_DOMAIN, &domain_type);
    IOT_Ioctl(IOTX_IOCTL_GET_DYNAMIC_REGISTER, &dynamic_register);
    dm_init_params.domain_type = domain_type;
    dm_init_params.secret_type = dynamic_register;
    dm_init_params.event_callback = _iotx_linkkit_event_callback;

    res = iotx_dm_start(&dm_init_params);
    if (res != SUCCESS_RETURN) {
        sdk_err("DM Start Failed");
        ctx->is_started = 0;
        return FAIL_RETURN;
    }

#if (CONFIG_SDK_THREAD_COST == 1)
    int stack_used = 0;
    res = HAL_ThreadCreate(&ctx->dispatch_thread, _iotx_linkkit_dispatch, NULL, NULL, &stack_used);
    if (res != SUCCESS_RETURN) {
        iotx_dm_close();
        ctx->is_started = 0;
        return FAIL_RETURN;
    }
#endif
    return SUCCESS_RETURN;
}

static int _iotx_linkkit_master_close(void)
{
    iotx_linkkit_ctx_t *ctx = _iotx_linkkit_get_ctx();

    _iotx_linkkit_mutex_lock();
    if (ctx->is_opened == 0) {
        _iotx_linkkit_mutex_unlock();
        return FAIL_RETURN;
    }
    ctx->is_opened = 0;

#if (CONFIG_SDK_THREAD_COST == 1)
    HAL_ThreadDelete(ctx->dispatch_thread);
#endif
    iotx_dm_close();
    _iotx_linkkit_mutex_unlock();
    HAL_MutexDestroy(ctx->mutex);
    memset(ctx, 0, sizeof(iotx_linkkit_ctx_t));

    return SUCCESS_RETURN;
}

int IOT_Linkkit_Open(iotx_linkkit_dev_type_t dev_type, iotx_linkkit_dev_meta_info_t *meta_info)
{
    int res = 0;

    if (dev_type < 0 || dev_type >= IOTX_LINKKIT_DEV_TYPE_MAX || meta_info == NULL) {
        sdk_err("Invalid Parameter");
        return FAIL_RETURN;
    }

    switch (dev_type) {
        case IOTX_LINKKIT_DEV_TYPE_MASTER: {
            res = _iotx_linkkit_master_open(meta_info);
            if (res == SUCCESS_RETURN) {
                res = IOTX_DM_LOCAL_NODE_DEVID;
            }
        }
        case IOTX_LINKKIT_DEV_TYPE_SLAVE: {
            /* TODO */
        }
        break;
        default: {
            sdk_err("Unknown Device Type");
            res = FAIL_RETURN;
        }
        break;
    }

    return res;
}

int IOT_Linkkit_Ioctl(int devid, iotx_linkkit_ioctl_cmd_t cmd, void *arg)
{
    int res = 0;
    iotx_linkkit_ctx_t *ctx = _iotx_linkkit_get_ctx();

    if (devid < 0 || cmd < 0 || cmd >= IOTX_LINKKIT_CMD_MAX) {
        sdk_err("Invalid Parameter");
        return FAIL_RETURN;
    }

    if (ctx->is_opened == 0) {
        return FAIL_RETURN;
    }

    _iotx_linkkit_mutex_lock();
    if (devid == IOTX_DM_LOCAL_NODE_DEVID) {
        res = iotx_dm_set_opt(cmd, arg);
    } else {
        res = FAIL_RETURN;
    }

    _iotx_linkkit_mutex_unlock();

    return res;
}

int IOT_Linkkit_Connect(int devid, iotx_linkkit_event_handler_t *hdlrs)
{
    int res = 0;
    iotx_linkkit_ctx_t *ctx = _iotx_linkkit_get_ctx();

    if (devid < 0) {
        sdk_err("Invalid Parameter");
        return FAIL_RETURN;
    }

    if (ctx->is_opened == 0) {
        return FAIL_RETURN;
    }

    _iotx_linkkit_mutex_lock();

    if (devid == IOTX_DM_LOCAL_NODE_DEVID) {
        if (hdlrs != NULL) {
            ctx->user_event_handler = hdlrs;
            res = _iotx_linkkit_master_start();
        } else {
            res = FAIL_RETURN;
        }
    } else {
        res = FAIL_RETURN;
    }
    _iotx_linkkit_mutex_unlock();

    return res;
}

void IOT_Linkkit_Yield(int timeout_ms)
{
    iotx_linkkit_ctx_t *ctx = _iotx_linkkit_get_ctx();

    if (timeout_ms <= 0) {
        sdk_err("Invalid Parameter");
        return;
    }

    if (ctx->is_opened == 0 || ctx->is_started == 0) {
        return;
    }

    iotx_dm_yield(timeout_ms);
#if (CONFIG_SDK_THREAD_COST == 0)
    iotx_dm_dispatch();
#endif
}

int IOT_Linkkit_Close(int devid)
{
    int res = 0;

    if (devid < 0) {
        sdk_err("Invalid Parameter");
        return FAIL_RETURN;
    }

    if (devid == IOTX_DM_LOCAL_NODE_DEVID) {
        res = _iotx_linkkit_master_close();
    } else {
        return FAIL_RETURN;
    }

    return res;
}

int IOT_Linkkit_Post(int devid, iotx_linkkit_msg_type_t msg_type, unsigned char *payload, int payload_len)
{
    int res = 0;
    iotx_linkkit_ctx_t *ctx = _iotx_linkkit_get_ctx();

    if (devid < 0 || msg_type < 0 || msg_type >= IOTX_LINKKIT_MSG_MAX || payload == NULL || payload_len <= 0) {
        sdk_err("Invalid Parameter");
        return FAIL_RETURN;
    }

    if (ctx->is_opened == 0 || ctx->is_started == 0) {
        return FAIL_RETURN;
    }

    _iotx_linkkit_mutex_lock();
    switch (msg_type) {
        case IOTX_LINKKIT_MSG_POST_PROPERTY: {
            res = iotx_dm_post_property(devid, (char *)payload, payload_len);
        }
        break;
        case IOTX_LINKKIT_MSG_DEVICEINFO_UPDATE: {
            res = iotx_dm_deviceinfo_update(devid, (char *)payload, payload_len);
        }
        break;
        case IOTX_LINKKIT_MSG_DEVICEINFO_DELETE: {
            res = iotx_dm_deviceinfo_delete(devid, (char *)payload, payload_len);
        }
        break;
        case IOTX_LINKKIT_MSG_POST_RAW_DATA: {
            res = iotx_dm_post_rawdata(devid, (char *)payload, payload_len);
        }
        break;
        default: {
            sdk_err("Unknown Message Type");
            res = FAIL_RETURN;
        }
        break;
    }
    _iotx_linkkit_mutex_unlock();
    return res;
}

int IOT_Linkkit_TriggerEvent(int devid, char *eventid, int eventid_len, char *payload, int payload_len)
{
    int res = 0;
    iotx_linkkit_ctx_t *ctx = _iotx_linkkit_get_ctx();

    if (devid < 0 || eventid == NULL || eventid_len <= 0 || payload == NULL || payload_len <= 0) {
        sdk_err("Invalid Parameter");
        return FAIL_RETURN;
    }

    if (ctx->is_opened == 0 || ctx->is_started == 0) {
        return FAIL_RETURN;
    }

    _iotx_linkkit_mutex_lock();
    res = iotx_dm_post_event(devid, eventid, eventid_len, payload, payload_len);
    _iotx_linkkit_mutex_unlock();

    return res;
}

#endif
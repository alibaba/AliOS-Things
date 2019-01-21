/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */





#include "iot_import.h"

#include "shadow_debug.h"
#include "iotx_utils.h"
#include "iotx_system.h"

#include "shadow_update.h"

extern void iotx_shadow_delta_entry(
            iotx_shadow_pt pshadow,
            const char *json_doc,
            size_t json_doc_len);


/* add a new wait element */
/* return: NULL, failed; others, pointer of element. */
iotx_update_ack_wait_list_pt iotx_shadow_update_wait_ack_list_add(
            iotx_shadow_pt pshadow,
            const char *ptoken, /* NOTE: this is NOT a string. */
            size_t token_len,
            iotx_push_cb_fpt cb,
            void *pcontext,
            uint32_t timeout)
{
    int i;
    iotx_update_ack_wait_list_pt list = pshadow->inner_data.update_ack_wait_list;

    HAL_MutexLock(pshadow->mutex);

    for (i = 0; i < IOTX_DS_UPDATE_WAIT_ACK_LIST_NUM; ++i) {
        if (0 == list[i].flag_busy) {
            list[i].flag_busy = 1;
            break;
        }
    }

    HAL_MutexUnlock(pshadow->mutex);

    if (i >= IOTX_DS_UPDATE_WAIT_ACK_LIST_NUM) {
        return NULL;
    }

    list[i].callback = cb;
    list[i].pcontext = pcontext;

    if (token_len >= IOTX_DS_TOKEN_LEN) {
        shadow_warning("token is too long.");
        token_len = IOTX_DS_TOKEN_LEN - 1;
    }
    memcpy(list[i].token, ptoken, token_len);
    list[i].token[token_len] = '\0';

    iotx_time_init(&list[i].timer);
    utils_time_countdown_ms(&list[i].timer, timeout);

    shadow_debug("Add update ACK list");

    return &list[i];
}


void iotx_shadow_update_wait_ack_list_remove(iotx_shadow_pt pshadow, iotx_update_ack_wait_list_pt element)
{
    HAL_MutexLock(pshadow->mutex);
    element->flag_busy = 0;
    memset(element, 0, sizeof(iotx_update_ack_wait_list_t));
    HAL_MutexUnlock(pshadow->mutex);
}


void iotx_ds_update_wait_ack_list_handle_expire(iotx_shadow_pt pshadow)
{
    size_t i;

    iotx_update_ack_wait_list_pt pelement = pshadow->inner_data.update_ack_wait_list;

    HAL_MutexLock(pshadow->mutex);

    for (i = 0; i < IOTX_DS_UPDATE_WAIT_ACK_LIST_NUM; ++i) {
        if (0 != pelement[i].flag_busy) {
            if (utils_time_is_expired(&pelement[i].timer)) {
                if (NULL != pelement[i].callback) {
                    pelement[i].callback(pelement[i].pcontext, IOTX_SHADOW_ACK_TIMEOUT, NULL, 0);
                }
                /* free it. */
                memset(&pelement[i], 0, sizeof(iotx_update_ack_wait_list_t));
            }
        }
    }

    HAL_MutexUnlock(pshadow->mutex);
}


/* handle response ACK of UPDATE */
void iotx_ds_update_wait_ack_list_handle_response(
            iotx_shadow_pt pshadow,
            const char *json_doc,
            size_t json_doc_len)
{
    int i;
    const char *pdata, *ppayload, *pToken;
    iotx_update_ack_wait_list_pt pelement = pshadow->inner_data.update_ack_wait_list;

    /* get token */
    pdata = LITE_json_value_of("clientToken", (char *)json_doc);
    if (NULL == pdata) {
        shadow_warning("Invalid JSON document: not 'clientToken' key");
        return;
    }
    pToken = pdata;

    ppayload = LITE_json_value_of("payload", (char *)json_doc);
    if (NULL == ppayload) {
        shadow_warning("Invalid JSON document: not 'payload' key");
        LITE_free(pdata);
        return;
    } else {
        shadow_debug("ppayload = %s", ppayload);
    }

    HAL_MutexLock(pshadow->mutex);
    for (i = 0; i < IOTX_DS_UPDATE_WAIT_ACK_LIST_NUM; ++i) {
        if (0 != pelement[i].flag_busy) {
            /* check the related */
            if (0 == memcmp(pdata, pelement[i].token, strlen(pelement[i].token))) {
                LITE_free(pdata);
                HAL_MutexUnlock(pshadow->mutex);
                shadow_debug("token=%s", pelement[i].token);
                do {
                    pdata = LITE_json_value_of("status", (char *)ppayload);
                    if (NULL == pdata) {
                        shadow_warning("Invalid JSON document: not 'payload.status' key");
                        break;
                    }

                    if (0 == strncmp(pdata, "success", strlen(pdata))) {
                        char    *temp = NULL;

                        /* If have 'state' keyword in @json_shadow.payload, attribute value should be updated. */
                        temp = LITE_json_value_of("state", (char *)ppayload);
                        if (NULL != temp) {
                            iotx_shadow_delta_entry(pshadow, json_doc, json_doc_len); /* update attribute */
                            LITE_free(temp);
                        }

                        pelement[i].callback(pelement[i].pcontext, IOTX_SHADOW_ACK_SUCCESS, NULL, 0);
                    } else if (0 == strncmp(pdata, "error", strlen(pdata))) {
                        int ack_code;

                        LITE_free(pdata);
                        pdata = LITE_json_value_of("content.errorcode", (char *)ppayload);
                        if (NULL == pdata) {
                            shadow_warning("Invalid JSON document: not 'content.errorcode' key");
                            break;
                        }
                        ack_code = atoi(pdata);
                        LITE_free(pdata);

                        pdata = LITE_json_value_of("content.errormessage", (char *)ppayload);
                        if (NULL == pdata) {
                            shadow_warning("Invalid JSON document: not 'content.errormessage' key");
                            break;
                        }

                        pelement[i].callback(pelement[i].pcontext, ack_code, pdata, strlen(pdata));
                        LITE_free(pdata);
                    } else {
                        shadow_warning("Invalid JSON document: value of 'status' key is invalid.");
                        LITE_free(pdata);
                    }

                    LITE_free(pdata);
                    LITE_free(ppayload);
                } while (0);

                HAL_MutexLock(pshadow->mutex);
                memset(&pelement[i], 0, sizeof(iotx_update_ack_wait_list_t));
                HAL_MutexUnlock(pshadow->mutex);
                return;
            }
        }
    }

    LITE_free(pToken);
    LITE_free(ppayload);
    HAL_MutexUnlock(pshadow->mutex);
    shadow_warning("Not match any wait element in list.");
}

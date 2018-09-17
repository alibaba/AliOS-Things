/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */





#include "iot_import.h"
#include "shadow_debug.h"
#include "iotx_utils.h"
#include "utils_timer.h"
#include "utils_list.h"
#include "iotx_system.h"

#include "shadow.h"
#include "shadow_common.h"

/* check return code */
#define CHECK_RETURN_CODE(ret_code) \
    do{ \
        if (ret_code < 0) { \
            return ret_code; \
        } \
    }while(0);


/* check return code of HAL_Snprintf */
#define CHECK_SNPRINTF_RET(ret_code, buf_len) \
    do{ \
        if ((ret_code) < 0) { \
            return FAIL_RETURN; \
        } else if ((ret_code) >= (buf_len)) { \
            return ERROR_NO_ENOUGH_MEM; \
        } \
    }while(0);


/* return handle of format data. */
iotx_err_t iotx_ds_common_format_init(iotx_shadow_pt pshadow,
                                      format_data_pt pformat,
                                      char *buf,
                                      uint16_t size,
                                      const char *method,
                                      const char *head_str)
{
    int ret;
    uint32_t size_free_space;
    memset(pformat, 0, sizeof(format_data_t));

    pformat->buf = buf;
    pformat->buf_size = size;

    if (NULL == method) {
        return ERROR_SHADOW_NO_METHOD;
    }

    size_free_space = pformat->buf_size;

    ret = HAL_Snprintf(pformat->buf,
                       size_free_space,
                       "{\"%s\":\"%s\"",
                       "method",
                       method);

    CHECK_SNPRINTF_RET(ret, size_free_space);
    pformat->offset = ret;


    /* copy the JOSN head */
    size_free_space = pformat->buf_size - pformat->offset;
    if (NULL != head_str) {
        ret = HAL_Snprintf(pformat->buf + pformat->offset,
                           size_free_space,
                           ",%s",
                           head_str);
        CHECK_SNPRINTF_RET(ret, size_free_space);
        pformat->offset += ret;
    }

    pformat->flag_new = IOT_TRUE;

    return SUCCESS_RETURN;
}


iotx_err_t iotx_ds_common_format_add(iotx_shadow_pt pshadow,
                                     format_data_pt pformat,
                                     const char *name,
                                     const void *pvalue,
                                     iotx_shadow_attr_datatype_t datatype)
{
    int ret;
    uint32_t size_free_space;

    if (pformat->flag_new) {
        pformat->flag_new = IOT_FALSE;
    } else {
        /* Add comma char. */
        size_free_space = pformat->buf_size - pformat->offset;
        if (size_free_space > 1) { /* there is enough space to accommodate ',' char. */
            *(pformat->buf + pformat->offset) = ',';
            *(pformat->buf + pformat->offset + 1) = '\0';
            ++pformat->offset;
        } else {
            return FAIL_RETURN;
        }
    }

    size_free_space = pformat->buf_size - pformat->offset;

    /* add the string: "${pattr->pattr_name}":" */
    ret = HAL_Snprintf(pformat->buf + pformat->offset,
                       size_free_space,
                       "\"%s\":",
                       name);

    CHECK_SNPRINTF_RET(ret, size_free_space);

    pformat->offset += ret;
    size_free_space = pformat->buf_size - pformat->offset;

    /* convert attribute data to JSON string, and add to buffer */
    ret = iotx_ds_common_convert_data2string(pformat->buf + pformat->offset,
            size_free_space,
            datatype,
            pvalue);
    if (ret < 0) {
        return FAIL_RETURN;
    }

    pformat->offset += ret;

    return SUCCESS_RETURN;
}


iotx_err_t iotx_ds_common_format_finalize(iotx_shadow_pt pshadow, format_data_pt pformat, const char *tail_str)
{
#define UPDATE_JSON_STR_END         ",\"clientToken\":\"%s-%d\",\"version\":%d}"

    int ret;
    uint16_t size_free_space = pformat->buf_size - pformat->offset;

    if (NULL != tail_str) {
        ret = HAL_Snprintf(pformat->buf + pformat->offset, size_free_space, "%s", tail_str);
        CHECK_SNPRINTF_RET(ret, size_free_space);
        pformat->offset += ret;
    }

    size_free_space = pformat->buf_size - pformat->offset;

    ret = HAL_Snprintf(pformat->buf + pformat->offset,
                       size_free_space,
                       UPDATE_JSON_STR_END,
                       iotx_device_info_get()->device_id,
                       iotx_ds_common_get_tokennum(pshadow),
                       iotx_ds_common_get_version(pshadow));

    CHECK_SNPRINTF_RET(ret, size_free_space);
    pformat->offset += ret;

    return SUCCESS_RETURN;

#undef UPDATE_JSON_STR_END
}


int iotx_ds_common_convert_data2string(
            char *buf,
            size_t buf_len,
            iotx_shadow_attr_datatype_t type,
            const void *pData)
{

    int ret = -1;

    if ((NULL == buf) || (buf_len == 0)
        || ((IOTX_SHADOW_NULL != type) && (NULL == pData))) {
        return ERROR_NULL_VALUE;
    }

    if (IOTX_SHADOW_INT32 == type) {
        ret = HAL_Snprintf(buf, buf_len, "%" PRIi32, *(int32_t *)(pData));
    } else if (IOTX_SHADOW_STRING == type) {
        ret = HAL_Snprintf(buf, buf_len, "\"%s\"", (char *)(pData));
    } else if (IOTX_SHADOW_NULL == type) {
        ret = HAL_Snprintf(buf, buf_len, "%s", "\"null\"");
    } else {
        shadow_err("Error data type");
        ret = -1;
    }

    if ((ret < 0) || (ret >= buf_len)) {
        return -1;
    }

    return ret;
}


iotx_err_t iotx_ds_common_convert_string2data(
            const char *buf,
            size_t buf_len,
            iotx_shadow_attr_datatype_t type,
            void *pdata)
{
    if ((NULL == buf) || (buf_len == 0) || (NULL == pdata)) {
        return ERROR_NULL_VALUE;
    }

    if (type == IOTX_SHADOW_INT32) {
        if (0 == strcmp(pdata, "true")) {
            *((int32_t *)pdata) = 1;
        } else if (0 == strcmp(pdata, "false")) {
            *((int32_t *)pdata) = 0;
        } else if (0 == strcmp(pdata, "null")) {
            *((int32_t *)pdata) = 0;
        } else {
            *((int32_t *)pdata) = atoi(buf);
        }
    } else if (type == IOTX_SHADOW_STRING) {
        memcpy(pdata, buf, buf_len);
    } else {
        shadow_err("Error data type");
        return ERROR_SHADOW_UNDEF_TYPE;
    }

    return SUCCESS_RETURN;
}


void iotx_ds_common_update_time(iotx_shadow_pt pshadow, uint32_t new_timestamp)
{
    HAL_MutexLock(pshadow->mutex);
    pshadow->inner_data.time.base_system_time = utils_time_get_ms();
    pshadow->inner_data.time.epoch_time = new_timestamp;
    HAL_MutexUnlock(pshadow->mutex);

    shadow_info("update system time");
}


int iotx_ds_common_check_attr_existence(
            iotx_shadow_pt pshadow,
            iotx_shadow_attr_pt pattr)
{
    list_node_t *node;

    HAL_MutexLock(pshadow->mutex);
    node = list_find(pshadow->inner_data.attr_list, pattr);
    HAL_MutexUnlock(pshadow->mutex);

    return (NULL != node);
}


/* register attribute to list */
iotx_err_t iotx_ds_common_register_attr(
            iotx_shadow_pt pshadow,
            iotx_shadow_attr_pt pattr)
{
    list_node_t *node = list_node_new(pattr);
    if (NULL == node) {
        return ERROR_NO_MEM;
    }

    HAL_MutexLock(pshadow->mutex);
    list_lpush(pshadow->inner_data.attr_list, node);
    HAL_MutexUnlock(pshadow->mutex);

    return SUCCESS_RETURN;
}


/* remove attribute to list */
iotx_err_t iotx_ds_common_remove_attr(
            iotx_shadow_pt pshadow,
            iotx_shadow_attr_pt pattr)
{
    iotx_err_t rc = SUCCESS_RETURN;
    list_node_t *node;

    HAL_MutexLock(pshadow->mutex);
    node = list_find(pshadow->inner_data.attr_list, pattr);
    if (NULL == node) {
        rc = ERROR_SHADOW_NO_ATTRIBUTE;
        shadow_err("Try to remove a non-existent attribute.");
    } else {
        list_remove(pshadow->inner_data.attr_list, node);
    }
    HAL_MutexUnlock(pshadow->mutex);

    return rc;
}


void iotx_ds_common_update_version(iotx_shadow_pt pshadow, uint32_t version)
{
    HAL_MutexLock(pshadow->mutex);

    /* version number always grow up */
    if (version > pshadow->inner_data.version) {
        pshadow->inner_data.version = version;
    }
    HAL_MutexUnlock(pshadow->mutex);

    shadow_info("update shadow version");
}



uint32_t iotx_ds_common_get_version(iotx_shadow_pt pshadow)
{
    uint32_t ver;
    HAL_MutexLock(pshadow->mutex);
    ++pshadow->inner_data.version;
    ver = pshadow->inner_data.version;
    ++pshadow->inner_data.version;
    HAL_MutexUnlock(pshadow->mutex);
    return ver;
}


uint32_t iotx_ds_common_get_tokennum(iotx_shadow_pt pshadow)
{
    uint32_t ver;
    HAL_MutexLock(pshadow->mutex);
    ++pshadow->inner_data.token_num;
    ver = pshadow->inner_data.token_num;
    ++pshadow->inner_data.token_num;
    HAL_MutexUnlock(pshadow->mutex);
    return ver;
}


char *iotx_ds_common_generate_topic_name(iotx_shadow_pt pshadow, const char *topic)
{
#define SHADOW_TOPIC_FMT      "/shadow/%s/%s/%s"
#define SHADOW_TOPIC_LEN      (PRODUCT_KEY_LEN + DEVICE_NAME_LEN)

    int len, ret;
    char *topic_full = NULL;
    iotx_device_info_pt pdevice_info = iotx_device_info_get();

    len = SHADOW_TOPIC_LEN + sizeof(SHADOW_TOPIC_FMT);

    topic_full = LITE_malloc(len + 1);
    if (NULL == topic_full) {
        shadow_err("Not enough memory");
        return NULL;
    }

    ret = HAL_Snprintf(topic_full,
                       len,
                       SHADOW_TOPIC_FMT,
                       topic,
                       pdevice_info->product_key,
                       pdevice_info->device_name);
    if (ret < 0) {
        LITE_free(topic_full);
        return NULL;
    }

    LITE_ASSERT(ret < len);

    return topic_full;
}


int iotx_ds_common_publish2update(iotx_shadow_pt pshadow, char *data, uint32_t data_len)
{
    iotx_mqtt_topic_info_t topic_msg;

    /* check if topic name have been generated or not */
    if (NULL == pshadow->inner_data.ptopic_update) {
        /* Have NOT update topic name, generate it. */
        pshadow->inner_data.ptopic_update = iotx_ds_common_generate_topic_name(pshadow, "update");
        if (NULL == pshadow->inner_data.ptopic_update) {
            return FAIL_RETURN;
        }
    }

    shadow_debug("publish msg: len=%d, str=%s", data_len, data);

    topic_msg.qos = IOTX_MQTT_QOS1;
    topic_msg.retain = 0;
    topic_msg.dup = 0;
    topic_msg.payload = (void *)data;
    topic_msg.payload_len = data_len;
    topic_msg.packet_id = 0;

    return IOT_MQTT_Publish(pshadow->mqtt, pshadow->inner_data.ptopic_update, &topic_msg);
}

/**
 * @file aiot_task_api.c
 * @brief TASK模块接口实现文件, 其中包含了TASK的所有用户API
 * @date 2019-12-27
 *
 * @copyright Copyright (C) 2015-2018 Alibaba Group Holding Limited
 *
 */

#include "core_mqtt.h"
#include "core_string.h"
#include "core_log.h"
#include "core_global.h"
#include "task_private.h"
#include "aiot_task_api.h"

static void _task_recv_notify_handler(void *handle, const aiot_mqtt_recv_t *msg, void *userdata);
static void _task_recv_get_reply_handler(void *handle, const aiot_mqtt_recv_t *msg, void *userdata);
static void _task_recv_update_reply_handler(void *handle, const aiot_mqtt_recv_t *msg, void *userdata);

static const task_status_map_t g_task_stauts_mapping[] = {
    {"QUEUED", AIOT_TASK_STATUS_QUEUED},
    {"SENT", AIOT_TASK_STATUS_SENT},
    {"IN_PROGRESS", AIOT_TASK_STATUS_IN_PROGRESS},
    {"SUCCEEDED", AIOT_TASK_STATUS_SUCCEEDED},
    {"FAILED", AIOT_TASK_STATUS_FAILED},
    {"REJECTED", AIOT_TASK_STATUS_REJECTED},
    {"CANCELLED", AIOT_TASK_STATUS_CANCELLED},
    {"REMOVED", AIOT_TASK_STATUS_REMOVED},
    {"TIMED_OUT", AIOT_TASK_STATUS_TIMED_OUT}
};

static const task_recv_topic_map_t g_task_recv_topic_mapping[] = {
    {
        "/sys/+/+/thing/job/notify",
        _task_recv_notify_handler,
    },
    {
        "/sys/+/+/thing/job/get_reply",
        _task_recv_get_reply_handler,
    },
    {
        "/sys/+/+/thing/job/update_reply",
        _task_recv_update_reply_handler,
    }
};

static char *_task_convert_status_to_str(aiot_task_status_t status)
{
    uint32_t i = 0;

    for (i = 0; i < sizeof(g_task_stauts_mapping) / sizeof(task_status_map_t); i++) {
        if (g_task_stauts_mapping[i].status == status) {
            return g_task_stauts_mapping[i].str;
        }
    }

    return NULL;
}

static aiot_task_status_t _task_cover_status_to_enum(char *str)
{
    uint32_t i = 0;

    if (NULL == str) {
        return AIOT_TASK_STATUS_FAILED;
    }

    for (i = 0; i < sizeof(g_task_stauts_mapping) / sizeof(task_status_map_t); i++) {
        if (strcmp(g_task_stauts_mapping[i].str, str) == 0) {
            return g_task_stauts_mapping[i].status;
        }
    }

    return AIOT_TASK_STATUS_FAILED;
}

static int32_t _task_parse_json(aiot_sysdep_portfile_t *sysdep, const void *input, uint32_t input_len, char *key_word,
                                char **out)
{
    int32_t res = STATE_SUCCESS;
    char *value = NULL, *buffer = NULL;
    uint32_t value_len = 0, buffer_len = 0;

    res = core_json_value((const char *)input, input_len, key_word, strlen(key_word), &value, &value_len);
    if (res != STATE_SUCCESS) {
        return STATE_TASK_PARSE_JSON_ERROR;
    }
    buffer_len = value_len + 1;
    buffer = sysdep->core_sysdep_malloc(buffer_len, TASK_MODULE_NAME);
    if (NULL == buffer) {
        return STATE_TASK_PARSE_JSON_MALLOC_FAILED;
    }
    memset(buffer, 0, buffer_len);
    memcpy(buffer, value, value_len);
    *out = buffer;
    return res;
}

static int32_t _task_parse_task_detail(task_handle_t *task_handle, const char *payload, uint32_t payload_len,
                                       task_desc_t *task)
{
    int32_t res = STATE_SUCCESS;
    if (STATE_SUCCESS != _task_parse_json(task_handle->sysdep, payload, payload_len, "taskId", &(task->task_id))) {
        return STATE_TASK_PARSE_JSON_MALLOC_FAILED;
    }

    char *status = NULL;
    if (STATE_SUCCESS != _task_parse_json(task_handle->sysdep, payload, payload_len, "status", &status)) {
        return STATE_TASK_PARSE_JSON_MALLOC_FAILED;
    }
    task->status = _task_cover_status_to_enum(status);
    task_handle->sysdep->core_sysdep_free(status);

    _task_parse_json(task_handle->sysdep, payload, payload_len, "jobDocument", &(task->job_document));

    char *job_file_key = "jobFile";
    char *job_file_value = NULL;
    uint32_t job_file_len = 0;
    if ((res = core_json_value(payload, payload_len, job_file_key, (uint32_t)strlen(job_file_key),
                               &job_file_value, &job_file_len)) != STATE_SUCCESS) {
        return STATE_SUCCESS;
    }

    if (STATE_SUCCESS != _task_parse_json(task_handle->sysdep, job_file_value, job_file_len, "signMethod",
                                          &(task->sign_method))) {
        return res;
    }

    if (STATE_SUCCESS != _task_parse_json(task_handle->sysdep, job_file_value, job_file_len, "sign", &(task->sign))) {
        return res;
    }

    if (STATE_SUCCESS != _task_parse_json(task_handle->sysdep, job_file_value, job_file_len, "fileUrl",
                                          &(task->document_file_url))) {
        return res;
    }

    return res;
}

static void _task_free_task_list(task_handle_t *task_handle, task_get_list_reply_t *list)
{
    int32_t i = 0;
    if (NULL == list->tasks) {
        return;
    }

    for (i = 0; i < list->number; i++) {
        if (list->tasks[i].task_id) {
            task_handle->sysdep->core_sysdep_free(list->tasks[i].task_id);
        }
    }
    task_handle->sysdep->core_sysdep_free(list->tasks);
}

static void _task_free_task_detail(task_handle_t *task_handle, task_desc_t *task)
{
    aiot_sysdep_portfile_t *sysdep = aiot_sysdep_get_portfile();
    if (NULL == task) {
        return;
    }

    if (task->task_id) {
        sysdep->core_sysdep_free(task->task_id);
    }

    if (task->job_document) {
        sysdep->core_sysdep_free(task->job_document);
    }

    if (task->sign_method) {
        sysdep->core_sysdep_free(task->sign_method);
    }

    if (task->sign) {
        sysdep->core_sysdep_free(task->sign);
    }

    if (task->document_file_url) {
        sysdep->core_sysdep_free(task->document_file_url);
    }
}

static void _task_free_update_reply(task_update_reply_t *reply)
{
    aiot_sysdep_portfile_t *sysdep = aiot_sysdep_get_portfile();
    if (NULL == reply) {
        return;
    }

    if (reply->task_id) {
        sysdep->core_sysdep_free(reply->task_id);
    }
}

static int32_t _task_send_notify_reply(task_handle_t *task_handle, char *msg_id, uint32_t code)
{
    char *topic = NULL, *payload = NULL;
    char code_string[11] = { 0 };
    char *src[2] = { NULL };
    int32_t res = STATE_SUCCESS;

    core_uint2str(code, code_string, NULL);

    src[0] = msg_id;
    src[1] = code_string;

    res = core_sprintf(task_handle->sysdep, &payload, TASK_NOTIFY_REPLY_FMT, src, sizeof(src) / sizeof(char *),
                       TASK_MODULE_NAME);
    if (res < 0) {
        return res;
    }

    char *topic_src[] = { core_mqtt_get_product_key(task_handle->mqtt_handle), core_mqtt_get_device_name(task_handle->mqtt_handle) };
    res = core_sprintf(task_handle->sysdep, &topic, TASK_NOTIFY_REPLY_TOPIC_FMT, topic_src,
                       sizeof(topic_src) / sizeof(char *),
                       TASK_MODULE_NAME);
    if (res < STATE_SUCCESS) {
        task_handle->sysdep->core_sysdep_free(payload);
        return res;
    }

    res = aiot_mqtt_pub(task_handle->mqtt_handle, (char *)topic, (uint8_t *)payload, strlen(payload), 0);
    task_handle->sysdep->core_sysdep_free(topic);
    task_handle->sysdep->core_sysdep_free(payload);

    return res;
}

static int32_t _task_parse_notify(task_handle_t *task_handle, const char *payload, uint32_t payload_len,
                                  task_desc_t *task)
{
    int32_t res = STATE_SUCCESS;

    char *params_key = "params";
    char *params_value = NULL;
    uint32_t params_len = 0;
    if ((res = core_json_value(payload, payload_len, params_key, (uint32_t)strlen(params_key),
                               &params_value, &params_len)) != STATE_SUCCESS) {
        return res;
    }

    char *task_key = "task";
    char *task_value = NULL;
    uint32_t task_len = 0;
    if ((res = core_json_value(params_value, params_len, task_key, (uint32_t)strlen(task_key),
                               &task_value, &task_len)) != STATE_SUCCESS) {
        return res;
    }

    res = _task_parse_task_detail(task_handle, task_value, task_len, task);

    return res;
}

static void _task_recv_notify_handler(void *handle, const aiot_mqtt_recv_t *msg, void *userdata)
{
    task_handle_t *task_handle = (task_handle_t *)userdata;
    aiot_task_recv_t recv;
    int32_t res = STATE_SUCCESS;
    char *id = NULL;

    if (NULL == task_handle->recv_handler) {
        return;
    }

    memset(&recv, 0, sizeof(aiot_task_recv_t));
    recv.type = AIOT_TASKRECV_NOTIFY;

    core_log(task_handle->sysdep, STATE_TASK_RECV_NOTIFY, "task recv notify\r\n");

    if (STATE_SUCCESS != _task_parse_json(task_handle->sysdep, (char *)msg->data.pub.payload, msg->data.pub.payload_len,
                                          "id", &id)) {
        return;
    }

    _task_send_notify_reply(task_handle, id, 200);
    if (NULL != id) {
        task_handle->sysdep->core_sysdep_free(id);
    }

    if ((res = _task_parse_notify(task_handle, (char *)msg->data.pub.payload, msg->data.pub.payload_len,
                                  &(recv.data.notify))) == STATE_SUCCESS) {
        task_handle->recv_handler(task_handle, &recv, task_handle->userdata);
    } else {
        core_log(task_handle->sysdep, STATE_TASK_PARSE_NOTIFY_FAILED, "task parse notify failed\r\n");
    }
    _task_free_task_detail(task_handle, &(recv.data.notify));
}

static int32_t _task_parse_get_list_reply_array(char *str, int32_t str_len, task_list_json *array)
{
    int32_t num = 0, len = 0;
    int32_t i = 0, new = 0;

    if ((NULL == str) || (str_len <= 2)) {
        return 0;
    }

    str++;
    if (*str == ']') { /* empty array */
        return 0;
    }

    while ((i < str_len) && (num < TASK_GET_LIST_REPLY_ARRAY_MAX)) {
        if (*str == '{') {
            len = 1;
            new = 1;
            array[num].pos = str;
        } else if (*str == '}') {
            len++;
            if (new == 1) {
                array[num].len = len;
                num++;
                new = 0;
            }
        } else {
            len++;
        }
        str++;
        i++;
    }
    return num;
}

static uint32_t _task_parse_get_list_reply(task_handle_t *task_handle, void *input, uint32_t input_len,
        task_get_list_reply_t *reply)
{
    int32_t res = STATE_SUCCESS;
    task_list_json json_array[TASK_GET_LIST_REPLY_ARRAY_MAX];
    int32_t num = _task_parse_get_list_reply_array(input, input_len, json_array);
    reply->number = num;
    if (0 == num) {
        return res;
    }

    task_summary_t *array = task_handle->sysdep->core_sysdep_malloc(sizeof(task_summary_t) * num, TASK_MODULE_NAME);

    if (NULL == array) {
        return STATE_TASK_PARSE_JSON_MALLOC_FAILED;
    }

    memset(array, 0, sizeof(task_summary_t)*num);

    reply->tasks = array;

    for (int32_t i = 0; i < num; i++) {
        if (STATE_SUCCESS != _task_parse_json(task_handle->sysdep, json_array[i].pos, json_array[i].len, "taskId",
                                              &(array[i].task_id))) {
            return res;
        }

        char *status = NULL;
        if (STATE_SUCCESS != _task_parse_json(task_handle->sysdep, json_array[i].pos, json_array[i].len, "status", &status)) {
            return res;
        }

        array[i].status = _task_cover_status_to_enum(status);
        task_handle->sysdep->core_sysdep_free(status);
    }

    return res;
}

static void _task_recv_get_reply_handler(void *handle, const aiot_mqtt_recv_t *msg, void *userdata)
{
    task_handle_t *task_handle = (task_handle_t *)userdata;
    aiot_task_recv_t recv;
    int32_t res = STATE_SUCCESS;

    if (NULL == task_handle->recv_handler) {
        return;
    }

    memset(&recv, 0, sizeof(aiot_task_recv_t));

    char *code_key = "code";
    char *code_value = NULL;
    uint32_t code_len = 0;
    if (core_json_value((char *)msg->data.pub.payload, msg->data.pub.payload_len, code_key, (uint32_t)strlen(code_key),
                        &code_value, &code_len) != STATE_SUCCESS) {
        return;
    }

    if (core_str2uint(code_value, (uint8_t)code_len, &(recv.data.get_detail_reply.code)) != STATE_SUCCESS) {
        return;
    }

    char *data_key = "data";
    char *data_value = NULL;
    uint32_t data_len = 0;
    if (core_json_value((char *)msg->data.pub.payload, msg->data.pub.payload_len, data_key, (uint32_t)strlen(data_key),
                        &data_value, &data_len) != STATE_SUCCESS) {
        return;
    }

    char *tasks_key = "tasks";
    char *tasks_value = NULL;
    uint32_t tasks_len = 0;
    if (core_json_value(data_value, data_len, tasks_key, (uint32_t)strlen(tasks_key),
                        &tasks_value, &tasks_len) == STATE_SUCCESS) {
        recv.type = AIOT_TASKRECV_GET_LIST_REPLY;
        task_get_list_reply_t *reply = &(recv.data.get_list_reply);
        if ((res = _task_parse_get_list_reply(task_handle, tasks_value, tasks_len, reply)) == STATE_SUCCESS) {
            task_handle->recv_handler(task_handle, &recv, task_handle->userdata);
        }
        _task_free_task_list(task_handle, &(recv.data.get_list_reply));
        return;
    }

    char *task_key = "task";
    char *task_value = NULL;
    uint32_t task_len = 0;
    if (core_json_value(data_value, data_len, task_key, (uint32_t)strlen(task_key),
                        &task_value, &task_len) == STATE_SUCCESS) {
        recv.type = AIOT_TASKRECV_GET_DETAIL_REPLY;
        task_desc_t *task = &(recv.data.get_detail_reply.task);
        if ((res = _task_parse_task_detail(task_handle, task_value, task_len, task)) == STATE_SUCCESS) {
            task_handle->recv_handler(task_handle, &recv, task_handle->userdata);
        }
        _task_free_task_detail(task_handle, task);
        return;
    }

    char *task_id_key = "taskId";
    char *task_id_value = NULL;
    uint32_t task_id_len = 0;
    if (core_json_value(data_value, data_len, task_id_key, (uint32_t)strlen(task_id_key),
                        &task_id_value, &task_id_len) == STATE_SUCCESS) {
        if (memcmp(task_id_value, TASK_GET_LIST_REPLY_TASK_ID, strlen(TASK_GET_LIST_REPLY_TASK_ID)) == 0) {
            recv.type = AIOT_TASKRECV_GET_LIST_REPLY;
            task_get_list_reply_t *reply = &(recv.data.get_list_reply);
            reply->number = 0;
            task_handle->recv_handler(task_handle, &recv, task_handle->userdata);
        } else {
            recv.type = AIOT_TASKRECV_GET_DETAIL_REPLY;
            task_desc_t *task = &(recv.data.get_detail_reply.task);

            char *id = task_handle->sysdep->core_sysdep_malloc(task_id_len + 1, TASK_MODULE_NAME);
            memset(id, 0, task_id_len + 1);
            memcpy(id, task_id_value, task_id_len);
            task->task_id = id;
            task->status = AIOT_TASK_STATUS_NOT_FOUND;
            task_handle->recv_handler(task_handle, &recv, task_handle->userdata);
            _task_free_task_detail(task_handle, task);
        }
    }
}

static void _task_recv_update_reply_handler(void *handle, const aiot_mqtt_recv_t *msg, void *userdata)
{
    task_handle_t *task_handle = (task_handle_t *)userdata;
    aiot_task_recv_t recv;

    if (NULL == task_handle->recv_handler) {
        return;
    }

    memset(&recv, 0, sizeof(aiot_task_recv_t));
    recv.type = AIOT_TASKRECV_UPDATE_REPLY;

    char *code_key = "code";
    char *code_value = NULL;
    uint32_t code_len = 0;
    if (core_json_value((char *)msg->data.pub.payload, msg->data.pub.payload_len, code_key, (uint32_t)strlen(code_key),
                        &code_value, &code_len) != STATE_SUCCESS) {
        return;
    }

    if (core_str2uint(code_value, (uint8_t)code_len, &(recv.data.get_detail_reply.code)) != STATE_SUCCESS) {
        return;
    }

    char *data_key = "data";
    char *data_value = NULL;
    uint32_t data_len = 0;
    if (core_json_value((char *)msg->data.pub.payload, msg->data.pub.payload_len, data_key, (uint32_t)strlen(data_key),
                        &data_value, &data_len) != STATE_SUCCESS) {
        return;
    }

    _task_parse_json(task_handle->sysdep, data_value, data_len, "taskId", &(recv.data.update_reply.task_id));

    task_handle->recv_handler(task_handle, &recv, task_handle->userdata);

    _task_free_update_reply(&(recv.data.update_reply));
}

static int32_t _task_setup_topic_mapping(void *mqtt_handle, void *task_handle)
{
    uint32_t i = 0;
    int32_t res = STATE_SUCCESS;
    aiot_mqtt_topic_map_t topic_mapping;

    for (i = 0; i < sizeof(g_task_recv_topic_mapping) / sizeof(task_recv_topic_map_t); i++) {
        topic_mapping.topic = g_task_recv_topic_mapping[i].topic;
        topic_mapping.handler = g_task_recv_topic_mapping[i].func;
        topic_mapping.userdata = task_handle;

        res = aiot_mqtt_setopt(mqtt_handle, AIOT_MQTTOPT_APPEND_TOPIC_MAP, &topic_mapping);
        if (STATE_SUCCESS != res) {
            break;
        }
    }
    return res;
}

static void _task_core_mqtt_process_handler(void *context, aiot_mqtt_event_t *event, core_mqtt_event_t *core_event)
{
    task_handle_t *task_handle = (task_handle_t *)context;

    if (core_event != NULL) {
        switch (core_event->type) {
            case CORE_MQTTEVT_DEINIT: {
                task_handle->mqtt_handle = NULL;
                return;
            }
            break;
            default: {

            }
            break;
        }
    }
}

static int32_t _task_core_mqtt_operate_process_handler(task_handle_t *task_handle, core_mqtt_option_t option)
{
    core_mqtt_process_data_t process_data;

    memset(&process_data, 0, sizeof(core_mqtt_process_data_t));
    process_data.handler = _task_core_mqtt_process_handler;
    process_data.context = task_handle;

    return core_mqtt_setopt(task_handle->mqtt_handle, option, &process_data);
}

static int32_t _task_send_query_task(task_handle_t *task_handle, char *task_id)
{
    char *topic = NULL, *payload = NULL;
    int32_t id = 0;
    char id_string[11] = { 0 };
    char *src[2] = { NULL };
    int32_t res = STATE_SUCCESS;

    if (NULL == task_id) {
        return STATE_TASK_QUERY_TASK_ID_IS_NULL;
    }

    core_global_alink_id_next(task_handle->sysdep, &id);
    core_int2str(id, id_string, NULL);

    src[0] = id_string;
    src[1] = task_id;

    res = core_sprintf(task_handle->sysdep, &payload, TASK_REQUEST_QUERY_TASK_FMT, src, sizeof(src) / sizeof(char *),
                       TASK_MODULE_NAME);
    if (res < 0) {
        return res;
    }

    char *topic_src[] = { core_mqtt_get_product_key(task_handle->mqtt_handle), core_mqtt_get_device_name(task_handle->mqtt_handle) };
    res = core_sprintf(task_handle->sysdep, &topic, TASK_QUERY_TASK_TOPIC_FMT, topic_src,
                       sizeof(topic_src) / sizeof(char *),
                       TASK_MODULE_NAME);
    if (res < STATE_SUCCESS) {
        task_handle->sysdep->core_sysdep_free(payload);
        return res;
    }

    res = aiot_mqtt_pub(task_handle->mqtt_handle, (char *)topic, (uint8_t *)payload, strlen(payload), 0);
    task_handle->sysdep->core_sysdep_free(payload);
    task_handle->sysdep->core_sysdep_free(topic);

    return res;
}

int32_t aiot_task_get_task_list(void *handle)
{
    task_handle_t *task_handle = (task_handle_t *)handle;

    if (NULL == handle) {
        return STATE_USER_INPUT_NULL_POINTER;
    }
    return _task_send_query_task(task_handle, "$list");
}

int32_t aiot_task_get_task_detail(void *handle, char *task_id)
{
    int32_t res = STATE_SUCCESS;
    task_handle_t *task_handle = (task_handle_t *)handle;

    if (NULL == handle) {
        return STATE_USER_INPUT_NULL_POINTER;
    }

    if (NULL == task_id) {
        res = _task_send_query_task(task_handle, "$next");
    } else {
        res = _task_send_query_task(task_handle, task_id);
    }

    return res;
}

int32_t aiot_task_update(void *handle, task_desc_t *task)
{
    int32_t res = STATE_SUCCESS;
    char *topic = NULL, *payload = NULL, *status = NULL;
    int32_t id = 0;
    char id_string[11] = { 0 };
    char progress_string[11] = { 0 };
    char *src[5] = { NULL };
    task_handle_t *task_handle = (task_handle_t *)handle;

    if ((NULL == handle) || (NULL == task)) {
        return STATE_USER_INPUT_NULL_POINTER;
    }

    if ((NULL != task->status_details) && (strcmp(task->status_details, "") == 0)) {
        return STATE_TASK_UPDATE_STATUS_DETAILS_INVALID;
    }

    status = _task_convert_status_to_str(task->status);

    if (NULL == status) {
        return STATE_TASK_UPDATE_STATUS_INVALID;
    }

    core_global_alink_id_next(task_handle->sysdep, &id);
    core_int2str(id, id_string, NULL);

    char *topic_src[] = { core_mqtt_get_product_key(task_handle->mqtt_handle), core_mqtt_get_device_name(task_handle->mqtt_handle) };
    res = core_sprintf(task_handle->sysdep, &topic, TASK_UPDATE_TASK_TOPIC_FMT, topic_src,
                       sizeof(topic_src) / sizeof(char *),
                       TASK_MODULE_NAME);
    if (res < STATE_SUCCESS) {
        return res;
    }

    core_uint2str(task->progress, progress_string, NULL);
    src[0] = id_string;
    src[1] = task->task_id;
    src[2] = _task_convert_status_to_str(task->status);
    src[3] = progress_string;

    if (NULL == task->status_details) {
        res = core_sprintf(task_handle->sysdep, &payload, TASK_REQUEST_UPDATE_TASK_NO_DETAIL_FMT, src, 4,
                           TASK_MODULE_NAME);
    } else {
        src[4] = task->status_details;
        res = core_sprintf(task_handle->sysdep, &payload, TASK_REQUEST_UPDATE_TASK_FMT, src, sizeof(src) / sizeof(char *),
                           TASK_MODULE_NAME);
    }

    if (res < STATE_SUCCESS) {
        task_handle->sysdep->core_sysdep_free(topic);
        return res;
    }

    res = aiot_mqtt_pub(task_handle->mqtt_handle, (char *)topic, (uint8_t *)payload, strlen(payload), 0);
    task_handle->sysdep->core_sysdep_free(topic);
    task_handle->sysdep->core_sysdep_free(payload);

    return res;
}

int32_t aiot_task_setopt(void *handle, aiot_task_option_t option, void *data)
{
    int32_t res = STATE_SUCCESS;
    task_handle_t *task_handle = (task_handle_t *)handle;
    aiot_sysdep_portfile_t *sysdep = NULL;

    if (NULL == task_handle) {
        return STATE_TASK_SETOPT_HANDLE_IS_NULL;
    }
    if (NULL == data) {
        return STATE_TASK_SETOPT_DATA_IS_NULL;
    }

    sysdep = task_handle->sysdep;
    sysdep->core_sysdep_mutex_lock(task_handle->data_mutex);
    switch (option) {
        case AIOT_TASKOPT_RECV_HANDLER: {
            task_handle->recv_handler = (aiot_task_recv_handler_t)data;
        }
        break;
        case AIOT_TASKOPT_USERDATA: {
            task_handle->userdata = data;
        }
        break;
        case AIOT_TASKOPT_MQTT_HANDLE: {
            task_handle->mqtt_handle = data;
            res = _task_setup_topic_mapping(data, task_handle);
            if (res >= STATE_SUCCESS) {
                res = _task_core_mqtt_operate_process_handler(task_handle, CORE_MQTTOPT_APPEND_PROCESS_HANDLER);
            }
        }
        break;
        default: {
            res = STATE_USER_INPUT_UNKNOWN_OPTION;
        }
        break;
    }
    sysdep->core_sysdep_mutex_unlock(task_handle->data_mutex);

    return res;
}

void *aiot_task_init(void)
{
    task_handle_t *task_handle = NULL;
    aiot_sysdep_portfile_t *sysdep = NULL;

    sysdep = aiot_sysdep_get_portfile();
    if (sysdep == NULL) {
        return NULL;
    }

    task_handle = sysdep->core_sysdep_malloc(sizeof(task_handle_t), TASK_MODULE_NAME);
    if (task_handle == NULL) {
        return NULL;
    }

    memset(task_handle, 0, sizeof(task_handle_t));
    task_handle->sysdep = sysdep;

    core_global_init(sysdep);
    return task_handle;
}

int32_t aiot_task_deinit(void **p_handle)
{
    task_handle_t *task_handle = NULL;
    aiot_sysdep_portfile_t *sysdep = NULL;
    uint8_t i = 0;

    if (NULL == p_handle || NULL == *p_handle) {
        return STATE_USER_INPUT_NULL_POINTER;
    }

    task_handle = *p_handle;
    sysdep = task_handle->sysdep;
    *p_handle = NULL;

    _task_core_mqtt_operate_process_handler(task_handle, CORE_MQTTOPT_REMOVE_PROCESS_HANDLER);

    /* remove mqtt topic mapping */
    for (i = 0; i < sizeof(g_task_recv_topic_mapping) / sizeof(task_recv_topic_map_t); i++) {
        aiot_mqtt_topic_map_t topic_mapping;
        memset(&topic_mapping, 0, sizeof(aiot_mqtt_topic_map_t));
        topic_mapping.topic = g_task_recv_topic_mapping[i].topic;
        topic_mapping.handler = g_task_recv_topic_mapping[i].func;

        aiot_mqtt_setopt(task_handle->mqtt_handle, AIOT_MQTTOPT_REMOVE_TOPIC_MAP, &topic_mapping);
    }

    sysdep->core_sysdep_free(task_handle);

    core_global_deinit(sysdep);
    return STATE_SUCCESS;
}

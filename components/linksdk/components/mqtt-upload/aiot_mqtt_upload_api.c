/**
 * @file aiot_mqtt_upload_api.c
 * @brief MQTT UPLOAD模块头文件, 提供通过mqtt进行文件上传的能力
 *
 * @copyright Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */

#include "mqtt_upload_private.h"

#define UPLOAD_GET_U16BIT(h, l)    (((h << 8) | l) & 0xFFFF)
#define UPLOAD_GET_U16BIT_HIGH(n)  ((n & 0xFF00) >> 8)
#define UPLOAD_GET_U16BIT_LOW(n)   (n & 0x00FF)

static int32_t _mqtt_upload_filelist_insert(mqtt_upload_handle_t *up_handle, upload_file_task_handle_t *up_task);
static void _mqtt_upload_filelist_destroy(mqtt_upload_handle_t *up_handle);
static int32_t _mqtt_upload_resend_pub(mqtt_upload_handle_t *up_handle, upload_file_task_handle_t *up_task);
static void _mqtt_upload_up_task_free(aiot_sysdep_portfile_t *sysdep, upload_file_task_handle_t *up_task);

static void _mqtt_upload_receive_init_response_handler(void *handle, const aiot_mqtt_recv_t *const packet, void *userdata);
static void _mqtt_upload_receive_upload_response_handler(void *handle, const aiot_mqtt_recv_t *const packet, void *userdata);
static void _mqtt_upload_receive_cancel_response_handler(void *handle, const aiot_mqtt_recv_t *const packet, void *userdata);

typedef struct {
    char *topic_fmt;
    aiot_mqtt_recv_handler_t func;
} _upload_sub_topic_map_t;

static _upload_sub_topic_map_t g_upload_sub_topic_map[MQTT_UPLOAD_DEFAULT_SUBTOPIC_NUM] =  {
    {
        MQTT_UPLOAD_UPLOAD_INIT_REPLY,
        _mqtt_upload_receive_init_response_handler
    },
    {
        MQTT_UPLOAD_UPLOAD_FILE_REPLY,
        _mqtt_upload_receive_upload_response_handler
    },
    {
        MQTT_UPLOAD_UPLOAD_CANCEL_REPLY,
        _mqtt_upload_receive_cancel_response_handler
    }
};

/*
 * CRC lookup table for bytes, generating polynomial is 0x8005
 * input: reflexed (LSB first)
 * output: reflexed also...
 */
const static uint16_t crc_ibm_table[256] = {
    0x0000, 0xc0c1, 0xc181, 0x0140, 0xc301, 0x03c0, 0x0280, 0xc241,
    0xc601, 0x06c0, 0x0780, 0xc741, 0x0500, 0xc5c1, 0xc481, 0x0440,
    0xcc01, 0x0cc0, 0x0d80, 0xcd41, 0x0f00, 0xcfc1, 0xce81, 0x0e40,
    0x0a00, 0xcac1, 0xcb81, 0x0b40, 0xc901, 0x09c0, 0x0880, 0xc841,
    0xd801, 0x18c0, 0x1980, 0xd941, 0x1b00, 0xdbc1, 0xda81, 0x1a40,
    0x1e00, 0xdec1, 0xdf81, 0x1f40, 0xdd01, 0x1dc0, 0x1c80, 0xdc41,
    0x1400, 0xd4c1, 0xd581, 0x1540, 0xd701, 0x17c0, 0x1680, 0xd641,
    0xd201, 0x12c0, 0x1380, 0xd341, 0x1100, 0xd1c1, 0xd081, 0x1040,
    0xf001, 0x30c0, 0x3180, 0xf141, 0x3300, 0xf3c1, 0xf281, 0x3240,
    0x3600, 0xf6c1, 0xf781, 0x3740, 0xf501, 0x35c0, 0x3480, 0xf441,
    0x3c00, 0xfcc1, 0xfd81, 0x3d40, 0xff01, 0x3fc0, 0x3e80, 0xfe41,
    0xfa01, 0x3ac0, 0x3b80, 0xfb41, 0x3900, 0xf9c1, 0xf881, 0x3840,
    0x2800, 0xe8c1, 0xe981, 0x2940, 0xeb01, 0x2bc0, 0x2a80, 0xea41,
    0xee01, 0x2ec0, 0x2f80, 0xef41, 0x2d00, 0xedc1, 0xec81, 0x2c40,
    0xe401, 0x24c0, 0x2580, 0xe541, 0x2700, 0xe7c1, 0xe681, 0x2640,
    0x2200, 0xe2c1, 0xe381, 0x2340, 0xe101, 0x21c0, 0x2080, 0xe041,
    0xa001, 0x60c0, 0x6180, 0xa141, 0x6300, 0xa3c1, 0xa281, 0x6240,
    0x6600, 0xa6c1, 0xa781, 0x6740, 0xa501, 0x65c0, 0x6480, 0xa441,
    0x6c00, 0xacc1, 0xad81, 0x6d40, 0xaf01, 0x6fc0, 0x6e80, 0xae41,
    0xaa01, 0x6ac0, 0x6b80, 0xab41, 0x6900, 0xa9c1, 0xa881, 0x6840,
    0x7800, 0xb8c1, 0xb981, 0x7940, 0xbb01, 0x7bc0, 0x7a80, 0xba41,
    0xbe01, 0x7ec0, 0x7f80, 0xbf41, 0x7d00, 0xbdc1, 0xbc81, 0x7c40,
    0xb401, 0x74c0, 0x7580, 0xb541, 0x7700, 0xb7c1, 0xb681, 0x7640,
    0x7200, 0xb2c1, 0xb381, 0x7340, 0xb101, 0x71c0, 0x7080, 0xb041,
    0x5000, 0x90c1, 0x9181, 0x5140, 0x9301, 0x53c0, 0x5280, 0x9241,
    0x9601, 0x56c0, 0x5780, 0x9741, 0x5500, 0x95c1, 0x9481, 0x5440,
    0x9c01, 0x5cc0, 0x5d80, 0x9d41, 0x5f00, 0x9fc1, 0x9e81, 0x5e40,
    0x5a00, 0x9ac1, 0x9b81, 0x5b40, 0x9901, 0x59c0, 0x5880, 0x9841,
    0x8801, 0x48c0, 0x4980, 0x8941, 0x4b00, 0x8bc1, 0x8a81, 0x4a40,
    0x4e00, 0x8ec1, 0x8f81, 0x4f40, 0x8d01, 0x4dc0, 0x4c80, 0x8c41,
    0x4400, 0x84c1, 0x8581, 0x4540, 0x8701, 0x47c0, 0x4680, 0x8641,
    0x8201, 0x42c0, 0x4380, 0x8341, 0x4100, 0x81c1, 0x8081, 0x4040,
};

static uint16_t crc_ibm(uint8_t const *buffer, size_t len)
{
    uint16_t crc = 0x0000;
    uint8_t lut;

    while (len--) {
        lut = (crc ^ (*buffer++)) & 0xFF;
        crc = (crc >> 8) ^ crc_ibm_table[lut];
    }
    return crc;
}

void _mqtt_upload_get_uuid(aiot_sysdep_portfile_t *sysdep, char *uuid) {
    uint32_t rand_value = 0;
    char uuid_temp[MQTT_UPLOAD_DEFAULT_UUID_SIZE] = {0};
    uint8_t len = 0;

    sysdep->core_sysdep_rand((uint8_t *)&rand_value, sizeof(rand_value));
	memset(uuid_temp, 0, sizeof(uuid_temp));
    rand_value = rand_value % 100;
    core_uint642str(sysdep->core_sysdep_time(), uuid_temp, &len);
    if (len > 13) {
        core_int2str(rand_value, &uuid_temp[13], NULL);
    } else {
        core_int2str(rand_value, &uuid_temp[len], NULL);
    }

    if (uuid != NULL) {
        // Limit length 1 to 15
        memcpy(uuid, uuid_temp, 15);
    }
}

static int32_t _mqtt_upload_get_payload(aiot_sysdep_portfile_t *sysdep, char *params, char **out)
{
    char *payload;
    int32_t id = 0;
    char id_string[11] = { 0 };
    char *src[2] = { NULL };

    if (sysdep == NULL) {
        return STATE_MQTT_UPLOAD_MQTT_HANDLE_IS_NULL;
    }

    core_global_alink_id_next(sysdep, &id);
    core_int2str(id, id_string, NULL);

    src[0] = id_string;
    src[1] = params;

    int res = core_sprintf(sysdep, &payload, MQTT_UPLOAD_DEFAULT_BASE_FMT, src, sizeof(src) / sizeof(char *),
                    MQTT_UPLOAD_MODULE_NAME);
    *out = payload;

    return res;
}

static int32_t _mqtt_upload_get_topic(mqtt_upload_handle_t *up_handle, char **topic, char *fmt)
{
    char *topic_string[2] = {NULL};
    char *temp_topic = NULL;
    int32_t res = STATE_SUCCESS;

    if (topic == NULL || fmt == NULL) {
        return STATE_MQTT_UPLOAD_PARAMS_IS_NULL;
    }

    if (up_handle == NULL || up_handle->mqtt_handle == NULL) {
        return STATE_MQTT_UPLOAD_MQTT_HANDLE_IS_NULL;
    }
    topic_string[0] = core_mqtt_get_product_key(up_handle->mqtt_handle);
    topic_string[1] = core_mqtt_get_device_name(up_handle->mqtt_handle);

    aiot_sysdep_portfile_t *sysdep = up_handle->sysdep;
    res = core_sprintf(sysdep, &temp_topic, fmt, topic_string, sizeof(topic_string) / sizeof(char *),
                 MQTT_UPLOAD_MODULE_NAME);
    *topic = temp_topic;

    return res;
}

static int32_t _mqtt_upload_data_encode_package(uint8_t *data, uint32_t data_len, char *head_payload, uint8_t *payload) {

    int32_t res = STATE_SUCCESS;
    uint16_t crc16 = 0;

    if (data == NULL || head_payload == NULL || payload == NULL) {
        return STATE_MQTT_UPLOAD_PARAMS_IS_NULL;
    }
    uint32_t head_len = strlen(head_payload);
    uint8_t *pos = payload;

    *pos++ = UPLOAD_GET_U16BIT_HIGH(head_len);
    *pos++ = UPLOAD_GET_U16BIT_LOW(head_len);

    memcpy(pos, head_payload, head_len);
    memcpy(pos + head_len, data, data_len);
    crc16 = crc_ibm(data, data_len);
    *(pos + head_len + data_len) = UPLOAD_GET_U16BIT_LOW(crc16);
    *(pos + head_len + data_len + 1) = UPLOAD_GET_U16BIT_HIGH(crc16);

    return res;
}

static int32_t _mqtt_upload_creat_string_malloc(aiot_sysdep_portfile_t *sysdep, const char *ch, uint32_t size, char **out) {

    if (*out != NULL) {
        sysdep->core_sysdep_free(*out);
        *out = NULL;
    }
    *out = sysdep->core_sysdep_malloc(size + 1, MQTT_UPLOAD_MODULE_NAME);
    memset(*out, 0, size + 1);
    if (ch != NULL) {
        memcpy(*out, ch, size);
    }
    return 0;
}

static int32_t _mqtt_upload_file_task_init(aiot_sysdep_portfile_t *sysdep, upload_file_task_handle_t *up_task, const char *file_name,
        const uint32_t file_size, const char *mode, const char *digest, const char *uuid, aiot_mqtt_upload_read_handler_t read_data_handler, void *userdata) {
    if (sysdep == NULL || up_task == NULL) {
        return STATE_MQTT_UPLOAD_UPTASK_IS_NULL;
    }

    if (file_name == NULL) {
        return STATE_MQTT_UPLOAD_FILENAME_IS_NULL;
    }

    up_task->file_size = file_size;
    up_task->failed_count = 0;
    up_task->file_offset = 0;
    up_task->status = STATE_MQTT_UPLOAD_REQUEST_INIT;
    up_task->read_data_handler = read_data_handler;
    up_task->userdata = userdata;
    up_task->is_rsp = 0;
    up_task->is_destory = 1;
    up_task->send_last_time = 0;

    _mqtt_upload_creat_string_malloc(sysdep, file_name, strlen(file_name), (char **)&(up_task->file_name));
    _mqtt_upload_creat_string_malloc(sysdep, mode, MQTT_UPLOAD_DEFAULT_MODE_MAX_SIZE, (char **)&(up_task->mode));
    _mqtt_upload_creat_string_malloc(sysdep, digest, MQTT_UPLOAD_DEFAULT_CRC64_LEN, (char **)&(up_task->crc64));
    _mqtt_upload_creat_string_malloc(sysdep, uuid, MQTT_UPLOAD_DEFAULT_UUID_SIZE, (char **)&(up_task->uuid));
    _mqtt_upload_creat_string_malloc(sysdep, NULL, MQTT_UPLOAD_DEFAULT_UPLOAD_SIZE, (char **)&(up_task->upload_id));

    return STATE_SUCCESS;
}

static int32_t _mqtt_upload_send_request_init(mqtt_upload_handle_t *up_handle, upload_file_task_handle_t *up_task, char *file_name,
    uint32_t file_size, const char *mode, const char *digest, const char *uuid) {

    char *params = NULL, *payload = NULL, *topic = NULL;
    char size_string[11] = { 0 };
    char *src[6];
    int32_t res = STATE_SUCCESS;
    aiot_sysdep_portfile_t *sysdep = NULL;

    if (up_handle == NULL) {
        return STATE_MQTT_UPLOAD_MQTT_HANDLE_IS_NULL;
    }

    if (NULL == file_name || mode == NULL) {
        return STATE_MQTT_UPLOAD_PARAMS_IS_NULL;
    }
    sysdep = up_handle->sysdep;
    core_int2str(file_size, size_string, NULL);
    src[0] = file_name;
    src[1] = size_string;
    src[2] = (char *)mode;

    if (digest == NULL || strlen(digest) == 0) {
        src[3] = (char *)uuid;
        src[4] = "";
        src[5] = "";
        if ((res = core_sprintf(sysdep, &params, MQTT_UPLOAD_OPEN_STREAM_PARAM_FMT, src,
                sizeof(src) / sizeof(char *) - 2, MQTT_UPLOAD_MODULE_NAME)) != STATE_SUCCESS) {
            goto exit;
        }
    } else {
        src[3] = "crc64";
        src[4] = (char *)digest;
        src[5] = (char *)uuid;
        if ((res = core_sprintf(sysdep, &params, MQTT_UPLOAD_OPEN_STREAM_PARAM_FMT_MD5, src,
                sizeof(src) / sizeof(char *), MQTT_UPLOAD_MODULE_NAME)) != STATE_SUCCESS) {
            goto exit;
        }
    }

    if ((res = _mqtt_upload_get_payload(sysdep, params, &payload)) != STATE_SUCCESS) {
        goto exit;
    }
    _mqtt_upload_get_topic(up_handle, &topic, MQTT_UPLOAD_UPLOAD_INIT);
    res = aiot_mqtt_pub(up_handle->mqtt_handle, topic, (uint8_t *)payload, strlen(payload), CORE_MQTT_QOS0);
    up_task->send_last_time = sysdep->core_sysdep_time();

exit:
    if(topic != NULL) {
        sysdep->core_sysdep_free(topic);
    }
    if(payload != NULL) {
        sysdep->core_sysdep_free(payload);
    }
    if(params != NULL) {
        sysdep->core_sysdep_free(params);
    }

    return res;
}

static int32_t _mqtt_upload_send_block_file(mqtt_upload_handle_t *up_handle, upload_file_task_handle_t *up_task, uint8_t *data,
    const uint32_t block_size, const uint32_t offset, const uint8_t is_complete)
{
    char *params = NULL, *head_payload = NULL, *topic = NULL, *payload = NULL;
    char bsize_string[11] = {0}, boffset_string[11] = {0};
    char *src[4] = { NULL };
    int32_t res = STATE_SUCCESS;
    aiot_sysdep_portfile_t *sysdep = NULL;
    uint32_t payload_len = 0;

    if (up_handle == NULL || up_handle->mqtt_handle == NULL) {
        return STATE_MQTT_UPLOAD_MQTT_HANDLE_IS_NULL;
    }

    if (data == NULL) {
        return STATE_MQTT_UPLOAD_FILE_DATA_IS_NULL;
    }

    if (up_task == NULL) {
        return STATE_MQTT_UPLOAD_UPTASK_IS_NULL;
    }

    sysdep = up_handle->sysdep;
    core_uint2str(block_size, bsize_string, NULL);
    core_uint2str(offset, boffset_string, NULL);

    src[0] = up_task->upload_id;
    src[1] = boffset_string;
    src[2] = bsize_string;
    if (is_complete == 1) {
        src[3] = "true";
    } else {
        src[3] = "false";
    }

    if ((res = core_sprintf(sysdep, &params, MQTT_UPLOAD_SEND_DATA_PARAM_FMT, src, sizeof(src) / sizeof(char *),
            MQTT_UPLOAD_MODULE_NAME)) != STATE_SUCCESS ) {
        goto exit;
    }

    if ((res = _mqtt_upload_get_payload(sysdep, params, &head_payload)) != STATE_SUCCESS ) {
        goto exit;
    }

    _mqtt_upload_get_topic(up_handle, &topic, MQTT_UPLOAD_UPLOAD_FILE);

    payload_len = block_size + strlen(head_payload) + sizeof(uint16_t) + sizeof(uint16_t);
    payload = sysdep->core_sysdep_malloc( payload_len, MQTT_UPLOAD_MODULE_NAME);
    if (payload == NULL) {
        goto exit;
    }
    memset(payload, 0x00, payload_len);
    _mqtt_upload_data_encode_package(data, block_size, head_payload, (uint8_t *)payload);
    res = aiot_mqtt_pub(up_handle->mqtt_handle, topic, (uint8_t *)payload, payload_len, CORE_MQTT_QOS0);
    up_task->send_last_time = sysdep->core_sysdep_time();

exit:
    if(topic != NULL) {
        sysdep->core_sysdep_free(topic);
    }
    if(head_payload != NULL) {
        sysdep->core_sysdep_free(head_payload);
    }
    if(params != NULL) {
        sysdep->core_sysdep_free(params);
    }
    if (payload != NULL) {
        sysdep->core_sysdep_free(payload);
    }

    return res;
}

static int32_t _mqtt_upload_send_request_cancel(mqtt_upload_handle_t *up_handle, upload_file_task_handle_t *up_task)
{
    char *params = NULL, *payload = NULL, *topic = NULL;
    int32_t res = STATE_SUCCESS;
    aiot_sysdep_portfile_t *sysdep = NULL;
    if (up_handle == NULL || up_task == NULL || up_handle->mqtt_handle == NULL) {
        return STATE_MQTT_UPLOAD_MQTT_HANDLE_IS_NULL;
    }
    sysdep = up_handle->sysdep;
    if ((res = core_sprintf(sysdep, &params, MQTT_UPLOAD_CLOSE_STREAM_PARAM_FMT,
            &up_task->upload_id, 1, MQTT_UPLOAD_MODULE_NAME)) != STATE_SUCCESS) {
        goto exit;
    }

    if ((res = _mqtt_upload_get_payload(sysdep, params, &payload)) != STATE_SUCCESS) {
        goto exit;
    }

    if ((_mqtt_upload_get_topic(up_handle, &topic, MQTT_UPLOAD_UPLOAD_CANCEL)) != STATE_SUCCESS) {
        goto exit;
    }

    res = aiot_mqtt_pub(up_handle->mqtt_handle, topic, (uint8_t *)payload, strlen(payload), CORE_MQTT_QOS0);
    up_task->send_last_time = sysdep->core_sysdep_time();
exit:
    if(topic != NULL) {
        sysdep->core_sysdep_free(topic);
    }
    if(payload != NULL) {
        sysdep->core_sysdep_free(payload);
    }
    if(params != NULL) {
        sysdep->core_sysdep_free(params);
    }

    return res;
}

static void _mqtt_upload_core_mqtt_process_handler(void *context, aiot_mqtt_event_t *event, core_mqtt_event_t *core_event)
{
    mqtt_upload_handle_t *up_handle = (mqtt_upload_handle_t *)context;

    if (core_event != NULL) {
        switch (core_event->type) {
            case CORE_MQTTEVT_DEINIT: {
                up_handle->mqtt_handle = NULL;
                return;
            }
            break;
            default: {

            }
            break;
        }
    }
}

static int32_t _mqtt_upload_mqtt_operate_process_handler(mqtt_upload_handle_t *up_handle, core_mqtt_option_t option)
{
    core_mqtt_process_data_t process_data;

    memset(&process_data, 0, sizeof(core_mqtt_process_data_t));
    process_data.handler = _mqtt_upload_core_mqtt_process_handler;
    process_data.context = up_handle;

    return core_mqtt_setopt(up_handle->mqtt_handle, option, &process_data);
}

static int32_t _mqtt_upload_parse_json(aiot_sysdep_portfile_t *sysdep, void *input, uint32_t input_len, char *key_word,
                               char **out)
{
    int32_t res = STATE_SUCCESS;
    char *value = NULL, *buffer = NULL;
    uint32_t value_len = 0, buffer_len = 0;

    res = core_json_value((const char *)input, input_len, key_word, strlen(key_word), &value, &value_len);
    if (res != STATE_SUCCESS) {
        return STATE_MQTT_UPLOAD_PARSE_JSON_FAILED;
    }
    buffer_len = value_len + 1;
    buffer = sysdep->core_sysdep_malloc(buffer_len, MQTT_UPLOAD_MODULE_NAME);
    if (NULL == buffer) {
        return STATE_MQTT_UPLOAD_MALLOC_FAILED;
    }
    memset(buffer, 0, buffer_len);
    memcpy(buffer, value, value_len);
    *out = buffer;
    return res;
}

static uint8_t _mqtt_upload_block_size_is_valid(uint32_t block_size, uint8_t is_final) {
    /* 长度0无效 */
    if (block_size == 0) {
        return 0;
    }
    if (is_final) {
        /* 最后一包不大于 MAX Size */
        return (block_size > MQTT_UPLOAD_DEFAULT_SEND_MAX_SIZE ? 0 : 1);
    } else {
        /* Uploading 不能小于256B */
        return (block_size < MQTT_UPLOAD_DEFAULT_SEND_MIN_SIZE || block_size > MQTT_UPLOAD_DEFAULT_SEND_MAX_SIZE) ? 0 : 1;
    }
}

static void _mqtt_upload_recv_response_process(void *handle, aiot_mqtt_upload_recv_t *packet, upload_file_task_handle_t *up_task)
{
    uint8_t *data = NULL;
    uint32_t data_len = 0;
    int32_t res = STATE_SUCCESS;
    aiot_sysdep_portfile_t *sysdep = NULL;

    if (handle == NULL || packet == NULL) {
        return;
    }
    mqtt_upload_handle_t *up_handle = (mqtt_upload_handle_t *)handle;
    sysdep = up_handle->sysdep;
    switch (packet->type) {
        case AIOT_MQTT_UPLOADRECV_CANCEL_REPLY :
            /* 取消是否成功 */
            if (packet->desc.code == UPLOAD_FILE_OK) {
                up_task->status = STATE_MQTT_UPLOAD_CANCEL_SUCCESS;
                up_task->failed_count = 0;
            } else if (packet->desc.code == UPLOAD_FILE_FILE_BLOCK_DUPLICATION) {
                /* 重发cancel */
                if (_mqtt_upload_resend_pub(up_handle, up_task) != STATE_SUCCESS) {
                    up_task->status = STATE_MQTT_UPLOAD_CANCEL_FAILED;
                }
            } else {
                up_task->status = STATE_MQTT_UPLOAD_CANCEL_FAILED;
            }

            /* 返回取消结果 */
            if (up_task->read_data_handler != NULL &&
                    (up_task->status == STATE_MQTT_UPLOAD_CANCEL_FAILED || up_task->status == STATE_MQTT_UPLOAD_CANCEL_SUCCESS)) {
                up_task->read_data_handler(packet, NULL, 0, up_task->userdata);
            } else {
                return;
            }
            core_log(sysdep, AIOT_MQTT_UPLOADRECV_CANCEL_REPLY, "AIOT_MQTT_UPLOADRECV_CANCEL_REPLY.\r\n");
            break;
        case AIOT_MQTT_UPLOADRECV_INIT_REPLY :
        case AIOT_MQTT_UPLOADRECV_UPLOAD_REPLY :

            if (packet->desc.code == UPLOAD_FILE_OK) {
                up_task->is_rsp = 1;
                up_task->failed_count = 0;
                up_task->file_offset = packet->desc.file_offset + packet->desc.block_size;
                if (packet->desc.complete == 1) {
                    up_task->status = STATE_MQTT_UPLOAD_FINISHED;
                    core_log1(sysdep, up_task->status,
                        "Complete upload total size:%d\r\n", &up_task->file_offset);
                    return;
                }

                up_task->status = STATE_MQTT_UPLOAD_IS_UPLOADING;
                packet->desc.file_offset += packet->desc.block_size;

                if (up_task->data == NULL) {
                    up_task->data = sysdep->core_sysdep_malloc(up_handle->default_block_size, MQTT_UPLOAD_MODULE_NAME);
                }

                data = up_task->data;
                data_len = 0;
                if (up_task->read_data_handler != NULL) {
                    data_len = up_task->read_data_handler(packet, data, up_handle->default_block_size, up_task->userdata);
                } else {
                    return;
                }
                core_log1(sysdep, STATE_MQTT_UPLOAD_PROCESS_REPLY_ERROR, "Read data_len:%d\r\n", &data_len);
                if (data_len != 0 && data != NULL &&
                        (_mqtt_upload_block_size_is_valid(data_len, ((up_task->file_offset + data_len) >= up_task->file_size) ? 1 : 0))) {

                    if ((res = _mqtt_upload_send_block_file(up_handle, up_task, data, data_len, up_task->file_offset, 0)) == STATE_SUCCESS) {
                        up_task->block_size = data_len;
                    } else {
                        up_task->status = STATE_MQTT_UPLOAD_FAILED;
                    }
                } else {
                    up_task->status = STATE_MQTT_UPLOAD_FAILED;
                }
                return;
            } else if (packet->desc.code == UPLOAD_FILE_FAILED_BLOCK_CRC) {
                /* CRC校验失败重新发送当前blcok */
                _mqtt_upload_resend_pub(up_handle, up_task);
            } else {
                if (packet->desc.code == UPLOAD_FILE_FAILED_WHOLE_CHECK) {
                    up_task->status = STATE_MQTT_UPLOAD_FAILED_WHOLE_CHECK;
                } else if(packet->desc.code == UPLOAD_FILE_HAVE_FINISHED) {
                    up_task->status = STATE_MQTT_UPLOAD_FINISHED;
                } else {
                    up_task->status = STATE_MQTT_UPLOAD_FAILED;
                }
                /* 其他异常发送失败，结束发送 */
                if (up_task->read_data_handler != NULL) {
                    up_task->read_data_handler(packet, NULL, 0, up_handle);
                }
            }

            break;
        default:
            break;
    }
}

static void _mqtt_upload_desc_free(aiot_sysdep_portfile_t *sysdep, aiot_mqtt_upload_recv_t *msg) {
    if (msg->desc.file_name != NULL) {
        sysdep->core_sysdep_free(msg->desc.file_name);
    }
    if (msg->desc.upload_id != NULL) {
        sysdep->core_sysdep_free(msg->desc.upload_id);
    }
    if (msg->desc.message != NULL) {
        sysdep->core_sysdep_free(msg->desc.message);
    }
}

static void _mqtt_upload_desc_new(aiot_sysdep_portfile_t *sysdep, aiot_mqtt_upload_recv_t *packet,
             aiot_mqtt_upload_recv_t *out_packet) {
    if (packet->desc.file_name != NULL) {
        if (out_packet->desc.file_name == NULL) {
            _mqtt_upload_creat_string_malloc(sysdep, packet->desc.file_name, strlen(packet->desc.file_name),
                    (char **)&(out_packet->desc.file_name));
        } else {
            memcpy(out_packet->desc.file_name, packet->desc.file_name, strlen(packet->desc.file_name));
        }
    }
    if (packet->desc.upload_id != NULL) {
        if (out_packet->desc.upload_id == NULL) {
            _mqtt_upload_creat_string_malloc(sysdep, packet->desc.upload_id, strlen(packet->desc.upload_id),
                    (char **)&(out_packet->desc.upload_id));
        } else {
            memcpy(out_packet->desc.upload_id, packet->desc.upload_id, strlen(packet->desc.upload_id));
        }
    }
    out_packet->desc.file_size = packet->desc.file_size;
    out_packet->desc.file_offset = packet->desc.file_offset;
    out_packet->desc.block_size = packet->desc.block_size;
    out_packet->desc.code = packet->desc.code;
}

static void _mqtt_upload_desc_copy(aiot_sysdep_portfile_t *sysdep, aiot_mqtt_upload_recv_t *packet,
             aiot_mqtt_upload_recv_t *out_packet) {
    if (packet->desc.file_name != NULL) {
        out_packet->desc.file_name = packet->desc.file_name;
    }
    if (packet->desc.upload_id != NULL) {
        out_packet->desc.upload_id = packet->desc.upload_id;
    }
    out_packet->desc.file_size = packet->desc.file_size;
    out_packet->desc.file_offset = packet->desc.file_offset;
    out_packet->desc.block_size = packet->desc.block_size;
    out_packet->desc.code = packet->desc.code;
}

static void _mqtt_upload_receive_init_response_handler(void *handle, const aiot_mqtt_recv_t *const packet, void *userdata) {
    mqtt_upload_handle_t *up_handle = NULL;
    char *value = NULL, *code_string = NULL, *offset_string = NULL, *filesize_string = NULL;
    uint32_t value_len = 0;
    int32_t res = STATE_SUCCESS;
    aiot_sysdep_portfile_t *sysdep = NULL;
    upload_file_task_handle_t *up_task = NULL;
    uint32_t payload_len = 0;
    char *payload = NULL;
    aiot_mqtt_upload_recv_t msg = {
        .type = AIOT_MQTT_UPLOADRECV_INIT_REPLY,
        .desc.file_name = NULL,
        .desc.upload_id = NULL,
        .desc.message = NULL
    };
    memset(&msg.desc, 0, sizeof(aiot_mqtt_upload_desc_t));

    if (AIOT_MQTTRECV_PUB != packet->type) {
        return;
    }

    /* Sub Topic Set userdata*/
    if (userdata == NULL) {
        return;
    }

    up_handle = (mqtt_upload_handle_t *)userdata;
    sysdep = up_handle->sysdep;
    payload = (char *)packet->data.pub.payload;
    payload_len = packet->data.pub.payload_len;
    /* Parse ID & Code & message */
    if ((res = core_json_value((char *)payload, payload_len, "id", strlen("id"),
            &value, &value_len)) != STATE_SUCCESS) {
        core_log(sysdep, STATE_MQTT_UPLOAD_PARSE_INIT_FAILED, "recv handle parse err id\r\n");
    }

    if (STATE_SUCCESS != _mqtt_upload_parse_json(sysdep, payload, payload_len, "code", &(code_string))) {
        core_log(sysdep, STATE_MQTT_UPLOAD_PARSE_INIT_FAILED, "json parse error errCode\r\n");
        goto exit;
    }
    core_str2uint(code_string, strlen(code_string), &msg.desc.code);

    if (STATE_SUCCESS != _mqtt_upload_parse_json(sysdep, payload, payload_len, "message", &(msg.desc.message))) {
        core_log(sysdep, STATE_MQTT_UPLOAD_PARSE_INIT_FAILED, "json parse error errMsg\r\n");
        goto exit;
    }

    /* Data */
    if ((res = core_json_value((char *)payload, payload_len, "data", strlen("data"),
                            &value, &value_len)) < 0 ) {
        core_log(sysdep, STATE_MQTT_UPLOAD_PARSE_INIT_FAILED, "json parse error data\r\n");
        goto exit;
    }
    {
        /* Parse Json Object */
        char *data = (char *)value;
        uint32_t data_len = value_len;
        if ((STATE_SUCCESS != _mqtt_upload_parse_json(sysdep, data, data_len, "fileName", &(msg.desc.file_name)))) {
            core_log(sysdep, STATE_MQTT_UPLOAD_PARSE_INIT_FAILED, "json parse error fileName\r\n");
            goto exit;
        }
        if (msg.desc.code == UPLOAD_FILE_OK) {
            if ((STATE_SUCCESS != _mqtt_upload_parse_json(sysdep, data, data_len, "uploadId", &(msg.desc.upload_id)))) {
                core_log(sysdep, STATE_MQTT_UPLOAD_PARSE_INIT_FAILED, "json parse error uploadId\r\n");
                goto exit;
            }
        } else {
            msg.desc.upload_id = NULL;
        }
        if ((STATE_SUCCESS == _mqtt_upload_parse_json(sysdep, data, data_len, "offset", &(offset_string)))) {
            core_str2uint(offset_string, strlen(offset_string), &msg.desc.file_offset);
        } else {
            msg.desc.file_offset = 0;
        }
        if ((STATE_SUCCESS == _mqtt_upload_parse_json(sysdep, data, data_len, "fileSize", &(filesize_string)))) {
            core_str2uint(filesize_string, strlen(filesize_string), &msg.desc.file_size);
        } else {
            msg.desc.file_size = 0;
        }
    }

    sysdep->core_sysdep_mutex_lock(up_handle->uptask_mutex);
    core_list_for_each_entry(up_task, &up_handle->file_list, linked_node, upload_file_task_handle_t) {
        if (msg.desc.file_name != NULL && up_task->file_name != NULL &&
                memcmp(msg.desc.file_name, up_task->file_name, strlen(up_task->file_name)) == 0) {
            if (msg.desc.upload_id == NULL) {
                /* Upload 为空异常 */
                core_log(sysdep, STATE_MQTT_UPLOAD_PARSE_INIT_FAILED, "[ERROR] Receive uploadID is NULL.\r\n");
            } else {
                if (up_task->upload_id == NULL) {
                    /* 第一次收到云端应答 */
                    if ((up_task->upload_id =
                            sysdep->core_sysdep_malloc(strlen(msg.desc.upload_id) + 1, MQTT_UPLOAD_MODULE_NAME)) == NULL) {

                        core_log(sysdep, STATE_MQTT_UPLOAD_MALLOC_FAILED, "Malloc upload_id error.\r\n");
                    } else {
                        memset(up_task->upload_id, 0, strlen(msg.desc.upload_id) + 1);
                        memcpy(up_task->upload_id, msg.desc.upload_id, strlen(msg.desc.upload_id));
                        msg.desc.block_size = 0;
                    }
                } else {
                    /* 重复收到云端应答 */
                    memset(up_task->upload_id, 0, strlen(msg.desc.upload_id) + 1);
                    memcpy(up_task->upload_id, msg.desc.upload_id, strlen(msg.desc.upload_id));
                    msg.desc.block_size = 0;
                }
            }
            up_task->is_rsp = 1;
            up_task->rsp_code = msg.desc.code;
            _mqtt_upload_desc_new(up_handle->sysdep, &msg, &up_task->packet);
	        _mqtt_upload_recv_response_process(up_handle, &msg, up_task);
            break;
        }
    }
    sysdep->core_sysdep_mutex_unlock(up_handle->uptask_mutex);
    core_log1(sysdep, STATE_MQTT_UPLOAD_RECV_UPLOADID_ERROR, "Recv file name:%s\r\n", msg.desc.file_name);
exit:
    if (code_string != NULL) {
        sysdep->core_sysdep_free(code_string);
    }
    if (offset_string != NULL) {
        sysdep->core_sysdep_free(offset_string);
    }
    if (filesize_string != NULL) {
        sysdep->core_sysdep_free(filesize_string);
    }
    _mqtt_upload_desc_free(sysdep, &msg);
}

static void _mqtt_upload_receive_upload_response_handler(void *handle, const aiot_mqtt_recv_t *const packet, void *userdata) {
    mqtt_upload_handle_t *up_handle = NULL;
    char *value = NULL;
    uint32_t value_len = 0;
    int32_t res = STATE_SUCCESS;
    char *offset_string = NULL, *bsize_string = NULL, *code_string = NULL, *complete_string = NULL;
    aiot_sysdep_portfile_t *sysdep = NULL;
    upload_file_task_handle_t *up_task = NULL;
    uint32_t payload_len = 0;
    char *payload = NULL;
    aiot_mqtt_upload_recv_t msg = {
        .type = AIOT_MQTT_UPLOADRECV_UPLOAD_REPLY,
        .desc.file_name = NULL,
        .desc.upload_id = NULL,
        .desc.message = NULL
    };
    memset(&msg.desc, 0, sizeof(aiot_mqtt_upload_desc_t));

    if (AIOT_MQTTRECV_PUB != packet->type) {
        return;
    }
    if (userdata == NULL) {
        return;
    }

    up_handle = (mqtt_upload_handle_t *)userdata;
    sysdep = up_handle->sysdep;
    payload = (char *)packet->data.pub.payload;
    payload_len = packet->data.pub.payload_len;

    if ((res = core_json_value((char *)payload, payload_len, "id", strlen("id"),
                               &value, &value_len)) < 0) {
        core_log(sysdep, STATE_MQTT_UPLOAD_PARSE_UPLOAD_REPLY_FAILED, "recv handle parse err id\r\n");
    }

    /* Result Code && message */
    if (STATE_SUCCESS != _mqtt_upload_parse_json(sysdep, payload, payload_len, "code", &(code_string))) {
        core_log(sysdep, STATE_MQTT_UPLOAD_PARSE_UPLOAD_REPLY_FAILED, "json parse error errCode\r\n");
        goto exit;
    }
    core_str2uint(code_string, strlen(code_string), &msg.desc.code);

    if (STATE_SUCCESS != _mqtt_upload_parse_json(sysdep, payload, payload_len, "message", &(msg.desc.message))) {
        core_log(sysdep, STATE_MQTT_UPLOAD_PARSE_UPLOAD_REPLY_FAILED, "json parse error errMsg\r\n");
        goto exit;
    }

    /* Parse data object */
    if ((res = core_json_value((char *)payload, payload_len, "data", strlen("data"),
                               &value, &value_len)) < 0 ) {
        core_log(sysdep, STATE_MQTT_UPLOAD_PARSE_UPLOAD_REPLY_FAILED, "json parse error data\r\n");
        goto exit;
    }
    {
        char *data = (char *)value;
        uint32_t data_len = value_len;

        if (STATE_SUCCESS != _mqtt_upload_parse_json(sysdep, data, data_len, "uploadId", &(msg.desc.upload_id))) {
            core_log(sysdep, STATE_MQTT_UPLOAD_PARSE_UPLOAD_REPLY_FAILED, "json parse error uploadId\r\n");
            goto exit;
        }

        if (STATE_SUCCESS != _mqtt_upload_parse_json(sysdep, data, data_len, "offset", &(offset_string))) {
            core_log(sysdep, STATE_MQTT_UPLOAD_PARSE_UPLOAD_REPLY_FAILED, "json parse error offset\r\n");
            goto exit;
        }
        core_str2uint(offset_string, strlen(offset_string), &msg.desc.file_offset);

        if (STATE_SUCCESS != _mqtt_upload_parse_json(sysdep, data, data_len, "bSize", &(bsize_string))) {
            core_log(sysdep, STATE_MQTT_UPLOAD_PARSE_UPLOAD_REPLY_FAILED, "json parse error bSize\r\n");
            goto exit;
        }
        core_str2uint(bsize_string, strlen(bsize_string), &msg.desc.block_size);

        /* 只有最后一包应答消息包括complete */
        if (STATE_SUCCESS == _mqtt_upload_parse_json(sysdep, data, data_len, "complete", &(complete_string))) {
            msg.desc.complete = (memcmp(complete_string, "true", strlen(complete_string)) == 0 ? 1 : 0);
        } else {
            msg.desc.complete = 0xFF;
        }
    }

    sysdep->core_sysdep_mutex_lock(up_handle->uptask_mutex);
    core_list_for_each_entry(up_task, &up_handle->file_list, linked_node, upload_file_task_handle_t) {
        if (msg.desc.upload_id != NULL && up_task->upload_id != NULL &&
                memcmp(msg.desc.upload_id, up_task->upload_id, strlen(msg.desc.upload_id)) == 0) {

            if (up_task->status !=  STATE_MQTT_UPLOAD_IS_UPLOADING) {
                /* 状态为非上传状态 */
                break;
            }
            /* offset & block size 不同 忽略 */
            if (msg.desc.file_offset != up_task->file_offset  ||
                    msg.desc.block_size != up_task->block_size) {
                /* 和上传的offset ，bszie不同,忽略，等待超时重发 */
                core_log2(sysdep, STATE_MQTT_UPLOAD_RECV_FILE_INFO_ERROR, "Receive upload ACK error,file_offset:%d, block_szie:%d\r\n",
                    &msg.desc.file_offset, &msg.desc.block_size);
            } else {

                /* Modified File offset */
                uint32_t name_size = strlen(up_task->file_name) + 1;
                msg.desc.file_name = sysdep->core_sysdep_malloc(name_size, MQTT_UPLOAD_MODULE_NAME);
                memset(msg.desc.file_name, 0, name_size);
                memcpy(msg.desc.file_name, up_task->file_name, strlen(up_task->file_name));
                up_task->is_rsp = 1;
                up_task->rsp_code = msg.desc.code;
                _mqtt_upload_recv_response_process(up_handle, &msg, up_task);
            }
            break;
        }
    }
    sysdep->core_sysdep_mutex_unlock(up_handle->uptask_mutex);

exit:
    if (offset_string != NULL) {
        sysdep->core_sysdep_free(offset_string);
    }
    if (bsize_string != NULL) {
        sysdep->core_sysdep_free(bsize_string);
    }
    if (code_string != NULL) {
        sysdep->core_sysdep_free(code_string);
    }
    if (complete_string != NULL) {
        sysdep->core_sysdep_free(complete_string);
    }
    _mqtt_upload_desc_free(sysdep, &msg);

    return;
}

static void _mqtt_upload_receive_cancel_response_handler(void *handle, const aiot_mqtt_recv_t *const packet, void *userdata) {
    mqtt_upload_handle_t *up_handle = NULL;
    char *code_string = NULL, *value = NULL;
    uint32_t value_len = 0;
    int32_t res = STATE_SUCCESS;
    aiot_sysdep_portfile_t *sysdep = NULL;
    upload_file_task_handle_t *up_task = NULL;
    uint32_t payload_len = 0;
    char *payload = NULL;
    aiot_mqtt_upload_recv_t msg = {
        .type = AIOT_MQTT_UPLOADRECV_CANCEL_REPLY,
    };
    memset(&msg.desc, 0, sizeof(aiot_mqtt_upload_desc_t));

    if (AIOT_MQTTRECV_PUB != packet->type) {
        return;
    }

    if (userdata == NULL) {
        return;
    }

    up_handle = (mqtt_upload_handle_t *)userdata;
    sysdep = up_handle->sysdep;
    payload = (char *)packet->data.pub.payload;
    payload_len = packet->data.pub.payload_len;

    if ((res = core_json_value((char *)payload, payload_len, "id", strlen("id"),
                               &value, &value_len)) < 0) {
        core_log(sysdep, STATE_MQTT_UPLOAD_PARSE_CANCEL_FAILED, "recv handle parse err id\r\n");
    }

    /* Result Code && Message. */
    if (STATE_SUCCESS != _mqtt_upload_parse_json(sysdep, payload, payload_len, "code", &(code_string))) {
        core_log(sysdep, STATE_MQTT_UPLOAD_PARSE_CANCEL_FAILED, "json parse error errCode\r\n");
        goto exit;
    }
    core_str2uint(code_string, strlen(code_string), &msg.desc.code);

    if (STATE_SUCCESS != _mqtt_upload_parse_json(sysdep, payload, payload_len, "message", &(msg.desc.message))) {
        core_log(sysdep, STATE_MQTT_UPLOAD_PARSE_CANCEL_FAILED, "json parse error errMsg\r\n");
        goto exit;
    }

    /* Parse data object */
    if ((res = core_json_value((char *)payload, payload_len, "data", strlen("data"),
            &value, &value_len)) < 0 ) {
        core_log(sysdep, STATE_MQTT_UPLOAD_PARSE_CANCEL_FAILED, "json parse error data\r\n");
        goto exit;
    }
    {
        char *data = (char *)value;
        uint32_t data_len = value_len;

        if (STATE_SUCCESS != _mqtt_upload_parse_json(sysdep, data, data_len, "uploadId", &(msg.desc.upload_id))) {
            core_log(sysdep, STATE_MQTT_UPLOAD_PARSE_CANCEL_FAILED, "json parse error uploadId\r\n");
            goto exit;
        }
    }

    sysdep->core_sysdep_mutex_lock(up_handle->uptask_mutex);
    core_list_for_each_entry(up_task, &up_handle->file_list, linked_node, upload_file_task_handle_t) {
        if (msg.desc.upload_id != NULL && up_task->upload_id != NULL &&
                memcmp(msg.desc.upload_id, up_task->upload_id, strlen(up_task->upload_id)) == 0) {

            if (up_task->status ==  STATE_MQTT_UPLOAD_NONE) {
                break;
            }
            up_task->is_rsp = 1;
            up_task->rsp_code = msg.desc.code;
            _mqtt_upload_recv_response_process(up_handle, &msg, up_task);
            break;
        }
    }
    sysdep->core_sysdep_mutex_unlock(up_handle->uptask_mutex);
exit:

    if (code_string != NULL) {
        sysdep->core_sysdep_free(code_string);
    }
    _mqtt_upload_desc_free(sysdep, &msg);
}

static int32_t _mqtt_upload_subscribe(void *mqtt_handle, mqtt_upload_handle_t *up_handle)
{
    uint8_t i = 0;
    int32_t res = STATE_SUCCESS;
    aiot_sysdep_portfile_t *sysdep = NULL;

    if(up_handle == NULL || mqtt_handle ==NULL) {
        return STATE_MQTT_UPLOAD_MQTT_HANDLE_IS_NULL;
    }
    sysdep = up_handle->sysdep;
    for (i = 0; i < MQTT_UPLOAD_DEFAULT_SUBTOPIC_NUM; i++) {
        char *topic_src[] = {core_mqtt_get_product_key(mqtt_handle), core_mqtt_get_device_name(mqtt_handle)};
        char *topic = g_upload_sub_topic_map[i].topic_fmt;

        res = core_sprintf(sysdep, &topic, g_upload_sub_topic_map[i].topic_fmt, topic_src, sizeof(topic_src) / sizeof(char *),
                           MQTT_UPLOAD_MODULE_NAME);
        if (res != STATE_SUCCESS) {
            break;
        }
        res = aiot_mqtt_sub(mqtt_handle, topic, g_upload_sub_topic_map[i].func, 0, (void *)up_handle);
        sysdep->core_sysdep_free(topic);
        if (res < 0) {
            break;
        }
    }
    return res;
}

static int32_t _mqtt_upload_unsubscribe(mqtt_upload_handle_t *up_handle)
{
    uint8_t i = 0;
    int32_t res = STATE_SUCCESS;
    aiot_sysdep_portfile_t *sysdep = NULL;

    if(up_handle == NULL || up_handle->mqtt_handle ==NULL) {
        return STATE_MQTT_UPLOAD_MQTT_HANDLE_IS_NULL;
    }
    sysdep = up_handle->sysdep;

    for (i = 0; i < MQTT_UPLOAD_DEFAULT_SUBTOPIC_NUM; i++) {
        char *topic_src[] = {core_mqtt_get_product_key(up_handle->mqtt_handle), core_mqtt_get_device_name(up_handle->mqtt_handle)};
        char *topic = g_upload_sub_topic_map[i].topic_fmt;

        res = core_sprintf(sysdep, &topic, g_upload_sub_topic_map[i].topic_fmt, topic_src, sizeof(topic_src) / sizeof(char *),
                       MQTT_UPLOAD_MODULE_NAME);
        if (res != STATE_SUCCESS) {
            break;
        }

        res = aiot_mqtt_unsub(up_handle->mqtt_handle, topic);
        sysdep->core_sysdep_free(topic);
        if (res < 0) {
            break;
        }
    }
    return res;
}

static int32_t _mqtt_upload_resend_pub(mqtt_upload_handle_t *up_handle, upload_file_task_handle_t *up_task) {
    int32_t res = STATE_SUCCESS;
    if (up_handle == NULL) {
        return STATE_MQTT_UPLOAD_MQTT_HANDLE_IS_NULL;
    }
    // /* 未知长度上传时使用aiot_mqtt_upload_send_data接口,不处理重发逻辑 */
    if (up_task->send_last_time == 0 || up_task->file_size == -1 ||
            up_task->status == STATE_MQTT_UPLOAD_REQUEST_INIT) {
        return res;
    }
    core_log(up_handle->sysdep, STATE_MQTT_UPLOAD_FAILED_TIMEOUT, "Resend pub\r\n");
    up_task->failed_count++;
    if (up_task->failed_count >= up_handle->retry_max_value) {
        up_task->status = STATE_MQTT_UPLOAD_FAILED_TIMEOUT;
        up_task->failed_count = 0;
        return STATE_MQTT_UPLOAD_FAILED_TIMEOUT;
    }

    if (up_task->status == STATE_MQTT_UPLOAD_IS_UPLOADING && up_task->block_size != 0) {
        res = _mqtt_upload_send_block_file(up_handle, up_task, up_task->data, up_task->block_size, up_task->file_offset, 0);
    } else if (up_task->status == STATE_MQTT_UPLOAD_REQUEST_CANCEL) {
        res = _mqtt_upload_send_request_cancel(up_handle, up_task);
    }
    return res;
}

static int32_t _mqtt_upload_filelist_insert(mqtt_upload_handle_t *up_handle, upload_file_task_handle_t *up_task)
{
    upload_file_task_handle_t *node = NULL;
    core_list_for_each_entry(node, &up_handle->file_list, linked_node, upload_file_task_handle_t) {
        /* 文件名存在 */
        if (memcmp(node->file_name, up_task->file_name, strlen(node->file_name)) == 0) {
            return STATE_SUCCESS;
        }
    }
    node = up_task;
    CORE_INIT_LIST_HEAD(&node->linked_node);
    core_list_add_tail(&node->linked_node, &up_handle->file_list);
    return STATE_SUCCESS;
}

static void _mqtt_upload_filelist_destroy(mqtt_upload_handle_t *up_handle)
{
    upload_file_task_handle_t *node = NULL, *next = NULL;
    aiot_sysdep_portfile_t *sysdep = NULL;
    sysdep = up_handle->sysdep;
    core_list_for_each_entry_safe(node, next, &up_handle->file_list,
                                  linked_node, upload_file_task_handle_t) {
        while (!node->is_destory)
        {
            sysdep->core_sysdep_sleep(1);
        }
        core_list_del(&node->linked_node);
        _mqtt_upload_up_task_free(sysdep, node);
    }
}

static void _mqtt_upload_up_task_free(aiot_sysdep_portfile_t *sysdep, upload_file_task_handle_t *up_task) {
    if (sysdep == NULL || up_task == NULL) {
        return;
    }
    if (up_task->file_name != NULL) {
        sysdep->core_sysdep_free(up_task->file_name);
    }
    if (up_task->upload_id != NULL) {
        sysdep->core_sysdep_free(up_task->upload_id);
    }
    if (up_task->mode != NULL) {
        sysdep->core_sysdep_free(up_task->mode);
    }
    if (up_task->crc64 != NULL) {
        sysdep->core_sysdep_free(up_task->crc64);
    }
    if (up_task->uuid != NULL) {
        sysdep->core_sysdep_free(up_task->uuid);
    }
    if (up_task->data != NULL) {
        sysdep->core_sysdep_free(up_task->data);
    }
    _mqtt_upload_desc_free(sysdep, &up_task->packet);
    if (up_task != NULL) {
        sysdep->core_sysdep_free(up_task);
    }
}

void *aiot_mqtt_upload_init(void)
{
    mqtt_upload_handle_t *up_handle = NULL;
    aiot_sysdep_portfile_t *sysdep = NULL;

    sysdep = aiot_sysdep_get_portfile();
    if (sysdep == NULL) {
        return NULL;
    }

    up_handle = sysdep->core_sysdep_malloc(sizeof(mqtt_upload_handle_t), MQTT_UPLOAD_MODULE_NAME);
    if (up_handle == NULL) {
        return NULL;
    }
    memset(up_handle, 0, sizeof(mqtt_upload_handle_t));
    /* Init upload param. */
    up_handle->sysdep = sysdep;
    up_handle->rsp_timeout = MQTT_UPLOAD_DEFAULT_RSP_TIMEOUT;
    up_handle->retry_max_value = MQTT_UPLOAD_DEFAULT_RETRY_MAX_COUNT;
    up_handle->default_block_size = MQTT_UPLOAD_DEFAULT_BLOCK_SIZE;
    /* Init upload mutex. */
    up_handle->data_mutex = sysdep->core_sysdep_mutex_init();
    up_handle->send_mutex = sysdep->core_sysdep_mutex_init();
    up_handle->open_mutex = sysdep->core_sysdep_mutex_init();
    up_handle->uptask_mutex = sysdep->core_sysdep_mutex_init();
    CORE_INIT_LIST_HEAD(&up_handle->file_list);
    core_log(up_handle->sysdep, STATE_MQTT_UPLOAD_HANDLE_INIT, "aiot_mqtt_upload_init init\r\n");
    return (mqtt_upload_handle_t *)up_handle;
}

int32_t aiot_mqtt_upload_deinit(void **handle)
{
    int32_t res = STATE_SUCCESS;
    mqtt_upload_handle_t *up_handle = NULL;
    aiot_sysdep_portfile_t *sysdep = NULL;
    if (NULL == handle || NULL == *handle) {
        return STATE_MQTT_UPLOAD_MQTT_HANDLE_IS_NULL;
    }

    up_handle = * (mqtt_upload_handle_t **)handle;
    sysdep = up_handle->sysdep;
    core_log(sysdep, STATE_MQTT_UPLOAD_HANDLE_DEINIT, "aiot_mqtt_upload_deinit\r\n");

    sysdep->core_sysdep_mutex_lock(up_handle->uptask_mutex);
    _mqtt_upload_filelist_destroy(up_handle);
    sysdep->core_sysdep_mutex_unlock(up_handle->uptask_mutex);

    if (NULL != up_handle->mqtt_handle) {
        _mqtt_upload_unsubscribe((mqtt_upload_handle_t *)up_handle);

        core_mqtt_process_data_t process_data;
        memset(&process_data, 0, sizeof(core_mqtt_process_data_t));
        process_data.handler = _mqtt_upload_core_mqtt_process_handler;
        process_data.context = handle;
        core_mqtt_setopt(up_handle->mqtt_handle, CORE_MQTTOPT_REMOVE_PROCESS_HANDLER, &process_data);
    }

    /* Deinit upload mutex. */
    sysdep->core_sysdep_mutex_deinit(&up_handle->data_mutex);
    sysdep->core_sysdep_mutex_deinit(&up_handle->send_mutex);
    sysdep->core_sysdep_mutex_deinit(&up_handle->open_mutex);
    sysdep->core_sysdep_mutex_deinit(&up_handle->uptask_mutex);

    sysdep->core_sysdep_free(up_handle);
    *handle = NULL;
    return res;
}

void _upload_get_crc64_hexstr(void *digest, char *crc64_str) {
    uint64_t crc64 = 0;
    uint64_t tmp = 1;
    uint8_t crc64_temp[8] = {0};
    if (digest != NULL) {
        crc64 = *(uint64_t *)digest;
        /* 转成16进制字符串,忽略大小端 */
        tmp = 1;
        for(uint8_t i = 0; i < 8; i++)
        {
            crc64_temp[7 - i] = ((crc64 / tmp) % 256) & 0xFF;
            tmp *= 256;
        }
        core_hex2str(crc64_temp, 8, crc64_str, 1);
    }
}

int32_t aiot_mqtt_upload_setopt(void *handle, aiot_mqtt_upload_option_t option, void *data)
{
    int32_t res = STATE_SUCCESS;

    if (NULL == handle || NULL == data) {
        return STATE_USER_INPUT_NULL_POINTER;
    }

    if (option >= AIOT_MQTT_UPLOADOPT_MAX) {
        return STATE_USER_INPUT_UNKNOWN_OPTION;
    }

    mqtt_upload_handle_t *up_handle = (mqtt_upload_handle_t *)handle;
    aiot_sysdep_portfile_t *sysdep = up_handle->sysdep;
    sysdep->core_sysdep_mutex_lock(up_handle->data_mutex);
    switch (option) {
        case AIOT_MQTT_UPLOADOPT_RETRY_COUNT: {
            up_handle->retry_max_value = *(uint32_t *)data;
        }
        break;
        case AIOT_MQTT_UPLOADOPT_RSP_TIMEOUT_MS: {
            if (*(uint32_t *)data != 0) {
                up_handle->rsp_timeout = *(uint32_t *)data;
            }
        }
        break;
        case AIOT_MQTT_UPLOADOPT_MQTT_HANDLE: {
            up_handle->mqtt_handle = data;
            res = _mqtt_upload_subscribe(data, up_handle);
            if (res >= STATE_SUCCESS) {
                res = _mqtt_upload_mqtt_operate_process_handler(up_handle, CORE_MQTTOPT_APPEND_PROCESS_HANDLER);
            }
        }
        break;
        case AIOT_MQTT_UPLOADOPT_DEFAULLT_BLOCK_SIZE: {
            if (_mqtt_upload_block_size_is_valid(*(uint32_t *)data, 0)) {
                up_handle->default_block_size = *(uint32_t *)data;
            } else {
                res = STATE_MQTT_UPLOAD_BLOCK_SIZE_ERROR;
            }
        }
        break;
        case AIOT_MQTT_UPLOADOPT_FILE_OPTION : {

            aiot_mqtt_upload_file_opt_t file_opt = *(aiot_mqtt_upload_file_opt_t *)data;
            upload_file_task_handle_t *up_task = NULL;
            up_handle->sysdep->core_sysdep_mutex_lock(up_handle->uptask_mutex);
            core_list_for_each_entry(up_task, &up_handle->file_list, linked_node, upload_file_task_handle_t) {
                if (memcmp(file_opt.file_name, up_task->file_name, strlen(up_task->file_name)) == 0) {
                    up_handle->sysdep->core_sysdep_mutex_unlock(up_handle->uptask_mutex);
                    return res;
                }
            }

            /* 文件名不存在，发起新的请求 */
            up_task = up_handle->sysdep->core_sysdep_malloc(sizeof(upload_file_task_handle_t), MQTT_UPLOAD_MODULE_NAME);
            memset(up_task, 0, sizeof(upload_file_task_handle_t));
            char uuid[MQTT_UPLOAD_DEFAULT_UUID_SIZE] = {0};
            memset(uuid, 0, sizeof(uuid));
            _mqtt_upload_get_uuid(up_handle->sysdep, uuid);
            up_task->is_rsp = 0;
            char crc64_str[MQTT_UPLOAD_DEFAULT_CRC64_LEN] = {0};
            memset(crc64_str, 0, MQTT_UPLOAD_DEFAULT_CRC64_LEN);
            _upload_get_crc64_hexstr(file_opt.digest, crc64_str);
            _mqtt_upload_file_task_init(up_handle->sysdep, up_task, file_opt.file_name,
                file_opt.file_size, file_opt.mode, crc64_str, uuid, file_opt.read_data_handler,
                file_opt.userdata);
            _mqtt_upload_filelist_insert(up_handle, up_task);
            up_handle->sysdep->core_sysdep_mutex_unlock(up_handle->uptask_mutex);

        }
        break;
        default: {
            res = STATE_USER_INPUT_UNKNOWN_OPTION;
        }
        break;
    }
    sysdep->core_sysdep_mutex_unlock(up_handle->data_mutex);

    return res;
}

int32_t aiot_mqtt_upload_open_stream(void *handle, char *file_name, aiot_mqtt_upload_recv_t *packet) {
    mqtt_upload_handle_t *up_handle = (mqtt_upload_handle_t *)handle;
    int32_t res = STATE_SUCCESS;
    uint64_t now = 0;
    uint8_t file_flag = 0;

    if (NULL == up_handle) {
        return STATE_MQTT_UPLOAD_MQTT_HANDLE_IS_NULL;
    }

    if (up_handle->sysdep == NULL) {
        return STATE_MQTT_UPLOAD_MQTT_SYSDEP_IS_NULL;
    }

    if (file_name == NULL) {
        return STATE_MQTT_UPLOAD_PARAMS_IS_NULL;
    }
    aiot_sysdep_portfile_t *sysdep = up_handle->sysdep;
    file_flag = 0;
    sysdep->core_sysdep_mutex_lock(up_handle->open_mutex);
    sysdep->core_sysdep_mutex_lock(up_handle->uptask_mutex);
    upload_file_task_handle_t *up_task = NULL;
    core_list_for_each_entry(up_task, &up_handle->file_list, linked_node, upload_file_task_handle_t) {
        if (memcmp(file_name, up_task->file_name, strlen(up_task->file_name)) == 0) {
            /* 请求文件信息已经存在，重新请求 */
            up_task->is_rsp = 0;
            if ((res = _mqtt_upload_send_request_init(up_handle, up_task, up_task->file_name,
                up_task->file_size, up_task->mode, up_task->crc64, up_task->uuid)) == STATE_SUCCESS) {
                up_task->failed_count = 0;
                up_task->is_destory = 0;
            }
            file_flag = 1;
            break;
        }
    }
    sysdep->core_sysdep_mutex_unlock(up_handle->uptask_mutex);

    if (file_flag != 1) {
        sysdep->core_sysdep_mutex_unlock(up_handle->open_mutex);
        return STATE_MQTT_UPLOAD_UPTASK_IS_NULL;
    }

    if (res != STATE_SUCCESS) {
        up_task->is_destory = 1;
        sysdep->core_sysdep_mutex_unlock(up_handle->open_mutex);
        return res;
    }

    now = sysdep->core_sysdep_time();
    while (up_task->is_rsp == 0) {
        if((sysdep->core_sysdep_time() - now) > up_handle->rsp_timeout) {
            core_log(sysdep, STATE_MQTT_UPLOAD_FAILED_TIMEOUT, "Wait init response timeout.\r\n");
            break;
        }
        sysdep->core_sysdep_sleep(MQTT_UPLOAD_SYNC_API_SLEEP_MS);
    }

    if (up_task->is_rsp == 0) {
        res = STATE_MQTT_UPLOAD_FAILED_TIMEOUT;
        goto exit;
    }
    if (up_task->rsp_code != UPLOAD_FILE_OK) {
        res = STATE_MQTT_UPLOAD_FAILED;
        goto exit;
    }
    if (packet != NULL) {
        _mqtt_upload_desc_copy(sysdep, &up_task->packet, packet);
    }
exit:
    up_task->is_destory = 1;
    sysdep->core_sysdep_mutex_unlock(up_handle->open_mutex);
    return res;
}

int32_t aiot_mqtt_upload_cancel_stream(void *handle, char *file_name) {
    mqtt_upload_handle_t *up_handle = (mqtt_upload_handle_t *)handle;
    int32_t res = STATE_MQTT_UPLOAD_NO_UPLOAD_TASK;
    if (NULL == up_handle || up_handle->sysdep == NULL) {

        return STATE_MQTT_UPLOAD_MQTT_HANDLE_IS_NULL;
    }

    if (file_name == NULL) {
        return STATE_MQTT_UPLOAD_FILENAME_IS_NULL;
    }
    aiot_sysdep_portfile_t *sysdep = up_handle->sysdep;
    sysdep->core_sysdep_mutex_lock(up_handle->uptask_mutex);
    upload_file_task_handle_t *up_task = NULL, *next = NULL;
    core_list_for_each_entry_safe(up_task, next, &up_handle->file_list, linked_node, upload_file_task_handle_t) {
        if (memcmp(file_name, up_task->file_name, strlen(up_task->file_name)) == 0) {
            if ( up_task->status == STATE_MQTT_UPLOAD_NONE ||
                    up_task->status == STATE_MQTT_UPLOAD_CANCEL_SUCCESS) {
                /* 未收到初始化消息,删除upload stask资源 */
                core_list_del(&up_task->linked_node);
                _mqtt_upload_up_task_free(sysdep, up_task);
                res = STATE_MQTT_UPLOAD_CANCEL_SUCCESS;
                break;
            } else if (up_task->status == STATE_MQTT_UPLOAD_FINISHED) {
                res = STATE_MQTT_UPLOAD_CANCEL_FAILED;
                break;
            }
            /* 发送取消文件上传的请求 */
            if ((res = _mqtt_upload_send_request_cancel(handle, up_task)) == STATE_SUCCESS) {
                up_task->status = STATE_MQTT_UPLOAD_REQUEST_CANCEL;
            }
            break;
        }
    }
    sysdep->core_sysdep_mutex_unlock(up_handle->uptask_mutex);
    return res;
}

int32_t aiot_mqtt_upload_send_data(void *handle, char *file_name,
            uint8_t *data, uint32_t datalen, uint8_t is_complete) {

    int32_t res = STATE_SUCCESS;
    uint8_t file_flag = 0;
    mqtt_upload_handle_t *up_handle = (mqtt_upload_handle_t *)handle;
    aiot_sysdep_portfile_t *sysdep = up_handle->sysdep;
    sysdep->core_sysdep_mutex_lock(up_handle->send_mutex);
    sysdep->core_sysdep_mutex_lock(up_handle->uptask_mutex);
    static upload_file_task_handle_t *up_task = NULL, *next = NULL;
    core_list_for_each_entry_safe(up_task, next, &up_handle->file_list, linked_node, upload_file_task_handle_t) {

        if (file_name != NULL && up_task->file_name != NULL &&
                memcmp(file_name, up_task->file_name, strlen(up_task->file_name)) == 0) {
            up_task->block_size = datalen;
            file_flag = 1;
            up_task->is_destory = 0;
            break;
        }
    }
    sysdep->core_sysdep_mutex_unlock(up_handle->uptask_mutex);
    /* 没有查询到对应的upload id */
    if (file_flag != 1) {
        sysdep->core_sysdep_mutex_unlock(up_handle->send_mutex);
        return STATE_MQTT_UPLOAD_FAILED;
    }
    if (up_task != NULL && strlen(up_task->upload_id) == 0) {
        up_task->is_destory = 1;
        sysdep->core_sysdep_mutex_unlock(up_handle->send_mutex);
        return STATE_MQTT_UPLOAD_FAILED;
    }


    up_task->is_rsp = 0;
    up_task->rsp_code = 0;
    res = _mqtt_upload_send_block_file(up_handle, up_task, data, datalen, up_task->file_offset, is_complete);

    // 等待应答包
    uint64_t now = sysdep->core_sysdep_time();
    while (res == STATE_SUCCESS && up_task->is_rsp == 0) {
        if((sysdep->core_sysdep_time() - now) > up_handle->rsp_timeout) {
            /* 应答超时 */
            break;
        }
        sysdep->core_sysdep_sleep(MQTT_UPLOAD_SYNC_API_SLEEP_MS);
    }

    if (up_task->is_rsp == 0) {
        res = STATE_MQTT_UPLOAD_FAILED_TIMEOUT;
        goto exit;
    }

    if (up_task->rsp_code != UPLOAD_FILE_OK) {
        res = STATE_MQTT_UPLOAD_FAILED;
        goto exit;
    }
exit:
    up_task->is_destory = 1;
    sysdep->core_sysdep_mutex_unlock(up_handle->send_mutex);
    return res;
}

aiot_mqtt_upload_result_t aiot_mqtt_upload_process(void *handle) {
    mqtt_upload_handle_t *up_handle = (mqtt_upload_handle_t *)handle;
    uint16_t file_name_len = 0;
    uint64_t now = 0;
    static aiot_mqtt_upload_result_t result = {
        .status = STATE_MQTT_UPLOAD_NONE,
        .err_code = 0
    };
    result.status = STATE_MQTT_UPLOAD_NONE;
    result.err_code = 0;
    memset(result.uploadid, 0, MQTT_UPLOAD_DEFAULT_UPLOAD_SIZE);
    memset(result.file_name, 0, MQTT_UPLOAD_DEFAULT_FILENAME_LEN);
    if (up_handle == NULL) {
        result.status = STATE_MQTT_UPLOAD_MQTT_HANDLE_IS_NULL;
        return result;
    }
    aiot_sysdep_portfile_t *sysdep = up_handle->sysdep;
    sysdep->core_sysdep_mutex_lock(up_handle->uptask_mutex);
    upload_file_task_handle_t *up_task = NULL, *next = NULL;
    core_list_for_each_entry_safe(up_task, next, &up_handle->file_list, linked_node, upload_file_task_handle_t) {

        result.status = up_task->status;
        file_name_len = strlen(up_task->file_name) > MQTT_UPLOAD_DEFAULT_FILENAME_LEN ?
                MQTT_UPLOAD_DEFAULT_FILENAME_LEN : strlen(up_task->file_name);
        memset(result.file_name, 0, MQTT_UPLOAD_DEFAULT_FILENAME_LEN);
        memset(result.uploadid, 0, sizeof(result.uploadid));

        memcpy(result.file_name, up_task->file_name, file_name_len);
        memcpy(result.uploadid, up_task->upload_id, strlen(up_task->upload_id));
        switch (up_task->status)
        {
            case STATE_MQTT_UPLOAD_NONE:
                /* 未开始上传任务 Nothing Todo */
                break;
            case STATE_MQTT_UPLOAD_REQUEST_INIT:
                /* 请求上传UploadID */
                now = sysdep->core_sysdep_time();

                if((now - up_task->send_last_time) > up_handle->rsp_timeout) {
                    _mqtt_upload_resend_pub(up_handle, up_task);
                }
                break;
            case STATE_MQTT_UPLOAD_IS_UPLOADING:
                /* 上传中 */
                now = sysdep->core_sysdep_time();
                if((now - up_task->send_last_time) > up_handle->rsp_timeout) {
                    _mqtt_upload_resend_pub(up_handle, up_task);
                }
                break;
            case STATE_MQTT_UPLOAD_REQUEST_CANCEL:
                /* 请求取消 */
                now = sysdep->core_sysdep_time();
                if((now - up_task->send_last_time) > up_handle->rsp_timeout) {
                    _mqtt_upload_resend_pub(up_handle, up_task);
                }
                break;
            case STATE_MQTT_UPLOAD_FINISHED: /* 上传完成 */
            case STATE_MQTT_UPLOAD_CANCEL_SUCCESS: /* 取消文件上传成功 */

                up_task->status = STATE_MQTT_UPLOAD_NONE;
                result.err_code = up_task->rsp_code;
                /* 结束上传，销毁对应的链表内存 */
                core_log(sysdep, STATE_MQTT_UPLOAD_BASE, "Finish upload,Deleted upload task.\r\n");
                while (!up_task->is_destory)
                {
                    sysdep->core_sysdep_sleep(1);
                }

                core_list_del(&up_task->linked_node);
                _mqtt_upload_up_task_free(sysdep, up_task);
                sysdep->core_sysdep_mutex_unlock(up_handle->uptask_mutex);
                return result;
            case STATE_MQTT_UPLOAD_CANCEL_FAILED: /* 取消文件上传失败 */
            case STATE_MQTT_UPLOAD_FAILED:   /* 上传失败 */
            case STATE_MQTT_UPLOAD_FAILED_TIMEOUT: /* 发送超时 */
            case STATE_MQTT_UPLOAD_FAILED_WHOLE_CHECK:    /* CRC64校验失败 */
                /* 回到空闲模式 */
                up_task->status = STATE_MQTT_UPLOAD_NONE;
                result.err_code = up_task->rsp_code;
                while (!up_task->is_destory)
                {
                    sysdep->core_sysdep_sleep(1);
                }
                core_list_del(&up_task->linked_node);
                _mqtt_upload_up_task_free(sysdep, up_task);
                sysdep->core_sysdep_mutex_unlock(up_handle->uptask_mutex);
                return result;
            default:
                break;
        }
    }
    sysdep->core_sysdep_mutex_unlock(up_handle->uptask_mutex);
    return result;
}

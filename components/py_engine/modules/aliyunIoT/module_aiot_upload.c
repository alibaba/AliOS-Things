/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "aiot_state_api.h"
#include "aiot_sysdep_api.h"
#include "aiot_dynreg_api.h"
#include "aiot_mqtt_api.h"
#include "module_aiot.h"

#include "aiot_state_api.h"
#include "aiot_sysdep_api.h"
#include "aiot_mqtt_api.h"

#include "aiot_mqtt_upload_api.h"
#include "upload_crc64.h"
#define MOD_STR "AIOT_UPLOAD"

#define MQTT_UPLOAD_RSP_TIMEOUT 2000
#define MQTT_UPLOAD_RETY_COUNT 5
#ifdef M5STACK_CORE2
#define MQTT_UPLOAD_BLOCK_SIZE (10 * 1024)
#else
#define MQTT_UPLOAD_BLOCK_SIZE (2 * 1024)
#endif

/* 位于portfiles/aiot_port文件夹下的系统适配函数集合 */
extern aiot_sysdep_portfile_t g_aiot_sysdep_portfile;

/* 位于external/ali_ca_cert.c中的服务器证书 */
extern const char *ali_ca_cert;
static char *g_data = NULL;
static int32_t g_data_len = 0;
static char *g_file_name = NULL;

static char *get_basename(char *path)
{
    char *s1;
    char *s2;

    s1 = strrchr(path, '/');
    s2 = strrchr(path, '\\');

    if (s1 && s2)
        path = (s1 > s2 ? s1 : s2) + 1;
    else if (s1)
        path = s1 + 1;
    else if (s2)
        path = s2 + 1;

    return path;
}

static uint32_t get_file_size(char *file_name)
{
    uint32_t total_size = 0;

    if (file_name == NULL)
        return -1;

    FILE *fp = fopen(file_name, "r");
    if (fp == NULL) {
        amp_error(MOD_STR, "open File:%s failed \r\n", file_name);
        return -1;
    }

    fseek(fp, 0, SEEK_END);
    total_size  = ftell(fp);

    fclose(fp);
    return total_size;
}

static uint64_t get_file_crc64(char *file_name, char *buf, int32_t buf_len)
{
    uint64_t crc = 0;
    uint32_t read_default_size = 2048;
    uint32_t read_len = 0;
    uint8_t data[2048] = {0};
    uint32_t read_offset = 0;

    if (data != NULL) {
        crc = upload_crc64_update(crc, buf, buf_len);
        return crc;
    }
    FILE *fp = fopen(file_name, "r");
    fseek(fp, 0, SEEK_END);
    uint32_t total_size  = ftell(fp);

    for (read_offset = 0; total_size != read_offset;) {
        fseek(fp, read_offset, SEEK_SET);
        read_len = fread(data, sizeof(uint8_t), read_default_size, fp);
        crc = upload_crc64_update(crc, data, read_len);
        read_offset += read_len;
    }

    fclose(fp);
    return crc;
}

static int32_t aiot_read_data_handler(const aiot_mqtt_upload_recv_t *packet, uint8_t *data, uint32_t size, void *userdata)
{
    int32_t read_len = 0;
    if (userdata != NULL) {
        uint32_t *test_userdata = (uint32_t *)userdata;
        amp_debug(MOD_STR, "test_userdata:%d\r\n", *test_userdata);
    }
    if (packet == NULL) {
        return 0;
    }

    if (packet->desc.code == UPLOAD_FILE_OK) {
        if (data != NULL && size != 0) {
            uint32_t read_size = size;
            FILE *fp;
            char *file_name = packet->desc.file_name;

            if (g_data != NULL) {
                uint32_t offset = packet->desc.file_offset;
                if (read_size > g_data_len - offset)
                    read_len = g_data_len - offset;
                else
                    read_len = read_size;
                memcpy(data, g_data + offset, read_len);
            } else {
                fp = fopen(g_file_name, "r");
                uint32_t offset = packet->desc.file_offset;
                fseek(fp, offset, SEEK_SET);
                read_len = fread(data, sizeof(uint8_t), read_size, fp);
                amp_debug(MOD_STR, "Open %s read at: %d,Read_len: %d \r\n", file_name, offset, read_len);
                fclose(fp);
            }
        }
    } else {
        amp_error(MOD_STR, "Error code:%d, message:%s\r\n", packet->desc.code, packet->desc.message);
    }

    return read_len;
}

char *pyamp_aiot_upload_mqtt(void *mqtt_handle, char *file_name, char *data, int32_t data_len, mp_obj_t cb)
{
    uint32_t file_len = 0;
    char *base_file_name = NULL;
    // MQTT Upload File Init.
    void *up_handle = aiot_mqtt_upload_init();
    aiot_mqtt_upload_setopt(up_handle, AIOT_MQTT_UPLOADOPT_MQTT_HANDLE, mqtt_handle);
    /* 如果网络速率比较慢，可以将延迟时间调整的更大 */
    uint32_t rsp_timeout = MQTT_UPLOAD_RSP_TIMEOUT;
    aiot_mqtt_upload_setopt(up_handle, AIOT_MQTT_UPLOADOPT_RSP_TIMEOUT_MS, &rsp_timeout);
    uint32_t rety_count = MQTT_UPLOAD_RETY_COUNT;
    aiot_mqtt_upload_setopt(up_handle, AIOT_MQTT_UPLOADOPT_RETRY_COUNT, &rety_count);
    /* 如果文件较大时可以调整block_size的大小 */
    uint32_t block_size = MQTT_UPLOAD_BLOCK_SIZE;
    aiot_mqtt_upload_setopt(up_handle, AIOT_MQTT_UPLOADOPT_DEFAULLT_BLOCK_SIZE, &block_size);

    uint32_t timeout_ms = 100;
    aiot_mqtt_setopt(mqtt_handle, AIOT_MQTTOPT_RECV_TIMEOUT_MS, (void *)&timeout_ms);

    if (file_name != NULL) {
        if (data != NULL) {
            g_data = data;
            g_data_len = data_len;
            file_len = data_len;
        } else {
            g_data = NULL;
            g_data_len = 0;
            g_file_name = file_name;
            file_len = get_file_size(file_name);
            if (file_len < 0)
                goto exit;
        }
        base_file_name = get_basename(file_name);

    #ifdef UPlOAD_WITH_CRC
        /* 配置上传文件的信息和回调，调用主动发送接口时，不需要配置回调，如果配置了回调参数SDK默认优先使用回调*/
        uint32_t test_userdata = 100;
        uint64_t crc = get_file_crc64(file_name);
        aiot_mqtt_upload_file_opt_t file_option = {
            .file_name = base_file_name,
            .file_size = file_len,
            .mode = AIOT_MQTT_UPLOAD_FILE_MODE_OVERWRITE,
            .digest = &crc,
            .read_data_handler = aiot_read_data_handler,
            .userdata = &test_userdata
        };
        amp_debug(MOD_STR, "aiot_mqtt_upload with crc %d\n", crc);
    #else
        /* 无crc64校验，无userdata传参 */
        aiot_mqtt_upload_file_opt_t file_option = {
            .file_name = base_file_name,
            .file_size = file_len,
            .mode = AIOT_MQTT_UPLOAD_FILE_MODE_OVERWRITE,
            .digest = NULL,
            .read_data_handler = aiot_read_data_handler,
            .userdata = NULL
        };
        amp_debug(MOD_STR, "aiot_mqtt_upload without crc\n");
    #endif
        aiot_mqtt_upload_setopt(up_handle, AIOT_MQTT_UPLOADOPT_FILE_OPTION, &file_option);
        /* 请求打开上传通道 */
        aiot_mqtt_upload_open_stream(up_handle, base_file_name, NULL);
    } else {
        amp_error(MOD_STR, "filename should not be null,please check it\n");
        goto exit;
    }
    static aiot_mqtt_upload_result_t result;
    while (1) {
        result = aiot_mqtt_upload_process(up_handle);
        if (result.status == STATE_MQTT_UPLOAD_FINISHED) {
            /* 上传成功 */
            amp_debug(MOD_STR, "MQTT Upload file(%s) ID(%s) success\r\n", result.file_name, result.uploadid);
            break;
        } else if (result.status == STATE_MQTT_UPLOAD_FAILED ||
                   result.status == STATE_MQTT_UPLOAD_FAILED_TIMEOUT ||
                   result.status == STATE_MQTT_UPLOAD_CANCEL_FAILED) {
            /* 上传失败 */
            amp_error(MOD_STR, "MQTT Upload file(%s) failed,res:-0x%.4X\r\n", result.file_name, -result.status);
            // printf("MQTT Upload file(%s) failed,res:-0x%.4X\r\n", result.file_name, -result.status);
            break;
            /* 销毁MQTT UPLOAD实例 */
        } else if (result.status == STATE_MQTT_UPLOAD_CANCEL_SUCCESS) {
            amp_error(MOD_STR, "MQTT Upload file(%s) cancel success,res:-0x%.4X\r\n", result.file_name, -result.status);
            break;
        } else if (result.status == STATE_MQTT_UPLOAD_FAILED_WHOLE_CHECK) {
            /* 云端校验文件的完整性失败 */
            amp_error(MOD_STR, "MQTT Upload file(%s) whole file md5 failed,res:-0x%.4X\r\n", result.file_name, -result.status);
            break;
        }
    }
exit:
    /* 销毁MQTT实例*/
    aiot_mqtt_upload_deinit(&up_handle);

    if (result.status == STATE_MQTT_UPLOAD_FINISHED) {
        return &result.uploadid;
    } else {
        return NULL;
    }
}

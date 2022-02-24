/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <string.h>

#if MICROPY_PY_HTTP

#include "esp_event.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hapy_http_client.h"
#include "nvs_flash.h"
#include "utility.h"

#define LOG_TAG "HTTP_CLIENT"

static int8_t resp_header_buffer[HTTP_HEADER_SIZE];
static int8_t *resp_body_buffer = NULL;

static hapy_http_param_t http_param_download = { 0 };
static hapy_http_param_t http_param_request = { 0 };

extern const uint8_t server_cert_pem_start[] asm("_binary_ca_cert_pem_start");
extern const uint8_t server_cert_pem_end[] asm("_binary_ca_cert_pem_end");

static char *_dirname(const char *path)
{
    const char *last_slash = strrchr(path, '/');

    if (last_slash == NULL) {
        ESP_LOGE(LOG_TAG, "_dirname: no slash in given path:%s", path);
        return NULL;
    }

    char *ret = malloc(last_slash + 1 - path);
    if (ret == NULL) {
        ESP_LOGE(LOG_TAG, "_dirname: no memory");
        return NULL;
    }
    memcpy(ret, path, last_slash - path);
    ret[last_slash - path] = '\0';
    return ret;
}

/*
 * https://gist.github.com/ChisholmKyle/0cbedcd3e64132243a39
 */
/* recursive mkdir based on
http://nion.modprobe.de/blog/archives/357-Recursive-directory-creation.html
*/
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define PATH_MAX_STRING_SIZE 256

/* recursive mkdir */
static int mkdir_p(const char *dir, const mode_t mode)
{
    char tmp[PATH_MAX_STRING_SIZE];
    char *p = NULL;
    struct stat sb;
    size_t len;

    /* copy path */
    len = strnlen(dir, PATH_MAX_STRING_SIZE);
    if (len == 0 || len == PATH_MAX_STRING_SIZE) {
        return -1;
    }
    memcpy(tmp, dir, len);
    tmp[len] = '\0';

    /* remove trailing slash */
    if (tmp[len - 1] == '/') {
        tmp[len - 1] = '\0';
    }

    /* check if path exists and is a directory */
    if (stat(tmp, &sb) == 0) {
        if (S_ISDIR(sb.st_mode)) {
            return 0;
        }
    }

    /* recursive mkdir */
    for (p = tmp + 1; *p; p++) {
        if (*p == '/') {
            *p = 0;
            /* test path */
            if (stat(tmp, &sb) != 0) {
                /* path does not exist - create directory */
                if (mkdir(tmp, mode) < 0) {
                    return -1;
                }
            } else if (!S_ISDIR(sb.st_mode)) {
                /* not a directory */
                return -1;
            }
            *p = '/';
        }
    }
    /* test path */
    if (stat(tmp, &sb) != 0) {
        /* path does not exist - create directory */
        if (mkdir(tmp, mode) < 0) {
            return -1;
        }
    } else if (!S_ISDIR(sb.st_mode)) {
        /* not a directory */
        return -1;
    }
    return 0;
}

static int32_t hapy_http_construct_header(char *buf, uint16_t buf_size, const char *name, const char *data)
{
    uint32_t hdr_len;
    uint32_t hdr_data_len;
    int32_t hdr_length;

    if (buf == NULL || buf_size == 0 || name == NULL || data == NULL) {
        ESP_LOGE(LOG_TAG, "hapy_http_construct_header args error !!!!\n");
        return 0;
    }

    hdr_len = strlen(name);
    hdr_data_len = strlen(data);
    hdr_length = hdr_len + hdr_data_len + 4;

    if (hdr_length < 0 || hdr_length > buf_size) {
        ESP_LOGE(LOG_TAG, "hapy_http_construct_header hdr_length error, hdr_length[%d], buf_size[%d]  !!!!\n",
                 hdr_length, buf_size);
        return 0;
    }

    memcpy(buf, name, hdr_len);
    buf += hdr_len;
    memcpy(buf, ": ", 2);
    buf += 2;
    memcpy(buf, data, hdr_data_len);
    buf += hdr_data_len;
    memcpy(buf, HTTP_CRLF, 2);

    return hdr_length;
}

static void http_request_notify(hapy_http_param_t *http_param_request, int8_t *header_buf, int8_t *body_buf)
{
    char dummy_buf[2] = { 0 };
    mp_sched_carg_t *carg = make_cargs(MP_SCHED_CTYPE_DICT);

    if (header_buf != NULL) {
        make_carg_entry(carg, 0, MP_SCHED_ENTRY_TYPE_STR, strlen(header_buf), header_buf, "header");
    } else {
        make_carg_entry(carg, 0, MP_SCHED_ENTRY_TYPE_STR, strlen(dummy_buf), dummy_buf, "header");
    }

    if (body_buf != NULL) {
        make_carg_entry(carg, 1, MP_SCHED_ENTRY_TYPE_STR, strlen(body_buf), body_buf, "body");
    } else {
        make_carg_entry(carg, 1, MP_SCHED_ENTRY_TYPE_STR, strlen(dummy_buf), dummy_buf, "body");
    }

    callback_to_python_LoBo(http_param_request->cb, mp_const_none, carg);
}

static void http_download_notify(hapy_http_param_t *http_param_download, http_dl_status ret)
{
    callback_to_python_LoBo(http_param_download->cb, MP_OBJ_NEW_SMALL_INT(ret), NULL);
}

static esp_err_t _http_request_event_handler(esp_http_client_event_t *evt)
{
    static int32_t body_len = 0;
    static int32_t header_len = 0;

    switch (evt->event_id) {
    case HTTP_EVENT_ERROR:
        ESP_LOGE(LOG_TAG, "HTTP_EVENT_ERROR");
        break;
    case HTTP_EVENT_ON_CONNECTED:
        ESP_LOGD(LOG_TAG, "HTTP_EVENT_ON_CONNECTED");
        break;
    case HTTP_EVENT_HEADER_SENT:
        ESP_LOGD(LOG_TAG, "HTTP_EVENT_HEADER_SENT");
        break;
    case HTTP_EVENT_ON_HEADER:
        ESP_LOGD(LOG_TAG, "HTTP_EVENT_ON_HEADER[%d], key=%s, value=%s", header_len, evt->header_key, evt->header_value);

        header_len += hapy_http_construct_header(resp_header_buffer + header_len, HTTP_HEADER_SIZE - header_len,
                                                 evt->header_key, evt->header_value);
        break;
    case HTTP_EVENT_ON_DATA:
        ESP_LOGD(LOG_TAG, "HTTP_EVENT_ON_DATA, body_len=%d, data_len=%d", body_len, evt->data_len);

        /*
         *  Check for chunked encoding is added as the URL for chunked encoding used
         * in this example returns binary data. However, event handler can also be
         * used in case chunked encoding is used.
         */
        if (!esp_http_client_is_chunked_response(evt->client)) {
            ESP_LOGD(LOG_TAG, "HTTP_EVENT_ON_DATA, len = %d", esp_http_client_get_content_length(evt->client));
            if (resp_body_buffer == NULL) {
                resp_body_buffer = (char *)malloc(esp_http_client_get_content_length(evt->client) + 1);
                body_len = 0;
                if (resp_body_buffer == NULL) {
                    ESP_LOGE(LOG_TAG, "Failed to allocate memory for output buffer");
                    return ESP_FAIL;
                }
            }
            memcpy(resp_body_buffer + body_len, evt->data, evt->data_len);
            body_len += evt->data_len;
        }
        break;
    case HTTP_EVENT_ON_FINISH:
        ESP_LOGD(LOG_TAG, "HTTP_EVENT_ON_FINISH");

        // Add end for resp_header_buffer and resp_body_buffer
        resp_header_buffer[header_len] = 0;
        if (resp_body_buffer != NULL) {
            resp_body_buffer[body_len] = 0;
        }

        header_len = 0;
        body_len = 0;
        break;
    case HTTP_EVENT_DISCONNECTED:
        ESP_LOGD(LOG_TAG, "HTTP_EVENT_DISCONNECTED");
        int32_t mbedtls_err = 0;
        esp_err_t err = esp_tls_get_and_clear_last_error(evt->data, &mbedtls_err, NULL);
        if (err != 0) {
            ESP_LOGE(LOG_TAG, "Last esp error code: 0x%x", err);
            ESP_LOGE(LOG_TAG, "Last mbedtls failure: 0x%x", mbedtls_err);
        }
        header_len = 0;
        body_len = 0;
        break;
    }
    return ESP_OK;
}

static esp_err_t _http_download_event_handler(esp_http_client_event_t *evt)
{
    switch (evt->event_id) {
    case HTTP_EVENT_ERROR:
        ESP_LOGE(LOG_TAG, "HTTP_EVENT_ERROR");
        break;
    case HTTP_EVENT_ON_CONNECTED:
        ESP_LOGD(LOG_TAG, "HTTP_EVENT_ON_CONNECTED");
        break;
    case HTTP_EVENT_HEADER_SENT:
        ESP_LOGD(LOG_TAG, "HTTP_EVENT_HEADER_SENT");
        break;
    case HTTP_EVENT_ON_FINISH:
        ESP_LOGD(LOG_TAG, "HTTP_EVENT_ON_FINISH");
        break;
    case HTTP_EVENT_DISCONNECTED:
        ESP_LOGD(LOG_TAG, "HTTP_EVENT_DISCONNECTED");
        break;
    default:
        ESP_LOGD(LOG_TAG, "HTTP_EVENT_DEFAULT");
    }
    return ESP_OK;
}

/* create task for http download */
static void task_http_download_func(void *arg)
{
    http_dl_status ret = HTTP_DL_STATUS_FAIL;

    if (http_param_download.filepath == NULL) {
        ESP_LOGE(LOG_TAG, "http download filepath NULL");
        ret = HTTP_DL_STATUS_PATH_NULL;
        goto finish;
    }

    /* Check folder and create if not exist */
    const char *path = _dirname(http_param_download.filepath);
    struct stat stat_i = { 0 };
    if (stat(path, &stat_i) < 0) {
        ESP_LOGI(LOG_TAG, "path %s not exist, create recursive", path);
        mkdir_p(path, 0755);  // 0755 表示8进制的755，0不能删除
    }
    free(path);

    int fd = open(http_param_download.filepath, O_CREAT | O_RDWR | O_TRUNC);
    if (fd < 0) {
        ESP_LOGE(LOG_TAG, "failed to open file[%s], fd = %d\n", http_param_download.filepath, fd);
        ret = HTTP_DL_STATUS_FAIL;
        goto finish;
    }

    esp_http_client_config_t config = { 0 };
    config.url = http_param_download.url;
    config.event_handler = _http_download_event_handler;

    esp_http_client_handle_t client = esp_http_client_init(&config);
    if (client == NULL) {
        ESP_LOGE(LOG_TAG, "Failed to initialise HTTP connection");
        goto fail;
    }

    esp_err_t err = esp_http_client_open(client, 0);
    if (err != ESP_OK) {
        ESP_LOGE(LOG_TAG, "Failed to open HTTP connection: %s", esp_err_to_name(err));
        goto fail;
    }

    uint8_t buffer[MAX_HTTP_RECV_BUFFER + 1] = { 0 };
    int32_t content_length = esp_http_client_fetch_headers(client);
    ESP_LOGD(LOG_TAG, "HTTP Stream reader Status = %d, content_length = %d", esp_http_client_get_status_code(client),
             esp_http_client_get_content_length(client));

    int32_t total_read_len = 0;
    int32_t read_len = 0;
    while (total_read_len < content_length) {
        read_len = esp_http_client_read(client, buffer, MAX_HTTP_RECV_BUFFER);
        if (read_len < 0) {
            ESP_LOGE(LOG_TAG, "Error read data, read_len = %d", read_len);
            break;
        } else if (read_len == 0) {
            ESP_LOGD(LOG_TAG, "read finished...");
            break;
        }

        buffer[read_len] = 0;
        total_read_len += read_len;

        ESP_LOGD(LOG_TAG, "read_len = %d, cb=%p", read_len, http_param_download.cb);

        /* write data to file */
        int num = write(fd, buffer, read_len);
        if (num < 0) {
            goto fail;
        }
    }

    ret = (esp_http_client_get_status_code(client) == 200) ? HTTP_DL_STATUS_OK : HTTP_DL_STATUS_FAIL;

fail:
    if (client != NULL) {
        esp_http_client_close(client);
        esp_http_client_cleanup(client);
    }

    if (fd >= 0) {
        close(fd);
        fd = -1;
    }

finish:
    http_download_notify(&http_param_download, ret);
    vTaskDelete(NULL);
}

/* create task for http request */
static void task_http_request_func(void *arg)
{
    /**
     * NOTE: All the configuration parameters for http_client must be spefied
     * either in URL or as host and path parameters. If host and path parameters
     * are not set, query parameter will be ignored. In such cases, query
     * parameter should be specified in URL.
     *
     * If URL as well as host and path parameters are specified, values of host
     * and path will be considered.
     */
    resp_body_buffer = NULL;
    memset(resp_header_buffer, 0, HTTP_HEADER_SIZE);
    esp_http_client_config_t config = { 0 };
    config.url = http_param_request.url;
    config.query = "HaaS_Python";
    config.event_handler = _http_request_event_handler;
    config.timeout_ms = http_param_request.timeout;

    esp_http_client_handle_t client = esp_http_client_init(&config);

    for (int32_t i = 0; i < http_param_request.header_size; i++) {
        esp_http_client_set_header(client, http_param_request.http_header[i].name,
                                   http_param_request.http_header[i].data);
    }

    switch (http_param_request.method) {
    case HTTP_GET:
        {
            esp_http_client_set_method(client, HTTP_METHOD_GET);
            break;
        }
    case HTTP_POST:
        {
            esp_http_client_set_method(client, HTTP_METHOD_POST);
            esp_http_client_set_post_field(client, http_param_request.params, strlen(http_param_request.params));
            break;
        }
    case HTTP_PUT:
        {
            esp_http_client_set_method(client, HTTP_METHOD_PUT);
            break;
        }
    case HTTP_PATCH:
        {
            esp_http_client_set_method(client, HTTP_METHOD_PATCH);
            esp_http_client_set_post_field(client, NULL, 0);
            break;
        }
    case HTTP_DELETE:
        {
            esp_http_client_set_method(client, HTTP_METHOD_DELETE);
            break;
        }
    case HTTP_HEAD:
        {
            esp_http_client_set_method(client, HTTP_METHOD_HEAD);
            break;
        }
    }

    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGD(LOG_TAG, "HTTP POST Status = %d, content_length = %d", esp_http_client_get_status_code(client),
                 esp_http_client_get_content_length(client));
    } else {
        printf("HTTP POST request failed: %s\r\n", esp_err_to_name(err));
    }

    if (client != NULL) {
        esp_http_client_close(client);
        esp_http_client_cleanup(client);
    }

    // notify to python app
    http_request_notify(&http_param_request, resp_header_buffer, resp_body_buffer);
    if (resp_body_buffer != NULL) {
        free(resp_body_buffer);
        resp_body_buffer = NULL;
    }

    ESP_LOGD(LOG_TAG, "Finish http request");
    vTaskDelete(NULL);
}

STATIC mp_obj_t http_download(mp_obj_t data, mp_obj_t callback)
{
    const int8_t *url = NULL;
    const int8_t *filepath = NULL;

    memset(&http_param_download, 0, sizeof(hapy_http_param_t));

    /* get http download url */
    mp_obj_t index = mp_obj_new_str_via_qstr("url", strlen("url"));
    url = mp_obj_str_get_str(mp_obj_dict_get(data, index));
    http_param_download.url = url;

    /* get http download filepath */
    index = mp_obj_new_str_via_qstr("filepath", strlen("filepath"));
    filepath = mp_obj_str_get_str(mp_obj_dict_get(data, index));
    http_param_download.filepath = filepath;

    /* callback */
    http_param_download.cb = callback;

    ESP_LOGD(LOG_TAG, "callback = %p\n", callback);

    xTaskCreate(&task_http_download_func, "task_http_download_func", 8192, NULL, ADDON_TSK_PRIORRITY, NULL);

    return mp_obj_new_int(0);
}
MP_DEFINE_CONST_FUN_OBJ_2(mp_obj_http_download, http_download);

static mp_obj_t http_request(mp_obj_t data, mp_obj_t callback)
{
    const int8_t *method = NULL;
    const int8_t *url = NULL;
    int32_t http_method = 0;
    int32_t timeout = 0;

    if (!mp_obj_is_dict_or_ordereddict(data)) {
        ESP_LOGE(LOG_TAG, "%s param type error, param type must be dict\r\n", __func__);
        return mp_obj_new_int(-EINVAL);
    }

    memset(&http_param_request, 0, sizeof(hapy_http_param_t));
    resp_body_buffer = NULL;

    /* get http request url */
    mp_obj_t index = mp_obj_new_str_via_qstr("url", 3);
    url = mp_obj_str_get_str(mp_obj_dict_get(data, index));
    http_param_request.url = url;

    /* get http request method */
    index = mp_obj_new_str_via_qstr("method", 6);
    method = mp_obj_str_get_str(mp_obj_dict_get(data, index));
    if (strcmp(method, "GET") == 0) {
        http_method = HTTP_GET;
    } else if (strcmp(method, "HEAD") == 0) {
        http_method = HTTP_HEAD;
    } else if (strcmp(method, "POST") == 0) {
        http_method = HTTP_POST;
    } else if (strcmp(method, "PUT") == 0) {
        http_method = HTTP_PUT;
    } else if (strcmp(method, "DELETE") == 0) {
        http_method = HTTP_DELETE;
    } else {
        http_method = HTTP_GET;
    }
    http_param_request.method = http_method;

    /* get http request timeout */
    index = mp_obj_new_str_via_qstr("timeout", 7);
    timeout = mp_obj_get_int(mp_obj_dict_get(data, index));
    http_param_request.timeout = timeout;

    /* get http request headers */
    index = mp_obj_new_str_via_qstr("headers", 7);
    mp_obj_t header = mp_obj_dict_get(data, index);
    if (header == MP_OBJ_NULL) {
        http_param_request.http_header[0].name = HTTP_DEFAULT_HEADER_NAME;
        http_param_request.http_header[0].data = HTTP_DEFAULT_HEADER_DATA;
        http_param_request.header_size = 1;
    } else if (mp_obj_is_type(header, &mp_type_dict)) {
        // dictionary
        mp_map_t *map = mp_obj_dict_get_map(header);
        // assert(args2_len + 2 * map->used <= args2_alloc); // should have
        // enough, since kw_dict_len is in this case hinted correctly above
        for (size_t i = 0; i < map->alloc; i++) {
            if (mp_map_slot_is_filled(map, i)) {
                // the key must be a qstr, so intern it if it's a string
                mp_obj_t key = map->table[i].key;
                if (!mp_obj_is_qstr(key)) {
                    key = mp_obj_str_intern_checked(key);
                }
                http_param_request.http_header[i].name = mp_obj_str_get_str(map->table[i].key);
                http_param_request.http_header[i].data = mp_obj_str_get_str(map->table[i].value);
                http_param_request.header_size = i + 1;
            }
        }
    }

    /* get params */
    index = mp_obj_new_str_via_qstr("params", 6);
    mp_obj_t params = mp_obj_dict_get(data, index);
    http_param_request.params = mp_obj_str_get_str(params);

    /* callback */
    http_param_request.cb = callback;

    ESP_LOGD(LOG_TAG, "url: %s", http_param_request.url);
    ESP_LOGD(LOG_TAG, "method: %d", http_param_request.method);
    ESP_LOGD(LOG_TAG, "timeout: %d", http_param_request.timeout);
    for (int i = 0; i < http_param_request.header_size; i++) {
        ESP_LOGD(LOG_TAG, "headers: %s:%s", http_param_request.http_header[i].name,
                 http_param_request.http_header[i].data);
    }
    ESP_LOGD(LOG_TAG, "params: %s", http_param_request.params);

    xTaskCreate(&task_http_request_func, "task_http_request_func", 8192, NULL, ADDON_TSK_PRIORRITY, NULL);

    return mp_obj_new_int(0);
}
MP_DEFINE_CONST_FUN_OBJ_2(mp_obj_http_request, http_request);

STATIC const mp_rom_map_elem_t http_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_http) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_request), MP_ROM_PTR(&mp_obj_http_request) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_download), MP_ROM_PTR(&mp_obj_http_download) },
    { MP_ROM_QSTR(MP_QSTR_OK), MP_ROM_INT(HTTP_DL_STATUS_OK) },
    { MP_ROM_QSTR(MP_QSTR_FAIL), MP_ROM_INT(HTTP_DL_STATUS_FAIL) },
    { MP_ROM_QSTR(MP_QSTR_NULL), MP_ROM_INT(HTTP_DL_STATUS_PATH_NULL) },
};

STATIC MP_DEFINE_CONST_DICT(http_locals_dict, http_locals_dict_table);

const mp_obj_module_t mp_module_http = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&http_locals_dict,
};

MP_REGISTER_MODULE(MP_QSTR_http, mp_module_http, MICROPY_PY_HTTP);

#endif  // MICROPY_PY_HTTP

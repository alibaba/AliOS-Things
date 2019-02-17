/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "cJSON.h"
#include "errno.h"

#include "aos/kernel.h"
#include "k_api.h"
#include "ulog/ulog.h"

#include "ota_log.h"
#include "ota_hal_plat.h"
#include "ota_hal_os.h"
#include "ota/ota_service.h"
#include "ota_verify.h"

#include "udev.h"
#include "udev_net.h"
#include "udev_mqtt_client.h"

#define OTA_BUFFER_MAX_SIZE 513
#define MQTT_TOKEN          "QWxpT1MtVGhpbmdzLXVkZXZ8dWRldiFAIyQl"

static aos_task_t udev_download_task = {NULL};
static aos_task_t udev_yield_task = {NULL};

static char udev_client_id[64] = {0};
static char udev_topic_progress[32] = {0};
static char udev_topic_upgrade[32] = {0};

static int udev_mqtt_fd = -1;
static unsigned char udev_upgrading = 0;
static char sub_payload[256] = {0};

#define HTTP_HEADER \
    "GET /%s HTTP/1.1\r\nAccept:*/*\r\n\
User-Agent: Mozilla/5.0\r\n\
Cache-Control: no-cache\r\n\
Connection: close\r\n\
Host:%s:%d\r\n\r\n"

#define HTTP_HEADER_RESUME \
    "GET /%s HTTP/1.1\r\nAccept:*/*\r\n\
User-Agent: Mozilla/5.0\r\n\
Cache-Control: no-cache\r\n\
Connection: close\r\n\
Range: bytes=%d-\r\n\
Host:%s:%d\r\n\r\n"

static char dl_url[125] = {0};
static int dl_size = 0;
static char dl_hash[33] = {0};
static int isHttps = 0;

extern ota_hal_module_t ota_hal_module;

void udev_yield(void *unused)
{
    (void)unused;

    while(1){
        udev_mqtt_yield(udev_mqtt_fd, 1000);
    }
}

int udev_publish_progress(unsigned char progress)
{
    unsigned char payload[32] = {0};

    mc_message_t message = {
        .qos = QOS0,
        .retained = 0,
        .payload = NULL,
        .payloadlen = 0
    };

    if(udev_mqtt_fd < 0)
        return -1;

    message.payload = payload;
    memset(payload, 0, sizeof(payload));
    sprintf(payload, "{\"progress\":%d}", progress);
    message.payloadlen = strlen(payload);
    if(udev_mqtt_publish(udev_mqtt_fd, udev_topic_progress, &message) != 0){
        LOGE("udev", "udev_mqtt_publish failed");
        return -1;
    }
    LOGI("udev", "udev_mqtt_publish topic:%s, payload:%s ok", udev_topic_progress, payload);

    return 0;
}

static void http_gethost_info(char *src, char **web, char **file, int *port)
{
    char *pa;
    char *pb;
    isHttps = 0;
    if (!src || strlen(src) == 0) {
        OTA_LOG_E("http_gethost_info parms error!\n");
        return;
    }
    *port = 0;

    pa = src;
    if (!strncmp(pa, "https://", strlen("https://"))) {
        pa      = src + strlen("https://");
        isHttps = 1;
    }
    if (!isHttps) {
        if (!strncmp(pa, "http://", strlen("http://"))) {
            pa = src + strlen("http://");
        }
    }
    *web = pa;
    pb   = strchr(pa, '/');
    if (pb) {
        *pb = 0;
        pb += 1;
        if (*pb) {
            *file                   = pb;
            *((*file) + strlen(pb)) = 0;
        }
    } else {
        (*web)[strlen(pa)] = 0;
    }
#if defined AOS_OTA_TLS || defined AOS_OTA_ITLS
    isHttps = 1;
#else /* defined AOS_OTA_TLS || defined AOS_OTA_ITLS */
    isHttps = 0;
#endif /* defined AOS_OTA_TLS || defined AOS_OTA_ITLS */
    pa = strchr(*web, ':');
    if (pa) {
        *pa   = 0;
        *port = atoi(pa + 1);
    } else {
        if (isHttps) {
            *port = 443;
        } else {
            *port = 80;
        }
    }
}

static int ota_parse_json(const char *json)
{
    int ret = 0;
    cJSON *root = NULL;
    root = cJSON_Parse(json);
    if (!root) {
        ret = -1;
        goto parse_failed;
    } else {
        cJSON *resourceUrl = cJSON_GetObjectItem(root, "url");
        if (!resourceUrl) {
            ret = -1;
            goto parse_failed;
        }
        strncpy(dl_url, resourceUrl->valuestring, sizeof(dl_url));

        cJSON *hash = cJSON_GetObjectItem(root, "MD5");
        if (!hash) {
            ret = -1;
            goto parse_failed;
        }
        strncpy(dl_hash, hash->valuestring, sizeof(dl_hash));
        dl_hash[32] = '\0';
        ota_to_capital(dl_hash, strlen(dl_hash));

        cJSON *size = cJSON_GetObjectItem(root, "size");
        if (!size) {
            ret = -1;
            goto parse_failed;
        }
        dl_size = size->valueint;

    }
    OTA_LOG_I("parse size: %d, url:%s, md5:%s\n", dl_size, dl_url, dl_hash);
    goto parse_success;
parse_failed:
    OTA_LOG_E("parse failed err:%d",ret);
    if (root) {
        cJSON_Delete(root);
    }
    return -1;

parse_success:
    if (root) {
        cJSON_Delete(root);
    }
    return 0;
}

static int ota_http_download(void)
{
    int                  ret          = 0;
    void                 *sockfd      = NULL;
    int                  port         = 0;
    int                  nbytes       = 0;
    int                  send         = 0;
    int                  totalsend    = 0;
    int                  size         = 0;
    int                  header_found = 0;
    char                *pos          = 0;
    int                  file_size    = 0;
    ota_hash_param_t    *hash_ctx     = NULL;
    char                *host_file    = NULL;
    char                *host_addr    = NULL;
    char                *http_buffer  = NULL;
    void                *ssl          = NULL;
    char                retry = 0;
    unsigned int        ota_percent = 0;
    unsigned int        divisor     = 10;
    int                 offset      = 0;
    ota_boot_param_t ota_param = {0};

    char *url = dl_url;
    if (!url || strlen(url) == 0) {
        ret = -1;
        return ret;
    }

    ota_hal_register_module(&ota_hal_module);

    ota_param.len = dl_size;
    ota_param.off_bp = 0;
    ota_param.res_type = OTA_FINISH;
    ret = ota_hal_init(&ota_param);
    if(ret < 0) {
        OTA_LOG_I("file_size:%d\r\n", file_size);
        ret  = OTA_DOWNLOAD_FAIL;
        return ret;
    }

    http_gethost_info(url, &host_addr, &host_file, &port);
    if (host_file == NULL || host_addr == NULL) {
        ret = OTA_DOWNLOAD_IP_FAIL;
        return ret;
    }

    sockfd = ota_socket_connect(host_addr, port);
    if ((int)sockfd < 0) {
        ret = OTA_DOWNLOAD_CON_FAIL;
        return ret;
    }
    http_buffer = (char *)ota_malloc(OTA_BUFFER_MAX_SIZE);
    if(NULL == http_buffer) {
        ret = OTA_DOWNLOAD_FAIL;
        goto END;
    }
    ret = ota_malloc_hash_ctx(MD5);
    if (ret < 0) {
        ret = OTA_DOWNLOAD_FAIL;
        goto END;
    }

    hash_ctx = ota_get_hash_ctx();
    if (hash_ctx == NULL || hash_ctx->ctx_hash == NULL || hash_ctx->ctx_size == 0) {
        ret = OTA_DOWNLOAD_FAIL;
        goto END;;
    }
    memset(http_buffer, 0, OTA_BUFFER_MAX_SIZE);
    ota_snprintf(http_buffer, OTA_BUFFER_MAX_SIZE-1, HTTP_HEADER, host_file, host_addr, port);
    if (ota_hash_init(hash_ctx->hash_method, hash_ctx->ctx_hash) < 0) {
        ret = OTA_VERIFY_HASH_FAIL;
        goto END;
    }
    ota_set_cur_hash(dl_hash);
    send      = 0;
    totalsend = 0;
    nbytes    = strlen(http_buffer);
    OTA_LOG_I("send %s", http_buffer);
    while (totalsend < nbytes) {
        send = ota_socket_send(sockfd, http_buffer + totalsend, nbytes - totalsend);
        if (send < 0) {
            ret = OTA_DOWNLOAD_WRITE_FAIL;
            goto END;
        }
        totalsend += send;
        OTA_LOG_I("%d bytes send.", totalsend);
    }
    memset(http_buffer, 0, OTA_BUFFER_MAX_SIZE);
    while ((nbytes = ota_socket_recv(sockfd, http_buffer, OTA_BUFFER_MAX_SIZE - 1)) != 0) {
        if((nbytes <= 0)&&(retry <= 5)){
             retry++;
             OTA_LOG_I("retry cn:%d",retry);
             continue;
        } else {
             retry=0;
        }
        if (nbytes < 0) {
            if (errno != EINTR) {
                ret = OTA_DOWNLOAD_READ_FAIL;
                break;
            } else {
                continue;
            }
        }
        if (!header_found) {
            if (!file_size) {
                char *ptr = strstr(http_buffer, "Content-Length:");
                if (ptr) {
                    ret = sscanf(ptr, "%*[^ ]%d", &file_size);
                    if(ret < 0) {
                        OTA_LOG_E("Content-Length error.");
                    }
                    /* ota_param.len = file_size; */
                    OTA_LOG_I("file_size:%d\r\n", file_size);
                }
            }
            pos = strstr(http_buffer, "\r\n\r\n");
            if (pos != NULL) {
                pos += 4;
                int len      = pos - http_buffer;
                header_found = 1;
                size         = nbytes - len;
                if (ota_hash_update((const unsigned char *)pos, size, hash_ctx->ctx_hash) < 0) {
                    ret = OTA_VERIFY_HASH_FAIL;
                    goto END;
                }
                ret = ota_hal_write(&ota_param.off_bp, pos, size);
                OTA_LOG_I("ota_hal_write\r\n");
                if (ret < 0) {
                    ret = OTA_UPGRADE_FAIL;
                    goto END;
                }
            }
            memset(http_buffer, 0, OTA_BUFFER_MAX_SIZE);
            continue;
        }
        size += nbytes;
        if (ota_hash_update((const unsigned char *)http_buffer, nbytes, hash_ctx->ctx_hash) < 0) {
            ret = OTA_VERIFY_HASH_FAIL;
            goto END;
        }
        ret = ota_hal_write(NULL,http_buffer, nbytes);
        if (ret < 0) {
            ret = OTA_UPGRADE_FAIL;
            goto END;
        }
        if(file_size) {
            ota_percent = ((long long)size * 100) / (long long)file_size;
            /* LOGI("udev", "ota_percent:%d, divisor:%d\r\n", ota_percent, divisor); */
            if(ota_percent / divisor) {
                divisor += 5;
                udev_publish_progress(ota_percent);
                OTA_LOG_I("s:%d %d per:%d",size,nbytes,ota_percent);
            }
        }
        if (size == file_size) {
            nbytes = 0;
            break;
        }
    }
    if (nbytes < 0) {
        ret = OTA_DOWNLOAD_FAIL;
    } else if (nbytes == 0) {
        ota_set_break_point(0);
    }
END:
    OTA_LOG_I("download finish ret:%d.",ret);

    if(http_buffer)
        ota_free(http_buffer);
    if(sockfd)
        ota_socket_close(sockfd);
    if(ret == 0){
        ret = ota_check_hash(MD5, dl_hash);
        ota_free_hash_ctx();
        if (ret < 0) {
            OTA_LOG_E("hash check error:%d.", ret);
            return ret;
        }
        aos_msleep(1000);
        ota_hal_boot(&ota_param);
    }

    udev_upgrading = 0;
    return ret;
}

void udev_http_download(void *unused)
{
    ota_http_download();
}

void udev_upgrade(mc_string_t* topic_name, mc_message_t* message)
{
    int ret = 0;

    /* LOGD("udev", "udev_upgrade"); */
    if( message->payloadlen >= sizeof(sub_payload)){
        LOGE("udev", "message too large");
        return;
    }

    if(udev_upgrading == 1){
        LOGE("udev", "upgrade is in progress\n");
        return;
    }
    udev_upgrading = 1;

    memcpy(sub_payload, message->payload, message->payloadlen);
    LOGI("udev", "upgrade payload:%s\n", sub_payload);
    ret = ota_parse_json(sub_payload);
    if(ret < 0){
        OTA_LOG_E("Parse json fail:%d\n",ret);
        udev_upgrading = 0;
        return;
    }

    aos_task_new_ext(&udev_download_task, "http_dl", udev_http_download, NULL, 4096, 40);
}

int udev_init(const char *pk, const char *dn)
{
    static unsigned char udev_inited = 0;
    char mqtt_token[48] = {0};
    int ret = 0, token_len = sizeof(mqtt_token);

    if(udev_inited == 1){
        LOGE("udev", "udev already init\n");
        return -1;
    }
    udev_inited = 1;

    if (pk == NULL || strlen(pk) == 0 ||
        dn == NULL || strlen(dn) == 0){
        LOGE("udev", "udev_init wrong pk/dn/ds\n");
        return -1;
    }

    snprintf(udev_client_id, sizeof(udev_client_id), "%s.%s", dn, pk);
    snprintf(udev_topic_progress, sizeof(udev_topic_progress), "/%s/%s/progress", pk, dn);
    snprintf(udev_topic_upgrade, sizeof(udev_topic_upgrade), "/%s/%s/upgrade", pk, dn);
    LOGI("udev", "client_id: %s\n", udev_client_id);
    LOGI("udev", "topic_progress: %s\n", udev_topic_progress);
    LOGI("udev", "topic_upgrade: %s\n", udev_topic_upgrade);

    if((ret = ota_base64_decode(MQTT_TOKEN, sizeof(MQTT_TOKEN) - 1, mqtt_token, &token_len)) < 0)
    {
        LOGE("udev", "base64 decode error, ret:%d, token: %d %d\n", ret, token_len, strlen(MQTT_TOKEN) - 1);
        return -1;
    }

    if((udev_mqtt_fd = udev_mqtt_connect(UDEV_BROKER_HOST, UDEV_BROKER_PORT, udev_client_id, mqtt_token, 2000)) < 0){
        LOGE("udev", "udev_mqtt_connect failed");
        return -1;
    }
    /* LOGI("udev", "udev_mqtt_connect: ok, fd=%d\n", udev_mqtt_fd); */

    if(udev_mqtt_subscript(udev_mqtt_fd, udev_topic_upgrade, QOS1, udev_upgrade) != 0){
        LOGE("udev", "udev_mqtt_subscript failed");
        return -1;
    }

    LOGI("udev", "udev_init ok, fd=%d", udev_mqtt_fd);

    aos_task_new_ext(&udev_yield_task, "udev", udev_yield, NULL, 4096, 40);

    return 0;
}

/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "ota/ota_service.h"
#include "ota_log.h"
#include "ota_hal_os.h"
#include "ota_hal_plat.h"
#include "ota_verify.h"

#ifndef EINTR
#define EINTR 4
#endif

#define OTA_BUFFER_MAX_SIZE 513

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

#if defined OTA_CONFIG_TLS
static const char *ca = \
{
    \
    "-----BEGIN CERTIFICATE-----\r\n"
    "MIIDdTCCAl2gAwIBAgILBAAAAAABFUtaw5QwDQYJKoZIhvcNAQEFBQAwVzELMAkG\r\n" \
    "A1UEBhMCQkUxGTAXBgNVBAoTEEdsb2JhbFNpZ24gbnYtc2ExEDAOBgNVBAsTB1Jv\r\n" \
    "b3QgQ0ExGzAZBgNVBAMTEkdsb2JhbFNpZ24gUm9vdCBDQTAeFw05ODA5MDExMjAw\r\n" \
    "MDBaFw0yODAxMjgxMjAwMDBaMFcxCzAJBgNVBAYTAkJFMRkwFwYDVQQKExBHbG9i\r\n" \
    "YWxTaWduIG52LXNhMRAwDgYDVQQLEwdSb290IENBMRswGQYDVQQDExJHbG9iYWxT\r\n" \
    "aWduIFJvb3QgQ0EwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDaDuaZ\r\n" \
    "jc6j40+Kfvvxi4Mla+pIH/EqsLmVEQS98GPR4mdmzxzdzxtIK+6NiY6arymAZavp\r\n" \
    "xy0Sy6scTHAHoT0KMM0VjU/43dSMUBUc71DuxC73/OlS8pF94G3VNTCOXkNz8kHp\r\n" \
    "1Wrjsok6Vjk4bwY8iGlbKk3Fp1S4bInMm/k8yuX9ifUSPJJ4ltbcdG6TRGHRjcdG\r\n" \
    "snUOhugZitVtbNV4FpWi6cgKOOvyJBNPc1STE4U6G7weNLWLBYy5d4ux2x8gkasJ\r\n" \
    "U26Qzns3dLlwR5EiUWMWea6xrkEmCMgZK9FGqkjWZCrXgzT/LCrBbBlDSgeF59N8\r\n" \
    "9iFo7+ryUp9/k5DPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNVHRMBAf8E\r\n" \
    "BTADAQH/MB0GA1UdDgQWBBRge2YaRQ2XyolQL30EzTSo//z9SzANBgkqhkiG9w0B\r\n" \
    "AQUFAAOCAQEA1nPnfE920I2/7LqivjTFKDK1fPxsnCwrvQmeU79rXqoRSLblCKOz\r\n" \
    "yj1hTdNGCbM+w6DjY1Ub8rrvrTnhQ7k4o+YviiY776BQVvnGCv04zcQLcFGUl5gE\r\n" \
    "38NflNUVyRRBnMRddWQVDf9VMOyGj/8N7yy5Y0b2qvzfvGn9LhJIZJrglfCm7ymP\r\n" \
    "AbEVtQwdpf5pLGkkeB6zpxxxYu7KyJesF12KwvhHhm4qxFYxldBniYUr+WymXUad\r\n" \
    "DKqC5JlR3XC321Y9YeRq4VzW9v493kHMB65jUr9TU/Qr6cf9tveCX4XSQRjbgbME\r\n" \
    "HMUfpIBvFSDJ3gyICh3WZlXi/EjJKSZp4A==\r\n" \
    "-----END CERTIFICATE-----"
};
#endif
static int isHttps = 0;
/**
 * @brief http_gethost_info
 *
 * @Param: src  url
 * @Param: web  WEB
 * @Param: file  download filename
 * @Param: port  default 80
 */
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
    if (!(*src)) {
        return;
    }
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
#if defined OTA_CONFIG_TLS || defined OTA_CONFIG_ITLS
    isHttps = 1;
#else
    isHttps = 0;
#endif
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

static int ota_download_start(void *pctx)
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
    ota_service_t* ctx = (ota_service_t*)pctx;
    if (!ctx) {
        ret = OTA_PARAM_FAIL;
        return ret;
    }
    ota_boot_param_t *ota_param = (ota_boot_param_t *)ctx->boot_param;
    if (!ctx->boot_param) {
        ret = OTA_PARAM_FAIL;
        return ret;
    }

    char* url = ctx->url;
    if (!url || strlen(url) == 0) {
        ret = OTA_PARAM_FAIL;
        return ret;
    }
    http_gethost_info(url, &host_addr, &host_file, &port);
    if (host_file == NULL || host_addr == NULL) {
        ret = OTA_DOWNLOAD_IP_FAIL;
        return ret;
    }

    if (isHttps) {
#if defined OTA_CONFIG_ITLS
        char pkps[PRODUCT_KEY_LEN + PRODUCT_SECRET_LEN] = {0};
        int len = strlen(ctx->pk);
        strncpy(pkps, ctx->pk, len);
        HAL_GetProductSecret(pkps + len + 1);
        len += strlen(pkps + len + 1) + 2;
        ssl = ota_ssl_connect(host_addr, port, pkps,len);
#elif defined OTA_CONFIG_TLS
        ssl = ota_ssl_connect(host_addr, port, ca, strlen(ca) + 1);
#endif
        if (ssl == NULL) {
            ret = OTA_DOWNLOAD_CON_FAIL;
            return ret;
        }
    } else {
        sockfd = ota_socket_connect(host_addr, port);
        if (sockfd < 0) {
            ret = OTA_DOWNLOAD_CON_FAIL;
            return ret;
        }
    }
    http_buffer = ota_malloc(OTA_BUFFER_MAX_SIZE);
    if(NULL == http_buffer) {
        ret = OTA_DOWNLOAD_FAIL;
        goto END;
    }
    hash_ctx = ota_get_hash_ctx();
    if (hash_ctx == NULL || hash_ctx->ctx_hash == NULL || hash_ctx->ctx_size == 0) {
        ret = OTA_DOWNLOAD_FAIL;
        goto END;;
    }
    memset(http_buffer, 0, OTA_BUFFER_MAX_SIZE);
    if (ota_param->off_bp) {
        ota_snprintf(http_buffer, OTA_BUFFER_MAX_SIZE - 1, HTTP_HEADER_RESUME, host_file, ota_param->off_bp, host_addr, port);
        ota_get_last_hash_ctx(hash_ctx);
    } else {
        ota_param->off_bp = 0;
        ota_snprintf(http_buffer, OTA_BUFFER_MAX_SIZE - 1, HTTP_HEADER, host_file, host_addr, port);
        if (ota_hash_init(hash_ctx->hash_method, hash_ctx->ctx_hash) < 0) {
            ret = OTA_VERIFY_HASH_FAIL;
            goto END;
        }
    }
    ota_set_cur_hash(ctx->hash);
    send      = 0;
    totalsend = 0;
    nbytes    = strlen(http_buffer);
    OTA_LOG_I("send %s", http_buffer);
    while (totalsend < nbytes) {
        send = ((isHttps) ? ota_ssl_send(ssl, (char *)(http_buffer + totalsend), (int)(nbytes - totalsend))
                 :ota_socket_send(sockfd, http_buffer + totalsend, nbytes - totalsend));
        if (send <= 0) {
            ret = OTA_DOWNLOAD_WRITE_FAIL;
            goto END;
        }
        totalsend += send;
        OTA_LOG_I("%d bytes send.", totalsend);
    }
    memset(http_buffer, 0, OTA_BUFFER_MAX_SIZE);
    while ((nbytes = ((isHttps) ? ota_ssl_recv(ssl, http_buffer, OTA_BUFFER_MAX_SIZE - 1)
          :ota_socket_recv(sockfd, http_buffer, OTA_BUFFER_MAX_SIZE - 1))) != 0) {
        if((nbytes <= 0)&&(retry <= 5)){
             retry++;
             OTA_LOG_I("retry cn:%d", retry);
             continue;
        } else {
             retry = 0;
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
                }
            }
            pos = strstr(http_buffer, "\r\n\r\n");
            if (pos != NULL) {
                pos += 4;
                int len      = pos - http_buffer;
                header_found = 1;
                size         = nbytes - len;
                if (ota_hash_update((const unsigned char *)pos, size, hash_ctx->ctx_hash) < 0) {
                    ota_set_break_point(0);
                    ret = OTA_VERIFY_HASH_FAIL;
                    goto END;
                }
                ret = ota_hal_write(&ota_param->off_bp, pos, size);
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
            ota_set_break_point(0);
            ret = OTA_VERIFY_HASH_FAIL;
            goto END;
        }
        ret = ota_hal_write(NULL, http_buffer, nbytes);
        if (ret < 0) {
            ret = OTA_UPGRADE_FAIL;
            goto END;
        }
        if(file_size) {
            ota_percent = ((long long)size * 100) / (long long)file_size;
            if(ota_percent / divisor) {
                divisor += 5;
#if (!defined BOARD_ESP8266)
                ctx->h_tr->status(ota_percent, ctx);
#endif
                OTA_LOG_I("s:%d %d per:%d", size, nbytes, ota_percent);
            }
        }
        if (size == file_size) {
            nbytes = 0;
            break;
        }

        if (ctx->upg_status == OTA_CANCEL) {
            break;
        }
    }
    if (nbytes < 0) {
        ota_save_state(size + ota_param->off_bp, hash_ctx);
        ret = OTA_DOWNLOAD_FAIL;
    } else if (nbytes == 0) {
        ota_set_break_point(0);
    } else {
        ota_save_state(size + ota_param->off_bp, hash_ctx);
        ret = OTA_CANCEL;
    }
END:
    OTA_LOG_I("download finish ret:%d.", ret);
    if(http_buffer)
        ota_free(http_buffer);
    if(sockfd)
        ota_socket_close(sockfd);
    return ret;
}

static int ota_download_stop(void)
{
    return 0;
}

static ota_download_t dl_http = {
    .start = ota_download_start,
    .stop  = ota_download_stop,
};

ota_download_t *ota_get_download(void)
{
    return &dl_http;
}

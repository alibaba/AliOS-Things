/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "utils_md5.h"
#include "ota_download.h"
#include "ota_util.h"
#include "ota_log.h"
#include "ota_manifest.h"
#include "ota_verify.h"
#include "ota_hal_os.h"
#include "ota_rsa_verify.h"
#include "ota_hash.h"


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

#if !defined ITLS_DOWNLOAD
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
#if defined HTTPS_DOWNLOAD || defined ITLS_DOWNLOAD
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

static int ota_download_start(char *url, ota_write_cb_t wcb, void *cur_hash)
{
    if (!url || strlen(url) == 0 || wcb == NULL || cur_hash == NULL) {
        OTA_LOG_E("ota_download parms error!\n");
        return OTA_DOWNLOAD_URL_FAIL;
    }
    int                  ret          = 0;
    int                  sockfd       = 0;
    int                  port         = 0;
    int                  nbytes       = 0;
    int                  send         = 0;
    int                  totalsend    = 0;
    uint32_t             breakpoint   = 0;
    int                  size         = 0;
    int                  header_found = 0;
    char                *pos          = 0;
    int                  file_size    = 0;
    ota_hash_params      last_hash;
    ota_hash_ctx_params *hash_ctx                         = NULL;
    char                *host_file                        = NULL;
    char                *host_addr                        = NULL;
    char                *http_buffer                      = NULL;
    void                *ssl                              = NULL;
    char                retry = 0;
    uint32_t            ota_percent = 0;
    uint32_t            divisor     = 10;
    http_gethost_info(url, &host_addr, &host_file, &port);

    if (host_file == NULL || host_addr == NULL) {
        ret = OTA_DOWNLOAD_URL_FAIL;
        return ret;
    }

    if (isHttps) {
#if defined ITLS_DOWNLOAD
        char pkps[PRODUCT_KEY_LEN + PRODUCT_SECRET_LEN] = {0};
        ota_service_manager *ctx = (ota_service_manager *)get_ota_service_manager();
        int len = strlen(ctx->pk);
        strncpy(pkps, ctx->pk, len);
        HAL_GetProductSecret(pkps + len + 1);
        len += strlen(pkps + len + 1) + 2;
        ssl = ota_ssl_connect(host_addr, port, pkps,len);
#else
        ssl = ota_ssl_connect(host_addr, port, ca, strlen(ca)+1);
#endif
        if (ssl == NULL) {
            OTA_LOG_E("ota_ssl_socket_connect error\n ");
            ret = OTA_DOWNLOAD_SOCKET_FAIL;
            return ret;
        }
    } else {
        sockfd = ota_socket_connect(host_addr, port);
        if (sockfd < 0) {
            OTA_LOG_E("ota_socket_connect error\n ");
            ret = OTA_DOWNLOAD_SOCKET_FAIL;
            return ret;
        }
    }
    hash_ctx = ota_get_global_hash_context();
    if (hash_ctx == NULL || hash_ctx->ctx_hash == NULL ||
        hash_ctx->ctx_size == 0) {
        OTA_LOG_E("ota get sign ctx fail\n ");
        if(sockfd)
        ota_socket_close(sockfd);
        ret = OTA_DOWNLOAD_FAIL;
        return ret;
    }
    breakpoint = ota_get_update_breakpoint();
    memset(&last_hash, 0x00, sizeof last_hash);
    ota_get_last_hash((char *)&last_hash);

    http_buffer = ota_malloc(OTA_BUFFER_MAX_SIZE);
    if(NULL == http_buffer) {
        OTA_LOG_E("memory fail\n ");
        if(sockfd)
        ota_socket_close(sockfd);
        ret = OTA_DOWNLOAD_FAIL;
        return ret;
    }
    memset(http_buffer, 0, OTA_BUFFER_MAX_SIZE);
    if (breakpoint && (ota_verify_hash_value(
                         last_hash, (*((ota_hash_params *)cur_hash))) == 0)) {
        OTA_LOG_I("----resume download,breakpoint=%d------", breakpoint);
        sprintf(http_buffer, HTTP_HEADER_RESUME, host_file, breakpoint,
                host_addr, port);
        ota_get_last_hash_context(hash_ctx);
    } else {
        breakpoint = 0;
        sprintf(http_buffer, HTTP_HEADER, host_file, host_addr, port);
        if (ota_hash_init(hash_ctx->hash_method, hash_ctx->ctx_hash) != 0) {
            OTA_LOG_E("ota sign init fail \n ");
            ret = OTA_DOWNLOAD_SIGN_INIT_FAIL;
            goto DOWNLOAD_END;
        }
    }
    ota_set_cur_hash((char *)cur_hash);
    send      = 0;
    totalsend = 0;
    nbytes    = strlen(http_buffer);
    OTA_LOG_I("send %s", http_buffer);
    while (totalsend < nbytes) {
        send = ((isHttps) ? ota_ssl_send(ssl, (char *)(http_buffer + totalsend),
                                         (int)(nbytes - totalsend))
                          : ota_socket_send(sockfd, http_buffer + totalsend,
                                            nbytes - totalsend));
        if (send < 0) {
            OTA_LOG_E("send error!%s\n ", strerror(errno));
            ret = OTA_DOWNLOAD_SEND_FAIL;
            goto DOWNLOAD_END;
        }
        totalsend += send;
        OTA_LOG_I("%d bytes send OK!\n ", totalsend);
    }
    memset(http_buffer, 0, OTA_BUFFER_MAX_SIZE); 
    while (
      (nbytes =
         ((isHttps) ? ota_ssl_recv(ssl, http_buffer, OTA_BUFFER_MAX_SIZE - 1)
                    : ota_socket_recv(sockfd, http_buffer,
                                      OTA_BUFFER_MAX_SIZE - 1))) != 0) {
        if((nbytes <= 0)&&(retry <= 5)){
             retry++;
             OTA_LOG_I("retry cn:%d",retry);
             continue;
        } else {
             retry=0;
        }
        if (nbytes < 0) {
            OTA_LOG_I("ota_socket_recv nbytes < 0");
            if (errno != EINTR) {
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
                if (OTA_CRYPTO_SUCCESS != ota_hash_update((const uint8_t *)pos,
                                                          size,
                                                          hash_ctx->ctx_hash)) {
                    OTA_LOG_E("ota hash update fail\n ");
                    ota_set_update_breakpoint(0);
                    ret = OTA_DOWNLOAD_SEND_FAIL;
                    goto DOWNLOAD_END;
                }
                ret = wcb(OTA_BUFFER_MAX_SIZE, (uint8_t *)pos, size, 0);
                if (ret < 0) {
                    OTA_LOG_I("write error:%d\n", ret);
                    ret = OTA_DOWNLOAD_FAIL;
                    goto DOWNLOAD_END;
                }
            }
            memset(http_buffer, 0, OTA_BUFFER_MAX_SIZE);
            continue;
        }
        size += nbytes;
        OTA_LOG_I("s:%d %d",size,nbytes);
        if (OTA_CRYPTO_SUCCESS != ota_hash_update((const uint8_t *)http_buffer,
                                                  nbytes, hash_ctx->ctx_hash)) {
            OTA_LOG_E("ota hash update fail\n ");
            ota_set_update_breakpoint(0);
            ret = OTA_DOWNLOAD_SEND_FAIL;
            goto DOWNLOAD_END;
        }
        ret = wcb(OTA_BUFFER_MAX_SIZE, (uint8_t *)http_buffer, nbytes, 0);
        if (ret < 0) {
            OTA_LOG_I("write error:%d\n", ret);
            ret = OTA_DOWNLOAD_FAIL;
            goto DOWNLOAD_END;
        }

        if(file_size) {
            ota_percent = (size * 100) / file_size;
            if(ota_percent / divisor) {
                divisor += 10;
#if (!defined IS_ESP8266)
                ota_status_post(ota_percent);
#endif
            }
        }
        if (size == file_size) {
            nbytes = 0;
            break;
        }

        if (ota_get_status() == OTA_CANCEL) {
            break;
        }
    }
    if (nbytes < 0) {
        OTA_LOG_E("download read error %s", strerror(errno));
        ota_save_state(size + breakpoint, hash_ctx);
        ret = OTA_DOWNLOAD_FAILED;
    } else if (nbytes == 0) {
        ota_set_update_breakpoint(0);
        ret = OTA_DOWNLOAD_FINISH;
    } else {
        ota_save_state(size + breakpoint, hash_ctx);
        ret = OTA_DOWNLOAD_CANCEL;
    }
DOWNLOAD_END:
    if(http_buffer)
        ota_free(http_buffer);
    if(sockfd)
        ota_socket_close(sockfd);
    return ret;
}


static int ota_download_init(void)
{
    return 0;
}

static int ota_download_stop(void)
{
    return 0;
}

static ota_download dl_http = {
    .init  = ota_download_init,
    .start = ota_download_start,
    .stop  = ota_download_stop,
};

const void *ota_get_download_http(void)
{
    return &dl_http;
}

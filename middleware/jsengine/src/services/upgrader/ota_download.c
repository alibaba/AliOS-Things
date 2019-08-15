/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "be_upgrader.h"
#include "ota_download.h"
#include "ota_socket.h"

#define OTA_BUFFER_MAX_SIZE 1461

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

static jse_md5_context g_ctx;
/**
 * @brief http_gethost_info
 *
 * @Param: src  url
 * @Param: web  WEB
 * @Param: file  download filename
 * @Param: port  default 80
 */
void http_gethost_info(char *src, char **web, char **file, int *port) {
    char *pa;
    char *pb;
    int isHttps = 0;

    if (!src || strlen(src) == 0) {
        jse_error("http_gethost_info parms error!\n");
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

    pa = strchr(*web, ':');
    if (pa) {
        *pa   = 0;
        *port = atoi(pa + 1);
    } else {
        /*
        if (isHttps) {
            *port = 80;/* 443 */
        } else {
            *port = 80;
        }*/
        *port = 80;
    }
}

int ota_download(char *url, write_flash_cb_t func, char *md5) {
    if (!url || strlen(url) == 0 || func == NULL || md5 == NULL) {
        jse_error("ota_download parms error!\n");
        return OTA_DOWNLOAD_URL_FAIL;
    }
    int ret                               = 0;
    int sockfd                            = 0;
    int port                              = 0;
    int nbytes                            = 0;
    int send                              = 0;
    int totalsend                         = 0;
    uint32_t breakpoint                   = 0;
    int size                              = 0;
    int header_found                      = 0;
    char *pos                             = 0;
    int file_size                         = 0;
    char last_md5[33]                     = {0};
    char *host_file                       = NULL;
    char *host_addr                       = NULL;
    char http_buffer[OTA_BUFFER_MAX_SIZE] = {0};
    http_gethost_info(url, &host_addr, &host_file, &port);

    if (host_file == NULL || host_addr == NULL) {
        ret = OTA_DOWNLOAD_URL_FAIL;
        return ret;
    }

    sockfd = ota_socket_connect(port, host_addr);
    if (sockfd < 0) {
        jse_error("ota_socket_connect error\n ");
        ret = OTA_DOWNLOAD_SOCKET_FAIL;
        return ret;
    }
#if 0
    breakpoint = ota_get_update_breakpoint();
    ota_get_last_MD5(last_md5);

    if (breakpoint && !strncmp(last_md5, md5, 32)) {
        jse_debug("----resume download,breakpoint=%d------", breakpoint);
        sprintf(http_buffer, HTTP_HEADER_RESUME, host_file, breakpoint,
        host_addr, port); ota_get_last_MD5_context(&g_ctx);
    } else {
        breakpoint = 0;
        sprintf(http_buffer, HTTP_HEADER, host_file, host_addr, port);
        MD5_Init(&g_ctx);
    }
    ota_set_cur_MD5(md5);
#endif

    jse_md5_init(&g_ctx);
    jse_md5_starts(&g_ctx);

    send      = 0;
    totalsend = 0;
    nbytes    = strlen(http_buffer);
    jse_debug("send %s", http_buffer);
    while (totalsend < nbytes) {
        send = ota_socket_send(sockfd, http_buffer + totalsend,
                               nbytes - totalsend);
        if (send == -1) {
            jse_error("send error!%s\n ", strerror(errno));
            ret = OTA_DOWNLOAD_SEND_FAIL;
            goto DOWNLOAD_END;
        }
        totalsend += send;
        jse_debug("%d bytes send OK!\n ", totalsend);
    }

    memset(http_buffer, 0, OTA_BUFFER_MAX_SIZE);
    while ((nbytes = ota_socket_recv(sockfd, http_buffer,
                                     OTA_BUFFER_MAX_SIZE - 1)) != 0) {
        /* aos_msleep(25);/* for slow-motion test */
        if (nbytes < 0) {
            jse_debug("ota_socket_recv nbytes < 0");
            if (errno != EINTR) {
                break;
            }
            if (ota_socket_check_conn(sockfd) < 0) {
                jse_error("download system error %s", strerror(errno));
                break;
            } else {
                continue;
            }
        }

        if (!header_found) {
            if (!file_size) {
                char *ptr = strstr(http_buffer, "Content-Length:");
                if (ptr) {
                    sscanf(ptr, "%*[^ ]%d", &file_size);
                }
            }

            pos = strstr(http_buffer, "\r\n\r\n");

            if (pos != NULL) {
                pos += 4;
                int len      = pos - http_buffer;
                header_found = 1;
                size         = nbytes - len;
                /* memcpy(headbuf, http_buffer, len);
                   jse_debug("headbuf=%s",headbuf);
                   MD5_Update(&g_ctx, (const uint8_t *)pos, size); */
                jse_md5_update(&g_ctx, (const uint8_t *)pos, size);
                func(OTA_BUFFER_MAX_SIZE, (uint8_t *)pos, size, 0);
            }
            memset(http_buffer, 0, OTA_BUFFER_MAX_SIZE);
            continue;
        }

        size += nbytes;
        /* jse_debug("size nbytes %d, %d", size, nbytes);
           MD5_Update(&g_ctx, (const uint8_t *) http_buffer, nbytes); */
        jse_md5_update(&g_ctx, (const uint8_t *)http_buffer, nbytes);
        func(OTA_BUFFER_MAX_SIZE, (uint8_t *)http_buffer, nbytes, 0);

        if (size == file_size) {
            nbytes = 0;
            break;
        }

        /* if (ota_get_status() == OTA_CANCEL) {
               break;
           } */
    }

    if (nbytes < 0) {
        jse_error("download read error %s", strerror(errno));
        /* save_state(size + breakpoint, &g_ctx); */
        ret = OTA_DOWNLOAD_FAILED;
    } else if (nbytes == 0) {
        /* ota_set_update_breakpoint(0); */
        ret = OTA_DOWNLOAD_FINISH;
    } else {
        /* save_state(size + breakpoint, &g_ctx); */
        ret = OTA_DOWNLOAD_CANCEL;
    }

DOWNLOAD_END:
    ota_socket_close(sockfd);
    return ret;
}

int check_md5(const char *buffer, const int32_t len) {
    uint8_t digest[16]  = {0};
    char digest_str[33] = {0};
    int i               = 0;
    jse_debug("digest=%s \n", buffer);
    /* MD5_Final((uint8_t *)digest, &g_ctx); */
    jse_md5_finish(&g_ctx, digest);

    for (; i < 16; i++) {
        snprintf(digest_str + i * 2, 2 + 1, "%02X", digest[i]);
    }
    jse_debug("url md5=%s", buffer);
    jse_debug("digestMD5=%s", digest_str);
    if (strncmp(digest_str, buffer, 32)) {
        jse_debug("update_packet md5 check FAIL!");
        return -1;
    }
    return 0;
}

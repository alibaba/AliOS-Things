/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <aos/aos.h>
#include <aos/network.h>

#include "md5.h"
#include "ota_download.h"
#include "ota_util.h"
#include "ota_log.h"
#include "ota_update_manifest.h"
#include "ota_socket.h"

#define KEY_OTA_BREAKPOINT  "key_ota_breakpoint"
#define KEY_OTA_MD5         "key_ota_md5"
#define KEY_OTA_MD5_CTX     "key_ota_md5_ctx"

#define OTA_BUFFER_MAX_SIZE  1536

static MD5_CTX            g_ctx;

static void save_state(uint32_t breakpoint, MD5_CTX *pMD5);

/**
 * @brief http_gethost_info
 *
 * @Param: src  url
 * @Param: web  WEB
 * @Param: file  download filename
 * @Param: port  default 80
 */
void http_gethost_info(char *src, char **web, char **file, int *port)
{
    char *pa;
    char *pb;
    int isHttps = 0;
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
        pa = src + strlen("https://");
        isHttps = 1;
    }
    if (!isHttps) {
        if (!strncmp(pa, "http://", strlen("http://"))) {
            pa = src + strlen("http://");
        }
    }

    pb = strchr(pa, '/');
    if (pb) {
        *web = pa;
        *pb = 0;
        //memcpy(web, pa, strlen(pa) - strlen(pb));
        if (*(pb + 1)) {
            *file = pb + 1;
            *((*file) + strlen(pb) - 1) = 0;
            //memcpy(file, pb + 1, strlen(pb) - 1);
            //file[strlen(pb) - 1] = 0;
        }
    } else {
        *web = pa;
        //memcpy(web, pa, strlen(pa));
    }
    if (pb) {
        (*web)[strlen(pa) - strlen(pb)] = 0;
    } else {
        (*web)[strlen(pa)] = 0;
    }
    pa = strchr(*web, ':');
    if (pa) {
        *pa = 0;
        *port = atoi(pa + 1);
    } else {
        if (isHttps) {
            *port = 80;//443
        } else {
            *port = 80;
        }

    }
}

int check_md5(const char *buffer, const int32_t len)
{
    uint8_t digest[16] = {0};
    char digest_str[33] = {0};
    int i = 0;
    OTA_LOG_D("digest=%s", buffer);
    MD5_Final((uint8_t *)digest, &g_ctx);
    for (; i < 16 ; i++) {
        snprintf(digest_str + i * 2, 2 + 1, "%02X", digest[i]);
    }
    OTA_LOG_I("url md5=%s", buffer);
    OTA_LOG_I("digestMD5=%s", digest_str);
    if (strncmp(digest_str, buffer, 32)) {
        OTA_LOG_E("update_packet md5 check FAIL!");
        return -1;
    }
    return 0;
}
#define HTTP_HEADER "GET /%s HTTP/1.1\r\nAccept:*/*\r\n\
User-Agent: Mozilla/5.0\r\n\
Cache-Control: no-cache\r\n\
Connection: close\r\n\
Host:%s:%d\r\n\r\n"

#define HTTP_HEADER_RESUME "GET /%s HTTP/1.1\r\nAccept:*/*\r\n\
User-Agent: Mozilla/5.0\r\n\
Cache-Control: no-cache\r\n\
Connection: close\r\n\
Range: bytes=%d-\r\n\
Host:%s:%d\r\n\r\n"

int http_download(char *url, write_flash_cb_t func, char *md5)
{
    if (!url || strlen(url) == 0 || func == NULL || md5 == NULL) {
        OTA_LOG_E("http_download url or func  error!\n");
        return OTA_DOWNLOAD_URL_FAIL;
    }
    int ret = 0;
    int sockfd = 0;
    int port = 0;
    int nbytes = 0;
    char *host_file = NULL;
    char *host_addr = NULL;
    char http_buffer[OTA_BUFFER_MAX_SIZE] = {0};
    // char host_file[OTA_URL_MAX_LEN] = {0};
    // char host_addr[256] = {0};
    int send = 0;
    int totalsend = 0;
    uint32_t breakpoint = 0;
    char last_md5[33] = {0};
    http_gethost_info(url, &host_addr, &host_file, &port);
    //  OTA_LOG_I("host_addr is: %s\n ", host_addr);
    //  OTA_LOG_I("host_file is: %s\n ", host_file);
    //  OTA_LOG_I("port is: %d\n ", port);
    if (host_file == NULL || host_addr == NULL) {
        ret = OTA_DOWNLOAD_URL_FAIL;
        return ret;
    }

    sockfd = ota_socket_connect(port, host_addr);
    if (sockfd < 0 ) {
        OTA_LOG_E("http_socket_init error\n ");
        ret = OTA_DOWNLOAD_SOCKET_FAIL;
        return ret;
    }
    breakpoint = ota_get_update_breakpoint();
    ota_get_last_MD5(last_md5);
    OTA_LOG_I("----breakpoint=%d------", breakpoint);
    if (breakpoint && !strncmp(last_md5, md5, 32)) {
        OTA_LOG_I("----resume download------");
        sprintf(http_buffer, HTTP_HEADER_RESUME, host_file, breakpoint, host_addr, port);
        ota_get_last_MD5_context(&g_ctx);
    } else {
        breakpoint = 0;
        sprintf(http_buffer, HTTP_HEADER, host_file, host_addr, port);
        MD5_Init(&g_ctx);
    }
    ota_set_cur_MD5(md5);
    send = 0;
    totalsend = 0;
    nbytes = strlen(http_buffer);
    OTA_LOG_I("send %s", http_buffer);
    while (totalsend < nbytes) {
        send = ota_socket_send(sockfd, http_buffer + totalsend, nbytes - totalsend);
        if (send == -1) {
            OTA_LOG_E("send error!%s\n ", strerror(errno));
            ret = OTA_DOWNLOAD_SEND_FAIL;
            goto DOWNLOAD_END;
        }
        totalsend += send;
        OTA_LOG_I("%d bytes send OK!\n ", totalsend);
    }
    int size = 0;
    memset(http_buffer, 0, OTA_BUFFER_MAX_SIZE);
    // char headbuf[OTA_BUFFER_MAX_SIZE + 1] = {0};
    int header_found = 0;
    char *pos = 0;
    int file_size = 0;
    int retry_cnt = 0;


    while ((nbytes = ota_socket_recv(sockfd, http_buffer, OTA_BUFFER_MAX_SIZE))) {
        //aos_msleep(25);//for slow-motion test
        if (nbytes < 0) {
            OTA_LOG_I("ota_socket_recv nbytes < 0");
            if (errno != EINTR) {
                break;
            }
            if (ota_socket_check_conn(sockfd) < 0) {
                OTA_LOG_E("download system error %s" , strerror(errno));
                break;
            } else if (retry_cnt++ < 20) {
                continue;
            } else {
                break;
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

            if (!pos) {
                //header pos
                //memcpy(headbuf, http_buffer, OTA_BUFFER_MAX_SIZE);
            } else {
                pos += 4;
                int len = pos - http_buffer;
                header_found = 1;
                size = nbytes - len;
                //memcpy(headbuf, http_buffer, len);
                //OTA_LOG_I("headbuf=%s",headbuf);
                MD5_Update(&g_ctx, (const uint8_t *)pos, size);
                func(OTA_BUFFER_MAX_SIZE, (uint8_t *)pos, size, 0);
            }
            //OTA_LOG_I("headbuf %s", headbuf);
            continue;
        }

        size += nbytes;
        //OTA_LOG_I("size nbytes %d, %d", size, nbytes);
        MD5_Update(&g_ctx, (const uint8_t *) http_buffer, nbytes);
        func(OTA_BUFFER_MAX_SIZE, (uint8_t *) http_buffer, nbytes, 0);
        memset(http_buffer, 0, OTA_BUFFER_MAX_SIZE);

        if (size == file_size) {
            nbytes = 0;
            break;
        }

        if (ota_get_status() == OTA_CANCEL) {
            break;
        }
    }

    if (nbytes < 0) {
        OTA_LOG_E("download read error %s" , strerror(errno));
        save_state(size + breakpoint, &g_ctx);
        ret = OTA_DOWNLOAD_FAILED;
    } else if (nbytes == 0) {
        ota_set_update_breakpoint(0);
        ret = OTA_DOWNLOAD_FINISH;
    } else {
        save_state(size + breakpoint, &g_ctx);
        ret = OTA_DOWNLOAD_CANCEL;
    }

DOWNLOAD_END:
    ota_socket_close(sockfd);
    return ret;
}

static void save_state(uint32_t breakpoint, MD5_CTX *pMD5)
{
    ota_set_update_breakpoint(breakpoint);
    ota_set_cur_MD5_context(pMD5);
}

uint32_t ota_get_update_breakpoint()
{
    uint32_t offset = 0;
    int len = 4;

    if (aos_kv_get(KEY_OTA_BREAKPOINT, &offset, &len)) {
        offset = 0;
    }
    //OTA_LOG_I("ota_get_update_breakpoint=%d",offset);
    return offset;
}

int ota_set_update_breakpoint(uint32_t offset)
{
    //OTA_LOG_I("ota_set_update_breakpoint=%d",offset);
    return  aos_kv_set(KEY_OTA_BREAKPOINT, &offset, 4, 1);
}

int ota_get_last_MD5(char *value)
{
    int len = 33;
    int ret = aos_kv_get(KEY_OTA_MD5, value, &len);
    return ret;
}

int ota_set_cur_MD5(char *value)
{
    return  aos_kv_set(KEY_OTA_MD5, value, 33, 1);
}

int ota_get_last_MD5_context(MD5_CTX *md5ctx)
{
    int len = sizeof(MD5_CTX);
    int ret = aos_kv_get(KEY_OTA_MD5_CTX, md5ctx, &len);
    return ret;
}

int ota_set_cur_MD5_context(MD5_CTX *md5ctx)
{
    return  aos_kv_set(KEY_OTA_MD5_CTX, md5ctx, sizeof(MD5_CTX), 1);
}

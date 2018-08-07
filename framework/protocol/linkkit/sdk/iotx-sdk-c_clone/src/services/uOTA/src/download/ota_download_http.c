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
#include "ota_socket.h"
#include "ota_verify.h"
#include "ota_hal_os.h"

#define OTA_BUFFER_MAX_SIZE  1461

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
        pa = src + strlen("https://");
        isHttps = 1;
    }
    if (!isHttps) {
        if (!strncmp(pa, "http://", strlen("http://"))) {
            pa = src + strlen("http://");
        }
    }
    *web = pa;
    pb = strchr(pa, '/');
    if (pb) {
        *pb = 0;
        pb += 1;
        if (*pb) {
            *file = pb;
            *((*file) + strlen(pb)) = 0;
        }
    } else {
        (*web)[strlen(pa)] = 0;
    }

    pa = strchr(*web, ':');
    if (pa) {
        *pa = 0;
        *port = atoi(pa + 1);
    } else {
        if (isHttps) {
            *port = 443;
        } else {
            *port = 80;
        }
    }
}

static int ota_download_start(char *url, ota_write_cb_t wcb, void* cur_sign)
{
    if (!url || strlen(url) == 0 || wcb == NULL || cur_sign == NULL) {
        OTA_LOG_E("ota_download parms error!\n");
        return OTA_DOWNLOAD_URL_FAIL;
    }
    int ret = 0;
    int sockfd = 0;
    int port = 0;
    int nbytes = 0;
    int send = 0;
    int totalsend = 0;
    uint32_t breakpoint = 0;
    int size = 0;
    int header_found = 0;
    char *pos = 0;
    int file_size = 0;
    ota_sign_params last_sign;
    ota_sign_ctx_params *sign_ctx = NULL;
    char *host_file = NULL;
    char *host_addr = NULL;
    char http_buffer[OTA_BUFFER_MAX_SIZE] = {0};
    void* ssl = NULL;
    http_gethost_info(url, &host_addr, &host_file, &port);

    if (host_file == NULL || host_addr == NULL) {
        ret = OTA_DOWNLOAD_URL_FAIL;
        return ret;
    }

    sockfd = ota_socket_connect(host_addr, port);
    if (sockfd < 0 ) {
        OTA_LOG_E("ota_socket_connect error\n ");
        ret = OTA_DOWNLOAD_SOCKET_FAIL;
        return ret;
    }
    if(isHttps) {
        const char* ca = ota_iotx_ca_get();
        ssl = ota_ssl_connect(sockfd,ca, strlen(ca));
        if (ssl < 0 ) {
           OTA_LOG_E("ota_ssl_socket_connect error\n ");
           ret = OTA_DOWNLOAD_SOCKET_FAIL;
           return ret;
        }
    }
    sign_ctx = ota_get_global_sign_context();
    if (sign_ctx == NULL || sign_ctx->ctx_sign == NULL || sign_ctx->ctx_size == 0) {
        OTA_LOG_E("ota get sign ctx fail\n ");
        ret = OTA_DOWNLOAD_FAIL;
        return ret;
    }
    breakpoint = ota_get_update_breakpoint();
    memset(&last_sign, 0x00, sizeof last_sign);
    ota_get_last_sign((char *)&last_sign);

    if (breakpoint && (ota_verify_sign(last_sign, (*((ota_sign_params*)cur_sign))) == 0)) {
        OTA_LOG_I("----resume download,breakpoint=%d------", breakpoint);
        sprintf(http_buffer, HTTP_HEADER_RESUME, host_file, breakpoint, host_addr, port);
        ota_get_last_sign_context(sign_ctx);
    } else {
        breakpoint = 0;
        sprintf(http_buffer, HTTP_HEADER, host_file, host_addr, port);
        if (ali_hash_init(sign_ctx->sign_method, sign_ctx->ctx_sign) != 0) {
            OTA_LOG_E("ota sign init fail \n ");
            ret = OTA_DOWNLOAD_SIGN_INIT_FAIL;
            return ret;
        }
    }
    ota_set_cur_sign((char *)cur_sign);
    send = 0;
    totalsend = 0;
    nbytes = strlen(http_buffer);
    OTA_LOG_I("send %s", http_buffer);
    while (totalsend < nbytes) {
        send = ((isHttps)? ota_ssl_send(ssl, (char *)(http_buffer + totalsend),(int)(nbytes - totalsend)):ota_socket_send(sockfd, http_buffer + totalsend, nbytes - totalsend));;
        if (send < 0) {
            OTA_LOG_E("send error!%s\n ", strerror(errno));
            ret = OTA_DOWNLOAD_SEND_FAIL;
            goto DOWNLOAD_END;
        }
        totalsend += send;
        OTA_LOG_I("%d bytes send OK!\n ", totalsend);
    }

    memset(http_buffer, 0, OTA_BUFFER_MAX_SIZE);
    while ((nbytes = ((isHttps)?ota_ssl_recv(ssl, http_buffer, OTA_BUFFER_MAX_SIZE - 1):ota_socket_recv(sockfd, http_buffer, OTA_BUFFER_MAX_SIZE - 1))) != 0) {
        if (nbytes < 0) {
            OTA_LOG_I("ota_socket_recv nbytes < 0");
            if (errno != EINTR) {
                break;
            }
            if (ota_socket_check_conn(sockfd) < 0) {
                OTA_LOG_E("download system error %s" , strerror(errno));
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
                int len = pos - http_buffer;
                header_found = 1;
                size = nbytes - len;
                //memcpy(headbuf, http_buffer, len);
                //OTA_LOG_I("headbuf=%s",headbuf);
                if (ALI_CRYPTO_SUCCESS != ali_hash_update((const uint8_t *)pos, size, sign_ctx->ctx_sign)) {
                    OTA_LOG_E("ota hash update fail\n ");
                    ret = OTA_DOWNLOAD_SEND_FAIL;
                    goto DOWNLOAD_END;
                }
	        ret = wcb(OTA_BUFFER_MAX_SIZE, (uint8_t *)pos, size, 0); 
		if(ret < 0) {
		    OTA_LOG_I("write error:%d\n",ret);
		    ret = OTA_DOWNLOAD_FAIL;
		    goto DOWNLOAD_END;
		}
            }
            memset(http_buffer, 0, OTA_BUFFER_MAX_SIZE);
            continue;
        }

        size += nbytes;
        //OTA_LOG_I("size nbytes %d, %d", size, nbytes);
        if (ALI_CRYPTO_SUCCESS != ali_hash_update((const uint8_t *)http_buffer, nbytes, sign_ctx->ctx_sign)) {
            OTA_LOG_E("ota hash update fail\n ");
            ret = OTA_DOWNLOAD_SEND_FAIL;
            goto DOWNLOAD_END;
        }
        ret = wcb(OTA_BUFFER_MAX_SIZE, (uint8_t *) http_buffer, nbytes, 0);
        if(ret < 0) {
            OTA_LOG_I("write error:%d\n",ret);
	    ret = OTA_DOWNLOAD_FAIL;
	    goto DOWNLOAD_END;
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
        OTA_LOG_E("download read error %s" , strerror(errno));
        ota_save_state(size + breakpoint, sign_ctx);
        ret = OTA_DOWNLOAD_FAILED;
    } else if (nbytes == 0) {
        ota_set_update_breakpoint(0);
        ret = OTA_DOWNLOAD_FINISH;
    } else {
        ota_save_state(size + breakpoint, sign_ctx);
        ret = OTA_DOWNLOAD_CANCEL;
    }
DOWNLOAD_END:
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
    .init = ota_download_init,
    .start = ota_download_start,
    .stop = ota_download_stop,
};

const void * ota_get_download_http(void) {
    return &dl_http;
}

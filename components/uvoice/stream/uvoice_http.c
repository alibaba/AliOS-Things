/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#ifdef __os_alios_things__
#include <sys/socket.h>
#include <netdb.h>
#elif defined(__os_linux__)
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif

#include "uvoice_types.h"
#include "uvoice_event.h"
#include "uvoice_player.h"

#include "uvoice_os.h"
#include "uvoice_config.h"
#include "uvoice_common.h"
#include "uvoice_format.h"
#include "uvoice_play.h"

#include "uvoice_cache.h"
#include "uvoice_http.h"
#include "uvoice_hls.h"
#include "uvoice_download.h"


#if defined(ALIGENIE_ENABLE)
#define HTTP_LOAD_DEEP_PAUSE_ENABLE        1
#else
#define HTTP_LOAD_DEEP_PAUSE_ENABLE        0
#endif

#define HTTP_CACHE_ENABLE                1

#define HTTP_CONNECT_TIMEOUT_MSEC        50000
#define UVOICE_MAX_HEADER_SIZE          1024

#define HTTP_REQUEST \
    "GET %s HTTP/1.1\r\nAccept:*/*\r\n\
User-Agent: Mozilla/5.0\r\n\
Cache-Control: no-cache\r\n\
Connection: keep-alive\r\n\
Host:%s:%d\r\n\r\n"

#define HTTP_REQUEST_CONTINUE \
    "GET %s HTTP/1.1\r\nAccept:*/*\r\n\
User-Agent: Mozilla/5.0\r\n\
Cache-Control: no-cache\r\n\
Connection: keep-alive\r\n\
Range: bytes=%d-\r\n\
Host:%s:%d\r\n\r\n"

#ifndef in_addr_t
typedef uint32_t in_addr_t;
#endif

static const char *ca_crt = \
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

int http_loader_restart(http_loader_t *loader);
int http_loader_start(http_loader_t *loader);
int http_loader_stop(http_loader_t *loader);

static cache_config_t *g_cache_config;

static char *strncasestr(const char *str, const char *key)
{
    int len;

    if (!str || !key)
        return NULL;

    len = strlen(key);
    if (len == 0)
        return NULL;

    while (*str) {
        if (!strncasecmp(str, key, len))
            return str;
        ++str;
    }
    return NULL;
}

static int http_setup(http_loader_t *loader, int sock)
{
    struct timeval timeout;
    int ret;

    if (!loader) {
        M_LOGE("loader null !\n");
        return -1;
    }

    timeout.tv_sec = 10;

#ifdef UVOICE_HLS_ENABLE
    if (loader->hls && loader->hls->live_stream)
        timeout.tv_sec = 5;
#endif

    timeout.tv_usec = 0;

#ifdef UVOICE_MUSICPLAYER_ENABLE
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
#endif

    ret = setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &timeout,
        sizeof(timeout));
    if (ret) {
        M_LOGE("set send timeout failed %d!\n", ret);
        return -1;
    }

    ret = setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout,
        sizeof(timeout));
    if (ret) {
        M_LOGE("set recv timeout failed %d!\n", ret);
        return -1;
    }

#if 0
    int buffer_size;
    int opt_len = sizeof(buffer_size);
    ret = getsockopt(sock, SOL_SOCKET, SO_RCVBUF, &buffer_size,
        &opt_len);
    if (ret) {
        M_LOGE("get sockopt failed !\n");
    }
    M_LOGI("buffer_size %d\n", buffer_size);

    buffer_size = 20 * 1024;
    ret = setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &buffer_size,
        sizeof(buffer_size));
    if (ret) {
        M_LOGE("set recv buffer size failed %d!\n", ret);
        return -1;
    }
#endif

    return 0;
}

static int http_get_dns(char *hostname, in_addr_t *addr)
{
    struct hostent *host;
    struct in_addr in_addr;
    char **pp = NULL;
#if defined(UVOICE_HTTP_MULTI_RETRY_TIMES)
    int retries = UVOICE_HTTP_MULTI_RETRY_TIMES;
#else
    int retries = 1;
#endif
    if (!hostname || !addr) {
        M_LOGE("args null !\n");
        return -1;
    }

    while (1) {
        host = gethostbyname(hostname);
        if (host)
            break;
        if (--retries <= 0)
            break;
#if defined(UVOICE_HTTP_MULTI_RETRY_TIMES)
        os_msleep(100);
        M_LOGD("try again\n");
#endif
    }

    if (!host) {
        M_LOGE("get host %s failed !\n", hostname);
        return -1;
    }

    pp = host->h_addr_list;
    if (!(*pp)) {
        M_LOGE("host addr null !\n");
        return -1;
    }

    in_addr.s_addr = *((in_addr_t *)*pp);
    pp++;

    *addr = in_addr.s_addr;
    return 0;
}
extern const char *iotx_ca_get(void);
static int http_connect(http_loader_t *loader)
{
    struct sockaddr_in server;
    in_addr_t addr;
    int sock;
    int ret;

    if (!loader) {
        M_LOGE("loader null !\n");
        return -1;
    }

    if (loader->https) {
        #ifdef UVOICE_HTTPS_ENABLE
        loader->sock = net_ssl_connect(loader->host_name,
            loader->port, ca_crt, strlen(ca_crt) + 1);
        if (!loader->sock) {
            M_LOGE("ssl connect failed !\n");
            loader->sock = -1;
            return -1;
        }

        M_LOGD("https connect\n");
        return 0;

        #else
        M_LOGE("https not support !\n");
        return -1;
        #endif
    }

    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock < 0) {
        M_LOGE("create socket failed !\n");
        return -1;
    }

    if (http_setup(loader, sock)) {
        M_LOGE("set socket option failed !\n");
        close(sock);
        return -1;
    }

    if (loader->hls && loader->host_addr != INADDR_NONE) {
        /* use existing dns addr */
        addr = (in_addr_t)loader->host_addr;
    } else {
        if (http_get_dns(loader->host_name, &addr)) {
            M_LOGE("get dns failed !\n");
            close(sock);
            return -1;
        }
        loader->host_addr = addr;
    }

    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(loader->port);
    server.sin_addr.s_addr = addr;

    ret = connect(sock, (struct sockaddr *)&server,
            sizeof(struct sockaddr));
    if (ret) {
        M_LOGE("socket connect failed %d!\n", ret);
        close(sock);
        return -1;
    }

    loader->sock = sock;
    return 0;
}

static int http_disconnect(http_loader_t *loader)
{
    if (!loader) {
        M_LOGE("loader null !\n");
        return -1;
    }

    if (loader->https) {
        #ifdef UVOICE_HTTPS_ENABLE
        net_ssl_disconnect(loader->sock);
        #endif
    } else {
        close(loader->sock);
    }

    loader->sock = -1;
    return 0;
}

static int http_urlparse(http_loader_t *loader)
{
    char *ptr = NULL;
    char *begin = NULL;
    char *end = NULL;
    int len = 0;

    if (!loader) {
        M_LOGE("loader null !\n");
        return -1;
    }

    ptr = loader->redirect ? loader->redirect_url : loader->url;

    if (!strncmp(ptr, "http://", strlen("http://"))) {
        ptr += strlen("http://");
        loader->https = 0;
        loader->port = 80;
    } else if (!strncmp(ptr, "https://", strlen("https://"))) {
        ptr += strlen("https://");
        loader->https = 1;
        loader->port = 443;
    } else {
        M_LOGE("url invalid !\n");
        return -1;
    }

    begin = ptr;
    ptr = strchr(begin, '/');
    if (!ptr) {
        M_LOGE("url invaild !\n");
        return -1;
    }
    end = ptr;

    len = MIN(end - begin, HTTP_HOSTNAME_LEN - 1);
    memcpy(loader->host_name, begin, len);
    loader->host_name[len] = '\0';

    ptr = strchr(begin, ':');
    if (ptr && ptr < end) {
        loader->port = atoi(ptr + 1);
        if (loader->port <= 0 || loader->port >= 65535) {
            M_LOGE("port invalid !\n");
            return -1;
        }
    }

    begin = end;
    len = MIN(HTTP_URL_LEN - 1, strlen(begin));
    memcpy(loader->uri, begin, len);
    loader->uri[len] = '\0';

    return 0;
}

static int http_request(http_loader_t *loader)
{
    net_cache_t *nc;
    char *header_buffer;
    char *ptr;
    int read_size;
    int ret;

    if (!loader) {
        M_LOGE("loader null !\n");
        return -1;
    }

    nc = loader->nc;
    if (!nc) {
        M_LOGE("nc null !\n");
        return -1;
    }

    if ((!nc->buffer) || (0 == nc->buffer_size)) {
        M_LOGE("nc buffer null !\n");
        return -1;
    }

    memset(nc->buffer, 0, nc->buffer_size);

    if (loader->breakpoint_pos <= 0) {
        snprintf(nc->buffer, nc->buffer_size,
            HTTP_REQUEST, loader->uri,
            loader->host_name, loader->port);
    } else {
        snprintf(nc->buffer, nc->buffer_size,
            HTTP_REQUEST_CONTINUE, loader->uri,
            loader->breakpoint_pos,
            loader->host_name, loader->port);
        M_LOGD("breakpoint pos %d\n", loader->breakpoint_pos);
    }

    if (loader->https) {
        #ifdef UVOICE_HTTPS_ENABLE
        ret = net_ssl_write(loader->sock, nc->buffer, strlen(nc->buffer), 5000);
        #else
        M_LOGE("https not support !\n");
        return -1;
        #endif
    } else {
        ret = send(loader->sock, nc->buffer,
            strlen(nc->buffer), 0);
    }
    if (ret < 0) {
        M_LOGE("send request failed %d!\n", ret);
        return -1;
    } else if (ret != strlen(nc->buffer)) {
        M_LOGW("send %d ret %d\n", strlen(nc->buffer), ret);
    }

    header_buffer = nc->buffer;
    read_size = MIN(UVOICE_MAX_HEADER_SIZE, nc->buffer_size) - 1; /* 1KB is enough */
    memset(nc->buffer, 0, nc->buffer_size);

    if (loader->https) {
        #ifdef UVOICE_HTTPS_ENABLE
        ret = net_ssl_read(loader->sock, header_buffer, read_size, 5000);
        #else
        M_LOGE("https not support !\n");
        return -1;
        #endif
    } else {
        ret = read(loader->sock, header_buffer, read_size);
    }
    if (ret <= 0) {
        M_LOGE("read header failed %d!\n", ret);
        return -1;
    } else if (ret != read_size) {
        read_size = ret;
    }

    // M_LOGD("HTTP Response:\n%s\n", header_buffer);
    header_buffer[read_size] = '\0';
    ptr = strchr(header_buffer, ' ');
    if (!ptr || !strcasestr(header_buffer, "http")) {
        M_LOGE("http response invalid !\n");
        return -1;
    }
    loader->http_status = atoi(ptr + 1);

    if (loader->breakpoint_pos <= 0) {
        if (loader->http_status != HTTP_STAT_SUCCESS &&
            loader->http_status != HTTP_CODE_PERM_REDIRECT &&
            loader->http_status != HTTP_CODE_TEMP_REDIRECT) {
            M_LOGE("http request failed %d\n",
                loader->http_status);
            return -1;
        }
    } else {
        if (loader->http_status != HTTP_CODE_BREAKPOINT &&
            loader->http_status != HTTP_CODE_PERM_REDIRECT &&
            loader->http_status != HTTP_CODE_TEMP_REDIRECT) {
            M_LOGE("http request failed %d\n",
                loader->http_status);
            return -1;
        }
    }

    ptr = strncasestr(header_buffer, "Location: ");
    if (ptr) {
        ptr += strlen("Location: ");
        char *loc_end_ptr = ptr;
        while (strncmp(loc_end_ptr, "\r\n", strlen("\r\n"))) {
            loc_end_ptr++;
            if (loc_end_ptr - header_buffer >= read_size - 1) {
                loc_end_ptr = NULL;
                break;
            }
        }

        if (loc_end_ptr) {
            int copy_size = loc_end_ptr - ptr;
            if (copy_size > HTTP_URL_LEN - 1) {
                M_LOGE("redirect url length %d overrange !\n",
                    copy_size);
                return -1;
            }
            memset(loader->redirect_url, 0, HTTP_URL_LEN);
            memcpy(loader->redirect_url, ptr, copy_size);
            loader->redirect_url[copy_size] = '\0';
            loader->redirect = 1;
            M_LOGD("redirect %s\n", loader->redirect_url);
            return 1;
        }
    }

    ptr = strncasestr(header_buffer, "Content-length: ");
    if (ptr) {
        ptr += strlen("Content-length: ");
        if (loader->breakpoint_pos <= 0) {
            loader->content_length = atoi(ptr);
            if (loader->content_length <= 0) {
                M_LOGE("content length %d invalid !\n",
                    loader->content_length);
                return -1;
            }
            nc->content_length = loader->content_length;
            nc->load_length = 0;
            M_LOGD("content len %d\n", loader->content_length);
        } else {
            M_LOGD("remaining content len %d\n", atoi(ptr));
            nc->content_length = loader->content_length;
            nc->load_length = loader->breakpoint_pos;
        }
    }

    ptr = strncasestr(header_buffer, "Content-Range: ");
    if (ptr) {
        ptr += strlen("Content-Range: ");
        char *range_end_ptr = ptr;
        while (strncmp(range_end_ptr, "\r\n", strlen("\r\n"))) {
            range_end_ptr++;
            if (range_end_ptr - header_buffer >= read_size) {
                range_end_ptr = NULL;
                break;
            }
        }

        if (range_end_ptr) {
            int range_size = range_end_ptr - ptr;
            char range_info[range_size + 1];
            memcpy(range_info, ptr, range_size);
            range_info[range_size] = '\0';
            M_LOGD("content range: %s\n", range_info);
        }
    }

    ptr = strncasestr(header_buffer, "Transfer-Encoding");
    if (ptr) {
        if (strncasestr(header_buffer, "chunked")) {
            loader->chunked = 1;
            M_LOGD("http chunk enable\n");
        }
    }

    ptr = strstr(header_buffer, "\r\n\r\n");
    if (ptr) {
        ptr += strlen("\r\n\r\n");
        nc->head_data_size = read_size - (ptr - header_buffer);
        if (nc->head_data_size > 0)
            snd_memmove(nc->buffer, ptr, nc->head_data_size);
        M_LOGD("head data size %d\n", nc->head_data_size);
    } else {
        M_LOGE("http header error !\n");
        return -1;
    }
    return 0;
}

static int http_read(void *priv, uint8_t *buffer, int nbytes)
{
    http_loader_t *loader = (http_loader_t *)priv;
    int read_bytes = 0;
    int ret;
    bool last = false;

    if (!loader) {
        M_LOGE("loader null !\n");
        return -1;
    }

    if (loader->force_restart) {
        loader->force_restart = 0;
        if (loader->nc && loader->nc->offset > 0)
            loader->nc->load_length -= loader->nc->offset;

        if (http_loader_restart(loader)) {
            M_LOGE("restart http load failed !\n");
            return -1;
        }

        if (loader->nc && loader->nc->head_data_size > 0) {
            M_LOGD("read head data %u\n",
                loader->nc->head_data_size);
            read_bytes += loader->nc->head_data_size;
            loader->nc->head_data_size = 0;
        }
    }

    while (read_bytes < nbytes) {
        if (loader->https) {
            #ifdef UVOICE_HTTPS_ENABLE
            ret = net_ssl_read(loader->sock,
                buffer + read_bytes, nbytes - read_bytes, 5000);
            #else
            M_LOGE("https not support !\n");
            return -1;
            #endif
        } else {
            #ifdef MUSICBOX_APP
            ret = recv(loader->sock,
                buffer + read_bytes, nbytes - read_bytes, 0);
            #else
            ret = read(loader->sock,
                buffer + read_bytes, nbytes - read_bytes);
            #endif
        }
        if (ret > 0) {
            read_bytes += ret;
        } else if (ret == 0) {
            M_LOGD("read end\n");
            break;
        } else {
#if !defined(MUSICBOX_APP)
            M_LOGE("read failed %d! read %d/%d\n",
                errno, read_bytes, nbytes);
#endif

            if (last)
                break;

            read_bytes = 0;

            if (loader->nc && loader->nc->offset > 0)
                loader->nc->load_length -= loader->nc->offset;

            if (http_loader_restart(loader)) {
                M_LOGE("restart http load failed !\n");
                return -1;
            }

            if (loader->nc && loader->nc->head_data_size > 0) {
                M_LOGD("read head data %u\n",
                    loader->nc->head_data_size);
                read_bytes += loader->nc->head_data_size;
                loader->nc->head_data_size = 0;
            }

            if (loader->cache_on) {
                if (loader->nc->load_length + nbytes >= loader->content_length)
                    last = true;
            } else {
                if (loader->load_pos + nbytes >= loader->content_length)
                    last = true;
            }
        }
    }

    return read_bytes;
}

int http_cache_config(cache_config_t *config)
{
    if (!g_cache_config) {
        g_cache_config = snd_zalloc(sizeof(cache_config_t), AFM_MAIN);
        if (!g_cache_config) {
            M_LOGE("alloc cache config fail !\n");
            return -1;
        }
    }

    memcpy(g_cache_config, config, sizeof(cache_config_t));
    M_LOGD("set cache config\n");
    return 0;
}

static int http_source_format(void *priv, media_format_t *format)
{
    media_loader_t *mloader = (media_loader_t *)priv;
    http_loader_t *loader;
    net_cache_t *nc;
    int read_size;
    media_format_t temp = MEDIA_FMT_UNKNOWN;
    int ret;

    if (!mloader) {
        M_LOGE("mloader null !\n");
        return -1;
    }

    loader = mloader->loader;
    if (!loader) {
        M_LOGE("loader null !\n");
        return -1;
    }

    nc = loader->nc;
    if (!nc) {
        M_LOGE("nc null !\n");
        return -1;
    }

    if (!loader->start) {
        if (http_loader_start(loader)) {
            M_LOGE("start http load failed !\n");
            return -1;
        }
        loader->start = 1;
    }

    if (loader->hls)
        format_parse_byname(loader->url, &temp);
    else if (strlen(loader->redirect_url) > strlen("http://"))
        format_parse_byname(loader->redirect_url, &temp);
    if (temp != MEDIA_FMT_UNKNOWN) {
        *format = temp;
        return 0;
    }

    read_size = MIN(nc->buffer_size - nc->head_data_size,
        nc->content_length);
    ret = http_read(loader,
        nc->buffer + nc->head_data_size, read_size);
    if (ret < 0) {
        M_LOGE("read %d failed !\n", ret);
        return -1;
    } else if (ret != read_size) {
        M_LOGW("read %d ret %d\n", read_size, ret);
    }
    nc->head_data_size += ret;

    if (mp3_format_check(nc->buffer, nc->head_data_size)) {
        *format = MEDIA_FMT_MP3;
        M_LOGD("format MP3\n");
    } else if (flac_format_check(nc->buffer, nc->head_data_size)) {
        *format = MEDIA_FMT_FLAC;
        M_LOGD("format FLAC\n");
    } else if (wav_format_check(nc->buffer, nc->head_data_size)) {
        *format = MEDIA_FMT_WAV;
        M_LOGD("format WAV\n");
    } else if (aac_format_check(nc->buffer, nc->head_data_size)) {
        *format = MEDIA_FMT_AAC;
        M_LOGD("format AAC\n");
    } else if (m4a_format_check(nc->buffer, nc->head_data_size)) {
        *format = MEDIA_FMT_M4A;
        M_LOGD("format M4A\n");
    } else if (ogg_format_check(nc->buffer, nc->head_data_size)) {
        *format = MEDIA_FMT_OGG;
        M_LOGD("format OGG\n");
    } else if (wma_format_check(nc->buffer, nc->head_data_size)) {
        *format = MEDIA_FMT_WMA;
        M_LOGD("format WMA\n");
    } else if (amrwb_format_check(nc->buffer, nc->head_data_size)) {
        *format = MEDIA_FMT_AMRWB;
        M_LOGD("format AMR-WB\n");
    } else if (amr_format_check(nc->buffer, nc->head_data_size)) {
        *format = MEDIA_FMT_AMR;
        M_LOGD("format AMR\n");
    } else {
        if (loader->hls)
            format_parse_byname(loader->url, format);
        else if (strlen(loader->redirect_url) > strlen("http://"))
            format_parse_byname(loader->redirect_url, format);
    }

    return 0;
}

static int http_get_cacheinfo(void *priv, cache_info_t *info)
{
    media_loader_t *mloader = (media_loader_t *)priv;
    http_loader_t *loader;

    if (!mloader) {
        M_LOGE("mloader null !\n");
        return -1;
    }

    if (!info) {
        M_LOGE("info null !\n");
        return -1;
    }

    loader = mloader->loader;
    if (!loader) {
        M_LOGE("loader null !\n");
        return -1;
    }

    if (!loader->nc) {
        M_LOGE("loader->nc null !\n");
        return -1;
    }

    if (loader->nc &&
        net_get_cacheinfo(loader->nc, info)) {
        M_LOGE("get cache info failed !\n");
        return -1;
    }

    info->seek_forward_limit = loader->content_length -
            loader->load_pos - loader->nc->buffer_size;
    info->seek_backward_limit = loader->load_pos;

    return 0;
}

static int http_get_mediainfo(void *priv,
        media_info_t *info, media_format_t format)
{
    media_loader_t *mloader = (media_loader_t *)priv;
    http_loader_t *loader;

    if (!mloader) {
        M_LOGE("mloader null !\n");
        return -1;
    }

    loader = mloader->loader;
    if (!loader) {
        M_LOGE("loader null !\n");
        return -1;
    }

    if (loader->nc &&
        net_get_mediainfo(loader->nc, info, format)) {
        M_LOGE("get media info failed !\n");
        return -1;
    }
    return 0;
}

static int http_get_filename(http_loader_t *loader)
{
    char *url;
    char *suffix;
    char *name_begin;
    char *name_end = NULL;
    int name_len;

    if (!loader) {
        M_LOGE("loader null !\n");
        return -1;
    }

    if (!loader->nc) {
        M_LOGE("nc null !\n");
        return -1;
    }

    url = loader->url;
    if (!url) {
        M_LOGE("url null !\n");
        return -1;
    }

    suffix = strstr(url, ".mp3");
    if (!suffix) {
        suffix = strstr(url, ".MP3");
        if (suffix) {
            name_end = suffix + 3;
        }
    } else {
        name_end = suffix + 3;
    }

    if (!name_end) {
        suffix = strstr(url, ".wav");
        if (!suffix) {
            suffix = strstr(url, ".WAV");
            if (suffix) {
                name_end = suffix + 3;
            }
        } else {
            name_end = suffix + 3;
        }
    }

    if (!name_end) {
        suffix = strstr(url, ".m4a");
        if (!suffix) {
            suffix = strstr(url, ".M4A");
            if (suffix) {
                name_end = suffix + 3;
            }
        } else {
            name_end = suffix + 3;
        }
    }

    if (!name_end) {
        suffix = strstr(url, ".pcm");
        if (!suffix) {
            suffix = strstr(url, ".PCM");
            if (suffix) {
                name_end = suffix + 3;
            }
        } else {
            name_end = suffix + 3;
        }
    }

    if (!name_end)
        return -1;

    for (name_begin = suffix - 1;
            name_begin >= url; name_begin--) {
        if (*name_begin == '.') {
            M_LOGD("replace sensitive character\n");
            *name_begin = '_';
        }

        if (*name_begin == '/') {
            name_begin++;
            break;
        }
    }

    if (name_begin == url || name_begin == suffix) {
        M_LOGE("find source name failed !\n");
        return -1;
    }

    name_len = name_end - name_begin + 1;
    if (name_len >= sizeof(loader->nc->filename)) {
        M_LOGW("name length %d overrange\n", name_len);
        name_len = sizeof(loader->nc->filename) - 1;
    }

    memcpy(loader->nc->filename,
        name_end - name_len + 1, name_len);
    loader->nc->filename[name_len] = '\0';
    return 0;
}

static int http_loader_reset(void *priv)
{
    media_loader_t *mloader = (media_loader_t *)priv;
    http_loader_t *loader;

    if (!mloader) {
        M_LOGE("mloader null !\n");
        return -1;
    }

    loader = mloader->loader;
    if (!loader) {
        M_LOGE("loader null !\n");
        return -1;
    }

    if (net_cache_reset(loader->nc)) {
        M_LOGE("cache reset failed !\n");
        return -1;
    }

    loader->http_status = 0;
    loader->sock = -1;
    loader->port = 0;
    loader->start = 0;
    loader->chunked = 0;
    loader->load_pos = 0;
    loader->breakpoint_pos = 0;
    loader->redirect = 0;
    loader->download = 0;
    memset(loader->host_name, 0, sizeof(loader->host_name));
    memset(loader->uri, 0, sizeof(loader->uri));
    memset(loader->url, 0, sizeof(loader->url));
    memset(loader->redirect_url, 0, sizeof(loader->redirect_url));

    M_LOGD("http load reset\n");
    return 0;
}

static int http_loader_event(void *priv, enum cache_event event, void *arg)
{
    http_loader_t *loader = (http_loader_t *)priv;
    media_loader_t *mloader;

    if (!loader) {
        M_LOGE("loader null !\n");
        return -1;
    }

    mloader = loader->priv;
    if (!mloader) {
        M_LOGE("mloader null !\n");
        return -1;
    }

    switch (event) {
        case CACHE_EV_CACHE_CPLT:
        if (loader->hls) {
#ifdef UVOICE_HLS_ENABLE
            if (uvoice_hls_file_next(loader)) {
                M_LOGE("request next seq failed !\n");
                return -1;
            }
#endif
        } else {
            uvoice_event_post(UVOICE_EV_PLAYER,
                UVOICE_CODE_PALYER_CACHE_CPLT, 0);
        }
        break;
        case CACHE_EV_SEEK_DONE:
        if (mloader->message)
            mloader->message(mloader->priv,
                PLAYER_MSG_SEEK_DONE, arg);
        loader->load_pos += (int)arg;
        break;
        case CACHE_EV_READ_BLOCK:
        if (mloader->message)
            mloader->message(mloader->priv,
                PLAYER_MSG_LOAD_BLOCK, arg);
        break;
        default:
        break;
    }

    return 0;
}

int http_loader_start(http_loader_t *loader)
{
    int retries;
    int ret;

    if (!loader) {
        M_LOGE("loader null !\n");
        return -1;
    }

__redirect:
    ret = http_urlparse(loader);
    if (ret) {
        M_LOGE("parse url failed !\n");
        return -1;
    }

#if defined(UVOICE_HTTP_MULTI_RETRY_TIMES)
    retries = UVOICE_HTTP_MULTI_RETRY_TIMES;
#else
    retries = 1;
#endif
    while (retries-- > 0) {
        if (loader->nc->cache_stop || loader->read_paused)
            return -1;
        ret = http_connect(loader);
        if (ret) {
            M_LOGE("connect socket failed ! retries %d\n", retries);
            if (retries - 1 <= 0) {
                return -1;
            }
#if defined(UVOICE_HTTP_MULTI_RETRY_TIMES)
            if (loader->nc->cache_stop)
                return -1;
            continue;
#endif
        }
        break;
    }

#if defined(UVOICE_HTTP_MULTI_RETRY_TIMES)
    retries = UVOICE_HTTP_MULTI_RETRY_TIMES;
#else
    retries = 1;
#endif
    while (retries-- > 0) {
        if (loader->nc->cache_stop || loader->read_paused) {
            http_disconnect(loader);
            return -1;
        }
        ret = http_request(loader);
        if (ret < 0) {
            M_LOGE("http request failed ! retries %d\n", retries);
            if (retries - 1 <= 0) {
                http_disconnect(loader);
                return -1;
            }
#if defined(UVOICE_HTTP_MULTI_RETRY_TIMES)
            os_msleep(200);
            continue;
#endif
        } else if (ret == 1) {
            http_disconnect(loader);
            goto __redirect;
        }
        break;
    }

    M_LOGD("http load start\n");
    return 0;
}

int http_loader_stop(http_loader_t *loader)
{
    if (!loader) {
        M_LOGE("loader null !\n");
        return -1;
    }

    M_LOGD("http load stop\n");
    return http_disconnect(loader);
}

int http_loader_restart(http_loader_t *loader)
{
    if (!loader) {
        M_LOGE("loader null !\n");
        return -1;
    }

    http_loader_stop(loader);
    loader->start = 0;
    loader->chunked = 0;
    loader->redirect = 0;
    if (loader->cache_on)
        loader->breakpoint_pos = loader->nc ?
            loader->nc->load_length : loader->load_pos;
    else
        loader->breakpoint_pos = loader->load_pos;

    M_LOGD("restart url %s\n", loader->url);
    if (http_loader_start(loader)) {
        M_LOGE("start http load failed !\n");
        loader->breakpoint_pos = loader->load_pos;
        return -1;
    }
    loader->breakpoint_pos = 0;

    loader->start = 1;
    return 0;
}

static int http_loader_read(void *priv, uint8_t *buffer, int nbytes)
{
    media_loader_t *mloader = (media_loader_t *)priv;
    http_loader_t *loader;
    net_cache_t *nc;
    int rd_ret;

    if (!mloader) {
        M_LOGE("mlaoder null !\n");
        return -1;
    }

    loader = mloader->loader;
    if (!loader) {
        M_LOGE("loader null !\n");
        return -1;
    }

    nc = loader->nc;
    if (!nc) {
        M_LOGE("nc null !\n");
        return -1;
    }

    if (loader->seek_offset != 0) {
        http_loader_event(loader, CACHE_EV_SEEK_DONE,
            (void *)loader->seek_offset);
        loader->seek_offset = 0;
    }

    if (nc->cache_read) {
        rd_ret = loader->nc->cache_read(nc, buffer, nbytes);
    } else {
        if (nc->head_data_size > 0) {
            if (nc->head_data_size >= nbytes) {
                M_LOGD("read head data %u\n", nbytes);
                snd_memcpy(buffer, nc->buffer, nbytes);
                nc->head_data_size -= nbytes;
                snd_memmove(nc->buffer, nc->buffer + nbytes,
                    nc->head_data_size);
                rd_ret = nbytes;
            } else {
                M_LOGD("read head data %u\n", nc->head_data_size);
                snd_memcpy(buffer, nc->buffer, nc->head_data_size);
                rd_ret = nc->head_data_size;
                rd_ret += http_read(loader,
                    buffer + nc->head_data_size,
                    nbytes - nc->head_data_size);
                nc->head_data_size = 0;
            }
        } else {
            rd_ret = http_read(loader, buffer, nbytes);
        }

    }
    if (rd_ret < 0) {
        M_LOGE("read failed %d!\n", rd_ret);
        return -1;
    } else if (rd_ret == 0) {
        M_LOGD("read end, load pos %d\n", loader->load_pos);
    }
    loader->load_pos += rd_ret;
    return rd_ret;
}

static void http_dload_task(void *arg)
{
#ifdef UVOICE_DOWNLOAD_ENABLE
    http_dload_t *dload = (http_dload_t *)arg;
    http_loader_t *loader;

    if (!dload) {
        M_LOGE("dload null !\n");
        goto __exit;
    }

    if (!dload->source) {
        M_LOGE("source null !\n");
        goto __exit;
    }

    if (!strncmp(dload->source, "https://", strlen("https://"))) {
        M_LOGW("https source can't download\n");
        goto __exit;
    }

    loader = snd_zalloc(sizeof(http_loader_t), AFM_EXTN);
    if (!loader) {
        M_LOGE("alloc http loader failed !\n");
        goto __exit;
    }

    memcpy(loader->url, dload->source,
        strlen(dload->source) + 1);

    M_LOGD("source: %s\n", loader->url);

    loader->nc = net_download_create();
    if (!loader->nc) {
        M_LOGE("create net download failed !\n");
        goto __exit1;
    }

    loader->nc->cache_load = http_read;
    loader->nc->event = http_loader_event;
    loader->nc->priv = loader;
    dload->nc = loader->nc;

    if (strlen(dload->filename) <= 0) {
        if (http_get_filename(loader)) {
            M_LOGE("get filename failed !\n");
            goto __exit2;
        }
    } else {
        memcpy(loader->nc->filename,
            dload->filename, strlen(dload->filename) + 1);
        memset(dload->filename, 0, sizeof(dload->filename));
    }

    if (http_loader_start(loader)) {
        M_LOGE("http start failed !\n");
        goto __exit2;
    }

    if (net_download_work(loader->nc)) {
        M_LOGE("download failed !\n");
        goto __exit3;
    }

    http_loader_stop(loader);
    net_download_release(loader->nc);
    snd_free(loader);

    dload->nc = NULL;
    uvoice_event_post(UVOICE_EV_PLAYER,
            UVOICE_CODE_PALYER_DLOAD_CPLT, 0);
    os_task_exit(NULL);
    return;

__exit3:
    http_loader_stop(loader);
__exit2:
    net_download_release(loader->nc);
__exit1:
    snd_free(loader);
__exit:
    loader->download = 0;
    dload->nc = NULL;
    M_LOGE("download failed !\n");
    os_task_exit(NULL);
#endif
}

static int http_loader_action(void *priv,
        player_action_t action, void *arg)
{
    media_loader_t *mloader = (media_loader_t *)priv;
    http_loader_t *loader;

    if (!mloader) {
        M_LOGE("mloader null !\n");
        return -1;
    }

    loader = mloader->loader;
    if (!loader) {
        M_LOGE("loader null !\n");
        return -1;
    }

    if (action == PLAYER_START) {
        if (!loader->nc) {
            loader->nc = net_cache_create(HTTP_READ_BUFF_SIZE,
                g_cache_config, !!loader->hls);
            if (!loader->nc) {
                M_LOGE("create net cache failed !\n");
                return -1;
            }
            loader->nc->cache_load = http_read;
            loader->nc->event = http_loader_event;
            loader->nc->priv = loader;
        }

        if (!loader->start) {
#ifdef UVOICE_HLS_ENABLE
            if (loader->hls) {
                if (loader->hls->live_stream) {
                    if (uvoice_hls_build(loader)) {
                        M_LOGE("parse seq list failed !\n");
                        net_cache_release(loader->nc);
                        return -1;
                    }
                    loader->nc->sequence = 1;
                }
            }
#endif
            if (http_loader_start(loader)) {
                M_LOGE("start http load failed !\n");
                return -1;
            }
            loader->start = 1;
        }

        if (net_cache_start(loader->nc)) {
            M_LOGE("start net cache failed !\n");
            http_loader_stop(loader);
            loader->start = 0;
            loader->chunked = 0;
            loader->redirect = 0;
            return -1;
        }

        if (mloader->message) {
            media_info_t media_info;
            memset(&media_info, 0, sizeof(media_info_t));
            media_info.duration = 0;
#ifdef UVOICE_HLS_ENABLE
            if (loader->hls)
                media_info.duration = loader->hls->duration;
#endif
            media_info.media_size = loader->nc->content_length;

            mloader->message(mloader->priv,
                PLAYER_MSG_MEDIA_INFO, &media_info);
        }
    } else if (action == PLAYER_STOP) {
        if (loader->nc) {
            if (net_cache_stop(loader->nc)) {
                M_LOGE("stop net cache failed !\n");
                return -1;
            }

            if (net_cache_reset(loader->nc)) {
                M_LOGE("cache reset failed !\n");
                return -1;
            }
        }

        if (loader->start) {
            http_loader_stop(loader);
            loader->start = 0;
            loader->chunked = 0;
            loader->redirect = 0;
        }

#ifdef UVOICE_HLS_ENABLE
        if (loader->hls) {
            if (loader->hls->live_stream) {
                memset(loader->url, 0, sizeof(loader->url));
                memcpy(loader->url, loader->hls->base,
                    strlen(loader->hls->base) + 1);
                loader->breakpoint_pos = 0;
                loader->hls->seg_current = 0;
                loader->hls->seg_begin = 0;
                loader->hls->seg_index = 0;
            } else {
                if (uvoice_hls_index_update(loader, 0)) {
                    M_LOGE("update list index failed !\n");
                    return -1;
                }
            }
        }
#endif

        loader->load_pos = 0;
        loader->breakpoint_pos = 0;
    } else if (action == PLAYER_UNBLOCK) {
        if (loader->nc) {
            M_LOGD("cache blocking cancel\n");
            if (net_cache_pause(loader->nc)) {
                M_LOGE("stop net cache failed !\n");
                return -1;
            }

            if (loader->nc->rebuild &&
                (loader->load_pos < loader->content_length ||
                loader->hls)) {
                if (net_cache_stop(loader->nc)) {
                    M_LOGE("stop net cache failed !\n");
                    return -1;
                }
            }
        }
    } else if (action == PLAYER_PAUSE) {
        if (!loader->deep_pause) {
            /* pause without net cache release */
            if (!loader->seeking) {
                loader->pause_time = os_current_time();
                loader->read_paused = 1;
                goto __exit;
            }
        }
        if (loader->nc) {
            if (net_cache_pause(loader->nc)) {
                M_LOGE("stop net cache failed !\n");
                return -1;
            }

            /* pause with net cache rebuild */
            if (loader->nc->rebuild &&
                (loader->load_pos < loader->content_length ||
                loader->hls)) {
                if (net_cache_stop(loader->nc)) {
                    M_LOGE("stop net cache failed !\n");
                    return -1;
                }
                net_cache_release(loader->nc);
                loader->nc = NULL;
                if (loader->start) {
                    http_loader_stop(loader);
                    loader->start = 0;
                    loader->chunked = 0;
                    loader->redirect = 0;
                    loader->breakpoint_pos = loader->load_pos;
                    M_LOGD("breakpoint pos %d\n", loader->breakpoint_pos);
                }

#ifdef UVOICE_HLS_ENABLE
                if (loader->hls) {
                    if (loader->hls->live_stream) {
                        memset(loader->url, 0, sizeof(loader->url));
                        memcpy(loader->url, loader->hls->base,
                            strlen(loader->hls->base) + 1);
                        loader->breakpoint_pos = 0;
                        M_LOGD("breakpoint pos %d\n", loader->breakpoint_pos);
                        loader->hls->seg_current = 0;
                        loader->hls->seg_begin = 0;
                        loader->hls->seg_index = 0;
                    } else {
                        int i;
                        os_mutex_lock(loader->hls->lock,
                            OS_WAIT_FOREVER);
                        for (i = loader->hls->seg_index; i >= 0; i--) {
                            if (loader->load_pos >
                                loader->hls->seg_offset[i])
                                break;
                        }

                        if (i < 0)
                            i = 0;

                        loader->breakpoint_pos = loader->load_pos -
                                loader->hls->seg_offset[i];
                        M_LOGD("load pos %d breakpoint pos %d index %d\n",
                                loader->load_pos,
                                loader->breakpoint_pos, i);

                        if (loader->hls->seg_index > i) {
                            loader->content_length =
                                loader->hls->seg_offset[i + 1] -
                                loader->hls->seg_offset[i];
                        }
                        os_mutex_unlock(loader->hls->lock);

                        if (uvoice_hls_index_update(loader, i)) {
                            M_LOGE("update list index failed !\n");
                            return -1;
                        }
                    }
                }
#endif
            }
        } else {
            if (loader->start) {
                http_loader_stop(loader);
                loader->start = 0;
                loader->chunked = 0;
                loader->redirect = 0;
                loader->breakpoint_pos = loader->load_pos;
                M_LOGD("breakpoint pos %d\n", loader->breakpoint_pos);
            }
        }
    } else if (action == PLAYER_RESUME) {
        if (!loader->deep_pause) {
            /* net cache not released */
            if (loader->read_paused)
                loader->read_paused = 0;
            if (!loader->seeking && loader->nc) {
                if (os_current_time() - loader->pause_time >
                    HTTP_CONNECT_TIMEOUT_MSEC)
                    loader->force_restart = 1;

                if (!loader->start) {
                    /* Some error arise during previous pause, such as restart failed */
                    if (loader->force_restart)
                        loader->force_restart = 0;

                    /* correct breakpoint pos because cache pool still valid */
                    loader->breakpoint_pos = loader->nc->load_length;
                    if (http_loader_start(loader)) {
                        M_LOGE("start http load failed !\n");
                        return -1;
                    }
                    loader->start = 1;

                    if (loader->nc->cache_cplt) {
                        if (net_cache_start(loader->nc)) {
                            M_LOGE("start net cache failed !\n");
                            http_loader_stop(loader);
                            loader->start = 0;
                            loader->chunked = 0;
                            loader->redirect = 0;
                            return -1;
                        }
                    }
                }
                goto __exit;
            }
        }
        if (!loader->nc) {
            loader->nc = net_cache_create(HTTP_READ_BUFF_SIZE,
                g_cache_config, !!loader->hls);
            if (!loader->nc) {
                M_LOGE("create net cache failed !\n");
                return -1;
            }
            loader->nc->cache_load = http_read;
            loader->nc->event = http_loader_event;
            loader->nc->priv = loader;
            loader->nc->rebuild = 1;

#ifdef UVOICE_HLS_ENABLE
            if (loader->hls) {
                if (loader->hls->live_stream) {
                    if (uvoice_hls_build(loader)) {
                        M_LOGE("parse hls failed !\n");
                        net_cache_release(loader->nc);
                        return -1;
                    }
                    loader->nc->sequence = 1;
                }
            }
#endif
        } else {
            if (net_cache_resume(loader->nc)) {
                M_LOGE("resume net cache failed !\n");
                return -1;
            }
#ifdef UVOICE_HLS_ENABLE
            if (loader->hls) {
                if (uvoice_hls_build(loader)) {
                    M_LOGE("parse seq list failed !\n");
                    net_cache_pause(loader->nc);
                    return -1;
                }
            }
#endif
        }

        if (!loader->start) {
            if (http_loader_start(loader)) {
                M_LOGE("start http load failed !\n");
                return -1;
            }
            loader->breakpoint_pos = 0;
            loader->start = 1;
        }

        if (loader->nc->rebuild) {
            if (net_cache_start(loader->nc)) {
                M_LOGE("start net cache failed !\n");
                http_loader_stop(loader);
                loader->start = 0;
                loader->chunked = 0;
                loader->redirect = 0;
                return -1;
            }
            loader->nc->rebuild = 0;
        }
    } else if (action == PLAYER_SEEK_BEGIN) {
        loader->seeking = 1;
    } else if (action == PLAYER_SEEK_END) {
        loader->seeking = 0;
    } else if (action == PLAYER_SEEK) {
        if (loader->hls) {
            M_LOGE("sequence stream seek unsupport\n");
            return -1;
        }

        int seek_offset = (int)arg;
        cache_info_t cache_info;

        M_LOGD("seek offset %d\n", seek_offset);
        if (loader->nc && seek_offset > 0) {
            memset(&cache_info, 0, sizeof(cache_info_t));
            net_get_cacheinfo(loader->nc, &cache_info);

            if (cache_info.avail_cache_size >= seek_offset)
                return net_cache_seek(loader->nc, seek_offset);
        }

        if (loader->nc && !loader->nc->rebuild && loader->start) {
            if (net_cache_stop(loader->nc)) {
                M_LOGE("stop net cache failed !\n");
                return -1;
            }
            net_cache_release(loader->nc);
            loader->nc = NULL;

            http_loader_stop(loader);
            loader->start = 0;
            loader->chunked = 0;
            loader->redirect = 0;
            loader->breakpoint_pos = loader->load_pos;
            M_LOGD("breakpoint pos %d\n", loader->breakpoint_pos);
        }

        if (seek_offset > 0 &&
            loader->load_pos + seek_offset <=
            loader->content_length - 1024) {
            loader->seek_offset = seek_offset;
            loader->breakpoint_pos =
                loader->load_pos + loader->seek_offset;
            M_LOGD("update breakpoint pos %d\n",
                loader->breakpoint_pos);
        } else if (seek_offset < 0 &&
            loader->load_pos + seek_offset >= 0) {
            loader->seek_offset = seek_offset;
            loader->breakpoint_pos =
                loader->load_pos + loader->seek_offset;
            M_LOGD("update breakpoint pos %d\n",
                loader->breakpoint_pos);
        }
    } else if (action == PLAYER_COMPLETE) {
        (void)net_cache_stop(loader->nc);
        if (loader->start) {
            http_loader_stop(loader);
            loader->start = 0;
            loader->host_addr = INADDR_NONE;
            loader->chunked = 0;
            loader->redirect = 0;
            loader->load_pos = 0;
        }
    } else if (action == PLAYER_NEXT) {
        if (loader->start) {
            M_LOGE("loader not stop !\n");
            return -1;
        }

        char *url = (char *)arg;
        if (strlen(url) + 1 > HTTP_URL_LEN) {
            M_LOGE("url length %zd overrange !\n", strlen(url));
            return -1;
        }
        memset(loader->url, 0, sizeof(loader->url));
        memcpy(loader->url, url, strlen(url) + 1);
        loader->url[strlen(url)] = '\0';

        /* net cache rebuild for different stream type */
        bool sequence = !!strstr(loader->url, ".m3u8");
        if (sequence != !!loader->hls) {
            M_LOGD("http stream type change\n");
            if (net_cache_release(loader->nc)) {
                M_LOGE("release net cache failed !\n");
                return -1;
            }

            loader->nc = net_cache_create(HTTP_READ_BUFF_SIZE,
                g_cache_config, sequence);
            if (!loader->nc) {
                M_LOGE("create net cache failed !\n");
                return -1;
            }
            loader->nc->cache_load = http_read;
            loader->nc->event = http_loader_event;
            loader->nc->priv = loader;
        }

#ifdef UVOICE_HLS_ENABLE
        if (uvoice_hls_build(loader)) {
            M_LOGE("parse seq list failed !\n");
            return -1;
        }
#endif
    } else if (action == PLAYER_DLOAD) {
#ifdef UVOICE_DOWNLOAD_ENABLE
        if (loader->hls) {
            M_LOGE("sequence stream download unsupport !\n");
            return -1;
        }
        if (!loader->download) {
            if (arg) {
                char *filename = (char *)arg;
                if (strlen(filename) > 0 &&
                    strlen(filename) < sizeof(loader->dload.filename)) {
                    memcpy(loader->dload.filename,
                        filename, strlen(filename) + 1);
                } else {
                    M_LOGE("filename length %zd overrange !\n", strlen(filename));
                    return -1;
                }
            }

            loader->dload.source = loader->url;

            M_LOGD("start download task\n");
            loader->download = 1;

            os_task_create(&loader->download_task, "uvoice_dload_task",
                http_dload_task,
                &loader->dload,
                2048, UVOICE_TASK_PRI_LOWEST);
        } else {
            M_LOGW("download already\n");
        }
#endif
    } else if (action == PLAYER_DLOAD_ABORT) {
#ifdef UVOICE_DOWNLOAD_ENABLE
        if (loader->download) {
            if (loader->dload.nc) {
                if (new_download_abort(loader->dload.nc)) {
                    M_LOGW("abort download failed\n");
                    return -1;
                }
                loader->download = 0;
                M_LOGD("abort download success\n");
            }
        }
#endif
    }

__exit:
    return 0;
}

int http_loader_create(media_loader_t *mloader, char *source)
{
    http_loader_t *loader;
    bool sequence = false;
    enum cache_type cachetype;

    if (!mloader) {
        M_LOGE("mloader null !\n");
        return -1;
    }

    if (!source) {
        M_LOGE("source null !\n");
        return -1;
    }

    if (strlen(source) + 1 > HTTP_URL_LEN) {
        M_LOGE("url length %zd overrange !\n", strlen(source));
        return -1;
    }

    loader = snd_zalloc(sizeof(http_loader_t), AFM_EXTN);
    if (!loader) {
        M_LOGE("alloc http loader failed !\n");
        return -1;
    }

    loader->sock = -1;
    loader->host_addr = INADDR_NONE;
    loader->cache_on = HTTP_CACHE_ENABLE;
    loader->deep_pause = HTTP_LOAD_DEEP_PAUSE_ENABLE;
    loader->priv = mloader;
    memcpy(loader->url, source, strlen(source));
    loader->url[strlen(source)] = '\0';

    sequence = !!strstr(loader->url, ".m3u8");

    cachetype = loader->cache_on ? PLAYER_CACHE_TYPE : CACHE_NONE;

    if (!g_cache_config) {
        g_cache_config = snd_zalloc(sizeof(cache_config_t), AFM_MAIN);
        if (!g_cache_config) {
            M_LOGE("alloc cache config fail !\n");
            snd_free(loader);
            return -1;
        }

        g_cache_config->place = loader->cache_on ? PLAYER_CACHE_TYPE : CACHE_NONE;
        if (g_cache_config->place == CACHE_FILE)
            snprintf(g_cache_config->file_path, sizeof(g_cache_config->file_path),
                "%s", PLAYER_CACHE_FILE_PATH);
        else if (g_cache_config->place == CACHE_MEM)
            g_cache_config->mem_size = PLAYER_CACHE_MEM_SIZE;
        M_LOGD("set default cache config\n");
    }

    loader->nc = net_cache_create(HTTP_READ_BUFF_SIZE,
        g_cache_config, sequence);
    if (!loader->nc) {
        M_LOGE("create net cache failed !\n");
        snd_free(loader);
        return -1;
    }
    loader->nc->cache_load = http_read;
    loader->nc->event = http_loader_event;
    loader->nc->priv = loader;

    mloader->read = http_loader_read;
    mloader->action = http_loader_action;
    mloader->get_format = http_source_format;
    mloader->get_mediainfo = http_get_mediainfo;
    mloader->get_cacheinfo = http_get_cacheinfo;
    mloader->reset = http_loader_reset;

#ifdef UVOICE_HLS_ENABLE
    if (uvoice_hls_build(loader)) {
        M_LOGE("parse hls m3u8 failed !\n");
        net_cache_release(loader->nc);
        snd_free(loader);
        return -1;
    }
#endif

    mloader->loader = loader;

    M_LOGD("http loader create\n");
    return 0;
}

int http_loader_release(media_loader_t *mloader)
{
    http_loader_t *loader;

    if (!mloader) {
        M_LOGE("mloader null !\n");
        return -1;
    }

    loader = mloader->loader;
    if (!loader) {
        M_LOGE("loader null !\n");
        return -1;
    }

#ifdef UVOICE_HLS_ENABLE
    if (loader->hls)
        uvoice_hls_release(loader);
#endif

    if (loader->start) {
        http_loader_stop(loader);
        loader->start = 0;
    }

    if (loader->nc) {
        net_cache_release(loader->nc);
        loader->nc = NULL;
    }

    snd_free(loader);
    mloader->loader = NULL;
    M_LOGD("http loader release\n");
    return 0;
}


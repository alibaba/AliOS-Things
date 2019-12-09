#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ulog/ulog.h"
#include "sds.h"
#include "http_ctx.h"
#include "http.h"
#include  "network/network.h"

#define HTTPCLIENT_MAX_SCHEME_LEN  8
#define HTTPCLIENT_MAX_HOST_LEN   64
#define HTTPCLIENT_MAX_URL_LEN    512
#define HTTPS_HDR_SIZE 128
#define REQ_MEM_SIZE 1024
#define RSP_MEM_SIZE 1024
#define RSP_TIMEOUT_MS 10000

#define TAG "httpdns"
#define HTTPDNS_ERR(fmt,arg...)   LOGE(TAG, fmt,##arg)
#define HTTPDNS_INFO(fmt,arg...)  LOGI(TAG, fmt,##arg)
#define HTTPDNS_WARN(fmt,arg...)  LOGW(TAG, fmt,##arg)

int http_ctx_init(http_ctx_t *ctx, char *url, int conn_timeout, int timeout)
{
    if((NULL == ctx) || (NULL == url)) {
        return -1;
    }

    ctx->conn_timeout = conn_timeout;
    ctx->timeout = timeout;
    ctx->url = url;
    ctx->request.size = REQ_MEM_SIZE;
    ctx->request.memory = malloc(REQ_MEM_SIZE);
    memset(ctx->request.memory, 0, RSP_MEM_SIZE);
    if (!ctx->request.memory) {
        HTTPDNS_ERR("malloc failed!");
        return -1;
    }

    ctx->response.size = RSP_MEM_SIZE;
    ctx->response.memory = malloc(RSP_MEM_SIZE);
    memset(ctx->response.memory, 0, RSP_MEM_SIZE);
    if (!ctx->response.memory) {
        HTTPDNS_ERR("malloc failed!");
        free(ctx->request.memory);
        return -1;
    }

    return 0;
}

void http_ctx_destroy(http_ctx_t *ctx)
{
    if(NULL != ctx->url) {
        sdsfree(ctx->url);
    }
    if(NULL != ctx->request.memory) {
        free(ctx->request.memory);
        ctx->request.memory = NULL;
    }

    if(NULL != ctx->response.memory) {
        free(ctx->response.memory);
        ctx->response.memory = NULL;
    }
}

int http_ctx_parse_url(const char *url, char *scheme, size_t max_scheme_len, char *host, size_t maxhost_len, int *port, char *path, size_t max_path_len)
{
    char *scheme_ptr = (char *) url;
    char *host_ptr = NULL;
    size_t scheme_len = 0;
    size_t path_len = 0;
    size_t host_len = 0;
    char *port_ptr;
    char *path_ptr;
    char *fragment_ptr;

    if (url == NULL) {
        HTTPDNS_ERR("Could not find url");
        return -1;
    }

    host_ptr = (char *) strstr(url, "://");

    if (host_ptr == NULL) {
        HTTPDNS_ERR("Could not find host");
        return -1; /* URL is invalid */
    }

    if ( max_scheme_len < host_ptr - scheme_ptr + 1 ) { /* including NULL-terminating char */
        HTTPDNS_ERR("Scheme str is too small (%d >= %d)", max_scheme_len, host_ptr - scheme_ptr + 1);
        return -1;
    }

    host_ptr += 3;
    memcpy(host, scheme_ptr, host_ptr - scheme_ptr);
    //scheme[host_ptr - scheme_ptr] = '\0';
    scheme_len = host_ptr - scheme_ptr;

    port_ptr = strchr(host_ptr, ':');
    if ( port_ptr != NULL ) {
        uint16_t tport;
        host_len = port_ptr - host_ptr;
        port_ptr++;
        if (sscanf(port_ptr, "%hu", &tport) != 1) {
            HTTPDNS_ERR("Could not find port");
            return -1;
        }
        *port = (int)tport;
    } else {
        *port = 0;
    }

    path_ptr = strchr(host_ptr, '/');
    if ( host_len == 0 ) {
        host_len = path_ptr - host_ptr;
    }

    if ( maxhost_len < host_len + 1 ) { /* including NULL-terminating char */
        HTTPDNS_ERR("Host str is too small (%d >= %d)", maxhost_len, host_len + 1);
        return -1;
    }
    memcpy(host + scheme_len, host_ptr, host_len);
    host[host_len + scheme_len] = '\0';

    fragment_ptr = strchr(host_ptr, '#');
    if (fragment_ptr != NULL) {
        path_len = fragment_ptr - path_ptr;
    } else {
        path_len = strlen(path_ptr);
    }

    if ( max_path_len < path_len + 1 ) { /* including NULL-terminating char */
        HTTPDNS_ERR("Path str is too small (%d >= %d)", max_path_len, path_len + 1);
        return -1;
    }
    memcpy(path, path_ptr, path_len);
    path[path_len] = '\0';

    return 0;
}

static int http_ctx_query(const char *serv_url,
                          const char *req_uri,
                          char *req_buf, int req_buf_size,
                          char *rsp_buf, int rsp_buf_size)
{
    http_rsp_info_t rsp_info;
    httpc_handle_t httpc_handle = 0;
    httpc_connection_t settings;
    int ret = -1;
    int fd = -1;
    char hdr[HTTPS_HDR_SIZE] = { 0 };

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        HTTPDNS_ERR("alloc socket fd fail\n");
        goto exit;
    }

    memset(&settings, 0, sizeof(settings));
    settings.socket = fd;
    settings.server_name = serv_url;
    settings.req_buf = req_buf;
    settings.req_buf_size = req_buf_size;

    httpc_handle = httpc_init(&settings);
    if (httpc_handle == 0) {
        HTTPDNS_ERR("http session init fail");
        goto exit;
    }

    ret = httpc_construct_header(hdr, HTTPS_HDR_SIZE, "Accept", "*/*");
    if (ret < 0) {
        HTTPDNS_ERR("http construct header fail");
        goto exit;
    }

    ret = httpc_send_request(httpc_handle, HTTP_GET, req_uri, hdr, NULL, NULL, 0);
    if (ret != HTTP_SUCCESS) {
        HTTPDNS_ERR("http send request fail");
        goto exit;
    }

    ret = httpc_recv_response(httpc_handle, rsp_buf, rsp_buf_size, &rsp_info, RSP_TIMEOUT_MS);
    if (ret < 0) {
        HTTPDNS_ERR("http recv response fail\n");
    } else {
        HTTPDNS_ERR("http session %x, buf size %d bytes, recv %d bytes data\n",
                    httpc_handle, rsp_buf_size, rsp_info.rsp_len);
        if (rsp_info.rsp_len > 0) {
            ret = rsp_info.rsp_len;
        }

        if (!rsp_info.message_complete) {
            HTTPDNS_WARN("http message incomplete!");
        }
    }

exit:
    if (fd >= 0)
        close(fd);
    
    if (httpc_handle)
        httpc_deinit(httpc_handle);

    return ret;
}

memory_t * http_ctx_get(http_ctx_t *ctx)
{
    char scheme[HTTPCLIENT_MAX_SCHEME_LEN] = {0};
    char host[HTTPCLIENT_MAX_HOST_LEN] = {0};
    char path[HTTPCLIENT_MAX_URL_LEN] = {0};
    int ret, port;

    if (!ctx)
        return NULL;

    ret = http_ctx_parse_url(ctx->url, 
                             scheme, sizeof(scheme),
                             host, sizeof(host), &(port),
                             path, sizeof(path));
    if (ret != 0) {
        HTTPDNS_ERR("httpclient_parse_url returned %d", ret);
        return ret;
    }

    http_ctx_query(host, path, 
                   ctx->request.memory,
                   ctx->request.size,
                   ctx->response.memory,
                   ctx->response.size);

    if(NULL != ctx->response.memory) {
        return &(ctx->response);
    }

    return NULL;
}

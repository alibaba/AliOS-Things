#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ulog/ulog.h"
#include "http_ctx.h"
#include "http.h"

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

    ctx->response.size = RSP_MEM_SIZE;
    ctx->response.memory = malloc(RSP_MEM_SIZE);
    memset(ctx->response.memory, 0, RSP_MEM_SIZE);
    if (!ctx->response.memory) {
        HTTPDNS_ERR("malloc failed!");
        return -1;
    }

    return 0;
}

void http_ctx_destroy(http_ctx_t *ctx)
{
    if(NULL != ctx->url) {
        sdsfree(ctx->url);
    }

    if(NULL != ctx->response.memory) {
        free(ctx->response.memory);
        ctx->response.memory = NULL;
    }
}

memory_t * http_ctx_get(http_ctx_t *ctx)
{
    httpclient_t client = { 0 };
    httpclient_data_t client_data = {0};

    client_data.response_buf = ctx->response.memory;
    client_data.response_buf_len = ctx->response.size;
    httpc_get(&client, ctx->url, &client_data);
    
    HTTPDNS_INFO("Data received: %s", client_data.response_buf);

    if(NULL != ctx->response.memory) {
        return &(ctx->response);
    }

    return NULL;
}

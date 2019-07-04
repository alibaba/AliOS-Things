/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include "iotx_ota_internal.h"

/* ofc, OTA fetch channel */

#define OFC_HTTP_HEADER_MAXLEN (128)

typedef struct {
    char *payload;
    int alread_download;
    int payload_len;
} ota_http_response_t;

typedef struct {

    const char *url;
    void *http_handle;
    int fetch_size;
} otahttp_Struct_t, *otahttp_Struct_pt;

void *ofc_Init(char *url, int offset)
{
    otahttp_Struct_pt h_odc;
    char *pub_key = NULL;
    int port = 0;
    char header[OFC_HTTP_HEADER_MAXLEN] = {0};
    char *header_fmt = "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n" \
                       "Accept-Encoding: gzip, deflate\r\n" \
                       "Range: bytes=%d-\r\n";
    char *protocol_end = NULL, *port_start = NULL, *port_end = NULL;
    int protocol_len = 0;
    iotx_http_method_t method = IOTX_HTTP_GET;

    /* protocol end location */
    protocol_end = strstr(url, "://");
    if (protocol_end != NULL) {
        protocol_len = strlen(url) - strlen(protocol_end);

        /* check protocol, http or https, and assign default port*/
        if ((strlen("http") == protocol_len) && (memcmp("http", url, protocol_len) == 0)) {
            OTA_LOG_INFO("protocol: http");
            port = 80;
        } else if ((strlen("https") == protocol_len) && (memcmp("https", url, protocol_len) == 0)) {
            OTA_LOG_INFO("protocol: https");
#ifdef SUPPORT_TLS
            {
                extern const char *iotx_ca_crt;
                pub_key = (char *)iotx_ca_crt;
            }
            port = 443;
#else
            port = 80;
#endif

        } else {
            OTA_LOG_ERROR("Invalid URL");
            return NULL;
        }
    } else {
        OTA_LOG_INFO("protocol: http");
        protocol_end = url;
        port = 80;
    }

    /* check port, if exist, override port */
    port_start = strstr(protocol_end + 1, ":");
    if (port_start != NULL) {
        port_end = strstr(port_start + 1, "/");
        if (port_end != NULL) {
            OTA_LOG_INFO("port exist: %.*s", (int)(port_end - port_start - 1), port_start + 1);
            infra_str2int((const char *)(port_start + 1), &port);
        }
    }

    HAL_Snprintf(header, OFC_HTTP_HEADER_MAXLEN, header_fmt, offset);

    if (NULL == (h_odc = OTA_MALLOC(sizeof(otahttp_Struct_t)))) {
        OTA_LOG_ERROR("allocate for h_odc failed");
        return NULL;
    }

    memset(h_odc, 0, sizeof(otahttp_Struct_t));

    h_odc->http_handle = wrapper_http_init();
    if (h_odc->http_handle == NULL) {
        OTA_FREE(h_odc);
        return NULL;
    }

    wrapper_http_setopt(h_odc->http_handle, IOTX_HTTPOPT_URL, (void *)url);
    wrapper_http_setopt(h_odc->http_handle, IOTX_HTTPOPT_PORT, (void *)&port);
    wrapper_http_setopt(h_odc->http_handle, IOTX_HTTPOPT_METHOD, (void *)&method);
    wrapper_http_setopt(h_odc->http_handle, IOTX_HTTPOPT_HEADER, (void *)header);
    wrapper_http_setopt(h_odc->http_handle, IOTX_HTTPOPT_CERT, (void *)pub_key);

    return h_odc;
}

static int _ota_fetch_callback(char *ptr, int length, int total_length, void *userdata)
{
    ota_http_response_t *response = (ota_http_response_t *)userdata;
    if (response->alread_download + length > response->payload_len) {
        return FAIL_RETURN;
    }

    memcpy(response->payload + response->alread_download, ptr, length);
    response->alread_download += length;

    return length;
}

int32_t ofc_Fetch(void *handle, char *buf, uint32_t buf_len, uint32_t timeout_s)
{
    int                 current_fetch_size = 0;
    int                 http_timeout_s = timeout_s * 1000;
    int                 http_recv_maxlen = buf_len;
    otahttp_Struct_pt   h_odc = (otahttp_Struct_pt)handle;
    ota_http_response_t response;

    memset(&response, 0, sizeof(ota_http_response_t));
    memset(buf, 0, buf_len);
    response.payload = buf;
    response.payload_len = buf_len;

    wrapper_http_setopt(h_odc->http_handle, IOTX_HTTPOPT_TIMEOUT, (void *)&http_timeout_s);
    wrapper_http_setopt(h_odc->http_handle, IOTX_HTTPOPT_RECVCALLBACK, (void *)_ota_fetch_callback);
    wrapper_http_setopt(h_odc->http_handle, IOTX_HTTPOPT_RECVMAXLEN, (void *)&http_recv_maxlen);
    wrapper_http_setopt(h_odc->http_handle, IOTX_HTTPOPT_RECVCONTEXT, (void *)&response);
    current_fetch_size = wrapper_http_perform(h_odc->http_handle, NULL, 0);

    if (current_fetch_size < 0) {
        OTA_LOG_ERROR("fetch firmware failed");
        return current_fetch_size;
    }

    h_odc->fetch_size += current_fetch_size;

    /*     OTA_LOG_ERROR("Download This Time: %d",current_fetch_size);
        OTA_LOG_ERROR("Download Total    : %d",h_odc->fetch_size); */

    return current_fetch_size;
}


int ofc_Deinit(void **handle)
{
    otahttp_Struct_t *h_odc = NULL;

    if (handle == NULL || *handle == NULL) {
        return FAIL_RETURN;
    }

    h_odc = *(otahttp_Struct_t **)handle;
    wrapper_http_deinit(&h_odc->http_handle);
    OTA_FREE(h_odc);

    *handle = NULL;

    return 0;
}




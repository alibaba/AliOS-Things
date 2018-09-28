/*
 * Copyright (c) 2014-2016 Alibaba Group. All rights reserved.
 * License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef __OTA_FETCH_C_H__
#define __OTA_FETCH_C_H__
#include <stdint.h>
#include <string.h>
#include "utils_httpc.h"
#include "ota_log.h"
/* ofc, OTA fetch channel */

typedef struct {

    const char *url;
    httpclient_t http;              /* http client */
    httpclient_data_t http_data;    /* http client data */

} otahttp_Struct_t, *otahttp_Struct_pt;

extern int httpclient_common(httpclient_t *client,
                             const char *url,
                             int port,
                             const char *ca_crt,
                             HTTPCLIENT_REQUEST_TYPE method,
                             uint32_t timeout_ms,
                             httpclient_data_t *client_data);

extern const char *iotx_ca_get(void);


void *ofc_Init(char *url)
{
    otahttp_Struct_pt h_odc;
    h_odc = ota_malloc(sizeof(otahttp_Struct_t));
    if (NULL == h_odc) {
        OTA_LOG_E("allocate for h_odc failed");
        return NULL;
    }

    memset(h_odc, 0, sizeof(otahttp_Struct_t));

    /* set http request-header parameter */
    h_odc->http.header = "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n" \
                         "Accept-Encoding: gzip, deflate\r\n";
#if defined(SUPPORT_ITLS)
    char *s_ptr = strstr(url, "://");
    if (strlen("https") == (s_ptr - url) && (0 == strncmp(url, "https", strlen("https")))) {
        strncpy(url + 1, url, strlen("http"));
        url++;
    }
#endif
    h_odc->url = url;

    return h_odc;
}


int32_t ofc_Fetch(void *handle, char *buf, uint32_t buf_len, uint32_t timeout_s)
{
    int diff;
    otahttp_Struct_pt h_odc = (otahttp_Struct_pt)handle;

    h_odc->http_data.response_buf = buf;
    h_odc->http_data.response_buf_len = buf_len;
    diff = h_odc->http_data.response_content_len - h_odc->http_data.retrieve_len;
#if defined(SUPPORT_ITLS)
    if (0 != httpclient_common(&h_odc->http, h_odc->url, 80, iotx_ca_get(), HTTPCLIENT_GET, timeout_s * 1000,
                               &h_odc->http_data)) {
#else
    if (0 != httpclient_common(&h_odc->http, h_odc->url, 443, iotx_ca_get(), HTTPCLIENT_GET, timeout_s * 1000,
                               &h_odc->http_data)) {
#endif
        OTA_LOG_E("fetch firmware failed");
        return -1;
    }

    return h_odc->http_data.response_content_len - h_odc->http_data.retrieve_len - diff;
}


int ofc_Deinit(void *handle)
{
    if (NULL != handle) {
        ota_free(handle);
    }

    return 0;
}

#endif

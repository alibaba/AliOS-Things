/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef HTTP_OPTS_H
#define HTTP_OPTS_H

#ifndef HTTPCLIENT_AUTHB_SIZE
#define HTTPCLIENT_AUTHB_SIZE     128
#endif

#ifndef HTTPCLIENT_CHUNK_SIZE
#define HTTPCLIENT_CHUNK_SIZE     (1024 * 2)
#endif

#ifndef HTTPCLIENT_SEND_BUF_SIZE
#define HTTPCLIENT_SEND_BUF_SIZE  512
#endif

#ifndef HTTPCLIENT_MAX_HOST_LEN
#define HTTPCLIENT_MAX_HOST_LEN   64
#endif

#ifndef HTTPCLIENT_MAX_URL_LEN
#define HTTPCLIENT_MAX_URL_LEN    512
#endif

#ifndef HTTPCLIENT_MAX_RECV_WAIT_MS
#define HTTPCLIENT_MAX_RECV_WAIT_MS 5000
#endif

#ifndef HTTP_PORT
#define HTTP_PORT   80 
#endif

#ifndef HTTPS_PORT
#define HTTPS_PORT  443
#endif

#endif /* HTTP_OPTS_H */

/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef HTTP_CLIENT_WRAPPER_H
#define HTTP_CLIENT_WRAPPER_H

#define HAL_Printf printf
#define http_info(...)    do{HAL_Printf(__VA_ARGS__);HAL_Printf("\r\n");}while(0)
#define http_err(...)     do{HAL_Printf(__VA_ARGS__);HAL_Printf("\r\n");}while(0)
#define http_debug(...)   do{HAL_Printf(__VA_ARGS__);HAL_Printf("\r\n");}while(0)

#ifndef MIN
#define MIN(x,y) (((x)<(y))?(x):(y))
#endif
#ifndef MAX
#define MAX(x,y) (((x)>(y))?(x):(y))
#endif


int http_tcp_conn_wrapper(httpclient_t *client, const char *host);
int http_tcp_close_wrapper(httpclient_t *client);
int http_tcp_send_wrapper(httpclient_t *client, const char *data, int length);
int http_tcp_recv_wrapper(httpclient_t *client, char *buf, int buflen, int timeout_ms, int *p_read_len);

#if CONFIG_HTTP_SECURE
int http_ssl_conn_wrapper(httpclient_t *client, const char *host);
int http_ssl_close_wrapper(httpclient_t *client);
int http_ssl_send_wrapper(httpclient_t *client, const char *data, size_t length);
int http_ssl_recv_wrapper(httpclient_t *client, char *buf, int buflen, int timeout_ms, int *p_read_len);
#endif

#endif  /* HTTP_CLIENT_WRAPPER_H */

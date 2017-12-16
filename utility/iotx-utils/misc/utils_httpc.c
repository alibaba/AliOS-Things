/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <string.h>
#include <stddef.h>
#include "iot_import.h"
#include "utils_timer.h"
#include "lite-log.h"
#include "utils_httpc.h"

#define HTTPCLIENT_MIN(x,y) (((x)<(y))?(x):(y))
#define HTTPCLIENT_MAX(x,y) (((x)>(y))?(x):(y))

#define HTTPCLIENT_AUTHB_SIZE     128

#define HTTPCLIENT_CHUNK_SIZE     256
#define HTTPCLIENT_SEND_BUF_SIZE  2048

#define HTTPCLIENT_MAX_HOST_LEN   64
#define HTTPCLIENT_MAX_URL_LEN    2048

#define HTTP_RETRIEVE_MORE_DATA   (1)            /**< More data needs to be retrieved. */

#if defined(MBEDTLS_DEBUG_C)
    #define DEBUG_LEVEL 2
#endif

static int httpclient_parse_host(const char *url, char *host, uint32_t maxhost_len);
static int httpclient_parse_url(const char *url, char *scheme, uint32_t max_scheme_len, char *host,
                                uint32_t maxhost_len, int *port, char *path, uint32_t max_path_len);
static iotx_err_t httpclient_conn(httpclient_t *client);
static int httpclient_recv(httpclient_t *client, char *buf, int min_len, int max_len, int *p_read_len,
                           uint32_t timeout);
static int httpclient_retrieve_content(httpclient_t *client, char *data, int len, uint32_t timeout,
                                       httpclient_data_t *client_data);
static int httpclient_response_parse(httpclient_t *client, char *data, int len, uint32_t timeout,
                                     httpclient_data_t *client_data);

static void httpclient_base64enc(char *out, const char *in)
{
    const char code[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
    int i = 0, x = 0, l = 0;

    for (; *in; in++) {
        x = x << 8 | *in;
        for (l += 8; l >= 6; l -= 6) {
            out[i++] = code[(x >> (l - 6)) & 0x3f];
        }
    }
    if (l > 0) {
        x <<= 6 - l;
        out[i++] = code[x & 0x3f];
    }
    for (; i % 4;) {
        out[i++] = '=';
    }
    out[i] = '\0';
}

static iotx_err_t httpclient_conn(httpclient_t *client)
{
    if (0 != client->net.connect(&client->net)) {
        log_err("establish connection failed");
        return ERROR_HTTP_CONN;
    }

    return SUCCESS_RETURN;
}

int httpclient_parse_url(const char *url, char *scheme, uint32_t max_scheme_len, char *host, uint32_t maxhost_len,
                         int *port, char *path, uint32_t max_path_len)
{
    char *scheme_ptr = (char *) url;
    char *host_ptr = (char *) strstr(url, "://");
    uint32_t host_len = 0;
    uint32_t path_len;
    //char *port_ptr;
    char *path_ptr;
    char *fragment_ptr;

    if (host_ptr == NULL) {
        log_err("Could not find host");
        return ERROR_HTTP_PARSE; /* URL is invalid */
    }

    if (max_scheme_len < host_ptr - scheme_ptr + 1) {
        /* including NULL-terminating char */
        log_err("Scheme str is too small (%u >= %u)", max_scheme_len, (uint32_t)(host_ptr - scheme_ptr + 1));
        return ERROR_HTTP_PARSE;
    }
    memcpy(scheme, scheme_ptr, host_ptr - scheme_ptr);
    scheme[host_ptr - scheme_ptr] = '\0';

    host_ptr += 3;

    *port = 0;

    path_ptr = strchr(host_ptr, '/');
    if (NULL == path_ptr) {
        log_err("invalid path");
        return -1;
    }

    if (host_len == 0) {
        host_len = path_ptr - host_ptr;
    }

    if (maxhost_len < host_len + 1) {
        /* including NULL-terminating char */
        log_err("Host str is too long (host_len(%d) >= max_len(%d))", host_len + 1, maxhost_len);
        return ERROR_HTTP_PARSE;
    }
    memcpy(host, host_ptr, host_len);
    host[host_len] = '\0';

    fragment_ptr = strchr(host_ptr, '#');
    if (fragment_ptr != NULL) {
        path_len = fragment_ptr - path_ptr;
    } else {
        path_len = strlen(path_ptr);
    }

    if (max_path_len < path_len + 1) {
        /* including NULL-terminating char */
        log_err("Path str is too small (%d >= %d)", max_path_len, path_len + 1);
        return ERROR_HTTP_PARSE;
    }
    memcpy(path, path_ptr, path_len);
    path[path_len] = '\0';

    return SUCCESS_RETURN;
}

int httpclient_parse_host(const char *url, char *host, uint32_t maxhost_len)
{
    const char *host_ptr = (const char *) strstr(url, "://");
    uint32_t host_len = 0;
    char *path_ptr;

    if (host_ptr == NULL) {
        log_err("Could not find host");
        return ERROR_HTTP_PARSE; /* URL is invalid */
    }
    host_ptr += 3;

    path_ptr = strchr(host_ptr, '/');
    if (host_len == 0) {
        host_len = path_ptr - host_ptr;
    }

    if (maxhost_len < host_len + 1) {
        /* including NULL-terminating char */
        log_err("Host str is too small (%d >= %d)", maxhost_len, host_len + 1);
        return ERROR_HTTP_PARSE;
    }
    memcpy(host, host_ptr, host_len);
    host[host_len] = '\0';

    return SUCCESS_RETURN;
}

int httpclient_get_info(httpclient_t *client, char *send_buf, int *send_idx, char *buf,
                        uint32_t len) /* 0 on success, err code on failure */
{
    int ret;
    int cp_len;
    int idx = *send_idx;

    if (len == 0) {
        len = strlen(buf);
    }

    do {
        if ((HTTPCLIENT_SEND_BUF_SIZE - idx) >= len) {
            cp_len = len;
        } else {
            cp_len = HTTPCLIENT_SEND_BUF_SIZE - idx;
        }

        memcpy(send_buf + idx, buf, cp_len);
        idx += cp_len;
        len -= cp_len;

        if (idx == HTTPCLIENT_SEND_BUF_SIZE) {
            //            if (client->remote_port == HTTPS_PORT)
            //            {
            //                WRITE_IOT_ERROR_LOG("send buffer overflow");
            //                return ERROR_HTTP;
            //            }
            //ret = httpclient_tcp_send_all(client->handle, send_buf, HTTPCLIENT_SEND_BUF_SIZE);
            ret = client->net.write(&client->net, send_buf, HTTPCLIENT_SEND_BUF_SIZE, 5000);
            if (ret) {
                return (ret);
            }
        }
    } while (len);

    *send_idx = idx;
    return SUCCESS_RETURN;
}

void httpclient_set_custom_header(httpclient_t *client, char *header)
{
    client->header = header;
}

int httpclient_basic_auth(httpclient_t *client, char *user, char *password)
{
    if ((strlen(user) + strlen(password)) >= HTTPCLIENT_AUTHB_SIZE) {
        return ERROR_HTTP;
    }
    client->auth_user = user;
    client->auth_password = password;
    return SUCCESS_RETURN;
}

int httpclient_send_auth(httpclient_t *client, char *send_buf, int *send_idx)
{
    char b_auth[(int)((HTTPCLIENT_AUTHB_SIZE + 3) * 4 / 3 + 1)];
    char base64buff[HTTPCLIENT_AUTHB_SIZE + 3];

    httpclient_get_info(client, send_buf, send_idx, "Authorization: Basic ", 0);
    sprintf(base64buff, "%s:%s", client->auth_user, client->auth_password);
    log_debug("bAuth: %s", base64buff) ;
    httpclient_base64enc(b_auth, base64buff);
    b_auth[strlen(b_auth) + 1] = '\0';
    b_auth[strlen(b_auth)] = '\n';
    log_debug("b_auth:%s", b_auth) ;
    httpclient_get_info(client, send_buf, send_idx, b_auth, 0);
    return SUCCESS_RETURN;
}

int httpclient_send_header(httpclient_t *client, const char *url, int method, httpclient_data_t *client_data)
{
    char scheme[8] = { 0 };
    //char host[HTTPCLIENT_MAX_HOST_LEN] = { 0 };
    //char path[HTTPCLIENT_MAX_URL_LEN] = { 0 };
    char *host;//[HTTPCLIENT_MAX_HOST_LEN] = { 0 };
    char *path;//[HTTPCLIENT_MAX_URL_LEN] = { 0 };
    int len;
    //char send_buf[HTTPCLIENT_SEND_BUF_SIZE] = { 0 };
    //char buf[HTTPCLIENT_SEND_BUF_SIZE] = { 0 };
    char *send_buf;
    char *buf;
    char *meth = (method == HTTPCLIENT_GET) ? "GET" : (method == HTTPCLIENT_POST) ? "POST" :
                 (method == HTTPCLIENT_PUT) ? "PUT" : (method == HTTPCLIENT_DELETE) ? "DELETE" :
                 (method == HTTPCLIENT_HEAD) ? "HEAD" : "";
    int ret;
    int port;
		int res;
    int rc = SUCCESS_RETURN;

    if (NULL == (host = (char *)HAL_Malloc(HTTPCLIENT_MAX_HOST_LEN))) {
        log_err("not enough memory");
        return FAIL_RETURN;
    }
    if (NULL == (path = (char *)HAL_Malloc(HTTPCLIENT_MAX_HOST_LEN))) {
        log_err("not enough memory");
        rc = FAIL_RETURN;
        goto GO_ERR_3;
    }
    if (NULL == (send_buf = (char *)HAL_Malloc(HTTPCLIENT_SEND_BUF_SIZE))) {
        log_err("not enough memory");
        rc = FAIL_RETURN;
        goto GO_ERR_2;
    }
    if (NULL == (buf = (char *)HAL_Malloc(HTTPCLIENT_SEND_BUF_SIZE))) {
        log_err("not enough memory");
        rc = FAIL_RETURN;
        goto GO_ERR_1;
    }
    /* First we need to parse the url (http[s]://host[:port][/[path]]) */
    res = httpclient_parse_url(url, scheme, sizeof(scheme), host, HTTPCLIENT_MAX_HOST_LEN, &port, path, HTTPCLIENT_MAX_HOST_LEN);
    if (res != SUCCESS_RETURN) {
        log_err("httpclient_parse_url returned %d", res);
        //return res;
        rc = res;
        goto GO_ERR;
    }

    //if (client->remote_port == 0)
    //{
    if (strcmp(scheme, "http") == 0) {
        //client->remote_port = HTTP_PORT;
    } else if (strcmp(scheme, "https") == 0) {
        //client->remote_port = HTTPS_PORT;
    }
    //}

    /* Send request */
    memset(send_buf, 0, HTTPCLIENT_SEND_BUF_SIZE);
    memset(buf, 0, HTTPCLIENT_SEND_BUF_SIZE);
    len = 0; /* Reset send buffer */

    snprintf(buf, HTTPCLIENT_SEND_BUF_SIZE, "%s %s HTTP/1.1\r\nHost: %s\r\n", meth, path, host); /* Write request */
    ret = httpclient_get_info(client, send_buf, &len, buf, strlen(buf));
    if (ret) {
        log_err("Could not write request");
        //return ERROR_HTTP_CONN;
        rc = ERROR_HTTP_CONN;
        goto GO_ERR;
    }

    /* Send all headers */
    if (client->auth_user) {
        httpclient_send_auth(client, send_buf, &len); /* send out Basic Auth header */
    }

    /* Add user header information */
    if (client->header) {
        httpclient_get_info(client, send_buf, &len, (char *) client->header, strlen(client->header));
    }

    if (client_data->post_buf != NULL) {
        snprintf(buf, HTTPCLIENT_SEND_BUF_SIZE, "Content-Length: %d\r\n", client_data->post_buf_len);
        httpclient_get_info(client, send_buf, &len, buf, strlen(buf));

        if (client_data->post_content_type != NULL) {
            snprintf(buf, HTTPCLIENT_SEND_BUF_SIZE, "Content-Type: %s\r\n", client_data->post_content_type);
            httpclient_get_info(client, send_buf, &len, buf, strlen(buf));
        }
    }

    /* Close headers */
    httpclient_get_info(client, send_buf, &len, "\r\n", 0);

    log_multi_line(LOG_DEBUG_LEVEL, "REQUEST", "%s", send_buf, ">");

    //ret = httpclient_tcp_send_all(client->net.handle, send_buf, len);
    ret = client->net.write(&client->net, send_buf, len, 5000);
    if (ret > 0) {
        log_debug("Written %d bytes", ret);
    } else if (ret == 0) {
        log_err("ret == 0,Connection was closed by server");
        //return ERROR_HTTP_CLOSED; /* Connection was closed by server */
        rc = ERROR_HTTP_CLOSED;
    } else {
        log_err("Connection error (send returned %d)", ret);
        //return ERROR_HTTP_CONN;
        rc = ERROR_HTTP_CONN;
    }
GO_ERR:
    HAL_Free(buf);
GO_ERR_1:
    HAL_Free(send_buf);
GO_ERR_2:
    HAL_Free(path);
GO_ERR_3:
    HAL_Free(host);
    return rc;//SUCCESS_RETURN;
}

int httpclient_send_userdata(httpclient_t *client, httpclient_data_t *client_data)
{
    int ret = 0;

    if (client_data->post_buf && client_data->post_buf_len) {
        log_debug("client_data->post_buf: %s", client_data->post_buf);
        {
            //ret = httpclient_tcp_send_all(client->handle, (char *)client_data->post_buf, client_data->post_buf_len);
            ret = client->net.write(&client->net, (char *)client_data->post_buf, client_data->post_buf_len, 5000);
            if (ret > 0) {
                log_debug("Written %d bytes", ret);
            } else if (ret == 0) {
                log_err("ret == 0,Connection was closed by server");
                return ERROR_HTTP_CLOSED; /* Connection was closed by server */
            } else {
                log_err("Connection error (send returned %d)", ret);
                return ERROR_HTTP_CONN;
            }
        }
    }

    return SUCCESS_RETURN;
}

/* 0 on success, err code on failure */
int httpclient_recv(httpclient_t *client, char *buf, int min_len, int max_len, int *p_read_len, uint32_t timeout_ms)
{
    int ret = 0;
    iotx_time_t timer;

    iotx_time_init(&timer);
    utils_time_countdown_ms(&timer, timeout_ms);

    *p_read_len = 0;

    ret = client->net.read(&client->net, buf, max_len, iotx_time_left(&timer));
    //log_debug("Recv: | %s", buf);

    if (ret > 0) {
        *p_read_len = ret;
    } else if (ret == 0) {
        //timeout
        return 0;
    } else if (-1 == ret) {
        log_info("Connection closed.");
        return ERROR_HTTP_CONN;
    } else {
        log_err("Connection error (recv returned %d)", ret);
        return ERROR_HTTP_CONN;
    }
    log_info("%u bytes has been read", *p_read_len);
    return 0;

    //    while (readLen <= min_len) {
    //        buf[readLen] = '\0';
    //        if (readLen < min_len) {
    //            //wait to read HTTP respond data
    //            ret = client->net.read(&client->net, buf + readLen, min_len - readLen, utils_timer_remain(&timer));
    //        } else {
    //            //read the rest data in TCP buffer (with little wait time)
    //            ret = client->net.read(&client->net, buf + readLen, max_len - readLen, 100);
    //        }
    //
    //        if (ret > 0) {
    //            readLen += ret;
    //        } else if (ret == 0) {
    //            //timeout
    //            break;
    //        } else if (-1 == ret) {
    //            log_info("Connection closed. %u bytes be read", readLen);
    //            break;
    //        } else {
    //            log_err("Connection error (recv returned %d)", ret);
    //            return ERROR_HTTP_CONN;
    //        }
    //    }
    //
    //    log_info("%u bytes be read", readLen);
    //    *p_read_len = readLen;
    //    return 0;
}

int httpclient_retrieve_content(httpclient_t *client, char *data, int len, uint32_t timeout_ms,
                                httpclient_data_t *client_data)
{
    int count = 0;
    int templen = 0;
    int crlf_pos;
    iotx_time_t timer;

    iotx_time_init(&timer);
    utils_time_countdown_ms(&timer, timeout_ms);

    /* Receive data */
    log_debug("Current data: %s", data);

    client_data->is_more = true;

    if (client_data->response_content_len == (uint32_t)-1 && client_data->is_chunked == false) {
        while (true) {
            int ret, max_len;
            if (count + len < client_data->response_buf_len - 1) {
                memcpy(client_data->response_buf + count, data, len);
                count += len;
                client_data->response_buf[count] = '\0';
            } else {
                memcpy(client_data->response_buf + count, data, client_data->response_buf_len - 1 - count);
                client_data->response_buf[client_data->response_buf_len - 1] = '\0';
                return HTTP_RETRIEVE_MORE_DATA;
            }

            max_len = HTTPCLIENT_MIN(HTTPCLIENT_CHUNK_SIZE - 1, client_data->response_buf_len - 1 - count);
            ret = httpclient_recv(client, data, 1, max_len, &len, iotx_time_left(&timer));

            /* Receive data */
            log_debug("data len: %d %d", len, count);

            if (ret == ERROR_HTTP_CONN) {
                log_debug("ret == ERROR_HTTP_CONN");
                return ret;
            }

            if (len == 0) {
                /* read no more data */
                log_debug("no more len == 0");
                client_data->is_more = false;
                return SUCCESS_RETURN;
            }
        }
    }

    while (true) {
        uint32_t readLen = 0;

        if (client_data->is_chunked && client_data->retrieve_len <= 0) {
            /* Read chunk header */
            bool foundCrlf;
            int n;
            do {
                foundCrlf = false;
                crlf_pos = 0;
                data[len] = 0;
                if (len >= 2) {
                    for (; crlf_pos < len - 2; crlf_pos++) {
                        if (data[crlf_pos] == '\r' && data[crlf_pos + 1] == '\n') {
                            foundCrlf = true;
                            break;
                        }
                    }
                }
                if (!foundCrlf) {
                    /* Try to read more */
                    if (len < HTTPCLIENT_CHUNK_SIZE) {
                        int new_trf_len, ret;
                        ret = httpclient_recv(client,
                                              data + len,
                                              0,
                                              HTTPCLIENT_CHUNK_SIZE - len - 1,
                                              &new_trf_len,
                                              iotx_time_left(&timer));
                        len += new_trf_len;
                        if (ret == ERROR_HTTP_CONN) {
                            return ret;
                        } else {
                            continue;
                        }
                    } else {
                        return ERROR_HTTP;
                    }
                }
            } while (!foundCrlf);
            data[crlf_pos] = '\0';
            n = sscanf(data, "%x", &readLen);/* chunk length */
            client_data->retrieve_len = readLen;
            client_data->response_content_len += client_data->retrieve_len;
            if (n != 1) {
                log_err("Could not read chunk length");
                return ERROR_HTTP_UNRESOLVED_DNS;
            }

            memmove(data, &data[crlf_pos + 2], len - (crlf_pos + 2)); /* Not need to move NULL-terminating char any more */
            len -= (crlf_pos + 2);

            if (readLen == 0) {
                /* Last chunk */
                client_data->is_more = false;
                log_debug("no more (last chunk)");
                break;
            }
        } else {
            readLen = client_data->retrieve_len;
        }

        log_debug("Total-Payload: %d Bytes; Read: %d Bytes", readLen, len);

        do {
            templen = HTTPCLIENT_MIN(len, readLen);
            if (count + templen < client_data->response_buf_len - 1) {
                memcpy(client_data->response_buf + count, data, templen);
                count += templen;
                client_data->response_buf[count] = '\0';
                client_data->retrieve_len -= templen;
            } else {
                memcpy(client_data->response_buf + count, data, client_data->response_buf_len - 1 - count);
                client_data->response_buf[client_data->response_buf_len - 1] = '\0';
                client_data->retrieve_len -= (client_data->response_buf_len - 1 - count);
                return HTTP_RETRIEVE_MORE_DATA;
            }

            if (len > readLen) {
                log_debug("memmove %d %d %d\n", readLen, len, client_data->retrieve_len);
                memmove(data, &data[readLen], len - readLen); /* chunk case, read between two chunks */
                len -= readLen;
                readLen = 0;
                client_data->retrieve_len = 0;
            } else {
                readLen -= len;
            }

            if (readLen) {
                int ret;
                int max_len = HTTPCLIENT_MIN(HTTPCLIENT_CHUNK_SIZE - 1, client_data->response_buf_len - 1 - count);
                max_len = HTTPCLIENT_MIN(max_len, readLen);
                ret = httpclient_recv(client, data, 1, max_len, &len, iotx_time_left(&timer));
                if (ret == ERROR_HTTP_CONN) {
                    return ret;
                }
            }
        } while (readLen);

        if (client_data->is_chunked) {
            if (len < 2) {
                int new_trf_len, ret;
                /* Read missing chars to find end of chunk */
                ret = httpclient_recv(client, data + len, 2 - len, HTTPCLIENT_CHUNK_SIZE - len - 1, &new_trf_len,
                                      iotx_time_left(&timer));
                if (ret == ERROR_HTTP_CONN) {
                    return ret;
                }
                len += new_trf_len;
            }
            if ((data[0] != '\r') || (data[1] != '\n')) {
                log_err("Format error, %s", data); /* after memmove, the beginning of next chunk */
                return ERROR_HTTP_UNRESOLVED_DNS;
            }
            memmove(data, &data[2], len - 2); /* remove the \r\n */
            len -= 2;
        } else {
            log_debug("no more (content-length)");
            client_data->is_more = false;
            break;
        }

    }

    return SUCCESS_RETURN;
}

int httpclient_response_parse(httpclient_t *client, char *data, int len, uint32_t timeout_ms,
                              httpclient_data_t *client_data)
{
    int crlf_pos;
    iotx_time_t timer;

    iotx_time_init(&timer);
    utils_time_countdown_ms(&timer, timeout_ms);

    client_data->response_content_len = (uint32_t)-1;

    char *crlf_ptr = strstr(data, "\r\n");
    if (crlf_ptr == NULL) {
        log_err("\r\n not found");
        return ERROR_HTTP_UNRESOLVED_DNS;
    }

    crlf_pos = crlf_ptr - data;
    data[crlf_pos] = '\0';

    /* Parse HTTP response */
    if (sscanf(data, "HTTP/%*d.%*d %d %*[^\r\n]", &(client->response_code)) != 1) {
        /* Cannot match string, error */
        log_err("Not a correct HTTP answer : %s\n", data);
        return ERROR_HTTP_UNRESOLVED_DNS;
    }

    if ((client->response_code < 200) || (client->response_code >= 400)) {
        /* Did not return a 2xx code; TODO fetch headers/(&data?) anyway and implement a mean of writing/reading headers */
        log_warning("Response code %d", client->response_code);
    }

    log_debug("Reading headers%s", data);

    memmove(data, &data[crlf_pos + 2], len - (crlf_pos + 2) + 1); /* Be sure to move NULL-terminating char as well */
    len -= (crlf_pos + 2);

    client_data->is_chunked = false;

    /* Now get headers */
    while (true) {
        char key[32];
        char value[32];
        int n;

        key[31] = '\0';
        value[31] = '\0';

        crlf_ptr = strstr(data, "\r\n");
        if (crlf_ptr == NULL) {
            if (len < HTTPCLIENT_CHUNK_SIZE - 1) {
                int new_trf_len, ret;
                ret = httpclient_recv(client, data + len, 1, HTTPCLIENT_CHUNK_SIZE - len - 1, &new_trf_len, iotx_time_left(&timer));
                len += new_trf_len;
                data[len] = '\0';
                log_debug("Read %d chars; In buf: [%s]", new_trf_len, data);
                if (ret == ERROR_HTTP_CONN) {
                    return ret;
                } else {
                    continue;
                }
            } else {
                log_debug("header len > chunksize");
                return ERROR_HTTP;
            }
        }

        crlf_pos = crlf_ptr - data;
        if (crlf_pos == 0) {
            /* End of headers */
            memmove(data, &data[2], len - 2 + 1); /* Be sure to move NULL-terminating char as well */
            len -= 2;
            break;
        }

        data[crlf_pos] = '\0';

        n = sscanf(data, "%31[^:]: %31[^\r\n]", key, value);
        if (n == 2) {
            log_debug("Read header : %s: %s", key, value);
            if (!strcmp(key, "Content-Length")) {
                sscanf(value, "%d", &(client_data->response_content_len));
                client_data->retrieve_len = client_data->response_content_len;
            } else if (!strcmp(key, "Transfer-Encoding")) {
                if (!strcmp(value, "Chunked") || !strcmp(value, "chunked")) {
                    client_data->is_chunked = true;
                    client_data->response_content_len = 0;
                    client_data->retrieve_len = 0;
                }
            }
            memmove(data, &data[crlf_pos + 2], len - (crlf_pos + 2) + 1); /* Be sure to move NULL-terminating char as well */
            len -= (crlf_pos + 2);

        } else {
            log_err("Could not parse header");
            return ERROR_HTTP;
        }
    }

    return httpclient_retrieve_content(client, data, len, iotx_time_left(&timer), client_data);
}

iotx_err_t httpclient_connect(httpclient_t *client)
{
    iotx_err_t ret = ERROR_HTTP_CONN;

    client->net.handle = 0;

    ret = httpclient_conn(client);
    //    if (0 == ret)
    //    {
    //        client->remote_port = HTTP_PORT;
    //    }

    return ret;
}

iotx_err_t httpclient_send_request(httpclient_t *client, const char *url, int method, httpclient_data_t *client_data)
{
    iotx_err_t ret = ERROR_HTTP_CONN;

    if (0 == client->net.handle) {
        log_debug("not connection have been established");
        return ret;
    }

    ret = (iotx_err_t)httpclient_send_header(client, url, method, client_data);
    if (ret != 0) {
        log_err("httpclient_send_header is error,ret = %d", (int)ret);
        return ret;
    }

    if (method == HTTPCLIENT_POST || method == HTTPCLIENT_PUT) {
        ret = (iotx_err_t)httpclient_send_userdata(client, client_data);
    }

    return ret;
}

iotx_err_t httpclient_recv_response(httpclient_t *client, uint32_t timeout_ms, httpclient_data_t *client_data)
{
    int reclen = 0; 
    iotx_err_t ret = ERROR_HTTP_CONN;
    char buf[HTTPCLIENT_CHUNK_SIZE] = { 0 };
    iotx_time_t timer;

    iotx_time_init(&timer);
    utils_time_countdown_ms(&timer, timeout_ms);

    if (0 == client->net.handle) {
        log_debug("not connection have been established");
        return ret;
    }

    if (client_data->is_more) {
        client_data->response_buf[0] = '\0';
        ret = (iotx_err_t)httpclient_retrieve_content(client, buf, reclen, iotx_time_left(&timer), client_data);
    } else {
        client_data->is_more = 1;
        ret = (iotx_err_t)httpclient_recv(client, buf, 1, HTTPCLIENT_CHUNK_SIZE - 1, &reclen, iotx_time_left(&timer));
        if (ret != 0) {
            return ret;
        }

        buf[reclen] = '\0';

        if (reclen) {
            log_multi_line(LOG_DEBUG_LEVEL, "RESPONSE", "%s", buf, "<");
            ret = (iotx_err_t)httpclient_response_parse(client, buf, reclen, iotx_time_left(&timer), client_data);
        }
    }

    return ret;
}

void httpclient_close(httpclient_t *client)
{
    if (client->net.handle > 0) {
        client->net.disconnect(&client->net);
    }
    client->net.handle = 0;
}

int httpclient_common(httpclient_t *client, const char *url, int port, const char *ca_crt, int method,
                      uint32_t timeout_ms,
                      httpclient_data_t *client_data)
{
    iotx_time_t timer;
    int ret = 0;
    char host[HTTPCLIENT_MAX_HOST_LEN] = { 0 };


    if (0 == client->net.handle) {
        //Establish connection if no.
    	httpclient_parse_host(url, host, sizeof(host));
    	log_debug("host: '%s', port: %d", host, port);

    	iotx_net_init(&client->net, host, port, ca_crt);

    	ret = httpclient_connect(client);
    	if (0 != ret) {
            log_err("httpclient_connect is error,ret = %d", ret);
            httpclient_close(client);
            return ret;
    	}

        ret = httpclient_send_request(client, url, method, client_data);
        if (0 != ret) {
            log_err("httpclient_send_request is error,ret = %d", ret);
            httpclient_close(client);
            return ret;
        }
    }

    iotx_time_init(&timer);
    utils_time_countdown_ms(&timer, timeout_ms);

     if ((NULL != client_data->response_buf)
         || (0 != client_data->response_buf_len)) {
        ret = httpclient_recv_response(client, iotx_time_left(&timer), client_data);
        if (ret < 0) {
            log_err("httpclient_recv_response is error,ret = %d", ret);
            httpclient_close(client);
            return ret;
        }
    }

    if (! client_data->is_more) {
        //Close the HTTP if no more data.
        log_info("close http channel");
        httpclient_close(client);
    }
    return (ret >= 0) ? 0 : -1;
}

int utils_get_response_code(httpclient_t *client)
{
    return client->response_code;
}

iotx_err_t iotx_post(
            httpclient_t *client,
            const char *url,
            int port,
            const char *ca_crt,
            uint32_t timeout_ms,
            httpclient_data_t *client_data)
{
    return (iotx_err_t)httpclient_common(client, url, port, ca_crt, HTTPCLIENT_POST, timeout_ms, client_data);
}


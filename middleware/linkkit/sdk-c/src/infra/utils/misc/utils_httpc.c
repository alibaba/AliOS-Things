/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */





#include <string.h>
#include <stddef.h>
#include <stdlib.h>

#include "iot_import.h"
#include "utils_timer.h"
#include "utils_httpc.h"
#include "iotx_utils_internal.h"

#define HTTPCLIENT_MIN(x,y) (((x)<(y))?(x):(y))
#define HTTPCLIENT_MAX(x,y) (((x)>(y))?(x):(y))

#define HTTPCLIENT_AUTHB_SIZE     128

#define HTTPCLIENT_CHUNK_SIZE     1024          /* read payload */
#define HTTPCLIENT_RAED_HEAD_SIZE 32            /* read header */
#define HTTPCLIENT_SEND_BUF_SIZE  1024          /* send */

#define HTTPCLIENT_MAX_HOST_LEN   128
#define HTTPCLIENT_MAX_URL_LEN    1024

#define HTTP_RETRIEVE_MORE_DATA   (1)            /**< More data needs to be retrieved. */

#if defined(MBEDTLS_DEBUG_C)
    #define DEBUG_LEVEL 2
#endif

static int httpclient_parse_host(const char *url, char *host, uint32_t maxhost_len);
static int httpclient_parse_url(const char *url, char *scheme, uint32_t max_scheme_len, char *host,
                                uint32_t maxhost_len, int *port, char *path, uint32_t max_path_len);
static int httpclient_conn(httpclient_t *client);
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

int httpclient_conn(httpclient_t *client)
{
    if (0 != client->net.connect(&client->net)) {
        utils_err("establish connection failed");
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
    /* char *port_ptr; */
    char *path_ptr;
    char *fragment_ptr;

    if (host_ptr == NULL) {
        utils_err("Could not find host");
        return ERROR_HTTP_PARSE; /* URL is invalid */
    }

    if (max_scheme_len < host_ptr - scheme_ptr + 1) {
        /* including NULL-terminating char */
        utils_err("Scheme str is too small (%u >= %u)", max_scheme_len, (uint32_t)(host_ptr - scheme_ptr + 1));
        return ERROR_HTTP_PARSE;
    }
    memcpy(scheme, scheme_ptr, host_ptr - scheme_ptr);
    scheme[host_ptr - scheme_ptr] = '\0';

    host_ptr += 3;

    *port = 0;

    path_ptr = strchr(host_ptr, '/');
    if (NULL == path_ptr) {
        utils_err("invalid path");
        return -1;
    }

    if (host_len == 0) {
        host_len = path_ptr - host_ptr;
    }

    if (maxhost_len < host_len + 1) {
        /* including NULL-terminating char */
        utils_err("Host str is too long (host_len(%d) >= max_len(%d))", host_len + 1, maxhost_len);
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
        utils_err("Path str is too small (%d >= %d)", max_path_len, path_len + 1);
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
        utils_err("Could not find host");
        return ERROR_HTTP_PARSE; /* URL is invalid */
    }
    host_ptr += 3;

    path_ptr = strchr(host_ptr, '/');
    if (host_len == 0) {
        host_len = path_ptr - host_ptr;
    }

    if (maxhost_len < host_len + 1) {
        /* including NULL-terminating char */
        utils_err("Host str is too small (%d >= %d)", maxhost_len, host_len + 1);
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
            /*            if (client->remote_port == HTTPS_PORT) */
            /*            { */
            /*                WRITE_IOT_ERROR_LOG("send buffer overflow"); */
            /*                return ERROR_HTTP; */
            /*            } */
            /* ret = httpclient_tcp_send_all(client->handle, send_buf, HTTPCLIENT_SEND_BUF_SIZE); */
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
    utils_debug("bAuth: %s", base64buff) ;
    httpclient_base64enc(b_auth, base64buff);
    b_auth[strlen(b_auth) + 1] = '\0';
    b_auth[strlen(b_auth)] = '\n';
    utils_debug("b_auth: %s", b_auth) ;
    httpclient_get_info(client, send_buf, send_idx, b_auth, 0);
    return SUCCESS_RETURN;
}

int httpclient_send_header(httpclient_t *client, const char *url, int method, httpclient_data_t *client_data)
{
    char scheme[8] = { 0 };
    char host[HTTPCLIENT_MAX_HOST_LEN] = { 0 };
    char path[HTTPCLIENT_MAX_URL_LEN] = { 0 };
    int len;
    char send_buf[HTTPCLIENT_SEND_BUF_SIZE] = { 0 };
    char buf[HTTPCLIENT_SEND_BUF_SIZE] = { 0 };
    char *meth = (method == HTTPCLIENT_GET) ? "GET" : (method == HTTPCLIENT_POST) ? "POST" :
                 (method == HTTPCLIENT_PUT) ? "PUT" : (method == HTTPCLIENT_DELETE) ? "DELETE" :
                 (method == HTTPCLIENT_HEAD) ? "HEAD" : "";
    int ret;
    int port;

    /* First we need to parse the url (http[s]://host[:port][/[path]]) */
    /* int res = httpclient_parse_url(url, scheme, sizeof(scheme), host, sizeof(host), &(client->remote_port), path, sizeof(path)); */
    int res = httpclient_parse_url(url, scheme, sizeof(scheme), host, sizeof(host), &port, path, sizeof(path));
    if (res != SUCCESS_RETURN) {
        utils_err("httpclient_parse_url fail returned %d", res);
        return res;
    }

    /* if (client->remote_port == 0) */
    /* { */
    if (strcmp(scheme, "http") == 0) {
        /* client->remote_port = HTTP_PORT; */
    } else if (strcmp(scheme, "https") == 0) {
        /* client->remote_port = HTTPS_PORT; */
    }
    /* } */

    /* Send request */
    memset(send_buf, 0, HTTPCLIENT_SEND_BUF_SIZE);
    len = 0; /* Reset send buffer */

#ifdef ON_PRE
    extern int g_domain_type;

    if (1 == g_domain_type) {
        utils_warning("hacking HTTP auth requeset for singapore+pre-online to 'iot-auth.ap-southeast-1.aliyuncs.com'");
        HAL_Snprintf(buf, sizeof(buf), "%s %s HTTP/1.1\r\nHost: %s\r\n", meth, path, "iot-auth.ap-southeast-1.aliyuncs.com"); /* Write request */
    }
#else
    HAL_Snprintf(buf, sizeof(buf), "%s %s HTTP/1.1\r\nHost: %s\r\n", meth, path, host); /* Write request */
#endif
    ret = httpclient_get_info(client, send_buf, &len, buf, strlen(buf));
    if (ret) {
        utils_err("Could not write request");
        return ERROR_HTTP_CONN;
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
        HAL_Snprintf(buf, sizeof(buf), "Content-Length: %d\r\n", client_data->post_buf_len);
        httpclient_get_info(client, send_buf, &len, buf, strlen(buf));

        if (client_data->post_content_type != NULL) {
            HAL_Snprintf(buf, sizeof(buf), "Content-Type: %s\r\n", client_data->post_content_type);
            httpclient_get_info(client, send_buf, &len, buf, strlen(buf));
        }
    }

    /* Close headers */
    httpclient_get_info(client, send_buf, &len, "\r\n", 0);

    log_multi_line(LOG_DEBUG_LEVEL, "REQUEST", "%s", send_buf, ">");

    /* ret = httpclient_tcp_send_all(client->net.handle, send_buf, len); */
    ret = client->net.write(&client->net, send_buf, len, 5000);
    if (ret > 0) {
        utils_debug("Written %d bytes", ret);
    } else if (ret == 0) {
        utils_err("ret == 0,Connection was closed by server");
        return ERROR_HTTP_CLOSED; /* Connection was closed by server */
    } else {
        utils_err("Connection error (send returned %d)", ret);
        return ERROR_HTTP_CONN;
    }

    return SUCCESS_RETURN;
}

int httpclient_send_userdata(httpclient_t *client, httpclient_data_t *client_data)
{
    int ret = 0;

    if (client_data->post_buf && client_data->post_buf_len) {
        utils_debug("client_data->post_buf: %s", client_data->post_buf);
        {
            /* ret = httpclient_tcp_send_all(client->handle, (char *)client_data->post_buf, client_data->post_buf_len); */
            ret = client->net.write(&client->net, (char *)client_data->post_buf, client_data->post_buf_len, 5000);
            if (ret > 0) {
                utils_debug("Written %d bytes", ret);
            } else if (ret == 0) {
                utils_err("ret == 0, Connection was closed by server");
                return ERROR_HTTP_CLOSED; /* Connection was closed by server */
            } else {
                utils_err("Connection error (send returned %d)", ret);
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
    /* utils_debug("Recv: | %s", buf); */

    if (ret > 0) {
        *p_read_len = ret;
    } else if (ret == 0) {
        /* timeout */
        return FAIL_RETURN;
    } else if (-1 == ret) {
        utils_err("Connection closed.");
        return ERROR_HTTP_CONN;
    } else {
        utils_err("Connection error (recv returned %d)", ret);
        return ERROR_HTTP_CONN;
    }
    utils_debug("%u bytes has been read", *p_read_len);
    return 0;

    /*    while (readLen <= min_len) { */
    /*        buf[readLen] = '\0'; */
    /*        if (readLen < min_len) { */
    /*            //wait to read HTTP respond data */
    /*            ret = client->net.read(&client->net, buf + readLen, min_len - readLen, utils_timer_remain(&timer)); */
    /*        } else { */
    /*            //read the rest data in TCP buffer (with little wait time) */
    /*            ret = client->net.read(&client->net, buf + readLen, max_len - readLen, 100); */
    /*        } */
    /* */
    /*        if (ret > 0) { */
    /*            readLen += ret; */
    /*        } else if (ret == 0) { */
    /*            //timeout */
    /*            break; */
    /*        } else if (-1 == ret) { */
    /*            log_info("Connection closed. %u bytes be read", readLen); */
    /*            break; */
    /*        } else { */
    /*            log_err("Connection error (recv returned %d)", ret); */
    /*            return ERROR_HTTP_CONN; */
    /*        } */
    /*    } */
    /* */
    /*    log_info("%u bytes be read", readLen); */
    /*    *p_read_len = readLen; */
    /*    return 0; */
}

int httpclient_retrieve_content(httpclient_t *client, char *data, int len,
                                uint32_t timeout_ms, httpclient_data_t *client_data)
{
    int count = 0;
    int templen = 0;
    int crlf_pos;
    iotx_time_t timer;

    iotx_time_init(&timer);
    utils_time_countdown_ms(&timer, timeout_ms);

    /* Receive data */
    utils_debug("Current data: %s", data);

    client_data->is_more = IOT_TRUE;

    /* the header is not received finished */
    if (client_data->response_content_len == -1 && client_data->is_chunked == IOT_FALSE) {
        /* can not enter this if */
        while (1) {
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

            /* try to read more header */
            max_len = HTTPCLIENT_MIN(HTTPCLIENT_RAED_HEAD_SIZE, client_data->response_buf_len - 1 - count);
            ret = httpclient_recv(client, data, 1, max_len, &len, iotx_time_left(&timer));

            /* Receive data */
            utils_debug("data len: %d %d", len, count);

            if (ret == ERROR_HTTP_CONN) {
                utils_err("ret == ERROR_HTTP_CONN");
                return ret;
            }

            if (len == 0) {
                /* read no more data */
                utils_debug("no more len == 0");
                client_data->is_more = IOT_FALSE;
                return SUCCESS_RETURN;
            }
        }
    }

    while (1) {
        uint32_t readLen = 0;

        if (client_data->is_chunked && client_data->retrieve_len <= 0) {
            /* Read chunk header */
            int foundCrlf;
            int n;
            do {
                foundCrlf = IOT_FALSE;
                crlf_pos = 0;
                data[len] = 0;
                if (len >= 2) {
                    for (; crlf_pos < len - 2; crlf_pos++) {
                        if (data[crlf_pos] == '\r' && data[crlf_pos + 1] == '\n') {
                            foundCrlf = IOT_TRUE;
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

            /* chunk length */
            /* n = sscanf(data, "%x", &readLen); */

            readLen = strtoul(data, NULL, 16);
            n = (0 == readLen) ? 0 : 1;
            client_data->retrieve_len = readLen;
            client_data->response_content_len += client_data->retrieve_len;
            if (readLen == 0) {
                /* Last chunk */
                client_data->is_more = IOT_FALSE;
                utils_debug("no more (last chunk)");
            }

            if (n != 1) {
                utils_err("Could not read chunk length");
                return ERROR_HTTP_UNRESOLVED_DNS;
            }

            memmove(data, &data[crlf_pos + 2], len - (crlf_pos + 2)); /* Not need to move NULL-terminating char any more */
            len -= (crlf_pos + 2);
        } else {
            readLen = client_data->retrieve_len;
        }

        utils_debug("Total-Payload: %d Bytes; Read: %d Bytes", readLen, len);

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
                utils_debug("memmove %d %d %d\n", readLen, len, client_data->retrieve_len);
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
                utils_err("Format error, %s", data); /* after memmove, the beginning of next chunk */
                return ERROR_HTTP_UNRESOLVED_DNS;
            }
            memmove(data, &data[2], len - 2); /* remove the \r\n */
            len -= 2;
        } else {
            utils_debug("no more (content-length)");
            client_data->is_more = IOT_FALSE;
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
    char *tmp_ptr, *ptr_body_end;

    int new_trf_len, ret;

    iotx_time_init(&timer);
    utils_time_countdown_ms(&timer, timeout_ms);

    client_data->response_content_len = -1;

    /* http client response */
    /* <status-line> HTTP/1.1 200 OK(CRLF)

       <headers> ...(CRLF)

       <blank line> (CRLF)

      [<response-body>] */
    char *crlf_ptr = strstr(data, "\r\n");
    if (crlf_ptr == NULL) {
        utils_err("\r\n not found");
        return ERROR_HTTP_UNRESOLVED_DNS;
    }

    crlf_pos = crlf_ptr - data;
    data[crlf_pos] = '\0';

    /* Parse HTTP response */
#if 0
    if (sscanf(data, "HTTP/%*d.%*d %d %*[^\r\n]", &(client->response_code)) != 1) {
        /* Cannot match string, error */
        utils_err("Not a correct HTTP answer : %s\n", data);
        return ERROR_HTTP_UNRESOLVED_DNS;
    }
#endif

    client->response_code = atoi(data + 9);

    if ((client->response_code < 200) || (client->response_code >= 400)) {
        /* Did not return a 2xx code; TODO fetch headers/(&data?) anyway and implement a mean of writing/reading headers */
        utils_warning("Response code %d", client->response_code);
    }

    utils_debug("Reading headers: %s", data);

    memmove(data, &data[crlf_pos + 2], len - (crlf_pos + 2) + 1); /* Be sure to move NULL-terminating char as well */
    len -= (crlf_pos + 2);       /* remove status_line length */

    client_data->is_chunked = IOT_FALSE;

    /*If not ending of response body*/
    /* try to read more header again until find response head ending "\r\n\r\n" */
    while (NULL == (ptr_body_end = strstr(data, "\r\n\r\n"))) {
        /* try to read more header */
        ret = httpclient_recv(client, data + len, 1, HTTPCLIENT_RAED_HEAD_SIZE, &new_trf_len, iotx_time_left(&timer));
        if (ret == ERROR_HTTP_CONN) {
            return ret;
        }
        len += new_trf_len;
        data[len] = '\0';
    }

    /* parse response_content_len */
    if (NULL != (tmp_ptr = strstr(data, "Content-Length"))) {
        client_data->response_content_len = atoi(tmp_ptr + strlen("Content-Length: "));
        client_data->retrieve_len = client_data->response_content_len;
    } else if (NULL != (tmp_ptr = strstr(data, "Transfer-Encoding"))) {
        int len_chunk = strlen("Chunked");
        char *chunk_value = data + strlen("Transfer-Encoding: ");

        if ((! memcmp(chunk_value, "Chunked", len_chunk))
            || (! memcmp(chunk_value, "chunked", len_chunk))) {
            client_data->is_chunked = IOT_TRUE;
            client_data->response_content_len = 0;
            client_data->retrieve_len = 0;
        }
    } else {
        utils_err("Could not parse header");
        return ERROR_HTTP;
    }

    /* remove header length */
    /* len is Had read body's length */
    /* if client_data->response_content_len != 0, it is know response length */
    /* the remain length is client_data->response_content_len - len */
    len = len - (ptr_body_end + 4 - data);
    memmove(data, ptr_body_end + 4, len + 1);
    client_data->response_received_len += len;
    return httpclient_retrieve_content(client, data, len, iotx_time_left(&timer), client_data);
}

int httpclient_connect(httpclient_t *client)
{
    int ret = ERROR_HTTP_CONN;

    client->net.handle = 0;

    ret = httpclient_conn(client);
    /*    if (0 == ret) */
    /*    { */
    /*        client->remote_port = HTTP_PORT; */
    /*    } */

    return ret;
}

int httpclient_send_request(httpclient_t *client, const char *url, HTTPCLIENT_REQUEST_TYPE method,
                            httpclient_data_t *client_data)
{
    int ret = ERROR_HTTP_CONN;

    if (0 == client->net.handle) {
        utils_err("not connection have been established");
        return ret;
    }

    ret = httpclient_send_header(client, url, method, client_data);
    if (ret != 0) {
        utils_err("httpclient_send_header is error,ret = %d", ret);
        return ret;
    }

    if (method == HTTPCLIENT_POST || method == HTTPCLIENT_PUT) {
        ret = httpclient_send_userdata(client, client_data);
    }

    return ret;
}

int httpclient_recv_response(httpclient_t *client, uint32_t timeout_ms, httpclient_data_t *client_data)
{
    int reclen = 0, ret = ERROR_HTTP_CONN;
    char buf[HTTPCLIENT_CHUNK_SIZE] = { 0 };
    iotx_time_t timer;

    iotx_time_init(&timer);
    utils_time_countdown_ms(&timer, timeout_ms);

    if (0 == client->net.handle) {
        utils_err("not connection have been established");
        return ret;
    }

    if (client_data->is_more) {
        client_data->response_buf[0] = '\0';
        ret = httpclient_retrieve_content(client, buf, reclen, iotx_time_left(&timer), client_data);
    } else {
        client_data->is_more = 1;
        /* try to read header */
        ret = httpclient_recv(client, buf, 1, HTTPCLIENT_RAED_HEAD_SIZE, &reclen, iotx_time_left(&timer));
        if (ret != 0) {
            return ret;
        }

        buf[reclen] = '\0';

        if (reclen) {
            log_multi_line(LOG_DEBUG_LEVEL, "RESPONSE", "%s", buf, "<");
            ret = httpclient_response_parse(client, buf, reclen, iotx_time_left(&timer), client_data);
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
    utils_info("client disconnected");
}

int httpclient_common(httpclient_t *client, const char *url, int port, const char *ca_crt,
                      HTTPCLIENT_REQUEST_TYPE method, uint32_t timeout_ms, httpclient_data_t *client_data)
{
    iotx_time_t timer;
    int ret = 0;
    char host[HTTPCLIENT_MAX_HOST_LEN] = { 0 };

    httpclient_parse_host(url, host, sizeof(host));
    utils_info("host: '%s', port: %d", host, port);

    if (0 == client->net.handle) {
        /* Establish connection if no. */
        ret = iotx_net_init(&client->net, host, port, ca_crt, NULL);
        if (0 != ret) {
            return ret;
        }

        ret = httpclient_connect(client);
        if (0 != ret) {
            utils_err("httpclient_connect is error, ret = %d", ret);
            httpclient_close(client);
            return ret;
        }

        ret = httpclient_send_request(client, url, method, client_data);
        if (0 != ret) {
            utils_err("httpclient_send_request is error, ret = %d", ret);
            httpclient_close(client);
            return ret;
        }
    }

    iotx_time_init(&timer);
    utils_time_countdown_ms(&timer, timeout_ms);

    if ((NULL != client_data->response_buf)
        && (0 != client_data->response_buf_len)) {
        ret = httpclient_recv_response(client, iotx_time_left(&timer), client_data);
        if (ret < 0) {
            utils_err("httpclient_recv_response is error,ret = %d", ret);
            httpclient_close(client);
            return ret;
        }
    }

    if (! client_data->is_more) {
        /* Close the HTTP if no more data. */
        utils_info("close http channel");
        httpclient_close(client);
    }

    ret = 0;
    return ret;
}

int utils_get_response_code(httpclient_t *client)
{
    return client->response_code;
}

int iotx_post(httpclient_t *client,
              const char *url,
              int port,
              const char *ca_crt,
              httpclient_data_t *client_data)
{
    /* return httpclient_common(client, url, port, ca_crt, HTTPCLIENT_POST, timeout_ms, client_data); */
    int ret = ERROR_HTTP;
    char host[HTTPCLIENT_MAX_HOST_LEN] = { 0 };

    httpclient_parse_host(url, host, sizeof(host));
    utils_info("host: '%s', port: %d", host, port);

    if (0 == client->net.handle) {
        /* Establish connection if no. */
        ret = iotx_net_init(&client->net, host, port, ca_crt, NULL);
        if (0 != ret) {
            return ret;
        }

        ret = httpclient_connect(client);
        if (0 != ret) {
            utils_err("httpclient_connect is error, ret = %d", ret);
            httpclient_close(client);
            return ret;
        }
    }

    ret = httpclient_send_request(client, url, HTTPCLIENT_POST, client_data);
    if (0 != ret) {
        utils_err("httpclient_send_request is error, ret = %d", ret);
        httpclient_close(client);
        return ret;
    }

    return ret;
}


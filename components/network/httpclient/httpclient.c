/* Copyright (C) 2012 mbed.org, MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute,
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <unistd.h>
#include "httpclient.h"
#include "stdio.h"
#include "string.h"
#include "network/network.h"
#ifdef HTTPCLIENT_SSL_ENABLE
#include "mbedtls/debug.h"
#endif

#include "lwip/opt.h"

#include "ulog/ulog.h"
#define HTTPCLIENT_DEBUG 0
#define TAG "httpclient"

#if HTTPCLIENT_DEBUG
#define ERR(fmt,arg...)   LOGE(TAG, "[HTTPClient]: "fmt,##arg)
#define WARN(fmt,arg...)   LOGW(TAG, "[HTTPClient]: "fmt,##arg)
#define DBG(fmt,arg...)   LOGI(TAG, "[HTTPClient]: "fmt,##arg)
#define INFO(fmt,arg...)   LOGI(TAG, "[HTTPClient]: "fmt,##arg)
#else
#define DBG(fmt,arg...)
#define WARN(fmt,arg...)   LOGW(TAG, "[HTTPClient]: "fmt,##arg)
#define ERR(fmt,arg...)    LOGE(TAG, "[HTTPClient]: "fmt,##arg)
#define INFO(fmt,arg...)   LOGI(TAG, "[HTTPClient]: "fmt,##arg)
#endif

#define MIN(x,y) (((x)<(y))?(x):(y))
#define MAX(x,y) (((x)>(y))?(x):(y))

#define HTTPCLIENT_AUTHB_SIZE     128

#ifndef HTTPCLIENT_CHUNK_SIZE
#define HTTPCLIENT_CHUNK_SIZE     1024
#endif

#ifndef HTTPCLIENT_SEND_BUF_SIZE
#define HTTPCLIENT_SEND_BUF_SIZE  512
#endif

#ifndef HTTPCLIENT_MAX_HOST_LEN
#define HTTPCLIENT_MAX_HOST_LEN   256
#endif

#ifndef HTTPCLIENT_MAX_URL_LEN
#define HTTPCLIENT_MAX_URL_LEN    2048
#endif

#if defined(MBEDTLS_DEBUG_C)
#define DEBUG_LEVEL 2
#endif

#define HTTP_DATA_SIZE    1500

#ifndef FORM_NODE_FILE_PATH_LEN
#define FORM_NODE_FILE_PATH_LEN 256
#endif

typedef struct formdata_node_t formdata_node_t;
struct formdata_node_t
{
    formdata_node_t *next;
    int is_file;
    char file_path[FORM_NODE_FILE_PATH_LEN];
    char* data;
    int  data_len;
};

typedef struct {
    int is_used;
    formdata_node_t *form_data;
    httpclient_data_t* client_data;
} formdata_info_t;

#define CLIENT_FORM_DATA_NUM  1
static formdata_info_t formdata_info[CLIENT_FORM_DATA_NUM] = {0};

// static int httpclient_parse_host(char *url, char *host, size_t maxhost_len);
static int httpclient_parse_url(const char *url, char *scheme, size_t max_scheme_len, char *host, size_t maxhost_len, int *port, char *path, size_t max_path_len);
static int httpclient_tcp_send_all(int sock_fd, char *data, int length);
static int httpclient_conn(httpclient_t *client, char *host);
static int httpclient_recv(httpclient_t *client, char *buf, int min_len, int max_len, int *p_read_len);
static int httpclient_retrieve_content(httpclient_t *client, char *data, int len, httpclient_data_t *client_data);
static int httpclient_response_parse(httpclient_t *client, char *data, int len, httpclient_data_t *client_data);
#ifdef HTTPCLIENT_SSL_ENABLE
static int httpclient_ssl_conn(httpclient_t *client, char *host);
static int httpclient_ssl_send_all(mbedtls_ssl_context *ssl, const char *data, size_t length);
static int httpclient_ssl_nonblock_recv(void *ctx, unsigned char *buf, size_t len);
static int httpclient_ssl_close(httpclient_t *client);
#endif

static void httpclient_base64enc(char *out, const char *in)
{
    const char code[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=" ;
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
    out[i] = '\0' ;
}

int httpclient_conn(httpclient_t *client, char *host)
{
    struct addrinfo hints, *addr_list, *cur;
    int ret = 0;
    char port[10] = {0};

    memset( &hints, 0, sizeof( hints ) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    snprintf(port, sizeof(port), "%d", client->remote_port) ;
    if ( getaddrinfo( host, port , &hints, &addr_list ) != 0 ) {
        ERR("getaddrinfo != 0, return HTTPCLIENT_UNRESOLVED_DNS");
        return HTTPCLIENT_UNRESOLVED_DNS;
    }

    /* Try the sockaddrs until a connection succeeds */
    ret = HTTPCLIENT_UNRESOLVED_DNS;
    for ( cur = addr_list; cur != NULL; cur = cur->ai_next ) {
        client->socket = (int) socket( cur->ai_family, cur->ai_socktype,
                                        cur->ai_protocol );
        if ( client->socket < 0 ) {
            ERR("%s %d socket create failed\n", __func__, __LINE__);
            ret = HTTPCLIENT_ERROR_CONN;
            continue;
        }

        if ( connect( client->socket, cur->ai_addr, (int)cur->ai_addrlen ) == 0 ) {
            ret = 0;
            break;
        }

        ERR("%s %d socket %d connect failed\n", __func__, __LINE__, client->socket);

        close( client->socket );
        ret = HTTPCLIENT_ERROR_CONN;
    }

    freeaddrinfo( addr_list );

    return ret;
}

int httpclient_parse_url(const char *url, char *scheme, size_t max_scheme_len, char *host, size_t maxhost_len, int *port, char *path, size_t max_path_len)
{
    char *scheme_ptr = (char *) url;
    char *host_ptr = NULL;
    size_t host_len = 0;
    size_t path_len = 0;
    char *port_ptr = NULL;
    char *path_ptr = NULL;
    char *fragment_ptr = NULL;

    if (url == NULL) {
        WARN("Could not find url");
        return HTTPCLIENT_ERROR_PARSE;
    }

    host_ptr = (char *) strstr(url, "://");

    if (host_ptr == NULL) {
        WARN("Could not find host");
        return HTTPCLIENT_ERROR_PARSE; /* URL is invalid */
    }

    if ( max_scheme_len < host_ptr - scheme_ptr + 1 ) { /* including NULL-terminating char */
        WARN("Scheme str is too small (%d >= %d)", max_scheme_len, host_ptr - scheme_ptr + 1);
        return HTTPCLIENT_ERROR_PARSE;
    }
    memcpy(scheme, scheme_ptr, host_ptr - scheme_ptr);
    scheme[host_ptr - scheme_ptr] = '\0';

    host_ptr += 3;

    port_ptr = strchr(host_ptr, ':');
    path_ptr = strchr(host_ptr, '/');
    if (NULL == path_ptr) {
        WARN("Failed to find url slash");
        return HTTPCLIENT_ERROR_PARSE;
    }

    if ( port_ptr != NULL && port_ptr < path_ptr) {
        uint16_t tport;
        host_len = port_ptr - host_ptr;
        port_ptr++;
        if ( sscanf(port_ptr, "%hu", &tport) != 1) {
            WARN("Could not find port");
            return HTTPCLIENT_ERROR_PARSE;
        }
        *port = (int)tport;
    } else {
        *port = 0;
    }

    if ( host_len == 0 ) {
        host_len = path_ptr - host_ptr;
    }

    if ( maxhost_len < host_len + 1 ) { /* including NULL-terminating char */
        WARN("Host str is too small (%d >= %d)", maxhost_len, host_len + 1);
        return HTTPCLIENT_ERROR_PARSE;
    }
    memcpy(host, host_ptr, host_len);
    host[host_len] = '\0';

    fragment_ptr = strchr(host_ptr, '#');
    if (fragment_ptr != NULL) {
        path_len = fragment_ptr - path_ptr;
    } else {
        path_len = strlen(path_ptr);
    }

    if ( max_path_len < path_len + 1 ) { /* including NULL-terminating char */
        WARN("Path str is too small (%d >= %d)", max_path_len, path_len + 1);
        return HTTPCLIENT_ERROR_PARSE;
    }
    memcpy(path, path_ptr, path_len);
    path[path_len] = '\0';

    return HTTPCLIENT_OK;
}

#if 0
int httpclient_parse_host(char *url, char *host, size_t maxhost_len)
{
    char *host_ptr = (char *) strstr(url, "://");
    size_t host_len = 0;
    char *port_ptr;
    char *path_ptr;

    if (host_ptr == NULL) {
        WARN("Could not find host");
        return HTTPCLIENT_ERROR_PARSE; /* URL is invalid */
    }
    host_ptr += 3;

    port_ptr = strchr(host_ptr, ':');
    if ( port_ptr != NULL ) {
        uint16_t tport;
        host_len = port_ptr - host_ptr;
        port_ptr++;
        if ( sscanf(port_ptr, "%hu", &tport) != 1) {
            WARN("Could not find port");
            return HTTPCLIENT_ERROR_PARSE;
        }
    }

    path_ptr = strchr(host_ptr, '/');
    if ( host_len == 0 ) {
        host_len = path_ptr - host_ptr;
    }

    if ( maxhost_len < host_len + 1 ) { /* including NULL-terminating char */
        WARN("Host str is too small (%d >= %d)", maxhost_len, host_len + 1);
        return HTTPCLIENT_ERROR_PARSE;
    }
    memcpy(host, host_ptr, host_len);
    host[host_len] = '\0';

    return HTTPCLIENT_OK;
}
#endif

int httpclient_get_info(httpclient_t *client, char *send_buf, int send_buf_size, int *send_idx, char *buf, size_t len)   /* 0 on success, err code on failure */
{
    int ret = 0;
    int cp_len ;
    int idx = *send_idx;

    if (len == 0) {
        len = strlen(buf);
    }

    do {
        if ((send_buf_size - idx) >= len) {
            cp_len = len ;
        } else {
            cp_len = send_buf_size - idx ;
        }

        memcpy(send_buf + idx, buf, cp_len) ;
        idx += cp_len ;
        len -= cp_len ;

        if (idx == send_buf_size) {
            WARN("http requst header buf full!\n");

#ifdef HTTPCLIENT_SSL_ENABLE
            if (client->is_http == false) {
                httpclient_ssl_t *ssl = (httpclient_ssl_t *)client->ssl;
                ret = httpclient_ssl_send_all(&ssl->ssl_ctx, send_buf, send_buf_size);
            } else
#endif
            {
                ret = httpclient_tcp_send_all(client->socket, send_buf, send_buf_size) ;
            }

            if (ret == send_buf_size) {
                idx -= ret;
                buf += cp_len;
            } else if (0 == ret) {
                return HTTPCLIENT_CLOSED;
            } else {
                return HTTPCLIENT_ERROR;
            }
        }
    } while (len) ;

    *send_idx = idx;
    return HTTPCLIENT_OK ;
}

void httpclient_set_custom_header(httpclient_t *client, char *header)
{
    client->header = header ;
}

int httpclient_basic_auth(httpclient_t *client, char *user, char *password)
{
    if ((strlen(user) + strlen(password)) >= HTTPCLIENT_AUTHB_SIZE) {
        return HTTPCLIENT_ERROR ;
    }
    client->auth_user = user;
    client->auth_password = password;
    return HTTPCLIENT_OK ;
}

int httpclient_send_auth(httpclient_t *client, char *send_buf, int send_buf_size, int *send_idx)
{
    int ret = HTTPCLIENT_OK;
    char b_auth[(int)((HTTPCLIENT_AUTHB_SIZE + 3) * 4 / 3 + 3) + 3] = {0};
    char base64buff[HTTPCLIENT_AUTHB_SIZE + 3] = {0};

    ret = httpclient_get_info(client, send_buf, send_buf_size, send_idx, "Authorization: Basic ", 0) ;
    if (ret != HTTPCLIENT_OK) {
        ERR("%s %d httpclient_get_info failed\n", __func__, __LINE__);
        return ret;
    }

    snprintf(base64buff, sizeof(base64buff), "%s:%s", client->auth_user, client->auth_password) ;
    DBG("bAuth: %s", base64buff) ;
    httpclient_base64enc(b_auth, base64buff) ;
    b_auth[strlen(b_auth) + 2] = '\0' ;
    b_auth[strlen(b_auth) + 1] = '\n' ;
    b_auth[strlen(b_auth)] = '\r' ;
    DBG("b_auth:%s", b_auth) ;

    ret = httpclient_get_info(client, send_buf, send_buf_size, send_idx, b_auth, 0) ;
    if (ret != HTTPCLIENT_OK) {
        ERR("%s %d httpclient_get_info failed\n", __func__, __LINE__);
        return ret;
    }

    return HTTPCLIENT_OK ;
}

int httpclient_tcp_send_all(int sock_fd, char *data, int length)
{
    int written_len = 0;

    while (written_len < length) {
        int ret = send(sock_fd, data + written_len, length - written_len, 0);
        if (ret > 0) {
            written_len += ret;
            continue;
        } else if (ret == 0) {
            return written_len;
        } else {
            ERR("Connection err ret=%d errno=%d\n", ret, errno);
            return -1; /* Connnection error */
        }
    }

    return written_len;
}

static formdata_info_t* found_formdata_info(httpclient_data_t * client_data) {
    int i;

    for(i = 0; i < CLIENT_FORM_DATA_NUM; i++) {
        if((formdata_info[i].client_data == client_data)
           && (formdata_info[i].is_used == 1)) {
            break;
        }
    }

    if(i == CLIENT_FORM_DATA_NUM) {
        return NULL;
    }

    return &formdata_info[i];
}

static formdata_info_t* found_empty_formdata_info() {
    int i;

    for(i = 0; i < CLIENT_FORM_DATA_NUM; i++) {
        if(formdata_info[i].is_used == 0) {
            break;
        }
    }

    if(i == CLIENT_FORM_DATA_NUM) {
        return NULL;
    }

    return &formdata_info[i];
}

#define TEXT_FORMAT              "\r\nContent-Disposition: %s; name=\"%s\"\r\n\r\n%s\r\n"
#define TEXT_CONTENT_TYPE_FORMAT "\r\nContent-Disposition :%s; name=\"%s\"\r\nContent-Type:%s\r\n\r\n%s\r\n"
int httpclient_formdata_addtext(httpclient_data_t* client_data, char* content_disposition, char* content_type, char* name, char* data, int data_len)
{
    int buf_len;
    formdata_info_t* data_info;
    formdata_node_t* previous;
    formdata_node_t* current;

    if((content_disposition == NULL) || (name == NULL) || (data == NULL) || (data_len == 0)) {
        ERR("%s:%d invalid params\n", __func__, __LINE__);
        return -1;
    }

    if(strlen(data) > data_len) {
        return -1;
        ERR("%s:%d invalid data_len(%d) strlen(data)=%d\n", __func__, __LINE__, data_len, strlen(data));
    }

    if((data_info = found_formdata_info(client_data)) == NULL) {
        if((data_info = found_empty_formdata_info()) == NULL) {
            ERR("%s:%d found no client_data info\n", __func__, __LINE__);
            return -1;
        }
    }

    if(data_info->is_used == 0) {
        data_info->is_used = 1;
        data_info->client_data = client_data;
        data_info->form_data = (formdata_node_t *)malloc(sizeof(formdata_node_t));
        if(data_info->form_data == NULL) {
            data_info->is_used = 0;
            ERR("%s:%d form data malloc failed\n", __func__, __LINE__);
            return -1;
        }
        previous = data_info->form_data;
        current = data_info->form_data;
    }
    else {
        current = data_info->form_data;

        while(current->next != NULL) {
            current = current->next;
        }

        current->next = (formdata_node_t *)malloc(sizeof(formdata_node_t));
        if(current->next == NULL) {
            ERR("%s:%d form data malloc failed\n", __func__, __LINE__);
            return -1;
        }
        previous = current;
        current = current->next;
    }

    memset(current, 0, sizeof(formdata_node_t));
    if(content_type == NULL) {
        buf_len = strlen(TEXT_FORMAT) - 6 + strlen(content_disposition) + strlen(name) + strlen(data) + 1;
    }
    else {
        buf_len = strlen(TEXT_CONTENT_TYPE_FORMAT) - 8 + strlen(content_disposition) + strlen(name) + strlen(data) + strlen(content_type) + 1;
    }
    current->data = (char*)malloc(buf_len+1);
    if( current->data == NULL) {
        if(previous == current ) {
            free(current);
            memset(data_info, 0, sizeof(formdata_info_t));
        }
        else {
            free(current);
            previous->next = NULL;
        }
        ERR("%s:%d data malloc failed\n", __func__, __LINE__);
        return -1;
    }
    memset(current->data, 0, sizeof(buf_len));
    if(content_type == NULL) {
        snprintf(current->data, buf_len, TEXT_FORMAT, content_disposition, name, data);
        current->data_len = strlen(current->data);
    }
    else {
        snprintf(current->data, buf_len, TEXT_CONTENT_TYPE_FORMAT, content_disposition, name, content_type, data);
        current->data_len = strlen(current->data);
    }
    return 0;
}

static int get_url_file_name(char* url)
{
    char * ptr = url;
    int offset = 0;
    int i = 0;

    while(*ptr != '\0')
    {
        i++;
        if(*ptr == '/') {
            offset = i;
        }
        ptr++;
    }
    return offset;
}

#define FILE_FORMAT_START                "\r\nContent-Disposition: %s; name=\"%s\"; filename=\"%s\"\r\n"
#define FILE_FORMAT_END                  "\r\nContent-Disposition: %s; name=\"%s\"; filename=\"\"\r\n"
#define FILE_FORMAT_CONTENT_TYPE_START   "\r\nContent-Disposition: %s; name=\"%s\"; filename=\"%s\"\r\nContent-Type: %s\r\n\r\n"
#define FILE_FORMAT_ATTACHMENT           "\r\nContent-Disposition: %s; filename=\"%s\"\r\nContent-Type: %s\r\n\r\n"
int httpclient_formdata_addfile(httpclient_data_t* client_data, char* content_disposition, char* name, char* content_type, char* file_path)
{
    int buf_len;
    formdata_info_t* data_info;
    formdata_node_t* previous;
    formdata_node_t* current;

    if((content_disposition == NULL) || (file_path == NULL)) {
        ERR("%s:%d invalid params\n", __func__, __LINE__);
        return -1;
    }

    if((data_info = found_formdata_info(client_data)) == NULL) {
        if((data_info = found_empty_formdata_info()) == NULL) {
            ERR("%s:%d found no client_data info\n", __func__, __LINE__);
            return -1;
        }
    }

    if(data_info->is_used == 0) {
        data_info->is_used = 1;
        data_info->client_data = client_data;
        data_info->form_data = (formdata_node_t *)malloc(sizeof(formdata_node_t));
        if(data_info->form_data == NULL) {
            data_info->is_used = 0;
            ERR("%s:%d data malloc failed\n", __func__, __LINE__);
            return -1;
        }

        previous = data_info->form_data;
        current = data_info->form_data;
    }
    else {
        current = data_info->form_data;

        while(current->next != NULL) {
            current = current->next;
        }

        current->next = (formdata_node_t *)malloc(sizeof(formdata_node_t));
        if(current->next == NULL) {
            ERR("%s:%d data malloc failed\n", __func__, __LINE__);
            return -1;
        }
        previous = current;
        current = current->next;
    }

    memset(current, 0, sizeof(formdata_node_t));
    if (name == NULL) {
        if (content_type != NULL) {
            buf_len = strlen(FILE_FORMAT_ATTACHMENT) - 6 + strlen(content_disposition) + strlen(file_path) - get_url_file_name(file_path) + strlen(content_type) + 1;
        } else {
            ERR("%s:%d unknown format\n", __func__, __LINE__);
            return -1;
        }
    } else {
        if(content_type == NULL) {
            buf_len = strlen(FILE_FORMAT_START) - 6 + strlen(content_disposition) + strlen(name) + strlen(file_path) - get_url_file_name(file_path) + 1;
        }
        else {
            buf_len = strlen(FILE_FORMAT_CONTENT_TYPE_START) - 8 + strlen(content_disposition) + strlen(name) + strlen(file_path) - get_url_file_name(file_path) + strlen(content_type) + 1;
        }
    }
    current->data = (char*)malloc(buf_len + 1);
    if( current->data == NULL) {
        if(previous == current ) {
            free(current);
            data_info->form_data = NULL;
        }
        else {
            free(current);
            previous->next = NULL;
        }
        ERR("%s:%d data malloc failed\n", __func__, __LINE__);
        return -1;
    }
    memset(current->data, 0, sizeof(buf_len));

    current->is_file = 1;

    memcpy(current->file_path, file_path, strlen(file_path));
    if (name == NULL) {
        if (content_type != NULL) {
            snprintf(current->data, buf_len, FILE_FORMAT_ATTACHMENT, content_disposition, file_path + get_url_file_name(file_path), content_type);
        } else {
            ERR("%s:%d unknown format\n", __func__, __LINE__);
            return -1;
        }
    } else{
        if(content_type == NULL) {
            snprintf(current->data, buf_len, FILE_FORMAT_START, content_disposition, name, file_path + get_url_file_name(file_path));
        }
        else {
            snprintf(current->data, buf_len, FILE_FORMAT_CONTENT_TYPE_START, content_disposition, name, file_path + get_url_file_name(file_path), content_type);
        }
    }

    current->data_len = strlen(current->data);
    return 0;
}

void form_data_clear(formdata_node_t* form_data) {
    if(form_data != NULL) {
        form_data_clear(form_data->next);
        form_data->next = NULL;
        if(form_data->data != NULL) {
            free(form_data->data);
        }
        free(form_data);
    }
}

void httpclient_clear_form_data(httpclient_data_t * client_data)
{
    formdata_info_t * data_info;
    formdata_node_t * current;

    data_info = found_formdata_info(client_data);

    if(data_info == NULL) {
        ERR("No form data info found\n");
        return;
    }

    current = data_info->form_data;
    if(current != NULL) {
        form_data_clear(current);
    }
    else {
        ERR("No form data in form data info\n");
    }

    memset(data_info, 0, sizeof(formdata_info_t));
}

static const char *boundary = "----WebKitFormBoundarypNjgoVtFRlzPquKE";
int httpclient_send_header(httpclient_t *client, const char *url, int method, httpclient_data_t *client_data)
{
    char scheme[8] = {0};
    char *host = NULL;
    char *path = NULL;
    char *send_buf = NULL;
    char *buf = NULL;

    int host_size = 0;
    int path_size = 0;
    int send_buf_size = 0;
    int buf_size = 0;

    int len;
    int total_len = 0;
    int url_len = 0;
    int ret = HTTPCLIENT_ERROR_CONN;
    int port;
    formdata_info_t* data_info = NULL;
    char *meth = (method == HTTPCLIENT_GET) ? "GET" : (method == HTTPCLIENT_POST) ? "POST" : (method == HTTPCLIENT_PUT) ? "PUT" : (method == HTTPCLIENT_DELETE) ? "DELETE" : (method == HTTPCLIENT_HEAD) ? "HEAD" : "";
    char *keep_alive = "close";

    url_len = strlen(url);
    if (url_len <= 0 || url_len >= HTTPCLIENT_MAX_URL_LEN) {
        ERR("request url len err %d", url_len);
        ret = HTTPCLIENT_ERROR_CONN;
        goto done;
    }

    host = (char *) malloc(HTTPCLIENT_MAX_HOST_LEN);
    if (!host) {
        ERR("%s %d malloc failed", __func__, __LINE__);
        ret = HTTPCLIENT_ERROR_CONN;
        goto done;
    }
    host_size = HTTPCLIENT_MAX_HOST_LEN;
    memset(host, 0, host_size);

    path = (char *) malloc(url_len);
    if (!path) {
        ERR("%s %d malloc failed", __func__, __LINE__);
        ret = HTTPCLIENT_ERROR_CONN;
        goto done;
    }
    path_size = url_len;
    memset(path, 0, path_size);

    send_buf = (char *) malloc(url_len + HTTPCLIENT_SEND_BUF_SIZE);
    if (!send_buf) {
        ERR("%s %d malloc failed", __func__, __LINE__);
        ret = HTTPCLIENT_ERROR_CONN;
        goto done;
    }
    send_buf_size = HTTPCLIENT_SEND_BUF_SIZE + url_len;
    memset(send_buf, 0, send_buf_size);

    buf = (char *) malloc(url_len + HTTPCLIENT_SEND_BUF_SIZE);
    if (!buf) {
        ERR("%s %d malloc failed", __func__, __LINE__);
        ret = HTTPCLIENT_ERROR_CONN;
        goto done;
    }
    buf_size = url_len + HTTPCLIENT_SEND_BUF_SIZE;
    memset(buf, 0, buf_size);

    /* First we need to parse the url (http[s]://host[:port][/[path]]) */
    ret = httpclient_parse_url(url, scheme, sizeof(scheme), host, host_size, &(port), path, path_size);
    if (ret != HTTPCLIENT_OK) {
        ERR("httpclient_parse_url returned %d", ret);
        goto done;
    }

    /* Send request */
    len = 0 ; /* Reset send buffer */

    if (client->keep_alive) {
        keep_alive = "keep-alive";
    }

    snprintf(buf, buf_size, "%s %s HTTP/1.1\r\nUser-Agent: AliOS-HTTP-Client/2.1\r\nCache-Control: no-cache\r\nConnection: %s\r\nHost: %s\r\n",
             meth, path, keep_alive, host); /* Write request */
    ret = httpclient_get_info(client, send_buf, send_buf_size, &len, buf, strlen(buf));
    if (ret) {
        ERR("Could not write request");
        goto done;
    }

    /* Send all headers */
    if (client->auth_user) {
        ret = httpclient_send_auth(client, send_buf, send_buf_size, &len) ; /* send out Basic Auth header */
        if (ret != HTTPCLIENT_OK) {
            ERR("%s %d failed\n", __func__, __LINE__);
            goto done;
        }
    }

    /* Add user header information */
    if (client->header) {
        ret = httpclient_get_info(client, send_buf, send_buf_size, &len, (char *)client->header, strlen(client->header));
        if (ret != HTTPCLIENT_OK) {
            ERR("%s %d failed\n", __func__, __LINE__);
            goto done;
        }
    }

    if(((data_info = found_formdata_info(client_data)) != NULL) && (data_info->is_used == 1)) {
        formdata_node_t * current;
        current = data_info->form_data;
        /* calculate content-length*/
        do {
            if(current->is_file == 1) {
                FILE* fd;
                long size;

                fd = fopen(current->file_path, "rb");
                if(fd == NULL) {
                    ERR("%s: open file(%s) failed errno=%d\n", __func__, current->file_path, errno);
                    ret = HTTPCLIENT_ERROR_CONN;
                    goto done;
                }

                fseek(fd,0,SEEK_END);
                size=ftell(fd);
                fseek(fd,0,SEEK_SET);
                fclose(fd);
                total_len += size;
            }

            total_len += current->data_len;

            total_len += strlen(boundary) + 4;

            current = current->next;
        }
        while(current != NULL);

        memset(buf, 0, buf_size);
        snprintf(buf, buf_size, "Accept: */*\r\n");
        ret = httpclient_get_info(client, send_buf, send_buf_size, &len, buf, strlen(buf));
        if (ret != HTTPCLIENT_OK) {
            ERR("%s %d failed\n", __func__, __LINE__);
            goto done;
        }

        if (client_data->post_content_type != NULL)  {
            memset(buf, 0, buf_size);
            snprintf(buf, buf_size, "Content-Type: %s\r\n", client_data->post_content_type);
            ret = httpclient_get_info(client, send_buf, send_buf_size, &len, buf, strlen(buf));
            if (ret != HTTPCLIENT_OK) {
                ERR("%s %d failed\n", __func__, __LINE__);
                goto done;
            }
        }
        else {
            memset(buf, 0, buf_size);
            snprintf(buf, buf_size, "Content-Type: multipart/form-data; boundary=%s\r\n", boundary);
            ret = httpclient_get_info(client, send_buf, send_buf_size, &len, buf, strlen(buf));
            if (ret != HTTPCLIENT_OK) {
                ERR("%s %d failed\n", __func__, __LINE__);
                goto done;
            }
        }

        total_len += strlen(boundary) + 8;
        memset(buf, 0, buf_size);
        snprintf(buf, buf_size, "Content-Length: %d\r\n", total_len);
        ret = httpclient_get_info(client, send_buf, send_buf_size, &len, buf, strlen(buf));
        if (ret != HTTPCLIENT_OK) {
            ERR("%s %d failed\n", __func__, __LINE__);
            goto done;
        }
    }
    else if ( client_data->post_buf != NULL ) {
        memset(buf, 0, buf_size);
        snprintf(buf, buf_size, "Content-Length: %d\r\n", client_data->post_buf_len);
        ret = httpclient_get_info(client, send_buf, send_buf_size, &len, buf, strlen(buf));
        if (ret != HTTPCLIENT_OK) {
            ERR("%s %d failed\n", __func__, __LINE__);
            goto done;
        }

        if (client_data->post_content_type != NULL)  {
            memset(buf, 0, buf_size);
            snprintf(buf, buf_size, "Content-Type: %s\r\n", client_data->post_content_type);
            ret = httpclient_get_info(client, send_buf, send_buf_size, &len, buf, strlen(buf));
            if (ret != HTTPCLIENT_OK) {
                ERR("%s %d failed\n", __func__, __LINE__);
                goto done;
            }
        }
    }
    else {
        DBG("Do nothing\n");
    }

    /* Close headers */
    ret = httpclient_get_info(client, send_buf, send_buf_size, &len, "\r\n", 0);
    if (ret != HTTPCLIENT_OK) {
        ERR("%s %d failed\n", __func__, __LINE__);
        goto done;
    }

    DBG("Trying to write %d bytes http header:%s", len, send_buf);

#ifdef HTTPCLIENT_SSL_ENABLE
    if (client->is_http == false) {
        httpclient_ssl_t *ssl = (httpclient_ssl_t *)client->ssl;
        ret = httpclient_ssl_send_all(&ssl->ssl_ctx, send_buf, len);
        if (ret == len) {
            ret = HTTPCLIENT_OK;
        } else {
            ERR("SSL_write failed");
            ret = HTTPCLIENT_ERROR;
        }
    } else
#endif
    {
        ret = httpclient_tcp_send_all(client->socket, send_buf, len);
        if (ret == len) {
            DBG("Written %d bytes, socket = %d", ret, client->socket);
            ret = HTTPCLIENT_OK;
        } else if ( ret == 0 ) {
            WARN("ret == 0,Connection was closed by server");
            ret = HTTPCLIENT_CLOSED; /* Connection was closed by server */
        } else {
            ERR("Connection error (send returned %d)", ret);
            ret = HTTPCLIENT_ERROR_CONN;
        }
    }

done:
    if (host) {
        free(host);
        host = NULL;
    }

    if (path) {
        free(path);
        path = NULL;
    }

    if (send_buf) {
        free(send_buf);
        send_buf = NULL;
    }

    if (buf) {
        free(buf);
        buf = NULL;
    }

    return ret;
}

int httpclient_send_userdata(httpclient_t *client, httpclient_data_t *client_data)
{
    int ret = HTTPCLIENT_OK;
    formdata_info_t* data_info = NULL;
    char *data = NULL;
    int data_size = 0;
    FILE* fd = NULL;

    if (client_data->post_buf && client_data->post_buf_len) {
        DBG("client_data->post_buf:%s", client_data->post_buf);
#ifdef HTTPCLIENT_SSL_ENABLE
        if (client->is_http == false) {
            httpclient_ssl_t *ssl = (httpclient_ssl_t *)client->ssl;
            if (httpclient_ssl_send_all(&ssl->ssl_ctx, client_data->post_buf, client_data->post_buf_len) != client_data->post_buf_len) {
                ERR("SSL_write failed");
                ret = HTTPCLIENT_ERROR;
                goto done;
            }
        } else
#endif
        {
            ret = httpclient_tcp_send_all(client->socket, client_data->post_buf, client_data->post_buf_len);
            if (ret == client_data->post_buf_len) {
                DBG("Written %d bytes", ret);
            } else if ( ret == 0 ) {
                WARN("ret == 0,Connection was closed by server");
                ret = HTTPCLIENT_CLOSED; /* Connection was closed by server */
                goto done;
            } else {
                ERR("Connection error (send returned %d)", ret);
                ret = HTTPCLIENT_ERROR_CONN;
                goto done;
            }
        }
    }
    else if(((data_info = found_formdata_info(client_data)) != NULL) &&(data_info->is_used == 1)) {
        formdata_node_t* current = data_info->form_data;
        data = (char *) malloc(HTTP_DATA_SIZE);
        if (!data) {
            ERR("form data malloc failed\n");
            ret = HTTPCLIENT_ERROR_CONN;
            goto done;
        }

        data_size = HTTP_DATA_SIZE;

        while(current != NULL)
        {
            /* set boundary */
            memset(data, 0, data_size);
            snprintf(data, data_size, "\r\n--%s", boundary);
            ret = httpclient_tcp_send_all(client->socket, data, strlen(data));
            if (ret != strlen(data)) {
                ret = HTTPCLIENT_ERROR_CONN;
                goto done;
            }
            ret = httpclient_tcp_send_all(client->socket, current->data, current->data_len);
            if (ret != current->data_len) {
                ret = HTTPCLIENT_ERROR_CONN;
                goto done;
            }
            if( current->is_file == 1 ) {
                break;
            }
            current = current->next;
        }

        if(current == NULL) {
            ret = HTTPCLIENT_ERROR_CONN;
            goto done;
        }

        fd = fopen(current->file_path, "rb");
        if(fd == NULL)
        {
            ERR("%s: open file(%s) failed errno=%d\n", __func__, current->file_path, errno);
            ret = HTTPCLIENT_ERROR_CONN;
            goto done;
        }

        while(!feof(fd)) {
            int read_size = fread(data, 1, data_size, fd);
            if(read_size <= 0) {
                ERR("fread failed returned %d errno=%d", ret, errno);
                ret = HTTPCLIENT_ERROR_CONN;
                goto done;
            }

            ret = httpclient_tcp_send_all(client->socket, data, read_size);
            if (ret == read_size) {
                DBG("Written %d bytes", ret);
            } else if ( ret == 0 ) {
                WARN("ret == 0,Connection was closed by server");
                ret = HTTPCLIENT_CLOSED; /* Connection was closed by server */
                goto done;
            } else {
                ERR("Connection error (send returned %d) errno=%d", ret, errno);
                ret = HTTPCLIENT_ERROR_CONN;
                goto done;
            }

            memset(data, 0, data_size);
            aos_msleep(100);
        }

        fclose(fd);
        fd = NULL;

        current = current->next;
        while(current != NULL)
        {
            memset(data, 0, data_size);
            snprintf(data, data_size, "\r\n--%s", boundary);
            ret = httpclient_tcp_send_all(client->socket, data, strlen(data));
            if (ret != strlen(data)) {
                ret = HTTPCLIENT_ERROR_CONN;
                goto done;
            }

            ret = httpclient_tcp_send_all(client->socket, current->data, current->data_len);
            if (ret != current->data_len) {
                ret = HTTPCLIENT_ERROR_CONN;
                goto done;
            }
            current = current->next;
        }

        memset(data, 0, data_size);
        snprintf(data, data_size, "\r\n--%s--\r\n", boundary);
        ret = httpclient_tcp_send_all(client->socket, data, strlen(data));
        if (ret != strlen(data)) {
            ret = HTTPCLIENT_ERROR_CONN;
            goto done;
        }

        ret = HTTPCLIENT_OK;
    }

done:
    if (data) {
        free(data);
        data = NULL;
    }

    if (fd) {
        fclose(fd);
        fd = NULL;
    }

    return ret;
}

#ifndef HTTPCLIENT_MAX_RECV_WAIT_TIME_SEC
#define HTTPCLIENT_MAX_RECV_WAIT_TIME_SEC 5
#endif

int httpclient_recv(httpclient_t *client, char *buf, int min_len, int max_len, int *p_read_len)   /* 0 on success, err code on failure */
{
    int ret = 0, select_ret;
    size_t readLen = 0;
    struct timeval timeout;
    fd_set         sets;
    int err_record;
#ifdef HTTPCLIENT_SSL_ENABLE
    httpclient_ssl_t *ssl = NULL;
#endif

    timeout.tv_sec = HTTPCLIENT_MAX_RECV_WAIT_TIME_SEC;
    timeout.tv_usec = 0;

#ifdef HTTPCLIENT_SOCK_PARAM
    /* set socket recv timeout */
    if (client->sock_param.recv_timeout_ms > 0) {
        timeout.tv_sec = client->sock_param.recv_timeout_ms / 1000;
        timeout.tv_usec = (client->sock_param.recv_timeout_ms % 1000) * 1000;
    }
#endif

#ifdef HTTPCLIENT_SSL_ENABLE
    if (!client->is_http) {
        ssl = (httpclient_ssl_t *)client->ssl;
        mbedtls_ssl_set_bio(&ssl->ssl_ctx, &ssl->net_ctx, mbedtls_net_send, mbedtls_net_recv, mbedtls_net_recv_timeout);
        mbedtls_ssl_conf_read_timeout(&ssl->ssl_conf, (timeout.tv_sec * 1000 +  timeout.tv_usec / 1000));
    }
#endif

    while (readLen < max_len) {
        buf[readLen] = '\0';
        if (client->is_http) {
                FD_ZERO(&sets);
                FD_SET(client->socket, &sets);

                DBG("before recv [blocking] return:%d", ret);
                select_ret = select(client->socket + 1, &sets, NULL, NULL, &timeout);
                err_record = errno;
                if (select_ret > 0) {
                    if (0 == FD_ISSET(client->socket, &sets)) {
                        ret = 0;
                        WARN("select continue");
                        continue;
                    }

                    ret = recv(client->socket, buf + readLen, max_len - readLen, 0);
                    err_record = errno;
                    if (ret < 0 ) {
                        if ((EINTR == err_record) || (EAGAIN == err_record) || (EWOULDBLOCK == err_record) ||
                           (EPROTOTYPE == err_record) || (EALREADY == err_record) || (EINPROGRESS == err_record)) {
                            WARN("recv continue errno %d, ret %d", err_record, ret);
                            continue;
                        }
                    } else if (ret == 0) {
                            WARN("recv return 0 disconnected");
                            ret = HTTPCLIENT_CLOSED;
                    }
                } else if (select_ret == 0) {
                    WARN("select return 0 may disconnected");
                    ret = HTTPCLIENT_CLOSED;
                } else {
                    WARN("select return %d", select_ret);
                    if (err_record == EINTR)
                        continue;
                    ret = select_ret;
                }
                DBG("after recv [blocking] return:%d", ret);
        }
#ifdef HTTPCLIENT_SSL_ENABLE
        else {
            DBG("before mbedtls_ssl_read [blocking]");
            ret = mbedtls_ssl_read(&ssl->ssl_ctx, (unsigned char *)buf + readLen, max_len - readLen);
            DBG("mbedtls_ssl_read [blocking] return:%d", ret);
            if (ret == 0) {
                ret = HTTPCLIENT_CLOSED;
            } else if (ret < 0) {
                WARN("mbedtls_ssl_read, return:%d", ret);
                if ((MBEDTLS_ERR_SSL_TIMEOUT == ret)
                       || (MBEDTLS_ERR_SSL_CONN_EOF == ret)
                       || (MBEDTLS_ERR_SSL_SESSION_TICKET_EXPIRED == ret)
                       || (MBEDTLS_ERR_SSL_NON_FATAL == ret)) {
                    /* read already complete */
                    /* if call mbedtls_ssl_read again, it will return 0 (means EOF) */
                }
            }

            if (MBEDTLS_ERR_SSL_WANT_READ == ret) {
                continue;
            }

            if (ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY) {
                ret = HTTPCLIENT_CLOSED;
            }
        }
#endif

        if (ret > 0) {
            readLen += ret;
        } else if (ret == HTTPCLIENT_CLOSED) {
            WARN("ret HTTPCLIENT_CLOSED");
            break;
        } else {
            ERR("Connection error (recv returned %d readLen:%d)", ret,readLen);
            *p_read_len = readLen;
            return HTTPCLIENT_ERROR_CONN;
        }
    }

    DBG("Read %d bytes", readLen);
    *p_read_len = readLen;
    buf[readLen] = '\0';

    if (ret == HTTPCLIENT_CLOSED) {
        return ret;
    } else {
        return HTTPCLIENT_OK;
    }
}

#define HTTPCLIENT_UNSET_RET -100
#define MAX_PRINT_PER_COUNT_LINE 20
int g_httpclient_line =  -1;
int g_httpclient_readlen =  -1;
int g_httpclient_len = -1;
int g_httpclient_ret = HTTPCLIENT_UNSET_RET;
int g_httpclient_print_count = -1;

static void reset_g_httpclient_var(void)
{
    g_httpclient_line =  -1;
    g_httpclient_readlen =  -1;
    g_httpclient_len = -1;
    g_httpclient_ret = HTTPCLIENT_UNSET_RET;
    g_httpclient_print_count = -1;
}

int httpclient_retrieve_content(httpclient_t *client, char *data, int len, httpclient_data_t *client_data)
{
    int count = 0;
    int templen = 0;
    int crlf_pos;
    /* Receive data */
    DBG("Receiving data:%s", data);
    client_data->is_more = true;
    int print_count = 0;

    if (client_data->response_content_len == -1 && client_data->is_chunked == false) {
        while(true)
        {
            int ret, max_len;
            if (count + len < client_data->response_buf_len - 1) {
                memcpy(client_data->response_buf + count, data, len);
                count += len;
                client_data->response_buf[count] = '\0';
            } else {
                memcpy(client_data->response_buf + count, data, client_data->response_buf_len - 1 - count);
                client_data->response_buf[client_data->response_buf_len - 1] = '\0';
                client_data->content_block_len = client_data->response_buf_len - 1;
                return HTTPCLIENT_RETRIEVE_MORE_DATA;
            }

            max_len = MIN(HTTPCLIENT_CHUNK_SIZE - 1, client_data->response_buf_len - 1 - count);
            if (max_len <= 0) {
                ERR("%s %d error max_len %d", __func__, __LINE__, max_len);
                return HTTPCLIENT_ERROR;
            }
            ret = httpclient_recv(client, data, 1, max_len, &len);

            /* Receive data */
            DBG("data len: %d %d", len, count);

            if (ret == HTTPCLIENT_ERROR_CONN) {
                DBG("ret == HTTPCLIENT_ERROR_CONN");
                client_data->content_block_len = count;
                return ret;
            }

            if (len == 0) {/* read no more data */
                DBG("no more len == 0");
                client_data->is_more = false;
                return HTTPCLIENT_OK;
            }
            DBG("in loop %s %d ret %d len %d count %d", __func__, __LINE__, ret, len, count);
        }
    }

    while (true) {
        size_t readLen = 0;

        if ( client_data->is_chunked && client_data->retrieve_len <= 0) {
            /* Read chunk header */
            bool foundCrlf;
            int n;
            do {
                int ret = HTTPCLIENT_UNSET_RET;
                DBG("len: %d", len);
                foundCrlf = false;
                crlf_pos = 0;
                data[len] = 0;
                if (len >= 2) {
                    for (; crlf_pos < len - 2; crlf_pos++) {
                        if ( data[crlf_pos] == '\r' && data[crlf_pos + 1] == '\n' ) {
                            foundCrlf = true;
                            break;
                        }
                    }
                }
                if (!foundCrlf) { /* Try to read more */
                    if ( len < HTTPCLIENT_CHUNK_SIZE ) {
                        int new_trf_len;
                        int max_recv = MIN(client_data->response_buf_len, HTTPCLIENT_CHUNK_SIZE);
                        if (max_recv - len - 1 <= 0) {
                            ERR("%s %d error max_len %d", __func__, __LINE__, max_recv - len - 1);
                            return HTTPCLIENT_ERROR;
                        }
                        ret = httpclient_recv(client, data + len, 0,  max_recv - len - 1 , &new_trf_len);
                        len += new_trf_len;
                        if (ret == HTTPCLIENT_ERROR_CONN || (ret == HTTPCLIENT_CLOSED && new_trf_len == 0)) {
                            return ret;
                        } else {
                            DBG("in loop %s %d ret %d len %d", __func__, __LINE__, ret, len);
                            continue;
                        }
                    } else {
                        return HTTPCLIENT_ERROR;
                    }
                }
                DBG("in loop %s %d len %d ret %d", __func__, __LINE__, len, ret);
            } while (!foundCrlf);
            data[crlf_pos] = '\0';
            n = sscanf(data, "%x", &readLen);/* chunk length */
            client_data->retrieve_len = readLen;
            client_data->response_content_len += client_data->retrieve_len;
            if (n != 1) {
                ERR("Could not read chunk length");
                return HTTPCLIENT_ERROR_PRTCL;
            }

            memmove(data, &data[crlf_pos + 2], len - (crlf_pos + 2)); /* Not need to move NULL-terminating char any more */
            len -= (crlf_pos + 2);

            if ( readLen == 0 ) {
               /* Last chunk */
                client_data->is_more = false;
                DBG("no more (last chunk)");
                break;
            }
        } else {
            readLen = client_data->retrieve_len;
        }

        DBG("Retrieving %d bytes, len:%d", readLen, len);
        reset_g_httpclient_var();

        do {
            int ret = HTTPCLIENT_UNSET_RET;
            DBG("readLen %d, len:%d", readLen, len);
            templen = MIN(len, readLen);
            if (count + templen < client_data->response_buf_len - 1) {
                memcpy(client_data->response_buf + count, data, templen);
                count += templen;
                client_data->response_buf[count] = '\0';
                client_data->retrieve_len -= templen;
            } else {
                memcpy(client_data->response_buf + count, data, client_data->response_buf_len - 1 - count);
                client_data->response_buf[client_data->response_buf_len - 1] = '\0';
                client_data->retrieve_len -= (client_data->response_buf_len - 1 - count);
                client_data->content_block_len = client_data->response_buf_len - 1;
                return HTTPCLIENT_RETRIEVE_MORE_DATA;
            }

            if ( len >= readLen ) {
                DBG("memmove %d %d %d", readLen, len, client_data->retrieve_len);
                memmove(data, &data[readLen], len - readLen); /* chunk case, read between two chunks */
                len -= readLen;
                readLen = 0;
                client_data->retrieve_len = 0;
            } else {
                readLen -= len;
            }

            if (readLen) {
                // int max_len = MIN(HTTPCLIENT_CHUNK_SIZE - 1, client_data->response_buf_len - 1 - count);
                int max_len = MIN(MIN(HTTPCLIENT_CHUNK_SIZE - 1, client_data->response_buf_len - 1 - count), readLen);
                if (max_len <= 0) {
                    ERR("%s %d error max_len %d", __func__, __LINE__, max_len);
                    return HTTPCLIENT_ERROR;
                }

                ret = httpclient_recv(client, data, 1, max_len, &len);
                if (ret == HTTPCLIENT_ERROR_CONN || (ret == HTTPCLIENT_CLOSED && len == 0)) {
                    return ret;
                }
            }

            if (print_count % MAX_PRINT_PER_COUNT_LINE == 0)
                DBG("in loop %s %d readLen %d len %d ret %d print_count %d", __func__, __LINE__, readLen, len, ret, print_count);

            g_httpclient_line =  __LINE__;
            g_httpclient_readlen =  readLen;
            g_httpclient_len = len;
            g_httpclient_ret = ret;
            g_httpclient_print_count = print_count;

            print_count++;
        } while (readLen);

        if ( client_data->is_chunked ) {
            if (len < 2) {
                int new_trf_len = 0, ret;
                int max_recv = MIN(client_data->response_buf_len - 1 - count + 2, HTTPCLIENT_CHUNK_SIZE - len - 1);
                if (max_recv <= 0) {
                    ERR("%s %d error max_len %d", __func__, __LINE__, max_recv);
                    return HTTPCLIENT_ERROR;
                }

                /* Read missing chars to find end of chunk */
                ret = httpclient_recv(client, data + len, 2 - len, max_recv, &new_trf_len);
                if (ret == HTTPCLIENT_ERROR_CONN || (ret == HTTPCLIENT_CLOSED && new_trf_len == 0)) {
                    return ret;
                }
                len += new_trf_len;
            }
            if ( (data[0] != '\r') || (data[1] != '\n') ) {
                ERR("Format error, %s", data); /* after memmove, the beginning of next chunk */
                return HTTPCLIENT_ERROR_PRTCL;
            }
            memmove(data, &data[2], len - 2); /* remove the \r\n */
            len -= 2;
        } else {
            DBG("no more(content-length)");
            client_data->is_more = false;
            break;
        }

    }
    client_data->content_block_len = count;

    return HTTPCLIENT_OK;
}

int httpclient_response_parse(httpclient_t *client, char *data, int len, httpclient_data_t *client_data)
{
    int crlf_pos;
    int header_buf_len = client_data->header_buf_len;
    char *header_buf = client_data->header_buf;
    int read_result;
    int print_count = 0;

    // reset the header buffer
    if (header_buf) {
        memset(header_buf, 0, header_buf_len);
    }

    client_data->response_content_len = -1;

    char *crlf_ptr = strstr(data, "\r\n");
    if (crlf_ptr == NULL) {
        ERR("\r\n not found");
        return HTTPCLIENT_ERROR_PRTCL;
    }

    crlf_pos = crlf_ptr - data;
    data[crlf_pos] = '\0';

    /* Parse HTTP response */
    if ( sscanf(data, "HTTP/%*d.%*d %d %*[^\r\n]", &(client->response_code)) != 1 ) {
        /* Cannot match string, error */
        ERR("Not a correct HTTP answer : %s", data);
        return HTTPCLIENT_ERROR_PRTCL;
    }

    if ( (client->response_code < 200) || (client->response_code >= 400) ) {
        /* Did not return a 2xx code; TODO fetch headers/(&data?) anyway and implement a mean of writing/reading headers */
        WARN("Response code %d", client->response_code);

        if (client->response_code == 416) {
            ERR("Requested Range Not Satisfiable");
            return HTTPCLIENT_ERROR;
        }
    }

    DBG("Reading headers%s", data);

    memmove(data, &data[crlf_pos + 2], len - (crlf_pos + 2) + 1); /* Be sure to move NULL-terminating char as well */
    len -= (crlf_pos + 2);

    client_data->is_chunked = false;

    reset_g_httpclient_var();
    /* Now get headers */
    while ( true ) {
        char *colon_ptr, *key_ptr, *value_ptr;
        int key_len, value_len;
        read_result = HTTPCLIENT_UNSET_RET;

        crlf_ptr = strstr(data, "\r\n");
        if (crlf_ptr == NULL) {
            if ( len < HTTPCLIENT_CHUNK_SIZE - 1 ) {
                int new_trf_len = 0;
                if (HTTPCLIENT_CHUNK_SIZE - len - 1 <= 0) {
                    ERR("%s %d error max_len %d", __func__, __LINE__, HTTPCLIENT_CHUNK_SIZE - len - 1);
                    return HTTPCLIENT_ERROR;
                }
                read_result = httpclient_recv(client, data + len, 1, HTTPCLIENT_CHUNK_SIZE - len - 1, &new_trf_len);
                len += new_trf_len;
                data[len] = '\0';
                DBG("Read %d chars; In buf: [%s]", new_trf_len, data);
                if (read_result == HTTPCLIENT_ERROR_CONN || (read_result == HTTPCLIENT_CLOSED && new_trf_len == 0)) {
                    return read_result;
                } else {
                    DBG("in loop %s %d ret %d len %d", __func__, __LINE__, read_result, len);
                    continue;
                }
            } else {
                DBG("header len > chunksize");
                return HTTPCLIENT_ERROR;
            }
        }

        crlf_pos = crlf_ptr - data;
        if (crlf_pos == 0) { /* End of headers */
            memmove(data, &data[2], len - 2 + 1); /* Be sure to move NULL-terminating char as well */
            len -= 2;
            break;
        }

        colon_ptr = strstr(data, ": ");
        if (colon_ptr) {
            if (header_buf_len >= crlf_pos + 2 && header_buf) {
                /* copy response header to caller buffer */
                memcpy(header_buf, data, crlf_pos + 2);
                header_buf += crlf_pos + 2;
                header_buf_len -= crlf_pos + 2;
            }

            key_len = colon_ptr - data;
            value_len = crlf_ptr - colon_ptr - strlen(": ");
            key_ptr = data;
            value_ptr = colon_ptr + strlen(": ");

            DBG("Read header : %.*s: %.*s", key_len, key_ptr, value_len, value_ptr);
            if (0 == strncasecmp(key_ptr, "Content-Length", key_len)) {
                sscanf(value_ptr, "%d[^\r]", &(client_data->response_content_len));
                client_data->retrieve_len = client_data->response_content_len;
            } else if (0 == strncasecmp(key_ptr, "Transfer-Encoding", key_len)) {
                if (0 == strncasecmp(value_ptr, "Chunked", value_len)) {
                    client_data->is_chunked = true;
                    client_data->response_content_len = 0;
                    client_data->retrieve_len = 0;
                }
            } else if ((client->response_code >= 300 && client->response_code < 400) && (0 == strncasecmp(key_ptr, "Location", key_len))) {

                if ( HTTPCLIENT_MAX_URL_LEN < value_len + 1 ) {
                    WARN("url is too large (%d >= %d)", value_len + 1, HTTPCLIENT_MAX_URL_LEN);
                    return HTTPCLIENT_ERROR;
                }

                if(client_data->redirect_url == NULL) {
                    client_data->redirect_url = (char* )malloc(HTTPCLIENT_MAX_URL_LEN);
                }

                memset(client_data->redirect_url, 0, HTTPCLIENT_MAX_URL_LEN);
                memcpy(client_data->redirect_url, value_ptr, value_len);
                client_data->is_redirected = 1;
           }

            memmove(data, &data[crlf_pos + 2], len - (crlf_pos + 2) + 1); /* Be sure to move NULL-terminating char as well */
            len -= (crlf_pos + 2);
        } else {
            ERR("Could not parse header");
            return HTTPCLIENT_ERROR;
        }

        if (print_count % MAX_PRINT_PER_COUNT_LINE == 0){
            DBG("in loop %s %d len %d ret %d", __func__, __LINE__, len, read_result);
        }

        g_httpclient_line =  __LINE__;
        g_httpclient_readlen =  -1;
        g_httpclient_len = len;
        g_httpclient_ret = read_result;
        g_httpclient_print_count = print_count;

        print_count++;
    }

    return httpclient_retrieve_content(client, data, len, client_data);
}

HTTPCLIENT_RESULT httpclient_connect(httpclient_t *client, const char *url)
{
    int ret = HTTPCLIENT_ERROR_CONN;
    char scheme[8] = {0};
    char *host = NULL;
    char *path = NULL;

    int url_len = 0;
    int path_size = 0;
    int host_size = 0;

    if (!client || !url) {
        ERR("%s %d invalid para", __func__, __LINE__);
        ret = HTTPCLIENT_ERROR_CONN;
        goto done;
    }

    url_len = strlen(url);
    if (url_len <= 0 || url_len >= HTTPCLIENT_MAX_URL_LEN) {
        ERR("request url len err %d", url_len);
        ret = HTTPCLIENT_ERROR_CONN;
        goto done;
    }

    host = (char *) malloc(HTTPCLIENT_MAX_HOST_LEN);
    if (!host) {
        ERR("%s %d malloc failed", __func__, __LINE__);
        ret = HTTPCLIENT_ERROR_CONN;
        goto done;
    }
    host_size = HTTPCLIENT_MAX_HOST_LEN;
    memset(host, 0, host_size);

    path = (char *) malloc(url_len);
    if (!path) {
        ERR("%s %d malloc failed", __func__, __LINE__);
        ret = HTTPCLIENT_ERROR_CONN;
        goto done;
    }
    path_size = url_len;
    memset(path, 0, path_size);

    /* First we need to parse the url (http[s]://host[:port][/[path]]) */
    ret = httpclient_parse_url(url, scheme, sizeof(scheme), host, host_size, &(client->remote_port), path, path_size);
    if (ret != HTTPCLIENT_OK) {
        ERR("%s %d httpclient_parse_url returned %d", __func__,  __LINE__, ret);
        goto done;
    }

    // http or https
    if (strcmp(scheme, "https") == 0) {
        client->is_http = false;
    }
    else if (strcmp(scheme, "http") == 0)
    {
        client->is_http = true;
    }

    // default http 80 port, https 443 port
    if (client->remote_port == 0) {
        if (client->is_http) {
            client->remote_port = HTTP_PORT;
        } else {
            client->remote_port = HTTPS_PORT;
        }
    }

    DBG("http?:%d, port:%d, host:%s", client->is_http, client->remote_port, host);

    client->socket = -1;
    if (client->is_http)
        ret = httpclient_conn(client, host);
#ifdef HTTPCLIENT_SSL_ENABLE
    else {
        ret = httpclient_ssl_conn(client, host);
        if (0 == ret) {
            httpclient_ssl_t *ssl = (httpclient_ssl_t *)client->ssl;
            client->socket = ssl->net_ctx.fd;
        }
    }
#endif

#ifdef HTTPCLIENT_SOCK_PARAM
    if (ret == 0 && client->socket >= 0) {
        /* set socket recv buf */
        if (client->sock_param.recv_buf_len > 0) {
            if (setsockopt( client->socket, SOL_SOCKET, SO_RCVBUF,
                (char *) &client->sock_param.recv_buf_len , sizeof(client->sock_param.recv_buf_len)) < 0) {
                ERR("http set recv buf %d failed", client->sock_param.recv_buf_len);
            }
        }

        /* set socket send buf */
        if (client->sock_param.send_buf_len > 0) {
            if (setsockopt( client->socket, SOL_SOCKET, SO_SNDBUF,
                (char *) &client->sock_param.send_buf_len , sizeof(client->sock_param.send_buf_len)) < 0) {
                ERR("http set send buf %d failed", client->sock_param.send_buf_len);
            }
        }
    }
#endif

done:
    if (host) {
        free(host);
        host = NULL;
    }

    if (path) {
        free(path);
        path = NULL;
    }

    DBG("httpclient_connect() result:%d, client:%p", ret, client);
    return (HTTPCLIENT_RESULT)ret;
}

HTTPCLIENT_RESULT httpclient_send_request(httpclient_t *client, const char *url, int method, httpclient_data_t *client_data)
{
    int ret = HTTPCLIENT_ERROR_CONN;

    if (!client || !url || !client_data || method < 0 ||
         client->socket < 0) {
        ERR("%s %d Invalid para", __func__, __LINE__);
        return (HTTPCLIENT_RESULT)ret;
    }

    ret = httpclient_send_header(client, url, method, client_data);
    if (ret != 0) {
        return (HTTPCLIENT_RESULT)ret;
    }

    if (method == HTTPCLIENT_POST || method == HTTPCLIENT_PUT) {
        ret = httpclient_send_userdata(client, client_data);
    }

    DBG("httpclient_send_request() result:%d, client:%p", ret, client);
    return (HTTPCLIENT_RESULT)ret;
}

HTTPCLIENT_RESULT httpclient_recv_response(httpclient_t *client, httpclient_data_t *client_data)
{
    int reclen = 0;
    int ret = HTTPCLIENT_ERROR_CONN;
    char *buf = NULL;

    if (!client || !client_data || client->socket < 0) {
        ERR("%s %d Invalid para", __func__, __LINE__);
        return (HTTPCLIENT_RESULT)ret;
    }

    buf = (char *) malloc(HTTPCLIENT_CHUNK_SIZE);
    if (!buf) {
        ERR("%s %d malloc failed", __func__, __LINE__);
        return (HTTPCLIENT_RESULT)ret;
    }

    if (client_data->is_more) {
        client_data->response_buf[0] = '\0';
        ret = httpclient_retrieve_content(client, buf, reclen, client_data);
    } else {
        ret = httpclient_recv(client, buf, 1, HTTPCLIENT_CHUNK_SIZE - 1, &reclen);
        if (ret != HTTPCLIENT_OK && ret != HTTPCLIENT_CLOSED) {
            goto done;
        }

        buf[reclen] = '\0';

        if (reclen) {
            DBG("reclen:%d, buf:%s", reclen, buf);
            ret = httpclient_response_parse(client, buf, reclen, client_data);
        }
    }

done:
    if (buf) {
        free(buf);
        buf = NULL;
    }

    DBG("httpclient_recv_response() result:%d, client:%p", ret, client);
    return (HTTPCLIENT_RESULT)ret;
}

void httpclient_close(httpclient_t *client)
{
    if (client->is_http) {
        if (client->socket >= 0)
            close(client->socket);
    }
#ifdef HTTPCLIENT_SSL_ENABLE
    else
        httpclient_ssl_close(client);
#endif

    client->socket = -1;
    DBG("httpclient_close() client:%p", client);
}

int httpclient_get_response_code(httpclient_t *client)
{
    return client->response_code;
}

int httpclient_get_response_header_value(char *header_buf, char *name, int *val_pos, int *val_len)
{
    char *data = header_buf;
    char *crlf_ptr, *colon_ptr, *key_ptr, *value_ptr;
    int key_len, value_len;

    if (header_buf == NULL || name == NULL || val_pos == NULL  || val_len == NULL )
        return -1;

    while (true) {
        crlf_ptr = strstr(data, "\r\n");
        colon_ptr = strstr(data, ": ");
        if (crlf_ptr && colon_ptr) {
            key_len = colon_ptr - data;
            value_len = crlf_ptr - colon_ptr - strlen(": ");
            key_ptr = data;
            value_ptr = colon_ptr + strlen(": ");

            DBG("Response header: %.*s: %.*s", key_len, key_ptr, value_len, value_ptr);
            if (0 == strncasecmp(key_ptr, name, key_len)) {
                *val_pos = value_ptr - header_buf;
                *val_len = value_len;
                return 0;
            } else {
                data = crlf_ptr + 2;
                continue;
            }
        } else
            return -1;

    }
}

HTTPCLIENT_RESULT httpclient_prepare(httpclient_source_t *source, int header_size, int body_size)
{
    HTTPCLIENT_RESULT ret = HTTPCLIENT_OK;

    source->client_data.header_buf   = (char *) malloc (header_size);
    source->client_data.response_buf = (char *) malloc (body_size);

    if (source->client_data.header_buf == NULL || source->client_data.response_buf == NULL){
        ERR("httpclient_prepare alloc memory failed \n");

        if(source->client_data.header_buf){
            free(source->client_data.header_buf);
            source->client_data.header_buf = NULL;
        }

        if(source->client_data.response_buf){
            free(source->client_data.response_buf);
            source->client_data.response_buf = NULL;
        }
        ret = HTTPCLIENT_ERROR;
        goto finish;
    }

    DBG("httpclient_prepare alloc memory \n");

    source->client_data.header_buf_len = header_size;
    source->client_data.response_buf_len = body_size;
    source->client_data.post_buf_len = 0;

    source->client_data.is_redirected = 0;
    source->client_data.redirect_url = NULL;

finish:
    return ret;
}

void httpclient_reset(httpclient_source_t *source)
{
    char *response_buf = source->client_data.response_buf;
    char *header_buf = source->client_data.header_buf;
    int response_buf_len = source->client_data.response_buf_len;
    int header_buf_len = source->client_data.header_buf_len;
    DBG("httpclient_reset \n");

    memset(&source->client, 0, sizeof (source->client));
    memset(&source->client_data, 0, sizeof (source->client_data));

    source->client_data.response_buf = response_buf;
    source->client_data.header_buf = header_buf;
    source->client_data.response_buf_len = response_buf_len;
    source->client_data.header_buf_len = header_buf_len;
}

HTTPCLIENT_RESULT httpclient_unprepare(httpclient_source_t* source)
{
    HTTPCLIENT_RESULT ret = HTTPCLIENT_OK;

    DBG("httpclient_unprepare relase memory \n");

    if (source == NULL){
        ret = HTTPCLIENT_ERROR;
        goto finish;
    }

    if(source->client_data.header_buf){
        free(source->client_data.header_buf);
        source->client_data.header_buf = NULL;
    }

    if(source->client_data.response_buf){
        free(source->client_data.response_buf);
        source->client_data.response_buf = NULL;
    }

    if(source->client_data.response_buf_dyn){
        free(source->client_data.response_buf_dyn);
        source->client_data.response_buf_dyn = NULL;
    }

    if (source->client_data.redirect_url) {
        free(source->client_data.redirect_url);
        source->client_data.redirect_url = NULL;
    }

    source->client_data.header_buf_len = 0;
    source->client_data.response_buf_len = 0;
    source->client_data.is_redirected = 0;

finish:
    return ret;
}

#ifdef HTTPCLIENT_SSL_ENABLE
#if 1
static int httpclient_ssl_nonblock_recv( void *ctx, unsigned char *buf, size_t len )
{
    int ret;
    int fd = ((mbedtls_net_context *) ctx)->fd;
    if ( fd < 0 ) {
        return ( MBEDTLS_ERR_NET_INVALID_CONTEXT );
    }

    ret = (int) recv( fd, buf, len, MSG_DONTWAIT );

    if ( ret < 0 ) {
#if ( defined(_WIN32) || defined(_WIN32_WCE) ) && !defined(EFIX64) && \
    !defined(EFI32)
        if ( WSAGetLastError() == WSAECONNRESET ) {
            return ( MBEDTLS_ERR_NET_CONN_RESET );
        }
#else
        if ( errno == EPIPE || errno == ECONNRESET ) {
            return ( MBEDTLS_ERR_NET_CONN_RESET );
        }

        if ( errno == EINTR ) {
            return ( MBEDTLS_ERR_SSL_WANT_READ );
        }
#endif
        if (ret == -1 && errno == EWOULDBLOCK) {
            return ret;
        }
        return ( MBEDTLS_ERR_NET_RECV_FAILED );
    }

    return ( ret );
}
#endif

static void httpclient_debug( void *ctx, int level, const char *file, int line, const char *str )
{
    // printf("%s\n", str);
    DBG("%s", str);
}

static int httpclient_ssl_send_all(mbedtls_ssl_context *ssl, const char *data, size_t length)
{
    size_t written_len = 0;

    while (written_len < length) {
        int ret = mbedtls_ssl_write(ssl, (unsigned char *)(data + written_len), (length - written_len));
        if (ret > 0) {
            written_len += ret;
            continue;
        } else if (ret == 0) {
            return written_len;
        } else {
            return -1; /* Connnection error */
        }
    }

    return written_len;
}

static int httpclient_random(void *prng, unsigned char *output, size_t output_len)
{
    uint32_t rnglen = output_len;
    uint8_t rngoffset = 0;

    while (rnglen > 0) {
        *(output + rngoffset) = (uint8_t)rand();
        rngoffset++;
        rnglen--;
    }

    return 0;
}

static int httpclient_ssl_conn(httpclient_t *client, char *host)
{
    int authmode = MBEDTLS_SSL_VERIFY_NONE;
#ifdef MBEDTLS_ENTROPY_C_ALIOS
    const char *pers = "https";
#endif
    int value, ret = 0;
    uint32_t flags;
    char port[10] = {0};
    httpclient_ssl_t *ssl;

    client->ssl = (httpclient_ssl_t *)malloc(sizeof(httpclient_ssl_t));
    if (!client->ssl) {
        ERR("Memory malloc error.");
        ret = -1;
        goto exit;
    }
    ssl = (httpclient_ssl_t *)client->ssl;

    if (client->server_cert)
        authmode = MBEDTLS_SSL_VERIFY_REQUIRED;

    /*
     * Initialize the RNG and the session data
     */
#if defined(MBEDTLS_DEBUG_C)
    mbedtls_debug_set_threshold(DEBUG_LEVEL);
#endif
    mbedtls_net_init(&ssl->net_ctx);
    mbedtls_ssl_init(&ssl->ssl_ctx);
    mbedtls_ssl_config_init(&ssl->ssl_conf);
#ifdef MBEDTLS_X509_CRT_PARSE_C
    mbedtls_x509_crt_init(&ssl->cacert);
    mbedtls_x509_crt_init(&ssl->clicert);
#endif
    mbedtls_pk_init(&ssl->pkey);
#ifdef MBEDTLS_ENTROPY_C_ALIOS
    mbedtls_ctr_drbg_init(&ssl->ctr_drbg);
    mbedtls_entropy_init(&ssl->entropy);
    if ((value = mbedtls_ctr_drbg_seed(&ssl->ctr_drbg,
                               mbedtls_entropy_func,
                               &ssl->entropy,
                               (const unsigned char*)pers,
                               strlen(pers))) != 0) {
        ERR("mbedtls_ctr_drbg_seed() failed, value:-0x%x.", -value);
        ret = -1;
        goto exit;
    }
#endif
    /*
    * Load the Client certificate
    */
    if (client->client_cert && client->client_pk) {
#ifdef MBEDTLS_X509_CRT_PARSE_C
        ret = mbedtls_x509_crt_parse(&ssl->clicert, (const unsigned char *)client->client_cert, client->client_cert_len);
        if (ret < 0) {
            ERR("Loading cli_cert failed! mbedtls_x509_crt_parse returned -0x%x.", -ret);
            ret = -1;
            goto exit;
        }
#endif
        ret = mbedtls_pk_parse_key(&ssl->pkey, (const unsigned char *)client->client_pk, client->client_pk_len, NULL, 0);
        if (ret != 0) {
            ERR("failed! mbedtls_pk_parse_key returned -0x%x.", -ret);
            ret = -1;
            goto exit;
        }
    }

    /*
    * Load the trusted CA
    */
    /* cert_len passed in is gotten from sizeof not strlen */
#ifdef MBEDTLS_X509_CRT_PARSE_C
    if (client->server_cert && ((value = mbedtls_x509_crt_parse(&ssl->cacert,
                                        (const unsigned char *)client->server_cert,
                                        client->server_cert_len)) < 0)) {
        ERR("mbedtls_x509_crt_parse() failed, value:-0x%x.", -value);
        ret = -1;
        goto exit;
    }
#endif

    /*
     * Start the connection
     */
    snprintf(port, sizeof(port), "%d", client->remote_port) ;
    if ((ret = mbedtls_net_connect(&ssl->net_ctx, host, port, MBEDTLS_NET_PROTO_TCP)) != 0) {
        ERR("failed! mbedtls_net_connect returned %d, port:%s.", ret, port);

#if (SOCKET_ALLOC_DEBUG == LWIP_DBG_ON)
    extern void print_sock_alloc_info(void);
    print_sock_alloc_info();
#endif

        ret = -1;
        goto exit;
    }

    /*
     * Setup stuff
     */
    if ((value = mbedtls_ssl_config_defaults(&ssl->ssl_conf,
                                           MBEDTLS_SSL_IS_CLIENT,
                                           MBEDTLS_SSL_TRANSPORT_STREAM,
                                           MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
        ERR("mbedtls_ssl_config_defaults() failed, value:-0x%x.", -value);
        ret = -1;
        goto exit;
    }

    // TODO: add customerization encryption algorithm
#ifdef MBEDTLS_X509_CRT_PARSE_C
    memcpy(&ssl->profile, ssl->ssl_conf.cert_profile, sizeof(mbedtls_x509_crt_profile));
    ssl->profile.allowed_mds = ssl->profile.allowed_mds | MBEDTLS_X509_ID_FLAG(MBEDTLS_MD_MD5);
    mbedtls_ssl_conf_cert_profile(&ssl->ssl_conf, &ssl->profile);

    mbedtls_ssl_conf_authmode(&ssl->ssl_conf, authmode);
    mbedtls_ssl_conf_ca_chain(&ssl->ssl_conf, &ssl->cacert, NULL);

    if (client->client_cert && (ret = mbedtls_ssl_conf_own_cert(&ssl->ssl_conf, &ssl->clicert, &ssl->pkey)) != 0) {
        ERR(" failed! mbedtls_ssl_conf_own_cert returned %d.", ret );
        ret = -1;
        goto exit;
    }
#endif

    mbedtls_ssl_conf_rng(&ssl->ssl_conf, httpclient_random, &ssl->ctr_drbg);
    mbedtls_ssl_conf_dbg(&ssl->ssl_conf, httpclient_debug, NULL);
    mbedtls_ssl_set_hostname(&ssl->ssl_ctx, host);

    if ((value = mbedtls_ssl_setup(&ssl->ssl_ctx, &ssl->ssl_conf)) != 0) {
        ERR("mbedtls_ssl_setup() failed, value:-0x%x.", -value);
        ret = -1;
        goto exit;
    }

    mbedtls_ssl_conf_read_timeout(&ssl->ssl_conf, HTTPCLIENT_MAX_RECV_WAIT_TIME_SEC * 1000);

    mbedtls_ssl_set_bio(&ssl->ssl_ctx, &ssl->net_ctx, mbedtls_net_send, mbedtls_net_recv, mbedtls_net_recv_timeout);

    /*
    * Handshake
    */
    while ((ret = mbedtls_ssl_handshake(&ssl->ssl_ctx)) != 0) {
        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
            ERR("%s mbedtls_ssl_handshake() failed, ret:-0x%x.", host, -ret);
            ret = -1;
            goto exit;
        }
    }

    /*
     * Verify the server certificate
     */
    /* In real life, we would have used MBEDTLS_SSL_VERIFY_REQUIRED so that the
        * handshake would not succeed if the peer's cert is bad.  Even if we used
        * MBEDTLS_SSL_VERIFY_OPTIONAL, we would bail out here if ret != 0 */
    if ((flags = mbedtls_ssl_get_verify_result(&ssl->ssl_ctx)) != 0) {
        char vrfy_buf[512];
        ERR("svr_cert varification failed.");
        mbedtls_x509_crt_verify_info(vrfy_buf, sizeof(vrfy_buf), "  ! ", flags);
        DBG("%s", vrfy_buf);
        ret = -1;
    }
    else {
        INFO("%s svr_cert varification ok.", host);
    }

exit:
    if (ret != 0) {
        ERR("ret=%d.", ret);
        httpclient_ssl_close(client);
    }
    return ret;
}

static int httpclient_ssl_close(httpclient_t *client)
{
    httpclient_ssl_t *ssl = (httpclient_ssl_t *)client->ssl;

    if (!ssl)
        return -1;

    client->ssl = NULL;
    client->client_cert = NULL;
    client->server_cert = NULL;
    client->client_pk = NULL;

    mbedtls_ssl_close_notify(&ssl->ssl_ctx);
    mbedtls_net_free(&ssl->net_ctx);
#ifdef MBEDTLS_X509_CRT_PARSE_C
    mbedtls_x509_crt_free(&ssl->cacert);
    mbedtls_x509_crt_free(&ssl->clicert);
#endif
    mbedtls_pk_free(&ssl->pkey);
    mbedtls_ssl_free(&ssl->ssl_ctx);
    mbedtls_ssl_config_free(&ssl->ssl_conf);
#ifdef MBEDTLS_ENTROPY_C_ALIOS
    mbedtls_ctr_drbg_free(&ssl->ctr_drbg);
    mbedtls_entropy_free(&ssl->entropy);
#endif
    free(ssl);
    return 0;
}

#endif

/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "httpclient.h"
#include "http_wrappers.h"
#include "http_form_data.h"
#include "http_opts.h"

#ifndef strncasecmp
static const unsigned char charmap[] = {
        '\000', '\001', '\002', '\003', '\004', '\005', '\006', '\007',
        '\010', '\011', '\012', '\013', '\014', '\015', '\016', '\017',
        '\020', '\021', '\022', '\023', '\024', '\025', '\026', '\027',
        '\030', '\031', '\032', '\033', '\034', '\035', '\036', '\037',
        '\040', '\041', '\042', '\043', '\044', '\045', '\046', '\047',
        '\050', '\051', '\052', '\053', '\054', '\055', '\056', '\057',
        '\060', '\061', '\062', '\063', '\064', '\065', '\066', '\067',
        '\070', '\071', '\072', '\073', '\074', '\075', '\076', '\077',
        '\100', '\141', '\142', '\143', '\144', '\145', '\146', '\147',
        '\150', '\151', '\152', '\153', '\154', '\155', '\156', '\157',
        '\160', '\161', '\162', '\163', '\164', '\165', '\166', '\167',
        '\170', '\171', '\172', '\133', '\134', '\135', '\136', '\137',
        '\140', '\141', '\142', '\143', '\144', '\145', '\146', '\147',
        '\150', '\151', '\152', '\153', '\154', '\155', '\156', '\157',
        '\160', '\161', '\162', '\163', '\164', '\165', '\166', '\167',
        '\170', '\171', '\172', '\173', '\174', '\175', '\176', '\177',
        '\200', '\201', '\202', '\203', '\204', '\205', '\206', '\207',
        '\210', '\211', '\212', '\213', '\214', '\215', '\216', '\217',
        '\220', '\221', '\222', '\223', '\224', '\225', '\226', '\227',
        '\230', '\231', '\232', '\233', '\234', '\235', '\236', '\237',
        '\240', '\241', '\242', '\243', '\244', '\245', '\246', '\247',
        '\250', '\251', '\252', '\253', '\254', '\255', '\256', '\257',
        '\260', '\261', '\262', '\263', '\264', '\265', '\266', '\267',
        '\270', '\271', '\272', '\273', '\274', '\275', '\276', '\277',
        '\300', '\341', '\342', '\343', '\344', '\345', '\346', '\347',
        '\350', '\351', '\352', '\353', '\354', '\355', '\356', '\357',
        '\360', '\361', '\362', '\363', '\364', '\365', '\366', '\367',
        '\370', '\371', '\372', '\333', '\334', '\335', '\336', '\337',
        '\340', '\341', '\342', '\343', '\344', '\345', '\346', '\347',
        '\350', '\351', '\352', '\353', '\354', '\355', '\356', '\357',
        '\360', '\361', '\362', '\363', '\364', '\365', '\366', '\367',
        '\370', '\371', '\372', '\373', '\374', '\375', '\376', '\377',
};

int
strncasecmp(const char *s1, const char *s2, register size_t n)
{
    register unsigned char u1, u2;
    for (; n != 0; --n) {
        u1 = (unsigned char) *s1++;
        u2 = (unsigned char) *s2++;
        if (charmap[u1] != charmap[u2]) {
            return charmap[u1] - charmap[u2];
        }
        if (u1 == '\0') {
            return 0;
        }
    }
    return 0;
}
#endif

/* base64 encode */
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

/* parse url according to http[s]://host[:port][/[path]] */
static int httpclient_parse_url(const char *url, char *scheme, size_t max_scheme_len, char *host, size_t maxhost_len, int *port, char *path, size_t max_path_len)
{
    char *scheme_ptr = (char *) url;
    char *host_ptr = NULL;
    size_t host_len = 0;
    size_t path_len;
    char *port_ptr;
    char *path_ptr;
    char *fragment_ptr;

    if (url == NULL) {
        http_err("Could not find url");
        return HTTP_EPARSE;
    }

    host_ptr = (char *) strstr(url, "://");

    if (host_ptr == NULL) {
        http_err("Could not find host");
        return HTTP_EPARSE; /* URL is invalid */
    }

    if ( max_scheme_len < host_ptr - scheme_ptr + 1 ) { /* including NULL-terminating char */
        http_err("Scheme str is too small (%d >= %d)", max_scheme_len, host_ptr - scheme_ptr + 1);
        return HTTP_EPARSE;
    }
    memcpy(scheme, scheme_ptr, host_ptr - scheme_ptr);
    scheme[host_ptr - scheme_ptr] = '\0';

    host_ptr += 3;

    port_ptr = strchr(host_ptr, ':');
    if ( port_ptr != NULL ) {
        uint16_t tport;
        host_len = port_ptr - host_ptr;
        port_ptr++;
        if ( sscanf(port_ptr, "%hu", &tport) != 1) {
            http_err("Could not find port");
            return HTTP_EPARSE;
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
        http_err("Host str is too small (%d >= %d)", maxhost_len, host_len + 1);
        return HTTP_EPARSE;
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
        http_err("Path str is too small (%d >= %d)", max_path_len, path_len + 1);
        return HTTP_EPARSE;
    }
    memcpy(path, path_ptr, path_len);
    path[path_len] = '\0';

    return HTTP_SUCCESS;
}

static int httpclient_get_info(httpclient_t *client, char *send_buf, int *send_idx, char *buf, size_t len)   /* 0 on success, err code on failure */
{
    int ret ;
    int cp_len ;
    int idx = *send_idx;

    if (len == 0) {
        len = strlen(buf);
    }

    do {
        if ((HTTPCLIENT_SEND_BUF_SIZE - idx) >= len) {
            cp_len = len ;
        } else {
            cp_len = HTTPCLIENT_SEND_BUF_SIZE - idx ;
        }

        memcpy(send_buf + idx, buf, cp_len) ;
        idx += cp_len ;
        len -= cp_len ;

        if (idx == HTTPCLIENT_SEND_BUF_SIZE) {
            if (client->is_http == false) {
                http_err("send buffer overflow");
                return HTTP_EUNKOWN;
            }
            ret = http_tcp_send_wrapper(client, send_buf, HTTPCLIENT_SEND_BUF_SIZE) ;
            if (ret) {
                return (ret) ;
            }
        }
    } while (len) ;

    *send_idx = idx;
    return HTTP_SUCCESS;
}

void httpclient_set_custom_header(httpclient_t *client, char *header)
{
    client->header = header ;
}

int httpclient_basic_auth(httpclient_t *client, char *user, char *password)
{
    if ((strlen(user) + strlen(password)) >= HTTPCLIENT_AUTHB_SIZE) {
        return HTTP_EUNKOWN;
    }
    client->auth_user = user;
    client->auth_password = password;
    return HTTP_SUCCESS;
}

static int httpclient_send_auth(httpclient_t *client, char *send_buf, int *send_idx)
{
    char b_auth[(int)((HTTPCLIENT_AUTHB_SIZE + 3) * 4 / 3 + 3)] ;
    char base64buff[HTTPCLIENT_AUTHB_SIZE + 3] ;

    httpclient_get_info(client, send_buf, send_idx, "Authorization: Basic ", 0) ;
    snprintf(base64buff, sizeof(base64buff), "%s:%s", client->auth_user, client->auth_password) ;
    http_debug("bAuth: %s", base64buff) ;
    httpclient_base64enc(b_auth, base64buff) ;
    b_auth[strlen(b_auth) + 2] = '\0' ;
    b_auth[strlen(b_auth) + 1] = '\n' ;
    b_auth[strlen(b_auth)] = '\r' ;
    http_debug("b_auth:%s", b_auth) ;
    httpclient_get_info(client, send_buf, send_idx, b_auth, 0) ;
    return HTTP_SUCCESS;
}

static const char *boundary = "----WebKitFormBoundarypNjgoVtFRlzPquKE";
/* send request header */
static int httpclient_send_header(httpclient_t *client, const char *url, int method, httpclient_data_t *client_data)
{
    char scheme[8] = {0};
    char host[HTTPCLIENT_MAX_HOST_LEN] = {0};
    char path[HTTPCLIENT_MAX_URL_LEN] = {0};
    int len, formdata_len;
    int total_len = 0;
    char send_buf[HTTPCLIENT_SEND_BUF_SIZE] = {0};
    char buf[HTTPCLIENT_SEND_BUF_SIZE] = {0};
    char *meth = (method == HTTP_GET) ? "GET" : (method == HTTP_POST) ? "POST" : (method == HTTP_PUT) ? "PUT" : (method == HTTP_DELETE) ? "DELETE" : (method == HTTP_HEAD) ? "HEAD" : "";
    int ret, port;

    /* First we need to parse the url (http[s]://host[:port][/[path]]) */
    int res = httpclient_parse_url(url, scheme, sizeof(scheme), host, sizeof(host), &(port), path, sizeof(path));
    if (res != HTTP_SUCCESS) {
        http_err("httpclient_parse_url returned %d", res);
        return res;
    }

    /* Send request */
    memset(send_buf, 0, HTTPCLIENT_SEND_BUF_SIZE);
    len = 0 ; /* Reset send buffer */

    snprintf(buf, sizeof(buf), "%s %s HTTP/1.1\r\nUser-Agent: AliOS-HTTP-Client/2.1\r\nCache-Control: no-cache\r\nConnection: close\r\nHost: %s\r\n", meth, path, host); /* Write request */
    ret = httpclient_get_info(client, send_buf, &len, buf, strlen(buf));
    if (ret) {
        http_err("Could not write request");
        return HTTP_ECONN;
    }

    /* Send all headers */
    if (client->auth_user) {
        httpclient_send_auth(client, send_buf, &len) ; /* send out Basic Auth header */
    }

    /* Add user header information */
    if (client->header) {
        httpclient_get_info(client, send_buf, &len, (char *)client->header, strlen(client->header));
    }

    if ((formdata_len = httpclient_formdata_len(client_data)) > 0) {
        total_len += formdata_len;

    	memset(buf, 0, sizeof(buf));
   		snprintf(buf, sizeof(buf), "Accept: */*\r\n");
    
   		httpclient_get_info(client, send_buf, &len, buf, strlen(buf));

	    if (client_data->post_content_type != NULL)  {
	        memset(buf, 0, sizeof(buf));
	        snprintf(buf, sizeof(buf), "Content-Type: %s\r\n", client_data->post_content_type);
	        httpclient_get_info(client, send_buf, &len, buf, strlen(buf));
	    } else {
	        memset(buf, 0, sizeof(buf));
	        snprintf(buf, sizeof(buf), "Content-Type: multipart/form-data; boundary=%s\r\n", boundary);
	        httpclient_get_info(client, send_buf, &len, buf, strlen(buf));
	    }

        total_len += strlen(boundary) + 8;
        snprintf(buf, sizeof(buf), "Content-Length: %d\r\n", total_len);
        httpclient_get_info(client, send_buf, &len, buf, strlen(buf));
    } else if ( client_data->post_buf != NULL ) {
        snprintf(buf, sizeof(buf), "Content-Length: %d\r\n", client_data->post_buf_len);
        httpclient_get_info(client, send_buf, &len, buf, strlen(buf));

        if (client_data->post_content_type != NULL)  {
            snprintf(buf, sizeof(buf), "Content-Type: %s\r\n", client_data->post_content_type);
            httpclient_get_info(client, send_buf, &len, buf, strlen(buf));
        }
    } else {
        http_debug("Do nothing");
    }

    /* Close headers */
    httpclient_get_info(client, send_buf, &len, "\r\n", 0);

    http_debug("Trying to write %d bytes http header:%s", len, send_buf);

#if CONFIG_HTTP_SECURE
    if (client->is_http == false) {
        if (http_ssl_send_wrapper(client, send_buf, len) != len) {
            http_err("SSL_write failed");
            return HTTP_EUNKOWN;
        }
        return HTTP_SUCCESS;
    }
#endif

    ret = http_tcp_send_wrapper(client, send_buf, len);
    if (ret > 0) {
        http_debug("Written %d bytes, socket = %d", ret, client->socket);
    } else if ( ret == 0 ) {
        http_err("ret == 0,Connection was closed by server");
        return HTTP_ECLSD; /* Connection was closed by server */
    } else {
        http_err("Connection error (send returned %d)", ret);
        return HTTP_ECONN;
    }

    return HTTP_SUCCESS;
}

static int httpclient_send_userdata(httpclient_t *client, httpclient_data_t *client_data)
{
    int ret = 0;

    if (client_data->post_buf && client_data->post_buf_len) {
        http_debug("client_data->post_buf:%s", client_data->post_buf);
#if CONFIG_HTTP_SECURE
        if (client->is_http == false) {
            if (http_ssl_send_wrapper(client, client_data->post_buf, client_data->post_buf_len) != client_data->post_buf_len) {
                http_err("SSL_write failed");
                return HTTP_EUNKOWN;
            }
        } else
#endif
        {
            ret = http_tcp_send_wrapper(client, client_data->post_buf, client_data->post_buf_len);
            if (ret > 0) {
                http_debug("Written %d bytes", ret);
            } else if ( ret == 0 ) {
                http_debug("ret == 0,Connection was closed by server");
                return HTTP_ECLSD; /* Connection was closed by server */
            } else {
                http_err("Connection error (send returned %d)", ret);
                return HTTP_ECONN;
            }
        }
    } else if(httpclient_send_formdata(client, client_data) < 0) {
    	return HTTP_ECONN;
    }

    return HTTP_SUCCESS;
}

static int httpclient_recv_data(httpclient_t *client, char *buf, int min_len, int max_len, int *p_read_len)
{
	int ret = 0;
	int timeout_ms = 5000;

    if (client->is_http) {
    	ret = http_tcp_recv_wrapper(client, buf, max_len, timeout_ms, p_read_len);
    } 
#if CONFIG_HTTP_SECURE
    else {
    	ret = http_ssl_recv_wrapper(client, buf, max_len, timeout_ms, p_read_len);

    }
#endif

	return ret;
}

static int httpclient_retrieve_content(httpclient_t *client, char *data, int len, httpclient_data_t *client_data)
{
    int count = 0;
    int templen = 0;
    int crlf_pos;
 
    client_data->is_more = true;

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
                return HTTP_EAGAIN;
            }

            max_len = MIN(HTTPCLIENT_CHUNK_SIZE - 1, client_data->response_buf_len - 1 - count);
            if (max_len <= 0) {
                http_err("%s %d error max_len %d", __func__, __LINE__, max_len);
                return HTTP_EUNKOWN;
            }
            ret = httpclient_recv_data(client, data, 1, max_len, &len);

            /* Receive data */
            http_debug("data len: %d %d", len, count);

            if (ret == HTTP_ECONN) {
                http_debug("ret == HTTP_ECONN");
                client_data->content_block_len = count;
                return ret;
            }

            if (len == 0) {/* read no more data */
                http_debug("no more len == 0");
                client_data->is_more = false;
                return HTTP_SUCCESS;
            }
            http_debug("in loop %s %d ret %d len %d", __func__, __LINE__, ret, len);
        }
    }

    while (true) {
        size_t readLen = 0;

        if ( client_data->is_chunked && client_data->retrieve_len <= 0) {
            /* Read chunk header */
            bool foundCrlf;
            int n;
            do {
                int ret;
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
                            http_err("%s %d error max_len %d", __func__, __LINE__, max_recv - len - 1);
                            return HTTP_EUNKOWN;
                        }
                        ret = httpclient_recv_data(client, data + len, 0,  max_recv - len - 1 , &new_trf_len);
                        len += new_trf_len;
                        if ((ret == HTTP_ECONN) || (ret == HTTP_ECLSD && new_trf_len == 0)) {
                            return ret;
                        } else {
                            http_debug("in loop %s %d ret %d len %d", __func__, __LINE__, ret, len);
                            continue;
                        }
                    } else {
                        return HTTP_EUNKOWN;
                    }
                }
                http_debug("in loop %s %d len %d ret %d", __func__, __LINE__, len, ret);
            } while (!foundCrlf);
            data[crlf_pos] = '\0';
            n = sscanf(data, "%x", &readLen);/* chunk length */
            client_data->retrieve_len = readLen;
            client_data->response_content_len += client_data->retrieve_len;
            if (n != 1) {
                http_err("Could not read chunk length");
                return HTTP_EPROTO;
            }

            memmove(data, &data[crlf_pos + 2], len - (crlf_pos + 2)); /* Not need to move NULL-terminating char any more */
            len -= (crlf_pos + 2);

            if ( readLen == 0 ) {
               /* Last chunk */
                client_data->is_more = false;
                http_debug("no more (last chunk)");
                break;
            }
        } else {
            readLen = client_data->retrieve_len;
        }

        http_debug("Retrieving %d bytes, len:%d", readLen, len);

        do {
            int ret;
            http_debug("readLen %d, len:%d", readLen, len);
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
                return HTTP_EAGAIN;
            }

            if ( len >= readLen ) {
                http_debug("memmove %d %d %d", readLen, len, client_data->retrieve_len);
                memmove(data, &data[readLen], len - readLen); /* chunk case, read between two chunks */
                len -= readLen;
                readLen = 0;
                client_data->retrieve_len = 0;
            } else {
                readLen -= len;
            }

            if (readLen) {
                int max_len = MIN(MIN(HTTPCLIENT_CHUNK_SIZE - 1, client_data->response_buf_len - 1 - count), readLen);
                if (max_len <= 0) {
                    http_err("%s %d error max_len %d", __func__, __LINE__, max_len);
                    return HTTP_EUNKOWN;
                }

                ret = httpclient_recv_data(client, data, 1, max_len, &len);
                if (ret == HTTP_ECONN || (ret == HTTP_ECLSD && len == 0)) {
                    return ret;
                }
            }
        } while (readLen);

        if ( client_data->is_chunked ) {
            if (len < 2) {
                int new_trf_len = 0, ret;
                int max_recv = MIN(client_data->response_buf_len - 1 - count + 2, HTTPCLIENT_CHUNK_SIZE - len - 1);
                if (max_recv <= 0) {
                    http_err("%s %d error max_len %d", __func__, __LINE__, max_recv);
                    return HTTP_EUNKOWN;
                }

                /* Read missing chars to find end of chunk */
                ret = httpclient_recv_data(client, data + len, 2 - len, max_recv, &new_trf_len);
                if ((ret == HTTP_ECONN) || (ret == HTTP_ECLSD && new_trf_len == 0)) {
                    return ret;
                }
                len += new_trf_len;
            }
            if ( (data[0] != '\r') || (data[1] != '\n') ) {
                http_err("Format error, %s", data); /* after memmove, the beginning of next chunk */
                return HTTP_EPROTO;
            }
            memmove(data, &data[2], len - 2); /* remove the \r\n */
            len -= 2;
        } else {
            http_err("no more(content-length)");
            client_data->is_more = false;
            break;
        }

    }
    client_data->content_block_len = count;

    return HTTP_SUCCESS;
}

static int httpclient_response_parse(httpclient_t *client, char *data, int len, httpclient_data_t *client_data)
{
    int crlf_pos;
    int header_buf_len = client_data->header_buf_len;
    char *header_buf = client_data->header_buf;
    int read_result;

    // reset the header buffer
    if (header_buf) {
        memset(header_buf, 0, header_buf_len);
    }

    client_data->response_content_len = -1;

    char *crlf_ptr = strstr(data, "\r\n");
    if (crlf_ptr == NULL) {
        http_err("\r\n not found");
        return HTTP_EPROTO;
    }

    crlf_pos = crlf_ptr - data;
    data[crlf_pos] = '\0';

    /* Parse HTTP response */
    if ( sscanf(data, "HTTP/%*d.%*d %d %*[^\r\n]", &(client->response_code)) != 1 ) {
        /* Cannot match string, error */
        http_err("Not a correct HTTP answer : %s", data);
        return HTTP_EPROTO;
    }

    if ( (client->response_code < 200) || (client->response_code >= 400) ) {
        /* Did not return a 2xx code; TODO fetch headers/(&data?) anyway and implement a mean of writing/reading headers */
        http_debug("Response code %d", client->response_code);

        if (client->response_code == 416) {
            http_err("Requested Range Not Satisfiable");
            return HTTP_EUNKOWN;
        }
    }

    memmove(data, &data[crlf_pos + 2], len - (crlf_pos + 2) + 1); /* Be sure to move NULL-terminating char as well */
    len -= (crlf_pos + 2);

    client_data->is_chunked = false;

    /* Now get headers */
    while ( true ) {
        char *colon_ptr, *key_ptr, *value_ptr;
        int key_len, value_len;

        crlf_ptr = strstr(data, "\r\n");
        if (crlf_ptr == NULL) {
            if ( len < HTTPCLIENT_CHUNK_SIZE - 1 ) {
                int new_trf_len = 0;
                if (HTTPCLIENT_CHUNK_SIZE - len - 1 <= 0) {
                    http_err("%s %d error max_len %d", __func__, __LINE__, HTTPCLIENT_CHUNK_SIZE - len - 1);
                    return HTTP_EUNKOWN;
                }
                read_result = httpclient_recv_data(client, data + len, 1, HTTPCLIENT_CHUNK_SIZE - len - 1, &new_trf_len);
                len += new_trf_len;
                data[len] = '\0';
                http_debug("Read %d chars; In buf: [%s]", new_trf_len, data);
                if ((read_result == HTTP_ECONN) || (read_result == HTTP_ECLSD && new_trf_len == 0)) {
                    return read_result;
                } else {
                    http_debug("in loop %s %d ret %d len %d", __func__, __LINE__, read_result, len);
                    continue;
                }
            } else {
                http_err("header len > chunksize");
                return HTTP_EUNKOWN;
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

            http_debug("Read header : %.*s: %.*s", key_len, key_ptr, value_len, value_ptr);
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
                    http_err("url is too large (%d >= %d)", value_len + 1, HTTPCLIENT_MAX_URL_LEN);
                    return HTTP_EUNKOWN;
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
            http_err("Could not parse header");
            return HTTP_EUNKOWN;
        }
    }

    return httpclient_retrieve_content(client, data, len, client_data);
}


HTTPC_RESULT httpclient_conn(httpclient_t *client, const char *url)
{
    int ret = HTTP_ECONN;
    char host[HTTPCLIENT_MAX_HOST_LEN] = {0};
    char scheme[8] = {0};
    char path[HTTPCLIENT_MAX_URL_LEN] = {0};

    /* First we need to parse the url (http[s]://host[:port][/[path]]) */
    int res = httpclient_parse_url(url, scheme, sizeof(scheme), host, sizeof(host), &(client->remote_port), path, sizeof(path));
    if (res != HTTP_SUCCESS) {
        http_err("httpclient_parse_url returned %d", res);
        return (HTTPC_RESULT)res;
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

    client->socket = -1;
    if (client->is_http) {
        ret = http_tcp_conn_wrapper(client, host);
    }
#if CONFIG_HTTP_SECURE
    else {
        ret = http_ssl_conn_wrapper(client, host);
    }
#endif

    http_debug("httpclient_conn() result:%d, client:%p", ret, client);
    return (HTTPC_RESULT)ret;
}

HTTPC_RESULT httpclient_send(httpclient_t *client, const char *url, int method, httpclient_data_t *client_data)
{
    int ret = HTTP_ECONN;

    if (client->socket < 0) {
        return (HTTPC_RESULT)ret;
    }

    ret = httpclient_send_header(client, url, method, client_data);
    if (ret != 0) {
        return (HTTPC_RESULT)ret;
    }

    if (method == HTTP_POST || method == HTTP_PUT) {
        ret = httpclient_send_userdata(client, client_data);
    }

    http_debug("httpclient_send() result:%d, client:%p", ret, client);
    return (HTTPC_RESULT)ret;
}


HTTPC_RESULT httpclient_recv(httpclient_t *client, httpclient_data_t *client_data)
{
    int reclen = 0;
    int ret = HTTP_ECONN;
    // TODO: header format:  name + value must not bigger than HTTPCLIENT_CHUNK_SIZE.
    char buf[HTTPCLIENT_CHUNK_SIZE] = {0};

    if (client->socket < 0) {
        http_err("Invalid socket fd %d!", client->socket);
        return (HTTPC_RESULT)ret;
    }

    if (client_data->is_more) {
        client_data->response_buf[0] = '\0';
        ret = httpclient_retrieve_content(client, buf, reclen, client_data);
    } else {
        ret = httpclient_recv_data(client, buf, 1, HTTPCLIENT_CHUNK_SIZE - 1, &reclen);
        if (ret != HTTP_SUCCESS && ret != HTTP_ECLSD) {
            return (HTTPC_RESULT)ret;
        }

        buf[reclen] = '\0';

        if (reclen) {
            http_debug("reclen:%d, buf:%s", reclen, buf);
            ret = httpclient_response_parse(client, buf, reclen, client_data);
        }
    }

    http_debug("httpclient_recv_data() result:%d, client:%p", ret, client);
    return (HTTPC_RESULT)ret;
}

void httpclient_clse(httpclient_t *client)
{
    if (client->is_http) {
        http_tcp_close_wrapper(client);
    }
#if CONFIG_HTTP_SECURE
    else
        http_ssl_close_wrapper(client);
#endif

    client->socket = -1;
    http_debug("httpclient_clse() client:%p", client);
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

            http_debug("Response header: %.*s: %.*s", key_len, key_ptr, value_len, value_ptr);
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

HTTPC_RESULT httpclient_prepare(httpclient_data_t *client_data, int header_size, int resp_size)
{
    HTTPC_RESULT ret = HTTP_SUCCESS;

    if (!client_data)
    	return HTTP_EUNKOWN;

    memset(client_data, 0, sizeof(httpclient_data_t));

    client_data->header_buf   = (char *) malloc (header_size);
    client_data->response_buf = (char *) malloc (resp_size);

    if (client_data->header_buf == NULL || client_data->response_buf == NULL){
        http_err("httpc_prepare alloc memory failed");
        if(client_data->header_buf){
            free(client_data->header_buf);
            client_data->header_buf = NULL;
        }

        if(client_data->response_buf){
            free(client_data->response_buf);
            client_data->response_buf = NULL;
        }
        ret = HTTP_EUNKOWN;
        goto finish;
    }

    http_debug("httpc_prepare alloc memory");

    client_data->header_buf_len = header_size;
    client_data->response_buf_len = resp_size;
    client_data->post_buf_len = 0;

    client_data->is_redirected = 0;
    client_data->redirect_url = NULL;

finish:
    return ret;
}

void httpclient_reset(httpclient_data_t *client_data)
{
    char *response_buf = client_data->response_buf;
    char *header_buf = client_data->header_buf;
    int response_buf_len = client_data->response_buf_len;
    int header_buf_len = client_data->header_buf_len;
   
    memset(client_data, 0, sizeof(httpclient_data_t));

    client_data->response_buf = response_buf;
    client_data->header_buf = header_buf;
    client_data->response_buf_len = response_buf_len;
    client_data->header_buf_len = header_buf_len;
}

HTTPC_RESULT httpclient_unprepare(httpclient_data_t *client_data)
{
    HTTPC_RESULT ret = HTTP_SUCCESS;

    if (!client_data){
        ret = HTTP_EUNKOWN;
        goto finish;
    }

    if(client_data->header_buf){
        free(client_data->header_buf);
        client_data->header_buf = NULL;
    }

    if(client_data->response_buf){
        free(client_data->response_buf);
        client_data->response_buf = NULL;
    }

    client_data->header_buf_len = 0;
    client_data->response_buf_len = 0;

    client_data->is_redirected = 0;
    if (client_data->redirect_url) {
    	free(client_data->redirect_url);
        client_data->redirect_url = NULL;
    }

finish:
    return ret;
}

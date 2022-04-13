#include <stdio.h>
#include <stdlib.h>

#include "httpclient.h"
#include "aos/cli.h"

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

static HTTPCLIENT_RESULT httpclient_common_dyn(httpclient_t *client, 
                                               char *url, int method, 
                                               httpclient_data_t *client_data)
{
    int total_len = 0, recv_len = 0, recv_total_len = 0;
    char *recv_buffer = NULL;
    HTTPCLIENT_RESULT ret = HTTPCLIENT_ERROR_CONN;

    /* reset httpclient redirect flag */
    client_data->is_redirected = 0;

    ret = httpclient_connect(client, url);
    if(HTTPCLIENT_OK != ret ) {   
        ERR("http connect failed ret %d", ret);
        goto exit;
    }

    ret = httpclient_send_request(client, url, method, client_data);
    if(HTTPCLIENT_OK != ret) {
        ERR("http send request failed");
        goto exit;
    }

    while (0 == total_len || recv_total_len < total_len) {
        ret = httpclient_recv_response(client, client_data);
        if ( HTTPCLIENT_OK == ret || HTTPCLIENT_RETRIEVE_MORE_DATA == ret || 
             HTTPCLIENT_CLOSED == ret) {
            if (0 == total_len) {
                total_len = client_data->response_content_len;

                if ( total_len <= 0 ) {
                    ERR("Invalid length %d", total_len);
                    break;
                }

                recv_buffer = (char *) malloc(total_len + 1);
                if (!recv_buffer) {
                    ERR("malloc response buffer failed");
                    break;
                }
                memset(recv_buffer, 0, total_len + 1);
                if (client_data->response_buf_dyn != NULL) {
                    free(client_data->response_buf_dyn);
                    client_data->response_buf_dyn = NULL;
                }

                client_data->response_buf_dyn = recv_buffer;
                INFO("http total content len %d", total_len);
            }

            recv_len = client_data->content_block_len;
            if (recv_len > 0 && recv_buffer != NULL) {
                int copy_len = 0;
                if (recv_total_len + recv_len <= total_len) {
                    copy_len = recv_len;
                } else {
                    copy_len = total_len - recv_total_len;
                }
                memcpy(recv_buffer + recv_total_len, client_data->response_buf, copy_len);
                recv_total_len += copy_len;
            }

            if (HTTPCLIENT_CLOSED == ret) {
                break;
            }
        } else {
            ERR("http recv failed %d redirect %d", ret, client_data->is_redirected);
            break;
        }
    }

    /* Don't reset form data when got a redirected response */
    if(client_data->is_redirected == 0) {
        httpclient_clear_form_data(client_data);
    }

exit:
    httpclient_close(client);
    return ret;
}

static HTTPCLIENT_RESULT httpclient_common(httpclient_t *client, char *url, int method, httpclient_data_t *client_data)
{
    HTTPCLIENT_RESULT ret = HTTPCLIENT_ERROR_CONN;

    /* reset httpclient redirect flag */
    client_data->is_redirected = 0;

    ret = httpclient_connect(client, url);

    if (!ret) {
        ret = httpclient_send_request(client, url, method, client_data);

        if (!ret) {
            ret = httpclient_recv_response(client, client_data);
        }
    }
    /* Don't reset form data when got a redirected response */
    if(client_data->is_redirected == 0) {
        httpclient_clear_form_data(client_data);
    }

    httpclient_close(client);

    return ret;
}

HTTPCLIENT_RESULT httpclient_get_dyn(httpclient_t *client, char *url, httpclient_data_t *client_data)
{
    int ret = httpclient_common_dyn(client, url, HTTPCLIENT_GET, client_data);

    while((0 == ret) && (1 == client_data->is_redirected)) {
        ret = httpclient_common_dyn(client, client_data->redirect_url, HTTPCLIENT_GET, client_data);
    }

    if(client_data->redirect_url != NULL) {
        free(client_data->redirect_url);
        client_data->redirect_url = NULL;
	}

    return ret;
}

HTTPCLIENT_RESULT httpclient_get(httpclient_t *client, char *url, httpclient_data_t *client_data)
{
    int ret = httpclient_common(client, url, HTTPCLIENT_GET, client_data);

    while((0 == ret) && (1 == client_data->is_redirected)) {
        ret = httpclient_common(client, client_data->redirect_url, HTTPCLIENT_GET, client_data);
    }

    if(client_data->redirect_url != NULL) {
        free(client_data->redirect_url);
        client_data->redirect_url = NULL;
	}

    return ret;
}

HTTPCLIENT_RESULT httpclient_post(httpclient_t *client, char *url, httpclient_data_t *client_data)
{
    int ret = httpclient_common(client, url, HTTPCLIENT_POST, client_data);

    while((0 == ret) && (1 == client_data->is_redirected)) {
        ret = httpclient_common(client, client_data->redirect_url, HTTPCLIENT_POST, client_data);
    }

    if(client_data->redirect_url != NULL) {
        free(client_data->redirect_url);
        client_data->redirect_url = NULL;
    }

    return ret;
}

HTTPCLIENT_RESULT httpclient_post_dyn(httpclient_t *client, char *url, httpclient_data_t *client_data)
{
    int ret = httpclient_common_dyn(client, url, HTTPCLIENT_POST, client_data);

    while((0 == ret) && (1 == client_data->is_redirected)) {
        ret = httpclient_common_dyn(client, client_data->redirect_url, HTTPCLIENT_POST, client_data);
    }

    if(client_data->redirect_url != NULL) {
        free(client_data->redirect_url);
        client_data->redirect_url = NULL;
    }

    return ret;
}


HTTPCLIENT_RESULT httpclient_put(httpclient_t *client, char *url, httpclient_data_t *client_data)
{
    return httpclient_common(client, url, HTTPCLIENT_PUT, client_data);
}

HTTPCLIENT_RESULT httpclient_put_dyn(httpclient_t *client, char *url, httpclient_data_t *client_data)
{
    return httpclient_common_dyn(client, url, HTTPCLIENT_PUT, client_data);
}

HTTPCLIENT_RESULT httpclient_delete(httpclient_t *client, char *url, httpclient_data_t *client_data)
{
    return httpclient_common(client, url, HTTPCLIENT_DELETE, client_data);
}

HTTPCLIENT_RESULT httpclient_delete_dyn(httpclient_t *client, char *url, httpclient_data_t *client_data)
{
    return httpclient_common_dyn(client, url, HTTPCLIENT_DELETE, client_data);
}

#define DEFAULT_HTTP_GET_URL  "https://www.aliyun.com/"
void http_get_test(int argc, char ** argv)
{
    httpclient_source_t http_source;
    int ret;
    char * customer_header = "Accept: */*\r\n";
    char * url  = DEFAULT_HTTP_GET_URL;

    if (argc >= 2) {
        url = argv[1];
    }

    memset(&http_source, 0, sizeof(http_source));
    httpclient_prepare(&http_source, 1024, 1024);

    httpclient_set_custom_header(&http_source.client, customer_header);

    ret = httpclient_get_dyn(&http_source.client, url, &http_source.client_data);
    if( ret >= 0 ) {
        INFO("Data received: %s", http_source.client_data.response_buf_dyn);
    }

    httpclient_unprepare(&http_source);
}

ALIOS_CLI_CMD_REGISTER(http_get_test, hget, HTTP Get Test);

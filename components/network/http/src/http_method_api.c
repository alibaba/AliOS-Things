/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <stdlib.h>

#include "httpclient.h"
#include "http_wrappers.h"
#include "http_form_data.h"

static HTTPC_RESULT httpclient_common(httpclient_t *client, const char *url, int method, httpclient_data_t *client_data)
{
    HTTPC_RESULT ret = HTTP_ECONN;

    /* reset httpclient redirect flag */
    client_data->is_redirected = 0;

    ret = httpclient_conn(client, url);

    if (!ret) {
        ret = httpclient_send(client, url, method, client_data);

        if (!ret) {
            ret = httpclient_recv(client, client_data);
        }
    }
    /* Don't reset form data when got a redirected response */
    if(client_data->is_redirected == 0) {
        httpclient_clear_form_data(client_data);
    }

    httpclient_clse(client);

    return ret;
}

HTTPC_RESULT httpclient_get(httpclient_t *client, const char *url, httpclient_data_t *client_data)
{
    int ret = httpclient_common(client, url, HTTP_GET, client_data);

    while((0 == ret) && (1 == client_data->is_redirected)) {
        ret = httpclient_common(client, client_data->redirect_url, HTTP_GET, client_data);
    }

    if(client_data->redirect_url != NULL) {
        free(client_data->redirect_url);
        client_data->redirect_url = NULL;
	}

    return ret;
}

HTTPC_RESULT httpclient_head(httpclient_t *client, const char *url, httpclient_data_t *client_data)
{
    int ret = httpclient_common(client, url, HTTP_HEAD, client_data);

    while((0 == ret) && (1 == client_data->is_redirected)) {
        ret = httpclient_common(client, client_data->redirect_url, HTTP_HEAD, client_data);
    }

    if(client_data->redirect_url != NULL) {
        free(client_data->redirect_url);
        client_data->redirect_url = NULL;
	}

    return ret;
}

HTTPC_RESULT httpclient_post(httpclient_t *client, const char *url, httpclient_data_t *client_data)
{
    int ret = httpclient_common(client, url, HTTP_POST, client_data);

    while((0 == ret) && (1 == client_data->is_redirected)) {
        ret = httpclient_common(client, client_data->redirect_url, HTTP_POST, client_data);
    }

    if(client_data->redirect_url != NULL) {
        free(client_data->redirect_url);
        client_data->redirect_url = NULL;
    }

    return ret;
}

HTTPC_RESULT httpclient_put(httpclient_t *client, const char *url, httpclient_data_t *client_data)
{
    int ret = httpclient_common(client, url, HTTP_PUT, client_data);

    while((0 == ret) && (1 == client_data->is_redirected)) {
        ret = httpclient_common(client, client_data->redirect_url, HTTP_PUT, client_data);
    }

    if(client_data->redirect_url != NULL) {
        free(client_data->redirect_url);
        client_data->redirect_url = NULL;
    }

    return ret;
}

HTTPC_RESULT httpclient_delete(httpclient_t *client, const char *url, httpclient_data_t *client_data)
{
    return httpclient_common(client, url, HTTP_DELETE, client_data);
}

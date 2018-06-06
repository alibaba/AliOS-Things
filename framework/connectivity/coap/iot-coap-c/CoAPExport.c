/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


#include "CoAPNetwork.h"
#include "CoAPExport.h"

#define COAP_DEFAULT_PORT        5683 /* CoAP default UDP port */
#define COAPS_DEFAULT_PORT       5684 /* CoAP default UDP port for secure transmission */

#define COAP_DEFAULT_SCHEME      "coap" /* the default scheme for CoAP URIs */
#define COAP_DEFAULT_HOST_LEN    128
#define NULL_STR                 "NULL"

unsigned int CoAPUri_parse(char *p_uri, coap_address_t *p_addr,
                           coap_endpoint_type *p_endpoint_type, char host[COAP_DEFAULT_HOST_LEN])
{
    int ret = -1;
    int len = 0;
    char *p = NULL, *q = NULL;
    if (NULL == p_uri || NULL == p_addr || NULL == p_endpoint_type) {
        return COAP_ERROR_INVALID_PARAM;
    }

    len = strlen(p_uri);
    p = p_uri;
    q = (char *)COAP_DEFAULT_SCHEME;
    while (len && *q && tolower(*p) == *q) {
        ++p;
        ++q;
        --len;
    }

    if (*q) {
        return COAP_ERROR_INVALID_URI;
    }
    if (tolower(*p) == 's') {
        ++p;
        --len;
        *p_endpoint_type = COAP_ENDPOINT_DTLS;
        p_addr->port     = COAPS_DEFAULT_PORT;
    } else {
        *p_endpoint_type = COAP_ENDPOINT_NOSEC;
        p_addr->port     = COAP_DEFAULT_PORT;
    }
    COAP_DEBUG("The endpoint type is: %d\r\n", *p_endpoint_type);

    q = (char *)"://";
    while (len && *q && tolower(*p) == *q) {
        ++p;
        ++q;
        --len;
    }

    if (*q) {
        return COAP_ERROR_INVALID_URI;
    }

    q = p;
    while (len && *q != ':') {
        ++q;
        --len;
    }
    if (p == q) {
        return COAP_ERROR_INVALID_URI;
    }

    if (COAP_DEFAULT_HOST_LEN - 1 < (q - p)) {
        return COAP_ERROR_INVALID_URI;
    } else {
        memset(host, 0x00, COAP_DEFAULT_HOST_LEN);
        strncpy(host , p, q - p);
    }
    COAP_DEBUG("The host name is: %s\r\n", host ? host : NULL_STR);
    ret = HAL_UDP_resolveAddress(host, p_addr->addr);
    if (0 != ret) {
        return COAP_ERROR_DNS_FAILED;
    }
    COAP_DEBUG("The address is: %s\r\n", p_addr->addr ? p_addr->addr : NULL_STR);

    if (len && *q == ':') {
        p = ++q;
        --len;

        while (len && isdigit (*q)) {
            ++q;
            --len;
        }

        if (p < q) {
            int uri_port = 0;

            while (p < q) {
                uri_port = uri_port * 10 + (*p++ - '0');
            }

            if (uri_port > 65535) {
                return COAP_ERROR_INVALID_URI;
            }
            p_addr->port = uri_port;
        }
    }
    COAP_DEBUG("The port is: %d\r\n", p_addr->port);

    return COAP_SUCCESS;
}

extern int CoAPMessage_cycle(CoAPContext *context);
static void cb_recv(int fd, void *arg)
{
    CoAPContext *p_ctx = (CoAPContext *)arg;
    if (NULL == p_ctx ) {
        COAP_ERR("Invalid paramter\r\n");
        return ;
    }
    CoAPMessage_cycle(p_ctx);
}

CoAPContext *CoAPContext_create(CoAPInitParam *param)
{
    unsigned int    ret   = COAP_SUCCESS;
    CoAPContext    *p_ctx = NULL;
    coap_network_init_t network_param;
    char host[COAP_DEFAULT_HOST_LEN] = {0};

    memset(&network_param, 0x00, sizeof(coap_network_init_t));
    p_ctx = coap_malloc(sizeof(CoAPContext));
    if (NULL == p_ctx) {
        COAP_ERR("Create coap new context failed\r\n");
        return NULL;
    }

    p_ctx->message_id = 1;
    p_ctx->notifier = NULL;
    p_ctx->sendbuf = coap_malloc(COAP_MSG_MAX_PDU_LEN);
    p_ctx->recvbuf = coap_malloc(COAP_MSG_MAX_PDU_LEN);

    /*CoAP message send list*/
    INIT_LIST_HEAD(&p_ctx->list.sendlist);
    p_ctx->list.count = 0;
    p_ctx->list.maxcount = param->maxcount;

    /*set the endpoint type by uri schema*/
    if (NULL != param->url) {
        ret = CoAPUri_parse(param->url, &network_param.remote, &network_param.ep_type, host);
    }

    if (COAP_SUCCESS != ret) {
        if (NULL != p_ctx) {
            if (NULL != p_ctx->recvbuf) {
                coap_free(p_ctx->recvbuf);
                p_ctx->recvbuf = NULL;
            }
            if (NULL != p_ctx->sendbuf) {
                coap_free(p_ctx->sendbuf);
                p_ctx->sendbuf = NULL;
            }
            coap_free(p_ctx);
            p_ctx    =  NULL;
            return NULL;
        }
    }

#ifdef COAP_DTLS_SUPPORT
    if (COAP_ENDPOINT_DTLS == network_param.ep_type) {
        extern const char *iotx_coap_get_ca(void);
        network_param.p_ca_cert_pem     =  (unsigned char *)iotx_coap_get_ca();
        network_param.ep_type           =   COAP_ENDPOINT_DTLS;
        network_param.p_host            =   host;
    }
#endif

    if (COAP_ENDPOINT_NOSEC == network_param.ep_type) {
        network_param.ep_type = COAP_ENDPOINT_NOSEC;
        network_param.p_ca_cert_pem = NULL;
    }

    /*CoAP network init*/
    ret = CoAPNetwork_init(&network_param,  &p_ctx->network);

    if (COAP_SUCCESS != ret) {
        if (NULL != p_ctx) {

            if (NULL != p_ctx->recvbuf) {
                coap_free(p_ctx->recvbuf);
                p_ctx->recvbuf = NULL;
            }
            if (NULL != p_ctx->sendbuf) {
                coap_free(p_ctx->sendbuf);
                p_ctx->sendbuf = NULL;
            }
            coap_free(p_ctx);
            p_ctx    =  NULL;
        }
    } else {
        aos_poll_read_fd(p_ctx->network.socket_id, cb_recv, p_ctx);
    }


    return p_ctx;
}


void CoAPContext_free(CoAPContext *p_ctx)
{
    CoAPSendNode *cur, *next;

    aos_cancel_poll_read_fd(p_ctx->network.socket_id, cb_recv, p_ctx);
    CoAPNetwork_deinit(&p_ctx->network);

    list_for_each_entry_safe(cur, next, &p_ctx->list.sendlist, CoAPSendNode, sendlist) {
        if (NULL != cur) {
            if (NULL != cur->message) {
                coap_free(cur->message);
                cur->message = NULL;
            }
            coap_free(cur);
            cur = NULL;
        }
    }

    if (NULL != p_ctx->recvbuf) {
        coap_free(p_ctx->recvbuf);
        p_ctx->recvbuf = NULL;
    }

    if (NULL != p_ctx->sendbuf) {
        coap_free(p_ctx->sendbuf);
        p_ctx->sendbuf = NULL;
    }

    coap_free(p_ctx);
}

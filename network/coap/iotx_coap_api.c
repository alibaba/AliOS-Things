/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <sys/stat.h>

#include "coap.h"
#include "iot_export.h"

#include "iotx_coap_internal.h"
#include "iot_export_coap.h"

#include "utils_hmac.h"
#include "lite-cjson.h"
#include "json_parser.h"
#include "iotx_utils.h"

#define IOTX_SIGN_LENGTH         (40+1)
#define IOTX_SIGN_SOURCE_LEN     (256)
#define IOTX_AUTH_TOKEN_LEN      (192+1)
#define IOTX_COAP_INIT_TOKEN     (0x01020304)
#define IOTX_AUTH_DEVICENAME_STR "{\"productKey\":\"%s\",\"deviceName\":\"%s\",\"clientId\":\"%s\",\"sign\":\"%s\"}"
#define IOTX_SIGN_SRC_STR  "clientId%sdeviceName%sproductKey%s"
#define IOTX_COAP_ONLINE_DTLS_SERVER_URL "coaps://%s.coap.cn-shanghai.link.aliyuncs.com:5684"

typedef struct coap_resp_cb_s {
    coap_tid_t id;
    iotx_response_callback_t resp_callback;
    struct coap_resp_cb_s *next;
} coap_resp_cb_t;

typedef struct {
    char                *p_auth_token;
    int                  auth_token_len;
    char                 is_authed;
    iotx_deviceinfo_t   *p_devinfo;
    coap_context_t      *p_coap_ctx;
    unsigned int         coap_token;
    unsigned int         seq;
    unsigned char        key[32];
    iotx_event_handle_t  event_handle;

    char                 uri[128];
    coap_resp_cb_t      *cb_list;
    int                  cb_list_size;
} iotx_coap_t;

typedef struct {
    char          *uri;
    unsigned int   ping_seconds;
    char          *identity;
    uint8_t       *key;
    unsigned int   key_len;
    char          *root_ca_file;
    char          *cert_file;
    char          *token;
} CoapInitParam;

void *g_coap_context;

#define COAP_MAX_WAIT_MSG_NUM 5
static int add_coap_msg_callback(iotx_coap_t *p_context, coap_tid_t id, iotx_response_callback_t cb)
{
    coap_resp_cb_t *p_resp_cb = NULL;
    coap_resp_cb_t *p_next = NULL;

    if (p_context == NULL || id < 0 || NULL == cb) {
        return -1;
    }

    p_resp_cb = (coap_resp_cb_t *)coap_malloc(sizeof(coap_resp_cb_t));
    if (NULL == p_resp_cb) {
        return -1;
    }

    p_resp_cb->id = id;
    p_resp_cb->resp_callback = cb;
    p_resp_cb->next = NULL;

    p_next = p_context->cb_list;
    if (!p_next) {
        p_context->cb_list = p_resp_cb;
    } else {
        while (p_next->next) {
            if (p_next->id == p_resp_cb->id) {
                coap_log(LOG_ERR, "Repeated message id!\n");
                goto err;
            }

            p_next = p_next->next;
        }

        p_next->next = p_resp_cb;
    }

    if (COAP_MAX_WAIT_MSG_NUM == p_context->cb_list_size) {
        p_next = p_context->cb_list;
        if (p_next) {
            coap_log(LOG_EMERG, "remove the oldest msg cb!\n");
            p_context->cb_list = p_next->next;
            coap_free(p_next);
        }
    } else {
        p_context->cb_list_size++;
    }
    
    return 0;
err:
    coap_free(p_resp_cb);
    return -1;
}

static iotx_response_callback_t obtain_coap_msg_callback(iotx_coap_t *p_context, coap_tid_t id)
{
    coap_resp_cb_t *p_next = NULL;
    coap_resp_cb_t *p_prev = NULL;
    iotx_response_callback_t cb = NULL;

    if (p_context == NULL || id < 0) {
        return NULL;
    }

    p_prev = p_next = p_context->cb_list;
    while (p_next) {
        if (p_next->id == id) {
            if (p_prev == p_next) {
                p_context->cb_list = NULL;
            } else {
                p_prev->next = p_next->next;
            }
            
            p_context->cb_list_size--;
            cb = p_next->resp_callback;
            coap_free(p_next);
            return cb;
        }

        p_prev = p_next;
        p_next = p_next->next;
    }

    return NULL;
}

static void free_coap_msg_callback(iotx_coap_t *p_context)
{
    coap_resp_cb_t *p_next = NULL;
    coap_resp_cb_t *p_prev = NULL;

    p_prev = p_next = p_context->cb_list;
    while (p_next) {
        p_prev = p_next;
        p_next = p_next->next;
        coap_free(p_prev);
    }
}

static int check_token(coap_pdu_t *send, coap_pdu_t *received) {
    return  received->token_length == send->token_length &&
            memcmp(received->token, send->token, send->token_length) == 0;
}

static int coap_uri_parse(char *p_uri, coap_uri_t *p_coap_uri)
{
    if (NULL == p_uri || NULL == p_coap_uri) {
    	coap_log(LOG_ERR, "invalid input URI\n");
        return -1;
    }

    /* split p_uri into Uri-* options */
    if (coap_split_uri((unsigned char *)p_uri, strlen(p_uri), p_coap_uri) < 0) {
        coap_log(LOG_ERR, "invalid CoAP URI\n");
        return -1;
    }

    if (p_coap_uri->scheme == COAP_URI_SCHEME_COAPS && !coap_dtls_is_supported()) {
        coap_log(LOG_EMERG,
                 "coaps URI scheme not supported in this version of libcoap\n");
        return -1;
    }

    return 0;
}

int coap_address_resolve(const coap_str_const_t *server, struct sockaddr *dst) {
    struct addrinfo *res, *ainfo;
    struct addrinfo hints;
    static char addrstr[256];
    int error, len=-1;

    memset(addrstr, 0, sizeof(addrstr));
    if (server->length)
        memcpy(addrstr, server->s, server->length);
    else
        memcpy(addrstr, "localhost", 9);

    memset ((char *)&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_family = AF_UNSPEC;

    error = getaddrinfo(addrstr, NULL, &hints, &res);
    if (error != 0) {
        coap_log(LOG_ERR, "getaddrinfo fail: %d\n", error);
        return error;
    }

    for (ainfo = res; ainfo != NULL; ainfo = ainfo->ai_next) {
        switch (ainfo->ai_family) {
            case AF_INET6:
            case AF_INET:
                len = ainfo->ai_addrlen;
                memcpy(dst, ainfo->ai_addr, len);
                goto finish;
            default:
            ;
        }
    }

finish:
    freeaddrinfo(res);
    return len;
}

coap_session_t *get_session(coap_context_t *ctx,
						    const char *local_addr,
						    const char *local_port,
						    coap_proto_t proto,
						    coap_address_t *dst,
						    const char *identity,
						    const uint8_t *key,
						    unsigned key_len,
						    const char *root_ca_file,
						    const char *cert_file) 
{
	  coap_session_t *session = NULL;

    /* If general root CAs are defined */
    if (root_ca_file) {
    	struct stat stbuf;
    	if ((stat(root_ca_file, &stbuf) == 0) && S_ISDIR(stbuf.st_mode)) {
    	  coap_context_set_pki_root_cas(ctx, NULL, root_ca_file);
    	} else {
    	  coap_context_set_pki_root_cas(ctx, root_ca_file, NULL);
    	}
    }

#if LIBCOAP_PSK_SUPPORT
    if (local_addr) {

      	int s;
      	struct addrinfo hints;
      	struct addrinfo *result = NULL, *rp;

      	memset( &hints, 0, sizeof( struct addrinfo ) );
      	hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
      	hints.ai_socktype = COAP_PROTO_RELIABLE(proto) ? SOCK_STREAM : SOCK_DGRAM; /* Coap uses UDP */
      	hints.ai_flags = AI_PASSIVE | AI_NUMERICHOST | AI_NUMERICSERV | AI_ALL;

      	s = getaddrinfo( local_addr, local_port, &hints, &result );
      	if ( s != 0 ) {
      	    coap_log(LOG_ERR, "getaddrinfo fail: %d\n", s);;
      	    return NULL;
      	}

  	    /* iterate through results until success */
  	    for ( rp = result; rp != NULL; rp = rp->ai_next ) {
      			coap_address_t bind_addr;
      			if ( rp->ai_addrlen <= sizeof( bind_addr.addr ) ) {
      			    coap_address_init( &bind_addr );
      			    bind_addr.size = rp->ai_addrlen;
      			    memcpy( &bind_addr.addr, rp->ai_addr, rp->ai_addrlen );
      			    if (cert_file && (proto == COAP_PROTO_DTLS || proto == COAP_PROTO_TLS)) {
      			        coap_dtls_pki_t *dtls_pki = setup_pki();
      			        session = coap_new_client_session_pki(ctx, &bind_addr, dst, proto, dtls_pki);
      			    }
      			    else if ((identity || key) &&
      			             (proto == COAP_PROTO_DTLS || proto == COAP_PROTO_TLS) ) {
      			        session = coap_new_client_session_psk( ctx, &bind_addr, dst, proto,
      			                       identity, key, key_len );
      			    }
      			    else {
      			      session = coap_new_client_session( ctx, &bind_addr, dst, proto );
      			    }
      			    if ( session )
      			      break;
      			}
  	    }
  	    freeaddrinfo( result );

  	} else {
    	if (cert_file && (proto == COAP_PROTO_DTLS || proto == COAP_PROTO_TLS)) {
    		coap_dtls_pki_t *dtls_pki = setup_pki();
    		session = coap_new_client_session_pki(ctx, NULL, dst, proto, dtls_pki);
    	} else if ((identity || key) &&
  		           (proto == COAP_PROTO_DTLS || proto == COAP_PROTO_TLS) ) {
  		    session = coap_new_client_session_psk( ctx, NULL, dst, proto,
  		                                           identity, key, key_len );
  	    } else {
#endif
  		    session = coap_new_client_session( ctx, NULL, dst, proto );
#if LIBCOAP_PSK_SUPPORT
  	    }
  	}
#endif
	return session;
}


static void message_handler(struct coap_context_t *ctx,
                            coap_session_t *session,
                            coap_pdu_t *sent,
                            coap_pdu_t *received,
                            const coap_tid_t id)
{
    iotx_coap_t *p_iotx_coap = NULL;
    iotx_response_callback_t msg_cb = NULL;

#ifndef NDEBUG
    coap_log(LOG_DEBUG, "** process incoming %d.%02d response:\n",
           (received->code >> 5), received->code & 0x1F);
    if (coap_get_log_level() < LOG_DEBUG)
        coap_show_pdu(LOG_INFO, received);
#endif

    p_iotx_coap = (iotx_coap_t *)g_coap_context;
    if (NULL == p_iotx_coap || NULL == sent || NULL == received) {
        coap_log(LOG_ERR, "Invalid iotx context!\n");
        return;
    }
   
    /* check if this is a response to our original request */
    if (!check_token(received, sent)) {
        coap_log(LOG_ERR, "Unmatched token\n");
        return;
    }

    if (received->type == COAP_MESSAGE_RST) {
        coap_log(LOG_INFO, "got RST\n");
         return;
    }

    msg_cb = obtain_coap_msg_callback(p_iotx_coap, sent->tid);
    if (msg_cb) {
        msg_cb((void *)p_iotx_coap, (void *)received);
    } else {
        coap_log(LOG_INFO, "No handler for message\n");
    }
}

coap_context_t *coap_context_create(CoapInitParam *param, char *host)
{
	coap_context_t *ctx = NULL;
    coap_session_t *session = NULL;
    coap_address_t dst;
    coap_proto_t proto;
    coap_uri_t coap_uri;
    int ret;

    if (NULL == param || NULL == param->uri) {
        coap_log(LOG_ERR, "Invalid input param");
        return NULL;
    }

	ret = coap_uri_parse(param->uri, &coap_uri);
	  if (ret != 0) {
		coap_log(LOG_ERR, "URI parser fail");
        return NULL;
	}

    ret = coap_address_resolve(&coap_uri.host, &dst.addr.sa);
    if (ret < 0) {
        coap_log(LOG_ERR, "failed to resolve address");
        return NULL;
    }

    if (host) {
        strncpy(host, (const char *)coap_uri.host.s, coap_uri.host.length);
    }

    dst.size = ret;
    dst.addr.sin.sin_port = htons(coap_uri.port);

    ctx = coap_new_context(NULL);
    if (!ctx) {
        coap_log(LOG_EMERG, "failed to create context\n" );
        goto err;
    }

    coap_context_set_keepalive(ctx, param->ping_seconds);

    proto = (coap_uri.scheme == COAP_URI_SCHEME_COAPS) ?
             COAP_PROTO_DTLS : COAP_PROTO_UDP;

    session = get_session(ctx, NULL, NULL, proto, &dst,
                          param->identity,
                          param->key,
                          param->key_len,
                          param->root_ca_file,
                          param->cert_file);
    if (!session) {
        coap_log(LOG_ERR, "failed to create client session\n");
        goto err;
    }
  
    /* TODO: check ver2 block should be used message handler per ctx or per message */
    coap_register_option(ctx, COAP_OPTION_BLOCK2);
    coap_register_response_handler(ctx, message_handler);

    return ctx;
err:
    coap_session_release(session);
    coap_free_context(ctx);
    coap_cleanup();

    return NULL;
}


static int iotx_get_token_from_json(char *p_str, char *p_token, int len)
{
    char *p_value = NULL;
    if (NULL == p_str || NULL == p_token) {
        coap_log(LOG_ERR,"Invalid paramter p_str %p, p_token %p", p_str, p_token);
        return IOTX_ERR_INVALID_PARAM;
    }

    p_value = LITE_json_value_of("token", p_str, MEM_MAGIC, "coap.cloud");
    if (NULL != p_value) {
        if (len - 1 < strlen(p_value)) {
            return IOTX_ERR_BUFF_TOO_SHORT;
        }
        memset(p_token, 0x00, len);
        strncpy(p_token, p_value, strlen(p_value));
        coap_free(p_value);
        return IOTX_SUCCESS;
    }

    return IOTX_ERR_AUTH_FAILED;
}

static void iotx_device_name_auth_callback(void *user, void *p_message)
{
    int ret_code = IOTX_SUCCESS;
    iotx_coap_t *p_iotx_coap = NULL;
    coap_pdu_t *message = NULL;

    if (NULL == user || NULL == p_message) {
        coap_log(LOG_ERR, "Invalid paramter, p_arg %p message %p", user, p_message);
        return;
    }
    p_iotx_coap = (iotx_coap_t *)user;
    message  = (coap_pdu_t *)p_message;

    coap_log(LOG_DEBUG, "Receive response message:");
    coap_log(LOG_DEBUG, "* Response Code : 0x%x", message->code);
    coap_log(LOG_DEBUG, "* Payload: %s", (char *)message->data);

    switch (message->code) {
        case COAP_MSG_CODE_205_CONTENT: {
#if LIBCOAP_PSK_SUPPORT
            if (COAP_ENDPOINT_PSK == p_iotx_coap->p_coap_ctx->network.ep_type) {
                ret_code = iotx_parse_auth_from_json((char *)message->payload, p_iotx_coap);
            } else {
#endif
                ret_code = iotx_get_token_from_json((char *)message->data, p_iotx_coap->p_auth_token, p_iotx_coap->auth_token_len);
#if LIBCOAP_PSK_SUPPORT
          }
#endif
            if (IOTX_SUCCESS == ret_code) {
                p_iotx_coap->is_authed = IOT_TRUE;
                coap_log(LOG_INFO, "CoAP authenticate success!!!");
            }
            break;
        }
        case COAP_MSG_CODE_500_INTERNAL_SERVER_ERROR: {
            coap_log(LOG_INFO, "CoAP internal server error, authenticate failed, will retry it");
            HAL_SleepMs(1000);
            IOT_CoAP_DeviceNameAuth((iotx_coap_context_t *)p_iotx_coap);
            break;
        }
        default:
            break;
    }

}

static unsigned int iotx_get_coap_token(iotx_coap_t *p_iotx_coap, unsigned char *p_encoded_data)
{
    unsigned int value = p_iotx_coap->coap_token;
    p_encoded_data[0] = (unsigned char)((value & 0x00FF) >> 0);
    p_encoded_data[1] = (unsigned char)((value & 0xFF00) >> 8);
    p_encoded_data[2] = (unsigned char)((value & 0xFF0000) >> 16);
    p_encoded_data[3] = (unsigned char)((value & 0xFF000000) >> 24);
    p_iotx_coap->coap_token++;
    return sizeof(unsigned int);
}


int iotx_calc_sign(const char *p_device_secret, const char *p_client_id,
                   const char *p_device_name, const char *p_product_key, char sign[IOTX_SIGN_LENGTH])
{
    char *p_msg = NULL;

    p_msg = (char *)coap_malloc(IOTX_SIGN_SOURCE_LEN);
    if (NULL == p_msg) {
        return IOTX_ERR_NO_MEM;
    }
    memset(sign,  0x00, IOTX_SIGN_LENGTH);
    memset(p_msg, 0x00, IOTX_SIGN_SOURCE_LEN);

    HAL_Snprintf(p_msg, IOTX_SIGN_SOURCE_LEN,
                 IOTX_SIGN_SRC_STR,
                 p_client_id,
                 p_device_name,
                 p_product_key);
    utils_hmac_md5(p_msg, strlen(p_msg), sign, p_device_secret, strlen(p_device_secret));

    coap_free(p_msg);
    coap_log(LOG_INFO, "The device name sign: %s", sign);
    return IOTX_SUCCESS;
}


#define IOTX_AUTH_STR      "auth"
int IOT_CoAP_DeviceNameAuth(iotx_coap_context_t *p_context)
{
    int len = 0;
    int ret = COAP_SUCCESS;
    coap_context_t   *p_coap_ctx = NULL;
    iotx_coap_t      *p_iotx_coap = NULL;
    coap_pdu_t       *pdu;
    coap_session_t   *p_session  =  NULL;  
    unsigned char    *p_payload  = NULL;
    unsigned char     token[8] = {0};
    char sign[IOTX_SIGN_LENGTH]   = {0};
    unsigned char buf[4] = {0};

    p_iotx_coap = (iotx_coap_t *)p_context;
    if (NULL == p_iotx_coap || (NULL != p_iotx_coap && (NULL == p_iotx_coap->p_auth_token ||
        NULL == p_iotx_coap->p_coap_ctx || 0 == p_iotx_coap->auth_token_len))) {
        coap_log(LOG_ERR, "Invalid paramter");
        return IOTX_ERR_INVALID_PARAM;
    }

    p_coap_ctx = (coap_context_t *)p_iotx_coap->p_coap_ctx;

    // TODO: session list
    p_session = p_coap_ctx->sessions;
    if (NULL == p_session) {
        coap_log(LOG_ERR, "Session not ready");
        return IOTX_ERR_INVALID_PARAM;
    }

    if (!(pdu = coap_new_pdu(p_session))) {
        coap_log(LOG_ERR, "fail to allocate coap pdu");
        return IOTX_ERR_NO_MEM;
    }

    pdu->type = COAP_MESSAGE_CON;
    pdu->tid = coap_new_message_id(p_session);
    pdu->code = COAP_REQUEST_POST;

    len = iotx_get_coap_token(p_iotx_coap, token);
    if (!coap_add_token(pdu, len, token)) {
        coap_log(LOG_ERR, "cannot add token to request\n");
    }

    coap_add_option(pdu, COAP_OPTION_URI_HOST, strlen(p_iotx_coap->uri),
                    (const uint8_t *)p_iotx_coap->uri);
    coap_add_option(pdu, COAP_OPTION_URI_PATH, strlen(IOTX_AUTH_STR),
                    (const uint8_t *)IOTX_AUTH_STR);
    coap_add_option(pdu, COAP_OPTION_CONTENT_FORMAT,
                    coap_encode_var_safe(buf, sizeof(buf),
                                         COAP_CT_APP_JSON),
                    buf);
    coap_add_option(pdu, COAP_OPTION_ACCEPT,
                    coap_encode_var_safe(buf, sizeof(buf),
                                         COAP_CT_APP_JSON),
                    buf);

    if (add_coap_msg_callback(p_context, pdu->tid,
                              iotx_device_name_auth_callback) != 0) {
        coap_log(LOG_ERR, "cannot add msg callback\n");
    }

    p_payload = coap_malloc(COAP_MSG_MAX_PDU_LEN);
    if (NULL == p_payload) {
        coap_log(LOG_ERR, "cannot allocate payload\n");
        coap_delete_pdu(pdu);
        return IOTX_ERR_NO_MEM;
    }
    memset(p_payload, 0x00, COAP_MSG_MAX_PDU_LEN);

#if LIBCOAP_PSK_SUPPORT
    if (COAP_ENDPOINT_PSK == p_iotx_coap->p_coap_ctx->network.ep_type) {
        iotx_calc_sign_with_seq(p_iotx_coap->p_devinfo->device_secret, p_iotx_coap->p_devinfo->device_id,
                                p_iotx_coap->p_devinfo->device_name, p_iotx_coap->p_devinfo->product_key, p_iotx_coap->seq, sign);
        HAL_Snprintf((char *)p_payload, COAP_MSG_MAX_PDU_LEN,
                     IOTX_AUTH_DEVICENAME_STR_WITH_SEQ,
                     p_iotx_coap->p_devinfo->product_key,
                     p_iotx_coap->p_devinfo->device_name,
                     p_iotx_coap->p_devinfo->device_id,
                     sign, p_iotx_coap->seq);

    } else {
#endif
    iotx_calc_sign(p_iotx_coap->p_devinfo->device_secret, p_iotx_coap->p_devinfo->device_id,
                       p_iotx_coap->p_devinfo->device_name, p_iotx_coap->p_devinfo->product_key, sign);
    HAL_Snprintf((char *)p_payload, COAP_MSG_MAX_PDU_LEN,
                     IOTX_AUTH_DEVICENAME_STR,
                     p_iotx_coap->p_devinfo->product_key,
                     p_iotx_coap->p_devinfo->device_name,
                     p_iotx_coap->p_devinfo->device_id,
                     sign);
#if LIBCOAP_PSK_SUPPORT
    }
#endif

    coap_add_data(pdu, strlen((char *)p_payload), p_payload);
    coap_log(LOG_INFO, "The payload is: %s", p_payload);
    coap_log(LOG_INFO, "Send authentication message to server");
    
    ret = coap_send(p_session, pdu);
    int count = 0;
    while (p_session->state != COAP_SESSION_STATE_ESTABLISHED) {
        coap_run_once(p_coap_ctx, 50);
        coap_log(LOG_DEBUG, "coap_run_once %d\n", count);
    }
     coap_run_once(p_coap_ctx, 0);
#if 0
    if (COAP_SUCCESS != ret) {
        coap_log(LOG_DEBUG, "Send authentication message to server failed, ret = %d", ret);
        return IOTX_ERR_SEND_MSG_FAILED;
    }

    ret = Cloud_CoAPMessage_recv(p_coap_ctx, CONFIG_COAP_AUTH_TIMEOUT, 2);
    if (0 < ret && !p_iotx_coap->is_authed) {
        COAP_INFO("CoAP authenticate failed");
        return IOTX_ERR_AUTH_FAILED;
    }

    iotx_set_report_func(coap_report_func);
    /* report module id */
    ret = iotx_report_mid(p_context);
    if (SUCCESS_RETURN != ret) {
        COAP_DEBUG("Send ModuleId message to server(CoAP) failed, ret = %d", ret);
        return IOTX_ERR_SEND_MSG_FAILED;
    }
    /* report device information */
    ret = iotx_report_devinfo(p_context);
    if (SUCCESS_RETURN != ret) {
        COAP_DEBUG("Send devinfo message to server(CoAP) failed, ret = %d", ret);
        return IOTX_ERR_SEND_MSG_FAILED;
    }
    /* report firmware */
    ret = iotx_report_firmware_version(p_context);
    if (SUCCESS_RETURN != ret) {
        COAP_DEBUG("Send firmware message to server(CoAP) failed, ret = %d", ret);
        return IOTX_ERR_SEND_MSG_FAILED;
    }    
#endif

    return IOTX_SUCCESS;
}

iotx_coap_context_t *IOT_CoAP_Init(iotx_coap_config_t *p_config) 
{
    CoapInitParam param;
    char url[128] = {0};
    iotx_coap_t *p_iotx_coap = NULL;

    if (NULL == p_config) {
        coap_log(LOG_ERR, "Invalid paramter p_config %p", p_config);
        return NULL;
    }

    p_iotx_coap = coap_malloc(sizeof(iotx_coap_t));
    if (NULL == p_iotx_coap) {
        coap_log(LOG_ERR, " Allocate memory for iotx_coap_context_t failed");
        return NULL;
    }
    memset(p_iotx_coap, 0x00, sizeof(iotx_coap_t));

    p_iotx_coap->p_auth_token = coap_malloc(IOTX_AUTH_TOKEN_LEN);
    if (NULL == p_iotx_coap->p_auth_token) {
        coap_log(LOG_ERR, " Allocate memory for auth token failed");
        goto err;
    }
    memset(p_iotx_coap->p_auth_token, 0x00, IOTX_AUTH_TOKEN_LEN);

    /*Set the client isn't authed*/
    p_iotx_coap->is_authed = IOT_FALSE;
    p_iotx_coap->auth_token_len = IOTX_AUTH_TOKEN_LEN;

    /*Get deivce information*/
    p_iotx_coap->p_devinfo = coap_malloc(sizeof(iotx_deviceinfo_t));
    if (NULL == p_iotx_coap->p_devinfo) {
        coap_log(LOG_ERR, " Allocate memory for iotx_deviceinfo_t failed");
        goto err;
    }
    memset(p_iotx_coap->p_devinfo, 0x00, sizeof(iotx_deviceinfo_t));

    /*It should be implement by the user*/
    if (NULL != p_config->p_devinfo) {
        memset(p_iotx_coap->p_devinfo, 0x00, sizeof(iotx_deviceinfo_t));
        strncpy(p_iotx_coap->p_devinfo->device_id,    p_config->p_devinfo->device_id,   IOTX_DEVICE_ID_LEN);
        strncpy(p_iotx_coap->p_devinfo->product_key,  p_config->p_devinfo->product_key, IOTX_PRODUCT_KEY_LEN);
        strncpy(p_iotx_coap->p_devinfo->device_secret, p_config->p_devinfo->device_secret, IOTX_DEVICE_SECRET_LEN);
        strncpy(p_iotx_coap->p_devinfo->device_name,  p_config->p_devinfo->device_name, IOTX_DEVICE_NAME_LEN);
    }

    /*Init coap token*/
    p_iotx_coap->coap_token = IOTX_COAP_INIT_TOKEN;
    //p_iotx_coap->seq        = iotx_get_seq();
    memset(p_iotx_coap->key, 0x00, sizeof(p_iotx_coap->key));

    memset(&param, 0x00, sizeof(CoapInitParam));

    if (NULL != p_config->p_url) {
        param.uri = p_config->p_url;
    } else {
        HAL_Snprintf(url, sizeof(url), IOTX_COAP_ONLINE_DTLS_SERVER_URL, p_iotx_coap->p_devinfo->product_key);
        param.uri = url;
        coap_log(LOG_ERR, "Using default CoAP server: %s", url);
    }

    p_iotx_coap->p_coap_ctx = coap_context_create(&param, p_iotx_coap->uri);
    if (NULL == p_iotx_coap->p_coap_ctx) {
        coap_log(LOG_ERR, " Create coap context failed");
        goto err;
    }

    /*Register the event handle to notify the application */
    p_iotx_coap->event_handle = p_config->event_handle;

    g_coap_context = (iotx_coap_context_t *)p_iotx_coap;
    return (iotx_coap_context_t *)p_iotx_coap;
err:
    /* Error, release the memory */
    if (NULL != p_iotx_coap) {
        if (NULL != p_iotx_coap->p_devinfo) {
            coap_free(p_iotx_coap->p_devinfo);
        }
        if (NULL != p_iotx_coap->p_auth_token) {
            coap_free(p_iotx_coap->p_auth_token);
        }
        if (NULL != p_iotx_coap->p_coap_ctx) {
            coap_free_context(p_iotx_coap->p_coap_ctx);
        }

        p_iotx_coap->auth_token_len = 0;
        p_iotx_coap->is_authed = IOT_FALSE;
        coap_free(p_iotx_coap);
    }
    return NULL;   
}

static int iotx_split_path_2_option(char *uri, coap_pdu_t *message)
{
    char *ptr     = NULL;
    char *pstr    = NULL;
    char  path[COAP_MSG_MAX_PATH_LEN]  = {0};

    if (NULL == uri || NULL == message) {
        coap_log(LOG_ERR, "Invalid paramter p_path %p, p_message %p", uri, message);
        return IOTX_ERR_INVALID_PARAM;
    }
    if (IOTX_URI_MAX_LEN < strlen(uri)) {
         coap_log(LOG_ERR, "The uri length is too loog,len = %d", (int)strlen(uri));
        return IOTX_ERR_URI_TOO_LOOG;
    }
    coap_log(LOG_DEBUG, "The uri is %s", uri);
    ptr = pstr = uri;
    while ('\0' != *ptr) {
        if ('/' == *ptr) {
            if (ptr != pstr) {
                memset(path, 0x00, sizeof(path));
                strncpy(path, pstr, ptr - pstr);
                coap_log(LOG_DEBUG, "path: %s,len=%d", path, (int)(ptr - pstr));
                coap_add_option(message, COAP_OPTION_URI_PATH, strlen(path),
                                (const uint8_t *)path);
            }
            pstr = ptr + 1;
        }
        if ('\0' == *(ptr + 1) && '\0' != *pstr) {
            memset(path, 0x00, sizeof(path));
            strncpy(path, pstr, sizeof(path) - 1);
            coap_log(LOG_DEBUG, "path: %s,len=%d", path, (int)strlen(path));
            coap_add_option(message, COAP_OPTION_URI_PATH, strlen(path),
                            (const uint8_t *)path);
        }
        ptr++;
    }
    return IOTX_SUCCESS;
}

int IOT_CoAP_SendMessage(iotx_coap_context_t *p_context, char *p_path, iotx_message_t *p_message)
{
    int len = 0;
    int ret = IOTX_SUCCESS;
    iotx_coap_t *p_iotx_coap = NULL;
    coap_context_t *p_coap_ctx = NULL;
    coap_pdu_t     *pdu;
    unsigned char    token[8] = {0};
    unsigned char    *payload  = NULL;
    coap_session_t   *p_session  =  NULL;
    unsigned char buf[4] = {0};

    p_iotx_coap = (iotx_coap_t *)p_context;
    if (NULL == p_context || NULL == p_path || NULL == p_message ||
        (NULL != p_iotx_coap && NULL == p_iotx_coap->p_coap_ctx)) {
         coap_log(LOG_ERR, "Invalid paramter p_context %p, p_uri %p, p_message %p",
                  p_context, p_path, p_message);
        return IOTX_ERR_INVALID_PARAM;
    }

     // TODO: session list
    p_coap_ctx = p_iotx_coap->p_coap_ctx;
    p_session = p_coap_ctx->sessions;
    if (NULL == p_session) {
        coap_log(LOG_ERR, "Session not ready");
        return IOTX_ERR_INVALID_PARAM;
    }    

#if defined(WITH_FAC_JSON_FLOW)
    coap_log(LOG_INFO, "Upstream Topic: '%s'", p_path);
    coap_log(LOG_INFO, "Upstream Payload:");
    iotx_facility_json_print((const char *)p_message->p_payload, LOG_INFO_LEVEL, '>');
#endif

    /* as this function only support POST request message, type ACK and RST shall be considered error parameters */
    if (p_message->msg_type != IOTX_MESSAGE_CON && p_message->msg_type != IOTX_MESSAGE_NON) {
        return IOTX_ERR_INVALID_PARAM;
    }

    if (p_message->payload_len >= COAP_MSG_MAX_PDU_LEN) {
        coap_log(LOG_ERR, "The payload length %d is too loog", p_message->payload_len);
        return IOTX_ERR_MSG_TOO_LOOG;
    }

    p_coap_ctx = (coap_context_t *)p_iotx_coap->p_coap_ctx;
    if (p_iotx_coap->is_authed) {
        if (!(pdu = coap_new_pdu(p_session))) {
            coap_log(LOG_ERR, "fail to allocate coap pdu");
            return IOTX_ERR_NO_MEM;
        }

        pdu->type = p_message->msg_type;
        pdu->tid = coap_new_message_id(p_session);
        pdu->code = COAP_REQUEST_POST;

        len = iotx_get_coap_token(p_iotx_coap, token);
        if (!coap_add_token(pdu, len, token)) {
            coap_log(LOG_ERR, "cannot add token to request\n");
        }

        iotx_split_path_2_option(p_path, pdu);
        if (IOTX_CONTENT_TYPE_CBOR == p_message->content_type) {
            coap_add_option(pdu, COAP_OPTION_CONTENT_FORMAT,
                            coap_encode_var_safe(buf, sizeof(buf),
                                                 COAP_CT_APP_CBOR),
                            buf);
            coap_add_option(pdu, COAP_OPTION_ACCEPT,
                            coap_encode_var_safe(buf, sizeof(buf),
                                                 COAP_CT_APP_CBOR),
                            buf);
        } else {
            coap_add_option(pdu, COAP_OPTION_CONTENT_FORMAT,
                            coap_encode_var_safe(buf, sizeof(buf),
                                                 COAP_CT_APP_JSON),
                            buf);
            coap_add_option(pdu, COAP_OPTION_ACCEPT,
                            coap_encode_var_safe(buf, sizeof(buf),
                                                 COAP_CT_APP_JSON),
                            buf);
        }

        coap_add_option(pdu, COAP_OPTION_AUTH_TOKEN, strlen(p_iotx_coap->p_auth_token),
                        (unsigned char *)p_iotx_coap->p_auth_token);
      
        if (add_coap_msg_callback(p_context, pdu->tid,
                                  p_message->resp_callback) != 0) {
            coap_log(LOG_ERR, "cannot add msg callback\n");
        }
#ifdef LIBCOAP_PSK_SUPPORT
    if (COAP_ENDPOINT_PSK == p_iotx_coap->p_coap_ctx->network.ep_type) {
            unsigned char buff[32] = {0};
            unsigned char seq[33] = {0};
            HAL_Snprintf((char *)buff, sizeof(buff) - 1, "%d", p_iotx_coap->seq++);
            len = iotx_aes_cbc_encrypt(buff, strlen((char *)buff), p_iotx_coap->key, seq);
            if (0 < len) {
                CoAPStrOption_add(&message,  COAP_OPTION_SEQ, (unsigned char *)seq, len);
            } else {
                COAP_INFO("Encrypt seq failed");
            }
            HEXDUMP_DEBUG(seq, len);

            payload = (unsigned char *)coap_malloc(COAP_MSG_MAX_PDU_LEN);
            if (NULL == payload) {
                return IOTX_ERR_NO_MEM;
            }
            memset(payload, 0x00, COAP_MSG_MAX_PDU_LEN);
            len = iotx_aes_cbc_encrypt(p_message->p_payload, p_message->payload_len, p_iotx_coap->key, payload);
            if (0 == len) {
                coap_free(payload);
                payload = NULL;
                return IOTX_ERR_INVALID_PARAM;
            }

            HEXDUMP_DEBUG(payload, len);
            CoAPMessagePayload_set(&message, payload, len);
    } else {
#endif
        coap_add_data(pdu, p_message->payload_len, p_message->p_payload);
#ifdef LIBCOAP_PSK_SUPPORT
    }
#endif
        ret = coap_send(p_session, pdu);
        if (ret != pdu->tid) {

        }

        if (NULL != payload) {
            coap_free(payload);
            payload = NULL;
        }

        return IOTX_SUCCESS;
    } else {
        /* COAP_INFO("The client hasn't auth success"); */
        return IOTX_ERR_NOT_AUTHED;
    }
}

#define DEFAULT_COAP_YIELD_TIMEOUT_MS 200
int IOT_CoAP_Yield(iotx_coap_context_t *p_context)
{
    iotx_coap_t *p_iotx_coap = NULL;
    p_iotx_coap = (iotx_coap_t *)p_context;
    unsigned int timeout_ms = DEFAULT_COAP_YIELD_TIMEOUT_MS;

    if (NULL == p_iotx_coap || (NULL != p_iotx_coap && NULL == p_iotx_coap->p_coap_ctx)) {
        coap_log(LOG_ERR, "Invalid paramter");
        return IOTX_ERR_INVALID_PARAM;
    }

    return coap_run_once((coap_context_t *)p_iotx_coap->p_coap_ctx, timeout_ms);
}

int IOT_CoAP_GetMessagePayload(void *p_message, unsigned char **pp_payload, int *p_len)
{
    coap_pdu_t  *message = NULL;

    if (NULL == p_message || NULL == pp_payload || NULL == p_len || NULL == g_coap_context) {
        coap_log(LOG_ERR, "Invalid paramter p_message %p, pp_payload %p, p_len %p",
                 p_message, pp_payload, p_len);
        return IOTX_ERR_INVALID_PARAM;
    }
   
    message = (coap_pdu_t *)p_message;

#if LIBCOAP_PSK_SUPPORT
    iotx_coap_t *p_iotx_coap = (iotx_coap_t *)g_coap_context;
    if (COAP_ENDPOINT_PSK == p_iotx_coap->p_coap_ctx->network.ep_type
        && 0 < message->payloadlen) {
        int len = 0;
        unsigned char *payload = NULL;
        payload = coap_malloc(COAP_MSG_MAX_PDU_LEN);
        if (NULL == payload) {
            return IOTX_ERR_NO_MEM;
        }
        memset(payload, 0x00, COAP_MSG_MAX_PDU_LEN);

        len = iotx_aes_cbc_decrypt(message->payload, message->payloadlen, p_iotx_coap->key, payload);
        HEXDUMP_DEBUG(message->payload, message->payloadlen);

        COAP_DEBUG("payload: %s, len %d", payload, len);
        if (len != 0) {
            memcpy(message->payload, payload, len);
            message->payloadlen = len;
        }
        coap_free(payload);

    }
#endif

    if (coap_get_data(message, (size_t *)p_len, pp_payload) != 1) {
        return IOTX_ERR_INVALID_PARAM;
    }

    return IOTX_SUCCESS;
}

int IOT_CoAP_GetMessageCode(void *p_message, iotx_coap_resp_code_t *p_resp_code)
{
    coap_pdu_t  *message = NULL;

    if (NULL == p_message || NULL == p_resp_code) {
        coap_log(LOG_ERR, "Invalid paramter p_message %p, p_resp_code %p",
                 p_message, p_resp_code);
        return IOTX_ERR_INVALID_PARAM;
    }
    message = (coap_pdu_t *)p_message;
    *p_resp_code = (iotx_coap_resp_code_t)message->code;

    return IOTX_SUCCESS;
}

void IOT_CoAP_Deinit(iotx_coap_context_t **pp_context)
{
    iotx_coap_t *p_iotx_coap = NULL;

    if (NULL != pp_context && NULL != *pp_context) {
        p_iotx_coap = (iotx_coap_t *)*pp_context;
        p_iotx_coap->is_authed = IOT_FALSE;
        p_iotx_coap->auth_token_len = 0;
        p_iotx_coap->coap_token = IOTX_COAP_INIT_TOKEN;

        if (NULL != p_iotx_coap->p_auth_token) {
            coap_free(p_iotx_coap->p_auth_token);
            p_iotx_coap->p_auth_token = NULL;
        }

        if (NULL != p_iotx_coap->p_devinfo) {
            coap_free(p_iotx_coap->p_devinfo);
            p_iotx_coap->p_devinfo = NULL;
        }

      if (NULL != p_iotx_coap->cb_list) {
            free_coap_msg_callback(p_iotx_coap);
            p_iotx_coap->cb_list_size = 0;
        }

        if (NULL != p_iotx_coap->p_coap_ctx) {
            coap_free_context(p_iotx_coap->p_coap_ctx);
            coap_cleanup();
            p_iotx_coap->p_coap_ctx = NULL;
        }

        coap_free(p_iotx_coap);

        *pp_context = NULL;
        g_coap_context = NULL;
    }
}

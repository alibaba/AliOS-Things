/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <aos/vfs.h>

#include "coap.h"
#include "coap_api.h"
#include "infra_aes.h"
#include "infra_md5.h"
#ifndef COAP_WITH_NOAUTH
#include "infra_cjson.h"
#include "infra_report.h"
#include "infra_sha256.h"
#include "infra_json_parser.h"
#endif

#define IOTX_SIGN_LENGTH         (40+1)
#define IOTX_SIGN_SOURCE_LEN     (256)
#define IOTX_AUTH_TOKEN_LEN      (192+1)
#define IOTX_COAP_INIT_TOKEN     (0x01020304)
#define IOTX_AUTH_DEVICENAME_STR "{\"productKey\":\"%s\",\"deviceName\":\"%s\",\"clientId\":\"%s\",\"sign\":\"%s\"}"

#define IOTX_SIGN_SRC_STR_WITH_SEQ  "clientId%sdeviceName%sproductKey%sseq%d"
#define IOTX_AUTH_DEVICENAME_STR_WITH_SEQ "{\"productKey\":\"%s\",\"deviceName\":\"%s\",\"clientId\":\"%s\",\"sign\":\"%s\",\"seq\":\"%d\"}"
#define COAP_OPTION_SEQ            2089
#ifndef UNUSED_PARAM
#define UNUSED_PARAM

#endif
#define IOTX_SIGN_SRC_STR  "clientId%sdeviceName%sproductKey%s"
#define IOTX_COAP_ONLINE_DTLS_SERVER_URL "coaps://%s.coap.cn-shanghai.link.aliyuncs.com:5684"

#define min(a,b) ((a) < (b) ? (a) : (b))

#ifndef COAP_MSG_MAX_PDU_LEN
#define COAP_MSG_MAX_PDU_LEN      1280
#endif

#ifndef COAP_MSG_MAX_PATH_LEN
#define COAP_MSG_MAX_PATH_LEN     128
#endif

#ifndef HEXDUMP_DEBUG
#define HEXDUMP_DEBUG(a, b)
#endif

typedef void (*iotx_response_callback_t)(void *p_arg, void *p_message);

typedef struct coap_resp_cb_s {
    coap_tid_t id;
    iotx_response_callback_t resp_callback;
    struct coap_resp_cb_s *next;
} coap_resp_cb_t;

typedef struct {
    char                *p_auth_token;
    char                 is_authed;
    int                  auth_token_len;
    iotx_coap_device_info_t   *p_devinfo;
    coap_context_t      *p_coap_ctx;
    unsigned int         coap_token;
    unsigned int         seq;
    iotx_event_handle_t  event_handle;
    unsigned char        key[32];

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

#ifndef COAP_WITH_NOAUTH
static void iotx_coap_report_rsphdl(void *arg, void *p_response)
{
    int                     p_payload_len = 0;
    unsigned char          *p_payload = NULL;
    char                   *msg = NULL;
    iotx_coap_resp_code_t   resp_code;

    IOT_CoAP_GetMessageCode(p_response, &resp_code);
    IOT_CoAP_GetMessagePayload(p_response, &p_payload, &p_payload_len);
    coap_log(LOG_DEBUG, "Report response: CoAP response code = %d\n", resp_code);
    coap_log(LOG_DEBUG, "Report response: CoAP msg_len = %d\n", p_payload_len);
    if (p_payload_len > 0) {
    coap_log(LOG_DEBUG, "Report response: CoAP payload = %s\n", p_payload);
        msg = json_get_value_by_name((char *)p_payload, p_payload_len, "id", &p_payload_len, 0);
        if (NULL != msg) {
            coap_log(LOG_DEBUG, "Report response: CoAP mid_report responseID = '%s', len = %d\n", msg, p_payload_len);
        } else {
            coap_log(LOG_WARNING, "Report response: CoAP mid_report responseID not found in msg\n");
        }
    } else {
        coap_log(LOG_WARNING, "Report response: CoAP response payload_len = 0\n");
    }
}

static int coap_report_func(void *handle, const char *topic_name, int req_ack, void *data, int len)
{
    iotx_message_t          message;
    char coap_topic[100] = {0};

    memset(&message, 0, sizeof(iotx_message_t));
    message.p_payload = (unsigned char *)data;
    message.payload_len = len;
    message.resp_callback = iotx_coap_report_rsphdl;
    if (req_ack == 0) {
        message.msg_type = IOTX_MESSAGE_NON;
    } else {
        message.msg_type = IOTX_MESSAGE_CON;
    }
    message.content_type = IOTX_CONTENT_TYPE_JSON;

    snprintf(coap_topic, 100, "/topic%s", topic_name);
    coap_log(LOG_ERR, "coap_report_func topic_name=%s\n", topic_name);
    return IOT_CoAP_SendMessage(handle, (char *)coap_topic, &message);
}
#endif

int iotx_aes_cbc_encrypt(const unsigned char *src, int len, const unsigned char *key, void *out)
{
    char *iv = "543yhjy97ae7fyfg";

    int len1 = len & 0xfffffff0;
    int len2 = len1 + 16;
    int pad = len2 - len;
    int ret = 0;

    p_Aes128_t aes_e_h = infra_aes128_init((unsigned char *)key, (unsigned char *)iv, AES_ENCRYPTION);
    if (len1) {
        ret = infra_aes128_cbc_encrypt(aes_e_h, src, len1 >> 4, out);
    }
    if (!ret && pad) {
        char buf[16] = {0};
        memcpy(buf, src + len1, len - len1);
        memset(buf + len - len1, pad, pad);
        ret = infra_aes128_cbc_encrypt(aes_e_h, buf, 1, (unsigned char *)out + len1);

    }

    infra_aes128_destroy(aes_e_h);
    
    coap_log(LOG_INFO, "to encrypt src: %s, len: %d", src, len2);
    return ret == 0 ? len2 : 0;
}

int iotx_aes_cbc_decrypt(const unsigned char *src, int len, const unsigned char *key, void *out)
{
    char *iv = "543yhjy97ae7fyfg";

    p_Aes128_t aes_d_h;
    int ret = 0;
    int n = len >> 4;

    aes_d_h  = infra_aes128_init((uint8_t *)key, (uint8_t *)iv, AES_DECRYPTION);
    if (!aes_d_h) {
        coap_log(LOG_ERR, "fail to decrypt");
        return  0;
    }
    if (n > 1) {
        ret = infra_aes128_cbc_decrypt(aes_d_h, src, n - 1, out);
    }

    if (ret == 0) {
        char *out_c = (char *)out;
        int offset = n > 0 ? ((n - 1) << 4) : 0;
        out_c[offset] = 0;

        if (aes_d_h) {
            ret = infra_aes128_cbc_decrypt(aes_d_h, src + offset, 1, out_c + offset);
        } else {
            coap_log(LOG_ERR, "fail to decrypt remain data");
        }

        if (ret == 0) {
            char pad = out_c[len - 1];
            out_c[len - pad] = 0;
            coap_log(LOG_INFO, "decrypt data:%s, len:%d", out_c, len - pad);
            infra_aes128_destroy(aes_d_h);
            return len - pad;
        }
    }
    infra_aes128_destroy(aes_d_h);

    return 0;
}

#ifndef COAP_WITH_NOAUTH
int iotx_calc_sign_with_seq(const char *p_device_secret, const char *p_client_id,
                            const char *p_device_name, const char *p_product_key, unsigned int seq, char sign[IOTX_SIGN_LENGTH])
{
    char *p_msg = NULL;

    p_msg = (char *)coap_malloc(IOTX_SIGN_SOURCE_LEN);
    if (NULL == p_msg) {
        return IOTX_ERR_NO_MEM;
    }
    memset(sign,  0x00, IOTX_SIGN_LENGTH);
    memset(p_msg, 0x00, IOTX_SIGN_SOURCE_LEN);

    snprintf(p_msg, IOTX_SIGN_SOURCE_LEN,
                 IOTX_SIGN_SRC_STR_WITH_SEQ,
                 p_client_id,
                 p_device_name,
                 p_product_key, seq);
    coap_log(LOG_INFO, "The source string: %s\n", p_msg);
    utils_hmac_md5(p_msg, strlen(p_msg), sign, p_device_secret, strlen(p_device_secret));

    coap_free(p_msg);
    coap_log(LOG_INFO, "The device name sign with seq: %s\n", sign);
    return IOTX_SUCCESS;
}

static int iotx_parse_auth_from_json(char *p_str, iotx_coap_t *p_iotx_coap)
{
    int ret = -1;
    lite_cjson_t root;
    lite_cjson_t node;
    unsigned char key[32] = {0};
    unsigned char buff[128] = {0};
    unsigned char random[32]   = {0};

    if (NULL == p_str || NULL == p_iotx_coap) {
        return IOTX_ERR_INVALID_PARAM;
    }

    memset(&root, 0x00, sizeof(lite_cjson_t));
    memset(&node, 0x00, sizeof(lite_cjson_t));
    ret = lite_cjson_parse(p_str, strlen(p_str), &root);
    if (-1 == ret) {
        return IOTX_ERR_AUTH_FAILED;
    }

    ret = lite_cjson_object_item(&root, "token", strlen("token"), &node);
    if (-1 == ret) {
        return IOTX_ERR_AUTH_FAILED;
    }
    if (p_iotx_coap->auth_token_len - 1 < node.value_length) {
        return IOTX_ERR_BUFF_TOO_SHORT;
    }
    memset(p_iotx_coap->p_auth_token, 0x00, node.value_length);
    strncpy(p_iotx_coap->p_auth_token, node.value, node.value_length);

    memset(&node, 0x00, sizeof(lite_cjson_t));
    ret = lite_cjson_object_item(&root, "seqOffset", strlen("seqOffset"), &node);
    if (-1 == ret) {
        return IOTX_ERR_AUTH_FAILED;
    }
    p_iotx_coap->seq = node.value_int;

    memset(&node, 0x00, sizeof(lite_cjson_t));
    ret = lite_cjson_object_item(&root, "random", strlen("random"), &node);
    if (-1 == ret) {
        return IOTX_ERR_AUTH_FAILED;
    }

    memcpy(random, node.value, node.value_length);
    snprintf((char *)buff, sizeof(buff), "%s,%s",
                 p_iotx_coap->p_devinfo->device_secret,  random);
    utils_sha256(buff,  strlen((char *)buff), key);
    memcpy(p_iotx_coap->key, key + 8, 16);

    HEXDUMP_DEBUG(key, 32);
    coap_log(LOG_INFO, "The short key:\n");
    HEXDUMP_DEBUG(p_iotx_coap->key, 16);

    return IOTX_SUCCESS;
}
#endif

static int
verify_cn_callback(const char *cn,
                   const uint8_t *asn1_public_cert UNUSED_PARAM,
                   size_t asn1_length UNUSED_PARAM,
                   coap_session_t *session UNUSED_PARAM,
                   unsigned depth,
                   int validated UNUSED_PARAM,
                   void *arg UNUSED_PARAM
) {
  coap_log(LOG_INFO, "CN '%s' presented by server (%s)\n",
           cn, depth ? "CA" : "Certificate");
  return 1;
}
static coap_uri_t uri;
static char *cert_file = NULL; /* Combined certificate and private key in PEM */
static char *ca_file = NULL;   /* CA for cert_file - for cert checking in PEM */
static coap_dtls_pki_t *
setup_pki(void) {
  static coap_dtls_pki_t dtls_pki;
  static char client_sni[256];

  memset (&dtls_pki, 0, sizeof(dtls_pki));
  dtls_pki.version = COAP_DTLS_PKI_SETUP_VERSION;
  if (ca_file) {
    /*
     * Add in additional certificate checking.
     * This list of enabled can be tuned for the specific
     * requirements - see 'man coap_encryption'.
     */
    dtls_pki.verify_peer_cert        = 1;
    dtls_pki.require_peer_cert       = 1;
    dtls_pki.allow_self_signed       = 1;
    dtls_pki.allow_expired_certs     = 1;
    dtls_pki.cert_chain_validation   = 1;
    dtls_pki.cert_chain_verify_depth = 2;
    dtls_pki.check_cert_revocation   = 1;
    dtls_pki.allow_no_crl            = 1;
    dtls_pki.allow_expired_crl       = 1;
    dtls_pki.validate_cn_call_back   = verify_cn_callback;
    dtls_pki.cn_call_back_arg        = NULL;
    dtls_pki.validate_sni_call_back  = NULL;
    dtls_pki.sni_call_back_arg       = NULL;
    memset(client_sni, 0, sizeof(client_sni));
    if (uri.host.length)
      memcpy(client_sni, uri.host.s, min(uri.host.length, sizeof(client_sni)));
    else
      memcpy(client_sni, "localhost", 9);
    dtls_pki.client_sni = client_sni;
  }
  dtls_pki.pki_key.key_type = COAP_PKI_KEY_PEM;
  dtls_pki.pki_key.key.pem.public_cert = cert_file;
  dtls_pki.pki_key.key.pem.private_key = cert_file;
  dtls_pki.pki_key.key.pem.ca_file = ca_file;
  return &dtls_pki;
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
        struct aos_stat stbuf;
        if ((aos_stat(root_ca_file, &stbuf) == 0) && S_ISDIR(stbuf.st_mode)) {
          coap_context_set_pki_root_cas(ctx, NULL, root_ca_file);
        } else {
          coap_context_set_pki_root_cas(ctx, root_ca_file, NULL);
        }
    }

    if (local_addr) {

          int s;
          struct addrinfo hints;
          struct addrinfo *result = NULL, *rp;

          memset( &hints, 0, sizeof( struct addrinfo ) );
          hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
          hints.ai_socktype = COAP_PROTO_RELIABLE(proto) ? SOCK_STREAM : SOCK_DGRAM; /* Coap uses UDP */
#ifdef CONFIG_NET_LWIP
          hints.ai_flags = AI_PASSIVE | AI_NUMERICHOST | AI_NUMERICSERV | AI_ALL;
#endif
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
                     (proto == COAP_PROTO_DTLS || proto == COAP_PROTO_TLS)) {
                     session = coap_new_client_session_psk( ctx, &bind_addr, dst, proto,
                               identity, key, key_len );
                }
                else {
                    session = coap_new_client_session( ctx, &bind_addr, dst, proto );
                }
                if ( session ) {
                    break;
                }
         }
     }
     freeaddrinfo( result );
     } else {
        if (cert_file && (proto == COAP_PROTO_DTLS || proto == COAP_PROTO_TLS)) {
            coap_dtls_pki_t *dtls_pki = setup_pki();
            session = coap_new_client_session_pki(ctx, NULL, dst, proto, dtls_pki);
        } else if ((identity || key) &&
                   (proto == COAP_PROTO_DTLS || proto == COAP_PROTO_TLS)) {
                session = coap_new_client_session_psk( ctx, NULL, dst, proto,
                                                       identity, key, key_len );
        } else {
            session = coap_new_client_session( ctx, NULL, dst, proto );
      }
    }
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
        coap_log(LOG_ERR, "Invalid input param\n");
        return NULL;
    }

    ret = coap_uri_parse(param->uri, &coap_uri);
    if (ret != 0) {
        coap_log(LOG_ERR, "URI parser fail\n");
        return NULL;
    }

    ret = coap_address_resolve(&coap_uri.host, &dst.addr.sa);
    if (ret < 0) {
        coap_log(LOG_ERR, "failed to resolve address\n");
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
  
    ctx->scheme = coap_uri.scheme;

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

#ifndef COAP_WITH_NOAUTH
static int iotx_get_token_from_json(char *p_str, char *p_token, int len)
{
    char *p_value = NULL;
    if (NULL == p_str || NULL == p_token) {
        coap_log(LOG_ERR,"Invalid paramter p_str %p, p_token %p", p_str, p_token);
        return IOTX_ERR_INVALID_PARAM;
    }

    p_value = LITE_json_value_of("token", p_str, 0x1234, "coap.cloud");
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
        coap_log(LOG_ERR, "Invalid paramter, p_arg %p message %p\n", user, p_message);
        return;
    }
    p_iotx_coap = (iotx_coap_t *)user;
    message  = (coap_pdu_t *)p_message;

    coap_log(LOG_DEBUG, "Receive response message:\n");
    coap_log(LOG_DEBUG, "* Response Code : 0x%x\n", message->code);
    coap_log(LOG_DEBUG, "* Payload: %s\n", (char *)message->data);

    switch (message->code) {
        case COAP_RESPONSE_CODE(205): {
            if (COAP_URI_SCHEME_COAP_PSK == p_iotx_coap->p_coap_ctx->scheme) {
                ret_code = iotx_parse_auth_from_json((char *)message->data, p_iotx_coap);
            } else {
                ret_code = iotx_get_token_from_json((char *)message->data, p_iotx_coap->p_auth_token, p_iotx_coap->auth_token_len);
          }
            if (IOTX_SUCCESS == ret_code) {
                p_iotx_coap->is_authed = IOT_TRUE;
                coap_log(LOG_INFO, "CoAP authenticate success!!!\n");
            }
            break;
        }
        case COAP_RESPONSE_CODE(500): {
            coap_log(LOG_INFO, "CoAP internal server error, authenticate failed, will retry it\n");
            IOT_CoAP_DeviceNameAuth((iotx_coap_context_t *)p_iotx_coap);
            break;
        }
        default:
            break;
    }

}
#endif

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

    snprintf(p_msg, IOTX_SIGN_SOURCE_LEN,
                 IOTX_SIGN_SRC_STR,
                 p_client_id,
                 p_device_name,
                 p_product_key);
    utils_hmac_md5(p_msg, strlen(p_msg), sign, p_device_secret, strlen(p_device_secret));

    coap_free(p_msg);
    coap_log(LOG_INFO, "The device name sign: %s\n", sign);
    return IOTX_SUCCESS;
}

#ifndef COAP_WITH_NOAUTH
#define IOTX_AUTH_STR      "auth"
int IOT_CoAP_DeviceNameAuth(iotx_coap_context_t *p_context)
{
    int len = 0;
    int ret = 0;
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
        coap_log(LOG_ERR, "Invalid paramter\n");
        return IOTX_ERR_INVALID_PARAM;
    }

    p_coap_ctx = (coap_context_t *)p_iotx_coap->p_coap_ctx;

    // TODO: session list
    p_session = p_coap_ctx->sessions;
    if (NULL == p_session) {
        coap_log(LOG_ERR, "Session not ready\n");
        return IOTX_ERR_INVALID_PARAM;
    }

    if (!(pdu = coap_new_pdu(p_session))) {
        coap_log(LOG_ERR, "fail to allocate coap pdu\n");
        return IOTX_ERR_NO_MEM;
    }

    pdu->type = COAP_MESSAGE_CON;
    pdu->tid = coap_new_message_id(p_session);
    pdu->code = COAP_REQUEST_POST;

    len = iotx_get_coap_token(p_iotx_coap, token);
    if (!coap_add_token(pdu, len, token)) {
        coap_log(LOG_ERR, "cannot add token to request\n");
    }
    coap_add_option(pdu, COAP_OPTION_URI_PATH, strlen(IOTX_AUTH_STR),
                    (const uint8_t *)IOTX_AUTH_STR);
    coap_add_option(pdu, COAP_OPTION_CONTENT_FORMAT,
                    coap_encode_var_safe(buf, sizeof(buf),
                                         COAP_MEDIATYPE_APPLICATION_JSON),
                    buf);
    coap_add_option(pdu, COAP_OPTION_ACCEPT,
                    coap_encode_var_safe(buf, sizeof(buf),
                                         COAP_MEDIATYPE_APPLICATION_JSON),
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

    if (COAP_URI_SCHEME_COAP_PSK == p_iotx_coap->p_coap_ctx->scheme) {
        iotx_calc_sign_with_seq(p_iotx_coap->p_devinfo->device_secret, p_iotx_coap->p_devinfo->device_id,
                                p_iotx_coap->p_devinfo->device_name, p_iotx_coap->p_devinfo->product_key, p_iotx_coap->seq, sign);
        snprintf((char *)p_payload, COAP_MSG_MAX_PDU_LEN,
                     IOTX_AUTH_DEVICENAME_STR_WITH_SEQ,
                     p_iotx_coap->p_devinfo->product_key,
                     p_iotx_coap->p_devinfo->device_name,
                     p_iotx_coap->p_devinfo->device_id,
                     sign, p_iotx_coap->seq);
    } else {
    iotx_calc_sign(p_iotx_coap->p_devinfo->device_secret, p_iotx_coap->p_devinfo->device_id,
                       p_iotx_coap->p_devinfo->device_name, p_iotx_coap->p_devinfo->product_key, sign);
    snprintf((char *)p_payload, COAP_MSG_MAX_PDU_LEN,
                     IOTX_AUTH_DEVICENAME_STR,
                     p_iotx_coap->p_devinfo->product_key,
                     p_iotx_coap->p_devinfo->device_name,
                     p_iotx_coap->p_devinfo->device_id,
                     sign);
    }
    coap_add_data(pdu, strlen((char *)p_payload), p_payload);
    coap_log(LOG_ERR, "The payload is: %s\n", p_payload);
    coap_log(LOG_ERR, "Send authentication message to server\n");
    coap_log(LOG_INFO, "The payload is: %s\n", p_payload);
    coap_log(LOG_INFO, "Send authentication message to server\n");
    if (coap_send(p_session, pdu) == COAP_INVALID_TID) {
       coap_log(LOG_WARNING, "cannot send pdu for transaction %u\n",
                   pdu->tid);
    }
    int count = 0;
    while (p_session->state != COAP_SESSION_STATE_ESTABLISHED) {
        coap_run_once(p_coap_ctx, 50);
        coap_log(LOG_DEBUG, "coap_run_once %d\n", count);
        count ++;
    }
    coap_run_once(p_coap_ctx, 0);

    if (!p_iotx_coap->is_authed) {
        coap_log(LOG_ERR, "CoAP authenticate failed\n");
        return IOTX_ERR_AUTH_FAILED;
    }
    iotx_set_report_func(coap_report_func);
    /* report module id */
    ret = iotx_report_mid(p_context);
    if (SUCCESS_RETURN != ret) {
        coap_log(LOG_ERR, "Send ModuleId message to server(CoAP) failed, ret = %d\n", ret);
        return IOTX_ERR_SEND_MSG_FAILED;
    }
    /* report device information */
    ret = iotx_report_devinfo(p_context);
    if (SUCCESS_RETURN != ret) {
        coap_log(LOG_ERR, "Send devinfo message to server(CoAP) failed, ret = %d\n", ret);
        return IOTX_ERR_SEND_MSG_FAILED;
    }
    /* report firmware */
    ret = iotx_report_firmware_version(p_context);
    if (SUCCESS_RETURN != ret) {
        coap_log(LOG_ERR, "Send firmware message to server(CoAP) failed, ret = %d\n", ret);
        return IOTX_ERR_SEND_MSG_FAILED;
    }    

    return IOTX_SUCCESS;
}
#endif

iotx_coap_context_t *IOT_CoAP_Init(iotx_coap_config_t *p_config) 
{
    CoapInitParam param;
    char url[128] = {0};
    iotx_coap_t *p_iotx_coap = NULL;

    if (NULL == p_config) {
        coap_log(LOG_ERR, "Invalid paramter p_config %p\n", p_config);
        return NULL;
    }

    p_iotx_coap = coap_malloc(sizeof(iotx_coap_t));
    if (NULL == p_iotx_coap) {
        coap_log(LOG_ERR, " Allocate memory for iotx_coap_context_t failed\n");
        return NULL;
    }
    memset(p_iotx_coap, 0x00, sizeof(iotx_coap_t));

    p_iotx_coap->p_auth_token = coap_malloc(IOTX_AUTH_TOKEN_LEN);
    if (NULL == p_iotx_coap->p_auth_token) {
        coap_log(LOG_ERR, " Allocate memory for auth token failed\n");
        goto err;
    }
    memset(p_iotx_coap->p_auth_token, 0x00, IOTX_AUTH_TOKEN_LEN);

    /*Set the client isn't authed*/
    p_iotx_coap->is_authed = IOT_FALSE;
    p_iotx_coap->auth_token_len = IOTX_AUTH_TOKEN_LEN;

    /*Get deivce information*/
    p_iotx_coap->p_devinfo = coap_malloc(sizeof(iotx_coap_device_info_t));
    if (NULL == p_iotx_coap->p_devinfo) {
        coap_log(LOG_ERR, " Allocate memory for iotx_coap_device_info_t failed\n");
        goto err;
    }
    memset(p_iotx_coap->p_devinfo, 0x00, sizeof(iotx_coap_device_info_t));

    /*It should be implement by the user*/
    if (NULL != p_config->p_devinfo) {
        memset(p_iotx_coap->p_devinfo, 0x00, sizeof(iotx_coap_device_info_t));
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
        snprintf(url, sizeof(url), IOTX_COAP_ONLINE_DTLS_SERVER_URL, p_iotx_coap->p_devinfo->product_key);
        param.uri = url;
        coap_log(LOG_ERR, "Using default CoAP server: %s\n", url);
    }

    p_iotx_coap->p_coap_ctx = coap_context_create(&param, p_iotx_coap->uri);
    if (NULL == p_iotx_coap->p_coap_ctx) {
        coap_log(LOG_ERR, " Create coap context failed\n");
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
        coap_log(LOG_ERR, "Invalid paramter p_path %p, p_message %p\n", uri, message);
        return IOTX_ERR_INVALID_PARAM;
    }
    if (IOTX_URI_MAX_LEN < strlen(uri)) {
         coap_log(LOG_ERR, "The uri length is too long,len = %d\n", (int)strlen(uri));
        return IOTX_ERR_URI_TOO_LOOG;
    }
    coap_log(LOG_DEBUG, "The uri is %s\n", uri);
    ptr = pstr = uri;
    while ('\0' != *ptr) {
        if ('/' == *ptr) {
            if (ptr != pstr) {
                memset(path, 0x00, sizeof(path));
                strncpy(path, pstr, ptr - pstr);
                coap_log(LOG_DEBUG, "path: %s,len=%d\n", path, (int)(ptr - pstr));
                coap_add_option(message, COAP_OPTION_URI_PATH, strlen(path),
                                (const uint8_t *)path);
            }
            pstr = ptr + 1;
        }
        if ('\0' == *(ptr + 1) && '\0' != *pstr) {
            memset(path, 0x00, sizeof(path));
            strncpy(path, pstr, sizeof(path) - 1);
            coap_log(LOG_DEBUG, "path: %s,len=%d\n", path, (int)strlen(path));
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
         coap_log(LOG_ERR, "Invalid paramter p_context %p, p_uri %p, p_message %p\n",
                  p_context, p_path, p_message);
        return IOTX_ERR_INVALID_PARAM;
    }

     // TODO: session list
    p_coap_ctx = p_iotx_coap->p_coap_ctx;
    p_session = p_coap_ctx->sessions;
    if (NULL == p_session) {
        coap_log(LOG_ERR, "Session not ready\n");
        return IOTX_ERR_INVALID_PARAM;
    }    

#if defined(WITH_FAC_JSON_FLOW)
    coap_log(LOG_INFO, "Upstream Topic: '%s'\n", p_path);
    coap_log(LOG_INFO, "Upstream Payload:\n");
    iotx_facility_json_print((const char *)p_message->p_payload, LOG_INFO_LEVEL, '>');
#endif

    /* as this function only support POST request message, type ACK and RST shall be considered error parameters */
    if (p_message->msg_type != IOTX_MESSAGE_CON && p_message->msg_type != IOTX_MESSAGE_NON) {
        return IOTX_ERR_INVALID_PARAM;
    }

    if (p_message->payload_len >= COAP_MSG_MAX_PDU_LEN) {
        coap_log(LOG_ERR, "The payload length %d is too long\n", p_message->payload_len);
        return IOTX_ERR_MSG_TOO_LOOG;
    }

    p_coap_ctx = (coap_context_t *)p_iotx_coap->p_coap_ctx;
    if (p_iotx_coap->is_authed) {
        if (!(pdu = coap_new_pdu(p_session))) {
            coap_log(LOG_ERR, "fail to allocate coap pdu\n");
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
                                                 COAP_MEDIATYPE_APPLICATION_CBOR),
                            buf);
            coap_add_option(pdu, COAP_OPTION_ACCEPT,
                            coap_encode_var_safe(buf, sizeof(buf),
                                                 COAP_MEDIATYPE_APPLICATION_CBOR),
                            buf);
        } else {
            coap_add_option(pdu, COAP_OPTION_CONTENT_FORMAT,
                            coap_encode_var_safe(buf, sizeof(buf),
                                                 COAP_MEDIATYPE_APPLICATION_JSON),
                            buf);
            coap_add_option(pdu, COAP_OPTION_ACCEPT,
                            coap_encode_var_safe(buf, sizeof(buf),
                                                 COAP_MEDIATYPE_APPLICATION_JSON),
                            buf);
        }

        coap_add_option(pdu, COAP_OPTION_AUTH_TOKEN, strlen(p_iotx_coap->p_auth_token),
                        (unsigned char *)p_iotx_coap->p_auth_token);
      
        if (add_coap_msg_callback(p_context, pdu->tid,
                                  p_message->resp_callback) != 0) {
            coap_log(LOG_ERR, "cannot add msg callback\n");
        }

        if (COAP_URI_SCHEME_COAP_PSK == p_iotx_coap->p_coap_ctx->scheme) {
            unsigned char buff[32] = {0};
            unsigned char seq[33] = {0};
            snprintf((char *)buff, sizeof(buff) - 1, "%d", p_iotx_coap->seq++);
            len = iotx_aes_cbc_encrypt(buff, strlen((char *)buff), p_iotx_coap->key, seq);
            if (0 < len) {
                coap_add_option(pdu,  COAP_OPTION_SEQ, len, (unsigned char *)seq);
            } else {
                coap_log(LOG_ERR, "Encrypt seq failed\n");
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
            coap_add_data(pdu, len, payload);
        } else {
            coap_add_data(pdu, p_message->payload_len, p_message->p_payload);
        }

        if (coap_send(p_session, pdu) == COAP_INVALID_TID) {
           coap_log(LOG_WARNING, "cannot send pdu for transaction %u\n",
                   pdu->tid);
        }

        if (NULL != payload) {
            coap_free(payload);
            payload = NULL;
        }

        return IOTX_SUCCESS;
    } else {
        if (!(pdu = coap_new_pdu(p_session))) {
            coap_log(LOG_ERR, "fail to allocate coap pdu\n");
            return IOTX_ERR_NO_MEM;
        }
        /* COAP_INFO("The client hasn't auth success"); */
        pdu->type = p_message->msg_type;
        pdu->tid = coap_new_message_id(p_session);
        pdu->code = COAP_REQUEST_POST;
        coap_add_data(pdu, p_message->payload_len, p_message->p_payload);

        if (coap_send(p_session, pdu) == COAP_INVALID_TID) {
           coap_log(LOG_WARNING, "cannot send pdu for transaction %u\n",
                   pdu->tid);
        }
        return IOTX_SUCCESS;
    }
}

#define DEFAULT_COAP_YIELD_TIMEOUT_MS 200
int IOT_CoAP_Yield(iotx_coap_context_t *p_context)
{
    iotx_coap_t *p_iotx_coap = NULL;
    p_iotx_coap = (iotx_coap_t *)p_context;
    unsigned int timeout_ms = DEFAULT_COAP_YIELD_TIMEOUT_MS;

    if (NULL == p_iotx_coap || (NULL != p_iotx_coap && NULL == p_iotx_coap->p_coap_ctx)) {
        coap_log(LOG_ERR, "Invalid paramter\n");
        return IOTX_ERR_INVALID_PARAM;
    }

    return coap_run_once((coap_context_t *)p_iotx_coap->p_coap_ctx, timeout_ms);
}

int IOT_CoAP_GetMessagePayload(void *p_message, unsigned char **pp_payload, int *p_len)
{
    coap_pdu_t  *message = NULL;
    if (NULL == p_message || NULL == pp_payload || NULL == p_len || NULL == g_coap_context) {
        coap_log(LOG_ERR, "Invalid paramter p_message %p, pp_payload %p, p_len %p\n",
                 p_message, pp_payload, p_len);
        return IOTX_ERR_INVALID_PARAM;
    }
   
    message = (coap_pdu_t *)p_message;

    if (coap_get_data(message, (size_t *)p_len, pp_payload) != 1) {
        return IOTX_ERR_INVALID_PARAM;
    }

    iotx_coap_t *p_iotx_coap = (iotx_coap_t *)g_coap_context;
    if (COAP_URI_SCHEME_COAP_PSK == p_iotx_coap->p_coap_ctx->scheme
        && 0 < *p_len) {
        int len = 0;
        unsigned char *payload = NULL;
        payload = coap_malloc(COAP_MSG_MAX_PDU_LEN);
        if (NULL == payload) {
            return IOTX_ERR_NO_MEM;
        }
        memset(payload, 0x00, COAP_MSG_MAX_PDU_LEN);

        len = iotx_aes_cbc_decrypt(*pp_payload, *p_len, p_iotx_coap->key, payload);
        HEXDUMP_DEBUG(pp_payload, *p_len);

        coap_log(LOG_DEBUG, "payload: %s, len %d\n", payload, len);
        if (len != 0) {
            coap_add_data(message, len, payload);
        }
        coap_free(payload);

        if (coap_get_data(message, (size_t *)p_len, pp_payload) != 1) {
            return IOTX_ERR_INVALID_PARAM;
        }

    }

    return IOTX_SUCCESS;
}

int IOT_CoAP_GetMessageCode(void *p_message, iotx_coap_resp_code_t *p_resp_code)
{
    coap_pdu_t  *message = NULL;

    if (NULL == p_message || NULL == p_resp_code) {
        coap_log(LOG_ERR, "Invalid paramter p_message %p, p_resp_code %p\n",
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

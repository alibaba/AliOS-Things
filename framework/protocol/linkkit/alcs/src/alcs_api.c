#include <time.h>
#include "alcs_api.h"
#include "alcs_coap.h"
#include "digest/utils_hmac.h"
#include "iot_import_awss.h"
#include "json_parser.h"
#include "alcs_api_internal.h"
#include "CoAPPlatform.h"
#include "CoAPObserve.h"

struct list_head secure_resource_cb_head;

static bool is_inited = 0;
#ifdef SUPPORT_MULTI_DEVICES
struct list_head device_list;

device_auth_list* get_device(CoAPContext *context)
{
    device_auth_list *node = NULL, *next = NULL;
    list_for_each_entry_safe(node, next, &device_list, lst, device_auth_list) {
        if(node->context == context ){
            return node;
        }
    }
    return NULL;
}

auth_list* get_list(CoAPContext *context)
{
    device_auth_list* dev_lst = get_device (context);
    return dev_lst? &dev_lst->lst_auth : NULL;
}

#ifdef ALCSCLIENT
struct list_head* get_ctl_session_list (CoAPContext *context)
{
    device_auth_list* dev_lst = get_device (context);
    if (!dev_lst || !(dev_lst->role & ROLE_CLIENT)) {
        return NULL;
    }
    return &dev_lst->lst_ctl_sessions;
}
#endif
#ifdef ALCSSERVER
struct list_head* get_svr_session_list (CoAPContext *context)
{
    device_auth_list* dev_lst = get_device (context);
    return dev_lst && (dev_lst->role & ROLE_SERVER)? &dev_lst->lst_svr_sessions : NULL;
}
#endif

#else
device_auth_list _device;
#endif

void remove_session (CoAPContext *ctx, session_item* session)
{
    COAP_INFO("remove_session");
    if (session) {
        CoapObsServerAll_delete (ctx, &session->addr);
        list_del (&session->lst);
        coap_free (session);
    }
}

#ifdef ALCSCLIENT
session_item* get_ctl_session (CoAPContext *ctx, NetworkAddr* from)
{
    session_item* node = NULL, *next = NULL;
    struct list_head* sessions = get_ctl_session_list(ctx);
    if (sessions) {
        COAP_DEBUG("get_ctl_session");
        list_for_each_entry_safe(node, next, sessions, lst, session_item) {
            if (is_networkadd_same(from, &node->addr)) {
                return node;
            }
        }
    }
    return NULL;
}

#endif

#ifdef ALCSSERVER
session_item* get_svr_session (CoAPContext *ctx, NetworkAddr* from)
{
    session_item* node = NULL, *next = NULL;
    struct list_head* sessions = get_svr_session_list(ctx);
    if (sessions) {
        list_for_each_entry_safe(node, next, sessions, lst, session_item) {
            if (is_networkadd_same(from, &node->addr)) {
                COAP_DEBUG("find node, sessionid:%d", node->sessionId);
                return node;
            }
        }
    }

    return NULL;
}
#endif

void gen_random_key(unsigned char random[], int len)
{
    int i = 0, flag = 0;

    memset(random, 0x00, len);
    srand((unsigned)time(NULL));

    for(i=0; i<len - 1; i++){
        flag = rand() % 3;
        switch (flag){
            case 0:
                random[i] = 'A' + rand() % 26;
                break;
            case 1:
                random[i] = 'a' + rand() % 26;                break;
            case 2:
                random[i] = '0' + rand() % 10;
                break;
            default:
                random[i] = 'x';
                break;
        }
    }
}

void alcs_rec_auth_info (CoAPContext *ctx, const char *paths, NetworkAddr* from, CoAPMessage* resMsg)
{
    int seqlen, paramlen;
    char* seq, *params;
    COAP_INFO ("alcs_rec_auth_info:%d", resMsg->payloadlen);

    do {
        if (!req_payload_parser((const char *)resMsg->payload, resMsg->payloadlen, &seq, &seqlen, &params, &paramlen)) {
            break;
        }
        char* data;
        int datalen;
        data = json_get_value_by_name(params, paramlen, "data", &datalen, NULL);
        if (!data || !datalen) {
            break;
        }
        int typelen;
        char* datatype = json_get_value_by_name(params, paramlen, "datatype", &typelen, NULL);
        if (!datatype || !typelen) {
            break;
        }
        int oplen;
        char* op = json_get_value_by_name(params, paramlen, "op", &oplen, NULL);
        if (!op || !oplen) {
            break;
        }
        alcs_auth_info (ctx, data, datalen, datatype, op);

    } while (0);

    CoAPMessage msg;
    char payloadbuf[256];
    snprintf (payloadbuf, sizeof(payloadbuf), "{\"id\":\"%.*s\",\"code\":%d}", seqlen, seq, 200);
    CoAPLenString payload = {strlen(payloadbuf), (unsigned char *)payloadbuf};

    alcs_msg_init (ctx, &msg, COAP_MSG_CODE_205_CONTENT, COAP_MESSAGE_TYPE_ACK, 0, &payload, NULL);
    CoAPLenString token = {resMsg->header.tokenlen,resMsg->token};
    alcs_sendrsp (ctx, from, &msg, 1, resMsg->header.msgid, &token);
}


#ifdef ALCSSERVER
extern void alcs_rec_auth_select (CoAPContext *context, const char *paths, NetworkAddr *remote, CoAPMessage *request);
extern void alcs_rec_auth (CoAPContext *context, const char *paths, NetworkAddr *remote, CoAPMessage *request);
extern void alcs_rec_heart_beat(CoAPContext *context, const char *paths, NetworkAddr *remote, CoAPMessage *request);
#endif

int alcs_auth_init(CoAPContext *ctx, const char* productKey, const char* deviceName, char role)
{
    if (is_inited) {
        return 0;
    }
    is_inited = 1;

    device_auth_list* dev;
    //auth_list* lst_auth;

#ifdef SUPPORT_MULTI_DEVICES
    INIT_LIST_HEAD(&device_list);

    dev = coap_malloc(sizeof(device_auth_list));
    list_add_tail(&dev->lst, &device_list);
#else
    dev = &_device;
#endif
    dev->context = ctx;
    dev->seq = 1;
    dev->role = role;
    memset (&dev->lst_auth, 0, sizeof(auth_list));
    //strcpy (dev->deviceName, deviceName);
    //strcpy (dev->productKey, productKey);

    INIT_LIST_HEAD(&dev->lst);
    INIT_LIST_HEAD(&secure_resource_cb_head);

    if (role & ROLE_SERVER) {
#ifdef ALCSSERVER
        INIT_LIST_HEAD(&dev->lst_svr_sessions);
        INIT_LIST_HEAD(&dev->lst_auth.lst_svr);

        char path[256];
        snprintf(path, sizeof(path), "/dev/%s/%s/core/service/auth", productKey, deviceName);
        alcs_resource_register (ctx, path, COAP_PERM_GET, COAP_CT_APP_JSON, 60, 0, alcs_rec_auth);
        strcat (path, "/select");
        alcs_resource_register (ctx, path, COAP_PERM_GET, COAP_CT_APP_JSON, 60, 0, alcs_rec_auth_select);
        alcs_resource_register (ctx, "/dev/core/service/heartBeat", COAP_PERM_GET, COAP_CT_APP_JSON, 60, 0, alcs_rec_heart_beat);
        alcs_resource_register (ctx, "/dev/core/service/auth_info", COAP_PERM_GET, COAP_CT_APP_JSON, 60, 1, alcs_rec_auth_info);
#endif
    }

    if (role & ROLE_CLIENT) {
#ifdef ALCSCLIENT
        INIT_LIST_HEAD(&dev->lst_ctl_sessions);
        INIT_LIST_HEAD(&dev->lst_auth.lst_ctl);
#endif
    }

    dev->lst_auth.list_mutex = HAL_MutexCreate();

    return COAP_SUCCESS;
}

void alcs_auth_deinit(void)
{
	alcs_resource_cb_deinit();
}

bool is_networkadd_same (NetworkAddr* addr1, NetworkAddr* addr2)
{
    if (!addr1 || !addr2) {
        return 0;
    }
    COAP_DEBUG("compare addr1:%s,addr2:%s", addr1->addr, addr2->addr);
    return addr1->port == addr2->port && !strcmp((const char *)addr1->addr, (const char *)addr2->addr);
}

static session_item* get_auth_info (CoAPContext *ctx, NetworkAddr* from)
{
#ifdef ALCSCLIENT
    session_item* node = get_ctl_session (ctx, from);
    if (node && node->sessionId) {
        return node;
    }
#endif
#ifdef ALCSSERVER
    session_item* node1 = get_svr_session (ctx, from);
    if (node1 && node1->sessionId) {
        return node1;
    }
#endif

    return NULL;
}

int alcs_encrypt (const char* src, int len, const char* key, void* out)
{
    char* iv = "a1b1c1d1e1f1g1h1";

    int len1 = len & 0xfffffff0;
    int len2 = len1 + 16;
    int pad = len2 - len;
    int ret = 0;

    if (len1) {
        p_HAL_Aes128_t aes_e_h = HAL_Aes128_Init ((uint8_t*)key, (uint8_t*)iv, HAL_AES_ENCRYPTION);
        if (aes_e_h != NULL) {
            ret = HAL_Aes128_Cbc_Encrypt(aes_e_h, src, len1 >> 4, out);
            HAL_Aes128_Destroy (aes_e_h);
            if (ret != 0) {
                COAP_ERR ("fail to encrypt");
                return 0;
            }
        } else {
            ret = -1;
            COAP_ERR ("fail to init aes128 encrypt");
        }
    }
    if (!ret && pad) {
        char buf[16];
        memcpy (buf, src + len1, len - len1);
        memset (buf + len - len1, pad, pad);
        p_HAL_Aes128_t aes_e_h = HAL_Aes128_Init ((uint8_t*)key, (uint8_t*)iv, HAL_AES_ENCRYPTION);
        if (aes_e_h != NULL) {
            ret = HAL_Aes128_Cbc_Encrypt(aes_e_h, buf, 1, out + len1);
            HAL_Aes128_Destroy (aes_e_h);
            if (ret != 0) {
                COAP_ERR ("fail to encrypt");
                return 0;
            }
        } else {
            ret = -1;
            COAP_ERR ("fail to init aes128 encrypt"); 
        }
    }

    COAP_DEBUG ("to encrypt src:%s, len:%d", src, len2);
    return ret == 0? len2 : 0;
}

int alcs_decrypt (const char* src, int len, const char* key, void* out)
{
    COAP_DEBUG ("to decrypt len:%d", len);
    char* iv = "a1b1c1d1e1f1g1h1";

    p_HAL_Aes128_t aes_d_h;
    int ret = 0;
    int n = len >> 4;
    if (n > 0) {
        aes_d_h  = HAL_Aes128_Init ((uint8_t*)key, (uint8_t*)iv, HAL_AES_DECRYPTION);
        if (aes_d_h != NULL) {
            ret = HAL_Aes128_Cbc_Decrypt(aes_d_h, src, n - 1, out);
            HAL_Aes128_Destroy(aes_d_h);
            if (ret != 0) {
                COAP_ERR ("fail to decrypt");
                return 0;
            }
        } else {
            COAP_ERR ("fail to init aes128");
            return -1;
        }
    }


    char* out_c = (char*)out;
    int offset = n > 0? ((n - 1) << 4) : 0;
    out_c[offset] = 0;

    aes_d_h  = HAL_Aes128_Init ((uint8_t*)key, (uint8_t*)iv, HAL_AES_DECRYPTION);
    if (aes_d_h != NULL) {
        int ret = HAL_Aes128_Cbc_Decrypt(aes_d_h, src + offset, 1, out_c + offset);
        HAL_Aes128_Destroy(aes_d_h);
        if (ret != 0) {
            COAP_ERR ("fail to decrypt");
            return 0;
        }
    } else {
        COAP_ERR ("fail to init aes128");
        return -1;
    }

    char pad = out_c[len - 1];
    out_c[len - pad] = 0;
    COAP_DEBUG ("decrypt data:%s, len:%d", out_c, len - pad);
    return ret == 0? len - pad : 0;
}

bool alcs_is_auth (CoAPContext *ctx, NetworkAddr* from)
{
    return get_auth_info(ctx, from) != NULL;
}

/*---------------------------------------------------------*/
typedef struct
{
    void* orig_user_data;
    bool  keep;
    CoAPSendMsgHandler orig_handler;
} secure_send_item;

int do_secure_send (CoAPContext *ctx, NetworkAddr* addr, CoAPMessage *message, const char* key, char* buf)
{
    int ret = COAP_SUCCESS;
    COAP_DEBUG("do_secure_send");

    CoAPMessage sendMsg;
    memcpy (&sendMsg, message, sizeof(CoAPMessage));

    sendMsg.payload = (unsigned char *)buf;
    sendMsg.payloadlen = alcs_encrypt ((const char *)message->payload, message->payloadlen, key, buf);
    if (sendMsg.payloadlen <= 0) {
        return COAP_ERROR_ENCRYPT_FAILED;
    }

    ret = CoAPMessage_send (ctx, addr, &sendMsg);

    return ret;
}

int secure_send (CoAPContext *ctx, NetworkAddr* addr, CoAPMessage *message, char observe)
{
    session_item* session = get_auth_info (ctx, addr);
    if (!session) {
        COAP_DEBUG("secure_send, session not found");
        return ALCS_ERR_AUTH_UNAUTH;
    }

    if (observe == 0) {
        CoAPUintOption_add (message, COAP_OPTION_OBSERVE, observe);
    }
    CoAPUintOption_add (message, COAP_OPTION_CONTENT_FORMAT, COAP_CT_APP_OCTET_STREAM);
    CoAPUintOption_add (message, COAP_OPTION_SESSIONID, session->sessionId);
    COAP_DEBUG("secure_send sessionId:%d", session->sessionId);

    int encryptlen = (message->payloadlen & 0xfffffff0) + 16;
    if (encryptlen > 64) {
        char* buf = (char*)coap_malloc(encryptlen);
        int rt = do_secure_send (ctx, addr, message, session->sessionKey, buf);
        coap_free (buf);
        return rt;
    } else {
        char buf[64];
        return do_secure_send (ctx, addr, message, session->sessionKey, buf);
    }
}

static void call_cb (CoAPContext *context, NetworkAddr *remote, CoAPMessage* message, const char* key, char* buf, secure_send_item* send_item)
{
    int len = alcs_decrypt ((const char *)message->payload, message->payloadlen, key, buf);
    CoAPMessage tmpMsg;
    memcpy (&tmpMsg, message, sizeof(CoAPMessage));
    tmpMsg.payload = (unsigned char *)buf;
    tmpMsg.payloadlen = len;
    send_item->orig_handler (context, COAP_REQUEST_SUCCESS, send_item->orig_user_data, remote, &tmpMsg);
}

void secure_sendmsg_handler(CoAPContext *context, CoAPReqResult result, void *userdata, NetworkAddr *remote, CoAPMessage *message)
{
    if (!userdata) {
        return;
    }

    secure_send_item* send_item = (secure_send_item*)userdata;
    if (result == COAP_RECV_RESP_TIMEOUT) {
        send_item->orig_handler (context, COAP_RECV_RESP_TIMEOUT, send_item->orig_user_data, remote, NULL);
        COAP_INFO("secure_sendmsg_handler timeout");
        return;
    }

    unsigned int sessionId = 0;
    CoAPUintOption_get (message, COAP_OPTION_SESSIONID, &sessionId);
    COAP_DEBUG("secure_sendmsg_handler, sessionID:%d", (int)sessionId);

    session_item* session = get_auth_info (context, remote);

    if (!session || session->sessionId != sessionId) {
        COAP_ERR ("secure_sendmsg_handler, need auth, from:%s", remote->addr);
        return;
    }

    if (message->payloadlen < 128) {
        char buf[128];
        call_cb (context, remote, message, session->sessionKey, buf, send_item);
    } else {
        char* buf = (char*)coap_malloc(message->payloadlen);
        if (buf) {
            call_cb (context, remote, message, session->sessionKey, buf, send_item);
            coap_free (buf);
        }
    }

    unsigned int obsVal;
    if (CoAPUintOption_get (message, COAP_OPTION_OBSERVE, &obsVal) != COAP_SUCCESS) {
        coap_free (send_item);
    }
}

int alcs_sendmsg_secure(CoAPContext *ctx, NetworkAddr* addr, CoAPMessage *message, char observe, CoAPSendMsgHandler handler)
{
    if (!ctx || !addr || !message) {
        return COAP_ERROR_INVALID_PARAM;
    }

    secure_send_item* item = (secure_send_item*)coap_malloc(sizeof(secure_send_item));
    item->orig_user_data = message->user;
    item->orig_handler = handler;
    item->keep = message->keep;
    message->handler = secure_sendmsg_handler;
    message->user = item;
    return secure_send (ctx, addr, message, observe);
}

int alcs_sendrsp_secure(CoAPContext *ctx, NetworkAddr* addr, CoAPMessage *message, char observe, unsigned short msgid, CoAPLenString* token)
{
    COAP_DEBUG("alcs_sendrsp_secure");
    if (!ctx || !addr || !message) {
        return COAP_ERROR_INVALID_PARAM;
    }

    if (msgid == 0) {
        message->header.msgid = CoAPMessageId_gen (ctx);
    } else {
        message->header.msgid = msgid;
    }

    if (token) {
        message->header.tokenlen = token->len;
        memcpy (&message->token, token->data, token->len);
    }

    return secure_send (ctx, addr, message, observe);
}

bool req_payload_parser (const char* payload, int len, char** seq, int* seqlen, char** data, int* datalen)
{
    if (!payload || !len || !seq || !seqlen || !datalen || !data) {
        return 0;
    }

    *seq = json_get_value_by_name((char*)payload, len, "id", seqlen, NULL);

    *data = json_get_value_by_name((char*)payload, len, "params", datalen, NULL);
    return *data && datalen;
}

extern void on_client_auth_timer (CoAPContext *);
extern void on_svr_auth_timer (CoAPContext *);

void on_auth_timer(void* param)
{
    if (!is_inited) {
        return;
    }

    CoAPContext *ctx = (CoAPContext *) param;
#ifdef ALCSCLIENT
    on_client_auth_timer (ctx);
#endif
#ifdef ALCSSERVER
    on_svr_auth_timer (ctx);
#endif
}

int alcs_auth_info (CoAPContext *context, char* data, int datalen, char* datatype, char* op)
{
    return 0;
}

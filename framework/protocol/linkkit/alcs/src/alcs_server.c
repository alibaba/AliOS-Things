#include "alcs_api_internal.h"
#include "json_parser.h"
#include "CoAPPlatform.h"
#include "utils_hmac.h"

#define RES_FORMAT "{\"id\":\"%.*s\",\"code\":%d,\"data\":{%s}}"

#ifdef ALCSSERVER
int sessionid_seed = 0xff;
static int default_heart_expire = 120000;

void alcs_rec_auth_select (CoAPContext *ctx, const char *paths, NetworkAddr* from, CoAPMessage* resMsg)
{
    int seqlen, datalen;
    char *seq, *data;
    char* targetKey = "";
    int targetLen = 0;
    //int res_code = 200;
    COAP_DEBUG ("receive data:%.*s", resMsg->payloadlen, resMsg->payload);

    do {
        if (!req_payload_parser((const char *)resMsg->payload, resMsg->payloadlen, &seq, &seqlen, &data, &datalen)) {
            break;
        }
        auth_list* lst = get_list(ctx);
        if (!lst) {
            break;
        }
        char* accesskeys;
        int keylen;
        accesskeys = json_get_value_by_name(data, datalen, "accessKeys", &keylen, NULL);
        if (!accesskeys || !keylen) {
            break;
        }
        COAP_DEBUG ("accessKeys:%.*s", keylen, accesskeys);
        char back;
        char *str_pos, *entry;
        int entry_len, type;

        backup_json_str_last_char (accesskeys, keylen, back);
        json_array_for_each_entry(accesskeys, keylen, str_pos, entry, entry_len, type) {
            COAP_DEBUG ("entry:%.*s", entry_len, entry);
            svr_key_item *node = NULL, *next = NULL;
            list_for_each_entry_safe(node, next, &lst->lst_svr, lst, svr_key_item) {
                COAP_DEBUG ("keyprefix:%s", node->keyprefix);
                if (strstr(entry, node->keyprefix) == entry) {
                    COAP_DEBUG ("target keyprefix:%s", entry);
                    targetKey = entry;
                    targetLen = entry_len;
                    break;
                }
            }
            if (targetKey) break;
        }
        restore_json_str_last_char (accesskeys, keylen, back);

    } while (0);

    COAP_DEBUG ("key:%s", targetKey);

    CoAPMessage msg;
    char keybuf[32];
    snprintf (keybuf, sizeof(keybuf), "\"accessKey\":\"%.*s\"", targetLen, targetKey);
    char payloadbuf[512];
    snprintf (payloadbuf, sizeof(payloadbuf), RES_FORMAT, seqlen, seq, targetKey? 200 : COAP_MSG_CODE_401_UNAUTHORIZED, keybuf);
    CoAPLenString payload = {strlen(payloadbuf), (unsigned char *)payloadbuf};

    alcs_msg_init (ctx, &msg, COAP_MSG_CODE_205_CONTENT, COAP_MESSAGE_TYPE_ACK, 0, &payload, NULL);
    CoAPLenString token = {resMsg->header.tokenlen,resMsg->token};
    alcs_sendrsp (ctx, from, &msg, 1, resMsg->header.msgid, &token);
}

svr_key_item* is_legal_key(CoAPContext *ctx, const char* keyprefix, int prefixlen, const char* keyseq, int seqlen, int* res_code)
{
    COAP_DEBUG ("islegal prefix:%.*s, seq:%.*s", prefixlen, keyprefix, seqlen, keyseq);

    auth_list* lst = get_list(ctx);
    if (lst) {
        COAP_DEBUG ("find devices");
        HAL_MutexLock(lst->list_mutex);

        if (lst->revocation) {
            int len = strlen(lst->revocation);
            int i;
            for (i = 0; i < len; i += KEYSEQ_LEN) {
                if (strncmp(keyseq, lst->revocation + i, seqlen) == 0) {
                    HAL_MutexUnlock(lst->list_mutex);
                    *res_code = ALCS_AUTH_REVOCATE;
                    COAP_INFO ("accesskey is revocated");
                    return NULL;
                }
            }
        }

        if (list_empty(&lst->lst_svr)) {
            *res_code = ALCS_AUTH_AUTHLISTEMPTY;
        } else {
            svr_key_item *node = NULL, *next = NULL;
            list_for_each_entry_safe(node, next, &lst->lst_svr, lst, svr_key_item) {
                COAP_DEBUG ("node prefix:%s", node->keyprefix);
                if (strlen(node->keyprefix) == prefixlen && strncmp (keyprefix, node->keyprefix, prefixlen) == 0) {
                    *res_code = ALCS_AUTH_OK;
                    HAL_MutexUnlock(lst->list_mutex);
                    return node;
                }
            }
            *res_code = ALCS_AUTH_UNMATCHPREFIX;
        }

        HAL_MutexUnlock(lst->list_mutex);
    }

    return NULL;
}

void alcs_rec_auth (CoAPContext *ctx, const char *paths, NetworkAddr* from, CoAPMessage* resMsg)
{
    int seqlen, datalen;
    char* seq, *data;
    //bool  result = 0;
    int res_code = 200;
    char body[200] = {0};
    COAP_INFO ("alcs_rec_auth:from:%s, payloadlen:%d", from->addr, resMsg->payloadlen);

    do {
        if (!req_payload_parser((const char *)resMsg->payload, resMsg->payloadlen, &seq, &seqlen, &data, &datalen)) {
            break;
        }
        char* accesskey, *randomkey, *sign;
        int tmplen;
        accesskey = json_get_value_by_name(data, datalen, "accessKey", &tmplen, NULL);
        COAP_INFO ("accesskey:%.*s", tmplen, accesskey);

        if (!accesskey || tmplen != KEYPREFIX_LEN + 1 + 1 + KEYSEQ_LEN) {
            break;
        }

        char* keyprefix = accesskey;
        char* keyseq = accesskey + KEYPREFIX_LEN + 1 + 1;

        svr_key_item* item = is_legal_key(ctx, keyprefix, KEYPREFIX_LEN, keyseq, KEYSEQ_LEN, &res_code);
        if (!item) {
            COAP_INFO ("islegal return null");
            break;
        }

        char accessToken[64];
        int tokenlen = sizeof(accessToken);
        utils_hmac_sha1_base64 (accesskey, tmplen, item->secret, strlen(item->secret), accessToken, &tokenlen);

        COAP_INFO ("accessToken:%.*s", tokenlen, accessToken);

        int randomkeylen;
        randomkey = json_get_value_by_name(data, datalen, "randomKey", &randomkeylen, NULL);
        if (!randomkey || !randomkeylen) {
            res_code = ALCS_AUTH_INVALIDPARAM;
            break;
        }

        /*calc sign, save in buf*/
        char buf[40];
        int calc_sign_len = sizeof(buf);
        utils_hmac_sha1_base64 (randomkey, randomkeylen, accessToken, tokenlen, buf, &calc_sign_len);

        COAP_INFO ("calc randomKey:%.*s,token:%.*s,sign:%.*s", randomkeylen, randomkey, tokenlen,
            accessToken, calc_sign_len, buf);

        sign = json_get_value_by_name(data, datalen, "sign", &tmplen, NULL);
        if (!sign || tmplen != calc_sign_len || strncmp(sign, buf, calc_sign_len)) {
            res_code = ALCS_AUTH_ILLEGALSIGN;
            break;
        }

        session_item* session = get_svr_session (ctx, from);
        if (!session) {
            session = (session_item*)coap_malloc(sizeof(session_item));
            gen_random_key((unsigned char *)session->randomKey, RANDOMKEY_LEN);
            session->sessionId = ++ sessionid_seed;
            memcpy (&session->addr, from, sizeof(NetworkAddr));
            COAP_INFO ("new session, addr:%s, port:%d", session->addr.addr, session->addr.port);
            struct list_head* svr_head = get_svr_session_list (ctx);
            list_add_tail(&session->lst, svr_head);
        }

        snprintf (buf, sizeof(buf), "%.*s%s", randomkeylen, randomkey, session->randomKey);
        utils_hmac_sha1_raw (buf,strlen(buf), session->sessionKey, accessToken, tokenlen);

        /*calc sign, save in buf*/
        calc_sign_len = sizeof(buf);
        utils_hmac_sha1_base64 (session->randomKey, RANDOMKEY_LEN, accessToken, tokenlen, buf, &calc_sign_len);
        snprintf (body, sizeof(body), "\"sign\":\"%.*s\",\"randomKey\":\"%s\",\"sessionId\":%d,\"expire\":86400",
             calc_sign_len, buf, session->randomKey, session->sessionId);

        session->authed_time = HAL_UptimeMs ();
        session->heart_time = session->authed_time;
        // ???
        //result = 1;

    } while (0);

    CoAPMessage message;
    char payloadbuf[512];
    snprintf (payloadbuf, sizeof(payloadbuf), RES_FORMAT, seqlen, seq, res_code, body);
    CoAPLenString payload = {strlen(payloadbuf), (unsigned char*)payloadbuf};

    alcs_msg_init (ctx, &message, COAP_MSG_CODE_205_CONTENT, COAP_MESSAGE_TYPE_ACK, 0, &payload, NULL);
    CoAPLenString token = {resMsg->header.tokenlen, resMsg->token};
    alcs_sendrsp (ctx, from, &message, 1, resMsg->header.msgid, &token);
}

int add_svr_key (CoAPContext *ctx, const char* keyprefix, const char* secret, bool isGroup)
{
    COAP_INFO("add_svr_key\n");

    auth_list* lst = get_list(ctx);
    if (!lst || lst->svr_count >= KEY_MAXCOUNT || strlen(keyprefix) != KEYPREFIX_LEN) {
        return COAP_ERROR_INVALID_LENGTH;
    }

    svr_key_item* item = (svr_key_item*) coap_malloc(sizeof(svr_key_item));
    if (!item) {
        return COAP_ERROR_MALLOC;
    }

    item->secret = (char*) coap_malloc(strlen(secret) + 1);
    if (!item->secret) {
        coap_free (item);
        return COAP_ERROR_MALLOC;
    }
    strcpy (item->secret, secret);
    strcpy (item->keyprefix, keyprefix);
    item->groupKey = isGroup;

    HAL_MutexLock(lst->list_mutex);
    list_add_tail(&item->lst, &lst->lst_svr);
    ++lst->svr_count;
    HAL_MutexUnlock(lst->list_mutex);

    return COAP_SUCCESS;
}

int alcs_add_svr_key (CoAPContext *ctx, const char* keyprefix, const char* secret)
{
    COAP_INFO("alcs_add_svr_key");
    return add_svr_key (ctx, keyprefix, secret, 0);
}


int alcs_remove_svr_key (CoAPContext *ctx, const char* keyprefix)
{
    auth_list* lst = get_list(ctx);
    if (!lst) {
        return COAP_ERROR_NULL;
    }

    svr_key_item *node = NULL, *next = NULL;
    HAL_MutexLock(lst->list_mutex);

    list_for_each_entry_safe(node, next, &lst->lst_svr, lst, svr_key_item) {
        if(strcmp(node->keyprefix, keyprefix) == 0){
            coap_free(node->secret);
            list_del(&node->lst);
            coap_free(node);
            break;
        }
    }
    HAL_MutexUnlock(lst->list_mutex);

    return COAP_SUCCESS;
}

int alcs_set_revocation (CoAPContext *ctx, const char* seqlist)
{
    auth_list* lst = get_list(ctx);
    if (!lst) {
        return COAP_ERROR_NULL;
    }

    HAL_MutexLock(lst->list_mutex);

    int len = seqlist? strlen(seqlist) : 0;
    if (lst->revocation) {
        coap_free(lst->revocation);
        lst->revocation = NULL;
    }

    if (len > 0) {
        lst->revocation = (char*)coap_malloc (len + 1);
        strcpy (lst->revocation, seqlist);
    }
    HAL_MutexUnlock(lst->list_mutex);

    return COAP_SUCCESS;
}

//-----------------------------------------

void send_err_rsp (CoAPContext* ctx, NetworkAddr*addr, int code, CoAPMessage* request)
{
    CoAPMessage sendMsg;
    CoAPLenString payload = {0};
    alcs_msg_init (ctx, &sendMsg, code, COAP_MESSAGE_TYPE_ACK, 0, &payload, NULL);
    CoAPLenString token = {request->header.tokenlen,request->token};
    alcs_sendrsp (ctx, addr, &sendMsg, 1, request->header.msgid, &token);
}

void call_cb (CoAPContext *context, const char *path, NetworkAddr *remote, CoAPMessage* message, const char* key, char* buf, CoAPRecvMsgHandler cb)
{
    CoAPMessage tmpMsg;
    memcpy (&tmpMsg, message, sizeof(CoAPMessage));

    if (key && buf) {
        int len = alcs_decrypt ((const char *)message->payload, message->payloadlen, key, buf);
        tmpMsg.payload = (unsigned char *)buf;
        tmpMsg.payloadlen = len;
    } else {
        tmpMsg.payload = NULL;
        tmpMsg.payloadlen = 0;
    }

    cb (context, path, remote, &tmpMsg);
}

void recv_msg_handler (CoAPContext *context, const char *path, NetworkAddr *remote, CoAPMessage *message)
{
    secure_resource_cb_item*node = NULL, *next = NULL, *cur = NULL;
    char path_calc[MAX_PATH_CHECKSUM_LEN] = {0};
    CoAPPathMD5_sum (path, strlen(path), path_calc, MAX_PATH_CHECKSUM_LEN);

    list_for_each_entry_safe(node, next, &secure_resource_cb_head, lst, secure_resource_cb_item) {
        if (memcmp(node->path, path_calc, MAX_PATH_CHECKSUM_LEN) == 0){
            cur = node;
            break;
        }
    }

    if (!cur) {
        COAP_ERR ("receive unknown request, path:%s", path);
        return;
    }

    unsigned int sessionId = 0;
    CoAPUintOption_get (message, COAP_OPTION_SESSIONID, &sessionId);
    COAP_DEBUG("recv_msg_handler, sessionID:%d", (int)sessionId);

    session_item* session = get_svr_session (context, remote);
    if (!session || session->sessionId != sessionId) {
        send_err_rsp (context, remote, COAP_MSG_CODE_401_UNAUTHORIZED, message);
        COAP_ERR ("need auth, path:%s, from:%s", path, remote->addr);
        return;
    } else {
        session->heart_time = HAL_UptimeMs();    
    }

    unsigned int obsVal;
    if (CoAPUintOption_get (message, COAP_OPTION_OBSERVE, &obsVal) == COAP_SUCCESS) {
        if (obsVal == 0) {
            CoAPObsServer_add (context, path, remote, message);
        }
    }

    if (message->payloadlen < 256) {
        char buf[256];
        call_cb (context, path, remote, message, session->sessionKey, buf, cur->cb);
    } else {
        char* buf = (char*)coap_malloc(message->payloadlen);
        if (buf) {
            call_cb (context, path, remote, message, session->sessionKey, buf, cur->cb);
            coap_free (buf);
        } else {
            COAP_ERR ("fail to malloc %dB", message->payloadlen);
        }
    }
}

int alcs_resource_register_secure (CoAPContext *context, const char *path, unsigned short permission,
            unsigned int ctype, unsigned int maxage, CoAPRecvMsgHandler callback)
{
    COAP_INFO("alcs_resource_register_secure");

    secure_resource_cb_item* item = (secure_resource_cb_item*)coap_malloc (sizeof(secure_resource_cb_item));
    item->cb = callback;
    CoAPPathMD5_sum (path, strlen(path), item->path, MAX_PATH_CHECKSUM_LEN);
    list_add_tail(&item->lst, &secure_resource_cb_head);

    return CoAPResource_register (context, path, permission, ctype, maxage, &recv_msg_handler);
}

void alcs_resource_cb_deinit(void)
{
	secure_resource_cb_item* del_item = NULL;

	list_for_each_entry(del_item,&secure_resource_cb_head,lst,secure_resource_cb_item)
	{
		list_del(&del_item->lst);
		coap_free(del_item);
		del_item = list_entry(&secure_resource_cb_head,secure_resource_cb_item,lst);
	}
}

void alcs_rec_heart_beat(CoAPContext *ctx, const char *paths, NetworkAddr *remote, CoAPMessage *request)
{
    COAP_DEBUG ("alcs_rec_heart_beat");

    session_item* session = get_svr_session (ctx, remote);
    if (session) {
        session->heart_time = HAL_UptimeMs();
    } else {
        COAP_INFO ("receive stale heart beat");
    }

    int seqlen, datalen;
    char *seq, *data;
    if (!req_payload_parser((const char *)request->payload, request->payloadlen, &seq, &seqlen, &data, &datalen)) {
        //do nothing
    }

    CoAPMessage msg;
    char databuf[32];
    char payloadbuf[128];

    if (session) {
        snprintf (databuf, sizeof(databuf), "\"delayTime\":%d", default_heart_expire / 1000);
        snprintf (payloadbuf, sizeof(payloadbuf), RES_FORMAT, seqlen, seq, 200, databuf);
    } else {
        snprintf (payloadbuf, sizeof(payloadbuf), RES_FORMAT, seqlen, seq, ALCS_HEART_FAILAUTH, "");
    }

    CoAPLenString payload = {strlen(payloadbuf), (unsigned char *)payloadbuf};
    alcs_msg_init (ctx, &msg, COAP_MSG_CODE_205_CONTENT, COAP_MESSAGE_TYPE_CON, 0, &payload, NULL);
    CoAPLenString token = {request->header.tokenlen,request->token};
    if (session) {
        alcs_sendrsp_secure (ctx, remote, &msg, 1, request->header.msgid, &token);
    } else {
        alcs_sendrsp (ctx, remote, &msg, 1, request->header.msgid, &token);
    }
}

int observe_data_encrypt(CoAPContext *ctx, NetworkAddr* from, CoAPMessage *message, CoAPLenString *src, CoAPLenString *dest)
{
    COAP_DEBUG("observe_data_encrypt, src:%.*s", src->len, src->data);

    session_item* session = get_svr_session (ctx, from);
    if (session) {
        dest->len = (src->len & 0xfffffff0) + 16;
        dest->data  = (unsigned char*)coap_malloc(dest->len);
        if (alcs_encrypt ((const char*)src->data, src->len, session->sessionKey, dest->data) <= 0) {
            return COAP_ERROR_ENCRYPT_FAILED;
        }
        CoAPUintOption_add (message, COAP_OPTION_SESSIONID, session->sessionId);
        return COAP_SUCCESS;
    } else {
        COAP_INFO ("observe_data_encrypt, no session found!");
    }

    return COAP_ERROR_ENCRYPT_FAILED;
}

void on_svr_auth_timer (CoAPContext* ctx)
{
    struct list_head* head = get_svr_session_list (ctx);
    if (!head || list_empty(head)) {
        return;
    }
    //COAP_INFO ("on_svr_auth_timer:%d", (int)HAL_UptimeMs());

    //device_auth_list* dev = get_device (ctx);
    int tick = HAL_UptimeMs();

    session_item *node = NULL, *next = NULL;
    list_for_each_entry_safe(node, next, head, lst, session_item) {
        if(node->sessionId && node->heart_time + default_heart_expire < tick) {
            COAP_ERR ("heart beat timeout");
            remove_session (ctx, node);
        }
    }
}
#endif

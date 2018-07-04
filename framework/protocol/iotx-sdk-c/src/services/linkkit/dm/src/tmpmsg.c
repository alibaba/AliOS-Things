#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "packet.h"
#include "tmpmsg.h"
#include "iot_import.h"
#include "json_parser.h"

#define PARAMS_FMT_TOPO_ADD       "[{\"deviceName\": \"%s\", \"productKey\": \"%s\", \"sign\": \"%s\", \"signmethod\": \"%s\", \"timestamp\": \"%s\", \"clientId\": \"%s\"}]"
#define PARAMS_FMT_TOPO_DELETE    "[{\"deviceName\": \"%s\", \"productKey\": \"%s\"}]"

#define PARAMS_FMT_LOGIN          "{\"productKey\":\"%s\",\"deviceName\":\"%s\",\"clientId\":\"%s\",\"timestamp\":\"%s\",\"signMethod\":\"%s\",\"sign\":\"%s\",\"cleanSession\":\"%s\"}"
#define PARAMS_FMT_LOGOUT         "{\"productKey\":\"%s\",\"deviceName\":\"%s\"}"

#define PARAMS_FMT_SUB_REGISTER   "[{\"deviceName\": \"%s\", \"productKey\": \"%s\"}]"
#define PARAMS_FMT_SUB_UNREGISTER "[{\"deviceName\": \"%s\", \"productKey\": \"%s\"}]"

#define PARAMS_FMT_POST_EXTINFO   "[{\"attrKey\": \"%s\", \"attrValue\": \"%s\"}]"
#define PARAMS_FMT_DELETE_EXTINFO "[{\"attrKey\": \"%s\"}]"
#if 0
extern void utils_hmac_md5(const char *msg, int msg_len, char *digest, const char *key, int key_len);

static int calc_sign(char *productKey, char *deviceName, char *deviceSecret,
                     char *clientId, char *timestamp, char *sign, int sign_len)
{
    char buf[256] = {0};
    char digist[64] = {0};
    HAL_Snprintf(buf, sizeof(buf), "clientId%sdeviceName%sproductKey%stimestamp%s",
            clientId, deviceName, productKey, timestamp);

    utils_hmac_md5(buf, strlen(buf), digist, deviceSecret, strlen(deviceSecret));

    if (sign_len - 1 < strlen(digist))
        return -1;

    strncpy(sign, digist, strlen(digist));

    return 0;
}

int tmpmsg_add_topo(char *reqbuf, int len, int id, char *productKey, char *deviceName, char *deviceSecret)
{
    char sign[64] = {0};
    char *signmethod = "hmacMd5";

    char timestamp[16] = {0};
    HAL_Snprintf(timestamp, sizeof(timestamp), "%llu", (unsigned long long)HAL_UptimeMs());

    char clientId[64] = {0};
    HAL_Snprintf(clientId, sizeof(clientId), "%s.%s", productKey, deviceName);

    if (calc_sign(productKey, deviceName, deviceSecret, clientId, timestamp, sign, sizeof(sign)) < 0)
        return -1;

    /* build request */
    linkkit_pkt_t pkt;
    packet_init_request(&pkt, reqbuf, len, id, "thing.topo.add");
    packet_printf(&pkt, PARAMS_FMT_TOPO_ADD, deviceName, productKey, sign, signmethod, timestamp, clientId);
    packet_final(&pkt);

    return 0;
}

int tmpmsg_get_topo(char *reqbuf, int len, int id)
{
    /* build request */
    linkkit_pkt_t pkt;
    packet_init_request(&pkt, reqbuf, len, id, "thing.topo.get");
    packet_final(&pkt);

    return 0;
}

int tmpmsg_delete_topo(char *reqbuf, int len, int id, char *productKey, char *deviceName)
{
    /* build request */
    linkkit_pkt_t pkt;
    packet_init_request(&pkt, reqbuf, len, id, "thing.topo.delete");
    packet_printf(&pkt, PARAMS_FMT_TOPO_DELETE, deviceName, productKey);
    packet_final(&pkt);

    return 0;
}

int tmpmsg_subdev_login(char *reqbuf, int len, int id, char *productKey, char *deviceName, char *deviceSecret)
{
    char sign[64] = {0};
    char *signmethod = "hmacMd5";

    char timestamp[16] = {0};
    HAL_Snprintf(timestamp, sizeof(timestamp), "%llu", (unsigned long long)HAL_UptimeMs());

    char clientId[64] = {0};
    HAL_Snprintf(clientId, sizeof(clientId), "%s.%s", productKey, deviceName);

    if (calc_sign(productKey, deviceName, deviceSecret, clientId, timestamp, sign, sizeof(sign)) < 0)
        return -1;

    /* build request */
    linkkit_pkt_t pkt;
    packet_init_request(&pkt, reqbuf, len, id, "combine.login");
    packet_printf(&pkt, PARAMS_FMT_LOGIN, productKey, deviceName, clientId, timestamp, signmethod, sign, "true");
    packet_final(&pkt);

    return 0;
}

int tmpmsg_subdev_logout(char *reqbuf, int len, int id, char *productKey, char *deviceName)
{
    /* build request */
    linkkit_pkt_t pkt;
    packet_init_request(&pkt, reqbuf, len, id, "combine.logout");
    packet_printf(&pkt, PARAMS_FMT_LOGOUT, productKey, deviceName);
    packet_final(&pkt);

    return 0;
}

int tmpmsg_subdev_register(char *reqbuf, int len, int id, char *productKey, char *deviceName)
{
    /* build request */
    linkkit_pkt_t pkt;
    packet_init_request(&pkt, reqbuf, len, id, "thing.sub.register");
    packet_printf(&pkt, PARAMS_FMT_SUB_REGISTER, deviceName, productKey);
    packet_final(&pkt);

    return 0;
}

int tmpmsg_subdev_unregister(char *reqbuf, int len, int id, char *productKey, char *deviceName)
{
    /* build request */
    linkkit_pkt_t pkt;
    packet_init_request(&pkt, reqbuf, len, id, "thing.sub.unregister");
    packet_printf(&pkt, PARAMS_FMT_SUB_UNREGISTER, deviceName, productKey);
    packet_final(&pkt);

    return 0;
}

int tmpmsg_post_extinfo(char *reqbuf, int len, int id, char *key, char *value)
{
    /* build request */
    linkkit_pkt_t pkt;
    packet_init_request(&pkt, reqbuf, len, id, "thing.deviceinfo.update");
    packet_printf(&pkt, PARAMS_FMT_POST_EXTINFO, key, value);
    packet_final(&pkt);

    return 0;
}

int tmpmsg_post_extinfos(char *reqbuf, int len, int id, linkkit_extinfo_t *extinfos, int nb_extinfos)
{
    int i;

    linkkit_pkt_t pkt;
    packet_init_request(&pkt, reqbuf, len, id, "thing.deviceinfo.update");

    packet_printf(&pkt, "[");
    for (i = 0; i < nb_extinfos; i++) {
        packet_printf(&pkt, "{\"attrKey\":\"%s\",\"attrValue\":\"%s\"}",
                extinfos[i].attrKey, extinfos[i].attrValue);
        if (i < nb_extinfos - 1)
            packet_printf(&pkt, ",");
    }
    packet_printf(&pkt, "]");

    packet_final(&pkt);

    return 0;
}

int tmpmsg_delete_extinfo(char *reqbuf, int len, int id, char *key)
{
    /* build request */
    linkkit_pkt_t pkt;
    packet_init_request(&pkt, reqbuf, len, id, "thing.deviceinfo.delete");
    packet_printf(&pkt, PARAMS_FMT_DELETE_EXTINFO, key);
    packet_final(&pkt);

    return 0;
}

int tmpmsg_delete_extinfos(char *reqbuf, int len, int id, linkkit_extinfo_t *extinfos, int nb_extinfos)
{
    /* build request */
    int i;

    linkkit_pkt_t pkt;
    packet_init_request(&pkt, reqbuf, len, id, "thing.deviceinfo.delete");

    packet_printf(&pkt, "[");
    for (i = 0; i < nb_extinfos; i++) {
        packet_printf(&pkt, "{\"attrKey\":\"%s\"}",
                extinfos[i].attrKey, extinfos[i].attrValue);
        if (i < nb_extinfos - 1)
            packet_printf(&pkt, ",");
    }
    packet_printf(&pkt, "]");

    packet_final(&pkt);

    return 0;
}

int tmpmsg_post_event(char *reqbuf, int len, int id, char *identifier, char *events)
{
    char method[64] = {0};
    HAL_Snprintf(method, sizeof(method), "thing.event.%s.post", identifier);

    linkkit_pkt_t pkt;
    packet_init_request(&pkt, reqbuf, len, id, method);
    packet_printf(&pkt, "%s", events);
    packet_final(&pkt);

    return 0;
}

int tmpmsg_delete_extinfo(char *reqbuf, int len, int id, char *key)
{
    /* build request */
    linkkit_pkt_t pkt;
    packet_init_request(&pkt, reqbuf, len, id, "thing.deviceinfo.delete");
    packet_printf(&pkt, PARAMS_FMT_DELETE_EXTINFO, key);
    packet_final(&pkt);

    return 0;
}

int tmpmsg_get_tsl(char *reqbuf, int len, int id)
{
    linkkit_pkt_t pkt;
    packet_init_request(&pkt, reqbuf, len, id, "thing.dsltemplate.get");
    packet_printf(&pkt, "%s", "{}");
    packet_final(&pkt);

    return 0;
}

int tmpmsg_parse_request(char *in, int in_len, tmp_req_t *req)
{
    memset(req, 0, sizeof(tmp_req_t));

    int len;
    char *id_str = json_get_value_by_name(in, in_len, "id", &len, NULL);
    if (id_str && len < sizeof(req->id))
        strncpy(req->id, id_str, len);

    char *ver_str = json_get_value_by_name(in, in_len, "version", &len, NULL);
    if (ver_str && len < sizeof(req->version))
        strncpy(req->version, ver_str, len);

    req->method = json_get_value_by_name(in, in_len, "method", &req->method_len, NULL);
    req->params = json_get_value_by_name(in, in_len, "params", &req->params_len, NULL);

    return 0;
}

int tmpmsg_parse_response(char *in, int in_len, tmp_resp_t *resp)
{
    memset(resp, 0, sizeof(tmp_resp_t));

    int len;
    char *id_str = json_get_value_by_name(in, in_len, "id", &len, NULL);
    if (id_str && len < sizeof(resp->id))
        strncpy(resp->id, id_str, len);

    char *code_str = json_get_value_by_name(in, in_len, "code", &len, NULL);
    if (code_str)
        resp->code = atoi(code_str);

    resp->msg = json_get_value_by_name(in, in_len, "message", &resp->msg_len, NULL);
    resp->data = json_get_value_by_name(in, in_len, "data", &resp->data_len, NULL);

    if (resp->data) {
        /* drop empty data */
        if (strncmp(resp->data, "{}", 2) == 0 || strncmp(resp->data, "[]", 2) == 0) {
            resp->data = NULL;
            resp->data_len = 0;
        }
    }

    return 0;
}

int tmpmsg_compose_response(char *buf, int buf_len, int id, int code, char *data, int data_len)
{
    linkkit_pkt_t pkt;
    packet_init_response(&pkt, buf, buf_len, id, code);

    if (packet_strncat(&pkt, data, data_len) < 0)
        return -1;

    if (packet_final(&pkt) < 0)
        return -1;

    return 0;
}
#endif

int tmpmsg_post_extinfos(char *reqbuf, int len, linkkit_extinfo_t *extinfos, int nb_extinfos)
{
    int i;
    linkkit_pkt_t pkt;

    packet_init(&pkt, reqbuf, len);
    packet_printf(&pkt, "[");
    for (i = 0; i < nb_extinfos; i++) {
        packet_printf(&pkt, "{\"attrKey\":\"%s\",\"attrValue\":\"%s\"}",
                extinfos[i].attrKey, extinfos[i].attrValue);
        if (i < nb_extinfos - 1)
            packet_printf(&pkt, ",");
    }
    packet_printf(&pkt, "]");

    return 0;
}

int tmpmsg_post_event(char *reqbuf, int len, char *events)
{
    linkkit_pkt_t pkt;
    packet_init(&pkt, reqbuf, len);
    packet_printf(&pkt, "%s", events);

    return 0;
}


int tmpmsg_delete_extinfos(char *reqbuf, int len, linkkit_extinfo_t *extinfos, int nb_extinfos)
{
    /* build request */
    int i;

    linkkit_pkt_t pkt;

    packet_init(&pkt, reqbuf, len);
    packet_printf(&pkt, "[");
    for (i = 0; i < nb_extinfos; i++) {
        packet_printf(&pkt, "{\"attrKey\":\"%s\"}",
                extinfos[i].attrKey, extinfos[i].attrValue);
        if (i < nb_extinfos - 1)
            packet_printf(&pkt, ",");
    }
    packet_printf(&pkt, "]");

    return 0;
}


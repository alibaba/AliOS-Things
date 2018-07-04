#ifndef __LINKKIT_TMPMSG_H__
#define __LINKKIT_TMPMSG_H__

#include "linkkit_gateway_export.h"
#if 0
int tmpmsg_get_topo(char *reqbuf, int len, int id);
int tmpmsg_add_topo(char *reqbuf, int len, int id, char *productKey, char *deviceName, char *deviceSecret);
int tmpmsg_delete_topo(char *reqbuf, int len, int id, char *productKey, char *deviceName);

int tmpmsg_subdev_login(char *reqbuf, int len, int id, char *productKey, char *deviceName, char *deviceSecret);
int tmpmsg_subdev_logout(char *reqbuf, int len, int id, char *productKey, char *deviceName);

int tmpmsg_subdev_register(char *reqbuf, int len, int id, char *productKey, char *deviceName);
int tmpmsg_subdev_unregister(char *reqbuf, int len, int id, char *productKey, char *deviceName);

int tmpmsg_post_extinfo(char *reqbuf, int len, int id, char *key, char *value);
int tmpmsg_post_extinfos(char *reqbuf, int len, int id, linkkit_extinfo_t *extinfos, int nb_extinfos);

int tmpmsg_delete_extinfo(char *reqbuf, int len, int id, char *key);
int tmpmsg_delete_extinfos(char *reqbuf, int len, int id, linkkit_extinfo_t *extinfos, int nb_extinfos);

int tmpmsg_post_event(char *reqbuf, int len, int id, char *identifier, char *events);

int tmpmsg_get_tsl(char *reqbuf, int len, int id);

typedef struct {
    char  id[16];
    char  version[16];

    char *method;
    int   method_len;

    char *params;
    int   params_len;
} tmp_req_t;

int tmpmsg_parse_request(char *in, int in_len, tmp_req_t *req);

typedef struct {
    char  id[16];
    int   code;

    char *msg;
    int   msg_len;

    char *data;
    int   data_len;
} tmp_resp_t;

int tmpmsg_parse_response(char *in, int in_len, tmp_resp_t *resp);

int tmpmsg_compose_response(char *buf, int buf_len, int id, int code, char *data, int data_len);
#endif
int tmpmsg_post_extinfos(char *reqbuf, int len, linkkit_extinfo_t *extinfos, int nb_extinfos);
int tmpmsg_delete_extinfos(char *reqbuf, int len, linkkit_extinfo_t *extinfos, int nb_extinfos);
int tmpmsg_post_event(char *reqbuf, int len, char *events);

#endif /* __LINKKIT_TMPMSG_H__ */

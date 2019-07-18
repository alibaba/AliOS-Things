/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef UAGENT_TYPE_H
#define UAGENT_TYPE_H

/* type define */
typedef unsigned short ua_mod_t;
typedef unsigned short ua_func_t;
typedef unsigned char service_agent_rlt_t;

#define SERVICE_AGENT_FAIL (service_agent_rlt_t)0x00
#define SERVICE_AGENT_OK   (service_agent_rlt_t)0x01

/* uagent support function name size, include null-terminated */
#define UAGENT_FUNC_NAME_SIZE 12

/* Values for ua_send_policy_t are constructed by a bitwise-inclusive OR of flags
 * from the @send_policy_bit_ctrl_t
 */
typedef unsigned char  ua_send_policy_t;

typedef enum{
    send_policy_save_cloud      = 0x01,
    send_policy_trans_guarantee = 0x02,
    send_policy_object          = 0x04,
    send_policy_delay           = 0x08,
    send_policy_delay_dump      = 0x10,
}send_policy_bit_ctrl_t;

#define POLICY_SET(POLICY, MASK) ((POLICY&MASK)==MASK)

/* uAgent module name */
#define UAGENT_MOD_UAGENT     (ua_mod_t)0

/* externel module  - cloud */
#define UAGENT_MOD_CLOUD      (ua_mod_t)5

/* uND module */
#define UAGENT_MOD_UND        (ua_mod_t)0x10

/* uLog module  */
#define UAGENT_MOD_ULOG       (ua_mod_t)0x11

/* uOTA module  */
#define UAGENT_MOD_UOTA       (ua_mod_t)0x12

/* CLI module  */
#define UAGENT_MOD_CLI        (ua_mod_t)0x13


#define UAGENT_MOD_BROAD_CAST (ua_mod_t)0xFF

/* ------------------------ Function List ------------------------ */
/* Inner function type used to register module */
#define UAGENT_FUNC_UA_REGISTER              (ua_func_t)0x0001

#define UAGENT_FUNC_UA_UNREGISTER            (ua_func_t)0x0002

#define UAGENT_FUNC_UA_RQST_CONN             (ua_func_t)0x0003

#define UAGENT_FUNC_UA_DUMP_DELAY_SEND       (ua_func_t)0x0004

#define UAGENT_FUNC_UA_DUMP_DELAY_SEND_PARAM (ua_func_t)0x0005

/**
 * Nofity connect build, format is {"comm conn":1}/ {"comm conn":0}.
 * Refer @UAGENT_NOTICE_CONNECT for format when using.
 * Remark: the cb will be called to indicates current situation right now when register.
 */
#define UAGENT_FUNC_NOTIFY_CLOUD_CONN (ua_func_t)0x0011

#define UAGENT_FUNC_INT_TEL           (ua_func_t)0x0021

#define UAGENT_FUNC_UA_SHOW_LIST      (ua_func_t)0x0051

/* aquire high level service is enable executed */
#define UAGENT_FUNC_UA_APPLY          (ua_func_t)0x8000

/* reboot */
#define UAGENT_FUNC_REBOOT            (ua_func_t)0x8002

/* Auth high level service is enable executed */
#define UAGENT_FUNC_UA_AUTH           (ua_func_t)0x8055

/* user only can use [H]BASE~[H]BASE+0x100 */
#define UAGENT_USER_FUNC_SIZE         (ua_func_t)0x100
/* base of user define function */
#define UAGENT_FUNC_USER_BASE         (ua_func_t)0x0400

/* base of user define function, which shall be authrized */
#define UAGENT_FUNC_AUTH_USER_BASE    (ua_func_t)0x8400

/* user service function, this call-back will be called if related telegram received
 * (inner telegram or externel telegram
 */
typedef int (*on_customer_service)(void *arg, const unsigned short len, void *str);

typedef struct {
    ua_mod_t      mod;
    unsigned char func_count;
    char          *name;
    char          *version;
} uagent_mod_info_t;

typedef struct _uagent_func_node_t {
    ua_func_t                  func;
    char                       func_name[UAGENT_FUNC_NAME_SIZE];
    on_customer_service        service;
    void                       *argu;
    struct _uagent_func_node_t *next;
} uagent_func_node_t;

typedef struct {
    uagent_mod_info_t  mod_info;
    uagent_func_node_t *header;
} mod_func_t;

#define UAGENT_INFO_STR \
"{\"id\":%lu,\"version\":\"1.0\",\"params\":\
{\"srcdev\":\"%s\",\"show\":%d,\"mod\":%d,\"func\":%d,\"param\":\"%s\"}}"

#define UAGENT_INFO_ACK_OK \
"{\"id\":%lu,\"version\":\"1.0\",\"params\":\
{\"srcdev\":\"%s\",\"show\":%d,\"mod\":%d,\"func\":%d, \"ack\":%d}}"

#define UAGENT_INFO_ACK_FAIL \
"{\"id\":%lu,\"version\":\"1.0\",\"params\":\
{\"srcdev\":\"%s\",\"show\":%d,\"mod\":%d,\"func\":%d, \"ack\":%d,\"param\":\"%s\"}}"

#define UAGENT_INFO_OBJ \
    "{\"id\":%lu,\"version\":\"1.0\",\"params\":\
{\"srcdev\":\"%s\",\"show\":%d,\"mod\":%d,\"func\":%d,\"param\":%s}}"

#define UAGENT_MAN_T \
"{\"id\":\"1\",\"version\":\"1.0\",\"params\":\
{\"dstdev\":\"%s\",\"mod\":%d,\"func\":%d,\"param\":\"%s\"}}"

#define UAGENT_NOTICE_CONNECT "{\"comm conn\":%d}"

#endif /* UAGENT_TYPE_H */


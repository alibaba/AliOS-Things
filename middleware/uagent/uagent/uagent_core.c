/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "uagent/uagent.h"
#include "uagent_inner_api.h"
#include "aos/kernel.h"
#include "aos/errno.h"
#include "k_config.h"
#include "cJSON.h"

typedef struct {
    ua_mod_t            mod;
    ua_func_t           func;
    on_customer_service service;
    void*               arg;
} uagent_service_node;

typedef struct {
    ua_mod_t       mod;
    ua_func_t      func;
    unsigned long  id;
    ua_mod_t       src;  /* not apply to down stream */
    unsigned short len;
}uagent_exchange_header_t;

typedef struct {
    ua_func_t            func;
    char                 func_name[UAGENT_FUNC_NAME_SIZE];
    on_customer_service  service;
    void*                arg;
}uagent_exchange_service_t;

uagent_service_attr_t uagent_service_attr = { 0, 0 };

static uagent_service_node *uagent_service_tb[UAGENT_SERVICE_SIZE] = { NULL };

static mod_func_t *mod_func[UAGENT_MOD_SIZE] = { NULL };

static aos_queue_t uagent_queue;
static uint8_t*    uagent_queue_buffer;
static aos_task_t uagent_task;

static aos_timer_t timer_request_cloud_connect;
static aos_timer_t timer_monitor_output_queue;

/* Private Operation Declaration */
static int update_create_uagent_service_node(const ua_mod_t mod, const ua_func_t func,
                                             on_customer_service service, void *arg, 
                                             const char add);

static int uagent_on_recv_handler(const unsigned short len, const char *str);

static void uagent_routine(void *para);

/* pre-define service of uAgent */
static void service_check_connect(void);
static void service_report_func_list(void);
static void service_update_delay_send_param(const unsigned short len, void *str);

static void uagent_timer_service(void *timer, void *para)
{
    if (NULL != timer) {
        if (timer == (&timer_request_cloud_connect)->hdl) {
            uagent_request_service(UAGENT_MOD_UAGENT, UAGENT_MOD_UAGENT, UAGENT_FUNC_UA_RQST_CONN, 0, NULL);
        } else if (timer == (&timer_monitor_output_queue)->hdl) {
            uagent_request_service(UAGENT_MOD_UAGENT, UAGENT_MOD_UAGENT, UAGENT_FUNC_UA_DUMP_DELAY_SEND, 0, NULL);
        }
    }
}

void start_monitor_delay_out(void)
{
    aos_timer_start(&timer_monitor_output_queue);
}

void stop_monitor_delay_out(void)
{
    aos_timer_stop(&timer_monitor_output_queue);
}

/* Public Operation Implementation */
int uagent_init(void)
{
    int rc = 0;

    if (!UAGENT_INITED_FINISH) {
        uagent_init_mutex();
        rc = -1;
        /* initial uagent queue */
        uagent_queue_buffer = aos_malloc(UAGENT_PAYLOAD_CMD_SIZE*UAGENT_QUEUE_COUNT);
        if (NULL != uagent_queue_buffer) {
            if (0 == aos_queue_new(&uagent_queue,
                uagent_queue_buffer,
                UAGENT_PAYLOAD_CMD_SIZE*UAGENT_QUEUE_COUNT,
                UAGENT_PAYLOAD_CMD_SIZE)) {
                if (0 == aos_task_new_ext(&uagent_task, "uagent", uagent_routine,
                    NULL, UAGENT_ROUTINE_TASK_STACK_DEPTH,
                    RHINO_CONFIG_USER_PRI_MAX - 2)) {
                    uagent_service_attr.initd = UAGENT_INITED;
                    rc = 0;
                } else {
                    aos_queue_free(&uagent_queue);
                    aos_free(uagent_queue_buffer);
                    uagent_queue_buffer = NULL;
                }
            } else {
                aos_free(uagent_queue_buffer);
                uagent_queue_buffer = NULL;
            }
        }
    }
    return rc;
}

int uagent_ext_comm_start(const char *pk, const char *dn)
{
    int rc = -EINVAL;
    if (uagent_service_attr.service_started==0 && NULL != pk && NULL != dn && strlen(pk)>0 && strlen(dn)>0 && UAGENT_INITED_FINISH) {
        strncpy(uagent_service_attr.pk, pk, sizeof(uagent_service_attr.pk) - 1);
        strncpy(uagent_service_attr.dn, dn, sizeof(uagent_service_attr.dn) - 1);
        rc = uagent_ext_comm_init(uagent_service_attr.pk, uagent_service_attr.dn, uagent_on_recv_handler);
        /* todo: re-try in routine if start fail */
#if UAGENT_CHECK_COMM_CYCLE!=0
        if (0 != aos_timer_new(&timer_request_cloud_connect,
            uagent_timer_service,
            NULL,
            UAGENT_CHECK_COMM_CYCLE,
            1)) {
            UAGENT_ERR("[uA]uagent start timer fail\n");
        }
#endif
        if (0 != aos_timer_new_ext(&timer_monitor_output_queue,
            uagent_timer_service,
            NULL,
            UAGENT_WAIT_BUFFER,
            0,
            0)) {
            UAGENT_ERR("[uA]uagent start timer fail\n");
        }
        uagent_service_attr.service_started = 1;
    }
    return rc;
}

int uagent_register(const ua_mod_t mod, const char *mod_name,  char *version,
                    const ua_func_t func, const char *func_name,
                    on_customer_service cb, void *arg)
{
    if (UAGENT_INITED_FINISH) {
        /* pre handle special event require */
        switch (func)
        {
        case UAGENT_FUNC_NOTIFY_CLOUD_CONN:
        {
            char buf[UAGENT_PAYLOAD_CMD_STR_SIZE];
            snprintf(buf, UAGENT_PAYLOAD_CMD_STR_SIZE, UAGENT_NOTICE_CONNECT, uagent_service_attr.cloud_connect);
            if (NULL != cb) {
                cb(arg, strlen(buf) + 1, buf);
            }
        }
        break;
        default:
            break;
        }
        uagent_exchange_service_t uagent_exchange_service = { func, "", cb, arg };
        memset(uagent_exchange_service.func_name, 0, sizeof(uagent_exchange_service.func_name));
        strncpy(uagent_exchange_service.func_name, func_name, sizeof(uagent_exchange_service.func_name) - 1);
        if (func >= UAGENT_FUNC_USER_BASE) {
            if (NULL != mod_name && NULL != version) {
                unsigned char i = 0;
                for (; i < UAGENT_MOD_SIZE; i++) {
                    if (mod_func[i] == NULL) {
                        mod_func[i] = (mod_func_t*)aos_malloc(sizeof(mod_func_t));
                        if (mod_func[i] != NULL) {
                            mod_func[i]->header = NULL;
                            mod_func[i]->mod_info.mod = mod;
                            mod_func[i]->mod_info.func_count = 0;
                            mod_func[i]->mod_info.name = (char*)aos_malloc(strlen(mod_name) + 1);
                            if (NULL != mod_func[i]->mod_info.name) {
                                strcpy(mod_func[i]->mod_info.name, mod_name);
                                mod_func[i]->mod_info.name[strlen(mod_name)] = '\0';
                            }
                            mod_func[i]->mod_info.version = (char*)aos_malloc(strlen(version) + 1);
                            if (NULL != mod_func[i]->mod_info.version) {
                                strcpy(mod_func[i]->mod_info.version, version);
                                mod_func[i]->mod_info.version[strlen(version)] = '\0';
                            }
                        }
                        break;
                    } else {
                        if (mod_func[i]->mod_info.mod == mod) { /* already exist */
                            break;
                        }
                    }
                }
            }
        }

        return uagent_request_service(mod, UAGENT_MOD_UAGENT, UAGENT_FUNC_UA_REGISTER, sizeof(uagent_exchange_service), &uagent_exchange_service);
    }
    return -1;
}

int uagent_unregister(const ua_mod_t mod, const ua_func_t func)
{
    if (UAGENT_INITED_FINISH) {
        uagent_exchange_service_t uagent_exchange_service = { func, "", NULL, NULL };
        return uagent_request_service(mod, UAGENT_MOD_UAGENT, UAGENT_FUNC_UA_UNREGISTER, sizeof(uagent_exchange_service), &uagent_exchange_service);
    }
    return -1;
}

int uagent_request_service(const ua_mod_t src, const ua_mod_t target,
                           const ua_func_t func, const unsigned short len,
                           const void *data)
{
    int rc = -1;
    if (UAGENT_INITED_FINISH) {
        /* make uagent info */
        char tmp_buf[UAGENT_PAYLOAD_CMD_SIZE];
        uagent_exchange_header_t* p = (uagent_exchange_header_t*)tmp_buf;
        p->src = src;
        p->mod = target;
        p->id = UAGENT_MSG_IDX_INVALID;
        p->func = func;
        p->len = len < UAGENT_PAYLOAD_CMD_SIZE - sizeof(uagent_exchange_header_t) - 1 ? len : UAGENT_PAYLOAD_CMD_SIZE - sizeof(uagent_exchange_header_t) - 1;

        memcpy(&tmp_buf[sizeof(uagent_exchange_header_t)], data, p->len);
        if (UAGENT_MOD_UAGENT != target && UAGENT_FUNC_UA_RQST_CONN != func) {
            UAGENT_INFO("[uA]call %s src 0x%x func 0x%x len %d\n", __func__, p->src, p->func, p->len);
        }
        tmp_buf[p->len + sizeof(uagent_exchange_header_t)] = 0;
        rc = aos_queue_send(&uagent_queue, p, p->len + sizeof(uagent_exchange_header_t) + 1);
    }

    return rc;
}

/**
 * update or create uagent service node
 *
 *
 * @param[in]  mod     Module type, reference @ua_mod_t
 * @param[in]  func    Service Name
 * @param[in]  service Service routine
 * @param[in]  add     add(update) or remove
 *
 * @return >=0: Success, -EINVAL: invalid parameter, -ENOMEN: no enough to malloc -EIO: Table is full
 *
 */
static int update_create_uagent_service_node(const ua_mod_t mod,
    const ua_func_t func,
    on_customer_service service,
    void *arg,
    const char add)
{
    int rc = -EIO;
    unsigned char i = 0;
    while (i < UAGENT_SERVICE_SIZE) {
        if (uagent_service_tb[i] != NULL) {
            if (uagent_service_tb[i]->mod == mod && uagent_service_tb[i]->func == func) {
                break;
            }
        }
        i++;
    }
    UAGENT_INFO("[uA]Register Service mod 0x%x func 0x%x\n", mod, func);
    if (UAGENT_SERVICE_SIZE == i) { /* no found it */
        if (add != 0) { /* create it */
            i = 0;
            while (i < UAGENT_SERVICE_SIZE) {
                if (uagent_service_tb[i] == NULL) {
                    break;
                }
                i++;
            }
            if (i < UAGENT_SERVICE_SIZE) {
                uagent_service_tb[i] = (uagent_service_node*)aos_malloc(sizeof(uagent_service_node));
                if (uagent_service_tb[i] == NULL) {
                    rc = -ENOMEM;
                } else {
                    uagent_service_tb[i]->mod = mod;
                    uagent_service_tb[i]->func = func;
                    uagent_service_tb[i]->service = service;
                    uagent_service_tb[i]->arg = arg;
                    rc = i;
                }
            } else {
                rc = -EIO;
            }
        } else {
            rc = 0;
        }
    } else { /* found it in list, */
        if (add != 0) { /* update call back */
            uagent_service_tb[i]->service = service;
            uagent_service_tb[i]->arg = arg;
            rc = i;
        } else {
            aos_free(uagent_service_tb[i]);
            uagent_service_tb[i] = NULL;
            rc = i;
        }
    }
    return rc;
}

static void update_func_list(const ua_mod_t mod, const ua_func_t func, char *func_name, const char add)
{
    unsigned char i = 0;
    while (mod_func[i] != NULL) {
        if (mod_func[i]->mod_info.mod == mod) {
            if (add) {
                add_list(mod_func[i], func, func_name);
            } else {
                del_list(mod_func[i], func);
            }
        }
        i++;
    }
}

static int ext_request_service(const ua_mod_t src,
    const ua_mod_t target,
    const ua_func_t func,
    const unsigned long id,
    const unsigned short len,
    const void *data)
{
    int rc = -1;
    if (UAGENT_INITED_FINISH) {
        /* make uagent info */
        char tmp_buf[UAGENT_PAYLOAD_CMD_SIZE];
        uagent_exchange_header_t* p = (uagent_exchange_header_t*)tmp_buf;
        p->src = src;
        p->mod = target;
        p->id = id;
        p->func = func;
        p->len = len < UAGENT_PAYLOAD_CMD_SIZE - sizeof(uagent_exchange_header_t) - 1 ? len : UAGENT_PAYLOAD_CMD_SIZE - sizeof(uagent_exchange_header_t) - 1;

        memcpy(&tmp_buf[sizeof(uagent_exchange_header_t)], data, p->len);
        if (UAGENT_MOD_UAGENT != target && UAGENT_FUNC_UA_RQST_CONN != func) {
            UAGENT_DEBUG("[uA]%s src 0x%x func 0x%x len %d\n", __func__, p->src, p->func, p->len);
        }
        tmp_buf[p->len + sizeof(uagent_exchange_header_t)] = 0;
        rc = aos_queue_send(&uagent_queue, p, p->len + sizeof(uagent_exchange_header_t) + 1);
    }

    return rc;
}

static void uagent_routine(void *para)
{
    char tmp_buf[UAGENT_PAYLOAD_CMD_SIZE];
    unsigned int rcv_size = 0;

    while (true) {
        if (0 == aos_queue_recv(&uagent_queue, AOS_WAIT_FOREVER, tmp_buf, &rcv_size)) {
            if (rcv_size >= sizeof(uagent_exchange_header_t)) {
                const uagent_exchange_header_t* p = (uagent_exchange_header_t*)tmp_buf;

                /* handle on pre-define service by UAGENT self */
                if (UAGENT_MOD_UAGENT == p->mod) {

                    int rc = -1;
                    switch (p->func)
                    {
                    case UAGENT_FUNC_UA_REGISTER:
                        if (sizeof(uagent_exchange_service_t) == p->len) {
                            uagent_exchange_service_t* service_info =
                                (uagent_exchange_service_t*)&tmp_buf[sizeof(uagent_exchange_header_t)];
                            if (0 > (rc = update_create_uagent_service_node(p->src,
                                service_info->func, service_info->service, service_info->arg, UAGENT_REGISTER))) {
                                UAGENT_ERR("[uA]register mod 0x%x, func str %s fail %d\n",
                                    p->src, service_info->func_name, rc);
                            } else {
                                update_func_list(p->src, service_info->func, service_info->func_name, UAGENT_FUNC_UA_REGISTER);
                            }
                        }
                        break;

                    case UAGENT_FUNC_UA_UNREGISTER:
                        if (sizeof(uagent_exchange_service_t) == p->len) {
                            uagent_exchange_service_t* service_info =
                                (uagent_exchange_service_t*)&tmp_buf[sizeof(uagent_exchange_header_t)];
                            if (0 > (rc = update_create_uagent_service_node(p->src,
                                service_info->func, NULL, NULL, UAGENT_UNREGISTER))) {
                                UAGENT_ERR("[uA]un-register mod 0x%x, func %d fail %d\n",
                                    p->src, service_info->func, rc);
                            } else {
                                update_func_list(p->src, service_info->func, service_info->func_name, UAGENT_FUNC_UA_UNREGISTER);
                            }
                        }
                        break;

                    case UAGENT_FUNC_UA_RQST_CONN:
                        service_check_connect();
                        break;

                    case UAGENT_FUNC_UA_SHOW_LIST:
                        UAGENT_DEBUG("[uA]call %s get request show list plen %d", __func__, p->len);
                        if (p->len > 0) {
                            if (strncmp(&tmp_buf[sizeof(uagent_exchange_header_t)], "?", 2) == 0) {
                                service_report_func_list();
                            } else {
                                UAGENT_ERR("[uA]call %s illegal cmd str %s\n",
                                    __func__, &tmp_buf[sizeof(uagent_exchange_header_t)]);
                            }
                        }

                        break;

                    case UAGENT_FUNC_UA_DUMP_DELAY_SEND:
                        uagent_send(UAGENT_MOD_CLI, UAGENT_FUNC_USER_BASE, 0, NULL, send_policy_delay_dump | send_policy_delay);
                        break;

                    case UAGENT_FUNC_UA_DUMP_DELAY_SEND_PARAM:
                        service_update_delay_send_param(p->len,
                            &tmp_buf[sizeof(uagent_exchange_header_t)]);
                        break;
                    default:
                        break;
                    }

                } else {
                    unsigned char i = 0;
                    while (i < UAGENT_SERVICE_SIZE) {
                        if (NULL != uagent_service_tb[i]) {
                            /* no break for broadcast function */
                            if (p->mod == UAGENT_MOD_BROAD_CAST) {
                                if (p->func == uagent_service_tb[i]->func) {
                                    UAGENT_DEBUG("handler mod 0x%x, func 0x%x, str %s @ %d\n",
                                        p->mod, p->func, &tmp_buf[sizeof(uagent_exchange_header_t)], i);
                                    if (NULL != uagent_service_tb[i]->service) {
                                        uagent_service_tb[i]->service(uagent_service_tb[i]->arg,
                                            p->len,
                                            &tmp_buf[sizeof(uagent_exchange_header_t)]);
                                    }
                                }
                            } else if ((p->mod == uagent_service_tb[i]->mod) &&
                                (p->func == uagent_service_tb[i]->func)) {

                                UAGENT_DEBUG("handler mod 0x%x, func 0x%x, str %s @ %d\n",
                                    p->mod, p->func, &tmp_buf[sizeof(uagent_exchange_header_t)], i);
                                if (NULL != uagent_service_tb[i]->service) {
                                    uagent_service_tb[i]->service(uagent_service_tb[i]->arg,
                                        p->len,
                                        &tmp_buf[sizeof(uagent_exchange_header_t)]);
                                    if (UAGENT_MSG_IDX_INVALID != p->id) {
                                        uagent_ack(SERVICE_AGENT_OK, p->id, p->mod, p->func, NULL);
                                    }
                                } else {
                                    if (UAGENT_MSG_IDX_INVALID != p->id) {
                                        uagent_ack(SERVICE_AGENT_FAIL, p->id, p->mod, p->func, "no service");
                                    }
                                    UAGENT_ERR("[uA]found mod 0x%x func 0x%x but service is null\n", p->mod, p->func);
                                }
                                break;
                            }
                        }
                        i++;
                    }
                    if (p->mod != UAGENT_MOD_BROAD_CAST && (UAGENT_SERVICE_SIZE == i)) {
                        UAGENT_ERR("[uA]no found mod 0x%x func 0x%x id %ld\n", p->mod, p->func, p->id);
                        if (UAGENT_MSG_IDX_INVALID != p->id) {
                            uagent_ack(SERVICE_AGENT_FAIL, p->id, p->mod, p->func, "no service");
                        }
                    }
                }
            }
        } else {
            UAGENT_ERR("[uA]uagent routine recv msg fail\n");
        }
    }
}

static int uagent_on_recv_handler(const unsigned short len, const char *str)
{
    int rc = -1;
    cJSON *root = NULL;
    root = cJSON_Parse(str);
    if (NULL != root) {
        cJSON *msgid = cJSON_GetObjectItem(root, "id");
        /* even no id can call service, w/o ack */
        unsigned long id = UAGENT_MSG_IDX_INVALID;
        if (NULL != msgid && cJSON_IsNumber(msgid)) {
            id = msgid->valueint;
        } else {
            UAGENT_ERR("[uA]call %s no id in payload\n", __func__);
        }
        cJSON *params = cJSON_GetObjectItem(root, "params");
        if (params != NULL) {
            cJSON *mod = cJSON_GetObjectItem(params, "mod");
            cJSON *func = cJSON_GetObjectItem(params, "func");

            if (NULL != mod && NULL != func) {
                if (NULL != mod && NULL != func) {
                    cJSON *param = cJSON_GetObjectItem(params, "param");
                    if (NULL != param) {
                        if (cJSON_IsObject(param)) {/* param is json object */
                            UAGENT_DEBUG("Json object\n");
                            char* value_str = cJSON_PrintUnformatted(param);
                            if (NULL != value_str) {
                                rc = 0;
                                ext_request_service(UAGENT_MOD_CLOUD,
                                    mod->valueint,
                                    func->valueint,
                                    id,
                                    strlen(value_str),
                                    value_str);
                                cJSON_free(value_str);
                                value_str = NULL;
                            }
                        } else if (cJSON_IsString(param)) {/* param is string */
                            UAGENT_DEBUG("String %s\n", param->valuestring);

                            rc = 0;
                            ext_request_service(UAGENT_MOD_CLOUD,
                                mod->valueint,
                                func->valueint,
                                id,
                                strlen(param->valuestring),
                                param->valuestring);
                        } else {
                            UAGENT_ERR("[uA]Not support format\n");
                        }
                    } else {
                        UAGENT_ERR("[uA]missing param\n");
                    }

                } else {
                    UAGENT_ERR("[uA]LLEGAL FORMAT\n");
                }
            } else {
                /* print topic name and topic message */
                char* json_str = cJSON_Print(params);

                if (NULL != json_str) {
                    UAGENT_ERR("[uA]LLEGAL FORMAT %s\n", json_str);
                    cJSON_free(json_str);
                    json_str = NULL;
                }
            }

        }
        cJSON_Delete(root);
        root = NULL;
    } else {
        UAGENT_ERR("[uA]ILLEGAL FORMAT, Not Json Object\n");
    }
    return rc;
}

static void service_check_connect()
{
    if (uagent_service_attr.cloud_connect != check_connect()) {
        UAGENT_INFO("[uA]cloud connect change %d->%d\n",
            uagent_service_attr.cloud_connect, check_connect());
        uagent_service_attr.cloud_connect = check_connect();
        /* make connect string */
        char buf[32];
        snprintf(buf, 32, UAGENT_NOTICE_CONNECT, uagent_service_attr.cloud_connect);
        uagent_request_service(UAGENT_MOD_UAGENT,
            UAGENT_MOD_BROAD_CAST,
            UAGENT_FUNC_NOTIFY_CLOUD_CONN,
            strlen(buf) + 1,
            buf);
        if (uagent_service_attr.cloud_connect) {
            service_report_func_list();
        }
    }
}

static void service_report_func_list()
{
    unsigned char mod_idx = 0;
    cJSON *root = NULL;
    cJSON *mod_obj[UAGENT_MOD_SIZE];
    root = cJSON_CreateObject();

    char init_mod_report = 0;
    while (NULL != mod_func[mod_idx]) {
        init_mod_report = 0;
        UAGENT_INFO("[uA]mod %s have func %d\n", mod_func[mod_idx]->mod_info.name, mod_func[mod_idx]->mod_info.func_count);
        uagent_func_node_t *p = mod_func[mod_idx]->header;
        cJSON *arr = NULL, *json = NULL;
        while (NULL != p) {
            if (p->func >= UAGENT_FUNC_USER_BASE) {
                UAGENT_INFO("[uA]Summit func %d name %s\n", p->func, p->func_name);
                if (0 == init_mod_report) {
                    init_mod_report = 1;
                    cJSON_AddItemToObject(root, mod_func[mod_idx]->mod_info.name, mod_obj[mod_idx] = cJSON_CreateObject());
                    cJSON_AddNumberToObject(mod_obj[mod_idx], "module", mod_func[mod_idx]->mod_info.mod);
                    cJSON_AddStringToObject(mod_obj[mod_idx], "version", mod_func[mod_idx]->mod_info.version);
                    cJSON_AddItemToObject(mod_obj[mod_idx], "funclist", arr = cJSON_CreateArray());
                }
                json = cJSON_CreateObject();

                cJSON_AddNumberToObject(json, "func", p->func);
                cJSON_AddStringToObject(json, "funcname", p->func_name);
                cJSON_AddItemToArray(arr, json);

            } else {
                UAGENT_DEBUG("[uA]call %s Not summit func %d name %s\n", __func__, p->func, p->func_name);
            }
            p = p->next;
        }

        mod_idx++;
    }

    char *out = cJSON_PrintUnformatted(root);
    uagent_send(UAGENT_MOD_UAGENT, UAGENT_FUNC_UA_SHOW_LIST,
        strlen(out) + 1,
        out,
        send_policy_trans_guarantee | send_policy_object);
    cJSON_free(out);
    cJSON_Delete(root);
}

static void service_update_delay_send_param(const unsigned short len, void *str)
{
    if (len > 0 && NULL != str) {
        char* str_value = NULL;
        if (NULL != (str_value = strstr(str, "cli cloud timeout="))) {
            UAGENT_DEBUG("call %s str value %s\n", __func__, str_value);
            const int timeout = strtoul(&str_value[strlen("cli cloud timeout=")], NULL, 10);
            if (timeout >= 50 && timeout <= 5000) {
                UAGENT_DEBUG("call %s delay timeout %d\n", __func__, timeout);
                aos_timer_change(&timer_monitor_output_queue, timeout);
            }
        }
    }
}

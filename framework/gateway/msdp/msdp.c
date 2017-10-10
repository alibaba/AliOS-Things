/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "aos/aos.h"
#include "msdp.h"
#include "service.h"
#include "alink_export_internal.h"
#include "devmgr.h"
#include "service_manager.h"
#include "connectivity_manager.h"
#include "msdp.h"

#define MODULE_NAME MODULE_NAME_MSDP

static int module_inited = 0;
static dlist_t g_helper_head;
static dlist_t g_task_head;
static void *g_task_mutex = NULL;

int register_remote_service(char *uuid, const char *service[]);
static void msdp_post_work_handler(void *work);

static void *post_device_status_work = NULL;

static int __convert_cmd_name2id(const char *cmd_name, uint8_t *cmd_id)
{
    if (strcmp(cmd_name, METHOD_GET_DEVICE_STATUS) == 0) {
        *cmd_id = ACB_GET_DEVICE_STATUS;
    } else if (strcmp(cmd_name, METHOD_SET_DEVICE_STATUS) == 0) {
        *cmd_id = ACB_SET_DEVICE_STATUS;
    } else if (strcmp(cmd_name, METHOD_SET_DEVICE_STATUS_ARRAY) == 0) {
        *cmd_id = ACB_SET_DEVICE_STATUS_ARRAY;
    } else if (strcmp(cmd_name, METHOD_REQUEST_REMOTE_SERVICE) == 0) {
        *cmd_id = ACB_REQUEST_REMOTE_SERVICE;
    } else {
        return SERVICE_RESULT_ERR;
    }

    return SERVICE_RESULT_OK;
}


int __ptr_array_to_json_array(char **ppservice, char *json_buff, int buff_size)
{
    int i = 0;
    if (buff_size < 2) {
        return SERVICE_RESULT_ERR;
    }


    int len = snprintf(json_buff, buff_size, "[");
    while (*(ppservice + i) != NULL) {
        if ( i == 0) {
            len += snprintf(json_buff + len, buff_size - len, "\"%s\"", *(ppservice + i));
        } else {
            len += snprintf(json_buff + len, buff_size - len, ",\"%s\"", *(ppservice + i));
        }
        i++;
    }
    len += snprintf(json_buff + len, buff_size - len, "]");

    if (len == buff_size) {
        return SERVICE_RESULT_ERR;
    }

    json_buff[len] = '\0';

    return SERVICE_RESULT_OK;
}


static void __dump_device_callback(const device_helper_ptr helper)
{
    int i;
    attr_hanler_ptr attr_handler = NULL;
    service_handler_ptr service_handler = NULL;

    os_printf("devtype:%02x helper:\n", helper->dev_type);

    for (i = 0; i < MAX_CALLBACK_NUM; i++) {
        if (helper->option_callback[i] == NULL) {
            continue;
        }
        os_printf("\toption_callback[%d]:\"0x%08x\"\n", i,
                  (unsigned long)helper->option_callback[i]);
    }

    os_printf("\tattr_handler:\n");
    dlist_for_each_entry(&(helper->attr_head), attr_handler, attr_hanler_t,
                         list_node) {
        os_printf("\t\tname:%-16s, get_cb:\"0x%08x\", set_cb:\"0x%08x\"\n",
                  attr_handler->name,
                  (unsigned long)attr_handler->get_cb, (unsigned long)attr_handler->set_cb);
    }

    os_printf("\tservice_handler:\n");
    dlist_for_each_entry(&(helper->srv_head), service_handler, service_handler_t,
                         list_node) {
        os_printf("\t\tname:%-16s, service_cb:\"0x%08x\"\n",
                  service_handler->name, (unsigned long)service_handler->service_cb);
    }
}


/*设备属性和服务节点释放*/
static void __release_device_helper(const device_helper_ptr helper)
{
    attr_hanler_ptr attr;
    dlist_t *attr_next = NULL;
    service_handler_ptr service;
    dlist_t *service_next = NULL;

    dlist_for_each_entry_safe(&(helper->attr_head), attr_next, attr, attr_hanler_t,
                              list_node) {
        dlist_del(&attr->list_node);
        msdp_free_buff((void *)attr);
    }

    dlist_for_each_entry_safe(&(helper->srv_head), service_next, service,
                              service_handler_t, list_node) {
        dlist_del(&service->list_node);
        msdp_free_buff((void *)service);
    }

    os_mutex_destroy(helper->mutex_lock);
}


static void __dump_all_type_handler()
{
    if (!dlist_empty(&g_helper_head)) {
        device_helper_ptr helper;
        dlist_for_each_entry(&g_helper_head, helper, device_helper_t, list_node) {
            __dump_device_callback(helper);
        }
    }
}


static int __module_init_check()
{
    /*msdp初始化检查*/
    if (!module_inited && SERVICE_RESULT_ERR == msdp_init()) {
        log_error("msdp init fail");
        return SERVICE_RESULT_ERR;
    }

    return SERVICE_RESULT_OK;
}

static void __free_helper_list()
{
    /*设备model链表释放*/
    if (!dlist_empty(&g_helper_head)) {
        device_helper_ptr helper;
        dlist_t *helper_next;

        /*设备属性和服务链表释放*/
        dlist_for_each_entry_safe(&g_helper_head, helper_next, helper, device_helper_t,
                                  list_node) {
            __release_device_helper(helper);
            dlist_del(&helper->list_node);
            msdp_free_buff((void *)helper);
        }
    }
}


static void __free_task_list(void)
{
    post_node_t *node;
    dlist_t *next;

    os_mutex_lock(g_task_mutex);
    dlist_for_each_entry_safe(&g_task_head, next, node, post_node_t, list_node) {
        dlist_del(&node->list_node);
        if (node->params) {
            msdp_free_buff(node->params);
        }
        msdp_free_buff(node);
    }
    os_mutex_unlock(g_task_mutex);

    return;
}

static void msdp_post_work_handler(void *work)
{
    int ret = SERVICE_RESULT_ERR;
    post_node_t *node;
    dlist_t *next;

    os_mutex_lock(g_task_mutex);
    dlist_for_each_entry_safe(&g_task_head, next, node, post_node_t, list_node) {
        os_mutex_unlock(g_task_mutex);
        if (node->params) {
            ret = msdp_post_device_data_array(node->params);
            RET_LOG(ret, "post device data fail, params:%s", node->params);
        }

        os_mutex_lock(g_task_mutex);

        dlist_del(&node->list_node);
        if (node->params) {
            msdp_free_buff(node->params);
        }
        msdp_free_buff(node);
    }
    os_mutex_unlock(g_task_mutex);

    return;
}


int msdp_add_asyncpost_task(char *params)
{
    post_node_t *node = msdp_new_buff(sizeof(post_node_t));
    if (NULL == node) {
        return SERVICE_RESULT_ERR;
    }

    node->params = params;
    os_mutex_lock(g_task_mutex);
    dlist_add_tail(&node->list_node, &g_task_head);
    os_mutex_unlock(g_task_mutex);

    post_device_status_work = aos_loop_schedule_work(0, msdp_post_work_handler, NULL,
                                                     NULL, NULL);

    return SERVICE_RESULT_OK;
}

void *msdp_dup_string(const char *src)
{
    char *dst;
    int n ;

    if (!src) {
        return NULL;
    }

    n = strlen(src);
    dst = (char *)os_malloc(n + 1);
    if (!dst) {
        return NULL;
    }

    memcpy(dst, src, n);
    dst[n] = '\0';

    return dst;
}

void *msdp_dup_buff(const void *buff, unsigned buff_size)
{
    char *dst;
    int n ;

    if (!buff) {
        return NULL;
    }

    dst = (char *)os_malloc(buff_size);
    if (!dst) {
        return NULL;
    }

    memcpy(dst, buff, buff_size);

    return dst;
}

void *msdp_new_buff(unsigned int buff_size)
{
    void *buff = os_malloc(buff_size);
    if (NULL != buff) {
        memset(buff, 0, buff_size);
    }

    return buff;
}

void msdp_free_buff(void *buff)
{
    if (buff) {
        os_free(buff);
    }
}


static int msdp_listener(int type, void *data, int dlen, void *result,
                         int *rlen)
{
    if (type == SERVICE_EVENT) {
        int st = *((int *)data);
        if (st == SERVICE_STATE_READY) {
            register_remote_service(config_get_main_uuid(), NULL);
        } else if (st == SERVICE_STATE_STOP) {
        }
    } else if (type == SERVICE_DATA) {
        alink_data_t *p = (alink_data_t *)data;

        uint8_t cmd_id = 0;
        int ret = __convert_cmd_name2id(p->method, &cmd_id);
        if (ret != SERVICE_RESULT_OK) {
            log_info("unknonw mehtod:%s", p->method);
            return EVENT_IGNORE;
        }

        const char *uuid_ptr = NULL;
        char uuid[MAX_UUID_LEN] = {0};
        if (ACB_SET_DEVICE_STATUS_ARRAY == cmd_id) {
            strncpy(uuid, uuid_ptr = config_get_main_uuid(), sizeof(uuid) - 1);
        } else {
            int len = 0;
            uuid_ptr = json_get_value_by_name(p->data, strlen(p->data), JSON_KEY_UUID, &len,
                                              NULL);
            if (NULL == uuid_ptr) {
                log_error("invalid parmas format");
                return EVENT_IGNORE;
            }
            strncpy(uuid, uuid_ptr, sizeof(uuid) > len ? len : sizeof(uuid) - 1);
        }

        msdp_dispatch_event(uuid, p->method, p->data);

        return EVENT_CONSUMED;
    }

    return EVENT_IGNORE;
}

static int msdp_add_attr_cb(device_helper_ptr helper,
                            const char *name,
                            void *pf_get,
                            void *pf_set)
{
    int ret = SERVICE_RESULT_OK;
    attr_hanler_ptr phandler = NULL;

    os_mutex_lock(helper->mutex_lock);
    if (msdp_get_attr_handler(helper, name)) {
        log_error("%s attribute already registered on devtype:%02x", name,
                  helper->dev_type);

        goto unlock_end;
    }

    phandler = msdp_new_buff(sizeof(attr_hanler_t));
    if (NULL == phandler) {
        ret = SERVICE_RESULT_ERR;
        log_error("pmalloc failed");
        goto unlock_end;
    }

    memset(phandler, 0, sizeof(attr_hanler_t));
    strncpy(phandler->name, name, sizeof(phandler->name) - 1);
    phandler->get_cb = pf_get;
    phandler->set_cb = pf_set;

    dlist_add_tail(&phandler->list_node, &(helper->attr_head));

unlock_end:
    os_mutex_unlock(helper->mutex_lock);

    return ret;
}


static void msdp_delete_attr_cb(device_helper_ptr helper,
                                const char *name)
{
    attr_hanler_ptr phandler = NULL;

    os_mutex_lock(helper->mutex_lock);
    phandler = msdp_get_attr_handler(helper, name);
    if (phandler) {
        dlist_del(&phandler->list_node);
        msdp_free_buff((void *)phandler);
    }
    os_mutex_unlock(helper->mutex_lock);

    return;
}


static int msdp_add_service_cb(device_helper_ptr helper,
                               const char *name, ALINK_SERVICE_EXECUTE_CB pf_exec)
{
    int ret = SERVICE_RESULT_OK;
    service_handler_ptr phandler = NULL;

    os_mutex_lock(helper->mutex_lock);

    if (msdp_get_service_handler(helper, name)) {
        goto unlock_end;
    }

    phandler = msdp_new_buff(sizeof(service_handler_t));
    if (NULL == phandler) {
        log_error("pmalloc failed");
        ret = SERVICE_RESULT_ERR;
        goto unlock_end;
    }

    memset(phandler, 0, sizeof(service_handler_t));
    strncpy(phandler->name, name, sizeof(phandler->name) - 1);
    phandler->service_cb = pf_exec;

    dlist_add_tail(&phandler->list_node, &(helper->srv_head));

unlock_end:
    os_mutex_unlock(helper->mutex_lock);

    return ret;
}

static void msdp_delete_service_cb(device_helper_ptr helper, const char *name)
{
    service_handler_ptr phandler = NULL;

    os_mutex_lock(helper->mutex_lock);

    phandler = msdp_get_service_handler(helper, name);
    if (phandler) {
        dlist_del(&phandler->list_node);
        msdp_free_buff((void *)phandler);
    } else {
        log_error("no exist %s service handler on devtype:%02x", name,
                  helper->dev_type);
    }
    os_mutex_unlock(helper->mutex_lock);

    return;
}


static device_helper_ptr msdp_get_helper_by_devtype(uint8_t dev_type)
{
    if (!dlist_empty(&g_helper_head)) {
        device_helper_ptr helper;

        dlist_for_each_entry(&g_helper_head, helper, device_helper_t, list_node) {
            if (helper->dev_type == dev_type) {
                return helper;
            }
        }
    }

    return NULL;
}


attr_hanler_ptr msdp_get_attr_handler(const device_helper_ptr helper,
                                      const char *name)
{
    attr_hanler_ptr phandler = NULL;
    dlist_for_each_entry(&(helper->attr_head), phandler, attr_hanler_t, list_node) {
        if (strcmp(name , phandler->name) == 0) {
            return phandler;
        }
    }

    return NULL;
}


service_handler_ptr msdp_get_service_handler(const device_helper_ptr helper,
                                             const char *name)
{
    service_handler_ptr phandler = NULL;

    dlist_for_each_entry(&(helper->srv_head), phandler, service_handler_t,
                         list_node) {
        if (strcmp(name , phandler->name) == 0) {
            return phandler;
        }
    }

    return NULL;
}


device_helper_ptr msdp_get_helper(const char *devid_or_uuid)
{
    int ret = -1;
    uint8_t dev_type = DEV_TYPE_GATEWAY;
    device_helper_ptr helper = NULL;

    if (strlen(devid_or_uuid) == 0 ||
        strcmp(devid_or_uuid, (char *)config_get_main_uuid()) == 0) {
        dev_type = DEV_TYPE_GATEWAY;
    } else {
        ret = devmgr_get_dev_type(devid_or_uuid, &dev_type);
        RET_GOTO(ret, out, CALL_FUCTION_FAILED, "devmgr_get_dev_type");
    }

    helper = msdp_get_helper_by_devtype(dev_type);

out:
    return helper;
}

int lmns_post_device_data(const char *devid, char *payload)
{
    //deviceDataChange
    log_trace("device status change, devid:%s, params:%s", devid, payload);
    return 0;
}

int lmns_post_remote_service_rsp(const char *devid, char *payload)
{
    //remoteServiceResponse
    log_trace("remote service response, devid:%s, params:%s", devid, payload);
    return 0;
}

int msdp_post_remote_service_rsp(char *params)
{
    int ret = SERVICE_RESULT_ERR;
    log_trace("params = %s\n", params);

    ret = alink_post(METHOD_POST_REMOTE_SERVICE_RSP, params);
    RET_RETURN(ret, CALL_FUCTION_FAILED, "alink_post");

    return ret;
}


int msdp_post_device_data(char *params)
{
    int ret = SERVICE_RESULT_ERR;
    log_trace("params = %s\n", params);

    ret = alink_post(METHOD_POST_DEVICE_DATA, params);
    RET_RETURN(ret, CALL_FUCTION_FAILED, "alink_post");

    return ret;
}


int msdp_post_device_data_array(char *params)
{
    int ret = SERVICE_RESULT_ERR;

    ret = alink_post(METHOD_POST_DEVICE_DATA_ARRAY, params);
    RET_RETURN(ret, CALL_FUCTION_FAILED, "alink_post");

    return ret;
}



/*获取指定设备所有属性*/
#define GATEWAY_ATTR_SET "[\"NetworkInformation\",\"ArmMode\",\"ArmEntryDelay\",\"ArmExitDelay\",\"DeviceWhiteList\"]"
int msdp_get_all_attrname(const char *uuid, char *attrset, int buff_size)
{
    int count = 0;
    int ret = SERVICE_RESULT_ERR;
    device_helper_ptr helper = NULL;
    attr_hanler_ptr phandler = NULL;

    int len = 0;
    memset(attrset, 0, buff_size);
    attrset[len++] = '[';
    /*获取所有属性值*/
    helper = msdp_get_helper(uuid);
    if (helper) {
        os_mutex_lock(helper->mutex_lock);
        dlist_for_each_entry(&(helper->attr_head), phandler, attr_hanler_t, list_node) {
            if (count++ != 0) {
                attrset[len++] = ',';
            }
            len += snprintf(attrset + len, buff_size - len, "\"%s\"", phandler->name);
        }
        os_mutex_unlock(helper->mutex_lock);
    }
    len += snprintf(attrset + len, buff_size - len, "]");

    if (count == 0 || len == buff_size) {
        return SERVICE_RESULT_ERR;
    }
    strncpy(attrset, GATEWAY_ATTR_SET, buff_size);

    return SERVICE_RESULT_OK;
}


int msdp_get_all_rpcname(uint8_t dev_type, char *name_array[], int array_size)
{
    service_handler_ptr phandler = NULL;
    device_helper_ptr helper = NULL;
    int num = 0;

    helper = msdp_get_helper_by_devtype(dev_type);
    PTR_RETURN(helper, num, "no exist device helper, type = %d", dev_type);

    os_mutex_lock(helper->mutex_lock);
    dlist_for_each_entry(&(helper->srv_head), phandler, service_handler_t,
                         list_node) {
        name_array[num++] = phandler->name;
        if (num == array_size) {
            break;
        }
    }
    os_mutex_unlock(helper->mutex_lock);
    log_trace("service count:%d", num);

    return num;
}

int msdp_register_remote_service(char *uuid, const char *service)
{
    int ret = SERVICE_RESULT_ERR;
    char *req_str = NULL;
    int buff_size = 512;

    req_str = os_malloc(buff_size);
    OS_ASSERT(req_str, "malloc fail");
    memset(req_str, 0x00, buff_size);
    snprintf(req_str, buff_size - 1, REGISTER_SERVICES_STRING_FMT, uuid, service);

    alink_data_t data = {METHOD_REGISTER_REMOTE_SERVICE, req_str};
    log_debug("send:%s", req_str);
    ret = ((service_t *)sm_get_service("accs"))->put((void *)&data,
                                                     sizeof(data)); //accs_put

    if (req_str) {
        os_free(req_str);
    }

    return ret;

}

//service为NULL时注册所有service
int register_remote_service(char *uuid, const char *service[])
{
    int count, ret = SERVICE_RESULT_ERR;
    char **service_name = NULL;
    char **ppservice = NULL;
    char *json_param = NULL;
    char *service_array = NULL;
    int buff_size = 512;

    if (NULL == service || NULL == service[0] || strlen(service[0]) == 0) {
        service_name = (char **)os_malloc(sizeof(char *) * MAX_SERVICE_COUNT);
        OS_ASSERT(service_name, "malloc fail");
        memset((void *)service_name, 0, sizeof(char *) * MAX_SERVICE_COUNT);

        count = msdp_get_all_rpcname(DEV_TYPE_GATEWAY, service_name,
                                     MAX_SERVICE_COUNT - 1);
        if (0 == count) {
            log_info("service count = %d, nothing todo\n", count);
            ret = SERVICE_RESULT_OK;
            goto out;
        }

        ppservice = (char **)service_name;
    } else {
        ppservice = (char **)service;
    }

    service_array = os_malloc(buff_size);
    OS_ASSERT(service_array, "malloc fail");

    memset(service_array, 0, buff_size);
    ret = __ptr_array_to_json_array(ppservice, service_array, buff_size);
    RET_GOTO(ret, out, "ptr array to json array fail");

    json_param = os_malloc(buff_size);
    OS_ASSERT(json_param, "malloc fail");
    memset(json_param, 0, buff_size);
    snprintf(json_param, buff_size - 1, REGISTER_SERVICES_STRING_FMT, uuid,
             service_array);

    alink_data_t data = {METHOD_REGISTER_REMOTE_SERVICE, json_param};
    log_debug("send:%s", json_param);
    ret = ((service_t *)sm_get_service("accs"))->put((void *)&data,
                                                     sizeof(data)); //accs_put

out:
    if (service_array) {
        os_free(service_array);
    }

    if (json_param) {
        os_free(json_param);
    }

    if (service_name) {
        os_free(service_name);
    }

    return ret;
}


int msdp_register_device_helper(uint8_t dev_type,
                                dev_option_cb callback[MAX_CALLBACK_NUM])
{
    int i, ret = SERVICE_RESULT_ERR;

    if (msdp_get_helper_by_devtype(dev_type)) {
        log_debug("exist device type %d", dev_type);
        return SERVICE_RESULT_OK;
    }

    device_helper_ptr helper = msdp_new_buff( sizeof(device_helper_t));
    PTR_RETURN(helper, ret, "pmalloc error");

    memset(helper, 0, sizeof(device_helper_t));
    helper->mutex_lock = os_mutex_init();
    dlist_init(&(helper->attr_head));
    dlist_init(&(helper->srv_head));
    helper->dev_type = dev_type;

    for (i = 0; i < MAX_CALLBACK_NUM; i++) {
        helper->option_callback[i] = callback[i];
    }

    dlist_add_tail(&helper->list_node, &g_helper_head);

    return SERVICE_RESULT_OK;
}


void msdp_unregister_device_helper(uint8_t dev_type)
{
    device_helper_ptr helper = NULL;

    helper = msdp_get_helper_by_devtype(dev_type);
    if (helper) {
        __release_device_helper(helper);

        dlist_del(&helper->list_node);
        msdp_free_buff((void *)helper);
    }

    return;
}



void msdp_dump_helper()
{
    if (!dlist_empty(&g_helper_head)) {
        device_helper_ptr sub_dev = NULL;

        /*子设备属性和服务节点*/
        dlist_for_each_entry(&g_helper_head, sub_dev, device_helper_t, list_node) {
            os_mutex_lock((sub_dev->mutex_lock));
            __dump_device_callback(sub_dev);
            os_mutex_unlock((sub_dev->mutex_lock));
        }
    }
}


int msdp_report_device_attrs(const char *uuid, const char *attrs[])
{
    int len = 0;
    int i = 0;
    int ret = SERVICE_RESULT_ERR;
    char params[512] = {0};
    char attr_set[256] = {0};

    if (NULL == uuid || strlen(uuid) == 0) {
        return ret;
    }

    attr_set[len++] = '[';
    while (attrs[i]) {
        if (attr_set[len - 1] == '[') {
            len += snprintf(attr_set + len, sizeof(attr_set) - len, "\"%s\"", attrs[i]);
        } else {
            len += snprintf(attr_set + len, sizeof(attr_set) - len, ",\"%s\"", attrs[i]);
        }
        i++;
    }
    len += snprintf(attr_set + len, sizeof(attr_set) - len, "]");
    if (len == sizeof(attr_set)) {
        return ret;
    }
    snprintf(params, sizeof(params) - 1, GET_DEVICE_ATTR_STRING_FMT, uuid,
             attr_set);

    return msdp_dispatch_event(uuid, METHOD_GET_DEVICE_STATUS, params);
}

int msdp_report_device_status(const char *uuid, const char *attr_name,
                              const char *attr_value)
{
    int buff_size, len, ret = SERVICE_RESULT_ERR;
    char *buff = NULL;

    /*msdp初始化检查*/
    ret = __module_init_check();
    RET_RETURN(ret, "msdp module init fail");

    buff_size = strlen(POST_ATTR_VALUE_STRING_FMT) + 64 + strlen(
                    attr_name) + strlen(attr_value);
    buff = msdp_new_buff( buff_size);
    PTR_RETURN(buff, SERVICE_RESULT_ERR, "pmalloc buff fail, size:%d", buff_size);
    memset(buff, 0, buff_size);

    /*fmt:{"value":"1","when":"1404443369"}}*/
    if (attr_value[0] != '{' && attr_value[0] != '[')
        snprintf(buff, buff_size, POST_ATTR_VALUE_STRING_FMT, uuid, attr_name,
                 attr_value, "");
    else
        snprintf(buff, buff_size, POST_ATTR_OBJECT_STRING_FMT, uuid, attr_name,
                 attr_value, "");

    len = strlen(buff);
    memmove(buff + 1, buff, len);
    buff[0] = '[';
    buff[len + 1] = ']';
    buff[len + 2] = '\0';

    log_trace("params = %s", buff);
    ret = msdp_add_asyncpost_task(buff);
    RET_GOTO(ret, out, "add async post task fail, params:%s", buff);
    buff = NULL;

out:
    if (buff) {
        msdp_free_buff(buff);
    }

    return ret;
}


int msdp_set_device_status(const char *uuid, const char *attr_name,
                           const char *attr_value)
{
    int ret = SERVICE_RESULT_ERR;
    char attr_set[64] = {0};
    char attr_buff[256] = {0};
    char params[256] = {0};
    int len = 0;

    if (NULL == uuid) {
        log_error("Invalid args: devid_or_uuid");
        return ret;
    }

    if (strlen(uuid) == 0) {
        uuid = config_get_main_uuid();
    }

    snprintf(attr_set, sizeof(attr_set) - 1, "[\"%s\"]", attr_name);
    if (attr_value[0] != '{' && attr_value[0] != '[') {
        len = snprintf(attr_buff, sizeof(attr_buff) - 1, ATTR_NAME_VALUE_STRING_FMT,
                       attr_name, attr_value, "");
    } else { /*value为json object*/
        len = snprintf(attr_buff, sizeof(attr_buff) - 1, ATTR_NAME_OBJECT_STRING_FMT,
                       attr_name, attr_value, "");
    }
    if (len >= sizeof(attr_buff) - 1) {
        return ret;
    }

    len = snprintf(params, sizeof(params) - 1, SET_DEVICE_ATTR_STRING_FMT, uuid,
                   attr_set, attr_buff);
    if (len >= sizeof(params) - 1) {
        return ret;
    }

    log_trace("params = %s", params);

    ret = msdp_dispatch_event(uuid, METHOD_SET_DEVICE_STATUS, params);
    RET_RETURN(ret, CALL_FUCTION_FAILED, "msdp_dispatch_event");

    return ret;
}

int msdp_request_remote_service(const char *uuid, const char *service_name,
                                const char *args)
{
    int ret = SERVICE_RESULT_ERR;
    char params[256] = {0};
    int len = 0;

    len = snprintf(params, sizeof(params) - 1, REQUEST_SERVICE_STRING_FMT, uuid,
                   service_name, args);
    if (len >= sizeof(params) - 1) {
        return ret;
    }

    log_trace("params = %s", params);

    ret = msdp_dispatch_event(uuid, METHOD_REQUEST_REMOTE_SERVICE, params);
    RET_RETURN(ret, CALL_FUCTION_FAILED, "msdp_dispatch_event");

    return ret;
}


int msdp_dispatch_event(const char *devid_or_uuid, const char *method,
                        char *params)
{
    int ret = SERVICE_RESULT_ERR;
    device_helper_ptr helper = NULL;
    uint8_t cmd_type = 0;

    log_trace("msdp dispatch event, uuid:%s, method:%s, parmas:%s", devid_or_uuid,
              method, params);

    /*msdp初始化检查*/
    ret = __module_init_check();
    RET_RETURN(ret, "msdp module init fail");

    ret = __convert_cmd_name2id(method, &cmd_type);
    RET_RETURN(ret, "covert to cmd id fail, method:%s", method);

    helper = msdp_get_helper(devid_or_uuid);
    if (helper) {
        if (NULL != helper->option_callback[(int)cmd_type]) {
            ret = helper->option_callback[(int)cmd_type](params);
        } else {
            log_error("cmd_callback is NULL, cmdtype = %d", cmd_type);
        }
    } else {
        log_debug("no exist model helper, uuid = %s", devid_or_uuid);
    }

    return ret;
}


int msdp_register_attr_cb(uint8_t dev_type,
                          const char *name,
                          void *pf_get,
                          void *pf_set)
{
    int ret = SERVICE_RESULT_ERR;
    device_helper_ptr helper = NULL;

    if (strlen(name) > MAX_ATTR_NAME_LEN - 1) {
        log_error("attrname too long\n");
        return ret;
    }

    /*msdp初始化检查*/
    ret = __module_init_check();
    RET_RETURN(ret, "msdp module init fail");

    helper = msdp_get_helper_by_devtype(dev_type);
    PTR_RETURN(helper, ret, "no exist device type: %d", dev_type);

    ret = msdp_add_attr_cb(helper, name, pf_get, pf_set);
    RET_RETURN(ret, "register %s attribute handler failed", name);
    log_trace("register %s attribute handler success on devtype:%02x", name,
              helper->dev_type);

    return SERVICE_RESULT_OK;
}


int msdp_register_service_cb(uint8_t dev_type,
                             const char *name,
                             void *pf_exec)
{
    int ret = SERVICE_RESULT_ERR;
    device_helper_ptr helper = NULL;

    if (strlen(name) > MAX_SERVICE_NAME_LEN - 1) {
        log_error("service name too long\n");
        return ret;
    }

    /*msdp初始化检查*/
    ret = __module_init_check();
    RET_RETURN(ret, "msdp module init fail");

    helper = msdp_get_helper_by_devtype(dev_type);
    PTR_RETURN(helper, ret, "no exist device type: %d", dev_type);

    ret = msdp_add_service_cb(helper, name, pf_exec);
    RET_RETURN(ret, "msdp_add_service_cb(%s)", name);

    log_trace("register %s service handler success", name);

    return SERVICE_RESULT_OK;
}

void msdp_unregister_service_cb(uint8_t dev_type, const char *name)
{
    device_helper_ptr helper = NULL;

    /*msdp初始化检查*/
    int ret = __module_init_check();
    if (SERVICE_RESULT_OK != ret) {
        log_error("msdp module init fail");
        return;
    }

    helper = msdp_get_helper_by_devtype(dev_type);
    if (NULL == helper) {
        log_debug("no exist device type %02x", dev_type);
        return;
    }
    msdp_delete_service_cb(helper, name);
    return;
}


int msdp_init()
{
    int ret = SERVICE_RESULT_ERR;

    if (module_inited) {
        return SERVICE_RESULT_OK;
    }

    dlist_init(&g_helper_head);
    dlist_init(&g_task_head);

    g_task_mutex = os_mutex_init();
    PTR_RETURN(g_task_mutex, ret, "task mutex init fail");

#if 0
    /*网关设备事件回调初始化*/
    ret = msdp_gw_init();
    RET_GOTO(ret, err, CALL_FUCTION_FAILED, "msdp_gw_init");
#endif

#ifdef GATEWAY_SDK
    /*zigbee设备事件回调初始化*/
    ret = msdp_zbnet_init();
    RET_GOTO(ret, err, CALL_FUCTION_FAILED, "msdp_zbnet_init");
#endif

    sm_attach_service("accs", &msdp_listener);
    module_inited = 1;

    log_trace("msdp init success");
    return ret;
err:
    __free_helper_list();

    log_trace("msdp exit");
    return ret;
}


void msdp_exit()
{
    if (!module_inited) {
        return;
    }

    aos_cancel_work(post_device_status_work, msdp_post_work_handler, NULL);
    __free_task_list();
    os_mutex_destroy(g_task_mutex);
    g_task_mutex = NULL;

    sm_detach_service("accs", &msdp_listener);

    /*设备model链表释放*/
    __free_helper_list();

    module_inited = 0;
}


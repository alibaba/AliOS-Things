/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include "aos/aos.h"
#include "json_parser.h"
#include "service.h"
#include "msdp.h"

#define MODULE_NAME MODULE_NAME_MSDP

typedef int (*status_array_cb)(char *entry, int entry_len, int entry_type,
                               const char *method_name);
typedef int (*get_status_cb)(char *entry, int entry_len, int entry_type,
                             const char *params, dlist_t *head, void *exec_cb);
typedef int (*set_status_cb)(char *entry, int entry_len, int entry_type,
                             const char *params, void *exec_cb);

typedef struct st_attribute_info {
    dlist_t list_node;
    char *attr_name;
    char *attr_value;
} attribute_info;

static const char *msdp_get_timestamp_string()
{
    static char time_string[32] = {0};

    snprintf(time_string, sizeof(time_string) - 1, "%d", (unsigned int)time(NULL));

    return time_string;
}

static int msdp_set_attr_array_each_cb(char *params, int str_len,
                                       const char *method_name)//va_list *ap)
{
    int uuid_len, ret = SERVICE_RESULT_ERR;
    char *str_pos;
    char uuid[MAX_UUID_LEN] = {0};

    str_pos = json_get_value_by_name(params, str_len, JSON_KEY_UUID, &uuid_len,
                                     NULL);
    PTR_RETURN(str_pos, SERVICE_RESULT_ERR, "get value fail, name:%s, params:%s",
               JSON_KEY_UUID, params);
    strncpy(uuid, str_pos, uuid_len);

    char *params_buff = msdp_dup_buff(str_pos, str_len + 1);
    PTR_RETURN(params_buff, ret, "pmalloc fail");
    params_buff[str_len] = '\0';
    log_trace("params = %s\n", params_buff);


    /*method转换为setDeviceStatus*/
    ret = msdp_dispatch_event(uuid, method_name, params_buff);
    RET_LOG(ret, "msdp_dispatch_event, params:%s", params_buff);
    msdp_free_buff(params_buff);

    return ret;
}


static int msdp_status_array_handler(char *params, void *cb,
                                     const char *method_name)
{
    int ret = SERVICE_RESULT_OK;

    //遍历设置设备状态数组
    char *entry, *pos;
    int entry_len, type;
    json_array_for_each_entry(params, pos, entry, entry_len, type) {
        ret = ((status_array_cb)cb)(entry, entry_len, type, method_name);
        RET_LOG(ret, CALL_FUCTION_FAILED, "json_array_for_each_entry");
    }

    return ret;
}



int msdp_set_attr_each_cb(const char *attr_name, int name_len, int type,
                          char *params, set_dev_attr_cb exec_cb)//va_list *ap)
{
    int str_len, ret = SERVICE_RESULT_ERR;
    char *str_pos, *attr_value = NULL;

    str_pos = json_get_value_by_name(params, strlen(params), JSON_KEY_UUID,
                                     &str_len, NULL);
    PTR_RETURN(str_pos, SERVICE_RESULT_ERR, "get uuid fail, params:%s", params);
    char uuid[MAX_UUID_LEN] = {0};
    strncpy(uuid, str_pos, str_len);

    char *name_str = msdp_dup_buff(attr_name, name_len + 1);
    PTR_GOTO(name_str, out, CALL_FUCTION_FAILED, "msdp_dup_buff");
    name_str[name_len] = '\0';
    str_pos = json_get_value_by_name(params, strlen(params), name_str, &str_len, NULL);
    PTR_GOTO(str_pos, out, "get %s value fail, params:%s", name_str, params);

    int value_len = 0;
    char *value_str = NULL;
    char *value_pos = json_get_value_by_name(str_pos, str_len, JSON_KEY_VALUE,
                                             &value_len, NULL);
    if (NULL != value_pos) {
        value_str = msdp_dup_buff(value_pos, value_len + 1);
    } else {
        value_len = str_len;
        value_str = msdp_dup_buff(str_pos, value_len + 1);
    }
    value_str[value_len] = '\0';
    log_trace("attr_name:%s, attr_value:%s", name_str, value_str);

    ret = exec_cb(uuid, name_str, value_str);
    RET_GOTO(ret, out, CALL_FUCTION_FAILED, "exec_cb");

    log_trace("set attibute \"%s\" = \"%s\" success", name_str, value_str);
out:
    if (name_str) {
        msdp_free_buff(name_str);
    }
    if (value_str) {
        msdp_free_buff(value_str);
    }

    return ret;
}


int msdp_get_attr_each_cb(const char *attr_name, int name_len, int type,
                          char *params,
                          dlist_t *head, get_dev_attr_cb exec_cb)// va_list *ap)
{
    int str_len, ret = SERVICE_RESULT_ERR;
    attribute_info *pnode = NULL;
    char *str_pos, *buff = NULL;
    char name_buff[MAX_UUID_LEN] = {0};

    strncpy(name_buff, attr_name, name_len);
    //log_trace("attr_name:%s, params:%s", name_buff, params);

    str_pos = json_get_value_by_name(params, strlen(params), JSON_KEY_UUID,
                                     &str_len, NULL);
    PTR_RETURN(str_pos, SERVICE_RESULT_ERR, "get uuid fail, params:%s", params);
    char uuid[MAX_UUID_LEN] = {0};
    strncpy(uuid, str_pos, str_len);
    log_trace("uuid:%s, attrname:%s", uuid, name_buff);

    ret = exec_cb(uuid, name_buff, &buff);
    RET_RETURN(ret, CALL_FUCTION_FAILED, "exec_cb");
    log_trace("get attibute \"%s\" handler output = %s", name_buff, buff);

    if (head && buff) {
        pnode = (attribute_info *)msdp_new_buff( sizeof(attribute_info));
        PTR_GOTO(pnode, err, "malloc failed\n");
        memset(pnode, 0, sizeof(attribute_info));

        //attr name
        pnode->attr_name = msdp_dup_buff(attr_name, name_len + 1);
        PTR_GOTO(pnode->attr_name, err, "pbufdup failed");
        pnode->attr_name[name_len] = '\0';
        //attr value
        pnode->attr_value = msdp_dup_string(buff);
        PTR_GOTO(pnode->attr_value, err, "pstrdup failed");

        dlist_add_tail(&pnode->list_node, head);
    }

    if (buff) {
        msdp_free_buff(buff);
    }

    return SERVICE_RESULT_OK;

err:
    if (pnode) {
        if (pnode->attr_name) {
            msdp_free_buff(pnode->attr_name);
        }
        if (pnode->attr_value) {
            msdp_free_buff(pnode->attr_value);
        }
        msdp_free_buff((void *)pnode);
    }

    if (buff) {
        msdp_free_buff(buff);
    }

    return SERVICE_RESULT_ERR;
}

/*
--> [{"uuid":"42E6E69DAEF6483FBBA412A28AF7CD76","attrSet":["wlanSwitchState"],"wlanSwitchState":{"value":"1","when":"1404443369"}}]
*/
int msdp_set_status_array(char *params)
{
    int ret = SERVICE_RESULT_OK;

    //遍历各设备，分别设置属性并返会新的属性值
    ret = msdp_status_array_handler(params, msdp_set_attr_array_each_cb,
                                    METHOD_SET_DEVICE_STATUS_ARRAY);
    RET_RETURN(ret, CALL_FUCTION_FAILED, "msdp_status_array_handler");

    return ret;
}


int msdp_set_device_status_handler(char *params, void *cb, void *exec_cb)
{
    int ret = SERVICE_RESULT_OK;
    char *str_pos, *attr_set = NULL;
    char uuid[MAX_UUID_LEN] = {0};
    int str_len  = 0;

    attr_set = json_get_value_by_name(params, strlen(params), JSON_KEY_ATTRSET,
                                      &str_len, NULL);
    PTR_RETURN(attr_set, SERVICE_RESULT_ERR, "get arrSet fail");

    log_trace("attr_set = %s, len:%d", attr_set, str_len);

    char *attr_set_dup = msdp_dup_buff(attr_set, str_len + 1);
    attr_set_dup[str_len] = '\0';
    char *entry, *pos;
    int entry_len, type;

    json_array_for_each_entry(attr_set_dup, pos, entry, entry_len, type) {
        ret = ((set_status_cb)cb)(entry, entry_len, type, params, exec_cb);
        RET_LOG(ret, CALL_FUCTION_FAILED, "json_array_for_each_entry");
    }
    msdp_free_buff(attr_set_dup);

    return ret;
}

int msdp_get_device_status_handler(char *params, void *cb, void *exec_cb,
                                   char **json_out)
{
    int str_len = 0;
    int ret = SERVICE_RESULT_OK;
    int uuid_len = 0;
    char *str_pos = NULL;
    char *report_params = NULL;
    attribute_info *pnode;
    dlist_t *pnode_next = NULL;
    AOS_DLIST_HEAD(head);
    char uuid[MAX_UUID_LEN] = {0};
    char *entry, *pos;
    int entry_len, type;
    char *attr_set_dup = NULL;

    char *attr_set = json_get_value_by_name(params, strlen(params),
                                            JSON_KEY_ATTRSET, &str_len, NULL);
    PTR_RETURN(attr_set, SERVICE_RESULT_ERR, "get arrSet fail");
    log_trace("attr_set = %s", attr_set);

    /*无需同步返回结果*/
    if (NULL == json_out) {
        attr_set_dup = msdp_dup_buff(attr_set, str_len + 1);
        attr_set_dup[str_len] = '\0';
        json_array_for_each_entry(attr_set_dup, pos, entry, entry_len, type) {
            ret = ((get_status_cb)cb)(entry, entry_len, type, params, NULL, exec_cb);
            RET_LOG(ret, CALL_FUCTION_FAILED, "json_array_for_each_entry");
        }
        msdp_free_buff(attr_set_dup);
        attr_set_dup = NULL;

        return ret;
    }


    attr_set_dup = msdp_dup_buff(attr_set, str_len + 1);
    attr_set_dup[str_len] = '\0';

    json_array_for_each_entry(attr_set_dup, pos, entry, entry_len, type) {
        ret = ((get_status_cb)cb)(entry, entry_len, type, params, &head, exec_cb);
        RET_LOG(ret, "get device status fail, params:%s", params);
    }
    msdp_free_buff(attr_set_dup);
    attr_set_dup = NULL;

    if (dlist_empty(&head)) {
        log_warn("attribute value list is empty, nothing todo");
        //输出串为空也返回OK，避免未注册get_callback的属性设置失败
        ret = SERVICE_RESULT_OK;
        //return SERVICE_RESULT_ERR;
        goto out;
    }

    str_pos = json_get_value_by_name(params, strlen(params), JSON_KEY_UUID,
                                     &str_len, NULL);
    PTR_RETURN(str_pos, SERVICE_RESULT_ERR, "get uuid fail");
    strncpy(uuid, str_pos, str_len);

    /*{"uuid":"%s"}*/
    unsigned int params_length = strlen(uuid) + strlen(JSON_KEY_UUID) + 20;
    dlist_for_each_entry(&head, pnode, attribute_info, list_node) {
        /*计算单个属性字符串长度:"attr_name":{"value":%s,"when":""}*/
        params_length += strlen(pnode->attr_name) + strlen(pnode->attr_value) + 30;
    }

    report_params = msdp_new_buff( params_length);
    PTR_GOTO(report_params, out, "pmalloc fail");
    memset(report_params, 0, params_length);

    int len = snprintf(report_params, params_length, "{\"uuid\":\"%s\"", uuid);
    dlist_for_each_entry_safe(&head, pnode_next, pnode, attribute_info, list_node) {
        if (pnode->attr_value[0] != '{' && pnode->attr_value[0] != '[')
            len += snprintf(report_params + len, params_length - len,
                            ",\"%s\":{\"value\":\"%s\"}",
                            pnode->attr_name, pnode->attr_value);
        else
            len += snprintf(report_params + len, params_length - len,
                            ",\"%s\":{\"value\":%s}",
                            pnode->attr_name, pnode->attr_value);
        if (len == params_length) {
            goto out;
            log_error("memory overflow");
        }

        /*移除节点*/
        dlist_del(&pnode->list_node);
        msdp_free_buff(pnode->attr_name);
        msdp_free_buff(pnode->attr_value);
        msdp_free_buff((void *)pnode);
    }
    report_params[len++] = '}';

    //#ifdef CONFIG_METHOD_POSTDEVICEDATAARRAY
    memmove(report_params + 1, report_params, len);
    report_params[0] = '[';
    report_params[len + 1] = ']';
    //#endif

    if (json_out) {
        *json_out = report_params;
        report_params = NULL;
    }

    ret = SERVICE_RESULT_OK;
    log_trace("json_out = %s", json_out ? *json_out : "NULL");

out:
    dlist_for_each_entry_safe(&head, pnode_next, pnode, attribute_info, list_node) {
        dlist_del(&pnode->list_node);
        msdp_free_buff(pnode->attr_name);
        msdp_free_buff(pnode->attr_value);
        msdp_free_buff((void *)pnode);
    }
    if (report_params) {
        msdp_free_buff(report_params);
    }

    return ret;
}


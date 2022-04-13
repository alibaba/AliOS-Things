/*
* Copyright (C) 2020-2021 Alibaba Group Holding Limited
*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#include "u_api.h"
#include "ulog/ulog.h"
#include "path_manager.h"
#include "rpc.h"
#include "aos/list.h"
#include "aos/kernel.h"

enum {
    FILE_TYPE = 1,
    DEVICE_TYPE = 2,
    SOCKET_TYPE = 3,
};

typedef struct {
    dlist_t  path_list;
    char path_name[NAME_LENGTH];
    char main_rpc_name[AOS_RPC_NAME_MAX_SIZE];
    char sub_rpc_name[AOS_RPC_NAME_MAX_SIZE];
    int type;
} path_rpc_name;

dlist_t  m_list;
aos_mutex_t pm_mutex;

static path_rpc_name *check_path_exist(char* path_name)
{
    dlist_t       *head = NULL;
    dlist_t       *iter = NULL;
    path_rpc_name *pdev = NULL;
    int len = 0;
    int path_len = 0;

    if (path_name == NULL) {
        return NULL;
    }
    path_len = strlen(path_name);

    head  = &m_list;
    iter  = head->next;
    while (iter != head) {
        pdev = dlist_entry(iter, path_rpc_name, path_list);
        iter = iter->next;
        len = strlen(pdev->path_name);
        if (path_len != len) {
            continue;
        }
        if (0 == (strncmp(path_name, pdev->path_name, len))) {
            return pdev;
        }
    }
    return NULL;
}

int registe_path(char *path, char *main_rpc, char *sub_rpc, int type)
{
    path_rpc_name *pdev = NULL;

    aos_mutex_lock(&pm_mutex, AOS_WAIT_FOREVER);
    pdev = check_path_exist(path);
    if (pdev != NULL) {
        memcpy(pdev->main_rpc_name, main_rpc, strlen(main_rpc) + 1);
        memcpy(pdev->sub_rpc_name, sub_rpc, strlen(sub_rpc) + 1);
        aos_mutex_unlock(&pm_mutex);
        return 0;
    }

    pdev = aos_malloc(sizeof(path_rpc_name));
    if (pdev == NULL) {
        printf("malloc failed\r\n");
        errno = ENOMEM;
        return -1;
    }
    memset(pdev, 0, sizeof(path_rpc_name));
    memcpy(pdev->path_name, path, strlen(path) + 1);
    memcpy(pdev->main_rpc_name, main_rpc, strlen(main_rpc) + 1);
    memcpy(pdev->sub_rpc_name, sub_rpc, strlen(sub_rpc) + 1);
    pdev->type = type;

    dlist_add(&pdev->path_list, &m_list);
    aos_mutex_unlock(&pm_mutex);
    return 0;
}

int unregiste_path(char *path)
{
    int ret = -1;
    dlist_t       *head = NULL;
    dlist_t       *iter = NULL;
    path_rpc_name *pdev = NULL;
    int len = 0;
    int path_len = 0;

    if (path == NULL) {
        return ret;
    }
    path_len = strlen(path);

    aos_mutex_lock(&pm_mutex, AOS_WAIT_FOREVER);
    head  = &m_list;
    iter  = head->next;
    while (iter != head) {
        pdev = dlist_entry(iter, path_rpc_name, path_list);
        iter = iter->next;
        len = strlen(pdev->path_name);
        if (path_len != len) {
            continue;
        }
        if(0 == (strncmp(path, pdev->path_name, len))){
            dlist_del(&pdev->path_list);
            aos_free(pdev);
            ret = 0;
            break;
        }
    }
    aos_mutex_unlock(&pm_mutex);
    return ret;
}

static int get_rpc_name_by_path_name(char* path_name, char* main_rpc_name, char* device_rpc_name, int *type)
{
    int ret = -1;
    dlist_t       *head = NULL;
    dlist_t       *iter = NULL;
    path_rpc_name *pdev = NULL;
    int len = 0;
    int path_len = 0;

    if (path_name == NULL) {
        return ret;
    }
    path_len = strlen(path_name);

    aos_mutex_lock(&pm_mutex, AOS_WAIT_FOREVER);
    head  = &m_list;
    iter  = head->next;
    while (iter != head) {
        pdev = dlist_entry(iter, path_rpc_name, path_list);
        iter = iter->next;
        len = strlen(pdev->path_name);
        if (pdev->type == DEVICE_TYPE) {
            //match device path fully
            if (path_len != len) {
                continue;
            }
        }
        //match file path which is register
        if(0 == (strncmp(path_name, pdev->path_name, len))){
            memcpy(main_rpc_name, pdev->main_rpc_name, strlen(pdev->main_rpc_name) + 1);
            memcpy(device_rpc_name, pdev->sub_rpc_name, strlen(pdev->sub_rpc_name) + 1);
            *type = pdev->type;
            ret = 0;
            break;
        }
    }
    aos_mutex_unlock(&pm_mutex);
    return ret;
}

void registe_error_event_cb(int eid, void *val)
{
    struct aos_revt_param_t *param = (struct aos_revt_param_t *)val;
    int reason = *(int *)param->param;

    if (eid != RPC_EID_STOP) {
        aos_rpc_close(param->srpc_handle);
    }

    printf("error reason:%d\r\n", reason);
}

void rpc_register_request(struct aos_parcel_t *pkt)
{
    int ret = 0;
    int ret_value = 0;
    int funcnum = 0;
    char path_name[128];
    char main_name[128];
    char sub_name[128];
    int type = 0;
    size_t len = 0;
    int reply_rpc_handle = 0;

    len = sizeof(reply_rpc_handle);
    ret = aos_parcel_get(AP_UVARINT, &reply_rpc_handle, &len, pkt);
    if (ret != RHINO_SUCCESS) {
        printf("pm aos_parcel_get len failed! ret:%d\r\n", ret);
        goto error;
    }
    len = sizeof(funcnum);
    ret = aos_parcel_get(AP_BUF, &funcnum, &len, pkt);
    if (ret != RHINO_SUCCESS) {
        printf("pm aos_parcel_get funcnum failed! ret:%d\r\n", ret);
        goto error;
    }
    len = sizeof(path_name);
    memset(path_name, 0, sizeof(path_name));
    ret = aos_parcel_get(AP_BUF, path_name, &len, pkt);
    if (ret != RHINO_SUCCESS) {
        printf("pm aos_parcel_get path_name failed! ret:%d\r\n", ret);
        goto error;
    }

    switch (funcnum) {
        case DRIVER_RPC_REGISTER:
            len = sizeof(main_name);
            memset(main_name, 0, sizeof(main_name));
            ret = aos_parcel_get(AP_BUF, main_name, &len, pkt);
            if (ret != RHINO_SUCCESS) {
                printf("pm aos_parcel_get main_name failed! ret:%d\r\n", ret);
                goto error;
            }
            len = sizeof(sub_name);
            memset(sub_name, 0, sizeof(sub_name));
            ret = aos_parcel_get(AP_BUF, sub_name, &len, pkt);
            if (ret != RHINO_SUCCESS) {
                printf("pm aos_parcel_get sub_name failed! ret:%d\r\n", ret);
                goto error;
            }
            len = sizeof(type);
            ret = aos_parcel_get(AP_BUF, &type, &len, pkt);
            if (ret != RHINO_SUCCESS) {
                printf("pm aos_parcel_get type failed! ret:%d\r\n", ret);
                goto error;
            }
            ret_value = registe_path(path_name, main_name, sub_name, type);
            ret = aos_rpc_resp(reply_rpc_handle, 1, sizeof(ret_value), &ret_value);
            if (ret != RHINO_SUCCESS) {
                printf("pm registe_path aos_rpc_resp failed! ret:%d\r\n", ret);
                goto error;
            }
            break;
        case DRIVER_RPC_UNREGISTER:
            ret_value = unregiste_path(path_name);
            ret = aos_rpc_resp(reply_rpc_handle, 1, sizeof(ret_value), &ret_value);
            if (ret != RHINO_SUCCESS) {
                printf("pm unregiste_path aos_rpc_resp failed! ret:%d\r\n", ret);
                goto error;
            }
            break;
        case GET_MAIN_RPC_NAME:
            ret = get_rpc_name_by_path_name(path_name, main_name, sub_name, &type);
            if (ret < 0) {
                goto error;
            } else {
                ret_value = RHINO_SUCCESS;
                ret = aos_rpc_resp(reply_rpc_handle, 4, sizeof(ret_value), &ret_value,
                    strlen(main_name), main_name, strlen(sub_name), sub_name, sizeof(type), &type);
                if (ret != RHINO_SUCCESS) {
                    printf("pm get_rpc_name aos_rpc_resp failed! ret:%d\r\n", ret);
                    goto error;
                }
            }
            break;
        default:
            goto error;
            break;
    }

    return;

error:
    ret_value = -1;
    ret = aos_rpc_resp(reply_rpc_handle, 1, sizeof(ret_value), &ret_value);
    if (ret != RHINO_SUCCESS) {
        printf("pm error aos_rpc_resp failed! ret:%d\r\n", ret);
    }
    return;
}

int path_manager_server_setup(void)
{
    int ret;
    int srpc_handle;

    dlist_init(&m_list);
    aos_mutex_new(&pm_mutex);

    ret = aos_rpc_regist(PATH_MANAGER_RPC_NAME, REGISTER_RPC_REQ_SIZE, &srpc_handle);
    if (ret != RHINO_SUCCESS) {
        printf("regist rpc service:%s failed!  ret:%d\r\n", PATH_MANAGER_RPC_NAME, ret);
        return ret;
    }
    ret = aos_rpc_run(srpc_handle, rpc_register_request, registe_error_event_cb);
    if (ret != RHINO_SUCCESS) {
        printf("run rpc %s failed! ret:%d\r\n", PATH_MANAGER_RPC_NAME, ret);
        return ret;
    }
    printf("%s ret:%d\n",__func__,ret);
    return ret;
}

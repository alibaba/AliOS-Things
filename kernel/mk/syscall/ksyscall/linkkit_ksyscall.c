/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "k_config.h"

#if (RHINO_CONFIG_LINKKIT_SYSCALL > 0)

#if !(RHINO_CONFIG_URES > 0)
#error "linkkit ksyscall depends on RHINO_CONFIG_URES"
#endif

#include "linkkit/dev_model_api.h"
#include "infra_compat.h"
#include "wrappers_defs.h"
#include "wrappers_os.h"
#include "linkkit_syscall_arg.h"
#include "k_api.h"
#include "utask.h"
#include "callback.h"
#include "res.h"

//#define LOCAL_DEBUG

#ifdef LOCAL_DEBUG
#include <stdio.h>
#define LDEBUG(...) printf(##__VA_ARGS__)
#else
#define LDEBUG(...)
#endif

static kbuf_queue_t *send_queue   = NULL;
static kbuf_queue_t *recv_queue   = NULL;
static int           active_pid   = 0;
static int           active_devid = -1;

int sys_IOT_Linkkit_Open_stub(void *arg)
{
    int ret;
    ktask_t *cur;
    task_group_t *group;
    IOT_Linkkit_Open_syscall_arg_t *_arg;

    CPSR_ALLOC();

    _arg = (IOT_Linkkit_Open_syscall_arg_t*)arg;

    ret = IOT_Linkkit_Open(_arg->dev_type, _arg->meta_info);
    if (ret != FAIL_RETURN) {
        RHINO_CRITICAL_ENTER();
        cur = krhino_cur_task_get();
        group = cur->task_group;
        send_queue = group->cb_call_buf_queue;
        recv_queue = group->cb_ret_buf_queue;
        active_pid = cur->pid;
        active_devid = ret;
        group->linkkit_active = 1;
        RHINO_CRITICAL_EXIT();
    }

    return ret;
}

int sys_IOT_Linkkit_Connect_stub(void *arg)
{
    IOT_Linkkit_Connect_syscall_arg_t *_arg;

    _arg = (IOT_Linkkit_Connect_syscall_arg_t*)arg;

    return IOT_Linkkit_Connect(_arg->devid);
}

void sys_IOT_Linkkit_Yield_stub(void *arg)
{
    IOT_Linkkit_Yield_syscall_arg_t *_arg;

    _arg = (IOT_Linkkit_Yield_syscall_arg_t*)arg;

    IOT_Linkkit_Yield(_arg->timeout_ms);
}

int sys_IOT_Linkkit_Close_stub(void *arg)
{
    int           ret;
    ktask_t      *cur;
    task_group_t *group;

    IOT_Linkkit_Close_syscall_arg_t *_arg;

    CPSR_ALLOC();

    _arg = (IOT_Linkkit_Close_syscall_arg_t*)arg;

    ret = IOT_Linkkit_Close(_arg->devid);
    if (ret != FAIL_RETURN) {
        RHINO_CRITICAL_ENTER();
        cur = krhino_cur_task_get();
        group = cur->task_group;
        recv_queue = NULL;
        send_queue = NULL;
        active_pid = 0;
        group->linkkit_active = 0;
        RHINO_CRITICAL_EXIT();
    }

    return ret;
}

int sys_IOT_Linkkit_Report_stub(void *arg)
{
    IOT_Linkkit_Report_syscall_arg_t *_arg;

    _arg = (IOT_Linkkit_Report_syscall_arg_t*)arg;

    return IOT_Linkkit_Report(_arg->devid, _arg->msg_type,
                              _arg->payload, _arg->payload_len);
}

int sys_IOT_Linkkit_Query_stub(void *arg)
{
    IOT_Linkkit_Query_syscall_arg_t *_arg;

    _arg = (IOT_Linkkit_Query_syscall_arg_t*)arg;

    return IOT_Linkkit_Query(_arg->devid, _arg->msg_type,
                             _arg->payload, _arg->payload_len);
}

int sys_IOT_Linkkit_TriggerEvent_stub(void *arg)
{
    IOT_Linkkit_TriggerEvent_syscall_arg_t *_arg;

    _arg = (IOT_Linkkit_TriggerEvent_syscall_arg_t*)arg;

    return IOT_Linkkit_TriggerEvent(_arg->devid, _arg->eventid,
                                    _arg->eventid_len, _arg->payload,
                                    _arg->payload_len);
}

void sys_IOT_SetLogLevel_stub(void *arg)
{
    IOT_SetLogLevel_syscall_arg_t *_arg;

    _arg = (IOT_SetLogLevel_syscall_arg_t*)arg;

    IOT_SetLogLevel(_arg->level);
}

void sys_IOT_DumpMemoryStats_stub(void *arg)
{
    IOT_DumpMemoryStats_syscall_arg_t *_arg;

    _arg = (IOT_DumpMemoryStats_syscall_arg_t*)arg;

    IOT_DumpMemoryStats(_arg->level);
}

int sys_IOT_SetupConnInfo_stub(void *arg)
{
    IOT_SetupConnInfo_syscall_arg_t *_arg;

    _arg = (IOT_SetupConnInfo_syscall_arg_t*)arg;

    return IOT_SetupConnInfo(_arg->product_key, _arg->device_name,
                             _arg->device_secret, _arg->info);
}

int sys_IOT_Ioctl_stub(void *arg)
{
    IOT_Ioctl_syscall_arg_t *_arg;

    _arg = (IOT_Ioctl_syscall_arg_t*)arg;

    return IOT_Ioctl(_arg->option, _arg->data);
}

int sys_IOT_HAL_Ioctl_stub(void *arg)
{
    int ret;

    IOT_HAL_Ioctl_syscall_arg_t *_arg;

    _arg = (IOT_HAL_Ioctl_syscall_arg_t*)arg;

    switch (_arg->cmd) {
        case IOT_HAL_SET_PRODUCT_KEY:
            ret = HAL_SetProductKey((char*)_arg->arg);
            break;
        case IOT_HAL_GET_PRODUCT_KEY:
            ret = HAL_GetProductKey((char*)_arg->arg);
            break;
        case IOT_HAL_SET_PRODUCT_SECRET:
            ret = HAL_SetProductSecret((char*)_arg->arg);
            break;
        case IOT_HAL_GET_PRODUCT_SECRET:
            ret = HAL_GetProductSecret((char*)_arg->arg);
            break;
        case IOT_HAL_SET_DEVICE_NAME:
            ret = HAL_SetDeviceName((char*)_arg->arg);
            break;
        case IOT_HAL_GET_DEVICE_NAME:
            ret = HAL_GetDeviceName((char*)_arg->arg);
            break;
        case IOT_HAL_SET_DEVICE_SECRET:
            ret = HAL_SetDeviceSecret((char*)_arg->arg);
            break;
        case IOT_HAL_GET_DEVICE_SECRET:
            ret = HAL_GetDeviceSecret((char*)_arg->arg);
            break;
        default:
            ret = -2;
            break;
    }

    return ret;
}

typedef struct {
    int eventid;
    void *callback_user;
} impl_callback_map_t;

static impl_callback_map_t g_impl_event_callback_map[];

static int ITE_AWSS_STATUS_cb(int arg0)
{
    kstat_t       stat;
    cb_call_msg_t call_msg;
    cb_ret_msg_t  ret_msg;
    size_t        size;

    call_msg.func_ptr     = g_impl_event_callback_map[ITE_AWSS_STATUS].callback_user;
    call_msg.arg_cnt      = 1;
    call_msg.arg_value[0] = (void*)arg0;
    call_msg.has_ret      = 1;

    stat = cb_call_buf_queue_push(send_queue, &call_msg);
    if (stat != RHINO_SUCCESS) {
        return -1;
    }

    stat = cb_ret_buf_queue_pop(recv_queue, &ret_msg, &size);
    if (stat != RHINO_SUCCESS) {
        return -2;
    }

    return(int)ret_msg.ret_val;
}

static int ITE_CONNECT_SUCC_cb(void)
{
    kstat_t       stat;
    cb_call_msg_t call_msg;
    cb_ret_msg_t  ret_msg;
    size_t        size;

    call_msg.func_ptr     = g_impl_event_callback_map[ITE_CONNECT_SUCC].callback_user;
    call_msg.arg_cnt      = 0;
    call_msg.has_ret      = 1;

    stat = cb_call_buf_queue_push(send_queue, &call_msg);
    if (stat != RHINO_SUCCESS) {
        return -1;
    }

    stat = cb_ret_buf_queue_pop(recv_queue, &ret_msg, &size);
    if (stat != RHINO_SUCCESS) {
        return -2;
    }

    return (int)ret_msg.ret_val;
}

static int ITE_CONNECT_FAIL_cb(void)
{
    kstat_t       stat;
    cb_call_msg_t call_msg;
    cb_ret_msg_t  ret_msg;
    size_t        size;

    call_msg.func_ptr     = g_impl_event_callback_map[ITE_CONNECT_FAIL].callback_user;
    call_msg.arg_cnt      = 0;
    call_msg.has_ret      = 1;

    stat = cb_call_buf_queue_push(send_queue, &call_msg);
    if (stat != RHINO_SUCCESS) {
        return -1;
    }

    stat = cb_ret_buf_queue_pop(recv_queue, &ret_msg, &size);
    if (stat != RHINO_SUCCESS) {
        return -2;
    }

    return (int)ret_msg.ret_val;
}

static int ITE_DISCONNECTED_cb(void)
{
    kstat_t       stat;
    cb_call_msg_t call_msg;
    cb_ret_msg_t  ret_msg;
    size_t        size;

    call_msg.func_ptr     = g_impl_event_callback_map[ITE_DISCONNECTED].callback_user;
    call_msg.arg_cnt      = 0;
    call_msg.has_ret      = 1;

    stat = cb_call_buf_queue_push(send_queue, &call_msg);
    if (stat != RHINO_SUCCESS) {
        return -1;
    }

    stat = cb_ret_buf_queue_pop(recv_queue, &ret_msg, &size);
    if (stat != RHINO_SUCCESS) {
        return -2;
    }

    return (int)ret_msg.ret_val;
}

static int ITE_RAWDATA_ARRIVED_cb(const int devid, const unsigned char *payload,
                                  const int payload_len)
{
    kstat_t        stat;
    cb_call_msg_t  call_msg;
    cb_ret_msg_t   ret_msg;
    size_t         size;
    int            ret;
    char          *user_ptr = NULL;

    /* alloc user space data buffer */
    if (NULL != payload) {

        user_ptr = res_malloc(active_pid, payload_len + 1);
        if (NULL == user_ptr) {
            ret = -1;
            goto out;
        }
        /* call user space callback func */
        memcpy(user_ptr, payload, payload_len);
        user_ptr[payload_len] = 0;
    }

    call_msg.func_ptr     = g_impl_event_callback_map[ITE_RAWDATA_ARRIVED].callback_user;
    call_msg.arg_cnt      = 3;
    call_msg.arg_value[0] = (void*)devid;
    call_msg.arg_value[1] = (void*)user_ptr;
    call_msg.arg_value[2] = (void*)payload_len;
    call_msg.has_ret      = 1;

    stat = cb_call_buf_queue_push(send_queue, &call_msg);
    if (stat != RHINO_SUCCESS) {
        ret = -2;
        goto out;
    }

    stat = cb_ret_buf_queue_pop(recv_queue, &ret_msg, &size);
    if (stat != RHINO_SUCCESS) {
        ret = -3;
        goto out;
    }

    ret = (int)ret_msg.ret_val;

out:
    /* free user space data buffer */
    if (NULL != user_ptr) {
        res_free(active_pid, user_ptr);
    }

    return ret;
}

// note: the API get user space buf, we need to free the buf and
// realloc the buf in kernel, and copy the buf data
static int ITE_SERVICE_REQUEST_cb(const int devid, const char *serviceid, const int serviceid_len,
                                 const char *request, const int request_len,
                                 char **response, int *response_len)
{
    kstat_t       stat;
    cb_call_msg_t call_msg;
    cb_ret_msg_t  ret_msg;
    size_t        size;
    int           ret;
    char          *serviceid_ptr = NULL;
    char          *request_ptr   = NULL;
    char          *response_ptr  = NULL;
    char          *kresponse_ptr  = NULL;
    int           *response_len_ptr = NULL;

    /* alloc user space data buffer */
    if (NULL != serviceid) {
        serviceid_ptr = (char*)res_malloc(active_pid, serviceid_len + 1);
        if (NULL == serviceid_ptr) {
            ret = -1;
            goto out;
        }
        /* call user space callback func */
        memcpy(serviceid_ptr, serviceid, serviceid_len);
        serviceid_ptr[serviceid_len] = 0;
    }

    if (NULL != request) {
        request_ptr = (char*)res_malloc(active_pid, request_len + 1);
        if (NULL == request_ptr) {
            ret = -2;
            goto out;
        }
        /* call user space callback func */
        memcpy(request_ptr, request, request_len);
        request_ptr[request_len] = 0;
    }

    if (NULL != response && NULL != response_len) {
        response_ptr = (char*)res_malloc(active_pid, sizeof(char*));
        if (NULL == response_ptr) {
            ret = -3;
            goto out;
        }

        response_len_ptr = (int*)res_malloc(active_pid, sizeof(int*));
        if (NULL == response_len_ptr) {
            ret = -4;
            goto out;
        }
    }

    call_msg.func_ptr     = g_impl_event_callback_map[ITE_SERVICE_REQUEST].callback_user;
    call_msg.arg_cnt      = 7;
    call_msg.arg_value[0] = (void*)devid;
    call_msg.arg_value[1] = (void*)serviceid_ptr;
    call_msg.arg_value[2] = (void*)serviceid_len;
    call_msg.arg_value[3] = (void*)request_ptr;
    call_msg.arg_value[4] = (void*)request_len;
    call_msg.arg_value[5] = (void*)response_ptr;
    call_msg.arg_value[6] = (void*)response_len_ptr;
    call_msg.has_ret      = 1;

    stat = cb_call_buf_queue_push(send_queue, &call_msg);
    if (stat != RHINO_SUCCESS) {
        ret = -5;
        goto out;
    }

    stat = cb_ret_buf_queue_pop(recv_queue, &ret_msg, &size);
    if (stat != RHINO_SUCCESS) {
        ret = -6;
        goto out;
    }

    ret = (int)ret_msg.ret_val;

    if (NULL != response && NULL != response_len) {
        kresponse_ptr = malloc(*response_len_ptr + 1);
        if (NULL == kresponse_ptr) {
            *response     = NULL;
            *response_len = 0;
        } else {
            if (NULL != *(size_t*)response_ptr) {
                memcpy(kresponse_ptr, (void*)(*(size_t*)response_ptr), *response_len_ptr);
                kresponse_ptr[*response_len_ptr] = 0;
                *response     = kresponse_ptr;
                *response_len = *response_len_ptr;
            } else {
                *response     = NULL;
                *response_len = 0;
            }
        }

        if (NULL != *(size_t*)response_ptr) {
            res_free(active_pid, (void*)(*(size_t*)response_ptr));
        }
    }

out:
    if (NULL != serviceid_ptr) {
        res_free(active_pid, serviceid_ptr);
    }

    if (NULL != request_ptr) {
        res_free(active_pid, request_ptr);
    }

    if (NULL != response_ptr) {
        res_free(active_pid, response_ptr);
    }

    if (NULL != response_len_ptr) {
        res_free(active_pid, response_len_ptr);
    }

    return ret;
}

static int ITE_SERVICE_REQUEST_EXT_cb(const int devid, const char *serviceid, const int serviceid_len,
                                      const char *request, const int request_len, void *p_service_ctx)
{
    return 0;
}

static int ITE_PROPERTY_SET_cb(const int devid, const char *request, const int request_len)
{
    kstat_t        stat;
    cb_call_msg_t  call_msg;
    cb_ret_msg_t   ret_msg;
    size_t         size;
    int            ret;
    char          *user_ptr = NULL;

    /* alloc user space data buffer */
    if (NULL != request) {

        user_ptr = res_malloc(active_pid, request_len + 1);
        if (NULL == user_ptr) {
            ret = -1;
            goto out;
        }
        /* call user space callback func */
        memcpy(user_ptr, request, request_len);
        user_ptr[request_len] = 0;
    }

    call_msg.func_ptr     = g_impl_event_callback_map[ITE_PROPERTY_SET].callback_user;
    call_msg.arg_cnt      = 3;
    call_msg.arg_value[0] = (void*)devid;
    call_msg.arg_value[1] = (void*)user_ptr;
    call_msg.arg_value[2] = (void*)request_len;
    call_msg.has_ret      = 1;

    stat = cb_call_buf_queue_push(send_queue, &call_msg);
    if (stat != RHINO_SUCCESS) {
        ret = -2;
        goto out;
    }

    stat = cb_ret_buf_queue_pop(recv_queue, &ret_msg, &size);
    if (stat != RHINO_SUCCESS) {
        ret = -3;
        goto out;
    }

    ret = (int)ret_msg.ret_val;

out:
    /* free user space data buffer */
    if (NULL != user_ptr) {
        res_free(active_pid, user_ptr);
    }

    return ret;
}

static int ITE_PROPERTY_GET_cb(const int devid, const char *request,
                               const int request_len, char **response, int *response_len)
{
    kstat_t        stat;
    cb_call_msg_t  call_msg;
    cb_ret_msg_t   ret_msg;
    size_t         size;
    int            ret;
    char          *request_ptr   = NULL;
    char          *response_ptr  = NULL;
    char          *kresponse_ptr  = NULL;
    int           *response_len_ptr = NULL;

    if (NULL != request) {

        request_ptr = (char*)res_malloc(active_pid, request_len + 1);
        if (NULL == request_ptr) {
            ret = -1;
            goto out;
        }
        /* call user space callback func */
        memcpy(request_ptr, request, request_len);
        request_ptr[request_len] = 0;
    }

    if (NULL != response && NULL != response_len) {
        response_ptr = (char*)res_malloc(active_pid, sizeof(char*));
        if (NULL == response_ptr) {
            res_free(active_pid, request_ptr);
            ret = -2;
            goto out;
        }

        response_len_ptr = (int*)res_malloc(active_pid, sizeof(int*));
        if (NULL == response_len_ptr) {
            ret = -3;
            goto out;
        }
    }

    call_msg.func_ptr     = g_impl_event_callback_map[ITE_PROPERTY_GET].callback_user;
    call_msg.arg_cnt      = 5;
    call_msg.arg_value[0] = (void*)devid;
    call_msg.arg_value[1] = (void*)request_ptr;
    call_msg.arg_value[2] = (void*)request_len;
    call_msg.arg_value[3] = (void*)response_ptr;
    call_msg.arg_value[4] = (void*)response_len_ptr;
    call_msg.has_ret      = 1;

    stat = cb_call_buf_queue_push(send_queue, &call_msg);
    if (stat != RHINO_SUCCESS) {
        ret = -4;
        goto out;
    }

    stat = cb_ret_buf_queue_pop(recv_queue, &ret_msg, &size);
    if (stat != RHINO_SUCCESS) {
        ret = -5;
        goto out;
    }

    ret = (int)ret_msg.ret_val;

    if (NULL != response && NULL != response_len) {
        kresponse_ptr = (char*)malloc(*response_len_ptr + 1);
        if (NULL == kresponse_ptr) {
            *response = NULL;
            *response_len = 0;
        } else {
            if (NULL != *(size_t*)response_ptr) {
                memcpy(kresponse_ptr, (void*)(*(size_t*)response_ptr), *response_len_ptr);
                kresponse_ptr[*response_len_ptr] = 0;
                *response = kresponse_ptr;
                *response_len = *response_len_ptr;
            } else {
                *response = NULL;
                *response_len = 0;
            }
        }

        if (NULL != *(size_t*)response_ptr) {
            res_free(active_pid, (void*)(*(size_t*)response_ptr));
        }
    }

out:
    if (NULL != request_ptr) {
        res_free(active_pid, request_ptr);
    }

    if (NULL != response_ptr) {
        res_free(active_pid, response_ptr);
    }

    if (NULL != response_len_ptr) {
        res_free(active_pid, response_len_ptr);
    }

    return ret;
}

#ifdef DEVICE_MODEL_SHADOW
static int ITE_PROPERTY_DESIRED_GET_REPLY_cb(const char *request, const int request_len)
{
    kstat_t        stat;
    cb_call_msg_t  call_msg;
    cb_ret_msg_t   ret_msg;
    size_t         size;
    int            ret;
    char          *user_ptr = NULL;

    /* alloc user space data buffer */
    if (NULL != request) {
        user_ptr = (char*)res_malloc(active_pid, request_len + 1);
        if (NULL == user_ptr) {
            ret = -1;
            goto out;
        }
        /* call user space callback func */
        memcpy(user_ptr, request, request_len);
        user_ptr[request_len] = 0;
    }

    call_msg.func_ptr     = g_impl_event_callback_map[ITE_PROPERTY_DESIRED_GET_REPLY].callback_user;
    call_msg.arg_cnt      = 2;
    call_msg.arg_value[0] = (void*)request;
    call_msg.arg_value[1] = (void*)request_len;
    call_msg.has_ret      = 1;

    stat = cb_call_buf_queue_push(send_queue, &call_msg);
    if (stat != RHINO_SUCCESS) {
        ret = -2;
        goto out;
    }

    stat = cb_ret_buf_queue_pop(recv_queue, &ret_msg, &size);
    if (stat != RHINO_SUCCESS) {
        ret = -3;
        goto out;
    }

    ret = (int)ret_msg.ret_val;

out:
    /* free user space data buffer */
    if (NULL != user_ptr) {
        res_free(active_pid, user_ptr);
    }

    return ret;
}
#endif

static int ITE_REPORT_REPLY_cb(const int devid, const int msgid, const int code,
                               const char *reply, const int reply_len)
{
    kstat_t        stat;
    cb_call_msg_t  call_msg;
    cb_ret_msg_t   ret_msg;
    size_t         size;
    int            ret;
    char          *user_ptr = NULL;

    /* alloc user space data buffer */
    if (NULL != reply) {
        user_ptr = (char*)res_malloc(active_pid, reply_len + 1);
        if (NULL == user_ptr) {
            ret = -1;
            goto out;
        }
        /* call user space callback func */
        memcpy(user_ptr, reply, reply_len);
        user_ptr[reply_len] = 0;
    }

    call_msg.func_ptr     = g_impl_event_callback_map[ITE_REPORT_REPLY].callback_user;
    call_msg.arg_cnt      = 5;
    call_msg.arg_value[0] = (void*)devid;
    call_msg.arg_value[1] = (void*)msgid;
    call_msg.arg_value[2] = (void*)code;
    call_msg.arg_value[3] = (void*)user_ptr;
    call_msg.arg_value[4] = (void*)reply_len;
    call_msg.has_ret      = 1;

    stat = cb_call_buf_queue_push(send_queue, &call_msg);
    if (stat != RHINO_SUCCESS) {
        ret = -2;
        goto out;
    }

    stat = cb_ret_buf_queue_pop(recv_queue, &ret_msg, &size);
    if (stat != RHINO_SUCCESS) {
        ret = -3;
        goto out;
    }

    ret = (int)ret_msg.ret_val;

out:
    /* free user space data buffer */
    if (NULL != user_ptr) {
        res_free(active_pid, user_ptr);
    }

    return ret;
}

static int ITE_TRIGGER_EVENT_REPLY_cb(const int devid, const int msgid, const int code, const char *eventid,
                                      const int event_len, const char *message, const int message_len)
{
    kstat_t       stat;
    cb_call_msg_t call_msg;
    cb_ret_msg_t  ret_msg;
    size_t        size;
    int           ret;
    char          *eventid_ptr = NULL;
    char          *message_ptr = NULL;

    /* alloc user space data buffer */
    if (NULL != eventid) {
        eventid_ptr = (char*)res_malloc(active_pid, event_len + 1);
        if (NULL == eventid_ptr) {
            ret = -1;
            goto out;
        }
        /* call user space callback func */
        memcpy(eventid_ptr, eventid, event_len);
        eventid_ptr[event_len] = 0;
    }

        /* alloc user space data buffer */
    if (NULL != message) {
        message_ptr = (char*)res_malloc(active_pid, message_len + 1);
        if (NULL == message_ptr) {
            ret = -2;
            goto out;
        }
        /* call user space callback func */
        memcpy(message_ptr, message, message_len);
        message_ptr[message_len] = 0;
    }

    call_msg.func_ptr     = g_impl_event_callback_map[ITE_TRIGGER_EVENT_REPLY].callback_user;
    call_msg.arg_cnt      = 7;
    call_msg.arg_value[0] = (void*)devid;
    call_msg.arg_value[1] = (void*)msgid;
    call_msg.arg_value[2] = (void*)code;
    call_msg.arg_value[3] = (void*)eventid_ptr;
    call_msg.arg_value[4] = (void*)event_len;
    call_msg.arg_value[5] = (void*)message_ptr;
    call_msg.arg_value[6] = (void*)message_len;
    call_msg.has_ret      = 1;

    stat = cb_call_buf_queue_push(send_queue, &call_msg);
    if (stat != RHINO_SUCCESS) {
        ret = -3;
    }

    stat = cb_ret_buf_queue_pop(recv_queue, &ret_msg, &size);
    if (stat != RHINO_SUCCESS) {
        ret = -4;
    }

    ret = (int)ret_msg.ret_val;

out:
    /* free user space data buffer */
    if (NULL != eventid_ptr) {
        res_free(active_pid, eventid_ptr);
    }

    if (NULL != message_ptr) {
        res_free(active_pid, message_ptr);
    }

    return ret;
}

static int ITE_TIMESTAMP_REPLY_cb(const char *timestamp)
{
    kstat_t        stat;
    cb_call_msg_t  call_msg;
    cb_ret_msg_t   ret_msg;
    size_t         size;
    int            ret;
    char          *user_ptr = NULL;

    /* alloc user space data buffer */
    if (NULL != timestamp) {

        user_ptr = (char*)res_malloc(active_pid, strlen(timestamp) + 1);
        if (NULL == user_ptr) {
            ret = -1;
            goto out;
        }
        /* call user space callback func */
        memcpy(user_ptr, timestamp, strlen(timestamp));
        user_ptr[strlen(timestamp)] = 0;
    }

    call_msg.func_ptr     = g_impl_event_callback_map[ITE_TIMESTAMP_REPLY].callback_user;
    call_msg.arg_cnt      = 1;
    call_msg.arg_value[0] = (void*)user_ptr;
    call_msg.has_ret      = 1;

    stat = cb_call_buf_queue_push(send_queue, &call_msg);
    if (stat != RHINO_SUCCESS) {
        ret = -2;
        goto out;
    }

    stat = cb_ret_buf_queue_pop(recv_queue, &ret_msg, &size);
    if (stat != RHINO_SUCCESS) {
        ret = -3;
        goto out;
    }

    ret = (int)ret_msg.ret_val;

out:
    /* free user space data buffer */
    if (NULL != user_ptr) {
        res_free(active_pid, user_ptr);
    }

    return ret;
}

static int ITE_TOPOLIST_REPLY_cb(const int arg0, const int arg1, const int arg2,
                                 const char *arg3, const int arg4)
{
    return 0;
}

static int ITE_PERMIT_JOIN_cb(const char *arg0, int arg1)
{
    return 0;
}

static int ITE_INITIALIZE_COMPLETED_cb(const int devid)
{
    kstat_t       stat;
    cb_call_msg_t call_msg;
    cb_ret_msg_t  ret_msg;
    size_t        size;
    int           ret;

    call_msg.func_ptr     = g_impl_event_callback_map[ITE_INITIALIZE_COMPLETED].callback_user;
    call_msg.arg_cnt      = 1;
    call_msg.arg_value[0] = (void*)devid;
    call_msg.has_ret      = 1;

    LDEBUG("@@@@ %s:%d: send_queue %p, recv_queue %p\r\n",
            __func__, __LINE__, send_queue, recv_queue);

    stat = cb_call_buf_queue_push(send_queue, &call_msg);
    if (stat != RHINO_SUCCESS) {
        return -1;
    }

    stat = cb_ret_buf_queue_pop(recv_queue, &ret_msg, &size);
    if (stat != RHINO_SUCCESS) {
        return -2;
    }

    ret = (int)ret_msg.ret_val;

    return ret;
}

static int ITE_FOTA_cb(const int type, const char *version)
{
    kstat_t        stat;
    cb_call_msg_t  call_msg;
    cb_ret_msg_t   ret_msg;
    size_t         size;
    int            ret;
    char          *user_ptr = NULL;

    /* alloc user space data buffer */
    if (NULL != version) {
        user_ptr = (char*)res_malloc(active_pid, strlen(version) + 1);
        if (NULL == user_ptr) {
            ret = -1;
            goto out;
        }
        /* call user space callback func */
        memcpy(user_ptr, version, strlen(version));
        user_ptr[strlen(version)] = 0;
    }

    call_msg.func_ptr     = g_impl_event_callback_map[ITE_FOTA].callback_user;
    call_msg.arg_cnt      = 2;
    call_msg.arg_value[0] = (void*)type;
    call_msg.arg_value[0] = (void*)user_ptr;
    call_msg.has_ret      = 1;

    stat = cb_call_buf_queue_push(send_queue, &call_msg);
    if (stat != RHINO_SUCCESS) {
        ret = -2;
        goto out;
    }

    stat = cb_ret_buf_queue_pop(recv_queue, &ret_msg, &size);
    if (stat != RHINO_SUCCESS) {
        ret = -3;
        goto out;
    }

    ret = (int)ret_msg.ret_val;

out:
    /* free user space data buffer */
    if (NULL != user_ptr) {
        res_free(active_pid, user_ptr);
    }

    return ret;
}

static int ITE_COTA_cb(const int type, const char *config_id, int config_size, const char *get_type,
                       const char *sign, const char *sign_method, const char *url)
{
    return 0;
}

static int ITE_MQTT_CONNECT_SUCC_cb(void)
{
    return 0;
}

static int ITE_CLOUD_ERROR_cb(const int code, const char *data, const char *detail)
{
    kstat_t       stat;
    cb_call_msg_t call_msg;
    cb_ret_msg_t  ret_msg;
    size_t        size;
    int           ret;
    char          *data_ptr = NULL;
    char          *detail_ptr = NULL;

    if (NULL != data) {
        data_ptr = (char*)res_malloc(active_pid, strlen(data) + 1);
        if (NULL == data_ptr) {
            ret = -1;
            goto out;
        }
        /* call user space callback func */
        memcpy(data_ptr, data, strlen(data));
        data_ptr[strlen(data)] = 0;
    }

    if (NULL != detail) {
        detail_ptr = (char*)res_malloc(active_pid, strlen(detail) + 1);
        if (NULL == detail_ptr) {
            ret = -2;
            goto out;
        }
        /* call user space callback func */
        memcpy(detail_ptr, detail, strlen(detail));
        detail_ptr[strlen(detail)] = 0;
    }

    call_msg.func_ptr     = g_impl_event_callback_map[ITE_INITIALIZE_COMPLETED].callback_user;
    call_msg.arg_cnt      = 3;
    call_msg.arg_value[0] = (void*)code;
    call_msg.arg_value[0] = (void*)data_ptr;
    call_msg.arg_value[0] = (void*)detail_ptr;
    call_msg.has_ret      = 1;

    stat = cb_call_buf_queue_push(send_queue, &call_msg);
    if (stat != RHINO_SUCCESS) {
        ret = -3;
        goto out;
    }

    stat = cb_ret_buf_queue_pop(recv_queue, &ret_msg, &size);
    if (stat != RHINO_SUCCESS) {
        ret = -4;
        goto out;
    }

    ret = (int)ret_msg.ret_val;

out:
    if (NULL != data_ptr) {
        res_free(active_pid, data_ptr);
    }

    if (NULL != detail_ptr) {
        res_free(active_pid, detail_ptr);
    }

    return ret;
}

static impl_callback_map_t g_impl_event_callback_map[] = {
    {ITE_AWSS_STATUS,          NULL},
    {ITE_CONNECT_SUCC,         NULL},
    {ITE_CONNECT_FAIL,         NULL},
    {ITE_DISCONNECTED,         NULL},
    {ITE_RAWDATA_ARRIVED,      NULL},
    {ITE_SERVICE_REQUEST,      NULL},
    {ITE_SERVICE_REQUEST_EXT,  NULL},
    {ITE_PROPERTY_SET,         NULL},
    {ITE_PROPERTY_GET,         NULL},
#ifdef DEVICE_MODEL_SHADOW
    {ITE_PROPERTY_DESIRED_GET_REPLY,    NULL},
#endif
    {ITE_REPORT_REPLY,         NULL},
    {ITE_TRIGGER_EVENT_REPLY,  NULL},
    {ITE_TIMESTAMP_REPLY,      NULL},
    {ITE_TOPOLIST_REPLY,       NULL},
    {ITE_PERMIT_JOIN,          NULL},
    {ITE_INITIALIZE_COMPLETED, NULL},
    {ITE_FOTA,                 NULL},
    {ITE_COTA,                 NULL},
    {ITE_MQTT_CONNECT_SUCC,    NULL},
    {ITE_CLOUD_ERROR,          NULL},
};

int sys_IOT_RegisterCallback_stub(void *arg)
{
    int ret;
    IOT_RegisterCallback_syscall_arg_t *_arg;

    _arg = (IOT_RegisterCallback_syscall_arg_t*)arg;

    if (_arg->event >= sizeof(g_impl_event_callback_map)/sizeof(impl_callback_map_t)) {
        return -1;
    }

    g_impl_event_callback_map[_arg->event].callback_user = (void*)_arg->callback;

    switch (_arg->event) {
        case ITE_AWSS_STATUS:
            ret = IOT_RegisterCallback(ITE_AWSS_STATUS, ITE_AWSS_STATUS_cb);
            break;
        case ITE_CONNECT_SUCC:
            ret = IOT_RegisterCallback(ITE_CONNECT_SUCC, ITE_CONNECT_SUCC_cb);
            break;
        case ITE_CONNECT_FAIL:
            ret = IOT_RegisterCallback(ITE_CONNECT_FAIL, ITE_CONNECT_FAIL_cb);
            break;
        case ITE_DISCONNECTED:
            ret = IOT_RegisterCallback(ITE_DISCONNECTED, ITE_DISCONNECTED_cb);
            break;
        case ITE_RAWDATA_ARRIVED:
            ret = IOT_RegisterCallback(ITE_RAWDATA_ARRIVED, ITE_RAWDATA_ARRIVED_cb);
            break;
        case ITE_SERVICE_REQUEST:
            ret = IOT_RegisterCallback(ITE_SERVICE_REQUEST, ITE_SERVICE_REQUEST_cb);
            break;
        case ITE_PROPERTY_SET:
            ret = IOT_RegisterCallback(ITE_PROPERTY_SET, ITE_PROPERTY_SET_cb);
            break;
        case ITE_PROPERTY_GET:
            ret = IOT_RegisterCallback(ITE_PROPERTY_GET, ITE_PROPERTY_GET_cb);
            break;
#ifdef DEVICE_MODEL_SHADOW
        case ITE_PROPERTY_DESIRED_GET_REPLY:
            ret = IOT_RegisterCallback(ITE_PROPERTY_DESIRED_GET_REPLY, ITE_PROPERTY_DESIRED_GET_REPLY_cb);
            break;
#endif
        case ITE_REPORT_REPLY:
            ret = IOT_RegisterCallback(ITE_REPORT_REPLY, ITE_REPORT_REPLY_cb);
            break;
        case ITE_TRIGGER_EVENT_REPLY:
            ret = IOT_RegisterCallback(ITE_TRIGGER_EVENT_REPLY, ITE_TRIGGER_EVENT_REPLY_cb);
            break;
        case ITE_TIMESTAMP_REPLY:
            ret = IOT_RegisterCallback(ITE_TIMESTAMP_REPLY, ITE_TIMESTAMP_REPLY_cb);
            break;
        case ITE_TOPOLIST_REPLY:
            ret = IOT_RegisterCallback(ITE_TOPOLIST_REPLY, ITE_TOPOLIST_REPLY_cb);
            break;
        case ITE_PERMIT_JOIN:
            ret = IOT_RegisterCallback(ITE_PERMIT_JOIN, ITE_PERMIT_JOIN_cb);
            break;
        case ITE_INITIALIZE_COMPLETED:
            ret = IOT_RegisterCallback(ITE_INITIALIZE_COMPLETED, ITE_INITIALIZE_COMPLETED_cb);
            break;
        case ITE_FOTA:
            ret = IOT_RegisterCallback(ITE_FOTA, ITE_FOTA_cb);
            break;
        case ITE_COTA:
            ret = IOT_RegisterCallback(ITE_COTA, ITE_COTA_cb);
            break;
        case ITE_MQTT_CONNECT_SUCC:
            ret = IOT_RegisterCallback(ITE_MQTT_CONNECT_SUCC, ITE_MQTT_CONNECT_SUCC_cb);
            break;
        case ITE_CLOUD_ERROR:
            ret = IOT_RegisterCallback(ITE_CLOUD_ERROR, ITE_CLOUD_ERROR_cb);
            break;
        default:
            ret = -2;
            break;
    }

    return ret;
}

int linkkit_close(int pid)
{
    task_group_t *group;

    CPSR_ALLOC();

    group = task_group_get_by_pid(pid);
    if (NULL == group) {
        return -1;
    }

    if (group->linkkit_active == 0) {
        LDEBUG("linkkit is not active\r\n");
        return 0;
    }

    IOT_Linkkit_Close(active_devid);
    RHINO_CRITICAL_ENTER();
    recv_queue = NULL;
    send_queue = NULL;
    active_pid = 0;
    active_devid = -1;
    group->linkkit_active = 0;
    RHINO_CRITICAL_EXIT();

    return 0;
}

#endif /* RHINO_CONFIG_LINKKIT_SYSCALL */


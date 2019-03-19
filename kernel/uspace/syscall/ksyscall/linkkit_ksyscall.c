/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_config.h>
#include <linkkit_syscall_arg.h>
#include <linkkit_ioctl.h>
#include <k_api.h>
#include <u_task.h>
#include <u_callback.h>
#include <u_res.h>

//#define LINKKIT_DEBUG

#ifdef LINKKIT_DEBUG
#include <aos/log.h>
#define LDEBUG(...) LOG(__VA_ARGS__)
#else
#define LDEBUG(...)
#endif

typedef struct {
    linkkit_ops_t *linkkit_ops;
    kbuf_queue_t  *send_queue;
    kbuf_queue_t  *recv_queue;
    int            active_pid;
} linkkit_cb_agent_t;

linkkit_cb_agent_t cb_agent = {NULL, NULL, NULL, -1};

int linkkit_invoke_uspace_post_cb(void *f, const void *thing_id, int respons_id, int code,
                                  const char *response_message, void *ctx)
{
    kstat_t stat;
    cb_call_msg_t call_msg;
    cb_ret_msg_t ret_msg;
    size_t size;
    size_t str_len;
    char *user_ptr = NULL;
    int ret;

    LDEBUG("callback\r\n");

    if (NULL != response_message) {
        str_len = strlen(response_message) + 1;
        user_ptr = (char *)u_res_malloc(cb_agent.active_pid, str_len);
        if (NULL == user_ptr) {
            return -2;
        }
        memset(user_ptr, 0, str_len);
        memcpy((void*)user_ptr, response_message, strlen(response_message));
    }

    call_msg.magic        = MSG_CALL_MAGIC;
    call_msg.func_ptr     = f;
    call_msg.arg_cnt      = 5;
    call_msg.arg_value[0] = (uint32_t)thing_id;
    call_msg.arg_value[1] = (uint32_t)respons_id;
    call_msg.arg_value[2] = (uint32_t)code;
    call_msg.arg_value[3] = (uint32_t)user_ptr;
    call_msg.arg_value[4] = (uint32_t)ctx;
    call_msg.has_ret      = 1;

    stat = cb_call_buf_queue_push(cb_agent.send_queue, &call_msg);
    if (stat != RHINO_SUCCESS) {
        LDEBUG("push msg failed, ret 0x%x\r\n", stat);
        return -3;
    }

    stat = cb_ret_buf_queue_pop(cb_agent.recv_queue, &ret_msg, &size);
    if (stat != RHINO_SUCCESS) {
        LDEBUG("pop msg failed, ret 0x%x\r\n", stat);
        return -4;
    }

    ret = (int)ret_msg.ret_val;

    if (response_message != NULL) {
        u_res_free(cb_agent.active_pid, user_ptr);
    }

    return ret;
}

int linkkit_invoke_uspace_subdev_cb(void *f, const void *thing_id, int code,
                                  const char *response_message, int success, void *ctx)
{
    kstat_t stat;
    cb_call_msg_t call_msg;
    cb_ret_msg_t ret_msg;
    size_t size;
    size_t str_len;
    char *user_ptr = NULL;
    int ret;

    LDEBUG("callback\r\n");

    if (NULL != response_message) {
        str_len = strlen(response_message) + 1;
        user_ptr = u_res_malloc(cb_agent.active_pid, str_len);
        if (NULL == user_ptr) {
            return -2;
        }

        memset((void*)user_ptr, 0, str_len);
        memcpy((void*)user_ptr, response_message, strlen(response_message));
    }

    call_msg.magic        = MSG_CALL_MAGIC;
    call_msg.func_ptr     = f;
    call_msg.arg_cnt      = 5;
    call_msg.arg_value[0] = (uint32_t)thing_id;
    call_msg.arg_value[1] = (uint32_t)code;
    call_msg.arg_value[2] = (uint32_t)user_ptr;
    call_msg.arg_value[3] = (uint32_t)success;
    call_msg.arg_value[4] = (uint32_t)ctx;
    call_msg.has_ret      = 1;

    stat = cb_call_buf_queue_push(cb_agent.send_queue, &call_msg);
    if (stat != RHINO_SUCCESS) {
        LDEBUG("push msg failed, ret 0x%x\r\n", stat);
        return -3;
    }

    stat = cb_ret_buf_queue_pop(cb_agent.recv_queue, &ret_msg, &size);
    if (stat != RHINO_SUCCESS) {
        LDEBUG("pop msg failed, ret 0x%x\r\n", stat);
        return -4;
    }

    ret = (int)ret_msg.ret_val;

    if (NULL != response_message) {
        u_res_free(cb_agent.active_pid, user_ptr);
    }

    return ret;
}

#ifdef LOCAL_CONN_ENABLE
int on_connect(void *ctx, int cloud)
{
    kstat_t stat;
    cb_call_msg_t call_msg;
    cb_ret_msg_t ret_msg;
    size_t size;

    LDEBUG("callback\r\n");

    call_msg.magic        = MSG_CALL_MAGIC;
    call_msg.func_ptr     = cb_agent.linkkit_ops->on_connect;
    call_msg.arg_cnt      = 2;
    call_msg.arg_value[0] = (uint32_t)ctx;
    call_msg.arg_value[1] = (uint32_t)cloud;
    call_msg.has_ret      = 1;

    stat = cb_call_buf_queue_push(cb_agent.send_queue, &call_msg);
    if (stat != RHINO_SUCCESS) {
        LDEBUG("push resv msg failed, ret 0x%x\r\n", stat);
        return -2;
    }

    stat = cb_ret_buf_queue_pop(cb_agent.recv_queue, &ret_msg, &size);
    if (stat != RHINO_SUCCESS) {
        LDEBUG("pop resv msg failed, ret 0x%x\r\n", stat);
        return -3;
    }

    return (int)ret_msg.ret_val;
}

int on_disconnect(void *ctx, int cloud)
{
    kstat_t stat;
    cb_call_msg_t call_msg;
    cb_ret_msg_t ret_msg;
    size_t size;

    LDEBUG("callback\r\n");

    call_msg.magic        = MSG_CALL_MAGIC;
    call_msg.func_ptr     = cb_agent.linkkit_ops->on_disconnect;
    call_msg.arg_cnt      = 2;
    call_msg.arg_value[0] = (uint32_t)ctx;
    call_msg.arg_value[1] = (uint32_t)cloud;
    call_msg.has_ret      = 1;

    stat = cb_call_buf_queue_push(cb_agent.send_queue, &call_msg);
    if (stat != RHINO_SUCCESS) {
        LDEBUG("push msg failed, ret 0x%x\r\n", stat);
        return -2;
    }

    stat = cb_ret_buf_queue_pop(cb_agent.recv_queue, &ret_msg, &size);
    if (stat != RHINO_SUCCESS) {
        LDEBUG("pop msg failed, ret 0x%x\r\n", stat);
        return -3;
    }

    return (int)ret_msg.ret_val;
}
#else
int on_connect(void *ctx)
{
    kstat_t stat;
    cb_call_msg_t call_msg;
    cb_ret_msg_t ret_msg;
    size_t size;

    LDEBUG("callback\r\n");

    call_msg.magic        = MSG_CALL_MAGIC;
    call_msg.func_ptr     = cb_agent.linkkit_ops->on_connect;
    call_msg.arg_cnt      = 1;
    call_msg.arg_value[0] = (uint32_t)ctx;
    call_msg.has_ret      = 1;

    stat = cb_call_buf_queue_push(cb_agent.send_queue, &call_msg);
    if (stat != RHINO_SUCCESS) {
        LDEBUG("push msg failed, ret 0x%x\r\n", stat);
        return -2;
    }

    stat = cb_ret_buf_queue_pop(cb_agent.recv_queue, &ret_msg, &size);
    if (stat != RHINO_SUCCESS) {
        LDEBUG("pop msg failed, ret 0x%x\r\n", stat);
        return -3;
    }

    return (int)ret_msg.ret_val;
}

int on_disconnect(void *ctx)
{
    kstat_t stat;
    cb_call_msg_t call_msg;
    cb_ret_msg_t ret_msg;
    size_t size;

    LDEBUG("callback\r\n");

    call_msg.magic        = MSG_CALL_MAGIC;
    call_msg.func_ptr     = cb_agent.linkkit_ops->on_disconnect;
    call_msg.arg_cnt      = 1;
    call_msg.arg_value[0] = (uint32_t)ctx;
    call_msg.has_ret      = 1;

    stat = cb_call_buf_queue_push(cb_agent.send_queue, &call_msg);
    if (stat != RHINO_SUCCESS) {
        LDEBUG("push msg failed, ret 0x%x\r\n", stat);
        return -2;
    }

    stat = cb_ret_buf_queue_pop(cb_agent.recv_queue, &ret_msg, &size);
    if (stat != RHINO_SUCCESS) {
        LDEBUG("pop msg failed, ret 0x%x\r\n", stat);
        return -3;
    }

    return (int)ret_msg.ret_val;
}
#endif

int raw_data_arrived(const void *thing_id, const void *data, int len, void *ctx)
{
    kstat_t stat;
    kbuf_queue_t *request_q;
    kbuf_queue_t *reply_q;
    cb_call_msg_t call_msg;
    cb_ret_msg_t ret_msg;
    void *user_ptr = NULL;
    size_t size;
    int ret;

    LDEBUG("callback\r\n");

    /* alloc user space data buffer */
    if (NULL != data) {
    	memset(&call_msg, 0, sizeof(cb_call_msg_t));
    	user_ptr = u_res_malloc(cb_agent.active_pid, len);
        if (NULL == user_ptr) {
            LDEBUG("get user buf failee\r\n");
            return -2;
        }

        /* call user space callback func */
        memcpy(user_ptr, data, len);
    }

    memset(&call_msg, 0, sizeof(cb_call_msg_t));
    call_msg.magic        = MSG_CALL_MAGIC;
    call_msg.func_ptr     = (void *)cb_agent.linkkit_ops->raw_data_arrived;
    call_msg.arg_cnt      = 4;
    call_msg.arg_value[0] = (uint32_t)thing_id;
    call_msg.arg_value[1] = (uint32_t)user_ptr;
    call_msg.arg_value[2] = (uint32_t)len;
    call_msg.arg_value[3] = (uint32_t)ctx;
    call_msg.has_ret      = 1;

    stat = cb_call_buf_queue_push(cb_agent.send_queue, &call_msg);
    if (stat != RHINO_SUCCESS) {
        LDEBUG("push msg failed, ret 0x%x\r\n", stat);
        return -3;
    }

    memset(&ret_msg, 0, sizeof(cb_ret_msg_t));
    stat = cb_ret_buf_queue_pop(cb_agent.recv_queue, &ret_msg, &size);
    if (stat != RHINO_SUCCESS) {
        LDEBUG("pop msg failed, ret 0x%x\r\n", stat);
        return -4;
    }
    ret = (int)ret_msg.ret_val;

    /* free user space data buffer */
    if (NULL != data) {
        u_res_free(cb_agent.active_pid, user_ptr);
    }

    return ret;
}

int thing_create(const void *thing_id, void *ctx)
{
    kstat_t stat;
    cb_call_msg_t call_msg;
    cb_ret_msg_t ret_msg;
    size_t size;

    LDEBUG("callback\r\n", __func__);

    call_msg.magic        = MSG_CALL_MAGIC;
    call_msg.func_ptr     = cb_agent.linkkit_ops->thing_create;
    call_msg.arg_cnt      = 2;
    call_msg.arg_value[0] = (uint32_t)thing_id;
    call_msg.arg_value[1] = (uint32_t)ctx;
    call_msg.has_ret      = 1;

    stat = cb_call_buf_queue_push(cb_agent.send_queue, &call_msg);
    if (stat != RHINO_SUCCESS) {
        LDEBUG("push msg failed, ret 0x%x\r\n",stat);
        return -2;
    }

    stat = cb_ret_buf_queue_pop(cb_agent.recv_queue, &ret_msg, &size);
    if (stat != RHINO_SUCCESS) {
        LDEBUG("pop msg failed, ret 0x%x\r\n", stat);
        return -3;
    }

    return (int)ret_msg.ret_val;
}

int thing_enable(const void *thing_id, void *ctx)
{
    kstat_t stat;
    kbuf_queue_t *call_buf_queue;
    kbuf_queue_t *ret_buf_queue;
    cb_call_msg_t call_msg;
    cb_ret_msg_t ret_msg;
    size_t size;

    LDEBUG("callback\r\n");

    call_msg.magic        = MSG_CALL_MAGIC;
    call_msg.func_ptr     = cb_agent.linkkit_ops->thing_enable;
    call_msg.arg_cnt      = 2;
    call_msg.arg_value[0] = (uint32_t)thing_id;
    call_msg.arg_value[1] = (uint32_t)ctx;
    call_msg.has_ret      = 1;

    stat = cb_call_buf_queue_push(cb_agent.send_queue, &call_msg);
    if (stat != RHINO_SUCCESS) {
        LDEBUG("push msg failed, ret 0x%x\r\n", stat);
        return -2;
    }

    stat = cb_ret_buf_queue_pop(cb_agent.recv_queue, &ret_msg, &size);
    if (stat != RHINO_SUCCESS) {
        LDEBUG("pop msg failed, ret 0x%x\r\n", stat);
        return -3;
    }

    return (int)ret_msg.ret_val;
}

int thing_disable(const void *thing_id, void *ctx)
{
    kstat_t stat;
    cb_call_msg_t call_msg;
    cb_ret_msg_t ret_msg;
    size_t size;

    LDEBUG("callback\r\n");

    call_msg.magic        = MSG_CALL_MAGIC;
    call_msg.func_ptr     = cb_agent.linkkit_ops->thing_disable;
    call_msg.arg_cnt      = 2;
    call_msg.arg_value[0] = (uint32_t)thing_id;
    call_msg.arg_value[1] = (uint32_t)ctx;
    call_msg.has_ret      = 1;

    stat = cb_call_buf_queue_push(cb_agent.send_queue, &call_msg);
    if (stat != RHINO_SUCCESS) {
        LDEBUG("push msg failed, ret 0x%x\r\n", stat);
        return -2;
    }

    stat = cb_ret_buf_queue_pop(cb_agent.recv_queue, &ret_msg, &size);
    if (stat != RHINO_SUCCESS) {
        LDEBUG("pop msg failed, ret 0x%x\r\n", stat);
        return -3;
    }

    return (int)ret_msg.ret_val;
}


//TODO: copy the service str to user space
int thing_call_service(const void *thing_id, const char *service, int request_id, void *ctx)
{
    kstat_t stat;
    cb_call_msg_t call_msg;
    cb_ret_msg_t ret_msg;
    void *user_ptr = NULL;
    size_t str_len;
    size_t size;
    int ret;

    LDEBUG("callback\r\n");

    if (NULL != service) {
        str_len = strlen(service) + 1;
        user_ptr = u_res_malloc(cb_agent.active_pid, str_len);
        if (NULL == user_ptr) {
            LDEBUG("get user buf failee\r\n");
            return -2;
        }
        memset(user_ptr, 0, str_len);
        memcpy((void*)user_ptr, service, strlen(service));
    }

    call_msg.magic        = MSG_CALL_MAGIC;
    call_msg.func_ptr     = cb_agent.linkkit_ops->thing_call_service;
    call_msg.arg_cnt      = 4;
    call_msg.arg_value[0] = (uint32_t)thing_id;
    call_msg.arg_value[1] = (uint32_t)user_ptr;
    call_msg.arg_value[2] = (uint32_t)request_id;
    call_msg.arg_value[3] = (uint32_t)ctx;
    call_msg.has_ret      = 1;

    stat = cb_call_buf_queue_push(cb_agent.send_queue, &call_msg);
    if (stat != RHINO_SUCCESS) {
        LDEBUG("push msg failed, ret 0x%x\r\n", stat);
        return -3;
    }

    stat = cb_ret_buf_queue_pop(cb_agent.recv_queue, &ret_msg, &size);
    if (stat != RHINO_SUCCESS) {
        LDEBUG("pop msg failed, ret 0x%x\r\n", stat);
        return -4;
    }

    ret = (int)ret_msg.ret_val;

    if (NULL != service) {
        u_res_free(cb_agent.active_pid, user_ptr);
    }

    return ret;
}

//TODO: copy property str to user space, check where does the property str come from
int thing_prop_changed(const void *thing_id, const char *property, void *ctx)
{
    kstat_t stat;
    cb_call_msg_t call_msg;
    cb_ret_msg_t ret_msg;
    void *user_ptr = NULL;
    size_t str_len;
    size_t size;
    int ret;

    LDEBUG("callback\r\n");

    if (NULL != property) {
        str_len = strlen(property) + 1;
        user_ptr = u_res_malloc(cb_agent.active_pid, str_len);
        if (NULL == user_ptr) {
            LDEBUG("get user buf failee\r\n");
            return -2;
        }

        memset(user_ptr, 0, str_len);
        memcpy((void*)user_ptr, property, strlen(property));
    }

    call_msg.magic        = MSG_CALL_MAGIC;
    call_msg.func_ptr     = cb_agent.linkkit_ops->thing_prop_changed;
    call_msg.arg_cnt      = 3;
    call_msg.arg_value[0] = (uint32_t)thing_id;
    call_msg.arg_value[1] = (uint32_t)user_ptr;
    call_msg.arg_value[2] = (uint32_t)ctx;
    call_msg.has_ret      = 1;

    stat = cb_call_buf_queue_push(cb_agent.send_queue, &call_msg);
    if (stat != RHINO_SUCCESS) {
        LDEBUG("push msg failed, ret 0x%x\r\n", stat);
        return -3;
    }

    stat = cb_ret_buf_queue_pop(cb_agent.recv_queue, &ret_msg, &size);
    if (stat != RHINO_SUCCESS) {
        LDEBUG("pop msg failed, ret 0x%x\r\n", stat);
        return -4;
    }

    ret = (int)ret_msg.ret_val;

    if (NULL != property) {
        u_res_free(cb_agent.active_pid, user_ptr);
    }

    return ret;
}

int linkit_data_arrived(const void *thing_id, const void *data, int len, void *ctx)
{
    kstat_t stat;
    cb_call_msg_t call_msg;
    cb_ret_msg_t ret_msg;
    size_t size;
    void *user_ptr = NULL;
    int ret;

    LDEBUG("callback\r\n");

    if (NULL != data) {
        user_ptr = u_res_malloc(cb_agent.active_pid, len);
        if (NULL == user_ptr) {
            LDEBUG("get user buf failee\r\n");
            return -2;
        }

        memcpy((void*)user_ptr, data, len);
    }

    memset(&call_msg, 0, sizeof(cb_call_msg_t));
    call_msg.magic        = MSG_CALL_MAGIC;
    call_msg.func_ptr     = (void *)cb_agent.linkkit_ops->linkit_data_arrived;
    call_msg.arg_cnt      = 4;
    call_msg.arg_value[0] = (uint32_t)thing_id;
    call_msg.arg_value[1] = (uint32_t)user_ptr;
    call_msg.arg_value[2] = (uint32_t)len;
    call_msg.arg_value[3] = (uint32_t)ctx;
    call_msg.has_ret      = 1;

    stat = cb_call_buf_queue_push(cb_agent.send_queue, &call_msg);
    if (stat != RHINO_SUCCESS) {
        LDEBUG("push msg failed, ret 0x%x\r\n", stat);
        return -3;
    }

    memset(&ret_msg, 0, sizeof(cb_ret_msg_t));
    stat = cb_ret_buf_queue_pop(cb_agent.recv_queue, &ret_msg, &size);
    if (stat != RHINO_SUCCESS) {
        LDEBUG("pop msg failed, ret 0x%x\r\n", stat);
        return -4;
    }

    ret = (int)ret_msg.ret_val;

    if (NULL != data) {
        u_res_free(cb_agent.active_pid, user_ptr);
    }

    return ret;
}

static linkkit_ops_t linkkit_ops = {
        .on_connect          = on_connect,
        .on_disconnect       = on_disconnect,
        .raw_data_arrived    = raw_data_arrived,
        .thing_create        = thing_create,
        .thing_enable        = thing_enable,
        .thing_disable       = thing_disable,
        .thing_call_service  = thing_call_service,
        .thing_prop_changed  = thing_prop_changed,
        .linkit_data_arrived = linkit_data_arrived,
};


int sys_linkkit_start_stub(void *arg)
{
    ktask_t *cur_task;
    task_group_t *group;
    linkkit_start_syscall_arg_t *_arg = arg;
    int ret;

    LDEBUG("syscall\r\n");

    ret = linkkit_start(_arg->max_buffered_msg,
                         _arg->get_tsl_from_cloud,
                         _arg->log_level,
                         &linkkit_ops,
                         _arg->domain_type,
                         _arg->user_context);

    if (SUCCESS_RETURN == ret) {
        cur_task = krhino_cur_task_get();
        group = get_task_group_by_pid(cur_task->pid);
        cb_agent.active_pid  = cur_task->pid;
        cb_agent.linkkit_ops = _arg->ops;
        cb_agent.send_queue  = group->cb_call_buf_queue;
        cb_agent.recv_queue  = group->cb_ret_buf_queue;
        group->linkkit_running = 1;
    }

    return ret;
}

//TODO: where to free agent???
int sys_linkkit_end_stub(void *arg)
{
    int ret;
    ktask_t *cur_task;
    task_group_t *group;

    //arg = arg;

    LDEBUG("syscall\r\n");

    ret = linkkit_end();
    if (SUCCESS_RETURN == ret) {
        cur_task = krhino_cur_task_get();
        group = get_task_group_by_pid(cur_task->pid);
        group->linkkit_running = 0;
        cb_agent.active_pid  = -1;
        cb_agent.linkkit_ops = NULL;
        cb_agent.send_queue  = NULL;
        cb_agent.recv_queue  = NULL;

    }

    return ret;
}

void* sys_linkkit_dispatch_stub(void *arg)
{
    arg = arg;

    //LDEBUG("syscall\r\n");
    return linkkit_dispatch();
}

int sys_linkkit_yield_stub(void *arg)
{
    linkkit_yield_syscall_arg_t *_arg = arg;

    //LDEBUG("syscall\r\n");
    return linkkit_yield(_arg->timeout_ms);
}

// it's safe on qualcomm platform
int sys_linkkit_set_value_stub(void *arg)
{
    linkkit_set_value_syscall_arg_t *_arg = arg;

    LDEBUG("syscall\r\n");
    return linkkit_set_value(_arg->method_set, _arg->thing_id, _arg->identifier,
                             _arg->value, _arg->value_str);
}

int sys_linkkit_get_value_stub(void *arg)
{
    linkkit_get_value_syscall_arg_t *_arg = arg;
    kbuf_queue_t *request_q;
    kbuf_queue_t *reply_q;
    size_t str_len;
    void *user_ptr;
    void *ctx;
    int ret;

    LDEBUG("syscall\r\n");
    ret = linkkit_get_value(_arg->method_get, _arg->thing_id,
                             _arg->identifier, _arg->value,
                             _arg->value_str);

    if ((SUCCESS_RETURN == ret) && (NULL != _arg->value_str) && (NULL != *(_arg->value_str))) {
        str_len = strlen(*_arg->value_str) + 1;
        user_ptr = u_res_malloc(cb_agent.active_pid, str_len);

        memset(user_ptr, 0, str_len);
        memcpy(user_ptr, (void*)(*(_arg->value_str)), strlen(*(_arg->value_str)));
        krhino_mm_free(*(_arg->value_str));
        *(_arg->value_str) = (char*)user_ptr;
    }

    return ret;
}

// it's okay on qualcomm platform
void *sys_linkkit_set_tsl_stub(void *arg)
{
    linkkit_set_tsl_syscall_arg_t *_arg = arg;

    LDEBUG("syscall\r\n");
    return linkkit_set_tsl(_arg->tsl, _arg->tsl_len);
}

// it's okay on qualcomm platform
int sys_linkkit_answer_service_stub(void *arg)
{
    linkkit_answer_service_syscall_arg_t *_arg = arg;

    LDEBUG("syscall\r\n");
    return linkkit_answer_service(_arg->thing_id, _arg->service_identifier,
                                  _arg->response_id, _arg->code);
}

// it's synchronous, safe here
int sys_linkkit_invoke_raw_service_stub(void *arg)
{
    linkkit_invoke_raw_service_syscall_arg_t *_arg = arg;

    LDEBUG("syscall\r\n");
    return linkkit_invoke_raw_service(_arg->thing_id, _arg->is_up_raw,
                                      _arg->raw_data, _arg->raw_data_length);
}

// added linkkit_invoke_uspace_post_cb at where the cb is invoked
int sys_linkkit_trigger_event_stub(void *arg)
{
    linkkit_trigger_event_syscall_arg_t *_arg = arg;

    LDEBUG("syscall\r\n");
    return linkkit_trigger_event(_arg->thing_id, _arg->event_identifier,
                                 _arg->cb);
}

int sys_linkkit_post_property_stub(void *arg)
{
    linkkit_post_property_syscall_arg_t *_arg = arg;

    LDEBUG("syscall\r\n");
    return linkkit_post_property(_arg->thing_id, _arg->property_identifier, _arg->cb);
}

// it's safe on qualcomm platform
int sys_linkkit_set_opt_stub(void *arg)
{
    linkkit_set_opt_syscall_arg_t *_arg = arg;

    LDEBUG("syscall\r\n");
    return linkkit_set_opt(_arg->opt, _arg->data);
}

void sys_linkkit_try_leave_stub(void *arg)
{
    arg = arg;

    //LDEBUG("syscall\r\n");
    return linkkit_try_leave();
}

int sys_linkkit_is_try_leave_stub(void *arg)
{
    arg = arg;

    //LDEBUG("syscall\r\n");
    return linkkit_is_try_leave();
}

int sys_linkkit_is_end_stub(void *arg)
{
    arg = arg;

    LDEBUG("syscall\r\n");
    return linkkit_is_end();
}

// it's safe on qualcomm platform
int sys_linkkit_trigger_xinfo_operate_stub(void *arg)
{
    linkkit_trigger_xinfo_operate_syscall_arg_t *_arg = arg;

    LDEBUG("syscall\r\n");
    return linkkit_trigger_extended_info_operate(_arg->thing_id,
                                                 _arg->params,
                                                 _arg->linkkit_xinfo_operation);
}

int sys_linkkit_ioctl_stub(void *arg)
{
    linkkit_ioctl_syscall_arg_t *_arg = arg;
    void *param;
    int cmd;
    int ret = 0;

    cmd = _arg->cmd;
    param = _arg->arg;

    switch(cmd) {
        case IOCTL_SET_PRODUCT_KEY:
            HAL_SetProductKey((char*)param);
            break;
        case IOCTL_GET_PRODUCT_KEY:
            HAL_GetProductKey((char*)param);
            break;
        case IOCTL_SET_PRODUCT_SECRET:
            HAL_SetProductSecret((char*)param);
            break;
        case IOCTL_GET_PRODUCT_SECRET:
            HAL_GetProductSecret((char*)param);
            break;
        case IOCTL_SET_DEVICE_NAME:
            HAL_SetDeviceName((char*)param);
            break;
        case IOCTL_GET_DEVICE_NAME:
            HAL_GetDeviceName((char*)param);
            break;
        case IOCTL_SET_DEVICE_SECRET:
            HAL_SetDeviceSecret((char*)param);
            break;
        case IOCTL_GET_DEVICE_SECRET:
            HAL_GetDeviceSecret((char*)param);
            break;
        case IOCTL_OPEN_LOG:
            IOT_OpenLog((const char*)param);
            break;
        case IOCTL_CLOSE_LOG:
            IOT_CloseLog();
            break;
        case IOCTL_SET_LOG_LEVEL:
            IOT_SetLogLevel((IOT_LogLevel)param);
            break;

        default:
            ret = -1;
            break;
    }

    return ret;
}

int linkkit_close(int pid)
{
    task_group_t *group;
    int           ret;

    group = get_task_group_by_pid(pid);
    if (NULL == group) {
        return -1;
    }

    if (group->linkkit_running) {
        linkkit_try_leave();
        ret = linkkit_end();
        if (SUCCESS_RETURN == ret) {
            group->linkkit_running = 0;
            cb_agent.active_pid  = -1;
            cb_agent.linkkit_ops = NULL;
            cb_agent.send_queue  = NULL;
            cb_agent.recv_queue  = NULL;
        }
    }

    return ret;
}


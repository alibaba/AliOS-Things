/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <arch_syscall.h>
#include <linkkit_syscall_arg.h>
#include <syscall_no.h>

int linkkit_start(int max_buffered_msg,
                  int get_tsl_from_cloud,
                  linkkit_loglevel_t log_level,
                  linkkit_ops_t *ops,
                  linkkit_cloud_domain_type_t domain_type,
                  void *user_context)
{
    linkkit_start_syscall_arg_t _arg;

    _arg.max_buffered_msg   = max_buffered_msg;
    _arg.get_tsl_from_cloud = get_tsl_from_cloud;
    _arg.log_level          = log_level;
    _arg.ops                = ops;
    _arg.domain_type        = domain_type;
    _arg.user_context       = user_context;

    return (int)SYSCALL(SYS_LINKKIT_START, (void*)&_arg);
}

int linkkit_end(void)
{
    return (int)SYSCALL(SYS_LINKKIT_END, NULL);
}

void* linkkit_dispatch(void)
{
    int ret;

    ret = (int)SYSCALL(SYS_LINKKIT_DISPATCH, NULL);

    return (void*)ret;
}

int linkkit_yield(int timeout_ms)
{
    linkkit_yield_syscall_arg_t _arg;

    _arg.timeout_ms = timeout_ms;

    return (int)SYSCALL(SYS_LINKKIT_YIELD, (void*)&_arg);
}


int linkkit_set_value(linkkit_method_set_t method_set,
                      const void *thing_id,
                      const char *identifier,
                      const void *value,
                      const char *value_str)
{
    linkkit_set_value_syscall_arg_t _arg;

    _arg.method_set = method_set;
    _arg.thing_id   = thing_id;
    _arg.identifier = identifier;
    _arg.value      = value;
    _arg.value_str  = value_str;

    return (int)SYSCALL(SYS_LINKKIT_SET_VALUE, (void*)&_arg);
}

int linkkit_get_value(linkkit_method_get_t method_get,
                      const void *thing_id,
                      const char *identifier,
                      void *value,
                      char **value_str)
{
    linkkit_get_value_syscall_arg_t _arg;

    _arg.method_get = method_get;
    _arg.thing_id   = thing_id;
    _arg.identifier = identifier;
    _arg.value      = value;
    _arg.value_str  = value_str;

    return (int)SYSCALL(SYS_LINKKIT_GET_VALUE, (void*)&_arg);

}


void *linkkit_set_tsl(const char *tsl, int tsl_len)
{
    linkkit_set_tsl_syscall_arg_t _arg;
    int ret;

    _arg.tsl     = tsl;
    _arg.tsl_len = tsl_len;

    ret = (int)SYSCALL(SYS_LINKKIT_SET_TSL, (void*)&_arg);

    return (void*)ret;
}


int linkkit_answer_service(const void *thing_id,
                           const char *service_identifier,
                           int response_id,
                           int code)
{
    linkkit_answer_service_syscall_arg_t _arg;

    _arg.thing_id           = thing_id;
    _arg.service_identifier = service_identifier;
    _arg.response_id        = response_id;
    _arg.code               = code;

    return (int)SYSCALL(SYS_LINKKIT_ANSWER_SERVICE, (void*)&_arg);

}


int linkkit_invoke_raw_service(const void *thing_id,
                               int is_up_raw,
                               void *raw_data,
                               int raw_data_length)
{
    linkkit_invoke_raw_service_syscall_arg_t _arg;

    _arg.thing_id        = thing_id;
    _arg.is_up_raw       = is_up_raw;
    _arg.raw_data        = raw_data;
    _arg.raw_data_length = raw_data_length;

    return (int)SYSCALL(SYS_LINKKIT_INVOKE_RAW_SERVICE, (void*)&_arg);

}

int linkkit_trigger_event(const void *thing_id,
                          const char *event_identifier,
                          handle_post_cb_fp_t cb)
{
    linkkit_trigger_event_syscall_arg_t _arg;

    _arg.thing_id         = thing_id;
    _arg.event_identifier = event_identifier;
    _arg.cb               = cb;

    return (int)SYSCALL(SYS_LINKKIT_TRIGGER_EVENT, (void*)&_arg);

}

int linkkit_post_property(const void *thing_id,
                          const char *property_identifier,
                          handle_post_cb_fp_t cb)
{
    linkkit_post_property_syscall_arg_t _arg;

    _arg.thing_id            = thing_id;
    _arg.property_identifier = property_identifier;
    _arg.cb                  = cb;

    return (int)SYSCALL(SYS_LINKKIT_POST_PROPERTY, (void*)&_arg);

}

int linkkit_set_opt(linkkit_opt_t opt, void *data)
{
    linkkit_set_opt_syscall_arg_t _arg;

    _arg.opt  = opt;
    _arg.data = data;

    return (int)SYSCALL(SYS_LINKKIT_SET_OPT, (void*)&_arg);
}

void linkkit_try_leave(void)
{
    SYSCALL(SYS_LINKKIT_TRY_LEAVE, NULL);
}

int linkkit_is_try_leave(void)
{
    return (int)SYSCALL(SYS_LINKKIT_IS_TRY_LEAVE, NULL);
}

int linkkit_is_end(void)
{
    return (int)SYSCALL(SYS_LINKKIT_IS_END, NULL);
}


int linkkit_trigger_extended_info_operate(const void *thing_id, const char *params,
                    linkkit_extended_info_operate_t linkkit_xinfo_operation)
{
    linkkit_trigger_xinfo_operate_syscall_arg_t _arg;

    _arg.thing_id                = thing_id;
    _arg.params                  = params;
    _arg.linkkit_xinfo_operation = linkkit_xinfo_operation;

    return (int)SYSCALL(SYS_LINKKIT_TRIGGER_XINFO_OPERATION, (void*)&_arg);

}

int linkkit_ioctl(int cmd, void *arg)
{
    linkkit_ioctl_syscall_arg_t _arg;

    _arg.cmd = cmd;
    _arg.arg = arg;

    return (int)SYSCALL(SYS_LINKKIT_IOCTL, (void*)&_arg);
}


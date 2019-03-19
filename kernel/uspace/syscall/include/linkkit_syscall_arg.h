/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef lINKKIT_SYSCALL_ARG_H
#define lINKKIT_SYSCALL_ARG_H

#include <iot_export.h>
#include <linkkit_export.h>

typedef struct {
    int max_buffered_msg;
    int get_tsl_from_cloud;
    linkkit_loglevel_t log_level;
    linkkit_ops_t *ops;
    linkkit_cloud_domain_type_t domain_type;
    void *user_context;
} linkkit_start_syscall_arg_t;

typedef struct {
    int timeout_ms;
} linkkit_yield_syscall_arg_t;

typedef struct {
    linkkit_method_set_t method_set;
    const void *thing_id;
    const char *identifier;
    const void *value;
    const char *value_str;
} linkkit_set_value_syscall_arg_t;

typedef struct {
    linkkit_method_get_t method_get;
    const void *thing_id;
    const char *identifier;
    void *value;
    char **value_str;
} linkkit_get_value_syscall_arg_t;

typedef struct {
    const char *tsl;
    int tsl_len;
} linkkit_set_tsl_syscall_arg_t;

typedef struct {
    const void *thing_id;
    const char *service_identifier;
    int response_id;
    int code;
} linkkit_answer_service_syscall_arg_t;

typedef struct {
    const void *thing_id;
    int is_up_raw;
    void *raw_data;
    int raw_data_length;
} linkkit_invoke_raw_service_syscall_arg_t;

typedef struct {
    const void *thing_id;
    const char *event_identifier;
    handle_post_cb_fp_t cb;
} linkkit_trigger_event_syscall_arg_t;

typedef struct {
    const void *thing_id;
    const char *property_identifier;
    handle_post_cb_fp_t cb;
} linkkit_post_property_syscall_arg_t;

typedef struct {
    linkkit_opt_t opt;
    void *data;
} linkkit_set_opt_syscall_arg_t;

typedef struct {
    const void *thing_id;
    const char *params;
    linkkit_extended_info_operate_t linkkit_xinfo_operation;
} linkkit_trigger_xinfo_operate_syscall_arg_t;

/*
typedef struct {
    void (*ntp_reply)(const char *ntp_offset_time_ms);
} linkkit_ntp_time_request_syscall_arg_t;
*/

typedef struct {
    int   cmd;
    void *arg;
} linkkit_ioctl_syscall_arg_t;

#endif /* lINKKIT_SYSCALL_ARG_H */


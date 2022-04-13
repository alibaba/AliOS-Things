/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef AOS_OBJ_SYSCALL_H
#define AOS_OBJ_SYSCALL_H

#include "object_api.h"
#include "aos/kernel.h"

typedef void (*time_fn)(void *, void *);
typedef void (*work_fn)(void *, void *);
/* kernel object common args */
typedef struct {
    obj_handle_t handle;
} aos_obj_comm_syscall_arg_t;

typedef struct {
    obj_handle_t handle;
    int          option;
} aos_obj_delete_syscall_arg_t;

typedef struct {
    const char *name;
} aos_obj_unlink_syscall_arg_t;

/* aos sem object args */
typedef struct {
    aos_hdl_t *phandle;
    uint32_t   count;
} aos_sem_new_syscall_arg_t;

typedef struct {
    aos_hdl_t    *phandle;
    const char   *name;
    int          flag;
    uint32_t     count;
} aos_sem_open_syscall_arg_t;

typedef struct {
    obj_handle_t handle;
    uint32_t  timeout;
} aos_sem_wait_syscall_arg_t;

typedef struct {
    obj_handle_t handle;
    uint32_t    *val;
} aos_sem_getvalue_syscall_arg_t;

/* aos event object args */
typedef struct {
    aos_hdl_t *phandle;
    uint32_t   count;
} aos_event_new_syscall_arg_t;

typedef struct {
    aos_hdl_t    *phandle;
    const char   *name;
    int          flag;
    uint32_t     count;
} aos_event_open_syscall_arg_t;

typedef struct {
    obj_handle_t handle;
    uint32_t value;
    uint8_t  opt;
    uint32_t *actl_value;
    uint32_t timeout;
} aos_event_get_syscall_arg_t;

typedef struct {
    obj_handle_t handle;
    uint32_t value;
    uint8_t  opt;
} aos_event_set_syscall_arg_t;

/* aos mutex object args */
typedef struct {
    aos_hdl_t *phandle;
} aos_mutex_new_syscall_arg_t;


typedef struct {
    obj_handle_t handle;
    uint32_t  timeout;
} aos_mutex_wait_syscall_arg_t;

/* aos queue object args */
typedef struct {
    aos_hdl_t    *phandle;
    const char   *name;
    int           flag;
    size_t        size;
    size_t        max_msg;
} aos_queue_open_syscall_arg_t;

typedef struct {
    aos_hdl_t *phandle;
    size_t    size;
    size_t    max_msg;
} aos_queue_new_syscall_arg_t;

typedef struct {
    obj_handle_t handle;
    void         *msg;
    size_t       size;
} aos_queue_send_syscall_arg_t;

typedef struct {
    obj_handle_t handle;
    uint32_t     ms;
    void         *msg;
    size_t       *size;
} aos_queue_recv_syscall_arg_t;

/* aos timer object args */
typedef struct {
    aos_hdl_t *phandle;
    time_fn    fn;
    void      *arg;
    uint32_t   ms;
    uint8_t    repeat;
    uint8_t    auto_run;
} aos_timer_new_syscall_arg_t;

typedef struct {
    obj_handle_t handle;
    uint32_t     ms;
} aos_timer_change_syscall_arg_t;

/* ----------------- futex ------------------- */
typedef struct {
    uint32_t *uaddr;
    int futex_op;
    uint32_t val;
    uint32_t timeout;
} aos_futex_syscall_arg_t;

#endif /* AOS_OBJ_SYSCALL_H */

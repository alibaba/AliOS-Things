/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef LINKKIT_KSYSCALL_H
#define LINKKIT_KSYSCALL_H

int sys_linkkit_start_stub(void *arg);

int sys_linkkit_end_stub(void *arg);

void* sys_linkkit_dispatch_stub(void *arg);

int sys_linkkit_yield_stub(void *arg);

int sys_linkkit_set_value_stub(void *arg);

int sys_linkkit_get_value_stub(void *arg);

void *sys_linkkit_set_tsl_stub(void *arg);

int sys_linkkit_answer_service_stub(void *arg);

int sys_linkkit_invoke_raw_service_stub(void *arg);

int sys_linkkit_trigger_event_stub(void *arg);

int sys_linkkit_post_property_stub(void *arg);

int sys_linkkit_set_opt_stub(void *arg);

void sys_linkkit_try_leave_stub(void *arg);

int sys_linkkit_is_try_leave_stub(void *arg);

int sys_linkkit_is_end_stub(void *arg);

int sys_linkkit_trigger_xinfo_operate_stub(void *arg);

int sys_linkkit_ioctl_stub(void *arg);

#endif /* LINKKIT_KSYSCALL_H */



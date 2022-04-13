/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */

#ifndef SIGNAL_KSYSCALL_H
#define SIGNAL_KSYSCALL_H

#ifdef __cplusplus
extern "C" {
#endif

int sys_aos_kill_stub(void *arg);
int sys_aos_tkill_stub(void *arg);
int sys_aos_sigpending_stub(void *arg);
int sys_aos_sigprocmask_stub(void *arg);
int sys_aos_sigaction_stub(void *arg);

#ifdef __cplusplus
}
#endif

#endif /* SIGNAL_KSYSCALL_H */

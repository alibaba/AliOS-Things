/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef RHINO_KSYSCALL_H
#define RHINO_KSYSCALL_H

#include "k_api.h"

/* ----------------- proc msg -------------------- */
size_t sys_krhino_msg_get_stub(void *arg);

kstat_t sys_krhino_msg_snd_stub(void *arg);

kstat_t sys_krhino_msg_recv_stub(void *arg);

kstat_t sys_krhino_msg_del_stub(void *arg);

kstat_t sys_krhino_ipc_create_stream_stub(void *arg);

kstat_t sys_krhino_ipc_create_fifo_stub(void *arg);

kstat_t sys_krhino_ipc_create_msg_stub(void *arg);

kstat_t sys_krhino_ipc_send_stub(void *arg);

kstat_t sys_krhino_ipc_sendto_stub(void *arg);

kstat_t sys_krhino_ipc_send_owner_stub(void *arg);

kstat_t sys_krhino_ipc_setopt_stub(void *arg);

kstat_t sys_krhino_ipc_getopt_stub(void *arg);

kstat_t sys_krhino_ipc_recv_stub(void *arg);

tick_t sys_krhino_ipc_cycle_stub(void *arg);

kstat_t sys_krhino_ipc_del_stub(void *arg);

kstat_t sys_krhino_ipc_perf_stub(void *arg);

#endif /* RHINO_KSYSCALL_H */


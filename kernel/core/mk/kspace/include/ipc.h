/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef U_IPC_H
#define U_IPC_H

#ifdef __cplusplus
extern "C" {
#endif

#define IPC_CONN_NAME_MAX_SIZE   (127)

enum {
    IPC_OPT_SETINFO = 0,
    IPC_OPT_GETINFO,
    IPC_OPT_ENABLE,
    IPC_OPT_DISABLE,
};

struct aos_ipc_opt_t {
    size_t size;
    void *owner;
    char name[IPC_CONN_NAME_MAX_SIZE + 1];
};

size_t krhino_msg_get(uint32_t key, uint32_t flg, size_t size);

kstat_t krhino_msg_snd(size_t msq_id, void *msg, size_t msg_sz);

kstat_t krhino_msg_recv(size_t msq_id, tick_t ticks, void *msg, size_t *msg_sz);

kstat_t krhino_msg_del(size_t msq_id);

void msg_exit(uint32_t pid);

kstat_t aos_ipc_create_stream(const char *name, size_t size, int *obj_handle);

kstat_t aos_ipc_create_msg(const char *name, size_t msg_count, int *obj_handle);

kstat_t aos_ipc_create_fifo(const char *name, size_t elem_size, size_t elem_count, int *obj_handle);

kstat_t aos_ipc_send(int handle, void *data, size_t dlen, tick_t ticks);

kstat_t aos_ipc_sendto(int handle, void *data, size_t dlen, tick_t ticks);

kstat_t aos_ipc_send_owner(int handle, void *data, size_t dlen, tick_t ticks);

kstat_t aos_ipc_recv(int handle, void *data, size_t *dlen, tick_t ticks);

kstat_t aos_ipc_setopt(int handle, int optyp, void *val);

kstat_t aos_ipc_getopt(int handle, int optyp, void *val);

kstat_t aos_ipc_handle_writable(int handle);

kstat_t aos_ipc_handle_readable(int handle);

kstat_t aos_ipc_del(int obj_handle);

kstat_t aos_ipc_perf();

tick_t aos_ipc_cycle();

void aos_ipc_exit(uint32_t pid);

#ifdef __cplusplus
}
#endif

#endif /* U_PROC_H */


/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef U_PROC_H
#define U_PROC_H

#ifdef __cplusplus
extern "C" {
#endif

size_t krhino_msg_get(uint32_t key, uint32_t flg, size_t size);

kstat_t krhino_msg_snd(size_t msq_id, void *msg, size_t msg_sz);

kstat_t krhino_msg_recv(size_t msq_id, tick_t ticks, void *msg, size_t *msg_sz);

kstat_t krhino_msg_del(size_t msq_id);

void msg_exit(uint32_t pid);

#ifdef __cplusplus
}
#endif

#endif /* U_PROC_H */


/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef K_FUTEX_H
#define K_FUTEX_H

/* futex_op value argument*/
#define KFUTEX_WAIT     0
#define KFUTEX_WAKE     1

#define KFUTEX_PRIVATE_FLAG     128
#define KFUTEX_CLOCK_REALTIME   256
#define KFUTEX_CMD_MASK         ~(KFUTEX_PRIVATE_FLAG | KFUTEX_CLOCK_REALTIME)


typedef struct futex_key {
    unsigned int pid;
    unsigned int address;
    unsigned int offset;
} futex_key_t;

typedef struct k_futex {
    ktask_t *task;
    futex_key_t key;
} k_futex_t;

void krhino_futex_init(void);


kstat_t krhino_futex(uint32_t *uaddr, int futex_op, uint32_t val, const tick_t timeout);



#endif //< K_FUTEX_H
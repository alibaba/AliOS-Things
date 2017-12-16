/**
 * Copyright (C) 2015 The YunOS Project. All rights reserved.
 */
#include "test_bed.h"

struct semaphore {
    int32_t rsvd;
};

void sema_init(struct semaphore *sem, int val)
{
    return;
}
void down(struct semaphore *sem)
{
    return;
}

int down_trylock(struct semaphore *sem)
{
    return -1;
}

int down_timeout(struct semaphore *sem, long jiffies)
{
    return -1;
}

int down_interruptible(struct semaphore *sem)
{
    return -1;
}

void up(struct semaphore *sem)
{
    return;
}

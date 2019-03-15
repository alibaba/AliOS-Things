/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>

#if 0
#include "unity.h"
#include "espos_mem.h"
#include "espos_task.h"
#include "espos_semaphore.h"
#include "espos_time.h"

static espos_sem_t sync_semaphore;

TEST_CASE("ESPOS mm api test", "[espos]")
{
    uint8_t *buf;
    uint8_t non_zero = 0;
    uint32_t i;

    // do malloc
    buf = espos_malloc(1024);
    memset(buf, 0xFF, 1024);
    TEST_ASSERT(buf != NULL);
    espos_free(buf);

    // do zalloc
    buf = espos_zalloc(1024);
    TEST_ASSERT(buf != NULL);
    for (i = 0; i < 1024; i++) {
        if (buf[i] != 0) {
            non_zero = 1;
            break;
        }
    }
    TEST_ASSERT(non_zero == 0);
    espos_free(buf);
}

static int tryAllocMem()
{
    int **mem;
    int i, noAllocated, j;
    mem = malloc(sizeof(int) * 1024);
    if (!mem) {
        return 0;
    }
    for (i = 0; i < 1024; i++) {
        mem[i] = malloc(1024);
        if (mem[i] == NULL) {
            break;
        }
        for (j = 0; j < 1024 / 4; j++) {
            mem[i][j] = (0xdeadbeef);
        }
    }
    noAllocated = i;
    for (i = 0; i < noAllocated; i++) {
        for (j = 0; j < 1024 / 4; j++) {
            TEST_ASSERT(mem[i][j] == (0xdeadbeef));
        }
        free(mem[i]);
    }
    free(mem);
    return noAllocated;
}

static void malloc_free_task(void *p)
{
    uint32_t i;
    uint8_t *buf;
    for (i = 0; i < 0xFFFF; i++) {
        buf = espos_malloc(1024);
        espos_task_delay(1);
        espos_free(buf);
    }
    espos_sem_give(sync_semaphore);
    espos_task_del(0);
}

TEST_CASE("ESPOS concurrent use of malloc free test", "[espos]")
{
    uint32_t m1, m2;
    espos_task_t new_task_handle;

    espos_sem_create(&sync_semaphore, NULL, 2, 0);

    m1 = tryAllocMem();

    espos_task_create(&new_task_handle, "malloc_free_task1", NULL, 1,
                      0, 1024, malloc_free_task, 0);
    espos_task_create(&new_task_handle, "malloc_free_task2", NULL, 1,
                      0, 1024, malloc_free_task, 0);

    // wait task exit
    espos_sem_take(sync_semaphore, ESPOS_MAX_DELAY);
    espos_sem_take(sync_semaphore, ESPOS_MAX_DELAY);
    espos_sem_del(sync_semaphore);

    m2 = tryAllocMem();

    TEST_ASSERT(m1 == m2);
}

TEST_CASE("ESPOS mm API invalid parameter test", "[espos]")
{
    uint8_t *buf;
    buf = espos_malloc(0xFFFFFFFF);
    TEST_ASSERT(buf == NULL);
    buf = espos_malloc(0);
    TEST_ASSERT(buf == NULL);
    buf = espos_zalloc(0xFFFFFFFF);
    TEST_ASSERT(buf == NULL);
    buf = espos_zalloc(0);
    TEST_ASSERT(buf == NULL);

    // TODO: need to check if free invalid pointer will cause exception
    espos_free(NULL);
    espos_free((void *)0xFFFF);
}

#endif

// To add more test cases for specific RTOS, like fragment handling


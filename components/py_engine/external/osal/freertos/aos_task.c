/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "aos/kernel.h"
#include "aos/errno.h"
#include "esp_heap_task_info.h"
#include "esp_heap_caps.h"

#define RHINO_CONFIG_USER_PRI_MAX 62

#define AOS_MAGIC 0x20171020
typedef struct {
    uint32_t key_bitmap;
    void *keys[4];
    void *stack;
    char name[32];
    uint32_t magic;
} AosStaticTask_t;

struct targ {
    AosStaticTask_t *task;
    void (*fn)(void *);
    void *arg;
};

aos_status_t aos_task_new(const char *name, void (*fn)(void *), void *arg, size_t stack_size)
{

    return aos_task_new_ext(NULL, name, fn, arg, stack_size, AOS_DEFAULT_APP_PRI);
}

aos_status_t aos_task_new_ext(aos_task_t *task, const char *name, void (*fn)(void *),
                              void *arg, size_t stack_size, int32_t prio)
{
    TaskHandle_t xHandle;
    int prio_delta, freertos_task_prio;
    BaseType_t ret;

    /* verify param */
    if (fn == NULL || (stack_size % 4 != 0)) {
        return -1;
    }

    /* task prio adapt */
    prio_delta = (int)(prio * configMAX_PRIORITIES / RHINO_CONFIG_USER_PRI_MAX);
    if (prio_delta > configMAX_PRIORITIES) {
        prio_delta = configMAX_PRIORITIES;
    }

    /* freertos task has high priority with high prio value, while alios reverse */
    freertos_task_prio = configMAX_PRIORITIES - prio_delta;

    /*create task */
    if (name == NULL) {
        name = "default_name";
    }
    ret = xTaskCreate(fn, name, stack_size, arg, freertos_task_prio, &xHandle);

    if (ret == pdPASS) {
        if (task) {
            *task = xHandle;
        }
        return 0;
    } else {
        return -1;
    }
}

void aos_task_exit(int32_t code)
{
    /* task exit by itself */
    vTaskDelete(NULL);
}

aos_status_t aos_task_delete(aos_task_t *task)
{
    TaskHandle_t p_handle;

    p_handle = (TaskHandle_t)(*task);

    vTaskDelete(p_handle);

    return 0;
}


aos_status_t aos_task_name_get(aos_task_t *task, char *buf, size_t buf_size)
{
    const char *name;
    int str_len, copy_len;
    TaskHandle_t ptask;

    // CHECK_HANDLE(task);

    if (buf == NULL || buf_size == 0) {
        return -EINVAL;
    }

    ptask = (TaskHandle_t)(*task);

    name = pcTaskGetTaskName(ptask);

    str_len = strlen(name);

    copy_len = (str_len > (buf_size - 1)) ? buf_size - 1 : str_len;
    memcpy(buf, name, copy_len);
    return 0;
}


aos_status_t aos_task_resume(aos_task_t *task)
{
    TaskHandle_t p_handle;

    p_handle = (TaskHandle_t)(*task);

    vTaskResume(p_handle);

    return 0;
}

aos_status_t aos_task_suspend(aos_task_t *task)
{
    TaskHandle_t p_handle;

    p_handle = (TaskHandle_t)(*task);

    vTaskSuspend(p_handle);

    return 0;
}

aos_status_t aos_task_yield()
{
    taskYIELD();

    return 0;
}

void aos_msleep(uint32_t ms)
{
    vTaskDelay(pdMS_TO_TICKS(ms));
    return;
}

uint64_t aos_now_ms(void)
{
    long long ms;
    uint32_t tick_cnt = xTaskGetTickCount();
    ms = tick_cnt * 1000 / configTICK_RATE_HZ;
    return ms;
}

uint64_t aos_now(void)
{
    return aos_now_ms() * 1000 * 1000;
}

aos_task_t aos_task_self(void)
{
    aos_task_t task;
    task = (aos_task_t)xTaskGetCurrentTaskHandle();

    return task;
}

aos_status_t aos_task_key_create(aos_task_key_t *key)
{
    // todo
    /*
    AosStaticTask_t *task = (AosStaticTask_t *)xTaskGetCurrentTaskHandle();
    int i;

    if (task->magic != AOS_MAGIC)
        return -1;

    for (i=0;i<4;i++) {
        if (task->key_bitmap & (1 << i))
            continue;

        task->key_bitmap |= 1 << i;
        *key = i;
        return 0;
    }
    */
    return -1;
}

void aos_task_key_delete(aos_task_key_t key)
{
    // todo
    /*
    AosStaticTask_t *task = (AosStaticTask_t *)xTaskGetCurrentTaskHandle();
    if (task->magic != AOS_MAGIC)
        return;
    task->key_bitmap &= ~(1 << key);
    */
}

aos_status_t aos_task_setspecific(aos_task_key_t key, void *vp)
{
    // todo
    /*
    AosStaticTask_t *task = (AosStaticTask_t *)xTaskGetCurrentTaskHandle();
    if (key >= 4)
        return -1;

    if (task->magic != AOS_MAGIC)
        return -1;

    task->keys[key] = vp;
    */

    return 0;
}

void *aos_task_getspecific(aos_task_key_t key)
{
    // todo
    /*
    AosStaticTask_t *task = (AosStaticTask_t *)xTaskGetCurrentTaskHandle();
    if (key >= 4)
        return NULL;

    if (task->magic != AOS_MAGIC)
        return NULL;

    return task->keys[key];
    */
    return NULL;
}

void *aos_malloc(size_t size)
{
    return malloc(size);
}

void *aos_calloc(size_t nitems, size_t size)
{
    return calloc(nitems, size);
}

void *aos_zalloc(size_t size)
{
    return calloc(size, 1);
}

void *aos_realloc(void *mem, size_t size)
{
    return realloc(mem, size);
}

void aos_free(void *mem)
{
    free(mem);
}

void aos_task_show_info(void)
{
    const size_t bytes_per_task = 40; /* see vTaskList description */
    char *task_list_buffer = malloc(uxTaskGetNumberOfTasks() * bytes_per_task);
    if (task_list_buffer == NULL) {
        printf("failed to allocate buffer for vTaskList output");
        return;
    }
    fputs("Task Name\tStatus\tPrio\tHWM\tTaskid#", stdout);
    fputs("\n", stdout);
    vTaskList(task_list_buffer);
    fputs(task_list_buffer, stdout);
    free(task_list_buffer);
}

#define MAX_TASK_NUM 20    /* Max number of per tasks info that it can store */
#define MAX_BLOCK_NUM 20   /* Max number of per block info that it can store */

static size_t s_prepopulated_num = 0;
static heap_task_totals_t s_totals_arr[MAX_TASK_NUM];
static heap_task_block_t s_block_arr[MAX_BLOCK_NUM];

void aos_mm_show_info(void)
{
    heap_task_info_params_t heap_info = {0};
    heap_info.caps[0] = MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT;
    heap_info.mask[0] = MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT;
    heap_info.caps[1] = MALLOC_CAP_SPIRAM   | MALLOC_CAP_8BIT;
    heap_info.mask[1] = MALLOC_CAP_SPIRAM   | MALLOC_CAP_8BIT;
    heap_info.tasks = NULL;
    heap_info.num_tasks = 0;
    heap_info.totals = s_totals_arr;
    heap_info.num_totals = &s_prepopulated_num;
    heap_info.max_totals = MAX_TASK_NUM;
    heap_info.blocks = s_block_arr;
    heap_info.max_blocks = MAX_BLOCK_NUM;

    heap_caps_get_per_task_info(&heap_info);

    for (int i = 0 ; i < *heap_info.num_totals; i++) {
        printf("Task: %s -> CAP_INTERNAL_8BIT: %d CAP_SPIRAM_8BIT: %d\n",
                heap_info.totals[i].task ? pcTaskGetTaskName(heap_info.totals[i].task) : "Pre-Scheduler allocs" ,
                heap_info.totals[i].size[0],
                heap_info.totals[i].size[1]);
    }

    printf("\r\n[internal]\r\n");
    heap_caps_print_heap_info(MALLOC_CAP_INTERNAL);
#if CONFIG_SPIRAM_USE_MALLOC
    printf("\r\n[spiram]\r\n");
    heap_caps_print_heap_info(MALLOC_CAP_SPIRAM);
    printf("\r\n[DMA]\r\n");
    heap_caps_print_heap_info(MALLOC_CAP_DMA);
#endif
}

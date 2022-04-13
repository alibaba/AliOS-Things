/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

/* JSEngine OS Abstract Layer */
#ifndef AMP_SYSTEM_H
#define AMP_SYSTEM_H

#if defined(__cplusplus)
extern "C" {
#endif

#include "stdarg.h"
#include "infra_types.h"
#include "wrappers_defs.h"

typedef enum {
    TimerRunOnce     = 0, /* one-shot timer */
    TimerRunPeriodic = 1  /* repeating timer */
} amp_timer_type;

typedef struct amp_heap_info {
    size_t heap_total; /* total heap memory */
    size_t heap_used;  /* used heap memory */
} amp_heap_info_t;

typedef void *amp_osTimerId;
typedef void *amp_osMessageQId;
typedef void (*os_timer_cb)(void *argument);

typedef struct _amp_os_thread {
    uint32_t                priority;     /*initial thread priority */
    void                    *stack_addr;   /* thread stack address malloced by caller, use system stack by . */
    int                     stack_size;   /* stack size requirements in bytes; 0 is default stack size */
    int                     detach_state; /* 0: not detached state; otherwise: detached state. */
    char                    *name;         /* thread name. */
} amp_os_thread_param_t;

typedef struct _amp_wireless_info_t {
    int rssi; /* Received Signal Strength Indication */
    int snr;  /* Signal to Noise Ratio */
    int per;  /* Packet Error Rate (Unit: PPM, Part Per Million) */
} amp_wireless_info_t;
/**
 * @brief   get RTOS default priority
 *
 * @return  default priority
 */
void *HAL_Malloc(uint32_t size);

/**
 * @brief   get RTOS default priority
 *
 * @return  default priority
 */
void HAL_Free(void *ptr);

/**
 * @brief   get RTOS default priority
 *
 * @return  default priority
 */
void *HAL_Calloc(uint32_t nitems, uint32_t size);

/**
 * @brief   get RTOS default priority
 *
 * @return  default priority
 */
void *HAL_Realloc(void *ptr, uint32_t size);

/**
 * @brief   get RTOS default priority
 *
 * @return  default priority
 */
uint64_t HAL_UptimeMs(void);

/**
 * @brief   get RTOS default priority
 *
 * @return  default priority
 */
void HAL_SleepMs(uint32_t ms);

/**
 * @brief   get RTOS default priority
 *
 * @return  default priority
 */
void HAL_Srandom(uint32_t seed);

/**
 * @brief   get RTOS default priority
 *
 * @return  default priority
 */
uint32_t HAL_Random(uint32_t region);

/**
 * @brief   get RTOS default priority
 *
 * @return  default priority
 */
void HAL_Printf(const char *fmt, ...);

/**
 * @brief   get RTOS default priority
 *
 * @return  default priority
 */
int HAL_Snprintf(char *str, const int len, const char *fmt, ...);

/**
 * @brief   get RTOS default priority
 *
 * @return  default priority
 */
int HAL_Vsnprintf(char *str, const int len, const char *format, va_list ap);

/**
 * @brief   get RTOS default priority
 *
 * @return  default priority
 */
void *HAL_SemaphoreCreate(void);

/**
 * @brief   get RTOS default priority
 *
 * @return  default priority
 */
void HAL_SemaphoreDestroy(void *sem);

/**
 * @brief   get RTOS default priority
 *
 * @return  default priority
 */
void HAL_SemaphorePost(void *sem);

/**
 * @brief   get RTOS default priority
 *
 * @return  default priority
 */
int HAL_SemaphoreWait(void *sem, uint32_t timeout_ms);

/**
 * @brief   get RTOS default priority
 *
 * @return  default priority
 */
int HAL_ThreadCreate(
            void **thread_handle,
            void *(*work_routine)(void *),
            void *arg,
            amp_os_thread_param_t *amp_os_thread_param,
            int *stack_used);

/**
 * @brief   get RTOS default priority
 *
 * @return  default priority
 */
int HAL_ThreadDelete(void *thread_handle);

/**
 * @brief   get RTOS default priority
 *
 * @return  default priority
 */
char *HAL_GetThreadName(void **thread_handle);

/**
 * @brief   get RTOS default priority
 *
 * @return  default priority
 */
int HAL_GetDefaultTaskPriority();


/**
 * @brief   get RTOS default priority
 *
 * @return  default priority
 */
void *HAL_MutexCreate(void);

/**
 * @brief   get RTOS default priority
 *
 * @return  default priority
 */
void HAL_MutexDestroy(void *mutex);

/**
 * @brief   get RTOS default priority
 *
 * @return  default priority
 */
void HAL_MutexLock(void *mutex);

/**
 * @brief   get RTOS default priority
 *
 * @return  default priority
 */
void HAL_MutexUnlock(void *mutex);

/**
 * @brief   get RTOS default priority
 *
 * @return  default priority
 */
void *HAL_Timer_Create(const char *name, void (*func)(void *), void *user_data);

/**
 * @brief   get RTOS default priority
 *
 * @return  default priority
 */
void *HAL_GetTimerPrams(void *timerHandle);

/**
 * @brief   get RTOS default priority
 *
 * @return  default priority
 */
int32_t HAL_Timer_Start(void *timer, int ms, amp_timer_type repeat);

/**
 * @brief   get RTOS default priority
 *
 * @return  default priority
 */
int32_t HAL_Timer_Stop(void *timer);

/**
 * @brief   get RTOS default priority
 *
 * @return  default priority
 */
int32_t HAL_Timer_Delete(void *timer);

/**
 * @brief   get RTOS default priority
 *
 * @return  default priority
 */
void *HAL_MessageQCreate(int32_t queue_length, int32_t item_size);

/**
 * @brief   get RTOS default priority
 *
 * @return  default priority
 */
int32_t HAL_MessageQDelete(void *mq);

/**
 * @brief   get RTOS default priority
 *
 * @return  default priority
 */
int32_t HAL_MessageQPut(void *mq, void *p_info, uint32_t size, uint32_t millisec);

/**
 * @brief   get RTOS default priority
 *
 * @return  default priority
 */
int32_t HAL_MessageQGet(void *mq, void *p_info, uint32_t size, uint32_t millisec);

/**
 * @brief   get RTOS default priority
 *
 * @return  default priority
 */
void HAL_Reboot(void);

/**
 * @brief   get RTOS default priority
 *
 * @return  default priority
 */
const char *HAL_Get_System_Version(void);

/**
 * @brief   get RTOS default priority
 *
 * @return  default priority
 */
const char *HAL_Get_system_platform_type(void);

/**
 * @brief   get RTOS default priority
 *
 * @return  default priority
 */
const char *HAL_Get_Device_Name(void);

/**
 * @brief   get RTOS default priority
 *
 * @return  default priority
 */
int32_t HAL_System_Sleep(void);

/**
 * @brief   get RTOS default priority
 *
 * @return  default priority
 */
int HAL_GetWirelessInfo(struct _amp_wireless_info_t *info);

/**
 * @brief   get RTOS default priority
 *
 * @return  default priority
 */
int32_t HAL_Heap_Memory_Info(amp_heap_info_t *heap_info);

/**
 * @brief   get RTOS default priority
 *
 * @return  default priority
 */
int32_t HAL_Sysinit(void);

/**
 * @brief   get RTOS default priority
 *
 * @return  default priority
 */
const char *HAL_Get_Module_Hardware_Version(void);

/**
 * @brief   get RTOS default priority
 *
 * @return  default priority
 */
const char *HAL_Get_Module_Software_Version(void);

#if defined(__cplusplus)
}
#endif

#endif /* AMP_SYSTEM_H */

/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/time.h>

#include <aos/kernel.h>
#include "iot_import.h"
//#include "aliot_platform_os.h"

#define PLATFORM_LINUX_LOG(format, ...) \
    do { \
        printf("Linux:%s:%d %s()| "format"\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__);\
        fflush(stdout);\
    }while(0);

void *HAL_MutexCreate(void)
{
    aos_mutex_t mutex;

    if (0 != aos_mutex_new(&mutex)) {
        return NULL;
    }

    return mutex.hdl;
    /*
    int err_num;
    pthread_mutex_t *mutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
    if (NULL == mutex)
    {
        return NULL;
    }

    if (0 != (err_num = pthread_mutex_init(mutex, NULL))) {
        perror("create mutex failed");
        free(mutex);
        return NULL;
    }

    return mutex;*/
}

void HAL_MutexDestroy(_IN_ void *mutex)
{
    aos_mutex_free((aos_mutex_t *)&mutex);
    /*
    int err_num;
    if (0 != (err_num = pthread_mutex_destroy((pthread_mutex_t *)mutex))) {
        perror("destroy mutex failed");
    }

    free(mutex);*/
}

void HAL_MutexLock(_IN_ void *mutex)
{
    aos_mutex_lock((aos_mutex_t *)&mutex, AOS_WAIT_FOREVER);
    /*
    int err_num;
    if (0 != (err_num = pthread_mutex_lock((pthread_mutex_t *)mutex))) {
        perror("lock mutex failed");
    }*/

}

void HAL_MutexUnlock(_IN_ void *mutex)
{
    aos_mutex_unlock((aos_mutex_t *)&mutex);
    /*
    int err_num;
    if (0 != (err_num = pthread_mutex_unlock((pthread_mutex_t *)mutex))) {
        perror("unlock mutex failed");
    }*/
}

void *HAL_Malloc(_IN_ uint32_t size)
{
    return aos_malloc(size);
    //return malloc(size);
}

void HAL_Free(_IN_ void *ptr)
{
    aos_free(ptr);
    //return free(ptr);
}

int aliot_platform_ota_start(const char *md5, uint32_t file_size)
{
    printf("this interface is NOT support yet.");
    return -1;
}

int aliot_platform_ota_write(_IN_ char *buffer, _IN_ uint32_t length)
{
    printf("this interface is NOT support yet.");
    return -1;
}

int aliot_platform_ota_finalize(_IN_ int stat)
{
    printf("this interface is NOT support yet.");
    return -1;
}

uint32_t HAL_UptimeMs(void)
{
    return aos_now_ms();
}

void HAL_SleepMs(_IN_ uint32_t ms)
{
    aos_msleep(ms);
    //usleep( 1000 * ms );
}

uint64_t aliot_platform_time_left(uint64_t t_end, uint64_t t_now)
{
    uint64_t t_left;

    if (t_end > t_now) {
        t_left = t_end - t_now;
    } else {
        t_left = 0;
    }

    return t_left;
}

void HAL_Printf(_IN_ const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);

    fflush(stdout);
}

char *HAL_GetPartnerID(char pid_str[])
{
    return NULL;
}

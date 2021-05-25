/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef __UVOICE_AMP_H__
#define __UVOICE_AMP_H__


#include "amp_platform.h"
#include "amp_kv.h"
#include "amp_fs.h"
#include "amp_defines.h"
#include "amp_system.h"
#include "wrappers_defs.h"

typedef void *os_dir_t;
typedef void *os_dirent_t;
typedef void *os_file_t;

typedef void *os_message_queue_t;
typedef void *os_mutex_t;
typedef void *os_sem_t;
typedef void *os_task_t;
typedef void *os_timer_t;


#define OS_SEEK_SET            HAL_SEEK_SET
#define OS_SEEK_CUR            HAL_SEEK_CUR
#define OS_SEEK_END            HAL_SEEK_END

#define OS_F_OK                0
#define OS_X_OK                1
#define OS_W_OK                2
#define OS_R_OK                3

#define OS_FILE_OPEN_FAIL(stream)    (stream == NULL)
#define OS_FILE_OPENING(stream)        (stream != NULL)
#define OS_FILE_CLOSED                (NULL)

#define OS_WAIT_FOREVER        PLATFORM_WAIT_INFINITE

#define os_container_of(ptr, type, member)    \
    ((type *)((char *)(ptr) - offsetof(type, member)))

#define UVOICE_TASK_PRI_DEFAULT (amp_get_default_task_priority() - 1)
#define UVOICE_TASK_PRI_IDLE    (amp_get_default_task_priority() - 1)
#define UVOICE_TASK_PRI_LOWEST    (amp_get_default_task_priority() - 1)
#define UVOICE_TASK_PRI_LOWER    (amp_get_default_task_priority() - 1)
#define UVOICE_TASK_PRI_NORMAL  (amp_get_default_task_priority() - 1)
#define UVOICE_TASK_PRI_HIGHER  (amp_get_default_task_priority() - 1)
#define UVOICE_TASK_PRI_HIGHEST (amp_get_default_task_priority() - 1)
#define UVOICE_TASK_PRI_REALTIME (amp_get_default_task_priority() - 1)


#ifdef UVOICE_BUILD_RELEASE
#define M_LOGD(fmt, ...)
#else
#define M_LOGD(fmt, ...)    amp_printf("%s: "fmt, __func__, ##__VA_ARGS__)
#endif
#define M_LOGI(fmt, ...)    amp_printf("%s: "fmt, __func__, ##__VA_ARGS__)
#define M_LOGW(fmt, ...)    amp_printf("%s: "fmt, __func__, ##__VA_ARGS__)
#define M_LOGE(fmt, ...)    amp_printf("%s: "fmt, __func__, ##__VA_ARGS__)
#define M_LOGR(fmt, ...)    amp_printf(fmt, ##__VA_ARGS__)

#define AFM_MAIN        0x1
#define AFM_EXTN        0x2

static inline void *snd_zalloc(size_t size, int flags)
{
    void *mem = amp_malloc(size);
    if (mem)
        memset(mem, 0, size);
    return mem;
}

static inline void snd_free(void *mem)
{
    amp_free(mem);
}

static inline void *snd_realloc(void *old, size_t newsize, int flags)
{
    void *mem = amp_realloc(old, newsize);
    if (mem)
        memset(mem, 0, newsize);
    return mem;
}

static inline void os_msleep(int msec)
{
    amp_msleep(msec);
}

static inline void os_usleep(int usec)
{
    amp_msleep(usec / 1000);
}

static inline long long os_current_time(void)
{
    return amp_uptime();
}

static inline int os_get_mac_address(char *mac)
{
    return 0;
}


static inline int os_kv_get(const char *key, void *buffer, int *len)
{
    return amp_kv_get(key, buffer, len);
}

static inline int os_kv_set(const char *key, const void *buffer, int len, int sync)
{
    return amp_kv_set(key, buffer, len, sync);
}

static inline int os_mkdir(const char *path)
{
    return amp_mkdir(path);
}

static inline os_dir_t *os_opendir(const char *path)
{
    return -1;
}

static inline os_dirent_t *os_readdir(os_dir_t *dir)
{
    return -1;
}

static inline int os_closedir(os_dir_t *dir)
{
    return -1;
}

static inline os_file_t os_fopen(const char *filename, const char *mode)
{
    return amp_fopen(filename, mode);
}

static inline size_t os_fread(void *buffer, size_t size, size_t count, os_file_t fp)
{
    return amp_fread(buffer, size, count, fp);
}

static inline size_t os_fwrite(const void *buffer, size_t size, size_t count, os_file_t fp)
{
    return amp_fwrite(buffer, size, count, fp);
}

static inline long os_ftell(os_file_t fp)
{
    return amp_ftell(fp);
}

static inline long os_fseek(os_file_t fp, long offset, int whence)
{
    int current_pos = -1;
    return amp_fseek(fp, offset, whence, &current_pos);
}

static inline char *os_fgets(char *buffer, int size, os_file_t fp)
{
    return 0;
}

static inline int os_fprintf(os_file_t fp, const char *format, ...)
{
    int ret;
    va_list args;
    va_start(args, format);
    ret = vfprintf(fp, format, args);
    va_end(args);
    return ret;
}

static inline int os_feof(os_file_t fp)
{
    return 0;
}

static inline int os_ferror(os_file_t fp)
{
    return 0;
}

static inline int os_fclose(os_file_t fp)
{
    return amp_fclose(fp);
}

static inline int os_access(const char *filename, int mode)
{
    os_file_t fp = os_fopen(filename, "r");
    if (!fp)
        return -1;
    os_fclose(fp);
    return 0;
}

static inline int os_remove(const char *filename)
{
    return amp_remove(filename);
}

static inline int os_message_queue_send(os_message_queue_t mq, void *msg,
    unsigned int size, unsigned int timeout)
{
    return amp_queue_send(mq, msg, size, timeout);
}

static inline int os_message_queue_recv(os_message_queue_t mq, void *msg,
    unsigned int size, unsigned int timeout)
{
    return amp_queue_recv(mq, msg, size, timeout);
}

static inline os_message_queue_t os_message_queue_create(int count, int size)
{
    return amp_queue_create(count, size);
}

static inline int os_message_queue_free(os_message_queue_t mq)
{
    return amp_queue_delete(mq);
}

static inline int os_event_post(uint16_t type, uint16_t code, int value)
{
    return -1;
}

static inline int os_event_register(uint16_t type, void *cb, void *data)
{
    return -1;
}

static inline int os_event_unregister(uint16_t type, void *cb, void *data)
{
    return -1;
}

static inline int os_mutex_lock(os_mutex_t mutex, unsigned int timeout)
{
    amp_mutex_lock(mutex);
    return 0;
}

static inline int os_mutex_unlock(os_mutex_t mutex)
{
    amp_mutex_unlock(mutex);
    return 0;
}

static inline os_mutex_t os_mutex_new(void)
{
    return amp_mutex_create();
}

static inline void os_mutex_free(os_mutex_t mutex)
{
    amp_mutex_destroy(mutex);
}

static inline int os_sem_is_valid(os_sem_t sem)
{
    return 1;
}

static inline int os_sem_wait(os_sem_t sem, unsigned int timeout)
{
    return amp_semaphore_wait(sem, timeout);
}

static inline void os_sem_signal(os_sem_t sem)
{
    amp_semaphore_post(sem);
}

static inline void os_sem_signal_all(os_sem_t sem)
{
    amp_semaphore_post(sem);
}

static inline os_sem_t os_sem_new(int count)
{
    return amp_semaphore_create();
}

static inline void os_sem_free(os_sem_t sem)
{
    amp_semaphore_destroy(sem);
}

static inline int os_timer_change(os_timer_t timer, int internal_ms)
{
    return -1;
}

static inline int os_timer_start(os_timer_t timer)
{
    return -1;
}

static inline int os_timer_stop(os_timer_t timer)
{
    return -1;
}

static inline os_timer_t os_timer_new(void (*func)(void *, void *), void *arg,
    int internal_ms, int repeat, unsigned char auto_run)
{
    return NULL;
}

static inline void os_timer_free(os_timer_t timer)
{
}

static inline int os_task_create(os_task_t *task, const char *name,
        void *(*fn)(void *), void *arg, int stack_size, int pri)
{
    amp_os_thread_param_t task_params = {0};
    int stack_used = stack_size;
    task_params.name = name;
    task_params.priority = pri;
    task_params.stack_size = stack_size;
    return amp_thread_create(task, fn, arg, &task_params, &stack_used);
}

static inline int os_task_exit(os_task_t task)
{
    return amp_thread_delete(task);
}

static inline const char *os_partition_name(int pt)
{
    return NULL;
}

static inline int os_partition_size(int pt)
{
    return -1;
}

static inline int os_partition_read(int pt, uint32_t *offset, uint8_t *buffer, uint32_t len)
{
    return -1;
}

static inline int os_partition_write(int pt, uint32_t *offset, const uint8_t *buffer , uint32_t len)
{
    return -1;
}

static inline int os_partition_erase(int pt, uint32_t offset, uint32_t len)
{
    return -1;
}


#endif /* __UVOICE_AMP_H__ */

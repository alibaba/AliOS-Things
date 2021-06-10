/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef __UVOICE_AOS_H__
#define __UVOICE_AOS_H__


#include <fcntl.h>
#include "aos/kv.h"
#include "aos/vfs.h"
#include <k_api.h>
#include <aos/kernel.h>

typedef aos_dir_t        os_dir_t;
typedef aos_dirent_t    os_dirent_t;
typedef int             os_file_t;

typedef aos_queue_t        os_message_queue_t;
typedef aos_mutex_t        os_mutex_t;
typedef aos_sem_t        os_sem_t;
typedef aos_task_t        os_task_t;
typedef aos_timer_t        os_timer_t;


#define OS_SEEK_SET            SEEK_SET
#define OS_SEEK_CUR            SEEK_CUR
#define OS_SEEK_END            SEEK_END

#define OS_F_OK                F_OK
#define OS_X_OK                X_OK
#define OS_W_OK                W_OK
#define OS_R_OK                R_OK

#define OS_FILE_OPEN_FAIL(stream)    (stream <= 0)
#define OS_FILE_OPENING(stream)        (stream > 0)
#define OS_FILE_CLOSED                (0)

#define OS_WAIT_FOREVER        AOS_WAIT_FOREVER

#define os_container_of(ptr, type, member)    \
    ((type *)((char *)(ptr) - offsetof(type, member)))

enum {
    UVOICE_TASK_PRI_LOWEST      = AOS_DEFAULT_APP_PRI + 5,
    UVOICE_TASK_PRI_LOWER       = AOS_DEFAULT_APP_PRI + 1,
    UVOICE_TASK_PRI_NORMAL      = AOS_DEFAULT_APP_PRI,
    UVOICE_TASK_PRI_HIGHER      = AOS_DEFAULT_APP_PRI - 1,
    UVOICE_TASK_PRI_HIGHEST     = AOS_DEFAULT_APP_PRI - 5,
};


#ifdef UVOICE_BUILD_RELEASE
#define M_LOGD(fmt, ...)
#else
#define M_LOGD(fmt, ...)    aos_printf("%s: "fmt, __func__, ##__VA_ARGS__)
#endif
#define M_LOGI(fmt, ...)    aos_printf("%s: "fmt, __func__, ##__VA_ARGS__)
#define M_LOGW(fmt, ...)    aos_printf("%s: "fmt, __func__, ##__VA_ARGS__)
#define M_LOGE(fmt, ...)    aos_printf("%s: "fmt, __func__, ##__VA_ARGS__)
#define M_LOGR(fmt, ...)    aos_printf(fmt, ##__VA_ARGS__)

#define AFM_MAIN        0x1
#define AFM_EXTN        0x2

static inline void *snd_zalloc(size_t size, int flags)
{
    void *mem = aos_malloc(size);
    if (mem)
        memset(mem, 0, size);
    return mem;
}

static inline void snd_free(void *mem)
{
    aos_free(mem);
}

static inline void *snd_realloc(void *old, size_t newsize, int flags)
{
    void *mem = aos_realloc(old, newsize);
    if (mem)
        memset(mem, 0, newsize);
    return mem;
}

static inline void os_msleep(int msec)
{
    aos_msleep(msec);
}

static inline void os_usleep(int usec)
{
    aos_msleep(usec / 1000);
}

static inline long long os_current_time(void)
{
    return aos_now_ms();
}

static inline int os_get_mac_address(char *mac)
{
    return 0;
}

static inline int os_kv_get(const char *key, void *buffer, int *len)
{
    return aos_kv_get(key, buffer, len);
}

static inline int os_kv_set(const char *key, const void *buffer, int len, int sync)
{
    return aos_kv_set(key, buffer, len, sync);
}

static inline int os_mkdir(const char *path)
{
    return aos_mkdir(path);
}

static inline os_dir_t *os_opendir(const char *path)
{
    return aos_opendir(path);
}

static inline os_dirent_t *os_readdir(os_dir_t *dir)
{
    return aos_readdir(dir);
}

static inline int os_closedir(os_dir_t *dir)
{
    return aos_closedir(dir);
}

static inline os_file_t os_fopen(const char *filename, const char *mode)
{
    int flags = 0;
    if (!strcmp(mode, "r") || !strcmp(mode, "rb")) {
        flags = O_RDONLY;
    } else if (!strcmp(mode, "w+") || !strcmp(mode, "wb+")) {
        flags = O_RDWR | O_CREAT;
    }
    return aos_open(filename, flags);
}

static inline size_t os_fread(void *buffer, size_t size, size_t count, os_file_t fp)
{
    return aos_read(fp, buffer, size * count);
}

static inline size_t os_fwrite(const void *buffer, size_t size, size_t count, os_file_t fp)
{
    return aos_write(fp, buffer, size * count);
}

static inline long os_ftell(os_file_t fp)
{
    return -1;
}

static inline long os_fseek(os_file_t fp, long offset, int whence)
{
    return aos_lseek(fp, offset, whence);
}

static inline char *os_fgets(char *buffer, int size, os_file_t fp)
{
    return 0;
}

static inline int os_fprintf(os_file_t fp, const char *format, ...)
{
    return 0;
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
    return aos_close(fp);
}

static inline int os_access(const char *filename, int mode)
{
    return aos_access(filename, mode);
}

static inline int os_remove(const char *filename)
{
    return aos_remove(filename);
}

static inline int os_message_queue_send(os_message_queue_t mq, void *msg,
    unsigned int size, unsigned int timeout)
{
    return aos_queue_send(&mq, msg, size);
}

static inline int os_message_queue_recv(os_message_queue_t mq, void *msg,
    unsigned int size, unsigned int timeout)
{
    return aos_queue_recv(&mq, timeout, msg, &size);
}

static inline os_message_queue_t os_message_queue_create(int count, int size)
{
    aos_queue_t q = NULL;
    aos_queue_new(&q, NULL, size * count, size);
    return q;
}

static inline int os_message_queue_free(os_message_queue_t mq)
{
    aos_queue_free(&mq);
    return 0;
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
    aos_mutex_lock(&mutex, timeout);
    return 0;
}

static inline int os_mutex_unlock(os_mutex_t mutex)
{
    aos_mutex_unlock(&mutex);
    return 0;
}

static inline os_mutex_t os_mutex_new(void)
{
    os_mutex_t mutex;
    aos_mutex_new(&mutex);
    return mutex;
}

static inline void os_mutex_free(os_mutex_t mutex)
{
    aos_mutex_free(&mutex);
}

static inline int os_sem_is_valid(os_sem_t sem)
{
    return 1;
}

static inline int os_sem_wait(os_sem_t sem, unsigned int timeout)
{
    return aos_sem_wait(&sem, timeout);
}

static inline void os_sem_signal(os_sem_t sem)
{
    aos_sem_signal(&sem);
}

static inline void os_sem_signal_all(os_sem_t sem)
{
    aos_sem_signal_all(&sem);
}

static inline os_sem_t os_sem_new(int count)
{
    os_sem_t sem;
    aos_sem_new(&sem, 0);
    return sem;
}

static inline void os_sem_free(os_sem_t sem)
{
    aos_sem_free(&sem);
}

static inline int os_task_create(os_task_t *task, const char *name,
        void (*fn)(void *), void *arg, int stack_size, int pri)
{
    return aos_task_new_ext(task, name, fn, arg, stack_size, pri);
}

static inline int os_task_exit(os_task_t task)
{
    aos_task_exit(0);
    return 0;
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


#endif /* __UVOICE_AOS_H__ */

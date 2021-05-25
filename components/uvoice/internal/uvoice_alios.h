/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef __UVOICE_ALIOS_H__
#define __UVOICE_ALIOS_H__


#include <stdarg.h>
#include <k_api.h>
#include <aos/kernel.h>
#include <aos/errno.h>
#include <aos/kv.h>
#include <aos/vfs.h>
#include <aos/list.h>
#ifdef AOS_LOOP
#include <aos/yloop.h>
#endif
#include <aos/hal/flash.h>

#include "ulog/ulog.h"


typedef aos_dir_t        os_dir_t;
typedef aos_dirent_t     os_dirent_t;
typedef FILE *os_file_t;

typedef void *os_message_queue_t;
typedef aos_mutex_t *os_mutex_t;
typedef aos_sem_t *os_sem_t;
typedef aos_task_t        os_task_t;
typedef aos_timer_t *os_timer_t;


#define OS_SEEK_SET            SEEK_SET
#define OS_SEEK_CUR            SEEK_CUR
#define OS_SEEK_END            SEEK_END

#define OS_F_OK                F_OK
#define OS_X_OK                X_OK
#define OS_W_OK                W_OK
#define OS_R_OK                R_OK

#define OS_FILE_OPEN_FAIL(stream)    (stream == NULL)
#define OS_FILE_OPENING(stream)        (stream != NULL)
#define OS_FILE_CLOSED                (NULL)

#define OS_WAIT_FOREVER        AOS_WAIT_FOREVER

#define os_container_of        aos_container_of

enum {
    UVOICE_TASK_PRI_IDLE        = RHINO_IDLE_PRI,
    UVOICE_TASK_PRI_LOWEST        = AOS_DEFAULT_APP_PRI + 5,
    UVOICE_TASK_PRI_LOWER        = AOS_DEFAULT_APP_PRI + 1,
    UVOICE_TASK_PRI_NORMAL        = AOS_DEFAULT_APP_PRI,
    UVOICE_TASK_PRI_HIGHER        = AOS_DEFAULT_APP_PRI - 1,
    UVOICE_TASK_PRI_HIGHEST        = AOS_DEFAULT_APP_PRI - 5,
    UVOICE_TASK_PRI_REALTIME    = RHINO_CONFIG_TIMER_TASK_PRI
};


#if (UVOICE_BUILD_RELEASE)
#define M_LOGD(fmt, ...)
#else
#define M_LOGD(fmt, ...)    printf("%s: "fmt, __func__, ##__VA_ARGS__)
#endif
#define M_LOGI(fmt, ...)    printf("%s: "fmt, __func__, ##__VA_ARGS__)
#define M_LOGW(fmt, ...)    printf("%s: "fmt, __func__, ##__VA_ARGS__)
#define M_LOGE(fmt, ...)    printf("%s: "fmt, __func__, ##__VA_ARGS__)
#define M_LOGR(fmt, ...)    printf(fmt, ##__VA_ARGS__)

#define AFM_MAIN        0x1
#define AFM_EXTN        0x2

static inline void *snd_zalloc(size_t size, int flags)
{
#ifdef UVOICE_ON_BK7251
    void *mem = NULL;
    if (flags == AFM_MAIN) {
        mem = malloc(size);
    } else if (flags == AFM_EXTN) {
#ifdef BK7251_IRAM_ENABLE
        mem = iram_heap_malloc(size);
#else
        mem = malloc(size);
#endif
    }
    if (mem)
        memset(mem, 0, size);
    return mem;
#else
    void *mem = malloc(size);
    if (mem)
        memset(mem, 0, size);
    return mem;
#endif
}

static inline void snd_free(void *mem)
{
#ifdef UVOICE_ON_BK7251
    if (mem >= 0x00400020 && mem <= 0x00440000)
        free(mem);
    else if (mem >= 0x00900000 && mem <= 0x00940000)
        iram_heap_free(mem);
#else
    free(mem);
#endif
}

static inline void *snd_realloc(void *old, size_t newsize, int flags)
{
#ifdef UVOICE_ON_BK7251
    void *mem = NULL;
    if (old >= 0x00400020 && old <= 0x00440000) {
        mem = realloc(old, newsize);
    } else if (old >= 0x00900000 && old <= 0x00940000) {
        iram_heap_free(old);
        mem = iram_heap_malloc(newsize);
    }
    if (mem)
        memset(mem, 0, newsize);
    return mem;
#else
    void *mem = realloc(old, newsize);
    if (mem)
        memset(mem, 0, newsize);
    return mem;
#endif
}

static inline void os_msleep(int msec)
{
    aos_msleep(msec);
}

static inline void os_usleep(int usec)
{
    if (usec < 1000)
        aos_msleep(1);
    else
        aos_msleep((usec + 600) / 1000);
}

static inline long long os_current_time(void)
{
    return aos_now_ms();
}

static inline int os_get_mac_address(char *mac)
{
    uint8_t mac_addr[6];

    memset(mac_addr, 0, sizeof(mac_addr));
    if (hal_wifi_get_mac_addr(NULL, mac_addr))
        return -1;

    snprintf(mac, 18, "%02X:%02X:%02X:%02X:%02X:%02X",
        mac_addr[0], mac_addr[1], mac_addr[2],
        mac_addr[3], mac_addr[4], mac_addr[5]);

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

static inline int os_access(const char *filename, int mode)
{
    return access(filename, mode);
}

static inline os_file_t os_fopen(const char *filename, const char *mode)
{
    FILE *fp = fopen(filename, mode);
    return (os_file_t)fp;
}

static inline size_t os_fread(void *buffer, size_t size, size_t count, os_file_t fp)
{
    return fread(buffer, size, count, fp);
}

static inline size_t os_fwrite(const void *buffer, size_t size, size_t count, os_file_t fp)
{
    return fwrite(buffer, size, count, fp);
}

static inline long os_ftell(os_file_t fp)
{
    return ftell(fp);
}

static inline long os_fseek(os_file_t fp, long offset, int whence)
{
    return fseek(fp, offset, whence);
}

static inline char *os_fgets(char *buffer, int size, os_file_t fp)
{
    return fgets(buffer, size, fp);
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
    return feof(fp);
}

static inline int os_ferror(os_file_t fp)
{
    return ferror(fp);
}

static inline int os_fclose(os_file_t fp)
{
    return fclose(fp);
}

static inline int os_remove(const char *filename)
{
    return remove(filename);
}

typedef struct {
    aos_queue_t queue;
    uint8_t *buffer;
    int item_size;
} aos_mq_impl_t;

static inline int os_message_queue_send(os_message_queue_t mq, void *msg,
    unsigned int size, unsigned int timeout)
{
    return aos_queue_send(&((aos_mq_impl_t *)mq)->queue, msg, ((aos_mq_impl_t *)mq)->item_size);
}

static inline int os_message_queue_recv(os_message_queue_t mq, void *msg,
    unsigned int size, unsigned int timeout)
{
    return aos_queue_recv(&((aos_mq_impl_t *)mq)->queue, timeout, msg, &size);
}

static inline os_message_queue_t os_message_queue_create(int count, int size)
{
    aos_mq_impl_t *mq;
    int ret;

    mq = snd_zalloc(sizeof(aos_mq_impl_t), AFM_MAIN);
    if (!mq)
        return NULL;

    mq->buffer = (uint8_t *)snd_zalloc(count * size, AFM_MAIN);
    if (!mq->buffer) {
        snd_free(mq);
        return NULL;
    }
    mq->item_size = size;

    ret = aos_queue_new(&mq->queue, mq->buffer, count * size, size);
    if (ret) {
        snd_free(mq->buffer);
        snd_free(mq);
        return NULL;
    }

    return mq;
}

static inline int os_message_queue_free(os_message_queue_t mq)
{
    aos_queue_free(&((aos_mq_impl_t *)mq)->queue);
    snd_free(((aos_mq_impl_t *)mq)->buffer);
    snd_free(mq);
    return 0;
}

static inline int os_event_post(uint16_t type, uint16_t code, int value)
{
#ifdef AOS_LOOP
    return aos_post_event(type, code, value);
#else
    return -1;
#endif
}

static inline int os_event_register(uint16_t type, void *cb, void *data)
{
#ifdef AOS_LOOP
    return aos_register_event_filter(type, (aos_event_cb)cb, data);
#else
    return -1;
#endif
}

static inline int os_event_unregister(uint16_t type, void *cb, void *data)
{
#ifdef AOS_LOOP
    return aos_unregister_event_filter(type, (aos_event_cb)cb, data);
#else
    return -1;
#endif
}

static inline int os_mutex_lock(os_mutex_t mutex, unsigned int timeout)
{
    return aos_mutex_lock(mutex, timeout);
}

static inline int os_mutex_unlock(os_mutex_t mutex)
{
    return aos_mutex_unlock(mutex);
}

static inline os_mutex_t os_mutex_new(void)
{
    aos_mutex_t *mutex = snd_zalloc(sizeof(aos_mutex_t), AFM_MAIN);
    if (!mutex)
        return NULL;
    if (aos_mutex_new(mutex)) {
        snd_free(mutex);
        return NULL;
    }

    return mutex;
}

static inline void os_mutex_free(os_mutex_t mutex)
{
    aos_mutex_free(mutex);
    snd_free(mutex);
}

static inline int os_sem_is_valid(os_sem_t sem)
{
    return aos_sem_is_valid(sem);
}

static inline int os_sem_wait(os_sem_t sem, unsigned int timeout)
{
    return aos_sem_wait(sem, timeout);
}

static inline void os_sem_signal(os_sem_t sem)
{
    aos_sem_signal(sem);
}

static inline void os_sem_signal_all(os_sem_t sem)
{
    aos_sem_signal_all(sem);
}

static inline os_sem_t os_sem_new(int count)
{
    aos_sem_t *sem = snd_zalloc(sizeof(aos_sem_t), AFM_MAIN);
    if (!sem)
        return NULL;
    if (aos_sem_new(sem, count)) {
        snd_free(sem);
        return NULL;
    }
    return sem;
}

static inline void os_sem_free(os_sem_t sem)
{
    aos_sem_free(sem);
    snd_free(sem);
}

static inline int os_timer_change(os_timer_t timer, int internal_ms)
{
    return aos_timer_change(timer, internal_ms);
}

static inline int os_timer_start(os_timer_t timer)
{
    return aos_timer_start(timer);
}

static inline int os_timer_stop(os_timer_t timer)
{
    return aos_timer_stop(timer);
}

static inline os_timer_t os_timer_new(void (*func)(void *, void *), void *arg,
    int internal_ms, int repeat, unsigned char auto_run)
{
    aos_timer_t *timer = snd_zalloc(sizeof(aos_timer_t), AFM_MAIN);
    if (!timer)
        return NULL;
    if (aos_timer_new_ext(timer, func, arg, internal_ms, repeat, auto_run)) {
        snd_free(timer);
        return NULL;
    }

    return timer;
}

static inline void os_timer_free(os_timer_t timer)
{
    aos_timer_free(timer);
}

static inline int os_task_create(os_task_t *task, const char *name,
        void (*fn)(void *), void *arg, int stack_size, int pri)
{
    return aos_task_new_ext(task, name, fn, arg, stack_size, pri);
}

static inline int os_task_exit(os_task_t task)
{
    return 0;
}

static inline const char *os_partition_name(int pt)
{
    hal_logic_partition_t info;
    memset(&info, 0, sizeof(info));
    if (hal_flash_info_get((hal_partition_t)pt, &info))
        return NULL;
    return info.partition_description;
}

static inline int os_partition_size(int pt)
{
    hal_logic_partition_t info;
    memset(&info, 0, sizeof(info));
    if (hal_flash_info_get((hal_partition_t)pt, &info))
        return NULL;
    return info.partition_length;
}

static inline int os_partition_read(int pt, uint32_t *offset, uint8_t *buffer, uint32_t len)
{
    return hal_flash_read((hal_partition_t)pt, offset, buffer, len);
}

static inline int os_partition_write(int pt, uint32_t *offset, const uint8_t *buffer , uint32_t len)
{
    return hal_flash_write((hal_partition_t)pt, offset, buffer, len);
}

static inline int os_partition_erase(int pt, uint32_t offset, uint32_t len)
{
    return hal_flash_erase((hal_partition_t)pt, offset, len);
}


#endif /* __UVOICE_ALIOS_H__ */

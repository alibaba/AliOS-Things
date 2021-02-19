/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef __UVOICE_LINUX_H__
#define __UVOICE_LINUX_H__


#include <stdio.h>
#include <stddef.h>
#include <stdarg.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <dirent.h>
#include <semaphore.h>
#include <pthread.h>

typedef DIR					os_dir_t;
typedef struct dirent 		os_dirent_t;
typedef FILE *				os_file_t;

typedef int					os_queue_t;
typedef pthread_mutex_t *	os_mutex_t;
typedef sem_t *				os_sem_t;
typedef pthread_t 			os_task_t;
typedef struct timer_list *	os_timer_t;


#define OS_SEEK_SET			SEEK_SET
#define OS_SEEK_CUR			SEEK_CUR
#define OS_SEEK_END			SEEK_END

#define OS_F_OK				F_OK
#define OS_X_OK				X_OK
#define OS_W_OK				W_OK
#define OS_R_OK				R_OK

#define OS_FILE_OPEN_FAIL(stream)	(stream == NULL)
#define OS_FILE_OPENING(stream)		(stream != NULL)
#define OS_FILE_CLOSED				(NULL)

#define OS_WAIT_FOREVER		0xffffffffu

#define os_container_of(ptr, type, member)	\
	((type *)((char *)(ptr) - offsetof(type, member)))

enum {
	UVOICE_TASK_PRI_DEFAULT = 0,
	UVOICE_TASK_PRI_IDLE,
	UVOICE_TASK_PRI_LOWEST,
	UVOICE_TASK_PRI_LOWER,
	UVOICE_TASK_PRI_NORMAL,
	UVOICE_TASK_PRI_HIGHER,
	UVOICE_TASK_PRI_HIGHEST,
	UVOICE_TASK_PRI_REALTIME,
};


#ifdef UVOICE_BUILD_RELEASE
#define M_LOGD(fmt, ...)
#else
#define M_LOGD(fmt, ...)	printf("%s: "fmt, __func__, ##__VA_ARGS__)
#endif
#define M_LOGI(fmt, ...)	printf("%s: "fmt, __func__, ##__VA_ARGS__)
#define M_LOGW(fmt, ...)	printf("%s: "fmt, __func__, ##__VA_ARGS__)
#define M_LOGE(fmt, ...)	printf("%s: "fmt, __func__, ##__VA_ARGS__)
#define M_LOGR(fmt, ...)	printf(fmt, ##__VA_ARGS__)

#define AFM_MAIN		0x1
#define AFM_EXTN		0x2

static inline void *snd_zalloc(size_t size, int flags)
{
	void *mem = malloc(size);
	if (mem)
		memset(mem, 0, size);
	return mem;
}

static inline void snd_free(void *mem)
{
	free(mem);
}

static inline void *snd_realloc(void *old, size_t newsize, int flags)
{
	void *mem = realloc(old, newsize);
	if (mem)
		memset(mem, 0, newsize);
	return mem;
}

static inline void os_msleep(int msec)
{
	usleep(msec * 1000);
}

static inline void os_usleep(int usec)
{
	usleep(usec);
}

static inline long long os_current_time(void)
{
	struct timeval tt;
	gettimeofday(&tt, NULL);
	return tt.tv_sec * 1000 + tt.tv_usec / 1000;
}

static inline int os_get_mac_address(char *mac)
{
	return 0;
}


static inline int os_kv_get(const char *key, void *buffer, int *len)
{
	FILE *fp = fopen(key, "rb");
	if (!fp)
		return -ENOENT;
	fread(buffer, *len, 1, fp);
	if (ferror(fp))
		return -EIO;
	fclose(fp);
	return 0;
}

static inline int os_kv_set(const char *key, const void *buffer, int len, int sync)
{
	FILE *fp = fopen(key, "wb+");
	if (!fp)
		return -ENOENT;
	fwrite(buffer, len, 1, fp);
	if (ferror(fp))
		return -EIO;
	fclose(fp);
	return 0;
}

static inline int os_mkdir(const char *path)
{
	return mkdir(path, S_IRWXU); /* posix api, <dirent.h> required */
}

static inline os_dir_t *os_opendir(const char *path)
{
	return opendir(path);
}

static inline os_dirent_t *os_readdir(os_dir_t *dir)
{
	return readdir(dir);
}

static inline int os_closedir(os_dir_t *dir)
{
	return closedir(dir);
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

static inline int os_queue_send(os_queue_t *queue, void *msg, unsigned int size)
{
	return -1;
}

static inline int os_queue_recv(os_queue_t *queue, unsigned int ms, void *msg,
                   unsigned int *size)
{
	return -1;
}

static inline int os_queue_new(os_queue_t *queue, void *buffer, unsigned int size, int msg_size)
{
	return -1;
}

static inline void os_queue_free(os_queue_t *queue)
{
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
	return pthread_mutex_lock(mutex);
}

static inline int os_mutex_unlock(os_mutex_t mutex)
{
	return pthread_mutex_unlock(mutex);
}

static inline os_mutex_t os_mutex_new(void)
{
	pthread_mutex_t *p_mutex = snd_zalloc(sizeof(pthread_mutex_t), AFM_EXTN);
	if (!p_mutex)
		return NULL;
	if (pthread_mutex_init(p_mutex, NULL)) {
		snd_free(p_mutex);
		return NULL;
	}
	return p_mutex;
}

static inline void os_mutex_free(os_mutex_t mutex)
{
	pthread_mutex_destroy(mutex);
	snd_free(mutex);
}

static inline int os_sem_is_valid(os_sem_t sem)
{
	return 1;
}

static inline int os_sem_wait(os_sem_t sem, unsigned int timeout)
{
	struct timespec ts;
	struct timeval tt;
	gettimeofday(&tt, NULL);
	ts.tv_sec = tt.tv_sec + timeout / 1000;
	ts.tv_nsec = tt.tv_usec * 1000 + (timeout % 1000) * 1000 * 1000;
	ts.tv_sec += ts.tv_nsec / (1000 * 1000 * 1000);
	ts.tv_nsec %= (1000 * 1000 * 1000);
	if (timeout == OS_WAIT_FOREVER)
		return sem_wait(sem);
	return sem_timedwait(sem, &ts);
}

static inline void os_sem_signal(os_sem_t sem)
{
	sem_post(sem);
}

static inline void os_sem_signal_all(os_sem_t sem)
{
	//TODO: Fix
	sem_post(sem);
}

static inline os_sem_t os_sem_new(int count)
{
	sem_t *sem = snd_zalloc(sizeof(sem_t), AFM_MAIN);
	if (!sem)
		return NULL;
	if (sem_init(sem, 0, count)) {
		snd_free(sem);
		return NULL;
	}
	return sem;
}

static inline void os_sem_free(os_sem_t sem)
{
	sem_destroy(sem);
	snd_free(sem);
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
	pthread_attr_t attr;
	size_t def_stack_size = 0;
	int ret;

	pthread_attr_init(&attr);
	if (!strcmp(name, "uvoice_event_task"))
		pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
	pthread_attr_getstacksize(&attr, &def_stack_size);
	if (def_stack_size < stack_size)
		pthread_attr_setstacksize(&attr, stack_size);

	ret = pthread_create(task, &attr, fn, arg);
	pthread_attr_destroy(&attr);
	return ret;
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


#endif /* __UVOICE_LINUX_H__ */

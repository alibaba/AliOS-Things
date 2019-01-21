/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __IOT_IMPORT_H__
#define __IOT_IMPORT_H__
#if defined(__cplusplus)
extern "C" {
#endif

#ifdef _WIN32
#if !defined(CC_IS_MINGW32)
#ifdef DLL_HAL_EXPORTS
#define DLL_HAL_API __declspec(dllexport)
#else
#define DLL_HAL_API __declspec(dllimport)
#endif
#else
#define DLL_HAL_API
#endif
#else
#define DLL_HAL_API
#endif

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <inttypes.h>
#if defined(_PLATFORM_IS_LINUX_)
#include <pthread.h>
#endif

#ifndef _IN_
#define _IN_
#endif
#ifndef _OU_
#define _OU_
#endif

#define IOT_TRUE                    (1)     /* indicate boolean value true */
#define IOT_FALSE                   (0)     /* indicate boolean value false */

/** @defgroup group_platform platform
 *  @{
 */

#ifdef SIM7000C_DAM
#include "qapi_ali_iot.h"
#else

/*********************************** mutex interface ***********************************/

/** @defgroup group_platform_mutex mutex
 *  @{
 */

/**
 * @brief Create a mutex.
 *
 * @retval NULL : Initialize mutex failed.
 * @retval NOT_NULL : The mutex handle.
 * @see None.
 * @note None.
 */
DLL_HAL_API void *HAL_MutexCreate(void);

/**
 * @brief Destroy the specified mutex object, it will release related resource.
 *
 * @param [in] mutex @n The specified mutex.
 * @return None.
 * @see None.
 * @note None.
 */
DLL_HAL_API void HAL_MutexDestroy(_IN_ void *mutex);



/**
 * @brief Waits until the specified mutex is in the signaled state.
 *
 * @param [in] mutex @n the specified mutex.
 * @return None.
 * @see None.
 * @note None.
 */
DLL_HAL_API void HAL_MutexLock(_IN_ void *mutex);

/**
 * @brief Releases ownership of the specified mutex object..
 *
 * @param [in] mutex @n the specified mutex.
 * @return None.
 * @see None.
 * @note None.
 */
DLL_HAL_API void HAL_MutexUnlock(_IN_ void *mutex);

#define PLATFORM_WAIT_INFINITE (~0)

typedef enum {
    os_thread_priority_idle = -3,        /* priority: idle (lowest) */
    os_thread_priority_low = -2,         /* priority: low */
    os_thread_priority_belowNormal = -1, /* priority: below normal */
    os_thread_priority_normal = 0,       /* priority: normal (default) */
    os_thread_priority_aboveNormal = 1,  /* priority: above normal */
    os_thread_priority_high = 2,         /* priority: high */
    os_thread_priority_realtime = 3,     /* priority: realtime (highest) */
    os_thread_priority_error = 0x84,     /* system cannot determine priority or thread has illegal priority */
} hal_os_thread_priority_t;

typedef struct _hal_os_thread {
    hal_os_thread_priority_t priority;     /*initial thread priority */
    void                    *stack_addr;   /* thread stack address malloced by caller, use system stack by . */
    size_t                   stack_size;   /* stack size requirements in bytes; 0 is default stack size */
    int                      detach_state; /* 0: not detached state; otherwise: detached state. */
    char                    *name;         /* thread name. */
} hal_os_thread_param_t;

/**
 * @brief   按照指定入参创建一个线程
 *
 * @param[out] thread_handle @n The new thread handle, memory allocated before thread created and return it, free it after thread joined or exit.
 * @param[in] start_routine @n A pointer to the application-defined function to be executed by the thread.
        This pointer represents the starting address of the thread.
 * @param[in] arg @n A pointer to a variable to be passed to the start_routine.
 * @param[in] hal_os_thread_param @n A pointer to stack params.
 * @param[out] stack_used @n if platform used stack buffer, set stack_used to 1, otherwise set it to 0.
 * @return
   @verbatim
     = 0: on success.
     = -1: error occur.
   @endverbatim
 * @see None.
 * @note None.
 */
DLL_HAL_API int HAL_ThreadCreate(
            _OU_ void **thread_handle,
            _IN_ void *(*work_routine)(void *),
            _IN_ void *arg,
            _IN_ hal_os_thread_param_t *hal_os_thread_param,
            _OU_ int *stack_used);

/**
 * @brief   设置指定的线程为`Detach`状态
 *
 * @param[in] thread_handle: pointer to thread handle.
 * @return None.
 * @see None.
 * @note None.
 */
DLL_HAL_API void HAL_ThreadDetach(_IN_ void *thread_handle);

/**
 * @brief   杀死指定的线程
 *
 * @param[in] thread_handle: pointer to thread handle, NULL means itself
 * @return None.
 * @see None.
 * @note None.
 */
DLL_HAL_API void HAL_ThreadDelete(_IN_ void *thread_handle);

/**
 * @brief   创建一个计数信号量
 *
 * @return semaphore handle.
 * @see None.
 * @note The recommended value of maximum count of the semaphore is 255.
 */
DLL_HAL_API void *HAL_SemaphoreCreate(void);

/**
 * @brief   销毁一个计数信号量, 回收其所占用的资源
 *
 * @param[in] sem @n the specified sem.
 * @return None.
 * @see None.
 * @note None.
 */
DLL_HAL_API void HAL_SemaphoreDestroy(_IN_ void *sem);

/**
 * @brief   在指定的计数信号量上做自减操作并等待
 *
 * @param[in] sem @n the specified semaphore.
 * @param[in] timeout_ms @n timeout interval in millisecond.
     If timeout_ms is PLATFORM_WAIT_INFINITE, the function will return only when the semaphore is signaled.
 * @return
   @verbatim
   =  0: The state of the specified object is signaled.
   =  -1: The time-out interval elapsed, and the object's state is nonsignaled.
   @endverbatim
 * @see None.
 * @note None.
 */
DLL_HAL_API int HAL_SemaphoreWait(_IN_ void *sem, _IN_ uint32_t timeout_ms);

/**
 * @brief   在指定的计数信号量上做自增操作, 解除其它线程的等待
 *
 * @param[in] sem @n the specified semaphore.
 * @return None.
 * @see None.
 * @note None.
 */
DLL_HAL_API void HAL_SemaphorePost(_IN_ void *sem);

/** @} */ /* end of platform_mutex */


/** @defgroup group_platform_memory_manage memory
 *  @{
 */

/**
 * @brief Allocates a block of size bytes of memory, returning a pointer to the beginning of the block.
 *
 * @param [in] size @n specify block size in bytes.
 * @return A pointer to the beginning of the block.
 * @see None.
 * @note Block value is indeterminate.
 */
DLL_HAL_API void *HAL_Malloc(_IN_ uint32_t size);

/**
 * @brief Changes the size of the memory block pointed to by ptr to size bytes.
 *
 * @param [in] ptr  @n pointer to be realloc
 * @param [in] size @n specify block size in bytes for newly allocated memory
 * @return A pointer to the beginning of newly allocated memory.
 * @see None.
 * @note Block value is indeterminate.
 */
DLL_HAL_API void *HAL_Realloc(_IN_ void *ptr, _IN_ uint32_t size);

/**
 * @brief Allocates memory for an array of nmemb elements of size bytes each and returns a pointer to the allocated memory.
 *
 * @param [in] nmemb  @n array elements item counts
 * @param [in] size @n specify block size in bytes for every array elements
 * @return A pointer to the beginning of allocated memory.
 * @see None.
 * @note Block value is indeterminate.
 */
DLL_HAL_API void *HAL_Calloc(_IN_ uint32_t nmemb, _IN_ uint32_t size);

/**
 * @brief Deallocate memory block
 *
 * @param[in] ptr @n Pointer to a memory block previously allocated with platform_malloc.
 * @return None.
 * @see None.
 * @note None.
 */
DLL_HAL_API void HAL_Free(_IN_ void *ptr);


/** @} */ /* end of platform_memory_manage */

/** @defgroup group_platform_other other
 *  @{
 */

/**
 * @brief Retrieves the number of milliseconds that have elapsed since the system was boot.
 *
 * @return the number of milliseconds.
 * @see None.
 * @note None.
 */
DLL_HAL_API uint64_t HAL_UptimeMs(void);

/**
 * @brief Retrieves the timer string.
 *
 * @param [buf] give buffer to save timer string
 * @param [len] the length of buffer
 * @return the string of timer.
 * @see None.
 * @note None.
 */
DLL_HAL_API char *HAL_GetTimeStr(_IN_ char *buf, _IN_ int len);

/**
 * @brief Sleep thread itself.
 *
 * @param [in] ms @n the time interval for which execution is to be suspended, in milliseconds.
 * @return None.
 * @see None.
 * @note None.
 */
DLL_HAL_API void HAL_SleepMs(_IN_ uint32_t ms);

/**
 * @brief Set seed for a sequence of pseudo-random integers, which will be returned by HAL_Random()
 *
 * @param [in] seed @n A start point for the random number sequence
 * @return None.
 * @see None.
 * @note None.
 */
DLL_HAL_API void HAL_Srandom(_IN_ uint32_t seed);

/**
 * @brief Get a random integer
 *
 * @param [in] region @n Range of generated random numbers
 * @return Random number
 * @see None.
 * @note None.
 */
DLL_HAL_API uint32_t HAL_Random(_IN_ uint32_t region);

/**
 * @brief Writes formatted data to stream.
 *
 * @param [in] fmt: @n String that contains the text to be written, it can optionally contain embedded format specifiers
     that specifies how subsequent arguments are converted for output.
 * @param [in] ...: @n the variable argument list, for formatted and inserted in the resulting string replacing their respective specifiers.
 * @return None.
 * @see None.
 * @note None.
 */
DLL_HAL_API void HAL_Printf(_IN_ const char *fmt, ...);

/**
 * @brief Writes formatted data to string.
 *
 * @param [out] str: @n String that holds written text.
 * @param [in] len: @n Maximum length of character will be written
 * @param [in] fmt: @n Format that contains the text to be written, it can optionally contain embedded format specifiers
     that specifies how subsequent arguments are converted for output.
 * @param [in] ...: @n the variable argument list, for formatted and inserted in the resulting string replacing their respective specifiers.
 * @return bytes of character successfully written into string.
 * @see None.
 * @note None.
 */
DLL_HAL_API int HAL_Snprintf(_OU_ char *str, _IN_ const int len, _IN_ const char *fmt, ...);

/**
 * @brief Writes formatted data to string.
 *
 * @param [out] str: @n String that holds written text.
 * @param [in] len: @n Maximum length of character will be written.
 * @param [in] fmt: @n Format that contains the text to be written.
 * @param [in] ap:  @n the variable argument list.
 * @return bytes of character successfully format into string.
 * @see None.
 * @note None.
 */
DLL_HAL_API int HAL_Vsnprintf(_OU_ char *str, _IN_ const int len, _IN_ const char *fmt, _IN_ va_list ap);

/** @} */ /* end of group_platform_other */

/** @defgroup group_platform_network network
 *  @{
 */

#define NETWORK_ADDR_LEN        (16)        /* IP网络地址的长度 */
#define HAL_MAC_LEN             (17 + 1)    /* MAC地址的长度 */

typedef struct _network_addr_t {
    unsigned char
    addr[NETWORK_ADDR_LEN];         /* 目标UDP主机地址, 点分十进制IP地址 */
    unsigned short  port;           /* 目标UDP端口, 范围是0-65535 */
} NetworkAddr;

/**
 * @brief   获取Wi-Fi网口的MAC地址, 格式应当是"XX:XX:XX:XX:XX:XX"
 *
 * @param   mac_str : 用于存放MAC地址字符串的缓冲区数组
 * @return  指向缓冲区数组起始位置的字符指针
 */
DLL_HAL_API char *HAL_Wifi_Get_Mac(_OU_ char mac_str[HAL_MAC_LEN]);

/**
 * @brief   获取Wi-Fi网口的IP地址, 点分十进制格式保存在字符串数组出参, 二进制格式则作为返回值, 并以网络字节序(大端)表达
 *
 * @param   ifname : 指定Wi-Fi网络接口的名字
 * @param   ip_str : 存放点分十进制格式的IP地址字符串的数组
 * @return  二进制形式的IP地址, 以网络字节序(大端)组织
 */
DLL_HAL_API uint32_t HAL_Wifi_Get_IP(_OU_ char ip_str[NETWORK_ADDR_LEN], _IN_ const char *ifname);

/**
 * @brief check system network is ready(get ip address) or not.
 *
 * @param None.
 * @return 0, net is not ready; 1, net is ready.
 * @see None.
 * @note None.
 */
DLL_HAL_API int HAL_Sys_Net_Is_Ready();

/**
 * @brief reboot system immediately.
 *
 * @param None.
 * @return None.
 * @see None.
 * @note None.
 */
DLL_HAL_API void    HAL_Reboot(void);

DLL_HAL_API int     HAL_Kv_Set(const char *key, const void *val, int len, int sync);
DLL_HAL_API int     HAL_Kv_Get(const char *key, void *buffer, int *buffer_len);
DLL_HAL_API int     HAL_Kv_Del(const char *key);

DLL_HAL_API void   *HAL_Timer_Create(const char *name, void (*func)(void *), void *user_data);
DLL_HAL_API int     HAL_Timer_Start(void *t, int ms);
DLL_HAL_API int     HAL_Timer_Stop(void *t);
DLL_HAL_API int     HAL_Timer_Delete(void *timer);

/**
 * @brief Set the UTC time in milliseconds.
 *
 * @param[in] ms: @the time value to be set in milliseconds.
 * @return None.
 * @see None.
 * @note None.
 */
DLL_HAL_API void HAL_UTC_Set(long long ms);

/**
 * @brief Get the UTC time in milliseconds.
 *
 * @param None.
 * @return  the UTC time in milliseconds.
 * @see None.
 * @note None.
 */
DLL_HAL_API long long HAL_UTC_Get(void);

typedef enum {
    HAL_SEEK_SET,
    HAL_SEEK_CUR,
    HAL_SEEK_END
} hal_fs_seek_type_t;

/**
 * @brief Opens the file whose name is specified in the parameter filename and associates it
 *  with a stream that can be identified in future operations by the void pointer returned.
 *
 * @param [in] path: @n The file path to open.With reference to fopen
 * @param [in] mode: @n C string containing a file access mode.
 * @return If the file is successfully opened, the function returns a pointer to void object that can be used to
 * identify the stream on future operations.Otherwise, a null pointer is returned.
 * @see None.
 * @note None.
 */
DLL_HAL_API void *HAL_Fopen(const char *path, const char *mode);
/**
 * @brief Reads an array of count elements, each one with a size of size bytes, from the stream and
 * stores them in the block of memory specified by ptr.
 *
 * @param [in] buff: @n Pointer to a block of memory with a size of at least (size*count) bytes, converted to a void*.
 * @param [in] size: @n size in bytes, of each element to be read.
 * @param [in] count: @n Number of elements, each one with a size of size bytes.
 * @param [in] stream: @n Pointer to void that specifies an input stream.
 * @return The total number of elements successfully read is returned.If either size or count is zero, the function returns zero
 * @see None.
 * @note None.
 */
DLL_HAL_API uint32_t HAL_Fread(void *buff, uint32_t size, uint32_t count, void *stream);

/**
 * @brief Writes an array of count elements, each one with a size of size bytes, from the block of memory pointed
 * by ptr to the current position in the stream.
 *
 * @param [in] ptr: @n Pointer to the array of elements to be written, converted to a const void*.
 * @param [in] size: @n Size in bytes of each element to be written.
 * @param [in] count: @n Number of elements, each one with a size of size bytes.
 * @param [in] stream: @n Pointer to void that specifies an output stream.
 * @return The total number of elements successfully written is returned.If either size or count is zero, the function returns zero.
 * @see None.
 * @note None.
 */
DLL_HAL_API uint32_t HAL_Fwrite(const void *ptr, uint32_t size, uint32_t count, void *stream);

/**
 * @brief Sets the position indicator associated with the stream to a new position.
 *
 * @param [in] stream: @n Pointer to void that identifies the stream.
 * @param [in] offset: @n Binary files: Number of bytes to offset from origin.
 * @param [in] origin: @n Position used as reference for the offset. It is specified by one of value enum in hal_fs_seek_type_t.
 *
 * @return If successful, the function returns zero.Otherwise, it returns non-zero value.
 * @see None.
 * @note None.
 */
DLL_HAL_API int HAL_Fseek(void *stream, long offset, int origin);

/**
 * @brief Closes the file associated with the stream and disassociates it.
 *
 * @param [in] stream: @n Pointer to void that identifies the stream.
 *
 * @return If the stream is successfully closed, a zero value is returned.On failure, non-zero is returned.
 * @see None.
 * @note None.
 */
DLL_HAL_API int HAL_Fclose(void *stream);

/**
 * @brief Returns the current value of the position indicator of the stream.
 *
 * @param [in] stream: @n Pointer to void that identifies the stream.
 *
 * @return On success, the current value of the position indicator is returned.On failure, -1L is returned.
 * @see None.
 * @note None.
 */
DLL_HAL_API long HAL_Ftell(void *stream);

#include "iot_export.h"
#include "imports/iot_import_config.h"
#include "imports/iot_import_product.h"
#include "imports/iot_import_crypt.h"
#include "imports/iot_import_awss.h"
#include "imports/iot_import_dtls.h"
#include "imports/iot_import_tls.h"
#include "imports/iot_import_tcp.h"
#include "imports/iot_import_udp.h"
#include "imports/iot_import_ota.h"

#endif  /* SIM7000C_DAM */
#if defined(__cplusplus)
}
#endif
#endif  /* __IOT_IMPORT_H__ */


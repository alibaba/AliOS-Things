/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */


#ifndef __IOT_IMPORT_H__
#define __IOT_IMPORT_H__
#if defined(__cplusplus)
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <inttypes.h>
#if defined(_PLATFORM_IS_LINUX_)
#include <pthread.h>
#endif

#include "iot_export.h"
#include "imports/iot_import_config.h"
#include "imports/iot_import_product.h"
#include "imports/iot_import_crypt.h"
#include "imports/iot_import_coap.h"
#include "imports/iot_import_awss.h"
#include "imports/iot_import_dtls.h"
#include "imports/iot_import_uota.h"

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
void *HAL_MutexCreate(void);

/**
 * @brief Destroy the specified mutex object, it will release related resource.
 *
 * @param [in] mutex @n The specified mutex.
 * @return None.
 * @see None.
 * @note None.
 */
void HAL_MutexDestroy(_IN_ void *mutex);



/**
 * @brief Waits until the specified mutex is in the signaled state.
 *
 * @param [in] mutex @n the specified mutex.
 * @return None.
 * @see None.
 * @note None.
 */
void HAL_MutexLock(_IN_ void *mutex);



/**
 * @brief Releases ownership of the specified mutex object..
 *
 * @param [in] mutex @n the specified mutex.
 * @return None.
 * @see None.
 * @note None.
 */
void HAL_MutexUnlock(_IN_ void *mutex);


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
void *HAL_Malloc(_IN_ uint32_t size);

/**
 * @brief Changes the size of the memory block pointed to by ptr to size bytes.
 *
 * @param [in] ptr  @n pointer to be realloc
 * @param [in] size @n specify block size in bytes for newly allocated memory
 * @return A pointer to the beginning of newly allocated memory.
 * @see None.
 * @note Block value is indeterminate.
 */
void *HAL_Realloc(_IN_ void *ptr, _IN_ uint32_t size);

/**
 * @brief Allocates memory for an array of nmemb elements of size bytes each and returns a pointer to the allocated memory.
 *
 * @param [in] nmemb  @n array elements item counts
 * @param [in] size @n specify block size in bytes for every array elements
 * @return A pointer to the beginning of allocated memory.
 * @see None.
 * @note Block value is indeterminate.
 */
void *HAL_Calloc(_IN_ uint32_t nmemb, _IN_ uint32_t size);

/**
 * @brief Deallocate memory block
 *
 * @param[in] ptr @n Pointer to a memory block previously allocated with platform_malloc.
 * @return None.
 * @see None.
 * @note None.
 */
void HAL_Free(_IN_ void *ptr);


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
uint64_t HAL_UptimeMs(void);

/**
 * @brief Retrieves the timer string.
 *
 * @param [buf] give buffer to save timer string
 * @param [len] the length of buffer
 * @return the string of timer.
 * @see None.
 * @note None.
 */
char *HAL_GetTimeStr(_IN_ char *buf, _IN_ int len);

/**
 * @brief Sleep thread itself.
 *
 * @param [in] ms @n the time interval for which execution is to be suspended, in milliseconds.
 * @return None.
 * @see None.
 * @note None.
 */
void HAL_SleepMs(_IN_ uint32_t ms);

/**
 * @brief Set seed for a sequence of pseudo-random integers, which will be returned by HAL_Random()
 *
 * @param [in] seed @n A start point for the random number sequence
 * @return None.
 * @see None.
 * @note None.
 */
void HAL_Srandom(_IN_ uint32_t seed);

/**
 * @brief Get a random integer
 *
 * @param [in] region @n Range of generated random numbers
 * @return Random number
 * @see None.
 * @note None.
 */
uint32_t HAL_Random(_IN_ uint32_t region);

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
void HAL_Printf(_IN_ const char *fmt, ...);

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
int HAL_Snprintf(_OU_ char *str, _IN_ const int len, _IN_ const char *fmt, ...);

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
int HAL_Vsnprintf(_OU_ char *str, _IN_ const int len, _IN_ const char *fmt, _IN_ va_list ap);

/**
 * @brief Get vendor ID of hardware module.
 *
 * @param [out] pid_str: @n Get vendor ID of hardware module form HAL_GetParternID
 * @return the strlen of pid_str[] successfully written into.
 */
int HAL_GetPartnerID(_OU_ char pid_str[PID_STRLEN_MAX]);

/**
 * @brief Get Module ID of hardware module.
 *
 * @param [out] mid_str: @n Get Module ID of hardware module form HAL_GetModuleID
 * @return the strlen of mid_str[] successfully written into.
 */
int HAL_GetModuleID(_OU_ char mid_str[MID_STRLEN_MAX]);

/** @} */ /* end of group_platform_other */

/** @defgroup group_platform_network network
 *  @{
 */

/**
 * @brief Establish a TCP connection.
 *
 * @param [in] host: @n Specify the hostname(IP) of the TCP server
 * @param [in] port: @n Specify the TCP port of TCP server
 *
 * @return The handle of TCP connection.
   @retval   0 : Fail.
   @retval > 0 : Success, the value is handle of this TCP connection.
 */
uintptr_t HAL_TCP_Establish(_IN_ const char *host, _IN_  uint16_t port);


/**
 * @brief Destroy the specific TCP connection.
 *
 * @param [in] fd: @n Specify the TCP connection by handle.
 *
 * @return The result of destroy TCP connection.
 * @retval < 0 : Fail.
 * @retval   0 : Success.
 */
int32_t HAL_TCP_Destroy(_IN_ uintptr_t fd);


/**
 * @brief Write data into the specific TCP connection.
 *        The API will return immediately if 'len' be written into the specific TCP connection.
 *
 * @param [in] fd @n A descriptor identifying a connection.
 * @param [in] buf @n A pointer to a buffer containing the data to be transmitted.
 * @param [in] len @n The length, in bytes, of the data pointed to by the 'buf' parameter.
 * @param [in] timeout_ms @n Specify the timeout value in millisecond. In other words, the API block 'timeout_ms' millisecond maximumly.
 *
 * @retval      < 0 : TCP connection error occur..
 * @retval        0 : No any data be write into the TCP connection in 'timeout_ms' timeout period.
 * @retval (0, len] : The total number of bytes be written in 'timeout_ms' timeout period.

 * @see None.
 */
int32_t HAL_TCP_Write(_IN_ uintptr_t fd, _IN_ const char *buf, _IN_ uint32_t len, _IN_ uint32_t timeout_ms);


/**
 * @brief Read data from the specific TCP connection with timeout parameter.
 *        The API will return immediately if 'len' be received from the specific TCP connection.
 *
 * @param [in] fd @n A descriptor identifying a TCP connection.
 * @param [out] buf @n A pointer to a buffer to receive incoming data.
 * @param [out] len @n The length, in bytes, of the data pointed to by the 'buf' parameter.
 * @param [in] timeout_ms @n Specify the timeout value in millisecond. In other words, the API block 'timeout_ms' millisecond maximumly.
 *
 * @retval       -2 : TCP connection error occur.
 * @retval       -1 : TCP connection be closed by remote server.
 * @retval        0 : No any data be received in 'timeout_ms' timeout period.
 * @retval (0, len] : The total number of bytes be received in 'timeout_ms' timeout period.

 * @see None.
 */
int32_t HAL_TCP_Read(_IN_ uintptr_t fd, _OU_ char *buf, _OU_ uint32_t len, _IN_ uint32_t timeout_ms);

/**
 * @brief Establish a SSL connection.
 *
 * @param [in] host: @n Specify the hostname(IP) of the SSL server
 * @param [in] port: @n Specify the SSL port of SSL server
 * @param [in] ca_crt @n Specify the root certificate which is PEM format.
 * @param [in] ca_crt_len @n Length of root certificate, in bytes.
 * @return SSL handle.
 * @see None.
 * @note None.
 */
uintptr_t HAL_SSL_Establish(
            _IN_ const char *host,
            _IN_ uint16_t port,
            _IN_ const char *ca_crt,
            _IN_ size_t ca_crt_len);


/**
 * @brief Destroy the specific SSL connection.
 *
 * @param[in] handle: @n Handle of the specific connection.
 *
 * @return The result of destroy ssl
 *
 * @retval < 0 : Fail.
 * @retval   0 : Success.
 */
int32_t HAL_SSL_Destroy(_IN_ uintptr_t handle);


/**
 * @brief Write data into the specific SSL connection.
 *        The API will return immediately if 'len' be written into the specific SSL connection.
 *
 * @param [in] handle @n A descriptor identifying a connection.
 * @param [in] buf @n A pointer to a buffer containing the data to be transmitted.
 * @param [in] len @n The length, in bytes, of the data pointed to by the 'buf' parameter.
 * @param [in] timeout_ms @n Specify the timeout value in millisecond. In other words, the API block 'timeout_ms' millisecond maximumly.
 * @retval      < 0 : SSL connection error occur..
 * @retval        0 : No any data be write into the SSL connection in 'timeout_ms' timeout period.
 * @retval (0, len] : The total number of bytes be written in 'timeout_ms' timeout period.
 * @see None.
 */
int32_t HAL_SSL_Write(_IN_ uintptr_t handle, _IN_ const char *buf, _IN_ int len, _IN_ int timeout_ms);


/**
 * @brief Read data from the specific SSL connection with timeout parameter.
 *        The API will return immediately if 'len' be received from the specific SSL connection.
 *
 * @param [in] handle @n A descriptor identifying a SSL connection.
 * @param [out] buf @n A pointer to a buffer to receive incoming data.
 * @param [out] len @n The length, in bytes, of the data pointed to by the 'buf' parameter.
 * @param [in] timeout_ms @n Specify the timeout value in millisecond. In other words, the API block 'timeout_ms' millisecond maximumly.
 *
 * @retval       -2 : SSL connection error occur.
 * @retval       -1 : SSL connection be closed by remote server.
 * @retval        0 : No any data be received in 'timeout_ms' timeout period.
 * @retval (0, len] : The total number of bytes be received in 'timeout_ms' timeout period.
 * @see None.
 */
int32_t HAL_SSL_Read(_IN_ uintptr_t handle, _OU_ char *buf, _OU_ int len, _IN_ int timeout_ms);

/**
 * @brief Establish a UDP connection.
 *
 * @param [in] host: @n Specify the hostname(IP) of the UDP server
 * @param [in] port: @n Specify the UDP port of UDP server
 *
 * @retval  < 0 : Fail.
 * @retval >= 0 : Success, the value is handle of this UDP connection.
 * @see None.
 */
intptr_t HAL_UDP_create(_IN_ char *host, _IN_ unsigned short port);

/**
 * @brief Destroy the specific UDP connection.
 *
 * @param [in] p_socket: @n Specify the UDP connection by handle.
 * @return None.
 * @see None .
 */
void HAL_UDP_close(_IN_ intptr_t p_socket);

/**
 * @brief Write data into the specific UDP connection.
 *
 * @param [in] p_socket @n A descriptor identifying a connection.
 * @param [in] p_data @n A pointer to a buffer containing the data to be transmitted.
 * @param [in] datalen @n The length, in bytes, of the data pointed to by the 'p_data' parameter.

 * @retval          < 0 : UDP connection error occur.
 * @retval [0,datalen ] : The number of bytes sent.
 * @see None.
 */
int HAL_UDP_write(
            _IN_ intptr_t p_socket,
            _IN_ const unsigned char *p_data,
            _IN_ unsigned int datalen);

/**
 * @brief Read data from the specific UDP connection by blocked
 *
 * @param [in] p_socket @n A descriptor identifying a UDP connection.
 * @param [in] p_data @n A pointer to a buffer to receive incoming data.
 * @param [out] datalen @n The length, in bytes, of the data pointed to by the 'p_data' parameter.
 * @return
 *
 * @retval < 0 : UDP connect error occur.
 * @retval = 0 : End of file.
 * @retval > 0 : The number of byte read.
 * @see None.
 */
int HAL_UDP_read(
            _IN_ intptr_t p_socket,
            _OU_ unsigned char *p_data,
            _OU_ unsigned int datalen);

/**
 * @brief Read data from the specific UDP connection with timeout parameter.
 *        The API will return immediately if 'datalen' be received from the specific UDP connection.
 *
 * @param [in] p_socket @n A descriptor identifying a UDP connection.
 * @param [out] p_data @n A pointer to a buffer to receive incoming data.
 * @param [out] datalen @n The length, in bytes, of the data pointed to by the 'p_data' parameter.
 * @param [in] timeouf_ms @n Specify the timeout value in millisecond. In other words, the API block timeout_ms millisecond maximumly.
 *
 * @retval          -4 : UDP connect error occur.
 * @retval          -3 : The  call  was interrupted by a signal before any data was read.
 * @retval          -2 : No any data be received in 'timeout_ms' timeout period.
 * @retval          -1 : Invalid parameter.
 * @retval           0 : End of file.
 * @retval (0,datalen] : The number of byte read.
 * @see None.
 */
int HAL_UDP_readTimeout(
            _IN_ intptr_t p_socket,
            _OU_ unsigned char *p_data,
            _OU_ unsigned int datalen,
            _IN_ unsigned int timeout_ms);

/** @} */ /* end of platform_network */
/** @} */ /* end of platform */

#define NETWORK_ADDR_LEN        (16)    /* UDP网络地址的长度 */

typedef struct _network_addr_t {
    unsigned char
    addr[NETWORK_ADDR_LEN];         /* 目标UDP主机地址, 点分十进制IP地址 */
    unsigned short  port;           /* 目标UDP端口, 范围是0-65535 */
} NetworkAddr;

/**
 * @brief   获取Wi-Fi网口的IP地址, 点分十进制格式保存在字符串数组出参, 二进制格式则作为返回值, 并以网络字节序(大端)表达
 *
 * @param   ifname : 指定Wi-Fi网络接口的名字
 * @param   ip_str : 存放点分十进制格式的IP地址字符串的数组
 * @return  二进制形式的IP地址, 以网络字节序(大端)组织
 */
uint32_t HAL_Wifi_Get_IP(_OU_ char ip_str[NETWORK_ADDR_LEN], _IN_ const char *ifname);

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
int HAL_ThreadCreate(
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
void HAL_ThreadDetach(_IN_ void *thread_handle);

/**
 * @brief   杀死指定的线程
 *
 * @param[in] thread_handle: pointer to thread handle, NULL means itself
 * @return None.
 * @see None.
 * @note None.
 */
void HAL_ThreadDelete(_IN_ void *thread_handle);

/**
 * @brief   创建一个计数信号量
 *
 * @return semaphore handle.
 * @see None.
 * @note The recommended value of maximum count of the semaphore is 255.
 */
void *HAL_SemaphoreCreate(void);

/**
 * @brief   销毁一个计数信号量, 回收其所占用的资源
 *
 * @param[in] sem @n the specified sem.
 * @return None.
 * @see None.
 * @note None.
 */
void HAL_SemaphoreDestroy(_IN_ void *sem);

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
int HAL_SemaphoreWait(_IN_ void *sem, _IN_ uint32_t timeout_ms);

/**
 * @brief   在指定的计数信号量上做自增操作, 解除其它线程的等待
 *
 * @param[in] sem @n the specified semaphore.
 * @return None.
 * @see None.
 * @note None.
 */
void HAL_SemaphorePost(_IN_ void *sem);

/**
 * @brief   创建一个本地的UDP socket, 但并不发起任何网络交互
 *
 * @param   host : UDP的源地址, 如果不指定地址，设为 NULL
 *          port : UDP的源端口
 *
 * @retval  -1 : 创建失败
 * @retval  其它 : 创建成功, 返回值是UDP socket的句柄
 */
intptr_t HAL_UDP_create_without_connect(_IN_ const char *host, _IN_ unsigned short port);

/**
 * @brief   设置UDP socket的目的地址和目的端口
 *
 * @param   host :  UDP的目的地址
 *          port : UDP的目的端口
 *
 * @retval  -1 : 失败
 * @retval  0 : 设置成功
 */
int HAL_UDP_connect(_IN_ intptr_t sockfd,
                    _IN_ const char *host,
                    _IN_ unsigned short port);


/**
 * @brief   在指定的UDP socket上发送指定缓冲区的指定长度, 阻塞时间不超过指定时长, 且指定长度若发送完需提前返回
 *
 * @param   sockfd : UDP socket的句柄
 * @param   p_remote : 目标网络地址结构体的首地址
 * @param   p_data : 被发送的缓冲区起始地址
 * @param   datalen: 被发送的数据长度, 单位是字节(Byte)
 * @param   timeout_ms : 可能阻塞的最大时间长度, 单位是毫秒
 *
 * @retval  < 0 : 发送过程中出现错误或异常
 * @retval  0 : 在指定的'timeout_ms'时间间隔内, 没有任何数据被成功发送
 * @retval  (0, len] : 在指定的'timeout_ms'时间间隔内, 被成功发送的数据长度, 单位是字节(Byte)
 */
int HAL_UDP_sendto(_IN_ intptr_t          sockfd,
                   _IN_ const NetworkAddr *p_remote,
                   _IN_ const unsigned char *p_data,
                   _IN_ unsigned int datalen,
                   _IN_ unsigned int timeout_ms);

/**
 * @brief   在指定的UDP socket上发送指定缓冲区的指定长度, 阻塞时间不超过指定时长, 且指定长度若发送完需提前返回
 * @param   sockfd : UDP socket的句柄
 * @param   p_data : 被发送的缓冲区起始地址
 * @param   datalen: 被发送的数据长度, 单位是字节(Byte)
 * @param   timeout_ms : 可能阻塞的最大时间长度, 单位是毫秒
 *
 * @retval  < 0 : 发送过程中出现错误或异常
 * @retval  0 : 在指定的'timeout_ms'时间间隔内, 没有任何数据被成功发送
 * @retval  (0, len] : 在指定的'timeout_ms'时间间隔内, 被成功发送的数据长度, 单位是字节(Byte)
 *
 * @note    调用该接口之前需要调用HAL_UDP_connect设置好目的地址和端口。
 */
int HAL_UDP_send(_IN_ intptr_t sockfd,
                 _IN_ const unsigned char *p_data,
                 _IN_ unsigned int datalen,
                 _IN_ unsigned int timeout_ms);

/**
 * @brief   从指定的UDP句柄接收指定长度数据到缓冲区, 阻塞时间不超过指定时长, 且指定长度若接收完需提前返回, 源地址保存在出参中
 *
 * @param   fd : UDP socket的句柄
 * @param   p_remote : 存放源网络地址的结构体首地址
 * @param   p_data : 存放被接收数据的缓冲区起始地址
 * @param   datalen : 接收并存放到缓冲区中数据的最大长度, 单位是字节(Byte)
 * @param   timeout_ms : 可能阻塞的最大时间长度, 单位是毫秒
 *
 * @retval  < 0 : 接收过程中出现错误或异常
 * @retval  0 : 在指定的'timeout_ms'时间间隔内, 没有任何数据被成功接收
 * @retval  (0, len] : 在指定的'timeout_ms'时间间隔内, 被成功接收的数据长度, 单位是字节(Byte)
 */
int HAL_UDP_recvfrom(_IN_ intptr_t sockfd,
                     _OU_ NetworkAddr *p_remote,
                     _OU_ unsigned char *p_data,
                     _IN_ unsigned int datalen,
                     _IN_ unsigned int timeout_ms);

/**
* @brief   从指定的UDP句柄接收指定长度数据到缓冲区, 阻塞时间不超过指定时长, 且指定长度若接收完需提前返回, 源地址保存在出参中
*          调用该接口之前需要调用HAL_UDP_connect设置好目的地址和端口。
* @param   fd : UDP socket的句柄
* @param   p_data : 存放被接收数据的缓冲区起始地址
* @param   datalen : 接收并存放到缓冲区中数据的最大长度, 单位是字节(Byte)
* @param   timeout_ms : 可能阻塞的最大时间长度, 单位是毫秒
*
* @retval  < 0 : 接收过程中出现错误或异常
* @retval  0 : 在指定的'timeout_ms'时间间隔内, 没有任何数据被成功接收
* @retval  (0, len] : 在指定的'timeout_ms'时间间隔内, 被成功接收的数据长度, 单位是字节(Byte)
*/
int HAL_UDP_recv(_IN_ intptr_t sockfd,
                 _OU_ unsigned char *p_data,
                 _IN_ unsigned int datalen,
                 _IN_ unsigned int timeout_ms);


/**
 * @brief   在指定的UDP socket上发送加入组播组的请求
 *
 * @param   sockfd : 指定用来发送组播请求的UDP socket
 * @param   p_group : 指定需要加入的组播组名字
 * @retval  < 0 : 发送过程中出现异常或失败
 * @retval  0 : 发送成功
 */
int HAL_UDP_joinmulticast(_IN_ intptr_t sockfd,
                          _IN_ char *p_group);

/**
 * @brief   绑定UDP socket到指定接口，只接收来自该接口的数据包
 *
 * @param   fd : 指定用来绑定的UDP socket
 * @param   ifname : 指定用来绑定socket的网络接口名字
 *
 * @retval  < 0 : 绑定异常或失败
 * @retval  0 : 发送成功
 */
int HAL_UDP_bindtodevice(_IN_ intptr_t fd,
                         _IN_ const char *ifname);

/**
 * @brief   销毁指定的UDP socket, 回收资源
 *
 * @param   sockfd : 将要关闭并销毁的UDP socket
 *
 * @return  操作的结果
 * @retval  < 0 : 操作失败
 * @retval  0 : 操作成功
 */
int HAL_UDP_close_without_connect(_IN_ intptr_t sockfd);

/**
 * @brief reboot system immediately.
 *
 * @param None.
 * @return None.
 * @see None.
 * @note None.
 */
void HAL_Sys_reboot(void);

/***************************** firmware upgrade interface *****************************/

/** @defgroup group_platform_ota ota
 *  @{
 */


/**
 * @brief initialize a firmware upgrade.
 *
 * @param None
 * @return None.
 * @see None.
 * @note None.
 */
void HAL_Firmware_Persistence_Start(void);


/**
 * @brief save firmware upgrade data to flash.
 *
 * @param[in] buffer: @n A pointer to a buffer to save data.
 * @param[in] length: @n The length, in bytes, of the data pointed to by the buffer parameter.
 * @return 0, Save success; -1, Save failure.
 * @see None.
 * @note None.
 */
int HAL_Firmware_Persistence_Write(_IN_ char *buffer, _IN_ uint32_t length);


/**
 * @brief indicate firmware upgrade data complete, and trigger data integrity checking,
     and then reboot the system.
 *
 * @param None.
 * @return 0: Success; -1: Failure.
 * @see None.
 * @note None.
 */
int HAL_Firmware_Persistence_Stop(void);

int HAL_Kv_Set(const char *key, const void *val, int len, int sync);
int HAL_Kv_Get(const char *key, void *buffer, int *buffer_len);
int HAL_Kv_Del(const char *key);

void *HAL_Timer_Create(const char *name, void (*func)(void *), void *user_data);
int HAL_Timer_Start(void *t, int ms);
int HAL_Timer_Stop(void *t);
int HAL_Timer_Delete(void *timer);
void HAL_Reboot(void);

/**
 * @brief Set the UTC time in milliseconds.
 *
 * @param[in] ms: @the time value to be set in milliseconds.
 * @return None.
 * @see None.
 * @note None.
 */
void HAL_UTC_Set(long long ms);

/**
 * @brief Get the UTC time in milliseconds.
 *
 * @param None.
 * @return  the UTC time in milliseconds.
 * @see None.
 * @note None.
 */
long long HAL_UTC_Get(void);


/** @} */ //end of platform_firmware_upgrade
#endif  /* SIM7000C_DAM */

#if defined(__cplusplus)
}
#endif
#endif  /* __IOT_IMPORT_H__ */

/*
 * Copyright (c) 2014-2016 Alibaba Group. All rights reserved.
 * License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef __IOT_IMPORT_H__
#define __IOT_IMPORT_H__
#if defined(__cplusplus)
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <inttypes.h>


#ifdef NO_STDINT_H
typedef signed char             int8_t;
typedef signed short            int16_t;
typedef signed int              int32_t;
typedef unsigned char           uint8_t;
typedef unsigned short          uint16_t;
typedef unsigned int            uint32_t;
typedef unsigned long long      uint64_t;
#else
#include <stdint.h>
#endif

#define _IN_                /* 参数前缀,    表示后面跟着的参数是一个输入参数 */
#define _OU_                /* 参数前缀,    表示后面跟着的参数是一个输出参数 */
#define _IN_OPT_            /* 参数前缀,    表示后面跟着的参数是一个可选的输入参数 */
#define _OU_OPT_            /* 参数前缀,    表示后面跟着的参数是一个可选的输出参数 */
#define IOT_TRUE    (1)     /* 逻辑真 */
#define IOT_FALSE   (0)     /* 逻辑假 */


#define PID_STRLEN_MAX           (64)
#define MID_STRLEN_MAX           (64)
#define IOTX_URI_MAX_LEN         (135)  /* IoTx CoAP/HTTP URI & MQTT topic maximal length */

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
 * @brief   创建一个互斥锁
 *
 * @retval  NULL : 创建互斥锁失败
 * @retval  非空 : 创建互斥锁成功, 返回值即该互斥锁的句柄
 */
void *HAL_MutexCreate(void);

/**
 * @brief   销毁一个互斥锁, 释放其所占用的资源
 *
 * @param   mutex : 被销毁互斥锁的句柄
 * @return  无返回值
 */
void HAL_MutexDestroy(_IN_ void *mutex);

/**
 * @brief   给互斥锁加锁, 开始临界代码段
 *
 * @param   mutex : 互斥锁的句柄
 * @return  无返回值
 */
void HAL_MutexLock(_IN_ void *mutex);

/**
 * @brief   给互斥锁解锁, 结束临界代码段
 *
 * @param   mutex : 互斥锁的句柄
 * @return  无返回值
 */
void HAL_MutexUnlock(_IN_ void *mutex);

/**
 * @brief   按照入参size申请一片可用内存, 并返回内存的首地址
 *
 * @param   size : 需要申请的内存大小, 以字节(Byte)为单位
 * @retval  NULL : 申请内存失败
 * @retval  非空 : 申请内存成功, 返回值是该内存区域的起始地址
 */
void *HAL_Malloc(_IN_ uint32_t size);

/**
 * @brief   按照入参ptr释放一片内存
 *
 * @param   ptr : 需要释放的内存起始地址, 也是之前调用HAL_Malloc()的非空返回值
 * @return  无返回值
 */
void HAL_Free(_IN_ void *ptr);

/**
 * @brief   获取自加电以来到当前时间, 已经过去的毫秒数
 *
 * @return  加电以来的毫秒数
 */
uint64_t HAL_UptimeMs(void);

/**
 * @brief get time left
 *
 * @param [in] ms time of end. [in] ms time of now
 * @return time left.
 * @see None.
 * @note None.
 */
uint64_t aliot_platform_time_left(uint64_t t_end, uint64_t t_now);


/**
 * @brief   使调用线程睡眠入参ms指定的毫秒数
 *
 * @param   ms : 需要睡眠的时间长度, 单位是毫秒
 * @return  无返回值
 */
void HAL_SleepMs(_IN_ uint32_t ms);

/**
 * @brief   设置一个伪随机数种子, 影响HAL_Random()的返回值
 *
 * @param   seed : 被设置的种子参数
 * @return  无返回值
 */
void HAL_Srandom(_IN_ uint32_t seed);

/**
 * @brief   获取指定范围内的一个随机无符号整数
 *
 * @param   region : 给定一个范围, 返回值将在[0, region]之间随机分布
 * @return  随机无符号整数
 */
uint32_t HAL_Random(_IN_ uint32_t region);

/**
 * @brief   按照指定格式将字符串打印显示到终端
 *
 * @param   fmt : 打印格式参数, 可以内含%s和%d等占位符, 语法等同C标准函数printf()
 * @param   ... : 变参列表, 用于替换fmt中内含的%s和%d等占位符
 * @return  无返回值
 */
void HAL_Printf(_IN_ const char *fmt, ...);

/**
 * @brief   按照指定格式将一系列变量输出到指定的字符串, 变参是变量的形式
 *
 * @param   str : 存放输出结果的字符串
 * @param   len : 可以向目标字符串中输出的最大长度, 单位是字节, 包含结束符'\0'
 * @param   fmt : 打印格式参数, 可以内含%s和%d等占位符, 语法等同C标准函数printf()
 * @param   ... : 变参列表, 用于替换fmt中内含的%s和%d等占位符
 * @return  fmt格式字符串中用变参替换掉所有占位符之后的实际字符串长度, 可能超过入参len
 */
int HAL_Snprintf(_OU_ char *str, _IN_ const int len, _IN_ const char *fmt, ...);

/**
 * @brief   按照指定格式将一系列变量输出到指定的字符串, 变参是`va_list`的形式
 *
 * @param   str : 存放输出结果的字符串
 * @param   len : 可以向目标字符串中输出的最大长度, 单位是字节, 包含结束符'\0'
 * @param   fmt : 打印格式参数, 可以内含%s和%d等占位符, 语法等同C标准函数printf()
 * @param   ap :  变参列表, 用于替换fmt中内含的%s和%d等占位符
 * @return  fmt格式字符串中用变参替换掉所有占位符之后的实际字符串长度, 可能超过入参len
 */
int HAL_Vsnprintf(_OU_ char *str, _IN_ const int len, _IN_ const char *fmt, _IN_ va_list ap);

/**
 * @brief   按照指定的目标主机名和目标端口创建一个TCP连接, 并返回该连接的句柄
 *
 * @param   host : 指定目标主机的地址, 形式是域名或者点分十进制的IP地址
 * @param   port : 指定目标端口
 *
 * @return  被建立的TCP连接的句柄
   @retval  -1 : 建立TCP连接失败
   @retval  >= 0 : 建立TCP连接成功, 返回值用来标识被建立的TCP连接
 */
intptr_t HAL_TCP_Establish(_IN_ const char *host, _IN_  uint16_t port);

/**
 * @brief   断开TCP连接并销毁句柄, 回收资源
 *
 * @param   fd : 需要操作的TCP连接的句柄
 *
 * @return  断开和销毁的操作结果
 * @retval  < 0 : 操作失败
 * @retval  0 : 操作成功
 */
int32_t HAL_TCP_Destroy(_IN_ uintptr_t fd);

/**
 * @brief   在指定的TCP连接上发送指定缓冲区的指定长度, 阻塞时间不超过指定时长, 且指定长度若发送完需提前返回
 *
 * @param   fd : TCP连接的句柄
 * @param   buf : 被发送的缓冲区起始地址
 * @param   len : 被发送的数据长度, 单位是字节(Byte)
 * @param   timeout_ms : 可能阻塞的最大时间长度, 单位是毫秒
 *
 * @retval  < 0 : 发送过程中出现错误或异常
 * @retval  0 : 在指定的'timeout_ms'时间间隔内, 没有任何数据被成功发送
 * @retval  (0, len] : 在指定的'timeout_ms'时间间隔内, 被成功发送的数据长度, 单位是字节(Byte)
 */
int32_t HAL_TCP_Write(_IN_ uintptr_t fd, _IN_ const char *buf, _IN_ uint32_t len, _IN_ uint32_t timeout_ms);

/**
 * @brief   从指定的TCP连接上接收指定长度到指定缓冲区, 阻塞时间不超过指定时长, 且指定长度若接收完需提前返回
 *
 * @param   fd : TCP连接的句柄
 * @param   buf : 存放被接收数据的缓冲区起始地址
 * @param   len : 接收并存放到缓冲区中数据的最大长度, 单位是字节(Byte)
 * @param   timeout_ms : 可能阻塞的最大时间长度, 单位是毫秒
 *
 * @retval  -2 : TCP连接发生其它未定义错误
 * @retval  -1 : TCP连接被远端服务器关闭
 * @retval  0 : 在指定的'timeout_ms'时间间隔内, 没有任何数据被成功接收
 * @retval  (0, len] : 在指定的'timeout_ms'时间间隔内, 被成功接收的数据长度, 单位是字节(Byte)
 */
int32_t HAL_TCP_Read(_IN_ uintptr_t fd, _OU_ char *buf, _OU_ uint32_t len, _IN_ uint32_t timeout_ms);

/**
 * @brief   向指定的目标地址和端口建立一个TCP+SSL连接, 使用参数中指定的CA证书, 并返回SSL连接的句柄
 *
 * @param   host : 指定目标主机的地址, 形式是域名或者点分十进制的IP地址
 * @param   port : 指定目标端口
 * @param   ca_crt : 指定连接时用于验证对方的根证书字符串, 格式是PEM
 * @param   ca_crt_len : 指定根证书字符串的长度, 单位是字节(Byte)
 *
 * @retval  0 : 建立TCP+SSL连接失败
 * @retval  > 0 : 建立TCP+SSL连接成功, 返回值是该连接的句柄
 */
uintptr_t HAL_SSL_Establish(
            _IN_ const char *host,
            _IN_ uint16_t port,
            _IN_ const char *ca_crt,
            _IN_ size_t ca_crt_len);

/**
 * @brief   断开指定的TCP+SSL连接, 并销毁句柄, 回收资源
 *
 * @param   handle : 将要断开并销毁的TCP+SSL连接句柄
 *
 * @return  操作的结果
 * @retval  < 0 : 操作失败
 * @retval  0 : 操作成功
 */
int32_t HAL_SSL_Destroy(_IN_ uintptr_t handle);

/**
 * @brief   在指定的SSL连接上发送指定缓冲区的指定长度, 阻塞时间不超过指定时长, 且指定长度若发送完需提前返回
 *
 * @param   handle : TCP+SSL连接的句柄
 * @param   buf : 被发送的缓冲区起始地址
 * @param   len : 被发送的数据长度, 单位是字节(Byte)
 * @param   timeout_ms : 可能阻塞的最大时间长度, 单位是毫秒
 *
 * @retval  < 0 : 发送过程中出现错误或异常
 * @retval  0 : 在指定的'timeout_ms'时间间隔内, 没有任何数据被成功发送
 * @retval  (0, len] : 在指定的'timeout_ms'时间间隔内, 被成功发送的数据长度, 单位是字节(Byte)
 */
int32_t HAL_SSL_Write(_IN_ uintptr_t handle, _IN_ const char *buf, _IN_ int len, _IN_ int timeout_ms);


/**
 * @brief   从指定的TCP+SSL连接上接收指定长度到指定缓冲区, 阻塞时间不超过指定时长, 且指定长度若接收完需提前返回
 *
 * @param   handle : TCP+SSL连接的句柄
 * @param   buf : 存放被接收数据的缓冲区起始地址
 * @param   len : 接收并存放到缓冲区中数据的最大长度, 单位是字节(Byte)
 * @param   timeout_ms : 可能阻塞的最大时间长度, 单位是毫秒
 *
 * @retval  -2 : TCP+SSL连接发生其它未定义错误
 * @retval  -1 : TCP+SSL连接被远端服务器关闭
 * @retval  0 : 在指定的'timeout_ms'时间间隔内, 没有任何数据被成功接收
 * @retval  (0, len] : 在指定的'timeout_ms'时间间隔内, 被成功接收的数据长度, 单位是字节(Byte)
 */
int32_t HAL_SSL_Read(_IN_ uintptr_t handle, _OU_ char *buf, _OU_ int len, _IN_ int timeout_ms);

#define NETWORK_ADDR_LEN        (16)    /* UDP网络地址的长度 */

typedef struct _network_addr_t {
    unsigned char
    addr[NETWORK_ADDR_LEN];         /* 目标UDP主机地址, 点分十进制IP地址 */
    unsigned short  port;           /* 目标UDP端口, 范围是0-65535 */
} NetworkAddr;

/**
 * @brief   创建一个本地的UDP socket, 但并不发起任何网络交互
 *
 * @param   host : UDP的源地址, 如果不指定地址，设为 NULL
 *          port : UDP的源端口
 *
 * @retval  -1 : 创建失败
 * @retval  其它 : 创建成功, 返回值是UDP socket的句柄
 */
intptr_t HAL_UDP_create(_IN_ const char *host, _IN_ unsigned short port);

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
int HAL_UDP_close(_IN_ intptr_t sockfd);

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


/** @} */ //end of platform_firmware_upgrade

#include "imports/iot_import_config.h"
#include "imports/iot_import_product.h"


#if defined(__cplusplus)
}
#endif
#endif  /* __IOT_IMPORT_H__ */

/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __ALINK_OS_H__
#define __ALINK_OS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "product/product_config.h"
#include "product/product.h"
#include "product/product_asr.h"
#include "platform/platform.h"

/** @defgroup group_os os
 *  @{
 */

#define OS_SOCKET_MAXNUMS       PLATFORM_SOCKET_MAXNUMS
#define OS_WAIT_INFINITE        PLATFORM_WAIT_INFINITE
#define OS_INVALID_FD           PLATFORM_INVALID_FD
#define OS_MAX_SSID_LEN         PLATFORM_MAX_SSID_LEN
#define OS_MAX_PASSWD_LEN       PLATFORM_MAX_PASSWD_LEN


#define OS_CONFIG_SIZE          PLATFORM_CONFIG_SIZE
#define OS_MAC_LEN              PLATFORM_MAC_LEN
#define OS_IP_LEN               PLATFORM_IP_LEN
#define OS_CID_LEN              PLATFORM_CID_LEN
#define OS_ETH_ALEN     ETH_ALEN
#define OS_VERSION_LEN          STR_SHORT_LEN
#define OS_MODULE_NAME_LEN      STR_SHORT_LEN

#define OS_PRODUCT_NAME_LEN     PRODUCT_NAME_LEN
#define OS_PRODUCT_SN_LEN       PRODUCT_SN_LEN
#define OS_PRODUCT_MODEL_LEN    PRODUCT_MODEL_LEN
#define OS_PRODUCT_KEY_LEN      PRODUCT_KEY_LEN
#define OS_PRODUCT_SECRET_LEN   PRODUCT_SECRET_LEN
#define OS_PRODUCT_VERSION_LEN  PRODUCT_VERSION_LEN
#define OS_DEVICE_KEY_LEN      DEVICE_KEY_LEN
#define OS_DEVICE_SECRET_LEN   DEVICE_SECRET_LEN
/***************************************** Misc Interface *****************************************/

/** @defgroup group_misc misc
 *  @{
 */
/**
 * @brief init os infrastructure
 *
 * @param None.
 * @return None.
 * @see None.
 * @note None.
 */
void os_init(void);

/**
 * @brief deinit os infrastructure
 *
 * @param None.
 * @return None.
 * @see None.
 * @note None.
 */
void os_exit(void);

/**
 * @brief .
 *
 * @param None.
 * @return
 * @verbatim
 * = 0, This platform is little endian.
 * = 1, This platform is big endian.
 * @endverbatim
 * @see None.
 * @note None.
 */
static inline int os_is_big_endian(void)
{
    uint32_t data = 0xFF000000;

    if (0xFF == *(uint8_t *) & data) {
        return 1;                                   //big endian
    }

    return 0;                                       //little endian
}

/**
 * @brief Remove the specific value(char) in the buffer.
 *
 * @param[in] buf @n The specific buffer.
 * @param[in] c @n The specific value to be removed.
 * @param[in] length @n The length of the buffer, in bytes.
 * @return None.
 * @see None.
 * @note None.
 */
void os_squeeze(uint8_t buf[], int c, int length);

/**
 * @brief Convert the data from byte order of current CPU(the "host") to big-endian.
   The size of the data is 16 bits.
 *
 * @param[in] data @n Data to be converted.
 * @return Conversion result.
 * @see None.
 * @note None.
 */
uint16_t os_htobe16(uint16_t data);

/**
 * @brief Convert the data from host to little-endian.
   The size of the data is 16 bits.
 *
 * @param[in] data @n Data to be converted.
 * @return Conversion result.
 * @see None.
 * @note None.
 */
uint16_t os_htole16(uint16_t data);

/**
 * @brief Convert the data from big-endian to host.
   The size of the data is 16 bits.
 *
 * @param[in] data @n Data to be converted.
 * @return Conversion result.
 * @see None.
 * @note None.
 */
uint16_t os_be16toh(uint16_t data);

/**
 * @brief Convert the data from little-endian to host.
   The size of the data is 16 bits.
 *
 * @param[in] data @n Data to be converted.
 * @return Conversion result.
 * @see None.
 * @note None.
 */
uint16_t os_le16toh(uint16_t data);

/**
 * @brief Convert the data from host to big-endian.
   The size of the data is 32 bits.
 *
 * @param[in] data @n Data to be converted.
 * @return Conversion result.
 * @see None.
 * @note None.
 */
uint32_t os_htobe32(uint32_t data);

/**
 * @brief Convert the data from host to little-endian.
   The size of the data is 32 bits.
 *
 * @param[in] data @n Data to be converted.
 * @return Conversion result.
 * @see None.
 * @note None.
 */
uint32_t os_htole32(uint32_t data);

/**
 * @brief Convert the data from big endian to host.
   The size of the data is 32 bits.
 *
 * @param[in] data @n Data to be converted.
 * @return Conversion result.
 * @see None.
 * @note None.
 */
uint32_t os_be32toh(uint32_t data);

/**
 * @brief Convert the data from little-endian to host.
   The size of the data is 32 bits.
 *
 * @param[in] data @n Data to be converted.
 * @return Conversion result.
 * @see None.
 * @note None.
 */
uint32_t os_le32toh(uint32_t data);

/**
 * @brief Convert the data from host to big-endian.
   The size of the data is 64 bits.
 *
 * @param[in] data @n Data to be converted.
 * @return Conversion result.
 * @see None.
 * @note None.
 */
uint64_t os_htobe64(uint64_t data);

/**
 * @brief Convert the data from host to little-endian.
   The size of the data is 64 bits.
 *
 * @param[in] data @n Data to be converted.
 * @return Conversion result.
 * @see None.
 * @note None.
 */
uint64_t os_htole64(uint64_t data);

/**
 * @brief Convert the data from big-endian to host.
   The size of the data is 64 bits.
 *
 * @param[in] data @n Data to be converted.
 * @return Conversion result.
 * @see None.
 * @note None.
 */
uint64_t os_be64toh(uint64_t data);

/**
 * @brief Convert the data from little-endian to host.
   The size of the data is 64 bits.
 *
 * @param[in] data @n Data to be converted.
 * @return Conversion result.
 * @see None.
 * @note None.
 */
uint64_t os_le64toh(uint64_t data);

/**
 * @brief Get unaligned 16 bits data which is big-endian and convert to host byte order.
 *
 * @param[in] ptr @n The start address of the unaligned data.
 * @return Result in host byte order.
 * @see None.
 * @note None.
 */
uint16_t os_get_unaligned_be16(uint8_t *ptr);

/**
 * @brief Get unaligned 16 bits data which is little-endian and convert to host byte order.
 *
 * @param[in] ptr @n The start address of the unaligned data.
 * @return Result in host byte order.
 * @see None.
 * @note None.
 */
uint16_t os_get_unaligned_le16(uint8_t *ptr);

/**
 * @brief Get unaligned 32 bits data which is big-endian and convert to host byte order.
 *
 * @param[in] ptr @n The start address of the unaligned data.
 * @return Result in host byte order.
 * @see None.
 * @note None.
 */
uint32_t os_get_unaligned_be32(uint8_t *ptr);

/**
 * @brief Get unaligned 32 bits data which is little-endian and convert to host byte order.
 *
 * @param[in] ptr @n The start address of the unaligned data.
 * @return Result in host byte order.
 * @see None.
 * @note None.
 */
uint32_t os_get_unaligned_le32(uint8_t *ptr);

/** @} *///end of os_misc

/***************************************** Debug Interface *****************************************/

#define DEBUG_PUTS(fmt, args ...) \
do{ \
    platform_printf("\r\nfile = %s, line = %d.\r\n", __FILE__, __LINE__); \
    if ( NULL != fmt ){ \
        platform_printf(fmt, ## args); \
    } \
}while(0)

#define ASSERT_FAILED_DO() \
do{ \
    while(1){\
    DEBUG_PUTS("#####assert failed!######\r\n"); \
    os_msleep(1000); \
    }\
}while(0)

#define ASSERT(expr, fmt, args...) \
do{ \
    if (!(expr)) { \
        DEBUG_PUTS(fmt, ## args); \
        ASSERT_FAILED_DO(); \
    } \
}while(0)

#ifdef DEBUG

#define TRACE(fmt, args...) \
    DEBUG_PUTS(fmt, ## args)
#else

#define TRACE(fmt, args...) do{ }while(0)

#endif

/** @defgroup group_os_debug debug
 *  @{
 */

// * @param[in] ...: @n
// * @return None.

/**
 * @brief ASSERT macro, prints a specific string if assert fail.
 *
 * @param[in] expr @n The expression to be assert.
 * @param[in] fmt @n String that contains the output text if ASSERT fail(boolean value of the expr is false),
   can optionally contain embedded format specifiers that
   specifies how subsequent arguments are converted for output.
 * @param[in] args @n The variable argument list,
   for formatted and inserted in the resulting string replacing their respective specifiers.
 * @return None.
 * @see None.
 * @note None.
 */
#define OS_ASSERT(expr, fmt, args...) \
    ASSERT(expr, fmt, ## args)

#define OS_CHECK_PARAMS(expr) \
    ASSERT(expr, "invalid params")

#define OS_CHECK_MALLOC(expr) \
    ASSERT(expr, "malloc failed")

/**
 * @brief Trace macro, prints a string(contains file name and line number) argument to the standard output.
 *
 * @param[in] fmt @n .
 * @param[in] args @n The variable argument list.
 * @return None.
 * @see None.
 * @note None.
 */
#define OS_TRACE(fmt, args...) \
    TRACE(fmt, ## args)

/** @} *///end of os_debug

/***************************************** thread Interface *****************************************/

/** @defgroup group_os_thread thread
 *  @{
 */

/**
 * @brief create a thread.
 *
 * @param[in] name @n thread name.
 * @param[in] start_addr @n A pointer to the application-defined function to be executed by the thread.
        This pointer represents the starting address of the thread.
 * @param[in] arg @n A pointer to a variable to be passed to the thread.
 * @return @n thread handle
 * @see None.
 * @note None.
 * @warning Must use as a pair with os_thread_exit, otherwise will occur fatal error.
 */
void *os_thread_create(_IN_ const char *thread_name, _IN_ void *thread_func,
                       _IN_ void *arg);

/**
 * @brief exit itself
 *
 * @param[in] thread_info @n The specific thread.
 * @return
 * @verbatim
 * =  0, no exist.
 * =  1, exist.
 * @endverbatim
 * @see None.
 * @note None
 * @warning
   @verbatim
   1, Input parameter must be its own handle, otherwise will occur fatal error.
   2, Must use as a pair with os_thread_create, otherwise will occur fatal error.
   @endverbatim
 *
 */
void os_thread_exit(_IN_ void *thread_info);

/**
 * @brief check whether the specific thread exist or not.
 *
 * @param[in] thread_info @n the specific thread.
 * @return
   @verbatim
   =  0, no exist.
   =  1, exist.
   @endverbatim
 * @see none.
 * @note none.
 */
int os_thread_is_existence(_IN_ void *thread_info);

/**
 * @brief wait the specific thread to exit and then free the related resource.
 *
 * @param[in] thread_info @n the specific thread.
 * @return
   @verbatim
   =  0, on success.
   <  0, error.
   @endverbatim
 * @see none.
 * @note none.
 */
int os_thread_join(_IN_ void *thread_info);

//int os_thread_post_msg(void *thread_info, void *msg);

//void *os_thread_wait_msg(void *thread_info, uint32_t timeout);

/**
 * @brief sleep thread itself.
 *
 * @param[in] ms @n the time interval for which execution is to be suspended, in milliseconds.
 * @return None.
 * @see None.
 * @note None.
 */
static inline void os_msleep(_IN_ uint32_t ms)
{
    platform_msleep(ms);
}

/**
 * @brief sleep thread itself.
 *
 * @param[in] sec @n the time interval for which execution is to be suspended, in seconds.
 * @return None.
 * @see None.
 * @note None.
 */
static inline void os_sleep(_IN_ uint32_t sec)
{
    platform_msleep(sec * 1000);
}

/** @} */// end of os_thread

/***************************************** mutex Interface *****************************************/

/** @defgroup group_os_mutex mutex
 *  @{
 */

/**
 * @brief Create a mutex.
 *
 * @return Mutex handle.
 * @see None.
 * @note None.
 */
static inline void *os_mutex_init(void)
{
    return platform_mutex_init();
}

/**
 * @brief Destroy the specified mutex object, it will free related resource.
 *
 * @param[in] mutex @n The specified mutex.
 * @return None.
 * @see None.
 * @note None.
 */
static inline void os_mutex_destroy(_IN_ void *mutex)
{
    platform_mutex_destroy(mutex);
}

/**
 * @brief Waits until the specified mutex is in the signaled state.
 *
 * @param[in] mutex @n the specified mutex.
 * @return None.
 * @see None.
 * @note None.
 */
static inline void os_mutex_lock(_IN_ void *mutex)
{
    platform_mutex_lock(mutex);
}

/**
 * @brief Releases ownership of the specified mutex object..
 *
 * @param[in] mutex @n the specified mutex.
 * @return None.
 * @see None.
 * @note None.
 */
static inline void os_mutex_unlock(_IN_ void *mutex)
{
    platform_mutex_unlock(mutex);
}

/** @} */// end of os_mutex

/*************************************** semaphore Interface ***************************************/

/** @defgroup group_os_semaphore semaphore
 *  @{
 */

/**
 * @brief Create a semaphore.
 *
 * @return semaphore handle.
 * @see None.
 * @note The recommended value of maximum count of the semaphore is 255.
 */
static inline void *os_semaphore_init(void)
{
    return platform_semaphore_init();
}

/**
 * @brief Destroy the specified semaphore object, it will free related resource.
 *
 * @param[in] sem @n the specified sem.
 * @return None.
 * @see None.
 * @note None.
 */
static inline void os_semaphore_destroy(_IN_ void *sem)
{
    platform_semaphore_destroy(sem);
}

/**
 * @brief Wait until the specified mutex is in the signaled state or the time-out interval elapses.
 *
 * @param[in] sem @n the specified semaphore.
 * @param[in] timeout_ms @n timeout interval in millisecond.
     If timeout_ms is OS_WAIT_INFINITE, the function will return only when the semaphore is signaled.
 * @return
   @verbatim
   =  0: The state of the specified object is signaled.
   =  -1: The time-out interval elapsed, and the object's state is nonsignaled.
   @endverbatim
 * @see None.
 * @note None.
 */
static inline int os_semaphore_wait(_IN_ void *sem, _IN_ uint32_t timeout_ms)
{
    return platform_semaphore_wait(sem, timeout_ms);
}

/**
 * @brief Increases the count of the specified semaphore object by 1.
 *
 * @param[in] sem @n the specified semaphore.
 * @return None.
 * @see None.
 * @note None.
 */
static inline void os_semaphore_post(_IN_ void *sem)
{
    platform_semaphore_post(sem);
}

/** @} */// end of os_semaphore

/**************************************** memory Interface ****************************************/

/** @defgroup group_os_memory_manage memory
 *  @{
 */

/**
 * @brief Allocates a block of size bytes of memory, returning a pointer to the beginning of the block.
 *
 * @param[in] size @n specify block size in bytes.
 * @return A pointer to the beginning of the block.
 * @see None.
 * @note Block value is indeterminate.
 */
extern void *__os_malloc_debug(const char *func, uint32_t size);
#ifdef DEBUG
#define os_malloc(size)     __os_malloc_debug(__func__, size)
#else
static inline void *os_malloc(_IN_ uint32_t size)
{
    return platform_malloc(size);
}
#endif


/**
 * @brief Deallocate memory block
 *
 * @param[in] ptr @n Pointer to a memory block previously allocated with platform_malloc.
 * @return None.
 * @see None.
 * @note None.
 */
extern void __os_free_debug(const char *func, void *ptr);
#ifdef DEBUG
#define os_free(ptr)    __os_free_debug(__func__, ptr);
#else
static inline void os_free(_IN_ void *ptr)
{
    platform_free(ptr);
}
#endif

/**
 * @brief Allocates a memory block with memory initialized to 0,
     returning a pointer to the beginning of the block.
 *
 * @param[in] size @n specify block size in bytes.
 * @return A pointer to the beginning of the block.
 * @see None.
 * @note Block value is indeterminate.
 */
static inline void *os_zalloc(uint32_t size)
{
    void *ptr = os_malloc(size);
    if (ptr != NULL) {
        memset(ptr, 0, size);
    }

    return ptr;
}

/** @} */// end of os_memory_manage

/**************************************** network Interface ****************************************/

/** @defgroup group_os_network network
 *  @{
 */

typedef platform_netaddr_t os_netaddr_t, *pos_netaddr_t;

/**
 * @brief Converts a uint32_t from host to TCP/IP network byte order(which is big-endian).
 *
 * @param[in] n @n The data to be converted.
 * @return Converted result.
 * @see None.
 * @note.
 */
uint32_t os_htonl(uint32_t n);

/**
 * @brief Converts a uint32_t from TCP/IP network byte order(which is big-endian) to host byte order.
 *
 * @param[in] n @n The data to be converted.
 * @return Converted result.
 * @see None.
 * @note.
 */
uint32_t os_ntohl(uint32_t n);

/**
 * @brief Converts a uint16_t from host to TCP/IP network byte order(which is big-endian).
 *
 * @param[in] n @n The data to be converted.
 * @return Converted result.
 * @see None.
 * @note.
 */
uint16_t os_htons(uint16_t n);

/**
 * @brief Converts a uint16_t from TCP/IP network byte order(which is big-endian) to host byte order.
 *
 * @param[in] n @n The data to be converted.
 * @return Converted result.
 * @see None.
 * @note.
 */
uint16_t os_ntohs(uint16_t n);

/**
 * @brief Convert numeric IP address (in network order) into decimal dotted ASCII representation.
 *
 * @param[in] ip @n The IP address, in network order.
 * @param[out] buf @n The target buffer where the string is stored.
 * @return Pointer to buf which now holds the ASCIIrepresentation of addr.
 * @see None.
 * @note.
 */
char *os_ntoa(const uint32_t ip, char buf[OS_IP_LEN]);

/**
 * @brief Check whether ip_str is a valid ASCII representation of an Internet address and convert to a binary address.
 *
 * @param[in] ip_str @n IP address in ASCII representation (e.g. "127.0.0.1").
 * @return
   @verbatim
   =  0, on failure.
   >  0, the successful result.
   @endverbatim
 * @see None.
 * @note.
 */
uint32_t os_aton(const char *ip_str);

/**
 * @brief Create a udp server with the specified port.
 *
 * @param[in] port @n The specified udp sever listen port.
 * @return Server handle.
   @verbatim
   -1(OS_INVALID_FD): fail.
   >= 0: success.
   @endverbatim
 * @see None.
 */
static inline void *os_udp_server_create(_IN_ uint16_t port)
{
    return platform_udp_server_create(port);
}

/**
 * @brief Create a udp client.
 *
 * @param None
 * @return Client handle.
   @verbatim
   -1(OS_INVALID_FD): fail.
   >= 0: success.
   @endverbatim
 * @see None.
 * @note None.
 */
static inline void *os_udp_client_create(void)
{
    return platform_udp_client_create();
}

/**
 * @brief Add this host to the specified udp multicast group.
 *
 * @param[in] ip @n Specify the ip address, in big endian.
 * @param[in] port @n Specify the multicast port.
 * @return Multicast handle.
   @verbatim
   -1(OS_INVALID_FD): fail.
   >= 0: success.
   @endverbatim
 * @see None.
 * @note None.
 */
static inline void *os_udp_multicast_server_create(pos_netaddr_t netaddr)
{
    return platform_udp_multicast_server_create((pplatform_netaddr_t) netaddr);
}

/**
 * @brief Closes an existing udp connection.
 *
 * @param[in] handle @n the specified connection.
 * @return None.
 * @see None.
 * @note None.
 */
static inline void os_udp_close(void *handle)
{
    platform_udp_close(handle);
}

/**
 * @brief Sends data to a specific destination.
 *
 * @param[in] handle @n A descriptor identifying a connection.
 * @param[in] buffer @n A pointer to a buffer containing the data to be transmitted.
 * @param[in] length @n The length, in bytes, of the data pointed to by the buffer parameter.
 * @param[in] netaddr @n A pointer to a netaddr structure that contains the address of the target.
 *
 * @return
   @verbatim
   > 0: the total number of bytes sent, which can be less than the number indicated by length.
   = -1: error occur.
   @endverbatim
 * @see None.
 * @note blocking API.
 */
static inline int os_udp_sendto(_IN_ void *handle, _IN_ const char *buffer,
                                _IN_ uint32_t length, _IN_ pplatform_netaddr_t netaddr)
{
    return platform_udp_sendto(handle, buffer, length, netaddr);
}

/**
 * @brief Receives data from a udp connection.
 *
 * @param[in] handle @n A descriptor identifying a connection.
 * @param[out] buffer @n A pointer to a buffer to receive incoming data.
 * @param[in] length @n The length, in bytes, of the data pointed to by the buffer parameter.
 * @param[out] netaddr @n A pointer to a netaddr structure that contains the address of the source.
 * @return
   @verbatim
   >  0: The total number of bytes received, which can be less than the number indicated by length.
   <  0: Error occur.
   @endverbatim
 *
 * @see None.
 * @note blocking API.
 */
static inline int os_udp_recvfrom(_IN_ void *handle, _OUT_ char *buffer,
                                  _IN_ uint32_t length, _OUT_OPT_ pplatform_netaddr_t netaddr)
{
    return platform_udp_recvfrom(handle, buffer, length, netaddr);
}

/**
 * @brief Create a tcp server with the specified port.
 *
 * @param[in] port @n The specified tcp sever listen port.
 * @return Server handle.
   @verbatim
   -1(OS_INVALID_FD): fail.
   >= 0: success.
   @endverbatim
 * @see None.
 * @note None.
 */
static inline void *os_tcp_server_create(_IN_ uint16_t port)
{
    return platform_tcp_server_create(port);
}

/**
 * @brief Permits an incoming connection attempt on a tcp server.
 *
 * @param[in] server @n The specified tcp sever.
 * @return Connection handle.
   @verbatim
   -1(OS_INVALID_FD): fail.
   >= 0: success.
   @endverbatim
 * @see None.
 * @note None.
 */
static inline void *os_tcp_server_accept(_IN_ void *server)
{
    return platform_tcp_server_accept(server);
}

/**
 * @brief Establish a connection.
 *
 * @param[in] netaddr @n The destination address.
 * @return Connection handle
   @verbatim
   -1(OS_INVALID_FD): fail.
   >= 0: success.
   @endverbatim
 * @see None.
 * @note None.
 */
static inline void *os_tcp_client_connect(_IN_ pplatform_netaddr_t netaddr)
{
    return platform_tcp_client_connect(netaddr);
}

/**
 * @brief Sends data on a connection.
 *
 * @param[in] handle @n A descriptor identifying a connection.
 * @param[in] buffer @n A pointer to a buffer containing the data to be transmitted.
 * @param[in] length @n The length, in bytes, of the data pointed to by the buffer parameter.
 * @return
   @verbatim
   >  0: The total number of bytes sent, which can be less than the number indicated by length.
   <  0: Error occur.
   @endverbatim
 * @see None.
 * @note Blocking API.
 */
static inline int os_tcp_send(_IN_ void *handle, _IN_ const char *buffer,
                              _IN_ uint32_t length)
{
    return platform_tcp_send(handle, buffer, length);
}

/**
 * @brief Receives data from a tcp connection.
 *
 * @param[in] handle @n A descriptor identifying a connection.
 * @param[out] buffer @n A pointer to a buffer to receive incoming data.
 * @param[in] length @n The length, in bytes, of the data pointed to by the buffer parameter.
 * @return
   @verbatim
   >  0: The total number of bytes received, which can be less than the number indicated by length.
   <  0: Error occur.
   @endverbatim
 *
 * @see None.
 * @note Blocking API.
 */
static inline int os_tcp_recv(_IN_ void *handle, _OUT_ char *buffer,
                              _IN_ uint32_t length)
{
    return platform_tcp_recv(handle, buffer, length);
}

/**
 * @brief Closes an existing tcp connection.
 *
 * @param[in] handle @n the specified connection.
 * @return None.
 * @see None.
 * @note None.
 */
static inline void os_tcp_close(_IN_ void *handle)
{
    platform_tcp_close(handle);
}

/**
 * @brief Determines the status of one or more connection, waiting if necessary, to perform synchronous I/O.
 *
 * @param[in,out] handle_read @n
   @verbatim
   [in]: An optional pointer to a set of connection to be checked for readability.
         handle_read[n] > 0, care the connection, and the value is handle of the careful connection.
         handle_read[n] = OS_INVALID_FD, uncare.
   [out]: handle_read[n] = OS_INVALID_FD, the connection unreadable; != OS_INVALID_FD, the connection readable.
   @endverbatim
 * @param[in,out] handle_write: @n
   @verbatim
   [in]: An optional pointer to a set of connection to be checked for writability.
         handle_write[n] > 0, care the connection, and the value is handle of the careful connection.
         handle_write[n] = OS_INVALID_FD, uncare.
   [out]: handle_write[n] = OS_INVALID_FD, the connection unwritable; != OS_INVALID_FD, the connection wirteable.
   @endverbatim
 * @param[in] timeout_ms: @n Timeout interval in millisecond.
 * @return
   @verbatim
   =  0: The timeout interval elapsed.
   >  0: The total number of connection handles that are ready.
   <  0: A connection error occur.
   @endverbatim
 * @see None.
 * @note None.
 */
static inline int os_select(_INOUT_OPT_ void *read_fds[OS_SOCKET_MAXNUMS],
                            _INOUT_OPT_ void *write_fds[OS_SOCKET_MAXNUMS], _IN_ int timeout_ms)
{
    return platform_select(read_fds, write_fds, timeout_ms);
}

/** @} */// end of os_network

/****************************************** SSL Interface ******************************************/

/** @defgroup group_os_network ssl
 *  @{
 */

/**
 * @brief Establish a ssl connection.
 *
 * @param[in] handle @n The network connection handle.
 * @param[in] server_cert @n Specify the sever certificate.
 * @param[in] server_cert_len @n Length of sever certificate in bytes , '\0' is not included.
 * @return SSL handle.
 * @see None.
 * @note None.
 */
static inline void *os_ssl_connect(_IN_ void *tcp_fd,
                                   _IN_ const char *server_cert, _IN_ int server_cert_len)
{
    return platform_ssl_connect(tcp_fd, server_cert, server_cert_len);
}

/**
 * @brief Sends data on a ssl connection.
 *
 * @param[in] ssl @n A descriptor identifying a ssl connection.
 * @param[in] buffer @n A pointer to a buffer containing the data to be transmitted.
 * @param[in] length @n The length, in bytes, of the data pointed to by the buffer parameter.
 * @return
   @verbatim
   >  0: The total number of bytes sent, which can be less than the number indicated by length.
   <  0: Error occur.
   @endverbatim
 * @see None.
 * @note Blocking API.
 */
static inline int os_ssl_send(_IN_ void *ssl, _IN_ const char *buffer,
                              _IN_ int length)
{
    return platform_ssl_send(ssl, buffer, length);
}

/**
 * @brief Receives data from a ssl connection.
 *
 * @param[in] ssl @n A descriptor identifying a ssl connection.
 * @param[out] buffer @n A pointer to a buffer to receive incoming data.
 * @param[in] length @n The length, in bytes, of the data pointed to by the buffer parameter.
 * @return
   @verbatim
   >  0: The total number of bytes received, which can be less than the number indicated by length.
   <  0: Error occur.
   @endverbatim
 *
 * @see None.
 * @note blocking API.
 */
static inline int os_ssl_recv(_IN_ void *ssl, _IN_ char *buffer,
                              _IN_ int length)
{
    return platform_ssl_recv(ssl, buffer, length);
}

/**
 * @brief Closes an existing ssl connection.
 *
 * @param[in] ssl: @n the specified connection.
 * @return None.
 * @see None.
 * @note None.
 */
static inline void os_ssl_close(_IN_ void *ssl)
{
    platform_ssl_close(ssl);
}

/** @} */// end of os_ssl

/**************************************** system Interface ****************************************/

/** @defgroup group_os_system system
 *  @{
 */

/**
 * @brief check system network is ready(get ip address) or not.
 *
 * @param None.
 * @return 0, net is not ready; 1, net is ready.
 * @see None.
 * @note None.
 */
static inline int os_sys_net_is_ready(void)
{
    return platform_sys_net_is_ready();
}

/**
 * @brief wait until system network is ready(get ip address).
 *
 * @param None.
 * @return None
 * @see None.
 * @note None.
 */
static inline void os_sys_net_wait_ready(void)
{
    while (0 == platform_sys_net_is_ready()) {
        os_msleep(500);
    }
}

/**
 * @brief reboot system immediately.
 *
 * @param None.
 * @return None.
 * @see None.
 * @note None.
 */
static inline void os_sys_reboot(void)
{
    platform_sys_reboot();
}

/**
 *  * @brief Retrieves the number of milliseconds that have elapsed since the system was boot.
 *   *
 *    * @param None.
 *     * @return the number of milliseconds.
 *      * @see None.
 *       * @note Add a big offset, for easier caught time overflow bug.
 *        */
static inline uint32_t os_get_time_ms(void)
{
    static uint32_t fixed_delta;

    if (!fixed_delta) {
        fixed_delta = platform_get_time_ms() - 0xFFFF0000;
    }

    /* add a big offset, for easier caught time overflow bug */
    return platform_get_time_ms() - fixed_delta;
}

/*
 *  *  These inlines deal with timer wrapping correctly. You are
 *   *  strongly encouraged to use them
 *    *  1. Because people otherwise forget
 *     *  2. Because if the timer wrap changes in future you won't have to
 *      *     alter your code.
 *       */
static inline uint32_t time_elapsed_ms_since(uint32_t start_timestamp)
{
    uint32_t now = os_get_time_ms();

    return now - start_timestamp;
}

//os_get_utc_time() returns the time since the Epoch (00:00:00 UTC, January 1, 1970), measured in seconds.
static inline uint64_t os_get_utc_time(_INOUT_ uint64_t *p_utc)
{
    return platform_get_utc_time(p_utc);
}

static inline os_time_struct *os_local_time_r(const _IN_ uint64_t *p_utc,
                                              _OUT_ os_time_struct *p_result)
{
    return platform_local_time_r(p_utc, p_result);
}

/** @} */// end of os_system

/*********************************** firmware upgrade Interface ***********************************/

/** @defgroup group_os_ota ota
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
static inline void os_firmware_start(void)
{
    platform_flash_program_start();
}

/**
 * @brief save firmware upgrade data to flash.
 *
 * @param[in] buffer: @n A pointer to a buffer to save data.
 * @param[in] length: @n The length, in bytes, of the data pointed to by the buffer parameter.
 * @return 0, Save success; -1, Save failure.
 * @see None.
 * @note None.
 */
static inline int os_firmware_write(_IN_ char *buffer, _IN_ uint32_t length)
{
    return platform_flash_program_write_block(buffer, length);
}

/**
 * @brief indicate firmware upgrade data complete, and trigger data integrity checking,
     and then reboot the system.
 *
 * @param None.
 * @return 0: Success; -1: Failure.
 * @see None.
 * @note None.
 */
static inline int os_firmware_upgrade(void)
{
    return platform_flash_program_stop();
}

/** @} */// end of os_firmware_upgrade

/****************************************** io Interface ******************************************/

/** @defgroup group_os_io io
 *  @{
 */

/**
 * @brief Writes formatted data to stream.
 *
 * @param[in] fmt: @n String that contains the text to be written, it can optionally contain embedded format specifiers
     that specifies how subsequent arguments are converted for output.
 * @param[in] ...: @n the variable argument list, for formatted and inserted in the resulting string replacing their respective specifiers.
 * @return None.
 * @see None.
 * @note None.
 */
#ifndef os_printf
#define os_printf(fmt, args ...) \
    platform_printf(fmt, ## args)
#endif

/** @} */// end of os_io

/*************************************** wifi module Interface ***************************************/

/** @defgroup group_os_wifi_module wifi
 *  @{
 */

/**
 * @brief Get WIFI received signal strength indication(rssi).
 *
 * @param None.
 * @return The level number, in dBm.
 * @see None.
 * @note None.
 */
static inline int os_wifi_get_rssi_dbm(void)
{
    return platform_wifi_get_rssi_dbm();
}

/**
 * @brief Get WIFI MAC string with format like: XX:XX:XX:XX:XX:XX.
 *
 * @param[out] mac_str @n Buffer for using to store wifi MAC string.
 * @return A pointer to the start address of mac_str.
 * @see None.
 * @note this func will format mac address string uppercase
 */
char *os_wifi_get_mac_str(char mac_str[OS_MAC_LEN]);

/**
 * @brief Get WIFI MAC address(digital form)
 *
 * @param[out] mac @n digital buffer to hold mac address
 * @return A pointer to the start address of mac.
 * @see None.
 * @note None.
 */
uint8_t *os_wifi_get_mac(uint8_t mac[OS_ETH_ALEN]);

/**
 * @brief Get WIFI IP string with format like: xx:xx:xx:xx:xx:xx,
   and return IP with binary form, in network byte order.
 *
 * @param[out] ip_str @n Buffer for using to store IP string, in numbers-and-dots notation form.
 * @return IP address with binary form, in network byte order.
 * @see None.
 * @note None.
 */
static inline uint32_t os_wifi_get_ip(char ip_str[OS_IP_LEN])
{
    return platform_wifi_get_ip(ip_str);
}

/**
 * @brief wifi module enter power saving mode for a period
 *
 * @param[in] timeout_ms @n during this period, wifi module enter power saving
 *          mode
 * @return 0 for success, -1 otherwise
 * @see None.
 * @note None.
 */
static inline int os_wifi_low_power(int timeout_ms)
{
    return platform_wifi_low_power(timeout_ms);
}

/**
 * @brief Get unique chip id string.
 *
 * @param[out] cid_str @n Buffer for using to store chip id string.
 * @return A pointer to the start address of cid_str.
 * @see None.
 * @note None.
 */
static inline char *os_get_chipid(char cid_str[OS_CID_LEN])
{
    return platform_get_chipid(cid_str);
}

/**
 * @brief Get the os version of wifi module firmware.
 *
 * @param[in] version_str @n Buffer for using to store version string.
 * @return  A pointer to the start address of version_str.
 * @see None.
 * @note None.
 */
static inline char *os_get_version(char version_str[OS_VERSION_LEN])
{
    return platform_get_os_version(version_str);
}

/**
 * @brief Get wifi module name.
 *
 * @param[in] model_str @n Buffer for using to store name string.
 * @return  A pointer to the start address of name_str.
 * @see None.
 * @note None.
 */
static inline char *os_get_module_name(char name_str[OS_MODULE_NAME_LEN])
{
    return platform_get_module_name(name_str);
}

/** @} */// end of os_wifi_module

/**************************************** config interface ****************************************/

/** @defgroup group_os_config config
 *  @{
 */

/**
 * @brief Get flash(R/W) storage directory path.
 *  alink SDK use this path to store data profile
 *
 * @param None.
 * @return return storage path.
 * @see None.
 * @note None.
 */
static inline const char *os_get_storage_directory(void)
{
    return platform_get_storage_directory();
}

/**
 * @brief Read configure data from the start of configure zone.
 *
 * @param[in] buffer @n A pointer to a buffer to receive incoming data.
 * @param[in] length @n Specify read length, in bytes.
 * @return
   @verbatim
   =  0, read success.
   =  -1, read failure.
   @endverbatim
 * @see None.
 * @note None.
 */
static inline int os_config_read(char *buffer, int length)
{
    return platform_config_read(buffer, length);
}

/**
 * @brief Write configure data from the start of configure zone.
 *
 * @param[in] buffer @n A pointer to a buffer to receive incoming data.
 * @param[in] length @n Specify write length, in bytes.
 * @return
   @verbatim
   =  0, write success.
   =  -1, write failure.
   @endverbatim
 * @see None.
 * @note None.
 */
static inline int os_config_write(const char *buffer, int length)
{
    return platform_config_write(buffer, length);
}

/** @} */// end of os_config

/*************************************** awss interface ***************************************/

/** @defgroup group_os_awss alink wireless setup service(awss)
 *  @{
 */

/**
 * @brief Get timeout interval, in millisecond, of per awss.
 *
 * @param None.
 * @return The timeout interval.
 * @see None.
 * @note The recommended value is 60,000ms.
 */
static inline int os_awss_get_timeout_interval_ms(void)
{
    return platform_awss_get_timeout_interval_ms();
}

/**
 * @brief Get timeout interval in millisecond to connect the default SSID if awss timeout happens.
 *
 * @param None.
 * @return The timeout interval.
 * @see None.
 * @note The recommended value is 0ms, which mean forever.
 */
static inline int os_awss_get_connect_default_ssid_timeout_interval_ms( void )
{
    return platform_awss_get_connect_default_ssid_timeout_interval_ms();
}

/**
 * @brief Get time length, in millisecond, of per channel scan.
 *
 * @param None.
 * @return The timeout interval.
 * @see None.
 * @note None. The recommended value is between 200ms and 400ms.
 */
static inline int os_awss_get_channelscan_interval_ms(void)
{
    return platform_awss_get_channelscan_interval_ms();
}

typedef platform_awss_recv_80211_frame_cb_t os_awss_recv_80211_frame_cb_t;
/**
 * @brief Set wifi running at monitor mode,
   and register a callback function which will be called when wifi receive a frame.
 *
 * @param[in] cb @n A function pointer, called back when wifi receive a frame.
 * @return None.
 * @see None.
 * @note None.
 */
static inline void os_awss_open_monitor(os_awss_recv_80211_frame_cb_t cb)
{
    platform_awss_open_monitor(cb);
}

/**
 * @brief Close wifi monitor mode, and set running at station mode.
 *
 * @param None.
 * @return None.
 * @see None.
 * @note None.
 */
static inline void os_awss_close_monitor(void)
{
    platform_awss_close_monitor();
}

/**
 * @brief Switch to specific wifi channel.
 *
 * @param[in] primary_channel @n Primary channel.
 * @param[in] secondary_channel @n Auxiliary channel.
 * @param[in] bssid @n A pointer to wifi BSSID.
 * @return None.
 * @see None.
 * @note None.
 */
static inline void os_awss_switch_channel(char primary_channel,
                                          char secondary_channel, uint8_t bssid[OS_ETH_ALEN])
{
    platform_awss_switch_channel(primary_channel, secondary_channel, bssid);
}

/**
 * @brief Wifi AP connect function
 *
 * @param[in] connection_timeout_ms @n AP connection timeout in ms or PLATFORM_WAIT_INFINITE
 * @param[in] ssid @n AP ssid
 * @param[in] passwd @n AP passwd
 * @param[in] auth @n optional(AWSS_AUTH_TYPE_INVALID), AP auth info
 * @param[in] encry @n optional(AWSS_ENC_TYPE_INVALID), AP encry info
 * @param[in] bssid @n optional(NULL or zero mac address), AP bssid info
 * @param[in] channel @n optional, AP channel info
 * @return
   @verbatim
     = 0: connect AP & DHCP success
     = -1: connect AP or DHCP fail/timeout
   @endverbatim
 * @see None.
 * @note None.
 */
static inline int os_awss_connect_ap(
    _IN_ uint32_t connection_timeout_ms,
    _IN_ char ssid[OS_MAX_SSID_LEN],
    _IN_ char passwd[OS_MAX_PASSWD_LEN],
    _IN_OPT_ enum AWSS_AUTH_TYPE auth,
    _IN_OPT_ enum AWSS_ENC_TYPE encry,
    _IN_OPT_ uint8_t bssid[ETH_ALEN],
    _IN_OPT_ uint8_t channel)
{
    return platform_awss_connect_ap(connection_timeout_ms, ssid, passwd,
                                    auth, encry, bssid, channel);
}

#define os_wifi_scan_result_cb_t    platform_wifi_scan_result_cb_t

/**
 * @brief send 80211 raw frame in current channel with basic rate(1Mbps)
 *
 * @param[in] type @n see enum platform_awss_frame_type, currently only FRAME_BEACON
 *                      FRAME_PROBE_REQ is used
 * @param[in] buffer @n 80211 raw frame, include complete mac header & FCS field
 * @param[in] len @n 80211 raw frame length
 * @return
   @verbatim
   =  0, send success.
   = -1, send failure.
   = -2, unsupported.
   @endverbatim
 * @see None.
 * @note awss use this API send raw frame in wifi monitor mode & station mode
 */
static inline int os_wifi_send_80211_raw_frame(_IN_ enum
                                               platform_awss_frame_type type,
                                               _IN_ uint8_t *buffer, _IN_ int len)
{
    return platform_wifi_send_80211_raw_frame(type, buffer, len);
}

/**
 * @brief management frame handler
 *
 * @param[in] buffer @n 80211 raw frame or ie(information element) buffer
 * @param[in] len @n buffer length
 * @param[in] buffer_type @n 0 when buffer is a 80211 frame,
 *                          1 when buffer only contain IE info
 * @return None.
 * @see None.
 * @note None.
 */
#define os_wifi_mgnt_frame_cb_t     platform_wifi_mgnt_frame_cb_t

/**
 * @brief enable/disable filter specific management frame in wifi station mode
 *
 * @param[in] filter_mask @n see mask macro in enum platform_awss_frame_type,
 *                      currently only FRAME_PROBE_REQ_MASK & FRAME_BEACON_MASK is used
 * @param[in] vendor_oui @n oui can be used for precise frame match, optional
 * @param[in] callback @n see platform_wifi_mgnt_frame_cb_t, passing 80211
 *                      frame or ie to callback. when callback is NULL
 *                      disable sniffer feature, otherwise enable it.
 * @return
   @verbatim
   =  0, success
   = -1, fail
   = -2, unsupported.
   @endverbatim
 * @see None.
 * @note awss use this API to filter specific mgnt frame in wifi station mode
 */
static inline int os_wifi_enable_mgnt_frame_filter(
    _IN_ uint32_t filter_mask,
    _IN_OPT_ uint8_t vendor_oui[3],
    _IN_ platform_wifi_mgnt_frame_cb_t callback)
{
    return platform_wifi_enable_mgnt_frame_filter(filter_mask, vendor_oui,
                                                  callback);
}

/**
 * @brief find all the wifi AP around adapted with device.
 *
 * @param[in] cb @n pass ssid info(scan result) to this callback one by one
 * @return 0 for wifi scan is done, otherwise return -1
 * @see None.
 * @note
 *      This API should NOT exit before the invoking for cb is finished.
 *      This rule is something like the following :
 *      platform_wifi_scan() is invoked...
 *      ...
 *      for (ap = first_ap; ap <= last_ap; ap = next_ap){
 *          cb(ap)
 *      }
 *      ...
 *      platform_wifi_scan() exit...
 */
static inline int os_wifi_scan(_IN_ platform_wifi_scan_result_cb_t cb)
{
    return platform_wifi_scan(cb);
}

/**
 * @brief initialize AES struct.
 *
 * @param[in] key:
 * @param[in] iv:
 * @param[in] dir: AES_ENCRYPTION or AES_DECRYPTION
 * @return AES128_t
   @verbatim None
   @endverbatim
 * @see None.
 * @note None.
 */
static inline p_aes128_t os_aes128_init(
    _IN_ const uint8_t *key,
    _IN_ const uint8_t *iv,
    _IN_ AES_DIR_t dir)
{
    return platform_aes128_init(key, iv, dir);
}

/**
 * @brief release AES struct.
 *
 * @param[in] aes:
 * @return
   @verbatim
     = 0: succeeded
     = -1: failed
   @endverbatim
 * @see None.
 * @note None.
 */
static inline int os_aes128_destroy(
    _IN_ p_aes128_t aes)
{
    return platform_aes128_destroy(aes);
}

/**
 * @brief encrypt data with aes (cbc/128bit key).
 *
 * @param[in] aes: AES handler
 * @param[in] src: plain data
 * @param[in] blockNum: plain data number of 16 bytes size
 * @param[out] dst: cipher data
 * @return
   @verbatim
     = 0: succeeded
     = -1: failed
   @endverbatim
 * @see None.
 * @note None.
 */
static inline int os_aes128_cbc_encrypt(
    _IN_ p_aes128_t aes,
    _IN_ const void *src,
    _IN_ size_t blockNum,
    _OUT_ void *dst )
{
    return platform_aes128_cbc_encrypt(aes, src, blockNum, dst);
}

/**
 * @brief decrypt data with aes (cbc/128bit key).
 *
 * @param[in] aes: AES handler
 * @param[in] src: cipher data
 * @param[in] blockNum: plain data number of 16 bytes size
 * @param[out] dst: plain data
 * @return
   @verbatim
     = 0: succeeded
     = -1: failed
   @endverbatim
 * @see None.
 * @note None.
 */
static inline int os_aes128_cbc_decrypt(
    _IN_ p_aes128_t aes,
    _IN_ const void *src,
    _IN_ size_t blockNum,
    _OUT_ void *dst )
{
    return platform_aes128_cbc_decrypt(aes, src, blockNum, dst);
}

/**
* @brief Get device secret string.
*
* @param[out] secret_str @n Buffer for using to store secret string.
* @return A pointer to the start address of secret_str.
* @see None.
* @note None.
*/
static inline char *os_get_device_secret(char secret_str[OS_DEVICE_SECRET_LEN])
{
    return (char *)product_get_device_secret(secret_str);
}


/**
 * @brief get the information of the connected AP.
 *
 * @param[out] ssid: array to store ap ssid. It will be null if ssid is not required.
 * @param[out] passwd: array to store ap password. It will be null if ap password is not required.
 * @param[out] bssid: array to store ap bssid. It will be null if bssid is not required.
* @return
   @verbatim
     = 0: succeeded
     = -1: failed
   @endverbatim
 * @see None.
 * @note None.
 */
static inline int os_wifi_get_ap_info(
    _OUT_ char ssid[PLATFORM_MAX_SSID_LEN],
    _OUT_ char passwd[PLATFORM_MAX_PASSWD_LEN],
    _OUT_ uint8_t bssid[ETH_ALEN])
{
    return platform_wifi_get_ap_info(ssid, passwd, bssid);
}

/** @} */// end of os_awss

/** @defgroup group_os_product product
 *  @{
 */

/**
 * @brief Get the product name string.
 *
 * @param[in] name_str @n Buffer for using to store product name string.
 * @return The product name string.
 * @see None.
 * @note None
 */
static inline char *os_product_get_name(char name_str[OS_PRODUCT_NAME_LEN])
{
    return product_get_name(name_str);
}

/**
 * @brief Get the product version string.
 *
 * @param[in] version_str @n Buffer for using to store version string.
 * @return The version string.
 * @see None.
 * @note
 */
static inline char *os_product_get_version(char
                                           version_str[OS_PRODUCT_VERSION_LEN])
{
    return product_get_version(version_str);
}

/**
 * @brief Get product SN string.
 *
 * @param[out] sn_str @n Buffer for using to store SN string.
 * @return A pointer to the start address of sn_str.
 * @see None.
 * @note None.
 */
static inline char *os_product_get_sn(char sn_str[OS_PRODUCT_SN_LEN])
{
    return product_get_sn(sn_str);
}

/**
 *@brief Get device key string.
 *
 *@param[out] key_str @n Buffer for using to store debug key string.
 *@return A pointer to the start address of key_str.
 * @see None.
 * @note None.
 */
static inline char *os_get_device_key(char key_str[OS_DEVICE_KEY_LEN])
{
    return (char *)product_get_device_key(key_str);
}


/**
 * @brief Get product model string.
 *
 * @param[out] model_str @n Buffer for using to store model string.
 * @return A pointer to the start address of model_str.
 * @see None.
 * @note None.
 */
static inline char *os_product_get_model(char model_str[OS_PRODUCT_MODEL_LEN])
{
    return product_get_model(model_str);
}

/**
 * @brief Get product key string.
 *
 * @param[out] key_str @n Buffer for using to store key string.
 * @return A pointer to the start address of key_str.
 * @see None.
 * @note None.
 */
static inline char *os_product_get_key(char key_str[OS_PRODUCT_KEY_LEN])
{
    return product_get_key(key_str);
}

/**
 * @brief Get product secret string.
 *
 * @param[out] secret_str @n Buffer for using to store secret string.
 * @return A pointer to the start address of secret_str.
 * @see None.
 * @note None.
 */
static inline char *os_product_get_secret(char
                                          secret_str[OS_PRODUCT_SECRET_LEN])
{
    return product_get_secret(secret_str);
}

/**
 * @brief Get product debug key string.
 *
 * @param[out] key_str @n Buffer for using to store debug key string.
 * @return A pointer to the start address of key_str.
 * @see None.
 * @note None.
 */
static inline char *os_product_get_debug_key(char key_str[OS_PRODUCT_KEY_LEN])
{
    return product_get_debug_key(key_str);
}

/**
 * @brief Get product debug secret string.
 *
 * @param[out] secret_str @n Buffer for using to store debug secret string.
 * @return A pointer to the start address of secret_str.
 * @see None.
 * @note None.
 */
static inline char *os_product_get_debug_secret(char
                                                secret_str[OS_PRODUCT_SECRET_LEN])
{
    return product_get_debug_secret(secret_str);
}

static inline char *os_product_asr_get_appkey(char
                                              app_key[PRODUCT_ASR_APP_KEY_LEN])
{
    return product_asr_get_appkey(app_key);
}
static inline char *os_product_asr_get_secret(char
                                              app_secret[PRODUCT_SECRET_LEN])
{
    return product_asr_get_secret(app_secret);
}

static inline audio_format_t os_product_asr_get_audio_format()
{
    return product_asr_get_audio_format();
}

static inline char *os_product_get_asr_ca(void)
{
    return product_asr_get_ca();
}

typedef struct platform_ble_dev_config os_ble_dev_config;
/**
 * @brief Start BLE Service
 *
 * @param[in] dev_conf @n Device configuration
 * @return
 * @verbatim
 * = 0, success.
 * = -1, failed.
 * @endverbatim
 * @see None.
 * @note None.
 */
static inline int os_ble_start(os_ble_dev_config *dev_conf)
{
    return platform_ble_start(dev_conf);
}

/**
 * @brief End BLE Service
 * @return
 * @verbatim
 * = 0, success.
 * = -1, failed.
 * @endverbatim
 * @see None.
 * @note None.
 */
static inline int os_ble_end()
{
    return platform_ble_end();
}

/**
 * @brief Post Device data
 *
 * @param[in] buffer @n Data needing to post.
 * @param[in] length @n Length of Data.
 * @return
 * @verbatim
 * = 0, success.
 * = -1, failed.
 * @endverbatim
 * @see None.
 * @note None.
 */
static inline int os_ble_post(uint8_t *buffer, int length)
{
    return platform_ble_post(buffer, length);
}


/**
 * @brief Post Device data
 *
 * @param[in] buffer @n Data needing to post.
 * @param[in] length @n Length of Data.
 * @return
 * @verbatim
 * = 0, success.
 * = -1, failed.
 * @endverbatim
 * @see None.
 * @note None.
 */
static inline int os_ble_post_fast(uint8_t *buffer, int length)
{
    return platform_ble_post_fast(buffer, length);
}


/**
 * @brief Update advertise packet
 *
 * @param[in] buffer @n new advertising packet.
 * @param[in] length @n Length of advertising packet.
 * @return
 * @verbatim
 * = 0, success.
 * = -1, failed.
 * @endverbatim
 * @see None.
 * @note None.
 */
static inline int os_ble_update_adv(uint8_t *buffer, int length)
{
    return platform_ble_update_adv(buffer, length);
}

/* */// end of group_product


/** @} */// end of group_os

#ifdef __cplusplus
}
#endif

#endif /* SRC_OSA_ABSTRAC_H_ */

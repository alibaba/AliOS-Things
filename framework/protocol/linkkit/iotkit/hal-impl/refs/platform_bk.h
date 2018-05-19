/*
 * Copyright (c) 2014-2016 Alibaba Group. All rights reserved.
 *
 * Alibaba Group retains all right, title and interest (including all
 * intellectual property rights) in and to this computer program, which is
 * protected by applicable intellectual property laws.  Unless you have
 * obtained a separate written license from Alibaba Group., you are not
 * authorized to utilize all or a part of this computer program for any
 * purpose (including reproduction, distribution, modification, and
 * compilation into object code), and you must immediately destroy or
 * return to Alibaba Group all copies of this computer program.  If you
 * are licensed by Alibaba Group, your rights to utilize this computer
 * program are limited by the terms of that license.  To obtain a license,
 * please contact Alibaba Group.
 *
 * This computer program contains trade secrets owned by Alibaba Group.
 * and, unless unauthorized by Alibaba Group in writing, you agree to
 * maintain the confidentiality of this computer program and related
 * information and to not disclose this computer program and related
 * information to any other person or entity.
 *
 * THIS COMPUTER PROGRAM IS PROVIDED AS IS WITHOUT ANY WARRANTIES, AND
 * Alibaba Group EXPRESSLY DISCLAIMS ALL WARRANTIES, EXPRESS OR IMPLIED,
 * INCLUDING THE WARRANTIES OF MERCHANTIBILITY, FITNESS FOR A PARTICULAR
 * PURPOSE, TITLE, AND NONINFRINGEMENT.
 */
#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

/* mask this line, if stdint.h is not defined */
#define USE_STDINT_H

#ifdef USE_STDINT_H
    #include <stdint.h>
#else
    typedef signed char     int8_t;
    typedef signed short    int16_t;
    typedef signed int     int32_t;
    typedef unsigned char   uint8_t;
    typedef unsigned short  uint16_t;
    typedef unsigned int       uint32_t;
    typedef unsigned long long uint64_t;
#endif

/** @defgroup group_platform platform
 *  @{
 */


#define _IN_			/**< indicate that this is a input parameter. */
#define _OUT_			/**< indicate that this is a output parameter. */
#define _INOUT_			/**< indicate that this is a io parameter. */
#define _IN_OPT_		/**< indicate that this is a optional input parameter. */
#define _OUT_OPT_		/**< indicate that this is a optional output parameter. */
#define _INOUT_OPT_		/**< indicate that this is a optional io parameter. */


#define PLATFORM_SOCKET_MAXNUMS			(10)
#define PLATFORM_WAIT_INFINITE			(~0)
#define PLATFORM_INVALID_FD             ((void *)-1)

#define STR_SHORT_LEN                   (32)
#define STR_LONG_LEN                    (128)

#ifndef ETH_ALEN
#define ETH_ALEN                        (6)
#endif

/* ssid: 32 octets at most, include the NULL-terminated */
#define PLATFORM_MAX_SSID_LEN			(32 + 1)
/* password: 8-63 ascii */
#define PLATFORM_MAX_PASSWD_LEN			(64 + 1)

/*********************************** thread interface ***********************************/

/** @defgroup group_platform_thread thread
 *  @{
 */

/**
 * @brief create a thread.
 *
 * @param[out] thread @n The new thread handle.
 * @param[in] name @n thread name.
 * @param[in] start_routine @n A pointer to the application-defined function to be executed by the thread.
        This pointer represents the starting address of the thread.
 * @param[in] arg @n A pointer to a variable to be passed to the start_routine.
 * @param[in] stack @n A pointer to stack buffer malloced by caller, if platform used this buffer, set stack_used to non-zero value,  otherwise set it to 0.
 * @param[in] stack_size @n The initial size of the stack, in bytes. see platform_get_thread_stack_size().
 * @param[out] stack_used @n if platform used stack buffer, set stack_used to 1, otherwise set it to 0.
 * @return
   @verbatim
     = 0: on success.
     = -1: error occur.
   @endverbatim
 * @see None.
 * @note None.
 */
int platform_thread_create(
        _OUT_ void **thread,
        _IN_ const char *name,
        _IN_ void *(*start_routine) (void *),
        _IN_ void *arg,
        _IN_ void *stack,
        _IN_ uint32_t stack_size,
        _OUT_ int *stack_used);

/**
 * @brief exit the thread itself.
 *
 * @param[in] thread: itself thread handle.
 * @return None.
 * @see None.
 * @note None.
 */
void platform_thread_exit(_IN_ void *thread);

/**
 * @brief get thread stack size in this platform.
 *
 * @param[in] thread_name: specify the thread by uniform thread name.
 * @return thread name.
 * @see None.
 * @note there are 6 threads included:
 *   1) wsf_receive_worker;
 *   2) wsf_send_worker;
 *   3) wsf_callback_worker;
 *   4) fota_thread;
 *   5) alcs_thread;
 */
int platform_thread_get_stack_size(_IN_ const char *thread_name);

/**
 * @brief sleep thread itself.
 *
 * @param[in] ms @n the time interval for which execution is to be suspended, in milliseconds.
 * @return None.
 * @see None.
 * @note None.
 */
void platform_msleep(_IN_ uint32_t ms);


/** @} */ //end of platform_thread


/*********************************** mutex interface ***********************************/

/** @defgroup group_platform_mutex mutex
 *  @{
 */

/**
 * @brief Create a mutex.
 *
 * @return Mutex handle.
 * @see None.
 * @note None.
 */
void *platform_mutex_init(void);



/**
 * @brief Destroy the specified mutex object, it will free related resource.
 *
 * @param[in] mutex @n The specified mutex.
 * @return None.
 * @see None.
 * @note None.
 */
void platform_mutex_destroy(_IN_ void *mutex);



/**
 * @brief Waits until the specified mutex is in the signaled state.
 *
 * @param[in] mutex @n the specified mutex.
 * @return None.
 * @see None.
 * @note None.
 */
void platform_mutex_lock(_IN_ void *mutex);



/**
 * @brief Releases ownership of the specified mutex object..
 *
 * @param[in] mutex @n the specified mutex.
 * @return None.
 * @see None.
 * @note None.
 */
void platform_mutex_unlock(_IN_ void *mutex);


/** @} */ //end of platform_mutex


/********************************* semaphore interface *********************************/


/** @defgroup group_platform_semaphore semaphore
 *  @{
 */

/**
 * @brief Create a semaphore.
 *
 * @return semaphore handle.
 * @see None.
 * @note The recommended value of maximum count of the semaphore is 255.
 */
void *platform_semaphore_init(void);



/**
 * @brief Destroy the specified semaphore object, it will free related resource.
 *
 * @param[in] sem @n the specified sem.
 * @return None.
 * @see None.
 * @note None.
 */
void platform_semaphore_destroy(_IN_ void *sem);



/**
 * @brief Wait until the specified mutex is in the signaled state or the time-out interval elapses.
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
int platform_semaphore_wait(_IN_ void *sem, _IN_ uint32_t timeout_ms);



/**
 * @brief Increases the count of the specified semaphore object by 1.
 *
 * @param[in] sem @n the specified semaphore.
 * @return None.
 * @see None.
 * @note None.
 */
void platform_semaphore_post(_IN_ void *sem);




/** @} */ //end of platform_semaphore

/********************************** memory interface **********************************/


/** @defgroup group_platform_memory_manage memory
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
void *platform_malloc(_IN_ uint32_t size);


/**
 * @brief Deallocate memory block
 *
 * @param[in] ptr @n Pointer to a memory block previously allocated with platform_malloc.
 * @return None.
 * @see None.
 * @note None.
 */
void platform_free(_IN_ void *ptr);


/** @} */ //end of platform_memory_manage


/********************************** network interface **********************************/

/** @defgroup group_network network
 *  @{
 */

/**
 * @brief this is a network address structure, including host(ip or host name) and port.
 */
typedef struct
{
    char *host; /**< host ip(dotted-decimal notation) or host name(string) */
    uint16_t port; /**< udp port or tcp port */
} platform_netaddr_t, *pplatform_netaddr_t;



/**
 * @brief Create a udp server with the specified port.
 *
 * @param[in] port @n The specified udp sever listen port.
 * @return Server handle.
   @verbatim
   =  NULL: fail.
   != NULL: success.
   @endverbatim
 * @see None.
 * @note It is recommended to add handle value by 1, if 0(NULL) is a valid handle value in your platform.
 */
void *platform_udp_server_create(_IN_ uint16_t port);



/**
 * @brief Create a udp client.
 *
 * @param None
 * @return Client handle.
   @verbatim
   =  NULL: fail.
   != NULL: success.
   @endverbatim
 * @see None.
 * @note None.
 */
void *platform_udp_client_create(void);



/**
 * @brief Add this host to the specified udp multicast group.
 *
 * @param[in] netaddr @n Specify multicast address.
 * @param[in] ifname @n Specify interface name.
 * @return Multicast handle.
   @verbatim
   =  NULL: fail.
   != NULL: success.
   @endverbatim
 * @see None.
 * @note None.
 *
 */
void *platform_udp_multicast_server_create(platform_netaddr_t *netaddr, const char *ifname);

/**
 * @brief Closes an existing udp connection.
 *
 * @param[in] handle @n the specified connection.
 * @return None.
 * @see None.
 * @note None.
 */
void platform_udp_close(void *handle);



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
int platform_udp_sendto(
        _IN_ void *handle,
        _IN_ const char *buffer,
        _IN_ uint32_t length,
        _IN_ platform_netaddr_t *netaddr);


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
int platform_udp_recvfrom(
        _IN_ void *handle,
        _OUT_ char *buffer,
        _IN_ uint32_t length,
        _OUT_OPT_ platform_netaddr_t *netaddr);



/**
 * @brief Create a tcp server with the specified port.
 *
 * @param[in] port @n The specified tcp sever listen port.
 * @return Server handle.
   @verbatim
   =  NULL: fail.
   != NULL: success.
   @endverbatim
 * @see None.
 * @note None.
 */
void *platform_tcp_server_create(_IN_ uint16_t port);



/**
 * @brief Permits an incoming connection attempt on a tcp server.
 *
 * @param[in] server @n The specified tcp sever.
 * @return Connection handle.
 * @see None.
 * @note None.
 */
void *platform_tcp_server_accept(_IN_ void *server);



/**
 * @brief Establish a connection.
 *
 * @param[in] netaddr @n The destination address.
 * @return Connection handle
   @verbatim
   =  NULL: fail.
   != NULL: success.
   @endverbatim
 * @see None.
 * @note the func will block until tcp connect success or fail.
 */
void *platform_tcp_client_connect(_IN_ platform_netaddr_t *netaddr);




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
int platform_tcp_send(_IN_ void *handle, _IN_ const char *buffer, _IN_ uint32_t length);



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
int platform_tcp_recv(_IN_ void *handle, _OUT_ char *buffer, _IN_ uint32_t length);



/**
 * @brief Closes an existing tcp connection.
 *
 * @param[in] handle @n the specified connection.
 * @return None.
 * @see None.
 * @note None.
 */
void platform_tcp_close(_IN_ void *handle);




/**
 * @brief Determines the status of one or more connection, waiting if necessary, to perform synchronous I/O.
 *
 * @param[in,out] handle_read @n
   @verbatim
   [in]: An optional pointer to a set of connection to be checked for readability.
         handle_read[n] > 0, care the connection, and the value is handle of the careful connection.
         handle_read[n] = NULL, uncare.
   [out]: handle_read[n] = NULL, the connection unreadable; != NULL, the connection readable.
   @endverbatim
 * @param[in,out] handle_write: @n
   @verbatim
   [in]: An optional pointer to a set of connection to be checked for writability.
         handle_write[n] > 0, care the connection, and the value is handle of the careful connection.
         handle_write[n] = NULL, uncare.
   [out]: handle_write[n] = NULL, the connection unwritable; != NULL, the connection wirteable.
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
int platform_select(
        _INOUT_OPT_ void *read_fds[PLATFORM_SOCKET_MAXNUMS],
        _INOUT_OPT_ void *write_fds[PLATFORM_SOCKET_MAXNUMS],
        _IN_ int timeout_ms);


/** @} */ //end of platform_network



/************************************ SSL interface ************************************/

/** @defgroup group_platform_ssl ssl
 *  @{
 */

/**
 * @brief Establish a ssl connection.
 *
 * @param[in] tcp_fd @n The network connection handle.
 * @param[in] server_cert @n Specify the sever certificate which is PEM format, and
 *          both root cert(CA) and user cert should be supported
 * @param[in] server_cert_len @n Length of sever certificate, in bytes.
 * @return SSL handle.
 * @see None.
 * @note None.
 */
void *platform_ssl_connect(_IN_ void *tcp_fd, _IN_ const char *server_cert, _IN_ int server_cert_len);



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
int platform_ssl_send(_IN_ void *ssl, _IN_ const char *buffer, _IN_ int length);


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
int platform_ssl_recv(_IN_ void *ssl, _IN_ char *buffer, _IN_ int length);


/**
 * @brief Closes an existing ssl connection.
 *
 * @param[in] ssl: @n the specified connection.
 * @return None.
 * @see None.
 * @note None.
 */
int platform_ssl_close(_IN_ void *ssl);


/** @} */ //end of platform_ssl

/********************************** system interface **********************************/

/** @defgroup group_platform_system system
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
int platform_sys_net_is_ready(void);



/**
 * @brief reboot system immediately.
 *
 * @param None.
 * @return None.
 * @see None.
 * @note None.
 */
void platform_sys_reboot(void);



/**
 * @brief Retrieves the number of milliseconds that have elapsed since the system was boot.
 *
 * @param None.
 * @return the number of milliseconds.
 * @see None.
 * @note None.
 */
uint32_t platform_get_time_ms(void);

typedef struct {
   int tm_sec;         /* seconds */
   int tm_min;         /* minutes */
   int tm_hour;        /* hours */
   int tm_mday;        /* day of the month */
   int tm_mon;         /* month */
   int tm_year;        /* year */
   int tm_wday;        /* day of the week */
   int tm_yday;        /* day in the year */
   int tm_isdst;       /* daylight saving time */
}os_time_struct;

uint64_t platform_get_utc_time(_INOUT_ uint64_t *p_utc);

os_time_struct *platform_local_time_r(const _IN_ uint64_t *p_utc,_OUT_ os_time_struct *p_result);

/**
 * @brief get the available memroy size in bytes
 *
 * @param None.
 * @return free memory size in bytes
 * @see None.
 * @note None.
 */
int platform_get_free_memory_size(void);


/** @} */ //end of platform_system

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
void platform_flash_program_start(void);


/**
 * @brief save firmware upgrade data to flash.
 *
 * @param[in] buffer: @n A pointer to a buffer to save data.
 * @param[in] length: @n The length, in bytes, of the data pointed to by the buffer parameter.
 * @return 0, Save success; -1, Save failure.
 * @see None.
 * @note None.
 */
int platform_flash_program_write_block(_IN_ char *buffer, _IN_ uint32_t length);


/**
 * @brief indicate firmware upgrade data complete, and trigger data integrity checking,
     and then reboot the system.
 *
 * @param None.
 * @return 0: Success; -1: Failure.
 * @see None.
 * @note None.
 */
int platform_flash_program_stop(void);


/** @} */ //end of platform_firmware_upgrade

/************************************ io interface ************************************/

/** @defgroup group_platform_io io
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
void platform_printf(_IN_ const char *fmt, ...);


/** @} */ //end of group_io

/********************************** config interface **********************************/

/** @defgroup group_platform_config config
 *  @{
 */

/**
 * @brief Get flash(R/W) storage directory path.
 * 	alink SDK use this path to store data profile
 *
 * @param None.
 * @return return storage path.
 * @see None.
 * @note None.
 */
    const char *platform_get_storage_directory(void);

#define PLATFORM_CONFIG_SIZE	(2048)

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
int platform_config_read(char *buffer, int length);


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
int platform_config_write(const char *buffer, int length);


/** @} */ //end of platform_config

/******************************** wifi module interface ********************************/

/** @defgroup group_platform_wifi_module wifi related
 *  @{
 */

/**
 * @brief Get model of the wifi module.
 *
 * @param[in] model_str @n Buffer for using to store model string.
 * @return  A pointer to the start address of model_str.
 * @see None.
 * @note None.
 */
char *platform_get_module_name(char name_str[STR_SHORT_LEN]);


/**
 * @brief Get WIFI received signal strength indication(rssi).
 *
 * @param None.
 * @return The level number, in dBm.
 * @see None.
 * @note None.
 */
int platform_wifi_get_rssi_dbm(void);

/**
 * @brief wifi module enter power saving mode for a period
 *
 * @param[in] timeout_ms @n during this period, wifi module enter power saving
 *          mode
 * @return 0 for success, -1 otherwise
 * @see None.
 * @note sample code
 * int platform_wifi_low_power(int timeout_ms)
 * {
 *      wifi_enter_power_saving_mode();
 *      msleep(timeout_ms);
 *      wifi_exit_power_saving_mode();
 *
 *      return 0;
 * }
 */
int platform_wifi_low_power(int timeout_ms);

/**
 * @brief Get WIFI received signal strength indication(rssi).
 *
 * @param None.
 * @return The level number, in dBm.
 * @see None.
 * @note None.
 */
int platform_rf433_get_rssi_dbm(void);


#define PLATFORM_MAC_LEN	(17 + 1)
/**
 * @brief Get WIFI MAC string, format should be XX:XX:XX:XX:XX:XX
 *
 * @param[out] mac_str @n Buffer for using to store wifi MAC string.
 * @return A pointer to the start address of mac_str.
 * @see None.
 * @note None.
 */
char *platform_wifi_get_mac(char mac_str[PLATFORM_MAC_LEN]);


#define PLATFORM_IP_LEN    (15 + 1)
/**
 * @brief Get WIFI IP string with format like: xx:xx:xx:xx:xx:xx,
   and return IP with binary form, in network byte order.
 *
 * @param[in] ifname @n Specify interface name.
 * @param[out] ip_str @n Buffer for using to store IP string, in numbers-and-dots notation form.
 * @return IP with binary form, in network byte order.
 * @see None.
 * @note None.
 */
uint32_t platform_wifi_get_ip(char ip_str[PLATFORM_IP_LEN], const char *ifname);


#define PLATFORM_CID_LEN (64 + 1)
/**
 * @brief Get unique chip id string.
 *
 * @param[out] cid_str @n Buffer for using to store chip id string.
 * @return A pointer to the start address of cid_str.
 * @see None.
 * @note None.
 */
char *platform_get_chipid(char cid_str[PLATFORM_CID_LEN]);


/**
 * @brief Get the os version of wifi module firmware.
 *
 * @param[in] version_str @n Buffer for using to store version string.
 * @return  A pointer to the start address of version_str.
 * @see None.
 * @note None.
 */
char *platform_get_os_version(char version_str[STR_SHORT_LEN]);


/** @} */ //end of platform_wifi_module


/************************* awss(alink wireless setup service) interface ***************************/

/** @defgroup group_platform_awss alink wireless setup service(awss)
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
int platform_awss_get_timeout_interval_ms(void);

/**
 * @brief Get timeout interval in millisecond to connect the default SSID if awss timeout happens.
 *
 * @param None.
 * @return The timeout interval.
 * @see None.
 * @note The recommended value is 0ms, which mean forever.
 */
int platform_awss_get_connect_default_ssid_timeout_interval_ms(void);

/**
 * @brief Get time length, in millisecond, of per channel scan.
 *
 * @param None.
 * @return The timeout interval.
 * @see None.
 * @note None. The recommended value is between 200ms and 400ms.
 */
int platform_awss_get_channelscan_interval_ms(void);


///* link type */
//enum AWSS_LINK_TYPE {
//    /*< rtos platform choose this type */
//    AWSS_LINK_TYPE_NONE,

//    /*< linux platform may choose the following type */
//    AWSS_LINK_TYPE_PRISM,
//    AWSS_LINK_TYPE_80211_RADIO,
//    AWSS_LINK_TYPE_80211_RADIO_AVS,
//    AWSS_LINK_TYPE_HT40_CTRL /* for espressif platform, see struct ht40_ctrl */
//};

struct ht40_ctrl {
    uint16_t length;
    uint8_t filter;
    char rssi;
};

/**
 * @brief 80211 frame handler, passing 80211 frame to this func
 *
 * @param[in] buf @n 80211 frame buffer, or pointer to struct ht40_ctrl
 * @param[in] length @n 80211 frame buffer length
 * @param[in] link_type @n AWSS_LINK_TYPE_NONE for most rtos platform,
 *              and for linux platform, do the following step to check
 *              which header type the driver supported.
   @verbatim
            a) iwconfig wlan0 mode monitor	#open monitor mode
            b) iwconfig wlan0 channel 6	#switch channel 6
            c) tcpdump -i wlan0 -s0 -w file.pacp	#capture 80211 frame & save
            d) open file.pacp with wireshark or omnipeek
                check the link header type and fcs included or not
   @endverbatim
 * @param[in] with_fcs @n 80211 frame buffer include fcs(4 byte) or not
 */
//typedef int (*platform_awss_recv_80211_frame_cb_t)(char *buf, int length,
//        enum AWSS_LINK_TYPE link_type, int with_fcs);

/**
 * @brief Set wifi running at monitor mode,
   and register a callback function which will be called when wifi receive a frame.
 *
 * @param[in] cb @n A function pointer, called back when wifi receive a frame.
 * @return None.
 * @see None.
 * @note None.
 */
//void platform_awss_open_monitor(platform_awss_recv_80211_frame_cb_t cb);



/**
 * @brief Close wifi monitor mode, and set running at station mode.
 *
 * @param None.
 * @return None.
 * @see None.
 * @note None.
 */
void platform_awss_close_monitor(void);



/**
 * @brief Switch to specific wifi channel.
 *
 * @param[in] primary_channel @n Primary channel.
 * @param[in] secondary_channel @n Auxiliary channel if 40Mhz channel is supported, currently
 *              this param is always 0.
 * @param[in] bssid @n A pointer to wifi BSSID on which awss lock the channel, most platform
 *              may ignore it.
 * @return None.
 * @see None.
 * @note None.
 */
void platform_awss_switch_channel(
        _IN_ char primary_channel,
        _IN_OPT_ char secondary_channel,
        _IN_OPT_ uint8_t bssid[ETH_ALEN]);

///* auth type */
//enum AWSS_AUTH_TYPE {
//	AWSS_AUTH_TYPE_OPEN,
//	AWSS_AUTH_TYPE_SHARED,
//	AWSS_AUTH_TYPE_WPAPSK,
//	AWSS_AUTH_TYPE_WPA8021X,
//	AWSS_AUTH_TYPE_WPA2PSK,
//	AWSS_AUTH_TYPE_WPA28021X,
//	AWSS_AUTH_TYPE_WPAPSKWPA2PSK,
//	AWSS_AUTH_TYPE_MAX = AWSS_AUTH_TYPE_WPAPSKWPA2PSK,
//	AWSS_AUTH_TYPE_INVALID = 0xff,
//};

///* encry type */
//enum AWSS_ENC_TYPE {
//	AWSS_ENC_TYPE_NONE,
//	AWSS_ENC_TYPE_WEP,
//	AWSS_ENC_TYPE_TKIP,
//	AWSS_ENC_TYPE_AES,
//	AWSS_ENC_TYPE_TKIPAES,
//	AWSS_ENC_TYPE_MAX = AWSS_ENC_TYPE_TKIPAES,
//	AWSS_ENC_TYPE_INVALID = 0xff,
//};

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
//int platform_awss_connect_ap(
//        _IN_ uint32_t connection_timeout_ms,
//        _IN_ char ssid[PLATFORM_MAX_SSID_LEN],
//        _IN_ char passwd[PLATFORM_MAX_PASSWD_LEN],
//        _IN_OPT_ enum AWSS_AUTH_TYPE auth,
//        _IN_OPT_ enum AWSS_ENC_TYPE encry,
//        _IN_OPT_ uint8_t bssid[ETH_ALEN],
//        _IN_OPT_ uint8_t channel);

///* 80211 frame type */
//enum platform_awss_frame_type {
//    FRAME_ACTION,
//    FRAME_BEACON,
//    FRAME_PROBE_REQ,
//    FRAME_PROBE_RESPONSE,
//    FRAME_DATA
//};

#define FRAME_ACTION_MASK       (1 << FRAME_ACTION)
#define FRAME_BEACON_MASK       (1 << FRAME_BEACON)
#define FRAME_PROBE_REQ_MASK    (1 << FRAME_PROBE_REQ)
#define FRAME_PROBE_RESP_MASK   (1 << FRAME_PROBE_RESPONSE)
#define FRAME_DATA_MASK         (1 << FRAME_DATA)

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
//int platform_wifi_send_80211_raw_frame(_IN_ enum platform_awss_frame_type type,
//        _IN_ uint8_t *buffer, _IN_ int len);

/**
 * @brief management frame handler
 *
 * @param[in] buffer @n 80211 raw frame or ie(information element) buffer
 * @param[in] len @n buffer length
 * @param[in] rssi_dbm @n rssi in dbm, set it to 0 if not supported
 * @param[in] buffer_type @n 0 when buffer is a 80211 frame,
 *                          1 when buffer only contain IE info
 * @return None.
 * @see None.
 * @note None.
 */
typedef void (*platform_wifi_mgnt_frame_cb_t)(_IN_ uint8_t *buffer,
        _IN_ int len, _IN_ char rssi_dbm, _IN_ int buffer_type);

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
int platform_wifi_enable_mgnt_frame_filter(
            _IN_ uint32_t filter_mask,
            _IN_OPT_ uint8_t vendor_oui[3],
            _IN_ platform_wifi_mgnt_frame_cb_t callback);

/** @} */ //end of platform__awss

//typedef struct {
//    enum AWSS_AUTH_TYPE auth;
//    enum AWSS_ENC_TYPE encry;
//    uint8_t channel;
//    char rssi_dbm;
//    char ssid[PLATFORM_MAX_SSID_LEN];
//    uint8_t mac[ETH_ALEN];
//} ap_info_t;

/**
 * @brief handle one piece of AP information from wifi scan result
 *
 * @param[in] ssid @n name of AP
 * @param[in] bssid @n mac address of AP
 * @param[in] channel @n AP channel
 * @param[in] rssi @n rssi range[-100, 0].
 *          the higher the RSSI number, the stronger the signal.
 * @param[in] is_last_ap @n this AP information is the last one if is_last_ap > 0.
 *          this AP information is not the last one if is_last_ap == 0.
 * @return 0 for wifi scan is done, otherwise return -1
 * @see None.
 * @note None.
 */
//typedef int (*platform_wifi_scan_result_cb_t)(
//        const char ssid[PLATFORM_MAX_SSID_LEN],
//        const uint8_t bssid[ETH_ALEN],
//        enum AWSS_AUTH_TYPE auth,
//        enum AWSS_ENC_TYPE encry,
//        uint8_t channel, char rssi,
//        int is_last_ap);

/**
 * @brief launch a wifi scan operation
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
 *      	cb(ap)
 *      }
 *      ...
 *      platform_wifi_scan() exit...
 */
//int platform_wifi_scan(platform_wifi_scan_result_cb_t cb);

//typedef enum {
//  PLATFORM_AES_ENCRYPTION = 0,
//  PLATFORM_AES_DECRYPTION = 1,
//} AES_DIR_t;

typedef void *p_aes128_t;

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
//p_aes128_t platform_aes128_init(
//    _IN_ const uint8_t* key,
//    _IN_ const uint8_t* iv,
//    _IN_ AES_DIR_t dir);

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
int platform_aes128_destroy(_IN_ p_aes128_t aes);

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
int platform_aes128_cbc_encrypt(
    _IN_ p_aes128_t aes,
    _IN_ const void *src,
    _IN_ size_t blockNum,
    _OUT_ void *dst );

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
int platform_aes128_cbc_decrypt(
    _IN_ p_aes128_t aes,
    _IN_ const void *src,
    _IN_ size_t blockNum,
    _OUT_ void *dst );

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
int platform_wifi_get_ap_info(
    _OUT_ char ssid[PLATFORM_MAX_SSID_LEN],
    _OUT_ char passwd[PLATFORM_MAX_PASSWD_LEN],
    _OUT_ uint8_t bssid[ETH_ALEN]);


/** @} */ //end of platform__awss

/** @} */ //end of group_platform

#ifdef __cplusplus
}
#endif

#endif


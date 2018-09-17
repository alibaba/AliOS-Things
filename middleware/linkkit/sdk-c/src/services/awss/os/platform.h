/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
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

#define _IN_OPT_
#define _OUT_
#define _OUT_OPT_
#define _INOUT_
#define _INOUT_OPT_

#include "iot_import.h"

/** @defgroup group_platform platform
 *  @{
 */

#define PLATFORM_SOCKET_MAXNUMS         (10)
#define PLATFORM_WAIT_INFINITE          (~0)
#define PLATFORM_INVALID_FD             ((void *)-1)

#define STR_LONG_LEN                    (128)


/* ssid: 32 octets at most, include the NULL-terminated */
#define PLATFORM_MAX_SSID_LEN           (HAL_MAX_SSID_LEN)
/* password: 8-63 ascii */
#define PLATFORM_MAX_PASSWD_LEN         (HAL_MAX_PASSWD_LEN)

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
#define platform_thread_create HAL_ThreadCreate

/**
 * @brief exit the thread itself.
 *
 * @param[in] thread: itself thread handle.
 * @return None.
 * @see None.
 * @note None.
 */
#define platform_thread_exit HAL_ThreadDetach

/**
 * @brief sleep thread itself.
 *
 * @param[in] ms @n the time interval for which execution is to be suspended, in milliseconds.
 * @return None.
 * @see None.
 * @note None.
 */
#define platform_msleep HAL_SleepMs

/**
 * @brief system reboot immediately.
 *
 * @return None.
 * @see None.
 * @note None.
 */
#define platform_reboot HAL_Sys_reboot

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
#define platform_mutex_init HAL_MutexCreate

/**
 * @brief Destroy the specified mutex object, it will free related resource.
 *
 * @param[in] mutex @n The specified mutex.
 * @return None.
 * @see None.
 * @note None.
 */
#define platform_mutex_destroy HAL_MutexDestroy

/**
 * @brief Waits until the specified mutex is in the signaled state.
 *
 * @param[in] mutex @n the specified mutex.
 * @return None.
 * @see None.
 * @note None.
 */
#define platform_mutex_lock HAL_MutexLock

/**
 * @brief Releases ownership of the specified mutex object..
 *
 * @param[in] mutex @n the specified mutex.
 * @return None.
 * @see None.
 * @note None.
 */
#define platform_mutex_unlock HAL_MutexUnlock

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
#define platform_semaphore_init HAL_SemaphoreCreate

/**
 * @brief Destroy the specified semaphore object, it will free related resource.
 *
 * @param[in] sem @n the specified sem.
 * @return None.
 * @see None.
 * @note None.
 */
#define platform_semaphore_destroy HAL_SemaphoreDestroy

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
#define platform_semaphore_wait HAL_SemaphoreWait

/**
 * @brief Increases the count of the specified semaphore object by 1.
 *
 * @param[in] sem @n the specified semaphore.
 * @return None.
 * @see None.
 * @note None.
 */
#define platform_semaphore_post HAL_SemaphorePost

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
#define platform_malloc HAL_Malloc

/**
 * @brief Deallocate memory block
 *
 * @param[in] ptr @n Pointer to a memory block previously allocated with platform_malloc.
 * @return None.
 * @see None.
 * @note None.
 */
#define platform_free HAL_Free

/** @} */ //end of platform_memory_manage

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
#define platform_sys_net_is_ready HAL_Sys_Net_Is_Ready

/**
 * @brief Retrieves the number of milliseconds that have elapsed since the system was boot.
 *
 * @param None.
 * @return the number of milliseconds.
 * @see None.
 * @note None.
 */
#define platform_get_time_ms (uint32_t)HAL_UptimeMs

/** @} */ //end of platform_system


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
#define platform_printf(fmt, args...) HAL_Printf(fmt, ##args)


/** @} */ //end of group_io

/******************************** wifi module interface ********************************/

/** @defgroup group_platform_wifi_module wifi related
 *  @{
 */


#define PLATFORM_MAC_LEN	(HAL_MAC_LEN)
/**
 * @brief Get WIFI MAC string, format should be XX:XX:XX:XX:XX:XX
 *
 * @param[out] mac_str @n Buffer for using to store wifi MAC string.
 * @return A pointer to the start address of mac_str.
 * @see None.
 * @note None.
 */
#define platform_wifi_get_mac(mac) HAL_Wifi_Get_Mac(mac);


#define PLATFORM_IP_LEN    (HAL_IP_LEN)
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
#define platform_wifi_get_ip HAL_Wifi_Get_IP


/** @} */ //end of platform_wifi_module


/************************* awss(alibaba wireless setup service) interface ***************************/

/** @defgroup group_platform_awss alibaba wireless setup service(awss)
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
#define platform_awss_get_timeout_interval_ms HAL_Awss_Get_Timeout_Interval_Ms

/**
 * @brief Get time length, in millisecond, of per channel scan.
 *
 * @param None.
 * @return The timeout interval.
 * @see None.
 * @note None. The recommended value is between 200ms and 400ms.
 */
#define platform_awss_get_channelscan_interval_ms HAL_Awss_Get_Channelscan_Interval_Ms

struct ht40_ctrl {
    uint16_t length;
    uint8_t filter;
    signed char rssi;
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
            a) iwconfig wlan0 mode monitor  #open monitor mode
            b) iwconfig wlan0 channel 6 #switch channel 6
            c) tcpdump -i wlan0 -s0 -w file.pacp    #capture 80211 frame & save
            d) open file.pacp with wireshark or omnipeek
                check the link header type and fcs included or not
   @endverbatim
 * @param[in] with_fcs @n 80211 frame buffer include fcs(4 byte) or not
 */
typedef awss_recv_80211_frame_cb_t platform_awss_recv_80211_frame_cb_t;

/**
 * @brief Set wifi running at monitor mode,
   and register a callback function which will be called when wifi receive a frame.
 *
 * @param[in] cb @n A function pointer, called back when wifi receive a frame.
 * @return None.
 * @see None.
 * @note None.
 */
#define platform_awss_open_monitor(cb) HAL_Awss_Open_Monitor(cb)

/**
 * @brief Close wifi monitor mode, and set running at station mode.
 *
 * @param None.
 * @return None.
 * @see None.
 * @note None.
 */
#define platform_awss_close_monitor HAL_Awss_Close_Monitor

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
#define platform_awss_switch_channel HAL_Awss_Switch_Channel

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
#define platform_awss_connect_ap HAL_Awss_Connect_Ap

/* 80211 frame type */
#define platform_awss_frame_type HAL_Awss_Frame_Type

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
#define platform_wifi_send_80211_raw_frame HAL_Wifi_Send_80211_Raw_Frame

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
                                              _IN_ int len, _IN_ signed char rssi_dbm, _IN_ int buffer_type);
typedef awss_wifi_mgmt_frame_cb_t platform_wifi_mgnt_frame_cb_t;

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
#define platform_wifi_enable_mgnt_frame_filter HAL_Wifi_Enable_Mgmt_Frame_Filter

/** @} */ //end of platform__awss

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
typedef awss_wifi_scan_result_cb_t platform_wifi_scan_result_cb_t;

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
 *          cb(ap)
 *      }
 *      ...
 *      platform_wifi_scan() exit...
 */
#define platform_wifi_scan HAL_Wifi_Scan

#define p_aes128_t p_HAL_Aes128_t
#define PLATFORM_AES_ENCRYPTION HAL_AES_ENCRYPTION
#define PLATFORM_AES_DECRYPTION HAL_AES_DECRYPTION

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
#define platform_aes128_init HAL_Aes128_Init

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
#define platform_aes128_destroy HAL_Aes128_Destroy

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
#define platform_aes128_cbc_encrypt HAL_Aes128_Cbc_Encrypt

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
#define platform_aes128_cbc_decrypt HAL_Aes128_Cbc_Decrypt

/**
 * @brief encrypt data with aes (cfb/128bit key).
 *
 * @param[in] aes: AES handler
 * @param[in] src: plain data
 * @param[in] len: plain data length of bytes
 * @param[out] dst: cipher data
 * @return
   @verbatim
     = 0: succeeded
     = -1: failed
   @endverbatim
 * @see None.
 * @note None.
 */
#define platform_aes128_cfb_encrypt HAL_Aes128_Cfb_Encrypt

/**
 * @brief decrypt data with aes (cfb/128bit key).
 *
 * @param[in] aes: AES handler
 * @param[in] src: cipher data
 * @param[in] len: plain data length of bytes
 * @param[out] dst: plain data
 * @return
   @verbatim
     = 0: succeeded
     = -1: failed
   @endverbatim
 * @see None.
 * @note None.
 */
#define platform_aes128_cfb_decrypt HAL_Aes128_Cfb_Decrypt

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
#define platform_wifi_get_ap_info HAL_Wifi_Get_Ap_Info

/**
 * @brief Get encrypt type for smartconfig.
 *
 * @param None.
 * @return.
   @verbatim
     = 0: open (no encrypt)
     = 1: aes256cbc with default aes-key and aes-iv
     = 2: aes128cbc with default aes-key and aes-iv
     = 3: aes128cbc with aes-key per product and aes-iv = 0
     = 4: aes128cbc with aes-key per device and aes-iv = 0
     = 5: aes128cbc with aes-key per manufacture and aes-iv = 0
     = others: invalid
   @endverbatim
 * @see None.
 * @note None.
 */
#define platform_get_encrypt_type HAL_Awss_Get_Encrypt_Type

/**
 * @brief    Get Security level for wifi configuration with connection.
 *           Used for AP solution of router and App.
 *
 * @param None.
 * @return The security level:
   @verbatim
    3: aes128cfb with aes-key per product and aes-iv = random
    4: aes128cfb with aes-key per device and aes-iv = random
    5: aes128cfb with aes-key per manufacture and aes-iv = random
    others: invalid
   @endverbatim
 * @see None.
 */
#define platform_get_conn_encrypt_type HAL_Awss_Get_Conn_Encrypt_Type

/** @} */ //end of platform__awss

/** @} */ //end of group_platform

#ifdef __cplusplus
}
#endif

#endif


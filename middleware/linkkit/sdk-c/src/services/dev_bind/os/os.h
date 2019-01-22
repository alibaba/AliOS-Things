/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __AWSS_OS_H__
#define __AWSS_OS_H__

#ifdef __cplusplus
extern "C" {
#endif
#include <ctype.h>
#include "iot_import.h"
#include "iot_export.h"
#include "product/product.h"
#include "platform.h"

/** @defgroup group_os os
 *  @{
 */

#define OS_MAX_SSID_LEN         PLATFORM_MAX_SSID_LEN
#define OS_MAX_PASSWD_LEN       PLATFORM_MAX_PASSWD_LEN

#define OS_MAC_LEN              PLATFORM_MAC_LEN
#define OS_IP_LEN               PLATFORM_IP_LEN
#define OS_ETH_ALEN             ETH_ALEN

#define OS_PRODUCT_KEY_LEN      PRODUCT_KEY_LEN
#define OS_PRODUCT_SECRET_LEN   PRODUCT_SECRET_LEN
#define OS_DEVICE_NAME_LEN      DEVICE_NAME_LEN
#define OS_DEVICE_SECRET_LEN    DEVICE_SECRET_LEN

#if 0
#ifndef in_range
#define in_range(c, lo, up)     ((uint8_t)(c) >= (lo) && (uint8_t)(c) <= (up))
#define isdigit(c)              in_range(c, '0', '9')
#define isxdigit(c)             (isdigit(c) || in_range(c, 'a', 'f') || in_range(c, 'A', 'F'))
#define islower(c)              in_range(c, 'a', 'z')
#define isspace(c)              ((c) == ' ' || (c) == '\f' || (c) == '\n' || (c) == '\r' || (c) == '\t' || (c) == '\v')
#endif
#endif
/***************************************** Misc Interface *****************************************/

/** @defgroup group_misc misc
 *  @{
 */
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
int os_is_big_endian(void);

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
/* uint64_t os_htobe64(uint64_t data); */

/**
 * @brief Convert the data from host to little-endian.
   The size of the data is 64 bits.
 *
 * @param[in] data @n Data to be converted.
 * @return Conversion result.
 * @see None.
 * @note None.
 */
/* uint64_t os_htole64(uint64_t data); */

/**
 * @brief Convert the data from big-endian to host.
   The size of the data is 64 bits.
 *
 * @param[in] data @n Data to be converted.
 * @return Conversion result.
 * @see None.
 * @note None.
 */
/* uint64_t os_be64toh(uint64_t data); */

/**
 * @brief Convert the data from little-endian to host.
   The size of the data is 64 bits.
 *
 * @param[in] data @n Data to be converted.
 * @return Conversion result.
 * @see None.
 * @note None.
 */
/* uint64_t os_le64toh(uint64_t data); */

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

/**
 * @brief sleep thread itself.
 *
 * @param[in] ms @n the time interval for which execution is to be suspended, in milliseconds.
 * @return None.
 * @see None.
 * @note None.
 */
void awss_msleep(_IN_ uint32_t ms);

/**
 * @brief sleep thread itself.
 *
 * @param[in] sec @n the time interval for which execution is to be suspended, in seconds.
 * @return None.
 * @see None.
 * @note None.
 */
void awss_sleep(_IN_ uint32_t sec);

/**
 * @brief system reboot immediately.
 *
 * @return None.
 * @see None.
 * @note None.
 */
void awss_reboot();

/** @} */

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
void *os_mutex_init(void);

/**
 * @brief Destroy the specified mutex object, it will free related resource.
 *
 * @param[in] mutex @n The specified mutex.
 * @return None.
 * @see None.
 * @note None.
 */
void os_mutex_destroy(_IN_ void *mutex);

/**
 * @brief Waits until the specified mutex is in the signaled state.
 *
 * @param[in] mutex @n the specified mutex.
 * @return None.
 * @see None.
 * @note None.
 */
void os_mutex_lock(_IN_ void *mutex);

/**
 * @brief Releases ownership of the specified mutex object..
 *
 * @param[in] mutex @n the specified mutex.
 * @return None.
 * @see None.
 * @note None.
 */
void os_mutex_unlock(_IN_ void *mutex);

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
void *os_semaphore_init(void);

/**
 * @brief Destroy the specified semaphore object, it will free related resource.
 *
 * @param[in] sem @n the specified sem.
 * @return None.
 * @see None.
 * @note None.
 */
void os_semaphore_destroy(_IN_ void *sem);

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
int os_semaphore_wait(_IN_ void *sem, _IN_ uint32_t timeout_ms);

/**
 * @brief Increases the count of the specified semaphore object by 1.
 *
 * @param[in] sem @n the specified semaphore.
 * @return None.
 * @see None.
 * @note None.
 */
void os_semaphore_post(_IN_ void *sem);

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
void *awss_malloc(_IN_ uint32_t size);

/**
 * @brief Deallocate memory block
 *
 * @param[in] ptr @n Pointer to a memory block previously allocated with platform_malloc.
 * @return None.
 * @see None.
 * @note None.
 */
void awss_free(_IN_ void *ptr);

/**
 * @brief Allocates a memory block with memory initialized to 0,
     returning a pointer to the beginning of the block.
 *
 * @param[in] size @n specify block size in bytes.
 * @return A pointer to the beginning of the block.
 * @see None.
 * @note Block value is indeterminate.
 */
void *awss_zalloc(uint32_t size);

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
int os_sys_net_is_ready(void);

/**
 * @brief Retrieves the number of milliseconds that have elapsed since the system was boot.
 *
 * @param None.
 * @return the number of milliseconds.
 * @see None.
 * @note Add a big offset, for easier caught time overflow bug.
 */
uint32_t os_get_time_ms(void);

/*
 *  These inlines deal with timer wrapping correctly. You are
 *  strongly encouraged to use them
 *  1. Because people otherwise forget
 *  2. Because if the timer wrap changes in future you won't have to
 *     alter your code.
 */
uint32_t time_elapsed_ms_since(uint32_t start_timestamp);

/** @} */

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
#define os_printf(fmt, ...) platform_printf(fmt, ## __VA_ARGS__)

/*************************************** wifi module Interface ***************************************/

/** @defgroup group_os_wifi_module wifi
 *  @{
 */

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
int os_get_encrypt_type(void);

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
int os_get_conn_encrypt_type(void);

/**
 * @brief Get WIFI MAC string with format like: XX:XX:XX:XX:XX:XX.
 *
 * @param[out] mac_str @n Buffer for using to store wifi MAC string.
 * @return A pointer to the start address of mac_str.
 * @see None.
 * @note this func will format mac address string uppercase
 */
char *os_wifi_get_mac_str(char mac_str[OS_MAC_LEN]);
char *os_wifi_str2mac(char mac_str[OS_MAC_LEN], char mac[OS_ETH_ALEN]);

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
 * @param[in] ifname @n Specify the interface name.The NULL value indicates the default network interface.
 * @param[out] ip_str @n Buffer for using to store IP string, in numbers-and-dots notation form.
 * @return IP address with binary form, in network byte order.
 * @see None.
 * @note None.
 */
uint32_t os_wifi_get_ip(char ip_str[OS_IP_LEN], const char *ifname);

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
int os_awss_get_timeout_interval_ms(void);

/**
 * @brief Get time length, in millisecond, of per channel scan.
 *
 * @param None.
 * @return The timeout interval.
 * @see None.
 * @note None. The recommended value is between 200ms and 400ms.
 */
int os_awss_get_channelscan_interval_ms(void);

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
void os_awss_open_monitor(os_awss_recv_80211_frame_cb_t cb);

/**
 * @brief Close wifi monitor mode, and set running at station mode.
 *
 * @param None.
 * @return None.
 * @see None.
 * @note None.
 */
void os_awss_close_monitor(void);

/**
  * @brief   开启设备热点（SoftAP模式）
  *
  * @param[in] ssid @n 热点的ssid字符；
  * @param[in] passwd @n 热点的passwd字符；
  * @param[in] beacon_interval @n 热点的Beacon广播周期（广播间隔）；
  * @param[in] hide @n 是否是隐藏热点，hide:0, 非隐藏, 其它值：隐藏；
  * @return，
     @verbatim
        = 0: success
        = -1: unsupported
        = -2: failure with system error
        = -3: failure with no memory
        = -4: failure with invalid parameters
     @endverbatim
  * @Note:
  *       1）ssid和passwd都是以'\0'结尾的字符串，如果passwd字符串的
  *          长度为0，表示该热点采用Open模式（不加密）；
  *       2）beacon_interval表示热点的Beacon广播间隔（或周期），单
  *          位为毫秒，一般会采用默认100ms；
  *       3）hide表示创建的热点是否是隐藏热点，hide=0表示非隐藏热
  *         点，其他值表示隐藏热点；
  */

int os_awss_open_ap(const char *ssid, const char *passwd, int beacon_interval, int hide);

/**
  * @brief   关闭当前设备热点，并把设备由SoftAP模式切换到Station模式
  *
  * @return，
     @verbatim
        = 0: success
        = -1: unsupported
        = -2: failure
     @endverbatim
  * @Note:
  *       1）如果当前设备已经开启热点，关闭当前热点，如果当前设备正
  *          在开热点，取消开热点的操作；
  *       2）如果当前设备不是以Station模式（包括Station+SoftAP模式和
  *          SoftAP模式）工作，设备必须切换到Station模式；
  *       3）Wi-Fi状态机需要切换到初始化状态，因为接下来很可能进行
  *          连接某一个路由器操作；
  */

int os_awss_close_ap();

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
void os_awss_switch_channel(char primary_channel, char secondary_channel, uint8_t bssid[OS_ETH_ALEN]);

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
int os_awss_connect_ap(
            _IN_ uint32_t connection_timeout_ms,
            _IN_ char ssid[OS_MAX_SSID_LEN],
            _IN_ char passwd[OS_MAX_PASSWD_LEN],
            _IN_OPT_ enum AWSS_AUTH_TYPE auth,
            _IN_OPT_ enum AWSS_ENC_TYPE encry,
            _IN_OPT_ uint8_t bssid[ETH_ALEN],
            _IN_OPT_ uint8_t channel);

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
int os_wifi_send_80211_raw_frame(_IN_ enum platform_awss_frame_type type,
        _IN_ uint8_t *buffer, _IN_ int len);

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
int os_wifi_enable_mgnt_frame_filter(
            _IN_ uint32_t filter_mask,
            _IN_OPT_ uint8_t vendor_oui[3],
            _IN_ platform_wifi_mgnt_frame_cb_t callback);

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
int os_wifi_scan(_IN_ platform_wifi_scan_result_cb_t cb);

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
p_aes128_t os_aes128_init(_IN_ const uint8_t *key,
            _IN_ const uint8_t *iv, _IN_ AES_DIR_t dir);

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
int os_aes128_destroy(_IN_ p_aes128_t aes);

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
int os_aes128_cbc_encrypt(
            _IN_ p_aes128_t aes,
            _IN_ const void *src,
            _IN_ size_t blockNum,
            _OUT_ void *dst);

/**
 * @brief encrypt data with aes (cfb/128bit key).
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
int os_aes128_cfb_encrypt(
            _IN_ p_aes128_t aes,
            _IN_ const void *src,
            _IN_ size_t blockNum,
            _OUT_ void *dst);

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
int os_aes128_cbc_decrypt(
            _IN_ p_aes128_t aes,
            _IN_ const void *src,
            _IN_ size_t blockNum,
            _OUT_ void *dst);

/**
 * @brief decrypt data with aes (cfb/128bit key).
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
int os_aes128_cfb_decrypt(
            _IN_ p_aes128_t aes,
            _IN_ const void *src,
            _IN_ size_t blockNum,
            _OUT_ void *dst);

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
int os_wifi_get_ap_info(
            _OUT_ char ssid[PLATFORM_MAX_SSID_LEN],
            _OUT_ char passwd[PLATFORM_MAX_PASSWD_LEN],
            _OUT_ uint8_t bssid[ETH_ALEN]);

/** @defgroup group_os_product product
 *  @{
 */

/**
 * @brief Get product key string.
 *
 * @param[out] key_str @n Buffer for using to store key string.
 * @return A pointer to the start address of key_str.
 * @see None.
 * @note None.
 */
int os_product_get_key(char key_str[OS_PRODUCT_KEY_LEN]);

/**
 * @brief Get product secret string.
 *
 * @param[out] secret_str @n Buffer for using to store secret string.
 * @return A pointer to the start address of secret_str.
 * @see None.
 * @note None.
 */
int os_product_get_secret(char secret_str[OS_PRODUCT_SECRET_LEN]);

/**
 * @brief Get device secret string.
 *
 * @param[out] secret_str @n Buffer for using to store secret string.
 * @return A pointer to the start address of secret_str.
 * @see None.
 * @note None.
 */
int os_device_get_secret(char secret_str[OS_DEVICE_SECRET_LEN]);

/**
 * @brief Get device key string.
 *
 * @param[out] key_str @n Buffer for using to store debug key string.
 * @return A pointer to the start address of key_str.
 * @see None.
 * @note None.
 */
int os_device_get_name(char key_str[OS_DEVICE_NAME_LEN]);


#ifdef __cplusplus
}
#endif

#endif /* SRC_OSA_ABSTRAC_H_ */

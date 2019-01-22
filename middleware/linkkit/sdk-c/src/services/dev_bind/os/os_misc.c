/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include "os.h"

/****** Convert values between host and big-/little-endian byte order ******/

/* reverse byte order */
uint16_t reverse_16bit(uint16_t data)
{
    return (data >> 8) | (data << 8);
}

/* host byte order to big endian */
uint16_t os_htobe16(uint16_t data)
{
    if (os_is_big_endian()) {
        return data;
    }

    return reverse_16bit(data);
}

/* host byte order to little endian */
uint16_t os_htole16(uint16_t data)
{
    if (os_is_big_endian()) {
        return reverse_16bit(data);
    }

    return data;
}

/* big endian to host byte order */
uint16_t os_be16toh(uint16_t data)
{
    return os_htobe16(data);
}

/* little endian to host byte order */
uint16_t os_le16toh(uint16_t data)
{
    return os_htole16(data);
}

/* get unaligned data in big endian. */
uint16_t os_get_unaligned_be16(uint8_t * ptr)
{
    uint16_t res;

    memcpy(&res, ptr, sizeof(uint16_t));

    return os_be16toh(res);
}

/* get unaligned data in little endian. */
uint16_t os_get_unaligned_le16(uint8_t * ptr)
{
    uint16_t res;

    memcpy(&res, ptr, sizeof(uint16_t));

    return os_le16toh(res);
}

#if 0
/* reverse byte order */
uint32_t reverse_32bit(uint32_t data)
{
    data = (data >> 16) | (data << 16);
    return ((data & 0xff00ff00UL) >> 8) | ((data & 0x00ff00ffUL) << 8);
}

/* host byte order to big endian */
uint32_t os_htobe32(uint32_t data)
{
    if (os_is_big_endian()) {
        return data;
    }

    return reverse_32bit(data);
}

/* host byte order to little endian */
uint32_t os_htole32(uint32_t data)
{
    if (os_is_big_endian()) {
        return reverse_32bit(data);
    }

    return data;
}

/* big endian to host byte order */
uint32_t os_be32toh(uint32_t data)
{
    return os_htobe32(data);
}

/* little endian to host byte order */
uint32_t os_le32toh(uint32_t data)
{
    return os_htole32(data);
}

/* reverse byte order */
uint64_t reverse_64bit(uint64_t data)
{
    data = (data >> 32) | (data << 32);
    data = ((data & 0xff00ff00ff00ff00ULL) >> 8) | ((data & 0x00ff00ff00ff00ffULL) << 8);

    return ((data & 0xffff0000ffff0000ULL) >> 16) | ((data & 0x0000ffff0000ffffULL) << 16);
}

/* host to big endian */
uint64_t os_htobe64(uint64_t data)
{
    if (os_is_big_endian()) {
        return data;
    }

    return reverse_64bit(data);
}

/* host to little endian */
uint64_t os_htole64(uint64_t data)
{
    if (os_is_big_endian()) {
        return reverse_64bit(data);
    }

    return data;
}

/* big endian to host */
uint64_t os_be64toh(uint64_t data)
{
    return os_htobe64(data);
}

/* little endian to host */
uint64_t os_le64toh(uint64_t data)
{
    return os_htole64(data);
}

uint32_t os_get_unaligned_be32(uint8_t * ptr)
{
    uint32_t res;

    memcpy(&res, ptr, sizeof(uint32_t));

    return os_be32toh(res);
}

/* get unaligned data in little endian. */
uint32_t os_get_unaligned_le32(uint8_t *ptr)
{
    uint32_t res;

    memcpy(&res, ptr, sizeof(uint32_t));

    return os_le32toh(res);

}

uint16_t os_htons(uint16_t n)
{
    return os_htobe16(n);
}

uint16_t os_ntohs(uint16_t n)
{
    return os_be16toh(n);
}

uint32_t os_htonl(uint32_t n)
{
    return os_htobe32(n);
}

uint32_t os_ntohl(uint32_t n)
{
    return os_be32toh(n);
}

char *os_ntoa(const uint32_t ip, char buf[OS_IP_LEN])
{
    char inv[3];
    char *rp;
    uint8_t *ap;
    uint8_t rem;
    uint8_t n;
    uint8_t i;

    rp = buf;
    ap = (uint8_t *) & ip;

    for (n = 0; n < 4; n++) {
        i = 0;
        do {
            rem = *ap % (uint8_t) 10;
            *ap /= (uint8_t) 10;
            inv[i++] = '0' + rem;
        } while (*ap);

        while (i--) {
            *rp++ = inv[i];
        }

        *rp++ = '.';
        ap++;
    }

    *--rp = 0;
    return buf;
}
#endif
/* format mac string uppercase */
char *os_wifi_get_mac_str(char mac_str[OS_MAC_LEN])
{
    char *str;
    int colon_num = 0, i;

    str = platform_wifi_get_mac(mac_str);

    /* sanity check */
    while (str) {
        str = strchr(str, ':');
        if (str) {
            colon_num ++;
            str ++; /* eating char ':' */
        }
    }

    /* convert to capital letter */
    for (i = 0; i < OS_MAC_LEN && mac_str[i]; i ++) {
        if ('a' <= mac_str[i] && mac_str[i] <= 'z') {
            mac_str[i] -= 'a' - 'A';
        }
    }

    return mac_str;
}

char *os_wifi_str2mac(char mac_str[OS_MAC_LEN], char mac[OS_ETH_ALEN])
{
    int i = 0;
    char *ptr = mac_str;
    char mac_addr[OS_ETH_ALEN] = {0};

    if (ptr == NULL)
        return NULL;

    while (isxdigit(*ptr) && i < OS_ETH_ALEN) {
        mac_addr[i ++] = (uint8_t)strtol(ptr, &ptr, 16);
        ++ ptr;
    }

    if (i < OS_ETH_ALEN)  /* don't touch mac when fail */
        return NULL;

    if (mac) memcpy(mac, mac_addr, OS_ETH_ALEN);

    return mac;
}

uint8_t *os_wifi_get_mac(uint8_t mac[OS_ETH_ALEN])
{
    char mac_str[OS_MAC_LEN] = {0};

    os_wifi_get_mac_str(mac_str);

    return (uint8_t *)os_wifi_str2mac(mac_str, (char *)mac);
}

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
int os_is_big_endian(void)
{
    uint32_t data = 0xFF000000;

    if (0xFF == *(uint8_t *) & data) {
        return 1;  /* big endian */
    }

    return 0;      /* little endian */
}

/**
 * @brief sleep thread itself.
 *
 * @param[in] ms @n the time interval for which execution is to be suspended, in milliseconds.
 * @return None.
 * @see None.
 * @note None.
 */
void awss_msleep(_IN_ uint32_t ms)
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
void awss_sleep(_IN_ uint32_t sec)
{
    platform_msleep(sec * 1000);
}

/**
 * @brief system reboot immediately.
 *
 * @return None.
 * @see None.
 * @note None.
 */
void awss_reboot()
{
    platform_reboot();
}

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
void *os_mutex_init(void)
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
void os_mutex_destroy(_IN_ void *mutex)
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
void os_mutex_lock(_IN_ void *mutex)
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
void os_mutex_unlock(_IN_ void *mutex)
{
    platform_mutex_unlock(mutex);
}

/** @} */

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
void *os_semaphore_init(void)
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
void os_semaphore_destroy(_IN_ void *sem)
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
int os_semaphore_wait(_IN_ void *sem, _IN_ uint32_t timeout_ms)
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
void os_semaphore_post(_IN_ void *sem)
{
    platform_semaphore_post(sem);
}

/** @} */

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
void *awss_malloc(_IN_ uint32_t size)
{
    return platform_malloc(size);
}


/**
 * @brief Deallocate memory block
 *
 * @param[in] ptr @n Pointer to a memory block previously allocated with platform_malloc.
 * @return None.
 * @see None.
 * @note None.
 */
void awss_free(_IN_ void *ptr)
{
    platform_free(ptr);
}

/**
 * @brief Allocates a memory block with memory initialized to 0,
     returning a pointer to the beginning of the block.
 *
 * @param[in] size @n specify block size in bytes.
 * @return A pointer to the beginning of the block.
 * @see None.
 * @note Block value is indeterminate.
 */
void *awss_zalloc(uint32_t size)
{
    void *ptr = awss_malloc(size);
    if (ptr != NULL) {
        memset(ptr, 0, size);
    }
    return ptr;
}

/** @} */

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
int os_sys_net_is_ready(void)
{
    return platform_sys_net_is_ready();
}

/**
 * @brief Retrieves the number of milliseconds that have elapsed since the system was boot.
 *
 * @param None.
 * @return the number of milliseconds.
 * @see None.
 * @note Add a big offset, for easier caught time overflow bug.
 */
uint32_t os_get_time_ms(void)
{
    static uint32_t fixed_delta = 0;

    if (!fixed_delta) {
        fixed_delta = platform_get_time_ms() - 0xFFFF0000;
    }

    /* add a big offset, for easier caught time overflow bug */
    return platform_get_time_ms() - fixed_delta;
}

/*
 *  These inlines deal with timer wrapping correctly. You are
 *  strongly encouraged to use them
 *  1. Because people otherwise forget
 *  2. Because if the timer wrap changes in future you won't have to
 *     alter your code.
 */
uint32_t time_elapsed_ms_since(uint32_t start_timestamp)
{
    uint32_t now = os_get_time_ms();

    return now - start_timestamp;
}

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

/** @} */

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
int os_get_encrypt_type(void)
{
    return platform_get_encrypt_type();
}

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
int os_get_conn_encrypt_type(void)
{
    return platform_get_conn_encrypt_type();
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
uint32_t os_wifi_get_ip(char ip_str[OS_IP_LEN], const char *ifname)
{
    return platform_wifi_get_ip(ip_str, ifname);
}

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
int os_awss_get_timeout_interval_ms(void)
{
    return platform_awss_get_timeout_interval_ms();
}

/**
 * @brief Get time length, in millisecond, of per channel scan.
 *
 * @param None.
 * @return The timeout interval.
 * @see None.
 * @note None. The recommended value is between 200ms and 400ms.
 */
int os_awss_get_channelscan_interval_ms(void)
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
void os_awss_open_monitor(os_awss_recv_80211_frame_cb_t cb)
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
void os_awss_close_monitor(void)
{
    platform_awss_close_monitor();
}

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

int os_awss_open_ap(const char *ssid, const char *passwd, int beacon_interval, int hide)
{
    return platform_awss_open_ap(ssid, passwd, beacon_interval, hide);
}

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

int os_awss_close_ap()
{
    return platform_awss_close_ap();
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
void os_awss_switch_channel(char primary_channel, char secondary_channel, uint8_t bssid[OS_ETH_ALEN])
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
int os_awss_connect_ap(
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
int os_wifi_send_80211_raw_frame(_IN_ enum platform_awss_frame_type type,
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
int os_wifi_enable_mgnt_frame_filter(
            _IN_ uint32_t filter_mask,
            _IN_OPT_ uint8_t vendor_oui[3],
            _IN_ platform_wifi_mgnt_frame_cb_t callback)
{
    return platform_wifi_enable_mgnt_frame_filter(filter_mask, vendor_oui, callback);
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
int os_wifi_scan(_IN_ platform_wifi_scan_result_cb_t cb)
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
p_aes128_t os_aes128_init(
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
int os_aes128_destroy(
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
int os_aes128_cbc_encrypt(
            _IN_ p_aes128_t aes,
            _IN_ const void *src,
            _IN_ size_t blockNum,
            _OUT_ void *dst)
{
    return platform_aes128_cbc_encrypt(aes, src, blockNum, dst);
}

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
            _OUT_ void *dst)
{
    return platform_aes128_cfb_encrypt(aes, src, blockNum, dst);
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
int os_aes128_cbc_decrypt(
            _IN_ p_aes128_t aes,
            _IN_ const void *src,
            _IN_ size_t blockNum,
            _OUT_ void *dst)
{
    return platform_aes128_cbc_decrypt(aes, src, blockNum, dst);
}

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
            _OUT_ void *dst)
{
    return platform_aes128_cfb_decrypt(aes, src, blockNum, dst);
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
int os_wifi_get_ap_info(
            _OUT_ char ssid[PLATFORM_MAX_SSID_LEN],
            _OUT_ char passwd[PLATFORM_MAX_PASSWD_LEN],
            _OUT_ uint8_t bssid[ETH_ALEN])
{
    return platform_wifi_get_ap_info(ssid, passwd, bssid);
}

/** @} */

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
int os_product_get_key(char key_str[OS_PRODUCT_KEY_LEN])
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
int os_product_get_secret(char secret_str[OS_PRODUCT_SECRET_LEN])
{
    return product_get_secret(secret_str);
}

/**
 * @brief Get device secret string.
 *
 * @param[out] secret_str @n Buffer for using to store secret string.
 * @return A pointer to the start address of secret_str.
 * @see None.
 * @note None.
 */
int os_device_get_secret(char secret_str[OS_DEVICE_SECRET_LEN])
{
    return device_get_secret(secret_str);
}

/**
 * @brief Get device key string.
 *
 * @param[out] key_str @n Buffer for using to store debug key string.
 * @return A pointer to the start address of key_str.
 * @see None.
 * @note None.
 */
int os_device_get_name(char key_str[OS_DEVICE_NAME_LEN])
{
    return device_get_name(key_str);
}

/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <memory.h>

#include "iot_import.h"
#include "iot_export.h"

#include <process.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <memory.h>
#include "iotx_hal_internal.h"

#define __DEMO__

#ifdef __DEMO__
    char _product_key[PRODUCT_KEY_LEN + 1];
    char _product_secret[PRODUCT_SECRET_LEN + 1];
    char _device_name[DEVICE_NAME_LEN + 1];
    char _device_secret[DEVICE_SECRET_LEN + 1];
#endif


void *HAL_MutexCreate(void)
{
    HANDLE mutex;

    if (NULL == (mutex = CreateMutex(NULL, FALSE, NULL))) {
        hal_err("create mutex error");
    }

    return mutex;
}

void HAL_MutexDestroy(_IN_ void *mutex)
{
    if (0 == CloseHandle(mutex)) {
        hal_err("destroy mutex error");
    }
}

void HAL_MutexLock(_IN_ void *mutex)
{
    if (WAIT_FAILED == WaitForSingleObject(mutex, INFINITE)) {
        hal_err("lock mutex error");
    }
}

void HAL_MutexUnlock(_IN_ void *mutex)
{
    ReleaseMutex(mutex);
}

void *HAL_Malloc(_IN_ uint32_t size)
{
    return malloc(size);
}

void *HAL_Realloc(_IN_ void *ptr, _IN_ uint32_t size)
{
    return realloc(ptr, size);
}

void *HAL_Calloc(_IN_ uint32_t nmemb, _IN_ uint32_t size)
{
    return calloc(nmemb, size);
}

void HAL_Free(_IN_ void *ptr)
{
    free(ptr);
}

uint64_t HAL_UptimeMs(void)
{
    return (uint64_t)(GetTickCount());
}

#if 0
char *HAL_GetTimeStr(_IN_ char *buf, _IN_ int len)
{
    time_t t;
    struct tm      *local;
    int str_len    = 0;

    if (buf == NULL && len >= 28) {
        return NULL;
    }
    t = time(NULL);
    local = localtime(&t);
    strftime(buf, 28, "%m-%d %H:%M:%S", local);
    return buf;
}
#endif  /* #if 0 */

void HAL_SleepMs(_IN_ uint32_t ms)
{
    Sleep(ms);
}

uint32_t orig_seed = 2;
void HAL_Srandom(uint32_t seed)
{
    orig_seed = seed;
}

uint32_t HAL_Random(uint32_t region)
{
    orig_seed = 1664525 * orig_seed + 1013904223;
    return (region > 0) ? (orig_seed % region) : 0;
}

void HAL_Printf(_IN_ const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);

    fflush(stdout);
}


int HAL_Snprintf(_IN_ char *str, const int len, const char *fmt, ...)
{
    int ret;
    va_list args;

    va_start(args, fmt);
    ret = _vsnprintf(str, len - 1, fmt, args);
    va_end(args);

    return ret;
}


int HAL_Vsnprintf(_IN_ char *str, _IN_ const int len, _IN_ const char *fmt, va_list ap)
{
    int ret;

    ret = _vsnprintf(str, len - 1, fmt, ap);

    return ret;
}

int HAL_GetPartnerID(char *pid_str)
{
    memset(pid_str, 0x0, PID_STRLEN_MAX);
#ifdef __DEMO__
    strcpy(pid_str, "example.demo.partner-id");
#endif
    return strlen(pid_str);
}

int HAL_GetModuleID(char *mid_str)
{
    memset(mid_str, 0x0, MID_STRLEN_MAX);
#ifdef __DEMO__
    strcpy(mid_str, "example.demo.module-id");
#endif
    return strlen(mid_str);
}

char *HAL_GetChipID(_OU_ char *cid_str)
{
    memset(cid_str, 0x0, HAL_CID_LEN);
#ifdef __DEMO__
    strncpy(cid_str, "rtl8188eu 12345678", HAL_CID_LEN);
    cid_str[HAL_CID_LEN - 1] = '\0';
#endif
    return cid_str;
}

int HAL_GetDeviceID(_OU_ char *device_id)
{
    memset(device_id, 0x0, DEVICE_ID_LEN);
#ifdef __DEMO__
    HAL_Snprintf(device_id, DEVICE_ID_LEN, "%s.%s", _product_key, _device_name);
    device_id[DEVICE_ID_LEN - 1] = '\0';
#endif

    return strlen(device_id);
}

int HAL_SetProductKey(_IN_ char *product_key)
{
    int len = strlen(product_key);
#ifdef __DEMO__
    if (len > PRODUCT_KEY_LEN) {
        return -1;
    }
    memset(_product_key, 0x0, PRODUCT_KEY_LEN + 1);
    strncpy(_product_key, product_key, len);
#endif
    return len;
}


int HAL_SetDeviceName(_IN_ char *device_name)
{
    int len = strlen(device_name);
#ifdef __DEMO__
    if (len > DEVICE_NAME_LEN) {
        return -1;
    }
    memset(_device_name, 0x0, DEVICE_NAME_LEN + 1);
    strncpy(_device_name, device_name, len);
#endif
    return len;
}


int HAL_SetDeviceSecret(_IN_ char *device_secret)
{
    int len = strlen(device_secret);
#ifdef __DEMO__
    if (len > DEVICE_SECRET_LEN) {
        return -1;
    }
    memset(_device_secret, 0x0, DEVICE_SECRET_LEN + 1);
    strncpy(_device_secret, device_secret, len);
#endif
    return len;
}


int HAL_SetProductSecret(_IN_ char *product_secret)
{
    int len = strlen(product_secret);
#ifdef __DEMO__
    if (len > PRODUCT_SECRET_LEN) {
        return -1;
    }
    memset(_product_secret, 0x0, PRODUCT_SECRET_LEN + 1);
    strncpy(_product_secret, product_secret, len);
#endif
    return len;
}

int HAL_GetProductKey(_OU_ char *product_key)
{
    int len = strlen(_product_key);
    memset(product_key, 0x0, PRODUCT_KEY_LEN);

#ifdef __DEMO__
    strncpy(product_key, _product_key, len);
#endif

    return len;
}

int HAL_GetProductSecret(_OU_ char *product_secret)
{
    int len = strlen(_product_secret);
    memset(product_secret, 0x0, PRODUCT_SECRET_LEN);

#ifdef __DEMO__
    strncpy(product_secret, _product_secret, len);
#endif

    return len;
}

int HAL_GetDeviceName(_OU_ char *device_name)
{
    int len = strlen(_device_name);
    memset(device_name, 0x0, DEVICE_NAME_LEN);

#ifdef __DEMO__
    strncpy(device_name, _device_name, len);
#endif

    return strlen(device_name);
}

int HAL_GetDeviceSecret(_OU_ char *device_secret)
{
    int len = strlen(_device_secret);
    memset(device_secret, 0x0, DEVICE_SECRET_LEN);

#ifdef __DEMO__
    strncpy(device_secret, _device_secret, len);
#endif

    return len;
}

int HAL_GetFirmwareVesion(_OU_ char *version)
{
    memset(version, 0x0, FIRMWARE_VERSION_MAXLEN);
#ifdef __DEMO__
    strncpy(version, "1.0", FIRMWARE_VERSION_MAXLEN);
    version[FIRMWARE_VERSION_MAXLEN - 1] = '\0';
#endif

    return strlen(version);
}

#if 1
void *HAL_SemaphoreCreate(void)
{
#if 0
    sem_t *sem = (sem_t *)malloc(sizeof(sem_t));
    if (NULL == sem) {
        return NULL;
    }

    if (0 != sem_init(sem, 0, 0)) {
        free(sem);
        return NULL;
    }

    return sem;
#endif
    return NULL;
}

void HAL_SemaphoreDestroy(_IN_ void *sem)
{
#if 0
    sem_destroy((sem_t *)sem);
    free(sem);
#endif
}

void HAL_SemaphorePost(_IN_ void *sem)
{
#if 0
    sem_post((sem_t *)sem);
#endif
}

int HAL_SemaphoreWait(_IN_ void *sem, _IN_ uint32_t timeout_ms)
{
#if 0
    if (PLATFORM_WAIT_INFINITE == timeout_ms) {
        sem_wait(sem);
        return 0;
    } else {
        struct timespec ts;
        int s;
        /* Restart if interrupted by handler */
        do {
            if (clock_gettime(CLOCK_REALTIME, &ts) == -1) {
                return -1;
            }

            s = 0;
            ts.tv_nsec += (timeout_ms % 1000) * 1000000;
            if (ts.tv_nsec >= 1000000000) {
                ts.tv_nsec -= 1000000000;
                s = 1;
            }

            ts.tv_sec += timeout_ms / 1000 + s;

        } while (((s = sem_timedwait(sem, &ts)) != 0) && errno == EINTR);

        return (s == 0) ? 0 : -1;
    }
#endif
    return 0;
}

int HAL_ThreadCreate(
            _OU_ void **thread_handle,
            _IN_ void *(*work_routine)(void *),
            _IN_ void *arg,
            _IN_ hal_os_thread_param_t *hal_os_thread_param,
            _OU_ int *stack_used)
{
#if 0
    int ret = -1;
    if (stack_used) {
        *stack_used = 0;
    }

    ret = pthread_create((pthread_t *)thread_handle, NULL, work_routine, arg);

    return ret;
#endif
    return 0;
}

void HAL_ThreadDetach(_IN_ void *thread_handle)
{
#if 0
    pthread_detach((pthread_t)thread_handle);
#endif
}

void HAL_ThreadDelete(_IN_ void *thread_handle)
{
#if 0
    if (NULL == thread_handle) {
        pthread_exit(0);
    } else {
        /*main thread delete child thread*/
        pthread_cancel((pthread_t)thread_handle);
    }
#endif
}
#endif  /* #if 0 */

#ifdef __DEMO__
    static FILE *fp;

    #define otafilename "/tmp/alinkota.bin"
#endif

void HAL_Firmware_Persistence_Start(void)
{
#ifdef __DEMO__
    fp = fopen(otafilename, "w");
    //    assert(fp);
#endif
    return;
}

int HAL_Firmware_Persistence_Write(_IN_ char *buffer, _IN_ uint32_t length)
{
#ifdef __DEMO__
    unsigned int written_len = 0;
    written_len = fwrite(buffer, 1, length, fp);

    if (written_len != length) {
        return -1;
    }
#endif
    return 0;
}

int HAL_Firmware_Persistence_Stop(void)
{
#ifdef __DEMO__
    if (fp != NULL) {
        fclose(fp);
    }
#endif

    /* check file md5, and burning it to flash ... finally reboot system */
    return 0;
}

int HAL_Kv_Get(const char *key, void *buffer, int *buffer_len)
{
    return 0;
}

int HAL_Kv_Set(const char *key, const void *val, int len, int sync)
{
    return 0;
}


void HAL_UTC_Set(int64_t ms)
{

}

int64_t HAL_UTC_Get(void)
{
    return 0;
}

int HAL_GetNetifInfo(char *nif_str)
{
    memset(nif_str, 0x0, NIF_STRLEN_MAX);
#ifdef __DEMO__
    /* if the device have only WIFI, then list as follow, note that the len MUST NOT exceed NIF_STRLEN_MAX */
    const char *net_info = "WiFi|03ACDEFF0032";
    strncpy(nif_str, net_info, strlen(net_info));
    /* if the device have ETH, WIFI, GSM connections, then list all of them as follow, note that the len MUST NOT exceed NIF_STRLEN_MAX */
    // const char *multi_net_info = "ETH|0123456789abcde|WiFi|03ACDEFF0032|Cellular|imei_0123456789abcde|iccid_0123456789abcdef01234|imsi_0123456789abcde|msisdn_86123456789ab");
    // strncpy(nif_str, multi_net_info, strlen(multi_net_info));
#endif
    return strlen(nif_str);
}

uint32_t HAL_Wifi_Get_IP(_OU_ char ip_str[NETWORK_ADDR_LEN], _IN_ const char *ifname)
{
	return 0;
}

void   *HAL_Timer_Create(const char *name, void(*func)(void *), void *user_data)
{
	return 0;
}

int     HAL_Timer_Start(void *t, int ms)
{
	return 0;
}


int     HAL_Timer_Stop(void *t)
{
	return 0;
}

int     HAL_Timer_Delete(void *timer)
{
	return 0;
}

intptr_t HAL_UDP_create_without_connect(_IN_ const char *host, _IN_ unsigned short port)
{
	return 0;
}

int HAL_UDP_sendto(_IN_ intptr_t sockfd,
	_IN_ const NetworkAddr *p_remote,
	_IN_ const unsigned char *p_data,
	_IN_ unsigned int datalen,
	_IN_ unsigned int timeout_ms)
{
	return 0;
}

int HAL_UDP_recvfrom(_IN_ intptr_t sockfd,
	_OU_ NetworkAddr *p_remote,
	_OU_ unsigned char *p_data,
	_IN_ unsigned int datalen,
	_IN_ unsigned int timeout_ms)
{
	return 0;
}

int HAL_UDP_joinmulticast(_IN_ intptr_t sockfd,
	_IN_ char *p_group)
{
	return 0;
}

int HAL_UDP_close_without_connect(_IN_ intptr_t sockfd)
{
	return 0;
}

int HAL_Awss_Connect_Ap(
	_IN_ uint32_t connection_timeout_ms,
	_IN_ char ssid[HAL_MAX_SSID_LEN],
	_IN_ char passwd[HAL_MAX_PASSWD_LEN],
	_IN_OPT_ enum AWSS_AUTH_TYPE auth,
	_IN_OPT_ enum AWSS_ENC_TYPE encry,
	_IN_OPT_ uint8_t bssid[ETH_ALEN],
	_IN_OPT_ uint8_t channel)
{
	return 0;
}

int HAL_Wifi_Scan(awss_wifi_scan_result_cb_t cb)
{
	return 0;
}

int HAL_Wifi_Get_Ap_Info(
	_OU_ char ssid[HAL_MAX_SSID_LEN],
	_OU_ char passwd[HAL_MAX_PASSWD_LEN],
	_OU_ uint8_t bssid[ETH_ALEN])
{
	return 0;
}

int HAL_Awss_Get_Conn_Encrypt_Type(void)
{
	return 0;
}


int HAL_Wifi_Send_80211_Raw_Frame(_IN_ enum HAL_Awss_Frame_Type type,
	_IN_ uint8_t *buffer, _IN_ int len)
{
	return 0;
}

int HAL_Awss_Get_Encrypt_Type(void)
{
	return 0;
}

int HAL_Awss_Get_Timeout_Interval_Ms(void)
{
	return 0;
}

int HAL_Awss_Get_Channelscan_Interval_Ms(void)
{
	return 0;
}

void HAL_Awss_Open_Monitor(_IN_ awss_recv_80211_frame_cb_t cb)
{
	return;
}

void HAL_Awss_Switch_Channel(
	_IN_ char primary_channel,
	_IN_OPT_ char secondary_channel,
	_IN_OPT_ uint8_t bssid[ETH_ALEN])
{
	return;
}

void HAL_Awss_Close_Monitor(void)
{
	return;
}

int HAL_Sys_Net_Is_Ready()
{
	return 0;
}

p_HAL_Aes128_t HAL_Aes128_Init(
	_IN_ const uint8_t *key,
	_IN_ const uint8_t *iv,
	_IN_ AES_DIR_t dir)
{
	return 0;
}

int HAL_Aes128_Destroy(_IN_ p_HAL_Aes128_t aes)
{
	return 0;
}

int HAL_Aes128_Cbc_Decrypt(
	_IN_ p_HAL_Aes128_t aes,
	_IN_ const void *src,
	_IN_ size_t blockNum,
	_OU_ void *dst)
{
	return 0;
}


int HAL_Aes128_Cfb_Decrypt(
	_IN_ p_HAL_Aes128_t aes,
	_IN_ const void *src,
	_IN_ size_t length,
	_OU_ void *dst)

{
	return 0;
}

void    HAL_Reboot(void)
{
	return;
}

char *HAL_Wifi_Get_Mac(_OU_ char mac_str[HAL_MAC_LEN])
{
	return 0;
}

int     HAL_Kv_Del(const char *key)
{
	return 0;
}


int HAL_Aes128_Cbc_Encrypt(
	_IN_ p_HAL_Aes128_t aes,
	_IN_ const void *src,
	_IN_ size_t blockNum,
	_OU_ void *dst)
{
	return 0;
}

void *HAL_Fopen(const char *path, const char *mode)
{
	return;
}

size_t HAL_Fread(void *buff, size_t size, size_t count, void *stream)
{
	return 0;
}

int HAL_Fseek(void *stream, long offset, int origin)
{
	return 0;
}

int HAL_Fclose(FILE *stream)
{
	return 0;
}

long HAL_Ftell(void *stream)
{
	return 0;
}

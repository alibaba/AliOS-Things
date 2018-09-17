/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */





#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <memory.h>
#include <pthread.h>
#include <unistd.h>
//#include <sys/prctl.h>
#include <sys/time.h>
#include <semaphore.h>
#include <errno.h>
#include <assert.h>
#include <net/if.h>       // struct ifreq
//inet_ntoa
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>

#include "iot_import.h"
#include "iotx_hal_internal.h"

#if defined(SUPPORT_SINGAPORE_DOMAIN)
static char DEMO_CASE_PRODUCT_KEY[PRODUCT_KEY_MAXLEN] = {"zSqW3X4e0lJ"};
static char DEMO_CASE_DEVICE_NAME[DEVICE_NAME_MAXLEN] = {"azhan_sg"};
static char DEMO_CASE_DEVICE_SECRET[DEVICE_SECRET_MAXLEN] = {"qUMFGaNQGjoTdDkxKppfVzQwtpnMxRBx"};
#elif defined(ON_DAILY)
static char DEMO_CASE_PRODUCT_KEY[PRODUCT_KEY_MAXLEN] = {"a1DA163qVT1"};
static char DEMO_CASE_DEVICE_NAME[DEVICE_NAME_MAXLEN] = {"zigbee_gateway"};
static char DEMO_CASE_DEVICE_SECRET[DEVICE_SECRET_MAXLEN] = {"oZvm6H9ydbXmVWhNl80AQjVXYJuT3s6a"};
#elif defined(ON_PRE)
static char DEMO_CASE_PRODUCT_KEY[PRODUCT_KEY_MAXLEN] = {"a1u36e7PPs1"};
static char DEMO_CASE_DEVICE_NAME[DEVICE_NAME_MAXLEN] = {"MxFRySXtCYbcR9vsYDyd"};
static char DEMO_CASE_DEVICE_SECRET[DEVICE_SECRET_MAXLEN] = {"5U0GSuYZHsfINMB3n9bxUT2Yo1fJyo9S"};
#else
static char DEMO_CASE_PRODUCT_KEY[PRODUCT_KEY_MAXLEN] = {"a1jteuBmM5J"};
static char DEMO_CASE_DEVICE_NAME[DEVICE_NAME_MAXLEN] = {"gateway_demo_parent"};
static char DEMO_CASE_DEVICE_SECRET[DEVICE_SECRET_MAXLEN] = {"TJRahoxHu9bJG0JWzjAtQ1MkquamTkzw"};
#endif
static char DEMO_CASE_PRODUCT_SECRET[PRODUCT_SECRET_MAXLEN] = {0};

void *HAL_MutexCreate(void)
{
    int err_num;
    pthread_mutex_t *mutex = (pthread_mutex_t *)HAL_Malloc(sizeof(pthread_mutex_t));
    if (NULL == mutex) {
        return NULL;
    }

    if (0 != (err_num = pthread_mutex_init(mutex, NULL))) {
        hal_err("create mutex failed");
        HAL_Free(mutex);
        return NULL;
    }

    return mutex;
}

void HAL_MutexDestroy(_IN_ void *mutex)
{
    int err_num;
    if (0 != (err_num = pthread_mutex_destroy((pthread_mutex_t *)mutex))) {
        hal_err("destroy mutex failed");
    }

    HAL_Free(mutex);
}

void HAL_MutexLock(_IN_ void *mutex)
{
    int err_num;
    if (0 != (err_num = pthread_mutex_lock((pthread_mutex_t *)mutex))) {
        hal_err("lock mutex failed");
    }
}

void HAL_MutexUnlock(_IN_ void *mutex)
{
    int err_num;
    if (0 != (err_num = pthread_mutex_unlock((pthread_mutex_t *)mutex))) {
        hal_err("unlock mutex failed");
    }
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

void HAL_Reboot(void)
{
    reboot(0);
}

#if defined(__MACH__) && !defined(CLOCK_REALTIME)

#include <sys/time.h>
#define CLOCK_REALTIME 0
#define CLOCK_REALTIME 0
#define CLOCK_MONOTONIC 0

// clock_gettime is not implemented on older versions of OS X (< 10.12).
// If implemented, CLOCK_REALTIME will have already been defined.

int clock_gettime(int clk_id, struct timespec *t)
{
    struct timeval now;
    int rv = gettimeofday(&now, NULL);
    if (rv) {
        return rv;
    }
    t->tv_sec  = now.tv_sec;
    t->tv_nsec = now.tv_usec * 1000;
    return 0;
}
#endif

uint64_t HAL_UptimeMs(void)
{
    uint64_t            time_ms;
    struct timespec     ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);
    time_ms = (ts.tv_sec * 1000) + (ts.tv_nsec / 1000 / 1000);

    return time_ms;
}

char *HAL_GetTimeStr(_IN_ char *buf, _IN_ int len)
{
    struct timeval tv;
    struct tm      tm;
    int str_len    = 0;

    if (buf == NULL && len >= 28) {
        return NULL;
    }
    gettimeofday(&tv, NULL);
    localtime_r(&tv.tv_sec, &tm);
    strftime(buf, 28, "%m-%d %H:%M:%S", &tm);
    str_len = strlen(buf);
    if (str_len + 3 < len) {
        snprintf(buf + str_len, len, ".%3.3d", (int)(tv.tv_usec) / 1000);
    }
    return buf;
}

void HAL_SleepMs(_IN_ uint32_t ms)
{
    usleep(1000 * ms);
}

void HAL_Srandom(uint32_t seed)
{
    srandom(seed);
}

uint32_t HAL_Random(uint32_t region)
{
    return (region > 0) ? (random() % region) : 0;
}

int HAL_Snprintf(_IN_ char *str, const int len, const char *fmt, ...)
{
    va_list args;
    int     rc;

    va_start(args, fmt);
    rc = vsnprintf(str, len, fmt, args);
    va_end(args);

    return rc;
}

int HAL_Vsnprintf(_IN_ char *str, _IN_ const int len, _IN_ const char *format, va_list ap)
{
    return vsnprintf(str, len, format, ap);
}

void HAL_Printf(_IN_ const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);

    fflush(stdout);
}

int HAL_GetPartnerID(char pid_str[PID_STR_MAXLEN])
{
    memset(pid_str, 0x0, PID_STR_MAXLEN);
    strcpy(pid_str, "example.demo.partner-id");
    return strlen(pid_str);
}

int HAL_GetModuleID(char mid_str[MID_STR_MAXLEN])
{
    memset(mid_str, 0x0, MID_STR_MAXLEN);
    strcpy(mid_str, "example.demo.module-id");
    return strlen(mid_str);
}

char *HAL_GetChipID(_OU_ char cid_str[HAL_CID_LEN])
{
    strncpy(cid_str, "rtl8188eu 12345678", HAL_CID_LEN);
    cid_str[HAL_CID_LEN - 1] = '\0';
    return cid_str;
}

int HAL_GetDeviceID(_OU_ char device_id[DEVICE_ID_MAXLEN])
{
    HAL_Snprintf(device_id, DEVICE_ID_MAXLEN, "%s.%s", DEMO_CASE_PRODUCT_KEY, DEMO_CASE_DEVICE_NAME);
    return strlen(device_id);
}

int HAL_GetDeviceName(_OU_ char device_name[DEVICE_NAME_MAXLEN])
{
    HAL_Snprintf(device_name, DEVICE_NAME_MAXLEN, "%s", DEMO_CASE_DEVICE_NAME);
    return strlen(device_name);
}

int HAL_SetDeviceName(_IN_ char device_name[DEVICE_NAME_MAXLEN])
{
    HAL_Snprintf(DEMO_CASE_DEVICE_NAME, DEVICE_NAME_MAXLEN, "%s", device_name);
    return strlen(DEMO_CASE_DEVICE_NAME);
}

int HAL_GetDeviceSecret(_OU_ char device_secret[DEVICE_SECRET_MAXLEN])
{
    HAL_Snprintf(device_secret, DEVICE_SECRET_MAXLEN, "%s", DEMO_CASE_DEVICE_SECRET);
    return strlen(device_secret);
}

int HAL_SetDeviceSecret(_IN_ char device_secret[DEVICE_SECRET_MAXLEN])
{
    HAL_Snprintf(DEMO_CASE_DEVICE_SECRET, DEVICE_SECRET_MAXLEN, "%s", device_secret);
    return strlen(DEMO_CASE_DEVICE_SECRET);
}

int HAL_GetFirmwareVesion(_OU_ char version[FIRMWARE_VERSION_MAXLEN])
{
    memset(version, 0x0, FIRMWARE_VERSION_MAXLEN);
    strncpy(version, "1.0", FIRMWARE_VERSION_MAXLEN);
    version[FIRMWARE_VERSION_MAXLEN - 1] = '\0';
    return strlen(version);
}

int HAL_GetProductKey(_OU_ char product_key[PRODUCT_KEY_MAXLEN])
{
    HAL_Snprintf(product_key, PRODUCT_KEY_MAXLEN, "%s", DEMO_CASE_PRODUCT_KEY);
    return strlen(product_key);
}

int HAL_SetProductKey(_IN_ char product_key[PRODUCT_KEY_MAXLEN])
{
    strncpy(DEMO_CASE_PRODUCT_KEY, product_key, PRODUCT_KEY_MAXLEN - 1);
    DEMO_CASE_PRODUCT_KEY[PRODUCT_KEY_MAXLEN - 1]  = '\0';
    return strlen(DEMO_CASE_PRODUCT_KEY);
}

int HAL_GetProductSecret(_OU_ char product_secret[PRODUCT_SECRET_MAXLEN])
{
    memset(product_secret, 0, PRODUCT_SECRET_MAXLEN);
    HAL_Snprintf(product_secret, PRODUCT_SECRET_MAXLEN, "%s", DEMO_CASE_PRODUCT_SECRET);
    return strlen(product_secret);
}

int HAL_SetProductSecret(_IN_ char product_secret[PRODUCT_SECRET_MAXLEN])
{
    strncpy(DEMO_CASE_PRODUCT_SECRET, product_secret, PRODUCT_SECRET_MAXLEN - 1);
    DEMO_CASE_PRODUCT_SECRET[PRODUCT_SECRET_MAXLEN - 1] = '\0';
    return strlen(DEMO_CASE_PRODUCT_SECRET);
}


typedef struct {
    int count;
    pthread_cond_t cond;
    pthread_mutex_t lock;
} os_sem_t;

void *HAL_SemaphoreCreate(void)
{
    os_sem_t *sem = malloc(sizeof(os_sem_t));
    if (!sem) {
        return NULL;
    }
    memset(sem, 0, sizeof(os_sem_t));

    sem->count = 0;
    pthread_cond_init(&sem->cond, NULL);
    pthread_mutex_init(&sem->lock, NULL);

    return sem;
}

void HAL_SemaphoreDestroy(_IN_ void *_sem)
{
    os_sem_t *sem = _sem;
    if (sem) {
        pthread_cond_destroy(&sem->cond);
        pthread_mutex_destroy(&sem->lock);
        free(sem);
    }
}

void HAL_SemaphorePost(_IN_ void *_sem)
{
    os_sem_t *sem = _sem;
    if (!sem) {
        return;
    }
    pthread_mutex_lock(&sem->lock);
    sem->count++;
    pthread_cond_signal(&sem->cond);
    pthread_mutex_unlock(&sem->lock);
}

int HAL_SemaphoreWait(_IN_ void *_sem, _IN_ uint32_t timeout_ms)
{
    os_sem_t *sem = _sem;
    if (!sem) {
        return -1;
    }

    struct timeval tv;
    if (gettimeofday(&tv, NULL) < 0) {
        return -1;
    }

    struct timespec ts;
    ts.tv_sec  = tv.tv_sec + timeout_ms / 1000;
    ts.tv_nsec = tv.tv_usec * 1000 + timeout_ms % 1000;

    pthread_mutex_lock(&sem->lock);
    while (sem->count <= 0) {
        int ret = pthread_cond_timedwait(&sem->cond, &sem->lock, &ts);
        if (ret != 0) {
            if (ret != EAGAIN) {
                pthread_mutex_unlock(&sem->lock);
                return -1;
            }
        } else if (ret == 0) {
            break;
        }
    }
    sem->count--;
    pthread_mutex_unlock(&sem->lock);

    return 0;
}

int HAL_ThreadCreate(
            _OU_ void **thread_handle,
            _IN_ void *(*work_routine)(void *),
            _IN_ void *arg,
            _IN_ hal_os_thread_param_t *hal_os_thread_param,
            _OU_ int *stack_used)
{
    int ret = -1;
    if (stack_used) {
        *stack_used = 0;
    }

    ret = pthread_create((pthread_t *)thread_handle, NULL, work_routine, arg);

    return ret;
}

void HAL_ThreadDetach(_IN_ void *thread_handle)
{
    pthread_detach((pthread_t)thread_handle);
}

void HAL_ThreadDelete(_IN_ void *thread_handle)
{
    if (NULL == thread_handle) {
        pthread_exit(0);
    } else {
        /*main thread delete child thread*/
        pthread_cancel((pthread_t)thread_handle);
    }
}

static FILE *fp;

#define otafilename "/tmp/alinkota.bin"
void HAL_Firmware_Persistence_Start(void)
{
    fp = fopen(otafilename, "w");
    assert(fp);
    return;
}

int HAL_Firmware_Persistence_Write(_IN_ char *buffer, _IN_ uint32_t length)
{
    unsigned int written_len = 0;
    written_len = fwrite(buffer, 1, length, fp);

    if (written_len != length) {
        return -1;
    }
    return 0;
}

int HAL_Firmware_Persistence_Stop(void)
{
    if (fp != NULL) {
        fclose(fp);
    }

    /* check file md5, and burning it to flash ... finally reboot system */

    return 0;
}

int HAL_Config_Write(const char *buffer, int length)
{
    FILE *fp;
    size_t written_len;
    char filepath[128] = {0};

    if (!buffer || length <= 0) {
        return -1;
    }

    snprintf(filepath, sizeof(filepath), "./%s", "alinkconf");
    fp = fopen(filepath, "w");
    if (!fp) {
        return -1;
    }

    written_len = fwrite(buffer, 1, length, fp);

    fclose(fp);

    return ((written_len != length) ? -1 : 0);
}

int HAL_Config_Read(char *buffer, int length)
{
    FILE *fp;
    size_t read_len;
    char filepath[128] = {0};

    if (!buffer || length <= 0) {
        return -1;
    }

    snprintf(filepath, sizeof(filepath), "./%s", "alinkconf");
    fp = fopen(filepath, "r");
    if (!fp) {
        return -1;
    }

    read_len = fread(buffer, 1, length, fp);
    fclose(fp);

    return ((read_len != length) ? -1 : 0);
}

#define REBOOT_CMD "reboot"
void HAL_Sys_reboot(void)
{
    if (system(REBOOT_CMD)) {
        perror("HAL_Sys_reboot failed");
    }
}

#define ROUTER_INFO_PATH        "/proc/net/route"
#define ROUTER_RECORD_SIZE      256

char *_get_default_routing_ifname(char *ifname, int ifname_size)
{
    FILE *fp = NULL;
    char line[ROUTER_RECORD_SIZE] = {0};
    char iface[IFNAMSIZ] = {0};
    char *result = NULL;
    unsigned int destination, gateway, flags, mask;
    unsigned int refCnt, use, metric, mtu, window, irtt;

    fp = fopen(ROUTER_INFO_PATH, "r");
    if (fp == NULL) {
        perror("fopen");
        return result;
    }

    char *buff = fgets(line, sizeof(line), fp);
    if (buff == NULL) {
        perror("fgets");
        goto out;
    }

    while (fgets(line, sizeof(line), fp)) {
        if (11 !=
            sscanf(line, "%s %08x %08x %x %d %d %d %08x %d %d %d",
                   iface, &destination, &gateway, &flags, &refCnt, &use,
                   &metric, &mask, &mtu, &window, &irtt)) {
            perror("sscanf");
            continue;
        }

        /*default route */
        if ((destination == 0) && (mask == 0)) {
            strncpy(ifname, iface, ifname_size - 1);
            result = ifname;
            break;
        }
    }

out:
    if (fp) {
        fclose(fp);
    }

    return result;
}

char *HAL_Wifi_Get_Mac(char mac_str[HAL_MAC_LEN])
{
#if 0
    struct ifreq ifr;
    int sock = -1;

    memset(mac_str, 0, HAL_MAC_LEN);

    char ifname_buff[IFNAMSIZ] = {0};
    char *ifname = _get_default_routing_ifname(ifname_buff, sizeof(ifname_buff));
    if (!ifname) {
        perror("get default routeing ifname");
        goto fail;
    }

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("socket");
        goto fail;
    }

    ifr.ifr_addr.sa_family = AF_INET; //ipv4 address
    strncpy(ifr.ifr_name, ifname, IFNAMSIZ - 1);

    if (ioctl(sock, SIOCGIFHWADDR, &ifr) < 0) {
        close(sock);
        perror("ioctl");
        goto fail;
    }

    close(sock);

    char *ptr = mac_str;
    char *end = mac_str + HAL_MAC_LEN;

    int i;
    for (i = 0; i < 6; i++) {
        if (i == 5) {
            ptr += snprintf(ptr, end - ptr, "%02x", (uint8_t)ifr.ifr_hwaddr.sa_data[i]);
        } else {
            ptr += snprintf(ptr, end - ptr, "%02x:", (uint8_t)ifr.ifr_hwaddr.sa_data[i]);
        }
    }

    return mac_str;

fail:
    strncpy(mac_str, "de:ad:be:ef:00:00", HAL_MAC_LEN - 1);
#endif
    return mac_str;
}

uint32_t HAL_Wifi_Get_IP(char ip_str[NETWORK_ADDR_LEN], const char *ifname)
{
    struct ifreq ifreq;
    int sock = -1;
    char ifname_buff[IFNAMSIZ] = {0};

    if ((NULL == ifname || strlen(ifname) == 0) &&
        NULL == (ifname = _get_default_routing_ifname(ifname_buff, sizeof(ifname_buff)))) {
        perror("get default routeing ifname");
        return -1;
    }

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        return -1;
    }

    ifreq.ifr_addr.sa_family = AF_INET; //ipv4 address
    strncpy(ifreq.ifr_name, ifname, IFNAMSIZ - 1);

    if (ioctl(sock, SIOCGIFADDR, &ifreq) < 0) {
        close(sock);
        perror("ioctl");
        return -1;
    }

    close(sock);

    strncpy(ip_str,
            inet_ntoa(((struct sockaddr_in *)&ifreq.ifr_addr)->sin_addr),
            NETWORK_ADDR_LEN);

    return ((struct sockaddr_in *)&ifreq.ifr_addr)->sin_addr.s_addr;
}

int HAL_Kv_Set(const char *key, const void *val, int len, int sync)
{
    return 0;
}

int HAL_Kv_Get(const char *key, void *buffer, int *buffer_len)
{
    return 0;
}

int HAL_Kv_Del(const char *key)
{
    return 0;
}

void HAL_UTC_Set(long long ms)
{

}

long long HAL_UTC_Get(void)
{
    return 0;
}


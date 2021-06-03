/**
 * ant_system.h
 *
 * 系统相关接口
 */
#ifndef __ANT_SYSTEM_H__
#define __ANT_SYSTEM_H__
#include <stdarg.h>
#include "ant_typedef.h"
#include "ant_event.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    ANT_POWER_ON_KEYPAD = 0,
    ANT_POWER_ON_SOFT_RESTART,
    ANT_POWER_ON_CHARGER_IN,
    ANT_POWER_ON_EXCEPTION,
} ant_poweron_mode_e;

// === STDIO STRING STDLIB ===
void *ant_memset(void *s, int c, size_t n);

void *ant_memcpy(void *dst, const void *src, size_t n);

void *ant_memmove(void *dst, const void *src, size_t n);

int   ant_memcmp(const void *s1, const void *s2, size_t n);

void *ant_memmem(const void *s, size_t len, const void *sub, size_t sub_len);

size_t ant_strlen(const char *s);

char *ant_strcpy(char *dst, const char *src);

char *ant_strncpy(char *dst, const char *src, int n);

char *ant_strcat(char *dst, const char *src);

char *ant_strncat(char *dst, const char *src, int n);

char *ant_strchr(const char *s, int c);

char *ant_strrchr(const char *s, int c);

char *ant_strstr(const char *str, const char *sub);

char *ant_strtok(char *str, const char *delim);

int   ant_split(char *src, const char *sep, char **dest, int num);

int   ant_tolower(int c);

char *ant_strdup(const char *s);

int   ant_stricmp(const char *s, const char *t);

int   ant_strnicmp(const char *s, const char *t, int n);

int   ant_strcmp(const char *s, const char *t);

int   ant_strncmp(const char *s, const char *t, int n);

int   ant_snprintf(char *str, size_t size, const char *format, ...);
int   ant_vsnprintf(char *str, size_t size, const char *format, va_list args);

int   ant_sscanf(const char *str, const char *format, ...);
int   ant_vsscanf(const char * buffer, const char * format, va_list ap);

unsigned long int ant_strtol(const char *nptr, char **endptr, int base);
unsigned long long ant_strtoll(const char *nptr, char **endptr, int base);
unsigned long int ant_strtoul(const char *nptr, char **endptr, int base);
unsigned long long ant_strtoull(const char *nptr, char **endptr, int base);
int ant_atoi(const char *nptr);
long ant_atol(const char *nptr);
long long ant_atoll(const char *nptr);
double ant_strtod(const char *nptr, char **endptr);
float ant_strtof(const char *nptr, char **endptr);
void ant_hextostr(char *str, const ant_u8 *hex, int n);
// === STDIO STRING STDLIB end ===

ant_u16 ant_ntohs(ant_u16 netshort);
ant_u32 ant_ntohl(ant_u32 netlong);
ant_u16 ant_htons(ant_u32 hostshort);
ant_u32 ant_htonl(ant_u32 hostlong);
char   *ant_inet_ntoa(ant_u32 addr);

/**
 * 产生系统随机数
 *
 * @param p_rng 随机数发生句柄，可填NULL
 * @param output 随机数缓存
 * @param output_len 随机数长度
 * @return
 */
int ant_random(void *p_rng, unsigned char *output, size_t output_len);

/**
 * 获取系统编译日期时间
 *
 * @return 返回系统编译日期时间，格式：“yyyy/MM/dd MM:hh:ss"
 */
char * ant_build_date_time(void);

ant_s32 ant_system_init(void);


/**
 * 获取设备的序列号
 *
 * @param sn 存储序列号的缓存
 * @param size 存储序列号缓存的大小
 *
 * @return  0： 获取成功
 *         -1： 获取失败
 */
ant_s32 ant_system_get_sn(char *sn, ant_u32 size);

/**
 * 获取设备的IMEI号
 *
 * @param imei 存储IMEI号的缓存
 * @param size 存储IMEI号缓存的大小
 *
 * @return  0： 获取成功
 *         -1： 获取失败
 */
ant_s32 ant_system_get_imei(ant_char *imei, ant_u32 size);

/**
 * 获取设备当前使用的流量卡的ICCID号
 *
 * @param imei 存储ICCID号的缓存
 * @param size 存储ICCID号缓存的大小
 *
 * @return  0： 获取成功
 *         -1： 获取失败
 */
ant_s32 ant_system_get_iccid(char *iccid, ant_u32 size);

/**
 * 获取设备的MAC地址
 *
 * @param mac 存储MAC地址的缓存
 * @param size 存储MAC地址缓存的大小
 *
 * @return  0： 获取成功
 *         -1： 获取失败
 */
ant_s32 ant_system_get_mac(char *mac, ant_u32 size);

/**
 * 系统重启
 *
 * @return  0： 系统重启成功
 *         -1： 系统重启失败
 */
ant_s32 ant_system_restart(void);

/**
 * 系统关机
 */
void    ant_system_shutdown(void);

/**
 * 获取系统启动模式
 *
 * @return 返回系统启动模式，取值参见 ant_poweron_mode_e
 */
ant_s32 ant_system_get_poweron_mode(void);

/**
 * 系统回调函数
 *
 * @param event 系统事件
 * @param param 系统事件参数
 * @param user_data 用户参数
 *
 */
typedef void (*ant_system_event_callback)(ant_event_e event, void *param, void *user_data);

/**
 * 注册系统事件回调
 *
 * @param cb 回调函数
 * @param user_data 回调函数参数，回调时传入
 */
void ant_system_register_event(ant_system_event_callback cb, void *user_data);

/**
 * 取消系统事件回调注册
 *
 * @param cb 回调函数
 */
void ant_system_unregister_event(ant_system_event_callback cb);


void __assert_func(const char *s1, int, const char *s2, const char *s3);

ant_s32 ant_get_crash_info(ant_s32 * count,ant_char * buffer);

ant_s32 ant_reset_crash_info();

char * ant_system_version_get(void);
    
ant_bool ant_system_security_get(void);  

void     ant_system_pwrkey_config(void);

    
#ifdef __cplusplus
} // extern "C"
#endif

#endif


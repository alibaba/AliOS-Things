#ifndef __ANT_LOG_H__
#define __ANT_LOG_H__
#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#include "ant_typedef.h"

typedef enum {
	LOG_LEVEL_OFF = 0,
	LOG_LEVEL_ERR = 1,
	LOG_LEVEL_WRN = 2,
	LOG_LEVEL_INF = 3,
	LOG_LEVEL_VBS = 4
} ant_log_level_e;

//#define ANT_CONFIG_DEBUG_OFF
#define ANT_CONFIG_IOTSDK_DEBUG_OFF
#ifdef DEBUG_KAL
#define ANT_CONFIG_USE_CATCHER_LOG
#endif


#ifndef __SDK_DEBUG_ENABLE__
#define MAXCHARS           255
#else 
#define MAXCHARS           1024
#endif

#ifndef ANT_CONFIG_DEBUG_OFF
#if 0//#ifdef CONFIG_SOC_8910
#define OSI_LOG_TAG OSI_MAKE_LOG_TAG('A', 'L', 'I', 'P')
#include "osi_log.h"
#define PAYBOX_LOGI(tag, format, ... ) OSI_LOGI(0, "["tag"]"format, ##__VA_ARGS__)
#define PAYBOX_LOGW(tag, format, ... ) OSI_LOGW(0, "["tag"]"format, ##__VA_ARGS__)
#define PAYBOX_LOGE(tag, format, ... ) OSI_LOGE(0, "["tag"]"format, ##__VA_ARGS__)
#else
#define PAYBOX_LOGI ant_logi
#define PAYBOX_LOGW ant_logw
#define PAYBOX_LOGE ant_loge
#define PAYBOX_LOGS ant_logs

#endif
#else
#define PAYBOX_LOGI(tag, format, ... )
#define PAYBOX_LOGW(tag, format, ... )
#define PAYBOX_LOGE(tag, format, ... )
#endif
void uart_send(char *buffer, ant_u32 size);
void ant_log_init(void);
void ant_print(const char *fmt,...);
void ant_printd(const char *fmt, ...);
void ant_logi(const char *tag, const char *format, ...);
void ant_logw(const char *tag, const char *format, ...);
void ant_loge(const char *tag, const char *format, ...);
void ant_log_set_level(ant_s32 level);
void ant_log_init(void);
void ant_log_switch(int on);
void ant_log(int log_level, const char *tag, const char *fmt, va_list ap);
void ant_log_ex(const char *file, ant_u32 line, int log_level, const char *tag, const char *fmt, va_list ap);
void ant_log_set_iotlog(int enable);
void ant_logs(const char *tag,const char * hdr,const char * body);


#ifdef __cplusplus
}
#endif
#endif


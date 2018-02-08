#ifndef LOG_SERVICE_H
#define LOG_SERVICE_H

#include "dlist.h"
/*
 * Include user defined options first. Anything not defined in these files
 * will be set to standard values. Override anything you dont like!
 */
#if defined(CONFIG_PLATFORM_8195A) || defined(CONFIG_PLATFORM_8711B)
#include "platform_opts.h"
#include "platform_stdlib.h"
#endif

//#ifdef __ICCARM__
//#define STRINGIFY(s) #s
//#define SECTION(_name) _Pragma( STRINGIFY(location=_name))
//#define log_module_init(fn) \
//	SECTION(".data.log_init") __root static void* log_##fn = (void*)fn
//#elif defined(__CC_ARM)
//#define log_module_init(fn) \
//static void* log_##fn __attribute__((section(".data.log_init")))  = (void*)fn;
//#define DiagPrintf printf
//#elif defined(__GNUC__)
//#define log_module_init(fn) \
//static void* log_##fn __attribute__((section(".data.log_init")))  = (void*)fn;
//#else
//#error "not implement"
//#endif

#define log_module_init(fn) 
		
#define ATC_INDEX_NUM 32

#ifndef SUPPORT_LOG_SERVICE
#define SUPPORT_LOG_SERVICE	1
#endif

//LOG_SERVICE_BUFLEN: default, only 63 bytes could be used for keeping input
//                                     cmd, the last byte is for string end ('\0').
#ifndef LOG_SERVICE_BUFLEN
#define LOG_SERVICE_BUFLEN     64     
#endif

#ifndef CONFIG_LOG_HISTORY
#define CONFIG_LOG_HISTORY	0
#if CONFIG_LOG_HISTORY
#define LOG_HISTORY_LEN    5
#endif
#endif //#ifndef CONFIG_LOG_HISTORY

#ifndef MAX_ARGC
#define MAX_ARGC 12
#endif

#ifndef CONFIG_LOG_SERVICE_LOCK
#define CONFIG_LOG_SERVICE_LOCK 0 // //to protect log_buf[], only one command processed per time
#endif

#define AT_BIT(n)           (1<<n)
#define AT_FLAG_DUMP        AT_BIT(0)
#define AT_FLAG_EDIT        AT_BIT(1)
#define AT_FLAG_ADC         AT_BIT(2)
#define AT_FLAG_GPIO        AT_BIT(3)
#define AT_FLAG_OTA         AT_BIT(4)
#define AT_FLAG_NFC         AT_BIT(5)
#define AT_FLAG_OS          AT_BIT(6)
#define AT_FLAG_LWIP        AT_BIT(7)
#define AT_FLAG_COMMON      AT_BIT(8)
#define AT_FLAG_WIFI        AT_BIT(9)
#define AT_FLAG_RDP         AT_BIT(10)

enum{
	AT_DBG_OFF = 0,
	AT_DBG_ALWAYS,
	AT_DBG_ERROR,
	AT_DBG_WARNING,
	AT_DBG_INFO
};

extern unsigned char  gDbgLevel;
extern unsigned int   gDbgFlag;

#define AT_PRINTK(...)			\
		do{							\
			printf(__VA_ARGS__); 	\
			printf("\r\n");			\
		}while(0)
#define _AT_PRINTK(...)	printf(__VA_ARGS__)
#define AT_DBG_MSG(flag, level, ...)					\
		do{														\
			if(((flag) & gDbgFlag) && (level <= gDbgLevel)){	\
				AT_PRINTK(__VA_ARGS__);							\
			}													\
		}while(0)
#define _AT_DBG_MSG(flag, level, ...)					\
		do{														\
			if(((flag) & gDbgFlag) && (level <= gDbgLevel)){	\
				_AT_PRINTK(__VA_ARGS__);						\
			}													\
		}while(0)

#ifndef SUPPORT_INTERACTIVE_MODE
#define SUPPORT_INTERACTIVE_MODE	0
#endif //#ifndef SUPPORT_INTERACTIVE_MODE

typedef void (*log_init_t)(void);
typedef void (*log_act_t)(void*);
typedef struct _at_command_item_{
	char *log_cmd;
	log_act_t at_act;
	struct list_head node;
}log_item_t;

void log_service_add_table(log_item_t *tbl, int len);
int parse_param(char *buf, char **argv);
#if CONFIG_LOG_SERVICE_LOCK
void log_service_lock_init(void);
void log_service_lock(void);
u32 log_service_lock_timeout(u32 ms);
void log_service_unlock(void);
#endif

#define C_NUM_AT_CMD			4 //"ATxx", 4 characters
#define C_NUM_AT_CMD_DLT		1 //"=", 1 charater
#define STR_END_OF_ATCMD_RET	"\r\n\n# " //each AT command response will end with this string
#define STR_END_OF_ATDATA_RET	"\r\n\n> " //data transparent transmission indicator
#endif

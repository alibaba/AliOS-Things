#include <stdarg.h>
#include <string.h>
#include <stdio.h>
//#include "FreeRTOS.h"
#include <aos_osdep.h>
#if defined(configUSE_WAKELOCK_PMU) && (configUSE_WAKELOCK_PMU == 1)
#include "freertos_pmu.h"
#endif
#include "log_service.h"
//#include "task.h"
//#include "semphr.h"
#include "main.h"
//#include "wifi_util.h"
#include "atcmd_wifi.h"
#if CONFIG_EXAMPLE_UART_ATCMD || CONFIG_EXAMPLE_SPI_ATCMD 
#include "atcmd_lwip.h"
#endif

#include "osdep_service.h"

#if SUPPORT_LOG_SERVICE
//======================================================
struct list_head log_hash[ATC_INDEX_NUM];

extern void at_wifi_init(void);
extern void at_fs_init(void);
extern void at_sys_init(void);
extern void at_ethernet_init(void);
extern void at_google_init(void);
extern void at_transport_init(void);
//extern void at_app_init(void);
extern void at_mp_init(void);

void at_log_init(void);

char log_buf[LOG_SERVICE_BUFLEN];
#if CONFIG_LOG_HISTORY
char log_history[LOG_HISTORY_LEN][LOG_SERVICE_BUFLEN];
static unsigned int log_history_count = 0;
#endif
_sema log_rx_interrupt_sema;
#if CONFIG_LOG_SERVICE_LOCK
_sema log_service_sema; 
#endif
extern _sema	uart_rx_interrupt_sema;

#if CONFIG_INIC_EN
extern unsigned char inic_cmd_ioctl;
#endif

//#if defined (__ICCARM__)
//#pragma section=".data.log_init"
//
//unsigned int __log_init_begin__;
//unsigned int __log_init_end__;
//#elif defined ( __CC_ARM   ) || defined(__GNUC__)
#if defined (__ICCARM__) || defined ( __CC_ARM   ) || defined(__GNUC__)
//#pragma section=".data.log_init"
log_init_t* __log_init_begin__;
log_init_t* __log_init_end__;
log_init_t log_init_table[] = {
	at_wifi_init,
	//	at_fs_init,

	//at_sys_init,
	at_log_init,
	//	at_app_init,
#if CONFIG_ETHERNET  
	at_ethernet_init,
#endif  

#if CONFIG_GOOGLE_NEST  
	at_google_init,
#endif 

#if CONFIG_TRANSPORT  
	at_transport_init,
#endif

#if CONFIG_ATCMD_MP
	at_mp_init,
#endif
};
#else
#error "not implement, add to linker script"
extern unsigned int __log_init_begin__;
extern unsigned int __log_init_end__;
#endif

#if defined(__GNUC__)
#define USE_STRSEP
#endif

//======================================================
int hash_index(char *str)
{
    unsigned int seed = 131; // 31 131 1313 13131 131313 etc..
    unsigned int hash = 0;

    while (*str)
    {
        hash = hash * seed + (*str++);
    }

    return (hash & 0x7FFFFFFF);	
}

void log_add_new_command(log_item_t *new)
{
	int index = hash_index(new->log_cmd)%ATC_INDEX_NUM;
	
	list_add(&new->node, &log_hash[index]);
}
void start_log_service(void);
void log_service_init(void)
{
	int i;
	
//#if defined (__ICCARM__)
//	log_init_t *log_init_table;
//	__log_init_begin__ = (unsigned int)__section_begin(".data.log_init");
//	__log_init_end__ = (unsigned int)__section_end(".data.log_init");
//	log_init_table = (log_init_t *)__log_init_begin__;
//#elif defined(__CC_ARM) || defined(__GNUC__)
#if defined (__ICCARM__) || defined(__CC_ARM) || defined(__GNUC__)
	__log_init_begin__ = log_init_table;
	__log_init_end__ = __log_init_begin__ + sizeof(log_init_table);
#else
	#error "not implement"
#endif
		
	
	for(i=0;i<ATC_INDEX_NUM;i++)
		INIT_LIST_HEAD(&log_hash[i]);
	
	for(i=0;i<(__log_init_end__-__log_init_begin__)/sizeof(log_init_t); i++)
		log_init_table[i]();

  	/* Initial uart rx swmaphore*/
	rtw_init_sema(&log_rx_interrupt_sema, 1);
	rtw_down_sema(&log_rx_interrupt_sema);
#if CONFIG_LOG_SERVICE_LOCK
	log_service_lock_init();
#endif
	start_log_service();
}

//sizeof(log_items)/sizeof(log_items[0])
void log_service_add_table(log_item_t *tbl, int len)
{
	int i;
	for(i=0;i<len;i++)
		log_add_new_command(&tbl[i]);	
}

void* log_action(char *cmd)
{
	int search_cnt=0;
	int index = hash_index(cmd)%ATC_INDEX_NUM;
	struct list_head *head = &log_hash[index];
	struct list_head *iterator;
	log_item_t *item;
	void *act = NULL;
	
	list_for_each(iterator, head) {
		item = list_entry(iterator, log_item_t, node);
		search_cnt++;
		if( strcmp(item->log_cmd, cmd) == 0){
			//printf("%s match %s, search cnt %d\n\r", cmd, item->log_cmd, search_cnt);
			act = (void*)item->at_act;
			break;
		}
	}
	
	return act;
}

void* log_handler(char *cmd)
{
	log_act_t action=NULL;
	char buf[LOG_SERVICE_BUFLEN];
	memset(buf, 0, LOG_SERVICE_BUFLEN);
	char *copy=buf;
	char *token = NULL;
	char *param = NULL;
	char tok[5] = {0};//'\0'
#if CONFIG_LOG_HISTORY
	strcpy(log_history[((log_history_count++)%LOG_HISTORY_LEN)], log_buf);
#endif
	strncpy(copy, cmd,LOG_SERVICE_BUFLEN-1);

#if defined(USE_STRSEP)
	token = _strsep(&copy, "=");
	param = copy;
#else
	token = strtok(copy, "=");
	param = strtok(NULL, NULL);
#endif
	if(token && (strlen(token) <= 4))
		strcpy(tok, token);
	else{
		//printf("\n\rAT Cmd format error!\n");
		return NULL;
	};
	//printf(" Command %s \n\r ", tok);
	//printf(" Param %s \n\r", param);
	action = (log_act_t)log_action(tok);
        
	if(action){	
		action(param);
	} 
	return (void*)action;

}

int parse_param(char *buf, char **argv)
{
	int argc = 1;
	char str_buf[LOG_SERVICE_BUFLEN];
	memset(str_buf, 0, LOG_SERVICE_BUFLEN);
	int str_count = 0;
	int buf_cnt = 0;

	if(buf == NULL)
		goto exit;
	
	while((argc < MAX_ARGC) && (*buf != '\0')) {
		while((*buf == ',') || (*buf == '[') || (*buf == ']')){
			if((*buf == ',') && (*(buf+1) == ',')){
				argv[argc] = NULL;
				argc++;
			}
			*buf = '\0';
			buf++;
		}

		if(*buf == '\0')
			break;
		else if(*buf == '"'){
			memset(str_buf,'\0',LOG_SERVICE_BUFLEN);
			str_count = 0;
			buf_cnt = 0;
			*buf = '\0';
			buf ++;         
			if(*buf == '\0')
			break;
		argv[argc] = buf;
			while((*buf != '"')&&(*buf != '\0')){
				if(*buf == '\\'){
		buf ++;
					buf_cnt++;
				}
				str_buf[str_count] = *buf;
				str_count++;
				buf_cnt++;
			buf ++;
			}
			*buf = '\0';
			memcpy(buf-buf_cnt,str_buf,buf_cnt);
		}
		else{
			argv[argc] = buf;
		}
		argc++;
		buf++;

		while( (*buf != ',')&&(*buf != '\0')&&(*buf != '[')&&(*buf != ']') )
			buf++;
	}
exit:
	return argc;
}

unsigned char  gDbgLevel = AT_DBG_ERROR;
unsigned int   gDbgFlag  = 0xFFFFFFFF;
void at_set_debug_level(unsigned char newDbgLevel)
{
    gDbgLevel = newDbgLevel;
}

void at_set_debug_mask(unsigned int newDbgFlag)
{
	gDbgFlag = newDbgFlag;
}

#if SUPPORT_INTERACTIVE_MODE
extern char uart_buf[64];
void legency_interactive_handler(unsigned char argc, unsigned char **argv)
{
#if 0 //defined(CONFIG_PLATFORM_8195A)
   if(argc<1)
	{
		DiagPrintf("Wrong argument number!\r\n");
		return;
	}


	DiagPrintf("Wlan Normal Mode\n");

	WlanNormal( argc, argv);
#else
   strncpy(uart_buf, log_buf, 63);//uart_buf[64]
   rtw_up_sema(&uart_rx_interrupt_sema);
#endif
}
#endif

#if CONFIG_WLAN
#ifndef WLAN0_NAME
  #define WLAN0_NAME		"wlan0"
#endif
#ifndef WLAN1_NAME
  #define WLAN1_NAME		"wlan1"
#endif
int mp_commnad_handler(char *cmd)
{
	char buf[64];
	char *token = NULL;
	memset(buf, 0, 64);
	
	//strcpy(buf, cmd);
        strncpy(buf, cmd, (64-1));
	token = strtok(buf, " ");
	if(token && (strcmp(buf, "iwpriv") == 0)){
		token = strtok(NULL, "");
		wext_private_command(WLAN0_NAME, token, 1);
		return 0;
	}
	return -1;
}
#endif
void print_help_msg(void){
#if CONFIG_WLAN
extern void print_wlan_help(void);
        print_wlan_help();
#endif
//add other help message print here
}

int print_help_handler(char *cmd){
	if(strcmp(cmd, "help") == 0){
                print_help_msg();
                return 0;
	}
        return -1;
}

#if CONFIG_LOG_SERVICE_LOCK
void log_service_lock(void)
{
	rtw_down_sema(&log_service_sema);
}

u32 log_service_lock_timeout(u32 ms)
{
	return rtw_down_timeout_sema(&log_service_sema, ms);
}

void log_service_unlock(void)
{
	rtw_up_sema(&log_service_sema);
}

void log_service_lock_init(void){
	rtw_init_sema(&log_service_sema, 1);
}
#endif

void log_service(void *param)
{
	_AT_DBG_MSG(AT_FLAG_COMMON, AT_DBG_ALWAYS, "\n\rStart LOG SERVICE MODE\n\r");
	_AT_DBG_MSG(AT_FLAG_COMMON, AT_DBG_ALWAYS, "\n\r# ");        
	while(1){
		while(rtw_down_sema(&log_rx_interrupt_sema) != pdTRUE);
#if CONFIG_LOG_SERVICE_LOCK
		log_service_lock();
#endif
		if(log_handler((char *)log_buf) == NULL){
#if CONFIG_WLAN
			if(mp_commnad_handler((char *)log_buf) < 0)
#endif                        
			{
			#if SUPPORT_INTERACTIVE_MODE
				print_help_handler((char *)log_buf);
				legency_interactive_handler(NULL, NULL);
			#else
				if(print_help_handler((char *)log_buf) < 0){
					at_printf("\r\nunknown command '%s'", log_buf);
				}
			#endif
			}
		}
		log_buf[0] = '\0';
#if CONFIG_INIC_EN
		inic_cmd_ioctl = 0;
#endif
		_AT_DBG_MSG(AT_FLAG_COMMON, AT_DBG_ALWAYS, "\n\r[MEM] After do cmd, available heap %d\n\r", rtw_getFreeHeapSize());
		_AT_DBG_MSG(AT_FLAG_COMMON, AT_DBG_ALWAYS, "\r\n\n# "); //"#" is needed for mp tool
#if CONFIG_EXAMPLE_UART_ATCMD
		if(atcmd_lwip_is_tt_mode())
			at_printf(STR_END_OF_ATDATA_RET);
		else
			at_printf(STR_END_OF_ATCMD_RET);
#endif
#if CONFIG_LOG_SERVICE_LOCK
		log_service_unlock();
#endif
#if defined(configUSE_WAKELOCK_PMU) && (configUSE_WAKELOCK_PMU == 1)
		pmu_release_wakelock(BIT(PMU_LOGUART_DEVICE));
#endif
	}
}

#define STACKSIZE               1280
ktask_t *g_log_service_task;

void start_log_service(void)
{
	//xTaskHandle CreatedTask;
	int result;

#if CONFIG_USE_TCM_HEAP
	/*********************************************************************
	 *                                                                                                                         
	 * ATCMD V2 supports commands for SSL                                                                                     
	 * It will cause problems while doing SSL operations if the stack is placed in TCM region   
	 *
	 *********************************************************************/
	void *stack_addr = NULL;
#if (ATCMD_VER == ATVER_1) || ((ATCMD_VER == ATVER_2)&&(ATCMD_SUPPORT_SSL == 0))
	extern void *tcm_heap_malloc(int size);
	stack_addr = tcm_heap_malloc(STACKSIZE * sizeof(int));

	if(stack_addr == NULL){
	}
#endif

	result = xTaskGenericCreate(
			log_service,
			( signed portCHAR * ) "log_service",
			STACKSIZE,
			NULL,
			tskIDLE_PRIORITY + 5,
			&CreatedTask,
			stack_addr,
			NULL);
#else		
	//result = krhino_task_dyn_create( log_service, ( signed portCHAR * ) "log_service", STACKSIZE, NULL, tskIDLE_PRIORITY + 5, &CreatedTask );
        result = krhino_task_dyn_create(&g_log_service_task, "log_service", 0, 6, 0, STACKSIZE, (task_entry_t)log_service, 1);

#endif
   
	if(result != 0) {
		printf("\n\r%s xTaskCreate failed", __FUNCTION__);
	}

}

void fAT_exit(void *arg){
	printf("\n\rLeave LOG SERVICE");
	krhino_task_dyn_del(NULL);
}
#if CONFIG_LOG_HISTORY
void fAT_log(void *arg){
        int i = 0;
        printf("[AT]log history:\n\n\r");
        if(log_history_count > LOG_HISTORY_LEN){
          for(i=0; i<4; i++)
            printf("  %s\n\r", log_history[((log_history_count+i)%LOG_HISTORY_LEN)]);
        }
        else{
          for(i=0; i<(log_history_count-1); i++)
            printf("  %s\n\r", log_history[i]);
        }
}
#endif
log_item_t at_log_items[ ] = {
      {"AT--", fAT_exit,},
#if CONFIG_LOG_HISTORY
      {"AT??", fAT_log,},
#endif
      {"ATxx", fAT_exit,}    
};
void at_log_init(void)
{
	log_service_add_table(at_log_items, sizeof(at_log_items)/sizeof(at_log_items[0]));
}
log_module_init(at_log_init);
#endif

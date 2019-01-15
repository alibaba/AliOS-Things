#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include "ulog/ulog.h"
#include "aos/kernel.h"
#include "k_config.h"

typedef void(* on_ulog_man_service)(const uint32_t, const uint32_t);

typedef uint8_t session_filter_para_t;

typedef struct{
    char*                func_mark;
    char*                sub_func_mark[2];
    on_ulog_man_service  handler;
}ulog_man_handler_service_t;

#define CHECK_PASS_POP_OUT(SESSION, LEVEL) (stop_filter_level[SESSION]>LEVEL)

/* used for indicates message has been filled into fifo */
static aos_sem_t sem_log_push;

static aos_task_t ulog_routine;
static char g_host_name[10];

#define TAG_FILE_NAME_MAX_LEN 12
static char* UNKNOWN_FILE_NAME = "unknown";


const char months[][4] = {"Jan", "Feb", "Mar", "Apr", "May",
                       "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

static aos_mutex_t ulog_mutex;

uint8_t push_stop_filter_level = LOG_EMERG;

/* stop filter, any level <= stop_filter_level(value >= this value) will be abonded */
session_filter_para_t stop_filter_level[SESSION_CNT] =
{
    STOP_FILTER_DEFAULT, STOP_FILTER_UDP
};

extern void on_update_syslog_watcher_addr(const uint32_t ip_d, const uint32_t port );
extern void on_tcpip_service_on(const uint32_t on, const uint32_t off);

const ulog_man_handler_service_t ulog_man_handler_service[] =
{
    { "listen", {"ip","port"}, on_update_syslog_watcher_addr },
    { "tcpip",  {"on","off" }, on_tcpip_service_on           },
};

static void ulog_man_handler(const char* raw_str)
{
    if(raw_str!=NULL){
        const uint8_t man_handler_service_size = sizeof(ulog_man_handler_service)/sizeof(ulog_man_handler_service_t);
        uint8_t i = 0;
        for(; i<man_handler_service_size; i++){
            if(0==strncmp(ulog_man_handler_service[i].func_mark, raw_str, strlen(ulog_man_handler_service[i].func_mark))){
                char* p = &raw_str[strlen(ulog_man_handler_service[i].func_mark)];
                if(p[0] == ' '){
                    uint32_t param[2] = {0, 0};
                    uint8_t j = 0;

                    for(; j<2; j++){
                        char * q = NULL;
                        if( NULL != (q=strstr(&p[1], ulog_man_handler_service[i].sub_func_mark[j])) ){
                            char* val_str = &q[strlen(ulog_man_handler_service[i].sub_func_mark[j])+1];
                            param[j] = strtoul(val_str, NULL, 10);
                        }
                    }
                    if( (param[0]!=0||param[1]!=0) && (ulog_man_handler_service[i].handler != NULL) ){
                        ulog_man_handler_service[i].handler(param[0], param[1]);
                    }
                }
                break;
            }
        }
    }
}

static void* wrapper_mutex_init()
{
    void* rc = NULL;

    if(0==aos_mutex_new(&ulog_mutex)){
        rc = (void*)&ulog_mutex;
    }
    return rc;
}

int32_t wrapper_mutex_lock(void* param)
{
    return aos_mutex_lock((aos_mutex_t*)param,AOS_WAIT_FOREVER);
}

int32_t wrapper_mutex_unlock(void* param)
{
    return aos_mutex_unlock((aos_mutex_t*)param);
}

//Result is like "Nov 28 15:19:20.122 "
char *syslog_format_time(char *buffer, const int len)
{
    const long long ms = aos_now_ms();
    if (buffer != NULL && len > 4) {
        time_t rawtime;
        time(&rawtime);
        struct tm *tm = localtime(&rawtime);
        //%b format of strftime() is platform-dependent, so we realized it by-self
        snprintf(buffer,len,"%s ",months[tm->tm_mon<12?tm->tm_mon:0]);
        strftime(&buffer[4], len-4, "%d %H:%M:%S", tm);
        const int32_t milli    = ms % 1000;
        char      ms_str[8] = "";
	    memset(ms_str, 0, sizeof(ms_str));
        snprintf(ms_str, sizeof(ms_str), ".%03d ", milli);
        strncat(buffer, ms_str, len - strlen(buffer) - 1);
    }

    return buffer;
}

static void ulog_push_cb(void* para)
{
    aos_sem_signal(&sem_log_push);
}

static char* trim_file_path(const char* path)
{
	char* filename = (char*)path;
    if(path!=NULL) {
        if( (filename=strrchr(path, '/')) != NULL ) {
            filename++;
        }
    }

    if(filename!=NULL) {
        const size_t filename_len=strlen(filename);
        if( filename_len>0 ) {
            if(filename_len>TAG_FILE_NAME_MAX_LEN) { //trig it using last 32 string
                filename += filename_len-TAG_FILE_NAME_MAX_LEN;
            }
        } else {//cannot get file name if input path is like "/d/timer/"
            filename = UNKNOWN_FILE_NAME;
        }
    } else { //using UNKNOWN_FILE_NAME
        filename = UNKNOWN_FILE_NAME;
    }

	return filename;
}

osi_uring_fifo os_related =
{
    &g_host_name[0],
    wrapper_mutex_init,
    wrapper_mutex_lock,
    wrapper_mutex_unlock,
    syslog_format_time,
    ulog_push_cb,
    trim_file_path
};

static void ulog_handler(void* para, const void* log_text, const uint16_t log_len)
{
    if( (log_text!=NULL) && log_len>0 ) {
        char* str = (char*)log_text;
#if EXTREAM_LOG_TEXT==0
        if( (str[0]=='<') && (strchr(str, '>')!=NULL) ) {//syslog format
            //pri = facility*8+level
            const uint32_t pri = strtoul(&str[1], NULL, 10);
            const uint8_t level = (pri&0x7);
            if(CHECK_PASS_POP_OUT(SESSION_DIRECTION,level)){
                if(log_get_mutex()){
                    puts(log_text);
                    log_release_mutex();
                }
            }

            if(CHECK_PASS_POP_OUT(SESSION_UDP,level)){
                pop_out_on_udp(log_text,log_len);
            }
        }else
#endif
        if(0==strncmp(str,ULOG_CMD_PREFIX,strlen(ULOG_CMD_PREFIX))){
            /* syslog management */
            ulog_man_handler(&str[strlen(ULOG_CMD_PREFIX)]);
        }else{
            if(log_get_mutex()){
                puts(log_text);
                log_release_mutex();
            }
        }
    }
}

static void log_routine(void* para)
{
    while(1)
    {
        //PRI      HEADER            MSG
        //<130>Oct 9 22:33:20.111 soc OS_kernel.c[111]: The audit daemon is exiting.
        if(aos_sem_wait(&sem_log_push, AOS_WAIT_FOREVER)==0)
        {
            uring_fifo_pop_cb(ulog_handler, NULL);
        }
    }
}

void ulog_async_init(const uint8_t host_name[8])
{
    uring_fifo_init(DEFAULT_ASYNC_SYSLOG_DEPTH);
    if( 0==aos_sem_new(&sem_log_push, 0) ) {
        if( 0==aos_task_new_ext(&ulog_routine,
            "ulog",
            log_routine,
            NULL,
            LOG_ROUTINE_TASK_STACK_DEPTH,
            RHINO_CONFIG_USER_PRI_MAX) ) {
            on_filter_level_change(SESSION_CNT,LOG_NONE);
            if(NULL!=host_name) {
                strncpy(g_host_name, host_name, sizeof(g_host_name));
                g_host_name[sizeof(g_host_name)-1] = '\0';
            }
        }
    }
}

void on_filter_level_change(const SESSION_TYPE session, const uint8_t level)
{
    bool next_handle = true;
    if(session==SESSION_CNT){//only happen on init, get the initial push level
        push_stop_filter_level = LOG_EMERG;
    }else if(session<SESSION_CNT && level<=LOG_NONE){
        stop_filter_level[session] = level;

        //suppose we use update session sf(stop filter) as push filter, this value will be updated below
        push_stop_filter_level = stop_filter_level[session];
    }else{
        next_handle = false;
    }
    if(next_handle){
        uint8_t i = 0;
        for(; i<SESSION_CNT;i++)
        {
            if(push_stop_filter_level<stop_filter_level[i]){
                push_stop_filter_level = stop_filter_level[i];
            }
        }
    }
}

void ulog_man(const char* cmd_str)
{
    if(cmd_str!=NULL && log_init){
        const uint16_t cmd_str_size = strlen(cmd_str)+strlen(ULOG_CMD_PREFIX)+1;
        char* tmpbuf = (char*)aos_malloc(cmd_str_size);
        strncpy(tmpbuf, ULOG_CMD_PREFIX, cmd_str_size);
        strncat(tmpbuf, cmd_str, cmd_str_size - strlen(ULOG_CMD_PREFIX) - 1);
        uring_fifo_push_s(tmpbuf, cmd_str_size);
        aos_free(tmpbuf);
    }
}


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include "ulog/ulog.h"
#include "aos/kernel.h"
#include "k_config.h"

typedef struct{
    uint8_t stop_filter_level;//stop filter, any level <= stop_filter_level(value >= this value) will be abonded
    uint8_t condition_session;//0 indicates condition fulfill. bit 1 indicates if udp ready
}session_filter_para_t;

#define CHECK_PASS_POP_OUT(SESSION, LEVEL) (session_filter_para[SESSION].stop_filter_level>LEVEL) &&\
                                           (0==session_filter_para[SESSION].stop_filter_level)

static aos_sem_t sem_log;

static aos_task_t ulog_routine;
static char g_host_name[10];

#define TAG_FILE_NAME_MAX_LEN 12
static char* UNKNOWN_FILE_NAME = "unknown";


const char months[][4] = {"Jan", "Feb", "Mar", "Apr", "May",
                       "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

static aos_mutex_t ulog_mutex;

uint8_t push_stop_filter_level = LOG_EMERG;

static session_filter_para_t session_filter_para[SESSION_CNT] =
{
    {STOP_FILTER_DEFAULT, 0x00},
    {STOP_FILTER_UDP    , 0x02}
};

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
static char *syslog_format_time(char *buffer, const int len)
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
        char      msStr[8] = "";
	    memset(msStr, 0, sizeof(msStr));
        snprintf(msStr, sizeof(msStr), ".%03d ", milli);
        strncat(buffer, msStr, len - strlen(buffer) - 1);
    }

    return buffer;
}

static void ulog_push_cb(void* para)
{
    aos_sem_signal(&sem_log);
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

static void pop_out_sessions(void* para, const void* log_text, const uint16_t log_len)
{
    if( (log_text!=NULL) && log_len>0 ) {
        char* str = (char*)log_text;
#if EXTREAM_LOG_TEXT==0
        if( (str[0]=='<') && (strchr(str, '>')!=NULL) ) {
            //pri = facility*8+level
            const uint32_t pri = strtoul(&str[1], NULL, 10);
            const uint8_t level = (pri&0x7);
            if(log_get_mutex()){
                puts(log_text);
                log_release_mutex();
            }
        }
#else
    if(log_get_mutex()){
        puts(log_text);
        log_release_mutex();
    }

#endif
    }
}

static void log_routine(void* para)
{
    while(1)
    {
        //PRI      HEADER            MSG
        //<130>Oct 9 22:33:20.111 soc OS_kernel.c[111]: The audit daemon is exiting.
        if(aos_sem_wait(&sem_log, AOS_WAIT_FOREVER)==0)
        {
            uring_fifo_pop_cb(pop_out_sessions, NULL);
        }
    }
}


void ulog_async_init(const uint8_t host_name[8])
{
    uring_fifo_init(DEFAULT_ASYNC_SYSLOG_DEPTH);
    if( 0==aos_sem_new(&sem_log, 0) ) {
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
    bool next_handle = false;
    if(session==SESSION_CNT){//only happen on init, get the first push level
        push_stop_filter_level = LOG_EMERG;
        next_handle = true;
    }else if(session<SESSION_CNT && level<=LOG_NONE){
        session_filter_para[session].stop_filter_level = level;
        push_stop_filter_level = session_filter_para[session].stop_filter_level;
        if(push_stop_filter_level>session_filter_para[session].stop_filter_level){//push_stop_filter may adjust
            next_handle = true;
        }
    }
    if(next_handle){
        uint8_t i = 0;
        for(; i<SESSION_CNT;i++)
        {
            if(push_stop_filter_level<session_filter_para[i].stop_filter_level){
                push_stop_filter_level = session_filter_para[i].stop_filter_level;
            }
        }
    }
}


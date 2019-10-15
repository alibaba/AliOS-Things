/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "ulog/ulog.h"
#include "aos/kernel.h"
#include "ulog_api.h"
#include "uagent/uagent.h"
#include "ulog_ring_fifo.h"
#include "aos/errno.h"
static char serverity_name[LOG_NONE] = { 'V', 'A', 'F', 'E', 'W', 'T', 'I', 'D' };

#define UNKNOWN_BUF ""

/* stop filter, any level <= stop_filter_level(value >= this value) will be abonded */
static uint8_t stop_filter_level[ulog_session_size] = {
    STOP_FILTER_DEFAULT,

#if ULOG_POP_CLOUD_ENABLE
    STOP_FILTER_CLOUD,
#endif

#if ULOG_POP_FS_ENABLE
   STOP_FILTER_FS,
#endif

#if ULOG_POP_UDP_ENABLE
   STOP_FILTER_UDP,
#endif

};

static uint8_t push_stop_filter_level = LOG_EMERG;


/* Prefix <248>~<255> */
/* using type char instead of int8_t which align with the prototype of string operation function */

#ifdef ULOG_CONFIG_ASYNC
static char ulog_buf[ULOG_SIZE];
#endif

bool check_pass_pop_out(const ulog_session_type_t session, const uint8_t level)
{
    return (stop_filter_level[session]>level);
}

int aos_set_log_level(aos_log_level_t log_level)
{
    int rc = -EINVAL;
    if(log_init){
        if (log_level <= AOS_LL_DEBUG) {
            on_filter_level_changes(ulog_session_std, log_level + 1);
            rc = 0;
        }
    }
    return rc;
}

char* get_sync_stop_level()
{
#ifdef CONFIG_LOGMACRO_SILENT
    return "NDEBUG mode active";
#else
    if (stop_filter_level[ulog_session_std] == LOG_NONE) {
        return "all log recorded";
    } else if (stop_filter_level[ulog_session_std] == LOG_EMERG) {
        return "all log stop";
    } else {
        static char buf[32];
        snprintf(buf, 24, "current log level %c",serverity_name[stop_filter_level[ulog_session_std]-1]);
        return buf;
    }
#endif
}

#if SYNC_DETAIL_COLOR
/*
 * color def.
 * see http://stackoverflow.com/questions/3585846/color-text-in-terminal-applications-in-unix
 */
#define COL_DEF "\x1B[0m"  /* white */
#define COL_RED "\x1B[31m" /* red */
#define COL_GRE "\x1B[32m" /* green */
#define COL_BLU "\x1B[34m" /* blue */
#define COL_YEL "\x1B[33m" /* yellow */
#define COL_WHE "\x1B[37m" /* white */
#define COL_CYN "\x1B[36m"
#define COL_MAG "\x1B[35m"

static char log_col_list[LOG_NONE][12] =
{
    COL_DEF, COL_RED, COL_RED, COL_RED, COL_BLU, COL_GRE, COL_GRE, COL_WHE
};
static char* log_col_def(const unsigned char level)
{
    if(level<LOG_NONE){
        return log_col_list[level];
    }else{
        return log_col_list[0];
    }
}
#else
#define log_col_def(x) ""
#endif

#ifdef ULOG_CONFIG_ASYNC
static uint8_t get_lowest_level(const ulog_session_type_t start);
#endif

int ulog(const unsigned char s, const char *mod, const char *f, const unsigned long l, const char *fmt, ...)
{
    int rc = -1;
    if (log_init &&
        (s < push_stop_filter_level) ){
        char log_time[24];
        if (log_get_mutex()) {
            const char* rpt_mod = NULL;
            if ((mod == NULL) || (0 == strlen(mod))) {
                rpt_mod = UNKNOWN_BUF;
            } else {
                rpt_mod = mod;
            }
#ifdef ULOG_CONFIG_ASYNC
            uint8_t facility = FACILITY_NORMAL_LOG;
            if(strlen(rpt_mod)==0 || 0==strncmp("MQTT",rpt_mod, 4)){
                facility = FACILITY_NORMAL_LOG_NO_POP_CLOUD;
            }

            snprintf(ulog_buf,ULOG_HEADER_TYPE_LEN+1,"<%03d>",s+(facility&0xF8));

#if SYNC_LOG_DETAILS
            snprintf(&ulog_buf[ULOG_HEADER_TYPE_LEN], ULOG_SIZE-ULOG_HEADER_TYPE_LEN, "%s[%s]<%c>%s %s[%d]: ",
                log_col_def(s),
                ulog_format_time(log_time, 24), serverity_name[s],  rpt_mod,
                trim_file_path(f),
                (int)l);
#else /* !SYNC_LOG_DETAILS */
            snprintf(&ulog_buf[ULOG_HEADER_TYPE_LEN], ULOG_SIZE-ULOG_HEADER_TYPE_LEN, "[%s]<%c>%s ",
                ulog_format_time(log_time, 24), serverity_name[s],  rpt_mod);
#endif

            va_list args;
            va_start(args, fmt);
            rc = vsnprintf(&ulog_buf[strlen(ulog_buf)], ULOG_SIZE-strlen(ulog_buf), fmt, args);
            va_end(args);
            rc = rc<ULOG_SIZE-1?rc:ULOG_SIZE-1;

            bool skip_session_std = false;
#if !LOG_DIR_ASYNC
            if(s < stop_filter_level[ulog_session_std]){
                skip_session_std = true;
                puts(&ulog_buf[LOG_PREFIX_LEN]);
            }
#endif
            if(!skip_session_std || (s<get_lowest_level(ulog_session_std))){
                uring_fifo_push_s(ulog_buf, strlen(ulog_buf)+1);
            }

#else /* !ULOG_CONFIG_ASYNC */

#if SYNC_LOG_DETAILS
            printf("%s[%s]<%c>%s %s[%d]: ",
                log_col_def(s),
                ulog_format_time(log_time, 24), serverity_name[s],  rpt_mod,
                trim_file_path(f),
                (int)l);
#else /* !SYNC_LOG_DETAILS */
            printf("[%s]<%c>%s ",
                ulog_format_time(log_time, 24), serverity_name[s],  rpt_mod);

#endif /* SYNC_LOG_DETAILS */

            va_list args;
            va_start(args, fmt);
            rc = vprintf(fmt, args);
            va_end(args);
            fflush(stdout);
            printf("\r\n");
#endif /* if def ULOG_CONFIG_ASYNC */
            log_release_mutex();
        }
    }
    return rc;
}

void on_sync_filter_level_change(const ulog_session_type_t session, const char level)
{
    if ('N' == level) {
        on_filter_level_changes(session, LOG_EMERG);
    } else {
        int8_t i = 0;
        for (; i < LOG_NONE; i++) {
            if (serverity_name[i] == level) {
                on_filter_level_changes(session, i+1);
                break;
            }
        }
    }
}

void on_filter_level_changes(const ulog_session_type_t session, const uint8_t level)
{
    bool next_handle = true;
    if (session == ulog_session_size) {/* only happen on init, get the initial push level */
        push_stop_filter_level = LOG_EMERG;
    } else if (session < ulog_session_size && level <= LOG_NONE) {
        stop_filter_level[session] = level;

        /* suppose we use update session sf(stop filter) as push filter, this value will be updated below */
        push_stop_filter_level = stop_filter_level[session];
    } else {
        next_handle = false;
    }
    if (next_handle) {
        uint8_t i = 0;
        for (; i < ulog_session_size; i++)
        {
            if (push_stop_filter_level < stop_filter_level[i]) {
                push_stop_filter_level = stop_filter_level[i];
            }
        }
    }
}

#ifdef ULOG_CONFIG_ASYNC
static uint8_t get_lowest_level(const ulog_session_type_t start)
{
    uint8_t i = start+1;
    uint8_t lowest_level = LOG_EMERG;
    for(; i<ulog_session_size; i++){
        if(lowest_level<stop_filter_level[i]){
            lowest_level = stop_filter_level[i];
        }
    }
    return lowest_level;
}
#endif

void on_filter_change(const ulog_session_type_t session, const char level)
{
    if(session<ulog_session_size && level<=LOG_NONE){
        on_filter_level_changes(session, level);
    }
}


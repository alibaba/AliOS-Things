/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "k_api.h"

#include "ulog/ulog.h"
#include "aos/kernel.h"
#include "ulog_api.h"
#include "ulog_config_file.h"
#include "uagent/uagent.h"
#include "ulog_ring_fifo.h"
#include "errno.h"
#include "utask.h"

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

#if ULOG_ENCODE_FS

#include "debug_api.h"

typedef struct {
    uint8_t  log_len;
    uint8_t  level;
    uint32_t time_s;
    uint32_t str_ptr;
    uint32_t usr_list[ULOG_ENCODE_USR_VAR_LEN]; /* usr para */
} __attribute__((packed)) ulog_encode_fs_t;

static ulog_encode_fs_t one_encode_log;

#endif

/*extern kstat_t krhino_task_name_get(ktask_t *task, const char **name);*/

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

int aos_set_log_level_fs(aos_log_level_t log_level)
{
    int rc = -1;
#if ULOG_POP_FS_ENABLE
    if(log_init){
        if (log_level <= AOS_LL_DEBUG) {
            on_filter_level_changes(ulog_session_file, log_level + 1);
            rc = 0;
        }
    }
#else /* !ULOG_POP_FS_ENABLE */
    ULOG_TRACE_ERR("set log level fail as fs log is disable");
#endif /* ULOG_POP_FS_ENABLE */
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

int do_ulog(const unsigned char s, const char *mod, const char *f, const unsigned long l, const char *fmt, va_list args)
{
    int rc  = -1;
    int ret = -1;

    const char *task_name;
    ktask_t    *task;

    if (log_init && (s < push_stop_filter_level)) {
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

            task = krhino_cur_task_get();
            if (task) {
#ifdef AOS_COMPILE_APP /* get task_name by svccall*/
                krhino_task_name_get(task, &task_name);
#else
                task_name = task->task_name;
#endif
                if (task_name) {
                    snprintf(&ulog_buf[ULOG_HEADER_TYPE_LEN], ULOG_SIZE-ULOG_HEADER_TYPE_LEN, "[%s]<%c>%s<%s> ",
                            ulog_format_time(log_time, 24), serverity_name[s], rpt_mod, task_name);
                } else {
                    snprintf(&ulog_buf[ULOG_HEADER_TYPE_LEN], ULOG_SIZE-ULOG_HEADER_TYPE_LEN, "[%s]<%c>%s ",
                            ulog_format_time(log_time, 24), serverity_name[s],  rpt_mod);
                }
            }
#endif
            rc = vsnprintf(&ulog_buf[strlen(ulog_buf)], ULOG_SIZE-strlen(ulog_buf), fmt, args);

#if ULOG_ENCODE_FS
            one_encode_log.time_s = (uint32_t)(aos_calendar_localtime_get() >> ULOG_ENCODE_TIME_OFFSET_BIT);

            /* now user-define string*/
            rc = print_driver(fmt, args, (unsigned int *)one_encode_log.usr_list);

            /* save : usr str */
            one_encode_log.str_ptr = (uint32_t)fmt;

            /* level: E/D/A.. + log len*/
            one_encode_log.level   = serverity_name[s];
            one_encode_log.log_len = rc * sizeof(uint32_t) + ULOG_ENCODE_HEADER_LEN_BYTES;

            //printf("uel:%d\n", one_encode_log.log_len);

            rc = one_encode_log.log_len;
            if ((rc > ULOG_ENCODE_MAX_LEN) ||
                (rc > (ULOG_SIZE - 1 - strlen(ulog_buf)))) {
                printf("uef!!! %d %d\n", rc, (ULOG_SIZE-1-strlen(ulog_buf)));
                rc = 0; /* over-long log or buf is almost full, encode to fs is discarded*/
            }
#endif
            rc = (rc < (ULOG_SIZE - 1 - strlen(ulog_buf)))? rc: (ULOG_SIZE - 1 - strlen(ulog_buf));

            bool skip_session_std = false;
#if !LOG_DIR_ASYNC
            if(s < stop_filter_level[ulog_session_std]){
                skip_session_std = true;
                puts(&ulog_buf[LOG_PREFIX_LEN]);
            }
#endif
            if(!skip_session_std || (s<get_lowest_level(ulog_session_std))){
#if ULOG_ENCODE_FS
                memcpy(&ulog_buf[strlen(ulog_buf) + 1], (char *)&one_encode_log, rc);
                ret = uring_fifo_push_s(ulog_buf, strlen(ulog_buf) + 1 + rc);
#else
                ret = uring_fifo_push_s(ulog_buf, strlen(ulog_buf)+1);
#endif
                if (ret == RHINO_BUF_QUEUE_FULL) {
                    printf("log miss\n");
                }
            }

#else /* !ULOG_CONFIG_ASYNC */

#if SYNC_LOG_DETAILS
            printf("%s[%s]<%c>%s %s[%d]: ",
                    log_col_def(s),
                    ulog_format_time(log_time, 24), serverity_name[s],  rpt_mod,
                    trim_file_path(f),
                    (int)l);
#else /* !SYNC_LOG_DETAILS */
            task = krhino_cur_task_get();
            if (task) {
#ifdef AOS_COMPILE_APP /* get task_name by svccall*/
                krhino_task_name_get(task, &task_name);
#else
                task_name = task->task_name;
#endif

#ifdef ULOG_CONFIG_ASYNC
                if (task_name) {
                    snprintf(&ulog_buf[ULOG_HEADER_TYPE_LEN], ULOG_SIZE-ULOG_HEADER_TYPE_LEN, "[%s]<%c>%s<%s> ",
                            ulog_format_time(log_time, 24), serverity_name[s],  rpt_mod, task->task_name);
                } else {
                    snprintf(&ulog_buf[ULOG_HEADER_TYPE_LEN], ULOG_SIZE-ULOG_HEADER_TYPE_LEN, "[%s]<%c>%s ",
                            ulog_format_time(log_time, 24), serverity_name[s],  rpt_mod);
                }
#endif
            }
#endif /* SYNC_LOG_DETAILS */

            rc = vprintf(fmt, args);
            fflush(stdout);
            printf("\r\n");
#endif /* if def ULOG_CONFIG_ASYNC */
            log_release_mutex();
        }
    }
    return rc;
}

int ulog(const unsigned char s, const char *mod, const char *f, const unsigned long l, const char *fmt, ...)
{
    int rc = -1;
    va_list args;
    va_start(args, fmt);
    rc = do_ulog(s, mod, f, l, fmt, args);
    va_end(args);
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
#if ULOG_POP_FS_ENABLE
        uint8_t level_from_cfg_file = 0xFF;
#endif
        push_stop_filter_level = LOG_EMERG;
#if ULOG_POP_FS_ENABLE
        if( (level_from_cfg_file = get_ulog_level_from_cfg(ulog_session_std))<=LOG_NONE ) {
            stop_filter_level[ulog_session_std] = level_from_cfg_file;
        }
        if( (level_from_cfg_file = get_ulog_level_from_cfg(ulog_session_file))<=LOG_NONE ) {
            stop_filter_level[ulog_session_file] = level_from_cfg_file;
        }
    } else if (session < ulog_session_size && level <= LOG_NONE) {
        stop_filter_level[session] = level;
        set_ulog_level_to_cfg(session, level);

        /* suppose we use update session sf(stop filter) as push filter, this value will be updated below */
        push_stop_filter_level = stop_filter_level[session];
#else
    } else if (session < ulog_session_size && level <= LOG_NONE) {
        stop_filter_level[session] = level;

        /* suppose we use update session sf(stop filter) as push filter, this value will be updated below */
        push_stop_filter_level = stop_filter_level[session];

#endif
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


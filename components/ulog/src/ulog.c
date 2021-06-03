/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "ulog/ulog.h"
#include "aos/kernel.h"
#include "ulog_api.h"
#include "ulog_ring_fifo.h"
#include "aos/errno.h"
static char serverity_name[LOG_NONE] = { 'V', 'A', 'F', 'E', 'W', 'T', 'I', 'D' };

#define UNKNOWN_BUF ""

void (*aos_cust_output_func)(const char *fmt, ...) = NULL;

int ulog_vprintf(const char *vformat, va_list varg)
{
    int i=0;
    int out_int;
    int *out_n;
    unsigned int out_hex_int;
    unsigned int out_unsigned_int;
    char out_char=NULL;
    char *out_string=NULL;
    char int_to_string[20];
    double out_f;
    double out_e;
    double out_a;
    unsigned int out_o;
    unsigned int out_p;
    int k=0;
    char number_array[6];
    int count=0;

    for (i = 0; vformat[i] != '\0';) {
        if (vformat[i] == '%') {
            i = i + 1;
            while (vformat[i] == '0'|| vformat[i] == '1'|| vformat[i] == '2'|| vformat[i] == '3'
                    || vformat[i] == '4' || vformat[i] == '5' || vformat[i] == '6' ||vformat[i] == '7'
                    || vformat[i] == '8' || vformat[i] == '9' || vformat[i] == '$' ||vformat[i] == '*'
                    || vformat[i] == '.') {
                if (vformat[i] == '%') {
                    break;
                } else {
                    number_array[k++] = vformat[i];
                    i = i + 1;
                }
            }
            //number=findNumber(number_array);
            //
            if (vformat[i] == 'd') {
                out_int = va_arg(varg, int);
                sprintf(int_to_string, "%d", out_int);
                count = count + strlen(int_to_string);
                aos_cust_output_func("%d",out_int);
                i = i + 1;
            } else if (vformat[i] == 'i') {
                out_int = va_arg(varg, int);
                sprintf(int_to_string, "%i", out_int);
                count = count + strlen(int_to_string);
                aos_cust_output_func("%i", out_int);
                i = i + 1;
            } else if (vformat[i] == 's') {
                out_string = va_arg(varg, char*);
                count = count+strlen(out_string);
                aos_cust_output_func("%s", out_string);
                i = i + 1;
            } else if (vformat[i] == 'c') {
                out_char = va_arg(varg, int);
                count++;
                aos_cust_output_func("%c", out_char);
                i = i + 1;
            } else if (vformat[i] == 'x') {
                out_hex_int = va_arg(varg, int);
                sprintf(int_to_string, "%x", out_hex_int);
                count = count + strlen(int_to_string);
                aos_cust_output_func("%x", out_hex_int);
                i = i + 1;
            } else if (vformat[i] == 'X') {
                out_hex_int = va_arg(varg, int);
                sprintf(int_to_string, "%X", out_hex_int);
                count= count + strlen(int_to_string);
                aos_cust_output_func("%X", out_hex_int);
                i = i + 1;
            } else if (vformat[i]=='f') {
                out_f = va_arg(varg, double);
                sprintf(int_to_string, "%f", out_f);
                count = count + strlen(int_to_string);
                aos_cust_output_func("%f", out_f);
                i = i + 1;
            } else if (vformat[i] == 'u') {
                out_unsigned_int = va_arg(varg, int);
                sprintf(int_to_string, "%u", out_unsigned_int);
                count = count + strlen(int_to_string);
                aos_cust_output_func("%u", out_unsigned_int);
                i=i+1;         
            } else if (vformat[i]=='h') {
                if (vformat[i+1] == 'n') {
                    out_n = (int *)va_arg(varg, int*);
                    //*out_n=count;
                    i = i + 2;
                } else {
                    count++;
                    aos_cust_output_func("%c", vformat[i]);
                    i = i + 1;
                }
            } else if (vformat[i]=='e') {
                out_e = va_arg(varg, double);
                sprintf(int_to_string, "%e", out_e);
                count = count + strlen(int_to_string);
                aos_cust_output_func("%e", out_e);
                i = i + 1;
            } else if (vformat[i] == 'o') {
                out_o = va_arg(varg, unsigned int);
                sprintf(int_to_string, "%o", out_o);
                count = count + strlen(int_to_string);
                aos_cust_output_func("%o", out_o);
                i = i + 1;
            } else if (vformat[i]=='p') {
                out_p = va_arg(varg, unsigned int);
                sprintf(int_to_string, "%p", out_p);
                count = count + strlen(int_to_string);
                aos_cust_output_func("%p", out_p);
                i = i + 1;
            } else if (vformat[i] == 'a') {
                out_a = va_arg(varg,double);
                sprintf(int_to_string, "%a", out_a);
                count = count + strlen(int_to_string);
                aos_cust_output_func("%a", out_a);
                i = i + 1;
            } else if (vformat[i] == 'n') {
                out_n=(int *)va_arg(varg,int*);
                //*out_n=count;                    //uncomment this for test1 and test2
                i=i+1;
            } else if (vformat[i] == '%') {
                count++;
                aos_cust_output_func("%c", vformat[i]);
                i = i + 1;
            } else {
                count++;
                i++;
            }
        } else {
            count++;
            aos_cust_output_func("%c", vformat[i]);
            i++;
        }
    }

    return 0;

}

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


static char ulog_buf[ULOG_SIZE + 1];

/*check whether session can pop log*/
bool check_pass_pop_out(const ulog_session_type_t session, const uint8_t level)
{
    return (stop_filter_level[session]>level);
}

/*获取日志停止输出的level*/
uint8_t ulog_stop_filter_level(const ulog_session_type_t session)
{
    return stop_filter_level[session];
}

/*设置本地终端日志输出level*/
int aos_set_log_level(aos_log_level_t log_level)
{
    int rc = -EINVAL;

    if(aos_ulog_init) {
        if (log_level <= AOS_LL_DEBUG) {
            on_filter_level_changes(ulog_session_std, log_level + 1);
            rc = 0;
        }
    }
    return rc;
}

/*设置输出到云端日志level*/
#if ULOG_POP_CLOUD_ENABLE
int aos_set_popcloud_log_level(aos_log_level_t log_level)
{
    int rc = -EINVAL;

    if (aos_ulog_init) {
        if (log_level <= AOS_LL_DEBUG) {
            on_filter_level_changes(ulog_session_cloud, log_level + 1);
            rc = 0;
        }
    }
    return rc;
}
#endif

/*设置输出到本地文件系统日志level*/
#if ULOG_POP_FS_ENABLE
int aos_set_popfs_log_level(aos_log_level_t log_level)
{
    int rc = -EINVAL;

    if (aos_ulog_init) {
        if (log_level <= AOS_LL_DEBUG) {
            on_filter_level_changes(ulog_session_file, log_level + 1);
            rc = 0;
        }
    }
    return rc;
}
#endif

/*设置输出到syslog 服务器到日志等级*/
#if ULOG_POP_UDP_ENABLE
int aos_set_popudp_log_level(aos_log_level_t log_level)
{
    int rc = -EINVAL;

    if (aos_ulog_init) {
        if (log_level <= AOS_LL_DEBUG) {
            on_filter_level_changes(ulog_session_udp, log_level + 1);
            rc = 0;
        }
    }
    return rc;
}
#endif

int aos_log_hexdump(const char *tag, char *buffer, int len)
{
    int i;

    if (aos_ulog_init == false) {
        return -1;
    }

    aos_cust_output_func("[%s]\n", tag);
    aos_cust_output_func("0x0000: ");
    for (i = 0; i < len; i++) {
        aos_cust_output_func("0x%02x ", buffer[i]);

        if (i % 8 == 7) {
            aos_cust_output_func("\n");
            aos_cust_output_func("0x%04x: ", i + 1);
        }
    }

    aos_cust_output_func("\n");
    return 0;
}

int aos_set_log_output(void (*output_func)(const char* fmt, ...))
{
    if (NULL == output_func) {
        return -1;
    }

    aos_cust_output_func = output_func;
    return 0;
}

char* get_sync_stop_level()
{
    static char buf[32];
#ifdef CONFIG_LOGMACRO_SILENT
    return "NDEBUG mode active";
#else
    if (stop_filter_level[ulog_session_std] == LOG_NONE) {
        return "all log recorded";
    } else if (stop_filter_level[ulog_session_std] == LOG_EMERG) {
        return "all log stop";
    } else {
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

static char log_col_list[LOG_NONE][12] = {
    COL_DEF, COL_RED, COL_RED, COL_RED, COL_BLU, COL_GRE, COL_GRE, COL_WHE
};
static char* log_col_def(const unsigned char level)
{
    if(level<LOG_NONE) {
        return log_col_list[level];
    } else {
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
    if (aos_ulog_init &&
        (s < push_stop_filter_level) ) {
        char log_time[24];
        if (log_get_mutex()) {
            const char* rpt_mod = NULL;
            if ((mod == NULL) || (0 == strlen(mod))) {
                rpt_mod = UNKNOWN_BUF;
            } else {
                rpt_mod = mod;
            }
#if ULOG_CONFIG_ASYNC
            uint8_t facility = FACILITY_NORMAL_LOG;
            if (strlen(rpt_mod) == 0 || 0 == strncmp("MQTT", rpt_mod, strlen("MQTT"))) {
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
            if(s < stop_filter_level[ulog_session_std]) {
                skip_session_std = true;
                puts(&ulog_buf[LOG_PREFIX_LEN]);
            }
#endif
            if(!skip_session_std || (s<get_lowest_level(ulog_session_std))) {
                uring_fifo_push_s(ulog_buf, strlen(ulog_buf)+1);
            }

#else /* !ULOG_CONFIG_ASYNC */

#if SYNC_LOG_DETAILS
            aos_cust_output_func("%s[%s]<%c>%s %s[%d]: ",
                log_col_def(s),
                ulog_format_time(log_time, 24), serverity_name[s],  rpt_mod,
                trim_file_path(f),
                (int)l);
#else /* !SYNC_LOG_DETAILS */
            rc = snprintf(ulog_buf, ULOG_SIZE,
                            "[%s]<%c>%s ", ulog_format_time(log_time, 24), serverity_name[s],  rpt_mod);

#endif /* SYNC_LOG_DETAILS */

            va_list args;
            va_start(args, fmt);
            rc = vsnprintf(&ulog_buf[strlen(ulog_buf)], ULOG_SIZE-strlen(ulog_buf), fmt, args);
            va_end(args);
            fflush(stdout);
            aos_cust_output_func(ulog_buf);

#endif /* if def ULOG_CONFIG_ASYNC */
            log_release_mutex();
        }
    }
    return rc;
}

/*用于loglevel 命令行处理*/
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
    uint8_t i = 0;

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
        /*遍历所有终端的 filter level , push pop filter 选择最低的一个，保证每一条日志都能吐出来*/
        for (; i < ulog_session_size; i++) {
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
    for(; i<ulog_session_size; i++) {
        if(lowest_level<stop_filter_level[i]) {
            lowest_level = stop_filter_level[i];
        }
    }
    return lowest_level;
}
#endif

void on_filter_change(const ulog_session_type_t session, const char level)
{
    if(session<ulog_session_size && level<=LOG_NONE) {
        on_filter_level_changes(session, level);
    }
}


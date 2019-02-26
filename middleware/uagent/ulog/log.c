/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "ulog/ulog.h"
#include "aos/kernel.h"
#include "ulog_api.h"

static char serverity_name[LOG_NONE] = { 'V', 'A', 'F', 'E', 'W', 'T', 'I', 'D' };

const char UNKNOWN_BUF[2] = "";

/* stop filter used in sync log, dedault value LOG_NONE, shall not larger than LOG_NONE */
static uint8_t stop_filter_level = LOG_NONE;

static aos_mutex_t log_mutex;


int log_get_mutex()
{
    return 0 == aos_mutex_lock(&log_mutex, AOS_WAIT_FOREVER);
}

void log_release_mutex()
{
    aos_mutex_unlock(&log_mutex);
}

void aos_set_log_level(aos_log_level_t log_level)
{
    if (log_level <= AOS_LL_DEBUG) {
        stop_filter_level = log_level + 1;
    }
}

char get_sync_stop_level()
{
    if (stop_filter_level == LOG_NONE) {
        return 'N';
    } else {
        return serverity_name[stop_filter_level];
    }
}

/* log init mutex */
void log_init_mutex(void)
{
    aos_mutex_new(&log_mutex);
}

#if SYNC_LOG_DETAILS
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

static char log_col_def[LOG_NONE][12] =
{
    COL_DEF, COL_RED, COL_RED, COL_RED, COL_BLU, COL_GRE, COL_GRE, COL_WHE
};

#endif

int rt_log(const unsigned char s, const char* mod, const char* f, const unsigned long l, const char *fmt, ...)
{
    int rc = -1;
    if (log_init && s < stop_filter_level) {
        va_list args;
        long long ms = aos_now_ms();
        if (log_get_mutex()) {
            const char* rpt_mod = NULL;
            char before_mod[2];
            if ((mod == NULL) || (0 == strlen(mod))) {
                before_mod[0] = 0;
                rpt_mod = UNKNOWN_BUF;
            } else {
                before_mod[0] =' ';
                before_mod[1] = 0;
                rpt_mod = mod;
            }
#if SYNC_DETAIL_COLOR
            printf("%s [%4d.%03d]<%c>%s%s [%s#%d] : ",
                log_col_def[s],
#else
            printf("[%4d.%03d]<%c>%s%s [%s#%d] : ",
#endif
                (int)(ms / 1000),
                (int)(ms % 1000), serverity_name[s], before_mod, rpt_mod, f, (int)l);
            va_start(args, fmt);
            rc = vprintf(fmt, args);
            va_end(args);

            fflush(stdout);
            printf("\r\n");
            log_release_mutex();
        }
    }
    return rc;
}

#else
#if SYNC_LOG_MOD
int rt_log(const unsigned char s, const char* mod, const char *fmt, ...)
#else
int rt_log(const unsigned char s, const char *fmt, ...)
#endif
{
    int rc = -1;
    if (log_init && s < stop_filter_level) {

        if (log_get_mutex()) {

            va_list args;

            printf("[%06d]<%c> ", (int)(aos_now_ms() % 1000000), serverity_name[s]);

#if SYNC_LOG_MOD
            uint16_t mod_len = 0;
            if ((mod == NULL) || (0 == (mod_len = strlen(mod)))) {
                /* not record any mode name */
            } else if (mod_len <= MOD_MAX_LEN) {
                printf("%s ", mod);
            } else {
                char mod_buf[MOD_MAX_LEN + 2];
                strncpy(mod_buf, mod, 10);
                mod_buf[MOD_MAX_LEN] = ' ';
                mod_buf[MOD_MAX_LEN + 1] = '\0';
                printf("%s", mod_buf);
            }
#endif
            va_start(args, fmt);
            rc = vprintf(fmt, args);
            va_end(args);

            fflush(stdout);
            printf("\r\n");
            log_release_mutex();
        }
    }
    return rc;
}

#endif

void on_sync_filter_level_change(const char level)
{
    if ('N' == level) {
        stop_filter_level = LOG_NONE;
    } else {
        int8_t i = 0;
        for (; i < LOG_NONE; i++) {
            if (serverity_name[i] == level) {
                stop_filter_level = i;
                break;
            }
        }
    }
}

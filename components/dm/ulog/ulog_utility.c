/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include "ulog/ulog.h"
#include "ulog_api.h"
#include "aos/kernel.h"
#include "aos/errno.h"
#include "ulog_ring_fifo.h"

#if SYSLOG_TIME_FORMAT
static const char months[][4] = { "Jan", "Feb", "Mar", "Apr", "May",
                       "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
#endif

static aos_mutex_t log_mutex;

/* log init mutex */
void log_init_mutex(void)
{
    aos_mutex_new(&log_mutex);
}

bool log_get_mutex()
{
    return 0 == aos_mutex_lock(&log_mutex, AOS_WAIT_FOREVER);
}

void log_release_mutex()
{
    aos_mutex_unlock(&log_mutex);
}

char* replace_char(char *str, char find, char replace)
{
    if (NULL != str) {
        char *current_pos = strchr(str, find);
        while (NULL != current_pos) {
            *current_pos = replace;
            current_pos = strchr(current_pos, find);
        }
    }
    return str;
}

/* result like 99.356 ,i.e. s.ms */
/* Result is like "Nov 28 15:19:20.122" */
char *ulog_format_time(char *buffer, const int len)
{
    if(NULL!=buffer && len>4){
        long long ms = aos_now_ms();
#if SYSLOG_TIME_FORMAT
        time_t rawtime;
        time(&rawtime);
        struct tm *tm = localtime(&rawtime);
        /* %b format of strftime() is platform-dependent, so we realized it by-self */
        snprintf(buffer, len, "%s ", months[tm->tm_mon < 12 ? tm->tm_mon : 0]);
        strftime(&buffer[4], len - 4, "%d %H:%M:%S", tm);
        const int32_t milli = ms % 1000;
        char      ms_str[8] = "";
        memset(ms_str, 0, sizeof(ms_str));
        snprintf(ms_str, sizeof(ms_str), ".%03d", milli);
        strncat(buffer, ms_str, len - strlen(buffer) - 1);
#else /* !SYSLOG_TIME_FORMAT */
        snprintf(buffer, len, "%4d.%03d", (int)(ms / 1000), (int)(ms % 1000));
#endif /* SYSLOG_TIME_FORMAT */
    }
    return buffer;
}

char* trim_file_path(const char* path)
{
#if SYNC_ABS_PATH
    return path;
#else
    char* filename = (char*)path;
    if (path != NULL) {
        if ((filename = strrchr(path, '/')) != NULL) {
            filename++;
        }
    }

    if (filename != NULL) {
        const size_t filename_len = strlen(filename);
        if (filename_len > 0) {
            if (filename_len > TAG_FILE_NAME_MAX_LEN) { /* trig it using last 32 string */
                filename += filename_len - TAG_FILE_NAME_MAX_LEN;
            }
        } else { /* cannot get file name if input path is like "/d/timer/" */
            filename = UNDEFINE_FILE_NAME;
        }
    } else { /* using UNDEFINE_FILE_NAME */
        filename = UNDEFINE_FILE_NAME;
    }

    return filename;
#endif
}

int ulog_man(const char* cmd_str)
{
#if ULOG_CONFIG_ASYNC
    int rc = -EINVAL;
    if (NULL != cmd_str && strlen(cmd_str) > 0) {
        rc = -EPERM;
        if (log_init) {
            const uint16_t cmd_str_size = strlen(cmd_str) + strlen(ULOG_CMD_PREFIX) + 1;
            char* tmpbuf = (char*)aos_malloc(cmd_str_size);
            strncpy(tmpbuf, ULOG_CMD_PREFIX, cmd_str_size);
            strncat(tmpbuf, cmd_str, cmd_str_size - strlen(ULOG_CMD_PREFIX) - 1);
            rc = uring_fifo_push_s(tmpbuf, cmd_str_size);
            aos_free(tmpbuf);
        }
    }
#else /*!ULOG_CONFIG_ASYNC */
    int rc = -EACCES;
#endif
    return rc;
}


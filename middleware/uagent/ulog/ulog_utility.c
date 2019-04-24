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

static const char months[][4] = { "Jan", "Feb", "Mar", "Apr", "May",
                       "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

/* Result is like "Nov 28 15:19:20.122 " */
char *syslog_format_time(char *buffer, const int len)
{
    const long long ms = aos_now_ms();
    if (buffer != NULL && len > 4) {
        time_t rawtime;
        time(&rawtime);
        struct tm *tm = localtime(&rawtime);
        /* %b format of strftime() is platform-dependent, so we realized it by-self */
        snprintf(buffer, len, "%s ", months[tm->tm_mon < 12 ? tm->tm_mon : 0]);
        strftime(&buffer[4], len - 4, "%d %H:%M:%S", tm);
        const int32_t milli = ms % 1000;
        char      ms_str[8] = "";
        memset(ms_str, 0, sizeof(ms_str));
        snprintf(ms_str, sizeof(ms_str), ".%03d ", milli);
        strncat(buffer, ms_str, len - strlen(buffer) - 1);
    }

    return buffer;
}

char* trim_file_path(const char* path)
{
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
}


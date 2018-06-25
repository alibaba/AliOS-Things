//
//  log.c
//  node_thing
//
//  Created by howie on 14/12/2017.
//  Copyright © 2017 alibaba. All rights reserved.
//

#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include "be_log.h"
#include "be_utils.h"

static be_log_callback log  = 0;

void be_log(const char* file, const char* tag, int level, int line, const char* func,
                const char* format, ...)
{
    if (log) {
        char message[BE_LOG_MAX_MESSAGE] = {0};
        va_list args;
        va_start(args, format);
        vsnprintf(message, sizeof(message), format, args);
        va_end(args);
        
        log(file, tag, level, line, func, message);
    }
}

void be_log_build(const char* file, const char* tag, int level, int line, const char* func,
                      const char* message, char* msg, size_t msg_len)
{
    char time_str[64] = {0};
    be_utils_timestr(time_str, sizeof(time_str));

    const char* msg_format = "%s %-5s [%-8s%3d] %s";
    
    switch (level) {
        case BE_LOG_LEVEL_DEBUG:
            sprintf(msg, msg_format, time_str, "DEBUG", tag, line, message);
            break;
            
        case BE_LOG_LEVEL_INFO:
            sprintf(msg, msg_format, time_str, "INFO ", tag, line, message);
            break;
            
        case BE_LOG_LEVEL_WARN:
            sprintf(msg, msg_format, time_str, "WARN ", tag, line, message);
            break;
            
        case BE_LOG_LEVEL_ERROR:
            sprintf(msg, msg_format, time_str, "ERROR", tag, line, message);
            break;
            
        default:
            break;
    }
}

void be_set_log(be_log_callback cb)
{
    log = cb;
}

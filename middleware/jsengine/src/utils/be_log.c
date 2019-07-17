/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "be_log.h"
#include <stdarg.h>
#include <stdio.h>
#include "be_utils.h"

static be_log_callback log = NULL;

void be_log(const char *tag, int level, const char *format, ...) {
    if (log) {
        char message[BE_LOG_MAX_MESSAGE];
        int len;

        va_list args;
        va_start(args, format);
        len = vsnprintf(message, BE_LOG_MAX_MESSAGE - 1, format, args);
        va_end(args);

        if (len != -1) {
            log(tag, level, message, len);
        }
    }
}

void be_set_log(be_log_callback cb) { log = cb; }

#include "aos_log.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "ulog/ulog.h"
#include "aos/kernel.h"
#include "aos/errno.h"

static int local_log_level = AOS_LOG_VERBOSE;

int  aos_log(const unsigned char s, const char *tag, const char *fmt, ...)
{
    if (s > local_log_level) {
        return 0;
    }

    va_list list;
    va_start(list, fmt);
    ulog(s, "AOS", tag, 0, fmt, list);
    va_end(list);

    return 0;
}

int  aos_log_set_level(haas_log_level_t log_level)
{
    local_log_level = log_level;

    aos_set_log_level(log_level);
}

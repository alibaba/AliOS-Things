#include "aos_log.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "esp_log.h"

static int local_log_level = AOS_LOG_VERBOSE;

int  aos_log(const unsigned char s, const char *tag, const char *fmt, ...)
{
    if (s > local_log_level) {
        return 0;
    }

    int log_level;
    switch (s) {
    case AOS_LOG_VERBOSE:
        log_level = ESP_LOG_VERBOSE;
        break;
    case AOS_LOG_DEBUG:
        log_level = ESP_LOG_DEBUG;
        break;
    case AOS_LOG_INFO:
        log_level = ESP_LOG_INFO;
        break;
    case AOS_LOG_WARN:
        log_level = ESP_LOG_WARN;
        break;
    case AOS_LOG_ERROR:
        log_level = ESP_LOG_ERROR;
        break;
    case AOS_LOG_FATAL:
        log_level = ESP_LOG_ERROR;
        break;
    default:
        log_level = ESP_LOG_VERBOSE;
        break;
    }

    va_list list;
    va_start(list, fmt);
    esp_log_writev(log_level, tag, fmt, list);
    va_end(list);

    return 0;
}

int  aos_log_set_level(haas_log_level_t log_level)
{
    local_log_level = log_level;
}

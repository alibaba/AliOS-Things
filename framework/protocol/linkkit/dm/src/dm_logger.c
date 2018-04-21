#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "class_interface.h"
#include "interface/log_abstract.h"
#include "dm_logger.h"
#include "dm_import.h"

#include "lite-log.h"
#include "iot_export.h"

void* _g_default_logger = NULL;

static const char string_log_class_name[] __DM_READ_ONLY__ = "log_cls";
static const char string_log_level_invalid_pattern[] __DM_READ_ONLY__ = "invalid input level: %d out of [%d, %d]";

static void dm_logger_close(const void* _self);
static void dm_logger_open(void* _self, void* _log_name);

static void* dm_logger_ctor(void* _self, va_list* params)
{
    dm_logger_t* self = _self;
    int buffer_size = 0;

    self->_log_name = va_arg(*params, void*);
    buffer_size = va_arg(*params, int);

    if (self->_log_name) dm_logger_open(self, self->_log_name);

    self->_log_level = log_level_debug; /* debug level as default. */

    if (buffer_size) {
        self->_log_buffer = dm_lite_calloc(1, buffer_size);
    } else {
        self->_log_buffer = NULL;
    }
    /* first created logger consider as the default logger. */
    if (_g_default_logger == NULL) _g_default_logger = self;

    return self;
}

static void* dm_logger_dtor(void* _self)
{
    dm_logger_t* self = _self;

    self->_log_level = log_level_debug;
    self->_log_name = NULL;

    if (self->_log_buffer) dm_lite_free(self->_log_buffer);
    self->_log_buffer = NULL;

    IOT_DumpMemoryStats(IOT_LOG_DEBUG);

    dm_logger_close(self);
    _g_default_logger = NULL;

    return self;
}

static void dm_logger_open(void* _self, void* _log_name)
{
    dm_logger_t* self = _self;

    self->_log_name = _log_name;

    LITE_openlog(_log_name);
}

static void dm_logger_close(const void* _self)
{
    const dm_logger_t* self = _self;

    self = self;

    LITE_closelog();

    return;
}

static void  dm_logger_set_log_level(void* _self, log_level_t _log_level)
{
    dm_logger_t* self = _self;

    self->_log_level = _log_level;

    if (self->_log_level > log_level_debug) {
        printf(string_log_level_invalid_pattern,
               _log_level,
               log_level_emerg,
               log_level_debug);
        return;
    }

    LITE_set_loglevel(self->_log_level);
}

static log_level_t  dm_logger_get_log_level(const void* _self)
{
    const dm_logger_t* self = _self;

    return self->_log_level;
}

static void*  dm_logger_get_log_name(const void* _self)
{
    const dm_logger_t* self = _self;

    return self->_log_name;
}
#ifdef DM_LOG_ENABLE
static char* _log_level_names[] = {
    "emg", "crt", "err", "wrn", "inf", "dbg",
};
#endif
static void dm_logger_print_log(const void* _self, const char* _func, const int _line,
                                const int _log_level, const char* fmt, va_list* params)
{
    const dm_logger_t* self = _self;
    if (self->_log_level < _log_level) return;

#ifdef DM_LOG_ENABLE
    /* temp implementation of log system. */
    fprintf(stdout, "[%s] %s(%d) ", _log_level_names[_log_level], _func, _line);
    vsprintf(self->_log_buffer, fmt, *params);
    fprintf(stdout, "%s", self->_log_buffer);
#else
    /* use LITE-log functions. */
    LITE_syslog_routine(_func, _line, _log_level, fmt, params);
#endif
    return;
}

static void dm_logger_log(const void* _self, const char* _func, const int _line,
                          log_level_t _log_level, const char* fmt, ...)
{
    const dm_logger_t* self = _self;
    va_list params;

    va_start(params, fmt);

    dm_logger_print_log(self, _func, _line, _log_level, fmt, &params);

    va_end(params);
    return;
}

static const log_t _dm_logger_class = {
    sizeof(dm_logger_t),
    string_log_class_name,
    dm_logger_ctor,
    dm_logger_dtor,
    dm_logger_open,
    dm_logger_close,
    dm_logger_set_log_level,
    dm_logger_get_log_level,
    dm_logger_get_log_name,
    dm_logger_print_log,
    dm_logger_log,
};

const void* get_dm_logger_class()
{
    return &_dm_logger_class;
}

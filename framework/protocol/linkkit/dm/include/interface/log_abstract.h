#ifndef LOG_ABSTRACT_H
#define LOG_ABSTRACT_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

typedef enum {
    log_level_emerg = 0,    /* emergency level */
    log_level_crit,         /* critical level */
    log_level_err,          /* error level */
    log_level_warning,      /* warning level */
    log_level_info,         /* information level */
    log_level_debug,        /* debug level */
} log_level_t;

typedef struct {
    size_t size;
    const char*  _class_name;
    void* (*ctor)(void* _self, va_list* params);
    void* (*dtor)(void* _self);
    void  (*open)(void* _self, void* _log_name);
    void  (*close)(const void* _self);
    void  (*set_log_level)(void* _self, log_level_t _log_level);
    log_level_t (*get_log_level)(const void* _self);
    void* (*get_log_name)(const void* _self);
    void  (*print_log)(const void* _self, const char* _func, const int _line, const int _log_level, const char* fmt, va_list* params);
    void  (*log)(const void* _self, const char* _func, const int _line, log_level_t _log_level, const char* fmt, ...);
} log_t;

#define dm_log_emerg(...)      (*(log_t**)_g_default_logger)->log(_g_default_logger, __FUNCTION__, __LINE__, log_level_emerg,   __VA_ARGS__)
#define dm_log_crit(...)       (*(log_t**)_g_default_logger)->log(_g_default_logger, __FUNCTION__, __LINE__, log_level_crit,    __VA_ARGS__)
#define dm_log_err(...)        (*(log_t**)_g_default_logger)->log(_g_default_logger, __FUNCTION__, __LINE__, log_level_err,     __VA_ARGS__)
#define dm_log_warning(...)    (*(log_t**)_g_default_logger)->log(_g_default_logger, __FUNCTION__, __LINE__, log_level_warning, __VA_ARGS__)
#define dm_log_info(...)       (*(log_t**)_g_default_logger)->log(_g_default_logger, __FUNCTION__, __LINE__, log_level_info,    __VA_ARGS__)
#define dm_log_debug(...)      (*(log_t**)_g_default_logger)->log(_g_default_logger, __FUNCTION__, __LINE__, log_level_debug,   __VA_ARGS__)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LOG_ABSTRACT_H */

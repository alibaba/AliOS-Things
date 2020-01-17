#ifndef _UMESH_LOG_H_
#define _UMESH_LOG_H_

#include <stdarg.h>
#include <ulog/ulog.h>

#define LOG_TAG "UMESH2"
#define log_d(...)          ulog(LOG_DEBUG, LOG_TAG, ULOG_TAG, __VA_ARGS__)
#define log_i(...)          ulog(LOG_INFO, LOG_TAG, ULOG_TAG, __VA_ARGS__)
#define log_w(...)          ulog(LOG_WARNING, LOG_TAG, ULOG_TAG, __VA_ARGS__)
#define log_e(...)          ulog(LOG_ERR, LOG_TAG, ULOG_TAG, __VA_ARGS__)

int log_hex(const char *tag, const uint8_t *data, uint16_t len);
#endif

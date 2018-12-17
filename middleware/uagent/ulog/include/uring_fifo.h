#ifndef U_RING_FIFO_H_
#define U_RING_FIFO_H_

#include "ulog_config.h"

#ifdef __cplusplus
extern "C" {
#endif

extern uint8_t push_stop_filter_level;
extern bool log_init;

//call back-function used to execute customer's action after push data
typedef void (*push_callback)(void*);

typedef void (*pop_callback)(void*, const void* src, const uint16_t len);

typedef struct{
    char*   _host_name;
    void*   (*_mutex_init)(void);
    int32_t (*_mutex_lock)(void *mutex);
    int32_t (*_mutex_unlock)(void *mutex);
    char *  (*_syslog_time)(char *buffer, const int len);
    void    (*_log_push_cb)(void*);
    char *  (*_trim_file_path)(const char* path);
} osi_uring_fifo;

extern bool uring_fifo_push_s(const void* buf, const uint16_t len, push_callback cb, void* cb_data);
extern bool post_log(const MOD_TYPE m, const uint8_t s, const char* f, const uint32_t l, const char *fmt, ...);

extern int32_t uring_fifo_pop(void* const buf, const uint16_t _len);

extern int32_t uring_fifo_pop_cb(pop_callback cb, void* cb_arg);

extern bool uring_fifo_init(const uint16_t queue_depth);
#ifdef __cplusplus
}
#endif

#endif /*U_RING_FIFO_H_*/

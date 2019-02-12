#ifndef _OS_PORT_DEF_H_
#define _OS_PORT_DEF_H_

#include <k_types.h>
#include <k_api.h>
#include "ulog/ulog.h"

typedef kbuf_queue_t _queue_t;
typedef ksem_t       _sem_t;
typedef aos_task_t   _task_t;
typedef cpu_stack_t  _stack_element_t;

#define BT_MOD "AOSBT"

#define SYS_LOG_DBG(...) LOGD(BT_MOD, ##__VA_ARGS__)
#define SYS_LOG_INF(...) LOGI(BT_MOD, ##__VA_ARGS__)
#define SYS_LOG_WRN(...) LOGW(BT_MOD, ##__VA_ARGS__)
#define SYS_LOG_ERR(...) LOGE(BT_MOD, ##__VA_ARGS__)
#define SYS_LOG_FAT(...) LOGF(BT_MOD, ##__VA_ARGS__)

#endif

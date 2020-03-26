

#ifndef __UMESH_DEFINE_H__
#define __UMESH_DEFINE_H__
#include "ulog/ulog.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LOG_TAG "LOCAL_COMM"
#define log_d(...)          ulog(LOG_DEBUG, LOG_TAG, ULOG_TAG, __VA_ARGS__)
#define log_i(...)          ulog(LOG_INFO, LOG_TAG, ULOG_TAG, __VA_ARGS__)
#define log_w(...)          ulog(LOG_WARNING, LOG_TAG, ULOG_TAG, __VA_ARGS__)
#define log_e(...)          ulog(LOG_ERR, LOG_TAG, ULOG_TAG, __VA_ARGS__)

#define hal_printf          printf

#define SERVICE_QUERY_DURATION 6
#define SERVICE_TTL            60
#define SERVICE_TIMEOUT_CNT    2
#define SERVICE_PORT           8771

#define SERVICE_FULL_TYPE_LEN_MAX   (SERVICE_NAME_LEN_MAX + SERVICE_TYPE_LEN_MAX + 11 +3)
#define SERVICE_TYPE_SUFFIX        "_udp.local"
#define SERVICE_TASK_STACK_SIZE    4098
#define SERVICE_AUTH_PAYLOAD_MAX   256
#define SERVICE_MAX_FOUND_NUM      36
#define SERVICE_DATA_MAX_LEN       1024
#define SERVICE_QUEUE_MSG_MAX_NUM  4
#define SERVICE_QUEUE_MSG_SIZE     8
#define UMESH_SRV_DEFAULT_NANE     "NO_NAME"

/** @} */
#ifdef __cplusplus
}
#endif
#endif  /* __UMESH_MESH_H__ */


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

#define SERVICE_QUERY_DURATION 5
#define SERVICE_TTL          120
#define SERVICE_PORT         8771

#define SERVICE_FULL_TYPE_LEN_MAX   (SERVICE_TYPE_LEN_MAX + 11)
#define SERVICE_TYPE_SUFFIX        "._udp.local"
#define SERVICE_TASK_STACK_SIZE    16000
#define SERVICE_AUTH_PAYLOAD_MAX   256
#define SERVICE_MAX_FOUND_NUM      36
#define SERVICE_DATA_MAX_LEN       1024
#define SERVICE_QUEUE_MSG_MAX_NUM  4
#define SERVICE_QUEUE_MSG_SIZE     8


/* User Input: 0x0100 ~ 0x01FF */
#define UMESH_SERVICE_ERR_BASE                       (-0x2000)

/*common err */
/* User input parameters contain unacceptable NULL pointer */
/* 用户传递给API的参数中含有不合理的空指针参数 */

#define UMESH_OK                             (0)
#define UMESH_ERR_NULL_POINTER               (UMESH_SERVICE_ERR_BASE - 0x0001)
#define UMESH_ERR_MALLOC_FAILED              (UMESH_SERVICE_ERR_BASE - 0x0002)
#define UMESH_ERR_OUT_OF_BOUNDS              (UMESH_SERVICE_ERR_BASE - 0x0003)
#define UMESH_NO_ACTION_REQUIRED             (UMESH_SERVICE_ERR_BASE - 0x0004)
#define UMESH_ERR_NOT_INIT                   (UMESH_SERVICE_ERR_BASE - 0x0005)
#define UMESH_ERR_MDNS                       (UMESH_SERVICE_ERR_BASE - 0x0006)
#define UMESH_ERR_MDNS_FORMAT                (UMESH_SERVICE_ERR_BASE - 0x0007)

#define UMESH_ERR_INIT                       (UMESH_SERVICE_ERR_BASE - 0x0008)
#define UMESH_ERR_AUTH                       (UMESH_SERVICE_ERR_BASE - 0x0009)
#define UMESH_ERR_AUTH_REFUSE               (UMESH_SERVICE_ERR_BASE - 0x000E)
#define UMESH_ERR_NO_AUTH_DATA               (UMESH_SERVICE_ERR_BASE - 0x000A)
#define UMESH_ERR_CREATE_SOCKET              (UMESH_SERVICE_ERR_BASE - 0x000B)
#define UMESH_ERR_BIND                       (UMESH_SERVICE_ERR_BASE - 0x000C)
#define UMESH_ERR_READ_DATA                  (UMESH_SERVICE_ERR_BASE - 0x000D)
#define UMESH_ERR_AUTH_NO_MATCH               (UMESH_SERVICE_ERR_BASE - 0x000F)
/** @} */
#ifdef __cplusplus
}
#endif
#endif  /* __UMESH_MESH_H__ */
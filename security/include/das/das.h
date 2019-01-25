/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __DAS_H__
#define __DAS_H__

#include <das/configure.h>
#include <das/platform.h>

#ifdef __cplusplus
extern "C"
#endif

typedef int das_result_t;

/* Result Codes */
#define DAS_SUCCESS                 (0)
#define DAS_ERROR_GENERIC           (-1)
#define DAS_ERROR_BAD_PARAMETERS    (-2)
#define DAS_ERROR_SHORT_BUFFER      (-3)
#define DAS_ERROR_EXCESS_DATA       (-4)
#define DAS_ERROR_OUT_OF_MEMORY     (-5)
#define DAS_ERROR_COMMUNICATION     (-6)
#define DAS_ERROR_NOT_SUPPORTED     (-7)
#define DAS_ERROR_NOT_IMPLEMENTED   (-8)
#define DAS_ERROR_TIMEOUT           (-9)

typedef int (*publish_handle_t)(const char *topic, const uint8_t *message, size_t msg_size, void *channel);

void *das_init(const char *product_name, const char *device_name);
void das_final(void *session);

const char *das_sub_topic(void *session, const char *topic);
const char *das_pub_topic(void *session, const char *topic);

void das_connection(void *session, publish_handle_t publish_handle, void *channel);

void das_on_connected(void *session);
void das_on_disconnected(void *session);
void das_on_message(void *session, const uint8_t *message, size_t msg_size);

das_result_t das_stepping(void *session, uint64_t now);

#ifdef __cplusplus
}
#endif

#endif /* __DAS_H__ */

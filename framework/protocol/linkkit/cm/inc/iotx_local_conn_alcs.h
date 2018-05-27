
#ifndef SRC_LOCAL_CONNECTION_ALCS_H_
#define SRC_LOCAL_CONNECTION_ALCS_H_

#ifdef CM_SUPPORT_LOCAL_CONN
#include "iotx_cm_connection.h"

typedef struct iotx_alcs_transfer_msg_context {
    uint16_t             port;
    char*                ip;
    char*                token;
    int                  token_len;
} iotx_alcs_transfer_msg_context_t;


void* iotx_local_conn_alcs_init(void* handle);

int iotx_local_conn_alcs_add_service(void *handle, const char *topic_filter, iotx_cm_message_ack_types_t ack_type, iotx_cm_message_auth_types_t auth_type);

int iotx_local_conn_alcs_remove_service(void *handle, const char *topic_filter);

int iotx_local_conn_alcs_send(void* handle, void* _context, iotx_connection_msg_t* message);

int iotx_local_conn_alcs_add_subdevice(void* handle, const char* pk, const char* dn);

int iotx_local_conn_alcs_remove_subdevice(void* handle, const char* pk, const char* dn);

int iotx_local_conn_alcs_deinit(void *handle);

int iotx_local_conn_alcs_yield(void *handle, int timeout_ms);

#endif /* CM_SUPPORT_LOCAL_CONN */
#endif /* SRC_LOCAL_CONNECTION_ALCS_H_ */

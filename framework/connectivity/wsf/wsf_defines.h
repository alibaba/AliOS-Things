/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef WSF_DEFINES_H
#define WSF_DEFINES_H

#ifdef  __cplusplus
extern "C"
{
#endif

typedef enum {
    WSF_SUCCESS = 0,
    WSF_FAIL,
    WSF_UNKNOWN_OPTION,
    WSF_HOSTNAME_ERROR,
    WSF_CONNECTION_ERROR,
    WSF_SEND_ERROR,
    WSF_CONNECTION_DEREGISTERED,
    WSF_CODE_END
}
wsf_code;

typedef enum {
    INVOKE_RIGHT = 0,
    INVOKE_LOGIN_SUCCESS,
    INVOKE_LOGIN_ERROR,
    INVOKE_URL_ERROR,
    INVOKE_NET_ERROR,
    INVOKE_CLASSNAME_ERROR,
    INVOKE_SERVER_ERROR,
    INVOKE_MSG_TYPE_ERROR,
    INVOKE_SESSION_EXPIRE_ERROR,
    INVOKE_RESULT_CODE_END
} invoke_result_code;


typedef enum {
    SERVER_NAME_STRING = 0,
    SERVER_PORT_INT,
    HEARTBEAT_INTERVAL_INT,
    VERSION_SHORT,
    REQUEST_TIMEOUT_INT,
    MSG_QUEUE_LENGTH_INT,
    MAX_MSG_RECV_LENGTH_INT,
    ENABLE_SSL_INT,
    SSL_VERSION_INT,
    USER_HEARTBEAT_INTERVAL_INT,
    CERTIFICATE_INT,
    CONFIG_OPT_END
} config_opt;

typedef enum {
    MSG_REQUEST = 0,
    MSG_RESPONSE,
    MSG_HEARTBEAT_REQ,
    MSG_HEARTBEAT_RSP,
    MSG_REGISTER_REQ,
    MSG_REGISTER_RSP,
    MSG_DEREGISTER_CONNECTION,
    MSG_TYPE_END
} msg_type;

typedef enum {
    SSLV23 = 0,
    SSLV2,
    SSLV3,
    TLSV1,
    SSL_VERSION_END
} ssl_version;

typedef struct wsf_response_t {
    invoke_result_code result;
    int                length;
    void               *data;
} wsf_response_t;


#ifdef  __cplusplus
}
#endif

#endif

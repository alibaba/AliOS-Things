#ifndef DM_CM_MSG_INFO_H
#define DM_CM_MSG_INFO_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define DM_CM_MSG_INFO_CLASS get_dm_cm_msg_info_class()
#define DM_CM_MSG_INFO_PARAMS_LENGTH_MAX 1200

#define DM_CM_MSG_INFO_MESSAGE_TYPE_REQUEST  0
#define DM_CM_MSG_INFO_MESSAGE_TYPE_RESPONSE 1
#define DM_CM_MSG_INFO_MESSAGE_TYPE_RAW      2

typedef struct _req_rsp_param {
    char* key;
    char* value;
} req_rsp_param_t;

typedef struct {
    const void*     _;
    char*           uri;
    char*           payload_buf;
    char*           params_data_buf;
    char*           raw_data_buf;
    int             raw_data_length;
    char*           product_key;
    char*           device_name;
    int             id;
    int             code;
    char*           version;
    void*           param_list;
    char*           method;
    int             message_type; /* 0: request; 1: response; 2: raw. */
    int             ret;
#ifdef LOCAL_CONN_ENABLE
    void*           conn_ctx;
#endif
} dm_cm_msg_info_t;

extern const void* get_dm_cm_msg_info_class();

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DM_CM_MSG_INFO_H */

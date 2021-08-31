/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef __MAP_API_H__
#define __MAP_API_H__

#include "bluetooth.h"
#include "obex_api.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BTIF_MAP_SESSION_FOLDER_LEVEL_MAX (32)

typedef void * btif_map_session_handle_t;

typedef char * telephone_number_t;
typedef char * short_message_t;

typedef enum {
    BTIF_MAP_SESSION_EVENT_OPEN_IND = 0,
    BTIF_MAP_SESSION_EVENT_OPEN, // 1
    BTIF_MAP_SESSION_EVENT_CLOSE, // 2
} btif_map_session_event_t;

typedef enum {
    BTIF_MAP_SESSION_ERR_NO_ERROR = 0,
    BTIF_MAP_SESSION_ERR_SDP_REQUEST_FAIL, // 1
    BTIF_MAP_SESSION_ERR_CHANNEL_CLOSE, // 2
} btif_map_session_error_t;

typedef struct {
    union {
        struct {
            btif_map_session_error_t error;
            uint8_t error_detail;
        } close;
        struct {
            uint8_t *packet;
            uint32_t packet_len;
        } data;
        struct {
            bt_bdaddr_t *remote;
        } open;
    } p;
} btif_map_session_cb_param_t;

typedef int (*btif_map_session_event_handler_t)(btif_map_session_handle_t handle,
    btif_map_session_event_t event, btif_map_session_cb_param_t *param);

typedef void (*btif_map_callback_t) (void * param, void * map_session);

typedef enum {
    BTIF_MAP_SESSION_TYPE_MAS = 0,
    BTIF_MAP_SESSION_TYPE_MNS, // 1
} btif_map_session_type_t;

typedef struct {
    btif_map_session_type_t type;
    btif_obex_session_role_t obex_role;
} btif_map_session_config_t;

#define BTIF_MAP_REQUEST_NUM_IN_QUEUE_MAX (10)
#define BTIF_MAP_SMS_TELEPHONE_NUMBER_SIZE_MAX (18)
#define BTIF_MAP_SMS_MSG_SIZE_MAX (256)

typedef struct {
    list_entry_t node;
    char number[BTIF_MAP_SMS_TELEPHONE_NUMBER_SIZE_MAX];
    char msg[BTIF_MAP_SMS_MSG_SIZE_MAX];
} btif_map_sms_t;

typedef struct {
    union {
        struct {
            const char *folder[BTIF_MAP_SESSION_FOLDER_LEVEL_MAX];
            uint8_t folder_level;
            uint8_t up_level;
        } SetFolder;
    } p;
} btif_map_session_function_param_t;

bt_status_t btif_map_initialize(void);
btif_map_session_handle_t btif_map_create_session(void);
btif_map_session_handle_t btif_map_get_session(bt_bdaddr_t *remote);
bt_status_t btif_map_session_open(btif_map_session_handle_t handle, bt_bdaddr_t *remote, btif_map_session_config_t *config,
    btif_map_session_event_handler_t handler);
bt_status_t btif_map_set_folder(btif_map_session_handle_t handle, btif_map_session_function_param_t *param);
bt_status_t btif_map_send_sms(btif_map_session_handle_t handle, telephone_number_t receipt, short_message_t msg);
bt_status_t btif_map_session_close(btif_map_session_handle_t handle);
bool btif_map_check_is_connected(btif_map_session_handle_t handle);
void btif_map_callback_register(btif_map_callback_t callback);
#if defined(IBRT)
#ifdef __BTMAP_ENABLE__
uint32_t btif_map_get_server_chnl(void);
uint32_t map_save_ctx(uint32_t server_chnl, uint32_t conn_id,uint8_t *buf, uint32_t buf_len);
uint32_t map_restore_ctx(bt_bdaddr_t *remote,uint32_t connection_id,uint32_t server_chnl);
uint32_t btif_map_get_rfcomm_handle(void);
uint32_t btif_map_profile_save_ctx(uint8_t *buf, uint32_t buf_len);
uint32_t btif_map_profile_restore_ctx(uint8_t *buf, uint32_t buf_len);
#endif
#endif

#ifdef __cplusplus
}
#endif

#endif /* __MAP_API_H__ */
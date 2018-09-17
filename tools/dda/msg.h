/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef DDA_MSG_H
#define DDA_MSG_H

enum {
    TYPE_MGMT=0x01,
    TYPE_MESH,
    TYPE_P2P,
    TYPE_MISC,
};

enum {
    CMD_ACK_FLAG =0x8000,
    CMD_NEED_ACK_FLAG =0x4000,
    /* mgmt command */
    CMD_MGMT_DEVICE_UP=0x00,
    CMD_MGMT_ATTACH,
    CMD_MGMT_STATUS,

    /* mesh command */
    CMD_MESH_DATA=0x10,
    CMD_MESH_SET_CHN,

    /* p2p command */
    CMD_P2P_REQ=0x20,
    CMD_P2P_RES=0x21,

    /* misc command */
    /* agent -> master */
    CMD_MISC_LOG=0x70,
    CMD_MISC_CLI=0x71,
    /* master -> agent */
    CMD_MISC_INPUT=0x80,
};
#define CMD_ACK(c) ((c) | CMD_ACK_FLAG)
#define CMD_NEED_ACK(c) ((c) | CMD_NEED_ACK_FLAG)

typedef void (*message_handler_t)(int cmd, void *buf, int len, void *ctx);
int dda_register_message_handler(int message_type, message_handler_t hdl, void *ctx);
int dda_unregister_message_handler(int message_type, message_handler_t hdl, void *ctx);

#include "umesh_hal.h"

typedef void (*dda_p2p_cb)(const char *, int, void *);
void dda_p2p_request(int dst_id, const char *cmd, dda_p2p_cb cb, void *cb_data);
void dda_p2p_remove_request(dda_p2p_cb cb, void *cb_data);
void dda_mesh_send_data(int type, int cmd, uint64_t cmd_pri, int dst_id, const void *buf, int len);
typedef void (*dda_mesh_cb)(frame_t *frm, frame_info_t *fino, void *cb_data);
void dda_mesh_register_receiver(int media_type, dda_mesh_cb, void *cb_data);
uint64_t dda_mesh_get_agent_id(void);

#define MAX_MTU_SIZE 2048
#define DEFAULT_MTU_SIZE 1024
#define MIN_MTU_SIZE 28
#endif

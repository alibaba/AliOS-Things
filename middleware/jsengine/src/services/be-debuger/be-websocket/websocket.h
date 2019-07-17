/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef BE_WEBSOCKET_H
#define BE_WEBSOCKET_H

#include <stdbool.h>

#define BWS_SHAKE_KEY_LEN 32
#define BWS_PATH_LEN 128

typedef enum {
    BWS_TXT_FRAME       = 0x81,  /* text frame */
    BWS_BIN_FRAME       = 0x82,  /* binary frame */
    BWS_CLOSE           = 0x88,  /* connection close */
    BWS_PING            = 0x89,  /* ping */
    BWS_PONG            = 0x8A,  /* pong */
    BWS_CONT_FRAME_TXT  = 0x01,  /* continuation frame text begin */
    BWS_CONT_FRAME_BIN  = 0x02,  /* continuation frame binary begin */
    BWS_CONT_FRAME_MID  = 0x00,  /* continuation frame middle */
    BWS_CONT_FRAME_LAST = 0x80,  /* continuation frame last */
    BWS_UNKNOW          = 0xFF,  /* unknow */
} bone_websocket_frame_type_t;

typedef enum {
    BWS_STATE_CONNECTING = 0x01,
    BWS_STATE_OPEN       = 0x02,
    BWS_STATE_CLOSING    = 0x03,
    BWS_STATE_CLOSED     = 0x04,
} bone_websocket_state_t;

typedef struct bone_websocket_client_s bone_websocket_client_t;
typedef void (*bone_websocket_on_open)(bone_websocket_client_t* client);
typedef void (*bone_websocket_on_error)(bone_websocket_client_t* client);
typedef void (*bone_websocket_on_message)(bone_websocket_client_t* client,
                                          unsigned char* msg, size_t msg_len);
typedef void (*bone_websocket_on_close)(bone_websocket_client_t* client);

#define BONE_WEBSOCKET_MAGIC 0x010255aa

#define BONE_WEBSOCKET_TMP_SIZE 64
struct bone_websocket_client_s {
    uint32_t magic;
    bone_websocket_on_open on_open;
    bone_websocket_on_error on_error;
    bone_websocket_on_message on_message;
    bone_websocket_on_close on_close;
    bone_websocket_state_t state;
    char ip[16];
    int port;
    char path[BWS_PATH_LEN];
    char shake_key[BWS_SHAKE_KEY_LEN];
    int fd;
    void* tsk_handle;
    int tsk_done;
    unsigned char* frame_buf;
    int frame_buf_length;

    int frame_size;
    int tmp_length;

    void* ping_tsk_handle;
    void* timeout_tsk_handle;
};

typedef struct {
    unsigned char* frame_data;
    size_t frame_total_len;
    unsigned char* payload;
    size_t payload_len;
    bool is_mask;
    bone_websocket_frame_type_t type;
} bone_websocket_frame_t;

bone_websocket_client_t* bone_websocket_getInstance();

int bone_websocket_init(bone_websocket_client_t* client);

int bone_websocket_deinit(bone_websocket_client_t* client);

int bone_websocket_connect(bone_websocket_client_t* client);

int bone_websocket_send_raw(bone_websocket_client_t* client,
                            unsigned char* data, size_t data_len);

/* websocket上报信息 */
int bone_websocket_send_frame(char* topic, int level, char* msg);

int bone_websocket_build_frame(unsigned char* payload, size_t payload_len,
                               bool is_mask, bone_websocket_frame_type_t type,
                               unsigned char* frame_buf, size_t frame_buf_size);

int bone_websocket_parse_frame(unsigned char* frame_data, size_t frame_data_len,
                               bone_websocket_frame_t* frame);

/**
201 Bad Request
202 Not Implemented
203 out of memory
204 Busy, please try again later
*/

#endif /* BE_WEBSOCKET_H */

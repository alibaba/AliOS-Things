/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <hal/soc/soc.h>

#define MAX_HDLC_TX_BUF_LEN 256
#define MAX_HDLC_RX_BUF_LEN 512

typedef enum {
    RECV_STATE_NO_SYNC = 0,
    RECV_STATE_SYNC    = 1,
    RECV_STATE_ESCAPED = 2,
} recv_state_t;

typedef struct encode_context {
    uint8_t  buf[MAX_HDLC_TX_BUF_LEN];
    uint16_t len;
    uint16_t fcs;
    bool ackreq;
} encode_context_t;

typedef struct decode_context {
    recv_state_t state;
    uint8_t  buf[MAX_HDLC_RX_BUF_LEN];
    uint8_t  raw[MAX_HDLC_RX_BUF_LEN];
    uint16_t len;
    uint16_t fcs;
    uint16_t read;
    bool ackreq;
    uint8_t seq;
    bool dup;
} decode_context_t;

int32_t hdlc_uart_send(encode_context_t *ctx, uart_dev_t *uart, const void *data,
                       uint32_t size, uint32_t timeout, bool ackreq);

int32_t hdlc_uart_recv(decode_context_t *ctx, uart_dev_t *uart, void *data,
                       uint32_t expect_size,  uint32_t *recv_size, uint32_t timeout);

int32_t hdlc_decode_context_init(decode_context_t *ctx);

int32_t hdlc_encode_context_init(encode_context_t *ctx);

int32_t hdlc_encode_context_finalize(encode_context_t *ctx);

int32_t hdlc_decode_context_finalize(decode_context_t *ctx);

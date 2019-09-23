/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#ifndef _HDLC_H_
#define _HDLC_H_


#define MAX_HDLC_TX_BUF_LEN 1024
#define MAX_HDLC_RX_BUF_LEN 1024
#define HDLC_ENCODE_BUF_MARGIN 200
#define MAX_HDLC_TX_STEP (MAX_HDLC_TX_BUF_LEN - HDLC_ENCODE_BUF_MARGIN)

typedef enum
{
    RECV_STATE_NO_SYNC = 0,
    RECV_STATE_SYNC,
    RECV_STATE_ESCAPED,
    RECV_STATE_SEQ,
} recv_state_t;

typedef struct encode_context
{
    uint8_t  buf[MAX_HDLC_TX_BUF_LEN];
    uint16_t len;
    uint16_t fcs;
} encode_context_t;

typedef struct
{
    uint8_t *buffer;
    uint32_t length;
    uint32_t head;
    uint32_t tail;
} ringbuf_t;

typedef struct decode_context
{
    bool        inited;
    uart_dev_t *uart;
    int32_t     timeout;

    ringbuf_t * decoded_buf;
    aos_mutex_t decoded_buf_mutex;
    aos_sem_t   decoded_buf_sem;

    ringbuf_t *raw_buf;

    recv_state_t state;
    uint16_t     len;
    uint16_t     fcs;
    uint32_t     undecoded_len;

    bool    ackreq;
    uint8_t recvseq;
    uint8_t expectseq;
} decode_context_t;

int32_t hdlc_uart_send(encode_context_t *ctx, uart_dev_t *uart,
                       const void *data, uint32_t size, uint32_t timeout,
                       bool ackreq);

int32_t hdlc_uart_recv(decode_context_t *ctx, uart_dev_t *uart, void *data,
                       uint32_t expect_size, uint32_t *recv_size,
                       uint32_t timeout);

int32_t hdlc_decode_context_init(decode_context_t *ctx, uart_dev_t *uart);

int32_t hdlc_encode_context_init(encode_context_t *ctx);

int32_t hdlc_encode_context_finalize(encode_context_t *ctx);

int32_t hdlc_decode_context_finalize(decode_context_t *ctx);

#endif //_HDLC_H_

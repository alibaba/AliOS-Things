/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <string.h>
#include "aos/kernel.h"
#include "ulog/ulog.h"
#include "hdlc.h"

#define MODULE_NAME "hdlc"

enum
{
    FLAG_SEQUENCE         = 0x7e, // HDLC Flag value
    FLAG_SEQUENCE_ACK_REQ = 0x7b, // HDLC Flag ack req value
    ESCAPE_SEQUENCE       = 0x7d, // HDLC Escape value
};

enum
{
    INIT_FCS16 = 0xffff, // initial FCS value.
    GOOD_FCS16 = 0xf0b8, // good FCS value.
};

/*
 * FCS lookup table
 */
static const uint16_t fcs16tab[256] = {
    0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf, 0x8c48,
    0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7, 0x1081, 0x0108,
    0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e, 0x9cc9, 0x8d40, 0xbfdb,
    0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876, 0x2102, 0x308b, 0x0210, 0x1399,
    0x6726, 0x76af, 0x4434, 0x55bd, 0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e,
    0xfae7, 0xc87c, 0xd9f5, 0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e,
    0x54b5, 0x453c, 0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd,
    0xc974, 0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
    0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3, 0x5285,
    0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a, 0xdecd, 0xcf44,
    0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72, 0x6306, 0x728f, 0x4014,
    0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9, 0xef4e, 0xfec7, 0xcc5c, 0xddd5,
    0xa96a, 0xb8e3, 0x8a78, 0x9bf1, 0x7387, 0x620e, 0x5095, 0x411c, 0x35a3,
    0x242a, 0x16b1, 0x0738, 0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862,
    0x9af9, 0x8b70, 0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e,
    0xf0b7, 0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
    0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036, 0x18c1,
    0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e, 0xa50a, 0xb483,
    0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5, 0x2942, 0x38cb, 0x0a50,
    0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd, 0xb58b, 0xa402, 0x9699, 0x8710,
    0xf3af, 0xe226, 0xd0bd, 0xc134, 0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7,
    0x6e6e, 0x5cf5, 0x4d7c, 0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1,
    0xa33a, 0xb2b3, 0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72,
    0x3efb, 0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
    0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a, 0xe70e,
    0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1, 0x6b46, 0x7acf,
    0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9, 0xf78f, 0xe606, 0xd49d,
    0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330, 0x7bc7, 0x6a4e, 0x58d5, 0x495c,
    0x3de3, 0x2c6a, 0x1ef1, 0x0f78
};

// unit ms
#define DEFAULT_ACK_TIMEOUT 300
#define HDLC_MAX_ATTEMPT 4
#define DEFAULT_HLDC_RETRY_DELAY 50
#define DEFAULT_DECODE_READ_TIMEOUT 1000
static aos_sem_t   acksem;
static aos_mutex_t hal_send_mutex;
static bool        waitack    = false;
static uint32_t    acktimeout = DEFAULT_ACK_TIMEOUT;

ringbuf_t *ringbuf_create(int length);
void       ringbuf_destroy(ringbuf_t *buffer);
int        ringbuf_read(ringbuf_t *buffer, uint8_t *target, uint32_t amount);
int        ringbuf_write(ringbuf_t *buffer, uint8_t *data, uint32_t length);
int  ringbuf_checkbyte(ringbuf_t *buffer, uint8_t *target, uint32_t offset);
int  ringbuf_available_write_space(ringbuf_t *buffer);
int  ringbuf_available_read_space(ringbuf_t *buffer);
void ringbuf_clear_all(ringbuf_t *buffer);
void ringbuf_clear_from_head(ringbuf_t *buffer, int len);
void ringbuf_clear_from_tail(ringbuf_t *buffer, int len);
int  ringbuf_available_write_space(ringbuf_t *buffer);
bool ringbuf_full(ringbuf_t *buffer);
bool ringbuf_empty(ringbuf_t *buffer);
int  ringbuf_move(ringbuf_t *dest, ringbuf_t *source, uint32_t len);
#define MIN(a, b) (a) < (b) ? (a) : (b)

static int32_t local_hal_uart_send(uart_dev_t *uart, const void *data,
                                   uint32_t size, uint32_t timeout)
{
    int ret;

    aos_mutex_lock(&hal_send_mutex, AOS_WAIT_FOREVER);
    ret = hal_uart_send(uart, data, size, timeout);
    aos_mutex_unlock(&hal_send_mutex);

    return ret;
}

static uint16_t fcs16(uint16_t fcs, uint8_t *cp, uint16_t len)
{
    while (len--) {
        fcs = (fcs >> 8) ^ fcs16tab[(fcs ^ *cp++) & 0xff];
    }

    return (fcs);
}

static int32_t encode_byte(encode_context_t *ctx, uint8_t byte)
{
    if (NULL == ctx) {
        return -1;
    }

    if ((ctx->len + 1) >= sizeof(ctx->buf)) {
        return -1;
    }

    // update fcs
    ctx->fcs = fcs16(ctx->fcs, &byte, 1);

    if (byte == FLAG_SEQUENCE || byte == FLAG_SEQUENCE_ACK_REQ ||
        byte == ESCAPE_SEQUENCE) {
        ctx->buf[ctx->len++] = ESCAPE_SEQUENCE;
        byte ^= 0x20;
    }

    ctx->buf[ctx->len++] = byte;

    return 0;
}

static int32_t encode_hdlc(encode_context_t *ctx, const uint8_t *buf,
                           uint32_t len, bool ackreq, uint32_t seqno)
{
    int32_t  i;
    uint16_t fcs;

    if (NULL == ctx || NULL == buf || (len + 4) > sizeof(ctx->buf)) {
        return -1;
    }

    // init
    memset(ctx, 0, sizeof(encode_context_t));
    ctx->len             = 0;
    ctx->fcs             = INIT_FCS16;
    ctx->buf[ctx->len++] = ackreq ? FLAG_SEQUENCE_ACK_REQ : FLAG_SEQUENCE;
    // seq
    if (ackreq) {
        ctx->buf[ctx->len++] = seqno;
    }

    for (i = 0; i < len; i++) {
        if (encode_byte(ctx, buf[i]) < 0) {
            return -1;
        }
    }

    fcs = ctx->fcs;
    fcs ^= 0xffff;

    // write fcs
    if (encode_byte(ctx, fcs & 0xff) < 0) {
        return -1;
    }

    if (encode_byte(ctx, fcs >> 8) < 0) {
        return -1;
    }

    // write final byte
    if (ctx->len >= sizeof(ctx->buf)) {
        return -1;
    }

    ctx->buf[ctx->len++] = ackreq ? FLAG_SEQUENCE_ACK_REQ : FLAG_SEQUENCE;

    return 0;
}

int32_t hdlc_uart_send(encode_context_t *ctx, uart_dev_t *uart,
                       const void *data, uint32_t size, uint32_t timeout,
                       bool ackreq)
{
    uint32_t        remain = size;
    uint32_t        sent   = 0;
    uint32_t        step;
    int             ret     = 0, uart_ret;
    int             attempt = 0;
    bool            frag    = true;
    static uint32_t seqno   = 0;

    if (remain <= MAX_HDLC_TX_STEP) {
        frag = false;
    }

    while (remain > 0) {
        if (++attempt > HDLC_MAX_ATTEMPT) {
            ret = -1;
            LOGE(MODULE_NAME, "attempt fail\n");
            goto exit;
        }

        step = MAX_HDLC_TX_STEP > remain ? remain : MAX_HDLC_TX_STEP;

        if (encode_hdlc(ctx, (uint8_t *)data + sent, step, ackreq, seqno) !=
            0) {
            ret = -1;
            LOGE(MODULE_NAME, "encode fail!\n");
            goto exit;
        }

        if ((uart_ret = hal_uart_send(uart, ctx->buf, ctx->len, timeout)) !=
            0) {
            ret = -1;
            LOGE(MODULE_NAME, "uart send fail! ret %d\n", uart_ret);
            if (ackreq) {
                aos_msleep(DEFAULT_HLDC_RETRY_DELAY);
                continue;
            } else {
                ret = -1;
                goto exit;
            }
        }

        if (!ackreq) {
            sent += step;
            remain -= step;
            attempt = 0;
            waitack = false;
            continue;
        }

        waitack = true;
        if ((ret = aos_sem_wait(&acksem, acktimeout)) != 0) {
            LOGE(MODULE_NAME, "attempt %d ack sem_wait failed", attempt);
            waitack = false;
            continue;
        }

        waitack = false;
        sent += step;
        remain -= step;
        attempt = 0;
        seqno++;

        LOGD(MODULE_NAME, "ack recv remain %d sent %d\n", remain, sent);
    }

exit:
    LOGD(MODULE_NAME, "hdlc send exit\n");
    return ret;
}

int32_t read_from_decoded_buf(decode_context_t *ctx, uint8_t *target,
                              uint32_t len)
{
    uint32_t read_size;

    if (!ctx || !target || !len) {
        return -1;
    }

    if (ringbuf_empty(ctx->decoded_buf)) {
        aos_sem_wait(&ctx->decoded_buf_sem, 1000);
    }

    read_size = ringbuf_read(ctx->decoded_buf, target, len);

    return read_size;
}

int32_t move_from_raw_to_decoded_buf(decode_context_t *ctx, uint32_t len)
{
    uint32_t move_size;
    bool     notify = false;

    if (!ctx || !len) {
        LOGE(MODULE_NAME, "Invalid input para\r\n");
        return -1;
    }

    if (ringbuf_empty(ctx->decoded_buf)) {
        notify = true;
    }

    move_size = ringbuf_move(ctx->decoded_buf, ctx->raw_buf, len);
    if (move_size < 0) {
        LOGE(MODULE_NAME,
             "Error move %d from raw operation fail, decoded buf available %d",
             len, ringbuf_available_write_space(ctx->decoded_buf));
    } else if (move_size == 0) {
        LOGD(
          MODULE_NAME,
          "Warning move %d from raw operation fail, decoded buf available %d",
          len, ringbuf_available_write_space(ctx->decoded_buf));
    } else {
        if (notify) {
            aos_sem_signal(&ctx->decoded_buf_sem);
        }
    }

    return move_size;
}

static uint8_t ringbuf_tmp[MAX_HDLC_RX_BUF_LEN];
int            ringbuf_move(ringbuf_t *dest, ringbuf_t *source, uint32_t len)
{
    int move_size = 0;

    if (!dest || !source || !len) {
        return -1;
    }

    move_size = MIN(ringbuf_available_read_space(source),
                    ringbuf_available_write_space(dest));

    move_size = MIN(len, move_size);

    if (move_size > MAX_HDLC_RX_BUF_LEN) {
        return -1;
    }

    if (move_size > 0) {
        ringbuf_read(source, ringbuf_tmp, move_size);
        ringbuf_write(dest, ringbuf_tmp, move_size);
    }

    return move_size;
}

void decode_state_reset_undecoded(decode_context_t *ctx, int len)
{
    if (!ctx) {
        return;
    }

    ctx->len -= len;
    ctx->fcs           = INIT_FCS16;
    ctx->state         = RECV_STATE_NO_SYNC;
    ctx->undecoded_len = 0;
    ctx->ackreq        = false;
}

void decode_state_reset(decode_context_t *ctx)
{
    if (!ctx) {
        return;
    }

    ctx->len           = 0;
    ctx->fcs           = INIT_FCS16;
    ctx->state         = RECV_STATE_NO_SYNC;
    ctx->undecoded_len = 0;
    ctx->ackreq        = false;
}

static int32_t decode_byte(decode_context_t *ctx, uint8_t byte)
{
    if (NULL == ctx) {
        return -1;
    }

    // if no space left, reset state and clear raw buf
    if (ringbuf_write(ctx->raw_buf, &byte, 1) != 1) {
        LOGE(MODULE_NAME, "Error raw buf full\r\n");
        return -1;
    }

    ctx->fcs = fcs16(ctx->fcs, &byte, 1);
    ctx->len++;
    ctx->undecoded_len++;

    return 0;
}

static char ack_str[]   = "ACK!";
static int  match_index = 0;
static bool match_response(uint8_t *buf, uint32_t len)
{
    int compare_size;

    compare_size = (strlen(ack_str) - match_index) > len
                     ? len
                     : (strlen(ack_str) - match_index);

    if (memcmp(buf, ack_str + match_index, compare_size) == 0) {
        if ((match_index + compare_size) == strlen(ack_str)) {
            match_index = 0;
            return true;
        } else {
            match_index++;
            return false;
        }
    } else {
        match_index = 0;
        return false;
    }
}

int32_t decode_hdlc_ringbuf(decode_context_t *ctx, uint8_t *buf, uint32_t len)
{
    int32_t  i;
    uint8_t  byte;
    int32_t  match_index_pre;
    uint8_t *input;
    uint32_t input_len;
    uint8_t  ackstr_buf[strlen(ack_str)];

    if (NULL == ctx || NULL == buf) {
        return -1;
    }

    input     = buf;
    input_len = len;

    if (waitack) {
        match_index_pre = match_index;
        if (match_response(buf, len) && aos_sem_is_valid(&acksem)) {
            aos_sem_signal(&acksem);
            return 0;
        }

        if (match_index > match_index_pre) {
            return 0;
        } else if (match_index_pre > 0 && 0 == match_index) {
            if (match_index_pre + len > strlen(ack_str)) {
                LOGE(MODULE_NAME, "ACK match error!\n");
                return -1;
            }

            memcpy(ackstr_buf, ack_str, match_index_pre);
            memcpy(ackstr_buf + match_index_pre, buf, len);

            input     = ackstr_buf;
            input_len = match_index_pre + len;
        }
    } else {
        match_index = 0;
    }

    for (i = 0; i < input_len; i++) {
        byte = input[i];

        /*LOG("recv string -->%c<--%u stat %u fcs %u len %u unlen %u \n",
             byte, byte, ctx->state, ctx->fcs, ctx->len, ctx->undecoded_len);*/

        switch (ctx->state) {
            case RECV_STATE_NO_SYNC:
                if (FLAG_SEQUENCE == byte || FLAG_SEQUENCE_ACK_REQ == byte) {
                    ctx->state  = RECV_STATE_SYNC;
                    ctx->len    = 0;
                    ctx->fcs    = INIT_FCS16;
                    ctx->ackreq = (FLAG_SEQUENCE_ACK_REQ == byte);

                    if (ctx->ackreq) {
                        ctx->state   = RECV_STATE_SEQ;
                        ctx->recvseq = 0;
                    }
                }
                break;

            case RECV_STATE_SEQ:
                ctx->recvseq = byte;
                ctx->state   = RECV_STATE_SYNC;
                break;

            case RECV_STATE_SYNC:
                switch (byte) {
                    case ESCAPE_SEQUENCE:
                        ctx->state = RECV_STATE_ESCAPED;
                        break;

                    case FLAG_SEQUENCE_ACK_REQ:
                    case FLAG_SEQUENCE:
                        if (ctx->len) {
                            // verify FCS
                            if ((ctx->fcs != GOOD_FCS16) || (ctx->len < 2)) {
                                LOGE(MODULE_NAME, "fcs error\n");
                                ringbuf_clear_from_tail(ctx->raw_buf,
                                                        ctx->undecoded_len);
                                decode_state_reset_undecoded(
                                  ctx, ctx->undecoded_len);

                                // for ringbuf we do not exit
                                // return -1;
                            } else {
                                if (ctx->ackreq) {
                                    uint8_t repeatseq = ctx->expectseq - 1;
                                    LOGD(MODULE_NAME,
                                         "recv seq: %d expect seq: %d\n",
                                         ctx->recvseq, ctx->expectseq);

                                    if (ctx->recvseq ==
                                        repeatseq) { // repeat msg
                                        local_hal_uart_send(ctx->uart, ack_str,
                                                            strlen(ack_str),
                                                            acktimeout);
                                        LOGE(MODULE_NAME, "sequence error\n");
                                        ringbuf_clear_from_tail(
                                          ctx->raw_buf, ctx->undecoded_len);
                                        decode_state_reset_undecoded(
                                          ctx, ctx->undecoded_len);
                                        return -1;
                                    } else if (ctx->recvseq ==
                                               ctx->expectseq) { // correct msg
                                        ctx->expectseq = ctx->recvseq + 1;
                                    } else { // jump msg
                                        LOGD(MODULE_NAME,
                                             "\n---------->expectseq "
                                             "jump!!!<------------\n");
                                        ctx->expectseq = ctx->recvseq + 1;
                                    }
                                }

                                // ignore FCS
                                ringbuf_clear_from_tail(ctx->raw_buf,
                                                        sizeof(uint16_t));
                                ctx->len -= sizeof(uint16_t);
                                ctx->undecoded_len = 0;

                                if (ctx->len > ringbuf_available_read_space(
                                                 ctx->raw_buf)) {
                                    LOGE(
                                      MODULE_NAME,
                                      "ctx len larger than readable len\r\n");
                                    ringbuf_clear_all(ctx->raw_buf);
                                    decode_state_reset(ctx);

                                    return -1;
                                } else {
                                    int32_t len;

                                    len = move_from_raw_to_decoded_buf(
                                      ctx, ctx->len - ctx->undecoded_len);
                                    if (len <= 0) {
                                        LOGE(
                                          MODULE_NAME,
                                          "move from raw to decoded buf fail");
                                    }

                                    LOGD(MODULE_NAME,
                                         "FCS done: move len %d ctx len %d "
                                         "undecode %d fcs %u\n",
                                         len, ctx->len, ctx->undecoded_len,
                                         ctx->fcs);

                                    if (ctx->ackreq) {
                                        LOGD(MODULE_NAME, "Send ACK!\n");
                                        local_hal_uart_send(ctx->uart, ack_str,
                                                            strlen(ack_str),
                                                            acktimeout);
                                    }

                                    decode_state_reset_undecoded(ctx, len);
                                }

                                // for ringbuf we do not exit
                                // return 0;
                            }
                        }
                        break;

                    default:
                        if (decode_byte(ctx, byte) < 0) {
                            decode_state_reset(ctx);
                            ringbuf_clear_all(ctx->raw_buf);
                            return -1;
                        }
                        break;
                }
                break;

            case RECV_STATE_ESCAPED:
                if (decode_byte(ctx, byte ^ 0x20) < 0) {
                    decode_state_reset(ctx);
                    ringbuf_clear_all(ctx->raw_buf);
                    return -1;
                    // continue;
                }
                ctx->state = RECV_STATE_SYNC;
                break;

            default:
                ctx->state = RECV_STATE_NO_SYNC;
                break;
        }
    }

    return 0;
}

int32_t push_raw_to_decoded_buf(decode_context_t *ctx)
{
    int32_t len;

    if (ctx->len == ctx->undecoded_len) {
        return 0;
    }

    len = move_from_raw_to_decoded_buf(ctx, ctx->len - ctx->undecoded_len);
    if (len < 0) {
        LOGE(MODULE_NAME, "move from raw to decoded buf fail");
        return -1;
    }

    return len;
}

static void hdlc_decode_worker(void *arg)
{
    uint8_t *         recvbuf = NULL;
    decode_context_t *ctx     = (decode_context_t *)arg;
    uint32_t          bufsize = MAX_HDLC_RX_BUF_LEN;
    uint32_t          recvsize;

    if (!ctx) {
        LOGE(MODULE_NAME, "wrong task parameter\n");
        goto exit;
    }

    // read size and timeout need to be adjusted
    recvbuf = aos_malloc(bufsize);
    if (!recvbuf) {
        LOGE(MODULE_NAME, "hdlc decode recvbuf malloc fail\r\n");
        goto exit;
    }

    while (true) {
        recvsize = 0;

        if (!ctx->inited) {
            goto exit;
        }

        if (push_raw_to_decoded_buf(ctx) < 0) {
            LOGE(MODULE_NAME, "hdlc decode push fail\r\n");
        }

        // force to read one
        if (hal_uart_recv_II(ctx->uart, (void *)recvbuf, 1, &recvsize,
                             ctx->timeout) != 0) {
            // LOGE(MODULE_NAME, "uart recv fail %d\r\n", recvsize);
            continue;
        }

        if (decode_hdlc_ringbuf(ctx, recvbuf, recvsize) != 0) {
            LOGE(MODULE_NAME, "hdlc decode fail\r\n");
            continue;
        }
    }

exit:
    aos_free(recvbuf);
    LOGD(MODULE_NAME, "hdlc decode worker exit\r\n");
    aos_task_exit(0);
}

int32_t hdlc_uart_recv(decode_context_t *ctx, uart_dev_t *uart, void *data,
                       uint32_t expect_size, uint32_t *recv_size,
                       uint32_t timeout)
{
    long long current_ms;
    uint32_t  read_size;
    int       offset = 0;
    uint32_t  timeout_used, timeout_min;

    if (NULL == ctx) {
        return -1;
    }

    timeout_min  = DEFAULT_ACK_TIMEOUT * HDLC_MAX_ATTEMPT + 50;
    timeout_used = timeout_min > timeout ? timeout_min : timeout;

    *recv_size = 0;
    current_ms = aos_now_ms();
    do {
        read_size =
          read_from_decoded_buf(ctx, (uint8_t *)data + offset, expect_size);
        if (read_size < 0) {
            LOGE(MODULE_NAME, "read fail");
            return -1;
        } else if (read_size > 0) {
            // LOG("read %d %c\n", read_size, *((char *) data + offset));
        } else {
            // LOG("read nothing\n");
        }

        if (read_size > expect_size) {
            LOGE(MODULE_NAME, "read size exceed expect size\r\n");
            return -1;
        }

        expect_size -= read_size;
        *recv_size += read_size;
        offset += read_size;

        if (0 == expect_size) {
            return 0;
        }

    } while ((aos_now_ms() - current_ms) < timeout_used);

    return -1;
}

int32_t hdlc_decode_context_init(decode_context_t *ctx, uart_dev_t *uart)
{
    if (!ctx || !uart) {
        return -1;
    }

    memset(ctx, 0, sizeof(decode_context_t));

    ctx->uart = uart;

    if (aos_sem_new(&ctx->decoded_buf_sem, 0) != 0) {
        LOGE(MODULE_NAME, "failed to allocate semaphore\r\n");
        goto err;
    }

    if (aos_mutex_new(&ctx->decoded_buf_mutex) != 0) {
        LOGE(MODULE_NAME, "Creating task mutex failed\r\n");
        goto err;
    }
    ctx->decoded_buf = ringbuf_create(MAX_HDLC_RX_BUF_LEN);
    ctx->raw_buf     = ringbuf_create(MAX_HDLC_RX_BUF_LEN);
    if (ctx->decoded_buf == NULL || ctx->raw_buf == NULL) {
        LOGE(MODULE_NAME, "ringbuf create fail\r\n");
        goto err;
    }

    if (aos_task_new("deocder_worker", hdlc_decode_worker, (void *)ctx, 1024)) {
        LOGE(MODULE_NAME, "create decoder worker fail\r\n");
        goto err;
    }

    ctx->timeout = DEFAULT_DECODE_READ_TIMEOUT;
    ctx->fcs     = INIT_FCS16;
    ctx->state   = RECV_STATE_NO_SYNC;
    ctx->inited  = true;

    LOGD(MODULE_NAME, "decoder init!\r\n");
    return 0;

err:
    if (aos_sem_is_valid(&ctx->decoded_buf_sem)) {
        aos_sem_free(&ctx->decoded_buf_sem);
    }

    if (aos_mutex_is_valid(&ctx->decoded_buf_mutex)) {
        aos_mutex_free(&ctx->decoded_buf_mutex);
    }

    ringbuf_destroy(ctx->decoded_buf);
    ringbuf_destroy(ctx->raw_buf);

    memset(ctx, 0, sizeof(decode_context_t));

    LOGE(MODULE_NAME, "decoder init fail!\r\n");

    return -1;
}

int32_t hdlc_decode_context_finalize(decode_context_t *ctx)
{
    if (NULL == ctx) {
        return -1;
    }

    ringbuf_destroy(ctx->decoded_buf);
    ringbuf_destroy(ctx->raw_buf);
    if (aos_mutex_is_valid(&ctx->decoded_buf_mutex)) {
        aos_mutex_free(&ctx->decoded_buf_mutex);
    }

    if (aos_sem_is_valid(&ctx->decoded_buf_sem)) {
        aos_sem_free(&ctx->decoded_buf_sem);
    }

    memset(ctx, 0, sizeof(decode_context_t));

    LOGD(MODULE_NAME, "decoder finalize!\n");
    return 0;
}

int32_t hdlc_encode_context_init(encode_context_t *ctx)
{
    if (NULL == ctx) {
        return -1;
    }

    if (aos_sem_new(&acksem, 0) != 0) {
        LOGE(MODULE_NAME, "failed to allocate semaphore");
        goto err;
    }

    if (aos_mutex_new(&hal_send_mutex) != 0) {
        LOGE(MODULE_NAME, "Creating send mutex failed\r\n");
        goto err;
    }

    memset(ctx, 0, sizeof(encode_context_t));
    ctx->fcs = INIT_FCS16;

    LOGD(MODULE_NAME, "encoder init!\n");
    return 0;


err:
    if (aos_sem_is_valid(&acksem)) {
        aos_sem_free(&acksem);
    }

    if (aos_mutex_is_valid(&hal_send_mutex)) {
        aos_mutex_free(&hal_send_mutex);
    }

    return -1;
}

int32_t hdlc_encode_context_finalize(encode_context_t *ctx)
{
    if (NULL == ctx) {
        return -1;
    }

    if (aos_sem_is_valid(&acksem)) {
        aos_sem_free(&acksem);
    }

    if (aos_mutex_is_valid(&hal_send_mutex)) {
        aos_mutex_free(&hal_send_mutex);
    }

    memset(ctx, 0, sizeof(encode_context_t));

    LOGD(MODULE_NAME, "encoder finalize!\n");
    return 0;
}

ringbuf_t *ringbuf_create(int length)
{
    ringbuf_t *buffer = aos_malloc(sizeof(ringbuf_t));

    if (buffer == NULL) {
        return NULL;
    }

    buffer->length = length;
    buffer->head   = 0;
    buffer->tail   = 0;
    buffer->buffer = aos_malloc(buffer->length + 1);
    if (buffer->buffer == NULL) {
        aos_free(buffer);
        return NULL;
    }

    return buffer;
}

void ringbuf_destroy(ringbuf_t *ringbuf)
{
    if (ringbuf) {
        if (ringbuf->buffer) {
            aos_free(ringbuf->buffer);
        }
        aos_free(ringbuf);
    }
}

int ringbuf_available_read_space(ringbuf_t *buffer)
{
    if (buffer->head == buffer->tail) {
        return 0;
    } else if (buffer->head < buffer->tail) {
        return buffer->tail - buffer->head;
    } else {
        return buffer->length + 1 - (buffer->head - buffer->tail);
    }
}

void ringbuf_clear_all(ringbuf_t *buffer)
{
    buffer->head = buffer->tail = 0;
}

void ringbuf_clear_from_head(ringbuf_t *buffer, int len)
{
    if (len > ringbuf_available_read_space(buffer)) {
        ringbuf_clear_all(buffer);
        return;
    }

    buffer->head = (buffer->head + len) % (buffer->length + 1);
}

void ringbuf_clear_from_tail(ringbuf_t *buffer, int len)
{
    if (len > ringbuf_available_read_space(buffer)) {
        ringbuf_clear_all(buffer);
        return;
    }

    buffer->tail =
      (buffer->tail + (buffer->length + 1) - len) % (buffer->length + 1);
}

int ringbuf_write(ringbuf_t *buffer, uint8_t *data, uint32_t length)
{
    int i = 0;

    if (buffer == NULL || data == NULL || length == 0) {
        return -1;
    }

    if (ringbuf_empty(buffer)) {
        ringbuf_clear_all(buffer);
    }

    for (i = 0; i < length; i++) {

        if (ringbuf_full(buffer)) {
            LOGE(MODULE_NAME, "ringbuf full!");
            break;
        }

        buffer->buffer[buffer->tail] = data[i];

        buffer->tail++;
        buffer->tail %= (buffer->length + 1);
    }

    return i;
}

int ringbuf_checkbyte(ringbuf_t *buffer, uint8_t *target, uint32_t offset)
{
    int pos;

    if (buffer == NULL || target == NULL) {
        return -1;
    }

    if (ringbuf_empty(buffer)) {
        return 0;
    }

    if (offset < 0 || offset >= ringbuf_available_read_space(buffer)) {
        return -1;
    }

    pos     = (buffer->head + offset) % (buffer->length + 1);
    *target = buffer->buffer[pos];

    return 1;
}


int ringbuf_read(ringbuf_t *buffer, uint8_t *target, uint32_t amount)
{
    int size = 0;
    int i;

    if (buffer == NULL || target == NULL || amount == 0) {
        return -1;
    }

    if (ringbuf_empty(buffer)) {
        return 0;
    }

    size = MIN(amount, ringbuf_available_read_space(buffer));


    for (i = 0; i < size; i++) {
        target[i] = buffer->buffer[buffer->head];

        buffer->head++;
        buffer->head %= (buffer->length + 1);
    }

    return size;
}

int ringbuf_available_write_space(ringbuf_t *buffer)
{
    return (buffer->length - ringbuf_available_read_space(buffer));
}

bool ringbuf_full(ringbuf_t *buffer)
{
    return (ringbuf_available_write_space(buffer) == 0);
}

bool ringbuf_empty(ringbuf_t *buffer)
{
    return (ringbuf_available_read_space(buffer) == 0);
}

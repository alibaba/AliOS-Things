/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <string.h>
#include <aos/aos.h>
#include <aos/log.h>
#include "hdlc.h"

#define MODULE_NAME "hdlc"

enum {
    FLAG_SEQUENCE         = 0x7e,  // HDLC Flag value
    FLAG_SEQUENCE_ACK_REQ = 0x7b,  // HDLC Flag ack req value
    ESCAPE_SEQUENCE       = 0x7d,  // HDLC Escape value
};

enum {
    INIT_FCS16 = 0xffff,  // initial FCS value.
    GOOD_FCS16 = 0xf0b8,  // good FCS value.
};

/*
* FCS lookup table
*/
static const uint16_t fcs16tab[256] = {
    0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
    0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
    0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
    0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
    0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
    0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
    0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
    0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
    0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
    0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
    0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
    0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
    0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
    0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
    0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
    0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
    0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
    0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
    0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
    0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
    0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
    0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
    0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
    0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
    0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
    0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
    0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
    0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
    0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
    0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
    0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
    0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
};

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

    if (byte == FLAG_SEQUENCE ||
        byte == FLAG_SEQUENCE_ACK_REQ ||
        byte == ESCAPE_SEQUENCE) {
        ctx->buf[ctx->len++] = ESCAPE_SEQUENCE;
        byte ^= 0x20;
    }

    ctx->buf[ctx->len++] = byte;

    return 0;
}

static int32_t encode_hdlc(encode_context_t *ctx, const uint8_t *buf,
                           uint32_t len, bool ackreq, uint8_t seqno)
{
    int32_t i;
    uint16_t fcs;

    if (NULL == ctx || NULL == buf ||
        (len + 4) > sizeof(ctx->buf)) {
        return -1;
    }

    // init
    memset(ctx, 0, sizeof(encode_context_t));
    ctx->len = 0;
    ctx->fcs = INIT_FCS16;
    ctx->buf[ctx->len++] = ackreq ? FLAG_SEQUENCE_ACK_REQ : FLAG_SEQUENCE;
    ctx->buf[ctx->len++] = seqno;

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

static aos_mutex_t hdlc_mutex;
static char ack[] = "ack";
int32_t hdlc_uart_send(encode_context_t *ctx, uart_dev_t *uart, const void *data,
                       uint32_t size, uint32_t timeout, bool ackreq)
{
    uint32_t remain = size;
    uint32_t sent = 0;
    uint32_t step;
    char repsond[3];
    uint32_t raw_size;
    int ret = 0;
    int attempt = 0;
    uint8_t seqno = 0, seqused;
    bool frag = true;

    if (remain <= sizeof(ctx->buf) / 2) {
        frag = false;
    }

    aos_mutex_lock(&hdlc_mutex, AOS_WAIT_FOREVER);
    while (remain > 0) {
        if (++attempt > 4) {
            ret = -1;
            LOG("attempt fail\n");
            goto exit;
        }

        step = sizeof(ctx->buf) / 2 >  remain ? remain : sizeof(ctx->buf) / 2;

        if (!frag || (seqno == 0 && attempt == 1)) {
            seqused = 255;
        } else {
            seqused = seqno;
        }

        if (encode_hdlc(ctx, (uint8_t *) data + sent, step, ackreq, seqused) != 0) {
            ret = -1;
            LOG("encode fail\n");
            goto exit;
        }

        LOG("send: %s\n", ctx->buf);
        if (hal_uart_send(uart, ctx->buf, ctx->len, timeout) != 0) {
            ret = -1;
            LOG("uart send fail\n");
            goto exit;
        }

        if (!ackreq) {
            sent += step;
            remain -= step;
            attempt = 0;
            continue;
        }

        aos_msleep(20);

        if (hal_uart_recv_II(uart, (void *) repsond, sizeof(repsond),
                             &raw_size, timeout) != 0) {
            if (0 == raw_size) {
                LOG("no ack!");
                continue;
            }
        }

        LOG("recv %d %s %s\n", raw_size, repsond, ack);
        if (memcmp(repsond, ack, strlen(ack)) == 0) {
            sent += step;
            remain -= step;
            attempt = 0;
            seqno++;
            if (seqno == 255) {
                seqno = 0;
            }

            memset(repsond, 0, sizeof(repsond));

            LOG("ack recv remain %d sent %d\n", remain, sent);
        }
    }

exit:
    aos_mutex_unlock(&hdlc_mutex);
    LOG("trans finish\n");

    return ret;
}

static int32_t decode_byte(decode_context_t *ctx, uint8_t byte)
{
    if (NULL == ctx) {
        return -1;
    }

    if (ctx->len >= sizeof(ctx->buf)) {
        ctx->state = RECV_STATE_NO_SYNC;
        return -1;
    }

    ctx->buf[ctx->len] = byte;
    ctx->fcs = fcs16(ctx->fcs, &byte, 1);
    ctx->len++;

    return 0;
}

int32_t decode_hdlc(decode_context_t *ctx, const uint8_t *buf, uint32_t len)
{
    int32_t i;
    uint8_t byte;

    if (NULL == ctx) {
        return -1;
    }

    for (i = 0; i < len; i++) {
        byte = buf[i];

        switch (ctx->state) {
            case RECV_STATE_NO_SYNC:
                if (FLAG_SEQUENCE == byte ||
                    FLAG_SEQUENCE_ACK_REQ == byte) {
                    ctx->state = RECV_STATE_SYNC;
                    ctx->len = 0;
                    ctx->read = 0;
                    ctx->fcs = INIT_FCS16;
                    ctx->ackreq = (FLAG_SEQUENCE_ACK_REQ == byte);

                    // seqno field
                    if (++i >= len) {
                        return -1;
                    }

                    ctx->dup = false;
                    // a new start
                    if (255 == buf[i]) {
                        ctx->seq = 1;
                    } else if (ctx->seq == buf[i]) {
                        ctx->seq++;
                    } else {
                        ctx->dup = true;
                        if (!ctx->ackreq) {
                            return -1;
                        }
                    }
                }
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
                                ctx->len = 0;
                                ctx->fcs = INIT_FCS16;
                                return -1;
                            }

                            // ignore FCS
                            ctx->len -= 2;
                            ctx->state = RECV_STATE_NO_SYNC;
                            return 0;
                        }
                        break;

                    default:
                        decode_byte(ctx, byte);
                        break;
                }
                break;

            case RECV_STATE_ESCAPED:
                if (decode_byte(ctx, byte ^ 0x20) < 0) {
                    continue;
                }
                ctx->state = RECV_STATE_SYNC;
                break;

            default:
                ctx->state = RECV_STATE_NO_SYNC;
                break;
        }
    }

    return -1;
}


int32_t hdlc_uart_recv(decode_context_t *ctx, uart_dev_t *uart, void *data,
                       uint32_t expect_size, uint32_t *recv_size, uint32_t timeout)
{
    long long current_ms;
    uint32_t read_size, raw_size;
    int has_data = 0;
    int ret = -1;

    if (NULL == ctx) {
        return -1;
    }

    *recv_size = 0;

    if ((ctx->len - ctx->read) > 0) {
        read_size = (ctx->len - ctx->read) >= expect_size ? expect_size : (ctx->len - ctx->read);
        memcpy(data, ctx->buf + ctx->read, read_size);
        ctx->read += read_size;
        *recv_size += read_size;

        if (expect_size == read_size) {
            return 0;
        }

        expect_size -= read_size;
    }

    current_ms = aos_now_ms();

    aos_mutex_lock(&hdlc_mutex, AOS_WAIT_FOREVER);
    do {
        raw_size = 0;

        if (hal_uart_recv_II(uart, (void *) ctx->raw, sizeof(ctx->raw),
                             &raw_size, timeout) != 0) {
            if (0 == raw_size) {
                goto exit;
            }
        }

        if (raw_size > 0) {
            has_data++;

            LOGD(MODULE_NAME, "hdlc_uart_recv %d %s\n", raw_size, (char *) ctx->raw);
            if (decode_hdlc(ctx, ctx->raw, raw_size) == 0) {
                if ((ctx->len - ctx->read) > 0) {
                    read_size = (ctx->len - ctx->read) >= expect_size ? expect_size : (ctx->len - ctx->read);
                    memcpy(data + *recv_size, ctx->buf + ctx->read, read_size);
                    ctx->read += read_size;
                    *recv_size += read_size;
                }
                LOG("hdlc_uart_recv success\n");

                if (ctx->ackreq) {
                    if (hal_uart_send(uart, ack, strlen(ack), timeout) != 0) {
                        goto exit;
                    }
                }

                if (!ctx->dup) {
                    ret = 0;
                }
                goto exit;
            } else {
                LOG("decode fail\n");
            }
        }
    } while ((aos_now_ms() - current_ms) < timeout);

    if (has_data > 0) {
        LOGD(MODULE_NAME, "hdlc_uart_recv recv data but fail to decode %d\n", has_data);
    }

exit:
    aos_mutex_unlock(&hdlc_mutex);
    ctx->state = RECV_STATE_NO_SYNC;
    return ret;
}

int32_t hdlc_decode_context_init(decode_context_t *ctx)
{
    if (NULL == ctx) {
        return -1;
    }

    memset(ctx, 0, sizeof(decode_context_t));

    ctx->fcs = INIT_FCS16;
    ctx->state = RECV_STATE_NO_SYNC;

    if (0 != aos_mutex_new(&hdlc_mutex)) {
        LOGE(MODULE_NAME, "Creating hdlc mutex failed\r\n");
        return -1;
    }

    LOGD(MODULE_NAME, "decoder init!\n");
    return 0;
}

int32_t hdlc_decode_context_finalize(decode_context_t *ctx)
{
    if (NULL == ctx) {
        return -1;
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

    memset(ctx, 0, sizeof(encode_context_t));
    ctx->fcs = INIT_FCS16;

    LOGD(MODULE_NAME, "encoder init!\n");
    return 0;
}

int32_t hdlc_encode_context_finalize(encode_context_t *ctx)
{
    if (NULL == ctx) {
        return -1;
    }

    memset(ctx, 0, sizeof(encode_context_t));

    LOGD(MODULE_NAME, "encoder finalize!\n");
    return 0;
}


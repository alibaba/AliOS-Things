/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "lwip/udp.h"
#include "lwip/timeouts.h"
#include "lwip/debug.h"
#include "lwip/apps/tftp.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct tftp_state_s {
    const tftp_context_t *ctx;
    struct udp_pcb *upcb;
    void           *handle;
    tftp_done_cb   cb;
    ip_addr_t      addr;
    uint16_t       port;
    uint16_t       seq_expect;
    uint16_t       seq_last;
    uint16_t       seq;
    int            flen;
    uint16_t       tick;
    uint16_t       last_tick;
    uint16_t       retries;
    uint32_t       time;
} tftp_state_t;

static tftp_state_t tftp_state;

static void tftp_tmr(void* arg);
void tftp_send_error(struct udp_pcb *pcb, const ip_addr_t *addr, u16_t port,
                     tftp_error_t code, const char *str);
void tftp_send_ack(struct udp_pcb *pcb, const ip_addr_t *addr, u16_t port, u16_t blknum);

static void
close_handle(int err)
{
    tftp_state_t *pstate = &tftp_state;
    sys_untimeout(tftp_tmr, NULL);
    udp_remove(pstate->upcb);

    if (pstate->handle) {
        pstate->ctx->close(pstate->handle);
        pstate->handle = NULL;
    }
    LWIP_DEBUGF(TFTP_DEBUG | LWIP_DBG_STATE, ("tftp: closing\n"));

    if (pstate->cb != NULL)
        pstate->cb(err, err == 0 ? pstate->flen : -1);
    memset(pstate, 0, sizeof(tftp_state_t));
}

static void tftp_tmr(void* arg)
{
    tftp_state_t *pstate = &tftp_state;
    tftp_state.tick++;

    if (tftp_state.handle == NULL) {
        return;
    }

    sys_timeout(TFTP_TIMER_MSECS, tftp_tmr, NULL);

    if ((pstate->tick - pstate->last_tick) > (TFTP_TIMEOUT_MSECS / TFTP_TIMER_MSECS)) {
        if ((pstate->seq_expect > 1) && (pstate->retries < TFTP_MAX_RETRIES)) {
            LWIP_DEBUGF(TFTP_DEBUG | LWIP_DBG_STATE, ("tftp: timeout, retrying...............\n"));
            tftp_send_ack(pstate->upcb, &pstate->addr, pstate->port, pstate->seq_last);
            pstate->retries++;
        } else {
            tftp_send_error(tftp_state.upcb, &tftp_state.addr, pstate->port,
                    TFTP_ERROR_ILLEGAL_OPERATION, "wait packet timeout");
            LWIP_DEBUGF(TFTP_DEBUG | LWIP_DBG_STATE, ("tftp: timeout\n"));
            close_handle(-1);
        }
    }
}

static void recv(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
    tftp_state_t *pstate = &tftp_state;
    if (pstate->seq_expect == 0 && ip_addr_cmp(&pstate->addr, addr)){
        pstate->port = port;
        pstate->seq_expect = 1;
    }

    if ( port != pstate->port || !ip_addr_cmp(&pstate->addr, addr) ) {
        tftp_send_error(pstate->upcb, addr, port,
                TFTP_ERROR_UNKNOWN_TRFR_ID, "port or addr illegal");
        pbuf_free(p);
        return;
    }

    u16_t *sbuf = (u16_t *) p->payload;
    pstate->last_tick = pstate->tick;
    uint16_t opcode = PP_NTOHS(sbuf[0]);
    uint16_t blknum, blklen;
    switch (opcode) {
        case TFTP_DATA:
            blknum = PP_NTOHS(sbuf[1]);
            blklen = p->tot_len - TFTP_HEADER_LENGTH;
            if (blknum < pstate->seq_expect) {
                LWIP_DEBUGF(TFTP_DEBUG | LWIP_DBG_STATE,
                        ("received repeated block '%d', len='%u'\n", blknum, blklen));
                tftp_send_ack(pstate->upcb, &pstate->addr, port, blknum);
                pstate->seq_last = blknum;
                break;
            }

            if (blknum > pstate->seq_expect) {
                LWIP_DEBUGF(TFTP_DEBUG | LWIP_DBG_STATE,
                        ("received error block '%d', len='%u'\n", blknum, blklen ));
                tftp_send_error(pstate->upcb, addr, port,
                        TFTP_ERROR_ILLEGAL_OPERATION, "seqno error");
                close_handle(-1);
                break;
            }

            LWIP_DEBUGF(TFTP_DEBUG | LWIP_DBG_STATE,
                    ("received new block '%d', len='%u'\n", blknum, blklen));
            pbuf_header(p, -TFTP_HEADER_LENGTH);
            int wlen = pstate->ctx->write(pstate->handle, p);
            if (wlen != blklen) {
                tftp_send_error(pstate->upcb, addr, port, TFTP_ERROR_DISK_FULL, "disk full");
                LWIP_DEBUGF(TFTP_DEBUG | LWIP_DBG_STATE, ("write block failed\n"));
                close_handle(-1);
                break;
            }

            pstate->seq_last = blknum;
            pstate->flen += blklen;
            pstate->seq_expect++;
            tftp_send_ack(pstate->upcb, &pstate->addr, port, blknum);

            if (blklen < 512) {
                pstate->time = aos_now_ms() - pstate->time;
                LWIP_DEBUGF(TFTP_DEBUG | LWIP_DBG_STATE,
                        ("get succeed: received %u bytes in %u mS\n", pstate->flen, pstate->time));
                close_handle(0);
                break;
            }
            break;
        case TFTP_ERROR:
            LWIP_DEBUGF(TFTP_DEBUG | LWIP_DBG_STATE,
                    ("sever return error '%d', msg: '%s'\n", PP_NTOHS(sbuf[1]), &sbuf[2]));
            close_handle(-1);
            break;
        default:
            break;
    }
    pbuf_free(p);
}

static void* tftp_fopen(const char* fname, const char* mode, u8_t write)
{
    FILE *fp = NULL;

    if (strncmp(mode, "netascii", 8) == 0) {
        fp = fopen(fname, write == 0 ? "r" : "w");
    } else if (strncmp(mode, "octet", 5) == 0) {
        fp = fopen(fname, write == 0 ? "rb" : "wb");
    }
    return (void*)fp;
}

static void tftp_fclose(void* handle)
{
    fclose((FILE*)handle);
}

static int tftp_fread(void* handle, void* buf, int bytes)
{
    size_t readbytes;
    readbytes = fread(buf, 1, (size_t)bytes, (FILE*)handle);
    return (int)readbytes;
}

static int tftp_fwrite(void* handle, struct pbuf* p)
{
    char buff[512];
    size_t writebytes = -1;
    pbuf_copy_partial(p, buff, p->tot_len, 0);
    /* writebytes = fwrite(buff, 1, p->tot_len, (FILE *)handle); */
    writebytes = p->tot_len;
    return (int)writebytes;
}

const tftp_context_t client_ctx = {
    .open = tftp_fopen,
    .close = tftp_fclose,
    .read = tftp_fread,
    .write = tftp_fwrite
};

int tftp_client_get(const ip_addr_t *paddr, const char *fname,
                    tftp_context_t *ctx, tftp_done_cb cb)
{
    err_t ret;
    tftp_state_t *pstate = &tftp_state;

    pstate->time = aos_now_ms();
    pstate->handle = ctx->open(fname, "netascii", 1);
    if (pstate->handle == NULL) {
        LWIP_DEBUGF(TFTP_DEBUG | LWIP_DBG_STATE, ("error: open file '%s' failed\n", fname));
        return -1;
    }

    struct udp_pcb *pcb = udp_new_ip_type(IPADDR_TYPE_ANY);
    if (pcb == NULL) {
        return ERR_MEM;
    }

    uint16_t port = rand() % 16384 + 49152;
    ret = udp_bind(pcb, IP4_ADDR_ANY, port);
    if (ret != ERR_OK) {
        LWIP_DEBUGF(TFTP_DEBUG | LWIP_DBG_STATE, ("error: bind to port '%u' failed\n", port));
        udp_remove(pcb);
        return ret;
    }

    /* send RRQ packet */
    int pkt_len = 4 + strlen("netascii") + strlen(fname);
    struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, pkt_len, PBUF_RAM);
    if (p == NULL) {
        LWIP_DEBUGF(TFTP_DEBUG | LWIP_DBG_STATE, ("error: alloc pbuf failed\n"));
        udp_remove(pcb);
        return ERR_MEM;
    }

    char *payload = (char *)p->payload;
    memset(payload, 0, pkt_len);
    payload[0] = 0x00;
    payload[1] = TFTP_RRQ;
    memcpy(&payload[2], fname, strlen(fname));
    memcpy(&payload[3 + strlen(fname)], "netascii", strlen("netascii"));
    pstate->port = TFTP_PORT;
    ret = udp_sendto(pcb, p, paddr, pstate->port);
    if (ret != ERR_OK) {
        LWIP_DEBUGF(TFTP_DEBUG | LWIP_DBG_STATE, ("error: send RRQ to server failed\n"));
        pbuf_free(p);
        udp_remove(pcb);
        return ERR_MEM;
    }

    udp_recv(pcb, recv, NULL);
    pstate->tick = 0;
    pstate->last_tick = 0;
    sys_timeout(TFTP_TIMER_MSECS, tftp_tmr, NULL);
    pstate->upcb = pcb;
    pstate->ctx = ctx;
    pstate->cb = cb;
    memcpy(&pstate->addr, paddr, sizeof(*paddr));
    pstate->seq_expect = 0;
    pstate->flen = 0;
    pbuf_free(p);
    return 0;
}

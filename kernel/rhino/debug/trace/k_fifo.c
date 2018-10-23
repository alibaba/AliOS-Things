/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include "k_fifo.h"

/*
 * internal helper to calculate the unused elements in a fifo
 */
static uint32_t fifo_unused(struct k_fifo *fifo)
{
    return (fifo->mask + 1) - (fifo->in - fifo->out);
}

static int8_t is_power_of_2(uint32_t n)
{
    return (n != 0 && ((n & (n - 1)) == 0));
}

int8_t fifo_init(struct k_fifo *fifo, void *buffer, uint32_t size)
{
    /*
     * round down to the next power of 2, since our 'let the indices
     * wrap' technique works only in this case.
     */
    if (!is_power_of_2(size)) {

        return 1;
    }

    fifo->in = 0;
    fifo->out = 0;
    fifo->data = buffer;

    if (size < 2) {
        fifo->mask = 0;
        return 1;
    }

    fifo->mask = size - 1;
    fifo->free_bytes = size;
    fifo->size = size;

    return 0;
}

static void fifo_copy_in(struct k_fifo *fifo, const void *src,
                         uint32_t len, uint32_t off)
{
    uint32_t l;

    uint32_t size = fifo->mask + 1;

    off &= fifo->mask;

    l = fifo_min(len, size - off);

    memcpy((unsigned  char *)fifo->data + off, src, l);
    memcpy(fifo->data, (unsigned  char *)src + l, len - l);


}

uint32_t fifo_in(struct k_fifo *fifo, const void *buf, uint32_t len)
{
    uint32_t l;

    CPSR_ALLOC();

    RHINO_CRITICAL_ENTER();

    l = fifo_unused(fifo);

    if (len > l) {
        len = l;
    }

    fifo_copy_in(fifo, buf, len, fifo->in);
    fifo->in += len;

    fifo->free_bytes -= len;

    RHINO_CRITICAL_EXIT();
    return len;
}

static void kfifo_copy_out(struct k_fifo *fifo, void *dst,
                           uint32_t len, uint32_t off)
{
    uint32_t l;
    uint32_t size = fifo->mask + 1;

    off &= fifo->mask;

    l = fifo_min(len, size - off);

    memcpy(dst, (unsigned  char *)fifo->data + off, l);
    memcpy((unsigned  char *)dst + l, fifo->data, len - l);

}

static uint32_t internal_fifo_out_peek(struct k_fifo *fifo,
                                       void *buf, uint32_t len)
{
    uint32_t l;

    l = fifo->in - fifo->out;

    if (len > l) {
        len = l;
    }

    kfifo_copy_out(fifo, buf, len, fifo->out);
    return len;
}

uint32_t fifo_out_peek(struct k_fifo *fifo,
                       void *buf, uint32_t len)
{

    uint32_t ret_len;

    CPSR_ALLOC();

    RHINO_CRITICAL_ENTER();

    ret_len = internal_fifo_out_peek(fifo, buf, len);

    RHINO_CRITICAL_EXIT();

    return ret_len;

}

uint32_t fifo_out(struct k_fifo *fifo, void *buf, uint32_t len)
{
    CPSR_ALLOC();

    RHINO_CRITICAL_ENTER();

    len = internal_fifo_out_peek(fifo, buf, len);
    fifo->out += len;

    fifo->free_bytes += len;

    RHINO_CRITICAL_EXIT();

    return len;
}

uint32_t fifo_out_all(struct k_fifo *fifo, void *buf)
{
    uint32_t len;

    CPSR_ALLOC();

    RHINO_CRITICAL_ENTER();

    len = fifo->size - fifo->free_bytes;

    if (len == 0) {

        RHINO_CRITICAL_EXIT();
        return 0;
    }

    len = internal_fifo_out_peek(fifo, buf, len);
    fifo->out += len;

    fifo->free_bytes += len;

    RHINO_CRITICAL_EXIT();

    return len;
}


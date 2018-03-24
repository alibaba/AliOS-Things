/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>

kstat_t ringbuf_init(k_ringbuf_t *p_ringbuf, void *buf, size_t len, size_t type,
                     size_t block_size)
{
    p_ringbuf->type     = type;
    p_ringbuf->buf      = buf;
    p_ringbuf->end      = (uint8_t *)buf + len;
    p_ringbuf->blk_size = block_size;

    ringbuf_reset(p_ringbuf);

    return RHINO_SUCCESS;

}
static size_t ringbuf_headlen_compress(size_t head_len, uint8_t *cmp_buf)
{
    size_t   len_bytes = 0;
    uint8_t *p_len   = NULL;
    uint32_t be_len  = 0;

    be_len = krhino_ntohl(head_len);
    p_len = (uint8_t *)&be_len;
    len_bytes = COMPRESS_LEN(head_len);

    if (len_bytes == 1) {
        cmp_buf[0] = RINGBUF_LEN_1BYTE_MAXVALUE & p_len[3];
    } else if (len_bytes == 2) {
        cmp_buf[0] = RINGBUF_LEN_VLE_2BYTES | p_len[2];
        cmp_buf[1] = p_len[3];
    } else if (len_bytes == 3) {
        cmp_buf[0] = RINGBUF_LEN_VLE_3BYTES | p_len[1];
        cmp_buf[1] = p_len[2];
        cmp_buf[2] = p_len[3];
    }

    return len_bytes;
}

static size_t ringbuf_headlen_decompress(size_t buf_len, uint8_t *cmp_buf)
{
    size_t   data_len = 0;
    uint32_t be_len   = 0;
    uint8_t *len_buf  = (uint8_t *)&be_len;

    memcpy(&len_buf[sizeof(uint32_t) - buf_len], cmp_buf, buf_len);

    if (buf_len > 1) {
        len_buf[sizeof(uint32_t) - buf_len] &= RINGBUF_LEN_MASK_CLEAN_TWOBIT;
    }

    data_len = krhino_ntohl(be_len);

    return data_len;
}

kstat_t ringbuf_push(k_ringbuf_t *p_ringbuf, void *data, size_t len)
{
    size_t   len_bytes                   = 0;
    size_t   split_len                   = 0;
    uint8_t  c_len[RINGBUF_LEN_MAX_SIZE] = {0};

    if (ringbuf_is_full(p_ringbuf)) {
        return RHINO_RINGBUF_FULL;
    }

    if (p_ringbuf->type == RINGBUF_TYPE_FIX) {
        if (p_ringbuf->tail == p_ringbuf->end) {
            p_ringbuf->tail = p_ringbuf->buf;
        }

        memcpy(p_ringbuf->tail, data, p_ringbuf->blk_size);
        p_ringbuf->tail     += p_ringbuf->blk_size;
        p_ringbuf->freesize -= p_ringbuf->blk_size;
    } else {
        len_bytes = ringbuf_headlen_compress(len, c_len);
        if (len_bytes == 0 || len_bytes > RINGBUF_LEN_MAX_SIZE ) {
            return RHINO_INV_PARAM;
        }

        /* for dynamic length ringbuf */
        if (p_ringbuf->freesize < len_bytes + len ) {
            return RHINO_RINGBUF_FULL;
        }

        if (p_ringbuf->tail == p_ringbuf->end) {
            p_ringbuf->tail = p_ringbuf->buf;
        }

        /* copy length data to buffer */
        if (p_ringbuf->tail >= p_ringbuf->head &&
            (split_len = p_ringbuf->end - p_ringbuf->tail) < len_bytes && split_len > 0) {
            memcpy(p_ringbuf->tail, &c_len[0], split_len);
            len_bytes -= split_len;
            p_ringbuf->tail =  p_ringbuf->buf;
            p_ringbuf->freesize -= split_len;
        } else {
            split_len = 0;
        }

        if (len_bytes > 0) {
            memcpy(p_ringbuf->tail, &c_len[split_len], len_bytes);
            p_ringbuf->freesize -= len_bytes;
            p_ringbuf->tail += len_bytes;
        }

        /* copy data to ringbuf, if break by buffer end, split data and copy to buffer head*/
        split_len = 0;

        if (p_ringbuf->tail == p_ringbuf->end) {
            p_ringbuf->tail = p_ringbuf->buf;
        }

        if (p_ringbuf->tail >= p_ringbuf->head &&
            ((split_len = p_ringbuf->end - p_ringbuf->tail) < len) &&
            split_len > 0) {
            memcpy(p_ringbuf->tail, data, split_len);
            data = (uint8_t *)data + split_len;
            len -= split_len;
            p_ringbuf->tail =  p_ringbuf->buf;
            p_ringbuf->freesize -= split_len;
        }

        memcpy(p_ringbuf->tail, data, len);
        p_ringbuf->tail += len;
        p_ringbuf->freesize -= len;
    }

   return RHINO_SUCCESS;
}

kstat_t ringbuf_pop(k_ringbuf_t *p_ringbuf, void *pdata, size_t *plen)
{
    size_t   split_len = 0;
    uint8_t *data      = pdata;
    size_t   len       = 0;
    uint8_t  c_len[RINGBUF_LEN_MAX_SIZE] = {0};
    size_t   len_bytes = 0;

    if (ringbuf_is_empty(p_ringbuf)) {
        return RHINO_RINGBUF_EMPTY;
    }

    if (p_ringbuf->type == RINGBUF_TYPE_FIX) {
        if (p_ringbuf->head == p_ringbuf->end) {
            p_ringbuf->head = p_ringbuf->buf;
        }

        memcpy(pdata, p_ringbuf->head, p_ringbuf->blk_size);
        p_ringbuf->head += p_ringbuf->blk_size;
        p_ringbuf->freesize += p_ringbuf->blk_size;

        if (plen != NULL) {
            *plen = p_ringbuf->blk_size;
        }

        return RHINO_SUCCESS;
    } else {
        if (p_ringbuf->head == p_ringbuf->end) {
            p_ringbuf->head = p_ringbuf->buf;
        }

        /*decode length */
        if ((*p_ringbuf->head & RINGBUF_LEN_MASK_ONEBIT) == 0 ) {
            /*length use one byte*/
            len_bytes = 1;
        } else if ((*p_ringbuf->head & RINGBUF_LEN_MASK_TWOBIT) ==
                   RINGBUF_LEN_VLE_2BYTES) {
            /*length use 2 bytes*/
            len_bytes = 2;
        } else if ((*p_ringbuf->head & RINGBUF_LEN_MASK_TWOBIT) ==
                   RINGBUF_LEN_VLE_3BYTES) {
            /*length use 3 bytes*/
            len_bytes = 3;
        } else {
            return RHINO_INV_PARAM;
        }


        if (((split_len = p_ringbuf->end - p_ringbuf->head) < len_bytes) &&
            split_len > 0) {

            memcpy(&c_len[0], p_ringbuf->head, split_len);

            p_ringbuf->head      =  p_ringbuf->buf;
            p_ringbuf->freesize += split_len;
        } else {
            split_len = 0;
        }

        if (len_bytes - split_len > 0) {
            memcpy(&c_len[split_len], p_ringbuf->head, (len_bytes - split_len));
            p_ringbuf->head     += (len_bytes - split_len);
            p_ringbuf->freesize += (len_bytes - split_len);
        }

        *plen = len = ringbuf_headlen_decompress(len_bytes, c_len);

        if (p_ringbuf->head == p_ringbuf->end) {
            p_ringbuf->head = p_ringbuf->buf;
        }

        if (p_ringbuf->head > p_ringbuf->tail &&
            (split_len = p_ringbuf->end - p_ringbuf->head) < len) {
            memcpy(pdata, p_ringbuf->head, split_len);
            data = (uint8_t *)pdata + split_len;
            len -= split_len;
            p_ringbuf->head      = p_ringbuf->buf;
            p_ringbuf->freesize += split_len;
        }

        memcpy(data, p_ringbuf->head, len);
        p_ringbuf->head     += len;
        p_ringbuf->freesize += len;

        return RHINO_SUCCESS;

    }
}

uint8_t ringbuf_is_full(k_ringbuf_t *p_ringbuf)
{
    if (p_ringbuf->type == RINGBUF_TYPE_DYN && p_ringbuf->freesize < 2) {
        return 1;
    }

    if (p_ringbuf->type == RINGBUF_TYPE_FIX &&
        p_ringbuf->freesize < p_ringbuf->blk_size) {
        return 1;
    }

    return 0;
}

uint8_t ringbuf_is_empty(k_ringbuf_t *p_ringbuf)
{
    if (p_ringbuf->freesize == (size_t)(p_ringbuf->end - p_ringbuf->buf)) {
        return 1;
    }

    return 0;
}
/*external api*/

kstat_t ringbuf_reset(k_ringbuf_t *p_ringbuf)
{
    p_ringbuf->head = p_ringbuf->buf;
    p_ringbuf->tail = p_ringbuf->buf;
    p_ringbuf->freesize = p_ringbuf->end - p_ringbuf->buf;

    return RHINO_SUCCESS;
}

#if (RHINO_CONFIG_RINGBUF_VENDOR > 0)

kstat_t krhino_ringbuf_reset(k_ringbuf_t *p_ringbuf)
{
    CPSR_ALLOC();
    kstat_t err;

    NULL_PARA_CHK(p_ringbuf);

    RHINO_CRITICAL_ENTER();
    err = ringbuf_reset(p_ringbuf);
    RHINO_CRITICAL_EXIT();

    return err;
}
kstat_t krhino_ringbuf_init(k_ringbuf_t *p_ringbuf, void *buf, size_t len,
                            size_t type, size_t block_size)
{
    CPSR_ALLOC();
    kstat_t err;

    NULL_PARA_CHK(p_ringbuf);
    NULL_PARA_CHK(buf);

    if (len == 0 || (type != RINGBUF_TYPE_DYN && type != RINGBUF_TYPE_FIX)) {
        return RHINO_INV_PARAM;
    }

    if (type == RINGBUF_TYPE_FIX) {
        if (len == 0 || block_size == 0 || len % block_size) {
            return RHINO_INV_PARAM;
        }
    }

    RHINO_CRITICAL_ENTER();
    err = ringbuf_init(p_ringbuf, buf, len, type, block_size);
    RHINO_CRITICAL_EXIT();

    return err;
}

kstat_t krhino_ringbuf_push(k_ringbuf_t *p_ringbuf, void *data, size_t len)
{
    CPSR_ALLOC();
    kstat_t err;

    NULL_PARA_CHK(p_ringbuf);
    NULL_PARA_CHK(data);

    if (len <= 0 || len > RINGBUF_LEN_3BYTES_MAXVALUE ||
        (p_ringbuf->type == RINGBUF_TYPE_FIX && len != p_ringbuf->blk_size) ) {

        return RHINO_INV_PARAM;
    }

    RHINO_CRITICAL_ENTER();
    err = ringbuf_push(p_ringbuf, data, len);
    RHINO_CRITICAL_EXIT();

    return err;
}

kstat_t krhino_ringbuf_pop(k_ringbuf_t *p_ringbuf, void *pdata, size_t *plen)
{
    CPSR_ALLOC();
    kstat_t err;

    NULL_PARA_CHK(p_ringbuf);
    NULL_PARA_CHK(pdata);

    if (p_ringbuf->type == RINGBUF_TYPE_DYN && plen == NULL) {
        return RHINO_INV_PARAM;
    }

    RHINO_CRITICAL_ENTER();
    err = ringbuf_pop(p_ringbuf, pdata, plen);
    RHINO_CRITICAL_EXIT();

    return err;
}

uint8_t krhino_ringbuf_is_empty(k_ringbuf_t *p_ringbuf)
{
    CPSR_ALLOC();
    uint8_t empty;

    NULL_PARA_CHK(p_ringbuf);

    RHINO_CRITICAL_ENTER();
    empty = ringbuf_is_empty(p_ringbuf);
    RHINO_CRITICAL_EXIT();

    return empty;
}

uint8_t krhino_ringbuf_is_full(k_ringbuf_t *p_ringbuf)
{
    CPSR_ALLOC();
    uint8_t full;

    NULL_PARA_CHK(p_ringbuf);

    RHINO_CRITICAL_ENTER();
    full = ringbuf_is_full(p_ringbuf);
    RHINO_CRITICAL_EXIT();

    return full;
}
#endif


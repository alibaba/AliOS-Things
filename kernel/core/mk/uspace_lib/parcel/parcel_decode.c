/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>

#include "k_api.h"
#include "parcel.h"
#include "proto_buf.h"
#include "aos/errno.h"
#include "aos/kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

static void endian_copy(void *dest, void *src, size_t destsize, size_t srcsize)
{
#ifdef __BIG_ENDIAN__
    memcpy(dest, (char *)src + (srcsize - destsize), destsize);
#else
    memcpy(dest, src, destsize);
#endif
}

static int buf_read(aos_parcel_t *stream, uint8_t *buf, size_t count)
{
    uint8_t *source = (uint8_t *)stream->state + stream->rbytes;

    if (buf != NULL) {
        memcpy(buf, source, count);
    }

    return RHINO_SUCCESS;
}

int parcel_read(aos_parcel_t *stream, uint8_t *buf, size_t count)
{

    if (stream->callback == NULL) {
        stream->callback = &buf_read;
    }

    if (stream->max_size < stream->rbytes + count) {
        return -RHINO_PARCEL_BUF_EMPTY;
    }

    if (RHINO_SUCCESS != stream->callback(stream, buf, count)) {
        return -RHINO_PARCEL_BUF_EMPTY;
    }

    stream->rbytes += count;

    return RHINO_SUCCESS;
}

int parcel_decode_varint(aos_parcel_t *stream, uint64_t *dest)
{
    uint8_t byte;
    uint8_t bitpos = 0;

    *dest = 0;

    while (bitpos < 64 && parcel_read(stream, &byte, 1) == RHINO_SUCCESS) {
        *dest |= (uint64_t)(byte & 0x7F) << bitpos;
        bitpos += 7;

        if (!(byte & 0x80)) {
            return RHINO_SUCCESS;
        }
    }

    return -RHINO_INV_PARAM;
}

int parcel_dec_varint(aos_parcel_t *stream, int dest_data_size, void *dest)
{
    uint64_t temp;
    int status = parcel_decode_varint(stream, &temp);
    endian_copy(dest, &temp, dest_data_size, sizeof(temp));
    return status;
}

static int parcel_decode_varint32(aos_parcel_t *stream, uint32_t *dest)
{
    uint64_t temp;
    int status = parcel_decode_varint(stream, &temp);
    *dest = temp;
    return status;
}

int parcel_dec_svarint(aos_parcel_t *stream, int dest_data_size, void *dest)
{
    uint64_t temp;
    int status = parcel_decode_varint(stream, &temp);
    temp = (temp >> 1) ^ -(int64_t)(temp & 1);
    endian_copy(dest, &temp, dest_data_size, sizeof(temp));
    return status;
}

int parcel_dec_fixed32(aos_parcel_t *stream, void *dest)
{
#ifdef __BIG_ENDIAN__
    uint8_t bytes[4] = {0};
    int status = parcel_read(stream, bytes, 4);
    uint8_t bebytes[4] = {bytes[3], bytes[2], bytes[1], bytes[0]};
    memcpy(dest, bebytes, 4);
    return status;
#else
    return parcel_read(stream, (uint8_t *)dest, 4);
#endif
}

int parcel_dec_fixed64(aos_parcel_t *stream, void *dest)
{
#ifdef __BIG_ENDIAN__
    uint8_t bytes[8] = {0};
    int status = parcel_read(stream, bytes, 8);
    uint8_t bebytes[8] = {bytes[7], bytes[6], bytes[5], bytes[4],
                          bytes[3], bytes[2], bytes[1], bytes[0]
                         };
    memcpy(dest, bebytes, 8);
    return status;
#else
    return parcel_read(stream, (uint8_t *)dest, 8);
#endif
}

int parcel_dec_bytes(aos_parcel_t *stream, int dest_data_size, void *dest)
{
    parcel_bytes_array_t *x = (parcel_bytes_array_t *)dest;

    uint32_t temp;

    if (RHINO_SUCCESS != parcel_decode_varint32(stream, &temp)) {
        return -RHINO_INV_PARAM;
    }
    x->size = temp;

    if (x->size > dest_data_size) {
        return -RHINO_NO_MEM;
    }

    return parcel_read(stream, x->bytes, x->size);
}

int parcel_dec_string(aos_parcel_t *stream, int *dest_data_size, void *dest)
{
    uint32_t size;
    int status;
    if (RHINO_SUCCESS != parcel_decode_varint32(stream, &size)) {
        return -RHINO_INV_PARAM;
    }

    if (size > *dest_data_size) {
        return -RHINO_NO_MEM;
    }

    status = parcel_read(stream, (uint8_t *)dest, size);
    if (size < *dest_data_size) {
        /* add \0 when buffer is available */
        *((uint8_t *)dest + size) = 0;
    }
    *dest_data_size = size;
    return status;
}

int parcel_dec_all(aos_parcel_t *stream, int *dest_data_size, void *dest)
{
    int dlen = 0, len = *dest_data_size;

    while (parcel_dec_string(stream, &len, (char *)dest + dlen) == RHINO_SUCCESS) {
        dlen += len;

        if (*dest_data_size <= dlen) break;

        len = *dest_data_size - dlen;
    }

    *dest_data_size = dlen;

    return dlen == 0 ? -RHINO_INV_PARAM : RHINO_SUCCESS;
}

int decode_tag(aos_parcel_t *stream)
{
    uint32_t temp;
    if (RHINO_SUCCESS != parcel_decode_varint32(stream, &temp)) {
        if (stream->rbytes >= stream->max_size) {
            return -RHINO_PARCEL_BUF_EMPTY;
        } else {
            return -RHINO_SYS_FATAL_ERR;    /* It was error */
        }
    }

    if (temp == 0) {
        return -RHINO_INV_PARAM;    /* Special feature: allow 0-terminated messages. */
    }

    return RHINO_SUCCESS;
}

#ifdef __cplusplus
}
#endif

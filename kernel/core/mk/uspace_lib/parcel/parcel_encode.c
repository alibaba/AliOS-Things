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

static void endian_copy(void *dest, const void *src, size_t destsize, size_t srcsize)
{
#ifdef __BIG_ENDIAN__
    memcpy((char *)dest + (destsize - srcsize), src, srcsize);
#else
    memcpy(dest, src, srcsize);
#endif
}

static int buf_write(aos_parcel_t *stream, uint8_t *buf, size_t count)
{
    uint8_t *dest = (uint8_t *)stream->state + stream->wbytes;

    memcpy(dest, buf, count);

    return RHINO_SUCCESS;
}

int parcel_write(aos_parcel_t *stream, const uint8_t *buf, size_t count)
{
    if (stream->callback == NULL) {
        stream->callback = &buf_write;
    }

    if (stream->wbytes + count > stream->max_size) {
        return -RHINO_PARCEL_BUF_FULL;
    }

    if (RHINO_SUCCESS != stream->callback(stream, (uint8_t *)buf, count)) {
        return -RHINO_PARCEL_BUF_FULL;
    }

    stream->wbytes += count;

    return RHINO_SUCCESS;
}

int parcel_encode_varint(aos_parcel_t *stream, uint64_t value)
{
    uint8_t buffer[10];
    int i = 0;

    if (value == 0) {
        return parcel_write(stream, (uint8_t *)&value, 1);
    }

    while (value) {
        buffer[i] = (value & 0x7F) | 0x80;
        value >>= 7;
        i ++;
    }
    buffer[i - 1] &= 0x7F; /* Unset top bit on last byte */

    return parcel_write(stream, buffer, i);
}

int parcel_encode_tag(aos_parcel_t *stream, parcel_wire_type_t wiretype, int field_number)
{
    int tag = wiretype | (field_number << 3);

    return parcel_encode_varint(stream, tag);
}

int parcel_encode_string(aos_parcel_t *stream, const uint8_t *buffer, size_t size)
{
    if (RHINO_SUCCESS != parcel_encode_varint(stream, size)) {
        return -RHINO_PARCEL_BUF_FULL;
    }

    return parcel_write(stream, buffer, size);
}


int parcel_enc_varint(aos_parcel_t *stream, int src_data_size, const void *src)
{
    uint64_t value = 0;
    endian_copy(&value, src, sizeof(value), src_data_size);
    return parcel_encode_varint(stream, value);
}

int parcel_enc_svarint(aos_parcel_t *stream, int src_data_size, const void *src)
{
    uint64_t value = 0;
    uint64_t zigzagged;
    uint64_t signbitmask, xormask;

    endian_copy(&value, src, sizeof(value), src_data_size);

    signbitmask = (uint64_t)0x80 << (src_data_size * 8 - 8);
    xormask = ((uint64_t) - 1) >> (64 - src_data_size * 8);
    if (value & signbitmask) {
        zigzagged = ((value ^ xormask) << 1) | 1;
    } else {
        zigzagged = value << 1;
    }

    return parcel_encode_varint(stream, zigzagged);
}

int parcel_enc_fixed64(aos_parcel_t *stream, const void *src)
{
#ifdef __BIG_ENDIAN__
    uint8_t bytes[8] = {0};
    memcpy(bytes, src, 8);
    uint8_t lebytes[8] = {bytes[7], bytes[6], bytes[5], bytes[4],
                          bytes[3], bytes[2], bytes[1], bytes[0]
                         };
    return parcel_write(stream, lebytes, 8);
#else
    return parcel_write(stream, (uint8_t *)src, 8);
#endif
}

int parcel_enc_fixed32(aos_parcel_t *stream, const void *src)
{
#ifdef __BIG_ENDIAN__
    uint8_t bytes[4] = {0};
    memcpy(bytes, src, 4);
    uint8_t lebytes[4] = {bytes[3], bytes[2], bytes[1], bytes[0]};
    return parcel_write(stream, lebytes, 4);
#else
    return parcel_write(stream, (uint8_t *)src, 4);
#endif
}

int parcel_enc_bytes(aos_parcel_t *stream, const void *src)
{
    parcel_bytes_array_t *bytes = (parcel_bytes_array_t *)src;
    return parcel_encode_string(stream, bytes->bytes, bytes->size);
}

int parcel_enc_string(aos_parcel_t *stream, const void *src)
{
    return parcel_encode_string(stream, (uint8_t *)src, strlen((const char *)src));
}

#ifdef __cplusplus
}
#endif

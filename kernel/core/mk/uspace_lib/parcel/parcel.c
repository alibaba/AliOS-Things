/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>

#include "k_api.h"
#include "aos/errno.h"
#include "aos/kernel.h"

#include "parcel.h"
#include "proto_buf.h"

#ifdef __cplusplus
extern "C" {
#endif

static int parcel_stream_from_buffer(struct aos_parcel_t *stream, uint8_t *buf, size_t bufsize)
{
    stream->callback = NULL;
    stream->state = buf;
    stream->max_size = bufsize;
    stream->wbytes = 0;

    return RHINO_SUCCESS;
}

int aos_parcel_init(struct aos_parcel_t *data, void *buf, size_t blen)
{
    if (data == NULL || buf == NULL || blen == 0) {
        return -RHINO_INV_PARAM;
    }

    memset(data, 0, sizeof(*data));

    return parcel_stream_from_buffer(data, buf, blen);
}

int aos_parcel_put(int type, void *buf, size_t blen, struct aos_parcel_t *data)
{
    if (type < AP_UVARINT || type > AP_BUF ||
        buf == NULL || blen == 0 || data == NULL) {
        return -RHINO_INV_PARAM;
    }

    switch (type) {
        case AP_UVARINT:
            return parcel_enc_varint(data, blen, buf);
        case AP_SVARINT:
            return parcel_enc_svarint(data, blen, buf);
        case AP_FIX32:
            return parcel_enc_fixed32(data, buf);
        case AP_FIX64:
            return parcel_enc_fixed64(data, buf);
        case AP_BUF:
            return parcel_encode_string(data, buf, blen);
        default:
            return -RHINO_PARCEL_NOT_SUPPORT;
    }
}

int aos_parcel_get(int type, void *buf, size_t *blen, struct aos_parcel_t *data)
{
    if (type < AP_UVARINT || type > AP_ALL ||
        buf == NULL || blen == NULL || *blen == 0 || data == NULL) {
        return -RHINO_INV_PARAM;
    }

    switch (type) {
        case AP_UVARINT:
            return parcel_dec_varint(data, *blen, buf);
        case AP_SVARINT:
            return parcel_dec_svarint(data, *blen, buf);
        case AP_FIX32:
            return parcel_dec_fixed32(data, buf);
        case AP_FIX64:
            return parcel_dec_fixed64(data, buf);
        case AP_BUF:
            return parcel_dec_string(data, (int *)blen, buf);
        case AP_ALL:
            return parcel_dec_all(data, (int *)blen, buf);
        default:
            return -RHINO_PARCEL_NOT_SUPPORT;
    }
}

int aos_parcel_params_free(struct aos_parcel_t *data)
{
    if (data && data->state) {
        free(data->state);
    }
    if (data) {
        free(data);
    }

    return RHINO_SUCCESS;
}

#ifdef __cplusplus
}
#endif

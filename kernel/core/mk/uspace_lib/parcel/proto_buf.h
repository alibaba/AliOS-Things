#ifndef _PROTO_BUF_H_
#define _PROTO_BUF_H_

#include <stdio.h>
#include <stdint.h>

#include "parcel.h"

typedef struct {
    size_t size;
    uint8_t bytes[1];
} parcel_bytes_array_t;

typedef enum {
    PARCEL_WT_VARINT = 0,
    PARCEL_WT_64BIT  = 1,
    PARCEL_WT_STRING = 2,
    PARCEL_WT_32BIT  = 5
} parcel_wire_type_t;

/* encode interfaces */
int parcel_enc_varint(aos_parcel_t *stream, int src_data_size, const void *src);
int parcel_enc_svarint(aos_parcel_t *stream, int src_data_size, const void *src);
int parcel_encode_string(aos_parcel_t *stream, const uint8_t *buffer, size_t size);
int parcel_enc_fixed64(aos_parcel_t *stream, const void *src);
int parcel_enc_fixed32(aos_parcel_t *stream, const void *src);
int parcel_enc_bytes(aos_parcel_t *stream, const void *src);
int parcel_enc_string(aos_parcel_t *stream, const void *src);

/* decode interfaces */
int parcel_dec_varint(aos_parcel_t *stream, int dest_data_size, void *dest);
int parcel_dec_svarint(aos_parcel_t *stream, int dest_data_size, void *dest);
int parcel_dec_fixed32(aos_parcel_t *stream, void *dest);
int parcel_dec_fixed64(aos_parcel_t *stream, void *dest);
int parcel_dec_bytes(aos_parcel_t *stream, int dest_data_size, void *dest);
int parcel_dec_string(aos_parcel_t *stream, int *dest_data_size, void *dest);
int parcel_dec_all(aos_parcel_t *stream, int *dest_data_size, void *dest);

#endif

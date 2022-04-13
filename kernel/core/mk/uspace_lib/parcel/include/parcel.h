/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */

#ifndef __AOS_PARCEL__
#define __AOS_PARCEL__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct aos_parcel_t aos_parcel_t;

struct aos_parcel_t {
    int (*callback)(aos_parcel_t *stream, uint8_t *buf, size_t count);
    void *state; /* Free field for use by callback implementation */
    size_t max_size; /* Limit number of output bytes written (or use SIZE_MAX). */
    union {
        size_t wbytes;
        size_t rbytes;
    };
};

enum {  /* Parcel data type */
    AP_UVARINT = 0,
    AP_SVARINT,
    AP_FIX32,  /* use for float */
    AP_FIX64,  /* use for double */
    AP_BUF,
    AP_ALL,    /* just for variable structure,
                  refer to https://www.geeksforgeeks.org/variable-length-arrays-in-c-and-c */
};

int aos_parcel_init(aos_parcel_t *data, void *buf, size_t blen);
int aos_parcel_put(int type, void *buf, size_t blen, aos_parcel_t *data);
int aos_parcel_get(int type, void *buf, size_t *blen, aos_parcel_t *data);
int aos_parcel_params_free(aos_parcel_t *data);

#ifdef __cplusplus
}
#endif

#endif /* __AOS_PARCEL__ */

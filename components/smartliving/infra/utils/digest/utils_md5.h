/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef _IOTX_COMMON_MD5_H_
#define _IOTX_COMMON_MD5_H_

#include "iot_import.h"

#ifdef BUILD_AOS

#include "mbedtls/md5.h"

#define iot_md5_context       mbedtls_md5_context

#define utils_md5_init        mbedtls_md5_init
#define utils_md5_free        mbedtls_md5_free
#define utils_md5_clone       mbedtls_md5_clone
#define utils_md5_starts      mbedtls_md5_starts
#define utils_md5_update      mbedtls_md5_update
#define utils_md5_finish      mbedtls_md5_finish
#define utils_md5_process     mbedtls_md5_process
#define utils_md5             mbedtls_md5

int8_t utils_hb2hex(uint8_t hb);

void utils_md5_hexstr(unsigned char input[16],unsigned char output[32]);

#else

typedef struct {
    uint32_t total[2];          /*!< number of bytes processed  */
    uint32_t state[4];          /*!< intermediate digest state  */
    unsigned char buffer[64];   /*!< data block being processed */
} iot_md5_context;

/**
 * \brief          Initialize MD5 context
 *
 * \param ctx      MD5 context to be initialized
 */
void utils_md5_init(iot_md5_context *ctx);

/**
 * \brief          Clear MD5 context
 *
 * \param ctx      MD5 context to be cleared
 */
void utils_md5_free(iot_md5_context *ctx);

/**
 * \brief          Clone (the state of) an MD5 context
 *
 * \param dst      The destination context
 * \param src      The context to be cloned
 */
void utils_md5_clone(iot_md5_context *dst,
                     const iot_md5_context *src);

/**
 * \brief          MD5 context setup
 *
 * \param ctx      context to be initialized
 */
void utils_md5_starts(iot_md5_context *ctx);

/**
 * \brief          MD5 process buffer
 *
 * \param ctx      MD5 context
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 */
void utils_md5_update(iot_md5_context *ctx, const unsigned char *input, size_t ilen);

/**
 * \brief          MD5 final digest
 *
 * \param ctx      MD5 context
 * \param output   MD5 checksum result
 */
void utils_md5_finish(iot_md5_context *ctx, unsigned char output[16]);

/* Internal use */
void utils_md5_process(iot_md5_context *ctx, const unsigned char data[64]);

/**
 * \brief          Output = MD5( input buffer )
 *
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 * \param output   MD5 checksum result
 */
void utils_md5(const unsigned char *input, size_t ilen, unsigned char output[16]);


int8_t utils_hb2hex(uint8_t hb);

void utils_md5_hexstr(unsigned char input[16], unsigned char output[32]);

#endif

#endif

/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2018 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on all ESPRESSIF SYSTEMS products, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */
#include "mbedtls/sha256.h"
#include "mbedtls/md.h"

#ifndef _AWS_SIG_V4_SIGNING_H_
#define _AWS_SIG_V4_SIGNING_H_

#ifdef __cplusplus
extern "C" {
#endif


#define AWS_SIG_V4_BUFFER_SIZE (2048)

/**
 * @brief      Amazon Signature V4 signing context
 */
typedef struct {
    mbedtls_sha256_context  sha256_ctx;                     /*!< mbedtls SHA256 context */
    mbedtls_md_context_t    md_ctx;                         /*!< mbedtls HMAC context */
    char                    buffer[AWS_SIG_V4_BUFFER_SIZE]; /*!< Buffer to use for this library */
    int                     buffer_offset;                  /*!< The buffer offset have been used */
} aws_sig_v4_context_t;

/**
 * @brief      Amazon Signature V4 signing configurations
 */
typedef struct {
    const char *service_name;           /*!< AWS Service name, ex: polly */
    const char *region_name;            /*!< AWS Region name, ex: us-east-1 */
    const char *secret_key;             /*!< AWS IAM user secret key */
    const char *access_key;             /*!< AWS IAM user access key */
    const char *host;                   /*!< Current request host name, ex: polly.us-east-1.amazonaws.com*/
    const char *method;                 /*!< Current request method, ex: POST */
    const char *path;                   /*!< Current request path, ex: "/" or "/v1/speech" */
    const char *query;                  /*!< Current request query, ex: "" or "?key=value" */
    const char *amz_date;               /*!< AWS Date, format as `%Y%m%dT%H%M%SZ` */
    const char *date_stamp;             /*!< Datestamp, format as `%Y%m%d` */
    const char *signed_headers;         /*!< Singed headers */
    const char *canonical_headers;      /*!< Canonical headers headers */
    const char *payload;                /*!< Payload data */
    int         payload_len;            /*!< Payload length */
} aws_sig_v4_config_t;

/**
 * @brief      Create HTTP Header for Amazon Signature V4 signing
 *
 * @param      ctx     The context
 * @param      config  The configuration
 *
 * @return     The HTTP Header value of `Authorization`
 */
char *aws_sig_v4_signing_header(aws_sig_v4_context_t *ctx, aws_sig_v4_config_t *config);

#ifdef __cplusplus
}
#endif

#endif

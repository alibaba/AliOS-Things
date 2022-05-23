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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "freertos/FreeRTOS.h"
#include "aws_sig_v4_signing.h"

#define HASH_LENGHT (32)
#define HASH_HEX_LENGTH (65)
static const char *aws_algorithm = "AWS4-HMAC-SHA256";
#define GET_BUFFER(ctx) (ctx->buffer + ctx->buffer_offset)
#define NEXT_BUFFER(ctx, len) (ctx->buffer_offset += len)
#define REMAIN_BUFFER(ctx) (AWS_SIG_V4_BUFFER_SIZE - ctx->buffer_offset)

static void _hmac(char *output, const char *key, int key_size, const char *payload, int payload_size)
{
    mbedtls_md_context_t ctx;
    mbedtls_md_init(&ctx);
    mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), 1);
    mbedtls_md_hmac_starts(&ctx, (const unsigned char *) key, key_size);
    mbedtls_md_hmac_update(&ctx, (const unsigned char *) payload, payload_size);
    mbedtls_md_hmac_finish(&ctx, (unsigned char *)output);
    mbedtls_md_free(&ctx);
}
static void _hmac_hex(char *output, const char *key, int key_size, const char *payload, int payload_size)
{
    char hmac[HASH_LENGHT];
    _hmac(hmac, key, key_size, payload, payload_size);
    for (int i = 0; i < sizeof(hmac); i++) {
        sprintf(output + i * 2, "%02x", (int)hmac[i]);
    }
    output[HASH_HEX_LENGTH - 1] = 0;
}

static void _sha256_hex(char *output, const char *data, int data_len)
{
    char sha256_res[HASH_LENGHT];
    mbedtls_sha256_context ctx;
    mbedtls_sha256_init(&ctx);
    mbedtls_sha256_starts(&ctx, 0); /* SHA-256, not 224 */
    mbedtls_sha256_update(&ctx, (const unsigned char*)data, data_len);
    mbedtls_sha256_finish(&ctx, (unsigned char *)sha256_res);
    mbedtls_sha256_free(&ctx);
    for (int i = 0; i < sizeof(sha256_res); i++) {
        sprintf(output + i * 2, "%02x", (int)sha256_res[i]);
    }
    output[HASH_HEX_LENGTH - 1] = 0;
}

static void _get_signature_key(char *output, const char *aws4_key, const char *date_stamp, const char *region_name, const char *service_name)
{
    char k_date[HASH_LENGHT], k_region[HASH_LENGHT], k_service[HASH_LENGHT];
    _hmac(k_date, aws4_key, strlen(aws4_key), date_stamp, strlen(date_stamp));
    _hmac(k_region, k_date, HASH_LENGHT, region_name, strlen(region_name));
    _hmac(k_service, k_region, HASH_LENGHT, service_name, strlen(service_name));
    _hmac(output, k_service, HASH_LENGHT, "aws4_request", strlen("aws4_request"));
}


char *aws_sig_v4_signing_header(aws_sig_v4_context_t *ctx, aws_sig_v4_config_t *config)
{
    memset(ctx, 0, sizeof(aws_sig_v4_context_t));

    char *payload_hash = GET_BUFFER(ctx);
    _sha256_hex(payload_hash, config->payload, config->payload_len);
    NEXT_BUFFER(ctx, HASH_HEX_LENGTH);

    char *canonical_request = GET_BUFFER(ctx);
    char separate = config->signed_headers && strlen(config->signed_headers) > 0 ? ';' : 0;
    int canonical_request_len = snprintf(canonical_request,
                                         REMAIN_BUFFER(ctx),
                                         "%s\n%s\n%s\n%shost:%s\nx-amz-date:%s\n\n%s%chost;x-amz-date\n%s",
                                         config->method,
                                         config->path,
                                         config->query,
                                         config->canonical_headers,
                                         config->host,
                                         config->amz_date,
                                         config->signed_headers,
                                         separate,
                                         payload_hash);
    NEXT_BUFFER(ctx, canonical_request_len + 1);

    char *canonical_request_sha256 = GET_BUFFER(ctx);
    _sha256_hex(canonical_request_sha256, canonical_request, canonical_request_len);
    NEXT_BUFFER(ctx, HASH_HEX_LENGTH);

    char *credential_scope = GET_BUFFER(ctx);
    int credential_scope_len = snprintf(credential_scope,
                                        REMAIN_BUFFER(ctx),
                                        "%s/%s/%s/aws4_request",
                                        config->date_stamp,
                                        config->region_name,
                                        config->service_name);
    NEXT_BUFFER(ctx, credential_scope_len + 1);

    char *aws4_key = GET_BUFFER(ctx);
    int aws4_key_len = snprintf(aws4_key,
                                REMAIN_BUFFER(ctx),
                                "AWS4%s",
                                config->secret_key);
    NEXT_BUFFER(ctx, aws4_key_len);

    char *signing_key = GET_BUFFER(ctx);
    _get_signature_key(signing_key, aws4_key, config->date_stamp, config->region_name, config->service_name);
    NEXT_BUFFER(ctx, HASH_LENGHT);

    char *string_to_sign = GET_BUFFER(ctx);
    int string_to_sign_len = snprintf(string_to_sign,
                                      REMAIN_BUFFER(ctx),
                                      "%s\n%s\n%s\n%s",
                                      aws_algorithm,
                                      config->amz_date,
                                      credential_scope,
                                      canonical_request_sha256);
    NEXT_BUFFER(ctx, string_to_sign_len + 1);

    char *signature = GET_BUFFER(ctx);
    _hmac_hex(signature, signing_key, HASH_LENGHT, string_to_sign, string_to_sign_len);
    NEXT_BUFFER(ctx, HASH_HEX_LENGTH);

    char *authorization_header = GET_BUFFER(ctx);
    snprintf(authorization_header,
             REMAIN_BUFFER(ctx),
             "%s Credential=%s/%s, SignedHeaders=%s%chost;x-amz-date, Signature=%s",
             aws_algorithm,
             config->access_key,
             credential_scope,
             config->signed_headers,
             separate,
             signature);
    return authorization_header;
}

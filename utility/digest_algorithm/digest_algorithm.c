/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include "digest_algorithm.h"
#include "md5.h"
#include "sha2.c"
#include "hmac.c"
#include "aos/aos.h"

void *digest_md5_init(void)
{
    MD5_CTX *ctx = (MD5_CTX *) aos_malloc(sizeof(MD5_CTX));
    if (NULL == ctx) {
        return NULL;
    }

    MD5_Init(ctx);
    return ctx;
}

int digest_md5_update(void *md5, const void *data, uint32_t length)
{
    MD5_Update(md5, data, length);
    return 0;
}

int digest_md5_final(void *md5, unsigned char *digest)
{
    MD5_Final(digest, md5);
    aos_free(md5);
    return 0;
}

int digest_md5(const void *data, uint32_t length, unsigned char *digest)
{
    MD5_CTX *ctx = (MD5_CTX *) aos_malloc(sizeof(MD5_CTX));
    if (NULL == ctx) {
        return -1;
    }

    MD5_Init(ctx);
    MD5_Update(ctx, data, length);
    MD5_Final(digest, ctx);
    aos_free(ctx);
    return 0;
}

int digest_md5_file(const char *path, unsigned char *md5)
{
    int bytes;
    unsigned char data[512];
    unsigned char digest[16];
    int i, fd;

    fd = aos_open(path, O_RDONLY);
    if (fd < 0) {
        return -1;
    }

    MD5_CTX *ctx = (MD5_CTX *) aos_malloc(sizeof(MD5_CTX));
    if (NULL == ctx) {
        aos_close(fd);
        return -1;
    }

    MD5_Init(ctx);

    do {
        bytes = aos_read(fd, data, sizeof(data));
        if (bytes > 0) {
            MD5_Update(ctx, data, bytes);
        }
    } while (bytes == sizeof(data));

    MD5_Final(digest, ctx);

    for (i = 0; i < 16; i++) {
        sprintf((char *)&md5[i * 2], "%02x", digest[i]);
    }

    aos_close(fd);
    aos_free(ctx);
    return 0;
}

void *digest_sha256_init(void)
{
    SHA256_CTX *ctx = (SHA256_CTX *) aos_malloc(sizeof(SHA256_CTX));
    if (NULL == ctx) {
        return NULL;
    }

    SHA256_Init(ctx);

    return ctx;
}

int digest_sha256_update(void *sha256, const void *data, uint32_t length)
{
    SHA256_Update(sha256, data, length);
    return 0;
}

int digest_sha256_final(void *sha256, unsigned char *digest)
{
    SHA256_Final(digest, sha256);
    aos_free(sha256);
    return 0;
}

int digest_sha256(const void *data, uint32_t length, unsigned char *digest)
{
    SHA256_CTX *ctx = (SHA256_CTX *) aos_malloc(sizeof(SHA256_CTX));
    if (NULL == ctx) {
        return -1;
    }

    memset(ctx, 0, sizeof(SHA256_CTX));

    SHA256_Init(ctx);
    SHA256_Update(ctx, data, length);
    SHA256_Final(digest, ctx);
    aos_free(ctx);

    return 0;
}

void *digest_sha384_init(void)
{
    SHA384_CTX *ctx = (SHA384_CTX *) aos_malloc(sizeof(SHA384_CTX));
    if (NULL == ctx) {
        return NULL;
    }

    SHA384_Init(ctx);

    return ctx;
}

int digest_sha384_update(void *sha384, const void *data, uint32_t length)
{
    SHA384_Update(sha384, data, length);
    return 0;
}

int digest_sha384_final(void *sha384, unsigned char *digest)
{
    SHA384_Final(digest, sha384);
    aos_free(sha384);
    return 0;
}

int digest_sha384(const void *data, uint32_t length, unsigned char *digest)
{
    SHA384_CTX *ctx = (SHA384_CTX *) aos_malloc(sizeof(SHA384_CTX));
    if (NULL == ctx) {
        return -1;
    }

    SHA384_Init(ctx);
    SHA384_Update(ctx, data, length);
    SHA384_Final(digest, ctx);
    aos_free(ctx);

    return 0;
}

void *digest_sha512_init(void)
{
    SHA512_CTX *ctx = (SHA512_CTX *) aos_malloc(sizeof(SHA512_CTX));
    if (NULL == ctx) {
        return NULL;
    }

    SHA512_Init(ctx);

    return ctx;
}

int digest_sha512_update(void *sha512, const void *data, uint32_t length)
{
    SHA512_Update(sha512, data, length);
    return 0;
}

int digest_sha512_final(void *sha512, unsigned char *digest)
{
    SHA512_Final(digest, sha512);
    aos_free(sha512);
    return 0;
}

int digest_sha512(const void *data, uint32_t length, unsigned char *digest)
{
    SHA512_CTX *ctx = (SHA512_CTX *) aos_malloc(sizeof(SHA512_CTX));
    if (NULL == ctx) {
        return -1;
    }

    SHA512_Init(ctx);
    SHA512_Update(ctx, data, length);
    SHA512_Final(digest, ctx);
    aos_free(ctx);

    return 0;
}

int digest_hmac(enum digest_type type, const unsigned char *data,
                uint32_t data_len, const unsigned char *key, uint32_t key_len,
                unsigned char *digest)
{
    switch (type) {
        case DIGEST_TYPE_MD5:
            return digest_hmac_md5(data, data_len, key, key_len, digest);

        case DIGEST_TYPE_SHA256:
            break;

        case DIGEST_TYPE_SHA384:
            break;

        case DIGEST_TYPE_SHA512:
            break;

        default:
            break;
    }

    return -1;
}

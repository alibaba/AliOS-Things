/**
 * Copyright (C) 2015 The YunOS Project. All rights reserved.
 **/

#ifndef _CRYPTO_DRV_H_
#define _CRYPTO_DRV_H_

#include "device.h"

#ifdef CONFIG_CRYPTO_DRV_DBG
#define CRYPTO_DRV_DBG_E(_f, _a ...)  tee_dbg_print(ERR, _f, ##_a)
#define CRYPTO_DRV_DBG_I(_f, _a ...)  tee_dbg_print(INF, _f, ##_a)
#else
#define CRYPTO_DRV_DBG_E(_f, _a ...)
#define CRYPTO_DRV_DBG_I(_f, _a ...)
#endif


enum {
#ifdef RSA_SUPPORT
        CMD_RSA_MODE_EXP = 0,
#endif
#ifdef AES_SUPPORT
        CMD_AES_BUF_SIZE,
        CMD_AES_INIT,
        CMD_AES_PROCESS,
        CMD_AES_FINAL,
#endif
#ifdef SHA_SUPPORT
        CMD_SHA_BUF_SIZE,
        CMD_SHA_INIT,
        CMD_SHA_UPDATE,
        CMD_SHA_FINAL,
#endif
#ifdef TRNG_SUPPORT
        CMD_GET_RAND,
#endif

        CMD_MAX_END = 0xffff,
};

#define CRYPTO_DEV_NAME     "CRYPTO"

#define INIT_MAGIC(m)   (m = 0x11223344)
#define VALID_MAGIC(m)  (0x11223344 == m)

#ifdef RSA_SUPPORT
typedef struct _rsa_drv_ioctl_t {
    uint32_t magic;
    int32_t words;
    int32_t d_valid_bits;
    uint32_t *m;
    uint32_t *d;
    uint32_t *c;
    uint32_t *b;
    uint32_t *r;
} rsa_drv_ioctl_t;
extern int32_t rsa_drv_suspend(dev_t *dev, int32_t state);
extern int32_t rsa_drv_resume(dev_t *dev);
extern int32_t rsa_drv_mode_exp(void *arg);
#endif

#ifdef AES_SUPPORT
typedef struct _aes_drv_ioctl_buf_size_t {
    uint32_t magic;
    int32_t ctx_size;
} aes_drv_ioctl_buf_size_t;
typedef struct _aes_drv_ioctl_init_t {
    uint32_t magic;

    int32_t mode;
    bool is_enc;
    uint32_t *key;
    int32_t keylen;
    uint32_t *iv;
    void *ctx;
} aes_drv_ioctl_init_t;
typedef struct _aes_drv_ioctl_process_t {
    uint32_t magic;

    uint32_t *src_blk;
    uint32_t *dst_blk;
    uint32_t size;
    void *ctx;
} aes_drv_ioctl_process_t;
typedef struct _aes_drv_ioctl_final_t {
    uint32_t magic;

    void *ctx;
} aes_drv_ioctl_final_t;
extern int32_t aes_drv_suspend(dev_t *dev, int32_t state);
extern int32_t aes_drv_resume(dev_t *dev);
extern int32_t aes_drv_get_ctx_size(void *arg);
extern int32_t aes_drv_init(void *arg);
extern int32_t aes_drv_process(void *arg);
extern int32_t aes_drv_finish(void *arg);
#endif

#ifdef SHA_SUPPORT
typedef enum _sha_mode_t {
    SHA_MODE_INVALID = 0,
    SHA_MODE_SHA1,
    SHA_MODE_SHA256,
    SHA_MODE_SHA224,
    SHA_MODE_SHA512,
    SHA_MODE_SHA384,
    SHA_MODE_MD5_SW,
    SHA_MODE_MAX,
} sha_mode_t;

typedef struct _sha_drv_ioctl_buf_size_t {
    uint32_t magic;
    uint32_t ctx_size;
} sha_drv_ioctl_buf_size_t;

typedef struct _sha_drv_ioctl_init_t {
    uint32_t magic;
    void *ctx;
    sha_mode_t mode;
} sha_drv_ioctl_init_t;

typedef struct _sha_drv_ioctl_proc_t {
    uint32_t magic;
    void *ctx;
    sha_mode_t mode;
    uint32_t src_addr;
    uint32_t src_size;
} sha_drv_ioctl_proc_t;

typedef struct _sha_drv_ioctl_final_t {
    uint32_t magic;
    void *ctx;
    sha_mode_t mode;
    uint32_t dst_addr;
} sha_drv_ioctl_final_t;

extern int32_t sha_drv_suspend(dev_t *dev, int32_t state);
extern int32_t sha_drv_resume(dev_t *dev);
extern int32_t sha_drv_get_ctx_size(void *arg);
extern int32_t sha_drv_init(void *arg);
extern int32_t sha_drv_process(void *arg);
extern int32_t sha_drv_finish(void *arg);
#endif

#ifdef TRNG_SUPPORT
typedef struct _trng_drv_ioctl_t {
    uint32_t magic;
    uint8_t *buf;
    size_t  len;
} trng_drv_ioctl_t;

extern int32_t trng_random_get(void *arg);
extern int32_t trng_drv_suspend(dev_t *dev, int32_t state);
int32_t trng_drv_resume(dev_t *dev);
#endif

#endif /* _CRYPTO_H_ */

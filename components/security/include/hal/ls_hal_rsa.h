/*
 * Copyright (C) 2016-2019 Alibaba Group Holding Limited
 */

#ifndef __LS_HAL_RSA_H__
#define __LS_HAL_RSA_H__

/********************************************************************/
/*              HASH HAL CRYPTO API                                 */
/********************************************************************/

#define MIN_RSA_KEY_SIZE   (256)
#define MAX_RSA_KEY_SIZE   (2048)

/*
 * hal rsa key pair struct definition
 * 
 * key_bytes: key size
 * n/e/d/p/q/dp/dq/qp: rsa key pair elements
 *
 */
typedef struct _rsa_priv_key_t {
    size_t   key_bytes;
    uint8_t  n[MAX_RSA_KEY_SIZE >> 3];
    uint8_t  e[MAX_RSA_KEY_SIZE >> 3];
    uint8_t  d[MAX_RSA_KEY_SIZE >> 3];
    uint8_t  p[MAX_RSA_KEY_SIZE >> 4];
    uint8_t  q[MAX_RSA_KEY_SIZE >> 4];
    uint8_t  dp[MAX_RSA_KEY_SIZE >> 4];
    uint8_t  dq[MAX_RSA_KEY_SIZE >> 4];
    uint8_t  qp[MAX_RSA_KEY_SIZE >> 4];
} hal_rsa_keypair_t;

/*
 * hal rsa public key struct definition
 *
 * keybytes: key size
 * n/e: rsa public key elements
 *
 */
typedef struct _hal_rsa_pubkey_t {
    size_t   key_bytes;
    uint8_t  n[MAX_RSA_KEY_SIZE >> 3];
    uint8_t  e[MAX_RSA_KEY_SIZE >> 3];
} hal_rsa_pubkey_t;

/*
 * Return the hal rsa ctx size for hal rsa process
 * the hal ctx should defined by the user according 
 * to the user's implementation
 */
int ls_hal_rsa_get_ctx_size(void);

/*
 * Initialize the hal_rsa_ctx
 *
 * context[in]: hal_ctx, must be pre-allocated by the caller,
 *              the size is got through ls_hal_rsa_get_ctx_size()
 */
int ls_hal_rsa_init(const void *context);

/*
 * Initialize the ctx for rsa public key operation
 *
 * ctx[in]: hal_ctx, must be pre-allocated by the caller,
 *          the size is got through ls_hal_rsa_get_ctx_size()
 * keybits: keysize in bit length
 * n/e:     elements required to generate rsa public key
 */
int ls_hal_rsa_init_pubkey(void *ctx, size_t keybits,
                           const uint8_t *n, size_t n_size,
                           const uint8_t *e, size_t e_size);

/*
 * Initialize the ctx for rsa keypair operation
 *
 * ctx[in]: hal_ctx, must be pre-allocated by the caller,
 *          the size is got through ls_hal_rsa_get_ctx_size()
 * keybits: keysize in bit length
 * n/e/d/p/q/dp/dq/qp: rsa key pair elements
 */
int ls_hal_rsa_init_keypair(void *context, size_t keybits,
                            const uint8_t *n, size_t n_size,
                            const uint8_t *e, size_t e_size,
                            const uint8_t *d, size_t d_size,
                            const uint8_t *p, size_t p_size,
                            const uint8_t *q, size_t q_size,
                            const uint8_t *dp, size_t dp_size,
                            const uint8_t *dq, size_t dq_size,
                            const uint8_t *qp, size_t qp_size);

/* 
 * RSA public key operation
 *
 * context[in]: hal_ctx, must be pre-allocated by the caller,
 *              the size is got through ls_hal_rsa_get_ctx_size()
 * src[in]:     input buffer
 * dst[out]:    output buffer size
 * size[in]:    output buffer size (must at least keybits >> 3)
 *              from ls_hal_rsa_init_keypair
 */
int ls_hal_rsa_public(const void *context,
                      const uint8_t *src, uint8_t *dst,
                      size_t size);

/* 
 * RSA private key operation
 *
 * context[in]: hal_ctx, must be pre-allocated by the caller,
 *              the size is got through ls_hal_rsa_get_ctx_size()
 * f_rng:       Random function
 * src[in]:     input buffer
 * dst[in]:     output buffer
 * size[in]:    output buffer size (must at least keybits >> 3)
 *              from ls_hal_rsa_init_keypair
 */
int ls_hal_rsa_private(const void *context,
           int (*f_rng)(void *, uint8_t *, size_t),
           const uint8_t *src, uint8_t *dst, size_t size);

#endif // __LS_HAL_RSA_H__

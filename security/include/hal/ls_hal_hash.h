/*
 * Copyright (C) 2016-2019 Alibaba Group Holding Limited
 */

#ifndef __LS_HAL_HASH_H__
#define __LS_HAL_HASH_H__

/********************************************************************/
/*              HASH HAL CRYPTO API                                 */
/********************************************************************/

/*
 * Return the hal md5 ctx size for hal md5 process
 * the hal ctx should defined by the user according 
 * to the user's implementation
 */
int ls_hal_md5_get_size(void);

/*
 * Initialize hal md5 ctx
 *
 * ctx[in]: hal_ctx, must be pre-allocated by the caller,
 *          the size is got through ls_hal_md5_get_size()
 */
int ls_hal_md5_init(void *ctx);

/*
 * HAL MD5 update process
 *
 * ctx[in]: hal_ctx, must be pre-allocated by the caller,
 *          the size is got through ls_hal_md5_get_size()
 * src[in]: input buffer
 * size[in]: the number of bytes to process
 */
int ls_hal_md5_update(void *ctx, const uint8_t *src, size_t size);

/*
 * HAL MD5 finish
 *
 * ctx[in]: hal_ctx, must be pre-allocated by the caller,
 *          the size is got through ls_hal_md5_get_size()
 * digest[out]: the digest output(should always be 16 bytes)
 */
int ls_hal_md5_finish(void *ctx, uint8_t digest[16]);

/*
 * Return the hal sha1 ctx size for hal sha1 process
 * the hal ctx should defined by the user according 
 * to the user's implementation
 */
int ls_hal_sha1_get_size(void);

/*
 * Initialize hal sha1 ctx
 *
 * ctx[in]: hal_ctx, must be pre-allocated by the caller,
 *          the size is got through ls_hal_sha1_get_size()
 */
int ls_hal_sha1_init(void *ctx);

/*
 * HAL sha1 update process
 *
 * ctx[in]: hal_ctx, must be pre-allocated by the caller,
 *          the size is got through ls_hal_sha1_get_size()
 * src[in]: input buffer
 * size[in]: the number of bytes to process
 */
int ls_hal_sha1_update(void *ctx, const uint8_t *src, size_t size);

/*
 * HAL SHA1 finish
 *
 * ctx[in]: hal_ctx, must be pre-allocated by the caller,
 *          the size is got through ls_hal_sha1_get_size()
 * digest[out]: the digest output(should always be 20 bytes)
 */
int ls_hal_sha1_finish(void *ctx, uint8_t digest[20]);

/*
 * Return the hal sha256 ctx size for hal sha256 process
 * the hal ctx should defined by the user according 
 * to the user's implementation
 */
int ls_hal_sha256_get_size(void);

/*
 * Initialize hal sha256 ctx
 *
 * ctx[in]: hal_ctx, must be pre-allocated by the caller,
 *          the size is got through ls_hal_sha256_get_size()
 */
int ls_hal_sha256_init(void *ctx);

/*
 * HAL sha256 update process
 *
 * ctx[in]: hal_ctx, must be pre-allocated by the caller,
 *          the size is got through ls_hal_sha256_get_size()
 * src[in]: input buffer
 * size[in]: the number of bytes to process
 */
int ls_hal_sha256_update(void *ctx, const uint8_t *src, size_t size);

/*
 * HAL SHA256 finish
 *
 * ctx[in]: hal_ctx, must be pre-allocated by the caller,
 *          the size is got through ls_hal_sha1_get_size()
 * digest[out]: the digest output(should always be 32 bytes)
 */
int ls_hal_sha256_finish(void *ctx, uint8_t digest[32]);

#endif // __LS_HAL_HASH_H__

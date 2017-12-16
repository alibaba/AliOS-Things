/**
 * Copyright (C) 2015 The YunOS Project. All rights reserved.
 **/

#include "hal_memmap.h"
#include "sha_drv.h"
#include "device.h"
#include "tee_dbg.h"
#include "crypto_drv.h"

#define SHA_MAX_TIME     20
#define SHA_MAX_SIZE     64

#define BLOCK_MAX_SIZE   128

static uint8_t sha1_hash[20] = {
    0xda, 0x39, 0xa3, 0xee, 0x5e, 0x6b, 0x4b, 0x0d, 0x32, 0x55,
    0xbf, 0xef, 0x95, 0x60, 0x18, 0x90, 0xaf, 0xd8, 0x07, 0x09
};
static uint8_t sha224_hash[28] = {
    0xd1, 0x4a, 0x02, 0x8c, 0x2a, 0x3a, 0x2b, 0xc9, 0x47, 0x61,
    0x02, 0xbb, 0x28, 0x82, 0x34, 0xc4, 0x15, 0xa2, 0xb0, 0x1f,
    0x82, 0x8e, 0xa6, 0x2a, 0xc5, 0xb3, 0xe4, 0x2f
};
static uint8_t sha256_hash[32] = {
    0xe3, 0xb0, 0xc4, 0x42, 0x98, 0xfc, 0x1c, 0x14, 0x9a, 0xfb,
    0xf4, 0xc8, 0x99, 0x6f, 0xb9, 0x24, 0x27, 0xae, 0x41, 0xe4,
    0x64, 0x9b, 0x93, 0x4c, 0xa4, 0x95, 0x99, 0x1b, 0x78, 0x52,
    0xb8, 0x55
};
static uint8_t sha384_hash[48] = {
    0x38, 0xb0, 0x60, 0xa7, 0x51, 0xac, 0x96, 0x38, 0x4c, 0xd9,
    0x32, 0x7e, 0xb1, 0xb1, 0xe3, 0x6a, 0x21, 0xfd, 0xb7, 0x11,
    0x14, 0xbe, 0x07, 0x43, 0x4c, 0x0c, 0xc7, 0xbf, 0x63, 0xf6,
    0xe1, 0xda, 0x27, 0x4e, 0xde, 0xbf, 0xe7, 0x6f, 0x65, 0xfb,
    0xd5, 0x1a, 0xd2, 0xf1, 0x48, 0x98, 0xb9, 0x5b
};
static uint8_t sha512_hash[64] = {
    0xcf, 0x83, 0xe1, 0x35, 0x7e, 0xef, 0xb8, 0xbd, 0xf1, 0x54,
    0x28, 0x50, 0xd6, 0x6d, 0x80, 0x07, 0xd6, 0x20, 0xe4, 0x05,
    0x0b, 0x57, 0x15, 0xdc, 0x83, 0xf4, 0xa9, 0x21, 0xd3, 0x6c,
    0xe9, 0xce, 0x47, 0xd0, 0xd1, 0x3c, 0x5d, 0x85, 0xf2, 0xb0,
    0xff, 0x83, 0x18, 0xd2, 0x87, 0x7e, 0xec, 0x2f, 0x63, 0xb9,
    0x31, 0xbd, 0x47, 0x41, 0x7a, 0x81, 0xa5, 0x38, 0x32, 0x7a,
    0xf9, 0x27, 0xda, 0x3e
};

#ifdef SHA_DRV_DBG
static void _print_msg_data(uint8_t *data, uint32_t len)
{
    uint32_t i;

    for (i = 0; i < len; i += 8) {
        CRYPTO_DRV_DBG_I("msg: 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n",
          data[i+0], data[i+1], data[i+2], data[i+3], data[i+4], data[i+5], data[i+6], data[i+7]);
    }
    return;
}
#endif

static uint32_t _sha_reverse_order(uint32_t data)
{
    uint32_t result = 0;
    __asm__("revb %0, %1\n"
            : "=r" (result)
            : "r" (data));
    return result;
}

static void _sha_set_msg_size(uint32_t msg, uint32_t msg_size,
                                  uint32_t total_size, sha_mode_t mode)
{
    uint32_t *msg_buf = (uint32_t *)msg;

    msg_buf += (msg_size + 0x3) / 4;

    if (mode == SHA_MODE_SHA1 || 
        mode == SHA_MODE_SHA224 || mode == SHA_MODE_SHA256) {
        *(msg_buf) = 0;
        *(msg_buf + 1) = total_size << 3;
    } else {
        *(msg_buf) = 0;
        *(msg_buf + 1) = 0;
        *(msg_buf + 2) = 0;
        *(msg_buf + 3) = total_size << 3;
    }

    return;
}

static uint32_t _sha_calc_done(void)
{
    uint32_t is_done = 0;

    volatile sha_reg_t *sha_regs = (sha_reg_t *)PLATFORM_SHA_BADDR;

    is_done = sha_regs->sha_intstate & 0x1;

    return is_done;
}

static int32_t _cal_sha_without_count(uint32_t mode,
                       uint32_t msg, uint32_t msg_len, uint32_t hash)
{
    uint32_t times = 0;
    uint32_t is_done = 0;
    volatile sha_reg_t *sha_regs  = (sha_reg_t *)PLATFORM_SHA_BADDR;

#ifdef SHA_DRV_DBG
    CRYPTO_DRV_DBG_I("without_count -- msg_len: %d mode: %d\n", msg_len, mode);
#endif

    /* new encode */
    sha_regs->sha_mode = 0x0;
    sha_regs->sha_intstate = 0x0;

    /* set src and dst addr */
    sha_regs->sha_baseaddr = msg;
    sha_regs->sha_destaddr = hash;

    sha_regs->sha_mode |= mode & 0x7;
    sha_regs->sha_mode |= 0x1 << 25;

    sha_regs->sha_counter0 = msg_len << 3;
    sha_regs->sha_counter1 = 0;
    sha_regs->sha_counter2 = 0;
    sha_regs->sha_counter3 = 0;

    /* enable encode */
    sha_regs->sha_ctrl |= 0x1;

    do {
        is_done = _sha_calc_done();
    } while (!is_done && times++ < SHA_MAX_TIME);

    if (times == SHA_MAX_TIME) {
        CRYPTO_DRV_DBG_E("_cal_sha_without_count：time out\n");
        return -1;
    }

    return 0;
}

static int32_t _cal_sha_without_count_enable_initial(uint32_t mode,
                         uint32_t msg, uint32_t msg_len, uint32_t hash)
{
    uint32_t times = 0;
    uint32_t is_done = 0;
    volatile sha_reg_t *sha_regs  = (sha_reg_t *)PLATFORM_SHA_BADDR;

#ifdef SHA_DRV_DBG
    CRYPTO_DRV_DBG_I("without_count_enable_initial -- msg_len: %d mode: %d\n", msg_len, mode);
#endif

    /* new encode */
    sha_regs->sha_mode = 0x0;
    sha_regs->sha_intstate = 0x0;

    /* set src and dst addr */
    sha_regs->sha_baseaddr = msg;
    sha_regs->sha_destaddr = hash;

    sha_regs->sha_mode |= mode & 0x7;
    sha_regs->sha_mode |= 0x1 << 3;
    sha_regs->sha_mode |= 0x1 << 25;

    sha_regs->sha_counter0 = msg_len << 3;
    sha_regs->sha_counter1 = 0;
    sha_regs->sha_counter2 = 0;
    sha_regs->sha_counter3 = 0;

    /* enable encode */
    sha_regs->sha_ctrl |= 0x1;

    do {
        is_done = _sha_calc_done();
    } while (!is_done && times++ < SHA_MAX_TIME);

    if (times == SHA_MAX_TIME) {
        CRYPTO_DRV_DBG_E("_cal_sha_without_count_enable_initial：time out\n");
        return -1;
    }

    return 0;
}

static int32_t _cal_sha_enable_initial(uint32_t mode,
                         uint32_t msg, uint32_t msg_len, uint32_t hash)
{
    uint32_t times = 0;
    uint32_t is_done = 0;
    volatile sha_reg_t *sha_regs  = (sha_reg_t *)PLATFORM_SHA_BADDR;

#ifdef SHA_DRV_DBG
    CRYPTO_DRV_DBG_I("enable_initial -- msg_len: %d mode: %d\n", msg_len, mode);
    _print_msg_data((uint8_t *)msg, msg_len);
#endif

    /* new encode */
    sha_regs->sha_mode = 0x0;
    sha_regs->sha_intstate = 0x0;

    /* set src and dst addr */
    sha_regs->sha_baseaddr = msg;
    sha_regs->sha_destaddr = hash;

    sha_regs->sha_mode |= mode & 0x7;
    sha_regs->sha_mode |= 0x1 << 3;

    sha_regs->sha_counter0 = msg_len << 3;
    sha_regs->sha_counter1 = 0;
    sha_regs->sha_counter2 = 0;
    sha_regs->sha_counter3 = 0;

    /* enable encode */
    sha_regs->sha_ctrl |= 0x1;

    do {
        is_done = _sha_calc_done();
    } while (!is_done && times++ < SHA_MAX_TIME);

    if (times == SHA_MAX_TIME) {
        CRYPTO_DRV_DBG_E("_cal_sha_enable_initial：time out\n");
        return -1;
    }

    return 0;
}

static void _sha_save_regs(uint32_t *buf)
{
    uint32_t i;
    uint32_t regs_num;
    volatile sha_reg_t *sha_regs  = (sha_reg_t *)PLATFORM_SHA_BADDR;

    regs_num = sizeof(sha_reg_t)/ sizeof(uint32_t);
    for (i = 0; i < regs_num; i++) {
        *(buf + i) = *((uint32_t *)sha_regs + i);
    }

    return;
}

static void _sha_restore_regs(uint32_t *buf)
{
    uint32_t i;
    uint32_t regs_num;
    sha_reg_t *sha_regs  = (sha_reg_t *)PLATFORM_SHA_BADDR;

    regs_num = sizeof(sha_reg_t)/ sizeof(uint32_t);
    for (i = 0; i < regs_num; i++) {
        *((uint32_t *)sha_regs + i) = *(buf + i);
    }

    return;
}

/* save hash value into dst buffer */
static void _sha_get_digest(uint32_t *src, uint32_t size, uint8_t *dst)
{
    uint32_t i;

    for (i = 0; i < size >> 2; i++) {
        src[i] = _sha_reverse_order(src[i]);
#ifdef SHA_DRV_DBG
        CRYPTO_DRV_DBG_I("hash: %08x\n", src[i]);
#endif
    }

    memcpy(dst, (uint8_t *)src, size);

    return;
}

int32_t sha_drv_suspend(dev_t *dev, int32_t state)
{
    return 0;
}

int32_t sha_drv_resume(dev_t *dev)
{
    return 0;
}

int32_t sha_drv_get_ctx_size(void *arg)
{
    sha_drv_ioctl_buf_size_t *get_arg;

    TEE_ASSERT(arg);

    get_arg = (sha_drv_ioctl_buf_size_t *)arg;
    if (!VALID_MAGIC(get_arg->magic)) {
        return -1;
    }

    get_arg->ctx_size = sizeof(sha_drv_ctx_t);

    return 0;
}

int32_t sha_drv_init(void *arg)
{
    sha_drv_ioctl_init_t *init_arg;
    sha_drv_ctx_t *sha_ctx;
    volatile sha_reg_t *sha_regs  = (sha_reg_t *)PLATFORM_SHA_BADDR;

    TEE_ASSERT(arg);

    init_arg = (sha_drv_ioctl_init_t *)arg;
    if (!VALID_MAGIC(init_arg->magic)) {
        return -1;
    }

    sha_ctx = (sha_drv_ctx_t *)init_arg->ctx;
    sha_ctx->state = SHA_DRV_INITED;
    sha_ctx->data_size = 0;
    sha_ctx->total_size = 0;

    switch(init_arg->mode) {
        case SHA_MODE_SHA1: {
            sha_regs->sha_h0l = 0x67452301;
            sha_regs->sha_h1l = 0xEFCDAB89;
            sha_regs->sha_h2l = 0x98BADCFE;
            sha_regs->sha_h3l = 0x10325476;
            sha_regs->sha_h4l = 0xC3D2E1F0;
            break;
        }
        case SHA_MODE_SHA256: {
            sha_regs->sha_h0l = 0x6A09E667;
            sha_regs->sha_h1l = 0xBB67AE85;
            sha_regs->sha_h2l = 0x3C6EF372;
            sha_regs->sha_h3l = 0xA54FF53A;
            sha_regs->sha_h4l = 0x510E527F;
            sha_regs->sha_h5l = 0x9B05688C;
            sha_regs->sha_h6l = 0x1F83D9AB;
            sha_regs->sha_h7l = 0x5BE0CD19;
            break;
        }
        case SHA_MODE_SHA224: {
            sha_regs->sha_h0l = 0xc1059ed8;
            sha_regs->sha_h1l = 0x367cd507;
            sha_regs->sha_h2l = 0x3070dd17;
            sha_regs->sha_h3l = 0xf70e5939;
            sha_regs->sha_h4l = 0xffc00b31;
            sha_regs->sha_h5l = 0x68581511;
            sha_regs->sha_h6l = 0x64f98fa7;
            sha_regs->sha_h7l = 0xbefa4fa4;
            break;
        }
        case SHA_MODE_SHA512: {
            sha_regs->sha_h0h = 0x6a09e667;
            sha_regs->sha_h0l = 0xf3bcc908;
            sha_regs->sha_h1h = 0xbb67ae85;
            sha_regs->sha_h1l = 0x84caa73b;
            sha_regs->sha_h2h = 0x3c6ef372;
            sha_regs->sha_h2l = 0xfe94f82b;
            sha_regs->sha_h3h = 0xa54ff53a;
            sha_regs->sha_h3l = 0x5f1d36f1;
            sha_regs->sha_h4h = 0x510e527f;
            sha_regs->sha_h4l = 0xade682d1;
            sha_regs->sha_h5h = 0x9b05688c;
            sha_regs->sha_h5l = 0x2b3e6c1f;
            sha_regs->sha_h6h = 0x1f83d9ab;
            sha_regs->sha_h6l = 0xfb41bd6b;
            sha_regs->sha_h7h = 0x5be0cd19;
            sha_regs->sha_h7l = 0x137e2179;
            break;
        }
        case SHA_MODE_SHA384: {
            sha_regs->sha_h0h = 0xcbbb9d5d;
            sha_regs->sha_h0l = 0xc1059ed8;
            sha_regs->sha_h1h = 0x629a292a;
            sha_regs->sha_h1l = 0x367cd507;
            sha_regs->sha_h2h = 0x9159015a;
            sha_regs->sha_h2l = 0x3070dd17;
            sha_regs->sha_h3h = 0x152fecd8;
            sha_regs->sha_h3l = 0xf70e5939;
            sha_regs->sha_h4h = 0x67332667;
            sha_regs->sha_h4l = 0xffc00b31;
            sha_regs->sha_h5h = 0x8eb44a87;
            sha_regs->sha_h5l = 0x68581511;
            sha_regs->sha_h6h = 0xdb0c2e0d;
            sha_regs->sha_h6l = 0x64f98fa7;
            sha_regs->sha_h7h = 0x47b5481d;
            sha_regs->sha_h7l = 0xbefa4fa4;
            break;
        }

        default: {
            CRYPTO_DRV_DBG_E("sha_drv_init: invalid sha mode(%d)\n", init_arg->mode);
            return -1;
        }
    }

    _sha_save_regs((uint32_t *)&(sha_ctx->sha_regs));

    return 0; 
}

int32_t sha_drv_process(void *arg)
{
    int ret;
    uint32_t block_size;
    uint32_t src_size;
    uint8_t *src_data;
    uint8_t hash[SHA_MAX_SIZE];
    sha_drv_ioctl_proc_t *proc_arg;
    sha_drv_ctx_t *sha_ctx;

    TEE_ASSERT(arg);

    proc_arg = (sha_drv_ioctl_proc_t *)arg;
    if (!VALID_MAGIC(proc_arg->magic)) {
        CRYPTO_DRV_DBG_E("sha_drv_process: bad magic(%08x)\n", proc_arg->magic);
        return -1;
    }

    if (proc_arg->mode >= SHA_MODE_MAX ||
        proc_arg->mode <= SHA_MODE_INVALID) {
        CRYPTO_DRV_DBG_E("sha_drv_process: bad mode(%d)\n", proc_arg->mode);
        return -1;
    }

    if (proc_arg->mode == SHA_MODE_SHA1 ||
        proc_arg->mode == SHA_MODE_SHA256 ||
        proc_arg->mode == SHA_MODE_SHA224) {
        block_size = 64;
    } else {
        block_size = 128;
    }

    src_data = (uint8_t *)proc_arg->src_addr;
    src_size = proc_arg->src_size;

    sha_ctx = (sha_drv_ctx_t *)proc_arg->ctx;
    _sha_restore_regs((uint32_t *)&sha_ctx->sha_regs);

    if (sha_ctx->data_size + src_size <= block_size) {
        if (src_size) {
            memcpy(sha_ctx->data + sha_ctx->data_size,
                           src_data, src_size);
            sha_ctx->data_size += src_size;
        }
        return 0;
    }

    if (sha_ctx->data_size < block_size) {
        memcpy(sha_ctx->data + sha_ctx->data_size,
                       src_data, block_size - sha_ctx->data_size);
        src_data += block_size - sha_ctx->data_size;
        src_size -= block_size - sha_ctx->data_size;
    }

    if (sha_ctx->state == SHA_DRV_INITED) {
        ret = _cal_sha_without_count(proc_arg->mode,
                   (uint32_t)sha_ctx->data, block_size, (uint32_t)hash);
        if (ret < 0) {
            CRYPTO_DRV_DBG_E("sha_drv_process: calc hash without count fail!\n");
            return -1;
        }
        sha_ctx->state = SHA_DRV_STARTED;
    } else {
        ret = _cal_sha_without_count_enable_initial(proc_arg->mode,
                    (uint32_t)sha_ctx->data, block_size, (uint32_t)hash);
        if (ret < 0) {
            CRYPTO_DRV_DBG_E("sha_drv_process: calc first block hash with continue fail!\n");
            return -1;
        }
    }
    sha_ctx->data_size = 0;
    sha_ctx->total_size += block_size;

    while (src_size > block_size) {
        uint32_t block[BLOCK_MAX_SIZE >> 2];

        memcpy((uint8_t *)block, src_data, block_size);
        ret = _cal_sha_without_count_enable_initial(
                   proc_arg->mode, (uint32_t)block, block_size, (uint32_t)hash);
        if (ret < 0) {
            CRYPTO_DRV_DBG_E("sha_drv_process: calc hash with continue fail!\n");
            return -1;
        }

        src_data += block_size;
        src_size -= block_size;
        sha_ctx->total_size += block_size;
    }

    memcpy(sha_ctx->data, src_data, src_size);
    sha_ctx->data_size = src_size;

    _sha_save_regs((uint32_t *)&sha_ctx->sha_regs);

    return 0;
}

int32_t sha_drv_finish(void *arg)
{
    int ret = 0;
    uint32_t block_size;
    uint64_t total_size;
    uint32_t msg_size;
    uint32_t hash_size;
    uint8_t *msg_buf = NULL;
    uint32_t hash[SHA_MAX_SIZE >> 2];
    sha_drv_ioctl_final_t *final_arg;
    sha_drv_ctx_t *sha_ctx;

    TEE_ASSERT(arg);

    final_arg = (sha_drv_ioctl_final_t *)arg;
    if (!VALID_MAGIC(final_arg->magic)) {
        CRYPTO_DRV_DBG_E("sha_drv_finish: bad magic(%08x)\n", final_arg->magic);
        return -1;
    }

    sha_ctx = (sha_drv_ctx_t *)final_arg->ctx;
    msg_size = sha_ctx->data_size;
    total_size = sha_ctx->total_size + msg_size;

    switch(final_arg->mode) {
        case SHA_MODE_SHA1: {
            hash_size = 20;
            block_size = 64;

            if (total_size == 0) {
                TEE_ASSERT(sha_ctx->state == SHA_DRV_INITED);
                memcpy((uint8_t *)final_arg->dst_addr,
                               sha1_hash, hash_size);
                return 0;
            }
            break;
        }
        case SHA_MODE_SHA256: {
            hash_size = 32;
            block_size = 64;

            if (total_size == 0) {
                TEE_ASSERT(sha_ctx->state == SHA_DRV_INITED);
                memcpy((uint8_t *)final_arg->dst_addr,
                               sha256_hash, hash_size);
                return 0;
            }
            break;
        }
        case SHA_MODE_SHA224: {
            hash_size = 28;
            block_size = 64;

            if (total_size == 0) {
                TEE_ASSERT(sha_ctx->state == SHA_DRV_INITED);
                memcpy((uint8_t *)final_arg->dst_addr,
                               sha224_hash, hash_size);
                return 0;
            }
            break;
        }
        case SHA_MODE_SHA512: {
            hash_size = 64;
            block_size = 128;

            if (total_size == 0) {
                TEE_ASSERT(sha_ctx->state == SHA_DRV_INITED);
                memcpy((uint8_t *)final_arg->dst_addr,
                               sha512_hash, hash_size);
                return 0;
            }
            break;
        }
        case SHA_MODE_SHA384: {
            hash_size = 48;
            block_size = 128;

            if (total_size == 0) {
                TEE_ASSERT(sha_ctx->state == SHA_DRV_INITED);
                memcpy((uint8_t *)final_arg->dst_addr,
                               sha384_hash, hash_size);
                return 0;
            }
            break;
        }
        default: {
            CRYPTO_DRV_DBG_E("sha_drv_finish: invalid sha mode(%d)\n", final_arg->mode);
            return -1;
        }
    }

    TEE_ASSERT(sha_ctx->data_size <= block_size);

    _sha_restore_regs((uint32_t *)&sha_ctx->sha_regs);

    msg_buf = (uint8_t *)malloc(block_size + 16);
    if (msg_buf == NULL) {
        CRYPTO_DRV_DBG_E("sha_drv_finish: malloc fail\n");
        return -1;
    }

    memset(msg_buf, 0x0, block_size + 16);
    memcpy(msg_buf, (void *)sha_ctx->data, sha_ctx->data_size);
    _sha_set_msg_size((uint32_t)msg_buf, msg_size, total_size, final_arg->mode);

    ret = _cal_sha_enable_initial(final_arg->mode,
                      (uint32_t)msg_buf, msg_size, (uint32_t)hash);
    if (ret < 0) {
        CRYPTO_DRV_DBG_E("sha_drv_finish: calc hash fail!\n");
        goto out;
    }

    _sha_get_digest(hash, hash_size, (uint8_t *)final_arg->dst_addr);

out:
    if (msg_buf) {
        free(msg_buf);
    }

    return ret;
}

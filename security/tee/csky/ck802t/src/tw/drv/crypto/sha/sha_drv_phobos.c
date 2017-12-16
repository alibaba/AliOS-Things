/**
 * Copyright (C) 2016 The YunOS Project. All rights reserved.
 **/

#include "hal_memmap.h"
#include "sha_drv_phobos.h"
#include "device.h"
#include "tee_dbg.h"
#include "crypto_drv.h"

#define SHA_MAX_TIME     20
#define SHA_MAX_SIZE     64

#define BLOCK_MAX_SIZE   128

static const uint8_t sha1_hash[20] = {
    0xda, 0x39, 0xa3, 0xee, 0x5e, 0x6b, 0x4b, 0x0d, 0x32, 0x55,
    0xbf, 0xef, 0x95, 0x60, 0x18, 0x90, 0xaf, 0xd8, 0x07, 0x09
};
static const uint8_t sha224_hash[28] = {
    0xd1, 0x4a, 0x02, 0x8c, 0x2a, 0x3a, 0x2b, 0xc9, 0x47, 0x61,
    0x02, 0xbb, 0x28, 0x82, 0x34, 0xc4, 0x15, 0xa2, 0xb0, 0x1f,
    0x82, 0x8e, 0xa6, 0x2a, 0xc5, 0xb3, 0xe4, 0x2f
};
static const uint8_t sha256_hash[32] = {
    0xe3, 0xb0, 0xc4, 0x42, 0x98, 0xfc, 0x1c, 0x14, 0x9a, 0xfb,
    0xf4, 0xc8, 0x99, 0x6f, 0xb9, 0x24, 0x27, 0xae, 0x41, 0xe4,
    0x64, 0x9b, 0x93, 0x4c, 0xa4, 0x95, 0x99, 0x1b, 0x78, 0x52,
    0xb8, 0x55
};
static const uint8_t sha384_hash[48] = {
    0x38, 0xb0, 0x60, 0xa7, 0x51, 0xac, 0x96, 0x38, 0x4c, 0xd9,
    0x32, 0x7e, 0xb1, 0xb1, 0xe3, 0x6a, 0x21, 0xfd, 0xb7, 0x11,
    0x14, 0xbe, 0x07, 0x43, 0x4c, 0x0c, 0xc7, 0xbf, 0x63, 0xf6,
    0xe1, 0xda, 0x27, 0x4e, 0xde, 0xbf, 0xe7, 0x6f, 0x65, 0xfb,
    0xd5, 0x1a, 0xd2, 0xf1, 0x48, 0x98, 0xb9, 0x5b
};
static const uint8_t sha512_hash[64] = {
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

static inline void sha_enable_calc(void)
{
    volatile sha_phobos_reg_t *ptr = (sha_phobos_reg_t *)PLATFORM_SHA_BADDR;

    ptr->SHA_CON |= 1<<SHA_CONTROL_BITS_RUN;
}

static inline void sha_enable_int(void)
{
    volatile sha_phobos_reg_t *ptr = (sha_phobos_reg_t *)PLATFORM_SHA_BADDR;

    ptr->SHA_CON |= 1<<4;
}

static inline void sha_message_done(void)
{
    volatile sha_phobos_reg_t *ptr = (sha_phobos_reg_t *)PLATFORM_SHA_BADDR;

    while((ptr->SHA_CON & 0x40)!=0);
}

static void sha_input_data(uint32_t *data, uint32_t length)
{
    volatile sha_phobos_reg_t *ptr = (sha_phobos_reg_t *)PLATFORM_SHA_BADDR;
    uint32_t *input_data = (uint32_t *)&(ptr->SHA_DATA1);
    uint8_t i;

    for(i=0; i<length; i++) {
        *(input_data + i) = *(data + i);
    }
}

static void sha_get_data(uint32_t *data)
{
    volatile sha_phobos_reg_t *ptr = (sha_phobos_reg_t *)PLATFORM_SHA_BADDR;
    uint32_t *result = (uint32_t *)&ptr->SHA_H0L;
    uint8_t i;

    for(i=0; i<5; i++) {
        data[i] = *(result + i);
    }
}

static inline void sha_reverse_order(uint8_t *pdata, int length)
{
    uint32_t wlen = length >> 2;
    uint32_t result = 0;
    uint32_t tmp_data[20];
    int i = 0;

    memcpy((void *)tmp_data, (void *)pdata, length);
    for (i = 0; i < wlen; i++) {
        __asm__("revb %0, %1\n"
                : "=r" (result)
                : "r" (tmp_data[i]));
        tmp_data[i] = result;
    }
    memcpy((void *)pdata, (void *)tmp_data, length);
}

static void sha_deal_last_block(uint8_t *input, int total_size)
{
    uint32_t index = 0;
    uint8_t  block_word=0;
    uint8_t  i;
    //uint8_t  block_num = 0;
    uint32_t left_len = 0;;
    uint32_t input_message[32];
    uint32_t length = total_size;
    uint32_t totallen = total_size;

  //  block_num    = length >> 6;
    left_len     = length & 0x3f;
    length       = length << 3;
    index        = left_len >> 2;
    totallen     = totallen << 3;

    if(index >= 14) {
        block_word = 16;
    }

    memcpy(input_message, input, left_len);

    if (left_len & 0x3) {
        *((uint8_t *)input_message+left_len) = 0x80;
        for(i=1; i<(4-(left_len & 0x3)); i++) {
            *((uint8_t *)input_message+left_len+i) = 0x00;
        }
        index++;
        input_message[index] = 0x00000000;
    } else {
        input_message[index] = 0x00000080;
    }

    for (i = index + 1; i < block_word + 15; i++) {
        input_message[i] = 0x0;
    }

    totallen = ((((totallen >> 0) & 0xff) << 24) | \
                (((totallen >> 8) & 0xff) << 16) | \
                (((totallen >> 16) & 0xff) << 8)  | \
                 ((totallen >> 24) & 0xff));
    input_message[i] = totallen;

    sha_input_data(input_message, 16);
    sha_enable_calc();
    sha_message_done();

    if(index >= 14) {
        sha_input_data(&input_message[16], 16);
        sha_enable_calc();
        sha_message_done();
    }
}

static void _sha_save_regs(uint32_t *buf)
{
    uint32_t i;
    uint32_t regs_num;
    volatile sha_phobos_reg_t *sha_regs  = (sha_phobos_reg_t *)PLATFORM_SHA_BADDR;

    regs_num = sizeof(sha_phobos_reg_t)/ sizeof(uint32_t);
    for (i = 0; i < regs_num; i++) {
        *(buf + i) = *((uint32_t *)sha_regs + i);
    }

    return;
}

static void _sha_restore_regs(uint32_t *buf)
{
    uint32_t i;
    uint32_t regs_num;
    sha_phobos_reg_t *sha_regs  = (sha_phobos_reg_t *)PLATFORM_SHA_BADDR;

    regs_num = sizeof(sha_phobos_reg_t)/ sizeof(uint32_t);
    for (i = 0; i < regs_num; i++) {
        *((uint32_t *)sha_regs + i) = *(buf + i);
    }

    return;
}

static int _sha_set_mode(sha_mode_t mode)
{
    volatile sha_phobos_reg_t *sha_regs = (sha_phobos_reg_t*)PLATFORM_SHA_BADDR;
    switch(mode) {
        case SHA_MODE_SHA1: {
            sha_regs->SHA_CON |= SHA_CONTROL_CALC_MODE_SHA1;
            break;
        }
        case SHA_MODE_SHA256: {
            sha_regs->SHA_CON |= SHA_CONTROL_CALC_MODE_SHA256;
            break;
        }
        case SHA_MODE_SHA224: {
            sha_regs->SHA_CON |= SHA_CONTROL_CALC_MODE_SHA224;
            break;
        }
        case SHA_MODE_SHA512: {
            sha_regs->SHA_CON |= SHA_CONTROL_CALC_MODE_SHA512;
            break;
        }
        case SHA_MODE_SHA384: {
            sha_regs->SHA_CON |= SHA_CONTROL_CALC_MODE_SHA384;
            break;
        }
        default: {
            CRYPTO_DRV_DBG_E("sha_drv_init: invalid sha mode(%d)\n", mode);
            return -1;
        }
    }
    return 0;
}

int32_t sha_drv_init(void *arg)
{
    sha_drv_ioctl_init_t *init_arg;
    sha_drv_ctx_t *sha_ctx;
    volatile sha_phobos_reg_t *sha_regs = (sha_phobos_reg_t*)PLATFORM_SHA_BADDR;

    TEE_ASSERT(arg);

    init_arg = (sha_drv_ioctl_init_t *)arg;
    if (!VALID_MAGIC(init_arg->magic)) {
        return -1;
    }

    sha_ctx = (sha_drv_ctx_t *)init_arg->ctx;
    sha_ctx->state = SHA_DRV_INITED;
    sha_ctx->data_size = 0;
    sha_ctx->total_size = 0;

    if (_sha_set_mode(init_arg->mode) != 0) {
        return -1;
    }

    sha_regs->SHA_CON |= SHA_CONTROL_ENDIAN_MODE_BIG_ENDIAN << SHA_CONTROL_BITS_ENDIAN;
    sha_regs->SHA_CON |= 0 << SHA_CONTROL_BITS_INTERRUPT;
    sha_regs->SHA_CON |= 1 << SHA_CONTROL_BITS_ENABLE_INIT;

    _sha_save_regs((uint32_t *)&(sha_ctx->sha_regs));

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

int32_t sha_drv_process(void *arg)
{
  //  int ret;
    uint32_t block_size;
    uint32_t src_size;
    uint8_t *src_data;
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

    if ((sha_ctx->state != SHA_DRV_INITED) && (sha_ctx->state != SHA_DRV_STARTED)) {
        return -1;
    }

    sha_ctx->state = SHA_DRV_STARTED;

    sha_input_data((uint32_t *)sha_ctx->data, block_size >> 2);
    sha_enable_calc();
    sha_message_done();

    sha_ctx->data_size = 0;
    sha_ctx->total_size += block_size;

    while (src_size > block_size) {
        uint32_t block[BLOCK_MAX_SIZE >> 2];

        memcpy((uint8_t *)block, src_data, block_size);
        sha_input_data((uint32_t *)block, block_size >> 2);
        sha_enable_calc();
        sha_message_done();

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

    sha_deal_last_block(sha_ctx->data, total_size);

    sha_get_data((uint32_t *)final_arg->dst_addr);
    sha_reverse_order((uint8_t *)final_arg->dst_addr, hash_size);

    return ret;
}

int32_t sha_drv_suspend(dev_t *dev, int32_t state)
{
    return 0;
}

int32_t sha_drv_resume(dev_t *dev)
{
    return 0;
}


/**
 * File: zx297100_sha.c
 * Brief: Implementation of Sanechips sha
 *
 * Copyright (C) 2017 Sanechips Technology Co., Ltd.
 * Author:
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

/*******************************************************************************
 *                              Include header files                            *
 ******************************************************************************/
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "drv_sha.h"
#include "zx29_sha.h"

/*******************************************************************************
 *                              Macro definitions                               *
 *******************************************************************************/
#define SHA_MAX_TIME        20
#define SHA_MAX_SIZE        20  //sha1 only surport 20
#define SHA_LENGTH          20
#define SHA1_CONTEXT_SIZE   sizeof(struct sha_reg_t)
#define ERR_SHA(errno) (CSI_DRV_ERRNO_SHA_BASE | errno)

/*******************************************************************************
*                               Type definitions                                *
*******************************************************************************/
typedef struct {
    uint32_t base;
    sha_event_cb_t cb;
    sha_status_t status;
} ck_sha_priv_t;


typedef enum _hash_type_t {
    SHA1    = 0,
    SHA224  = 1,
    SHA256  = 2,
    SHA384  = 3,
    SHA512  = 4,
    MD5     = 5,
} hash_type_t;

/*******************************************************************************
*                               Local function declarations                     *
*******************************************************************************/

/*******************************************************************************
*                               Local variable definitions                      *
*******************************************************************************/

/*******************************************************************************
*                               Global variable definitions                     *
*******************************************************************************/
static volatile hash_type_t sha_mode;
static uint32_t hash[SHA_MAX_SIZE >> 2];
static const sha_capabilities_t driver_capabilities = {
    .sha1 = 1, /* sha1 mode */
};

/*******************************************************************************
*                            function   extern                                  *
*******************************************************************************/
extern int32_t target_get_sha_count(void);
extern int32_t target_get_sha(int32_t idx, uint32_t *base, uint32_t *irq);

/*******************************************************************************
*                               Local function implementations                  *
*******************************************************************************/

static void crypto_sha_mode_set()
{
    *(volatile uint32_t *)(ZX29_SECURE_BASE + 0x4) = (1 << 2);
}

static void crypto_sha_mode_clear()
{
    *(volatile uint32_t *)(ZX29_SECURE_BASE + 0x4) = 0;
}

/**
* Brief      reverse data order
*
* param[in]data  the data to be reverseed
*
* return    the data reverseed
*/
static uint32_t _sha_reverse_order(uint32_t data)
{
    uint32_t result = 0;
    __asm__("revb %0, %1\n"
            : "=r"(result)
            : "r"(data));
    return result;
}

/**
* Brief      Enable SHA1 Algorithm
*
* param[in] null
*
* return    0-success , negative-error code.
*/
static int32_t _sha_enable(void)
{
    volatile struct sha_reg_t *sha_reg = (struct sha_reg_t *)ZX29_SHA_BASE;

    sha_reg->sha_ctrl |= (0x1 << 4); /*Enable SHA1 Algorithm*/

    return 0;
}

/**
* Brief      set sha type
*
* param[in]type the type of sha, \ref hash_type_t
*
* return    0-success , negative-error code.
*/
static int32_t _sha_set_mode(hash_type_t type)
{
    sha_mode = type;
    return 0;
}

/**
* Brief      set sha endian mode
*
* param[in]type the type of sha, \ref hash_type_t
*
* return    0-success , negative-error code.
*/
static int32_t _sha_set_endian(enum sha_endian_t endian)
{
    bool _g_sha_big_endian;

    _g_sha_big_endian = ((endian == SHA_ENDIAN_BIG) ? (true) : (false));

    volatile struct sha_reg_t *sha_reg = (struct sha_reg_t *)ZX29_SHA_BASE;

    /*****  Endian Control bit is bit 3 in sha_ctrl register ******/
    sha_reg->sha_ctrl = ((_g_sha_big_endian == true) ? (sha_reg->sha_ctrl | 0x1000) : (sha_reg->sha_ctrl & (~0x1000)));

    return 0;
}

/**
* Brief      set sha byte length
*
* param[in]byte_len     the length of data
*
* return    0-success , negative-error code.
*/
static int32_t _sha_set_len(uint32_t byte_len)
{
    volatile struct sha_reg_t *sha_reg = (struct sha_reg_t *)ZX29_SHA_BASE;

    sha_reg->sha_len[1] = byte_len << 3;    /*low 32 bits*/
    sha_reg->sha_len[0] = byte_len >> 29;   /*high 32 bits*/

    return 0;
}
/**
* Brief      set data to sha registers
*
* param[in]src          pointer where src data is stored
* param[in]byte_len     the length of data
*
* return    0-success , negative-error code.
*/
static int32_t _sha_set_data(char *src , uint32_t byte_len)
{
    volatile struct sha_reg_t *sha_reg = (struct sha_reg_t *)ZX29_SHA_BASE;
    uint32_t tmp, i;
    uint32_t len_word_align;
    uint32_t len_byte_left;

    //start write message data into fifo
    //len_word_align = (byte_len + 3) / 4;
    len_word_align = byte_len  / 4;
    len_byte_left =  byte_len % 4;

    for (i = 0; i < len_word_align; i++) {
        tmp = src[i * 4] | (src[i * 4 + 1] << 8) | (src[i * 4 + 2] << 16) | (src[i * 4 + 3] << 24);
        sha_reg->sha_data = tmp;
    }

    switch (len_byte_left) {
        case 1:
            tmp = src[i * 4];
            sha_reg->sha_data = tmp;
            break;

        case 2:
            tmp = src[i * 4] | (src[i * 4 + 1] << 8);
            sha_reg->sha_data = tmp;
            break;

        case 3:
            tmp = src[i * 4] | (src[i * 4 + 1] << 8) | (src[i * 4 + 2] << 16);
            sha_reg->sha_data = tmp;
            break;

        default:
            return -1;
    }

    return 0;
}
/**
* Brief      set data to sha registers
*
* param[out]src     pointer where dst data is stored
*
* return    0-success , negative-error code.
*/
static int32_t _sha_get_value(uint32_t *dsc)
{
    volatile struct sha_reg_t *sha_reg = (struct sha_reg_t *)ZX29_SHA_BASE;
    uint32_t i;

    for (i = 0; i < SHA_LENGTH / 4; i++) {
        dsc[i] = sha_reg->sha_result[i];
    }

    return 0;
}

/**
* Brief     sha  calculate status
*
* param[in] null
*
* return    the sha  calculate status
*/
static uint32_t _sha_calc_done(void)
{
    uint32_t is_done;

    volatile struct sha_reg_t *sha_regs = (struct sha_reg_t *)ZX29_SHA_BASE;

    is_done = sha_regs->sha_start & 0x1;

    /*0:idle 1:active*/
    return is_done;
}
/**
* Brief      start sha calculate
*
* param[in] null
*
* return    0-success , negative-error code.
*/
static int32_t _sha_calc_enable(void)
{
    volatile struct sha_reg_t *sha_regs = (struct sha_reg_t *)ZX29_SHA_BASE;

    sha_regs->sha_start = 0x1;

    return 0;
}

/**
* Brief       sha calculate proccess
*
* param[in]mode     the hash type
* param[in]msg      pointer where src data is stored
* param[in]msg_len  the length of src data
* param[out]hash    pointer where hashed data is stored
* return    0-success , negative-error code.
*/
int32_t _sha_calculate(hash_type_t mode,
                       const uint8_t   *msg, size_t msg_len, uint32_t *hash)
{
    uint32_t times = 0;
    uint32_t is_done;

    _sha_set_len(msg_len);

    _sha_calc_enable();

    _sha_set_data((char *)msg, msg_len);

    do {
        is_done = _sha_calc_done();
    } while (is_done && times++ < SHA_MAX_TIME);

    _sha_get_value(hash);

    if (times == SHA_MAX_TIME) {
        return -1;
    }

    return 0;
}

/**
* Brief      set crypto mode to hash
*
* param[in] null
*
* return    0-success , negative-error code.
*/
int32_t _sha_clk_on(void)
{
    volatile struct sha_reg_t *sha_reg = (struct sha_reg_t *)ZX29_SHA_BASE;

    /****** sec_mux bit 0 is sha mux, 1 enable********/
    crypto_sha_mode_set();

    sha_reg->sha_ctrl |= 0x1;
    return 0;
}

/**
* Brief      clear crypto mode
*
* param[in] null
*
* return    0-success , negative-error code.
*/
int32_t _sha_clk_off(void)
{
    volatile struct sha_reg_t *sha_reg = (struct sha_reg_t *)ZX29_SHA_BASE;

    crypto_sha_mode_clear();

    sha_reg->sha_ctrl &= ~0x1;
    return 0;
}

/**
* Brief       copy data from src to dst
*
* param[in]src      pointer where src data is stored
* param[in]size     the length of src data
* param[out]dst pointer where hashed data is stored
* return null.
*/
static void _sha_get_digest(uint32_t *src, uint32_t size, uint8_t *dst)
{
    uint32_t i;
    uint32_t tmp_hash[SHA_MAX_SIZE >> 2];

    for (i = 0; i < size >> 2; i++) {
        tmp_hash[i] = _sha_reverse_order(src[i]);
    }

    memcpy(dst, (uint8_t *)tmp_hash, size);

    return;
}
/*******************************************************************************
*                               Gloable function implementations                *
*******************************************************************************/
/**
  \brief       Initialize SHA Interface. 1. Initializes the resources needed for the SHA interface 2.registers event callback function
  \param[in]   context Pointer to the buffer store the sha context
  \param[in]   cb_event  Pointer to \ref sha_event_cb_t
  \return      return sha handle if success
*/
sha_handle_t csi_sha_initialize(int32_t idx, void *context, sha_event_cb_t cb_event)
{
    (void)idx;
    uint32_t base = 0u;
    uint32_t irq;
    /* obtain the sha information */
    target_get_sha(0, &base, &irq);

    ck_sha_priv_t *sha_priv = context;

    sha_priv->base = base;

    /* initialize the sha context */
    sha_priv->cb = cb_event;
    sha_priv->status.busy = 0;

    return (sha_handle_t)sha_priv;
}

/**
 * brief       De-initialize sha Interface. stops operation and releases
 * the software resources used by the interface
 * param[in]   handle  sha handle to operate.
 * return      error code
*/
int32_t csi_sha_uninitialize(sha_handle_t handle)
{
    ck_sha_priv_t *sha_priv = handle;
    sha_priv->cb = NULL;
    return 0;
}

/**
 * brief       Get driver capabilities.
 * param[in]   idx device id
 * return      \ref sha_capabilities_t
*/
sha_capabilities_t csi_sha_get_capabilities(int32_t idx)
{
    return driver_capabilities;
}

/**
 * brief       config sha mode.
 * param[in]handle      sha handle to operate.
 * param[in]mode        sha mode ,ref sha_mode_e
 * param[in]endian      endian mode of sha,\ref sha_endian_mode_e
 * return           0-success , negative-error code.
*/
int32_t csi_sha_config(sha_handle_t handle,
                       sha_mode_e mode,
                       sha_endian_mode_e endian
                      )
{
    _sha_clk_off();
    _sha_clk_on();

    _sha_set_endian(SHA_ENDIAN_LITTLE);


    if (mode == SHA_MODE_1) {
        _sha_set_mode(SHA1);
    } else {
        return -1;
    }

    return 0;
}

/**
 * brief       start sha calculate
 * param[in]handle      sha handle to operate.
 * param[in/out]context pointer where context data is stored or to be stored
 * return           0-success , negative-error code.
*/
int32_t csi_sha_start(sha_handle_t handle, void *context)
{
    _sha_enable();
    return 0;
}

/**
* brief    data calculate proccess
* param[in]handle       sha handle to operate.
* param[in/out]context  pointer where context data is stored or to be stored
* param[in]input        pointer where src data is stored
* param[in]len          the length  of  data to be calculated
* return            0-success , negative-error code.
*/
int32_t csi_sha_update(sha_handle_t handle, void *context, const void *input, uint32_t len)
{
    const uint8_t *src = input;
    uint32_t size = len;

    if ((NULL == src) || (0 == size)) {
        return -1;
    }

    if (sha_mode != SHA1) {
        return -1;
    }

    _sha_calculate(SHA1, src, size, hash);

    _sha_clk_off();

    return 0;
}

/**
* brief       Get shaed data.
* param[in]handle       sha handle to operate.
* param[out]output      pointer where hashed data is to be stored
* param[in/out]context  pointer where context data is stored or to be stored
* return            0-success , negative-error code.
*/
int32_t csi_sha_finish(sha_handle_t handle, void *context, void *output)
{
    uint8_t *dgst = output;

    if (NULL == dgst) {
        return ERR_SHA(DRV_ERROR_PARAMETER);
    }

    _sha_get_digest(hash, SHA_LENGTH, dgst);

    return 0;
}

/**
* brief       Get sha status.
* param[in]   handle  sha handle to operate.
* return      sha status \ref sha_status_t
*/
sha_status_t csi_sha_get_status(sha_handle_t handle)
{
    ck_sha_priv_t *sha_priv = handle;

    return sha_priv->status;
}



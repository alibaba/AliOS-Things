/**
 * File: zx297100_aes.c
 * Brief: Implementation of Sanechips aes
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
#include "drv_aes.h"
#include "zx29_aes.h"

/*******************************************************************************
 *                              Macro definitions                               *
 *******************************************************************************/
#define IV_BYTES    16
#define IV_WORDS    (IV_BYTES>>2)
#define SRC_BYTES_COUNT_ONCE    16
#define BUFER_SIZE  (sizeof(struct aes_reg_t)>>2)

/*******************************************************************************
*                               Type definitions                                *
*******************************************************************************/
typedef struct {
    uint32_t base;
    void *iv;
    aes_event_cb_t cb;
    aes_status_t status;
} ck_aes_priv_t;

/*******************************************************************************
*                               Local function declarations                     *
*******************************************************************************/

/*******************************************************************************
*                               Local variable definitions                      *
*******************************************************************************/

/*******************************************************************************
*                               Global variable definitions                     *
*******************************************************************************/
static uint32_t buffer[BUFER_SIZE];
static ck_aes_priv_t aes_handle[CONFIG_AES_NUM];

static const aes_capabilities_t driver_capabilities = {
    .ecb_mode = 1, /* ECB mode */
    .cbc_mode = 1, /* CBC mode */
};

/*******************************************************************************
*                               Inline function implementations                 *
*******************************************************************************/

/*******************************************************************************
*                            function   extern                                  *
*******************************************************************************/
extern int32_t target_get_aes_count(void);
extern int32_t target_get_aes(int32_t idx, uint32_t *base, uint32_t *irq);

/*******************************************************************************
*                               Local function implementations                  *
*******************************************************************************/

static void crypto_aes_mode_set()
{
    *(volatile uint32_t *)(ZX29_SECURE_BASE + 0x4) = (1 << 0);
}

static void crypto_aes_mode_clear()
{
    *(volatile uint32_t *)(ZX29_SECURE_BASE + 0x4) = 0;
}

/**
 * Brief         wait aes tx complete
 *
 * param[in]    size        size of tx fifo     data
 *
 * return       null
 */
static void _aes_wait_tx_complete(uint32_t size)
{
    volatile struct aes_reg_t *aes_reg = (struct aes_reg_t *)ZX29_AES_BASE;

    /******* [4:0]tx fifo data num    [12:8]rx fifo data num ********/
    while ((aes_reg->fifo_data_num & 0x1f) != size);
}

/**
 * Brief    Set AES Mode, CBC or EBC
 * param[in]fifo_mode fifo mode
 *                    AES_RX_FIFO
 *                    AES_TX_FIFO
 * return   null
 */
static void _aes_wait_fifo_empty(enum aes_fifo_t fifo_mode)
{
    volatile struct aes_reg_t *aes_reg = (struct aes_reg_t *)ZX29_AES_BASE;

    /******* [4:0]tx fifo data num    [12:8]rx fifo data num ********/
    if (fifo_mode == AES_RX_FIFO)
        while (aes_reg->fifo_data_num & (0x1f << 8));
    else
        while (aes_reg->fifo_data_num & 0x1f);
}

/**
* Brief         set  256 bits Key
*
* param[in]pkey        a pointer point to key address where the key is saved
* param[in]keylength   length of key  be set
*
* return               0-success , negative-error code.
*/
static int32_t _aes_set_key(uint8_t *pkey, enum aes_keylength_t keylength)
{
    volatile struct aes_reg_t *aes_reg = (struct aes_reg_t *)ZX29_AES_BASE;
    uint32_t length = keylength ? ((keylength & 0x1) ? 6 : 8) : 4;
    int32_t i;
    uint32_t tmp;

    for (i = 0; i < 8 - length; i++) {
        aes_reg->key[i] = 0;
    }

    for (i = 8 - length; i < 8; i++) {
        tmp = pkey[3] | (pkey[2] << 8) | (pkey[1] << 16) | (pkey[0] << 24);
        aes_reg->key[i] = tmp;
        pkey += 4;
    }

    return 0;
}

/**
* Brief         set  the  vector value
*
* param[in]iv      a pointer point to vector address where the vector is saved
*
* return           0-success , negative-error code.
*/
static int32_t _aes_set_iv(uint8_t *piv)
{
    volatile struct aes_reg_t *aes_reg = (struct aes_reg_t *)ZX29_AES_BASE;
    int32_t i;
    uint32_t tmp;

    for (i = 0; i < 4; i++) {
        tmp = piv[3] | (piv[2] << 8) | (piv[1] << 16) | (piv[0] << 24);
        aes_reg->counter[i] = tmp;
        piv += 4;
    }

    return 0;
}

/**
* Brief         set  the  endian mode
*
* param[in]endian ---- aes_endian_t : AES_Endian_Little or AES_Endian_Big

* return               0-success , negative-error code.
*/
static int32_t _aes_set_endian(enum aes_endian_t endian)
{
    volatile struct aes_reg_t *aes_reg = (struct aes_reg_t *)ZX29_AES_BASE;
    bool aes_is_big_endian;/* pAES->CTRL: endian bit read always return 0 */

    aes_is_big_endian = ((endian == AES_ENDIAN_BIG1) ? (true) : (false));


    /*****  Endian Control bit is bit 3 in mode_ctrl register ******/
    aes_reg->mode_ctrl = ((aes_is_big_endian == true) ? (aes_reg->mode_ctrl | 0x8) : (aes_reg->mode_ctrl & (~0x8)));

    return 0;
}

/**
 * Brief            Set opcode , encrypt or decrypt or key expand
 * param[in]opcode  opcode mode
 *                      AES_OPCODE_ENCRYPT
 *                      AES_OPCODE_DECRYPT
 * Return: 0-success , negative-error code.
 *
*/
static int32_t _aes_set_opcode(enum aes_opcode_t opcode)
{
    volatile struct aes_reg_t *aes_reg = (struct aes_reg_t *)ZX29_AES_BASE;
    uint32_t tmp;

    /****** Opcode Control bits is bit[0] *******/
    tmp = aes_reg->mode_ctrl & (~0x1);
    tmp |= opcode;
    aes_reg->mode_ctrl = tmp;

    return 0;
}

/**
 * Brief: Get opcode , encrypt or decrypt or key expand
 * param[in]: void
 * Return: opcode ---- aes_opcode_t
 *                      AES_OPCODE_ENCRYPT
 *                      AES_OPCODE_DECRYPT
 *
*/
static enum aes_opcode_t _aes_get_opcode(void)
{
    volatile struct aes_reg_t *aes_reg = (struct aes_reg_t *)ZX29_AES_BASE;

    return (aes_reg->mode_ctrl) & 0x1;
}


/**
 * Function: Set Key Length , 128 bits or 192 bits or 256 bits
 * param[in]: keylength ---- aes_keylen_t
 *                         AES_KeyLen_128
 *                         AES_KeyLen_192
 *                         AES_KeyLen_256
 * return               0-success , negative-error code.
 *
*/
static int32_t _aes_set_keylen(enum aes_keylength_t keylength)
{
    volatile struct aes_reg_t *aes_reg = (struct aes_reg_t *)ZX29_AES_BASE;
    uint32_t tmp;

    /******* Key Length control bit is bit[7:6] *****/
    tmp = aes_reg->mode_ctrl & (~(0x3 << 6));
    tmp |= (keylength << 6);
    aes_reg->mode_ctrl = tmp;

    return 0;
}

/**
 * Brief: Set AES Mode, CBC or EBC
 * param[in]: mode ---- aes_mode_t
 *                    AES_MODE_ECB
 *                    AES_MODE_CTR
 *                    AES_MODE_CBC
 *                    AES_MODE_XCBC_MAC_96
return          0-success , negative-error code.
 *
*/
static int32_t _aes_set_mode(enum aes_mode_t mode)
{
    volatile struct aes_reg_t *aes_reg = (struct aes_reg_t *)ZX29_AES_BASE;
    uint32_t tmp;

    /******* Mode control bit is bit [2:1] ********/
    tmp = aes_reg->mode_ctrl & (~(0x3 << 1));
    tmp = tmp | (mode << 1);
    aes_reg->mode_ctrl = tmp;;  //set mode

    return 0;
}

/**
 * Brief: Get AES Mode, CBC or EBC
 * param[in]: None
 * Return: @mode ---- aes_mode_t
                    AES_MODE_ECB
                    AES_MODE_CTR
                    AES_MODE_CBC
                    AES_MODE_XCBC_MAC_96
 *
*/
static enum aes_mode_t _aes_get_mode(void)
{
    volatile struct aes_reg_t *aes_reg = (struct aes_reg_t *)ZX29_AES_BASE;

    return ((aes_reg->mode_ctrl) >> 1) & 0x3;
}

/**
* Brief        Disable data interrupter
*
* param[in]    null
*
* return       null
*/
static void _aes_disable_data_intr(void)
{
    volatile struct aes_reg_t *aes_reg = (struct aes_reg_t *)ZX29_AES_BASE;

    /****** Interrupt Control bit is bit [3:0] ********/
    aes_reg->fifo_int_en &= ~0xf;

}


/**
* Brief        enable aes
*
* param[in]    null
*
* return       null
*/
static void _aes_enable(void)
{
    volatile struct aes_reg_t *aes_reg = (struct aes_reg_t *)ZX29_AES_BASE;

    /****** AES Crypt Control bit is bit 0 ********/
    aes_reg->crypt_en = 1;

}

/**
* Brief        disable aes
*
* param[in]    null
*
* return       null
*/
static void _aes_disable(void)
{
    volatile struct aes_reg_t *aes_reg = (struct aes_reg_t *)ZX29_AES_BASE;

    /****** AES Crypt Control bit is bit 0 ********/
    aes_reg->crypt_en = 0;

}

/**
 * Brief         set 128 bits Data input
 *
 * param[in]pdata   a pointer point to data address which used to save data
 *
 * return           0-success , negative-error code.
 */
static int32_t _aes_set_data(const uint8_t *pdata)
{
    volatile struct aes_reg_t *aes_reg = (struct aes_reg_t *)ZX29_AES_BASE;

    int32_t i;
    _aes_wait_fifo_empty(AES_RX_FIFO);

    /*cbc and xcbc-mac-96 mode can be started  when there is one group data,
    but other aes modes caculate can be started only when there are two groups datas ,
    delete the useless group data when get the result */

    if ((AES_MODE_ECB1 == _aes_get_mode()) || (AES_MODE_CTR1 == _aes_get_mode()) ||
        (AES_MODE_CBC1 == _aes_get_mode())) {
        for (i = 0; i < 4; i++) {
            aes_reg->fifo_data_input = pdata[0] | (pdata[1] << 8) | (pdata[2] << 16) | (pdata[3] << 24);
            pdata += 4;
        }

        for (i = 0; i < 4; i++) {
            aes_reg->fifo_data_input = 0;
        }

    } else {
        for (i = 0; i < 4; i++) {
            aes_reg->fifo_data_input = pdata[0] | (pdata[1] << 8) | (pdata[2] << 16) | (pdata[3] << 24);
            pdata += 4;
        }
    }

    return 0;
}

/**
 * Brief         get 128 bits Data out
 *
 * param[in]data    a pointer point to data address which used to save data
 *
 * return           0-success , negative-error code.
 */
static int32_t _aes_get_data(uint8_t *data)
{
    volatile struct aes_reg_t *aes_reg = (struct aes_reg_t *)ZX29_AES_BASE;
    int32_t i;
    int32_t tmp;
    uint32_t num;

    if ((AES_MODE_ECB1 == _aes_get_mode()) || (AES_MODE_CTR1 == _aes_get_mode()) || (AES_MODE_CBC1 == _aes_get_mode())) {
        num = 8;
    } else {
        num = 4;
    }

    _aes_wait_tx_complete(num);

    /*cbc and xcbc-mac-96 mode can be started  when there is one group data,
    but other aes modes caculate can be started only when there are two groups datas ,
    delete the useless group data when get the result */

    if ((AES_MODE_ECB1 == _aes_get_mode()) || (AES_MODE_CTR1 == _aes_get_mode()) || (AES_MODE_CBC1 == _aes_get_mode())) {
        for (i = 0; i < 4; i++) {
            tmp = aes_reg->fifo_data_output;
            data[3] = (tmp >> 24) & 0xff;
            data[2] = (tmp >> 16) & 0xff;
            data[1] = (tmp >> 8) & 0xff;
            data[0] = (tmp >> 0) & 0xff;
            data += 4;
        }

    } else {
        for (i = 0; i < 4; i++) {
            tmp = aes_reg->fifo_data_output;
            data[3] = (tmp >> 24) & 0xff;
            data[2] = (tmp >> 16) & 0xff;
            data[1] = (tmp >> 8) & 0xff;
            data[0] = (tmp >> 0) & 0xff;
            data += 4;
        }
    }

    _aes_wait_fifo_empty(AES_TX_FIFO);

    return 0;
}
/**
 * Brief         save aes registers to  buf
 *
 * param[in]buf a pointer point to buffer address which used to save context
 *
 * return           0-success , negative-error code.
 */
static uint32_t _aes_save_context(uint32_t *buf)
{
    volatile struct aes_reg_t *aes_reg = (struct aes_reg_t *)ZX29_AES_BASE;
    uint32_t i;
    uint32_t sz = (sizeof(struct aes_reg_t) >> 2);

    for (i = 0; i < (sz - 5); i++) {
        *(buf + i) = *((uint32_t *)aes_reg + i);
    }

    return 0;
}

/**
* Brief         restore  aes registers from buf
*
* param[in]iv      a pointer point to buf address where the aes registers is saved
*
* return           0-success , negative-error code.
*/
static uint32_t _aes_restore_context(uint32_t *buf)
{
    volatile struct aes_reg_t *aes_reg = (struct aes_reg_t *)ZX29_AES_BASE;
    uint32_t i, sz;
    sz = (sizeof(struct aes_reg_t) >> 2);

    for (i = 0; i < (sz - 5); i++) {
        *((uint32_t *)aes_reg + i) = *(buf + i);
    }

    return 0;
}


/*******************************************************************************
*                               Gloable function implementations                *
*******************************************************************************/
/**
 *brief      Initialize aes Interface. 1. Initializes the resources needed for
 *              the aes interface 2.registers event callback function
 *
 *param[in]  idx device id
 *param[in]  cb_event  Pointer to \ref aes_event_cb_t
 *return     pointer to aes handle
*/
aes_handle_t csi_aes_initialize(int32_t idx, aes_event_cb_t cb_event)
{
    if (idx < 0 || idx >= CONFIG_AES_NUM) {
        return NULL;
    }

    uint32_t irq = 0u;
    uint32_t base = 0u;
    /* obtain the aes information */
    int32_t real_idx = target_get_aes(idx, &base, &irq);

    if (real_idx != idx) {
        return NULL;
    }

    ck_aes_priv_t *aes_priv = &aes_handle[idx];

    aes_priv->base = base;

    /* initialize the aes context */
    aes_priv->cb = cb_event;
    aes_priv->status.busy = 0;

    return (aes_handle_t)aes_priv;
}

/**
 * brief       De-initialize aes Interface. stops operation and releases
 * the software resources used by the interface
 * param[in]   handle  aes handle to operate.
 * return      error code
*/
int32_t csi_aes_uninitialize(aes_handle_t handle)
{
    ck_aes_priv_t *aes_priv = handle;
    aes_priv->cb = NULL;

    return 0;
}
/**
 * brief       Get driver capabilities.
 * param[in]   idx device id
 * return      \ref aes_capabilities_t
*/
aes_capabilities_t csi_aes_get_capabilities(int32_t idx)
{
    return driver_capabilities;
}

/**
 * brief       config aes mode.
 * param[in]handle      aes handle to operate.
 * param[in]mode        aes mode ,ref aes_mode_e:ECB/CBC
 * param[in]keylen_bits length of aes key
 * param[in]endian      endian mode of aes,\ref aes_endian_mode_e
 * param[inarg         the address where iv is stored
 * return           0-success , negative-error code.
*/

int32_t csi_aes_config(aes_handle_t handle, aes_mode_e mode,
                       aes_key_len_bits_e keylen_bits, aes_endian_mode_e endian)
{

   // ck_aes_priv_t *aes_priv = handle;

    crypto_aes_mode_set();

    /* disable aes for safe */
    _aes_disable();

    /* reset aes engine */
    _aes_disable_data_intr();
    _aes_disable();

    _aes_set_endian(endian);

    if (mode == AES_MODE_ECB) {
        _aes_set_mode(AES_MODE_ECB1);
    } else if (mode == AES_MODE_CBC) {
      //  aes_priv->iv = (void *)arg;
        _aes_set_mode(AES_MODE_CBC1);
    }

    /* disable aes after initialization*/
    //_aes_disable();
    return 0;
}

/**
 * brief       set key of aes.
 * param[in]handle      aes handle to operate.
 * param[in/out]context caller allocated memory used as internal context
 * param[in]key         pointer where aes key is stored
 * param[in]key_len     length of aes key
 * param[in]enc         aes mode, DECRYPT/ENCRYPT
 *
 * return           0-success , negative-error code.
*/
int32_t csi_aes_set_key(aes_handle_t handle, void *context,
                        void *key, aes_key_len_bits_e key_len, aes_crypto_mode_e enc)
{
    enum aes_keylength_t keylength;

    if (enc == AES_CRYPTO_MODE_DECRYPT) {
        _aes_set_opcode(AES_OPCODE_DECRYPT);
    } else {
        _aes_set_opcode(AES_OPCODE_ENCRYPT);
    }

    if (key_len == AES_KEY_LEN_BITS_128) {
        keylength = AES_KEYLEN_128;
    } else if (key_len == AES_KEY_LEN_BITS_192) {
        keylength = AES_KEYLEN_192;
    } else {
        keylength = AES_KEYLEN_256;
    }

    _aes_set_key(key, keylength);

    _aes_set_keylen(keylength);

    _aes_disable();

    _aes_save_context(buffer);

    return 0;
}

/**
 * brief       aes process.
 * param[in]handle      aes handle to operate.
 * param[in/out]context caller allocated memory used as internal context
 * param[in]in          pointer where src data is stored
 * param[in]len         size of src data
 * param[in]padding     padding type of aes
 * param[out]out        pointer where output data is stored

 *return            0-success , negative-error code.
*/
int32_t csi_aes_crypto(
    aes_handle_t handle, void *context,
    void *in, void *out, uint32_t len,
    unsigned char iv[16])
{
    uint32_t i;
    uint8_t temp_src[SRC_BYTES_COUNT_ONCE];
    uint8_t *src = in;
    uint8_t *dst = out;
    size_t size = len;

    volatile uint32_t loop = size / (SRC_BYTES_COUNT_ONCE);

    ck_aes_priv_t *aes_priv = handle;

    //if (padding != AES_PADDING_MODE_NO) {
    //    return -1;
    //}

    //crypto_aes_mode_set();

    /* disable aes for safe */
    _aes_disable();

#ifdef CRYPTO_DEBUG

    if (AES_MODE_CBC1 == _aes_get_mode()) {
        LOG_HEX("in:", (uint8_t *) in, size);
    }

#endif

    for (i = 0; i < loop; i++) {

        _aes_restore_context(buffer);

        _aes_enable();

        if (AES_MODE_CBC1 == _aes_get_mode()) {

#ifdef CRYPTO_DEBUG
            LOG_HEX("set aes_priv->iv:", (uint8_t *) aes_priv->iv, 16);
#endif
            _aes_set_iv(aes_priv->iv);

            if (AES_OPCODE_DECRYPT == _aes_get_opcode()) {
                memcpy(temp_src, src, IV_BYTES);
            }
        }

        /* set data */
        _aes_set_data(src);

        /* get data */
        _aes_get_data(dst);

        if (AES_OPCODE_ENCRYPT == _aes_get_opcode()) {
            memcpy(aes_priv->iv, dst, IV_BYTES);
        } else {
            memcpy(aes_priv->iv, temp_src, IV_BYTES);
        }

#ifdef  CRYPTO_DEBUG

        if (AES_MODE_CBC1 == _aes_get_mode()) {
            LOG_HEX("aes_priv->iv:", (uint8_t *)aes_priv->iv, 16);
        }

#endif

        src += SRC_BYTES_COUNT_ONCE;
        dst += SRC_BYTES_COUNT_ONCE;
        _aes_disable();

    }

    out = dst - size;
    in = src - size;

#ifdef CRYPTO_DEBUG

    if (_aes_get_mode()) {
        LOG_HEX("out:", (uint8_t *) out, size);
    }

#endif

    crypto_aes_mode_clear();
    return 0;
}

/**
* brief       Get aes status.
* param[in]   handle  aes handle to operate.
* return      aes status \ref aes_status_t
*/
aes_status_t csi_aes_get_status(aes_handle_t handle)
{
    ck_aes_priv_t *aes_priv = handle;
    return aes_priv->status;
}




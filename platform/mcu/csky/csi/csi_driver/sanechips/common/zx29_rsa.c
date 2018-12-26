/**
 * File: zx297100_rsa.c
 * Brief: Implementation of Sanechips rsa
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
*                               Include header files                            *
*******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "drv_rsa.h"
#include "zx29_rsa.h"

/*******************************************************************************
*                               Macro definitions                               *
*******************************************************************************/
#define ERR_RSA(errno) (CSI_DRV_ERRNO_RSA_BASE | errno)
#define RSA_NULL_PARAM_CHK(para)                        \
    do {                                                \
        if (para == NULL) {                             \
            return ERR_RSA(DRV_ERROR_PARAMETER);             \
        }                                               \
    } while (0)

/*******************************************************************************
*                               Type definitions                                *
*******************************************************************************/
typedef struct {
    uint32_t base;
    rsa_data_bits_e data_bit;
    rsa_padding_t padding;
} zx297100_rsa_priv_t;

enum {
    MD5_HASH_SIZE       = 16,
    SHA1_HASH_SIZE      = 20,
    SHA224_HASH_SIZE    = 28,
    SHA256_HASH_SIZE    = 32,
    SHA384_HASH_SIZE    = 48,
    SHA512_HASH_SIZE    = 64,
    MAX_HASH_SIZE       = 64,
};

/*******************************************************************************
*                           Local function declarations                         *
*******************************************************************************/

/*******************************************************************************
*                            Local variable definitions                         *
*******************************************************************************/

/*******************************************************************************
*                           Global variable definitions                         *
*******************************************************************************/
static zx297100_rsa_priv_t rsa_handle[CONFIG_RSA_NUM];

static const rsa_capabilities_t driver_capabilities = {
    .bits_2048 = 1  /* 2048bits modular mode */
};

static const uint8_t der_sha256_t[] = {
    0x30, 0x31,
    0x30, 0x0d,
    0x06, 0x09, 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x01, /* id-sha256 */
    0x05, 0x00,
    0x04, 0x20
};

static const uint8_t der_sha1_t[] = {
    0x30, 0x21,
    0x30, 0x09,
    0x06, 0x05, 0x2b, 0x0e, 0x03, 0x02, 0x1a,
    0x05, 0x00,
    0x04, 0x14
};

static const uint8_t der_md5_t[] = {
    0x30, 0x20, /* type Sequence, length 0x20 (32) */
    0x30, 0x0c, /* type Sequence, length 0x09 */
    0x06, 0x08, /* type OID, length 0x05 */
    0x2a, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x02, 0x05, /* id-md5 */
    0x05, 0x00, /* NULL */
    0x04, 0x10  /* Octet string, length 0x10 (16), followed by md5 hash */
};

/*******************************************************************************
*                           Inline function implementations                     *
*******************************************************************************/

/*******************************************************************************
*                            function   extern                                  *
*******************************************************************************/
extern int32_t target_get_rsa_count(void);
extern int32_t target_get_rsa(int32_t idx, uint32_t *base, uint32_t *irq);

/*******************************************************************************
*                           Local function implementations                      *
*******************************************************************************/

static void crypto_rsa_mode_set()
{
    *(volatile uint32_t *)(ZX29_SECURE_BASE + 0x4) = (1 << 3);
}

static void crypto_rsa_mode_clear()
{
    *(volatile uint32_t *)(ZX29_SECURE_BASE + 0x4) = 0;
}

/**
* Brief        wait rsa caculate done
*
* param[in]    null
*
* return       1: interrupt, calculate finish; 0: no interrupt,  calculating
*/
static uint32_t _rsa_done(void)
{
    volatile struct rsa_reg_t *rsa_reg = (struct rsa_reg_t *)ZX29_RSA_BASE;
    return (rsa_reg->interrupt_status) & 0x1;
}

/**
* Brief        write 1 to clear irq
*
* param[in]    null
*
* return       null
*/
static void _rsa_clear_intr(void)
{
    volatile struct rsa_reg_t  *rsa_reg = (struct rsa_reg_t *)ZX29_RSA_BASE;
    rsa_reg->interrupt_status = 0x1;
}

/**
* Brief        clear  m_data ram where the message to be calculated is stored
*
* param[in]        null
*
* return[out]  null
*/
static void _rsa_clear_mram(void)
{
    uint32_t i;

    volatile struct rsa_reg_t *rsa_reg = (struct rsa_reg_t *)ZX29_RSA_BASE;

    for (i = 0; i < M_DATA_WORDS; i++) {
        rsa_reg->m_data_ram[i] = 0;
    }
}
/**
* Brief        clear  e_data ram where the exponentiation is stored
*
* param[in]    null
*
* return       null
*/
static void _rsa_clear_eram(void)
{
    uint32_t i;

    volatile struct rsa_reg_t *rsa_reg = (struct rsa_reg_t *)ZX29_RSA_BASE;

    for (i = 0; i < E_DATA_WORDS; i++) {
        rsa_reg->e_data_ram[i] = 0;
    }
}
/**
* Brief        clear  n_data ram where the modular  is stored
*
* param[in]    null
*
* return       null
*/
static void _rsa_clear_nram(void)
{
    uint32_t i;

    volatile struct rsa_reg_t *rsa_reg = (struct rsa_reg_t *)ZX29_RSA_BASE;

    for (i = 0; i < N_DATA_WORDS; i++) {
        rsa_reg->n_data_ram[i] = 0;
    }
}
/**
* Brief        clear  c_data ram where the caculated result under RSA_INIT_CAL mode
*
* param[in]    null
*
* return       null
*/
static void _rsa_clear_cram(void)
{
    uint32_t i;

    volatile struct rsa_reg_t *rsa_reg = (struct rsa_reg_t *)ZX29_RSA_BASE;

    for (i = 0; i < C_DATA_WORDS; i++) {
        rsa_reg->c_data_ram[i] = 0;
    }
}

/**
* Brief        calculate  the n0_inv---
*
* param[in]    n0  the lowest 32 bits of N  parameter
*
* return       the n0_inv---
*/
static uint32_t _rsa_cal_n0_inv(uint32_t  n0)
{

    uint32_t n0_inv = 1;
    uint32_t i;
    uint32_t a;
    uint32_t b;

    for (i = 1; i < 32; i++) {
        a = 1 << i;
        b = (n0 * n0_inv) & ((2 << i) - 1);

        if (a < b) {
            n0_inv = n0_inv + a;
        }
    }

    return (0xffffffff - n0_inv + 1);
}
/**
* Brief        write the lenth of modulus to modular_len register
*
* param[in]len lenth  of modulus
*
* return           0-success , negative-error code.
*/
static void _rsa_set_modular_len(uint32_t len)
{
    volatile struct rsa_reg_t *rsa_reg = (struct rsa_reg_t *)ZX29_RSA_BASE;
    rsa_reg->modular_len = len;
}

/**
* Brief        write the lenth of exponentiation to exponentiation_len register
*
* param[in]e_len   width of exponentiation length
*
* return[out]      0-success , negative-error code.
*/
static void _rsa_set_exponentiation_len(uint32_t len)
{
    volatile struct rsa_reg_t *rsa_reg = (struct rsa_reg_t *)ZX29_RSA_BASE;
    rsa_reg->exponentiation_len = len;
}

/**
* Brief        write rsa calculate mode to calculate_mode register
*
* param[in]cal_mode        rsa calculate mode
*
* return                   0-success , negative-error code.
*/
static void _rsa_set_calculate_mode(enum rsa_calmode cal_mode)
{
    volatile struct rsa_reg_t *rsa_reg = (struct rsa_reg_t *)ZX29_RSA_BASE;
    rsa_reg->calculate_mode = cal_mode;
}

/**
* Brief        get lenth of len data  from a pointer point to ram address
           to write them to address with the start of udReg
*
* param[in]cal_mode        rsa calculate mode
*
* return       null
*/
static void _rsa_write_data_to_register(uint8_t *ram_addr, uint32_t udReg, uint32_t len)
{
    uint32_t udi;
    volatile uint32_t tmp;

    for (udi = 0; udi < len; udi++) {
        if (udReg == RSA_M_RAM) {
            //模块低地址寄存器存放低地址数据，数据存放是低地址放高位数据
            tmp = ram_addr[len * 4 - udi * 4 - 4] | (ram_addr[len * 4 - udi * 4 - 3] << 8) |
                  (ram_addr[len * 4 - udi * 4 - 2] << 16) | (ram_addr[len * 4 - udi * 4 - 1] << 24);
            tmp = tmp >> 24 | ((tmp & 0xff0000) >> 8) |
                  ((tmp & 0xff00) << 8) | ((tmp & 0xff) << 24);
            *(volatile uint32_t *)(udReg + udi * 4) = tmp;

        } else {
            *(volatile uint32_t *)(udReg + udi * 4) = ram_addr[udi * 4 + 0] |
                    (ram_addr[udi * 4 + 1] << 8) |
                    (ram_addr[udi * 4 + 2] << 16) |
                    (ram_addr[udi * 4 + 3] << 24);
        }
    }
}

/**
* Brief        get lenth of len data  from address with the start of udReg to
           write them to  a pointer point to ram address
*
* param[in]cal_mode        rsa calculate mode
*
* return       null
*/
static void _rsa_read_dada_from_register(uint8_t *ram_addr, uint32_t udReg, uint32_t len)
{
    uint32_t udi;
    uint32_t tmp;

    for (udi = 0; udi < len; udi++) {
        //模块低地址寄存器存放低地址数据，数据存放是低地址放高位数据
        tmp = *(volatile uint32_t *)(udReg + udi * 4);
        ram_addr[len * 4 - udi * 4 - 4] = tmp >> 24;
        ram_addr[len * 4 - udi * 4 - 3] = ((tmp & 0xff0000) >> 16);
        ram_addr[len * 4 - udi * 4 - 2] = ((tmp & 0xff00) >> 8);
        ram_addr[len * 4 - udi * 4 - 1] = ((tmp & 0xff) >> 0);

    }
}

/**
 * Brief        init calculate
 *
 * param[in]m_bit_len   module length
 * param[in]pmram       a pointer point to buf address where the data input is saved
 * param[in]presult     a pointer point to buf address where use to store the rsa result
 *
 * return               0-success , negative-error code.
 */
static uint32_t _rsa_init_calculate(uint32_t m_bit_len, uint8_t *pmram, uint8_t *presult)
{
    uint32_t mlen_word;
    uint32_t temp;
    volatile struct rsa_reg_t *rsa_reg = (struct rsa_reg_t *)ZX29_RSA_BASE;

    //input N, m_bit_len
    if (m_bit_len > 2048 || m_bit_len == 0 || pmram == NULL || presult == NULL) {
        return 1;
    }

    rsa_reg->interrupt_mask   = 0;  /*unmask interrupt*/
    rsa_reg->interrupt_enable   = 0;/*disable interrupt*/

    //配置N 到RAM寄存器中
    mlen_word = (m_bit_len + 31) / 32; //0<mlen_word<=64
    _rsa_write_data_to_register(pmram, RSA_M_RAM, mlen_word);
    //highest word of N, if the highest bit of N is 0, set (m_bit_len - 2) to  rsa_reg->modular_len
    temp = (pmram[mlen_word * 4 - 4] | (pmram[mlen_word * 4 - 3] << 8) |
            (pmram[mlen_word * 4 - 2] << 16) | (pmram[mlen_word * 4 - 1] << 24));
    //LOG("\n enc highest word:0x%x;\n",temp);

    /*set modelength, unit:bit*/
    if ((temp & 0x80000000) == 0) {

        //rsa_reg->modular_len = m_bit_len-2;/*set modelength, 单位bit*/
        _rsa_set_modular_len(m_bit_len - 2);
    } else {
        // rsa_reg->modular_len  = m_bit_len-1;/*set modelength, 单位bit*/
        _rsa_set_modular_len(m_bit_len - 1);
    }

    //enable compute
    rsa_reg->module_enable = 1;

    /*check interrupt status, waiting for calculating finished*/
    while (!_rsa_done());

    /* clear the interrupt,input any */
    _rsa_clear_intr();

    /* read the result */
    _rsa_read_dada_from_register(presult, RSA_CALC_RAM, mlen_word);

    /*close mode enable*/
    rsa_reg->module_enable = 0;

    return 0;

}

/**
 * Brief        big_num_multi
 *
 * param[in]m_bit_len   module length
 * param[in]pmram       a pointer point to buf address where the data input is saved
 * param[in]peram       a pointer point to buf address where the module is saved
 * param[in]presult     a pointer point to buf address where use to store the rsa result
 *
 * return               0-success , negative-error code.
 */
static uint32_t _rsa_big_num_multi(
    uint32_t m_bit_len, uint8_t *pmram,
    uint8_t *peram, uint8_t *presult)
{
    uint32_t  mlen_word;
    volatile struct rsa_reg_t *rsa_reg = (struct rsa_reg_t *)ZX29_RSA_BASE;

    //input M, E, m_bit_len
    if (m_bit_len > 2048 || m_bit_len == 0 || pmram == NULL || peram == NULL || presult == NULL) {
        return 1;
    }

    rsa_reg->interrupt_mask   = 0;  /*unmask interrupt*/
    rsa_reg->interrupt_enable   = 0;/*disable interrupt*/

    //配置M,E 到RAM寄存器中
    mlen_word = (m_bit_len + 31) / 32;
    _rsa_write_data_to_register(pmram, RSA_M_RAM, mlen_word);
    _rsa_write_data_to_register(peram, RSA_E_RAM, mlen_word);

    //配置N 的bit 长度
    // rsa_reg->modular_len = m_bit_len-1;/*set modelength, 单位bit*//* different*/
    _rsa_set_modular_len(m_bit_len - 1);

    //enable compute
    rsa_reg->module_enable = 1;

    /*check interrupt status, waiting for calculating finished*/
    while (!_rsa_done());

    /* clear the interrupt,input any */
    _rsa_clear_intr();

    /* read the result */
    _rsa_read_dada_from_register(presult, RSA_RESULT_RAM, mlen_word * 2);

    /*close mode enable*/
    rsa_reg->module_enable = 0;

    return 0;

}

/**
 * Brief        multi_no_init
 *
 * param[in]m_bit_len   module length
 * param[in]e_bit_len   exponentiation length
 * param[in]n0_inv      the n0_inv
 * param[in]pmram       a pointer point to buf address where the data input is saved
 * param[in]peram       a pointer point to buf address where the module is saved
 * param[in]pnram       a pointer point to buf address where the exponentiation is saved
 * param[in]pcram       a pointer point to buf address where use to store the init calculate result
 * param[in]presult     a pointer point to buf address where use to store the rsa result
 *
 * return               0-success , negative-error code.
 */
static uint32_t _rsa_mod_multi_no_init(
    uint32_t m_bit_len, uint32_t e_bit_len, uint32_t n0_inv,
    uint8_t *pmram, uint8_t *peram, uint8_t *pnram,
    uint8_t *pcram, uint8_t *presult)
{
    uint32_t mlen_word;
    uint32_t elen_word;
    uint32_t temp;
    volatile struct rsa_reg_t *rsa_reg = (struct rsa_reg_t *)ZX29_RSA_BASE;

    //input M, E, N, C, m_bit_len, e_bit_len
    if (m_bit_len > 2048 || m_bit_len == 0 || e_bit_len > 2048 || e_bit_len == 0 || pmram == NULL || peram == NULL || pnram == NULL || pcram == NULL || presult == NULL) {
        return 1;
    }

    rsa_reg->interrupt_mask   = 0;  /*unmask interrupt*/
    rsa_reg->interrupt_enable   = 0;/*disable interrupt*/

    mlen_word = (m_bit_len + 31) / 32;
    elen_word = (e_bit_len + 31) / 32;

    //loweset word of N
    temp = pnram[0] | (pnram[1] << 8) | (pnram[2] << 16) | (pnram[3] << 24);

    //LOG("\n enc loweset word:0x%x;\n",temp);

    //set loweset word to rsa_reg->nzero
    rsa_reg->nzero = temp;
    // 设置参数N0'  ,    等于- N^-1 mod 2^32
    rsa_reg->nzero_inv = _rsa_cal_n0_inv(temp);


    //配置M,E,N 到RAM寄存器中
    _rsa_write_data_to_register(pmram, RSA_M_RAM, mlen_word);
    _rsa_write_data_to_register(peram, RSA_E_RAM, elen_word);
    _rsa_write_data_to_register(pnram, RSA_N_RAM, mlen_word);
    _rsa_write_data_to_register(pcram, RSA_CALC_RAM, mlen_word);

    //highest word of N, if the highest bit of N is 0, set (m_bit_len - 2) to  rsa_reg->modular_len
    temp = (pnram[mlen_word * 4 - 4] | (pnram[mlen_word * 4 - 3] << 8) |
            (pnram[mlen_word * 4 - 2] << 16) | (pnram[mlen_word * 4 - 1] << 24));
    //LOG("\n enc highest word:0x%x;\n",temp);

    /*set modelength, unit:bit*/
    if ((temp & 0x80000000) == 0) {

        //rsa_reg->modular_len = m_bit_len-2;/*set modelength, 单位bit*/
        _rsa_set_modular_len(m_bit_len - 2);
    } else {
        // rsa_reg->modular_len = m_bit_len-1;/*set modelength, 单位bit*/
        _rsa_set_modular_len(m_bit_len - 1);
    }

    //rsa_reg->exponentiation_len = (elen_word-1)<<5;      /*set expolength, 单位word*/
    _rsa_set_exponentiation_len((elen_word - 1) << 5);

    //enable compute
    rsa_reg->module_enable = 1;

    /*check interrupt status, waiting for calculating finished*/
    while (!_rsa_done());

    /* clear the interrupt,input any */
    _rsa_clear_intr();

    /* read the result */
    _rsa_read_dada_from_register(presult, RSA_RESULT_RAM, mlen_word);

    /*close mode enable*/
    rsa_reg->module_enable = 0;

    return 0;
}

/**
 * Brief        multi_with_init
 *
 * param[in]m_bit_len   module length
 * param[in]e_bit_len   exponentiation length
 * param[in]n0_inv      the n0_inv
 * param[in]pmram       a pointer point to buf address where the data input is saved
 * param[in]peram       a pointer point to buf address where the module is saved
 * param[in]pnram       a pointer point to buf address where the exponentiation is saved
 * param[in]presult     a pointer point to buf address where use to store the rsa result
 *
 * return               0-success , negative-error code.
 */
static uint32_t _rsa_mod_multi_with_init(
    uint32_t m_bit_len, uint32_t e_bit_len, uint32_t n0_inv,
    uint8_t *pmram, uint8_t *peram, uint8_t *pnram, uint8_t *presult)
{
    uint32_t mlen_word;
    uint32_t elen_word;
    uint32_t temp;

    volatile struct rsa_reg_t *rsa_reg = (struct rsa_reg_t *)ZX29_RSA_BASE;

    //input M, E, N, m_bit_len, e_bit_len
    if (m_bit_len > 2048 || m_bit_len == 0 || e_bit_len > 2048 || e_bit_len == 0 ||
        pmram == NULL || peram == NULL || pnram == NULL || presult == NULL) {
        return 1;
    }

    rsa_reg->interrupt_mask     = 0;    /*unmask interrupt*/
    rsa_reg->interrupt_enable   = 0;/*disable interrupt*/

    mlen_word = (m_bit_len + 31) / 32;
    elen_word = (e_bit_len + 31) / 32;

    //loweset word of N
    temp = pnram[0] | (pnram[1] << 8) | (pnram[2] << 16) | (pnram[3] << 24);

    //LOG("\n enc loweset word:0x%x;\n",temp);

    //set loweset word to rsa_reg->nzero
    rsa_reg->nzero = temp;
    // 设置参数N0'  ,    等于- N^-1 mod 2^32
    rsa_reg->nzero_inv = _rsa_cal_n0_inv(temp);

    //配置M,E,N 到RAM寄存器中
    _rsa_write_data_to_register(pmram, RSA_M_RAM, mlen_word);
    _rsa_write_data_to_register(peram, RSA_E_RAM, elen_word);
    _rsa_write_data_to_register(pnram, RSA_N_RAM, mlen_word);


    //highest word of N, if the highest bit of N is 0, set (m_bit_len - 2) to  rsa_reg->modular_len
    temp = (pnram[mlen_word * 4 - 4] | (pnram[mlen_word * 4 - 3] << 8) |
            (pnram[mlen_word * 4 - 2] << 16) | (pnram[mlen_word * 4 - 1] << 24));
    //LOG("\n enc highest word:0x%x;\n",temp);

    /*set modelength, unit:bit*/
    if ((temp & 0x80000000) == 0) {
        _rsa_set_modular_len(m_bit_len - 2);
    } else {
        _rsa_set_modular_len(m_bit_len - 1);
    }

    /*set expolength, uint:word*/
    _rsa_set_exponentiation_len((elen_word - 1) << 5);

    //enable compute
    rsa_reg->module_enable = 1;

    /*check interrupt status, waiting for calculating finished*/
    while (!_rsa_done());

    /* clear the interrupt,input any */
    _rsa_clear_intr();

    /* read the result */
    _rsa_read_dada_from_register(presult, RSA_RESULT_RAM, mlen_word);

    /*close mode enable*/
    rsa_reg->module_enable = 0;

    return 0;
}

/**
 * Brief        expon_no_init
 *
 * param[in]m_bit_len   module length
 * param[in]e_bit_len   exponentiation length
 * param[in]n0_inv      the n0_inv
 * param[in]pmram       a pointer point to buf address where the data input is saved
 * param[in]peram       a pointer point to buf address where the module is saved
 * param[in]pnram       a pointer point to buf address where the exponentiation is saved
 * param[in]pcram       a pointer point to buf address where use to store the init calculate result
 * param[in]presult     a pointer point to buf address where use to store the rsa result
 *
 * return               0-success , negative-error code.
 */
static uint32_t _rsa_mod_expon_no_init(
    uint32_t m_bit_len, uint32_t e_bit_len, uint32_t n0_inv,
    uint8_t *pmram, uint8_t *peram, uint8_t *pnram,
    uint8_t *pcram, uint8_t *presult)
{
    uint32_t mlen_word;
    uint32_t elen_word;
    uint32_t temp;
    volatile struct rsa_reg_t *rsa_reg = (struct rsa_reg_t *)ZX29_RSA_BASE;

    //input M, E, N, C, m_bit_len, e_bit_len
    if (m_bit_len > 2048 || m_bit_len == 0 || e_bit_len > 2048 || e_bit_len == 0 ||
        pmram == NULL || peram == NULL || pnram == NULL || pcram == NULL || presult == NULL) {
        return 1;
    }

    rsa_reg->interrupt_mask   = 0;                          /*unmask interrupt*/
    rsa_reg->interrupt_enable   = 0;                        /*disable interrupt*/

    mlen_word = (m_bit_len + 31) / 32;
    elen_word = (e_bit_len + 31) / 32;

    //loweset word of N
    temp = pnram[0] | (pnram[1] << 8) | (pnram[2] << 16) | (pnram[3] << 24);

    //LOG("\n enc loweset word:0x%x;\n",temp);

    //set loweset word to rsa_reg->nzero
    rsa_reg->nzero = temp;
    // 设置参数N0'  ,    等于- N^-1 mod 2^32
    rsa_reg->nzero_inv = _rsa_cal_n0_inv(temp);

    //配置M,E,N 到RAM寄存器中
    _rsa_write_data_to_register(pmram, RSA_M_RAM, mlen_word);
    _rsa_write_data_to_register(peram, RSA_E_RAM, elen_word);
    _rsa_write_data_to_register(pnram, RSA_N_RAM, mlen_word);
    _rsa_write_data_to_register(pcram, RSA_CALC_RAM, mlen_word);


    //highest word of N, if the highest bit of N is 0, set (m_bit_len - 2) to  rsa_reg->modular_len
    temp = (pnram[mlen_word * 4 - 4] | (pnram[mlen_word * 4 - 3] << 8) |
            (pnram[mlen_word * 4 - 2] << 16) | (pnram[mlen_word * 4 - 1] << 24));
    //LOG("\n enc highest word:0x%x;\n",temp);

    /*set modelength, unit:bit*/
    if ((temp & 0x80000000) == 0) {
        _rsa_set_modular_len(m_bit_len - 2);
    } else {
        _rsa_set_modular_len(m_bit_len - 1);
    }

    /*set expolength, uint:word*/
    _rsa_set_exponentiation_len((elen_word - 1) << 5);


    //enable compute
    rsa_reg->module_enable = 1;

    /*check interrupt status, waiting for calculating finished*/

    while (!_rsa_done());

    /* clear the interrupt,input any */
    _rsa_clear_intr();
    /* read the result */
    _rsa_read_dada_from_register(presult, RSA_RESULT_RAM, mlen_word);

    /*close mode enable*/
    rsa_reg->module_enable = 0;


    return 0;
}

/**
 * Brief        expon_with_init
 *
 * param[in]m_bit_len   module length
 * param[in]e_bit_len   exponentiation length
 * param[in]n0_inv      the n0_inv
 * param[in]pmram       a pointer point to buf address where the data input is saved
 * param[in]peram       a pointer point to buf address where the module is saved
 * param[in]pnram       a pointer point to buf address where the exponentiation is saved
 * param[in]presult     a pointer point to buf address where use to store the rsa result
 *
 * return               0-success , negative-error code.
 */
static uint32_t _rsa_mod_expon_with_init(
    uint32_t m_bit_len, uint32_t e_bit_len, uint32_t n0_inv,
    uint8_t *pmram, uint8_t *peram, uint8_t *pnram, uint8_t *presult)
{
    uint32_t mlen_word;
    uint32_t elen_word;
    uint32_t temp;
    volatile struct rsa_reg_t *rsa_reg = (struct rsa_reg_t *)ZX29_RSA_BASE;

    //input M, E, N, m_bit_len, e_bit_len
    if (m_bit_len > 2048 || m_bit_len == 0 || e_bit_len > 2048 || e_bit_len == 0 ||
        pmram == NULL || peram == NULL || pnram == NULL || presult == NULL) {
        return -1;
    }

    rsa_reg->interrupt_mask   = 0;  /*unmask interrupt*/
    rsa_reg->interrupt_enable   = 0;/*disable interrupt*/

    mlen_word = (m_bit_len + 31) / 32;
    elen_word = (e_bit_len + 31) / 32;

    //loweset word of N
    temp = pnram[0] | (pnram[1] << 8) | (pnram[2] << 16) | (pnram[3] << 24);

    //LOG("\n enc loweset word:0x%x;\n",temp);

    //set loweset word to rsa_reg->nzero
    rsa_reg->nzero = temp;
    // 设置参数N0'  ,    等于- N^-1 mod 2^32
    rsa_reg->nzero_inv = _rsa_cal_n0_inv(temp);

    //配置M,E,N 到RAM寄存器中
    _rsa_write_data_to_register(pmram, RSA_M_RAM, mlen_word);
    _rsa_write_data_to_register(peram, RSA_E_RAM, elen_word);
    _rsa_write_data_to_register(pnram, RSA_N_RAM, mlen_word);


    //highest word of N, if the highest bit of N is 0, set (m_bit_len - 2) to  rsa_reg->modular_len
    temp = (pnram[mlen_word * 4 - 4] | (pnram[mlen_word * 4 - 3] << 8) |
            (pnram[mlen_word * 4 - 2] << 16) | (pnram[mlen_word * 4 - 1] << 24));
    //LOG("\n enc highest word:0x%x;\n",temp);

    /*set modelength, unit:bit*/
    if ((temp & 0x80000000) == 0) {
        _rsa_set_modular_len(m_bit_len - 2);
    } else {
        _rsa_set_modular_len(m_bit_len - 1);
    }

    /*set expolength, uint:word*/
    _rsa_set_exponentiation_len((elen_word - 1) << 5);


    //enable compute
    rsa_reg->module_enable = 1;

    /*check interrupt status, waiting for calculating finished*/
    while (!_rsa_done());

    /* clear the interrupt,input any */
    _rsa_clear_intr();

    /* read the result */
    _rsa_read_dada_from_register(presult, RSA_RESULT_RAM, mlen_word);

    /*close mode enable*/
    rsa_reg->module_enable = 0;

    return 0;
}
/**
 * Brief        rsa caculate process
 *
 * param[in]input       the pointer to the arguments
 * param[in]cal_mode    rsa calculate mode
 *
 * return               0-success , negative-error code.
 */
static int32_t _rsa_calculate(uint32_t mod_bits, uint32_t expon_bits,
                              uint8_t *n, uint8_t *e,
                              uint8_t *m, uint8_t *c, uint8_t *r,
                              enum rsa_calmode cal_mode)
{
    switch (cal_mode) {

        case RSA_INIT_CAL:
            return _rsa_init_calculate(mod_bits, n, r);

        case RSA_BIG_NUM_CAL:
            return _rsa_big_num_multi(mod_bits, n, e, r);

        case RSA_MOD_MULTI_NO_INIT_CAL:
            return _rsa_mod_multi_no_init(mod_bits, expon_bits, 0, m, e, n, c, r);

        case RSA_MOD_MULTI_WITH_INIT_CAL:
            return _rsa_mod_multi_with_init(mod_bits, expon_bits, 0, m, e, n, r);

        case RSA_MOD_EXPON_NO_INIT_CAL:
            return _rsa_mod_expon_no_init(mod_bits, expon_bits, 0, m, e, n , c, r);

        case RSA_MOD_EXPON_WITH_INIT_CAL:
            return _rsa_mod_expon_with_init(mod_bits, expon_bits, 0, m, e, n, r);

        default:
            return 1;
    }
}

/**
 * Brief        _rsa_drv_process
 *
 * param[in]n           a pointer point to buf address where the mudule is saved
 * param[in]n_size      the module size
 * param[in]e           a pointer point to buf address where the d/e is saved
 * param[in]e_size      the d/e size
 * param[in]src         a pointer point to buf address where the src data is saved
 * param[in]src_size    the src data size
 * param[in]dst         a pointer point to buf address where the dst data is saved
 * param[in]dst_size    the dst data size
 *
 * return               0-success , negative-error code.
 */
static int32_t _rsa_drv_process(
    const uint8_t *n, size_t n_size,
    const uint8_t *e, size_t e_size,
    const uint8_t *src, size_t src_size,
    uint8_t *dst, size_t *dst_size)
{
    if ((n == NULL) || (e == NULL) || (n_size != KEY_BYTES) || (e_size != KEY_BYTES) ||
        (dst == NULL) || (src == NULL)) {
        return ERR_RSA(DRV_ERROR_PARAMETER);
    }

    uint8_t cram[KEY_BYTES];
    int32_t ret = 0;
    uint32_t i;
    uint32_t loop = src_size / KEY_BYTES;
    enum rsa_calmode cal_mode = RSA_MOD_EXPON_WITH_INIT_CAL;

    /*select rsa mode*/
    crypto_rsa_mode_set();

    /* clear and disable intr */
    _rsa_clear_intr();

    /*set calculate_mode*/
    _rsa_set_calculate_mode(cal_mode);

    for (i = 0; i < loop; i++) {
        /*clear ram before calculte*/
        _rsa_clear_mram();
        _rsa_clear_eram();
        _rsa_clear_nram();
        _rsa_clear_cram();

        ret = _rsa_calculate(KEY_BITS, KEY_BITS,
                             (uint8_t *)n, (uint8_t *)e,
                             (uint8_t *)src, (uint8_t *)&cram,
                             (uint8_t *)dst, cal_mode);
        src += (KEY_BITS / 32);
        dst += (KEY_BITS / 32);
    }

    *dst_size = KEY_BYTES * loop;

    crypto_rsa_mode_clear();

    return ret;

}

/**
 * Brief        padding process for rsa_encrypt
 *
 * param[in]src             a pointer point to buf address where the src data is saved
 * param[in]src_size        the src data size
 * param[in]src_padded      a pointer point to buf address where the padded data is saved
 * param[in]src_padded_size the paddded data size
 * param[in]padding         the padding type
 * param[in]pub_or_pri      encrypt/decrypt by public or private key
 *
 * return               0-success , negative-error code.
 */
static int32_t _rsa_drv_padding_pkcs(
    const uint8_t *src, size_t src_size,
    uint8_t *src_padded, size_t *src_padded_size ,
    rsa_padding_t padding, uint32_t pub_or_pri)
{
    uint32_t i;
    uint8_t *p;
    uint32_t pslen;

    if ((src == NULL) || (src_padded == NULL) || (src_padded_size == NULL) || (src_size == 0)) {
        return ERR_RSA(DRV_ERROR_PARAMETER);
    }

    if (padding.padding_type == RSA_PADDING_MODE_PKCS1) {

        p = src_padded;

        *(p++) = 0x00;

        if (pub_or_pri == RSA_PUBLIC) {
            *(p++) = 0x02;
        } else {
            *(p++) = 0x01;
        }

        /* pad out with 0xff data */
        pslen = KEY_BYTES - 3 - src_size;

        for (i = 0; i < pslen; i++) {
            p[i] = 0xff; /* PS */
        }

        p += pslen;
        *(p++) = 0x0;

        for (i = 0; i < src_size; i++) {
            p[i] = src[i];
        }

        *src_padded_size = KEY_BYTES;

    } else {
        memcpy(src_padded, src, src_size);
        *src_padded_size = src_size;
    }

    return 0;
}

/**
 * Brief        dispadding process for rsa_decrypt
 *
 * param[in]src                     a pointer point to buf address where the src data is saved
 * param[in]src_size                the src data size
 * param[in]src_disapadded          a pointer point to buf address where the dispadded data is saved
 * param[in]src_disapadded_size     the dispaddded data size
 * param[in]padding                 the padding type
 *
 * return               0-success , negative-error code.
 */
static int32_t _rsa_drv_dispadding_pkcs(
    const uint8_t *src, size_t src_size,
    uint8_t *src_disapadded, size_t  *src_disapadded_size ,
    rsa_padding_t padding)
{
    uint32_t i;
    uint8_t *p = (uint8_t *)src;
    uint32_t pslen = src_size;

    if ((src == NULL) || (src_disapadded == NULL) || (src_disapadded_size == NULL) ||
        (src_size == 0)) {
        return ERR_RSA(DRV_ERROR_PARAMETER);
    }

    if (padding.padding_type == RSA_PADDING_MODE_PKCS1) {
        p++;
        p++;
        pslen = src_size - 2;

        //fisrt two bytes after paddded are 0x00,0x01/0x02,and the last paddded byte is 0x00
        while (pslen--) {
            if (*(p++) == 0x0) {
                break;
            }
        }

        *src_disapadded_size = pslen;

        for (i = 0; i < pslen; i++) {
            src_disapadded[i] = *(p + i);
        }
    } else {
        memcpy(src_disapadded, src, src_size);
        *src_disapadded_size = src_size;
    }

    return 0;
}

/**
 * Brief        padding process for rsa_sign
 *
 * param[in]src             a pointer point to buf address where the src data is saved
 * param[in]src_size        the src data size
 * param[in]src_padded      a pointer point to buf address where the padded data is saved
 * param[in]src_padded_size the paddded data size
 * param[in]padding         the padding type
 * param[in]pub_or_pri      encrypt/decrypt by public or private key
 *
 * return               0-success , negative-error code.
 */
static int32_t _rsa_drv_es_padding_pkcs(
    const uint8_t *src, size_t src_size,
    uint8_t *src_padded, size_t *src_padded_size ,
    rsa_padding_t padding, uint32_t pub_or_pri)
{
    uint32_t i;
    uint8_t *p;
    uint8_t *der;
    uint32_t der_len;
    uint32_t hashlen;
    uint32_t pslen;

    if ((src == NULL) || (src_padded == NULL) || (src_padded_size == NULL) || (src_size == 0)) {
        return ERR_RSA(DRV_ERROR_PARAMETER);;
    }

    if (padding.hash_type == RSA_HASH_TYPE_MD5) {
        der     = (uint8_t *)der_md5_t;
        der_len = sizeof(der_md5_t);
        hashlen = MD5_HASH_SIZE;
    } else if (padding.hash_type == RSA_HASH_TYPE_SHA1) {
        der     = (uint8_t *)der_sha1_t;
        der_len = sizeof(der_sha1_t);
        hashlen = SHA1_HASH_SIZE;
    } else if (padding.hash_type == RSA_HASH_TYPE_SHA256) {
        der     = (uint8_t *)der_sha256_t;
        der_len = sizeof(der_sha256_t);
        hashlen = SHA256_HASH_SIZE;
    } else {
        return ERR_RSA(DRV_ERROR_PARAMETER);
    }

    p = src_padded;

    *(p++) = 0x00;

    if (pub_or_pri == RSA_PUBLIC) {
        *(p++) = 0x02;
    } else {
        *(p++) = 0x01;
    }

    /* pad out with 0xff data */
    pslen = KEY_BYTES - 3 - hashlen - der_len;

    for (i = 0; i < pslen; i++) {
        p[i] = 0xff; /* PS 0xff*/
    }

    p += pslen;
    *(p++) = 0x0;

    for (i = 0; i < der_len; i++) {
        p[i] = der[i]; /*PS der*/
    }

    p += der_len;

    for (i = 0; i < hashlen; i++) {
        p[i] = src[i];
    }

    *src_padded_size = KEY_BYTES;

    return 0;
}

/**
 * Brief        dispadding process for rsa_verify
 *
 * param[in]src                     a pointer point to buf address where the src data is saved
 * param[in]src_size                the src data size
 * param[in]src_disapadded          a pointer point to buf address where the dispadded data is saved
 * param[in]src_disapadded_size     the dispaddded data size
 * param[in]padding                 the padding type
 * param[in]pub_or_pri              encrypt/decrypt by public or private key
 *
 * return               0-success , negative-error code.
 */
static int32_t _rsa_drv_es_dispadding_pkcs(
    const uint8_t *src, size_t src_size,
    uint8_t *src_disapadded, size_t  *src_disapadded_size ,
    rsa_padding_t padding, uint32_t pub_or_pri)
{
    uint32_t i;
    uint32_t ret;
    const uint8_t *p;
    uint8_t *der;
    uint32_t der_len;
    uint32_t hashlen;
    uint32_t pslen;

    if (padding.hash_type == RSA_HASH_TYPE_MD5) {
        der     = (uint8_t *)der_md5_t;
        der_len = sizeof(der_md5_t);
        hashlen = MD5_HASH_SIZE;
    } else if (padding.hash_type == RSA_HASH_TYPE_SHA1) {
        der     = (uint8_t *)der_sha1_t;
        der_len = sizeof(der_sha1_t);
        hashlen = SHA1_HASH_SIZE;
    } else if (padding.hash_type == RSA_HASH_TYPE_SHA256) {
        der     = (uint8_t *)der_sha256_t;
        der_len = sizeof(der_sha256_t);
        hashlen = SHA256_HASH_SIZE;
    } else {
        return ERR_RSA(DRV_ERROR_PARAMETER);
    }

    pslen = KEY_BYTES - 3 - der_len - hashlen;

    p = src;
    p++;

    if ((pub_or_pri == RSA_PUBLIC) && (*(p) != 0x01)) {
        ret = -1;
        goto _verify_fail;
    } else if ((pub_or_pri == RSA_PRIVATE) && (*(p) != 0x02)) {
        ret = -1;
        goto _verify_fail;
    }

    p++;

    /* scan PS */
    for (i = 0; i < pslen; i++) {
        if (*(p + i) != 0xff) {
            ret = -2;
            goto _verify_fail;
        }
    }

    p += pslen;

    if ((*p) != 0x00) {
        ret = -1;
        goto _verify_fail;
    }

    p++;

    /* scan der*/
    for (i = 0; i < der_len; i++) {
        if (*(p + i) != der[i]) {
            ret = -3;
            goto _verify_fail;
        }
    }

    p += der_len;

    for (i = 0; i < hashlen; i++) {
        src_disapadded[i] = *(p + i);
#if 0

        if (*(p + i) != src[i]) {
            ret = -4;
            goto _verify_fail;
        }

#endif
    }

    *src_disapadded_size = hashlen;

    return 0;

_verify_fail:

    return ret;
}

/*******************************************************************************
*                           gloable function implementations                    *
*******************************************************************************/
/**
*brief       Initialize RSA Interface. 1. Initializes the resources needed for
*               the RSA interface 2.registers event callback function
*
*param[in]   idx device id
*param[in]   cb_event  Pointer to \ref rsa_event_cb_t
*return      pointer to rsa handle
*/
rsa_handle_t csi_rsa_initialize(int32_t idx, rsa_event_cb_t cb_event)
{
    if (idx < 0 || idx >= CONFIG_RSA_NUM) {
        return NULL;
    }

    /* obtain the rsa information */
    uint32_t base = 0u;
    int32_t real_idx = target_get_rsa(idx, &base, NULL);

    if (real_idx != idx) {
        return NULL;
    }

    zx297100_rsa_priv_t *rsa_priv = &rsa_handle[idx];
    memset(rsa_priv, 0, sizeof(zx297100_rsa_priv_t));

    rsa_priv->base = base;

    rsa_priv->padding.padding_type = RSA_PADDING_MODE_PKCS1;
    rsa_priv->padding.hash_type = RSA_HASH_TYPE_SHA1;

    return (rsa_handle_t)rsa_priv;
}

/**
  \brief       De-initialize RSA Interface. stops operation and releases
  \the software resources used by the interface
  \param[in]   handle  rsa handle to operate.
  \return      error code
*/
int32_t csi_rsa_uninitialize(rsa_handle_t handle)
{
    RSA_NULL_PARAM_CHK(handle);

    zx297100_rsa_priv_t *rsa_priv = handle;
    memset(rsa_priv, 0, sizeof(zx297100_rsa_priv_t));

    return 0;
}

/**
  \brief       Get driver capabilities.
  \param[in]   idx device id
  \return      \ref rsa_capabilities_t
*/
rsa_capabilities_t csi_rsa_get_capabilities(int32_t idx)
{
    return driver_capabilities;
}

/**
  \brief       config rsa mode.
  \param[in]   handle  rsa handle to operate.
  \param[in]   data_bits \ref rsa_data_bits_e
  \param[in]   endian    \ref rsa_endian_mode_e
  \return      error code
*/
int32_t csi_rsa_config(rsa_handle_t handle,
                       rsa_data_bits_e data_bits,
                       rsa_endian_mode_e endian,
                       void *arg)
{
    RSA_NULL_PARAM_CHK(handle);

    zx297100_rsa_priv_t *rsa_priv = handle;

    /* config the data bits */
    switch (data_bits) {
        case RSA_DATA_BITS_192:
        case RSA_DATA_BITS_256:
        case RSA_DATA_BITS_512:
            return ERR_RSA(DRV_ERROR_UNSUPPORTED);

        case RSA_DATA_BITS_1024:
        case RSA_DATA_BITS_2048:
            rsa_priv->data_bit = data_bits;
            break;

        default:
            return ERR_RSA(DRV_ERROR_PARAMETER);
    }

    return 0;
}

/**
  \brief       encrypt
  \param[in]   handle  rsa handle to operate.
  \param[in]   n         Pointer to the public modulus
  \param[in]   e         Pointer to the public exponent
  \param[in]   src       Pointer to the source data.
  \param[in]   src_size  the source data len
  \param[out]  out       Pointer to the result buffer
  \param[out]  out_size  the result size
  \param[in]   padding   \ref  rsa_padding_t
  \return      error code
*/
int32_t csi_rsa_encrypt(rsa_handle_t handle, void *n, void *e,
                        void *src, int32_t src_size,
                        void *out, uint32_t *out_size,
                        rsa_padding_t padding)
{
    int ret;

    RSA_NULL_PARAM_CHK(handle);
    RSA_NULL_PARAM_CHK(n);
    RSA_NULL_PARAM_CHK(e);
    RSA_NULL_PARAM_CHK(src);
    RSA_NULL_PARAM_CHK(out);
    RSA_NULL_PARAM_CHK(out_size);
    uint8_t src_padded[KEY_BYTES];
    size_t src_padded_size;

    ret = _rsa_drv_padding_pkcs((uint8_t *)src, src_size,
                                (uint8_t *)src_padded, &src_padded_size,
                                padding, RSA_PUBLIC);

    if (ret) {
        return ret;
    }

    ret = _rsa_drv_process(n, KEY_BYTES, e, KEY_BYTES,
                           src_padded, src_padded_size, out, out_size);

#ifdef CRYPTO_DEBUG
    LOG("\n enc src_size:%d; out_size:%d \n", src_size, *out_size);
    LOG_HEX("enc src_padded:", (uint8_t *)src_padded, src_padded_size);
    LOG_HEX("enc out:", (uint8_t *)out, *out_size);
#endif

    if (ret) {
        return ret;
    }

    return 0;
}

/**
  \brief       decrypt
  \param[in]   handle  rsa handle to operate.
  \param[in]   n         Pointer to the public modulus
  \param[in]   d         Pointer to the privte exponent
  \param[in]   src       Pointer to the source data.
  \param[in]   src_size  the source data len
  \param[out]  out       Pointer to the result buffer
  \param[out]  out_size  the result size
  \param[in]   padding   \ref rsa_padding_t
  \return      error code
*/
int32_t csi_rsa_decrypt(rsa_handle_t handle, void *n, void *d,
                        void *src, uint32_t src_size,
                        void *out, uint32_t *out_size,
                        rsa_padding_t padding)
{
    int ret;
    uint8_t dst_padded[KEY_BYTES];
    size_t   dst_padded_size;

    RSA_NULL_PARAM_CHK(handle);
    RSA_NULL_PARAM_CHK(n);
    RSA_NULL_PARAM_CHK(d);
    RSA_NULL_PARAM_CHK(src);
    RSA_NULL_PARAM_CHK(out);
    RSA_NULL_PARAM_CHK(out_size);

    if (src_size <= 0 ||
        (padding.padding_type != RSA_PADDING_MODE_PKCS1 &&
         padding.padding_type != RSA_PADDING_MODE_NO)) {
        return ERR_RSA(DRV_ERROR_PARAMETER);
    }

    ret = _rsa_drv_process(n, KEY_BYTES, d, KEY_BYTES, src, src_size,
                           (uint8_t *)dst_padded, &dst_padded_size);

    if (ret) {
        return ret;
    }

    ret = _rsa_drv_dispadding_pkcs(dst_padded, dst_padded_size, out, out_size, padding);

#ifdef CRYPTO_DEBUG
    LOG("\n dec src_size:%d; out_size:%d \n", src_size, *out_size);
    LOG_HEX("dec dst_padded:", (uint8_t *)dst_padded, dst_padded_size);
    LOG_HEX("dec out:", (uint8_t *)out, *out_size);
#endif


    if (ret) {
        return ret;
    }

    return 0;
}

/**
  \brief       rsa sign
  \param[in]   handle  rsa handle to operate.
  \param[in]   n         Pointer to the public modulus
  \param[in]   d         Pointer to the privte exponent
  \param[in]   src       Pointer to the source data.
  \param[in]   src_size  the source data len
  \param[out]  signature Pointer to the signature
  \param[out]  sig_size  the signature size
  \param[in]   padding   \ref rsa_padding_t
  \return      error code
*/
int32_t csi_rsa_sign(rsa_handle_t handle, void *n, void *d,
                     void *src, uint32_t src_size,
                     void *signature, void *sig_size,
                     rsa_padding_t padding)
{
    int32_t ret;
    uint8_t src_padded[KEY_BYTES];
    size_t src_padded_size;

    RSA_NULL_PARAM_CHK(handle);
    RSA_NULL_PARAM_CHK(n);
    RSA_NULL_PARAM_CHK(d);
    RSA_NULL_PARAM_CHK(src);
    RSA_NULL_PARAM_CHK(signature);
    RSA_NULL_PARAM_CHK(sig_size);

    if (src_size <= 0 ||
        (padding.hash_type != RSA_HASH_TYPE_MD5
         && padding.hash_type != RSA_HASH_TYPE_SHA1
         && padding.hash_type != RSA_HASH_TYPE_SHA256)) {
        return ERR_RSA(DRV_ERROR_PARAMETER);
    }

    ret = _rsa_drv_es_padding_pkcs(src, src_size, (uint8_t *)src_padded,
                                   &src_padded_size, padding, RSA_PRIVATE);

    if (ret) {
        return ret;
    }

    ret = _rsa_drv_process(n, KEY_BYTES, d, KEY_BYTES,
                           src_padded, src_padded_size, signature, sig_size);

    if (ret) {
        return ret;
    }

    *(uint32_t *)sig_size = src_padded_size;

#ifdef CRYPTO_DEBUG
    LOG("\n sign src_size:%d; sig_size:%d \n", src_size, *(uint32_t *)sig_size);
    LOG_HEX("src:", (uint8_t *)src, src_size);
    LOG_HEX("src_padded:", src_padded, src_padded_size);
    LOG_HEX("signature:", (uint8_t *)signature, *(uint32_t *)sig_size);
#endif

    return 0;
}

/**
  \brief       rsa verify
  \param[in]   handle  rsa handle to operate.
  \param[in]   n         Pointer to the public modulus
  \param[in]   e         Pointer to the public exponent
  \param[in]   src       Pointer to the source data.
  \param[in]   src_size  the source data len
  \param[in]   signature Pointer to the signature
  \param[in]   sig_size  the signature size
  \param[out]  result    Pointer to the result
  \param[in]   padding   \ref rsa_padding_t
  \return      error code
*/
int32_t csi_rsa_verify(rsa_handle_t handle, void *n, void *e,
                       void *src, uint32_t src_size,
                       void *signature, uint32_t sig_size,
                       void *result, rsa_padding_t padding)
{
    int ret;
    uint8_t dst_padded[KEY_BYTES];
    uint8_t dst_disapadded[KEY_BYTES];
    size_t  dst_disapadded_size;
    size_t  dst_padded_size;

    RSA_NULL_PARAM_CHK(handle);
    RSA_NULL_PARAM_CHK(n);
    RSA_NULL_PARAM_CHK(e);
    RSA_NULL_PARAM_CHK(src);
    RSA_NULL_PARAM_CHK(signature);
    RSA_NULL_PARAM_CHK(result);

    if (src_size <= 0 || sig_size <= 0 ||
        (padding.hash_type != RSA_HASH_TYPE_MD5 &&
         padding.hash_type != RSA_HASH_TYPE_SHA1 &&
         padding.hash_type != RSA_HASH_TYPE_SHA256)) {
        return ERR_RSA(DRV_ERROR_PARAMETER);
    }

    ret = _rsa_drv_process(n, KEY_BYTES, e, KEY_BYTES, signature, sig_size,
                           (uint8_t *)dst_padded, &dst_padded_size);

    if (ret) {
        return ret;
    }

    ret = _rsa_drv_es_dispadding_pkcs(dst_padded, dst_padded_size,
                                      (uint8_t *)dst_disapadded, &dst_disapadded_size,
                                      padding, RSA_PUBLIC);

    if (ret) {
        return ret;
    }

#ifdef CRYPTO_DEBUG
    LOG("\n verify src_size:%d; dst_disapadded_size:%d \n", src_size, dst_disapadded_size);
    LOG_HEX("signature:", (uint8_t *)signature, sig_size);
    LOG_HEX("dst_padded:", dst_padded, dst_padded_size);
    LOG_HEX("dst_disapadded:", dst_disapadded, dst_disapadded_size);
#endif

    if ((dst_disapadded_size != src_size) ||
        (0 != memcmp(dst_disapadded, src, src_size))) {
        *(uint8_t *)result = 0;
    } else {
        *(uint8_t *)result = 1;
    }

    return 0;
}

/**
  \brief       Get RSA status.
  \param[in]   handle  rsa handle to operate.
  \return      RSA status \ref rsa_status_t
*/
rsa_status_t csi_rsa_get_status(rsa_handle_t handle)
{
    rsa_status_t status = {0};
    return status;
}

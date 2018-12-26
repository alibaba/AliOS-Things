/**
 * File: zx297100_trng.c
 * Brief: Implementation of Sanechips trng
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
#include "drv_trng.h"
#include "zx29_trng.h"

/*******************************************************************************
 *                              Macro definitions                               *
 *******************************************************************************/
#define ERR_TRNG(errno) (CSI_DRV_ERRNO_TRNG_BASE | errno)

/*******************************************************************************
*                               Type definitions                                *
*******************************************************************************/
typedef struct {
    uint32_t base;
    trng_event_cb_t cb;
    trng_status_t status;
} ck_trng_priv_t;

/*******************************************************************************
*                               Local function declarations                     *
*******************************************************************************/

/*******************************************************************************
*                               Local variable definitions                      *
*******************************************************************************/

/*******************************************************************************
*                               Global variable definitions                     *
*******************************************************************************/
static ck_trng_priv_t trng_handle[CONFIG_TRNG_NUM];

/* Driver Capabilities */
static const trng_capabilities_t driver_capabilities = {
    .lowper_mode = 1 /* low power mode */
};

/*******************************************************************************
*                            function   extern                                  *
*******************************************************************************/
extern int32_t target_get_trng_count(void);
extern int32_t target_get_trng(int32_t idx, uint32_t *base);

/*******************************************************************************
*                               Local function implementations                  *
*******************************************************************************/

static void crypto_trng_mode_set()
{
    *(volatile uint32_t *)(ZX29_SECURE_BASE + 0x4) = (1 << 4);
}

static void crypto_trng_mode_clear()
{
    *(volatile uint32_t *)(ZX29_SECURE_BASE + 0x4) = 0;
}

/**
* Brief      set trng crm
*
* param[in]null
*
* return    null
*/
static void _trng_crm_cfg(void)
{
    /****** open sec clk ********/
    *(volatile uint32_t *)(SEC_MOD_CLKEN0) |= (0x3f << 1);
}

/**
* Brief     init trng config
*
* param[in]null
*
* return    null
*/
static void _trng_cfg(void)
{
    volatile struct trng_reg_t *trng_reg = (struct trng_reg_t *)ZX29_RAND_BASE;

    //set buffer ram size
    trng_reg->strap = Buffer_2Kbit;
    //close trng
    trng_reg->conctrl = 0x10000;
    //set enabled fro num  ----0x20
    trng_reg->fro_enable = 0x00ffffff;
    //ÅäÖÃFROÆµÂÊµ÷Õû,0x24, ÅäÖÃ1¿ÉÊ¹FROµÄÆµÂÊ¿ì´óÔ¼5%
    trng_reg->fro_detune = 0x00ffffff;
    //ÅäÖÃ¾¯¸æÆÁ±Î¼Ä´æÆ÷----0x28,
    trng_reg->alarm_mask = 0;
    //ÅäÖÃ¾¯¸æÍ£Ö¹¼Ä´æÆ÷,0x2c,Ä³Ò»Î»ÓÐÐ§£¬±íÊ¾¶ÔÓ¦µÄFRO¾­Àú²»Ö¹Ò»¸ö¾¯±¨ÖÐ¶Ï
    trng_reg->alarm_stop = 0; //need 0
    /*ÅäÖÃ¾¯±¨¼ÆÊý¼Ä´æÆ÷,
    bit0~7:Alarm_threshold
    bi16~20:Shutdown_threshold,±¨´íµÄ¾§Õñ¸öÊýµÄãÐÖµ ----
    bit23:   Shutdown_fatal
    bit24~29:Shutdown_count  */
    trng_reg->alarmcnt = 0x000200ff; //0x1c
    //ÇåÖÐ¶Ï
    trng_reg->sta_intack = 0x7f;

    trng_reg->config = 0x10001;
    //²âÊÔ¼Ä´æÆ÷ÇåÁã
    trng_reg->test = 0;
}

/**
* Brief     trng init
*
* param[in]null
*
* return    null
*/
static void _trng_init(void)
{
    crypto_trng_mode_set();
    _trng_crm_cfg();
    _trng_cfg();
}

/*******************************************************************************
*                               Gloable function implementations                *
*******************************************************************************/
/**
 * brief       Get driver capabilities.
 * param[in]   idx device id.
 * return      \ref trng_capabilities_t
*/
trng_capabilities_t csi_trng_get_capabilities(int32_t idx)
{
    return driver_capabilities;
}

/**
 *brief      Initialize trng Interface. 1. Initializes the resources needed for
 *              the trng interface 2.registers event callback function
 *
 *param[in]  idx device id
 *param[in]  cb_event  Pointer to \ref trng_event_cb_t
 *return     pointer to trng handle
*/
trng_handle_t csi_trng_initialize(int32_t idx, trng_event_cb_t cb_event)
{
    if (idx < 0 || idx >= CONFIG_TRNG_NUM) {
        return NULL;
    }

    /* obtain the trng information */
    uint32_t base = 0u;
    int32_t real_idx = target_get_trng(idx, &base);

    if (real_idx != idx) {
        return NULL;
    }

    ck_trng_priv_t *trng_priv = &trng_handle[idx];
    trng_priv->base = base;

    /* initialize the trng context */

    trng_priv->cb = cb_event;
    trng_priv->status.busy = 0;
    trng_priv->status.data_valid = 0;

    return (trng_handle_t)trng_priv;
}

/**
 * brief       De-initialize trng Interface. stops operation and releases
 * the software resources used by the interface
 * param[in]   handle  trng handle to operate.
 * return      error code
*/
int32_t csi_trng_uninitialize(trng_handle_t handle)
{
    ck_trng_priv_t *trng_priv = handle;
    trng_priv->cb = NULL;

    return 0;
}
/**
* brief    Get trng with length of num .
* param[in]handle   trng handle to operate.
* param[out]data    pointer where trng is to be stored
* param[in]num      length of trng is to be stored
* return            0-success , negative-error code.
*/
int32_t csi_trng_get_data(trng_handle_t handle, void *data, uint32_t num)
{
    ck_trng_priv_t *trng_priv = handle;
    volatile struct trng_reg_t *trng_reg = (struct trng_reg_t *)ZX29_RAND_BASE;

    uint32_t i;
    uint32_t k;
    uint32_t len = num;
    uint32_t tmp_buf[4];
    uint8_t *buf = data;

    if (buf == NULL) {
        return ERR_TRNG(DRV_ERROR_PARAMETER);
    }

    trng_priv->status.busy = 1U;
    trng_priv->status.data_valid = 0U;

    _trng_init();
    //enbale trng
    trng_reg->conctrl |= ((0x1 << Enable_trng) | (0x1 << Test_mode) | (0x1 << Ready_mask));

    while ((trng_reg->conctrl & 0x501) != 0x501);

    //get ramdom
    for (i = 0; i < len / 16; i++) {
        //'1'=data is available in the TRNG_OUTPUT_0¡­3 registers
        while ((trng_reg->sta_intack & 0x1) != 0x1);

        //¶ÁÈ¡²úÉúµÄËæ»úÊý
        for (k = 0; k < 4; k++) {
            buf[i * 16 + k * 4 + 0] = (trng_reg->in_output[k] & 0x000000ff) >> 0;
            buf[i * 16 + k * 4 + 1] = (trng_reg->in_output[k] & 0x0000ff00) >> 8;
            buf[i * 16 + k * 4 + 2] = (trng_reg->in_output[k] & 0x00ff0000) >> 16;
            buf[i * 16 + k * 4 + 3] = (trng_reg->in_output[k] & 0xff000000) >> 24;
        }

        //clear ready bit
        trng_reg->sta_intack |= 0x1;
    }

    if (len % 16) {
        //'1'=data is available in the TRNG_OUTPUT_0¡­3 registers
        while ((trng_reg->sta_intack & 0x1) != 0x1);

        //¶ÁÈ¡²úÉúµÄËæ»úÊý
        for (i = 0; i < 4; i++) {
            tmp_buf[i] = trng_reg->in_output[i];
        }

        buf += (len - len % 16);
        memcpy(buf, tmp_buf, len % 16);

        //clear ready bit
        trng_reg->sta_intack |= 0x1;
    }

    //disable trng
    trng_reg->conctrl &= ~((0x1 << Enable_trng) | (0x1 << Test_mode) | (0x1 << Ready_mask));
    crypto_trng_mode_clear();

    trng_priv->status.busy = 0U;
    trng_priv->status.data_valid = 1U;

    if (trng_priv->cb) {
        trng_priv->cb(1,TRNG_EVENT_DATA_GENERATE_COMPLETE);
    }

    return 0;
}

/**
* brief       Get trng status.
* param[in]   handle  trng handle to operate.
* return      trng status \ref trng_status_t
*/
trng_status_t csi_trng_get_status(trng_handle_t handle)
{
    ck_trng_priv_t *trng_priv = handle;

    return trng_priv->status;
}

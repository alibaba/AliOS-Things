/*
 * Copyright (C) 2017 C-SKY Microsystems Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/******************************************************************************
 * @file     zx29_wdt.c
 * @brief    CSI Source File for WDT Driver
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/

#include <csi_config.h>
#include <stdio.h>
#include <stdint.h>
#include <drv_wdt.h>
#include <drv_intc.h>
#include <soc.h>
#include <io.h>
#include <zx29_wdt.h>
#include <csi_core.h>


#define ERR_WDT(errno) (CSI_DRV_ERRNO_WDT_BASE | errno)

#define WDT_NULL_PARAM_CHK(para)                  \
    do {                                    \
        if (para == NULL) {                 \
            return ERR_WDT(DRV_ERROR_PARAMETER);   \
        }                                   \
    } while (0)

typedef struct {
    uint32_t          addr;           /* the base-address of watchdog */
    uint32_t          srcclk;         /* the src clk of watchdog */
    uint32_t          workclk;        /* the work clk of watchdog */
    wdt_event_cb_t    handler;        /* Current user interrupt handler */
    uint32_t          timeout;
} zx29_wdt_priv_t;

extern int32_t target_get_wdt_count(void);
extern int32_t target_get_wdt(uint32_t idx, uint32_t *base, uint32_t *irq);


/*******************************************************************************
                         Local function declarations
******************************************************************************/
static void wdt_reset(void);

static void wdt_clken(void);

static void wdt_refresh(uint8_t id);

/*******************************************************************************
                         Local variable definitions
******************************************************************************/
static zx29_wdt_priv_t ck_wdt_info = {
    .addr     = CK_WDT_BASE,    /* the base-address of wdt */
    .srcclk   = SRC_CLK_32K,
    .workclk  = 0,
    .handler  = NULL,
    .timeout  = 0,
};

static zx29_wdt_priv_t *const wdt_instance[] = {
    &ck_wdt_info,
    NULL,
};

static int32_t wdt_get_id(wdt_handle_t handle)
{
    zx29_wdt_priv_t *wdt_priv = (zx29_wdt_priv_t *)handle;
    volatile zx29_wdt_reg_t *addr = (zx29_wdt_reg_t *)wdt_priv->addr;

    if ((uint32_t)addr == CK_WDT_BASE) {
        return 0;
    }

    return -1;
}

/**
* This function is used for wtd reset
* @return void
*/
static void wdt_reset(void)
{
    putreg32(0x0, (uint32_t *)WDT_RESET_REG);
    putreg32(0x3, (uint32_t *)WDT_RESET_REG);
    __write_bits(2, 4, (uint32_t *)WDT_RESET_TYPE, 0x3);
}

/**
* This function is used for enable wtd clk
* @return void
*/
static void wdt_clken(void)
{
    putreg32(0x7, (uint32_t *)WDT_CRM_REG);
}

/**
* This function is used for refresh wtd config
* @param[in] id wtd id
* @return         void
*/
static void wdt_refresh(uint8_t id)
{
    zx29_wdt_priv_t *info = wdt_instance[id];
    volatile zx29_wdt_reg_t *pwdt_reg = (zx29_wdt_reg_t *)info->addr;

    if (pwdt_reg->set_en == 0x0015) {
        pwdt_reg->set_en = 0x1234002A;
    } else {
        pwdt_reg->set_en = 0x12340015;
    }

    while ((pwdt_reg->status & 0x2) == 0x0);
}

/**
* This function is used for set wtd count value
* @param[in] id      wtd id
* @param[in] value wtd count value
* @return the operation status, 0 is OK, others is error
*/
static int wdt_set_value(uint8_t id, uint32_t value)
{
    if (value > 0xffff) {
        return FAILURE;
    }

    zx29_wdt_priv_t *info = wdt_instance[id];
    volatile zx29_wdt_reg_t *pwdt_reg = (zx29_wdt_reg_t *)info->addr;

    pwdt_reg->value_load = (0x1234 << 16) | (value & 0xffff);      /*set count value */

    wdt_refresh(id);

    while ((pwdt_reg->status & 0x2) == 0x0);                   /*waiting for value be loaded*/

    return SUCCESS;
}

/**
* This function is used for set wtd int value
* @param[in] id      wtd id
* @param[in] value wtd int value
* @return the operation status, 0 is OK, others is error
*/
static int wdt_set_int_value(uint8_t id, uint32_t value)
{
    if (value > 0xffff) {
        return FAILURE;
    }

    zx29_wdt_priv_t *info = wdt_instance[id];
    volatile zx29_wdt_reg_t *pwdt_reg = (zx29_wdt_reg_t *)info->addr;

    pwdt_reg->value_int = (0x1234 << 16) | (value & 0xffff);  /*set int count value */

    return SUCCESS;
}

/**
* This function is used for set wtd clk value
* @param[in] id      wtd id
* @param[in] value wtd clk value
* @return the operation status, 0 is OK, others is error
*/
static int wdt_set_clkdiv(uint8_t id, uint32_t value)
{
    if (value > 0xff) {
        return FAILURE;
    }

    zx29_wdt_priv_t *info = wdt_instance[id];
    volatile zx29_wdt_reg_t *wdt_reg = (zx29_wdt_reg_t *)info->addr;

    wdt_reg->config = (0x1234 << 16) | (value << 8);        /*set wdt sysclk divide */

    return SUCCESS;
}

/**
* This function is used for clear wtd int
* @return void
*/
static void wtd_int_clear(void)
{
    __set_bit(0, (uint32_t *)WDT_CRM_INT_CLEAR);
    __clear_bit(0, (uint32_t *)WDT_CRM_INT_CLEAR);
}


void zx29_wdt_irqhandler(int32_t idx)
{
    wtd_int_clear();
    zx29_wdt_priv_t *wdt_priv = wdt_instance[idx];
    csi_wdt_restart(wdt_priv);
}

/**
  \brief       Initialize WDT Interface. 1. Initializes the resources needed for the WDT interface 2.registers event callback function
  \param[in]   idx   wdt index
  \param[in]   cb_event  Pointer to \ref wdt_event_cb_t
  \return      pointer to wdt instance
*/
wdt_handle_t csi_wdt_initialize(int32_t idx, wdt_event_cb_t cb_event)
{
    if (idx < 0 || idx >= CONFIG_WDT_NUM) {
        return NULL;
    }

    uint32_t base = 0u;
    uint32_t irq = 0u;

    int32_t real_idx = target_get_wdt(idx, &base, &irq);

    if (real_idx != idx) {
        return NULL;
    }

    zx29_wdt_priv_t *wdt_priv = wdt_instance[idx];
    wdt_priv->addr = base;
    wdt_priv->handler = cb_event;

    wdt_reset();
    wdt_clken();
    csi_intc_set_attribute(irq, 1 , INT_MODE_HIGH_LEVEL);
    csi_intc_ack_irq(irq);
    csi_intc_enable_irq(irq);
    return (wdt_handle_t)wdt_priv;
}

/**
  \brief       De-initialize WDT Interface. stops operation and releases the software resources used by the interface
  \param[in]   instance  wdt instance to operate.
  \return      \ref execution_status
*/
int32_t csi_wdt_uninitialize(wdt_handle_t handle)
{
    WDT_NULL_PARAM_CHK(handle);

    zx29_wdt_priv_t *wdt_priv = handle;

    uint32_t irq = 0u;
    uint32_t base = 0u;
    int32_t idx = wdt_get_id(handle);
    int32_t real_idx = target_get_wdt(idx, &base, &irq);

    if (real_idx != idx) {
        return NULL;
    }

    wdt_priv->handler = NULL;
    csi_intc_disable_irq(irq);
    return 0;
}

/**
  \brief       Set the WDT value. value = (2^t*0xffff * 10^6 /freq)/10^3(t: 0 ~ 15).
  \param[in]   handle wdt handle to operate.
  \param[in]   value     the timeout value(ms) \ref:timeout_ms[]
  \return      \ref execution_status
*/
int32_t csi_wdt_set_timeout(wdt_handle_t handle, uint32_t value)
{
    WDT_NULL_PARAM_CHK(handle);

    uint32_t idx = wdt_get_id(handle);
    wdt_set_clkdiv(idx, 31);

    int ret = wdt_set_int_value(idx, 0);

    if (ret != SUCCESS) {
        return ERR_WDT(DRV_ERROR_PARAMETER);
    }

    ret = wdt_set_value(idx, value);

    if (ret != SUCCESS) {
        return ERR_WDT(DRV_ERROR_PARAMETER);
    }

    return 0;
}

/**
  \brief       Start the WDT.
  \param[in]   handle wdt handle to operate.
  \return      \ref execution_status
*/
int32_t csi_wdt_start(wdt_handle_t handle)
{
    WDT_NULL_PARAM_CHK(handle);

    uint32_t idx = wdt_get_id(handle);
    zx29_wdt_priv_t *info = wdt_instance[idx];
    volatile zx29_wdt_reg_t *pwdt_reg = (zx29_wdt_reg_t *)info->addr;
    pwdt_reg->start |= 0x12340001;
    return SUCCESS;
}

/**
  \brief       Stop the WDT.
  \param[in]   handle wdt handle to operate.
  \return      \ref execution_status
*/
int32_t csi_wdt_stop(wdt_handle_t handle)
{
    WDT_NULL_PARAM_CHK(handle);

    uint32_t idx = wdt_get_id(handle);
    zx29_wdt_priv_t *info = wdt_instance[idx];
    volatile zx29_wdt_reg_t *pwdt_reg = (zx29_wdt_reg_t *)info->addr;
    pwdt_reg->start = 0x12340000;
    return SUCCESS;
}

/**
  \brief       Restart the WDT.
  \param[in]   handle wdt handle to operate.
  \return      \ref execution_status
*/
int32_t csi_wdt_restart(wdt_handle_t handle)
{
    uint32_t idx = wdt_get_id(handle);

    csi_wdt_stop(handle);
    wdt_refresh(idx);
    csi_wdt_start(handle);

    return SUCCESS;
}

/**
  \brief       Read the WDT Current value.
  \param[in]   handle wdt handle to operate.
  \param[in]   value     Pointer to the Value.
  \return      \ref execution_status
*/
int32_t csi_wdt_read_current_value(wdt_handle_t handle, uint32_t *value)
{
    WDT_NULL_PARAM_CHK(handle);

    zx29_wdt_priv_t *wdt_priv = handle;
    volatile zx29_wdt_reg_t *addr = (zx29_wdt_reg_t *)(wdt_priv->addr);

    *value = addr->count;
    return 0;
}


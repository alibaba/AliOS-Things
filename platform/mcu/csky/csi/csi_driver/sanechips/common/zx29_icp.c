/**
 * File: icp_si.c
 * Brief: bsp APIs of Sanechips icp
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
 *                           Include header files                              *
 ******************************************************************************/
#include <sys/types.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <csi_kernel.h>
#include "drvs_irq.h"
#include "zx29_icp.h"
#include "infra_config.h"
#include "bsp/yunos_bsp_intc.h"
#include "bsp/yunos_bsp_icp.h"

#define FALSE 0
#define TRUE  1

/*******************************************************************************
 *                             Type definitions                                *
 ******************************************************************************/
struct icp_info_s {
    bool        bopened;        /* indicate whether have been opended or not */
    uint32_t    addr;           /* the base-address of icp */
    uint32_t    irq;            /* the interrupt number of icp */
    icp_isr_t   isr;
    icpcb_t     handler;       /* Current user interrupt handler */
};

/*******************************************************************************
 *                        Local function declarations                          *
 ******************************************************************************/
static void icp_mask_int(uint8_t cpu_id, uint32_t chan_id);
static void icp_unmask_int(uint8_t cpu_id, uint32_t chan_id);

static uint32_t icp_get_recevied_int(uint8_t cpu_id);

static void icp_clear_int(uint8_t cpu_id, uint32_t chan_id);

static void icp_m0_isr(void);

void icp_ceva_isr(void);

extern void csi_intc_enable_irq(int32_t IRQn);

/*******************************************************************************
 *                         Local variable definitions                          *
 ******************************************************************************/
static struct icp_info_s s_icp_to_m0_info = {
    .bopened = FALSE,
    .addr    = ZX29_ICP_TO_M0_BASE,  /* the base-address of icp_m0 */
    .irq     = PIC_IRQ_ICP_M02CK,    /* the interrupt number of icp_m0 */
    .isr     = icp_m0_isr,
    .handler = NULL
};

static struct icp_info_s s_icp_to_ceva_info = {
    .bopened = TRUE,
    .addr    = ZX29_ICP_TO_CEVA_BASE,      /* the base-address of icp_ceva */
    .irq     = PIC_IRQ_ICP_DSP2CK,         /* the interrupt number of icp_ceva */
    .isr     = icp_ceva_isr,
    .handler = NULL
};

static struct icp_info_s *const s_icp_infos[DEVICE_COUNT] = {
    NULL,
    &s_icp_to_m0_info,
    &s_icp_to_ceva_info,
};

/*******************************************************************************
 *                      Local function implementations                         *
 ******************************************************************************/
static void icp_mask_int(uint8_t cpu_id, uint32_t chan_id)
{
    struct icp_info_s *info = s_icp_infos[cpu_id];

    volatile struct icp_reg *pReg = (volatile struct icp_reg *)info->addr;

    pReg->mask |= (0x1 << chan_id);
}

static void icp_unmask_int(uint8_t cpu_id, uint32_t chan_id)
{
    struct icp_info_s *info = s_icp_infos[cpu_id];

    volatile struct icp_reg *pReg = (volatile struct icp_reg *)info->addr;

    pReg->mask &= ~(0x1 << chan_id);
}

static uint32_t icp_get_recevied_int(uint8_t cpu_id)
{
    struct icp_info_s *info = s_icp_infos[cpu_id];

    volatile struct icp_reg *pReg = (volatile struct icp_reg *)info->addr;

    return pReg->sta_out;
}

static void icp_clear_int(uint8_t cpu_id, uint32_t chan_id)
{
    struct icp_info_s *info = s_icp_infos[cpu_id];

    volatile struct icp_reg *pReg = (volatile struct icp_reg *)info->addr;

    pReg->clear = chan_id;
}

static void icp_m0_isr(void)
{
    uint32_t icp_int_status = 0;

    struct icp_info_s *info = s_icp_infos[CPU_M0_ID];

    icp_int_status = icp_get_recevied_int(CPU_M0_ID);

    if (info->handler != NULL) {
        info->handler(CPU_M0_ID, icp_int_status);
    }

    icp_clear_int(CPU_M0_ID, icp_int_status);
}

void icp_ceva_isr(void)
{
    csi_kernel_intrpt_enter();
    uint32_t icp_int_status = 0;

    struct icp_info_s *info = s_icp_infos[CPU_CEVA_ID];

    icp_int_status = icp_get_recevied_int(CPU_CEVA_ID);

    if (info->handler != NULL) {
        info->handler(CPU_CEVA_ID, icp_int_status);
    }

    icp_clear_int(CPU_CEVA_ID, icp_int_status);
    csi_kernel_intrpt_exit();
}

/*******************************************************************************
 *                      Global function implementations                        *
 ******************************************************************************/
/**
 * This function is used for mask icp int
 * @param[in]  cpu_id   actor cpu id
 * @param[in]  chan_id  icp channel id, 0-31
 * @return the operation status, 0 is OK, others is error
 */
int yunos_bsp_icp_mask_int(uint8_t cpu_id, uint32_t chan_id)
{
    if ((cpu_id >= DEVICE_COUNT) || (chan_id >= CHANNEL_COUNT)) {
        return -1;
    }

    icp_mask_int(cpu_id, chan_id);

    return 0;
}

/**
 * This function is used for unmask icp int
 * @param[in]  cpu_id   actor cpu id
 * @param[in]  chan_id  icp channel id, 0-31
 * @return the operation status, 0 is OK, others is error
 */
int yunos_bsp_icp_unmask_int(uint8_t cpu_id, uint32_t chan_id)
{
    if ((cpu_id >= DEVICE_COUNT) || (chan_id >= CHANNEL_COUNT)) {
        return -1;
    }

    icp_unmask_int(cpu_id, chan_id);

    return 0;
}

/**
* This function is used for get icp status of other cpu
* @param[in]  cpu_id   actor cpu id
* @return the icp status of the other cpu
*/
int yunos_bsp_icp_get_int(uint8_t cpu_id)
{
    if (cpu_id >= DEVICE_COUNT) {
        return -1;
    }

    struct icp_info_s *info = s_icp_infos[cpu_id];

    volatile struct icp_reg *pReg = (volatile struct icp_reg *)info->addr;

    return pReg->sta_in;
}

/**
 * This function is used for send icp int
 * @param[in]  cpu_id   actor cpu id
 * @param[in]  chan_id  icp channel id, 0-31
 * @return the operation status, 0 is OK, others is error
 */
int yunos_bsp_icp_send_int(uint8_t cpu_id, uint32_t chan_id)
{
    if ((cpu_id >= DEVICE_COUNT) || (chan_id >= CHANNEL_COUNT)) {
        return -1;
    }

    struct icp_info_s *info = s_icp_infos[cpu_id];

    volatile struct icp_reg *pReg = (volatile struct icp_reg *)info->addr;

    if (((yunos_bsp_icp_get_int(cpu_id) >> chan_id) & 0x1) == 0) {
        pReg->config = (1 << chan_id);
    }

    return 0;
}

/**
 * This function is used for register icp callback func
 * @param[in]  cpu_id   actor cpu id
 * @param[in]  handler  icp callback func
 * @return the operation status, 0 is OK, others is error
 */
int yunos_bsp_icp_sethandler(uint8_t cpu_id, icpcb_t handler)
{
    if (cpu_id >= DEVICE_COUNT) {
        return -1;
    }

    struct icp_info_s *info = s_icp_infos[cpu_id];

    info->handler = handler;

    return BSP_OK;
}

/**
 * This function is used for init icp
 * @return the operation status, 0 is OK, others is error
 */
int yunos_bsp_icp_init(void)
{
    uint8_t i = 0;

    printf("\nDriver icp init\n");

    for (i = 0; i < CHANNEL_COUNT; i++) {
        icp_mask_int(CPU_M0_ID, i);
        icp_mask_int(CPU_CEVA_ID, i);
    }

#if 0
    struct icp_info_s *info;
    for (i = 1; i < DEVICE_COUNT; i++) {
        info = s_icp_infos[i];

        if (info->bopened == true) {
            //yunos_bsp_intc_attach_irq(info->irq, (int)info->isr);
            //yunos_bsp_intc_enable_irq(info->irq);
            //csi_intc_enable_irq(info->irq);
        }
    }
#endif

    return BSP_OK;
}


int yunos_bsp_icp_eable_irq(void)
{
    struct icp_info_s *info;
    uint8_t i = 0;

    for (i = 1; i < DEVICE_COUNT; i++) {
        info = s_icp_infos[i];

        if (info->bopened == true) {
            //yunos_bsp_intc_attach_irq(info->irq, (int)info->isr);
            //yunos_bsp_intc_enable_irq(info->irq);
            csi_intc_enable_irq(info->irq);
        }
    }

    return BSP_OK;
}



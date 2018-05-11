/* ---------------------------------------------------------------------------
 * Copyright (C) 2016 CSKY Limited. All rights reserved.
 *
 * Redistribution and use of this software in source and binary forms,
 * with or without modification, are permitted provided that the following
 * conditions are met:
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *   * Neither the name of CSKY Ltd. nor the names of CSKY's contributors may
 *     be used to endorse or promote products derived from this software without
 *     specific prior written permission of CSKY Ltd.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 * OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 * -------------------------------------------------------------------------- */

/******************************************************************************
 * @file    ck_usi.c
 * @brief   CSI Source File for USI Driver
 * @version  V1.0
 * @date    02, June 2017
 ******************************************************************************/
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include "ck_usi.h"
#include "usi_pin_planning.h"

#define ERR_USI(errno) (CSI_DRV_ERRNO_USI_BASE | errno)

extern void ck_usi_usart_irqhandler(int idx);
extern void ck_usi_i2c_irqhandler(int idx);
extern void ck_usi_spi_irqhandler(int idx);
extern int32_t target_usi_init(int32_t idx, uint32_t *base, uint32_t *irq);
//------------------------------------------
//            data private
//------------------------------------------
typedef struct {
    uint32_t base;
    uint32_t irq;
} ck_usi_priv_t;

static ck_usi_priv_t usi_instance[CONFIG_USI_NUM];

void ck_usi_set_rxfifo_th(ck_usi_reg_t *addr, uint32_t length)
{
    addr->USI_INTR_CTRL &= ~USI_INTR_CTRL_TH_MODE;
    addr->USI_INTR_CTRL &= USI_INTR_CTRL_RXFIFO_TH;

    if (length >= USI_RX_MAX_FIFO) {
        addr->USI_INTR_CTRL |= USI_INTR_CTRL_RXFIFO_TH_12 | USI_INTR_CTRL_TH_MODE;
    } else if (length >= USI_RX_MAX_FIFO - 4) {
        addr->USI_INTR_CTRL |= USI_INTR_CTRL_RXFIFO_TH_8 | USI_INTR_CTRL_TH_MODE;
    } else if (length >= 4) {
        addr->USI_INTR_CTRL |= USI_INTR_CTRL_RXFIFO_TH_4 | USI_INTR_CTRL_TH_MODE;
    } else {
        addr->USI_INTR_CTRL |= USI_INTR_CTRL_RXFIFO_TH_4;
    }
}

//------------------------------------------
//            CSI_DRIVER
//------------------------------------------
void ck_usi_irqhandler(int32_t idx)
{
    ck_usi_priv_t *usi_priv = &usi_instance[idx];
    ck_usi_reg_t *addr = (ck_usi_reg_t *)(usi_priv->base);

    switch (addr->USI_MODE_SEL & 0x3) {
        case USI_MODE_UART:
            ck_usi_usart_irqhandler(idx);
            break;

        case USI_MODE_I2C:
            ck_usi_i2c_irqhandler(idx);
            break;

        case USI_MODE_SPI:
            ck_usi_spi_irqhandler(idx);
            break;

        default:
            return;
    }
}

int32_t drv_usi_initialize(int32_t idx)
{
    uint32_t base = 0u;
    uint32_t irq = 0u;

    int32_t ret = target_usi_init(idx, &base, &irq);

    if (ret < 0 || ret >= CONFIG_USI_NUM) {
        return ERR_USI(DRV_ERROR_PARAMETER);
    }

    ck_usi_priv_t *usi_priv = &usi_instance[idx];
    usi_priv->base = base;
    usi_priv->irq  = irq;

    return 0;
}

int32_t drv_usi_uninitialize(int32_t idx)
{
    if (idx <= 0 || idx >= CONFIG_USI_NUM) {
        return ERR_USI(DRV_ERROR_PARAMETER);
    }

    return 0;
}

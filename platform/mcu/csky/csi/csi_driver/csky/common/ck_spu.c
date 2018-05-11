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
 * @file     ck_spu.c
 * @brief    CSI Source File for SPU Driver
 * @version  V1.3
 * @date     02. February 2018
 ******************************************************************************/
#include <csi_config.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "drv_spu.h"
#include "ck_spu.h"
#include "soc.h"

extern int32_t spu_func_index_size(spu_dev_type_e dev_type, int32_t arg);
extern int32_t spu_func_size(spu_dev_type_e dev_type, int32_t arg);
extern int32_t SPU_USART_FUNC_INDEX[];
extern int32_t SPU_USART_FUNC[];
extern int32_t SPU_SPI_FUNC_INDEX_POL0_PHA0[];
extern int32_t SPU_SPI_FUNC_INDEX_POL0_PHA1[];
extern int32_t SPU_SPI_FUNC_INDEX_POL1_PHA0[];
extern int32_t SPU_SPI_FUNC_INDEX_POL1_PHA1[];
extern int32_t SPU_SPI_FUNC[];
extern int32_t SPU_IIC_FUNC_INDEX_MASTER[];
extern int32_t SPU_IIC_FUNC_MASTER[];
extern int32_t SPU_IIC_FUNC_INDEX_SLAVE[];
extern int32_t SPU_IIC_FUNC_SLAVE[];
extern int32_t SPU_IIS_FUNC_INDEX[];
extern int32_t SPU_IIS_FUNC[];

//extern void spu_file_pin(int32_t spu_idx, pin_t pin[]);
extern spu_capabilities_t spu_file_prog_base(int32_t spu_idx, uint32_t *prog_size);
extern spu_pgpio_info_t spu_get_pgpio_info(int32_t spu_idx, uint8_t pgpio);
extern void ck_spu_usart_irqhandler(int32_t dev_idx, spu_dev_signal_e signal);
extern void ck_spu_spi_irqhandler(int32_t dev_idx, spu_dev_signal_e signal);
extern void ck_spu_iic_irqhandler(int32_t dev_idx, spu_dev_signal_e signal);
extern void ck_spu_i2s_irqhandler(int32_t dev_idx, spu_dev_signal_e signal);

int32_t target_spu_init(int32_t idx, uint32_t *base, uint32_t *irq);

//------------------------------------------
//            data private
//------------------------------------------
typedef struct {
    //spu base address & vector number
    uint32_t base;
    uint32_t irq;

    //instruction ram address of function index
    uint32_t usart_func_index_base;
    uint32_t spi_func_index_base_pol0_pha0;
    uint32_t spi_func_index_base_pol0_pha1;
    uint32_t spi_func_index_base_pol1_pha0;
    uint32_t spi_func_index_base_pol1_pha1;
    uint32_t iic_func_index_base_master;
    uint32_t iic_func_index_base_slave;
    uint32_t i2s_func_index_base;

    //spu capabilities
    spu_capabilities_t capabilities;
} ck_spu_priv_t;

static ck_spu_priv_t spu_instance[CONFIG_SPU_NUM];

//------------------------------------------
//                  ISR
//------------------------------------------
void __attribute__((weak)) ck_spu_usart_irqhandler(int32_t dev_idx, spu_dev_signal_e signal) {}
void __attribute__((weak)) ck_spu_spi_irqhandler(int32_t dev_idx, spu_dev_signal_e signal) {}
void __attribute__((weak)) ck_spu_iic_irqhandler(int32_t dev_idx, spu_dev_signal_e signal) {}
void __attribute__((weak)) ck_spu_i2s_irqhandler(int32_t dev_idx, spu_dev_signal_e signal) {}

void ck_spu_irqhandler(int32_t idx)
{
    ck_spu_priv_t *priv = &spu_instance[idx];

    uint32_t global_base;
    uint32_t global_data;
    uint32_t mask;
    uint8_t pgpio_id = 0;

    global_base = priv->base + GLOBAL_BASE;
    getData(global_data, global_base, OFFSET_INT_STATUS);

    spu_pgpio_info_t pgpio_info;

    do {
        mask = global_data & 0x1;

        if (mask != 0) {
            pgpio_info = spu_get_pgpio_info(idx, pgpio_id);

            switch (pgpio_info.dev_type) {
                case ENUM_SPU_USART:
                    ck_spu_usart_irqhandler(pgpio_info.dev_idx, pgpio_info.dev_signal);
                    break;

                case ENUM_SPU_SPI:
                    ck_spu_spi_irqhandler(pgpio_info.dev_idx, pgpio_info.dev_signal);
                    break;

                case ENUM_SPU_IIC:
                    ck_spu_iic_irqhandler(pgpio_info.dev_idx, pgpio_info.dev_signal);
                    break;

                case ENUM_SPU_IIS:
                    ck_spu_i2s_irqhandler(pgpio_info.dev_idx, pgpio_info.dev_signal);
                    break;

                default:
                    break;
            }
        }

        pgpio_id++;
        global_data >>= 1;
    } while (global_data != 0);
}

//------------------------------------------
//            CSI_DRIVER
//------------------------------------------
spu_handle_t drv_spu_initialize(int32_t idx)
{
    //get base address & vector num
    uint32_t base;
    uint32_t irq;
    ck_spu_priv_t *priv;

    int32_t ret = target_spu_init(idx, &base, &irq);

    if (ret < 0 || ret >= CONFIG_SPU_NUM) {
        return NULL;
    }

    priv = &spu_instance[idx];
    priv->base = base;
    priv->irq  = irq;

    //check if PROG_RAM size enough or not
    uint32_t prog_size;

    priv->capabilities = spu_file_prog_base(idx, &prog_size);

    if (prog_size > SPU_PROG_RAM_SIZE) {
        return NULL;
    }

    //check if INST_RAM size enough or not
    uint32_t func_index_size = 128u;
    uint32_t func_size = 0u;
    uint32_t usart_func_index_size = 0u;
    uint32_t usart_func_size = 0u;
    uint32_t spi_func_index_size_pol0_pha0 = 0u;
    uint32_t spi_func_index_size_pol0_pha1 = 0u;
    uint32_t spi_func_index_size_pol1_pha0 = 0u;
    uint32_t spi_func_index_size_pol1_pha1 = 0u;
    uint32_t spi_func_size = 0u;
    uint32_t iic_func_index_size_master = 0u;
    uint32_t iic_func_size_master = 0u;
    uint32_t iic_func_index_size_slave = 0u;
    uint32_t iic_func_size_slave = 0u;
    uint32_t i2s_func_index_size = 0u;
    uint32_t i2s_func_size = 0u;

    if (priv->capabilities.spu_usart) {
        //func_index_size
        priv->usart_func_index_base = func_index_size;
        usart_func_index_size = spu_func_index_size(ENUM_SPU_USART, 0);
        func_index_size += usart_func_index_size;

        //func_size
        usart_func_size = spu_func_size(ENUM_SPU_USART, 0);
        func_size += usart_func_size;
    }

    if (priv->capabilities.spu_spi) {
        //func_index_size: POL0_PHA0
        priv->spi_func_index_base_pol0_pha0 = func_index_size;
        spi_func_index_size_pol0_pha0 = spu_func_index_size(ENUM_SPU_SPI, 0);
        func_index_size += spi_func_index_size_pol0_pha0;

        //func_index_size: POL0_PHA1
        priv->spi_func_index_base_pol0_pha1 = func_index_size;
        spi_func_index_size_pol0_pha1 = spu_func_index_size(ENUM_SPU_SPI, 1);
        func_index_size += spi_func_index_size_pol0_pha1;

        //func_index_size: POL1_PHA0
        priv->spi_func_index_base_pol1_pha0 = func_index_size;
        spi_func_index_size_pol1_pha0 = spu_func_index_size(ENUM_SPU_SPI, 2);
        func_index_size += spi_func_index_size_pol1_pha0;

        //func_index_size: POL1_PHA1
        priv->spi_func_index_base_pol1_pha1 = func_index_size;
        spi_func_index_size_pol1_pha1 = spu_func_index_size(ENUM_SPU_SPI, 3);
        func_index_size += spi_func_index_size_pol1_pha1;

        //func_size
        spi_func_size = spu_func_size(ENUM_SPU_SPI, 0);
        func_size += spi_func_size;
    }

    if (priv->capabilities.spu_iic) {
        //func_index_size: MASTER
        priv->iic_func_index_base_master = func_index_size;
        iic_func_index_size_master = spu_func_index_size(ENUM_SPU_IIC, 0);
        func_index_size += iic_func_index_size_master;

        //func_index_size: SLAVE
        priv->iic_func_index_base_slave = func_index_size;
        iic_func_index_size_slave = spu_func_index_size(ENUM_SPU_IIC, 1);
        func_index_size += iic_func_index_size_slave;

        //func_size: MASTER
        iic_func_size_master = spu_func_size(ENUM_SPU_IIC, 0);
        func_size += iic_func_size_master;

        //func_size: SLAVE
        iic_func_size_slave = spu_func_size(ENUM_SPU_IIC, 1);
        func_size += iic_func_size_slave;
    }

    if (priv->capabilities.spu_i2s) {
        //func_index_size
        priv->i2s_func_index_base = func_index_size;
        i2s_func_index_size = spu_func_index_size(ENUM_SPU_IIS, 0);
        func_index_size += i2s_func_index_size;

        //func_size
        i2s_func_size = spu_func_size(ENUM_SPU_IIS, 0);
        func_size += i2s_func_size;
    }

    func_size += func_index_size;

    if ((func_size) > SPU_INST_RAM_SIZE) {
        return NULL;
    }

    //write INST_RAM
    uint32_t func_base;
    uint32_t glb_func_index_base;
    uint32_t glb_func_base;
    uint32_t func_base_offset;
    uint32_t data;
    int i;

    func_base = func_index_size;
    glb_func_index_base = 128 + (priv->base + INST_BASE);
    glb_func_base = func_base + (priv->base + INST_BASE);

    if (priv->capabilities.spu_usart) {
        func_base_offset = (func_base << 16) | func_base;

        for (i = 0; i < usart_func_index_size; i += 4) {
            data = SPU_USART_FUNC_INDEX[i >> 2] + func_base_offset;
            putData(data, glb_func_index_base, i);
        }

        glb_func_index_base += usart_func_index_size;

        for (i = 0; i < usart_func_size; i += 4) {
            putData(SPU_USART_FUNC[i >> 2], glb_func_base, i);
        }

        func_base += usart_func_size;
        glb_func_base += usart_func_size;
    }

    if (priv->capabilities.spu_spi) {
        func_base_offset = (func_base << 16) | func_base;

        //pol0_pha0
        for (i = 0; i < spi_func_index_size_pol0_pha0; i += 4) {
            data = SPU_SPI_FUNC_INDEX_POL0_PHA0[i >> 2] + func_base_offset;
            putData(data, glb_func_index_base, i);
        }

        glb_func_index_base += spi_func_index_size_pol0_pha0;

        //pol0_pha1
        for (i = 0; i < spi_func_index_size_pol0_pha1; i += 4) {
            data = SPU_SPI_FUNC_INDEX_POL0_PHA1[i >> 2] + func_base_offset;
            putData(data, glb_func_index_base, i);
        }

        glb_func_index_base += spi_func_index_size_pol0_pha1;

        //pol1_pha0
        for (i = 0; i < spi_func_index_size_pol1_pha0; i += 4) {
            data = SPU_SPI_FUNC_INDEX_POL1_PHA0[i >> 2] + func_base_offset;
            putData(data, glb_func_index_base, i);
        }

        glb_func_index_base += spi_func_index_size_pol1_pha0;

        //pol1_pha1
        for (i = 0; i < spi_func_index_size_pol1_pha1; i += 4) {
            data = SPU_SPI_FUNC_INDEX_POL1_PHA1[i >> 2] + func_base_offset;
            putData(data, glb_func_index_base, i);
        }

        glb_func_index_base += spi_func_index_size_pol1_pha1;

        for (i = 0; i < spi_func_size; i += 4) {
            putData(SPU_SPI_FUNC[i >> 2], glb_func_base, i);
        }

        func_base += spi_func_size;
        glb_func_base += spi_func_size;
    }

    if (priv->capabilities.spu_iic) {
        //master
        func_base_offset = (func_base << 16) | func_base;

        for (i = 0; i < iic_func_index_size_master; i += 4) {
            data = SPU_IIC_FUNC_INDEX_MASTER[i >> 2] + func_base_offset;
            putData(data, glb_func_index_base, i);
        }

        glb_func_index_base += iic_func_index_size_master;

        for (i = 0; i < iic_func_size_master; i += 4) {
            putData(SPU_IIC_FUNC_MASTER[i >> 2], glb_func_base, i);
        }

        func_base += iic_func_size_master;
        glb_func_base += iic_func_size_master;

        //slave
        func_base_offset = (func_base << 16) | func_base;

        for (i = 0; i < iic_func_index_size_slave; i += 4) {
            data = SPU_IIC_FUNC_INDEX_SLAVE[i >> 2] + func_base_offset;
            putData(data, glb_func_index_base, i);
        }

        glb_func_index_base += iic_func_index_size_slave;

        for (i = 0; i < iic_func_size_slave; i += 4) {
            putData(SPU_IIC_FUNC_SLAVE[i >> 2], glb_func_base, i);
        }

        func_base += iic_func_size_slave;
        glb_func_base += iic_func_size_slave;
    }

    if (priv->capabilities.spu_i2s) {
        func_base_offset = (func_base << 16) | func_base;

        for (i = 0; i < i2s_func_index_size; i += 4) {
            data = SPU_IIS_FUNC_INDEX[i >> 2] + func_base_offset;
            putData(data, glb_func_index_base, i);
        }

        glb_func_index_base += i2s_func_index_size;

        for (i = 0; i < i2s_func_size; i += 4) {
            putData(SPU_IIS_FUNC[i >> 2], glb_func_base, i);
        }

        func_base += i2s_func_size;
        glb_func_base += i2s_func_size;
    }

    //enable interrupt
    csi_vic_enable_irq(priv->irq);

    ////file spu pin
    //spu_file_pin(idx,pin);

    return priv;
}

int32_t drv_spu_uninitialize(spu_handle_t handle)
{
    ck_spu_priv_t *priv = handle;

    putData(0x0, priv->base + GLOBAL_BASE, OFFSET_INT_STATUS); //clear interrupt status
    csi_vic_disable_irq(priv->irq);  //disable interrupt
    return 0;
}

spu_capabilities_t drv_spu_get_capabilities(spu_handle_t handle)
{
    ck_spu_priv_t *priv = handle;
    return priv->capabilities;
}

//------------------------------------------
//          function for device
//------------------------------------------
int32_t spu_get_base(int32_t spu_idx)
{
    ck_spu_priv_t *priv = &spu_instance[spu_idx];
    return priv->base;
}

int32_t spu_get_usart_func_index_base(int32_t spu_idx)
{
    ck_spu_priv_t *priv = &spu_instance[spu_idx];
    return priv->usart_func_index_base;
}

int32_t spu_get_spi_func_index_base(int32_t spu_idx, int32_t arg)
{
    ck_spu_priv_t *priv = &spu_instance[spu_idx];

    switch (arg) {
        case 0 :
            return priv->spi_func_index_base_pol0_pha0;

        case 1 :
            return priv->spi_func_index_base_pol0_pha1;

        case 2 :
            return priv->spi_func_index_base_pol1_pha0;

        case 3 :
            return priv->spi_func_index_base_pol1_pha1;

        default:
            return -1;
    }
}

int32_t spu_get_iic_func_index_base(int32_t spu_idx, int32_t arg)
{
    ck_spu_priv_t *priv = &spu_instance[spu_idx];

    switch (arg) {
        case 0:
            return priv->iic_func_index_base_master;

        case 1:
            return priv->iic_func_index_base_slave;

        default:
            return -1;
    }
}

int32_t spu_get_i2s_func_index_base(int32_t spu_idx)
{
    ck_spu_priv_t *priv = &spu_instance[spu_idx];
    return priv->i2s_func_index_base;
}


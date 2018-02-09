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
* @file     ck_dmac_v2.c
* @brief    CSI Source File for DMAC Driver
* @version  V1.0
* @date     02. June 2017
******************************************************************************/

#include <csi_config.h>
#include <soc.h>
#include <stdbool.h>
#include <ck_dmac_v2.h>
#include <csi_core.h>
#include <drv_dmac.h>
#include <string.h>

#define ERR_DMA(errno) (CSI_DRV_ERRNO_DMA_BASE | errno)

typedef struct {
    uint32_t base;
    uint32_t irq;
    dma_event_cb_t cb_event;
    uint8_t ch_num;
    dma_channel_req_mode_e ch_mode;
} ck_dma_priv_t;

extern int32_t target_get_dmac(int32_t idx, uint32_t *base, uint32_t *irq);
extern int32_t target_get_dmac_count(void);

static ck_dma_priv_t dma_instance[CONFIG_DMAC_NUM];

static const dma_capabilities_t dma_capabilities = {
    .unalign_addr = 1,          ///< support for unalign address transfer when memory is source
};

static volatile dma_status_e status[CK_DMA_MAXCHANNEL] = {DMA_STATE_FREE, DMA_STATE_FREE};
static volatile uint8_t ch_opened[CK_DMA_MAXCHANNEL] = {0, 0};

static int32_t ck_dma_set_total_size(ck_dma_reg_t *addr, uint32_t len)
{
    addr->CHCTRLA &= 0xff000fff;
    addr->CHCTRLA |= (len - 1) << 12;

    return 0;
}

static int32_t ck_dma_set_channel(ck_dma_reg_t *addr, uint32_t source, uint32_t dest)
{
    addr->SAR = source;
    addr->DAR = dest ;

    return 0;
}

static int32_t ck_dma_set_addrinc(ck_dma_reg_t *addr, enum_addr_state_e src_addrinc, enum_addr_state_e dst_addrinc)
{
    if ((src_addrinc != DMA_ADDR_INCREMENT && src_addrinc != DMA_ADDR_DECREMENT && src_addrinc != DMA_ADDR_NOCHANGE) ||
        (dst_addrinc != DMA_ADDR_INCREMENT && dst_addrinc != DMA_ADDR_DECREMENT && dst_addrinc != DMA_ADDR_NOCHANGE)) {
        return ERR_DMA(DRV_ERROR_PARAMETER);
    }

    uint32_t  temp = addr->CHCTRLA;
    temp &= 0xffffff0f;
    temp |= (src_addrinc << 6);
    temp |= (dst_addrinc << 4);
    addr->CHCTRLA = temp;

    return 0;
}

 int32_t ck_dma_set_transferwidth(ck_dma_reg_t *addr, dma_datawidth_e src_width, dma_datawidth_e dst_width)
{
    if ((src_width != DMA_DATAWIDTH_SIZE8 && src_width != DMA_DATAWIDTH_SIZE16 && src_width != DMA_DATAWIDTH_SIZE32) ||
        (dst_width != DMA_DATAWIDTH_SIZE8 && dst_width != DMA_DATAWIDTH_SIZE16 && dst_width != DMA_DATAWIDTH_SIZE32)) {
        return ERR_DMA(DRV_ERROR_PARAMETER);
    }

    uint32_t temp = addr->CHCTRLA;
    temp &= 0xfffffff0;
    temp |= (src_width - 1) << 2;
    temp |= (dst_width -1);
    addr->CHCTRLA = temp;

    return 0;
}

static int32_t ck_dma_trans_mode_set(ck_dma_reg_t *addr, dma_trig_trans_mode_e mode)
{
     addr->CHCTRLB &= 0xfffffff9;
     addr->CHCTRLB |= (mode << 1);
     return 0;
}

static int32_t ck_dma_set_addr_endian(ck_dma_reg_t *addr, dma_addr_endian_e src_endian, dma_addr_endian_e dst_endian)
{
    addr->CHCTRLB &= 0xffff9fff;
    addr->CHCTRLB |= (src_endian << 13);
    addr->CHCTRLB |= (dst_endian << 14);
    return 0;
}

static int32_t ck_dma_set_singlemode(ck_dma_reg_t *addr, dma_config_t *config)
{
    addr->CHCTRLA &= 0xefffffff;
    addr->CHCTRLA |= (config->single_dir << 26);

    if (config->preemption == 0) {
        if (config->single_dir == SOURCE) {
            addr->CHCTRLA &= 0xfbffffff;
        } else {
            addr->CHCTRLA &= 0xfdffffff;
        }
    } else if (config->preemption == 1) {
        if (config->single_dir == DEST) {
            addr->CHCTRLA |= 0x04000000;
        } else {
            addr->CHCTRLA |= 0x02000000;
        }
    } else {
        return ERR_DMA(DRV_ERROR_PARAMETER);
    }

    return 0;
}

static int32_t ck_dma_set_groupmode(ck_dma_reg_t *addr, dma_config_t *config)
{
    if (config->src_inc == DMA_ADDR_INC || config->src_inc == DMA_ADDR_DEC) {
        addr->CHCTRLA &= 0xdfffffff;
    } else if (config->src_inc == DMA_ADDR_CONSTANT) {
        addr->CHCTRLA |= (1 << 29);
    } else {
        return ERR_DMA(DRV_ERROR_PARAMETER);
    }

    if (config->dst_inc == DMA_ADDR_INC || config->dst_inc == DMA_ADDR_DEC) {
        addr->CHCTRLA &= 0x7fffffff;
    } else if (config->dst_inc == DMA_ADDR_CONSTANT) {
        addr->CHCTRLA |= (1 << 31);
    } else {
        return ERR_DMA(DRV_ERROR_PARAMETER);
    }

    if (config->preemption == 0) {
        addr->CHCTRLA &= 0xfeffffff;
    } else if (config->preemption == 1) {
        //Channel interrupt by high priority channel mode Control
        addr->CHCTRLA |= (1 << 24);
    } else {
        return ERR_DMA(DRV_ERROR_PARAMETER);
    }

    addr->CHCTRLA &= 0xfffff0ff;
    addr->CHCTRLA |= ((config->group_len - 1) << 8);
    return 0;
}

static int32_t ck_dma_set_blockmode(ck_dma_reg_t *addr)
{

    return 0;
}

void ck_dma_irqhandler(int32_t idx)
{
    ck_dma_priv_t *dma_priv = &dma_instance[idx];
    ck_dma_reg_t *addr = (ck_dma_reg_t *)(dma_priv->base);

    /*
     * StatusInt_temp contain the information that which types of interrupr are
     * requested.
     */
    uint32_t count = 0;
    uint32_t temp = 0;
    uint32_t val_dma_chints;
    uint32_t reg;

    for (count = 0; count < dma_priv->ch_num; count++) {
        addr = (ck_dma_reg_t *)(dma_priv->base + count * 0x30);
        reg = dma_priv->base + count * 0x30;
        val_dma_chints = *(volatile uint32_t *)(reg + 0x14);

        if (val_dma_chints != 0) {
             addr->CHINTC = 0xf;
            break;
        }
    }
    temp = val_dma_chints;

    /* dma transfer complete */
    if (temp & CK_DMA_TFR) {
        status[count] = DMA_STATE_DONE;

        if (dma_priv->cb_event) {
            dma_priv->cb_event(count, DMA_EVENT_TRANSFER_DONE);
        }
    }

    /* dam transfer half done*/
    if (temp & CK_DMA_HTFR) {

        if (dma_priv->cb_event) {
            dma_priv->cb_event(count, DMA_EVENT_TRANSFER_HALF_DONE);
        }
    }

    /* transfer complete in a certain dma trigger mode */
    if (temp & CK_DMA_TRGETCMPFR) {

        if (dma_priv->cb_event) {
            dma_priv->cb_event(count, DMA_EVENT_TRANSFER_MODE_DONE);
        }
    }

    /* transfer error */
    if (temp & CK_DMA_ERR) {
        status[count] =  DMA_STATE_ERROR;

        if (dma_priv->cb_event) {
            dma_priv->cb_event(count, DMA_EVENT_TRANSFER_ERROR);
        }
    }

    uint32_t i;
    temp = *(uint32_t *)CK_V2_DMAC_STATUSCHPEND;
    for (i = 0; i < dma_priv->ch_num; i++) {
        if ((1 << i) & temp) {
            if (dma_priv->cb_event) {
                dma_priv->cb_event(i, DMA_EVENT_CAHNNEL_PEND);
            }
            break;
        }
    }

}

/**
  \brief       Initialize DMA Interface. 1. Initializes the resources needed for the DMA interface 2.registers event callback function
  \param[in]   dmac idx
  \return      pointer to dma instances
*/
dmac_handle_t csi_dma_initialize(int32_t idx)
{
    if (idx < 0 || idx >= CONFIG_DMAC_NUM) {
        return NULL;
    }

    uint32_t base = 0u;
    uint32_t irq = 0u;

    int32_t real_idx = target_get_dmac(idx, &base, &irq);

    if (real_idx != idx) {
        return NULL;
    }

    ck_dma_priv_t *dma_priv = &dma_instance[idx];

    dma_priv->base = base;
    dma_priv->irq  = irq;
    dma_priv->ch_num = CK_DMA_MAXCHANNEL;
    csi_vic_enable_irq(dma_priv->irq);
    uint8_t count = 0u;

    for (count = 0; count < dma_priv->ch_num; count++) {
        ck_dma_reg_t *addr = (ck_dma_reg_t *)(dma_priv->base + count * 0x30);
        addr->CHINTM |= ~CK_DMA_MASK;
        addr->CHINTC |= CK_DMA_INTC;
    }

    *(volatile uint32_t *)CK_V2_DMAC_STATUSCHPEND = CK_DMA_ALL_CAHNNELS_PEND_INTR_VALID;
    *(volatile uint32_t *)CK_V2_DMAC_CHSR = CK_DMA_ALL_CAHNNELS_DATA_TRANS_BUSY_VALID;

    return (dmac_handle_t)dma_priv;
}

/**
  \brief       De-initialize DMA Interface. stops operation and releases the software resources used by the interface
  \param[in]   handle damc handle to operate.
  \return      error code
*/
int32_t csi_dma_uninitialize(dmac_handle_t handle)
{
    if (handle == NULL) {
        return ERR_DMA(DRV_ERROR_PARAMETER);
    }

    ck_dma_priv_t *dma_priv = handle;
    ck_dma_reg_t *addr = (ck_dma_reg_t *)(dma_priv->base);

    uint8_t count;

    for (count = 0; count < dma_priv->ch_num; count++) {
        addr = (ck_dma_reg_t *)(dma_priv->base + count * 0x30);
        addr->CHINTM = CK_CHINTM_RESETVALUE;
        addr->CHINTC = CK_CHINTC_RESETVALUE;
    }

    csi_vic_disable_irq(dma_priv->irq);

    return 0;
}

/**
  \brief       Get driver capabilities.
  \param[in]   idx    dmac index.
  \return      \ref dma_capabilities_t
*/
dma_capabilities_t csi_dma_get_capabilities(int32_t idx)
{
    if (idx > (CONFIG_DMAC_NUM - 1)) {
        dma_capabilities_t ret;
        memset(&ret, 0, sizeof(dma_capabilities_t));
        return ret;
    }

    return dma_capabilities;
}

/**
  \brief     get one free dma channel
  \param[in] handle damc handle to operate.
  \param[in] ch channel num. if -1 then allocate a free channal in this dma
  \return    -1 - no channel can be used, other - channel index
 */
int32_t csi_dma_alloc_channel(dmac_handle_t handle, int32_t ch)
{
    ck_dma_priv_t *dma_priv = handle;

    if (handle == NULL || ch > dma_priv->ch_num) {
        return ERR_DMA(DRV_ERROR_PARAMETER);
    }

    uint8_t ch_num = 0;
    ck_dma_reg_t *addr = NULL;

    if (ch == -1) {     // alloc a free channal
        for (ch_num = 0; ch_num < dma_priv->ch_num; ch_num++) {
            addr = (ck_dma_reg_t *)(dma_priv->base + ch_num * 0x30);

            if (ch_opened[ch_num] != 0x1) {
                ch_opened[ch_num] = 1;
                break;
            }
        }

        if (ch_num >= dma_priv->ch_num) {
            return -1;
        }
    } else {    //alloc a fixed channel
        addr = (ck_dma_reg_t *)(dma_priv->base + ch * 0x30);

        if (ch_opened[ch] == 0x1) {
            return ERR_DMA(DRV_ERROR_BUSY);
        }

        ch_opened[ch] = 1;
        ch_num = ch;
    }

    addr->CHINTM |= ~CK_DMA_MASK;
    addr->CHINTC |= CK_DMA_INTC;

    status[ch_num] = DMA_STATE_READY;

    return ch_num;
}

/**
  \brief        release dma channel and related resources
  \param[in]    handle damc handle to operate.
  \param[in]    ch  channel num.
  \return       error code
 */
int32_t csi_dma_release_channel(dmac_handle_t handle, int32_t ch)
{
    ck_dma_priv_t *dma_priv = handle;

    if (handle == NULL || ch >= dma_priv->ch_num || ch < 0) {
        return ERR_DMA(DRV_ERROR_PARAMETER);
    }

    ck_dma_reg_t *addr = (ck_dma_reg_t *)(dma_priv->base + ch * 0x30);
    status[ch] = DMA_STATE_FREE;
    ch_opened[ch] = 0;

    addr->CHINTM = CK_CHINTM_RESETVALUE;
    addr->CHINTC = CK_CHINTC_RESETVALUE;

    return 0;
}

/**
  \brief
  \param[in]    handle damc handle to operate.
  \param[in]    ch          channel num. if -1 then allocate a free channal in this dma
  \param[in]    psrcaddr    dma transfer source address
  \param[in]    pstdaddr    dma transfer dest address
  \param[in]    length      dma transfer length
  \param[in]    config      dma transfer configure
  \param[in]    cb_event    Pointer to \ref dma_event_cb_t
  \return       error code
 */
int32_t csi_dma_config(dmac_handle_t handle, int32_t ch,
                       void *psrcaddr, void *pstdaddr,
                       uint32_t length, dma_config_t *config, dma_event_cb_t cb_event)
{
    ck_dma_priv_t *dma_priv = handle;
    uint32_t src_in_bytes;
    uint32_t dst_in_bytes;
    int32_t ret;

    if (handle == NULL || ch >= dma_priv->ch_num || config == NULL) {
        return ERR_DMA(DRV_ERROR_PARAMETER);
    }

    if (!(config->src_tw) || !(config->dst_tw)) {
        return ERR_DMA(DRV_ERROR_PARAMETER);
    }

    src_in_bytes = config->src_tw ;
    dst_in_bytes = config->dst_tw ;

    if (config->mode == GROUP_TRIGGER || config->mode == BLOCK_TRIGGER) {
        if ((length % src_in_bytes != 0) || (length % dst_in_bytes != 0)) {
            return ERR_DMA(DRV_ERROR_PARAMETER);
        }
    }

    if ((length * config->src_tw) % config->dst_tw != 0) {
        return ERR_DMA(DRV_ERROR_PARAMETER);
    }

    if (ch == -1) { //alloc a free channel
        ch = csi_dma_alloc_channel(handle, -1);

        if (ch < 0) {
            return ERR_DMA(DRV_ERROR_BUSY);
        }
    }

    dma_priv->cb_event = cb_event;
    dma_priv->ch_mode = config->ch_mode;

    ck_dma_reg_t *addr = (ck_dma_reg_t *)(dma_priv->base + ch * 0x30);

    /* Initializes corresponding channel registers */

    ret = ck_dma_set_total_size(addr, length);
    if (ret < 0) {
        return ret;
    }

    ret = ck_dma_set_transferwidth(addr, config->src_tw, config->dst_tw);
    if (ret < 0) {
        return ret;
    }

    ret = ck_dma_set_addrinc(addr , config->src_inc, config->dst_inc);
    if (ret < 0) {
        return ret;
    }

    ret = ck_dma_set_addr_endian(addr, config->src_endian, config->dst_endian);
    if (ret < 0) {
        return ret;
    }

    ret = ck_dma_trans_mode_set(addr, config->mode);
    if (ret < 0) {
        return ret;
    }

    if (config->mode == SINGLE_TRIGGER) {
        ret = ck_dma_set_singlemode(addr, config);
        if (ret < 0) {
            return ret;
        }
    } else if (config->mode == GROUP_TRIGGER) {
        if ((config->group_len != DMA_GROUP_LEN1) && (config->group_len != DMA_GROUP_LEN2) && (config->group_len != DMA_GROUP_LEN3) && (config->group_len != DMA_GROUP_LEN4) && (config->group_len != DMA_GROUP_LEN5) && (config->group_len != DMA_GROUP_LEN6) && (config->group_len != DMA_GROUP_LEN7) && (config->group_len != DMA_GROUP_LEN8)) {
            return ERR_DMA(DRV_ERROR_PARAMETER);;
        }

        if ((config->group_len % src_in_bytes != 0) || (config->group_len % dst_in_bytes != 0)) {
            return ERR_DMA(DRV_ERROR_PARAMETER);
        }

        ret = ck_dma_set_groupmode(addr, config);
        if (ret < 0) {
            return ret;
        }
    } else if (config->mode == BLOCK_TRIGGER) {
        ret =ck_dma_set_blockmode(addr);
        if (ret < 0) {
            return ret;
        }
    } else {
        return ERR_DMA(DRV_ERROR_PARAMETER);
    }

    ret = ck_dma_set_channel(addr, (uint32_t)psrcaddr, (uint32_t)pstdaddr);
    if (ret < 0) {
        return ret;
    }

    status[ch] = DMA_STATE_READY;

    return 0;
}

/**
  \brief       start generate dma signal.
  \param[in]   handle damc handle to operate.
  \param[in]   ch  channel num.
  \return      error code
*/
int32_t csi_dma_start(dmac_handle_t handle, int32_t ch)
{
    ck_dma_priv_t *dma_priv = handle;

    if (handle == NULL || ch >= dma_priv->ch_num || ch < 0) {
        return ERR_DMA(DRV_ERROR_PARAMETER);
    }

    ck_dma_reg_t *addr = (ck_dma_reg_t *)(dma_priv->base + ch * 0x30);

    addr->CHCTRLB |= CK_DMA_INT_EN;

    //global register DMACCFG : the global DMAC enable bit only can be operated in security wolrd.
    //already do in startup.S
    //*(volatile uint32_t *)CK_V2_DMAC_DMACCFG = CK_DMACCFG_EN;

    addr->CHEN |= CK_DMA_CH_EN;
    if (dma_priv->ch_mode == SOFTWARE) {
        addr->CHSREQ = CK_DMA_CH_REQ;
    }

    status[ch] = DMA_STATE_BUSY;
    return 0;
}

/**
  \brief       Stop generate dma signal.
  \param[in]   handle damc handle to operate.
  \param[in]   ch  channel num.
  \return      error code
*/
int32_t csi_dma_stop(dmac_handle_t handle, int32_t ch)
{
    if (handle == NULL) {
        return ERR_DMA(DRV_ERROR_PARAMETER);
    }

    ck_dma_priv_t *dma_priv = handle;

    if (ch >= dma_priv->ch_num || ch < 0) {
        return ERR_DMA(DRV_ERROR_PARAMETER);
    }

    ck_dma_reg_t *addr = (ck_dma_reg_t *)(dma_priv->base + ch * 0x30);
    addr->CHCTRLB &= ~CK_DMA_INT_EN;      // interrupt disable
    addr->CHEN    &= ~CK_DMA_CH_EN;

    if (dma_priv->ch_mode == SOFTWARE) {
        addr->CHSREQ = 0;
    }

    status[ch] = DMA_STATE_DONE;
    return 0;
}

/**
  \brief       Get DMA status.
  \param[in]   handle damc handle to operate.
  \param[in]   ch  channel num.
  \return      DMA status \ref dma_status_t
*/
dma_status_e csi_dma_get_status(dmac_handle_t handle, int32_t ch)
{
    if (handle == NULL) {
        return ERR_DMA(DRV_ERROR_PARAMETER);
    }

    ck_dma_priv_t *dma_priv = handle;

    if (ch >= dma_priv->ch_num || ch < 0) {
        return ERR_DMA(DRV_ERROR_PARAMETER);
    }

    uint32_t tmp = *(uint32_t *)CK_V2_DMAC_CHSR;
    if ((1 << ch) & tmp) {
        if (status[ch] != DMA_STATE_BUSY) {
            status[ch] = DMA_STATE_BUSY;
        }
    }

    return status[ch];
}


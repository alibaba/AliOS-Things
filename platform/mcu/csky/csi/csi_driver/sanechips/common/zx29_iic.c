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
 * @file     zx29_iic.c
 * @brief    CSI Source File for IIC Driver
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/
#include "drv_iic.h"
#include "drv_clk.h"
#include "zx29_iic.h"
#include "soc.h"
#include "csi_core.h"
#include "string.h"

#define ERR_IIC(errno) (CSI_DRV_ERRNO_I2C_BASE | errno)

#define IIC_NULL_PARAM_CHK(para)                  \
    do {                                        \
        if (para == NULL) {                     \
            return ERR_IIC(DRV_ERROR_PARAMETER);   \
        }                                       \
    } while (0)

typedef struct  {
    uint32_t base;
    uint32_t irq;
    iic_event_cb_t cb_event;
    uint32_t rx_total_num;
    uint32_t tx_total_num;
    uint8_t *rx_buf;
    uint8_t *tx_buf;
    volatile uint32_t rx_cnt;
    volatile uint32_t tx_cnt;
    uint32_t status;             ///< status of iic transfer
    clk_name pclk;
    clk_name wclk;
} zx29_iic_priv_t;

extern int32_t target_iic_init(int32_t idx, uint32_t *base, uint32_t *irq);
extern void csi_intc_enable_irq(int32_t IRQn);

static zx29_iic_priv_t iic_instance[CONFIG_IIC_NUM];

static const iic_capabilities_t iic_capabilities = {
    .address_10_bit = 0  /* supports 10-bit addressing */
};

#if 0
static inline void zx29_iic_disable(zx29_iic_reg_t *addr)
{
    /* First clear ACTIVITY, then Disable IIC */
    addr->IC_CLR_ACTIVITY;
    addr->IC_ENABLE = ZX29_IIC_DISABLE;

}

static uint32_t zx29_iic_read_clear_intrbits(zx29_iic_reg_t *addr)
{
    uint32_t  stat = 0;

    stat = addr->IC_INTR_STAT;

    if (stat & ZX29_IIC_INTR_RX_UNDER) {
        addr->IC_CLR_RX_UNDER;
    }

    if (stat & ZX29_IIC_INTR_RX_OVER) {
        addr->IC_CLR_RX_OVER;
    }

    if (stat & ZX29_IIC_INTR_TX_OVER) {
        addr->IC_CLR_TX_OVER;
    }

    if (stat & ZX29_IIC_INTR_RD_REQ) {
        addr->IC_CLR_RD_REQ;
    }

    if (stat & ZX29_IIC_INTR_TX_ABRT) {
        addr->IC_TX_ABRT_SOURCE;
    }

    if (stat & ZX29_IIC_INTR_RX_DONE) {
        addr->IC_CLR_RX_DONE;
    }

    if (stat & ZX29_IIC_INTR_ACTIVITY) {
        addr->IC_CLR_ACTIVITY;
    }

    if (stat & ZX29_IIC_INTR_STOP_DET) {
        addr->IC_CLR_STOP_DET;
    }

    if (stat & ZX29_IIC_INTR_START_DET) {
        addr->IC_CLR_START_DET;
    }

    if (stat & ZX29_IIC_INTR_GEN_CALL) {
        addr->IC_CLR_GEN_CALL;
    }

    return stat;
}

#endif
/**
  \brief       interrupt service function for transmit FIFO empty interrupt.
  \param[in]   iic_priv pointer to iic private.
*/
static void zx29_iic_intr_tx_empty(int32_t idx, zx29_iic_priv_t *iic_priv, uint32_t intr_stat)
{
    zx29_iic_reg_t *addr = (zx29_iic_reg_t *)(iic_priv->base);

    if (intr_stat & I2C_STATUS_I_TXF_EMPTY) {

        uint32_t remain_txfifo = iic_priv->tx_total_num - iic_priv->tx_cnt;
        uint8_t emptyfifo = (remain_txfifo > (I2C_FIFO_DEPTH - (addr->txf_stat >> I2C_RXF_STAT_CNT_SHIFT))) ? I2C_FIFO_DEPTH - (addr->txf_stat >> I2C_RXF_STAT_CNT_SHIFT) : remain_txfifo;
        uint32_t i = 0u;

        for (i = 0; i < emptyfifo; i++) {
            addr->data = *iic_priv->tx_buf++;
        }

        iic_priv->tx_cnt += emptyfifo;

        if (iic_priv->tx_cnt == iic_priv->tx_total_num) {
            addr->cmd &= ~I2C_CMD_IE_TXF_EMPTY;
        }
    }

    if (intr_stat & I2C_STATUS_I_MST_STOP) {
        iic_priv->status  = IIC_STATE_DONE;

        if (iic_priv->tx_cnt != iic_priv->tx_total_num) {
            addr->cmd &= ~I2C_CMD_IE_TXF_EMPTY;
        }

//        zx29_iic_disable(addr);
//            addr->cmd &= ~I2C_CMD_IE_TXF_EMPTY;
        addr->cmd &= ~I2C_CMD_IE;
        addr->tx_ctrl &= ~I2C_TX_CTRL_FIFO_RST;

        if (iic_priv->cb_event) {
            iic_priv->cb_event(idx, I2C_EVENT_TRANSFER_DONE);
        }
    }

}
/**
  \brief       interrupt service function for receive FIFO full interrupt .
  \param[in]   iic_priv pointer to iic private.
*/
static void zx29_iic_intr_rx_full(int32_t idx, zx29_iic_priv_t *iic_priv, uint32_t intr_stat)
{
    zx29_iic_reg_t *addr = (zx29_iic_reg_t *)(iic_priv->base);

//    if (intr_stat & I2C_STATUS_I_RXF_FULL) {
    uint32_t remain_rxfifo = iic_priv->rx_total_num - iic_priv->rx_cnt;
    uint8_t emptyfifo = (remain_rxfifo > (addr->rxf_stat >> I2C_RXF_STAT_CNT_SHIFT)) ? (addr->rxf_stat >> I2C_RXF_STAT_CNT_SHIFT) : remain_rxfifo;
    uint32_t i = 0u;

    for (i = 0; i < emptyfifo; i++) {
        *iic_priv->rx_buf++ = addr->data;
    }

    iic_priv->rx_cnt += emptyfifo;
//    }

    if (intr_stat & I2C_STATUS_I_MST_STOP) {
        if (iic_priv->rx_cnt == iic_priv->rx_total_num) {
            iic_priv->status  = IIC_STATE_DONE;
            addr->cmd &= ~I2C_CMD_IE;

            if (iic_priv->cb_event) {
                iic_priv->cb_event(idx, I2C_EVENT_TRANSFER_DONE);
            }
        }
    }

}
void zx29_iic_irqhandler(int32_t idx)
{
    zx29_iic_priv_t *iic_priv = &iic_instance[idx];
    zx29_iic_reg_t *addr = (zx29_iic_reg_t *)(iic_priv->base);

    uint32_t intr_stat = addr->status;

    if (intr_stat & I2C_STATUS_ERR) {
        iic_priv->status = IIC_STATE_DONE;
        addr->cmd &= ~I2C_CMD_IE;
        addr->rx_ctrl &= ~I2C_RX_CTRL_FIFO_RST;
        addr->tx_ctrl &= ~I2C_TX_CTRL_FIFO_RST;

        if (iic_priv->cb_event) {
            iic_priv->cb_event(idx ,I2C_EVENT_BUS_ERROR);
        }
    }

    switch (iic_priv->status) {
        case IIC_STATE_DATASEND: {
            zx29_iic_intr_tx_empty(idx, iic_priv, intr_stat);
            break;
        }

        case IIC_STATE_WFDATA: {
            zx29_iic_intr_rx_full(idx, iic_priv, intr_stat);
            break;
        }

        /* unexpected state,SW fault */
        default: {
            addr->rx_ctrl &= ~I2C_RX_CTRL_FIFO_RST;
            addr->tx_ctrl &= ~I2C_TX_CTRL_FIFO_RST;
            addr->cmd &= ~I2C_CMD_IE;

            if (iic_priv->cb_event) {
                iic_priv->cb_event(idx, I2C_EVENT_ARBITRATION_LOST);
            }
        }
    }
}

/**
  \brief       Initialize IIC Interface. 1. Initializes the resources needed for the IIC interface 2.registers event callback function
  \param[in]   handle  iic handle to operate.
  \param[in]   cb_event  Pointer to \ref iic_event_cb_t
  \return      error code
*/
iic_handle_t csi_iic_initialize(int32_t idx, iic_event_cb_t cb_event)
{
    uint32_t base = 0u;
    uint32_t irq = 0u;

    int32_t ret = target_iic_init(idx, &base, &irq);

    if (ret < 0 || ret >= CONFIG_IIC_NUM) {
        return NULL;
    }

    zx29_iic_priv_t *iic_priv = &iic_instance[idx];
    iic_priv->base = base;
    iic_priv->irq  = irq;


    iic_priv->cb_event = cb_event;
    iic_priv->rx_total_num = 0;
    iic_priv->tx_total_num = 0;
    iic_priv->rx_buf = NULL;
    iic_priv->tx_buf = NULL;
    iic_priv->rx_cnt = 0;
    iic_priv->tx_cnt = 0;
    iic_priv->status = 0;

    if (idx == 0) {
        iic_priv->pclk = I2C0_PCLK;
        iic_priv->wclk = I2C0_WCLK;
    } else {
        iic_priv->pclk = I2C1_PCLK;
        iic_priv->wclk = I2C1_WCLK;
    }

    csi_clk_reset(iic_priv->pclk, 1);
    csi_clk_reset(iic_priv->wclk, 1);
    csi_clk_set_freq(iic_priv->wclk, I2C_WCLK_SEL_LF);
    csi_clk_sw_gate_ctrl(iic_priv->pclk, 1);
    csi_clk_sw_gate_ctrl(iic_priv->wclk, 1);
    csi_clk_hw_gate_ctrl(iic_priv->pclk, 1);
    csi_clk_hw_gate_ctrl(iic_priv->wclk, 1);


    zx29_iic_reg_t *addr = (zx29_iic_reg_t *)(iic_priv->base);

    addr->cmd = I2C_CMD_MS_SEL;
    addr->clk_div = I2C_CLK_DIV_STANDARD;

//    csi_vic_enable_irq(iic_priv->irq);
    csi_intc_enable_irq(iic_priv->irq);

    return iic_priv;
}

/**
  \brief       De-initialize IIC Interface. stops operation and releases the software resources used by the interface
  \param[in]   handle  iic handle to operate.
  \return      error code
*/
int32_t csi_iic_uninitialize(iic_handle_t handle)
{
    IIC_NULL_PARAM_CHK(handle);

    /* First clear ACTIVITY, then Disable IIC */
    zx29_iic_priv_t *iic_priv = handle;
    zx29_iic_reg_t *addr = (zx29_iic_reg_t *)(iic_priv->base);

    addr->cmd &= ~I2C_CMD_IE;

    iic_priv->cb_event = NULL;
    iic_priv->rx_total_num = 0;
    iic_priv->tx_total_num = 0;
    iic_priv->rx_buf = NULL;
    iic_priv->tx_buf = NULL;
    iic_priv->rx_cnt = 0;
    iic_priv->tx_cnt = 0;
    iic_priv->status = 0;

    csi_clk_reset(iic_priv->pclk, 0);
    csi_clk_reset(iic_priv->wclk, 0);

    csi_vic_disable_irq(iic_priv->irq);

    return 0;
}

/**
  \brief       Get driver capabilities.
  \param[in]   idx iic index.
  \return      \ref iic_capabilities_t
*/
iic_capabilities_t csi_iic_get_capabilities(int32_t idx)
{
    if (idx < 0 || idx >= CONFIG_SPI_NUM) {
        iic_capabilities_t ret;
        memset(&ret, 0, sizeof(iic_capabilities_t));
        return ret;
    }

    return iic_capabilities;
}

/**
  \brief       config iic.
  \param[in]   handle  iic handle to operate.
  \param[in]   mode      \ref iic_mode_e.if negative, then this attribute not changed
  \param[in]   speed     \ref iic_speed_e.if negative, then this attribute not changed
  \param[in]   addr_mode \ref iic_address_mode_e.if negative, then this attribute not changed
  \param[in]   slave_addr slave address.if negative, then this attribute not changed
  \return      error code
*/
int32_t csi_iic_config(iic_handle_t handle,
                       iic_mode_e mode,
                       iic_speed_e speed,
                       iic_address_mode_e addr_mode,
                       int32_t slave_addr)
{
    int32_t ret;

    ret = csi_iic_config_mode(handle, mode);

    if (ret < 0) {
        return ret;
    }

    ret = csi_iic_config_speed(handle, speed);

    if (ret < 0) {
        return ret;
    }

    ret = csi_iic_config_addr_mode(handle, addr_mode);

    if (ret < 0) {
        return ret;
    }

    ret = csi_iic_config_slave_addr(handle, slave_addr);

    if (ret < 0) {
        return ret;
    }

    return 0;
}


/**
  \brief       config iic mode.
  \param[in]   handle  iic handle to operate.
  \param[in]   mode      \ref iic_mode_e.if negative, then this attribute not changed
  \return      error code
*/
int32_t csi_iic_config_mode(iic_handle_t handle, iic_mode_e mode)
{
    IIC_NULL_PARAM_CHK(handle);

    zx29_iic_priv_t *iic_priv = handle;
    zx29_iic_reg_t *addr = (zx29_iic_reg_t *)(iic_priv->base);

    switch (mode) {
        case IIC_MODE_MASTER:
            addr->cmd |= I2C_CMD_MS_SEL;
            break;

        case IIC_MODE_SLAVE:
            return ERR_IIC(DRV_ERROR_UNSUPPORTED);
            break;

        default:
            return ERR_IIC(DRV_ERROR_PARAMETER);
    }

    return 0;
}

/**
  \brief       config iic speed.
  \param[in]   handle  iic handle to operate.
  \param[in]   speed     \ref iic_speed_e.if negative, then this attribute not changed
  \return      error code
*/
int32_t csi_iic_config_speed(iic_handle_t handle, iic_speed_e speed)
{
    IIC_NULL_PARAM_CHK(handle);

    zx29_iic_reg_t *addr = (zx29_iic_reg_t *)(((zx29_iic_priv_t *)handle)->base);

    switch (speed) {
        case I2C_BUS_SPEED_STANDARD:
            addr->clk_div = I2C_CLK_DIV_STANDARD;
            break;

        case I2C_BUS_SPEED_FAST:
            addr->clk_div = I2C_CLK_DIV_FAST;
            break;

        case I2C_BUS_SPEED_FAST_PLUS:
        case I2C_BUS_SPEED_HIGH:
            return ERR_IIC(DRV_ERROR_UNSUPPORTED);

        default:
            return ERR_IIC(DRV_ERROR_PARAMETER);
    }

    return 0;
}

/**
  \brief       config iic address mode.
  \param[in]   handle  iic handle to operate.
  \param[in]   addr_mode \ref iic_address_mode_e.if negative, then this attribute not changed
  \return      error code
*/
int32_t csi_iic_config_addr_mode(iic_handle_t handle, iic_address_mode_e addr_mode)
{
    IIC_NULL_PARAM_CHK(handle);

    zx29_iic_reg_t *addr = (zx29_iic_reg_t *)(((zx29_iic_priv_t *)handle)->base);

    switch (addr_mode) {
        case I2C_ADDRESS_10BIT:
            addr->cmd |= I2C_CMD_ADDR_MODE;
            break;

        case I2C_ADDRESS_7BIT:
            addr->cmd &= ~I2C_CMD_ADDR_MODE;
            break;

        default:
            return ERR_IIC(DRV_ERROR_PARAMETER);
    }

    return 0;
}

/**
  \brief       config iic slave address.
  \param[in]   handle  iic handle to operate.
  \param[in]   slave_addr slave address.if negative, then this attribute not changed
  \return      error code
*/
int32_t csi_iic_config_slave_addr(iic_handle_t handle, int32_t slave_addr)
{
    IIC_NULL_PARAM_CHK(handle);

    if (slave_addr < 0) {
        return ERR_IIC(DRV_ERROR_PARAMETER);
    }

    zx29_iic_reg_t *addr = (zx29_iic_reg_t *)(((zx29_iic_priv_t *)handle)->base);

    addr->device = slave_addr;
    return 0;
}

/**
  \brief       Start transmitting data as I2C Master.
  \param[in]   handle       iic handle to operate.
  \param[in]   data           Pointer to buffer with data to transmit to I2C Slave
  \param[in]   num            Number of data items to send
  \param[in]   xfer_pending   Transfer operation is pending - Stop condition will not be generated
  \return      error code
*/
int32_t csi_iic_master_send(iic_handle_t handle, uint32_t devaddr, const void *data, uint32_t num, bool xfer_pending)
{
    IIC_NULL_PARAM_CHK(handle);

    if (data == NULL || num == 0) {
        return ERR_IIC(DRV_ERROR_PARAMETER);
    }

    zx29_iic_priv_t *iic_priv = handle;
    zx29_iic_reg_t *addr = (zx29_iic_reg_t *)(iic_priv->base);

    iic_priv->tx_buf          = (uint8_t *)data;
    iic_priv->tx_total_num    = num;
    iic_priv->tx_cnt          = 0;
    iic_priv->status          = IIC_STATE_DATASEND;

    addr->cmd &= ~I2C_CMD_I2C_RW;
    addr->tx_ctrl = num | I2C_TX_CTRL_FIFO_RST;
    addr->status = I2C_STATUS_CLEAR;

    addr->cmd |= I2C_CMD_IE;
    addr->cmd |= I2C_CMD_START;

#if 0

    while (num > 0) {
        for (; num > 0; num--) {
            if (->txf_stat & I2C_TXF_STAT_FULL) {
                break;
            }

            iic_priv->data = *data;
            data++;
        }

        if (!started) {
            iic_priv->cmd |= I2C_CMD_START;
            started = true;
        }

        ret = i2c_si_wait_trans_over(inst);

        if (ret != 0) {
            return ret;
        }

        iic_priv->status = I2C_STATUS_I_TXF_EMPTY;
    }

#endif
    return 0;

}

/**
  \fn          int32_t csi_iic_master_receive (iic_handle_t handle,const void *data, uint32_t num, bool xfer_pending)
  \brief       Start receiving data as I2C Master.
  \param[in]   handle  iic handle to operate.
  \param[out]  data  Pointer to buffer for data to receive from IIC receiver
  \param[in]   num   Number of data items to receive
  \param[in]   xfer_pending   Transfer operation is pending - Stop condition will not be generated
  \return      error code
*/
int32_t csi_iic_master_receive(iic_handle_t handle, uint32_t devaddr, void *data, uint32_t num, bool xfer_pending)
{
    IIC_NULL_PARAM_CHK(handle);

    if (data == NULL || num == 0) {
        return ERR_IIC(DRV_ERROR_PARAMETER);
    }

    zx29_iic_priv_t *iic_priv = handle;

    iic_priv->rx_buf          = (uint8_t *)data;
    iic_priv->rx_total_num      = num;
    iic_priv->rx_cnt          = 0;
    iic_priv->status          = IIC_STATE_WFDATA;

    zx29_iic_reg_t *addr = (zx29_iic_reg_t *)(iic_priv->base);

//    addr->cmd &= ~I2C_CMD_MS_SEL;
    addr->cmd |= I2C_CMD_I2C_RW;
    addr->rx_ctrl = num | I2C_RX_CTRL_FIFO_RST;
    addr->status = I2C_STATUS_CLEAR;

//    addr->cmd |= I2C_CMD_IE;
    addr->cmd |= I2C_CMD_IE_RXF_FULL;
    addr->cmd |= I2C_CMD_IE_MST_STOP;
    addr->cmd |= I2C_CMD_START;

#if 0

    while (num > 0) {
        ret = i2c_si_wait_trans_over(iic_priv);

        if (ret != 0) {
            return ret;
        }

        for (; num > 0; num--) {
            if (iic_priv->rxf_stat & I2C_RXF_STAT_EMPTY) {
                break;
            }

            *data = iic_priv->data;
            data++;

            if (iic_priv->status & I2C_STATUS_I_RXF_FULL) {
                iic_priv->status = I2C_STATUS_I_RXF_FULL;
            }
        }
    }

#endif

    return 0;

}

/**
  \brief       Start transmitting data as I2C Slave.
  \param[in]   handle  iic handle to operate.
  \param[in]   data  Pointer to buffer with data to transmit to I2C Master
  \param[in]   num   Number of data items to send
  \return      error code
*/
int32_t csi_iic_slave_send(iic_handle_t handle, const void *data, uint32_t num)
{
    return ERR_IIC(DRV_ERROR_UNSUPPORTED);
}

/**
  \fn          int32_t  csi_iic_slave_receive (iic_handle_t handle, const void *data, uint32_t num)
  \brief       Start receiving data as I2C Slave.
  \param[in]   handle  iic handle to operate.
  \param[out]  data  Pointer to buffer for data to receive from I2C Master
  \param[in]   num   Number of data items to receive
  \return      error code
*/
int32_t csi_iic_slave_receive(iic_handle_t handle, const void *data, uint32_t num)
{
    return ERR_IIC(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       abort transfer.
  \param[in]   handle  iic handle to operate.
  \return      error code
*/
int32_t csi_iic_abort_transfer(iic_handle_t handle)
{
    IIC_NULL_PARAM_CHK(handle);

    zx29_iic_priv_t *iic_priv = handle;
//    zx29_iic_reg_t *addr = (zx29_iic_reg_t *)(iic_priv->base);

//    zx29_iic_disable(addr);

//    if (iic_priv->status == IIC_STATE_DATASEND) {
//        addr->IC_FIFO_RST_EN = ZX29_IIC_FIFO_RST_EN;
//    }
    zx29_iic_reg_t *addr = (zx29_iic_reg_t *)(iic_priv->base);
    addr->cmd &= ~I2C_CMD_IE;

    iic_priv->rx_cnt          = 0;
    iic_priv->tx_cnt          = 0;
    iic_priv->rx_buf          = NULL;
    iic_priv->tx_buf          = NULL;
    return 0;
}


/**
  \brief       Get IIC status.
  \param[in]   handle  iic handle to operate.
  \return      IIC status \ref iic_status_t
*/
iic_status_t csi_iic_get_status(iic_handle_t handle)
{
    iic_status_t iic_status = {0};

    if (handle == NULL) {
        return iic_status;
    }

    return iic_status;
#if 0
    zx29_iic_priv_t *iic_priv = handle;
//    zx29_iic_reg_t *addr = (zx29_iic_reg_t *)(iic_priv->base);

    int32_t tmp = addr->IC_STATUS;

    if (tmp & 0x1) {
        iic_status.busy = 1;

        if (tmp & 0x40) {
            iic_status.mode = 0;
        } else {
            iic_status.mode = 1;
        }
    }

    if (iic_priv->status == IIC_STATE_WFDATA) {
        iic_status.direction = 1;
    }

    if (addr->IC_RAW_INTR_STAT & 0x800) {
        iic_status.general_call = 1;
    }

    if (iic_priv->status == IIC_STATE_ERROR) {
        iic_status.bus_error = 1;
    }

    return iic_status;
#endif
}

/**
  \brief       Get IIC transferred data count.
  \param[in]   handle  iic handle to operate.
  \return      number of data bytes transferred
*/
uint32_t csi_iic_get_data_count(iic_handle_t handle)
{
#if 0
    uint32_t cnt = 0;

    if (handle == NULL) {
        return 0;
    }

    zx29_iic_priv_t *iic_priv = handle;

    if (iic_priv->status == IIC_STATE_WFDATA) {
        cnt = iic_priv->rx_cnt;
    } else if (iic_priv->status == IIC_STATE_DATASEND) {
        cnt = iic_priv->tx_cnt;
    }

    return cnt;
#endif
    return 0;
}

/**
  \brief       control IIC power.
  \param[in]   handle  iic handle to operate.
  \param[in]   state   power state.\ref csi_power_stat_e.
  \return      error code
*/
int32_t csi_iic_power_control(iic_handle_t handle, csi_power_stat_e state)
{
    return ERR_IIC(DRV_ERROR_UNSUPPORTED);
}
/**
  \brief       Send START command.
  \param[in]   handle  iic handle to operate.
  \return      error code
*/
int32_t csi_iic_send_start(iic_handle_t handle)
{
    return ERR_IIC(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       Send STOP command.
  \param[in]   handle  iic handle to operate.
  \return      error code
*/
int32_t csi_iic_send_stop(iic_handle_t handle)
{
    return ERR_IIC(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       Reset I2C peripheral.
  \param[in]   handle  iic handle to operate.
  \return      error code
  \note The action here. Most of the implementation sends stop.
*/
int32_t csi_iic_reset(iic_handle_t handle)
{
    return csi_iic_send_stop(handle);
}

/**
  \brief       Read a single byte from the I2C bus.
  \param[in]   handle  iic handle to operate.
  \param[in]   last  Acknoledge,indicates if the byte is to be acknowledged (1 = acknowledge)
  \return      error code if negative, else the data is the lowest byte of return value
*/
int32_t csi_iic_read_byte(iic_handle_t handle, int32_t last)
{
    return ERR_IIC(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       Write one byte.
  \param[in]   handle  iic handle to operate.
  \return      0 if NAK was received, 1 if ACK was received, 2 for timeout. negative for error
*/
int32_t csi_iic_write_byte(iic_handle_t handle, uint8_t data)
{
    return ERR_IIC(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       Check to see if the I2C slave has been addressed.
  \param[in]   handle  iic handle to operate.
  \return      1 - read addressed, 2 - write to all slaves,
               3 - write addressed, 0 - the slave has not been addressed.
               negative for error
*/
int32_t csi_iic_slave_check_addressed(iic_handle_t handle)
{
    return ERR_IIC(DRV_ERROR_UNSUPPORTED);
}

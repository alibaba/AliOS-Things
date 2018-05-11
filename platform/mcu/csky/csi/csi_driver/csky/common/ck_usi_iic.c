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
 * @file     ck_usi_iic.c
 * @brief    CSI Source File for USI IIC Driver
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/
#include <csi_config.h>
#include "string.h"
#include "drv_usi_iic.h"
#include "ck_usi_iic.h"
#include "ck_usi.h"
#include "soc.h"

#define ERR_IIC(errno) (CSI_DRV_ERRNO_IIC_BASE | errno)

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
    volatile uint32_t rx_clk;
    volatile uint32_t tx_cnt;
    uint32_t status;             ///< status of iic transfer
    int32_t idx;
} ck_usi_iic_priv_t;

extern int32_t target_usi_iic_init(int32_t idx, uint32_t *base, uint32_t *irq);

static ck_usi_iic_priv_t iic_instance[CONFIG_USI_NUM];

static const iic_capabilities_t iic_capabilities = {
    .address_10_bit = 0,  /* supports 10-bit addressing */
};

static inline void ck_iic_set_transfer_speed(ck_usi_reg_t *addr, CKENUM_IIC_SPEED speed)
{
    if (speed == CK_IIC_FASTSPEED) {
        addr->USI_I2CM_CTRL &= ~CK_USI_I2CM_CTRL_HS_MODE;
        /* Fast Speed 400kHz set duty 50% */
        addr->USI_CLK_DIV0  = (LSP_DEFAULT_FREQ / 400000 - 2) / 2;
        addr->USI_CLK_DIV1  = addr->USI_CLK_DIV0;
    } else {
        addr->USI_I2CM_CTRL |= CK_USI_I2CM_CTRL_HS_MODE;
    }
}

static inline void ck_iic_set_target_address(ck_usi_reg_t *addr, uint16_t address)
{
    uint16_t temp = addr->USI_I2C_ADDR;
    temp &= 0xfc00;
    temp |= address;
    addr->USI_I2C_ADDR = temp;
}
static inline void ck_iic_set_addr_mode(ck_usi_reg_t *addr, iic_address_mode_e addr_mode)
{
    if (addr_mode == IIC_ADDRESS_7BIT) {
        addr->USI_I2CM_CTRL &= CK_USI_I2CM_CTRL_ADDR_MODE;
        addr->USI_I2CM_CTRL |= CK_USI_I2CM_CTRL_ADDR7_MODE;
    } else {
        addr->USI_I2CM_CTRL &= CK_USI_I2CM_CTRL_ADDR_MODE;
        addr->USI_I2CM_CTRL |= CK_USI_I2CM_CTRL_ADDR10_MODE;
    }

    addr->USI_I2CM_CTRL |= (1 << 1);
}

/**
  \brief       interrupt service function for transmit FIFO empty interrupt.
  \param[in]   iic_priv pointer to iic private.
*/
static void ck_iic_intr_tx_empty(int32_t idx, ck_usi_iic_priv_t *iic_priv, uint32_t intr_stat)
{
    ck_usi_reg_t *addr = (ck_usi_reg_t *)(iic_priv->base);
    uint8_t emptyfifo = 0;

    if (intr_stat & USI_INT_TX_EMPTY) {
        uint32_t remain_txfifo = iic_priv->tx_total_num - iic_priv->tx_cnt;
        emptyfifo = (remain_txfifo > (USI_TX_MAX_FIFO - USI_FIFO_STA_TX_NUM(addr))) ? USI_TX_MAX_FIFO - USI_FIFO_STA_TX_NUM(addr) : remain_txfifo;
        uint32_t i = 0u;
        uint16_t tx_data;

        for (i = 0; i < emptyfifo; i++) {
            tx_data = (uint16_t)(*iic_priv->tx_buf);
            addr->USI_TX_RX_FIFO = tx_data;
            iic_priv->tx_buf++;
        }
    }

    if (iic_priv->tx_cnt == iic_priv->tx_total_num) {
        addr->USI_I2CM_CTRL |= (1 << 1);
        addr->USI_INTR_EN &= ~USI_INT_TX_EMPTY;
    }

    if (intr_stat & USI_INT_I2C_STOP) {
        iic_priv->status  = IIC_STATE_SEND_DONE;
        addr->USI_EN = 0;

        if (iic_priv->cb_event) {
            iic_priv->cb_event(idx, IIC_EVENT_TRANSFER_DONE);
        }
    }

    iic_priv->tx_cnt += emptyfifo;

    if (intr_stat & USI_INT_TX_WERR) {
        iic_priv->status = IIC_STATE_ERROR;
        addr->USI_EN = 0;

        if (iic_priv->cb_event) {
            iic_priv->cb_event(idx, IIC_EVENT_BUS_ERROR);
        }
    }


}
/**
  \brief       interrupt service function for receive FIFO full interrupt .
  \param[in]   iic_priv pointer to iic private.
*/
static void ck_iic_intr_rx_full(int32_t idx, ck_usi_iic_priv_t *iic_priv, uint32_t intr_stat)
{
    ck_usi_reg_t *addr = (ck_usi_reg_t *)(iic_priv->base);

    if (intr_stat & USI_INT_RX_THOLD) {
        addr->USI_INTR_CLR = USI_INT_RX_THOLD;
        uint32_t rx_num = USI_FIFO_STA_RX_NUM(addr);
        uint8_t i;

        for (i = 0; i < rx_num; i++) {
            *iic_priv->rx_buf = addr->USI_TX_RX_FIFO;
            iic_priv->rx_buf++;
            iic_priv->rx_cnt++;;
        }

        uint32_t tx_num = USI_FIFO_STA_TX_NUM(addr);
        uint8_t rxfifo = iic_priv->rx_clk > (USI_RX_MAX_FIFO - tx_num) ? (USI_RX_MAX_FIFO - tx_num) : 1;

        if (iic_priv->rx_clk == 0) {
            rxfifo = 0;
        }

        for (i = 0; i < rxfifo; i++) {
            addr->USI_TX_RX_FIFO = 0x100;
            iic_priv->rx_clk--;
        }

        ck_usi_set_rxfifo_th(addr, rxfifo);

        if (rxfifo == 0) {
            addr->USI_INTR_EN |= USI_INT_I2C_STOP;
        }
    }

    if (iic_priv->rx_cnt == iic_priv->rx_total_num) {
        addr->USI_I2CM_CTRL |= (1 << 1);
        addr->USI_INTR_EN &= ~USI_INT_RX_THOLD;
    }

    if ((intr_stat & USI_INT_RX_WERR) || (intr_stat & USI_INT_RX_RERR)) {
        iic_priv->status = IIC_STATE_ERROR;
        addr->USI_EN = 0;

        if (iic_priv->cb_event) {
            iic_priv->cb_event(idx, IIC_EVENT_BUS_ERROR);
        }
    }

    if (intr_stat & USI_INT_I2C_STOP) {
        addr->USI_INTR_CLR = USI_INT_I2C_STOP;
        iic_priv->status  = IIC_STATE_RECV_DONE;
        addr->USI_EN = 0;

        if (iic_priv->cb_event) {
            iic_priv->cb_event(idx, IIC_EVENT_TRANSFER_DONE);
        }
    }

}
void ck_usi_i2c_irqhandler(int32_t idx)
{
    ck_usi_iic_priv_t *iic_priv = &iic_instance[idx];
    ck_usi_reg_t *addr = (ck_usi_reg_t *)(iic_priv->base);

    uint32_t intr_state = addr->USI_INTR_STA & 0x3ffff;

    if ((intr_state & USI_INT_I2CM_LOSE_ARBI) || (intr_state & USI_INT_I2C_NACK)) {
        /* If arbitration fault, it indicates either a slave device not
        * responding as expected, or other master which is not supported
        * by this SW.
        */
        iic_priv->status    = IIC_STATE_DONE;
        addr->USI_EN = 0;
        addr->USI_INTR_UNMASK = 0;
        addr->USI_INTR_EN = 0;
        addr->USI_INTR_CLR = intr_state;

        if (iic_priv->cb_event) {
            iic_priv->cb_event(idx, IIC_EVENT_BUS_ERROR);
            return;
        }
    }

    switch (iic_priv->status) {
            /* send data to slave */
        case IIC_STATE_DATASEND: {
            ck_iic_intr_tx_empty(idx, iic_priv, intr_state);
            addr->USI_INTR_CLR = intr_state;
            break;
        }

        /* wait for data from slave */
        case IIC_STATE_WFDATA: {
            ck_iic_intr_rx_full(idx, iic_priv, intr_state);
            break;
        }

        /* unexpected state,SW fault */
        default: {
            addr->USI_INTR_CLR = intr_state;
            addr->USI_INTR_UNMASK = 0;
            addr->USI_INTR_EN = 0;
            addr->USI_EN = 0;

            if (iic_priv->cb_event) {
                iic_priv->cb_event(idx, IIC_EVENT_ARBITRATION_LOST);
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
iic_handle_t drv_usi_iic_initialize(int32_t idx, iic_event_cb_t cb_event)
{
    uint32_t base = 0u;
    uint32_t irq = 0u;

    int32_t ret = target_usi_iic_init(idx, &base, &irq);

    if (ret  < 0 || ret >= CONFIG_USI_NUM) {
        return NULL;
    }

    ret = drv_usi_initialize(idx);

    if (ret < 0) {
        return NULL;
    }

    ck_usi_iic_priv_t *iic_priv = &iic_instance[idx];
    iic_priv->base = base;
    iic_priv->irq  = irq;
    iic_priv->idx  = idx;

    iic_priv->cb_event = cb_event;
    iic_priv->rx_total_num = 0;
    iic_priv->tx_total_num = 0;
    iic_priv->rx_buf = NULL;
    iic_priv->tx_buf = NULL;
    iic_priv->rx_cnt = 0;
    iic_priv->tx_cnt = 0;
    iic_priv->status = 0;

    ck_usi_reg_t *addr = (ck_usi_reg_t *)(iic_priv->base);

    /* mask all interrupts */
    addr->USI_MODE_SEL = USI_MODE_I2C; /* select i2c mode */
    csi_vic_enable_irq(iic_priv->irq);

    return iic_priv;
}

/**
  \brief       De-initialize IIC Interface. stops operation and releases the software resources used by the interface
  \param[in]   handle  iic handle to operate.
  \return      0 for success, negative for error code
*/
int32_t drv_usi_iic_uninitialize(iic_handle_t handle)
{
    IIC_NULL_PARAM_CHK(handle);

    /* First clear ACTIVITY, then Disable IIC */
    ck_usi_iic_priv_t *iic_priv = handle;
    ck_usi_reg_t *addr = (ck_usi_reg_t *)(iic_priv->base);

    addr->USI_INTR_EN = 0;      /* disable all interrupts */
    addr->USI_EN = 0x0;         /* usi module disbale */

    iic_priv->cb_event = NULL;
    iic_priv->rx_total_num = 0;
    iic_priv->tx_total_num = 0;
    iic_priv->rx_buf = NULL;
    iic_priv->tx_buf = NULL;
    iic_priv->rx_cnt = 0;
    iic_priv->tx_cnt = 0;
    iic_priv->status = 0;

    csi_vic_disable_irq(iic_priv->irq);

    int32_t  ret = drv_usi_uninitialize(iic_priv->idx);

    if (ret < 0) {
        return ERR_IIC(DRV_ERROR_PARAMETER);
    }

    return 0;
}

/**
  \brief       Get driver capabilities.
  \param[in]   idx iic index.
  \return      \ref iic_capabilities_t
*/
iic_capabilities_t drv_usi_iic_get_capabilities(int32_t idx)
{
    if (idx < 0 || idx >= CONFIG_USI_NUM) {
        iic_capabilities_t ret;
        memset(&ret, 0, sizeof(iic_capabilities_t));
        return ret;
    }

    return iic_capabilities;
}

/**
  \brief       config iic attributes.
  \param[in]   handle    iic handle to operate.
  \param[in]   mode      iic mode \ref iic_mode_e. if negative, then this attribute not changed.
  \param[in]   speed     iic speed \ref iic_speed_e. if negative, then this attribute not changed.
  \param[in]   addr_mode iic address mode \ref iic_address_mode_e. if negative, then this attribute not changed.
  \param[in]   slave_addr iic address of slave. if negative, then this attribute not changed.
  \return      0 for success, negative for error code
*/
int32_t drv_usi_iic_config(iic_handle_t handle,
                           iic_mode_e mode,
                           iic_speed_e speed,
                           iic_address_mode_e addr_mode,
                           int32_t slave_addr)
{
    int32_t ret;

    ret = drv_usi_iic_config_mode(handle, mode);

    if (ret < 0) {
        return ret;
    }

    ret = drv_usi_iic_config_speed(handle, speed);

    if (ret < 0) {
        return ret;
    }

    ret = drv_usi_iic_config_addr_mode(handle, addr_mode);

    if (ret < 0) {
        return ret;
    }

    ret = drv_usi_iic_config_slave_addr(handle, slave_addr);

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
int32_t drv_usi_iic_config_mode(iic_handle_t handle, iic_mode_e mode)
{
    IIC_NULL_PARAM_CHK(handle);

    if ((int32_t)mode < 0) {
        return 0;
    }

    ck_usi_reg_t *addr = (ck_usi_reg_t *)(((ck_usi_iic_priv_t *)handle)->base);

    switch (mode) {
        case IIC_MODE_MASTER:
            addr->USI_I2C_MODE = CK_USI_I2C_MODE_MATER;   /* I2CM mode*/
            break;

        case IIC_MODE_SLAVE:
            addr->USI_I2C_MODE = CK_USI_I2C_MODE_SLAVE;   /* I2CS mode*/
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
int32_t drv_usi_iic_config_speed(iic_handle_t handle, iic_speed_e speed)
{
    IIC_NULL_PARAM_CHK(handle);

    if ((int32_t)speed < 0) {
        return 0;
    }

    ck_usi_reg_t *addr = (ck_usi_reg_t *)(((ck_usi_iic_priv_t *)handle)->base);

    switch (speed) {
        case IIC_BUS_SPEED_STANDARD:
            ck_iic_set_transfer_speed(addr, CK_IIC_FASTSPEED);
            break;

        case IIC_BUS_SPEED_FAST:
            ck_iic_set_transfer_speed(addr, CK_IIC_FASTSPEED);
            break;

        case IIC_BUS_SPEED_FAST_PLUS:
            return ERR_IIC(DRV_ERROR_PARAMETER);

        case IIC_BUS_SPEED_HIGH:
            return ERR_IIC(DRV_ERROR_PARAMETER);
            break;

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
int32_t drv_usi_iic_config_addr_mode(iic_handle_t handle, iic_address_mode_e addr_mode)
{
    IIC_NULL_PARAM_CHK(handle);

    if ((int32_t)addr_mode < 0) {
        return 0;
    }

    ck_usi_reg_t *addr = (ck_usi_reg_t *)(((ck_usi_iic_priv_t *)handle)->base);

    switch (addr_mode) {
        case IIC_ADDRESS_10BIT:
        case IIC_ADDRESS_7BIT:
            ck_iic_set_addr_mode(addr, addr_mode);
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
int32_t drv_usi_iic_config_slave_addr(iic_handle_t handle, int32_t slave_addr)
{
    IIC_NULL_PARAM_CHK(handle);

    if (slave_addr < 0) {
        return 0;
    }

    ck_usi_reg_t *addr = (ck_usi_reg_t *)(((ck_usi_iic_priv_t *)handle)->base);

    ck_iic_set_target_address(addr, slave_addr);
    return 0;
}

/**
  \brief       Start transmitting data as IIC Master.
               This function is non-blocking,\ref iic_event_e is signaled when transfer completes or error happens.
               \ref drv_usi_iic_get_status can get operating status.
  \param[in]   handle         iic handle to operate.
  \param[in]   devaddr        iic addrress of slave device.
  \param[in]   data           data to send to IIC Slave
  \param[in]   num            Number of data items to send
  \param[in]   xfer_pending   Transfer operation is pending - Stop condition will not be generated
                              Master generates STOP condition (if xfer_pending is "false")
  \return      0 for success, negative for error code
*/
int32_t drv_usi_iic_master_send(iic_handle_t handle, uint32_t devaddr, const void *data, uint32_t num, bool xfer_pending)
{
    IIC_NULL_PARAM_CHK(handle);

    if (data == NULL || num == 0 || devaddr == 0) {
        return ERR_IIC(DRV_ERROR_PARAMETER);
    }

    ck_usi_iic_priv_t *iic_priv = handle;
    ck_usi_reg_t *addr = (ck_usi_reg_t *)(iic_priv->base);

    iic_priv->tx_buf          = (uint8_t *)data;
    iic_priv->tx_total_num    = num;
    iic_priv->tx_cnt          = 0;
    iic_priv->status          = IIC_STATE_DATASEND;
    addr->USI_EN = 0x0;

    /* open and set corresponding interrupts */
    addr->USI_INTR_UNMASK = USI_INT_I2C_STOP | USI_INT_TX_EMPTY;
    addr->USI_INTR_EN = 0;
    addr->USI_EN = 0xf;     /* enable USI module */
    addr->USI_I2CM_CTRL &= ~(1 << 1);
    addr->USI_I2C_HOLD = 5;
    uint16_t tx_data;
    tx_data = (uint16_t) * (iic_priv->tx_buf);
    addr->USI_TX_RX_FIFO = tx_data | 0x200;
    iic_priv->tx_buf++;
    iic_priv->tx_cnt++;
    addr->USI_INTR_EN = USI_INT_I2C_STOP | USI_INT_TX_EMPTY;
    /* blocking mode */
#if 0
    addr->USI_INTR_EN = 0;
    addr->USI_EN = 0xf;     /* enable USI module */
    uint16_t tx_date;
    tx_date = (uint16_t) * (iic_priv->tx_buf);
    addr->USI_TX_RX_FIFO = tx_date | 0x200;
    iic_priv->tx_buf++;
    num--;

    while (num--) {
        while ((addr->USI_FIFO_STA & 0x2) == 0x2);

        tx_date = (uint16_t) * (iic_priv->tx_buf);
        addr->USI_TX_RX_FIFO = tx_date;
        iic_priv->tx_buf++;
    }

    while (addr->USI_I2C_STA & 0x1);

    addr->USI_EN = 0;
#endif
    return 0;

}

/**
  \brief       Start receiving data as IIC Master.
               This function is non-blocking,\ref iic_event_e is signaled when transfer completes or error happens.
               \ref drv_usi_iic_get_status can get operating status.
  \param[in]   handle  iic handle to operate.
  \param[in]   devaddr        iic addrress of slave device.
  \param[out]  data    Pointer to buffer for data to receive from IIC receiver
  \param[in]   num     Number of data items to receive
  \param[in]   xfer_pending   Transfer operation is pending - Stop condition will not be generated
  \return      0 for success, negative for error code
*/
int32_t drv_usi_iic_master_receive(iic_handle_t handle, uint32_t devaddr, void *data, uint32_t num, bool xfer_pending)
{
    IIC_NULL_PARAM_CHK(handle);

    if (data == NULL || num == 0) {
        return ERR_IIC(DRV_ERROR_PARAMETER);
    }

    ck_usi_iic_priv_t *iic_priv = handle;

    iic_priv->rx_buf          = (uint8_t *)data;
    iic_priv->rx_total_num    = num;
    iic_priv->rx_cnt          = 0;
    iic_priv->rx_clk          = num;
    iic_priv->status          = IIC_STATE_WFDATA;

    ck_usi_reg_t *addr = (ck_usi_reg_t *)(iic_priv->base);
    addr->USI_EN = 0;
    /* set usi fifo rx threshold */

    ck_usi_set_rxfifo_th(addr, 1);
    addr->USI_I2CM_CTRL &= ~(1 << 1);
    /* open and set corresponding interrupts */
    addr->USI_INTR_UNMASK = USI_INT_I2C_STOP | USI_INT_RX_THOLD;
    addr->USI_INTR_EN =  0;
    addr->USI_EN = 0xf;     /* enable USI module */
    addr->USI_TX_RX_FIFO = 3 << 8;      /* generate restart condition */
    iic_priv->rx_clk--;
    addr->USI_INTR_EN =  USI_INT_RX_THOLD;
    /* blocking mode */
#if 0
    addr->USI_I2CM_CTRL |= (1 << 1);

    addr->USI_INTR_EN = 0;
    addr->USI_EN = 0xf;     /* enable USI module */
    addr->USI_TX_RX_FIFO = 3 << 8;      /* generate restart condition */

    while (num--) {
        while ((addr->USI_FIFO_STA & 0x4) == 0x4);

        *iic_priv->rx_buf = addr->USI_TX_RX_FIFO;
        iic_priv->rx_buf++;
        addr->USI_TX_RX_FIFO = 1 << 8;      /* generate restart condition */
    }

    addr->USI_EN = 0x0;     /* enable USI module */
#endif
    return 0;
}

/**
  \brief       Start transmitting data as IIC Slave.
               This function is non-blocking,\ref iic_event_e is signaled when transfer completes or error happens.
               \ref drv_usi_iic_get_status can get operating status.
  \param[in]   handle  iic handle to operate.
  \param[in]   data  Pointer to buffer with data to transmit to IIC Master
  \param[in]   num   Number of data items to send
  \return      0 for success, negative for error code
*/
int32_t drv_usi_iic_slave_send(iic_handle_t handle, const void *data, uint32_t num)
{
    IIC_NULL_PARAM_CHK(handle);
    IIC_NULL_PARAM_CHK(data);
    return ERR_IIC(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       Start receiving data as IIC Slave.
               This function is non-blocking,\ref iic_event_e is signaled when transfer completes or error happens.
               \ref drv_usi_iic_get_status can get operating status.
  \param[in]   handle  iic handle to operate.
  \param[out]  data  Pointer to buffer for data to receive from IIC Master
  \param[in]   num   Number of data items to receive
  \return      0 for success, negative for error code
*/
int32_t drv_usi_iic_slave_receive(iic_handle_t handle, void *data, uint32_t num)
{
    IIC_NULL_PARAM_CHK(handle);
    IIC_NULL_PARAM_CHK(data);
    return ERR_IIC(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       abort transfer.
  \param[in]   handle  iic handle to operate.
  \return      error code
*/
int32_t drv_usi_iic_abort_transfer(iic_handle_t handle)
{
    IIC_NULL_PARAM_CHK(handle);

    ck_usi_iic_priv_t *iic_priv = handle;
    ck_usi_reg_t *addr = (ck_usi_reg_t *)(iic_priv->base);

    addr->USI_EN              = 0;  /* disable usi module */
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
iic_status_t drv_usi_iic_get_status(iic_handle_t handle)
{
    iic_status_t iic_status;
    memset(&iic_status, 0, sizeof(iic_status_t));

    if (handle == NULL) {
        return iic_status;
    }

    ck_usi_iic_priv_t *iic_priv = handle;
    ck_usi_reg_t *addr = (ck_usi_reg_t *)(iic_priv->base);

    if ((addr->USI_I2C_MODE & 0x1) == 0x1) {
        iic_status.mode = 1;        /* master mode */

        if (addr->USI_I2C_STA & CK_USI_I2C_STA_MASTER_WORK) {
            iic_status.busy = 1;    /* i2c master is active */
        } else {
            iic_status.busy = 0;    /* i2c master is idle */
        }
    } else {
        iic_status.mode = 0;

        if (addr->USI_I2C_STA & CK_USI_I2C_STA_SLAVE_WORK) {
            iic_status.busy = 1;    /* i2c slave is active */
        } else {
            iic_status.busy = 0;    /* i2c slave is idle */
        }
    }

    if (iic_priv->status == IIC_STATE_WFDATA) {
        iic_status.direction = 1;
    } else {
        iic_status.direction = 0;
    }

    iic_status.general_call = 0;
    iic_status.arbitration_lost = 0;

    if (iic_priv->status == IIC_STATE_ERROR) {
        iic_status.bus_error = 1;
    }

    return iic_status;
}

/**
  \brief       Get IIC transferred data count.
  \param[in]   handle  iic handle to operate.
  \return      the number of the currently transferred data items
*/
uint32_t drv_usi_iic_get_data_count(iic_handle_t handle)
{
    IIC_NULL_PARAM_CHK(handle);
    uint32_t cnt = 0;
    ck_usi_iic_priv_t *iic_priv = handle;

    if ((iic_priv->status == IIC_STATE_WFDATA) || (iic_priv->status == IIC_STATE_RECV_DONE)) {
        cnt = iic_priv->rx_cnt;
    } else if ((iic_priv->status == IIC_STATE_DATASEND) || (iic_priv->status == IIC_STATE_SEND_DONE)) {
        cnt = iic_priv->tx_cnt;
    }

    return cnt;
}

/**
  \brief       control IIC power.
  \param[in]   handle  iic handle to operate.
  \param[in]   state   power state.\ref drv_usi_power_stat_e.
  \return      error code
*/
int32_t drv_usi_iic_power_control(iic_handle_t handle, csi_power_stat_e state)
{
    IIC_NULL_PARAM_CHK(handle);
    return ERR_IIC(DRV_ERROR_UNSUPPORTED);
}
/**
  \brief       Send START command.
  \param[in]   handle  iic handle to operate.
  \return      error code
*/
int32_t drv_usi_iic_send_start(iic_handle_t handle)
{
    IIC_NULL_PARAM_CHK(handle);
    return ERR_IIC(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       Send STOP command.
  \param[in]   handle  iic handle to operate.
  \return      error code
*/
int32_t drv_usi_iic_send_stop(iic_handle_t handle)
{
    IIC_NULL_PARAM_CHK(handle);
    return ERR_IIC(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       Reset IIC peripheral.
  \param[in]   handle  iic handle to operate.
  \return      error code
*/
int32_t drv_usi_iic_reset(iic_handle_t handle)
{
    IIC_NULL_PARAM_CHK(handle);
    return drv_usi_iic_send_stop(handle);
}


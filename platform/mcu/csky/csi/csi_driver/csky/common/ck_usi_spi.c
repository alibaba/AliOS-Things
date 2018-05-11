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
 * @file     ck_usi_spi.c
 * @brief    CSI Source File for USI SPI Driver
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/
#include "csi_config.h"
#include <string.h>
#include "drv_usi_spi.h"
#include "ck_usi_spi.h"
#include "drv_gpio.h"
#include "soc.h"   /*CONFIG_SPI_NUM*/

#define ERR_SPI(errno) (CSI_DRV_ERRNO_SPI_BASE | errno)

#define SPI_NULL_PARAM_CHK(para)                  \
    do {                                        \
        if (para == NULL) {                     \
            return ERR_SPI(DRV_ERROR_PARAMETER);   \
        }                                       \
    } while (0)

typedef struct {
    uint32_t base;
    uint32_t irq;
    int32_t ssel;
    spi_event_cb_t cb_event;
    uint32_t send_num;
    uint32_t recv_num;
    uint8_t *send_buf;
    uint8_t *recv_buf;
    uint8_t enable_slave;
    uint32_t transfer_num;
    uint32_t clk_num;            //clock number with a process of communication
    uint8_t state;               //Current SPI state
    uint32_t mode;               //Current SPI mode
    uint8_t ss_mode;
    spi_status_t status;
    int32_t block_mode;
    int32_t idx;
#ifdef CONFIG_SPI_DMA
    dmac_handle_t dma_handle;
    int32_t dma_tx_id;
    int32_t dma_rx_id;
#endif
#define TRANSFER_STAT_IDLE      0
#define TRANSFER_STAT_SEND      1
#define TRANSFER_STAT_RCV       2
#define TRANSFER_STAT_TRAN      3
    uint8_t  transfer_stat;     //TRANSFER_STAT_* : 0 - idle, 1 - send , 2 -receive , 3 - transceive
    uint32_t tot_num;
} ck_usi_spi_priv_t;

int32_t target_usi_spi_init(int32_t idx, uint32_t *base, uint32_t *irq, uint32_t *ssel);
static ck_usi_spi_priv_t spi_instance[CONFIG_USI_NUM];
static gpio_pin_handle_t   pgpio_pin_handle;

static const spi_capabilities_t spi_capabilities = {
    .simplex = 1,           /* Simplex Mode (Master and Slave) */
    .ti_ssi = 0,            /* TI Synchronous Serial Interface */
    .microwire = 0,         /* Microwire Interface */
    .event_mode_fault = 0   /* Signal Mode Fault event: \ref CSKY_SPI_EVENT_MODE_FAULT */
};

/**
  \brief use gpio pin to simulate ss line for hardware controlled Output mode.
*/
static int32_t ck_spi_ss_init(ck_usi_spi_priv_t *spi_priv)
{
    pgpio_pin_handle = csi_gpio_pin_initialize(spi_priv->ssel, NULL);
    csi_gpio_pin_config_mode(pgpio_pin_handle, GPIO_MODE_PULLNONE);
    csi_gpio_pin_config_direction(pgpio_pin_handle, GPIO_DIRECTION_OUTPUT);
    csi_gpio_pin_write(pgpio_pin_handle, true);
    spi_priv->ss_mode = SPI_SS_MASTER_HW_OUTPUT;
    return 0;
}

/**
  \brief control ss line depend on controlled Output mode.
*/
static int32_t ck_spi_ss_control(ck_usi_spi_priv_t *spi_priv, spi_ss_stat_e stat)
{
    if (spi_priv->ss_mode == SPI_SS_MASTER_HW_OUTPUT) {
        if (stat == SPI_SS_INACTIVE) {
            csi_gpio_pin_write(pgpio_pin_handle, true);
        } else if (stat == SPI_SS_ACTIVE) {
            csi_gpio_pin_write(pgpio_pin_handle, false);
        } else {
            return -1;
        }
    }

    return 0;
}

/**
  \brief       Set the SPI datawidth.
  \param[in]   handle     spi handle
  \param[in]   datawidth  date frame size in bits
  \return      error code
*/
int32_t drv_usi_spi_config_datawidth(spi_handle_t handle, uint32_t datawidth)
{
    SPI_NULL_PARAM_CHK(handle);

    ck_usi_spi_priv_t *spi_priv = handle;
    ck_usi_reg_t *addr = (ck_usi_reg_t *)(spi_priv->base);

    if ((datawidth >= (CKENUM_SPI_DATASIZE_4 + 1)) && (datawidth <= (CKENUM_SPI_DATASIZE_16  + 1))) {
        addr->USI_SPI_CTRL &= CK_USI_SPI_CTRL_DATA_SIZE;
        addr->USI_SPI_CTRL |= (datawidth - 1);

        spi_priv->state |= SPI_CONFIGURED;
        return 0;
    }

    return ERR_SPI(SPI_ERROR_DATA_BITS);
}

/**
  \brief       Set the SPI clock divider.
  \param[in]   handle   spi handle
  \param[in]   baud     spi baud rate
  \return      error code
*/
int32_t drv_usi_spi_config_baudrate(spi_handle_t handle, uint32_t baud)
{
    SPI_NULL_PARAM_CHK(handle);

    ck_usi_spi_priv_t *spi_priv = handle;
    ck_usi_reg_t *addr = (ck_usi_reg_t *)(spi_priv->base);

    if (spi_priv->status.busy) {
        return ERR_SPI(DRV_ERROR_BUSY);
    }

    int32_t sckdv = LSP_DEFAULT_FREQ / baud;

    if (sckdv < 0x1000000) {
        addr->USI_CLK_DIV0 =  sckdv;
    } else {
        return ERR_SPI(DRV_ERROR_PARAMETER);
    }

    spi_priv->state |= SPI_CONFIGURED;

    return 0;
}

/**
  \brief       Set the SPI polarity.
  \param[in]   addr  pointer to register address
  \param[in]   polarity spi polarity
  \return      error code
*/
static int32_t ck_spi_set_polarity(ck_usi_reg_t *addr, CKENUM_SPI_POLARITY polarity)
{
    /* To config the polarity, we can set the usi_spi_ctrl bit(7) as below:
     *     0 - inactive state of serial clock is low
     *     1 - inactive state of serial clock is high
     */
    switch (polarity) {
        case CKENUM_SPI_CLOCK_POLARITY_LOW:
            addr->USI_SPI_CTRL &= (~CK_USI_SPI_CTRL_CPOL);
            break;

        case CKENUM_SPI_CLOCK_POLARITY_HIGH:
            addr->USI_SPI_CTRL |= CK_USI_SPI_CTRL_CPOL;
            break;

        default:
            return ERR_SPI(DRV_ERROR_PARAMETER);
    }

    return 0;
}

/**
  \brief       Set the SPI Phase.
  \param[in]   addr  pointer to register address
  \param[in]   phase    Serial clock phase
  \return      error code
*/
static int32_t ck_spi_set_phase(ck_usi_reg_t *addr, CKENUM_SPI_PHASE phase)
{
    switch (phase) {
        case CKENUM_SPI_CLOCK_PHASE_MIDDLE:
            addr->USI_SPI_CTRL &= (~CK_USI_SPI_CTRL_CPHA);
            break;

        case CKENUM_SPI_CLOCK_PHASE_START:
            addr->USI_SPI_CTRL |= CK_USI_SPI_CTRL_CPHA;
            break;

        default:
            return ERR_SPI(DRV_ERROR_PARAMETER);
    }

    return 0;
}

/**
  \brief       config the SPI format.
  \param[in]   handle   spi handle
  \param[in]   format   spi format. \ref spi_format_e
  \return      error code
*/
int32_t drv_usi_spi_config_format(spi_handle_t handle, spi_format_e format)
{
    SPI_NULL_PARAM_CHK(handle);

    ck_usi_spi_priv_t *spi_priv = handle;
    ck_usi_reg_t *addr = (ck_usi_reg_t *)(spi_priv->base);

    if (spi_priv->status.busy) {
        return ERR_SPI(DRV_ERROR_BUSY);
    }

    switch (format) {
        case SPI_FORMAT_CPOL0_CPHA0:
            ck_spi_set_polarity(addr, CKENUM_SPI_CLOCK_POLARITY_LOW);
            ck_spi_set_phase(addr, CKENUM_SPI_CLOCK_PHASE_MIDDLE);
            break;

        case SPI_FORMAT_CPOL0_CPHA1:
            ck_spi_set_polarity(addr, CKENUM_SPI_CLOCK_POLARITY_LOW);
            ck_spi_set_phase(addr, CKENUM_SPI_CLOCK_PHASE_START);
            break;

        case SPI_FORMAT_CPOL1_CPHA0:
            ck_spi_set_polarity(addr, CKENUM_SPI_CLOCK_POLARITY_HIGH);
            ck_spi_set_phase(addr, CKENUM_SPI_CLOCK_PHASE_MIDDLE);
            break;

        case SPI_FORMAT_CPOL1_CPHA1:
            ck_spi_set_polarity(addr, CKENUM_SPI_CLOCK_POLARITY_HIGH);
            ck_spi_set_phase(addr, CKENUM_SPI_CLOCK_PHASE_START);
            break;

        default:
            return ERR_SPI(SPI_ERROR_FRAME_FORMAT);
    }

    spi_priv->state |= SPI_CONFIGURED;

    return 0;
}

/**
  \brief       config the SPI mode.
  \param[in]   handle   spi handle
  \param[in]   mode     spi mode. \ref spi_mode_e
  \return      error code
*/
int32_t drv_usi_spi_config_mode(spi_handle_t handle, spi_mode_e mode)
{
    SPI_NULL_PARAM_CHK(handle);

    ck_usi_spi_priv_t *spi_priv = handle;
    ck_usi_reg_t *addr = (ck_usi_reg_t *)(spi_priv->base);

    if (spi_priv->status.busy) {
        return ERR_SPI(DRV_ERROR_BUSY);
    }

    if (mode == SPI_MODE_MASTER) {
        spi_priv->state |= SPI_CONFIGURED;
        addr->USI_SPI_MODE |= CK_USI_SPI_MODE_MASTER;
        return 0;
    } else if (mode == SPI_MODE_SLAVE) {
        addr->USI_SPI_MODE &= ~CK_USI_SPI_MODE_MASTER;
    }

    return ERR_SPI(DRV_ERROR_UNSUPPORTED);;
}
/**
\brief       config the SPI block mode.
\param[in]   handle   spi handle
\param[in]   flag 1 - enbale the block mode. 0 - disable the block mode
\return      error code
*/
int32_t drv_usi_spi_config_block_mode(spi_handle_t handle, int32_t flag)
{
    SPI_NULL_PARAM_CHK(handle);
    ck_usi_spi_priv_t *spi_priv = handle;

    if (flag == 1) {
        spi_priv->block_mode = 1;
    } else if (flag == 0) {
        spi_priv->block_mode = 0;
    } else {
        return ERR_SPI(DRV_ERROR_PARAMETER);
    }

    return 0;
}
/**
  \brief       config the SPI bit order.
  \param[in]   handle   spi handle
  \param[in]   order    spi bit order.\ref spi_bit_order_e
  \return      error code
*/
int32_t drv_usi_spi_config_bit_order(spi_handle_t handle, spi_bit_order_e order)
{
    SPI_NULL_PARAM_CHK(handle);
    ck_usi_spi_priv_t *spi_priv = handle;

    if (spi_priv->status.busy) {
        return ERR_SPI(DRV_ERROR_BUSY);
    }

    if (order == SPI_ORDER_MSB2LSB) {
        spi_priv->state |= SPI_CONFIGURED;
        return 0;
    }

    return ERR_SPI(SPI_ERROR_BIT_ORDER);;
}

/**
  \brief       config the SPI slave select mode.
  \param[in]   handle   spi handle
  \param[in]   ss_mode  spi slave select mode. \ref spi_ss_mode_e
  \return      error code
*/
int32_t drv_usi_spi_config_ss_mode(spi_handle_t handle, spi_ss_mode_e ss_mode)
{
    SPI_NULL_PARAM_CHK(handle);

    ck_usi_spi_priv_t *spi_priv = handle;

    if (spi_priv->status.busy) {
        return ERR_SPI(DRV_ERROR_BUSY);
    }

    switch (ss_mode) {
        case SPI_SS_MASTER_SW:
            spi_priv->ss_mode = SPI_SS_MASTER_SW;
            break;

        case SPI_SS_MASTER_HW_OUTPUT:
            ck_spi_ss_init(spi_priv);
            break;

        default:
            return ERR_SPI(SPI_ERROR_SS_MODE);
    }

    spi_priv->state |= SPI_CONFIGURED;

    return 0;
}

/**
  \brief       Set the SPI mode.
  \param[in]   addr  pointer to register address
  \param[in]   mode     SPI_Mode
  \return      error code
*/

static int32_t ck_spi_set_mode(spi_handle_t handle, CKENUM_SPI_MODE mode)
{
    ck_usi_spi_priv_t *spi_priv = handle;
    ck_usi_reg_t *addr = (ck_usi_reg_t *)(spi_priv->base);

    /* It is impossible to write to this register when the SSI is enabled.*/
    /* we can set the TMOD to config transfer mode as below:
     *     TMOD_BIT9  TMOD_BIT8      transfer mode
     *         0          0         transmit & receive
     *         0          1           transmit only
     *         1          0           receive only
     *         1          1             reserved
     */
    switch (mode) {
        case CKENUM_SPI_TXRX:
            addr->USI_SPI_CTRL &= ~CK_USI_SPI_CTRL_TMODE;
            addr->USI_SPI_CTRL |= CK_USI_SPI_CTRL_TMODE_TXRX;
            break;

        case CKENUM_SPI_TX:
            addr->USI_SPI_CTRL &= ~CK_USI_SPI_CTRL_TMODE;
            addr->USI_SPI_CTRL |= CK_USI_SPI_CTRL_TMODE_TX;
            break;

        case CKENUM_SPI_RX:
            addr->USI_SPI_CTRL &= ~CK_USI_SPI_CTRL_TMODE;
            addr->USI_SPI_CTRL |= CK_USI_SPI_CTRL_TMODE_RX;
            break;

        default:
            return ERR_SPI(DRV_ERROR_UNSUPPORTED);;
    }

    spi_priv->mode = mode;
    return 0;
}

/**
  \brief       interrupt service function for receive FIFO full interrupt .
  \param[in]   spi_priv pointer to spi private.
*/
static void ck_spi_intr_rx_full(ck_usi_spi_priv_t *spi_priv)
{
    ck_usi_reg_t *addr = (ck_usi_reg_t *)(spi_priv->base);

    uint8_t *pbuffer = spi_priv->recv_buf;
    uint32_t length = spi_priv->recv_num;

    uint8_t rxnum = USI_FIFO_STA_RX_NUM(addr);
    uint32_t rxdata_num = (rxnum > length) ? length : rxnum;
    uint32_t i = 0u;

    for (i = 0; i < rxdata_num; i++) {
        *pbuffer = addr->USI_TX_RX_FIFO;
        pbuffer++;
    }

    length -= rxdata_num;

    if (length <= 0) {
        /* disable interrupt */
        addr->USI_INTR_EN &= ~USI_INT_SPI_STOP;
        spi_priv->status.busy = 0U;
        addr->USI_EN = 0x0;
        addr->USI_EN = 0xf;
        addr->USI_EN = 0x0;
        spi_priv->recv_num = 0;

        ck_spi_ss_control(spi_priv, SPI_SS_INACTIVE);

        if (spi_priv->cb_event) {
            spi_priv->cb_event(spi_priv->idx, SPI_EVENT_RX_COMPLETE);
            return;
        }
    } else {
        ck_spi_set_mode(spi_priv, CKENUM_SPI_RX);
        spi_priv->recv_buf = pbuffer;
        spi_priv->recv_num = length;
    }

}

/**
  \brief       interrupt service function for transmit FIFO empty interrupt.
  \param[in]   spi_priv pointer to spi private.
*/
static void ck_spi_intr_tx_empty(ck_usi_spi_priv_t *spi_priv)
{
    ck_usi_reg_t *addr = (ck_usi_reg_t *)(spi_priv->base);

    /* transfer mode:transmit & receive */
    uint32_t i = 0u;

    if (spi_priv->mode == CKENUM_SPI_TXRX) {
        /* read data out from rx FIFO */
        while (spi_priv->transfer_num) {
            *spi_priv->recv_buf = addr->USI_TX_RX_FIFO;
            spi_priv->recv_buf++;
            spi_priv->transfer_num--;
        }

        if (spi_priv->clk_num >= USI_TX_MAX_FIFO) {
            spi_priv->transfer_num = USI_TX_MAX_FIFO;
        } else {
            spi_priv->transfer_num = spi_priv->clk_num;
        }

        for (i = 0; i < spi_priv->transfer_num; i++) {
            addr->USI_TX_RX_FIFO = *spi_priv->send_buf;
            spi_priv->send_buf++;
            spi_priv->send_num--;
        }
    } else {    //transfer mode :transmit only
        if (spi_priv->clk_num >= USI_TX_MAX_FIFO) {
            spi_priv->transfer_num = USI_TX_MAX_FIFO;
        } else {
            spi_priv->transfer_num = spi_priv->clk_num;
        }

        for (i = 0; i < spi_priv->transfer_num; i++) {
            addr->USI_TX_RX_FIFO = *spi_priv->send_buf;
            spi_priv->send_buf++;
            spi_priv->send_num--;
        }

    }

    if (spi_priv->clk_num == 0) {

        /* disable interrupt */
        addr->USI_INTR_EN &= ~USI_INT_TX_EMPTY;
        spi_priv->status.busy = 0U;
        spi_priv->send_num = 0;
        ck_spi_ss_control(spi_priv, SPI_SS_INACTIVE);
        addr->USI_EN = 0x0;
        addr->USI_EN = 0xf;

        if (spi_priv->mode == CKENUM_SPI_TXRX) {
            if (spi_priv->cb_event) {
                spi_priv->cb_event(spi_priv->idx, SPI_EVENT_TRANSFER_COMPLETE);
                return;
            }
        } else {
            if (spi_priv->cb_event) {
                spi_priv->cb_event(spi_priv->idx, SPI_EVENT_TX_COMPLETE);
                return;
            }
        }
    }

    spi_priv->clk_num -= spi_priv->transfer_num;

}
/**
  \brief       handler the interrupt.
  \param[in]   spi      Pointer to \ref SPI_RESOURCES
*/
void ck_usi_spi_irqhandler(int32_t idx)
{
    ck_usi_spi_priv_t *spi_priv = &spi_instance[idx];
    ck_usi_reg_t *addr = (ck_usi_reg_t *)(spi_priv->base);

    uint32_t intr_state = addr->USI_INTR_STA & 0x3ffff;

    /* deal with receive FIFO full interrupt */
    if (intr_state & USI_INT_SPI_STOP) {
        ck_spi_intr_rx_full(spi_priv);
        addr->USI_INTR_CLR = USI_INT_SPI_STOP;
    }

    /* deal with transmit FIFO empty interrupt */
    if (intr_state & USI_INT_TX_EMPTY) {
        ck_spi_intr_tx_empty(spi_priv);
        addr->USI_INTR_CLR = USI_INT_TX_EMPTY;
    }

    addr->USI_INTR_CLR = intr_state;
}

/**
  \brief       Initialize SPI Interface. 1. Initializes the resources needed for the SPI interface 2.registers event callback function
  \param[in]   idx spi index
  \param[in]   cb_event  event callback function \ref spi_event_cb_t
  \param[in]   cb_arg    argument for call back function
  \return      return spi handle if success
*/
spi_handle_t drv_usi_spi_initialize(int32_t idx, spi_event_cb_t cb_event)
{
    uint32_t base = 0u;
    uint32_t irq = 0u;
    uint32_t ssel = 0u;

    int32_t ret = target_usi_spi_init(idx, &base, &irq, &ssel);

    if (ret < 0 || ret >= CONFIG_USI_NUM) {
        return NULL;
    }

    ret = drv_usi_initialize(idx);

    if (ret < 0) {
        return NULL;
    }


    ck_usi_spi_priv_t *spi_priv = &spi_instance[idx];

    spi_priv->base = base;
    spi_priv->irq  = irq;
    spi_priv->ssel = ssel;
    spi_priv->idx  = idx;
    spi_priv->cb_event          = cb_event;
    spi_priv->status.busy       = 0U;
    spi_priv->status.data_lost  = 0U;
    spi_priv->status.mode_fault = 0U;
    spi_priv->enable_slave      = 1U;
    spi_priv->state             = SPI_INITIALIZED;

    ck_usi_reg_t *addr = (ck_usi_reg_t *)(spi_priv->base);
    addr->USI_MODE_SEL = USI_MODE_SPI;
    addr->USI_SPI_MODE = CK_USI_SPI_MODE_MASTER;   /* spi master mode */
    csi_vic_enable_irq(spi_priv->irq);
    return (spi_handle_t)spi_priv;
}

/**
  \brief       De-initialize SPI Interface. stops operation and releases the software resources used by the interface
  \param[in]   handle spi handle to operate.
  \return      error code
*/
int32_t drv_usi_spi_uninitialize(spi_handle_t handle)
{
    SPI_NULL_PARAM_CHK(handle);

    ck_usi_spi_priv_t *spi_priv = handle;

    spi_priv->cb_event          = NULL;
    spi_priv->state             = 0U;
    spi_priv->status.busy       = 0U;
    spi_priv->status.data_lost  = 0U;
    spi_priv->status.mode_fault = 0U;

    int32_t  ret = drv_usi_uninitialize(spi_priv->idx);

    if (ret < 0) {
        return ERR_SPI(DRV_ERROR_PARAMETER);
    }

    ck_usi_reg_t *addr = (ck_usi_reg_t *)(spi_priv->base);
    csi_vic_disable_irq(spi_priv->irq);
    addr->USI_EN = 0x0;

    return 0;
}

/**
  \brief       Get driver capabilities.
  \param[in]   spi instance to operate.
  \return      \ref spi_capabilities_t
*/
spi_capabilities_t drv_usi_spi_get_capabilities(int32_t idx)
{
    if (idx < 0 || idx >= CONFIG_USI_NUM) {
        spi_capabilities_t ret;
        memset(&ret, 0, sizeof(spi_capabilities_t));
        return ret;
    }

    return spi_capabilities;
}

/**
  \brief       config spi mode.
  \param[in]   handle spi handle to operate.
  \param[in]   baud      spi baud rate. if negative, then this attribute not changed
  \param[in]   mode      \ref spi_mode_e . if negative, then this attribute not changed
  \param[in]   format    \ref spi_format_e . if negative, then this attribute not changed
  \param[in]   order     \ref spi_bit_order_e . if negative, then this attribute not changed
  \param[in]   ss_mode   \ref spi_ss_mode_e . if negative, then this attribute not changed
  \param[in]   bit_width spi data bitwidth: (1 ~ SPI_DATAWIDTH_MAX) . if negative, then this attribute not changed
  \return      error code
*/
int32_t drv_usi_spi_config(spi_handle_t handle,
                           int32_t          baud,
                           spi_mode_e       mode,
                           spi_format_e     format,
                           spi_bit_order_e  order,
                           spi_ss_mode_e    ss_mode,
                           int32_t          bit_width)
{
    SPI_NULL_PARAM_CHK(handle);
    ck_usi_spi_priv_t *spi_priv = handle;

    if ((spi_priv->state & SPI_INITIALIZED) == 0U) {
        return ERR_SPI(DRV_ERROR_UNSUPPORTED);
    }

    if (spi_priv->status.busy) {
        return ERR_SPI(DRV_ERROR_BUSY);
    }

    spi_priv->status.busy       = 0U;
    spi_priv->status.data_lost  = 0U;
    spi_priv->status.mode_fault = 0U;

    ck_usi_reg_t *addr = (ck_usi_reg_t *)(spi_priv->base);
    addr->USI_EN = 0;

    int32_t ret = 0;

    if (baud >= 0) {
        ret = drv_usi_spi_config_baudrate(handle, baud);

        if (ret < 0) {
            return ret;
        }
    }

    if (mode >= 0) {
        ret = drv_usi_spi_config_mode(handle, mode);

        if (ret < 0) {
            return ret;
        }
    }

    if (mode >= 0) {
        ret = drv_usi_spi_config_format(handle, format);

        if (ret < 0) {
            return ret;
        }
    }

    if (order > 0) {
        ret = drv_usi_spi_config_bit_order(handle, order);

        if (ret < 0) {
            return ret;
        }
    }

    if (ss_mode >= 0) {
        ret = drv_usi_spi_config_ss_mode(handle, ss_mode);

        if (ret < 0) {
            return ret;
        }
    }

    if (bit_width >= 0) {
        ret = drv_usi_spi_config_datawidth(handle, bit_width);

        if (ret < 0) {
            return ret;
        }
    }

    spi_priv->state |= SPI_CONFIGURED;

    return 0;
}

/**
  \brief       config spi default tx value.
  \param[in]   handle spi handle to operate.
  \param[in]   value     default tx value
  \return      error code
*/
int32_t drv_usi_spi_set_default_tx_value(spi_handle_t handle, uint32_t value)
{
    SPI_NULL_PARAM_CHK(handle);

    return ERR_SPI(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       sending data to SPI transmitter,(received data is ignored).
               if non-blocking mode, this function only start the sending,
               \ref spi_event_e is signaled when operation completes or error happens.
               \ref drv_usi_spi_get_status can indicates operation status.
               if blocking mode, this function return after operation completes or error happens.
  \param[in]   handle spi handle to operate.
  \param[in]   data  Pointer to buffer with data to send to SPI transmitter. data_type is : uint8_t for 1..8 data bits, uint16_t for 9..16 data bits,uint32_t for 17..32 data bits,
  \param[in]   num   Number of data items to send.
  \return      error code
*/
int32_t drv_usi_spi_send(spi_handle_t handle, const void *data, uint32_t num)

{
    if (handle == NULL || data == NULL || num == 0) {
        return ERR_SPI(DRV_ERROR_PARAMETER);
    }

    ck_usi_spi_priv_t *spi_priv = handle;

    if ((spi_priv->state & SPI_CONFIGURED) == 0U) {
        return ERR_SPI(DRV_ERROR_UNSUPPORTED);
    }

    if (spi_priv->status.busy) {
        return ERR_SPI(DRV_ERROR_BUSY);
    }

    spi_priv->status.busy       = 1U;
    spi_priv->status.data_lost  = 0U;
    spi_priv->status.mode_fault = 0U;
    spi_priv->tot_num           = num;
    spi_priv->transfer_stat     = TRANSFER_STAT_SEND;

    ck_usi_reg_t *addr = (ck_usi_reg_t *)(spi_priv->base);
    addr->USI_EN            = 0x0 ;
    ck_spi_set_mode(spi_priv, CKENUM_SPI_TX);
    uint8_t *ptxbuffer = (uint8_t *)data;

    if (spi_priv->block_mode) {
        spi_priv->send_num      = num;

        uint32_t once_len , i;
        addr->USI_EN    = 0x0;
        addr->USI_EN    = 0xf;
        ck_spi_ss_control(spi_priv, SPI_SS_ACTIVE);

        while (spi_priv->send_num) {
            once_len = (spi_priv->send_num >= USI_TX_MAX_FIFO) ?
                       USI_TX_MAX_FIFO : spi_priv->send_num;

            for (i = 0; i < once_len; i++) {
                addr->USI_TX_RX_FIFO = *ptxbuffer++;
            }

            while (addr->USI_SPI_STA == 0x1);

            spi_priv->send_num -= once_len;
        }

        ck_spi_ss_control(spi_priv, SPI_SS_INACTIVE);
        spi_priv->status.busy   = 0U;
        addr->USI_EN = 0;
    } else {
        spi_priv->send_num      = num;
        spi_priv->clk_num       = num;
        spi_priv->send_buf      = ptxbuffer;
        spi_priv->transfer_num  = 0;
        ck_spi_ss_control(spi_priv, SPI_SS_ACTIVE);
        addr->USI_EN            = 0x3;
        addr->USI_INTR_EN       = USI_INT_TX_EMPTY | USI_INT_TX_WERR | USI_INT_TX_RERR;
        addr->USI_INTR_UNMASK   = USI_INT_TX_EMPTY | USI_INT_TX_WERR | USI_INT_TX_RERR;
        addr->USI_EN            = 0xf;
    }

    return 0;
}

/**
\brief      receiving data from SPI receiver.transmits the default value as specified by drv_usi_spi_set_default_tx_value
            if non-blocking mode, this function only start the receiving,
            \ref spi_event_e is signaled when operation completes or error happens.
            \ref drv_usi_spi_get_status can indicates operation status.
            if blocking mode, this function return after operation completes or error happens.
\param[in]  handle spi handle to operate.
\param[out] data  Pointer to buffer for data to receive from SPI receiver
\param[in]  num   Number of data items to receive
\return     error code
*/
int32_t drv_usi_spi_receive(spi_handle_t handle, void *data, uint32_t num)
{

    if (handle == NULL || data == NULL || num == 0) {
        return ERR_SPI(DRV_ERROR_PARAMETER);
    }

    ck_usi_spi_priv_t *spi_priv = handle;

    if ((spi_priv->state & SPI_CONFIGURED) == 0U) {
        return ERR_SPI(DRV_ERROR_UNSUPPORTED);
    }

    if (spi_priv->status.busy) {
        return ERR_SPI(DRV_ERROR_BUSY);
    }

    spi_priv->status.busy = 1U;
    spi_priv->status.data_lost = 0U;
    spi_priv->status.mode_fault = 0U;
    spi_priv->tot_num           = num;
    spi_priv->transfer_stat     = TRANSFER_STAT_RCV;

    ck_usi_reg_t *addr = (ck_usi_reg_t *)(spi_priv->base);
    uint8_t *prx_buffer = data;

    if (1) {
        ck_spi_ss_control(spi_priv, SPI_SS_ACTIVE);

        spi_priv->recv_buf = prx_buffer;
        spi_priv->recv_num = num;

        while (spi_priv->recv_num) {
            ck_spi_set_mode(spi_priv, CKENUM_SPI_TXRX);

            addr->USI_EN = 0x3;
            addr->USI_EN = 0xf;
            uint32_t once_len = (spi_priv->recv_num >= USI_RX_MAX_FIFO) ?
                                USI_RX_MAX_FIFO : spi_priv->recv_num;
            int i = 0;

            for (i = 0; i < once_len; i++) {
                addr->USI_TX_RX_FIFO = 0;
            }

            while (addr->USI_SPI_STA == 0x1);

            for (i = 0; i < once_len; i++) {
                *spi_priv->recv_buf++ = addr->USI_TX_RX_FIFO;
            }

            spi_priv->recv_num -= once_len;
        }

        ck_spi_ss_control(spi_priv, SPI_SS_INACTIVE);
        spi_priv->status.busy   = 0U;
        addr->USI_EN = 0;

        if (spi_priv->cb_event) {
            spi_priv->cb_event(spi_priv->idx, SPI_EVENT_RX_COMPLETE);
        }
    } else {
        ck_spi_ss_control(spi_priv, SPI_SS_ACTIVE);

        spi_priv->recv_buf = prx_buffer;
        spi_priv->recv_num = num;
        ck_spi_set_mode(spi_priv, CKENUM_SPI_RX);
        //ck_usi_set_rxfifo_th(addr, num);

        addr->USI_EN = 0x3;
        addr->USI_INTR_EN = USI_INT_SPI_STOP | USI_INT_RX_WERR | USI_INT_RX_RERR;
        addr->USI_INTR_UNMASK = USI_INT_SPI_STOP | USI_INT_RX_WERR | USI_INT_RX_RERR;
        addr->USI_EN = 0xf;

    }

    return 0;
}

/**
  \brief       sending/receiving data to/from SPI transmitter/receiver.
               if non-blocking mode, this function only start the transfer,
               \ref spi_event_e is signaled when operation completes or error happens.
               \ref drv_usi_spi_get_status can indicates operation status.
               if blocking mode, this function return after operation completes or error happens.
  \param[in]   handle spi handle to operate.
  \param[in]   data_out  Pointer to buffer with data to send to SPI transmitter
  \param[out]  data_in   Pointer to buffer for data to receive from SPI receiver
  \param[in]   num_out      Number of data items to send
  \param[in]   num_in       Number of data items to receive
  \return      error code
*/
int32_t drv_usi_spi_transfer(spi_handle_t handle, const void *data_out, void *data_in, uint32_t num_out, uint32_t num_in)
{
    if (handle == NULL || data_in == NULL || num_out == 0 || num_in == 0 || data_out == NULL) {
        return ERR_SPI(DRV_ERROR_PARAMETER);
    }

    ck_usi_spi_priv_t *spi_priv = handle;

    if ((spi_priv->state & SPI_CONFIGURED) == 0U) {
        return ERR_SPI(DRV_ERROR_UNSUPPORTED);
    }

    if (spi_priv->status.busy) {
        return ERR_SPI(DRV_ERROR_BUSY);
    }

    spi_priv->status.busy = 1U;
    spi_priv->status.data_lost = 0U;
    spi_priv->status.mode_fault = 0U;

    spi_priv->tot_num = (num_out > num_in) ? num_out : num_in;
    spi_priv->transfer_stat     = TRANSFER_STAT_TRAN;

    ck_spi_ss_control(spi_priv, SPI_SS_ACTIVE);
    ck_usi_reg_t *addr = (ck_usi_reg_t *)(spi_priv->base);

    ck_spi_set_mode(spi_priv, CKENUM_SPI_TXRX);
    uint8_t *ptx_buffer = (uint8_t *)data_out;
    uint8_t *prx_buffer = (uint8_t *)data_in;
    uint32_t i = 0u;

    if (spi_priv->block_mode) {
        addr->USI_EN    = USI_FM_EN | USI_USI_EN;
        addr->USI_EN    |= USI_RX_FIFO_EN | USI_TX_FIFO_EN;
        uint32_t txrx_num;

        while (spi_priv->tot_num) {
            if (spi_priv->tot_num > USI_TX_MAX_FIFO) {
                txrx_num = USI_TX_MAX_FIFO;
            } else {
                txrx_num = spi_priv->tot_num;
            }

            for (i = 0; i < txrx_num ; i++) {  /* transmit datas in transmit-buffer */
                addr->USI_TX_RX_FIFO = *ptx_buffer;
                ptx_buffer++;
            }

            while (addr->USI_SPI_STA == 0x1);

            for (i = 0; i < txrx_num; i++) {
                *prx_buffer = addr->USI_TX_RX_FIFO;
                prx_buffer++;
            }

            spi_priv->tot_num -= txrx_num;
        }

        ck_spi_ss_control(spi_priv, SPI_SS_INACTIVE);
        spi_priv->status.busy = 0U;
        addr->USI_EN = 0;
    } else {
        spi_priv->send_buf      = ptx_buffer;
        spi_priv->recv_buf      = prx_buffer;
        spi_priv->send_num      = spi_priv->tot_num;
        spi_priv->recv_num      = spi_priv->tot_num;
        spi_priv->clk_num       = spi_priv->tot_num;
        spi_priv->transfer_num  = 0;
        addr->USI_EN    = USI_FM_EN | USI_USI_EN;
        //ck_usi_set_rxfifo_th(addr, spi_priv->tot_num);
        /* enable transmit FIFO empty interrupt */
        addr->USI_INTR_UNMASK |= USI_INT_TX_EMPTY | USI_INT_TX_WERR | USI_INT_TX_RERR;
        addr->USI_INTR_EN |= USI_INT_TX_EMPTY | USI_INT_TX_WERR | USI_INT_TX_RERR;
        addr->USI_EN    = 0xf;
    }

    return 0;
}

/**
  \brief       abort spi transfer.
  \param[in]   handle spi handle to operate.
  \return      error code
*/
int32_t drv_usi_spi_abort_transfer(spi_handle_t handle)
{
    SPI_NULL_PARAM_CHK(handle);

    ck_usi_spi_priv_t *spi_priv = handle;
    ck_usi_reg_t *addr = (ck_usi_reg_t *)(spi_priv->base);

    addr->USI_EN = 0x0;
    spi_priv->status.busy = 0U;
    spi_priv->recv_buf = NULL;
    spi_priv->recv_num = 0;

    return 0;
}

/**
  \brief       Get SPI status.
  \param[in]   handle spi handle to operate.
  \return      SPI status \ref spi_status_t
*/
spi_status_t drv_usi_spi_get_status(spi_handle_t handle)
{
    if (handle == NULL) {
        spi_status_t spi_status;
        memset(&spi_status, 0, sizeof(spi_status_t));
        return spi_status;
    }

    ck_usi_spi_priv_t *spi_priv = handle;

    return spi_priv->status;
}


/**
  \brief       control spi power.
  \param[in]   handle  spi handle to operate.
  \param[in]   state   power state.\ref drv_usi_power_stat_e.
  \return      error code
*/
int32_t drv_usi_spi_power_control(spi_handle_t handle, csi_power_stat_e state)
{
    SPI_NULL_PARAM_CHK(handle);
    return ERR_SPI(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       Get spi transferred data count.
  \param[in]   handle  spi handle to operate.
  \return      number of data bytes transferred
*/
uint32_t drv_usi_spi_get_data_count(spi_handle_t handle)
{
    SPI_NULL_PARAM_CHK(handle);
    uint32_t cnt = 0;

    ck_usi_spi_priv_t *spi_priv = handle;

    if (spi_priv->transfer_stat == TRANSFER_STAT_SEND) {
        cnt = spi_priv->tot_num - spi_priv->send_num;
    } else if (spi_priv->transfer_stat == TRANSFER_STAT_RCV) {
        cnt = spi_priv->tot_num - spi_priv->recv_num;
    } else if (spi_priv->transfer_stat == TRANSFER_STAT_TRAN) {
        cnt = spi_priv->tot_num - (spi_priv->recv_num > spi_priv->send_num ? spi_priv->recv_num : spi_priv->send_num);
    }

    return cnt;
}

/**
  \brief       Control the Slave Select signal (SS).
  \param[in]   handle  spi handle to operate.
  \param[in]   stat    SS state. \ref spi_ss_stat_e.
  \return      error code
*/
int32_t drv_usi_spi_ss_control(spi_handle_t handle, spi_ss_stat_e stat)
{
    SPI_NULL_PARAM_CHK(handle);
    ck_usi_spi_priv_t *spi_priv = handle;

    if ((stat != SPI_SS_INACTIVE) || (stat != SPI_SS_ACTIVE)) {
        return ERR_SPI(DRV_ERROR_PARAMETER);
    }

    return ck_spi_ss_control(spi_priv, stat);
}


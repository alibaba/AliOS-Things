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
 * @file     zx29_spi.c
 * @brief    CSI Source File for SPI Driver
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/
#include <string.h>
#include "drv_spi.h"
#include "zx29_spi.h"
#include "drv_gpio.h"
#include "soc.h"   /*CONFIG_SPI_NUM*/
#include "csi_core.h"
#include "pinmux.h"
#include "pin_name.h"

#define ERR_SPI(errno) (CSI_DRV_ERRNO_SPI_BASE | errno)

#define SPI_NULL_PARAM_CHK(para)                  \
    do {                                        \
        if (para == NULL) {                     \
            return ERR_SPI(DRV_ERROR_PARAMETER);   \
        }                                       \
    } while (0)

static const spi_capabilities_t spi_capabilities = {
    .simplex = 1,           /* Simplex Mode (Master and Slave) */
    .ti_ssi = 1,            /* TI Synchronous Serial Interface */
    .microwire = 1,         /* Microwire Interface */
    .event_mode_fault = 0   /* Signal Mode Fault event: \ref CSKY_SPI_EVENT_MODE_FAULT */
};

typedef enum _t_ssp_status {
    SSP_STA_INIT = 0,
    SSP_STA_OPENED,
    SSP_STA_XFER,
    SSP_STA_DMA_ERR,
    SSP_STA_DONE,
}
t_ssp_status;

enum transfer_e {
    SPI_SEND_ONLY = 0,
    SPI_RECV_ONLY,
    SPI_SEND_RECV,
    SPI_TRANSFER,
};

//t_ssp_status g_ssp_status[CONFIG_SPI_NUM];

/* SPI Device constant data, Instance */
typedef struct {
    uint32_t id;
    uint32_t base;
    uint32_t clk_reg;        /* SPI clock register address */
    uint32_t irq;
    spi_event_cb_t cb_event;
    void *cb_arg;
    uint32_t freq_src;
    uint32_t send_num;
    uint32_t recv_num;
    uint8_t *send_buf;
    uint8_t *recv_buf;
    uint8_t ss_mode;
    t_ssp_status g_ssp_status;
    spi_status_t status;
} zx29_spi_priv_t;

extern int32_t target_spi_init(int32_t idx, uint32_t *base, uint32_t *irq, uint32_t *ssel);

/*******************************************************************************
 *                          Local function declarations                           *
 ******************************************************************************/

static uint32_t ssp_read_empty(uint8_t id);
static uint32_t ssp_transfer(uint8_t id, void *txBuf, void *rxBuf, uint32_t Len, enum transfer_e type);

/*******************************************************************************
 *                           Local variable definitions                           *
 ******************************************************************************/

static zx29_spi_priv_t spi_si_inst_0 = {
    .clk_reg    = SPI0_CLK_REG,
    .freq_src   = SSP_CLK_SRC0,
};

static zx29_spi_priv_t spi_si_inst_1 = {
    .clk_reg    = SPI1_CLK_REG,
    .freq_src   = SSP_CLK_SRC1,
};

static zx29_spi_priv_t *spi_si_inst[] = {
    &spi_si_inst_0,
    &spi_si_inst_1,
};

gpio_pin_handle_t pgpio_pin_handle;

static int32_t zx29_spi_ss_init(zx29_spi_priv_t *spi_priv)
{
//    csi_gpio_port_initialize(PORTA);
    pgpio_pin_handle = csi_gpio_pin_initialize(GPIO8_GPIO8, NULL);
    csi_gpio_pin_config_mode(pgpio_pin_handle, GPIO_MODE_PULLNONE);
    csi_gpio_pin_config_direction(pgpio_pin_handle, GPIO_DIRECTION_OUTPUT);
    csi_gpio_pin_write(pgpio_pin_handle, true);
    spi_priv->ss_mode = SPI_SS_MASTER_HW_OUTPUT;
    return 0;
}

static int32_t zx29_spi_ss_control(zx29_spi_priv_t *spi_priv, spi_ss_stat_e stat)
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
int32_t csi_spi_config_datawidth(spi_handle_t handle, uint32_t datawidth)
{
    SPI_NULL_PARAM_CHK(handle);

    zx29_spi_priv_t *spi_priv = handle;

    volatile uint32_t *regPtr;

    if (spi_priv->id >= CONFIG_SPI_NUM) {
        return ERR_SPI(DRV_ERROR_PARAMETER);
    }

    if (datawidth != SPI_DATASIZE8 + 1  && datawidth != SPI_DATASIZE16 + 1  && datawidth != SPI_DATASIZE32 + 1) {
        return ERR_SPI(SPI_ERROR_DATA_BITS);
    }

    regPtr = (volatile uint32_t *)spi_priv->base;

    SET_REG_FIELD((void *)regPtr + FMT_CTRL_REG, FMT_CTRL_DSS, (datawidth - 1));
    SET_REG_FIELD((void *)regPtr + FIFO_CTRL_REG, FIFO_CTRL_RX_THRES, 2);
    SET_REG_FIELD((void *)regPtr + FIFO_CTRL_REG, FIFO_CTRL_TX_THRES, 2);


    return 0;
}

/**
  \brief       Set the SPI clock divider.
  \param[in]   handle   spi handle
  \param[in]   baud     spi baud rate
  \param[in]   apbfreq  sysclk for spi module.
  \return      error code
*/
int32_t csi_spi_config_baudrate(spi_handle_t handle, uint32_t baud)
{
    SPI_NULL_PARAM_CHK(handle);

    zx29_spi_priv_t *spi_priv = handle;

    volatile uint32_t *regPtr;
    uint32_t i;
    uint32_t cnt[SSP_FREQ_SRC_CNT] = {0,};

    if (spi_priv->id >= CONFIG_SPI_NUM) {
        return ERR_SPI(DRV_ERROR_PARAMETER);
    }

    if ((baud < SSP_MIN_FREQ) || (baud > SSP_MAX_FREQ) || (baud % 2)) {
        return ERR_SPI(DRV_ERROR_PARAMETER);
    }

    for (i = 0; i < SSP_FREQ_SRC_CNT; i++) {
        cnt [i] = (spi_priv->freq_src + spi_priv->freq_src / 10) / baud;

        //change odd to even
        if (!cnt [i]) {
            continue;
        }

        if (cnt[i] == 1) {
            break;
        }

        //cnt > 0, must be even. choice the least clk source.
        if (cnt[i] % 2) {
            cnt[i]++;
        }

        break;
    }

    //can't find.
    if (i == SSP_FREQ_SRC_CNT) {
        return ERR_SPI(DRV_ERROR_PARAMETER);
    }

//    regPtr = spi_si_inst[id]->clk_reg;
    regPtr = (volatile uint32_t *)spi_priv->clk_reg;

    SET_REG_FIELD(regPtr, WCLK_DIV, (cnt[i] - 1));
    SET_REG_FIELD(regPtr, WCLK_SEL, i);
    SET_REG_FIELD(regPtr, WCLK_GATE, WCLK_GATE_MSK);
    SET_REG_FIELD(regPtr, WCLK_SW_GATE, WCLK_SW_GATE_MSK);

    return 0;
}

/**
  \brief       config the SPI format.
  \param[in]   handle   spi handle
  \param[in]   format   spi format. \ref spi_format_e
  \return      error code
*/
int32_t csi_spi_config_format(spi_handle_t handle, spi_format_e format)
{
    SPI_NULL_PARAM_CHK(handle);
    zx29_spi_priv_t *spi_priv = handle;

    if (spi_priv->id >= CONFIG_SPI_NUM) {
        return ERR_SPI(DRV_ERROR_PARAMETER);
    }

    volatile uint32_t *regPtr = (volatile uint32_t *)spi_priv->base;

    if (format != SPI_FORMAT_CPOL0_CPHA0 && format != SPI_FORMAT_CPOL0_CPHA1
        && format != SPI_FORMAT_CPOL1_CPHA0 && format != SPI_FORMAT_CPOL1_CPHA1) {
        return ERR_SPI(SPI_ERROR_FRAME_FORMAT);
    }

    SET_REG_FIELD((void *)regPtr + FMT_CTRL_REG, FMT_CTRL_POL, format);

    return 0;

}

/**
  \brief       config the SPI mode.
  \param[in]   handle   spi handle
  \param[in]   mode     spi mode. \ref spi_mode_e
  \return      error code
*/
int32_t csi_spi_config_mode(spi_handle_t handle, spi_mode_e mode)
{
    SPI_NULL_PARAM_CHK(handle);

    zx29_spi_priv_t *spi_priv = handle;
    volatile uint32_t *regPtr;

    if (spi_priv->id >= CONFIG_SPI_NUM) {
        return ERR_SPI(DRV_ERROR_PARAMETER);
    }

    if (mode > SPI_MODE_SLAVE) {
        return ERR_SPI(SPI_ERROR_SS_MODE);
    }

    regPtr = (volatile uint32_t *)spi_priv->base;
    SET_REG_FIELD((void *)regPtr + COM_CTRL_REG, COM_CTRL_MS, (mode - 1));

    return 0;
}

/**
  \brief       config the SPI mode.
  \param[in]   handle   spi handle
  \param[in]   order    spi bit order.\ref spi_bit_order_e
  \return      error code
*/
int32_t csi_spi_config_bit_order(spi_handle_t handle, spi_bit_order_e order)
{
#if 0
    SPI_NULL_PARAM_CHK(handle);

    zx29_spi_priv_t *spi_priv = handle;

    if (spi_priv->status.busy) {
        return ERR_SPI(DRV_ERROR_BUSY);
    }

    if (order == SPI_ORDER_MSB2LSB) {
        spi_priv->state |= SPI_CONFIGURED;
        return 0;
    }

    return ERR_SPI(SPI_ERROR_BIT_ORDER);;
#endif
    return 0;
}

/**
  \brief       config the SPI slave select mode.
  \param[in]   handle   spi handle
  \param[in]   ss_mode  spi slave select mode. \ref spi_ss_mode_e
  \return      error code
*/
int32_t csi_spi_config_ss_mode(spi_handle_t handle, spi_ss_mode_e ss_mode)
{
    SPI_NULL_PARAM_CHK(handle);

    zx29_spi_priv_t *spi_priv = handle;

    if (spi_priv->status.busy) {
        return ERR_SPI(DRV_ERROR_BUSY);
    }

    switch (ss_mode) {
        case SPI_SS_MASTER_SW:
            spi_priv->ss_mode = SPI_SS_MASTER_SW;
            break;

        case SPI_SS_MASTER_HW_OUTPUT:
            zx29_spi_ss_init(spi_priv);
            break;

        default:
            return ERR_SPI(SPI_ERROR_SS_MODE);
    }

    return 0;
}

/**
  \brief       Set the SPI mode.
  \param[in]   addr  pointer to register address
  \param[in]   mode     SPI_Mode
  \return      error code
*/
#if 0
static int32_t zx29_spi_set_mode(spi_handle_t handle, DWENUM_SPI_MODE mode)
{
    zx29_spi_priv_t *spi_priv = handle;
    zx29_spi_reg_t *addr = (zx29_spi_reg_t *)(spi_priv->base);

    /* It is impossible to write to this register when the SSI is enabled.*/
    /* we can set the TMOD to config transfer mode as below:
     *     TMOD_BIT9  TMOD_BIT8      transfer mode
     *         0          0         transmit & receive
     *         0          1           transmit only
     *         1          0           receive only
     *         1          1             reserved
     */
    switch (mode) {
        case DWENUM_SPI_TXRX:
            addr->CTRLR0 &= (~DW_SPI_TMOD_BIT8);
            addr->CTRLR0 &= (~DW_SPI_TMOD_BIT9);
            break;

        case DWENUM_SPI_TX:
            addr->CTRLR0 |= DW_SPI_TMOD_BIT8;
            addr->CTRLR0 &= (~DW_SPI_TMOD_BIT9);
            break;

        case DWENUM_SPI_RX:
            addr->CTRLR0 &= (~DW_SPI_TMOD_BIT8);
            addr->CTRLR0 |= DW_SPI_TMOD_BIT9;
            break;

        default:
            addr->CTRLR0 |= DW_SPI_TMOD_BIT8;
            addr->CTRLR0 |= DW_SPI_TMOD_BIT9;
            break;
    }

    spi_priv->mode = mode;
    return 0;
}
#endif

/**
  \brief       handler the interrupt.
  \param[in]   spi      Pointer to \ref SPI_RESOURCES
*/
void zx29_spi_irqhandler(int32_t idx)
{
#if 0
    zx29_spi_priv_t *spi_priv = &spi_instance[idx];
    zx29_spi_reg_t *addr = (zx29_spi_reg_t *)(spi_priv->base);

    uint32_t intr = addr->ISR;

    /* deal with receive FIFO full interrupt */
    if (intr & DW_SPI_RXFIFO_FULL) {
        zx29_spi_intr_rx_full(spi_priv);
    }
    /* deal with transmit FIFO empty interrupt */
    else if (intr & DW_SPI_TXFIFO_EMPTY) {
        zx29_spi_intr_tx_empty(spi_priv);
    }

#endif
}

/**
  \brief       Initialize SPI Interface. 1. Initializes the resources needed for the SPI interface 2.registers event callback function
  \param[in]   cb_event  event call back function \ref spi_event_cb_t
  \return      return spi handle if success
*/
spi_handle_t csi_spi_initialize(int32_t idx, spi_event_cb_t cb_event)
{
    uint32_t base = 0u;
    uint32_t irq = 0u;

    int32_t ret = target_spi_init(idx, &base, &irq, NULL);

    if (ret < 0 || ret >= CONFIG_SPI_NUM) {
        return NULL;
    }

    zx29_spi_priv_t *spi_priv = (zx29_spi_priv_t *)spi_si_inst[idx];

    spi_priv->base = base;
    spi_priv->irq  = irq;

    spi_priv->id = idx;
    spi_priv->cb_event          = cb_event;
    spi_priv->status.busy       = 0U;
    spi_priv->status.data_lost  = 0U;
    spi_priv->status.mode_fault = 0U;
    spi_priv->g_ssp_status = SSP_STA_INIT;

//    csi_vic_enable_irq(spi_priv->irq);

    return (spi_handle_t)spi_priv;
}

/**
  \brief       De-initialize SPI Interface. stops operation and releases the software resources used by the interface
  \param[in]   handle spi handle to operate.
  \return      error code
*/
int32_t csi_spi_uninitialize(spi_handle_t handle)
{
    SPI_NULL_PARAM_CHK(handle);

    zx29_spi_priv_t *spi_priv = handle;
//    csi_vic_disable_irq(spi_priv->irq);

    spi_priv->cb_event          = NULL;
    spi_priv->status.busy       = 0U;
    spi_priv->status.data_lost  = 0U;
    spi_priv->status.mode_fault = 0U;

    return 0;
}

/**
  \brief       Get driver capabilities.
  \param[in]   idx spi index
  \return      \ref spi_capabilities_t
*/
spi_capabilities_t csi_spi_get_capabilities(int32_t idx)
{
    if (idx < 0 || idx >= CONFIG_SPI_NUM) {
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
int32_t csi_spi_config(spi_handle_t handle,
                       int32_t          baud,
                       spi_mode_e       mode,
                       spi_format_e     format,
                       spi_bit_order_e  order,
                       spi_ss_mode_e    ss_mode,
                       int32_t  bit_width)
{
    SPI_NULL_PARAM_CHK(handle);

    zx29_spi_priv_t *spi_priv = handle;

    spi_priv->status.busy       = 0U;
    spi_priv->status.data_lost  = 0U;
    spi_priv->status.mode_fault = 0U;

    int32_t ret = 0;

    ret = csi_spi_config_baudrate(handle, baud);

    if (ret < 0) {
        return ret;
    }

    ret = csi_spi_config_mode(handle, mode);

    if (ret < 0) {
        return ret;
    }

    ret = csi_spi_config_format(handle, format);

    if (ret < 0) {
        return ret;
    }

    ret = csi_spi_config_bit_order(handle, order);

    if (ret < 0) {
        return ret;
    }

    ret = csi_spi_config_ss_mode(handle, ss_mode);

    if (ret < 0) {
        return ret;
    }

    ret = csi_spi_config_datawidth(handle, bit_width);

    if (ret < 0) {
        return ret;
    }

    spi_priv->g_ssp_status = SSP_STA_OPENED;

    return 0;
}

/**
  \brief       config spi default tx value.
  \param[in]   handle spi handle to operate.
  \param[in]   value     default tx value
  \return      error code
*/
int32_t csi_spi_set_default_tx_value(spi_handle_t handle, uint32_t value)
{
    SPI_NULL_PARAM_CHK(handle);

    return ERR_SPI(DRV_ERROR_UNSUPPORTED);
}

/**
 * This function will Exchange a block of data on SPI without using DMA
 * @param[in]  id  spi channel index
 * @param[in]  txbuffer  A pointer to the buffer of data to be sent
 * @param[out]  rxbuffer  A pointer to a buffer in which to receive data
 * @param[in]  len  the length of data to be exchaned in units of words.
 * @return the length of data be exchaned
 */
#if 0
#if SPI_LBM_TEST
char test_yu_recv[512] = {0}; //
#endif
static int zx29_spi_exchange(uint8_t id, const char *txbuffer,
                             char *rxbuffer, size_t len)
{

    if (id >= CONFIG_SPI_NUM) {
        return ERR_SPI(DRV_ERROR_PARAMETER);
    }

#if SPI_LBM_TEST
    volatile uint32_t *regPtr = spi_si_inst[id]->base;
    SET_REG_FIELD((void *)regPtr + COM_CTRL_REG, COM_CTRL_LBM, COM_CTRL_LBM_MSK);

    if (!rxbuffer)  {
        memset(test_yu_recv, 0, sizeof(test_yu_recv));
        ssp_transfer(id, txbuffer, test_yu_recv, len);
    }

#else
    return  ssp_transfer(id, (void *)txbuffer, rxbuffer, len, 0);
#endif
}
#endif
/**
  \brief       sending data to SPI transmitter,(received data is ignored).
               if non-blocking mode, this function only start the sending,
               \ref spi_event_e is signaled when operation completes or error happens.
               \ref csi_spi_get_status can indicates operation status.
               if blocking mode, this function return after operation completes or error happens.
  \param[in]   handle spi handle to operate.
  \param[in]   data  Pointer to buffer with data to send to SPI transmitter. data_type is : uint8_t for 1..8 data bits, uint16_t for 9..16 data bits,uint32_t for 17..32 data bits,
  \param[in]   num   Number of data items to send.
  \return      error code
*/
int32_t csi_spi_send(spi_handle_t handle, const void *data, uint32_t num)

{
    if (handle == NULL || data == NULL || num == 0) {
        return ERR_SPI(DRV_ERROR_PARAMETER);
    }

    zx29_spi_priv_t *spi_priv = handle;

    spi_priv->status.busy       = 0U;
    spi_priv->status.data_lost  = 0U;
    spi_priv->status.mode_fault = 0U;

    if (spi_priv->id >= CONFIG_SPI_NUM) {
        return ERR_SPI(DRV_ERROR_PARAMETER);
    }

    if (spi_priv->g_ssp_status < SSP_STA_OPENED) {
        return ERR_SPI(DRV_ERROR_PARAMETER);
    } else if (spi_priv->g_ssp_status == SSP_STA_XFER) {
        return ERR_SPI(DRV_ERROR_PARAMETER);
    } else {
        spi_priv->g_ssp_status = SSP_STA_XFER;
    }

    spi_priv->send_num = num;
    spi_priv->send_buf = (uint8_t *)data;
    enum transfer_e type = SPI_SEND_ONLY;

    zx29_spi_ss_control(spi_priv, SPI_SS_ACTIVE);

    int ret = ssp_transfer(spi_priv->id, (uint8_t *)data, NULL, num, type);
    spi_priv->g_ssp_status = SSP_STA_DONE;

    if (ret != num) {
        return -1;
    }

    spi_priv->status.busy = 0U;

    zx29_spi_ss_control(spi_priv, SPI_SS_INACTIVE);

    return 0;
}

/**
\brief      receiving data from SPI receiver.transmits the default value as specified by csi_spi_set_default_tx_value
            if non-blocking mode, this function only start the receiving,
            \ref spi_event_e is signaled when operation completes or error happens.
            \ref csi_spi_get_status can indicates operation status.
            if blocking mode, this function return after operation completes or error happens.
\param[in]  handle spi handle to operate.
\param[out] data  Pointer to buffer for data to receive from SPI receiver
\param[in]  num   Number of data items to receive
\return     error code
*/
int32_t csi_spi_receive(spi_handle_t handle, void *data, uint32_t num)
{

    if (handle == NULL || data == NULL || num == 0) {
        return ERR_SPI(DRV_ERROR_PARAMETER);
    }

    zx29_spi_priv_t *spi_priv = handle;

    spi_priv->status.busy = 0U;
    spi_priv->status.data_lost = 0U;
    spi_priv->status.mode_fault = 0U;

    if (spi_priv->id >= CONFIG_SPI_NUM) {
        return ERR_SPI(DRV_ERROR_PARAMETER);
    }

    if (spi_priv->g_ssp_status < SSP_STA_OPENED) {
        return ERR_SPI(DRV_ERROR_PARAMETER);
    } else if (spi_priv->g_ssp_status == SSP_STA_XFER) {
        return ERR_SPI(DRV_ERROR_PARAMETER);
    } else {
        spi_priv->g_ssp_status = SSP_STA_XFER;
    }

    zx29_spi_ss_control(spi_priv, SPI_SS_ACTIVE);

    int ret = ssp_transfer(spi_priv->id, NULL, (uint8_t *)data, num, SPI_RECV_ONLY);
    spi_priv->g_ssp_status = SSP_STA_DONE;

    if (ret != num) {
        return -1;
    }

    spi_priv->status.busy = 0U;

    zx29_spi_ss_control(spi_priv, SPI_SS_INACTIVE);

    return 0;
}

/**
  \brief       sending/receiving data to/from SPI transmitter/receiver.
               if non-blocking mode, this function only start the transfer,
               \ref spi_event_e is signaled when operation completes or error happens.
               \ref csi_spi_get_status can indicates operation status.
               if blocking mode, this function return after operation completes or error happens.
  \param[in]   handle spi handle to operate.
  \param[in]   data_out  Pointer to buffer with data to send to SPI transmitter
  \param[out]  data_in   Pointer to buffer for data to receive from SPI receiver
  \param[in]   num_out      Number of data items to send
  \param[in]   num_in       Number of data items to receive
  \return      error code
*/
int32_t csi_spi_transfer(spi_handle_t handle, const void *data_out, void *data_in, uint32_t num_out, uint32_t num_in)
{
    if (handle == NULL || data_in == NULL || num_out == 0 || num_in == 0 || data_out == NULL) {
        return ERR_SPI(DRV_ERROR_PARAMETER);
    }

    zx29_spi_priv_t *spi_priv = handle;

    spi_priv->status.busy = 0U;
    spi_priv->status.data_lost = 0U;
    spi_priv->status.mode_fault = 0U;

    if (spi_priv->id >= CONFIG_SPI_NUM) {
        return ERR_SPI(DRV_ERROR_PARAMETER);
    }

    if (spi_priv->g_ssp_status < SSP_STA_OPENED) {
        return ERR_SPI(DRV_ERROR_PARAMETER);
    } else if (spi_priv->g_ssp_status == SSP_STA_XFER) {
        return ERR_SPI(DRV_ERROR_PARAMETER);
    } else {
        spi_priv->g_ssp_status = SSP_STA_XFER;
    }

    zx29_spi_ss_control(spi_priv, SPI_SS_ACTIVE);
    int ret = ssp_transfer(spi_priv->id, (uint8_t *)data_out, data_in, num_out, SPI_TRANSFER);
    spi_priv->g_ssp_status = SSP_STA_DONE;

    if (ret != num_out) {
        return -1;
    }

    spi_priv->status.busy = 0U;
    zx29_spi_ss_control(spi_priv, SPI_SS_INACTIVE);
    return 0;
}

/**
  \brief       abort spi transfer.
  \param[in]   handle spi handle to operate.
  \return      error code
*/
int32_t csi_spi_abort_transfer(spi_handle_t handle)
{
    SPI_NULL_PARAM_CHK(handle);

    zx29_spi_priv_t *spi_priv = handle;
    spi_priv->status.busy = 0U;

    return 0;
}

/**
  \brief       Get SPI status.
  \param[in]   handle spi handle to operate.
  \return      SPI status \ref ARM_SPI_STATUS
*/
spi_status_t csi_spi_get_status(spi_handle_t handle)
{
    spi_status_t spi_status = {0};

    if (handle == NULL) {
        return spi_status;
    }

    zx29_spi_priv_t *spi_priv = handle;

    return spi_priv->status;
}


/**
  \brief       control spi power.
  \param[in]   handle  spi handle to operate.
  \param[in]   state   power state.\ref csi_power_stat_e.
  \return      error code
*/
int32_t csi_spi_power_control(spi_handle_t handle, csi_power_stat_e state)
{
    return ERR_SPI(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       Get spi transferred data count.
  \param[in]   handle  spi handle to operate.
  \return      number of data bytes transferred
*/
uint32_t csi_spi_get_data_count(spi_handle_t handle)
{
#if 0
    uint32_t cnt = 0;

    if (handle == NULL) {
        return 0;
    }

    zx29_spi_priv_t *spi_priv = handle;

    if (spi_priv->transfer_stat == TRANSFER_STAT_SEND) {
        cnt = spi_priv->tot_num - spi_priv->send_num;
    } else if (spi_priv->transfer_stat == TRANSFER_STAT_RCV) {
        cnt = spi_priv->tot_num - spi_priv->recv_num;
    } else if (spi_priv->transfer_stat == TRANSFER_STAT_TRAN) {
        cnt = spi_priv->tot_num - (spi_priv->recv_num > spi_priv->send_num ? spi_priv->recv_num : spi_priv->send_num);
    }

    return cnt;
#endif
    return 0;
}

/**
  \brief       Check if a value is available to read when spi is in slave mode.
  \param[in]   handle  spi handle to operate.
  \return      non-zero if a value is available
*/
int32_t csi_spi_slave_readable(spi_handle_t handle)
{
    SPI_NULL_PARAM_CHK(handle);
    //zx29_spi_priv_t *spi_priv = handle;

    return ERR_SPI(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       Control the Slave Select signal (SS).
  \param[in]   handle  spi handle to operate.
  \param[in]   stat    SS state. \ref spi_ss_stat_e.
  \return      error code
*/
int32_t csi_spi_ss_control(spi_handle_t handle, spi_ss_stat_e stat)
{
#if 0
    SPI_NULL_PARAM_CHK(handle);
    zx29_spi_priv_t *spi_priv = handle;

    return zx29_spi_ss_control(spi_priv, stat);
#endif
    return 0;
}

static uint32_t ssp_transfer(uint8_t id, void *txBuf, void *rxBuf, uint32_t Len, enum transfer_e type)
{
    //only used by zDrvSsp_Transfer , no PARAM check here
    volatile uint32_t *regPtr = (volatile uint32_t *)spi_si_inst[id]->base;
    uint32_t tx_count = 0;
    uint32_t rx_count = 0;
    uint32_t count = 0;
    uint32_t xferWidth;

    ssp_read_empty(id);

    SET_REG_FIELD((void *)regPtr + COM_CTRL_REG, COM_CTRL_SSPE, SSP_ENABLE);

    while (GET_REG_FIELD((void *)regPtr + COM_CTRL_REG, COM_CTRL_SSPEBACK) == SSP_DISABLE);

    xferWidth = GET_REG_FIELD((void *)regPtr + FMT_CTRL_REG, FMT_CTRL_DSS);

    switch (xferWidth) {
        case SPI_DATASIZE8 : {
            uint8_t *_txBuf = (uint8_t *)txBuf;
            uint8_t *_rxBuf = (uint8_t *)rxBuf;

            do {
                if (type == SPI_SEND_ONLY) {
                    while (!GET_REG_FIELD((void *)regPtr + FIFO_SR_REG, FIFO_SR_TX_CNTR));

                    SET_REG_VALUE((void *)regPtr + DR_REG, _txBuf[tx_count]);
                    //while( GET_REG_FIELD((void*)regPtr+FIFO_SR_REG, FIFO_SR_BUSY) == 1) ;
                    tx_count++;
                } else if (type == SPI_RECV_ONLY) {
                    if (rx_count < Len) {
                        while (!GET_REG_FIELD((void *)regPtr + FIFO_SR_REG, FIFO_SR_TX_CNTR));

                        SET_REG_VALUE((void *)regPtr + DR_REG, 0);
                    }

                    while (GET_REG_FIELD((void *)regPtr + FIFO_SR_REG, FIFO_SR_RX_CNTR) == 0);

                    _rxBuf[rx_count] = GET_REG_VALUE((void *)regPtr + DR_REG);
                    rx_count++;
                } else if (type == SPI_TRANSFER) {
                    SET_REG_VALUE((void *)regPtr + DR_REG, _txBuf[tx_count]);
                    //while( GET_REG_FIELD((void*)regPtr+FIFO_SR_REG, FIFO_SR_BUSY) == 1) ;
                    tx_count++;

                    while (GET_REG_FIELD((void *)regPtr + FIFO_SR_REG, FIFO_SR_RX_CNTR) == 0);

                    if (count == 0) {
                        GET_REG_VALUE((void *)regPtr + DR_REG);
                    } else {
                        _rxBuf[rx_count] = GET_REG_VALUE((void *)regPtr + DR_REG);
                        rx_count++;
                    }

                    count++;
                }
            } while (tx_count < Len && rx_count < Len);

            break;
        }

        case SPI_DATASIZE16 :
        case SPI_DATASIZE32 :
        default:
            break;
    }

    while (GET_REG_FIELD((void *)regPtr + FIFO_SR_REG, FIFO_SR_BUSY) == 1);

    SET_REG_FIELD((void *)regPtr + COM_CTRL_REG, COM_CTRL_SSPE, SSP_DISABLE);

    while (GET_REG_FIELD((void *)regPtr + COM_CTRL_REG, COM_CTRL_SSPEBACK) == SSP_ENABLE);

    return tx_count ? tx_count : rx_count;
}

static uint32_t ssp_read_empty(uint8_t id)
{
    volatile uint32_t *regPtr = (volatile uint32_t *)spi_si_inst[id]->base;

    while (GET_REG_FIELD((void *)regPtr + FIFO_SR_REG, FIFO_SR_RX_CNTR) != 0) {
        GET_REG_VALUE((void *)regPtr + DR_REG);
    }

    return 0;
}


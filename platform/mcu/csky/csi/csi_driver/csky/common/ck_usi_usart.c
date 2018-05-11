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
 * @file     ck_usi_usart.c
 * @brief    CSI Source File for USI usart Driver
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/
#include <csi_config.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "drv_usi_usart.h"
#include "ck_usi_usart.h"
#include "ck_usi.h"
#include "soc.h"
#include "pin_name.h"

#define ERR_USART(errno) (CSI_DRV_ERRNO_USART_BASE | errno)

#define USART_NULL_PARAM_CHK(para)                   \
    do {                                        \
        if (para == NULL) {                     \
            return ERR_USART(DRV_ERROR_PARAMETER);   \
        }                                       \
    } while (0)

//-------------------------------------------------
//                   Data Private
//-------------------------------------------------
typedef struct {
    uint32_t base;
    uint32_t irq;
    usart_event_cb_t cb_event;           ///< Event callback
    uint32_t rx_total_num;
    uint32_t tx_total_num;
    uint8_t *rx_buf;
    uint8_t *tx_buf;
    volatile uint32_t rx_cnt;
    volatile uint32_t tx_cnt;
    volatile uint32_t tx_busy;
    volatile uint32_t rx_busy;
    //for get data count
    uint32_t last_tx_num;
    uint32_t last_rx_num;
    int32_t idx;
} ck_usi_usart_priv_t;

static ck_usi_usart_priv_t usi_usart_instance[CONFIG_USI_NUM];

static const usart_capabilities_t usart_capabilities = {
    .asynchronous = 1,          /* supports USART (Asynchronous) mode */
    .synchronous_master = 0,    /* supports Synchronous Master mode */
    .synchronous_slave = 0,     /* supports Synchronous Slave mode */
    .single_wire = 0,           /* supports USART Single-wire mode */
    .event_tx_complete = 1,     /* Transmit completed event */
    .event_rx_timeout = 0,      /* Signal receive character timeout event */
};
extern int32_t target_usi_usart_init(int32_t idx, uint32_t *base, uint32_t *irq);

/**
  \brief       set the baud rate of usart.
  \param[in]   baud  usart base to operate.
  \param[in]   baudrate baud rate
  \return      error code
*/
int32_t drv_usi_usart_config_baudrate(usart_handle_t handle, uint32_t baudrate)
{
    ck_usi_usart_priv_t *usart_priv = handle;
    ck_usi_reg_t *addr = (ck_usi_reg_t *)(usart_priv->base);

    addr->USI_EN = 0x0;
    /* baudrate=(seriak clock freq)/(16* (divisor + 1); algorithm :rounding*/
    uint32_t divisor = ((LSP_DEFAULT_FREQ  * 10) / baudrate) >> 4;

    if ((divisor % 10) >= 5) {
        divisor = (divisor / 10);
    } else {
        divisor = divisor / 10 - 1;
    }

    addr->USI_CLK_DIV0 = divisor;
    addr->USI_EN = 0xf;
    return 0;
}

/**
  \brief       config usart mode.
  \param[in]   handle  usart handle to operate.
  \param[in]   mode    \ref usart_mode_e
  \return      error code
*/
int32_t drv_usi_usart_config_mode(usart_handle_t handle, usart_mode_e mode)
{
    USART_NULL_PARAM_CHK(handle);
    ck_usi_usart_priv_t *usart_priv = handle;
    ck_usi_reg_t *addr = (ck_usi_reg_t *)(usart_priv->base);
    addr->USI_EN = 0x0;

    if (mode == USART_MODE_ASYNCHRONOUS) {
        addr->USI_EN = 0xf;
        return 0;
    }

    addr->USI_EN = 0xf;
    return ERR_USART(USART_ERROR_MODE);
}

/**
  \brief       config usart parity.
  \param[in]   handle  usart handle to operate.
  \param[in]   parity    \ref usart_parity_e
  \return      error code
*/
int32_t drv_usi_usart_config_parity(usart_handle_t handle, usart_parity_e parity)
{
    USART_NULL_PARAM_CHK(handle);
    ck_usi_usart_priv_t *usart_priv = handle;
    ck_usi_reg_t *addr = (ck_usi_reg_t *)(usart_priv->base);
    addr->USI_EN = 0x0;

    switch (parity) {
        case USART_PARITY_NONE:
            addr->USI_UART_CTRL &= ~CTRL_PARITY_ENABLE;
            break;

        case USART_PARITY_ODD:
            addr->USI_UART_CTRL &= CTRL_PARITY_BITS;
            addr->USI_UART_CTRL |= CTRL_PARITY_ENABLE;
            addr->USI_UART_CTRL |= CTRL_PARITY_ODD;
            break;

        case USART_PARITY_EVEN:
            addr->USI_UART_CTRL &= CTRL_PARITY_BITS;
            addr->USI_UART_CTRL |= CTRL_PARITY_ENABLE;
            addr->USI_UART_CTRL |= CTRL_PARITY_EVEN;
            break;

        default:
            return ERR_USART(USART_ERROR_PARITY);
    }

    addr->USI_EN = 0xf;

    return 0;
}

/**
  \brief       config usart stop bit number.
  \param[in]   handle  usart handle to operate.
  \param[in]   stopbits  \ref usart_stop_bits_e
  \return      error code
*/
int32_t drv_usi_usart_config_stopbits(usart_handle_t handle, usart_stop_bits_e stopbit)
{
    USART_NULL_PARAM_CHK(handle);
    ck_usi_usart_priv_t *usart_priv = handle;
    ck_usi_reg_t *addr = (ck_usi_reg_t *)(usart_priv->base);
    addr->USI_EN = 0x0;

    switch (stopbit) {
        case USART_STOP_BITS_1:
            /* Clear the STOP bit to set 1 stop bit*/
            addr->USI_UART_CTRL &= CTRL_STOP_BITS;
            addr->USI_UART_CTRL |= CTRL_STOP_1;
            break;

        case USART_STOP_BITS_1_5:
            addr->USI_UART_CTRL &= CTRL_STOP_BITS;
            addr->USI_UART_CTRL |= CTRL_STOP_1_5;
            break;

        case USART_STOP_BITS_2:
            addr->USI_UART_CTRL &= CTRL_STOP_BITS;
            addr->USI_UART_CTRL |= CTRL_STOP_2;
            break;

        default:
            return ERR_USART(USART_ERROR_STOP_BITS);
    }

    addr->USI_EN = 0xf;

    return 0;
}

/**
  \brief       config usart data length.
  \param[in]   handle  usart handle to operate.
  \param[in]   databits      \ref usart_data_bits_e
  \return      error code
*/
int32_t drv_usi_usart_config_databits(usart_handle_t handle, usart_data_bits_e databits)
{
    USART_NULL_PARAM_CHK(handle);
    ck_usi_usart_priv_t *usart_priv = handle;
    ck_usi_reg_t *addr = (ck_usi_reg_t *)(usart_priv->base);

    addr->USI_EN = 0x0;

    switch (databits) {
        case USART_DATA_BITS_5:
            addr->USI_UART_CTRL &= CTRL_DBIT_BITS;
            addr->USI_UART_CTRL |= CTRL_DBIT_5;
            break;

        case USART_DATA_BITS_6:
            addr->USI_UART_CTRL &= CTRL_DBIT_BITS;
            addr->USI_UART_CTRL |= CTRL_DBIT_6;
            break;

        case USART_DATA_BITS_7:
            addr->USI_UART_CTRL &= CTRL_DBIT_BITS;
            addr->USI_UART_CTRL |= CTRL_DBIT_7;
            break;

        case USART_DATA_BITS_8:
            addr->USI_UART_CTRL &= CTRL_DBIT_BITS;
            addr->USI_UART_CTRL |= CTRL_DBIT_8;
            break;

        default:
            return ERR_USART(USART_ERROR_DATA_BITS);
    }

    addr->USI_EN = 0xf;

    return 0;
}
/**
  \brief       interrupt service function for transmitter holding register empty.
  \param[in]   usart_priv usart private to operate.
*/
static void ck_usart_intr_threshold_empty(ck_usi_usart_priv_t *usart_priv)
{
    if (usart_priv->tx_total_num == 0) {
        return;
    }

    ck_usi_reg_t *addr = (ck_usi_reg_t *)(usart_priv->base);

    if (usart_priv->tx_cnt >= usart_priv->tx_total_num) {
        addr->USI_INTR_EN &= (~USI_INT_TX_EMPTY);
        addr->USI_INTR_EN |= USI_INT_UART_STOP;
        usart_priv->last_tx_num = usart_priv->tx_total_num;

        usart_priv->tx_cnt = 0;
        usart_priv->tx_busy = 0;
        usart_priv->tx_buf = NULL;
        usart_priv->tx_total_num = 0;

        if (usart_priv->cb_event) {
            usart_priv->cb_event(usart_priv->idx, USART_EVENT_SEND_COMPLETE);
        }
    } else {
        uint32_t remain_txdata  = usart_priv->tx_total_num - usart_priv->tx_cnt;
        uint32_t txdata_num = (remain_txdata > (USI_TX_MAX_FIFO - 1)) ? (USI_TX_MAX_FIFO - 1) : remain_txdata;
        volatile uint32_t i = 0u;

        for (i = 0; i < txdata_num; i++) {
            addr->USI_TX_RX_FIFO = *((uint8_t *)usart_priv->tx_buf);
            usart_priv->tx_cnt++;
            usart_priv->tx_buf++;
        }

        addr->USI_INTR_CLR = USI_INT_TX_EMPTY;
    }
}

/**
  \brief        interrupt service function for receiver data available.
  \param[in]   usart_priv usart private to operate.
*/
static void ck_usart_intr_recv_data(ck_usi_usart_priv_t *usart_priv)
{
    if ((usart_priv->rx_total_num == 0) || (usart_priv->rx_buf == NULL)) {
        usart_priv->cb_event(usart_priv->idx, USART_EVENT_RECEIVED);
        return;
    }

    ck_usi_reg_t *addr = (ck_usi_reg_t *)(usart_priv->base);
    uint32_t rxfifo_num = (addr->USI_FIFO_STA >> 16) & 0xf;
    uint32_t rxdata_num = (rxfifo_num > usart_priv->rx_total_num) ? usart_priv->rx_total_num : rxfifo_num;
    uint32_t i;

    for (i = 0; i < rxdata_num; i++) {
        *((uint8_t *)usart_priv->rx_buf) = addr->USI_TX_RX_FIFO;;
        usart_priv->rx_cnt++;
        usart_priv->rx_buf++;
    }

    if (usart_priv->rx_cnt >= usart_priv->rx_total_num) {
        usart_priv->last_rx_num = usart_priv->rx_total_num;
        usart_priv->rx_cnt = 0;
        usart_priv->rx_buf = NULL;
        usart_priv->rx_busy = 0;
        usart_priv->rx_total_num = 0;

        if (usart_priv->cb_event) {
            usart_priv->cb_event(usart_priv->idx, USART_EVENT_RECEIVE_COMPLETE);
        }
    }
}

/**
  \brief        interrupt service function for character timeout.
  \param[in]   usart_priv usart private to operate.
*/
static void ck_usart_intr_char_timeout(ck_usi_usart_priv_t *usart_priv)
{
    if ((usart_priv->rx_total_num != 0) && (usart_priv->rx_buf != NULL)) {
        ck_usart_intr_recv_data(usart_priv);
        return;
    }

    if (usart_priv->cb_event) {
        usart_priv->cb_event(usart_priv->idx, USART_EVENT_RECEIVED);
    } else {
        ck_usi_reg_t *addr = (ck_usi_reg_t *)(usart_priv->base);

        while (!(addr->USI_FIFO_STA & 0x4)) {
            addr->USI_TX_RX_FIFO = addr->USI_TX_RX_FIFO;
        }
    }
}

/**
  \brief        interrupt service function for receiver line.
  \param[in]   usart_priv usart private to operate.
*/
static void ck_usart_intr_recv_line(ck_usi_usart_priv_t *usart_priv)
{
    ck_usi_reg_t *addr = (ck_usi_reg_t *)(usart_priv->base);
    addr->USI_EN &= ~USI_RX_FIFO_EN;
    addr->USI_EN |= USI_RX_FIFO_EN;

    if (usart_priv->cb_event) {
        usart_priv->cb_event(usart_priv->idx, USART_EVENT_RX_PARITY_ERROR);
    }
}

void ck_usi_usart_irqhandler(int idx)
{
    ck_usi_usart_priv_t *usart_priv = &usi_usart_instance[idx];
    ck_usi_reg_t *addr = (ck_usi_reg_t *)(usart_priv->base);

    uint32_t intr_state = addr->USI_INTR_STA & 0x3ffff;

    if (intr_state & USI_INT_TX_EMPTY) {
        ck_usart_intr_threshold_empty(usart_priv);
        addr->USI_INTR_CLR = USI_INT_TX_EMPTY;
    }

    if (intr_state & USI_INT_RX_THOLD) {
        ck_usart_intr_recv_data(usart_priv);
        addr->USI_INTR_CLR = USI_INT_RX_THOLD;
    }

    if (intr_state & USI_INT_UART_STOP) {
        if (USI_FIFO_STA_RX_NUM(addr) > 0) {
            ck_usart_intr_char_timeout(usart_priv);     //receive small data
        }

        addr->USI_INTR_CLR = USI_INT_RX_THOLD;
    }

    if (intr_state & USI_INT_UART_PERR) {
        ck_usart_intr_recv_line(usart_priv);
        addr->USI_INTR_CLR = USI_INT_RX_THOLD;
    }

    addr->USI_INTR_CLR = intr_state;
}

//-------------------------------------------------
//                    function
//-------------------------------------------------

/**
  \brief       get character in query mode.
  \param[in]   handle  usart handle to operate.
  \param[in]   the pointer to the recieve charater.
  \return      error code
*/
int32_t drv_usi_usart_getchar(usart_handle_t handle, uint8_t *ch)
{
    ck_usi_usart_priv_t *usart_priv = handle;
    ck_usi_reg_t *addr = (ck_usi_reg_t *)(usart_priv->base);

    addr->USI_EN = 0x7;
    addr->USI_EN = 0xf;

    while (!(addr->USI_FIFO_STA & 0x4));

    *ch = addr->USI_TX_RX_FIFO;

    return 0;
}

/**
  \brief       transmit character in query mode.
  \param[in]   handle  usart handle to operate.
  \param[in]   ch  the input charater
  \return      error code
*/
int32_t drv_usi_usart_putchar(usart_handle_t handle, uint8_t ch)
{
    ck_usi_usart_priv_t *usart_priv = handle;
    ck_usi_reg_t *addr = (ck_usi_reg_t *)(usart_priv->base);
    //addr->USI_EN = 0xb;
    //addr->USI_EN = 0xf;
    addr->USI_TX_RX_FIFO = ch;

    while (!(addr->USI_FIFO_STA & 0x1));

    return 0;
}

//==============================================================
//                           DRIVER
//==============================================================

/**
  \brief       Get driver capabilities.
  \param[in]   idx usart index
  \return      \ref usart_capabilities_t
*/
usart_capabilities_t drv_usi_usart_get_capabilities(int32_t idx)
{
    if (idx < 0 || idx >= CONFIG_USI_NUM) {
        usart_capabilities_t ret;
        memset(&ret, 0, sizeof(usart_capabilities_t));
        return ret;
    }

    return usart_capabilities;
}

/**
  \brief       Initialize USART Interface. 1. Initializes the resources needed for the USART interface 2.registers event callback function
  \param[in]   idx usart index
  \param[in]   cb_event event call back function \ref usart_event_cb_t
  \return      return usart handle if success
*/
usart_handle_t drv_usi_usart_initialize(int32_t idx, usart_event_cb_t cb_event)
{
    //initialize instace
    uint32_t base;
    uint32_t irq;

    int32_t ret  = target_usi_usart_init(idx, &base, &irq);

    if (ret < 0 || ret >= CONFIG_USI_NUM) {
        return NULL;
    }

    ret = drv_usi_initialize(idx);

    if (ret < 0) {
        return NULL;
    }

    ck_usi_usart_priv_t *usart_priv = &usi_usart_instance[idx];

    usart_priv->base = base;
    usart_priv->irq = irq;
    usart_priv->cb_event = cb_event;
    ck_usi_reg_t *addr = (ck_usi_reg_t *)(usart_priv->base);

    addr->USI_EN = 0x0;
    addr->USI_INTR_UNMASK = CK_UART_INT_ENABLE_DEFAUL;
    addr->USI_INTR_EN = CK_UART_INT_ENABLE_DEFAUL;

    ck_usi_set_rxfifo_th(addr, USI_RX_MAX_FIFO);

    addr->USI_MODE_SEL = USI_MODE_UART;
    csi_vic_enable_irq(usart_priv->irq);

    return usart_priv;
}

/**
  \brief       De-initialize UART Interface. stops operation and releases the software resources used by the interface
  \param[in]   handle  usart handle to operate.
  \return      error code
*/
int32_t drv_usi_usart_uninitialize(usart_handle_t handle)
{
    USART_NULL_PARAM_CHK(handle);

    ck_usi_usart_priv_t *usart_priv = handle;
    ck_usi_reg_t *addr = (ck_usi_reg_t *)(usart_priv->base);

    csi_vic_disable_irq(usart_priv->irq);
    addr->USI_EN = 0;
    usart_priv->cb_event   = NULL;
    int32_t  ret = drv_usi_uninitialize(usart_priv->idx);

    if (ret < 0) {
        return ERR_USART(DRV_ERROR_PARAMETER);
    }

    return 0;
}

/**
  \brief       config usart mode.
  \param[in]   handle  usart handle to operate.
  \param[in]   mode      \ref usart_mode_e
  \param[in]   parity    \ref usart_parity_e
  \param[in]   stopbits  \ref usart_stop_bits_e
  \param[in]   bits      \ref usart_data_bits_e
  \param[in]   baud      configured baud
  \return      error code
*/
int32_t drv_usi_usart_config(usart_handle_t handle,
                             uint32_t baud,
                             usart_mode_e mode,
                             usart_parity_e parity,
                             usart_stop_bits_e stopbits,
                             usart_data_bits_e bits)
{
    int32_t ret;
    ck_usi_usart_priv_t *usart_priv = handle;
    ck_usi_reg_t *addr = (ck_usi_reg_t *)(usart_priv->base);

    addr->USI_EN = 0x0;
    /* control the data_bit of the usart*/
    ret = drv_usi_usart_config_baudrate(handle, baud);

    if (ret < 0) {
        return ret;
    }

    /* control the parity of the usart*/
    ret = drv_usi_usart_config_parity(handle, parity);

    if (ret < 0) {
        return ret;
    }

    /* control mode of the usart*/
    ret = drv_usi_usart_config_mode(handle, mode);

    if (ret < 0) {
        return ret;
    }

    /* control the stopbit of the usart*/
    ret = drv_usi_usart_config_stopbits(handle, stopbits);

    if (ret < 0) {
        return ret;
    }

    ret = drv_usi_usart_config_databits(handle, bits);

    if (ret < 0) {
        return ret;
    }

    addr->USI_EN = 0xf;
    return 0;
}

/**
  \brief       Start sending data to UART transmitter,(received data is ignored).
               The function is non-blocking,UART_EVENT_TRANSFER_COMPLETE is signaled when transfer completes.
               drv_usi_usart_get_status can indicates if transmission is still in progress or pending
  \param[in]   handle  usart handle to operate.
  \param[in]   data  Pointer to buffer with data to send to UART transmitter. data_type is : uint8_t for 1..8 data bits, uint16_t for 9..16 data bits,uint32_t for 17..32 data bits,
  \param[in]   num   Number of data items to send
  \return      error code
*/
int32_t drv_usi_usart_send(usart_handle_t handle, const void *data, uint32_t num)
{
    USART_NULL_PARAM_CHK(handle);
    USART_NULL_PARAM_CHK(data);

    if (num == 0) {
        return ERR_USART(DRV_ERROR_PARAMETER);
    }

    ck_usi_usart_priv_t *usart_priv = handle;
    ck_usi_reg_t *addr = (ck_usi_reg_t *)(usart_priv->base);

    usart_priv->tx_buf = (uint8_t *)data;
    usart_priv->tx_total_num = num;
    usart_priv->tx_cnt = 0;
    usart_priv->tx_busy = 1;
    usart_priv->last_tx_num = 0;

    /* enable the interrupt*/
    addr->USI_INTR_UNMASK   |= USI_INT_TX_EMPTY;
    addr->USI_INTR_EN       |= USI_INT_TX_EMPTY;
    addr->USI_INTR_EN       &= ~USI_INT_UART_STOP;

    addr->USI_EN = 0xf;
    return 0;
}

/**
  \brief       Abort Send data to UART transmitter
  \param[in]   handle  usart handle to operate.
  \return      error code
*/
int32_t drv_usi_usart_abort_send(usart_handle_t handle)
{
    USART_NULL_PARAM_CHK(handle);
    ck_usi_usart_priv_t *usart_priv = handle;

    ck_usi_reg_t *addr = (ck_usi_reg_t *)(usart_priv->base);
    addr->USI_INTR_EN &= (~USI_INT_TX_EMPTY);

    usart_priv->tx_cnt = usart_priv->tx_total_num;
    usart_priv->tx_cnt = 0;
    usart_priv->tx_busy = 0;
    usart_priv->tx_buf = NULL;
    usart_priv->tx_total_num = 0;
    return 0;
}
/**
  \brief       Start receiving data from USART receiver. \n
               This function is non-blocking,
               \ref usart_event_e is signaled when operation completes or error happens.
               \ref drv_usi_usart_get_status can get operation status.
  \param[in]   handle  usart handle to operate.
  \param[out]  data  Pointer to buffer for data to receive from USART receiver.data_type is : uint8_t for 5..8 data bits, uint16_t for 9 data bits
  \param[in]   num   Number of data items to receive
  \return      error code
*/
int32_t drv_usi_usart_receive(usart_handle_t handle, void *data, uint32_t num)
{
    USART_NULL_PARAM_CHK(handle);
    USART_NULL_PARAM_CHK(data);

    ck_usi_usart_priv_t *usart_priv = handle;

    usart_priv->rx_buf = (uint8_t *)data;   // Save receive buffer usart
    usart_priv->rx_total_num = num;         // Save number of data to be received
    usart_priv->rx_cnt = 0;
    usart_priv->rx_busy = 1;
    usart_priv->last_rx_num = 0;
    return 0;
}

/**
  \brief       query data from UART receiver FIFO.
  \param[in]   handle  usart handle to operate.
  \param[out]  data  Pointer to buffer for data to receive from UART receiver
  \param[in]   num   Number of data items to receive
  \return      fifo data num to receive
*/
int32_t drv_usi_usart_receive_query(usart_handle_t handle, void *data, uint32_t num)
{
    USART_NULL_PARAM_CHK(handle);
    USART_NULL_PARAM_CHK(data);

    ck_usi_usart_priv_t *usart_priv = handle;
    ck_usi_reg_t *addr = (ck_usi_reg_t *)(usart_priv->base);
    int32_t recv_num = 0;
    uint8_t *dest = (uint8_t *)data;

    while ((addr->USI_FIFO_STA & 0x4) == 0) {
        *dest++ = addr->USI_TX_RX_FIFO;
        recv_num++;

        if (recv_num >= num) {
            break;
        }
    }

    return recv_num;
}

/**
  \brief       Abort Receive data from UART receiver
  \param[in]   handle  usart handle to operate.
  \return      error code
*/
int32_t drv_usi_usart_abort_receive(usart_handle_t handle)
{
    USART_NULL_PARAM_CHK(handle);
    ck_usi_usart_priv_t *usart_priv = handle;

    usart_priv->rx_cnt = usart_priv->rx_total_num;
    return 0;
}

/**
  \brief       Start synchronously sends data to the USART transmitter and receives data from the USART receiver. used in synchronous mode
               This function is non-blocking,\ref usart_event_e is signaled when operation completes or error happens.
               \ref drv_usi_usart_get_status can get operation status.
  \param[in]   handle  usart handle to operate.
  \param[in]   data_out  Pointer to buffer with data to send to USART transmitter.data_type is : uint8_t for 5..8 data bits, uint16_t for 9 data bits
  \param[out]  data_in   Pointer to buffer for data to receive from USART receiver.data_type is : uint8_t for 5..8 data bits, uint16_t for 9 data bits
  \param[in]   num       Number of data items to transfer
  \return      error code
*/
int32_t drv_usi_usart_transfer(usart_handle_t handle, const void *data_out, void *data_in, uint32_t num)
{
    USART_NULL_PARAM_CHK(handle);
    return ERR_USART(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       abort sending/receiving data to/from USART transmitter/receiver.
  \param[in]   handle  usart handle to operate.
  \return      error code
*/
int32_t drv_usi_usart_abort_transfer(usart_handle_t handle)
{
    USART_NULL_PARAM_CHK(handle);
    return ERR_USART(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       Get USART status.
  \param[in]   handle  usart handle to operate.
  \return      USART status \ref usart_status_t
*/
usart_status_t drv_usi_usart_get_status(usart_handle_t handle)
{
    if (handle == NULL) {
        usart_status_t ret;
        memset(&ret, 0, sizeof(usart_status_t));
        return ret;
    }

    usart_status_t usart_status;
    ck_usi_usart_priv_t *usart_priv = handle;

    usart_status.tx_busy = usart_priv->tx_busy;
    usart_status.rx_busy = usart_priv->rx_busy;
    usart_status.tx_enable  = 1;
    usart_status.rx_enable  = 1;

    return usart_status;
}

/**
  \brief       control the transmitter.
  \param[in]   handle  usart handle to operate.
  \param[in]   enable  1 - enable the transmitter. 0 - disable the transmitter
  \return      error code
*/
int32_t drv_usi_usart_control_tx(usart_handle_t handle, uint32_t enable)
{
    USART_NULL_PARAM_CHK(handle);
    return ERR_USART(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       control the receiver.
  \param[in]   handle  usart handle to operate.
  \param[in]   enable  1 - enable the receiver. 0 - disable the receiver
  \return      error code
*/
int32_t drv_usi_usart_control_rx(usart_handle_t handle, uint32_t enable)
{
    USART_NULL_PARAM_CHK(handle);
    return ERR_USART(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       flush receive/send data.
  \param[in]   handle usart handle to operate.
  \param[in]   type \ref usart_flush_type_e.
  \return      error code
*/
int32_t drv_usi_usart_flush(usart_handle_t handle, usart_flush_type_e type)
{
    USART_NULL_PARAM_CHK(handle);

    ck_usi_usart_priv_t *usart_priv = handle;
    ck_usi_reg_t *addr = (ck_usi_reg_t *)(usart_priv->base);

    if (type == USART_FLUSH_WRITE) {
        addr->USI_EN &= ~USI_TX_FIFO_EN;
        addr->USI_EN |= USI_TX_FIFO_EN;
    } else if (type == USART_FLUSH_READ) {
        addr->USI_EN &= ~USI_RX_FIFO_EN;
        addr->USI_EN |= USI_RX_FIFO_EN;
    } else {
        return ERR_USART(DRV_ERROR_PARAMETER);
    }

    return 0;
}
/**
  \brief       set interrupt mode.
  \param[in]   handle usart handle to operate.
  \param[in]   type \ref usart_intr_type_e.
  \param[in]   flag 0-OFF, 1-ON.
  \return      error code
*/
int32_t drv_usi_usart_set_interrupt(usart_handle_t handle, usart_intr_type_e type, int flag)
{
    USART_NULL_PARAM_CHK(handle);

    ck_usi_usart_priv_t *usart_priv = handle;
    ck_usi_reg_t *addr = (ck_usi_reg_t *)(usart_priv->base);

    switch (type) {
        case USART_INTR_WRITE:
            if (flag == 0) {
                addr->USI_INTR_EN &= ~USI_INT_TX_EMPTY;
            } else if (flag == 1) {
                addr->USI_INTR_EN |= USI_INT_TX_EMPTY;
            } else {
                return ERR_USART(DRV_ERROR_PARAMETER);
            }

            break;

        case USART_INTR_READ:
            if (flag == 0) {
                addr->USI_INTR_EN &= ~USI_INT_RX_THOLD;
            } else if (flag == 1) {
                addr->USI_INTR_EN |= USI_INT_RX_THOLD;
            } else {
                return ERR_USART(DRV_ERROR_PARAMETER);
            }

            break;

        default:
            return ERR_USART(DRV_ERROR_PARAMETER);

    }

    return 0;
}

/**
  \brief       Get usart send data count.
  \param[in]   handle  usart handle to operate.
  \return      number of currently transmitted data bytes
*/
uint32_t drv_usi_usart_get_tx_count(usart_handle_t handle)
{
    USART_NULL_PARAM_CHK(handle);

    if (handle == NULL) {
        return 0;
    }

    ck_usi_usart_priv_t *usart_priv = handle;

    if (usart_priv->tx_busy) {
        return usart_priv->tx_cnt;
    } else {
        return usart_priv->last_tx_num;
    }
}

/**
  \brief       Get usart receive data count.
  \param[in]   handle  usart handle to operate.
  \return      number of currently received data bytes
*/
uint32_t drv_usi_usart_get_rx_count(usart_handle_t handle)
{
    USART_NULL_PARAM_CHK(handle);

    if (handle == NULL) {
        return 0;
    }

    ck_usi_usart_priv_t *usart_priv = handle;

    if (usart_priv->rx_busy) {
        return usart_priv->rx_cnt;
    } else {
        return usart_priv->last_rx_num;
    }
}

/**
  \brief       config usart flow control type.
  \param[in]   handle  usart handle to operate.
  \param[in]   flowctrl_type   flow control type.\ref usart_flowctrl_type_e.
  \return      error code
*/
int32_t drv_usi_usart_power_control(usart_handle_t handle, csi_power_stat_e state)
{
    USART_NULL_PARAM_CHK(handle);
    return ERR_USART(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       config usart flow control type.
  \param[in]   handle  usart handle to operate.
  \param[in]   flowctrl_type   flow control type.\ref usart_flowctrl_type_e.
  \return      error code
*/
int32_t drv_usi_usart_config_flowctrl(usart_handle_t handle,
                                      usart_flowctrl_type_e flowctrl_type)
{
    USART_NULL_PARAM_CHK(handle);

    switch (flowctrl_type) {
        case USART_FLOWCTRL_CTS:
            return ERR_USART(DRV_ERROR_UNSUPPORTED);

        case USART_FLOWCTRL_RTS:
            return ERR_USART(DRV_ERROR_UNSUPPORTED);

        case USART_FLOWCTRL_CTS_RTS:
            return ERR_USART(DRV_ERROR_UNSUPPORTED);

        case USART_FLOWCTRL_NONE:
            return ERR_USART(DRV_ERROR_UNSUPPORTED);

        default:
            return ERR_USART(DRV_ERROR_UNSUPPORTED);
    }

    return 0;
}

/**
  \brief       config usart clock Polarity and Phase.
  \param[in]   handle  usart handle to operate.
  \param[in]   cpol    Clock Polarity.\ref usart_cpol_e.
  \param[in]   cpha    Clock Phase.\ref usart_cpha_e.
  \return      error code
*/
int32_t drv_usi_usart_config_clock(usart_handle_t handle, usart_cpol_e cpol, usart_cpha_e cpha)
{
    USART_NULL_PARAM_CHK(handle);
    return ERR_USART(DRV_ERROR_UNSUPPORTED);
}


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
 * @file     zx29_usart.c
 * @brief    CSI Source File for usart Driver
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/

#include <csi_config.h>
#include <stdbool.h>
#include <string.h>
#include "drv_usart.h"
#include "drv_intc.h"
#include "zx29_usart.h"
#include "soc.h"
#include "csi_core.h"

#define ERR_USART(errno) (CSI_DRV_ERRNO_USART_BASE | errno)

#define putreg32(v, x) (*(volatile uint32_t*)(x) = (v))
#define getreg32(x)    (*(volatile uint32_t *)(x))

/*
 * setting config may be accessed when the USART is not
 * busy(USR[0]=0) and the DLAB bit(LCR[7]) is set.
 */

#define WAIT_USART_IDLE(addr)\
    do {                       \
        int32_t timecount = 0;  \
        while ((addr->USR & USR_UART_BUSY) && (timecount < UART_BUSY_TIMEOUT)) {\
            timecount++;\
        }\
        if (timecount >= UART_BUSY_TIMEOUT) {\
            return ERR_USART(DRV_ERROR_TIMEOUT);\
        }                                   \
    } while(0)

#define USART_NULL_PARAM_CHK(para)                   \
    do {                                        \
        if (para == NULL) {                     \
            return ERR_USART(DRV_ERROR_PARAMETER);   \
        }                                       \
    } while (0)

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
    uint32_t last_tx_num;
    uint32_t last_rx_num;
    int32_t idx;
} zx29_usart_priv_t;

extern int32_t target_usart_init(int32_t idx, uint32_t *base, uint32_t *irq);

static zx29_usart_priv_t usart_instance[CONFIG_USART_NUM];

static const usart_capabilities_t usart_capabilities = {
    .asynchronous = 1,          /* supports USART (Asynchronous) mode */
    .synchronous_master = 0,    /* supports Synchronous Master mode */
    .synchronous_slave = 0,     /* supports Synchronous Slave mode */
    .single_wire = 0,           /* supports USART Single-wire mode */
    .event_tx_complete = 1,     /* Transmit completed event */
    .event_rx_timeout = 0,      /* Signal receive character timeout event */
};

inline uint32_t serial_getreg(zx29_usart_priv_t *priv, int offset)
{
    return (getreg32((volatile uint32_t *)(priv->base + offset)));
}

inline void serial_setreg(zx29_usart_priv_t *priv, int offset, uint32_t value)
{
    putreg32(value, (volatile uint32_t *)(priv->base + offset));
}

/* write while USR regester is not busy */
static inline int usr_idle_write(zx29_usart_priv_t *priv, int offset, uint32_t value)
{
    uint32_t readvalue;
    readvalue = serial_getreg(priv, offset);

    if (readvalue == value) {
        return 0;
    }

    serial_setreg(priv, offset, value);
    readvalue = serial_getreg(priv, offset);

    if (readvalue != value) {
        return -1;
    }

    return 0;
}

/**
  \brief       set the bautrate of usart.
  \param[in]   addr  usart base to operate.
  \param[in]   baudrate.
  \param[in]   apbfreq the frequence of the apb.
  \return      error code
*/
static int32_t zx29_usart_set_baudrate(zx29_usart_priv_t *priv, uint32_t baudrate)
{
    unsigned int ibrd, fbrd;

    /* the baudrates that uart surported as follows: */
    if ((baudrate == B4800) || (baudrate == B9600) ||
        (baudrate == B14400) || (baudrate == B19200) ||
        (baudrate == B38400) || (baudrate == B56000) ||
        (baudrate == B57600) || (baudrate == B115200)) {

        /*
         * DLH and DLL may be accessed when the UART is not
         * busy(USR[0]=0) and the DLAB bit(LCR[7]) is set.
         */

        /* baudrate=(seriak clock freq)/(16*divisor). */
        // divisor  = ((apbfreq / baudrate) >> 4);
        ibrd = LSP_DEFAULT_FREQ  / (baudrate << 4);
        fbrd = ((LSP_DEFAULT_FREQ % (baudrate << 4)) * 8 + baudrate) / (2 * baudrate);

        serial_setreg(priv, zx29_UART_FBRD, fbrd);
        serial_setreg(priv, zx29_UART_IBRD, ibrd);
    }

    return 0;
}

/**
  \brief       enable or disable parity.
  \param[in]   addr  usart base to operate.
  \param[in]   parity ODD=8, EVEN=16, or NONE=0.
  \return      error code
*/

static int32_t zx29_usart_set_parity(zx29_usart_priv_t *priv, usart_parity_e parity)
{
    uint32_t tmp = 0;

    switch (parity) {
        case USART_PARITY_NONE:
            /* clear the PEN bit(LCR[3]) to disable parity */
            tmp  = serial_getreg(priv, zx29_UART_LCRH);
            tmp &= (~UART_LCRH_PEN);

            if (usr_idle_write(priv, zx29_UART_LCRH, tmp) < 0) {
                return -1;
            }

            break;


        case USART_PARITY_ODD:
            /* set PEN and clear EPS(LCR[4]) to set the ODD parity */
            tmp  = serial_getreg(priv, zx29_UART_LCRH);
            tmp |= UART_LCRH_PEN;
            tmp &= (~UART_LCRH_EPS);

            if (usr_idle_write(priv, zx29_UART_LCRH, tmp) < 0) {
                return -1;
            }

            break;


        case USART_PARITY_EVEN:
            /* set PEN and EPS(LCR[4]) to set the EVEN parity */
            tmp  = serial_getreg(priv, zx29_UART_LCRH);
            tmp |= UART_LCRH_PEN;
            tmp |= UART_LCRH_EPS;

            if (usr_idle_write(priv, zx29_UART_LCRH, tmp) < 0) {
                return -1;
            }

            break;

        default:
            return ERR_USART(USART_ERROR_PARITY);
    }

    return 0;
}

/**
  \brief       set the stop bit.
  \param[in]   addr  usart base to operate.
  \param[in]   stopbit two possible value: USART_STOP_BITS_1 and USART_STOP_BITS_2.
  \return      error code
*/
static int32_t zx29_usart_set_stopbit(zx29_usart_priv_t *priv, usart_stop_bits_e stopbit)
{
    uint32_t tmp = 0;

    switch (stopbit) {
        case USART_STOP_BITS_1:
            /* Clear the STOP bit to set 1 stop bit*/
            tmp  = serial_getreg(priv, zx29_UART_LCRH);
            tmp &= (~LCR_STOP_BIT2);

            if (usr_idle_write(priv, zx29_UART_LCRH, tmp) < 0) {
                return -1;
            }

            break;

        case USART_STOP_BITS_2:
            /*
             * If the STOP bit is set "1",we'd gotten 1.5 stop
             * bits when DLS(LCR[1:0]) is zero, else 2 stop bits.
             */
            tmp  = serial_getreg(priv, zx29_UART_LCRH);
            tmp |= LCR_STOP_BIT2;

            if (usr_idle_write(priv, zx29_UART_LCRH, tmp) < 0) {
                return -1;
            }

            break;

        default:
            return ERR_USART(USART_ERROR_STOP_BITS);
    }

    return 0;
}

/**
  \brief       the transmit data length,and we have four choices:5, 6, 7, and 8 bits.
  \param[in]   addr  usart base to operate.
  \param[in]   databits the data length that user decides.
  \return      error code
*/
static int32_t zx29_usart_set_databit(zx29_usart_priv_t *priv, usart_data_bits_e databits)
{
    uint32_t tmp = 0;

    /* The word size decides by the DLS bits(LCR[1:0]), and the
     * corresponding relationship between them is:
     *   DLS   word size
     *       00 -- 5 bits
     *       01 -- 6 bits
     *       10 -- 7 bits
     *       11 -- 8 bits
     */
    tmp  = serial_getreg(priv, zx29_UART_LCRH);

    switch (databits) {
        case USART_DATA_BITS_5:
            tmp &= 0x9f;
            tmp &= LCR_WORD_SIZE_5;
            break;

        case USART_DATA_BITS_6:
            tmp &= 0x9f;
            tmp |= LCR_WORD_SIZE_6;
            break;

        case USART_DATA_BITS_7:
            tmp &= 0x9f;
            tmp |= LCR_WORD_SIZE_7;
            break;

        case USART_DATA_BITS_8:
            tmp |= LCR_WORD_SIZE_8;
            break;

        case USART_DATA_BITS_9:
            tmp |= LCR_WORD_SIZE_9;
            break;

        default:
            return ERR_USART(USART_ERROR_DATA_BITS);
    }

    tmp |= UART_LCRH_FEN;

    if (usr_idle_write(priv, zx29_UART_LCRH, tmp) < 0) {
        return -1;
    }

    return 0;
}
static int32_t zx29_usart_enable_intr(zx29_usart_priv_t *priv)
{
    uint32_t control = (UART_RXIM | UART_RTIM | UART_FEIM | UART_PEIM | UART_BEIM | UART_OEIM);

    if (usr_idle_write(priv, zx29_UART_IMSC, control) < 0) {
        return -1;
    }

    return 0;
}

/**
  \brief       get character in query mode.
  \param[in]   instance  usart instance to operate.
  \param[in]   the pointer to the recieve charater.
  \return      error code
*/
int32_t csi_usart_getchar(usart_handle_t handle, uint8_t *ch)
{
    zx29_usart_priv_t *usart_priv = handle;

    while ((serial_getreg(usart_priv, zx29_UART_FR) & UART_FR_RXFE));

    return serial_getreg(usart_priv, zx29_UART_DR) & 0xff;
}

/**
  \brief       transmit character in query mode.
  \param[in]   instance  usart instance to operate.
  \param[in]   ch  the input charater
  \return      error code
*/
int32_t csi_usart_putchar(usart_handle_t handle, uint8_t ch)
{
    zx29_usart_priv_t *usart_priv = handle;

    /* Wait until there is space in the FIFO */
    while ((serial_getreg(usart_priv, zx29_UART_FR) & UART_FR_TXFF));

    /* Send the character */
    serial_setreg(usart_priv, zx29_UART_DR, ch);

    while ((serial_getreg(usart_priv, zx29_UART_FR) & UART_FR_TXBUSY));

    return 0;

}
/**
  \brief       interrupt service function for transmitter holding register empty.
  \param[in]   usart_priv usart private to operate.
*/
static void zx29_usart_intr_threshold_empty(int32_t idx, zx29_usart_priv_t *usart_priv)
{
    if (usart_priv->tx_total_num == 0) {
        return;
    }


    if (usart_priv->tx_cnt >= usart_priv->tx_total_num) {
        uint32_t tmp = serial_getreg(usart_priv, zx29_UART_IMSC);
        tmp &= ~UART_TXIM;
        serial_setreg(usart_priv, zx29_UART_IMSC, tmp);
        //addr->IER &= (~IER_THRE_INT_ENABLE);
        usart_priv->last_tx_num = usart_priv->tx_total_num;

        usart_priv->tx_cnt = 0;
        usart_priv->tx_busy = 0;
        usart_priv->tx_buf = NULL;
        usart_priv->tx_total_num = 0;

        if (usart_priv->cb_event) {
            usart_priv->cb_event(idx, USART_EVENT_SEND_COMPLETE);
        }
    } else {
        while ((usart_priv->tx_cnt != usart_priv->tx_total_num) && (!(serial_getreg(usart_priv, zx29_UART_FR) & UART_FR_TXFF))) {
            /* Send the character */
            serial_setreg(usart_priv, zx29_UART_DR, *((uint8_t *)usart_priv->tx_buf));
            usart_priv->tx_cnt++;
            usart_priv->tx_buf++;
        }
    }
}

/**
  \brief        interrupt service function for receiver data available.
  \param[in]   usart_priv usart private to operate.
*/
static void zx29_usart_intr_recv_data(int32_t idx, zx29_usart_priv_t *usart_priv)
{
    if ((usart_priv->rx_total_num == 0) || (usart_priv->rx_buf == NULL)) {
        usart_priv->cb_event(idx, USART_EVENT_RECEIVED);
        return;
    }

    //while(!(serial_getreg(usart_priv, zx29_UART_FR) & UART_FR_RXFE)){
    while ((usart_priv->rx_cnt != usart_priv->rx_total_num)  && (!(serial_getreg(usart_priv, zx29_UART_FR) & UART_FR_RXFE))) {
        *((uint8_t *)usart_priv->rx_buf) = serial_getreg(usart_priv, zx29_UART_DR) & 0xff;
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
            usart_priv->cb_event(idx, USART_EVENT_RECEIVE_COMPLETE);
        }
    }

}

/**
  \brief        interrupt service function for receiver line.
  \param[in]   usart_priv usart private to operate.
*/
static void x29_usart_intr_recv_line(int32_t idx, zx29_usart_priv_t *usart_priv, uint32_t  intr_state)
{
    while (!(serial_getreg(usart_priv, zx29_UART_FR) & UART_FR_RXFE)) {
        serial_getreg(usart_priv, zx29_UART_DR);
    }

    /** Break Interrupt bit. This is used to indicate the detection of a
      * break sequence on the serial input data.
      */
    if (intr_state & UART_BEIM) {
        if (usart_priv->cb_event) {
            usart_priv->cb_event(idx, USART_EVENT_RX_BREAK);
        }
    }

    /** Framing Error bit. This is used to indicate the occurrence of a
      * framing error in the receiver. A framing error occurs when the receiver
      * does not detect a valid STOP bit in the received data.
      */
    if (intr_state & UART_FEIC) {
        if (usart_priv->cb_event) {
            usart_priv->cb_event(idx, USART_EVENT_RX_FRAMING_ERROR);
        }
    }

    if (intr_state & UART_PEIC) {
        if (usart_priv->cb_event) {
            usart_priv->cb_event(idx, USART_EVENT_RX_PARITY_ERROR);
        }
    }

    /** Overrun error bit. This is used to indicate the occurrence of an overrun error.
      * This occurs if a new data character was received before the previous data was read.
      */
    if (intr_state & UART_OEIC) {
        if (usart_priv->cb_event) {
            usart_priv->cb_event(idx ,USART_EVENT_RX_OVERFLOW);
        }
    }

}
/**
  \brief        interrupt service function for character timeout.
  \param[in]   usart_priv usart private to operate.
*/
static void x29_usart_intr_char_timeout(int32_t idx, zx29_usart_priv_t *usart_priv)
{
    //if ((usart_priv->rx_total_num != 0) && (usart_priv->rx_buf != NULL)) {
    if (usart_priv->rx_total_num != 0) {
        zx29_usart_intr_recv_data(idx, usart_priv);
        return;
    }

    if (usart_priv->cb_event) {
        usart_priv->cb_event(idx, USART_EVENT_RECEIVED);
    } else {
        while (!(serial_getreg(usart_priv, zx29_UART_FR) & UART_FR_RXFE)) {
            serial_getreg(usart_priv, zx29_UART_DR);
        }
    }
}

/**
  \brief       the interrupt service function.
  \param[in]   index of usart instance.
*/
void zx29_usart_irqhandler(int32_t idx)
{
    zx29_usart_priv_t *usart_priv = &usart_instance[idx];
    csi_intc_ack_irq(usart_priv->irq);

    uint32_t intr_state = serial_getreg(usart_priv, zx29_UART_MIS);

    serial_setreg(usart_priv, zx29_UART_ICR, intr_state);

    if ((intr_state & UART_TXIM) != false) { /* interrupt source:transmitter interrupt */
        zx29_usart_intr_threshold_empty(idx, usart_priv);
    }

    if ((intr_state & UART_RXIM) != false) { /* interrupt source:receiver interrupt */
        zx29_usart_intr_recv_data(idx, usart_priv);
    }

    if ((intr_state & UART_RTIM) != false) { /* interrupt source:receiver timeout interrupt */
        x29_usart_intr_char_timeout(idx, usart_priv);
    }

    if ((intr_state & (UART_FEIM | UART_PEIM | UART_BEIM | UART_OEIM)) != false) {
        x29_usart_intr_recv_line(idx, usart_priv, intr_state);
    }
}
/* init the uart related registers */
static int32_t zx29_uart_reg_init(zx29_usart_priv_t *usart_priv, uint8_t id)
{
    uint32_t control;
    //priv->ioportctr = HOBBIT_GPIO0_PORTCTR;

    /* Clear all pending error and receive interrupts */
    serial_setreg(usart_priv, zx29_UART_ICR, 0);

    if ((id == ZX29_UART3_NUM) || id == ZX29_UART0_NUM) {
        /* set interrupt fifo level RX:8byte , TX:16byte */
        serial_setreg(usart_priv,  zx29_UART_IFLS, UART_IFLS_RX8_32 | UART_IFLS_TX16_32);
    } else {
        /* set interrupt fifo level RX:1/2 Full, TX:1/2 Full */
        serial_setreg(usart_priv,  zx29_UART_IFLS, UART_IFLS_RX4_8 | UART_IFLS_TX4_8);
    }

    /* Provoke TX FIFO interrupt into asserting. */
    control = UART_CR_UARTEN | UART_CR_TXE | UART_CR_LBE;
    serial_setreg(usart_priv, zx29_UART_CR, control);
    serial_setreg(usart_priv, zx29_UART_FBRD, 0);
    serial_setreg(usart_priv, zx29_UART_IBRD, 1);
    serial_setreg(usart_priv, zx29_UART_LCRH, 0);
    serial_setreg(usart_priv, zx29_UART_DR, 0);

    while (serial_getreg(usart_priv, zx29_UART_FR) & UART_FR_TXBUSY);

    control = UART_CR_UARTEN | UART_CR_RXE | UART_CR_TXE;
    serial_setreg(usart_priv, zx29_UART_CR, control);

    /* Clear out any spuriously appearing RX interrupts */
    serial_setreg(usart_priv, zx29_UART_ICR, (UART_RTIS | UART_RXIS));
    control = 0;
    serial_setreg(usart_priv,  zx29_UART_IMSC, control);

    return 0;
}

/**
  \brief       Get driver capabilities.
  \param[in]   idx usart index
  \return      \ref usart_capabilities_t
*/
usart_capabilities_t csi_usart_get_capabilities(int32_t idx)
{
    if (idx < 0 || idx >= CONFIG_USART_NUM) {
        usart_capabilities_t ret;
        memset(&ret, 0, sizeof(usart_capabilities_t));
        return ret;
    }

    return usart_capabilities;
}

/**
  \brief       Initialize USART Interface. 1. Initializes the resources needed for the USART interface 2.registers event callback function
  \param[in]   usart pin of tx
  \param[in]   usart pin of rx
  \param[in]   cb_event  Pointer to \ref usart_event_cb_t
  \return      return usart handle if success
*/
usart_handle_t csi_usart_initialize(int32_t idx, usart_event_cb_t cb_event)
{
    uint32_t base = 0u;
    uint32_t irq = 0u;

    int32_t ret = target_usart_init(idx, &base, &irq);

    if (ret < 0 || ret >= CONFIG_USART_NUM) {
        return NULL;
    }

    zx29_usart_priv_t *usart_priv = &usart_instance[idx];
    usart_priv->base = base;
    usart_priv->irq = irq;
    usart_priv->cb_event = cb_event;
    usart_priv->idx = idx;


    return usart_priv;
}

/**
  \brief       De-initialize UART Interface. stops operation and releases the software resources used by the interface
  \param[in]   handle  usart handle to operate.
  \return      error code
*/
int32_t csi_usart_uninitialize(usart_handle_t handle)
{
    USART_NULL_PARAM_CHK(handle);

    zx29_usart_priv_t *usart_priv = handle;

    csi_vic_disable_irq(usart_priv->irq);
    usart_priv->cb_event   = NULL;

    return 0;
}

/**
  \brief       config usart mode.
  \param[in]   handle  usart handle to operate.
  \param[in]   sysclk    configured system clock.
  \param[in]   mode      \ref usart_mode_e
  \param[in]   parity    \ref usart_parity_e
  \param[in]   stopbits  \ref usart_stop_bits_e
  \param[in]   bits      \ref usart_data_bits_e
  \param[in]   baud      configured baud
  \return      error code
*/
int32_t csi_usart_config(usart_handle_t handle,
                         uint32_t baud,
                         usart_mode_e mode,
                         usart_parity_e parity,
                         usart_stop_bits_e stopbits,
                         usart_data_bits_e bits)
{
    USART_NULL_PARAM_CHK(handle);
    zx29_usart_priv_t *usart_priv = handle;

    int32_t ret = zx29_uart_reg_init(usart_priv, usart_priv->idx);

    if (ret < 0) {
        return ret;
    }

    /* control the data_bit of the usart*/
    ret = zx29_usart_set_baudrate(usart_priv, baud);

    if (ret < 0) {
        return ret;
    }

    /* control the parity of the usart*/
    ret = zx29_usart_set_parity(usart_priv, parity);

    if (ret < 0) {
        return ret;
    }

    /* control the stopbit of the usart*/
    ret = zx29_usart_set_stopbit(usart_priv, stopbits);

    if (ret < 0) {
        return ret;
    }

    ret = zx29_usart_set_databit(usart_priv, bits);

    if (ret < 0) {
        return ret;
    }

    ret = zx29_usart_enable_intr(usart_priv);

    if (ret < 0) {
        return ret;
    }
    csi_intc_set_attribute(usart_priv->irq, 3 , INT_MODE_RISING_EDGE);
    csi_intc_ack_irq(usart_priv->irq);
    csi_intc_enable_irq(usart_priv->irq);
    return 0;
}
/**
  \brief       config usart default tx value. used in syn mode
  \param[in]   handle  usart handle to operate.
  \param[in]   value  default tx value
  \return      error code
*/
int32_t csi_usart_set_default_tx_value(usart_handle_t handle, uint32_t value)
{
    USART_NULL_PARAM_CHK(handle);
    return ERR_USART(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       Start sending data to UART transmitter,(received data is ignored).
               The function is non-blocking,UART_EVENT_TRANSFER_COMPLETE is signaled when transfer completes.
               csi_usart_get_status can indicates if transmission is still in progress or pending
  \param[in]   handle  usart handle to operate.
  \param[in]   data  Pointer to buffer with data to send to UART transmitter. data_type is : uint8_t for 1..8 data bits, uint16_t for 9..16 data bits,uint32_t for 17..32 data bits,
  \param[in]   num   Number of data items to send
  \return      error code
*/
int32_t csi_usart_send(usart_handle_t handle, const void *data, uint32_t num)
{
    USART_NULL_PARAM_CHK(handle);
    USART_NULL_PARAM_CHK(data);

    if (num == 0) {
        return ERR_USART(DRV_ERROR_PARAMETER);
    }

    zx29_usart_priv_t *usart_priv = handle;

    usart_priv->tx_buf = (uint8_t *)data;
    usart_priv->tx_total_num = num;
    usart_priv->tx_cnt = 0;
    usart_priv->tx_busy = 1;

    // uint32_t tmp = serial_getreg(usart_priv,zx29_UART_IMSC);
    // tmp |= UART_TXIM;

    // serial_setreg(usart_priv,  zx29_UART_IMSC,tmp);
    //if (usr_idle_write(usart_priv, zx29_UART_IMSC, tmp) < 0) {
    //  return -1;
    //}
    while (num--) {
        /* Wait until there is space in the FIFO */
        while ((serial_getreg(usart_priv, zx29_UART_FR) & UART_FR_TXFF));

        /* Send the character */
        serial_setreg(usart_priv, zx29_UART_DR, *((uint8_t *)usart_priv->tx_buf));
        usart_priv->tx_buf++;
    }

    while ((serial_getreg(usart_priv, zx29_UART_FR) & UART_FR_TXBUSY));

    //addr->IER &= (~IER_THRE_INT_ENABLE);
    usart_priv->last_tx_num = usart_priv->tx_total_num;

    usart_priv->tx_cnt = 0;
    usart_priv->tx_busy = 0;
    usart_priv->tx_buf = NULL;
    usart_priv->tx_total_num = 0;

    if (usart_priv->cb_event) {
        usart_priv->cb_event(usart_priv->idx, USART_EVENT_SEND_COMPLETE);
    }

    return 0;
}

/**
  \brief       Abort Send data to UART transmitter
  \param[in]   handle  usart handle to operate.
  \return      error code
*/
int32_t csi_usart_abort_send(usart_handle_t handle)
{
    USART_NULL_PARAM_CHK(handle);
    zx29_usart_priv_t *usart_priv = handle;

    usart_priv->tx_cnt = usart_priv->tx_total_num;
    return 0;
}

/**
  \brief       Start receiving data from UART receiver.transmits the default value as specified by csi_usart_set_default_tx_value
  \param[in]   handle  usart handle to operate.
  \param[out]  data  Pointer to buffer for data to receive from UART receiver
  \param[in]   num   Number of data items to receive
  \return      error code
*/
int32_t csi_usart_receive(usart_handle_t handle, void *data, uint32_t num)
{
    USART_NULL_PARAM_CHK(handle);
    USART_NULL_PARAM_CHK(data);

    zx29_usart_priv_t *usart_priv = handle;

    usart_priv->rx_buf = (uint8_t *)data;   // Save receive buffer usart
    usart_priv->rx_total_num = num;         // Save number of data to be received
    usart_priv->rx_cnt = 0;
    usart_priv->rx_busy = 1;

    return 0;

}

/**
  \brief       query data from UART receiver FIFO.
  \param[in]   handle  usart handle to operate.
  \param[out]  data  Pointer to buffer for data to receive from UART receiver
  \param[in]   num   Number of data items to receive
  \return      receive fifo data num
*/
int32_t csi_usart_receive_query(usart_handle_t handle, void *data, uint32_t num)
{
    USART_NULL_PARAM_CHK(handle);
    USART_NULL_PARAM_CHK(data);
    zx29_usart_priv_t *usart_priv = handle;
    int32_t recv_num = 0;
    uint8_t *dest = (uint8_t *)data;

    while (!(serial_getreg(usart_priv, zx29_UART_FR) & UART_FR_RXFE)) {
        *dest++ = serial_getreg(usart_priv, zx29_UART_DR) & 0xff;
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
int32_t csi_usart_abort_receive(usart_handle_t handle)
{
    USART_NULL_PARAM_CHK(handle);
    zx29_usart_priv_t *usart_priv = handle;

    usart_priv->rx_cnt = usart_priv->rx_total_num;
    return 0;
}

/**
  \brief       Start sending/receiving data to/from UART transmitter/receiver.
  \param[in]   handle  usart handle to operate.
  \param[in]   data_out  Pointer to buffer with data to send to USART transmitter
  \param[out]  data_in   Pointer to buffer for data to receive from USART receiver
  \param[in]   num       Number of data items to transfer
  \return      error code
*/
int32_t csi_usart_transfer(usart_handle_t handle, const void *data_out, void *data_in, uint32_t num)
{
    USART_NULL_PARAM_CHK(handle);
    return ERR_USART(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       abort sending/receiving data to/from USART transmitter/receiver.
  \param[in]   handle  usart handle to operate.
  \return      error code
*/
int32_t csi_usart_abort_transfer(usart_handle_t handle)
{
    USART_NULL_PARAM_CHK(handle);
    return ERR_USART(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       Get USART status.
  \param[in]   handle  usart handle to operate.
  \return      USART status \ref usart_status_t
*/
usart_status_t csi_usart_get_status(usart_handle_t handle)
{
    usart_status_t usart_status;
    zx29_usart_priv_t *usart_priv = handle;

    usart_status.tx_busy = usart_priv->tx_busy;
    usart_status.rx_busy = usart_priv->rx_busy;

    return usart_status;
}

/**
  \brief       control the transmit.
  \param[in]   handle  usart handle to operate.
  \param[in]   enable the transmitter.
  \return      error code
*/
int32_t csi_usart_control_tx(usart_handle_t handle, uint32_t enable)
{
    USART_NULL_PARAM_CHK(handle);
    return ERR_USART(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       control the receive.
  \param[in]   handle  usart handle to operate.
  \param[in]   enable the receive.
  \return      error code
*/
int32_t csi_usart_control_rx(usart_handle_t handle, uint32_t enable)
{
    USART_NULL_PARAM_CHK(handle);
    return ERR_USART(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       control the break.
  \param[in]   handle  usart handle to operate.
  \param[in]   enable the break.
  \return      error code
*/
int32_t csi_usart_control_break(usart_handle_t handle, uint32_t enable)
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
int32_t csi_usart_flush(usart_handle_t handle, usart_flush_type_e type)
{
    USART_NULL_PARAM_CHK(handle);

    return 0;
}

/**
  \brief       control interrupt on/off.
  \param[in]   handle usart handle to operate.
  \param[in]   type \ref usart_intr_type_e.
  \param[in]   flag 0-OFF, 1-ON.
  \return      error code
*/
int32_t csi_usart_interrupt_on_off(usart_handle_t handle, usart_intr_type_e type, int flag)
{
    return 0;
}

/**
  \brief       control usart power.
  \param[in]   handle  usart handle to operate.
  \param[in]   state   power state.\ref csi_power_stat_e.
  \return      error code
*/
int32_t csi_usart_power_control(usart_handle_t handle, csi_power_stat_e state)
{
    USART_NULL_PARAM_CHK(handle);
    return ERR_USART(DRV_ERROR_UNSUPPORTED);

}

/**
  \brief       config usart flow control type.
  \param[in]   handle  usart handle to operate.
  \param[in]   flowctrl_type   flow control type.\ref usart_flowctrl_type_e.
  \param[in]   tx_flow  The TX flow pin name
  \param[in]   rx_flow  The RX flow pin name
  \return      error code
*/
int32_t csi_usart_config_flowctrl(usart_handle_t handle,
                                  usart_flowctrl_type_e flowctrl_type)
{
    USART_NULL_PARAM_CHK(handle);
    return ERR_USART(DRV_ERROR_UNSUPPORTED);

}
/**
  \brief       usart modem control.
  \param[in]   handle  usart handle to operate.
  \param[in]   modem_ctrl   modem control action.\ref usart_modem_ctrl_e.
  \return      error code
*/
int32_t csi_usart_modem_ctrl(usart_handle_t handle, usart_modem_ctrl_e modem_ctrl)
{
    USART_NULL_PARAM_CHK(handle);
    return ERR_USART(DRV_ERROR_UNSUPPORTED);

}


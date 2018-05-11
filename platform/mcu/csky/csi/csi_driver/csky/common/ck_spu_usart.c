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
 * @file     ck_spu_usart.c
 * @brief    CSI Source File for SPU usart Driver
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "drv_spu_usart.h"
#include "ck_spu_usart.h"
#include "ck_spu.h"

//#include "soc.h"
#include "spu_pin_planning.h"

#define ERR_SPU_USART(errno) (CSI_DRV_ERRNO_USART_BASE  | errno)

#define USART_NULL_PARAM_CHK(para)                   \
    do {                                        \
        if (para == NULL) {                     \
            return ERR_SPU_USART(DRV_ERROR_PARAMETER);   \
        }                                       \
    } while (0)

extern uint32_t SPU_USART_PROG_BASE[];
extern int32_t spu_get_base(int32_t spu_idx);
extern int32_t spu_get_usart_func_index_base(int32_t spu_idx);
extern int32_t spu_get_usart_info(int32_t dev_idx, int32_t *spu_idx, uint8_t *pgpio_tx, uint8_t *pgpio_rx);

//==============================================================
//                   Data Private
//==============================================================
typedef struct {
    //hardware configuration
    int32_t spu_idx;
    uint8_t pgpio_tx;
    uint8_t pgpio_rx;
    uint32_t spu_base;
    uint32_t global_base;
    uint32_t pgpio_base;
    uint32_t inst_base;
    uint32_t prog_base_tx;
    uint32_t prog_base_rx;
    //call back
    usart_event_cb_t cb_event;
    //variable
    usart_mode_e mode;
    usart_data_bits_e data_bits;
    usart_parity_e parity;
    volatile usart_status_t status;
    volatile bool rcv_trigger;

    void *tx_data_buf;
    uint32_t tx_total_num;
    volatile uint32_t tx_cur_num;

    void *rx_data_buf;
    uint32_t rx_total_num;
    volatile uint32_t rx_cur_num;
} ck_spu_usart_priv_t;

static ck_spu_usart_priv_t usart_instance[CONFIG_SPU_USART_NUM];

static const usart_capabilities_t usart_capabilities = {
    .asynchronous       = 1,      ///< supports UART (Asynchronous) mode
    .synchronous_master = 0,      ///< supports Synchronous Master mode
    .synchronous_slave  = 0,      ///< supports Synchronous Slave mode
    .single_wire        = 1,      ///< supports UART Single-wire mode
    .irda               = 0,      ///< supports UART IrDA mode
    .smart_card         = 0,      ///< supports UART Smart Card mode
    .smart_card_clock   = 0,      ///< Smart Card Clock generator available
    .flow_control_rts   = 0,      ///< RTS Flow Control available
    .flow_control_cts   = 0,      ///< CTS Flow Control available
    .event_tx_complete  = 1,      ///< Transmit completed event: \ref USART_EVENT_TX_COMPLETE
    .event_rx_timeout   = 0,      ///< Signal receive character timeout event: \ref USART_EVENT_RX_TIMEOUT
    .rts                = 0,      ///< RTS Line: 0=not available, 1=available
    .cts                = 0,      ///< CTS Line: 0=not available, 1=available
    .dtr                = 0,      ///< DTR Line: 0=not available, 1=available
    .dsr                = 0,      ///< DSR Line: 0=not available, 1=available
    .dcd                = 0,      ///< DCD Line: 0=not available, 1=available
    .ri                 = 0,      ///< RI Line: 0=not available, 1=available
    .event_cts          = 0,      ///< Signal CTS change event: \ref USART_EVENT_CTS
    .event_dsr          = 0,      ///< Signal DSR change event: \ref USART_EVENT_DSR
    .event_dcd          = 0,      ///< Signal DCD change event: \ref USART_EVENT_DCD
    .event_ri           = 0,      ///< Signal RI change event: \ref USART_EVENT_RI
};

//==============================================================
//                    function
//==============================================================
static uint32_t get_unit_clk(uint32_t baud)
{
    uint32_t sample_freq;
    float cal;

    sample_freq = baud << 2;
    cal = SPU_CLK_FREQUENCY / (float)sample_freq;

    if ((int)cal < (int)(cal + (float)0.5)) {
        return (int)cal;
    } else {
        return (int)cal - 1;
    }
}

static void config_ctrl(usart_handle_t handle)
{
    ck_spu_usart_priv_t *priv = handle;
    uint32_t ctrl;

    getData(ctrl, priv->prog_base_tx, USART_OFFSET_TX_CTRL);
    ctrl &= 0x3;

    if (priv->parity == USART_PARITY_NONE) {
        ctrl |= (priv->data_bits + 4) << 2;
    } else {
        ctrl |= (priv->data_bits + 5) << 2;

        switch (priv->parity) {
            case USART_PARITY_EVEN :
                ctrl |= 0x80;
                break;

            case USART_PARITY_ODD  :
                ctrl |= 0x180;
                break;

            case USART_PARITY_1    :
                ctrl |= 0x600;
                break;

            case USART_PARITY_0    :
                ctrl |= 0x200;
                break;

            default                :
                break;
        }
    }

    putData(ctrl, priv->prog_base_tx, USART_OFFSET_TX_CTRL);
    putData(ctrl, priv->prog_base_rx, USART_OFFSET_RX_CTRL);
}

static void write_fifo(usart_handle_t handle, uint32_t len)
{
    ck_spu_usart_priv_t *priv = handle;

    //write PROG_RAM base to GLOBAL_REG firstly
    putData(priv->prog_base_tx - priv->spu_base, priv->global_base, OFFSET_FIFO_BADDR);

    //send data
    uint32_t offset_tx_fifo;
    uint32_t trans_data;

    priv->tx_cur_num += len;
    offset_tx_fifo = FIFO_BASE + (priv->pgpio_tx << 3);

    if (priv->data_bits > USART_DATA_BITS_8) {
        while (len > 0) {
            trans_data = *(volatile uint16_t *)(priv->tx_data_buf);
            putData(trans_data, priv->spu_base, offset_tx_fifo);
            priv->tx_data_buf = (uint16_t *)(priv->tx_data_buf) + 1;
            len--;
        }
    } else {
        while (len > 0) {
            trans_data = *(volatile uint8_t *)(priv->tx_data_buf);
            putData(trans_data, priv->spu_base, offset_tx_fifo);
            priv->tx_data_buf = (uint8_t *)(priv->tx_data_buf) + 1;
            len--;
        }
    }
}

static void read_fifo(usart_handle_t handle, uint32_t len)
{
    ck_spu_usart_priv_t *priv = handle;

    //write PROG_RAM base to GLOBAL_REG firstly
    putData(priv->prog_base_rx - priv->spu_base, priv->global_base, OFFSET_FIFO_BADDR);

    //receive data
    uint32_t offset_rx_fifo;
    uint32_t trans_data;
    uint32_t data_mask;

    priv->rx_cur_num += len;
    offset_rx_fifo = FIFO_BASE + (priv->pgpio_rx << 3) + 4;
    data_mask = (1 << (priv->data_bits + 5)) - 1;

    if (priv->data_bits > USART_DATA_BITS_8) {
        while (len > 0) {
            getData(trans_data, priv->spu_base, offset_rx_fifo);
            trans_data &= data_mask;
            *((volatile uint16_t *)(priv->rx_data_buf)) = (uint16_t)trans_data;
            priv->rx_data_buf = (uint16_t *)(priv->rx_data_buf) + 1;
            len--;
        }
    } else {
        while (len > 0) {
            getData(trans_data, priv->spu_base, offset_rx_fifo);
            trans_data &= data_mask;
            *((volatile uint8_t *)(priv->rx_data_buf)) = (uint8_t)trans_data;
            priv->rx_data_buf = (uint8_t *)(priv->rx_data_buf) + 1;
            len--;
        }
    }
}

void usart_receive_trigger(usart_handle_t handle)
{
    ck_spu_usart_priv_t *priv = handle;

    //single_wire
    uint32_t offset_pgpio;
    uint32_t data_pgpio;
    uint32_t offset_inst;
    uint32_t data_inst;

    if (priv->mode == USART_MODE_SINGLE_WIRE) {
        offset_pgpio = (priv->pgpio_tx << 4);
        putData(0x0, priv->pgpio_base, offset_pgpio);
        offset_pgpio += 4;
        putData(0x0, priv->pgpio_base, offset_pgpio);
        offset_pgpio += 4;
        getData(data_pgpio, priv->pgpio_base, offset_pgpio);
        data_pgpio &= 0xfff;
        putData(data_pgpio, priv->pgpio_base, offset_pgpio);

        offset_inst = (priv->pgpio_tx << 3);
        getData(data_inst, priv->inst_base, offset_inst);
        data_inst &= 0xFFFF0000;
        data_inst |= (priv->prog_base_rx - priv->spu_base);
        putData(data_inst, priv->inst_base, offset_inst);
    }

    //RX_FIFO_HEAD
    putData(USART_RX_FIFO_HEAD_TRIGGER, priv->prog_base_rx, USART_OFFSET_RX_FIFO_HEAD);

    //RX_FIFO_STATUS
    putData(0xff0000, priv->prog_base_rx, USART_OFFSET_RX_FIFO_STATUS);

    //INT
    putData(0, priv->prog_base_rx, USART_OFFSET_RX_INT);

    //TRANS LEN
    putData(0xffffffff, priv->prog_base_rx, USART_OFFSET_RX_LEN);

    //variable
    priv->status.rx_busy = 0;
    priv->status.rx_overflow = 0;
    priv->status.rx_break = 0;
    priv->status.rx_framing_error = 0;
    priv->status.rx_parity_error = 0;

    //start PE
    if (priv->mode == USART_MODE_SINGLE_WIRE) {
        offset_pgpio = (priv->pgpio_tx << 4) + 4;
    } else {
        offset_pgpio = (priv->pgpio_rx << 4) + 4;
    }

    putData(0x800, priv->pgpio_base, offset_pgpio);
    offset_pgpio += 4;
    getData(data_pgpio, priv->pgpio_base, offset_pgpio);
    data_pgpio |= 0x8000;
    putData(data_pgpio, priv->pgpio_base, offset_pgpio);
}

//==============================================================
//                           ISR
//==============================================================
/**
  \brief       the interrupt service function.
  \param[in]   index of usart instance.
*/
void ck_spu_usart_irqhandler(int32_t dev_idx, spu_dev_signal_e signal)
{
    ck_spu_usart_priv_t *priv = &usart_instance[dev_idx];
    spu_dev_signal_e int_signal;
    usart_event_e event = 0;
    bool event_vld = false;
    uint32_t int_status;
    uint32_t bytes_to_send;
    uint32_t trans_len;
    uint32_t bytes_to_rcv;

    if (priv->mode == USART_MODE_SINGLE_WIRE) {
        if (priv->status.tx_busy == 1) {
            int_signal = ENUM_USART_TX;
        } else {
            int_signal = ENUM_USART_RX;
        }
    } else {
        int_signal = signal;
    }

    if (int_signal == ENUM_USART_TX) {
        getData(int_status, priv->prog_base_tx, USART_OFFSET_TX_INT);

        if ((int_status & USART_INT_TX_DONE) != 0) {
            priv->status.tx_busy = 0;
            event_vld = true;
            event = USART_EVENT_SEND_COMPLETE;
        } else {
            getData(int_status, priv->prog_base_tx, USART_OFFSET_TX_FIFO_STATUS);

            if ((int_status & USART_INT_TX_UNDERFLOW) != 0) {
                priv->status.tx_underflow = 1;
                priv->status.tx_busy = 0;
                event_vld = true;
                event = USART_EVENT_TX_UNDERFLOW;
            } else if ((int_status & USART_INT_TX_ALEMPTY) != 0) {
                bytes_to_send = priv->tx_total_num - priv->tx_cur_num;

                if (bytes_to_send <= (USART_TX_FIFO_DEPTH - USART_TX_FIFO_TL + 1)) {
                    trans_len = bytes_to_send;
                    int_status |= 0xf0000; //close interrupt
                    putData(int_status, priv->prog_base_tx, USART_OFFSET_TX_FIFO_STATUS);
                } else {
                    trans_len = USART_TX_FIFO_DEPTH - USART_TX_FIFO_TL + 1;
                }

                write_fifo(priv, trans_len);
            }
        }
    }

    if (int_signal == ENUM_USART_RX) {
        getData(int_status, priv->prog_base_rx, USART_OFFSET_RX_INT);

        if ((int_status & USART_INT_RX_DONE) != 0) {
            bytes_to_rcv = priv->rx_total_num - priv->rx_cur_num;
            read_fifo(priv, bytes_to_rcv);
            priv->status.rx_busy = 0;
            event_vld = true;
            event = USART_EVENT_RECEIVE_COMPLETE;
            //receive trigger
            priv->rcv_trigger = true;
            usart_receive_trigger(priv);
        } else if ((int_status & USART_INT_STOP_ERR) != 0) {
            priv->status.rx_framing_error = 1;
            priv->status.rx_busy = 0;
            event_vld = true;
            event = USART_EVENT_RX_FRAMING_ERROR;
        } else if ((int_status & USART_INT_PARITY_ERR) != 0) {
            priv->status.rx_parity_error = 1;
            priv->status.rx_busy = 0;
            event_vld = true;
            event = USART_EVENT_RX_PARITY_ERROR;
        } else {
            getData(int_status, priv->prog_base_rx, USART_OFFSET_RX_FIFO_STATUS);

            if ((int_status & USART_INT_RX_OVERFLOW) != 0) {
                priv->status.rx_overflow = 1;
                priv->status.rx_busy = 0;
                event_vld = true;
                event = USART_EVENT_RX_OVERFLOW;
            } else if ((int_status & USART_INT_RX_ALFULL) != 0) {
                if (!priv->rcv_trigger) {
                    //read_fifo(priv,USART_RX_FIFO_TL+1);
                    read_fifo(priv, USART_RX_FIFO_TL);
                } else {
                    event_vld = true;
                    event = USART_EVENT_RECEIVED;
                    putData(0xffff0000, priv->prog_base_rx, USART_OFFSET_RX_FIFO_STATUS);
                }
            }
        }
    }

    if (priv->cb_event && event_vld) {
        priv->cb_event(dev_idx, event);
    }
}

//==============================================================
//                           DRIVER
//==============================================================
/**
  \brief       Initialize USART Interface. 1. Initializes the resources needed for the USART interface 2.registers event callback function
  \param[in]   tx usart pin of tx
  \param[in]   rx usart pin of rx
  \param[in]   cb_event  Pointer to \ref usart_event_cb_t
  \param[in]   cb_arg    argument for cb_event
  \return      return usart handle if success
*/
usart_handle_t drv_spu_usart_initialize(int32_t idx, usart_event_cb_t cb_event)
{
    //usart instance
    int32_t spu_idx;
    uint8_t pgpio_tx;
    uint8_t pgpio_rx;
    ck_spu_usart_priv_t *priv;

    int32_t ret = spu_get_usart_info(idx, &spu_idx, &pgpio_tx, &pgpio_rx);

    if (ret < 0 || ret >= CONFIG_SPU_USART_NUM) {
        return NULL;
    }

    priv = &usart_instance[idx];
    priv->spu_idx = spu_idx;
    priv->pgpio_tx = pgpio_tx;
    priv->pgpio_rx = pgpio_rx;

    //base address
    int32_t apply_prog;

    priv->spu_base = spu_get_base(spu_idx);
    priv->global_base = priv->spu_base + GLOBAL_BASE;
    priv->pgpio_base = priv->spu_base + PGPIO_BASE;
    priv->inst_base = priv->spu_base + INST_BASE;

    apply_prog = SPU_USART_PROG_BASE[idx];
    priv->prog_base_tx = priv->spu_base + PROG_BASE + apply_prog;
    priv->prog_base_rx = priv->prog_base_tx + USART_TX_PROG_BYTES;

    //inst_ram
    uint32_t func_index_base;
    uint32_t offset_inst;
    uint32_t data_inst;

    func_index_base = spu_get_usart_func_index_base(priv->spu_idx);

    offset_inst = (priv->pgpio_rx << 3);
    data_inst = (func_index_base << 16) | (PROG_BASE + apply_prog + USART_TX_PROG_BYTES);
    putData(data_inst, priv->inst_base, offset_inst);

    offset_inst = (priv->pgpio_rx << 3) + 4;
    putData(priv->pgpio_rx, priv->inst_base, offset_inst);

    offset_inst = (priv->pgpio_tx << 3);
    data_inst = (func_index_base << 16) | (PROG_BASE + apply_prog);
    putData(data_inst, priv->inst_base, offset_inst);

    offset_inst = (priv->pgpio_tx << 3) + 4;
    putData(priv->pgpio_tx, priv->inst_base, offset_inst);

    //register callback function
    priv->cb_event = cb_event;
    return priv;
}

/**
  \brief       De-initialize UART Interface. stops operation and releases the software resources used by the interface
  \param[in]   handle  usart handle to operate.
  \return      error code
*/
int32_t drv_spu_usart_uninitialize(usart_handle_t handle)
{
    USART_NULL_PARAM_CHK(handle);
    ck_spu_usart_priv_t *priv = handle;
    priv->cb_event   = NULL;
    return 0;
}

/**
  \brief       Get driver capabilities.
  \param[in]   handle  usart handle to operate.
  \return      \ref usart_capabilities_t
*/
usart_capabilities_t drv_spu_usart_get_capabilities(int32_t idx)
{
    return usart_capabilities;
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
int32_t drv_spu_usart_config(usart_handle_t handle,
                             uint32_t baud,
                             usart_mode_e mode,
                             usart_parity_e parity,
                             usart_stop_bits_e stopbits,
                             usart_data_bits_e bits)
{
    ck_spu_usart_priv_t *priv = handle;


    if (parity > USART_PARITY_0) {
        return ERR_SPU_USART(USART_ERROR_PARITY);
    }

    if (stopbits > USART_STOP_BITS_0_5) {
        return ERR_SPU_USART(USART_ERROR_STOP_BITS);
    }

    if (bits != 0 && bits != 1 && bits != 2 && bits != 3 && bits != 4) {
        return ERR_SPU_USART(USART_ERROR_DATA_BITS);
    }

    //mode
    /*
    if(mode==USART_MODE_SYNCHRONOUS_MASTER || mode==USART_MODE_SYNCHRONOUS_SLAVE){
        return ERR_SPU_USART(DRV_ERROR_UNSUPPORTED);
    }
    priv->mode = mode;
    */
    int32_t ret = drv_spu_usart_config_mode(handle, mode);

    if (ret != 0) {
        return ERR_SPU_USART(DRV_ERROR_UNSUPPORTED);
    }

    //baud
    /*
    uint32_t unit_clk;
    uint32_t offset_pgpio;
    uint32_t data_pgpio;

    unit_clk = get_unit_clk(baud);

    offset_pgpio = (priv->pgpio_rx << 4) + 4;
    putData(0x0,priv->pgpio_base,offset_pgpio);
    offset_pgpio += 4;
    putData(unit_clk,priv->pgpio_base,offset_pgpio);

    offset_pgpio = (priv->pgpio_tx << 4) + 4;
    putData(0x0,priv->pgpio_base,offset_pgpio);
    offset_pgpio += 4;
    data_pgpio = unit_clk | 0x22000;
    putData(data_pgpio,priv->pgpio_base,offset_pgpio);
    */
    drv_spu_usart_config_baudrate(handle, baud);

    //parity, stopbits, bits
    /*
    uint32_t ctrl;
    priv->data_bits = bits;
    priv->parity = parity;

    switch(stopbits){
      case USART_STOP_BITS_1:   ctrl = 1;break;
      case USART_STOP_BITS_1_5: ctrl = 2;break;
      case USART_STOP_BITS_2:   ctrl = 3;break;
      default:                  ctrl = 0;break;
    }

    if(parity==USART_PARITY_NONE){
        ctrl |= (bits+4) << 2;
    }
    else{
        ctrl |= (bits+5) << 2;
        switch(parity){
            case USART_PARITY_EVEN : ctrl |= 0x80; break;
            case USART_PARITY_ODD  : ctrl |= 0x180; break;
            case USART_PARITY_1    : ctrl |= 0x600; break;
            case USART_PARITY_0    : ctrl |= 0x200; break;
            default                : break;
        }
    }

    putData(ctrl,priv->prog_base_tx,USART_OFFSET_TX_CTRL);
    putData(ctrl,priv->prog_base_rx,USART_OFFSET_RX_CTRL);
    */
    priv->data_bits = bits;
    priv->parity = parity;
    config_ctrl(handle);

    //variable
    priv->status.tx_busy = 0;
    priv->status.rx_busy = 0;

    //receive trigger
    priv->rcv_trigger = true;
    usart_receive_trigger(handle);

    return 0;
}

/**
  \brief       Start sending data to UART transmitter,(received data is ignored).
               The function is non-blocking,UART_EVENT_TRANSFER_COMPLETE is signaled when transfer completes.
               drv_spu_usart_get_status can indicates if transmission is still in progress or pending
  \param[in]   handle  usart handle to operate.
  \param[in]   data  Pointer to buffer with data to send to UART transmitter. data_type is : uint8_t for 1..8 data bits, uint16_t for 9..16 data bits,uint32_t for 17..32 data bits,
  \param[in]   num   Number of data items to send
  \return      error code
*/
int32_t drv_spu_usart_send(usart_handle_t handle, const void *data, uint32_t num)
{
    USART_NULL_PARAM_CHK(handle);
    USART_NULL_PARAM_CHK(data);

    ck_spu_usart_priv_t *priv = handle;

    priv->tx_data_buf = (void *)data;
    priv->tx_total_num = num;
    priv->tx_cur_num = 0;

    //single_wire
    uint32_t offset_pgpio;
    uint32_t data_pgpio;
    uint32_t offset_inst;
    uint32_t data_inst;

    if (priv->mode == USART_MODE_SINGLE_WIRE) {
        offset_pgpio = (priv->pgpio_tx << 4) + 4;
        putData(0x0, priv->pgpio_base, offset_pgpio);
        offset_pgpio += 4;
        getData(data_pgpio, priv->pgpio_base, offset_pgpio);
        data_pgpio = data_pgpio | 0x22000;
        putData(data_pgpio, priv->pgpio_base, offset_pgpio);

        offset_inst = (priv->pgpio_tx << 3);
        getData(data_inst, priv->inst_base, offset_inst);
        data_inst &= 0xFFFF0000;
        data_inst |= (priv->prog_base_tx - priv->spu_base);
        putData(data_inst, priv->inst_base, offset_inst);
    }

    //TX_FIFO_HEAD
    putData(USART_TX_FIFO_HEAD, priv->prog_base_tx, USART_OFFSET_TX_FIFO_HEAD);

    //TX_FIFO_STATUS
    putData(0xffff0000, priv->prog_base_tx, USART_OFFSET_TX_FIFO_STATUS); //disable int firstly

    //INT
    putData(0, priv->prog_base_tx, USART_OFFSET_TX_INT);

    //TRANS LEN
    putData(num, priv->prog_base_tx, USART_OFFSET_TX_LEN);

    //write fifo
    if (num <= (USART_TX_FIFO_DEPTH + 1)) {
        write_fifo(priv, num);
    } else {
        write_fifo(priv, USART_TX_FIFO_DEPTH + 1);
        putData(0xff000000, priv->prog_base_tx, USART_OFFSET_TX_FIFO_STATUS); //enable int
    }

    //variable
    priv->status.tx_busy = 1;
    priv->status.tx_underflow = 0;

    //start PE
    offset_pgpio = (priv->pgpio_tx << 4) + 4;
    putData(0x3C00, priv->pgpio_base, offset_pgpio);
    offset_pgpio += 4;
    getData(data_pgpio, priv->pgpio_base, offset_pgpio);
    data_pgpio |= 0x8000;
    putData(data_pgpio, priv->pgpio_base, offset_pgpio);

    return 0;
}

/**
  \brief       Abort Send data to UART transmitter
  \param[in]   handle  usart handle to operate.
  \return      error code
*/
int32_t drv_spu_usart_abort_send(usart_handle_t handle)
{
    USART_NULL_PARAM_CHK(handle);
    ck_spu_usart_priv_t *priv = handle;

    uint32_t tx_len;

    getData(tx_len, priv->prog_base_tx, USART_OFFSET_TX_LEN);

    if (tx_len > 1) {
        putData(1, priv->prog_base_tx, USART_OFFSET_TX_LEN);
    }

    while (priv->status.tx_busy);

    return 0;
}

/**
  \brief       Start receiving data from USART receiver. \n
               This function is non-blocking,\ref usart_event_e is signaled when operation completes or error happens.
               \ref drv_spu_usart_get_status can get operation status.
  \param[in]   handle  usart handle to operate.
  \param[out]  data  Pointer to buffer for data to receive from USART receiver.data_type is : uint8_t for 5..8 data bits, uint16_t for 9 data bits
  \param[in]   num   Number of data items to receive
  \return      error code
*/
int32_t drv_spu_usart_receive(usart_handle_t handle, void *data, uint32_t num)
{
    USART_NULL_PARAM_CHK(handle);
    USART_NULL_PARAM_CHK(handle);
    ck_spu_usart_priv_t *priv = handle;

    priv->rx_data_buf = data;
    priv->rx_total_num = num;
    priv->rx_cur_num = 0;

    //single_wire
    uint32_t offset_pgpio;
    uint32_t data_pgpio;
    uint32_t offset_inst;
    uint32_t data_inst;

    if (priv->mode == USART_MODE_SINGLE_WIRE) {
        offset_pgpio = (priv->pgpio_tx << 4);
        putData(0x0, priv->pgpio_base, offset_pgpio);
        offset_pgpio += 4;
        putData(0x0, priv->pgpio_base, offset_pgpio);
        offset_pgpio += 4;
        getData(data_pgpio, priv->pgpio_base, offset_pgpio);
        data_pgpio &= 0xfff;
        putData(data_pgpio, priv->pgpio_base, offset_pgpio);

        offset_inst = (priv->pgpio_tx << 3);
        getData(data_inst, priv->inst_base, offset_inst);
        data_inst &= 0xFFFF0000;
        data_inst |= (priv->prog_base_rx - priv->spu_base);
        putData(data_inst, priv->inst_base, offset_inst);
    }

    //RX_FIFO_HEAD
    putData(USART_RX_FIFO_HEAD, priv->prog_base_rx, USART_OFFSET_RX_FIFO_HEAD);

    //RX_FIFO_STATUS
    if (num <= (USART_RX_FIFO_DEPTH + 1)) {
        putData(0xffff0000, priv->prog_base_rx, USART_OFFSET_RX_FIFO_STATUS);
    } else {
        putData(0xff0000, priv->prog_base_rx, USART_OFFSET_RX_FIFO_STATUS);
    }

    //INT
    putData(0, priv->prog_base_rx, USART_OFFSET_RX_INT);

    //TRANS LEN
    putData(num, priv->prog_base_rx, USART_OFFSET_RX_LEN);

    //variable
    priv->status.rx_busy = 1;
    priv->status.rx_overflow = 0;
    priv->status.rx_break = 0;
    priv->status.rx_framing_error = 0;
    priv->status.rx_parity_error = 0;
    priv->rcv_trigger = false;

    //start PE
    if (priv->mode == USART_MODE_SINGLE_WIRE) {
        offset_pgpio = (priv->pgpio_tx << 4) + 4;
    } else {
        offset_pgpio = (priv->pgpio_rx << 4) + 4;
    }

    putData(0x800, priv->pgpio_base, offset_pgpio);
    offset_pgpio += 4;
    getData(data_pgpio, priv->pgpio_base, offset_pgpio);
    data_pgpio |= 0x8000;
    putData(data_pgpio, priv->pgpio_base, offset_pgpio);

    return 0;
}

/**
  \brief       query data from UART receiver FIFO.
  \param[in]   handle  usart handle to operate.
  \param[out]  data  Pointer to buffer for data to receive from UART receiver
  \param[in]   num   Number of data items to receive
  \return      receive fifo data num
*/
int32_t drv_spu_usart_receive_query(usart_handle_t handle, void *data, uint32_t num)
{
    uint32_t rx_fifo_head;
    uint32_t trans_len;
    ck_spu_usart_priv_t *priv = handle;

    priv->rx_data_buf = data;
    priv->rx_cur_num = 0;

    //do{
    getData(rx_fifo_head, priv->prog_base_rx, USART_OFFSET_RX_FIFO_HEAD);
    trans_len = (rx_fifo_head & 0x3f) - priv->rx_cur_num;

    if (trans_len > num) {
        trans_len = num;
    }

    if (trans_len != 0) {
        read_fifo(handle, trans_len);
    }

    //} while(priv->rx_cur_num!=num);

    putData(0xffffffff, priv->prog_base_rx, USART_OFFSET_RX_LEN);
    putData(0xff0000, priv->prog_base_rx, USART_OFFSET_RX_FIFO_STATUS);

    return num;
}

/**
  \brief       Abort Receive data from UART receiver
  \param[in]   handle  usart handle to operate.
  \return      error code
*/
int32_t drv_spu_usart_abort_receive(usart_handle_t handle)
{
    USART_NULL_PARAM_CHK(handle);
    ck_spu_usart_priv_t *priv = handle;

    uint32_t rx_len;

    getData(rx_len, priv->prog_base_rx, USART_OFFSET_RX_LEN);

    if (rx_len > 1) {
        putData(1, priv->prog_base_rx, USART_OFFSET_RX_LEN);
    }

    while (priv->status.rx_busy);

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
int32_t drv_spu_usart_transfer(usart_handle_t handle, const void *data_out, void *data_in, uint32_t num)
{
    return ERR_SPU_USART(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       abort sending/receiving data to/from USART transmitter/receiver.
  \param[in]   handle  usart handle to operate.
  \return      error code
*/
int32_t drv_spu_usart_abort_transfer(usart_handle_t handle)
{
    return ERR_SPU_USART(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       Get USART status.
  \param[in]   handle  usart handle to operate.
  \return      USART status \ref usart_status_t
*/
usart_status_t drv_spu_usart_get_status(usart_handle_t handle)
{
    ck_spu_usart_priv_t *priv = handle;
    return priv->status;
}

/**
  \brief       flush receive/send data.
  \param[in]   handle usart handle to operate.
  \param[in]   type \ref usart_flush_type_e.
  \return      error code
*/
int32_t drv_spu_usart_flush(usart_handle_t handle, usart_flush_type_e type)
{
    return ERR_SPU_USART(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       set interrupt mode.
  \param[in]   handle usart handle to operate.
  \param[in]   type \ref usart_intr_type_e.
  \param[in]   flag 0-OFF, 1-ON.
  \return      error code
*/
int32_t drv_spu_usart_set_interrupt(usart_handle_t handle, usart_intr_type_e type, int32_t flag)
{
    return ERR_SPU_USART(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       set the baud rate of usart.
  \param[in]   baud  usart base to operate.
  \param[in]   baudrate baud rate
  \return      error code
*/
int32_t drv_spu_usart_config_baudrate(usart_handle_t handle, uint32_t baud)
{
    ck_spu_usart_priv_t *priv = handle;
    uint32_t unit_clk;
    uint32_t offset_pgpio;
    uint32_t data_pgpio;

    unit_clk = get_unit_clk(baud);

    offset_pgpio = (priv->pgpio_rx << 4) + 4;
    putData(0x0, priv->pgpio_base, offset_pgpio);
    offset_pgpio += 4;
    putData(unit_clk, priv->pgpio_base, offset_pgpio);

    offset_pgpio = (priv->pgpio_tx << 4) + 4;
    putData(0x0, priv->pgpio_base, offset_pgpio);
    offset_pgpio += 4;
    data_pgpio = unit_clk | 0x22000;
    putData(data_pgpio, priv->pgpio_base, offset_pgpio);

    return 0;
}

/**
  \brief       config usart mode.
  \param[in]   handle  usart handle to operate.
  \param[in]   mode    \ref usart_mode_e
  \return      error code
*/
int32_t drv_spu_usart_config_mode(usart_handle_t handle, usart_mode_e mode)
{
    ck_spu_usart_priv_t *priv = handle;

    if (mode == USART_MODE_SYNCHRONOUS_MASTER || mode == USART_MODE_SYNCHRONOUS_SLAVE) {
        return ERR_SPU_USART(DRV_ERROR_UNSUPPORTED);
    }

    priv->mode = mode;

    return 0;
}

/**
  \brief       config usart parity.
  \param[in]   handle  usart handle to operate.
  \param[in]   parity    \ref usart_parity_e
  \return      error code
*/
int32_t drv_spu_usart_config_parity(usart_handle_t handle, usart_parity_e parity)
{
    ck_spu_usart_priv_t *priv = handle;
    priv->parity = parity;
    config_ctrl(handle);

    return 0;
}

/**
  \brief       config usart stop bit number.
  \param[in]   handle  usart handle to operate.
  \param[in]   stopbits  \ref usart_stop_bits_e
  \return      error code
*/
int32_t drv_spu_usart_config_stopbits(usart_handle_t handle, usart_stop_bits_e stopbit)
{
    ck_spu_usart_priv_t *priv = handle;
    uint32_t ctrl;

    getData(ctrl, priv->prog_base_tx, USART_OFFSET_TX_CTRL);
    ctrl &= 0xfffffffc;

    switch (stopbit) {
        case USART_STOP_BITS_1:
            ctrl |= 1;
            break;

        case USART_STOP_BITS_1_5:
            ctrl |= 2;
            break;

        case USART_STOP_BITS_2:
            ctrl |= 3;
            break;

        default:
            break;
    }

    putData(ctrl, priv->prog_base_tx, USART_OFFSET_TX_CTRL);
    putData(ctrl, priv->prog_base_rx, USART_OFFSET_RX_CTRL);

    return 0;
}

/**
  \brief       config usart data length.
  \param[in]   handle  usart handle to operate.
  \param[in]   databits      \ref usart_data_bits_e
  \return      error code
*/
int32_t drv_spu_usart_config_databits(usart_handle_t handle, usart_data_bits_e databits)
{
    ck_spu_usart_priv_t *priv = handle;
    priv->data_bits = databits;
    config_ctrl(handle);

    return 0;
}

/**
  \brief       get character in query mode.
  \param[in]   handle  usart handle to operate.
  \param[out]  ch  pointer to the received character.
  \return      error code
*/
int32_t drv_spu_usart_getchar(usart_handle_t handle, uint8_t *ch)
{
    ck_spu_usart_priv_t *priv = handle;

    while (priv->status.rx_busy);

    drv_spu_usart_receive(priv, ch, 1);

    while (priv->status.rx_busy);

    return 0;
}


/**
  \brief       transmit character in query mode.
  \param[in]   handle  usart handle to operate.
  \param[in]   ch  the input character
  \return      error code
*/
int32_t drv_spu_usart_putchar(usart_handle_t handle, uint8_t ch)
{
    ck_spu_usart_priv_t *priv = handle;
    char send_char;

    while (priv->status.tx_busy);

    if (ch == '\n') {
        send_char = '\r';
        drv_spu_usart_send(handle, &send_char, 1);

        while (priv->status.tx_busy);
    }

    send_char = ch;
    drv_spu_usart_send(handle, &send_char, 1);

    while (priv->status.tx_busy);

    return 0;
}

/**
  \brief       Get usart send data count.
  \param[in]   handle  usart handle to operate.
  \return      number of currently transmitted data bytes
*/
uint32_t drv_spu_usart_get_tx_count(usart_handle_t handle)
{
    ck_spu_usart_priv_t *priv = handle;

    uint32_t tx_len;

    getData(tx_len, priv->prog_base_tx, USART_OFFSET_TX_LEN);
    return (priv->tx_total_num - tx_len);
}

/**
  \brief       Get usart received data count.
  \param[in]   handle  usart handle to operate.
  \return      number of currently received data bytes
*/
uint32_t drv_spu_usart_get_rx_count(usart_handle_t handle)
{
    ck_spu_usart_priv_t *priv = handle;

    uint32_t rx_len;

    getData(rx_len, priv->prog_base_rx, USART_OFFSET_RX_LEN);
    return (priv->rx_total_num - rx_len);
}

/**
  \brief       control usart power.
  \param[in]   handle  usart handle to operate.
  \param[in]   state   power state.\ref drv_spu_power_stat_e.
  \return      error code
*/
int32_t drv_spu_usart_power_control(usart_handle_t handle, csi_power_stat_e state)
{
    return ERR_SPU_USART(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       config usart flow control type.
  \param[in]   handle  usart handle to operate.
  \param[in]   flowctrl_type   flow control type.\ref usart_flowctrl_type_e.
  \return      error code
*/
int32_t drv_spu_usart_config_flowctrl(usart_handle_t handle,
                                      usart_flowctrl_type_e flowctrl_type)
{
    return ERR_SPU_USART(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       config usart clock Polarity and Phase.
  \param[in]   handle  usart handle to operate.
  \param[in]   cpol    Clock Polarity.\ref usart_cpol_e.
  \param[in]   cpha    Clock Phase.\ref usart_cpha_e.
  \return      error code
*/
int32_t drv_spu_usart_config_clock(usart_handle_t handle, usart_cpol_e cpol, usart_cpha_e cpha)
{
    return ERR_SPU_USART(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       control the transmitter.
  \param[in]   handle  usart handle to operate.
  \param[in]   enable  1 - enable the transmitter. 0 - disable the transmitter
  \return      error code
*/
int32_t drv_spu_usart_control_tx(usart_handle_t handle, uint32_t enable)
{
    return ERR_SPU_USART(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       control the receiver.
  \param[in]   handle  usart handle to operate.
  \param[in]   enable  1 - enable the receiver. 0 - disable the receiver
  \return      error code
*/
int32_t drv_spu_usart_control_rx(usart_handle_t handle, uint32_t enable)
{
    return ERR_SPU_USART(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       control the break.
  \param[in]   handle  usart handle to operate.
  \param[in]   enable  1- Enable continuous Break transmission,0 - disable continuous Break transmission
  \return      error code
*/
int32_t drv_spu_usart_control_break(usart_handle_t handle, uint32_t enable)
{
    return ERR_SPU_USART(DRV_ERROR_UNSUPPORTED);
}


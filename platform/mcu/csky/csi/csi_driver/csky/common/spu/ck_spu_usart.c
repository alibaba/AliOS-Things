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
 * @brief    CSI Source File for usart Driver
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/

#include <csi_config.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "csi_core.h"
#include "drv_usart.h"
#include "ck_spu_usart.h"
#include "ck_spu.h"
#include "soc.h"
#include "pin_name.h"
#include "spu_pin_planning.h"

#define ERR_USART(errno) (CSI_DRV_ERRNO_USART_BASE | errno)

extern uint32_t spu_get_base(int32_t idx);
extern void spu_enable_clock(int32_t idx,int32_t pgpio);
extern void spu_disable_clock(int32_t idx,int32_t pgpio);
extern int32_t spu_apply_prog(int32_t idx,int32_t bytes);
extern int32_t spu_get_usart_func_index_base(int32_t idx);
extern int32_t get_usart_info(int32_t int32_tx, int32_t pin_rx, int32_t *spu_idx, int32_t *pgpio_tx, int32_t *pgpio_rx);

//-------------------------------------------------
//                   Data Private
//-------------------------------------------------
typedef struct{
    //hardware config
    uint32_t spu_base;
    uint32_t spu_irq;
    uint32_t global_base;
    uint32_t pgpio_base;
    uint32_t prog_base_tx;
    uint32_t prog_base_rx;
    uint32_t inst_base;
    uint8_t sin_id;
    uint8_t sout_id;
    uint32_t spu_idx;

    //call back
    usart_event_cb_t cb_event;

    //mode
    usart_mode_e mode;
    usart_data_bits_e data_bits;

    //variable
    volatile bool tx_busy;
    volatile bool rx_busy;

    void *tx_data_buf;
    uint32_t tx_total_num;
    volatile uint32_t tx_cur_num;

    void *rx_data_buf;
    uint32_t rx_total_num;
    volatile uint32_t rx_cur_num;
} ck_spu_usart_priv_t;

static ck_spu_usart_priv_t usart_instance[CONFIG_SPU_USART_NUM];

static const usart_capabilities_t usart_capabilities = {
    .asynchronous = 1,          /* supports USART (Asynchronous) mode */
    .synchronous_master = 0,    /* supports Synchronous Master mode */
    .synchronous_slave = 0,     /* supports Synchronous Slave mode */
    .single_wire = 0,           /* supports USART Single-wire mode */
    .event_tx_complete = 1,     /* Transmit completed event */
    .event_rx_timeout = 0,      /* Signal receive character timeout event */
};

//-------------------------------------------------
//                    function
//-------------------------------------------------
static void enable_pgpio(usart_handle_t handle)
{
    ck_spu_usart_priv_t *priv = handle;
    uint32_t data;

    getData(data,priv->global_base,OFFSET_PGPIO_CLK_EN);
    data |= (1 << priv->sout_id);
    data |= (1 << priv->sin_id);
    putData(data,priv->global_base,OFFSET_PGPIO_CLK_EN);
}

static void disable_pgpio(usart_handle_t handle)
{
    ck_spu_usart_priv_t *priv = handle;
    uint32_t data;

    getData(data,priv->global_base,OFFSET_PGPIO_CLK_EN);
    data &= ~(1 << priv->sout_id);
    data &= ~(1 << priv->sin_id);
    putData(data,priv->global_base,OFFSET_PGPIO_CLK_EN);
}

static uint32_t get_clock_div(uint32_t sysclk,uint32_t baud)
{
    uint32_t clock_freq;
    clock_freq = baud << 4;
    return (sysclk / clock_freq);
}

static void write_tx_fifo(usart_handle_t handle, uint32_t value)
{
    ck_spu_usart_priv_t *priv = handle;
    uint32_t fifo_head_w0;
    uint32_t fifo_head_w1;
    uint32_t fifo_depth;
    uint32_t wr_offset;
    uint32_t mem_addr;
    uint32_t wr_pointer;

    //write fifo
    getData(fifo_head_w0,priv->prog_base_tx,USART_OFFSET_FIFO_HEAD_W0);
    getData(fifo_head_w1,priv->prog_base_tx,USART_OFFSET_FIFO_HEAD_W1);
    fifo_depth = fifo_head_w0 >> 24;
    wr_offset = fifo_head_w1 & fifo_depth;
    mem_addr = fifo_head_w1 >> 16;
    wr_pointer = ((mem_addr + wr_offset)<< 2) + priv->spu_base;

    putData(value,wr_pointer,0);

    //increase fifo pointer
    uint32_t head_hf_h;
    uint32_t head_hf_l;

    head_hf_h = fifo_head_w1 & 0xFFFF0000;
    fifo_head_w1++;
    head_hf_l = fifo_head_w1 & 0xFFFF;
    fifo_head_w1 = head_hf_h | head_hf_l;
    putData(fifo_head_w1,priv->prog_base_tx,USART_OFFSET_FIFO_HEAD_W1);
}

static void send_data(usart_handle_t handle)
{
    ck_spu_usart_priv_t *priv = handle;
    uint32_t fifo_head_w0;
    uint32_t fifo_head_w1;
    uint32_t head_sub;
    uint32_t fifo_depth;
    uint32_t mask;
    uint32_t valid_entries;

    //check tx_fifo valid entry
    getData(fifo_head_w0,priv->prog_base_tx,USART_OFFSET_FIFO_HEAD_W0);
    getData(fifo_head_w1,priv->prog_base_tx,USART_OFFSET_FIFO_HEAD_W1);
    head_sub = fifo_head_w1 - fifo_head_w0;
    fifo_depth =  fifo_head_w0 >> 24;
    mask = (fifo_depth + 1) | fifo_depth;
    valid_entries = (fifo_depth + 1) - (head_sub & mask);

    //send data
    uint32_t bytes_to_send;
    uint32_t trans_len;
    uint32_t intr;
    uint32_t trans_data;

    bytes_to_send = priv->tx_total_num - priv->tx_cur_num;

    if(bytes_to_send <= valid_entries){
        trans_len = bytes_to_send;
        //close interrupt
        getData(intr,priv->prog_base_tx,USART_OFFSET_INT);
        intr |= (USART_INT_TX_EMPTY << 16);
        putData(intr,priv->prog_base_tx,USART_OFFSET_INT);
    }
    else{
        trans_len = valid_entries;
    }

    priv->tx_cur_num += trans_len;

    uint32_t data_mask;
    data_mask = (1<<(priv->data_bits+5))-1;

    if(priv->data_bits>USART_DATA_BITS_8){
        while(trans_len>0){
            trans_data = *(volatile uint16_t*)(priv->tx_data_buf);
			trans_data &= data_mask;
            write_tx_fifo(priv,trans_data);
            priv->tx_data_buf = (uint16_t*)(priv->tx_data_buf) + 1;
            trans_len--;
        }
    }
    else{
        while(trans_len>0){
            trans_data = *(volatile uint8_t*)(priv->tx_data_buf);
			trans_data &= data_mask;
            write_tx_fifo(priv,trans_data);
            priv->tx_data_buf = (uint8_t*)(priv->tx_data_buf) + 1;
            trans_len--;
        }
    }
/*
    trans_data = *(priv->tx_data_buf);
    trans_data &= data_mask;
    write_tx_fifo(priv,trans_data);
    priv->tx_data_buf++;
    trans_len--;
*/
}

static uint32_t read_rx_fifo(usart_handle_t handle)
{
    ck_spu_usart_priv_t *priv = handle;
    uint32_t prog_base;
    uint32_t fifo_head_w0;
    uint32_t fifo_head_w1;
    uint32_t fifo_depth;
    uint32_t rd_offset;
    uint32_t mem_addr;
    uint32_t rd_pointer;
    uint32_t rd_data;

    if(priv->mode==USART_MODE_SINGLE_WIRE){
        prog_base = priv->prog_base_tx;
    }
    else{
        prog_base = priv->prog_base_rx;
    }

    //read fifo
    getData(fifo_head_w0,prog_base,USART_OFFSET_FIFO_HEAD_W0);
    getData(fifo_head_w1,prog_base,USART_OFFSET_FIFO_HEAD_W1);
    fifo_depth = fifo_head_w0 >> 24;
    rd_offset = fifo_head_w0 & fifo_depth;
    mem_addr = fifo_head_w1 >> 16;
    rd_pointer = ((mem_addr + rd_offset) << 2) + priv->spu_base;

    getData(rd_data,rd_pointer,0);

    //increase fifo pointer
    uint32_t head_hf_h;
    uint32_t head_hf_l;

    head_hf_h = fifo_head_w0 & 0xFFFF0000;
    fifo_head_w0++;
    head_hf_l = fifo_head_w0 & 0xFFFF;
    fifo_head_w0 = head_hf_h | head_hf_l;
    putData(fifo_head_w0,prog_base,USART_OFFSET_FIFO_HEAD_W0);

    return rd_data;
}

static void receive_data(usart_handle_t handle)
{
    ck_spu_usart_priv_t *priv = handle;
    uint32_t prog_base;
    uint32_t fifo_head_w0;
    uint32_t fifo_head_w1;
    uint32_t head_sub;
    uint32_t fifo_depth;
    uint32_t mask;
    uint32_t valid_entries;

    if(priv->mode==USART_MODE_SINGLE_WIRE){
        prog_base = priv->prog_base_tx;
    }
    else{
        prog_base = priv->prog_base_rx;
    }

    //check rx_fifo valid entry
    getData(fifo_head_w0,prog_base,USART_OFFSET_FIFO_HEAD_W0);
    getData(fifo_head_w1,prog_base,USART_OFFSET_FIFO_HEAD_W1);
    head_sub = fifo_head_w1 - fifo_head_w0;
    fifo_depth =  fifo_head_w0 >> 24;
    mask = (fifo_depth + 1) | fifo_depth;
    valid_entries = head_sub & mask;

    //receive data
    uint32_t trans_data;
    priv->rx_cur_num += valid_entries;

    if(priv->data_bits>USART_DATA_BITS_8){
        while(valid_entries > 0){
            trans_data = read_rx_fifo(priv);
            *((volatile uint16_t *)(priv->rx_data_buf)) = (uint16_t)trans_data;
            priv->rx_data_buf = (uint16_t*)(priv->rx_data_buf) + 1;
            valid_entries--;
        }
    }
    else{
        while(valid_entries > 0){
            trans_data = read_rx_fifo(priv);
            *((volatile uint8_t *)(priv->rx_data_buf)) = (uint8_t)trans_data;
            priv->rx_data_buf = (uint8_t*)(priv->rx_data_buf) + 1;
            valid_entries--;
        }
    }
}

/**
  \brief       get character in query mode.
  \param[in]   instance  usart instance to operate.
  \param[in]   the pointer to the recieve charater.
  \return      error code
*/
int32_t csi_usart_getchar(usart_handle_t handle, uint8_t *ch)
{
    ck_spu_usart_priv_t *priv = handle;

    while(priv->rx_busy);
    csi_usart_receive(priv, ch, 1);
    while(priv->rx_busy);

    return 0;
}

/**
  \brief       transmit character in query mode.
  \param[in]   instance  usart instance to operate.
  \param[in]   ch  the input charater
  \return      error code
*/
int32_t csi_usart_putchar(usart_handle_t handle, uint8_t ch)
{
    ck_spu_usart_priv_t *priv = handle;
    char send_char;

    while(priv->tx_busy);

    if(ch=='\n'){
        send_char='\r';
        csi_usart_send(handle,&send_char,1);
        while(priv->tx_busy);
    }

    send_char=ch;
    csi_usart_send(handle,&send_char,1);
    while(priv->tx_busy);

    return 0;
}

//==============================================================
//                           ISR
//==============================================================
/**
  \brief       the interrupt service function.
  \param[in]   index of usart instance.
*/
void ck_spu_usart_irqhandler(int32_t idx,spu_dev_signal_e signal)
{
    ck_spu_usart_priv_t *priv = &usart_instance[idx];

    //get interrupt status
    uint32_t prog_base;
    uint32_t int_status;

    if(signal==ENUM_USART_TX){
        prog_base = priv->prog_base_tx;
    }
    else{
        prog_base = priv->prog_base_rx;
    }

    getData(int_status,prog_base,USART_OFFSET_INT);

    //deal with interrupt
    if((int_status & USART_INT_TX_DONE)!=0){
        priv->tx_busy = false;
        if(priv->cb_event){
          priv->cb_event(USART_EVENT_SEND_COMPLETE, idx);
        }
    }
    else if((int_status & USART_INT_TX_EMPTY)!=0){
        send_data(priv);
    }

    if((int_status & USART_INT_RX_DONE)!=0){
        receive_data(priv);
        priv->rx_busy = false;
        if(priv->cb_event){
          priv->cb_event(USART_EVENT_RECEIVE_COMPLETE, idx);
        }
    }
    else if((int_status & USART_INT_RX_FULL)!=0){
        receive_data(priv);
    }

    if((int_status & USART_INT_PARITY_ERR)!=0){
        if(priv->cb_event){
          priv->cb_event(USART_EVENT_RX_PARITY_ERROR, idx);
        }
    }
    if((int_status & USART_INT_STOP_ERR)!=0){
        if(priv->cb_event){
          priv->cb_event(USART_EVENT_RX_FRAMING_ERROR,idx);
        }
    }

    int_status &= 0xFFFF0000;
    putData(int_status,prog_base,USART_OFFSET_INT);
}

//==============================================================
//                           DRIVER
//==============================================================
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
usart_handle_t csi_usart_initialize(int32_t idx, usart_event_cb_t cb_event
{
    //initialize instace
    int32_t spu_idx;
    int32_t pgpio_tx;
    int32_t pgpio_rx;
    int32_t usart_idx;
    ck_spu_usart_priv_t *priv;
    int32_t apply_prog;

    usart_idx = get_usart_info(idx, &spu_idx, &pgpio_tx, &pgpio_rx);
    if(usart_idx<0){
      return NULL;
    }

    priv = &usart_instance[usart_idx];

    priv->spu_base = spu_get_base(spu_idx);
    priv->global_base = priv->spu_base + GLOBAL_BASE;
    priv->pgpio_base = priv->spu_base + PGPIO_BASE;
    priv->inst_base = priv->spu_base + INST_BASE;
    priv->spu_idx = spu_idx;
    priv->sout_id = pgpio_tx;
    priv->sin_id = pgpio_rx;

    apply_prog = spu_apply_prog(priv->spu_idx,USART_PROG_BYTES);
    if(apply_prog==-1){
        return NULL;
    }else{
        priv->prog_base_tx = priv->spu_base + PROG_BASE + apply_prog;
        priv->prog_base_rx = priv->prog_base_tx + USART_PROG_BYTES;
    }


    //initialize USART bus
    uint32_t offset_pgpio;

    enable_pgpio(priv);

    offset_pgpio = (priv->sout_id << 4) + 8;
    putData(0xE,priv->pgpio_base,offset_pgpio);  //SDOUT: CTRL

    if(priv->sout_id!=priv->sin_id){
        offset_pgpio = (priv->sin_id << 4) + 8;
        putData(0x0,priv->pgpio_base,offset_pgpio);  //SIN: CTRL
    }

    //inst_ram: index
    spu_enable_clock(priv->spu_idx,priv->sout_id);

    uint32_t func_index_base;
    uint32_t offset_inst;
    uint32_t data_inst;

    func_index_base = spu_get_usart_func_index_base(priv->spu_idx);

    offset_inst = (priv->sout_id << 3);
    data_inst = (func_index_base<<16) | (priv->prog_base_tx-priv->spu_base);
    putData(data_inst,priv->inst_base,offset_inst);

    offset_inst = (priv->sout_id << 3) + 4;
    putData(priv->sout_id,priv->inst_base,offset_inst);

    offset_inst = (priv->sin_id << 3);
    data_inst = (func_index_base<<16) | (priv->prog_base_rx-priv->spu_base);
    putData(data_inst,priv->inst_base,offset_inst);

    offset_inst = (priv->sin_id << 3) + 4;
    putData(priv->sin_id,priv->inst_base,offset_inst);

    //variable
    priv->tx_busy = false;
    priv->rx_busy = false;

    //register callback function
    priv->cb_event = cb_event;

    return priv;
}

/**
  \brief       De-initialize UART Interface. stops operation and releases the software resources used by the interface
  \param[in]   handle  usart handle to operate.
  \return      error code
*/
int32_t csi_usart_uninitialize(usart_handle_t handle)
{
    ck_spu_usart_priv_t *priv = handle;
    disable_pgpio(priv);
    spu_disable_clock(priv->spu_idx,priv->sout_id);
    priv->cb_event   = NULL;

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
int32_t csi_usart_config(usart_handle_t handle,
                         uint32_t baud,
                         usart_mode_e mode,
                         usart_parity_e parity,
                         usart_stop_bits_e stopbits,
                         usart_data_bits_e bits)
{
    ck_spu_usart_priv_t *priv = handle;

    //mode
    priv->mode = mode;

    //baud
    uint32_t offset_pgpio;
    uint32_t clk_div;

    offset_pgpio = (priv->sout_id << 4) + 4;
    clk_div = get_clock_div(baud);
    clk_div |= (USART_FILTER_COE << 23);

    putData(clk_div,priv->pgpio_base,offset_pgpio);

    if(mode==USART_MODE_ASYNCHRONOUS){
        offset_pgpio = (priv->sin_id << 4) + 4;
        putData(clk_div,priv->pgpio_base,offset_pgpio);
    }

    //CTRL
    uint32_t ctrl;
    uint32_t chg_stopbits;

    priv->data_bits = bits;
    ctrl = (bits + 5) << 2;  //data_bits

    ctrl |= (1 << parity)<<8;  //parity
    switch(stopbits){
      case USART_STOP_BITS_0_5: chg_stopbits = 0;
      case USART_STOP_BITS_1: chg_stopbits = 1;
      case USART_STOP_BITS_1_5: chg_stopbits = 2;
      case USART_STOP_BITS_2: chg_stopbits = 3;
    }
    ctrl |= chg_stopbits<<18;  //stop bits

    putData(ctrl,priv->prog_base_tx,USART_OFFSET_CTRL);

    if(mode==USART_MODE_ASYNCHRONOUS){
        ctrl |= 0x2000; //set receive_flag
        putData(ctrl,priv->prog_base_rx,USART_OFFSET_CTRL);
    }


    return 0;
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
    ck_spu_usart_priv_t *priv = handle;

    priv->tx_data_buf = (void *)data;
    priv->tx_total_num = num;
    priv->tx_cur_num = 0;
/*
    if(priv->data_bits>USART_DATA_BITS_8){
        priv->tx_data_buf = (volatile uint16_t*)(data);
    }else{
        priv->tx_data_buf = (volatile uint8_t*)(data);
    }
*/
    //config: ctrl
    uint32_t ctrl;
    if(priv->mode==USART_MODE_SINGLE_WIRE){
        getData(ctrl,priv->prog_base_tx,USART_OFFSET_CTRL);
        ctrl &= 0xFFFFDFFF;  //clear receive_flag
        putData(ctrl,priv->prog_base_tx,USART_OFFSET_CTRL);
    }

    //config: INT
    putData(0,priv->prog_base_tx,USART_OFFSET_INT);

    //config: tx_fifo_head
    uint32_t fifo_head;

    fifo_head = (USART_FIFO_DEPTH << 24) | (USART_FIFO_EMPTY_TL << 16);
    putData(fifo_head,priv->prog_base_tx,USART_OFFSET_FIFO_HEAD_W0);
    fifo_head = (priv->prog_base_tx - priv->spu_base + USART_OFFSET_FIFO_MEM) << 14;
    putData(fifo_head,priv->prog_base_tx,USART_OFFSET_FIFO_HEAD_W1);

    //send data
    send_data(priv);
    priv->tx_busy = true;

    //start PE
    uint32_t pgpio_decode;

    pgpio_decode = (1 << priv->sout_id);
    putData(pgpio_decode,priv->global_base,OFFSET_PGPIO_START_EN);


    return 0;
}

/**
  \brief       Abort Send data to UART transmitter
  \param[in]   handle  usart handle to operate.
  \return      error code
*/
int32_t csi_usart_abort_send(usart_handle_t handle)
{
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
    ck_spu_usart_priv_t *priv = handle;

    priv->rx_data_buf = data;
    priv->rx_total_num = num;
    priv->rx_cur_num = 0;

    //config: ctrl
    uint32_t ctrl;
    uint32_t prog_base;

    if(priv->mode==USART_MODE_SINGLE_WIRE){
        getData(ctrl,priv->prog_base_tx,USART_OFFSET_CTRL);
        ctrl |= 0x2000;  //set receive_flag
        putData(ctrl,priv->prog_base_tx,USART_OFFSET_CTRL);

        prog_base = priv->prog_base_tx;
    }
    else{
        prog_base = priv->prog_base_rx;
    }

    //config: TRANS_LEN
    putData(num,prog_base,USART_OFFSET_TRANS_LEN);

    //config: INT
    putData(0,prog_base,USART_OFFSET_INT);

    //config: rx_fifo_head
    uint32_t fifo_head;

    fifo_head = (USART_FIFO_DEPTH << 24) | (USART_FIFO_FULL_TL << 16);
    putData(fifo_head,prog_base,USART_OFFSET_FIFO_HEAD_W0);
    fifo_head = (prog_base - priv->spu_base + USART_OFFSET_FIFO_MEM) << 14;
    putData(fifo_head,prog_base,USART_OFFSET_FIFO_HEAD_W1);

    //start PE
    uint32_t pgpio_decode;


    if(priv->mode==USART_MODE_SINGLE_WIRE){
        pgpio_decode = (1 << priv->sout_id);
    }
    else{
        pgpio_decode = (1 << priv->sin_id);
    }
    putData(pgpio_decode,priv->global_base,OFFSET_PGPIO_START_EN);

    priv->rx_busy = true;

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
    ck_spu_usart_priv_t *priv = handle;
    if(priv->rx_busy){
	return 0;
    }else{
        return priv->rx_total_num;
    }
}

/**
  \brief       Abort Receive data from UART receiver
  \param[in]   handle  usart handle to operate.
  \return      error code
*/
int32_t csi_usart_abort_receive(usart_handle_t handle)
{
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
int32_t csi_csi_usart_transfer(usart_handle_t handle, const void *data_out, void *data_in, uint32_t num)
{
    return ERR_USART(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       abort sending/receiving data to/from USART transmitter/receiver.
  \param[in]   handle  usart handle to operate.
  \return      error code
*/
int32_t csi_usart_abort_transfer(usart_handle_t handle)
{
    return ERR_USART(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       Get USART status.
  \param[in]   handle  usart handle to operate.
  \return      USART status \ref usart_status_t
*/
usart_status_t csi_usart_get_status(usart_handle_t handle)
{
    ck_spu_usart_priv_t *priv = handle;

    usart_status_t usart_status;

    if(priv->tx_busy){
        usart_status.tx_busy = 1;
    }else{
        usart_status.tx_busy = 0;
    }

    if(priv->rx_busy){
        usart_status.rx_busy = 1;
    }else{
        usart_status.rx_busy = 0;
    }

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
    return ERR_USART(DRV_ERROR_UNSUPPORTED);
    //return 0;
}

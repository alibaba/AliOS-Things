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
 * @file
 * @brief
 * @version  V1.0
 * @date
 ******************************************************************************/

#include <csi_config.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "csi_core.h"
#include "drv_spi.h"
#include "ck_spu.h"
#include "ck_spu_spi.h"

#include "soc.h"
#include "pin_name.h"
#include "spu_pin_planning.h"

#include "stdio.h"
#include <string.h>

extern uint32_t spu_get_base(int32_t idx);
extern void spu_enable_clock(int32_t idx,int32_t pgpio);
extern void spu_disable_clock(int32_t idx,int32_t pgpio);
extern int32_t spu_apply_prog(int32_t idx,int32_t bytes);
extern int32_t spu_get_spi_func_index_base(int32_t idx);
extern int32_t get_spi_info(int32_t idx, int32_t *spu_idx,
                     int32_t *pgpio_sclk, int32_t *pgpio_mosi, int32_t *pgpio_miso, int32_t *pgpio_ssel);

//-------------------------------------------------
//                   data structure
//-------------------------------------------------
typedef struct{
    //Hardware configuration
    uint32_t spu_base;
    uint32_t global_base;
    uint32_t pgpio_base;
    uint32_t prog_base;
    uint32_t inst_base;
    uint8_t spu_idx;
    uint8_t pgpio_sclk;
    uint8_t pgpio_ssel;
    uint8_t pgpio_mosi;
    uint8_t pgpio_miso;
    //ssel pin
    int32_t pin_ssel;

    //call back
    spi_event_cb_t cb_event;

    //spi mode
    spi_mode_e mode;
    spi_format_e format;
    spi_bit_order_e order;
    spi_ss_mode_e ss_mode;
    int32_t bit_width;
    uint32_t default_value;

    //variable
    volatile spi_status_t status;
    uint8_t block_mode;

    void *tx_data_buf;
    uint32_t tx_total_num;
    volatile uint32_t tx_cur_num;

    void *rx_data_buf;
    uint32_t rx_total_num;
    volatile uint32_t rx_cur_num;
} ck_spu_spi_priv_t;

static ck_spu_spi_priv_t spi_instance[CONFIG_SPI_NUM];

static const spi_capabilities_t spi_capabilities = {
    1, /* Simplex Mode (Master and Slave) */
    0, /* TI Synchronous Serial Interface */
    0, /* Microwire Interface */
    0  /* Signal Mode Fault event: \ref CSKY_SPI_EVENT_MODE_FAULT */
};

//-------------------------------------------------
//                    function
//-------------------------------------------------
static void enable_pgpio(spi_handle_t handle)
{
    ck_spu_spi_priv_t *priv = handle;
    uint32_t data;

    getData(data,priv->global_base,OFFSET_PGPIO_CLK_EN);
    data |= (1 << priv->pgpio_sclk);
    data |= (1 << priv->pgpio_ssel);
    data |= (1 << priv->pgpio_mosi);
    data |= (1 << priv->pgpio_miso);
    putData(data,priv->global_base,OFFSET_PGPIO_CLK_EN);
}

static void disable_pgpio(spi_handle_t handle)
{
    ck_spu_spi_priv_t *priv = handle;
    uint32_t data;

    getData(data,priv->global_base,OFFSET_PGPIO_CLK_EN);
    data &= ~(1 << priv->pgpio_sclk);
    data &= ~(1 << priv->pgpio_ssel);
    data &= ~(1 << priv->pgpio_mosi);
    data &= ~(1 << priv->pgpio_miso);
    putData(data,priv->global_base,OFFSET_PGPIO_CLK_EN);
}

static uint32_t get_clock_div(uint32_t sysclk,uint32_t baud)
{
    return (sysclk / (baud << 1));
}

static void write_tx_fifo(spi_handle_t handle, uint32_t value)
{
    ck_spu_spi_priv_t *priv = handle;
    uint32_t fifo_head_w0;
    uint32_t fifo_head_w1;
    uint32_t fifo_depth;
    uint32_t wr_offset;
    uint32_t mem_addr;
    uint32_t wr_pointer;
    uint32_t wr_data;

    //write fifo
    getData(fifo_head_w0,priv->prog_base,SPI_OFFSET_TX_HEAD_W0);
    getData(fifo_head_w1,priv->prog_base,SPI_OFFSET_TX_HEAD_W1);
    fifo_depth = fifo_head_w0 >> 24;
    wr_offset = fifo_head_w1 & fifo_depth;
    mem_addr = fifo_head_w1 >> 16;
    wr_pointer = ((mem_addr + wr_offset)<< 2) + priv->spu_base;

    if(priv->order==SPI_ORDER_MSB2LSB){
        wr_data = value << (32 - priv->bit_width);
    }
    else{
        wr_data = value;
    }

    putData(wr_data,wr_pointer,0);

    //increase fifo pointer
    uint32_t head_hf_h;
    uint32_t head_hf_l;

    head_hf_h = fifo_head_w1 & 0xFFFF0000;
    fifo_head_w1++;
    head_hf_l = fifo_head_w1 & 0xFFFF;
    fifo_head_w1 = head_hf_h | head_hf_l;
    putData(fifo_head_w1,priv->prog_base,SPI_OFFSET_TX_HEAD_W1);
}

static void send_data(spi_handle_t handle)
{
    ck_spu_spi_priv_t *priv = handle;

    uint32_t fifo_head_w0;
    uint32_t fifo_head_w1;
    uint32_t head_sub;
    uint32_t fifo_depth;
    uint32_t mask;
    uint32_t valid_entries;

    //check tx_fifo valid entry
    getData(fifo_head_w0,priv->prog_base,SPI_OFFSET_TX_HEAD_W0);
    getData(fifo_head_w1,priv->prog_base,SPI_OFFSET_TX_HEAD_W1);
    head_sub = fifo_head_w1 - fifo_head_w0;
    fifo_depth =  fifo_head_w0 >> 24;
    mask = (fifo_depth + 1) | fifo_depth;
    valid_entries = (fifo_depth + 1) - (head_sub & mask);
    valid_entries--;  //PE's requirement

    //send data
    uint32_t bytes_to_send;
    uint32_t trans_len;
    uint32_t intr;
    uint32_t trans_data;

    bytes_to_send = priv->tx_total_num - priv->tx_cur_num;

    if(bytes_to_send <= valid_entries){
        trans_len = bytes_to_send;
        //close interrupt
        getData(intr,priv->prog_base,SPI_OFFSET_INT);
        intr |= (SPI_INT_TX_EMPTY << 16);
        putData(intr,priv->prog_base,SPI_OFFSET_INT);
    }
    else{
        trans_len = valid_entries;
    }

    priv->tx_cur_num += trans_len;

    if(priv->bit_width>8){
        while(trans_len>0){
            trans_data = *(volatile uint16_t*)(priv->tx_data_buf);
            write_tx_fifo(priv,trans_data);
            priv->tx_data_buf = (uint16_t*)(priv->tx_data_buf) + 1;
            trans_len--;
        }
    }
    else{
        while(trans_len>0){
            trans_data = *(volatile uint8_t*)(priv->tx_data_buf);
            write_tx_fifo(priv,trans_data);
            priv->tx_data_buf = (uint8_t*)(priv->tx_data_buf) + 1;
            trans_len--;
        }
    }
}

static uint32_t read_rx_fifo(spi_handle_t handle)
{
    ck_spu_spi_priv_t *priv = handle;

    uint32_t fifo_head_w0;
    uint32_t fifo_head_w1;
    uint32_t fifo_depth;
    uint32_t rd_offset;
    uint32_t mem_addr;
    uint32_t rd_pointer;
    uint32_t rd_data;

    //read fifo
    getData(fifo_head_w0,priv->prog_base,SPI_OFFSET_RX_HEAD_W0);
    getData(fifo_head_w1,priv->prog_base,SPI_OFFSET_RX_HEAD_W1);
    fifo_depth = fifo_head_w0 >> 24;
    rd_offset = fifo_head_w0 & fifo_depth;
    mem_addr = fifo_head_w1 >> 16;
    rd_pointer = ((mem_addr + rd_offset) << 2) + priv->spu_base;

    getData(rd_data,rd_pointer,0);
    if(priv->order==SPI_ORDER_LSB2MSB){
        rd_data = rd_data >> (32 - priv->bit_width);
    }
    else{
        rd_data = rd_data & (0xFFFFFFFF >> (32 - priv->bit_width));
    }

    //increase fifo pointer
    uint32_t head_hf_h;
    uint32_t head_hf_l;

    head_hf_h = fifo_head_w0 & 0xFFFF0000;
    fifo_head_w0++;
    head_hf_l = fifo_head_w0 & 0xFFFF;
    fifo_head_w0 = head_hf_h | head_hf_l;
    putData(fifo_head_w0,priv->prog_base,SPI_OFFSET_RX_HEAD_W0);

    return rd_data;
}

static void receive_data(spi_handle_t handle)
{
    ck_spu_spi_priv_t *priv = handle;

    uint32_t fifo_head_w0;
    uint32_t fifo_head_w1;
    uint32_t head_sub;
    uint32_t fifo_depth;
    uint32_t mask;
    uint32_t valid_entries;

    //check rx_fifo valid entry
    getData(fifo_head_w0,priv->prog_base,SPI_OFFSET_RX_HEAD_W0);
    getData(fifo_head_w1,priv->prog_base,SPI_OFFSET_RX_HEAD_W1);
    head_sub = fifo_head_w1 - fifo_head_w0;
    fifo_depth =  fifo_head_w0 >> 24;
    mask = (fifo_depth + 1) | fifo_depth;
    valid_entries = head_sub & mask;

    //receive data
    uint32_t trans_data;
    priv->rx_cur_num += valid_entries;

    if(priv->bit_width>8){
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

static void get_status(spi_handle_t handle){
    ck_spu_spi_priv_t *priv = handle;
    uint32_t int_status;

    getData(int_status,priv->prog_base,SPI_OFFSET_INT);
    //deal with interrupt
    if(((int_status & SPI_INT_TX_DONE)!=0) || ((int_status & SPI_INT_RX_DONE)!=0)){
        priv->status.busy = 0U;
        int_status &= 0xFFFF0000;
        putData(int_status,priv->prog_base,SPI_OFFSET_INT);
    }
}
/*

*/
//-------------------------------------------------
//                     IRQ
//-------------------------------------------------
void ck_spu_spi_irqhandler(int32_t idx,spu_dev_signal_e signal)
{
    ck_spu_spi_priv_t *priv = &spi_instance[idx];

    //get interrupt status
    uint32_t int_status;

    getData(int_status,priv->prog_base,SPI_OFFSET_INT);

    //deal with interrupt
    if((int_status & SPI_INT_TX_EMPTY)!=0){
        send_data(priv);
    }else if((int_status & SPI_INT_RX_FULL)!=0){
        receive_data(priv);
    }

    if(((int_status & SPI_INT_TX_DONE)!=0) && ((int_status & SPI_INT_RX_DONE)!=0)){
        receive_data(priv);
        priv->status.busy = 0U;
        if(priv->cb_event){
            priv->cb_event(SPI_EVENT_TRANSFER_COMPLETE, idx);
        }
    }
    else if((int_status & SPI_INT_TX_DONE)!=0){
        priv->status.busy = 0U;
        if(priv->cb_event){
            priv->cb_event(SPI_EVENT_TX_COMPLETE, idx);
        }
    }
    else if((int_status & SPI_INT_RX_DONE)!=0){
        receive_data(priv);
        priv->status.busy = 0U;
        if(priv->cb_event){
            priv->cb_event(SPI_EVENT_RX_COMPLETE, idx);
        }
    }


    int_status &= 0xFFFF0000;
    putData(int_status,priv->prog_base,SPI_OFFSET_INT);
}


//-------------------------------------------------
//                     API
//-------------------------------------------------
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
  \brief       Initialize SPI Interface. 1. Initializes the resources needed for the SPI interface 2.registers event callback function
  \param[in]   idx spi index
  \param[in]   cb_event  event call back function \ref spi_event_cb_t
  \return      return spi handle if success
*/
spi_handle_t csi_spi_initialize(int32_t idx, spi_event_cb_t cb_event)
{
    //initialize instace
    int32_t spu_idx;
    int32_t pgpio_sclk;
    int32_t pgpio_mosi;
    int32_t pgpio_miso;
    int32_t pgpio_ssel;
    int32_t spi_idx;
    ck_spu_spi_priv_t *priv;
    int32_t apply_prog;

    spi_idx = get_spi_info(idx,&spu_idx,&pgpio_sclk,&pgpio_mosi,&pgpio_miso,&pgpio_ssel);
    if(spi_idx<0){
      return NULL;
    }

    priv = &spi_instance[spi_idx];

    priv->spu_base = spu_get_base(spu_idx);
    priv->global_base = priv->spu_base + GLOBAL_BASE;
    priv->pgpio_base = priv->spu_base + PGPIO_BASE;
    priv->inst_base = priv->spu_base + INST_BASE;
    priv->spu_idx = spu_idx;
    priv->pgpio_sclk = pgpio_sclk;
    priv->pgpio_ssel = pgpio_ssel;
    priv->pgpio_mosi = pgpio_mosi;
    priv->pgpio_miso = pgpio_miso;

    apply_prog = spu_apply_prog(priv->spu_idx,SPI_PROG_BYTES);
    if(apply_prog==-1){
        return NULL;
    }else{
        priv->prog_base = priv->spu_base + PROG_BASE + apply_prog;
    }

    priv->pin_ssel = ssel;

    //initialize SPI bus
    uint32_t offset_pgpio;

    enable_pgpio(priv);

    offset_pgpio = (priv->pgpio_sclk << 4) + 8;
    putData(0x0,priv->pgpio_base,offset_pgpio);  //SCLK: CTRL

    offset_pgpio = (priv->pgpio_ssel << 4) + 8;
    putData(0xE,priv->pgpio_base,offset_pgpio);  //SSEL: CTRL

    offset_pgpio = (priv->pgpio_mosi << 4) + 8;
    putData(0x0,priv->pgpio_base,offset_pgpio);  //MOSI: CTRL

    offset_pgpio = (priv->pgpio_miso << 4) + 8;
    putData(0x0,priv->pgpio_base,offset_pgpio);  //MISO: CTRL

    //initialize INST_RAM
    spu_enable_clock(priv->spu_idx,priv->pgpio_ssel);

    //initialize variable
    priv->default_value = 0;
    priv->mode = SPI_MODE_INACTIVE;
    priv->status.busy       = 0U;
    priv->status.data_lost  = 0U;
    priv->status.mode_fault = 0U;

    //register callback function
    priv->cb_event = cb_event;

    return priv;
}

/**
  \brief       De-initialize SPI Interface. stops operation and releases the software resources used by the interface
  \param[in]   handle  spi handle to operate.
  \return      0 for success, negative for error code
*/
int32_t csi_spi_uninitialize(spi_handle_t handle)
{
    ck_spu_spi_priv_t *priv = handle;

    //initialize SPI bus
    uint32_t offset_pgpio;

    enable_pgpio(priv);

    offset_pgpio = (priv->pgpio_sclk << 4) + 8;
    putData(0x0,priv->pgpio_base,offset_pgpio);  //SCLK: CTRL

    offset_pgpio = (priv->pgpio_ssel << 4) + 8;
    putData(0x0,priv->pgpio_base,offset_pgpio);  //SSEL: CTRL

    offset_pgpio = (priv->pgpio_mosi << 4) + 8;
    putData(0x0,priv->pgpio_base,offset_pgpio);  //MOSI: CTRL

    offset_pgpio = (priv->pgpio_miso << 4) + 8;
    putData(0x0,priv->pgpio_base,offset_pgpio);  //MISO: CTRL

    disable_pgpio(priv);
    spu_disable_clock(priv->spu_idx,priv->pgpio_ssel);

    //release data
    priv->cb_event = NULL;

    return 0;
}

/**
  \brief       config spi attributes.
  \param[in]   handle    spi handle to operate.
  \param[in]   baud      spi baud rate. if negative, then this attribute not changed
  \param[in]   mode      spi mode \ref spi_mode_e. if negative, then this attribute not changed.
  \param[in]   speed     spi speed \ref spi_speed_e.if negative, then this attribute not changed.
  \param[in]   addr_mode spi address mode \ref spi_address_mode_e. if negative, then this attribute not changed.
  \param[in]   slave_addr spi address of slave. if negative, then this attribute not changed.
  \return      0 for success, negative for error code
*/
int32_t csi_spi_config(spi_handle_t handle,
                       int32_t baud,
                       spi_mode_e mode,
                       spi_format_e format,
                       spi_bit_order_e order,
                       spi_ss_mode_e ss_mode,
                       int32_t bit_width)
{
    ck_spu_spi_priv_t *priv = handle;

    //format
    uint32_t func_index;
    uint32_t offset_inst;
    uint32_t data_inst;
    uint32_t pgpio_ctrl_ssel = 0;
    uint32_t pgpio_ctrl_sclk = 0;
    bool sclk_ctrl_reconfig;

    func_index = spu_get_spi_func_index_base(priv->spu_idx);

    if(mode==SPI_MODE_MASTER_SIMPLEX || mode==SPI_MODE_MASTER){
        switch(format){
            case SPI_FORMAT_CPOL0_CPHA0:
                pgpio_ctrl_sclk = 0x2;
                break;
            case SPI_FORMAT_CPOL1_CPHA0:
                func_index += 64;
                pgpio_ctrl_sclk = 0xE;
                break;
            case SPI_FORMAT_CPOL0_CPHA1:
                func_index += 128;
                pgpio_ctrl_sclk = 0x2;
                break;
            case SPI_FORMAT_CPOL1_CPHA1:
                func_index += 192;
                pgpio_ctrl_sclk = 0xE;
                break;
        }
        sclk_ctrl_reconfig = true;
        pgpio_ctrl_ssel = 0xE;
    }
    else{
        switch(format){
            case SPI_FORMAT_CPOL0_CPHA0:
                func_index += 256;
                break;
            case SPI_FORMAT_CPOL1_CPHA0:
                func_index += 320;
                break;
            case SPI_FORMAT_CPOL0_CPHA1:
                func_index += 384;
                break;
            case SPI_FORMAT_CPOL1_CPHA1:
                func_index += 448;
                break;
        }

        if(priv->mode!=SPI_MODE_SLAVE_SIMPLEX && priv->mode!=SPI_MODE_SLAVE){
        sclk_ctrl_reconfig = true;
            pgpio_ctrl_ssel = 0x0;
            pgpio_ctrl_sclk = 0x0;
        }
        else{
        sclk_ctrl_reconfig = false;
        }
    }

    offset_inst = priv->pgpio_ssel << 3;
    data_inst = (func_index<<16) | (priv->prog_base - priv->spu_base);
    putData(data_inst,priv->inst_base,offset_inst);

    //mode
    uint32_t context;

    priv->mode = mode;
    offset_inst = (priv->pgpio_ssel << 3) + 4;

    if(mode==SPI_MODE_MASTER_SIMPLEX){
        context = (priv->pgpio_sclk) | (priv->pgpio_mosi << 5) | (priv->pgpio_mosi << 10) | (priv->pgpio_ssel << 15);
        putData(context,priv->inst_base,offset_inst);
    }
    else if(mode==SPI_MODE_SLAVE_SIMPLEX){
        context = (priv->pgpio_sclk) | (priv->pgpio_miso << 5) | (priv->pgpio_miso << 10) | (priv->pgpio_ssel << 15);
        putData(context,priv->inst_base,offset_inst);
    }
    else{
        context = (priv->pgpio_sclk) | (priv->pgpio_mosi << 5) | (priv->pgpio_miso << 10) | (priv->pgpio_ssel << 15);
        putData(context,priv->inst_base,offset_inst);
    }

    //ss_mode
    uint32_t io_reuse_addr=0;
    uint32_t io_reuse_data;

        //HOBBIT_GIPO0_PORTCTL_REG
    getData(io_reuse_data,CSKY_IOC_BASE, io_reuse_addr);
    //printf("io_reuse_data0=%x\n",io_reuse_data);

    if(ss_mode==SPI_SS_MASTER_HW_OUTPUT){
        io_reuse_data |= 1<<priv->pin_ssel;
        putData(io_reuse_data,CSKY_IOC_BASE, io_reuse_addr);
    }else if(ss_mode==SPI_SS_MASTER_SW){
        io_reuse_data &= ~(1<<priv->pin_ssel);
        putData(io_reuse_data,CSKY_IOC_BASE, io_reuse_addr);
    }

        //HOBBIT_IOMUX0L_REG
    io_reuse_addr += 8;
    getData(io_reuse_data,CSKY_IOC_BASE, io_reuse_addr);
    //printf("io_reuse_data1=%x\n",io_reuse_data);

    if(ss_mode==SPI_SS_MASTER_HW_OUTPUT){
        io_reuse_data |= 2<<(priv->pin_ssel*2);
        putData(io_reuse_data,CSKY_IOC_BASE, io_reuse_addr);
    }

    //baud
    uint32_t offset_pgpio;
    uint32_t clk_div;

    clk_div = (SPI_FILTER_COE) << 23;
    if(mode==SPI_MODE_MASTER || mode==SPI_MODE_MASTER_SIMPLEX){
        clk_div |= get_clock_div(LSP_DEFAULT_FREQ,baud);
    }
    else{
    clk_div |= 1;
    }

    offset_pgpio = (priv->pgpio_sclk << 4) + 4;
    putData(clk_div,priv->pgpio_base,offset_pgpio);  //SCLK: CLK_DIV
    if(sclk_ctrl_reconfig){
        offset_pgpio+=4;
        putData(pgpio_ctrl_sclk,priv->pgpio_base,offset_pgpio);  //SCLK: CTRL
    }

    offset_pgpio = (priv->pgpio_ssel << 4) + 4;
    putData(clk_div,priv->pgpio_base,offset_pgpio);  //SSEL: CLK_DIV
    offset_pgpio+=4;
    putData(pgpio_ctrl_ssel,priv->pgpio_base,offset_pgpio);  //SSEL: CTRL

    offset_pgpio = (priv->pgpio_mosi << 4) + 4;
    putData(clk_div,priv->pgpio_base,offset_pgpio);  //MOSI: CLK_DIV

    offset_pgpio = (priv->pgpio_miso << 4) + 4;
    putData(clk_div,priv->pgpio_base,offset_pgpio);  //MISO: CLK_DIV

    //order
    //bit_width
    uint32_t refer_clk;
    priv->order = order;
    priv->bit_width = bit_width;

    refer_clk = (priv->pgpio_sclk << 21);
    refer_clk |= ((bit_width-1) << 2);
    if(order==SPI_ORDER_MSB2LSB){refer_clk |= (1 << 28);}
    putData(refer_clk, priv->prog_base, SPI_OFFSET_REFER_CLK);

    //variable
    priv->status.busy       = 0U;
    priv->status.data_lost  = 0U;
    priv->status.mode_fault = 0U;

    return 0;
}

/**
\brief         sending data to SPI transmitter,(received data is ignored).
               if non-blocking mode, this function only start the sending,
               \ref spi_event_e is signaled when operation completes or error happens.
               \ref csi_spi_get_status can indicates operation status.
               if blocking mode, this function return after operation completes or error happens.
  \param[in]   handle spi handle to operate.
  \param[in]   data  Pointer to buffer with data to send to SPI transmitter. data_type is : uint8_t for 1..8 data bits, uint16_t for 9..16 data bits,uint32_t for 17..32 data bits,
  \param[in]   num   Number of data items to send.
  \return      error code
*/
//int32_t csi_spi_send(spi_handle_t handle, const void *data, uint32_t num, uint8_t block_mode, bool back_to_back, bool cs_pend)
int32_t csi_spi_send(spi_handle_t handle, const void *data, uint32_t num)
{
    ck_spu_spi_priv_t *priv = handle;

    priv->tx_data_buf = (void *)data;
    priv->tx_total_num = num;
    priv->tx_cur_num = 0;

    //config: ctrl
    uint32_t ctrl=SPI_TMOD_TRANSMIT | SPI_BACK_TO_BACK;
//    if(back_to_back){
//        ctrl |= SPI_BACK_TO_BACK;
//    }
//    if(cs_pend){
//        ctrl |= SPI_CS_PEND;
//    }

    putData(ctrl,priv->prog_base,SPI_OFFSET_CTRL);

    //config: TRANS_LEN
    putData(num,priv->prog_base,SPI_OFFSET_TRANS_LEN);

    //config: INT
    putData(0,priv->prog_base,SPI_OFFSET_INT);

    //config: tx_fifo_head
    uint32_t fifo_head;

    fifo_head = (SPI_TX_FIFO_DEPTH << 24) | (SPI_TX_FIFO_EMPTY_TL << 16);
    putData(fifo_head,priv->prog_base,SPI_OFFSET_TX_HEAD_W0);
    fifo_head = (priv->prog_base - priv->spu_base + SPI_OFFSET_TX_MEM) << 14;
    putData(fifo_head,priv->prog_base,SPI_OFFSET_TX_HEAD_W1);

    //send data
    priv->status.busy = 1U;
    send_data(priv);

    //start PE
    uint32_t pgpio_decode;

    pgpio_decode = (1 << priv->pgpio_ssel);
    putData(pgpio_decode,priv->global_base,OFFSET_PGPIO_START_EN);

    if(priv->block_mode){
        do{
            get_status(priv);
        }while(priv->status.busy);
    }

    return 0;
}

/**
  \brief       Start receiving data as SPI Master.
               This function is non-blocking,\ref spi_event_e is signaled when transfer completes or error happens.
               drv_*_get_status can indicates transmission status.
  \param[in]   handle  spi handle to operate.
  \param[out]  data    Pointer to buffer for data to receive from SPI receiver
  \param[in]   num     Number of data items to receive
  \return      0 for success, negative for error code
*/
//int32_t csi_spi_receive(spi_handle_t handle, void *data, uint32_t num, uint8_t block_mode, bool back_to_back, bool cs_pend)
int32_t csi_spi_receive(spi_handle_t handle, void *data, uint32_t num)
{
    ck_spu_spi_priv_t *priv = handle;

    priv->rx_data_buf = data;
    priv->rx_total_num = num;
    priv->rx_cur_num = 0;

    //config: ctrl
    uint32_t ctrl=SPI_TMOD_RECEIVE | SPI_BACK_TO_BACK;
    //if(back_to_back){
    //    ctrl |= SPI_BACK_TO_BACK;
    //}
    //if(cs_pend){
    //    ctrl |= SPI_CS_PEND;
    //}

    putData(ctrl,priv->prog_base,SPI_OFFSET_CTRL);
    //putData(SPI_TMOD_RECEIVE,priv->prog_base,SPI_OFFSET_CTRL);

    //config: TRANS_LEN
    putData(num,priv->prog_base,SPI_OFFSET_TRANS_LEN);

    //config: INT
    putData(0,priv->prog_base,SPI_OFFSET_INT);

    //config: rx_fifo_head
    uint32_t fifo_head;

    fifo_head = (SPI_RX_FIFO_DEPTH << 24) | (SPI_RX_FIFO_FULL_TL << 16);
    putData(fifo_head,priv->prog_base,SPI_OFFSET_RX_HEAD_W0);
    fifo_head = (priv->prog_base - priv->spu_base + SPI_OFFSET_RX_MEM) << 14;
    putData(fifo_head,priv->prog_base,SPI_OFFSET_RX_HEAD_W1);

    //start PE
    uint32_t pgpio_decode;
    priv->status.busy = 1U;

    pgpio_decode = (1 << priv->pgpio_ssel);
    putData(pgpio_decode,priv->global_base,OFFSET_PGPIO_START_EN);


    if(priv->block_mode){
        do{
            get_status(priv);
        }while(priv->status.busy);
    }

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
  \param[in]   block_mode   blocking and non_blocking to selcect
  \return      error code
*/
//int32_t csi_spi_transfer(spi_handle_t handle, const void *data_out, void *data_in, uint32_t num_out, uint32_t num_in, uint8_t block_mode, bool back_to_back, bool cs_pend)
int32_t csi_spi_transfer(spi_handle_t handle, const void *data_out, void *data_in, uint32_t num_out, uint32_t num_in)
{
    ck_spu_spi_priv_t *priv = handle;

    priv->tx_data_buf = (void *)data_out;
    priv->tx_total_num = num_out;
    priv->tx_cur_num = 0;

    priv->rx_data_buf = data_in;
    priv->rx_total_num = num_in;
    priv->rx_cur_num = 0;

    //config: ctrl
    uint32_t ctrl=SPI_TMOD_TRANSFER | SPI_BACK_TO_BACK;
    //if(back_to_back){
    //    ctrl |= SPI_BACK_TO_BACK;
    //}
    //if(cs_pend){
    //    ctrl |= SPI_CS_PEND;
    //}

    putData(ctrl,priv->prog_base,SPI_OFFSET_CTRL);
    //putData(SPI_TMOD_TRANSFER,priv->prog_base,SPI_OFFSET_CTRL);

    //config: TRANS_LEN
    putData(num_in,priv->prog_base,SPI_OFFSET_TRANS_LEN);

    //config: INT
    putData(0x0,priv->prog_base,SPI_OFFSET_INT);

    //config: tx_fifo_head & rx_fifo_head
    uint32_t fifo_head;

    fifo_head = (SPI_TX_FIFO_DEPTH << 24) | (SPI_TX_FIFO_EMPTY_TL << 16);
    putData(fifo_head,priv->prog_base,SPI_OFFSET_TX_HEAD_W0);
    fifo_head = (priv->prog_base - priv->spu_base + SPI_OFFSET_TX_MEM) << 14;
    putData(fifo_head,priv->prog_base,SPI_OFFSET_TX_HEAD_W1);

    fifo_head = (SPI_RX_FIFO_DEPTH << 24) | (SPI_RX_FIFO_FULL_TL << 16);
    putData(fifo_head,priv->prog_base,SPI_OFFSET_RX_HEAD_W0);
    fifo_head = (priv->prog_base - priv->spu_base + SPI_OFFSET_RX_MEM) << 14;
    putData(fifo_head,priv->prog_base,SPI_OFFSET_RX_HEAD_W1);

    //send data
    send_data(priv);
    priv->status.busy = 1U;

    //start PE
    uint32_t pgpio_decode;

    pgpio_decode = (1 << priv->pgpio_ssel);
    putData(pgpio_decode,priv->global_base,OFFSET_PGPIO_START_EN);


    if(priv->block_mode){
        do{
            get_status(priv);
        }while(priv->status.busy);
    }

    return 0;
}

int32_t csi_spi_abort_transfer(spi_handle_t handle){
    return 0;
}

spi_status_t csi_spi_get_status(spi_handle_t handle){
    ck_spu_spi_priv_t *priv = handle;

    if(priv->block_mode){
        get_status(priv);
    }

    return priv->status;
}

/**
  \brief       config the SPI block mode.
  \param[in]   handle   spi handle
  \param[in]   flag 1 - enbale the block mode. 0 - disable the block mode
  \return      error code
*/
int32_t csi_spi_config_block_mode(spi_handle_t handle, int32_t flag)
{
    dw_spi_priv_t *spi_priv = handle;
    if (flag == 1) {
        spi_priv->block_mode = 1;
    } else if (flag == 0) {
        spi_priv->block_mode = 0;
    } else {
        ERR_SPI(SPI_ERROR_MODE);
    }
    return 0;
}


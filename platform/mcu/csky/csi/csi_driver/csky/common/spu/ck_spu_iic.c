
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
#include "drv_iic.h"
#include "ck_spu_iic.h"
#include "ck_spu.h"
#include "soc.h"
#include "pin_name.h"
#include "spu_pin_planning.h"

extern uint32_t spu_get_base(int32_t idx);
extern void spu_enable_clock(int32_t idx,int32_t pgpio);
extern void spu_disable_clock(int32_t idx,int32_t pgpio);
extern int32_t spu_apply_prog(int32_t idx,int32_t bytes);
extern int32_t spu_get_i2c_func_index_base(int32_t idx);
extern int32_t get_i2c_info(int32_t pin_sda, int32_t pin_scl, int32_t *spu_idx, int32_t *pgpio_sda, int32_t *pgpio_scl);

//-------------------------------------------------
//                   data structure
//-------------------------------------------------
typedef enum {
    NACK_DURING_ADDR,   //slave answers NACK during master sending address
    NACK_DURING_DATA,   //slave answers NACK during master sending data
    NACK_NORMALLY,      //slave answers NACK after master sending all data
} ENUM_I2C_NACK_TYPE;

typedef enum {
    STOP_DONE,          //master sends STOP after all data being transfered
    STOP_INCOMPLETE,    //master sends STOP during transformation
} ENUM_I2C_STOP_TYPE;

//Hardware configuration
typedef struct{
    uint32_t spu_base;
    uint32_t global_base;
    uint32_t pgpio_base;
    uint32_t prog_base;
    uint32_t inst_base;
    uint32_t spu_idx;
    uint8_t sda_id;
    uint8_t scl_id;

    //call back
    iic_event_cb_t cb_event;

    //config
    iic_mode_e mode;
    iic_speed_e speed_mode;
    iic_address_mode_e addr_mode;

    uint8_t master_code;
    uint32_t slave_addr;

    //variable
    bool read_flag;
    bool tx_pending;
    bool rx_pending;

    char *data_buf;
    uint32_t total_num;
    volatile uint32_t cur_num;
} ck_spu_i2c_priv_t;

ck_spu_i2c_priv_t i2c_instance[CONFIG_SPU_I2C_NUM];

static const iic_capabilities_t iic_capabilities = {
    1  /* supports 10-bit addressing */
};
//-------------------------------------------------
//                    function
//-------------------------------------------------
static void enable_pgpio(iic_handle_t handle)
{
    ck_spu_i2c_priv_t *priv = handle;
    uint32_t data;

    getData(data,priv->global_base,OFFSET_PGPIO_CLK_EN);
    data |= (1 << priv->sda_id);
    data |= (1 << priv->scl_id);
    putData(data,priv->global_base,OFFSET_PGPIO_CLK_EN);
}

static void disable_pgpio(iic_handle_t handle)
{
    ck_spu_i2c_priv_t *priv = handle;
    uint32_t data;

    getData(data,priv->global_base,OFFSET_PGPIO_CLK_EN);
    data &= ~(1 << priv->sda_id);
    data &= ~(1 << priv->scl_id);
    putData(data,priv->global_base,OFFSET_PGPIO_CLK_EN);
}

static uint32_t get_clock_div(uint32_t freq)
{
    return (SPU_CLK_FREQUENCY / (freq << 1));
}

static void write_tx_fifo(iic_handle_t handle, uint32_t value)
{
    ck_spu_i2c_priv_t *priv = handle;
    uint32_t offset_tx_head_w0;
    uint32_t offset_tx_head_w1;
    uint32_t fifo_head_w0;
    uint32_t fifo_head_w1;
    uint32_t fifo_depth;
    uint32_t wr_offset;
    uint32_t mem_addr;
    uint32_t wr_pointer;
    uint32_t data_bigendian;
    uint32_t sr_flag;
    uint32_t wr_data;

    //write fifo
    if(priv->mode==IIC_MODE_MASTER){
        offset_tx_head_w0 = I2C_M_OFFSET_TX_HEAD_W0;
        offset_tx_head_w1 = I2C_M_OFFSET_TX_HEAD_W1;
    }
    else{
        offset_tx_head_w0 = I2C_S_OFFSET_FIFO_HEAD_W0;
        offset_tx_head_w1 = I2C_S_OFFSET_FIFO_HEAD_W1;
    }

    getData(fifo_head_w0,priv->prog_base,offset_tx_head_w0);
    getData(fifo_head_w1,priv->prog_base,offset_tx_head_w1);
    fifo_depth = fifo_head_w0 >> 24;
    wr_offset = fifo_head_w1 & fifo_depth;
    mem_addr = fifo_head_w1 >> 16;
    wr_pointer = ((mem_addr + wr_offset)<< 2) + priv->spu_base;

    data_bigendian = value << 24;
    sr_flag = value >> 31;
    wr_data = data_bigendian | sr_flag;
    putData(wr_data,wr_pointer,0);

    //increase fifo pointer
    uint32_t head_hf_h;
    uint32_t head_hf_l;

    head_hf_h = fifo_head_w1 & 0xFFFF0000;
    fifo_head_w1++;
    head_hf_l = fifo_head_w1 & 0xFFFF;
    fifo_head_w1 = head_hf_h | head_hf_l;
    putData(fifo_head_w1,priv->prog_base,offset_tx_head_w1);
}

static void send_data(iic_handle_t handle)
{
    ck_spu_i2c_priv_t *priv = handle;
    uint32_t offset_tx_head_w0;
    uint32_t offset_tx_head_w1;
    uint32_t fifo_head_w0;
    uint32_t fifo_head_w1;
    uint32_t head_sub;
    uint32_t fifo_depth;
    uint32_t mask;
    uint32_t valid_entries;

    //check tx_fifo valid entry
    if(priv->mode==IIC_MODE_MASTER){
        offset_tx_head_w0 = I2C_M_OFFSET_TX_HEAD_W0;
        offset_tx_head_w1 = I2C_M_OFFSET_TX_HEAD_W1;
    }
    else{
        offset_tx_head_w0 = I2C_S_OFFSET_FIFO_HEAD_W0;
        offset_tx_head_w1 = I2C_S_OFFSET_FIFO_HEAD_W1;
    }

    getData(fifo_head_w0,priv->prog_base,offset_tx_head_w0);
    getData(fifo_head_w1,priv->prog_base,offset_tx_head_w1);
    head_sub = fifo_head_w1 - fifo_head_w0;
    fifo_depth =  fifo_head_w0 >> 24;
    mask = (fifo_depth + 1) | fifo_depth;
    valid_entries = (fifo_depth + 1) - (head_sub & mask);

    //send data
    uint32_t bytes_to_send;
    uint32_t trans_len;
    uint32_t intr;
    uint8_t trans_data;

    bytes_to_send = priv->total_num - priv->cur_num;

    if(bytes_to_send <= valid_entries){
        trans_len = bytes_to_send;
        //close interrupt
        getData(intr,priv->prog_base,I2C_OFFSET_INT);
        intr |= (I2C_INT_TX_EMPTY << 16);
        putData(intr,priv->prog_base,I2C_OFFSET_INT);
    }
    else{
        trans_len = valid_entries;
    }

    priv->cur_num += trans_len;

    while(trans_len > 0){
        trans_data = *(volatile uint8_t *)(priv->data_buf);
        write_tx_fifo(priv,trans_data);
        (priv->data_buf)++;
        trans_len--;
    }
}

static uint32_t read_rx_fifo(iic_handle_t handle)
{
    ck_spu_i2c_priv_t *priv = handle;
    uint32_t offset_rx_head_w0;
    uint32_t offset_rx_head_w1;
    uint32_t fifo_head_w0;
    uint32_t fifo_head_w1;
    uint32_t fifo_depth;
    uint32_t rd_offset;
    uint32_t mem_addr;
    uint32_t rd_pointer;
    uint32_t rd_data;

    //read fifo
    if(priv->mode==IIC_MODE_MASTER){
        offset_rx_head_w0 = I2C_M_OFFSET_RX_HEAD_W0;
        offset_rx_head_w1 = I2C_M_OFFSET_RX_HEAD_W1;
    }
    else{
        offset_rx_head_w0 = I2C_S_OFFSET_FIFO_HEAD_W0;
        offset_rx_head_w1 = I2C_S_OFFSET_FIFO_HEAD_W1;
    }

    getData(fifo_head_w0,priv->prog_base,offset_rx_head_w0);
    getData(fifo_head_w1,priv->prog_base,offset_rx_head_w1);
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
    putData(fifo_head_w0,priv->prog_base,offset_rx_head_w0);

    return rd_data;
}

static void receive_data(iic_handle_t handle)
{
    ck_spu_i2c_priv_t *priv = handle;
    uint32_t offset_rx_head_w0;
    uint32_t offset_rx_head_w1;
    uint32_t fifo_head_w0;
    uint32_t fifo_head_w1;
    uint32_t head_sub;
    uint32_t fifo_depth;
    uint32_t mask;
    uint32_t valid_entries;

    //check rx_fifo valid entry
    if(priv->mode==IIC_MODE_MASTER){
        offset_rx_head_w0 = I2C_M_OFFSET_RX_HEAD_W0;
        offset_rx_head_w1 = I2C_M_OFFSET_RX_HEAD_W1;
    }
    else{
        offset_rx_head_w0 = I2C_S_OFFSET_FIFO_HEAD_W0;
        offset_rx_head_w1 = I2C_S_OFFSET_FIFO_HEAD_W1;
    }

    getData(fifo_head_w0,priv->prog_base,offset_rx_head_w0);
    getData(fifo_head_w1,priv->prog_base,offset_rx_head_w1);
    head_sub = fifo_head_w1 - fifo_head_w0;
    fifo_depth =  fifo_head_w0 >> 24;
    mask = (fifo_depth + 1) | fifo_depth;
    valid_entries = head_sub & mask;

    //receive data
    priv->cur_num += valid_entries;

    while(valid_entries > 0){
        *((volatile uint8_t *)(priv->data_buf)) = read_rx_fifo(priv);
        (priv->data_buf)++;
        valid_entries--;
    }
}

static ENUM_I2C_NACK_TYPE get_nack_type(iic_handle_t handle)
{
    ck_spu_i2c_priv_t *priv = handle;

    //address length
    uint32_t addr_len=0;

    if(priv->speed_mode==I2C_BUS_SPEED_HIGH){
        addr_len++;
    }

    if(priv->addr_mode==I2C_ADDRESS_7BIT){
        addr_len++;
    }
    else if(priv->read_flag){
        addr_len += 3;
    }
    else{
        addr_len += 2;
    }

    //check tx_fifo send length
    uint32_t fifo_head_w0;
    uint32_t fifo_head_w1;
    uint32_t send_len;
    bool fifo_empty;

    getData(fifo_head_w0,priv->prog_base,I2C_M_OFFSET_TX_HEAD_W0);
    getData(fifo_head_w1,priv->prog_base,I2C_M_OFFSET_TX_HEAD_W1);

    send_len = fifo_head_w0 & 0xFFFF;
    fifo_empty = (fifo_head_w0 & 0xFFFF) == (fifo_head_w1 & 0xFFFF);

    if(fifo_empty && (priv->total_num==priv->cur_num)){
        return NACK_NORMALLY;
    }
    else if(priv->cur_num > 0xFFFF){
        return NACK_DURING_DATA;
    }
    else if((send_len<=addr_len)&&(!priv->tx_pending)){
        return NACK_DURING_ADDR;
    }
    else{
        return NACK_DURING_DATA;
    }
}

static ENUM_I2C_STOP_TYPE get_stop_type(iic_handle_t handle)
{
    ck_spu_i2c_priv_t *priv = handle;
    uint32_t fifo_head_w0;
    uint32_t fifo_head_w1;
    uint32_t send_len;
    bool fifo_empty;

    if(priv->cur_num!=priv->total_num){
        return STOP_INCOMPLETE;
    }
    else if(priv->read_flag){   //slave receive
        return STOP_DONE;
    }
    else{   //slave transmit
        getData(fifo_head_w0,priv->prog_base,I2C_S_OFFSET_FIFO_HEAD_W0);
        getData(fifo_head_w1,priv->prog_base,I2C_S_OFFSET_FIFO_HEAD_W1);
        send_len = fifo_head_w0 & 0xFFFF;
        fifo_empty = (fifo_head_w0 & 0xFFFF) == (fifo_head_w1 & 0xFFFF);
        if(fifo_empty){
            return STOP_DONE;
        }
        else{
            return STOP_INCOMPLETE;
        }
    }
}

//-------------------------------------------------
//                      ISR
//-------------------------------------------------
void ck_spu_i2c_irqhandler(int32_t idx,spu_dev_signal_e signal)
{
    ck_spu_i2c_priv_t *priv = &i2c_instance[idx];

    //get interrupt status
    uint32_t int_status;

    getData(int_status,priv->prog_base,I2C_OFFSET_INT);
    int_status &= 0xFF;

    //deal with interrupt
    ENUM_I2C_NACK_TYPE nack_type;
    ENUM_I2C_STOP_TYPE stop_type;

    if(priv->mode==IIC_MODE_MASTER){
        switch(int_status){
            case I2C_INT_TX_EMPTY:
                if(priv->total_num!=priv->cur_num){
                    send_data(priv);
                }
                break;

            case I2C_INT_RX_FULL:
                receive_data(priv);
                break;

            case I2C_INT_TX_NACK:
                priv->tx_pending = false;
                priv->rx_pending = false;
                if(priv->cb_event){
                  nack_type = get_nack_type(priv);
                  if(nack_type==NACK_DURING_ADDR){
                      priv->cb_event(I2C_EVENT_ADDRESS_NACK, idx);
                  }
                  else if(nack_type==NACK_DURING_DATA){
                      priv->cb_event(I2C_EVENT_TRANSFER_INCOMPLETE, idx);
                  }
                  else if(nack_type==NACK_NORMALLY){
                      priv->cb_event(I2C_EVENT_TRANSFER_DONE,idx);
                  }
                }
                break;

            case I2C_INT_DONE:
                priv->tx_pending = false;
                priv->rx_pending = false;
                if(priv->read_flag){
                    receive_data(priv);
                }
                if(priv->cb_event){
                    priv->cb_event(I2C_EVENT_TRANSFER_DONE, idx);
                }
                break;

            case I2C_INT_PENDING:
                if(priv->read_flag){
                    priv->rx_pending = true;
                    receive_data(priv);
                }
                else{
                    priv->tx_pending = true;
                }

                if(priv->cb_event){
                    priv->cb_event(I2C_EVENT_TRANSFER_DONE, idx);
                }
                break;

            case I2C_INT_ARB_LOST:
                priv->tx_pending = false;
                priv->rx_pending = false;
                if(priv->cb_event){
                    priv->cb_event(I2C_EVENT_ARBITRATION_LOST, idx);
                }
                break;
        }
    }
    else{
        switch(int_status){
            case I2C_INT_SLV_TX:
                if(priv->cb_event){
                    priv->cb_event(I2C_EVENT_SLAVE_TRANSMIT, idx);
                }
                break;

            case I2C_INT_SLV_RX:
                if(priv->cb_event){
                    priv->cb_event(I2C_EVENT_SLAVE_RECEIVE, idx);
                }
                break;

            case I2C_INT_GENE_CALL:
                if(priv->cb_event){
                    priv->cb_event(I2C_EVENT_GENERAL_CALL, idx);
                }
                break;

            case I2C_INT_TX_EMPTY:
                if(priv->total_num!=priv->cur_num){
                    send_data(priv);
                }
                break;

            case I2C_INT_RX_FULL:
                receive_data(priv);
                break;

            case I2C_INT_DONE:
                receive_data(priv);
                stop_type = get_stop_type(priv);
                if(priv->cb_event){
                    if(stop_type==STOP_DONE){
                        priv->cb_event(I2C_EVENT_TRANSFER_DONE, idx);
                    }
                    else{
                        priv->cb_event(I2C_EVENT_TRANSFER_INCOMPLETE, idx);
                    }
                }
                break;
        }
    }
}
//-------------------------------------------------
//                     CSI
//-------------------------------------------------
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
  \brief       Initialize IIC Interface specified by pins. \n
               1. Initializes the resources needed for the IIC interface 2.registers event callback function
  \param[in]   idx iic index
  \param[in]   cb_event  Pointer to \ref iic_event_cb_t
  \return      0 for success, negative for error code
*/
iic_handle_t csi_iic_initialize(int32_t idx, iic_event_cb_t cb_event)
{
    //initialize instace
    int32_t spu_idx;
    int32_t pgpio_sda;
    int32_t pgpio_scl;
    int32_t i2c_idx;
    ck_spu_i2c_priv_t *priv;
    int32_t apply_prog;

    i2c_idx = get_i2c_info(idx, &spu_idx, &pgpio_sda, &pgpio_scl);
    if(i2c_idx<0){
      return NULL;
    }

    priv = &i2c_instance[i2c_idx];

    priv->spu_base = spu_get_base(spu_idx);
    priv->global_base = priv->spu_base + GLOBAL_BASE;
    priv->pgpio_base = priv->spu_base + PGPIO_BASE;
    priv->inst_base = priv->spu_base + INST_BASE;
    priv->spu_idx = spu_idx;
    priv->sda_id = pgpio_sda;
    priv->scl_id = pgpio_scl;

    apply_prog = spu_apply_prog(priv->spu_idx,I2C_M_PROG_BYTES);
    if(apply_prog==-1){
        return NULL;
    }else{
        priv->prog_base = priv->spu_base + PROG_BASE + apply_prog;
    }

    //initialize I2C bus
    enable_pgpio(priv);

    uint32_t offset_pgpio;
    uint32_t clk_div;

    offset_pgpio = (priv->sda_id << 4) + 4;  //SDA_CLK_DIV
    clk_div = (I2C_FILTER_COE) << 23;
    clk_div |= 0x10000000;
    putData(clk_div,priv->pgpio_base,offset_pgpio);

    offset_pgpio+=4;  //SDA_CTRL
    putData(0xE,priv->pgpio_base,offset_pgpio);

    offset_pgpio = (priv->scl_id << 4) + 4;  //SCL_CLK_DIV
    putData(clk_div,priv->pgpio_base,offset_pgpio);

    offset_pgpio+=4;  //SCL_CTRL
    putData(0xE,priv->pgpio_base,offset_pgpio);

    //initialize INST_RAM
    spu_enable_clock(priv->spu_idx,priv->sda_id);

    uint32_t offset_inst;
    uint32_t data_inst;

    offset_inst = (priv->sda_id << 3)+4;
    data_inst = (priv->scl_id<<5) | priv->sda_id;
    putData(data_inst,priv->inst_base,offset_inst);

    //initialize PROG_RAM
    uint32_t refer_clk;

    refer_clk = priv->scl_id << 21;
    putData(refer_clk,priv->prog_base,I2C_OFFSET_REFER_CLK);

    //register callback function
    priv->cb_event = cb_event;

    priv->tx_pending = false;
    priv->rx_pending = false;

    return priv;
}

/**
  \brief       De-initialize IIC Interface. stops operation and releases the software resources used by the interface
  \param[in]   handle  iic handle to operate.
  \return      error code
*/
int32_t csi_iic_uninitialize(iic_handle_t handle)
{
    ck_spu_i2c_priv_t *priv = handle;

    //initialize I2C bus
    uint32_t offset_pgpio;

    enable_pgpio(priv);

    offset_pgpio = (priv->sda_id << 4) + 8;  //SDA_CTRL
    putData(0,priv->pgpio_base,offset_pgpio);
    offset_pgpio = (priv->scl_id << 4) + 8;  //SCL_CTRL
    putData(0,priv->pgpio_base,offset_pgpio);

    disable_pgpio(priv);
    spu_disable_clock(priv->spu_idx,priv->sda_id);

    //release data
    priv->cb_event = NULL;

    return 0;
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
    ck_spu_i2c_priv_t *priv = handle;

    uint32_t ctrl=0;

    //mode
    uint32_t func_index_base;
    uint32_t offset_inst;
    uint32_t data_inst;

    func_index_base = spu_get_i2c_func_index_base(priv->spu_idx);
    offset_inst = priv->sda_id << 3;

    if(mode==IIC_MODE_MASTER){
        data_inst = (func_index_base<<16) | (priv->prog_base - priv->spu_base);
        putData(data_inst,priv->inst_base,offset_inst);
    }
    else{
        func_index_base += 64;
        data_inst = (func_index_base<<16) | (priv->prog_base - priv->spu_base);
        putData(data_inst,priv->inst_base,offset_inst);
    }

    //initialize speed
    uint32_t clk_div = 0;
    uint32_t clk_div_0;
    uint32_t clk_div_1;
    uint32_t offset_pgpio;

    priv->speed_mode = speed;

    if(mode==IIC_MODE_MASTER){
        switch(speed){
            case I2C_BUS_SPEED_STANDARD:
                clk_div = get_clock_div(I2C_FREQUENCY_STANDARD); break;
            case I2C_BUS_SPEED_FAST:
            case I2C_BUS_SPEED_HIGH:
                clk_div = get_clock_div(I2C_FREQUENCY_FAST); break;
            case I2C_BUS_SPEED_FAST_PLUS:
                clk_div = get_clock_div(I2C_FREQUENCY_FAST_PLUS); break;
        }

        clk_div |= (I2C_FILTER_COE) << 23;  //SDA CLK_DIV
        clk_div_0 = clk_div | 0x50000000;
        offset_pgpio = (priv->sda_id << 4) + 4;
        putData(clk_div_0,priv->pgpio_base,offset_pgpio);

        offset_pgpio = (priv->scl_id << 4) + 4;  //SCL CLK_DIV
        clk_div_1 = clk_div | 0x30000000;
        putData(clk_div_1,priv->pgpio_base,offset_pgpio);

        if(speed==I2C_BUS_SPEED_HIGH){
            clk_div = get_clock_div(I2C_FREQUENCY_HIGH);  //PROG_RAM:CLK_DIV_H
            ctrl = clk_div << 8;
        }
    }

    //initialize addr_mode
    priv->addr_mode = addr_mode;
    priv->slave_addr = slave_addr;

    if(mode==IIC_MODE_SLAVE){
        if(addr_mode==I2C_ADDRESS_7BIT){
            ctrl = slave_addr<<17;
        }
        else{
            ctrl = ((slave_addr & 0xFF) << 16) |
                   (((slave_addr & 0x300) | 0x7800) << 17);
        }
    }

    putData(ctrl,priv->prog_base,I2C_OFFSET_CTRL);

    //initialize variable
    priv->tx_pending = false;
    priv->rx_pending = false;

    //initialize mode
    priv->mode = mode;

    if(mode==IIC_MODE_SLAVE){
        //config: fifo_head
        uint32_t fifo_head;

        fifo_head = (I2C_S_FIFO_DEPTH << 24) | (I2C_S_TX_EMPTY_TL << 16);
        putData(fifo_head,priv->prog_base,I2C_S_OFFSET_FIFO_HEAD_W0);
        fifo_head = (priv->prog_base - priv->spu_base + I2C_S_OFFSET_FIFO_MEM) << 14;
        putData(fifo_head,priv->prog_base,I2C_S_OFFSET_FIFO_HEAD_W1);

        uint32_t pgpio_ctrl;
        offset_pgpio = (priv->sda_id << 4) + 8;  //SDA CTRL
        pgpio_ctrl = 0xB400981C | (priv->scl_id << 21);
        putData(pgpio_ctrl,priv->pgpio_base,offset_pgpio);

        offset_pgpio = (priv->scl_id << 4) + 8;  //SCL CTRL
        putData(0x0,priv->pgpio_base,offset_pgpio);
    }

    return 0;
}

/**
 \brief       Start transmitting data as I2C Master.
              This function is non-blocking,\ref iic_event_e is signaled when transfer completes or error happens.
              \ref csi_iic_get_status can indicates transmission status.
 \param[in]   handle         iic handle to operate.
 \param[in]   devaddr        iic addrress of slave device.
 \param[in]   data           data to send to I2C Slave
 \param[in]   num            Number of data items to send
 \param[in]   xfer_pending   Transfer operation is pending - Stop condition will not be generated
 \return      0 for success, negative for error code
*/
int32_t csi_iic_master_send(iic_handle_t handle, uint32_t devaddr,const void *data, uint32_t num, bool xfer_pending/*,bool asynch*/)
{
    ck_spu_i2c_priv_t *priv = handle;

    priv->read_flag = false;
    priv->data_buf = (void *)data;
    priv->total_num = num;
    priv->cur_num = 0;

    //config: ctrl
    uint32_t ctrl;

    getData(ctrl,priv->prog_base,I2C_OFFSET_CTRL);
    ctrl &= 0xFFFFFF00;

    if(priv->tx_pending){
        ctrl |= I2C_TX_CONTINUE;
    }
    else if(priv->speed_mode==I2C_BUS_SPEED_HIGH){
        ctrl |= I2C_HS_FLAG;
    }

    if(xfer_pending){
        ctrl |= I2C_PEND_EN;
    }

    putData(ctrl,priv->prog_base,I2C_OFFSET_CTRL);

    //config: INT_MASK
    putData(0,priv->prog_base,I2C_OFFSET_INT);

    //config: tx_fifo_head
    uint32_t fifo_head;

    fifo_head = (I2C_M_FIFO_DEPTH_BIG << 24) | (I2C_M_TX_EMPTY_TL << 16);
    putData(fifo_head,priv->prog_base,I2C_M_OFFSET_TX_HEAD_W0);
    fifo_head = (priv->prog_base - priv->spu_base + I2C_M_OFFSET_FIFO_MEM_BIG) << 14;
    putData(fifo_head,priv->prog_base,I2C_M_OFFSET_TX_HEAD_W1);

    //send address to tx fifo
    uint32_t slave_addr;

    if(priv->tx_pending){
        send_data(priv);
    }
    else{
        if(priv->speed_mode==I2C_BUS_SPEED_HIGH){
            slave_addr = priv->master_code | 0x08;
            write_tx_fifo(priv,slave_addr);
        }

        if(priv->addr_mode==I2C_ADDRESS_7BIT){
            slave_addr = priv->slave_addr << 1;
            write_tx_fifo(priv,slave_addr);
        }
        else{
            slave_addr = priv->slave_addr >> 8;
            slave_addr |= 0x78;
            slave_addr = slave_addr << 1;
            write_tx_fifo(priv,slave_addr);
            slave_addr = priv->slave_addr & 0xFF;
            write_tx_fifo(priv,slave_addr);
        }
    }
    //send data
    if(!priv->tx_pending){
        send_data(priv);
    }
    //start PE
    uint32_t pgpio_decode;

    pgpio_decode = (1 << priv->sda_id);
    putData(pgpio_decode,priv->global_base,OFFSET_PGPIO_START_EN);



    return 0;
}

/**
  \brief       Start receiving data as I2C Master.
               This function is non-blocking,\ref iic_event_e is signaled when transfer completes or error happens.
               \ref csi_iic_get_status can indicates transmission status.
  \param[in]   handle  iic handle to operate.
  \param[in]   devaddr        iic addrress of slave device.
  \param[out]  data    Pointer to buffer for data to receive from IIC receiver
  \param[in]   num     Number of data items to receive
  \param[in]   xfer_pending   Transfer operation is pending - Stop condition will not be generated
  \return      0 for success, negative for error code
*/
int32_t csi_iic_master_receive(iic_handle_t handle, uint32_t devaddr, void *data, uint32_t num, bool xfer_pending/*,bool asynch*/)
{
    ck_spu_i2c_priv_t *priv = handle;

    priv->read_flag = true;
    priv->data_buf = data;
    priv->total_num = num;
    priv->cur_num = 0;


    //config: ctrl
    uint32_t ctrl;

    getData(ctrl,priv->prog_base,I2C_OFFSET_CTRL);
    ctrl &= 0xFFFFFF00;

    if(priv->rx_pending){
        ctrl |= I2C_RX_CONTINUE;
    }
    else if(priv->speed_mode==I2C_BUS_SPEED_HIGH){
        ctrl |= I2C_HS_FLAG;
    }

    ctrl |= I2C_READ_FLAG;

    if(xfer_pending){
        ctrl |= I2C_PEND_EN;
    }

    putData(ctrl,priv->prog_base,I2C_OFFSET_CTRL);

    //config: TRANS_LEN
    putData(num,priv->prog_base,I2C_OFFSET_TRANS_LEN);

    //config: INT_MASK
    putData(I2C_INT_TX_EMPTY<<16,priv->prog_base,I2C_OFFSET_INT);

    //config: tx_fifo_head/rx_fifo_head
    uint32_t fifo_head;

    fifo_head = (I2C_M_FIFO_DEPTH_LITTLE << 24);
    putData(fifo_head,priv->prog_base,I2C_M_OFFSET_TX_HEAD_W0);
    fifo_head = (priv->prog_base - priv->spu_base + I2C_M_OFFSET_FIFO_MEM_LITTLE) << 14;
    putData(fifo_head,priv->prog_base,I2C_M_OFFSET_TX_HEAD_W1);

    fifo_head = (I2C_M_FIFO_DEPTH_BIG << 24) | (I2C_M_RX_FULL_TL << 16);
    putData(fifo_head,priv->prog_base,I2C_M_OFFSET_RX_HEAD_W0);
    fifo_head = (priv->prog_base - priv->spu_base + I2C_M_OFFSET_FIFO_MEM_BIG) << 14;
    putData(fifo_head,priv->prog_base,I2C_M_OFFSET_RX_HEAD_W1);

    //send address to tx fifo
    uint32_t slave_addr;
    uint32_t slave_addr2;

    if(!priv->rx_pending){
        if(priv->speed_mode==I2C_BUS_SPEED_HIGH){
            slave_addr = priv->master_code | 0x08;
            write_tx_fifo(priv,slave_addr);
        }

        if(priv->addr_mode==I2C_ADDRESS_7BIT){
            slave_addr = (priv->slave_addr << 1) | 0x1;
            write_tx_fifo(priv,slave_addr);
        }
        else{
            slave_addr = priv->slave_addr >> 8;
            slave_addr |= 0x78;
            slave_addr = slave_addr << 1;
            write_tx_fifo(priv,slave_addr);
            slave_addr2 = (priv->slave_addr & 0xFF) | I2C_SR_IN_DATA;
            write_tx_fifo(priv,slave_addr2);
            slave_addr |= 0x1;
            write_tx_fifo(priv,slave_addr);
        }
    }

    //start PE
    uint32_t pgpio_decode;

    pgpio_decode = (1 << priv->sda_id);
    putData(pgpio_decode,priv->global_base,OFFSET_PGPIO_START_EN);

    return 0;
}

/**
  \brief       Start transmitting data as I2C Slave.
               This function is non-blocking,\ref iic_event_e is signaled when transfer completes or error happens.
               drv_*_get_status can indicates transmission status.
  \param[in]   handle  iic handle to operate.
  \param[in]   data  Pointer to buffer with data to transmit to I2C Master
  \param[in]   num   Number of data items to send
  \return      0 for success, negative for error code
*/
int32_t csi_iic_slave_send(iic_handle_t handle,const void *data, uint32_t num/*,bool asynch*/)
{
    ck_spu_i2c_priv_t *priv = handle;

    priv->read_flag = false;
    priv->data_buf = (void *)data;
    priv->total_num = num;
    priv->cur_num = 0;

    //config: ctrl
    uint32_t ctrl;

    getData(ctrl,priv->prog_base,I2C_OFFSET_CTRL);
    ctrl &= 0xFFFFFF00;
    ctrl |= I2C_SLV_BUSY;
    putData(ctrl,priv->prog_base,I2C_OFFSET_CTRL);

    //config: INT_MASK
    putData(0,priv->prog_base,I2C_OFFSET_INT);

    //config: TRANS_LEN
    putData(num,priv->prog_base,I2C_OFFSET_TRANS_LEN);

    //write fifo
    send_data(priv);

    //start PE
    uint32_t pgpio_decode;
    pgpio_decode = (1 << priv->sda_id);
    putData(pgpio_decode,priv->global_base,OFFSET_PGPIO_START_EN);
    return 0;
}
/**
  \brief       Start receiving data as I2C Slave.
               This function is non-blocking,\ref iic_event_e is signaled when transfer completes or error happens.
               drv_*_get_status can indicates transmission status.
  \param[in]   handle  iic handle to operate.
  \param[out]  data  Pointer to buffer for data to receive from I2C Master
  \param[in]   num   Number of data items to receive
  \return      0 for success, negative for error code
*/
int32_t csi_iic_slave_receive(iic_handle_t handle,const void *data, uint32_t num/*,bool asynch*/)
{
    ck_spu_i2c_priv_t *priv = handle;

    priv->read_flag = true;
    priv->data_buf = (void *)data;
    priv->total_num = num;
    priv->cur_num = 0;

    //config: ctrl
    uint32_t ctrl;

    getData(ctrl,priv->prog_base,I2C_OFFSET_CTRL);
    ctrl &= 0xFFFFFF00;
    ctrl |= (I2C_SLV_BUSY | I2C_SLV_RX);
    putData(ctrl,priv->prog_base,I2C_OFFSET_CTRL);

    //config: TRANS_LEN
    uint32_t trans_len;

    if(priv->addr_mode==I2C_ADDRESS_7BIT){
        trans_len = num;
    }
    else{
        trans_len = num-1;
    }

    putData(trans_len,priv->prog_base,I2C_OFFSET_TRANS_LEN);

    //config: INT_MASK
    putData(0,priv->prog_base,I2C_OFFSET_INT);

    //start PE
    uint32_t pgpio_decode;
    pgpio_decode = (1 << priv->sda_id);
    putData(pgpio_decode,priv->global_base,OFFSET_PGPIO_START_EN);
    return 0;
}

/**
  \brief       abort transfer.
  \param[in]   handle  iic handle to operate.
  \return      error code
*/
int32_t csi_iic_abort_transfer(iic_handle_t handle)
{
    return 0;
}


/**
  \brief       Get IIC status.
  \param[in]   handle  iic handle to operate.
  \return      IIC status \ref iic_status_t
*/
iic_status_t csi_iic_get_status(iic_handle_t handle)
{
    ck_spu_i2c_priv_t *priv = handle;
    iic_status_t iic_status = {0};
    //get interrupt status
    uint32_t int_status;

    getData(int_status,priv->prog_base,I2C_OFFSET_INT);
    int_status &= 0xFF;

    //deal with interrupt
    if(priv->mode==IIC_MODE_MASTER){
        iic_status.mode=1;
        if(int_status==I2C_INT_ARB_LOST){
          iic_status.general_call=1;
        }
    }else{
        iic_status.mode=0;
        if(int_status==I2C_INT_GENE_CALL){
          iic_status.arbitration_lost=1;
        }
    }

    if(int_status==I2C_INT_DONE){
        iic_status.busy=0;
    }

    return iic_status;
}



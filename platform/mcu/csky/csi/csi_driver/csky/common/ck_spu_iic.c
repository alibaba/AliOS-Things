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
 * @file     ck_spu_iic.c
 * @brief
 * @version  V1.3
 * @date     28. February 2018
 ******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "drv_spu_iic.h"
#include "ck_spu.h"
#include "ck_spu_iic.h"
#include "spu_pin_planning.h"

#define ERR_SPU_IIC(errno) (CSI_DRV_ERRNO_IIC_BASE |  errno)

extern uint32_t SPU_IIC_PROG_BASE[];
extern int32_t spu_get_base(int32_t spu_idx);
extern int32_t spu_get_iic_func_index_base(int32_t spu_idx, int32_t arg);
extern int32_t spu_get_iic_info(int32_t dev_idx, int32_t *spu_idx, uint8_t *pgpio_scl, uint8_t *pgpio_sda);

//-------------------------------------------------
//                   data structure
//-------------------------------------------------
typedef enum {
    NACK_DURING_ADDR,   //slave answers NACK during master sending address
    NACK_DURING_DATA,   //slave answers NACK during master sending data
    NACK_NORMALLY,      //slave answers NACK after master sending all data
} ENUM_IIC_NACK_TYPE;

//Hardware configuration
typedef struct {
    int32_t spu_idx;
    uint8_t pgpio_sda;
    uint8_t pgpio_scl;
    uint32_t spu_base;
    uint32_t global_base;
    uint32_t prog_base;
    uint32_t inst_base;
    uint32_t pgpio_base;

    //call back
    iic_event_cb_t cb_event;

    //variable
    iic_address_mode_e addr_mode;
    uint32_t slave_addr;
    volatile iic_status_t status;
    bool pending_flag;

    uint8_t *data_buf;
    uint32_t total_num;
    volatile uint32_t cur_num;
} ck_spu_iic_priv_t;

static  ck_spu_iic_priv_t iic_instance[CONFIG_SPU_IIC_NUM];

static const iic_capabilities_t iic_capabilities = {
    1  /* supports 10-bit addressing */
};

//-------------------------------------------------
//                    function
//-------------------------------------------------
static uint32_t get_unit_clk(uint32_t freq)
{
    return (SPU_CLK_FREQUENCY / (freq << 1)) - 1;
}

static void write_fifo(iic_handle_t handle, uint32_t len)
{
    ck_spu_iic_priv_t *priv = handle;

    //write PROG_RAM base to GLOBAL_REG firstly
    putData(priv->prog_base - priv->spu_base, priv->global_base, OFFSET_FIFO_BADDR);
    //send data
    uint32_t offset_tx_fifo;
    uint32_t trans_data;

    priv->cur_num += len;
    offset_tx_fifo = FIFO_BASE + (priv->pgpio_sda << 3);

    while (len > 0) {
        trans_data = *(priv->data_buf);
        trans_data <<= 24;
        putData(trans_data, priv->spu_base, offset_tx_fifo);
        priv->data_buf++;
        len--;
    }
}

static void read_fifo(iic_handle_t handle, uint32_t len)
{
    ck_spu_iic_priv_t *priv = handle;

    //write PROG_RAM base to GLOBAL_REG firstly
    putData(priv->prog_base - priv->spu_base, priv->global_base, OFFSET_FIFO_BADDR);

    //receive data
    uint32_t offset_rx_fifo;
    uint32_t trans_data;

    priv->cur_num += len;
    offset_rx_fifo = FIFO_BASE + (priv->pgpio_sda << 3) + 4;

    while (len > 0) {
        getData(trans_data, priv->spu_base, offset_rx_fifo);
        *(priv->data_buf) = (uint8_t)trans_data;
        priv->data_buf++;
        len--;
    }
}


static ENUM_IIC_NACK_TYPE get_nack_type(iic_handle_t handle)
{
    ck_spu_iic_priv_t *priv = handle;

    //check tx_fifo send length
    uint32_t data_prog;
    getData(data_prog, priv->prog_base, IIC_OFFSET_TX_LEN);

    if (priv->status.direction) {
        return NACK_DURING_ADDR;
    } else if (data_prog >= priv->total_num) {
        return NACK_DURING_ADDR;
    } else if ((data_prog == 0) && (!priv->pending_flag)) {
        return NACK_NORMALLY;
    } else {
        return NACK_DURING_DATA;
    }
}

//-------------------------------------------------
//                      ISR
//-------------------------------------------------
void ck_spu_iic_irqhandler(int32_t idx, spu_dev_signal_e signal)
{
    ck_spu_iic_priv_t *priv = &iic_instance[idx];

    //get interrupt status
    uint32_t int_status;
    uint32_t bytes_to_trans;
    uint32_t trans_len;
    ENUM_IIC_NACK_TYPE nack_type;
    uint32_t undone_len;
    iic_event_e event = 0;
    bool event_vld = true;

    getData(int_status, priv->prog_base, IIC_OFFSET_INT);
    int_status &= 0xFF;

    if (priv->status.mode == IIC_MODE_MASTER) {
        switch (int_status) {
            case IIC_INT_NACK:
                priv->pending_flag = false;
                priv->status.busy = 0;
                nack_type = get_nack_type(priv);

                if (nack_type == NACK_DURING_ADDR) {
                    event = IIC_EVENT_ADDRESS_NACK;
                } else if (nack_type == NACK_DURING_DATA) {
                    event = IIC_EVENT_TRANSFER_INCOMPLETE;
                } else if (nack_type == NACK_NORMALLY) {
                    event = IIC_EVENT_TRANSFER_DONE;
                }

                break;

            case IIC_INT_DONE:
                priv->status.busy = 0;

                if (priv->status.direction) { //receive
                    bytes_to_trans = priv->total_num - priv->cur_num;
                    read_fifo(priv, bytes_to_trans);
                }

                event = IIC_EVENT_TRANSFER_DONE;
                break;

            case IIC_INT_PENDING:
                if (priv->status.direction) { //receive
                    bytes_to_trans = priv->total_num - priv->cur_num;
                    read_fifo(priv, bytes_to_trans);
                }

                event = IIC_EVENT_TRANSFER_DONE;
                break;

            case IIC_INT_ARB_LOST:
                priv->pending_flag = false;
                priv->status.busy = 0;
                priv->status.arbitration_lost = 1;
                event = IIC_EVENT_ARBITRATION_LOST;
                break;

            default:

                event_vld = false;
                getData(int_status, priv->prog_base, IIC_OFFSET_FIFO_STATUS);

                if (priv->status.direction) { //receive
                    if ((int_status & IIC_INT_RX_ALFULL) != 0) {
                        //read_fifo(priv,IIC_RX_FIFO_TL+1);
                        read_fifo(priv, IIC_RX_FIFO_TL);
                    }

                    break;
                } else {
                    if ((int_status & IIC_INT_TX_ALEMPTY) != 0) {
                        bytes_to_trans = priv->total_num - priv->cur_num;

                        if (bytes_to_trans <= (IIC_FIFO_DEPTH - IIC_TX_FIFO_TL + 1)) {
                            trans_len = bytes_to_trans;
                            //close interrupt
                            int_status |= 0x40000;
                            putData(int_status, priv->prog_base, IIC_OFFSET_FIFO_STATUS);
                        } else {
                            trans_len = IIC_FIFO_DEPTH - IIC_TX_FIFO_TL + 1;
                        }

                        write_fifo(priv, trans_len);
                    }
                }
        }
    }

    if (priv->status.mode == IIC_MODE_SLAVE) {
        switch (int_status) {
            case IIC_INT_GENE_CALL:
                priv->status.general_call = 1;
                event = IIC_EVENT_GENERAL_CALL;
                break;

            case IIC_INT_SLAVE_RX:
                priv->status.general_call = 0;
                event = IIC_EVENT_SLAVE_RECEIVE;
                break;

            case IIC_INT_SLAVE_TX:
                priv->status.general_call = 0;
                event = IIC_EVENT_SLAVE_TRANSMIT;
                break;

            case IIC_INT_DONE:
                priv->status.busy = 0;
                getData(undone_len, priv->prog_base, IIC_OFFSET_LEN);

                if (priv->status.direction) { //receive
                    bytes_to_trans = priv->total_num - priv->cur_num - undone_len;
                    read_fifo(priv, bytes_to_trans);
                }

                if (undone_len != 0) {
                    event = IIC_EVENT_TRANSFER_INCOMPLETE;
                    break;
                } else {
                    event = IIC_EVENT_TRANSFER_DONE;
                    break;
                }

            default:
                event_vld = false;
                getData(int_status, priv->prog_base, IIC_OFFSET_FIFO_STATUS);

                if (priv->status.direction) { //receive
                    if ((int_status & IIC_INT_RX_ALFULL) != 0) {
                        //read_fifo(priv,IIC_RX_FIFO_TL+1);
                        read_fifo(priv, IIC_RX_FIFO_TL);
                    }

                    break;
                } else {
                    if ((int_status & IIC_INT_TX_ALEMPTY) != 0) {
                        bytes_to_trans = priv->total_num - priv->cur_num;

                        if (bytes_to_trans <= (IIC_FIFO_DEPTH - IIC_TX_FIFO_TL + 1)) {
                            trans_len = bytes_to_trans;
                            //close interrupt
                            int_status |= 0x40000;
                            putData(int_status, priv->prog_base, IIC_OFFSET_FIFO_STATUS);
                        } else {
                            trans_len = IIC_FIFO_DEPTH - IIC_TX_FIFO_TL + 1;
                        }

                        write_fifo(priv, trans_len);
                    }
                }
        }
    }

    if (priv->cb_event && event_vld) {
        priv->cb_event(idx, event);
    }

}
//-------------------------------------------------
//                     CSI
//-------------------------------------------------
/**
  \brief       Initialize IIC Interface. 1. Initializes the resources needed for the IIC interface 2.registers event callback function
  \param[in]   handle  iic handle to operate.
  \param[in]   cb_event  Pointer to \ref iic_event_cb_t
  \return      error code
*/
iic_handle_t drv_spu_iic_initialize(int32_t idx, iic_event_cb_t cb_event)
{
    //iic instace
    int32_t spu_idx;
    uint8_t pgpio_sda;
    uint8_t pgpio_scl;
    ck_spu_iic_priv_t *priv;

    int32_t ret = spu_get_iic_info(idx, &spu_idx, &pgpio_scl, &pgpio_sda);

    if (ret < 0 || ret >= CONFIG_SPU_IIC_NUM) {
        return NULL;
    }

    priv = &iic_instance[idx];
    priv->spu_idx = spu_idx;
    priv->pgpio_scl = pgpio_scl;
    priv->pgpio_sda = pgpio_sda;

    //base address
    int32_t apply_prog;

    priv->spu_base = spu_get_base(spu_idx);
    priv->global_base = priv->spu_base + GLOBAL_BASE;
    priv->pgpio_base = priv->spu_base + PGPIO_BASE;
    priv->inst_base = priv->spu_base + INST_BASE;


    apply_prog = SPU_IIC_PROG_BASE[idx];

    if (apply_prog == -1) {
        return NULL;
    }

    priv->prog_base = priv->spu_base + PROG_BASE + apply_prog;
    //callback
    priv->cb_event = cb_event;

    return priv;
}

/**
  \brief       De-initialize IIC Interface. stops operation and releases the software resources used by the interface
  \param[in]   handle  iic handle to operate.
  \return      error code
*/
int32_t drv_spu_iic_uninitialize(iic_handle_t handle)
{
    ck_spu_iic_priv_t *priv = handle;
    uint32_t offset_pgpio;

    offset_pgpio = (priv->pgpio_scl << 4) + 4;
    putData(0, priv->pgpio_base, offset_pgpio);
    offset_pgpio = (priv->pgpio_sda << 4) + 4;
    putData(0, priv->pgpio_base, offset_pgpio);

    priv->cb_event = NULL;

    return 0;
}

/**
  \brief       Get driver capabilities.
  \return      \ref iic_capabilities_t
*/
iic_capabilities_t drv_spu_iic_get_capabilities(int32_t idx)
{
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
int32_t drv_spu_iic_config(iic_handle_t handle,
                           iic_mode_e mode,
                           iic_speed_e speed,
                           iic_address_mode_e addr_mode,
                           int32_t slave_addr)
{
    ck_spu_iic_priv_t *priv = handle;
    priv->status.mode = mode;
    priv->addr_mode = addr_mode;
    priv->slave_addr = slave_addr;

    //====INST_RAM
    uint32_t func_index_base;
    uint32_t offset_inst;
    uint32_t data_inst;
    uint32_t data_inst2;

    if (mode > IIC_MODE_SLAVE) {
        return ERR_SPU_IIC(DRV_ERROR_PARAMETER);
    }

    if (mode == IIC_MODE_MASTER) {
        func_index_base = spu_get_iic_func_index_base(priv->spu_idx, 0);
    } else {
        func_index_base = spu_get_iic_func_index_base(priv->spu_idx, 1);
    }

    if (addr_mode > IIC_ADDRESS_10BIT) {
        return ERR_SPU_IIC(DRV_ERROR_PARAMETER);
    }

    if (speed == IIC_BUS_SPEED_FAST_PLUS || speed == IIC_BUS_SPEED_HIGH) {
        return ERR_SPU_IIC(DRV_ERROR_UNSUPPORTED);
    }

    offset_inst = (priv->pgpio_scl << 3);
    data_inst = (func_index_base << 16) | (priv->prog_base - priv->spu_base);
    putData(data_inst, priv->inst_base, offset_inst);
    offset_inst += 4;
    data_inst2 = (priv->pgpio_sda << 4) | (priv->pgpio_scl);
    putData(data_inst2, priv->inst_base, offset_inst);

    offset_inst = (priv->pgpio_sda << 3);
    putData(data_inst, priv->inst_base, offset_inst);
    offset_inst += 4;
    putData(data_inst2, priv->inst_base, offset_inst);
    //====PGPIO
    uint32_t unit_clk = 0u;
    uint32_t offset_pgpio;
    uint32_t data_pgpio;
    uint32_t iic_slave_addr;

    offset_pgpio = (priv->pgpio_scl << 4) + 4;

    if (speed == IIC_BUS_SPEED_STANDARD) {
        unit_clk = get_unit_clk(IIC_FREQUENCY_STANDARD);
    } else {
        unit_clk = get_unit_clk(IIC_FREQUENCY_FAST);
    }

    if (mode == IIC_MODE_MASTER) {
        putData(0x0, priv->pgpio_base, offset_pgpio);
        offset_pgpio += 4;
        data_pgpio = unit_clk | 0x20000;
        putData(data_pgpio, priv->pgpio_base, offset_pgpio);
        data_pgpio |= 0x3000;
        putData(data_pgpio, priv->pgpio_base, offset_pgpio);

        offset_pgpio = (priv->pgpio_sda << 4) + 4;

        putData(0x0, priv->pgpio_base, offset_pgpio);
        offset_pgpio += 4;
        data_pgpio = unit_clk | 0x20000;
        putData(data_pgpio, priv->pgpio_base, offset_pgpio);
        data_pgpio |= 0x3000;
        putData(data_pgpio, priv->pgpio_base, offset_pgpio);
    } else {
        //write SLAVE ADDRESS
        if (addr_mode == IIC_ADDRESS_7BIT) {
            iic_slave_addr = slave_addr;
            putData(slave_addr, priv->prog_base, IIC_OFFSET_SLAVE_ADDR);
        } else if (addr_mode == IIC_ADDRESS_10BIT) {
            iic_slave_addr = (slave_addr >> 8) | 0x78;
            iic_slave_addr |= (slave_addr & 0xff) << 8;
            putData(iic_slave_addr, priv->prog_base, IIC_OFFSET_SLAVE_ADDR);
            putData(IIC_RX_FIFO_HEAD, priv->prog_base, IIC_OFFSET_RX_FIFO_HEAD);
        } else {
            return ERR_SPU_IIC(DRV_ERROR_PARAMETER);
        }

        //start PE
        putData(0x0, priv->pgpio_base, offset_pgpio);
        offset_pgpio += 4;
        data_pgpio = unit_clk | 0x1000;
        putData(data_pgpio, priv->pgpio_base, offset_pgpio);

        offset_pgpio = (priv->pgpio_sda << 4) + 4;

        putData(0x3C00, priv->pgpio_base, offset_pgpio);
        offset_pgpio += 4;
        data_pgpio = unit_clk | 0x9000;
        putData(data_pgpio, priv->pgpio_base, offset_pgpio);
    }

    //variable
    priv->pending_flag = false;

    return 0;
}

/**
  \brief       Start transmitting data as IIC Master.
               This function is non-blocking,\ref iic_event_e is signaled when transfer completes or error happens.
               drv_*_get_status can indicates transmission status.
  \param[in]   handle         iic hand      *<D> /home/guomk/SPU_V2/rtl/pgpio/pgpio.v(582): r_tx <= 1'b0;
le to operate.
  \param[in]   data           data to send to IIC Slave
  \param[in]   num            Number of data items to send
  \param[in]   xfer_pending   Transfer operation is pending - Stop condition will not be generated
  \return      0 for success, negative for error code
*/
int32_t drv_spu_iic_master_send(iic_handle_t handle, uint32_t devaddr, const void *data, uint32_t num, bool xfer_pending)
//int32_t drv_spu_iic_master_send(iic_handle_t handle,const void *data, uint32_t num, bool xfer_pending/*,bool asynch*/)
{
    ck_spu_iic_priv_t *priv = handle;

    priv->data_buf = (void *)data;

    if (priv->pending_flag) {
        priv->total_num += num;
    } else {
        priv->total_num = num;
        priv->cur_num = 0;
    }

    //====PROG_RAM
    //tx fifo head
    putData(IIC_TX_FIFO_HEAD, priv->prog_base, IIC_OFFSET_TX_FIFO_HEAD);

    //fifo status
    putData(0xffff0000, priv->prog_base, IIC_OFFSET_FIFO_STATUS);

    uint32_t addr_len;
    uint32_t total_len;

    if (priv->pending_flag) {
        addr_len = 0;
    } else if (priv->addr_mode == IIC_ADDRESS_7BIT) {
        addr_len = 1;
    } else {
        addr_len = 2;
    }

    total_len = num + addr_len;


    //int
    putData(0x0, priv->prog_base, IIC_OFFSET_INT);

    //ctrl
    uint32_t ctrl = 0;

    if (priv->pending_flag) {
        ctrl |= IIC_CTRL_CONTINUE;
    }

    if (xfer_pending) {
        ctrl |= IIC_CTRL_PEND;
    }

    putData(ctrl, priv->prog_base, IIC_OFFSET_CTRL);

    //tx_len
    putData(total_len, priv->prog_base, IIC_OFFSET_TX_LEN);

    //send slave address
    uint32_t offset_tx_fifo;
    uint32_t slave_addr;

    if (!priv->pending_flag) {
        putData(priv->prog_base - priv->spu_base, priv->global_base, OFFSET_FIFO_BADDR);
        offset_tx_fifo = FIFO_BASE + (priv->pgpio_sda << 3);

        if (priv->addr_mode == IIC_ADDRESS_7BIT) {
            slave_addr = priv->slave_addr << 25;
            putData(slave_addr, priv->spu_base, offset_tx_fifo);
        } else {
            slave_addr = priv->slave_addr >> 8;
            slave_addr |= 0x78;
            slave_addr = slave_addr << 25;
            putData(slave_addr, priv->spu_base, offset_tx_fifo);
            slave_addr = priv->slave_addr << 24;
            putData(slave_addr, priv->spu_base, offset_tx_fifo);
        }
    }

    //send data
    uint32_t send_len;

    if ((IIC_FIFO_DEPTH + 1) >= (total_len)) {
        write_fifo(priv, num);
    } else {
        send_len = IIC_FIFO_DEPTH + 1 - addr_len;
        write_fifo(priv, send_len);
        putData(0xff000000, priv->prog_base, IIC_OFFSET_FIFO_STATUS);
    }

    //start PE
    uint32_t data_pgpio;
    uint32_t offset_pgpio;
    offset_pgpio = (priv->pgpio_sda << 4) + 4;
    putData(0x3C00, priv->pgpio_base, offset_pgpio);
    offset_pgpio += 4;
    getData(data_pgpio, priv->pgpio_base, offset_pgpio);
    data_pgpio |= 0x8000;
    putData(data_pgpio, priv->pgpio_base, offset_pgpio);
    //variable
    priv->status.busy = 1;
    priv->status.direction = 0;
    priv->status.general_call = 0;
    priv->status.arbitration_lost = 0;
    priv->status.bus_error = 0;

    priv->pending_flag = xfer_pending;

    return 0;
}

/**
  \brief       Start receiving data as IIC Master.
               This function is non-blocking,\ref iic_event_e is signaled when transfer completes or error happens.
               drv_*_get_status can indicates transmission status.
  \param[in]   handle  iic handle to operate.
  \param[out]  data    Pointer to buffer for data to receive from IIC receiver
  \param[in]   num     Number of data items to receive
  \param[in]   xfer_pending   Transfer operation is pending - Stop condition will not be generated
  \return      0 for success, negative for error code
*/
//int32_t drv_spu_iic_master_receive(iic_handle_t handle, const void *data, uint32_t num, bool xfer_pending)
int32_t drv_spu_iic_master_receive(iic_handle_t handle, uint32_t devaddr, void *data, uint32_t num, bool xfer_pending)
{
    ck_spu_iic_priv_t *priv = handle;

    priv->data_buf = (void *)data;

    if (priv->pending_flag) {
        priv->total_num += num;
    } else {
        priv->total_num = num;
        priv->cur_num = 0;
    }

    //====PROG_RAM
    //tx,rx fifo head
    putData(IIC_TX_FIFO_HEAD, priv->prog_base, IIC_OFFSET_TX_FIFO_HEAD);
    putData(IIC_RX_FIFO_HEAD, priv->prog_base, IIC_OFFSET_RX_FIFO_HEAD);

    //fifo status
    if ((IIC_FIFO_DEPTH + 1) >= num) {
        putData(0xffff0000, priv->prog_base, IIC_OFFSET_FIFO_STATUS);
    } else {
        putData(0xff0000, priv->prog_base, IIC_OFFSET_FIFO_STATUS);
    }

    //int
    putData(0x0, priv->prog_base, IIC_OFFSET_INT);

    //ctrl
    uint32_t ctrl = IIC_CTRL_READ;

    if (priv->pending_flag) {
        ctrl |= (IIC_CTRL_CONTINUE | IIC_CTRL_CONT_RX);
    }

    if (xfer_pending) {
        ctrl |= IIC_CTRL_PEND;
    }

    putData(ctrl, priv->prog_base, IIC_OFFSET_CTRL);

    //tx_len
    //send slave address
    uint32_t offset_tx_fifo;
    uint32_t slave_addr;
    uint32_t slave_addr2;

    if (!priv->pending_flag) {
        putData(priv->prog_base - priv->spu_base, priv->global_base, OFFSET_FIFO_BADDR);
        offset_tx_fifo = FIFO_BASE + (priv->pgpio_sda << 3);

        if (priv->addr_mode == IIC_ADDRESS_7BIT) {
            putData(1, priv->prog_base, IIC_OFFSET_TX_LEN);
            slave_addr = priv->slave_addr << 25;
            slave_addr |= 0x1000000;
            putData(slave_addr, priv->spu_base, offset_tx_fifo);
        } else {
            putData(3, priv->prog_base, IIC_OFFSET_TX_LEN);
            slave_addr = priv->slave_addr >> 8;
            slave_addr |= 0x78;
            slave_addr = slave_addr << 25;
            putData(slave_addr, priv->spu_base, offset_tx_fifo);

            slave_addr2 = priv->slave_addr << 24;
            slave_addr2 |= 0x1; //sr flag
            putData(slave_addr2, priv->spu_base, offset_tx_fifo);

            slave_addr |= 0x1000000;  //read flag
            putData(slave_addr, priv->spu_base, offset_tx_fifo);
        }
    }

    //rx_len
    putData(num, priv->prog_base, IIC_OFFSET_RX_LEN);

    //start PE
    uint32_t data_pgpio;
    uint32_t offset_pgpio;
    offset_pgpio = (priv->pgpio_sda << 4) + 4;
    putData(0x3C00, priv->pgpio_base, offset_pgpio);
    offset_pgpio += 4;
    getData(data_pgpio, priv->pgpio_base, offset_pgpio);
    data_pgpio |= 0x8000;
    putData(data_pgpio, priv->pgpio_base, offset_pgpio);

    //variable
    priv->status.busy = 1;
    priv->status.direction = 1;
    priv->status.general_call = 0;
    priv->status.arbitration_lost = 0;
    priv->status.bus_error = 0;

    priv->pending_flag = xfer_pending;

    return 0;
}

///**
//  \brief       Start transmitting data as IIC Slave.
//               This function is non-blocking,\ref iic_event_e is signaled when transfer completes or error happens.
//               drv_*_get_status can indicates transmission status.
//  \param[in]   handle  iic handle to operate.
//  \param[in]   data  Pointer to buffer with data to transmit to IIC Master
//  \param[in]   num   Number of data items to send
//  \return      0 for success, negative for error code
//*/

int32_t drv_spu_iic_slave_send(iic_handle_t handle, const void *data, uint32_t num/*,bool asynch*/)
{
    ck_spu_iic_priv_t *priv = handle;

    priv->data_buf = (void *)data;
    priv->total_num = num;
    priv->cur_num = 0;

    //tx fifo head
    putData(IIC_TX_FIFO_HEAD, priv->prog_base, IIC_OFFSET_TX_FIFO_HEAD);

    //fifo status
    putData(0xffff0000, priv->prog_base, IIC_OFFSET_FIFO_STATUS);

    //int
    putData(0x0, priv->prog_base, IIC_OFFSET_INT);

    //ctrl
    putData(IIC_CTRL_SLAVE_TX, priv->prog_base, IIC_OFFSET_CTRL);

    //tx_len
    putData(num, priv->prog_base, IIC_OFFSET_LEN);

    //send data
    uint32_t send_len;


    if ((IIC_FIFO_DEPTH + 1) >= num) {
        write_fifo(priv, num);
    } else {
        send_len = IIC_FIFO_DEPTH + 1;
        write_fifo(priv, send_len);
        putData(0xff000000, priv->prog_base, IIC_OFFSET_FIFO_STATUS);
    }

    //start PE
    uint32_t data_pgpio;
    uint32_t offset_pgpio;
    offset_pgpio = (priv->pgpio_sda << 4) + 4;
    putData(0x800, priv->pgpio_base, offset_pgpio);
    offset_pgpio += 4;
    getData(data_pgpio, priv->pgpio_base, offset_pgpio);
    data_pgpio |= 0x8000;
    putData(data_pgpio, priv->pgpio_base, offset_pgpio);

    //variable
    priv->status.busy = 1;
    priv->status.direction = 0;
    priv->status.arbitration_lost = 0;
    priv->status.bus_error = 0;

    return 0;
}

/**
  \brief       Start receiving data as IIC Slave.
               This function is non-blocking,\ref iic_event_e is signaled when transfer completes or error happens.
               drv_*_get_status can indicates transmission status.
  \param[in]   handle  iic handle to operate.
  \param[out]  data  Pointer to buffer for data to receive from IIC Master
  \param[in]   num   Number of data items to receive
  \return      0 for success, negative for error code
*/
//int32_t drv_spu_iic_slave_receive(iic_handle_t handle,const void *data, uint32_t num/*,bool asynch*/)
int32_t drv_spu_iic_slave_receive(iic_handle_t handle, void *data, uint32_t num)
{
    ck_spu_iic_priv_t *priv = handle;

    priv->data_buf = (void *)data;
    priv->total_num = num;
    priv->cur_num = 0;

    //rx fifo head
    //rx_len
    if (priv->addr_mode == IIC_ADDRESS_7BIT) {
        putData(IIC_RX_FIFO_HEAD, priv->prog_base, IIC_OFFSET_RX_FIFO_HEAD);
        putData(num, priv->prog_base, IIC_OFFSET_LEN);
    } else {
        putData(num - 1, priv->prog_base, IIC_OFFSET_LEN);
    }

    //fifo status
    if ((IIC_FIFO_DEPTH + 1) >= num) {
        putData(0xffff0000, priv->prog_base, IIC_OFFSET_FIFO_STATUS);
    } else {
        putData(0xff0000, priv->prog_base, IIC_OFFSET_FIFO_STATUS);
    }

    //int
    putData(0x0, priv->prog_base, IIC_OFFSET_INT);

    //config: ctrl
    putData(IIC_CTRL_SLAVE_RX, priv->prog_base, IIC_OFFSET_CTRL);


    //start PE
    uint32_t data_pgpio;
    uint32_t offset_pgpio;
    offset_pgpio = (priv->pgpio_sda << 4) + 4;
    putData(0x800, priv->pgpio_base, offset_pgpio);
    offset_pgpio += 4;
    getData(data_pgpio, priv->pgpio_base, offset_pgpio);
    data_pgpio |= 0x8000;
    putData(data_pgpio, priv->pgpio_base, offset_pgpio);

    //variable
    priv->status.busy = 1;
    priv->status.direction = 1;
    priv->status.arbitration_lost = 0;
    priv->status.bus_error = 0;

    return 0;
}

/**
  \brief       abort transfer.
  \param[in]   handle  iic handle to operate.
  \return      error code
*/
int32_t drv_spu_iic_abort_transfer(iic_handle_t handle)
{
    ck_spu_iic_priv_t *priv = handle;

    while (priv->status.busy);

    return 0;
}


/**
  \brief       Get IIC status.
  \param[in]   handle  iic handle to operate.
  \return      IIC status \ref iic_status_t
*/
iic_status_t drv_spu_iic_get_status(iic_handle_t handle)
{
    ck_spu_iic_priv_t *priv = handle;
    return priv->status;
}


/**
  \brief       control IIC power.
  \param[in]   handle  iic handle to operate.
  \param[in]   state   power state.\ref drv_spu_power_stat_e.
  \return      error code
*/
int32_t drv_spu_iic_power_control(iic_handle_t handle, csi_power_stat_e state)
{
    return ERR_SPU_IIC(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       config iic mode.
  \param[in]   handle  iic handle to operate.
  \param[in]   mode      \ref iic_mode_e.if negative, then this attribute not changed
  \return      error code
*/
int32_t drv_spu_iic_config_mode(iic_handle_t handle, iic_mode_e mode)
{
    return ERR_SPU_IIC(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       config iic speed.
  \param[in]   handle  iic handle to operate.
  \param[in]   speed     \ref iic_speed_e.if negative, then this attribute not changed
  \return      error code
*/
int32_t drv_spu_iic_config_speed(iic_handle_t handle, iic_speed_e speed)
{
    return ERR_SPU_IIC(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       config iic address mode.
  \param[in]   handle  iic handle to operate.
  \param[in]   addr_mode \ref iic_address_mode_e.if negative, then this attribute not changed
  \return      error code
*/
int32_t drv_spu_iic_config_addr_mode(iic_handle_t handle, iic_address_mode_e addr_mode)
{
    return ERR_SPU_IIC(DRV_ERROR_UNSUPPORTED);
}


/**
  \brief       config iic slave address.
  \param[in]   handle  iic handle to operate.
  \param[in]   slave_addr slave address.if negative, then this attribute not changed
  \return      error code
*/
int32_t drv_spu_iic_config_slave_addr(iic_handle_t handle, int32_t slave_addr)
{
    return ERR_SPU_IIC(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       Get IIC transferred data count.
  \param[in]   handle  iic handle to operate.
  \return      the number of the currently transferred data items
*/
uint32_t drv_spu_iic_get_data_count(iic_handle_t handle)
{
    ck_spu_iic_priv_t *priv = handle;
    return priv->cur_num;
}

/**
  \brief       Send START command.
  \param[in]   handle  iic handle to operate.
  \return      error code
*/
int32_t drv_spu_iic_send_start(iic_handle_t handle)
{
    return ERR_SPU_IIC(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       Send STOP command.
  \param[in]   handle  iic handle to operate.
  \return      error code
*/
int32_t drv_spu_iic_send_stop(iic_handle_t handle)
{
    return ERR_SPU_IIC(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       Reset IIC peripheral.
  \param[in]   handle  iic handle to operate.
  \return      error code
*/
int32_t drv_spu_iic_reset(iic_handle_t handle)
{
    return ERR_SPU_IIC(DRV_ERROR_UNSUPPORTED);
}


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
 * @file     ck_spu_spi.c
 * @brief    CSI Source File for SPU spi Driver
 * @version  V1.3
 * @date
 ******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "drv_spu_spi.h"
#include "ck_spu.h"
#include "ck_spu_spi.h"
#include "spu_pin_planning.h"

//#define ERR_SPU_SPI(errno) (CSI_DRV_ERRNO_SPU_BASE | ERRNO_SPU_SPI_OFFSET | errno)
#define ERR_SPU_SPI(errno) (CSI_DRV_ERRNO_SPI_BASE | errno)

extern uint32_t SPU_SPI_PROG_BASE[];
extern int32_t spu_get_base(int32_t spu_idx);
extern int32_t spu_get_spi_func_index_base(int32_t spu_idx, int32_t arg);
extern int32_t spu_get_spi_info(int32_t dev_idx, int32_t *spu_idx, uint8_t *pgpio_mosi, uint8_t *pgpio_miso, uint8_t *pgpio_sclk, uint8_t *pgpio_ssel);
void target_spu_spi_cs_init(int32_t spu_idx, uint8_t cs_port);
//-------------------------------------------------
//                   data structure
//-------------------------------------------------
typedef struct {
    //hardware configuration
    int32_t spu_idx;
    uint8_t pgpio_sclk;
    uint8_t pgpio_ssel;
    uint8_t pgpio_mosi;
    uint8_t pgpio_miso;
    uint32_t spu_base;
    uint32_t global_base;
    uint32_t pgpio_base;
    uint32_t inst_base;
    uint32_t prog_base;

    //call back
    spi_event_cb_t cb_event;

    //spi mode
    spi_mode_e mode;
    spi_format_e format;
    spi_bit_order_e order;
    int32_t bit_width;
    uint32_t default_value;

    //variable
    uint8_t trans_type;  //0=tx;1=rx;2=transfer
    volatile spi_status_t status;

    void *tx_data_buf;
    uint32_t tx_total_num;
    volatile uint32_t tx_cur_num;

    void *rx_data_buf;
    uint32_t rx_total_num;
    volatile uint32_t rx_cur_num;
} ck_spu_spi_priv_t;

static ck_spu_spi_priv_t spi_instance[CONFIG_SPU_SPI_NUM];

static const spi_capabilities_t spi_capabilities = {
    1, /* Simplex Mode (Master and Slave) */
    0, /* TI Synchronous Serial Interface */
    0, /* Microwire Interface */
    0  /* Signal Mode Fault event: \ref CSKY_SPI_EVENT_MODE_FAULT */
};

//-------------------------------------------------
//                    function
//-------------------------------------------------
static uint32_t get_unit_clk(uint32_t baud)
{
    return (SPU_CLK_FREQUENCY / (baud << 1) - 1);
}

static void write_fifo(spi_handle_t handle, uint32_t len)
{
    ck_spu_spi_priv_t *priv = handle;

    //write PROG_RAM base to GLOBAL_REG firstly
    putData(priv->prog_base - priv->spu_base, priv->global_base, OFFSET_FIFO_BADDR);

    //send data
    uint32_t offset_tx_fifo;
    uint32_t trans_data;
    uint32_t shift_bits = 0;

    priv->tx_cur_num += len;
    offset_tx_fifo = FIFO_BASE + (priv->pgpio_ssel << 3);

    if (priv->order == SPI_ORDER_MSB2LSB) {
        shift_bits = 32 - priv->bit_width;
    }

    if (priv->bit_width > 8) {
        while (len > 0) {
            trans_data = *(volatile uint16_t *)(priv->tx_data_buf);
            trans_data <<= shift_bits;
            putData(trans_data, priv->spu_base, offset_tx_fifo);
            priv->tx_data_buf = (uint16_t *)(priv->tx_data_buf) + 1;
            len--;
        }
    } else {
        while (len > 0) {
            trans_data = *(volatile uint8_t *)(priv->tx_data_buf);
            trans_data <<= shift_bits;
            putData(trans_data, priv->spu_base, offset_tx_fifo);
            priv->tx_data_buf = (uint8_t *)(priv->tx_data_buf) + 1;
            len--;
        }
    }
}

static void read_fifo(spi_handle_t handle, uint32_t len)
{
    ck_spu_spi_priv_t *priv = handle;

    //write PROG_RAM base to GLOBAL_REG firstly
    putData(priv->prog_base - priv->spu_base, priv->global_base, OFFSET_FIFO_BADDR);

    //receive data
    uint32_t offset_rx_fifo;
    uint32_t trans_data;
    uint32_t shift_bits = 0;

    priv->rx_cur_num += len;
    offset_rx_fifo = FIFO_BASE + (priv->pgpio_ssel << 3) + 4;

    if (priv->order == SPI_ORDER_LSB2MSB) {
        shift_bits = 32 - priv->bit_width;
    }

    if (priv->bit_width > 8) {
        while (len > 0) {
            getData(trans_data, priv->spu_base, offset_rx_fifo);
            trans_data >>= shift_bits;
            *((volatile uint16_t *)(priv->rx_data_buf)) = (uint16_t)trans_data;
            priv->rx_data_buf = (uint16_t *)(priv->rx_data_buf) + 1;
            len--;
        }
    } else {
        while (len > 0) {
            getData(trans_data, priv->spu_base, offset_rx_fifo);
            trans_data >>= shift_bits;
            *((volatile uint8_t *)(priv->rx_data_buf)) = (uint8_t)trans_data;
            priv->rx_data_buf = (uint8_t *)(priv->rx_data_buf) + 1;
            len--;
        }
    }
}

//-------------------------------------------------
//                     IRQ
//-------------------------------------------------
void ck_spu_spi_irqhandler(int32_t idx, spu_dev_signal_e signal)
{
    ck_spu_spi_priv_t *priv = &spi_instance[idx];
    spi_event_e event = 0;
    bool event_vld = false;
    uint32_t int_status;
    uint32_t bytes_to_trans;
    uint32_t trans_len;

    getData(int_status, priv->prog_base, SPI_OFFSET_INT);

    if ((int_status & SPI_INT_TRANS_DONE) != 0) {
        event_vld = true;

        if (priv->trans_type == 0) { //TX
            priv->status.busy = 0;
            event = SPI_EVENT_TX_COMPLETE;
        } else if (priv->trans_type == 1) { //RX
            bytes_to_trans = priv->rx_total_num - priv->rx_cur_num;
            read_fifo(priv, bytes_to_trans);
            priv->status.busy = 0;
            event = SPI_EVENT_RX_COMPLETE;
        } else if (priv->trans_type == 2) { //transfer
            bytes_to_trans = priv->rx_total_num - priv->rx_cur_num;
            read_fifo(priv, bytes_to_trans);
            priv->status.busy = 0;
            event = SPI_EVENT_TRANSFER_COMPLETE;
        }
    } else {
        getData(int_status, priv->prog_base, SPI_OFFSET_FIFO_STATUS);

        if ((int_status & SPI_INT_TX_UNDERFLOW) != 0 || (int_status & SPI_INT_RX_OVERFLOW) != 0) {
            priv->status.busy = 0;
            event_vld = true;
            event = SPI_EVENT_DATA_LOST;
        } else {
            if ((int_status & SPI_INT_TX_ALEMPTY) != 0) {
                bytes_to_trans = priv->tx_total_num - priv->tx_cur_num;

                if (bytes_to_trans <= (SPI_TX_FIFO_DEPTH - SPI_TX_FIFO_TL + 1)) {
                    trans_len = bytes_to_trans;
                    int_status |= 0xffff0000; //close interrupt
                    putData(int_status, priv->prog_base, SPI_OFFSET_FIFO_STATUS);
                } else {
                    trans_len = SPI_TX_FIFO_DEPTH - SPI_TX_FIFO_TL + 1;
                }

                write_fifo(priv, trans_len);
            }

            if ((int_status & SPI_INT_RX_ALFULL) != 0) {
                //read_fifo(priv,SPI_RX_FIFO_TL+1);
                read_fifo(priv, SPI_RX_FIFO_TL);
            }
        }
    }

    if (priv->cb_event && event_vld) {
        priv->cb_event(idx, event);
    }
}

//-------------------------------------------------
//                     API
//-------------------------------------------------
/**
  \brief       Initialize SPI Interface. 1. Initializes the resources needed for the SPI interface 2.registers event callback function
  \param[in]   idx spi index
  \param[in]   cb_event  event callback function \ref spi_event_cb_t
  \return      return spi handle if success
*/
spi_handle_t drv_spu_spi_initialize(int32_t idx, spi_event_cb_t cb_event)
{
    //spi instance
    int32_t spi_idx;
    int32_t spu_idx;
    uint8_t pgpio_sclk;
    uint8_t pgpio_mosi;
    uint8_t pgpio_miso;
    uint8_t pgpio_ssel;
    ck_spu_spi_priv_t *priv;

    spi_idx = spu_get_spi_info(idx, &spu_idx, &pgpio_mosi, &pgpio_miso, &pgpio_sclk, &pgpio_ssel);

    if (spi_idx < 0) {
        return NULL;
    }

    priv = &spi_instance[spi_idx];
    priv->spu_idx = spu_idx;
    priv->pgpio_sclk = pgpio_sclk;
    priv->pgpio_mosi = pgpio_mosi;
    priv->pgpio_miso = pgpio_miso;
    priv->pgpio_ssel = pgpio_ssel;

    //base address
    int32_t apply_prog;

    priv->spu_base = spu_get_base(spu_idx);
    priv->global_base = priv->spu_base + GLOBAL_BASE;
    priv->pgpio_base = priv->spu_base + PGPIO_BASE;
    priv->inst_base = priv->spu_base + INST_BASE;

    apply_prog = SPU_SPI_PROG_BASE[spi_idx];
    priv->prog_base = priv->spu_base + PROG_BASE + apply_prog;

    //register callback function
    priv->cb_event = cb_event;
    return priv;
}

/**
  \brief       De-initialize SPI Interface. stops operation and releases the software resources used by the interface
  \param[in]   handle  spi handle to operate.
  \return      0 for success, negative for error code
*/
int32_t drv_spu_spi_uninitialize(spi_handle_t handle)
{
    ck_spu_spi_priv_t *priv = handle;

    uint32_t offset_pgpio;

    offset_pgpio = priv->pgpio_sclk << 4;
    putData(0x0, priv->pgpio_base, offset_pgpio);
    offset_pgpio += 4;
    putData(0x0, priv->pgpio_base, offset_pgpio);

    offset_pgpio = priv->pgpio_mosi << 4;
    putData(0x0, priv->pgpio_base, offset_pgpio);
    offset_pgpio += 4;
    putData(0x0, priv->pgpio_base, offset_pgpio);

    offset_pgpio = priv->pgpio_miso << 4;
    putData(0x0, priv->pgpio_base, offset_pgpio);
    offset_pgpio += 4;
    putData(0x0, priv->pgpio_base, offset_pgpio);

    offset_pgpio = priv->pgpio_ssel << 4;
    putData(0x0, priv->pgpio_base, offset_pgpio);
    offset_pgpio += 4;
    putData(0x0, priv->pgpio_base, offset_pgpio);

    priv->cb_event = NULL;

    return 0;
}

/**
  \brief       Get driver capabilities.
  \param[in]   spi instance to operate.
  \return      \ref spi_capabilities_t
*/
spi_capabilities_t drv_spu_spi_get_capabilities(int32_t idx)
{
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
int32_t drv_spu_spi_config(spi_handle_t     handle,
                           int32_t          baud,
                           spi_mode_e       mode,
                           spi_format_e     format,
                           spi_bit_order_e  order,
                           spi_ss_mode_e    ss_mode,
                           int32_t          bit_width)
{
    ck_spu_spi_priv_t *priv = handle;
    priv->mode = mode;
    priv->format = format;
    priv->order = order;

    if ((bit_width >= (4)) && (bit_width <= (16))) {
        priv->bit_width = bit_width;
    } else {
        return ERR_SPU_SPI(SPI_ERROR_DATA_BITS);
    }


    //====PGPIO
    uint32_t unit_clk;
    uint32_t offset_pgpio;
    uint32_t data_pgpio;

    unit_clk = get_unit_clk(baud);

    if (unit_clk > 0x10000) {
        return ERR_SPU_SPI(DRV_ERROR_PARAMETER);
    }

    if (mode == SPI_MODE_MASTER || mode == SPI_MODE_MASTER_SIMPLEX) {
        //ssel
        offset_pgpio = (priv->pgpio_ssel << 4) + 4;;
        putData(0x0, priv->pgpio_base, offset_pgpio);
        offset_pgpio += 4;
        data_pgpio = unit_clk | 0x22000;
        putData(data_pgpio, priv->pgpio_base, offset_pgpio);
        //sclk
        offset_pgpio = (priv->pgpio_sclk << 4) + 4;
        putData(0x0, priv->pgpio_base, offset_pgpio);
        offset_pgpio += 4;

        switch (format) {
            case SPI_FORMAT_CPOL0_CPHA0 :
                data_pgpio = 0x10000;
                break;

            case SPI_FORMAT_CPOL0_CPHA1 :
                data_pgpio = 0x10000;
                break;

            case SPI_FORMAT_CPOL1_CPHA0 :
                data_pgpio = 0x20000;
                break;

            case SPI_FORMAT_CPOL1_CPHA1 :
                data_pgpio = 0x20000;
                break;
        }

        putData(data_pgpio, priv->pgpio_base, offset_pgpio);
        data_pgpio = unit_clk | 0x2000;
        putData(data_pgpio, priv->pgpio_base, offset_pgpio);
        //mosi
        offset_pgpio = (priv->pgpio_mosi << 4) + 4;
        putData(0x0, priv->pgpio_base, offset_pgpio);
        offset_pgpio += 4;
        putData(0x20000, priv->pgpio_base, offset_pgpio);
        putData(0x2000, priv->pgpio_base, offset_pgpio);
        //miso
        offset_pgpio = (priv->pgpio_miso << 4) + 4;
        putData(0x0, priv->pgpio_base, offset_pgpio);
        offset_pgpio += 4;
        putData(0x0, priv->pgpio_base, offset_pgpio);
    }

    if (mode == SPI_MODE_SLAVE || mode == SPI_MODE_SLAVE_SIMPLEX) {
        //ssel
        offset_pgpio = (priv->pgpio_ssel << 4) + 4;
        putData(0x0, priv->pgpio_base, offset_pgpio);
        offset_pgpio += 4;
        putData(0x0, priv->pgpio_base, offset_pgpio);
        //sclk
        offset_pgpio = (priv->pgpio_sclk << 4) + 4;
        putData(0x0, priv->pgpio_base, offset_pgpio);
        offset_pgpio += 4;
        putData(0x0, priv->pgpio_base, offset_pgpio);
        //mosi
        offset_pgpio = (priv->pgpio_mosi << 4) + 4;
        putData(0x0, priv->pgpio_base, offset_pgpio);
        offset_pgpio += 4;
        putData(0x0, priv->pgpio_base, offset_pgpio);
        //miso
        offset_pgpio = (priv->pgpio_miso << 4) + 4;
        putData(0x0, priv->pgpio_base, offset_pgpio);
        offset_pgpio += 4;
        putData(0x20000, priv->pgpio_base, offset_pgpio);
        putData(0x2000, priv->pgpio_base, offset_pgpio);
    }

    //====ss_mode
    if (ss_mode == SPI_SS_MASTER_HW_OUTPUT) {
        target_spu_spi_cs_init(priv->spu_idx, priv->pgpio_ssel);
    }

    //====INST_RAM
    uint32_t func_index_base = 0u;
    uint32_t offset_inst;
    uint32_t data_inst;

    //index_addr
    switch (format) {
        case SPI_FORMAT_CPOL0_CPHA0:
            func_index_base = spu_get_spi_func_index_base(priv->spu_idx, 0);
            break;

        case SPI_FORMAT_CPOL0_CPHA1:
            func_index_base = spu_get_spi_func_index_base(priv->spu_idx, 1);
            break;

        case SPI_FORMAT_CPOL1_CPHA0:
            func_index_base = spu_get_spi_func_index_base(priv->spu_idx, 2);
            break;

        case SPI_FORMAT_CPOL1_CPHA1:
            func_index_base = spu_get_spi_func_index_base(priv->spu_idx, 3);
            break;

        default:
            return ERR_SPU_SPI(SPI_ERROR_FRAME_FORMAT);
    }

    offset_inst = (priv->pgpio_ssel << 3);
    data_inst = (func_index_base << 16) | (priv->prog_base - priv->spu_base);
    putData(data_inst, priv->inst_base, offset_inst);
    //context
    offset_inst += 4;

    switch (mode) {
        case SPI_MODE_MASTER_SIMPLEX:
            data_inst = (priv->pgpio_sclk) | (priv->pgpio_mosi << 4) | (priv->pgpio_mosi << 8) | (priv->pgpio_ssel << 12);
            break;

        case SPI_MODE_SLAVE_SIMPLEX:
            data_inst = (priv->pgpio_sclk) | (priv->pgpio_miso << 4) | (priv->pgpio_miso << 8) | (priv->pgpio_ssel << 12);
            break;

        default:
            data_inst = (priv->pgpio_sclk) | (priv->pgpio_mosi << 4) | (priv->pgpio_miso << 8) | (priv->pgpio_ssel << 12);
            break;
    }

    putData(data_inst, priv->inst_base, offset_inst);
    return 0;
}


/**
\brief         sending data to SPI transmitter,(received data is ignored).
               if non-blocking mode, this function only start the sending,
               \ref spi_event_e is signaled when operation completes or error happens.
               \ref drv_spu_spi_get_status can indicates operation status.
               if blocking mode, this function return after operation completes or error happens.
  \param[in]   handle spi handle to operate.
  \param[in]   data  Pointer to buffer with data to send to SPI transmitter. data_type is : uint8_t for 1..8 data bits, uint16_t for 9..16 data bits,uint32_t for 17..32 data bits,
  \param[in]   num   Number of data items to send.
  \param[in]   block_mode   blocking and non_blocking to selcect
  \return      error code
*/
int32_t drv_spu_spi_send(spi_handle_t handle, const void *data, uint32_t num)
{
    ck_spu_spi_priv_t *priv = handle;

    priv->trans_type = 0;
    priv->tx_data_buf = (void *)data;
    priv->tx_total_num = num;
    priv->tx_cur_num = 0;

    //====PGPIO
    uint32_t offset_pgpio;

    if (priv->mode == SPI_MODE_MASTER_SIMPLEX) {
        offset_pgpio = (priv->pgpio_mosi << 4) + 8;
        putData(0x2000, priv->pgpio_base, offset_pgpio);
    } else if (priv->mode == SPI_MODE_SLAVE_SIMPLEX) {
        offset_pgpio = (priv->pgpio_miso << 4) + 8;
        putData(0x2000, priv->pgpio_base, offset_pgpio);
    }

    //====PROG_RAM
    //tx fifo head
    putData(SPI_TX_FIFO_HEAD, priv->prog_base, SPI_OFFSET_TX_FIFO_HEAD);

    //fifo status
    uint32_t send_len;

    if ((SPI_TX_FIFO_DEPTH + 1) >= num) {
        putData(0xffff0000, priv->prog_base, SPI_OFFSET_FIFO_STATUS);
        send_len = num;
    } else {
        putData(0xff000000, priv->prog_base, SPI_OFFSET_FIFO_STATUS);
        send_len = SPI_TX_FIFO_DEPTH + 1;
    }

    //int
    putData(0x0, priv->prog_base, SPI_OFFSET_INT);

    //config: ctrl
    uint32_t ctrl;
    ctrl = SPI_TMOD_TRANSMIT | SPI_CS_TYPE;
    ctrl |= (priv->bit_width - 1) << 2;

    if (priv->order == SPI_ORDER_MSB2LSB) {
        ctrl |= SPI_SHIFT_LEFT;
    }

    putData(ctrl, priv->prog_base, SPI_OFFSET_CTRL);

    //config: TRANS_LEN
    putData(num, priv->prog_base, SPI_OFFSET_LEN);

    //====send data
    write_fifo(priv, send_len);

    //====variable
    priv->status.busy = 1;

    //====start PE
    uint32_t data_pgpio;
    offset_pgpio = (priv->pgpio_ssel << 4) + 4;

    if (priv->mode == SPI_MODE_MASTER || priv->mode == SPI_MODE_MASTER_SIMPLEX) {
        putData(0x3c00, priv->pgpio_base, offset_pgpio);
    } else {
        putData(0x800, priv->pgpio_base, offset_pgpio);
    }

    offset_pgpio += 4;
    getData(data_pgpio, priv->pgpio_base, offset_pgpio);
    data_pgpio |= 0x8000;
    putData(data_pgpio, priv->pgpio_base, offset_pgpio);

    return 0;
}

/**
  \brief       Start receiving data as SPI Master.
               This function is non-blocking,\ref spi_event_e is signaled when transfer completes or error happens.
               drv_*_get_status can indicates transmission status.
  \param[in]   handle  spi handle to operate.
  \param[out]  data    Pointer to buffer for data to receive from SPI receiver
  \param[in]   num     Number of data items to receive
  \param[in]   xfer_pending   Transfer operation is pending - Stop condition will not be generated
  \return      0 for success, negative for error code
*/
//int32_t drv_spu_spi_receive(spi_handle_t handle, void *data, uint32_t num, uint8_t block_mode, bool back_to_back, bool cs_pend)
int32_t drv_spu_spi_receive(spi_handle_t handle, void *data, uint32_t num)
{
    ck_spu_spi_priv_t *priv = handle;

    priv->trans_type = 1;
    priv->rx_data_buf = data;
    priv->rx_total_num = num;
    priv->rx_cur_num = 0;

    //====PGPIO
    uint32_t offset_pgpio;

    if (priv->mode == SPI_MODE_MASTER_SIMPLEX) {
        offset_pgpio = (priv->pgpio_mosi << 4) + 8;
        putData(0x0, priv->pgpio_base, offset_pgpio);
    } else if (priv->mode == SPI_MODE_SLAVE_SIMPLEX) {
        offset_pgpio = (priv->pgpio_miso << 4) + 8;
        putData(0x0, priv->pgpio_base, offset_pgpio);
    }

    //====PROG_RAM
    //rx fifo head
    putData(SPI_RX_FIFO_HEAD, priv->prog_base, SPI_OFFSET_RX_FIFO_HEAD);

    //fifo status
    if ((SPI_RX_FIFO_DEPTH + 1) >= num) {
        putData(0xffff0000, priv->prog_base, SPI_OFFSET_FIFO_STATUS);
    } else {
        putData(0xff0000, priv->prog_base, SPI_OFFSET_FIFO_STATUS);
    }

    //int
    putData(0x0, priv->prog_base, SPI_OFFSET_INT);

    //config: ctrl
    uint32_t ctrl;
    ctrl = SPI_TMOD_RECEIVE | SPI_CS_TYPE;
    ctrl |= (priv->bit_width - 1) << 2;

    if (priv->order == SPI_ORDER_MSB2LSB) {
        ctrl |= SPI_SHIFT_LEFT;
    }

    putData(ctrl, priv->prog_base, SPI_OFFSET_CTRL);

    //config: TRANS_LEN
    putData(num, priv->prog_base, SPI_OFFSET_LEN);

    //====variable
    priv->status.busy = 1;

    //====start PE
    uint32_t data_pgpio;
    offset_pgpio = (priv->pgpio_ssel << 4) + 4;

    if (priv->mode == SPI_MODE_MASTER || priv->mode == SPI_MODE_MASTER_SIMPLEX) {
        putData(0x3c00, priv->pgpio_base, offset_pgpio);
    } else {
        putData(0x800, priv->pgpio_base, offset_pgpio);
    }

    offset_pgpio += 4;
    getData(data_pgpio, priv->pgpio_base, offset_pgpio);
    data_pgpio |= 0x8000;
    putData(data_pgpio, priv->pgpio_base, offset_pgpio);

    return 0;
}

/**
  \brief       sending/receiving data to/from SPI transmitter/receiver.
               if non-blocking mode, this function only start the transfer,
               \ref spi_event_e is signaled when operation completes or error happens.
               \ref drv_spu_spi_get_status can indicates operation status.
               if blocking mode, this function return after operation completes or error happens.
  \param[in]   handle spi handle to operate.
  \param[in]   data_out  Pointer to buffer with data to send to SPI transmitter
  \param[out]  data_in   Pointer to buffer for data to receive from SPI receiver
  \param[in]   num_out      Number of data items to send
  \param[in]   num_in       Number of data items to receive
  \param[in]   block_mode   blocking and non_blocking to selcect
  \return      error code
*/
int32_t drv_spu_spi_transfer(spi_handle_t handle, const void *data_out, void *data_in, uint32_t num_out, uint32_t num_in)
{
    ck_spu_spi_priv_t *priv = handle;

    priv->trans_type = 2;
    priv->tx_data_buf = (void *)data_out;
    priv->tx_total_num = num_out;
    priv->tx_cur_num = 0;

    priv->rx_data_buf = (void *)data_in;
    priv->rx_total_num = num_in;
    priv->rx_cur_num = 0;

    if (priv->mode == SPI_MODE_MASTER_SIMPLEX || priv->mode == SPI_MODE_SLAVE_SIMPLEX) {
        return ERR_SPU_SPI(DRV_ERROR_UNSUPPORTED);
    }

    //====PROG_RAM
    //tx/rx fifo head
    putData(SPI_TX_FIFO_HEAD, priv->prog_base, SPI_OFFSET_TX_FIFO_HEAD);
    putData(SPI_RX_FIFO_HEAD, priv->prog_base, SPI_OFFSET_RX_FIFO_HEAD);

    //fifo status
    uint32_t send_len;

    if ((SPI_TX_FIFO_DEPTH + 1) >= num_out) {
        putData(0xffff0000, priv->prog_base, SPI_OFFSET_FIFO_STATUS);
        send_len = num_out;
    } else {
        putData(0x0, priv->prog_base, SPI_OFFSET_FIFO_STATUS);
        send_len = SPI_TX_FIFO_DEPTH + 1;
    }

    //int
    putData(0x0, priv->prog_base, SPI_OFFSET_INT);

    //config: ctrl
    uint32_t ctrl;
    ctrl = SPI_TMOD_TRANSFER | SPI_CS_TYPE;
    ctrl |= (priv->bit_width - 1) << 2;

    if (priv->order == SPI_ORDER_MSB2LSB) {
        ctrl |= SPI_SHIFT_LEFT;
    }

    putData(ctrl, priv->prog_base, SPI_OFFSET_CTRL);

    //config: TRANS_LEN
    putData(num_out, priv->prog_base, SPI_OFFSET_LEN);

    //====send data
    write_fifo(priv, send_len);

    //====variable
    priv->status.busy = 1;

    //====start PE
    uint32_t offset_pgpio;
    uint32_t data_pgpio;
    offset_pgpio = (priv->pgpio_ssel << 4) + 4;

    if (priv->mode == SPI_MODE_MASTER || priv->mode == SPI_MODE_MASTER_SIMPLEX) {
        putData(0x3c00, priv->pgpio_base, offset_pgpio);
    } else {
        putData(0x800, priv->pgpio_base, offset_pgpio);
    }

    offset_pgpio += 4;
    getData(data_pgpio, priv->pgpio_base, offset_pgpio);
    data_pgpio |= 0x8000;
    putData(data_pgpio, priv->pgpio_base, offset_pgpio);

    return 0;
}

int32_t drv_spu_spi_abort_transfer(spi_handle_t handle)
{
    return 0;
}

spi_status_t drv_spu_spi_get_status(spi_handle_t handle)
{
    ck_spu_spi_priv_t *priv = handle;
    return priv->status;
}

/**
  \brief       config the SPI mode.
  \param[in]   handle   spi handle
  \param[in]   mode     spi mode. \ref spi_mode_e
  \return      error code
*/
int32_t drv_spu_spi_config_mode(spi_handle_t handle, spi_mode_e  mode)
{
    return ERR_SPU_SPI(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       config the SPI block mode.
  \param[in]   handle   spi handle
  \param[in]   flag 1 - enbale the block mode. 0 - disable the block mode
  \return      error code
*/
int32_t drv_spu_spi_config_block_mode(spi_handle_t handle, int32_t flag)
{
    return ERR_SPU_SPI(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       Set the SPI baudrate.
  \param[in]   handle   spi handle
  \param[in]   baud     spi baud rate
  \return      error code
*/
int32_t drv_spu_spi_config_baudrate(spi_handle_t handle, uint32_t baud)
{
    return ERR_SPU_SPI(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       config the SPI bit order.
  \param[in]   handle   spi handle
  \param[in]   order    spi bit order.\ref spi_bit_order_e
  \return      error code
*/
int32_t drv_spu_spi_config_bit_order(spi_handle_t handle, spi_bit_order_e order)
{
    return ERR_SPU_SPI(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       Set the SPI datawidth.
  \param[in]   handle     spi handle
  \param[in]   datawidth  date frame size in bits
  \return      error code
*/
int32_t drv_spu_spi_config_datawidth(spi_handle_t handle, uint32_t datawidth)
{
    return ERR_SPU_SPI(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       config the SPI format.
  \param[in]   handle   spi handle
  \param[in]   format   spi format. \ref spi_format_e
  \return      error code
*/
int32_t drv_spu_spi_config_format(spi_handle_t handle, spi_format_e format)
{
    return ERR_SPU_SPI(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       config the SPI slave select mode.
  \param[in]   handle   spi handle
  \param[in]   ss_mode  spi slave select mode. \ref spi_ss_mode_e
  \return      error code
*/
int32_t drv_spu_spi_config_ss_mode(spi_handle_t handle, spi_ss_mode_e ss_mode)
{
    ck_spu_spi_priv_t *priv = handle;

    //====ss_mode
    if (ss_mode == SPI_SS_MASTER_HW_OUTPUT) {
        target_spu_spi_cs_init(priv->spu_idx, priv->pgpio_ssel);
    }

    return 0;
}

/**
  \brief       Get the number of the currently transferred.
  \param[in]   handle  spi handle to operate.
  \return      the number of the currently transferred data items
*/
uint32_t drv_spu_spi_get_data_count(spi_handle_t handle)
{
    ck_spu_spi_priv_t *priv = handle;

    switch (priv->trans_type) {
        case 0 :
            return priv->tx_cur_num;

        case 1 :
            return priv->rx_cur_num;

        default:
            return priv->tx_cur_num;
    }
}

/**
  \brief       control spi power.
  \param[in]   handle  spi handle to operate.
  \param[in]   state   power state.\ref drv_spu_power_stat_e.
  \return      error code
*/
int32_t drv_spu_spi_power_control(spi_handle_t handle, csi_power_stat_e state)
{
    return ERR_SPU_SPI(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       Control the Slave Select signal (SS).
  \param[in]   handle  spi handle to operate.
  \param[in]   stat    SS state. \ref spi_ss_stat_e.
  \return      error code
*/
int32_t drv_spu_spi_ss_control(spi_handle_t handle, spi_ss_stat_e stat)
{
    return ERR_SPU_SPI(DRV_ERROR_UNSUPPORTED);
}



/**
 * Copyright (c) 2015 - 2017, Nordic Semiconductor ASA
 * 
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 * 
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 * 
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 * 
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 * 
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */
#include "sdk_common.h"
#if NRF_MODULE_ENABLED(APP_UART)
#include "app_uart.h"
#include "app_fifo.h"
#include "nrf_drv_uart.h"
#include "nrf_assert.h"
#include "nrf_delay.h"

#include "k_api.h"
#include "hal/soc/uart.h"


#define MAX_BUF_UART_BYTES  256
kbuf_queue_t g_buf_queue_uart;
char g_buf_uart[MAX_BUF_UART_BYTES];


static nrf_drv_uart_t app_uart_inst = NRF_DRV_UART_INSTANCE(APP_UART_DRIVER_INSTANCE);

static __INLINE uint32_t fifo_length(app_fifo_t * const fifo)
{
  uint32_t tmp = fifo->read_pos;
  return fifo->write_pos - tmp;
}

#define FIFO_LENGTH(F) fifo_length(&F)              /**< Macro to calculate length of a FIFO. */


static app_uart_event_handler_t   m_event_handler;            /**< Event handler function. */
static uint8_t tx_buffer[1];
static uint8_t rx_buffer[1];
static bool m_rx_ovf;

static app_fifo_t                  m_rx_fifo;                               /**< RX FIFO buffer for storing data received on the UART until the application fetches them using app_uart_get(). */
static app_fifo_t                  m_tx_fifo;                               /**< TX FIFO buffer for storing data to be transmitted on the UART when TXD is ready. Data is put to the buffer on using app_uart_put(). */

static void uart_event_handler(nrf_drv_uart_event_t * p_event, void* p_context)
{
    app_uart_evt_t app_uart_event;
    uint32_t err_code;
    uint8_t bytedata;

    switch (p_event->type)
    {
        case NRF_DRV_UART_EVT_RX_DONE:
            // Write received byte to FIFO.
            bytedata = p_event->data.rxtx.p_data[0];
            krhino_buf_queue_send(&g_buf_queue_uart, &bytedata, 1);
            
            (void)nrf_drv_uart_rx(&app_uart_inst, rx_buffer, 1);
            break;

        case NRF_DRV_UART_EVT_ERROR:
            (void)nrf_drv_uart_rx(&app_uart_inst, rx_buffer, 1);
            break;

        case NRF_DRV_UART_EVT_TX_DONE:
            // Get next byte from FIFO.
            if (app_fifo_get(&m_tx_fifo, tx_buffer) == NRF_SUCCESS)
            {
                (void)nrf_drv_uart_tx(&app_uart_inst, tx_buffer, 1);
            }
            else
            {
                // Last byte from FIFO transmitted, notify the application.
            }
            break;

        default:
            break;
    }
}


uint32_t app_uart_init(const app_uart_comm_params_t * p_comm_params,
                             app_uart_buffers_t *     p_buffers,
                             app_uart_event_handler_t event_handler,
                             app_irq_priority_t       irq_priority)
{
    uint32_t err_code;
    kstat_t ret;

    m_event_handler = event_handler;

    if (p_buffers == NULL)
    {
        return NRF_ERROR_INVALID_PARAM;
    }

    err_code = krhino_buf_queue_create(&g_buf_queue_uart, "buf_queue_uart",
      g_buf_uart, MAX_BUF_UART_BYTES, 1);
    VERIFY_SUCCESS(err_code);

    // Configure buffer RX buffer.
    err_code = app_fifo_init(&m_rx_fifo, p_buffers->rx_buf, p_buffers->rx_buf_size);
    VERIFY_SUCCESS(err_code);

    // Configure buffer TX buffer.
    err_code = app_fifo_init(&m_tx_fifo, p_buffers->tx_buf, p_buffers->tx_buf_size);
    VERIFY_SUCCESS(err_code);

    nrf_drv_uart_config_t config = NRF_DRV_UART_DEFAULT_CONFIG;
    config.baudrate = (nrf_uart_baudrate_t)p_comm_params->baud_rate;
    config.hwfc = (p_comm_params->flow_control == APP_UART_FLOW_CONTROL_DISABLED) ?
            NRF_UART_HWFC_DISABLED : NRF_UART_HWFC_ENABLED;
    config.interrupt_priority = irq_priority;
    config.parity = p_comm_params->use_parity ? NRF_UART_PARITY_INCLUDED : NRF_UART_PARITY_EXCLUDED;
    config.pselcts = p_comm_params->cts_pin_no;
    config.pselrts = p_comm_params->rts_pin_no;
    config.pselrxd = p_comm_params->rx_pin_no;
    config.pseltxd = p_comm_params->tx_pin_no;

    err_code = nrf_drv_uart_init(&app_uart_inst, &config, uart_event_handler);
    VERIFY_SUCCESS(err_code);
    m_rx_ovf = false;

    // Turn on receiver if RX pin is connected
    if (p_comm_params->rx_pin_no != UART_PIN_DISCONNECTED)
    {
#ifdef UARTE_PRESENT
        if (!config.use_easy_dma)
#endif
        {
            nrf_drv_uart_rx_enable(&app_uart_inst);
        }

        return nrf_drv_uart_rx(&app_uart_inst, rx_buffer,1);
    }
    else
    {
        return NRF_SUCCESS;
    }
}


uint32_t app_uart_flush(void)
{
    uint32_t err_code;

    err_code = app_fifo_flush(&m_rx_fifo);
    VERIFY_SUCCESS(err_code);

    err_code = app_fifo_flush(&m_tx_fifo);
    VERIFY_SUCCESS(err_code);

    return NRF_SUCCESS;
}


uint32_t app_uart_get(uint8_t * p_byte)
{
    ASSERT(p_byte);
    bool rx_ovf = m_rx_ovf;

    ret_code_t err_code =  app_fifo_get(&m_rx_fifo, p_byte);

    // If FIFO was full new request to receive one byte was not scheduled. Must be done here.
    if (rx_ovf)
    {
        m_rx_ovf = false;
        uint32_t uart_err_code = nrf_drv_uart_rx(&app_uart_inst, rx_buffer, 1);

        // RX resume should never fail.
        APP_ERROR_CHECK(uart_err_code);
    }

    return err_code;
}


uint32_t app_uart_put(uint8_t byte)
{
    uint32_t err_code;
    err_code = app_fifo_put(&m_tx_fifo, byte);
    if (err_code == NRF_SUCCESS)
    {
        // The new byte has been added to FIFO. It will be picked up from there
        // (in 'uart_event_handler') when all preceding bytes are transmitted.
        // But if UART is not transmitting anything at the moment, we must start
        // a new transmission here.
        
        if (!nrf_drv_uart_tx_in_progress(&app_uart_inst))
        {
            // This operation should be almost always successful, since we've
            // just added a byte to FIFO, but if some bigger delay occurred
            // (some heavy interrupt handler routine has been executed) since
            // that time, FIFO might be empty already.
            if (app_fifo_get(&m_tx_fifo, tx_buffer) == NRF_SUCCESS)
            {
                err_code = nrf_drv_uart_tx(&app_uart_inst, tx_buffer, 1);
            }
        }
    }
    return err_code;
}

uint32_t app_uart_putall(void)
{
    uint32_t err_code;
    if (err_code == NRF_SUCCESS)
    {
        // The new byte has been added to FIFO. It will be picked up from there
        // (in 'uart_event_handler') when all preceding bytes are transmitted.
        // But if UART is not transmitting anything at the moment, we must start
        // a new transmission here.
        
        if (!nrf_drv_uart_tx_in_progress(&app_uart_inst))
        {
            // This operation should be almost always successful, since we've
            // just added a byte to FIFO, but if some bigger delay occurred
            // (some heavy interrupt handler routine has been executed) since
            // that time, FIFO might be empty already.
            if (app_fifo_get(&m_tx_fifo, tx_buffer) == NRF_SUCCESS)
            {
                err_code = nrf_drv_uart_tx(&app_uart_inst, tx_buffer, 1);
            }
        }
    }
    return err_code;
}


uint32_t app_uart_close(void)
{
    nrf_drv_uart_uninit(&app_uart_inst);
    return NRF_SUCCESS;
}

int32_t hal_uart_recv(uart_dev_t *uart, void *data, uint32_t expect_size, uint32_t timeout)
{
    uint8_t *pdata = (uint8_t *)data;
    int i = 0;
    uint32_t rx_count = 0;
    int32_t ret = -1;
    int32_t rev_size;

    if (data == NULL) {
        return -1;
    }

    for (i = 0; i < expect_size; i++)
    {
        ret = krhino_buf_queue_recv(&g_buf_queue_uart, RHINO_WAIT_FOREVER, &pdata[i], &rev_size);
        if((ret == 0) && (rev_size == 1))
        {
            rx_count++;
        }else {
            break;
        }

    }

    if(rx_count != 0)
    {
        ret = 0;
    }
    else
    {
        ret = -1;
    }

    return ret;
}

int32_t hal_uart_recv_II(uart_dev_t *uart, void *data, uint32_t expect_size,
                      uint32_t *recv_size, uint32_t timeout)
{
    uint8_t *pdata = (uint8_t *)data;
    int i = 0;
    uint32_t rx_count = 0;
    int32_t ret = -1;
    int32_t rev_size;

    if (data == NULL) {
        return -1;
    }

    for (i = 0; i < expect_size; i++)
    {
        ret = krhino_buf_queue_recv(&g_buf_queue_uart, RHINO_WAIT_FOREVER, &pdata[i], &rev_size);
        if((ret == 0) && (rev_size == 1))
        {
            rx_count++;
        }else {
            break;
        }

    }
    
    if (recv_size)
        *recv_size = rx_count;

    if(rx_count != 0)
    {
        ret = 0;
    }
    else
    {
        ret = -1;
    }

    return ret;
}


int32_t hal_uart_send(uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout)
{
    /*
    uint32_t ret =  nrf_drv_uart_tx(&app_uart_inst, data, size);
    if (NRF_ERROR_BUSY == ret)
    {
        return NRF_ERROR_NO_MEM;
    }
    else if (ret != NRF_SUCCESS)
    {
        return NRF_ERROR_INTERNAL;
    }
    else
    {
        return NRF_SUCCESS;
    }*/
    uint32_t i;
    uint8_t byte;
    uint32_t ret;
    for(i=0;i<size;i++)
    {
        byte = *((uint8_t *)data + i);
        ret |= app_fifo_put(&m_tx_fifo, byte);
        /*nrf_delay_ms(5);*/
    }
    app_uart_putall();

    return ret;

}



#endif //NRF_MODULE_ENABLED(APP_UART)

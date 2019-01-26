/* Microcontroller Library
 * Copyright (c) 2015-2016 Nuvoton
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "objects.h"
#include "nu_modutil.h"
#include "nu_bitutil.h"
#include "hal_status.h"
#include <k_api.h>

#include "aos/hal/uart.h"
#include "board.h"

#define MAX_BUF_UART_BYTES 512

struct nu_uart_var {
    uint32_t    ref_cnt;                // Reference count of the H/W module
    struct serial_s *  obj;
//  kbuf_queue_t    fifo_queue_tx;
    char *              fifo_buf;
    kbuf_queue_t    fifo_queue_rx;  
    kmutex_t            port_tx_mutex;
    kmutex_t            port_rx_mutex;
};

static struct nu_uart_var uart0_var = {
    .ref_cnt  = 0,
    .obj      = NULL,
    .fifo_buf = NULL,
};
static struct nu_uart_var uart1_var = {
    .ref_cnt  = 0,
    .obj      = NULL,
    .fifo_buf = NULL,
};
static struct nu_uart_var uart2_var = {
    .ref_cnt  = 0,
    .obj      = NULL,
    .fifo_buf = NULL,
};
static struct nu_uart_var uart3_var = {
    .ref_cnt  = 0,
    .obj      = NULL,
    .fifo_buf = NULL,
};
static struct nu_uart_var uart4_var = {
    .ref_cnt  = 0,
    .obj      = NULL,
    .fifo_buf = NULL,
};
static struct nu_uart_var uart5_var = {
    .ref_cnt  = 0,
    .obj      = NULL,
    .fifo_buf = NULL,
};

static const struct nu_modinit_s uart_modinit_tab[] = 
{
    {UART_0, UART0_MODULE, CLK_CLKSEL1_UART0SEL_HIRC, CLK_CLKDIV0_UART0(1), UART0_RST, UART0_IRQn, &uart0_var},
    {UART_1, UART1_MODULE, CLK_CLKSEL1_UART1SEL_HIRC, CLK_CLKDIV0_UART1(1), UART1_RST, UART1_IRQn, &uart1_var},
    {UART_2, UART2_MODULE, CLK_CLKSEL3_UART2SEL_HIRC, CLK_CLKDIV4_UART2(1), UART2_RST, UART2_IRQn, &uart2_var},
    {UART_3, UART3_MODULE, CLK_CLKSEL3_UART3SEL_HIRC, CLK_CLKDIV4_UART3(1), UART3_RST, UART3_IRQn, &uart3_var},
    {UART_4, UART4_MODULE, CLK_CLKSEL3_UART4SEL_HIRC, CLK_CLKDIV4_UART4(1), UART4_RST, UART4_IRQn, &uart4_var},
    {UART_5, UART5_MODULE, CLK_CLKSEL3_UART5SEL_HIRC, CLK_CLKDIV4_UART5(1), UART5_RST, UART5_IRQn, &uart5_var},

    {NC, 0, 0, 0, 0, (IRQn_Type) 0, NULL}
};

static uint32_t uart_modinit_mask = 0;


static void hal_uart_rxbuf_irq ( struct nu_uart_var* psNuUartVar )
{
    uint8_t dat;
    struct serial_s *obj = psNuUartVar->obj;
    UART_T *uart_base = (UART_T *) NU_MODBASE(obj->uart);
    while (UART_IS_RX_READY(uart_base)) {
        dat = (uint8_t)uart_base->DAT;
        krhino_buf_queue_send(&psNuUartVar->fifo_queue_rx, &dat, 1);
    };
}

static void uart_irq(struct nu_uart_var* psNuUartVar)
{
        struct serial_s *obj = psNuUartVar->obj;    
    UART_T *uart_base = (UART_T *) NU_MODBASE(obj->uart);

    if (uart_base->INTSTS & (UART_INTSTS_RDAINT_Msk | UART_INTSTS_RXTOINT_Msk)) {
        // Simulate clear of the interrupt flag. Temporarily disable the interrupt here and to be recovered on next read.
        UART_DISABLE_INT(uart_base, (UART_INTEN_RDAIEN_Msk | UART_INTEN_RXTOIEN_Msk));
        hal_uart_rxbuf_irq ( psNuUartVar );
        UART_ENABLE_INT ( uart_base, (UART_INTEN_RDAIEN_Msk | UART_INTEN_RXTOIEN_Msk) );            
    }

#if 0
    if (uart_base->INTSTS & UART_INTSTS_THREINT_Msk) {
        // Simulate clear of the interrupt flag. Temporarily disable the interrupt here and to be recovered on next write.
        UART_DISABLE_INT(uart_base, UART_INTEN_THREIEN_Msk);
        if (obj->serial.irq_handler) {
            ((uart_irq_handler) obj->serial.irq_handler)(obj->serial.irq_id, TxIrq);
        }
    }       
#endif
        
    // FIXME: Ignore all other interrupt flags. Clear them. Otherwise, program will get stuck in interrupt.
    uart_base->INTSTS = uart_base->INTSTS;
    uart_base->FIFOSTS = uart_base->FIFOSTS;
}

void UART0_IRQHandler (void) { uart_irq(&uart0_var); }
void UART1_IRQHandler (void) { uart_irq(&uart1_var); }
void UART2_IRQHandler (void) { uart_irq(&uart2_var); }
void UART3_IRQHandler (void) { uart_irq(&uart3_var); }
void UART4_IRQHandler (void) { uart_irq(&uart4_var); }
void UART5_IRQHandler (void) { uart_irq(&uart5_var); }


/**
 * Configure UART flowcontrol
 *
 *
 * @param[in]  pUart    the UART register base of NuMicro
 * @param[in]  hufc_t   the flowcontrol setting.
 * 
 * @return  0 : on success, EIO : if an error occurred with any step
 */
static int32_t hal_uart_set_flowcontol ( UART_T* pUart, hal_uart_flow_control_t hufc_t )
{   
    if (!pUart)
        goto exit_hal_uart_set_flowcontol;
    
    // Disable to CTS/RTS first.    
    pUart->INTEN &= ~(UART_INTEN_ATORTSEN_Msk | UART_INTEN_ATOCTSEN_Msk);
    switch (hufc_t)
    {
        case FLOW_CONTROL_DISABLED:
            break;
        
        case FLOW_CONTROL_CTS:  //CTS only
            /* Set CTS pin input is low level active */
            pUart->MODEMSTS |= UART_MODEMSTS_CTSACTLV_Msk;

            /* Set RTS and CTS auto flow control enable */
            pUart->INTEN |= UART_INTEN_ATOCTSEN_Msk;
            break;
            
        case FLOW_CONTROL_RTS:  //RTS only
            /* Set RTS pin output is low level active */
            pUart->MODEM |= UART_MODEM_RTSACTLV_Msk;

            /* Set RTS and CTS auto flow control enable */
            pUart->INTEN |= UART_INTEN_ATORTSEN_Msk ;

        case FLOW_CONTROL_CTS_RTS:
            /* Set RTS pin output is low level active */
            pUart->MODEM |= UART_MODEM_RTSACTLV_Msk;

            /* Set CTS pin input is low level active */
            pUart->MODEMSTS |= UART_MODEMSTS_CTSACTLV_Msk;

            /* Set RTS and CTS auto flow control enable */
            pUart->INTEN |= UART_INTEN_ATORTSEN_Msk | UART_INTEN_ATOCTSEN_Msk;
        break;
        
        default:
            goto exit_hal_uart_set_flowcontol;
        break;
    }
    
    return HAL_OK;
    
exit_hal_uart_set_flowcontol:

    return HAL_ERROR;
}

/**
 * Configure UART line configuration
 *
 *
 * @param[in]  pUart    the UART register base of NuMicro
 * @param[in]  hufc_t   the flowcontrol setting.
 * 
 * @return  0 : on success, EIO : if an error occurred with any step
 */
static int32_t hal_uart_set_lineconf ( UART_T* pUart, hal_uart_data_width_t udw, hal_uart_stop_bits_t usb, hal_uart_parity_t up)
{   
    uint32_t u32data_width;
    uint32_t u32parity;
    uint32_t u32stop_bits;
    
    if (!pUart)
        goto exit_hal_uart_set_lineconf;
        
    // Check data width parameters
    switch (udw)
    {
        case DATA_WIDTH_5BIT: u32data_width=UART_WORD_LEN_5; break;
        case DATA_WIDTH_6BIT: u32data_width=UART_WORD_LEN_6; break;
        case DATA_WIDTH_7BIT: u32data_width=UART_WORD_LEN_7; break;
        case DATA_WIDTH_8BIT: u32data_width=UART_WORD_LEN_8; break;
        
        case DATA_WIDTH_9BIT:   //Not support
        default:
            goto exit_hal_uart_set_lineconf;
        break;
    }

    // Check stop bit parameters
    switch (usb)
    {
        case STOP_BITS_1: u32stop_bits=UART_STOP_BIT_1; break;
        case STOP_BITS_2: u32stop_bits=UART_STOP_BIT_2; break;
        default:
            goto exit_hal_uart_set_lineconf;
        break;
    }   
    
    // Check parity parameters
    switch (up)
    {       
        case NO_PARITY:   u32parity=UART_PARITY_NONE;   break;
        case ODD_PARITY:  u32parity=UART_PARITY_ODD;    break;
        case EVEN_PARITY: u32parity=UART_PARITY_EVEN;   break;      
        default:
            goto exit_hal_uart_set_lineconf;
        break;
    }

    // Dont change baudrate setting.
    UART_SetLineConfig ( pUart, 0, u32data_width, u32parity, u32stop_bits );
        
    return HAL_OK;
    
exit_hal_uart_set_lineconf:
    return HAL_ERROR;
}

static struct serial_s* hal_get_serial_s ( uart_dev_t *uart )
{
    if ( !(uart) || (uart->port >= i32BoardMaxUartNum) )
        goto exit_hal_get_serial_s;

    // Get UART Private configuration, these setting are defined in board/xxxxx.
    return (struct serial_s*)&board_uart[uart->port];

exit_hal_get_serial_s:
    return NULL;
}

/**
 * Initialises a UART interface
 *
 *
 * @param[in]  uart  the interface which should be initialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
 int32_t hal_uart_init(uart_dev_t *uart)
{
    struct nu_modinit_s *modinit;
    struct serial_s* pserial_s;
  struct nu_uart_var *var;
    UARTName s_UartName;
    UART_T* pUart;

    if ( !(pserial_s=hal_get_serial_s ( uart )) )
        goto exit_hal_uart_init;

  // found UARTNAME from the two PinName.
  uint32_t uart_tx = pinmap_peripheral(pserial_s->pin_tx, PinMap_UART_TX);
  uint32_t uart_rx = pinmap_peripheral(pserial_s->pin_rx, PinMap_UART_RX);

    // Get final UARTName
    s_UartName = (UARTName) pinmap_merge(uart_tx, uart_rx);
    if ( s_UartName == NC )
        goto exit_hal_uart_init;

    // Find entry by UARTNAME
  if ( !(modinit = get_modinit(s_UartName, uart_modinit_tab)) ) 
    goto exit_hal_uart_init;

    var = (struct nu_uart_var *) modinit->var;
    if (! var->ref_cnt) {

        do {
            // Reset this module
            SYS_ResetModule(modinit->rsetidx);

            // Select IP clock source
            CLK_SetModuleClock(modinit->clkidx, modinit->clksrc, modinit->clkdiv);
            
            // Enable IP clock
            CLK_EnableModuleClock(modinit->clkidx);

            pinmap_pinout(pserial_s->pin_tx, PinMap_UART_TX);
            pinmap_pinout(pserial_s->pin_rx, PinMap_UART_RX);

        } while (0);

                if ( var->fifo_buf == NULL ) 
                        var->fifo_buf = aos_malloc(MAX_BUF_UART_BYTES);
                
                if ( var->fifo_buf == NULL ) {
                        printf("fail to malloc memory size %d at %s %d \r\n", MAX_BUF_UART_BYTES, __FILE__, __LINE__);
                        goto exit_hal_uart_init;
                }
                memset( var->fifo_buf, 0, MAX_BUF_UART_BYTES );
            
                if ( krhino_buf_queue_create(&var->fifo_queue_rx, "buf_queue_uart", var->fifo_buf, MAX_BUF_UART_BYTES, 1) != RHINO_SUCCESS )
                    goto exit_hal_uart_init;

                if ( krhino_mutex_create(&var->port_tx_mutex, "uartTxMutex") != RHINO_SUCCESS )
                    goto exit_hal_uart_init;

            if ( krhino_mutex_create(&var->port_rx_mutex, "uartTxMutex") != RHINO_SUCCESS )
                    goto exit_hal_uart_init;

                // Get Uart base address
                pUart = (UART_T *) NU_MODBASE(s_UartName);
                
                /* Set UART Baudrate */
                UART_Open(pUart, uart->config.baud_rate);
                
                // Set line configuration
                if ( hal_uart_set_lineconf ( pUart, uart->config.data_width, uart->config.stop_bits, uart->config.parity) < 0 )
                    goto exit_hal_uart_init;

                // Set flow-control parameters
                if ( hal_uart_set_flowcontol ( pUart, uart->config.flow_control ) < 0 )
                    goto exit_hal_uart_init;

                // Check mode parameters
                switch (uart->config.mode)
                {
                    case MODE_TX:
                    case MODE_RX:
                    case MODE_TX_RX:
                    break;
                    
                    default:
                        goto exit_hal_uart_init;
                    break;
                } //switch

                NVIC_EnableIRQ(modinit->irq_n);

                /* Enable Interrupt */
                UART_ENABLE_INT(pUart, (UART_INTEN_RDAIEN_Msk));

                /* Link parent and children. */
                var->obj = pserial_s;

    } //if (! var->ref_cnt)
   
    var->ref_cnt ++;
    
    if (var->ref_cnt) {
        // Mark this module to be inited.
        int i = modinit - uart_modinit_tab;
        uart_modinit_mask |= 1 << i;
    }

    return HAL_OK;

exit_hal_uart_init:

    if ( pUart )
        UART_Close(pUart);
            
    return HAL_ERROR;   
}

/**
 * Transmit data on a UART interface
 *
 * @param[in]  uart     the UART interface
 * @param[in]  data     pointer to the start of data
 * @param[in]  size     number of bytes to transmit
 * @param[in]  timeout  timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                      if you want to wait forever
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_uart_send(uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout)
{
    struct nu_modinit_s *modinit;
    struct serial_s* pserial_s;
  struct nu_uart_var *var;
    UART_T* pUart;
  kstat_t stat = RHINO_SUCCESS;

    pserial_s = hal_get_serial_s ( uart );
    if ( !pserial_s )
        goto exit_hal_uart_send;
    
    modinit = get_modinit(pserial_s->uart, uart_modinit_tab);
    /* Valid? */
    if ( !modinit ) goto exit_hal_uart_send;
    
    var = modinit->var; 
    /* Initialized? */
    if ( !var->ref_cnt ) goto exit_hal_uart_send;

  if (timeout == 0)
      timeout = 0xffffffff;

    pserial_s = var->obj;
    pUart = (UART_T *) NU_MODBASE(pserial_s->uart);

  /* Wait for Lock */
  stat = krhino_mutex_lock(&var->port_tx_mutex, timeout);
  if (stat != RHINO_SUCCESS)
        goto exit_hal_uart_send;

    if ( UART_Write(pUart, (uint8_t*)data, size) != size )
        goto exit_hal_uart_send;
    //Todo: implement timeout feature.

    /* Unlock before exiting. */
  stat = krhino_mutex_unlock(&var->port_tx_mutex);
  if (stat != RHINO_SUCCESS)
    goto exit_hal_uart_send;
 
    return HAL_OK;
    
exit_hal_uart_send:

    return HAL_ERROR;
}


int32_t platform_uart_read(struct nu_uart_var *var, uint8_t pu8RxBuf[], uint32_t u32ReadBytes, uint32_t timeout )
{
    int32_t  ret;
    uint32_t rx_count = 0;
    size_t   rev_size = 0;

    struct serial_s *pserial_s = var->obj;

    kbuf_queue_t *pBuffer_queue = &var->fifo_queue_rx;

    while (rx_count < u32ReadBytes) {
        rev_size = u32ReadBytes - rx_count;
        ret = krhino_buf_queue_recv(pBuffer_queue, krhino_ms_to_ticks(timeout), &pu8RxBuf[rx_count], &rev_size);
        if (u32ReadBytes < 1) {
            printf("%s %d %d %d\r\n", __func__, u32ReadBytes, rx_count, rev_size);
        }

        if ((ret == 0) && (rev_size == 1)) {
            rx_count += rev_size;
        } else {
            break;
        }
    }

    return rx_count;
}

/**
 * Receive data on a UART interface
 *
 * @param[in]   uart         the UART interface
 * @param[out]  data         pointer to the buffer which will store incoming data
 * @param[in]   expect_size  number of bytes to receive
 * @param[in]   timeout      timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                           if you want to wait forever
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_uart_recv(uart_dev_t *uart, void *data, uint32_t expect_size, uint32_t timeout)
{
    struct nu_modinit_s *modinit;   
    struct serial_s* pserial_s;
    struct nu_uart_var *var;
    kstat_t stat = RHINO_SUCCESS;

    int count=0;

    pserial_s = hal_get_serial_s ( uart );
    if ( !pserial_s )
        goto exit_hal_uart_recv;

    if (timeout == 0)
        timeout = 0xffffffff; 

    modinit = get_modinit(pserial_s->uart, uart_modinit_tab);
    /* Valid? */
    if ( !modinit ) goto exit_hal_uart_recv;
    
    var = (struct nu_uart_var *)modinit->var;
    /* Initialized? */
    if ( !var->ref_cnt ) goto exit_hal_uart_recv;

    pserial_s = var->obj;

    /* Wait for Lock */
    stat = krhino_mutex_lock(&var->port_rx_mutex, timeout);
    if (stat != RHINO_SUCCESS)
        goto exit_hal_uart_recv;

    count = platform_uart_read(var, (uint8_t*)data, expect_size, timeout);  

    /* Unlock before exiting. */
    stat = krhino_mutex_unlock(&var->port_rx_mutex);
    if (stat != RHINO_SUCCESS)
        goto exit_hal_uart_recv;
    
    if ( count != expect_size  )
        goto exit_hal_uart_recv;

    return HAL_OK;
    
exit_hal_uart_recv:

    return HAL_ERROR;
}

/**
 * Receive data on a UART interface
 *
 * @param[in]   uart         the UART interface
 * @param[out]  data         pointer to the buffer which will store incoming data
 * @param[in]   expect_size  number of bytes to receive
 * @param[out]  recv_size    number of bytes received
 * @param[in]   timeout      timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                           if you want to wait forever
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_uart_recv_II(uart_dev_t *uart, void *data, uint32_t expect_size,
                         uint32_t *recv_size, uint32_t timeout)
{
    struct nu_modinit_s *modinit;       
    struct serial_s* pserial_s;
  struct nu_uart_var *var;
  kstat_t stat = RHINO_SUCCESS;
    int count=0;

    pserial_s = hal_get_serial_s ( uart );
    if ( !pserial_s )
        goto exit_hal_uart_recv_ii;

  if (timeout == 0)
      timeout = 0xffffffff; 
    
    modinit = get_modinit(pserial_s->uart, uart_modinit_tab);
    /* Valid? */
    if ( !modinit ) goto exit_hal_uart_recv_ii;
    
    var = (struct nu_uart_var *)modinit->var;
    /* Initialized? */
    if ( !var->ref_cnt ) goto exit_hal_uart_recv_ii;

    pserial_s = var->obj;

    /* Wait for Lock */
  stat = krhino_mutex_lock(&var->port_rx_mutex, timeout);
  if (stat != RHINO_SUCCESS)
     goto exit_hal_uart_recv_ii;

    *recv_size = platform_uart_read(var, (uint8_t*)data, expect_size, timeout); 
    
    /* Unlock before exiting. */
  stat = krhino_mutex_unlock(&var->port_rx_mutex);
  if (stat != RHINO_SUCCESS)
    goto exit_hal_uart_recv_ii;
    
  return HAL_OK;
    
exit_hal_uart_recv_ii:
    *recv_size = 0;
    return HAL_ERROR;
}

/**
 * Deinitialises a UART interface
 *
 * @param[in]  uart  the interface which should be deinitialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_uart_finalize(uart_dev_t *uart)
{
    struct serial_s* pserial_s;
    struct nu_modinit_s *modinit;
    struct nu_uart_var *var;
    UART_T* pUart;

    pserial_s = hal_get_serial_s ( uart );
    if ( !pserial_s )
        goto exit_hal_uart_finalize;

    modinit = get_modinit(pserial_s->uart, uart_modinit_tab);
    /* Valid? */
    if ( !modinit ) goto exit_hal_uart_finalize;

    var = (struct nu_uart_var *)modinit->var;
    /* Initialized? */
    if ( !var->ref_cnt ) goto exit_hal_uart_finalize;

    pUart = (UART_T *) NU_MODBASE(pserial_s->uart);
    
    var->ref_cnt --;
    
    if (! var->ref_cnt) {
        
        do {
                    UART_Close(pUart);

                    UART_DISABLE_INT(pUart, (UART_INTEN_RDAIEN_Msk | UART_INTEN_THREIEN_Msk | UART_INTEN_RXTOIEN_Msk));
                    NVIC_DisableIRQ(modinit->irq_n);

                    // Disable IP clock
                    CLK_DisableModuleClock(modinit->clkidx);
        } while (0);

        krhino_mutex_del(&var->port_tx_mutex);
        krhino_buf_queue_del(&var->fifo_queue_rx);
        
        /* Unlink parent and children. */
        var->obj = NULL ;
    }

    if (! var->ref_cnt) {
        // Mark this module to be deinited.
        int i = modinit - uart_modinit_tab;
        uart_modinit_mask &= ~(1 << i);
    }

    return HAL_OK;
    
exit_hal_uart_finalize:

    return HAL_ERROR;

}

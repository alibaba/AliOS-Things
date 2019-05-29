/**
******************************************************************************
* @file    paltform_uart.c 
* @author  William Xu
* @version V1.0.0
* @date    05-May-2014
* @brief   This file provide UART driver functions.
******************************************************************************
*
*  The MIT License
*  Copyright (c) 2014 MXCHIP Inc.
*
*  Permission is hereby granted, free of charge, to any person obtaining a copy 
*  of this software and associated documentation files (the "Software"), to deal
*  in the Software without restriction, including without limitation the rights 
*  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*  copies of the Software, and to permit persons to whom the Software is furnished
*  to do so, subject to the following conditions:
*
*  The above copyright notice and this permission notice shall be included in
*  all copies or substantial portions of the Software.
*
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
*  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
*  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
*  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR 
*  IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
******************************************************************************
*/ 

#include "aos_debug.h"

#include "mico_rtos.h"
#include "mico_platform.h"

#include "board.h"
#include "platform_peripheral.h"
#include "debug.h"

#include "pinmap.h"

#define mico_log(M, ...) custom_log("MICO", M, ##__VA_ARGS__)
#define mico_log_trace() custom_log_trace("MICO")


/******************************************************
*                    Constants
******************************************************/

/******************************************************
*                   Enumerations
******************************************************/

/******************************************************
*                 Type Definitions
******************************************************/

/******************************************************
*                    Structures
******************************************************/

/******************************************************
*               Variables Definitions
******************************************************/
/******************************************************
*        Static Function Declarations
******************************************************/
/* Interrupt service functions - called from interrupt vector table */
#ifndef NO_MICO_RTOS
static void thread_wakeup(void *arg);
static void RX_PIN_WAKEUP_handler(void *arg);
#endif

/******************************************************
*               Function Definitions
******************************************************/
OSStatus platform_uart_init( platform_uart_driver_t* driver, const platform_uart_t* peripheral, const platform_uart_config_t* config, ring_buffer_t* optional_ring_buffer )
{
  	OSStatus          err = kNoErr;
	int wordlen;
	SerialParity parity;
	int stopbit;
	FlowControl flowcontrol;
   
  	platform_mcu_powersave_disable();

  	require_action_quiet( ( driver != NULL ) && ( peripheral != NULL ) && ( config != NULL ), exit, err = kParamErr);
  	require_action_quiet( (optional_ring_buffer == NULL) || ((optional_ring_buffer->buffer != NULL ) && (optional_ring_buffer->size != 0)), exit, err = kParamErr);

  	driver->rx_size              = 0;
  	driver->tx_size              = 0;
  	driver->last_transmit_result = kNoErr;
  	driver->last_receive_result  = kNoErr;
  	driver->peripheral           = (platform_uart_t*)peripheral;
#ifndef NO_MICO_RTOS
  	aos_sem_new( &driver->tx_complete, 1 );
  	aos_sem_new( &driver->rx_complete, 0 );
  	aos_sem_new( &driver->sem_wakeup,  0 );
  	aos_mutex_new( &driver->tx_mutex );    
#else
  	driver->tx_complete = false;
  	driver->rx_complete = false;
#endif

    	if (optional_ring_buffer != NULL){
      		/* Note that the ring_buffer should've been initialised first */
      		driver->rx_buffer = optional_ring_buffer;
      		driver->rx_size   = 0;
      		//platform_uart_receive_bytes( uart, optional_rx_buffer->buffer, optional_rx_buffer->size, 0 );
    	}else{
	      	driver->rx_buffer = NULL;
	}
            

        if(peripheral->tx == PA_30 && peripheral->rx == PA_29){
            DBG_8195A("platform_uart_init 22222 \r\n");
	    DIAG_UartReInit((IRQ_FUN) platform_loguart_irq);
	    NVIC_SetPriority(UART_LOG_IRQ, 10); /* this is rom_code_patch */
            LOGUART_SetBaud(config->baud_rate);
            return err;
        }

    	serial_init((serial_t*)&peripheral->serial_obj, peripheral->tx, peripheral->rx);
        
  	switch ( config->data_width)
  	{
    		case DATA_WIDTH_7BIT  :
      			wordlen = 7;
      			break;
    		case DATA_WIDTH_8BIT:
      			wordlen = 8;
      			break;
    		default:
      			err = kParamErr;
      			goto exit;
  	}

  	switch ( config->parity )
  	{
    		case NO_PARITY:
      			parity = ParityNone;
      			break;

    		case EVEN_PARITY:
      			parity = ParityEven;				
      			break;

    		case ODD_PARITY:
      			parity = ParityOdd;				
      			break;
    		default:
      			err = kParamErr;
      			goto exit;
  	}

  	switch ( config->stop_bits )
  	{
    		case STOP_BITS_1:
      			stopbit = 1;
      			break;
    		case STOP_BITS_2:
      			stopbit = 2;
      			break;
    		default:
      			err = kParamErr;
      			goto exit;
  	} 

  	switch ( config->flow_control )
  	{
    		case FLOW_CONTROL_DISABLED:
			flowcontrol = FlowControlNone;
			break;
    		case FLOW_CONTROL_RTS:		
      			flowcontrol = FlowControlRTS;
      			break;
    		case FLOW_CONTROL_CTS:		
      			flowcontrol = FlowControlCTS;
      			break;
    		case FLOW_CONTROL_CTS_RTS:		
      			flowcontrol = FlowControlRTSCTS;
      			break;
    		default:
      			err = kParamErr;
      			goto exit;
  	}

	/*only UART0 support flow control*/
	if(config->flow_control != FLOW_CONTROL_DISABLED &&  peripheral->serial_obj.uart_idx != 0)
	{
      		err = kUnsupportedErr;
      		goto exit;		
	}else if(peripheral->serial_obj.uart_idx == 0)
		serial_set_flow_control((serial_t*)&peripheral->serial_obj, flowcontrol, peripheral->rx, peripheral->tx);	


    	serial_baud((serial_t*)&peripheral->serial_obj,config->baud_rate);
    	serial_format((serial_t*)&peripheral->serial_obj, wordlen, parity, stopbit);
    	serial_irq_set((serial_t*)&peripheral->serial_obj, RxIrq, 1);
    	serial_irq_set((serial_t*)&peripheral->serial_obj, TxIrq, 1);

    	if(!optional_ring_buffer){
      		serial_recv_comp_handler((serial_t*)&peripheral->serial_obj, (void*)platform_uart_rx_dma_irq, (uint32_t) driver);
    	}else{  
          	serial_irq_handler((serial_t*)&peripheral->serial_obj, platform_uart_irq, (uint32_t)driver);	  
    	}

    	serial_send_comp_handler((serial_t*)&peripheral->serial_obj, (void*)platform_uart_tx_dma_irq, (uint32_t) driver);

		
exit:
    	platform_mcu_powersave_enable();
  	return err;
}

OSStatus platform_uart_deinit( platform_uart_driver_t* driver )
{
  	uint8_t          uart_number;
  	OSStatus          err = kNoErr;

  	platform_mcu_powersave_disable();

  	require_action_quiet( ( driver != NULL ), exit, err = kParamErr);

        if(driver->peripheral->tx == PA_30 && driver->peripheral->rx == PA_29){
            
        }else{
  	    serial_free((serial_t*)&driver->peripheral->serial_obj);
        }
  
#ifndef NO_MICO_RTOS
  	aos_sem_free( &driver->rx_complete );
  	aos_sem_free( &driver->tx_complete );
  	aos_mutex_free( &driver->tx_mutex );
   	aos_sem_free( &driver->sem_wakeup);
#else
  	driver->rx_complete = false;
  	driver->tx_complete = true;
#endif
  	driver->rx_size              = 0;
  	driver->tx_size              = 0;
  	driver->last_transmit_result = kNoErr;
  	driver->last_receive_result  = kNoErr;

exit:
    	platform_mcu_powersave_enable();
    	return err;
}

OSStatus platform_uart_transmit_bytes( platform_uart_driver_t* driver, const uint8_t* data_out, uint32_t size )
{
  	OSStatus err = kNoErr;
    	int32_t ret;
		
  	platform_mcu_powersave_disable();  

#if 0
/* Wait for transmission complete */
#ifndef NO_MICO_RTOS
  	ret = aos_sem_wait( &driver->tx_complete, AOS_WAIT_FOREVER );
        if(ret != 0)
            DBG_8195A("ret %d, sema 0x%x\r\n", ret, &driver->tx_complete);
#else 
  	while( driver->tx_complete == false ){ 
            rtw_msleep_os(2);
        }    
  	driver->tx_complete = false;
#endif
#endif
        if(driver->peripheral->tx == PA_30 && driver->peripheral->rx == PA_29){
            while (size){
		DiagPrintf("%c", *data_out);
       	        size--;
		data_out++;
	    }
        }else{
            while (size){
		serial_putc((serial_t*)&driver->peripheral->serial_obj, *data_out);
       	        size--;
		data_out++;
	    }
        }

#if 0
Retry:
	//ret = serial_send_stream_dma((serial_t*)&driver->peripheral->serial_obj, (char*)data_out, size);
	ret = serial_send_stream((serial_t*)&driver->peripheral->serial_obj, (char*)data_out, size);	
    	if(ret != 0){
                DBG_8195A("** %d\r\n", size);
      		err = kGeneralErr;
		goto Retry;
	}
#endif

  	/* Disable DMA and clean up */
  	driver->tx_size = 0;
  	err = driver->last_transmit_result;

exit:  
  	platform_mcu_powersave_enable();
  	return err;
}

OSStatus platform_uart_receive_bytes( platform_uart_driver_t* driver, uint8_t* data_in, uint32_t expected_data_size, uint32_t timeout_ms )
{
  	OSStatus err = kNoErr;

  	platform_mcu_powersave_disable();

  	require_action_quiet( ( driver != NULL ) && ( data_in != NULL ) && ( expected_data_size != 0 ), exit, err = kParamErr);

  	if ( driver->rx_buffer != NULL )
  	{
    		while (expected_data_size != 0)
    		{
      			uint32_t transfer_size = MIN( driver->rx_buffer->size/2, expected_data_size );
      
      			/* Check if ring buffer already contains the required amount of data. */
      			if ( transfer_size > ring_buffer_used_space( driver->rx_buffer ) )
      			{
        			/* Set rx_size and wait in rx_complete semaphore until data reaches rx_size or timeout occurs */
        			driver->rx_size = transfer_size;
        
#ifndef NO_MICO_RTOS
        			if ( aos_sem_wait( &driver->rx_complete, timeout_ms) != kNoErr )
        			{
          				driver->rx_size = 0;
          				return kTimeoutErr;
        			}
#else
        			driver->rx_complete = false;
        			u32 delay_start = SYSTIMER_TickGet();
        			while(driver->rx_complete == false){
          				if(SYSTIMER_TickGet() >= delay_start + timeout_ms && timeout_ms != MICO_NEVER_TIMEOUT){
            					driver->rx_size = 0;
            					return kTimeoutErr;
          				}
        			}
#endif      
        			/* Reset rx_size to prevent semaphore being set while nothing waits for the data */
        			driver->rx_size = 0;
      			}
      
      			expected_data_size -= transfer_size;
      
      			// Grab data from the buffer
      			do
      			{
        			uint8_t* available_data;
        			uint32_t bytes_available;
        
        			ring_buffer_get_data( driver->rx_buffer, &available_data, &bytes_available );
        			bytes_available = MIN( bytes_available, transfer_size );
        			memcpy( data_in, available_data, bytes_available );
        			transfer_size -= bytes_available;
        			data_in = ( (uint8_t*) data_in + bytes_available );
        			ring_buffer_consume( driver->rx_buffer, bytes_available );
      			} while ( transfer_size != 0 );
    		}
    
    		if ( expected_data_size != 0 )
    		{
      			return kGeneralErr;
    		}
    		else
    		{
      			return kNoErr;
    		}
  	}
  	else
  	{
      		err = serial_recv_stream_dma_timeout((serial_t*)&driver->peripheral->serial_obj, data_in, expected_data_size, timeout_ms, NULL );
		if(err == expected_data_size)
			return kNoErr;
		else
			return kTimeoutErr;
	}
  
exit:
  	platform_mcu_powersave_enable();
  	return err;
}


uint32_t platform_uart_get_length_in_buffer( platform_uart_driver_t* driver )
{  
  	return ring_buffer_used_space( driver->rx_buffer );
}


#if 0
uint8_t platform_uart_get_port_number( USART_TypeDef* uart )
{
    if ( uart == USART1 )
    {
        return 0;
    }
    else if ( uart == USART2 )
    {
        return 1;
    }
    else if ( uart == USART3 )
    {
        return 2;
    }
    else if ( uart == UART4 )
    {
        return 3;
    }
    else if ( uart == UART5 )
    {
        return 4;
    }
    else if ( uart == USART6 )
    {
        return 5;
    }
    else
    {
        return 0xff;
    }
}
#endif

#ifndef NO_MICO_RTOS
static void thread_wakeup(void *arg)
{
#if 0
  platform_uart_driver_t* driver = arg;
  
  while(1){
    if( mico_rtos_get_semaphore( driver->sem_wakeup, 1000) != kNoErr )
    {
      platform_gpio_irq_enable( driver->peripheral->pin_rx, IRQ_TRIGGER_FALLING_EDGE, RX_PIN_WAKEUP_handler, driver );
      platform_mcu_powersave_enable( );
    }
  }
#endif  
}
#endif

/******************************************************
*            Interrupt Service Routines
******************************************************/
#ifndef NO_MICO_RTOS
void RX_PIN_WAKEUP_handler(void *arg)
{
#if 0
  (void)arg;
  platform_uart_driver_t* driver = arg;
  uint32_t uart_number;
  
  platform_gpio_enable_clock( driver->peripheral->pin_rx );

  uart_number = platform_uart_get_port_number( driver->peripheral->port );

  uart_peripheral_clock_functions[ uart_number ]( uart_peripheral_clocks[ uart_number ], ENABLE );

  /* Enable DMA peripheral clock */
  if ( driver->peripheral->tx_dma_config.controller == DMA1 )
  {
      RCC->AHB1ENR |= RCC_AHB1Periph_DMA1;
  }
  else
  {
      RCC->AHB1ENR |= RCC_AHB1Periph_DMA2;
  }

  platform_gpio_irq_disable( driver->peripheral->pin_rx );
  platform_mcu_powersave_disable( );
  mico_rtos_set_semaphore( &driver->sem_wakeup );
#endif
}
#endif

extern platform_uart_driver_t platform_uart_drivers[MICO_UART_MAX];
void platform_loguart_irq( void* id)
{
    platform_uart_driver_t* driver = &platform_uart_drivers[0];
    u8      UartReceiveData = 0;
    BOOL    PullMode = _FALSE;
    volatile u8 reg_iir;
    u32 RegValue;

    reg_iir = UART_IntStatus(UART2_DEV);
    if ((reg_iir & RUART_RECEIVE_LINE_STATUS) != 0) {
        RegValue = UART_LineStatusGet(UART2_DEV);
        return;
    }

    u32 IrqEn = DiagGetIsrEnReg();

    DiagSetIsrEnReg(0);

    UartReceiveData = DiagGetChar(PullMode);

    ring_buffer_write( driver->rx_buffer, &UartReceiveData,1 );

    		// Notify thread if sufficient data are available
    if ( ( driver->rx_size > 0 ) &&
            ( ring_buffer_used_space( driver->rx_buffer ) >= driver->rx_size ) )
    {
  #ifndef NO_MICO_RTOS
        aos_sem_signal( &driver->rx_complete );
  #else
        driver->rx_complete = true;
  #endif
        driver->rx_size = 0;
    }
    
    DiagSetIsrEnReg(IrqEn);
}

void platform_uart_irq( uint32_t id, SerialIrq event)
{
  	uint8_t rxData;
	platform_uart_driver_t* driver = (platform_uart_driver_t*)id;
	if(event == RxIrq){
    		rxData = serial_getc((serial_t*)&driver->peripheral->serial_obj);	
    		ring_buffer_write( driver->rx_buffer, &rxData,1 );

    		//DBG_8195A("\r\nnuart irq\r\n");	 

    		// Notify thread if sufficient data are available
    		if ( ( driver->rx_size > 0 ) &&
        		( ring_buffer_used_space( driver->rx_buffer ) >= driver->rx_size ) )
    		{
  #ifndef NO_MICO_RTOS
      			aos_sem_signal( &driver->rx_complete );
  #else
      			driver->rx_complete = true;
  #endif
      			driver->rx_size = 0;
    		}
	}
}

void platform_uart_tx_dma_irq( uint32_t id )
{
	platform_uart_driver_t* driver = (platform_uart_driver_t*)id;
/* Set semaphore regardless of result to prevent waiting thread from locking up */
#ifndef NO_MICO_RTOS
	aos_sem_signal( &driver->tx_complete );
#else
	driver->tx_complete = true;
#endif
}

void platform_uart_rx_dma_irq( uint32_t id )
{
	platform_uart_driver_t* driver = (platform_uart_driver_t*)id;

/* Set semaphore regardless of result to prevent waiting thread from locking up */
#ifndef NO_MICO_RTOS
	aos_sem_signal( &driver->rx_complete );
#else
	driver->rx_complete = true;
#endif
}



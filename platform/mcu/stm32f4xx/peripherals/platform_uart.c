
#include "platform.h"
#include "platform_peripheral.h"
#include "debug.h"

/******************************************************
*                    Constants
******************************************************/

#define DMA_INTERRUPT_FLAGS  ( DMA_IT_TC | DMA_IT_TE | DMA_IT_DME | DMA_IT_FE )

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
/* UART alternate functions */
static const uint8_t uart_alternate_functions[NUMBER_OF_UART_PORTS] =
{
    [0] = GPIO_AF_USART1,
    [1] = GPIO_AF_USART2,
    [2] = GPIO_AF_USART3,
    [3] = GPIO_AF_UART4,
    [4] = GPIO_AF_UART5,
    [5] = GPIO_AF_USART6,
};

/* UART peripheral clock functions */
static const platform_peripheral_clock_function_t uart_peripheral_clock_functions[NUMBER_OF_UART_PORTS] =
{
    [0] = RCC_APB2PeriphClockCmd,
    [1] = RCC_APB1PeriphClockCmd,
    [2] = RCC_APB1PeriphClockCmd,
    [3] = RCC_APB1PeriphClockCmd,
    [4] = RCC_APB1PeriphClockCmd,
    [5] = RCC_APB2PeriphClockCmd,
};

/* UART peripheral clocks */
static const uint32_t uart_peripheral_clocks[NUMBER_OF_UART_PORTS] =
{
    [0] = RCC_APB2Periph_USART1,
    [1] = RCC_APB1Periph_USART2,
    [2] = RCC_APB1Periph_USART3,
    [3] = RCC_APB1Periph_UART4,
    [4] = RCC_APB1Periph_UART5,
    [5] = RCC_APB2Periph_USART6,
};

/* UART interrupt vectors */
#if defined (STM32F401xx) || defined (STM32F411xE)
static const IRQn_Type uart_irq_vectors[NUMBER_OF_UART_PORTS] =
{
    [0] = USART1_IRQn,
    [1] = USART2_IRQn,
    [2] = (IRQn_Type)0xFF,
    [3] = (IRQn_Type)0xFF,
    [4] = (IRQn_Type)0xFF,
    [5] = USART6_IRQn,
};
#elif defined (STM32F412xG)
static const IRQn_Type uart_irq_vectors[NUMBER_OF_UART_PORTS] =
{
    [0] = USART1_IRQn,
    [1] = USART2_IRQn,
    [2] = USART3_IRQn,
    [3] = (IRQn_Type)0xFF,
    [4] = (IRQn_Type)0xFF,
    [5] = USART6_IRQn,
};
#else
static const IRQn_Type uart_irq_vectors[NUMBER_OF_UART_PORTS] =
{
    [0] = USART1_IRQn,
    [1] = USART2_IRQn,
    [2] = USART3_IRQn,
    [3] = UART4_IRQn,
    [4] = UART5_IRQn,
    [5] = USART6_IRQn,
};
#endif

/******************************************************
*        Static Function Declarations
******************************************************/
static OSStatus receive_bytes       ( platform_uart_driver_t* driver, void* data, uint32_t size, uint32_t timeout );
static uint32_t get_dma_irq_status  ( DMA_Stream_TypeDef* stream );
static void     clear_dma_interrupts( DMA_Stream_TypeDef* stream, uint32_t flags );

/******************************************************
*               Function Definitions
******************************************************/

OSStatus platform_uart_init( platform_uart_driver_t* driver, const platform_uart_t* peripheral, const platform_uart_config_t* config, ring_buffer_t* optional_ring_buffer )
{
  DMA_InitTypeDef   dma_init_structure;
  USART_InitTypeDef uart_init_structure;
  uint32_t          uart_number;
  OSStatus          err = kNoErr;

  platform_mcu_powersave_disable();

  require_action_quiet( ( driver != NULL ) && ( peripheral != NULL ) && ( config != NULL ), exit, err = kParamErr);
  require_action_quiet( (optional_ring_buffer == NULL) || ((optional_ring_buffer->buffer != NULL ) && (optional_ring_buffer->size != 0)), exit, err = kParamErr);
  
  uart_number = platform_uart_get_port_number( peripheral->port );

  driver->rx_size              = 0;
  driver->tx_size              = 0;
  driver->last_transmit_result = kNoErr;
  driver->last_receive_result  = kNoErr;
  driver->peripheral           = (platform_uart_t*)peripheral;

  if( driver->initialized == false )  
  {
    mico_rtos_init_semaphore( &driver->tx_complete, 1 );
    mico_rtos_init_semaphore( &driver->rx_complete, 1 );
    mico_rtos_init_mutex    ( &driver->tx_mutex );

    /* Configure TX and RX pin_mapping */
    platform_gpio_set_alternate_function( peripheral->pin_tx->port, peripheral->pin_tx->pin_number, GPIO_OType_PP, GPIO_PuPd_UP, uart_alternate_functions[ uart_number ] );
    platform_gpio_set_alternate_function( peripheral->pin_rx->port, peripheral->pin_rx->pin_number, GPIO_OType_PP, GPIO_PuPd_UP, uart_alternate_functions[ uart_number ] );    
  }

  if ( ( peripheral->pin_cts != NULL ) && ( config->flow_control == FLOW_CONTROL_CTS || config->flow_control == FLOW_CONTROL_CTS_RTS ) )
  {
      platform_gpio_set_alternate_function( peripheral->pin_cts->port, peripheral->pin_cts->pin_number, GPIO_OType_PP, GPIO_PuPd_NOPULL, uart_alternate_functions[ uart_number ] );
  }

  if ( ( peripheral->pin_rts != NULL ) && ( config->flow_control == FLOW_CONTROL_RTS || config->flow_control == FLOW_CONTROL_CTS_RTS ) )
  {
      platform_gpio_set_alternate_function( peripheral->pin_rts->port, peripheral->pin_rts->pin_number, GPIO_OType_PP, GPIO_PuPd_NOPULL, uart_alternate_functions[ uart_number ] );
  }
  
  /* Enable UART peripheral clock */
  uart_peripheral_clock_functions[ uart_number ]( uart_peripheral_clocks[ uart_number ], ENABLE );

  uart_init_structure.USART_Mode       = USART_Mode_Rx | USART_Mode_Tx;
  uart_init_structure.USART_BaudRate   = config->baud_rate;
  uart_init_structure.USART_WordLength = ( ( config->data_width == DATA_WIDTH_9BIT ) || ( ( config->data_width == DATA_WIDTH_8BIT ) && ( config->parity != NO_PARITY ) ) ) ? USART_WordLength_9b : USART_WordLength_8b;
  uart_init_structure.USART_StopBits   = ( config->stop_bits == STOP_BITS_1 ) ? USART_StopBits_1 : USART_StopBits_2;

  switch ( config->parity )
  {
    case NO_PARITY:
      uart_init_structure.USART_Parity = USART_Parity_No;
      break;

    case EVEN_PARITY:
      uart_init_structure.USART_Parity = USART_Parity_Even;
      break;

    case ODD_PARITY:
      uart_init_structure.USART_Parity = USART_Parity_Odd;
      break;

    default:
      err = kParamErr;
      goto exit;
  }

  switch ( config->flow_control )
  {
    case FLOW_CONTROL_DISABLED:
      uart_init_structure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
      break;

    case FLOW_CONTROL_CTS:
      uart_init_structure.USART_HardwareFlowControl = USART_HardwareFlowControl_CTS;
      break;

    case FLOW_CONTROL_RTS:
      uart_init_structure.USART_HardwareFlowControl = USART_HardwareFlowControl_RTS;
      driver->is_flow_control = true;
      break;

    case FLOW_CONTROL_CTS_RTS:
      uart_init_structure.USART_HardwareFlowControl = USART_HardwareFlowControl_RTS_CTS;
      driver->is_flow_control = true;
      break;

    default:
      err = kParamErr;
      goto exit;
  }


  /* Initialise USART peripheral */
  USART_DeInit( peripheral->port );
  USART_Init( peripheral->port, &uart_init_structure );
  
  /**************************************************************************
  * Initialise STM32 DMA registers
  * Note: If DMA is used, USART interrupt isn't enabled.
  **************************************************************************/
  /* Enable DMA peripheral clock */
  if ( peripheral->tx_dma_config.controller == DMA1 )
  {
      RCC->AHB1ENR |= RCC_AHB1Periph_DMA1;
  }
  else
  {
      RCC->AHB1ENR |= RCC_AHB1Periph_DMA2;
  }
  
  /* Fill init structure with common DMA settings */
  dma_init_structure.DMA_PeripheralInc   = DMA_PeripheralInc_Disable;
  dma_init_structure.DMA_MemoryInc       = DMA_MemoryInc_Enable;
  dma_init_structure.DMA_Priority        = DMA_Priority_VeryHigh;
  dma_init_structure.DMA_FIFOMode        = DMA_FIFOMode_Disable;
  dma_init_structure.DMA_FIFOThreshold   = DMA_FIFOThreshold_Full;
  dma_init_structure.DMA_MemoryBurst     = DMA_MemoryBurst_Single;
  dma_init_structure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;

  if ( config->data_width == DATA_WIDTH_9BIT )
  {
    dma_init_structure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    dma_init_structure.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;
  }
  else
  {
    dma_init_structure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    dma_init_structure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
  }

  /* Initialise TX DMA */
  DMA_DeInit( peripheral->tx_dma_config.stream );
  dma_init_structure.DMA_Channel            = peripheral->tx_dma_config.channel;
  dma_init_structure.DMA_PeripheralBaseAddr = (uint32_t) &peripheral->port->DR;
  dma_init_structure.DMA_Memory0BaseAddr    = (uint32_t) 0;
  dma_init_structure.DMA_DIR                = DMA_DIR_MemoryToPeripheral;
  dma_init_structure.DMA_BufferSize         = 0xFFFF;                     // This parameter will be configured during communication
  dma_init_structure.DMA_Mode               = DMA_Mode_Normal;
  DMA_Init( peripheral->tx_dma_config.stream, &dma_init_structure );

  /* Initialise RX DMA */
  DMA_DeInit( peripheral->rx_dma_config.stream );
  dma_init_structure.DMA_Channel            = peripheral->rx_dma_config.channel;
  dma_init_structure.DMA_PeripheralBaseAddr = (uint32_t) &peripheral->port->DR;
  dma_init_structure.DMA_Memory0BaseAddr    = (uint32_t) 0;
  dma_init_structure.DMA_DIR                = DMA_DIR_PeripheralToMemory;
  dma_init_structure.DMA_BufferSize         = 0xFFFF;                     // This parameter will be configured during communication
  dma_init_structure.DMA_Mode               = DMA_Mode_Normal;
  DMA_Init( peripheral->rx_dma_config.stream, &dma_init_structure );

  /**************************************************************************
  * Initialise STM32 DMA interrupts
  **************************************************************************/
  
  /* Configure TX DMA interrupt on Cortex-M3 */
  NVIC_EnableIRQ( peripheral->tx_dma_config.irq_vector );

  /* Enable TC (transfer complete) and TE (transfer error) interrupts on source */
  clear_dma_interrupts( peripheral->tx_dma_config.stream, peripheral->tx_dma_config.complete_flags | peripheral->tx_dma_config.error_flags );
  DMA_ITConfig( peripheral->tx_dma_config.stream, DMA_INTERRUPT_FLAGS, ENABLE );

  /* Enable USART interrupt vector in Cortex-M3 */
  NVIC_EnableIRQ( uart_irq_vectors[uart_number] );
  USART_DMACmd( driver->peripheral->port, USART_DMAReq_Tx, DISABLE );

  /* Enable USART */
  USART_Cmd( peripheral->port, ENABLE );

  /* Enable both transmit and receive */
  peripheral->port->CR1 |= USART_CR1_TE;
  peripheral->port->CR1 |= USART_CR1_RE;

  /* Setup ring buffer */
  if ( optional_ring_buffer != NULL )
  {
    /* Note that the ring_buffer should've been initialised first */
    driver->rx_buffer = optional_ring_buffer;
    driver->rx_size   = 0;
    driver->peripheral->rx_dma_config.stream->CR |= DMA_SxCR_CIRC;

    // Enabled individual byte interrupts so progress can be updated
    USART_ClearITPendingBit( driver->peripheral->port, USART_IT_RXNE );
    USART_ITConfig( driver->peripheral->port, USART_IT_RXNE, ENABLE );
  }
  else
  {
    /* Not using ring buffer. Configure RX DMA interrupt on Cortex-M3 */
    NVIC_EnableIRQ( peripheral->rx_dma_config.irq_vector );

    /* Enable TC (transfer complete) and TE (transfer error) interrupts on source */
    clear_dma_interrupts( peripheral->rx_dma_config.stream, peripheral->rx_dma_config.complete_flags | peripheral->rx_dma_config.error_flags );
    DMA_ITConfig( peripheral->rx_dma_config.stream, DMA_INTERRUPT_FLAGS, ENABLE );
  }

  driver->initialized = true;
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

  uart_number = platform_uart_get_port_number( driver->peripheral->port );

  /* Disable USART */
  USART_Cmd( driver->peripheral->port, DISABLE );

  /* Deinitialise USART */
  USART_DeInit( driver->peripheral->port );

  /**************************************************************************
   * De-initialise STM32 DMA and interrupt
   **************************************************************************/

  /* Deinitialise DMA streams */
  DMA_DeInit( driver->peripheral->tx_dma_config.stream );
  DMA_DeInit( driver->peripheral->rx_dma_config.stream );

  /* Disable TC (transfer complete) interrupt at the source */
  DMA_ITConfig( driver->peripheral->tx_dma_config.stream, DMA_INTERRUPT_FLAGS, DISABLE );
  DMA_ITConfig( driver->peripheral->rx_dma_config.stream, DMA_INTERRUPT_FLAGS, DISABLE );

  /* Disable transmit DMA interrupt at Cortex-M3 */
  NVIC_DisableIRQ( driver->peripheral->tx_dma_config.irq_vector );

  /**************************************************************************
   * De-initialise STM32 USART interrupt
   **************************************************************************/

  USART_ITConfig( driver->peripheral->port, USART_IT_RXNE, DISABLE );

  /* Disable UART interrupt vector on Cortex-M3 */
  NVIC_DisableIRQ( driver->peripheral->rx_dma_config.irq_vector );

  /* Disable registers clocks */
  uart_peripheral_clock_functions[uart_number]( uart_peripheral_clocks[uart_number], DISABLE );

  mico_rtos_deinit_semaphore( &driver->rx_complete );
  mico_rtos_deinit_semaphore( &driver->tx_complete );
  mico_rtos_deinit_mutex( &driver->tx_mutex );
  driver->rx_size              = 0;
  driver->tx_size              = 0;
  driver->last_transmit_result = kNoErr;
  driver->last_receive_result  = kNoErr;
  driver->initialized          = false;

exit:
    platform_mcu_powersave_enable();
    return err;
}

OSStatus platform_uart_transmit_bytes( platform_uart_driver_t* driver, const uint8_t* data_out, uint32_t size )
{
  OSStatus err = kNoErr;

  platform_mcu_powersave_disable();
  
  mico_rtos_lock_mutex( &driver->tx_mutex );

  require_action_quiet( ( driver != NULL ) && ( data_out != NULL ) && ( size != 0 ), exit, err = kParamErr);

  /* Clear interrupt status before enabling DMA otherwise error occurs immediately */
  clear_dma_interrupts( driver->peripheral->tx_dma_config.stream, driver->peripheral->tx_dma_config.complete_flags | driver->peripheral->tx_dma_config.error_flags );

  /* Init DMA parameters and variables */
  driver->last_transmit_result                    = kGeneralErr;
  driver->tx_size                                 = size;
  driver->peripheral->tx_dma_config.stream->CR   &= ~(uint32_t) DMA_SxCR_CIRC;
  driver->peripheral->tx_dma_config.stream->NDTR  = size;
  driver->peripheral->tx_dma_config.stream->M0AR  = (uint32_t)data_out;
  
  USART_DMACmd( driver->peripheral->port, USART_DMAReq_Tx, ENABLE );
  USART_ClearFlag( driver->peripheral->port, USART_FLAG_TC );
  driver->peripheral->tx_dma_config.stream->CR   |= DMA_SxCR_EN;
  
/* Wait for transmission complete */
  mico_rtos_get_semaphore( &driver->tx_complete, 100 );

  while ( ( driver->peripheral->port->SR & USART_SR_TC ) == 0 )
  {
  }

  /* Disable DMA and clean up */
  USART_DMACmd( driver->peripheral->port, USART_DMAReq_Tx, DISABLE );
  driver->tx_size = 0;
  err = driver->last_transmit_result;

exit:  
  mico_rtos_unlock_mutex( &driver->tx_mutex );
  platform_mcu_powersave_enable();
  return err;
}

OSStatus platform_uart_receive_bytes( platform_uart_driver_t* driver, uint8_t* data_in, uint32_t expected_data_size, uint32_t timeout_ms )
{
  OSStatus err = kNoErr;

  //platform_mcu_powersave_disable();

  require_action_quiet( ( driver != NULL ) && ( data_in != NULL ) && ( expected_data_size != 0 ), exit, err = kParamErr);

  mico_rtos_get_semaphore( &driver->rx_complete, 0 );

  if ( driver->rx_buffer != NULL)
  {
      if( ( driver->is_flow_control == true  ) &&  ( driver->is_recv_over_flow == true ) )
      {
          driver->is_recv_over_flow = false;
          USART_ITConfig( driver->peripheral->port, USART_IT_RXNE, ENABLE );
      }

    while ( expected_data_size != 0 )
    {
      uint32_t transfer_size = MIN( driver->rx_buffer->size / 2, expected_data_size );

        /* Set rx_size and wait in rx_complete semaphore until data reaches rx_size or timeout occurs */
        driver->last_receive_result = kNoErr;
        driver->rx_size             = transfer_size;
      
      /* Check if ring buffer already contains the required amount of data. */
      if ( transfer_size > ring_buffer_used_space( driver->rx_buffer ) )
      {
        err = mico_rtos_get_semaphore( &driver->rx_complete, timeout_ms );

        /* Reset rx_size to prevent semaphore being set while nothing waits for the data */
        driver->rx_size = 0;

        if( err != kNoErr )
          goto exit;
      }else {
        driver->rx_size = 0;
      }
      err = driver->last_receive_result;
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
  }
  else
  {
    err = receive_bytes( driver, data_in, expected_data_size, timeout_ms );
  }
exit:
  //platform_mcu_powersave_enable();
  return err;
}

static OSStatus receive_bytes( platform_uart_driver_t* driver, void* data, uint32_t size, uint32_t timeout )
{
  OSStatus err = kNoErr;

  if ( driver->rx_buffer != NULL )
  {
    driver->peripheral->rx_dma_config.stream->CR |= DMA_SxCR_CIRC;
    
    // Enabled individual byte interrupts so progress can be updated
    USART_ClearITPendingBit( driver->peripheral->port, USART_IT_RXNE );
    USART_ITConfig( driver->peripheral->port, USART_IT_RXNE, ENABLE );
  }
  else
  {
    driver->rx_size = size;
    driver->peripheral->rx_dma_config.stream->CR &= ~(uint32_t) DMA_SxCR_CIRC;
  }
  
  clear_dma_interrupts( driver->peripheral->rx_dma_config.stream, driver->peripheral->rx_dma_config.complete_flags | driver->peripheral->rx_dma_config.error_flags );

  driver->peripheral->rx_dma_config.stream->NDTR  = size;
  driver->peripheral->rx_dma_config.stream->M0AR  = (uint32_t)data;
  driver->peripheral->rx_dma_config.stream->CR   |= DMA_SxCR_EN;
  USART_DMACmd( driver->peripheral->port, USART_DMAReq_Rx, ENABLE );
  
  if ( timeout > 0 )
  {
    err = mico_rtos_get_semaphore( &driver->rx_complete, timeout );
  }
  return err;
}

uint32_t platform_uart_get_length_in_buffer( platform_uart_driver_t* driver )
{  
  return ring_buffer_used_space( driver->rx_buffer );
}

static void clear_dma_interrupts( DMA_Stream_TypeDef* stream, uint32_t flags )
{
    if ( stream <= DMA1_Stream3 )
    {
        DMA1->LIFCR |= flags;
    }
    else if ( stream <= DMA1_Stream7 )
    {
        DMA1->HIFCR |= flags;
    }
    else if ( stream <= DMA2_Stream3 )
    {
        DMA2->LIFCR |= flags;
    }
    else
    {
        DMA2->HIFCR |= flags;
    }
}

static uint32_t get_dma_irq_status( DMA_Stream_TypeDef* stream )
{
    if ( stream <= DMA1_Stream3 )
    {
        return DMA1->LISR;
    }
    else if ( stream <= DMA1_Stream7 )
    {
        return DMA1->HISR;
    }
    else if ( stream <= DMA2_Stream3 )
    {
        return DMA2->LISR;
    }
    else
    {
        return DMA2->HISR;
    }
}

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


/******************************************************
*            Interrupt Service Routines
******************************************************/


void platform_uart_irq( platform_uart_driver_t* driver )
{
    uint8_t recv_byte = 0;
    platform_uart_port_t* uart = (platform_uart_port_t*) driver->peripheral->port;

    /* Receive new data */
    if ( USART_GetITStatus( uart, USART_IT_RXNE ) == SET )
    {
         if ( ring_buffer_is_full( driver->rx_buffer ) == 0 )
         {
              recv_byte = USART_ReceiveData( uart );
              ring_buffer_write( driver->rx_buffer, &recv_byte, 1 );
              if ( ( driver->rx_size > 0 ) && ( ring_buffer_used_space( driver->rx_buffer ) >= driver->rx_size ) )
              {
                   mico_rtos_set_semaphore( &driver->rx_complete );
                   driver->rx_size = 0;
              }
          } else
          {
              if( driver->is_flow_control == true ){
                   USART_ITConfig( driver->peripheral->port, USART_IT_RXNE, DISABLE );
                   driver->is_recv_over_flow = true;
              }else{
                   USART_ReceiveData( uart );
              }
          }
    }
}

void platform_uart_tx_dma_irq( platform_uart_driver_t* driver )
{
    if ( ( get_dma_irq_status( driver->peripheral->tx_dma_config.stream ) & driver->peripheral->tx_dma_config.complete_flags ) != 0 )
    {
        clear_dma_interrupts( driver->peripheral->tx_dma_config.stream, driver->peripheral->tx_dma_config.complete_flags );
        driver->last_transmit_result = kNoErr;
    }

    if ( ( get_dma_irq_status( driver->peripheral->tx_dma_config.stream ) & driver->peripheral->tx_dma_config.error_flags ) != 0 )
    {
        clear_dma_interrupts( driver->peripheral->tx_dma_config.stream, driver->peripheral->tx_dma_config.error_flags );
        driver->last_transmit_result = kGeneralErr;
    }

    if ( driver->tx_size > 0 )
    {
        /* Set semaphore regardless of result to prevent waiting thread from locking up */
        mico_rtos_set_semaphore( &driver->tx_complete );
    }
}

void platform_uart_rx_dma_irq( platform_uart_driver_t* driver )
{
    if ( ( get_dma_irq_status( driver->peripheral->rx_dma_config.stream ) & driver->peripheral->rx_dma_config.complete_flags ) != 0 )
    {
        clear_dma_interrupts( driver->peripheral->rx_dma_config.stream, driver->peripheral->rx_dma_config.complete_flags );
        driver->last_receive_result = kNoErr;
    }

    if ( ( get_dma_irq_status( driver->peripheral->rx_dma_config.stream ) & driver->peripheral->rx_dma_config.error_flags ) != 0 )
    {
        clear_dma_interrupts( driver->peripheral->rx_dma_config.stream, driver->peripheral->rx_dma_config.error_flags );
        driver->last_receive_result = kGeneralErr;
    }

    if ( driver->rx_size > 0 )
    {
        /* Set semaphore regardless of result to prevent waiting thread from locking up */
        mico_rtos_set_semaphore( &driver->rx_complete );
    }
}



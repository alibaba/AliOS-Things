
#include "platform.h"
#include "platform_config.h"
#include "platform_peripheral.h"
#include "debug.h"

/******************************************************
*                    Constants
******************************************************/
#define MAX_NUM_SPI_PRESCALERS     (8)

/******************************************************
*                   Enumerations
******************************************************/

/******************************************************
*                 Type Definitions
******************************************************/

/******************************************************
*                    Structures
******************************************************/
typedef struct
{
  uint16_t factor;
  uint16_t prescaler_value;
} spi_baudrate_division_mapping_t;

/******************************************************
*               Static Function Declarations
******************************************************/

static OSStatus calculate_prescaler( uint32_t speed, uint16_t* prescaler );
static uint16_t spi_transfer       ( const platform_spi_t* spi, uint16_t data );
static OSStatus spi_dma_transfer   ( const platform_spi_t* spi, const platform_spi_config_t* config );
static void     spi_dma_config     ( const platform_spi_t* spi, const platform_spi_message_segment_t* message );

/******************************************************
*               Variables Definitions
******************************************************/
static const spi_baudrate_division_mapping_t spi_baudrate_prescalers[MAX_NUM_SPI_PRESCALERS] =
{
  { 2,   SPI_BaudRatePrescaler_2   },
  { 4,   SPI_BaudRatePrescaler_4   },
  { 8,   SPI_BaudRatePrescaler_8   },
  { 16,  SPI_BaudRatePrescaler_16  },
  { 32,  SPI_BaudRatePrescaler_32  },
  { 64,  SPI_BaudRatePrescaler_64  },
  { 128, SPI_BaudRatePrescaler_128 },
  { 256, SPI_BaudRatePrescaler_256 },
};

/******************************************************
*               Function Definitions
******************************************************/
uint8_t platform_spi_get_port_number( platform_spi_port_t* spi )
{
  if ( spi == SPI1 )
  {
    return 0;
  }
  else if ( spi == SPI2 )
  {
    return 1;
  }
  else if ( spi == SPI3 )
  {
    return 2;
  }
  else
  {
    return 0xFF;
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

OSStatus platform_spi_init( platform_spi_driver_t* driver, const platform_spi_t* peripheral, const platform_spi_config_t* config )
{
  SPI_InitTypeDef   spi_init;
  OSStatus          err;
  
  platform_mcu_powersave_disable();
  
  require_action_quiet( ( driver != NULL ) && ( peripheral != NULL ) && ( config != NULL ), exit, err = kParamErr);

/* Calculate prescaler */
  err = calculate_prescaler( config->speed, &spi_init.SPI_BaudRatePrescaler );
  require_noerr(err, exit);
      
  /* Configure data-width */
  if ( config->bits == 8 )
  {
    spi_init.SPI_DataSize = SPI_DataSize_8b;
  }
  else if ( config->bits == 16 )
  {
    require_action( !(config->mode & SPI_USE_DMA), exit, err = kUnsupportedErr);
    spi_init.SPI_DataSize = SPI_DataSize_16b;
  }
  else
  {
    err = kUnsupportedErr;
    goto exit;
  }
  
  /* Configure MSB or LSB */
  if ( config->mode & SPI_MSB_FIRST )
  {
    spi_init.SPI_FirstBit = SPI_FirstBit_MSB;
  }
  else
  {
    spi_init.SPI_FirstBit = SPI_FirstBit_LSB;
  }
  
  /* Configure mode CPHA and CPOL */
  if ( config->mode & SPI_CLOCK_IDLE_HIGH )
  {
    spi_init.SPI_CPOL = SPI_CPOL_High;
  }
  else
  {
    spi_init.SPI_CPOL = SPI_CPOL_Low;
  }
  
  if ( config->mode & SPI_CLOCK_RISING_EDGE )
  {
    spi_init.SPI_CPHA = ( config->mode & SPI_CLOCK_IDLE_HIGH ) ? SPI_CPHA_2Edge : SPI_CPHA_1Edge;
  }
  else
  {
    spi_init.SPI_CPHA = ( config->mode & SPI_CLOCK_IDLE_HIGH ) ? SPI_CPHA_1Edge : SPI_CPHA_2Edge;
  }

  driver->peripheral = (platform_spi_t *)peripheral;
    
  /* Init SPI GPIOs */
  platform_gpio_set_alternate_function( peripheral->pin_clock->port, peripheral->pin_clock->pin_number, GPIO_OType_PP, GPIO_PuPd_NOPULL, peripheral->gpio_af );
  platform_gpio_set_alternate_function( peripheral->pin_mosi->port,  peripheral->pin_mosi->pin_number,  GPIO_OType_PP, GPIO_PuPd_NOPULL, peripheral->gpio_af );
  platform_gpio_set_alternate_function( peripheral->pin_miso->port,  peripheral->pin_miso->pin_number,  GPIO_OType_PP, GPIO_PuPd_UP, peripheral->gpio_af );
  
  /* Init the chip select GPIO */
  platform_gpio_init( config->chip_select, OUTPUT_PUSH_PULL );
  platform_gpio_output_high( config->chip_select );
  
  
  /* Enable SPI peripheral clock */
  (peripheral->peripheral_clock_func)( peripheral->peripheral_clock_reg, ENABLE );
  (peripheral->peripheral_clock_func)( peripheral->peripheral_clock_reg, ENABLE );
  
  SPI_I2S_DeInit( peripheral->port );
  
  spi_init.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  spi_init.SPI_Mode      = SPI_Mode_Master;
  spi_init.SPI_NSS       = SPI_NSS_Soft;
  spi_init.SPI_CRCPolynomial = 0x7; /* reset value */
  SPI_CalculateCRC( peripheral->port, DISABLE );
  
  /* Init and enable SPI */
  SPI_Init( peripheral->port, &spi_init );
  
  SPI_I2S_DMACmd( peripheral->port, SPI_I2S_DMAReq_Rx, DISABLE );
  SPI_I2S_DMACmd( peripheral->port, SPI_I2S_DMAReq_Tx, DISABLE );
  
  SPI_Cmd ( peripheral->port, ENABLE );
  
  if ( config->mode & SPI_USE_DMA ){
    DMA_DeInit( peripheral->rx_dma.stream );
    DMA_DeInit( peripheral->tx_dma.stream );
    
    if ( peripheral->tx_dma.controller == DMA1 )
    {
      RCC->AHB1ENR |= RCC_AHB1Periph_DMA1;
    }
    else
    {
      RCC->AHB1ENR |= RCC_AHB1Periph_DMA2;
    }
    
     if ( peripheral->rx_dma.controller == DMA1 )
    {
      RCC->AHB1ENR |= RCC_AHB1Periph_DMA1;
    }
    else
    {
      RCC->AHB1ENR |= RCC_AHB1Periph_DMA2;
    }
    SPI_I2S_DMACmd( peripheral->port, SPI_I2S_DMAReq_Rx, ENABLE );
    SPI_I2S_DMACmd( peripheral->port, SPI_I2S_DMAReq_Tx, ENABLE );
  }

exit:
  platform_mcu_powersave_enable();
  return err;
}



OSStatus platform_spi_deinit( platform_spi_driver_t* driver )
{
  UNUSED_PARAMETER( driver );
  /* TODO: unimplemented */
  return kUnsupportedErr;
}


OSStatus platform_spi_transfer( platform_spi_driver_t* driver, const platform_spi_config_t* config, const platform_spi_message_segment_t* segments, uint16_t number_of_segments )
{
  OSStatus err    = kNoErr;
  uint32_t count  = 0;
  uint16_t i;
  
  platform_mcu_powersave_disable();
  
  require_action_quiet( ( driver != NULL ) && ( config != NULL ) && ( segments != NULL ) && ( number_of_segments != 0 ), exit, err = kParamErr);
  
  /* Activate chip select */
  platform_gpio_output_low( config->chip_select );
  
  for ( i = 0; i < number_of_segments; i++ )
  {
    /* Check if we are using DMA */
    if ( config->mode & SPI_USE_DMA )
    {
      if( segments[ i ].length != 0){
        //platform_log( "length: %d, i:%d", segments[ i ].length, i );
        
        spi_dma_config( driver->peripheral, &segments[ i ] );
      
        err = spi_dma_transfer( driver->peripheral, config );
        require_noerr(err, cleanup_transfer);
      }
    }
    else
    {
      count = segments[i].length;
      
      /* in interrupt-less mode */
      if ( config->bits == 8 )
      {
        const uint8_t* send_ptr = ( const uint8_t* )segments[i].tx_buffer;
        uint8_t*       rcv_ptr  = ( uint8_t* )segments[i].rx_buffer;
        
        while ( count-- )
        {
          uint16_t data = 0xFF;
          
          if ( send_ptr != NULL )
          {
            data = *send_ptr++;
          }
          
          data = spi_transfer( driver->peripheral, data );
          
          if ( rcv_ptr != NULL )
          {
            *rcv_ptr++ = (uint8_t)data;
          }
        }
      }
      else if ( config->bits == 16 )
      {
        const uint16_t* send_ptr = (const uint16_t *) segments[i].tx_buffer;
        uint16_t*       rcv_ptr  = (uint16_t *) segments[i].rx_buffer;
        
        /* Check that the message length is a multiple of 2 */
        
        require_action_quiet( ( count % 2 ) == 0, cleanup_transfer, err = kSizeErr);
        
        /* Transmit/receive data stream, 16-bit at time */
        while ( count != 0 )
        {
          uint16_t data = 0xFFFF;
          
          if ( send_ptr != NULL )
          {
            data = *send_ptr++;
          }
          
          data = spi_transfer( driver->peripheral, data );
          
          if ( rcv_ptr != NULL )
          {
            *rcv_ptr++ = data;
          }
          
          count -= 2;
        }
      }
    }
  }
  
cleanup_transfer:
  /* Deassert chip select */
  platform_gpio_output_high( config->chip_select );
  
exit:
  platform_mcu_powersave_enable( );
  return err;
}

static uint16_t spi_transfer( const platform_spi_t* spi, uint16_t data )
{
  /* Wait until the transmit buffer is empty */
  while ( SPI_I2S_GetFlagStatus( spi->port, SPI_I2S_FLAG_TXE ) == RESET )
  {
  }
  
  /* Send the byte */
  SPI_I2S_SendData( spi->port, data );
  
  /* Wait until a data is received */
  while ( SPI_I2S_GetFlagStatus( spi->port, SPI_I2S_FLAG_RXNE ) == RESET )
  {
  }
  
  /* Get the received data */
  return SPI_I2S_ReceiveData( spi->port );
}


static OSStatus calculate_prescaler( uint32_t speed, uint16_t* prescaler )
{
  uint8_t  i;
  OSStatus err = kNoErr;
  
  require_action_quiet( prescaler != NULL, exit, err = kParamErr);
  
  for( i = 0 ; i < MAX_NUM_SPI_PRESCALERS ; i++ )
  {
    if( ( 100000000 / spi_baudrate_prescalers[i].factor ) <= speed )
    {
      *prescaler = spi_baudrate_prescalers[i].prescaler_value;
      goto exit;
    }
  }
  
exit:
  return err;
}

static OSStatus spi_dma_transfer( const platform_spi_t* spi, const platform_spi_config_t* config )
{  
  /* Enable dma channels that have just been configured */
  DMA_Cmd( spi->rx_dma.stream, ENABLE );
  DMA_Cmd( spi->tx_dma.stream, ENABLE );
  
  /* Wait for DMA to complete */
  /* TODO: This should wait on a semaphore that is triggered from an IRQ */  
  while ( ( get_dma_irq_status( spi->rx_dma.stream ) & spi->rx_dma.complete_flags ) == 0  )
  {
  }

  return kNoErr;
}

static void spi_dma_config( const platform_spi_t* spi, const platform_spi_message_segment_t* message )
{
  DMA_InitTypeDef dma_init;
  static uint8_t  dummy = 0xFF;
  
  /* Setup DMA for SPI TX if it is enabled */
  DMA_DeInit( spi->tx_dma.stream );
  
  /* Setup DMA stream for TX */
  dma_init.DMA_Channel            = spi->tx_dma.channel;
  dma_init.DMA_PeripheralBaseAddr = ( uint32_t )&spi->port->DR;
  dma_init.DMA_DIR                = DMA_DIR_MemoryToPeripheral;
  dma_init.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
  dma_init.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  dma_init.DMA_BufferSize         = message->length;
  dma_init.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
  dma_init.DMA_Mode               = DMA_Mode_Normal;
  dma_init.DMA_Priority           = DMA_Priority_VeryHigh;
  dma_init.DMA_FIFOMode           = DMA_FIFOMode_Disable;
  dma_init.DMA_FIFOThreshold      = DMA_FIFOThreshold_Full;
  dma_init.DMA_MemoryBurst        = DMA_MemoryBurst_Single;
  dma_init.DMA_PeripheralBurst    = DMA_PeripheralBurst_Single;
  
  if ( message->tx_buffer != NULL )
  {
    dma_init.DMA_Memory0BaseAddr = ( uint32_t )message->tx_buffer;
    dma_init.DMA_MemoryInc       = DMA_MemoryInc_Enable;
  }
  else
  {
    dma_init.DMA_Memory0BaseAddr = ( uint32_t )(&dummy);
    dma_init.DMA_MemoryInc       = DMA_MemoryInc_Disable;
  }
  
  DMA_Init( spi->tx_dma.stream, &dma_init );
  
  /* Activate SPI DMA mode for transmission */
  
  
  /* TODO: Init TX DMA finished semaphore  */
  
  /* Setup DMA for SPI RX stream */
  DMA_DeInit( spi->rx_dma.stream );
  dma_init.DMA_Channel            = spi->rx_dma.channel;
  dma_init.DMA_PeripheralBaseAddr = ( uint32_t )&spi->port->DR;
  dma_init.DMA_DIR                = DMA_DIR_PeripheralToMemory;
  dma_init.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
  dma_init.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  dma_init.DMA_BufferSize         = message->length;
  dma_init.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
  dma_init.DMA_Mode               = DMA_Mode_Normal;
  dma_init.DMA_Priority           = DMA_Priority_VeryHigh;
  dma_init.DMA_FIFOMode           = DMA_FIFOMode_Disable;
  dma_init.DMA_FIFOThreshold      = DMA_FIFOThreshold_Full;
  dma_init.DMA_MemoryBurst        = DMA_MemoryBurst_Single;
  dma_init.DMA_PeripheralBurst    = DMA_PeripheralBurst_Single;
  if ( message->rx_buffer != NULL )
  {
    dma_init.DMA_Memory0BaseAddr = (uint32_t)message->rx_buffer;
    dma_init.DMA_MemoryInc = DMA_MemoryInc_Enable;
  }
  else
  {
    dma_init.DMA_Memory0BaseAddr = (uint32_t)&dummy;
    dma_init.DMA_MemoryInc = DMA_MemoryInc_Disable;
  }
  
  /* Init and activate RX DMA channel */
  DMA_Init( spi->rx_dma.stream, &dma_init );
  
  
  /* TODO: Init RX DMA finish semaphore */
}










#include "mico.h"
#include "misc.h"
#include "string.h" /* For memcpy */
#include "platform_config.h"
#include "platform_peripheral.h"
#include "platform_logging.h"
#include "wlan_platform_common.h"

/******************************************************
 *             Constants
 ******************************************************/

#define DMA_TIMEOUT_LOOPS      (10000000)

/**
 * Transfer direction for the mico platform bus interface
 */
typedef enum
{
    /* If updating this enum, the bus_direction_mapping variable will also need to be updated */
    BUS_READ,
    BUS_WRITE
} bus_transfer_direction_t;

/******************************************************
 *             Structures
 ******************************************************/

/******************************************************
 *             Variables
 ******************************************************/
static mico_mutex_t Spi_mutex = NULL;
static bool spi_initialized = false;

static mico_semaphore_t spi_transfer_finished_semaphore = NULL;

/******************************************************
 *             Function declarations
 ******************************************************/
extern void wlan_notify_irq( void );

static OSStatus wlan_spi_init( void );
static OSStatus wlan_spi_deinit( void );
static OSStatus wlan_spi_transfer( const mico_spi_message_segment_t segment );

/******************************************************
 *             Function definitions
 ******************************************************/
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


static void spi_irq_handler( void* arg )
{
    UNUSED_PARAMETER(arg);
#ifndef MICO_DISABLE_MCU_POWERSAVE
    platform_mcu_powersave_exit_notify( );
#endif /* ifndef MICO_DISABLE_MCU_POWERSAVE */
    wlan_notify_irq( );
}

void platform_wifi_spi_rx_dma_irq( void )
{
    /* Clear interrupt */
    clear_dma_interrupts( wifi_spi.rx_dma.stream, wifi_spi.rx_dma.complete_flags );

    mico_rtos_set_semaphore( &spi_transfer_finished_semaphore );
}

/* This function is called by wlan driver */
OSStatus host_platform_bus_init( void )
{
    /* Setup the interrupt input for WLAN_IRQ */
    platform_gpio_init( &wifi_spi_pins[WIFI_PIN_SPI_IRQ], INPUT_HIGH_IMPEDANCE );
    platform_gpio_irq_enable( &wifi_spi_pins[WIFI_PIN_SPI_IRQ], IRQ_TRIGGER_RISING_EDGE, spi_irq_handler, 0 );

    return platform_wlan_spi_init( &wifi_spi_pins[WIFI_PIN_SPI_CS] );
}

/* This function is called by wlan driver */
OSStatus host_platform_bus_deinit( void )
{
    return platform_wlan_spi_deinit( &wifi_spi_pins[WIFI_PIN_SPI_CS] );
}

OSStatus host_platform_spi_transfer( bus_transfer_direction_t dir, uint8_t* buffer, uint16_t buffer_length )
{
    platform_spi_message_segment_t segment = { buffer, NULL, buffer_length };

    if( dir == BUS_READ)
        segment.rx_buffer = buffer;

    return platform_wlan_spi_transfer( &wifi_spi_pins[WIFI_PIN_SPI_CS], &segment, 1 );
}

OSStatus wlan_spi_init()
{
    SPI_InitTypeDef  spi_init;
    DMA_InitTypeDef  dma_init_structure;
    uint32_t         a;
    
    if( spi_initialized == true )
      return kNoErr;
    
    spi_initialized = true;
    
    platform_mcu_powersave_disable();

    if( spi_transfer_finished_semaphore == NULL )
    {
      mico_rtos_init_semaphore( &spi_transfer_finished_semaphore, 1 );
    }

    /* Enable SPI_SLAVE DMA clock */
    if ( wifi_spi.tx_dma.controller == DMA1 )
    {
        RCC->AHB1ENR |= RCC_AHB1Periph_DMA1;
    }
    else
    {
        RCC->AHB1ENR |= RCC_AHB1Periph_DMA2;
    }


    if ( wifi_spi.rx_dma.controller == DMA1 )
    {
        RCC->AHB1ENR |= RCC_AHB1Periph_DMA1;
    }
    else
    {
        RCC->AHB1ENR |= RCC_AHB1Periph_DMA2;
    }
    
    
    /* Enable SPI_SLAVE Periph clock */
    (wifi_spi.peripheral_clock_func)( wifi_spi.peripheral_clock_reg, ENABLE );

    /* Enable SYSCFG. Needed for selecting EXTI interrupt line */
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_SYSCFG, ENABLE );

    /* Setup the SPI lines */
    for ( a = WIFI_PIN_SPI_CLK; a < WIFI_PIN_SPI_MAX; a++ )
    {
        platform_gpio_set_alternate_function( wifi_spi_pins[ a ].port, wifi_spi_pins[ a ].pin_number, GPIO_OType_PP, GPIO_PuPd_NOPULL, wifi_spi.gpio_af );
    }

#ifdef MICO_WIFI_USE_GPIO_FOR_BOOTSTRAP_0
    /* Set GPIO_B[1:0] to 01 to put WLAN module into gSPI mode */
    platform_gpio_init( &wifi_control_pins[WIFI_PIN_BOOTSTRAP_0], OUTPUT_PUSH_PULL );
    platform_gpio_output_high( &wifi_control_pins[WIFI_PIN_BOOTSTRAP_0] );
#endif
#ifdef MICO_WIFI_USE_GPIO_FOR_BOOTSTRAP_1
    platform_gpio_init( &wifi_control_pins[WIFI_PIN_BOOTSTRAP_1], OUTPUT_PUSH_PULL );
    platform_gpio_output_low( &wifi_control_pins[WIFI_PIN_BOOTSTRAP_1] );
#endif
    
    /* Setup DMA for SPIX RX */
    DMA_DeInit( wifi_spi.rx_dma.stream );
    dma_init_structure.DMA_Channel = wifi_spi.rx_dma.channel;
    dma_init_structure.DMA_PeripheralBaseAddr = (uint32_t) &(wifi_spi.port->DR);
    dma_init_structure.DMA_Memory0BaseAddr = 0;
    dma_init_structure.DMA_DIR = DMA_DIR_PeripheralToMemory;
    dma_init_structure.DMA_BufferSize = 0;
    dma_init_structure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    dma_init_structure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    dma_init_structure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    dma_init_structure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    dma_init_structure.DMA_Mode = DMA_Mode_Normal;
    dma_init_structure.DMA_Priority = DMA_Priority_VeryHigh;
    dma_init_structure.DMA_FIFOMode = DMA_FIFOMode_Disable;
    dma_init_structure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
    dma_init_structure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    dma_init_structure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_Init( wifi_spi.rx_dma.stream, &dma_init_structure );

    /* Setup DMA for SPIX TX */
    DMA_DeInit( wifi_spi.tx_dma.stream );
    dma_init_structure.DMA_Channel =  wifi_spi.tx_dma.channel;
    dma_init_structure.DMA_PeripheralBaseAddr = (uint32_t) &(wifi_spi.port->DR);
    dma_init_structure.DMA_Memory0BaseAddr = 0;
    dma_init_structure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
    dma_init_structure.DMA_BufferSize = 0;
    dma_init_structure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    dma_init_structure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    dma_init_structure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    dma_init_structure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    dma_init_structure.DMA_Mode = DMA_Mode_Normal;
    dma_init_structure.DMA_Priority = DMA_Priority_VeryHigh;
    dma_init_structure.DMA_FIFOMode = DMA_FIFOMode_Disable;
    dma_init_structure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
    dma_init_structure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    dma_init_structure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_Init( wifi_spi.tx_dma.stream, &dma_init_structure );
    
    /* Must be lower priority than the value of configMAX_SYSCALL_INTERRUPT_PRIORITY */
    /* otherwise FreeRTOS will not be able to mask the interrupt */
    /* keep in mind that ARMCM3 interrupt priority logic is inverted, the highest value */
    /* is the lowest priority */
    NVIC_EnableIRQ( wifi_spi.rx_dma.irq_vector );

    /* Enable DMA for TX */
    SPI_I2S_DMACmd( wifi_spi.port, SPI_I2S_DMAReq_Tx | SPI_I2S_DMAReq_Rx, ENABLE );

    /* Setup SPI */
    spi_init.SPI_Direction         = SPI_Direction_2Lines_FullDuplex;
    spi_init.SPI_Mode              = SPI_Mode_Master;
    spi_init.SPI_DataSize          = SPI_DataSize_8b;
    spi_init.SPI_CPOL              = SPI_CPOL_High;
    spi_init.SPI_CPHA              = SPI_CPHA_2Edge;
    spi_init.SPI_NSS               = SPI_NSS_Soft;
    spi_init.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
    spi_init.SPI_FirstBit          = SPI_FirstBit_MSB;
    spi_init.SPI_CRCPolynomial     = (uint16_t) 7;

    /* Init SPI and enable it */
    SPI_Init( wifi_spi.port, &spi_init );
    SPI_Cmd( wifi_spi.port, ENABLE );
       
    platform_mcu_powersave_enable();
    
    return kNoErr;
}

OSStatus wlan_spi_deinit()
{
    uint32_t         a;
    
    platform_mcu_powersave_disable();

    /* Disable SPI and SPI DMA */
    SPI_Cmd( wifi_spi.port, DISABLE );
    SPI_I2S_DeInit( wifi_spi.port );
    SPI_I2S_DMACmd( wifi_spi.port, SPI_I2S_DMAReq_Tx | SPI_I2S_DMAReq_Rx, DISABLE );
    DMA_DeInit( wifi_spi.tx_dma.stream );
    DMA_DeInit( wifi_spi.rx_dma.stream );

#if defined ( MICO_WIFI_USE_GPIO_FOR_BOOTSTRAP )
    /* Clear GPIO_B[1:0] */
    platform_gpio_init( &wifi_control_pins[WIFI_PIN_BOOTSTRAP_0], INPUT_HIGH_IMPEDANCE );
    platform_gpio_init( &wifi_control_pins[WIFI_PIN_BOOTSTRAP_1], INPUT_HIGH_IMPEDANCE );
#endif

    /* Clear the SPI lines */
    for ( a = WIFI_PIN_SPI_CLK; a < WIFI_PIN_SPI_MAX; a++ )
    {
        platform_gpio_init( &wifi_spi_pins[ a ], INPUT_HIGH_IMPEDANCE );
    }

    platform_gpio_irq_disable( &wifi_spi_pins[WIFI_PIN_SPI_IRQ] );
    platform_gpio_init( &wifi_spi_pins[WIFI_PIN_SPI_IRQ], INPUT_HIGH_IMPEDANCE );

    /* Disable SPI_SLAVE Periph clock and DMA1 clock */
    (wifi_spi.peripheral_clock_func)( wifi_spi.peripheral_clock_reg, DISABLE );
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_SYSCFG, DISABLE );

    platform_mcu_powersave_enable();

    return kNoErr;
}

OSStatus wlan_spi_transfer( const mico_spi_message_segment_t segment )
{
    OSStatus result;
    static uint8_t  dummy = 0xFF;
  
    platform_mcu_powersave_disable();

    wifi_spi.rx_dma.stream->CR  |= ( 1 << 4);
    wifi_spi.tx_dma.stream->NDTR = segment.length;
    wifi_spi.rx_dma.stream->NDTR = segment.length;

    if( segment.tx_buffer != NULL )
    {
        wifi_spi.tx_dma.stream->M0AR = (uint32_t) segment.tx_buffer;
        wifi_spi.tx_dma.stream->CR  |= DMA_MemoryInc_Enable;
    }
    else
    {
        wifi_spi.tx_dma.stream->M0AR = (uint32_t) &dummy;
        wifi_spi.tx_dma.stream->CR &= ~DMA_MemoryInc_Enable;
    }

    if( segment.rx_buffer != NULL )
    {
        wifi_spi.rx_dma.stream->M0AR = (uint32_t) segment.rx_buffer;
        wifi_spi.rx_dma.stream->CR  |= DMA_MemoryInc_Enable;
    }
    else
    {
        wifi_spi.rx_dma.stream->M0AR = (uint32_t) &dummy;
        wifi_spi.rx_dma.stream->CR &= ~DMA_MemoryInc_Enable;
    }
    
    DMA_Cmd( wifi_spi.rx_dma.stream, ENABLE );
    DMA_Cmd( wifi_spi.tx_dma.stream, ENABLE );
    
    /* Wait for DMA to complete */
    result = mico_rtos_get_semaphore( &spi_transfer_finished_semaphore, 100 );

    DMA_Cmd( wifi_spi.rx_dma.stream, DISABLE );
    DMA_Cmd( wifi_spi.tx_dma.stream, DISABLE );
    
    clear_dma_interrupts( wifi_spi.rx_dma.stream, wifi_spi.rx_dma.complete_flags );
    clear_dma_interrupts( wifi_spi.tx_dma.stream, wifi_spi.tx_dma.complete_flags );
    
    platform_mcu_powersave_enable();

    return result;
}



/* This function is called by other spi devices use the same port as wlan spi bus*/
OSStatus platform_wlan_spi_init( const platform_gpio_t* chip_select )
{
    OSStatus err = kNoErr;

    if( Spi_mutex == NULL)
    {
        mico_rtos_init_mutex( &Spi_mutex );
    }
    require( Spi_mutex, exit );

    mico_rtos_lock_mutex( &Spi_mutex );
    platform_gpio_init( chip_select, OUTPUT_PUSH_PULL );
    platform_gpio_output_high( chip_select );
    err = wlan_spi_init();
    mico_rtos_unlock_mutex( &Spi_mutex ); 
exit:
    if( err != kNoErr )
    {
        if( Spi_mutex )
        {
            mico_rtos_deinit_mutex( &Spi_mutex );
            Spi_mutex = NULL;
        }
    }
    return err;
}

/* This function is called by other spi devices use the same port as wlan spi bus*/
OSStatus platform_wlan_spi_deinit( const platform_gpio_t* chip_select )
{
    OSStatus err = kNoErr;

    require_quiet( Spi_mutex, exit );

    mico_rtos_lock_mutex( &Spi_mutex );
    platform_gpio_init( chip_select, INPUT_HIGH_IMPEDANCE );
    err = wlan_spi_deinit();
    mico_rtos_unlock_mutex( &Spi_mutex );

    mico_rtos_deinit_mutex( &Spi_mutex );
    Spi_mutex = NULL;
exit:
    return err;
}

/* This function is called by other spi devices use the same port as wlan spi bus*/
OSStatus platform_wlan_spi_transfer( const platform_gpio_t* chip_select, const platform_spi_message_segment_t* segments, uint16_t number_of_segments )
{
    OSStatus err = kNoErr;
    uint8_t i;

    if( Spi_mutex == NULL)
    {
        err = platform_wlan_spi_init( chip_select );
        require_noerr(err, exit);
    }

    mico_rtos_lock_mutex( &Spi_mutex );

    platform_gpio_output_low( chip_select );

    for( i = 0 ;i < number_of_segments ; i++ )
    {
        if( segments[i].length != 0 )
        {
          wlan_spi_transfer( segments[i] );
        }
    }

    platform_gpio_output_high( chip_select );

    mico_rtos_unlock_mutex( &Spi_mutex );
exit:
    return err;
}





#include "misc.h"
#include "string.h" /* For memcpy */
#include "platform.h"
#include "platform_config.h"
#include "platform_peripheral.h"
#include "platform_logging.h"
#include "wlan_platform_common.h"

/******************************************************
 *             Constants
 ******************************************************/

#define COMMAND_FINISHED_CMD52_TIMEOUT_LOOPS (100000)
#define COMMAND_FINISHED_CMD53_TIMEOUT_LOOPS (100000)
#define SDIO_TX_RX_COMPLETE_TIMEOUT_LOOPS    (100000)
#define SDIO_DMA_TIMEOUT_LOOPS               (1000000)
#define MAX_TIMEOUTS                         (30)
#define SDIO_ERROR_MASK                      ( SDIO_STA_CCRCFAIL | SDIO_STA_DCRCFAIL | SDIO_STA_CTIMEOUT | SDIO_STA_DTIMEOUT | SDIO_STA_TXUNDERR | SDIO_STA_RXOVERR | SDIO_STA_STBITERR )
#define SDIO_IRQ_CHANNEL                     ((IRQn_Type)0x31)
#define DMA2_3_IRQ_CHANNEL                   ((IRQn_Type)DMA2_Stream3_IRQn)
#define BUS_LEVEL_MAX_RETRIES                (5)
#define SDIO_ENUMERATION_TIMEOUT_MS          (500)

/******************************************************
 *             Structures
 ******************************************************/

typedef struct
{
    /*@shared@*/ /*@null@*/ uint8_t* data;
    uint16_t length;
} sdio_dma_segment_t;

/******************************************************
 *             Enumerations
 ******************************************************/
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
 *               Variables Definitions
 ******************************************************/
static const uint32_t bus_direction_mapping[] =
{
    [BUS_READ]  = SDIO_TransferDir_ToSDIO,
    [BUS_WRITE] = SDIO_TransferDir_ToCard
};

/******************************************************
 *                   Enumerations
 ******************************************************/

/*
 * SDIO specific constants
 */
typedef enum
{
    SDIO_CMD_0  =  0,
    SDIO_CMD_3  =  3,
    SDIO_CMD_5  =  5,
    SDIO_CMD_7  =  7,
    SDIO_CMD_52 = 52,
    SDIO_CMD_53 = 53,
    __MAX_VAL   = 64
} sdio_command_t;

typedef enum
{
    SDIO_BLOCK_MODE = ( 0 << 2 ), /* These are STM32 implementation specific */
    SDIO_BYTE_MODE  = ( 1 << 2 )  /* These are STM32 implementation specific */
} sdio_transfer_mode_t;

typedef enum
{
    SDIO_1B_BLOCK    =  1,
    SDIO_2B_BLOCK    =  2,
    SDIO_4B_BLOCK    =  4,
    SDIO_8B_BLOCK    =  8,
    SDIO_16B_BLOCK   =  16,
    SDIO_32B_BLOCK   =  32,
    SDIO_64B_BLOCK   =  64,
    SDIO_128B_BLOCK  =  128,
    SDIO_256B_BLOCK  =  256,
    SDIO_512B_BLOCK  =  512,
    SDIO_1024B_BLOCK = 1024,
    SDIO_2048B_BLOCK = 2048
} sdio_block_size_t;

typedef enum
{
    RESPONSE_NEEDED,
    NO_RESPONSE
} sdio_response_needed_t;


/******************************************************
 *             Variables
 ******************************************************/

static uint8_t                      temp_dma_buffer[2*1024];
static uint8_t*                     user_data;
static uint32_t                     user_data_size;
static uint8_t*                     dma_data_source;
static uint32_t                     dma_transfer_size;
static mico_semaphore_t             sdio_transfer_finished_semaphore;
static bool                         sdio_transfer_failed;
static bus_transfer_direction_t     current_transfer_direction;
static uint32_t                     current_command;
/******************************************************
 *             Function declarations
 ******************************************************/

static uint32_t          sdio_get_blocksize_dctrl   ( sdio_block_size_t block_size );
static sdio_block_size_t find_optimal_block_size    ( uint32_t data_size );
static void              sdio_prepare_data_transfer ( bus_transfer_direction_t direction, sdio_block_size_t block_size, /*@unique@*/ uint8_t* data, uint16_t data_size ) /*@modifies dma_data_source, user_data, user_data_size, dma_transfer_size@*/;

void dma_irq ( void );
OSStatus host_platform_sdio_transfer( bus_transfer_direction_t direction, sdio_command_t command, sdio_transfer_mode_t mode, sdio_block_size_t block_size, uint32_t argument, /*@null@*/ uint32_t* data, uint16_t data_size, sdio_response_needed_t response_expected, /*@out@*/ /*@null@*/ uint32_t* response );
extern void wlan_notify_irq( void );

/******************************************************
 *             Function definitions
 ******************************************************/

#if !(defined (MICO_DISABLE_MCU_POWERSAVE)) && !(defined (SDIO_1_BIT)) //SDIO 4 Bit mode and enable MCU powersave, need an OOB interrupt
static void sdio_oob_irq_handler( void* arg )
{
    UNUSED_PARAMETER(arg);
    wlan_notify_irq( );
}

OSStatus host_enable_oob_interrupt( void )
{
    platform_gpio_init( &wifi_sdio_pins[WIFI_PIN_SDIO_OOB_IRQ], INPUT_HIGH_IMPEDANCE );
    platform_gpio_irq_enable( &wifi_sdio_pins[WIFI_PIN_SDIO_OOB_IRQ], IRQ_TRIGGER_RISING_EDGE, sdio_oob_irq_handler, 0 );
    return kNoErr;
}

uint8_t host_platform_get_oob_interrupt_pin( void )
{
    return MICO_WIFI_OOB_IRQ_GPIO_PIN;
}

#elif defined (MICO_DISABLE_MCU_POWERSAVE) && !(defined (SDIO_1_BIT)) //SDIO 4 Bit mode and disable MCU powersave, do not need OOB interrupt 

OSStatus host_enable_oob_interrupt( void )
{
    return kNoErr;
}

uint8_t host_platform_get_oob_interrupt_pin( void )
{
    return 0;
}
#endif

#ifdef SDIO_1_BIT
static void sdio_int_pin_irq_handler( void* arg ) //SDIO 1 Bit mode
{
    UNUSED_PARAMETER(arg);
    wlan_notify_irq( );
}

bool host_platform_is_sdio_int_asserted(void)
{
    if ( platform_gpio_input_get( &wifi_sdio_pins[WIFI_PIN_SDIO_IRQ] ) == true) //SDIO INT pin is high
        return false;
    else
        return true; // SDIO D1 is low, data need read
}

OSStatus host_enable_oob_interrupt( void )
{
    return kNoErr;
}

#endif

static void sdio_enable_bus_irq( void )
{
    SDIO->MASK = SDIO_MASK_SDIOITIE | SDIO_MASK_CMDRENDIE | SDIO_MASK_CMDSENTIE;
}

static void sdio_disable_bus_irq( void )
{
    SDIO->MASK = 0;
}


void sdio_irq( void )
{
    uint32_t intstatus = SDIO->STA;

    if ( ( intstatus & ( SDIO_STA_CCRCFAIL | SDIO_STA_DCRCFAIL | SDIO_STA_TXUNDERR | SDIO_STA_RXOVERR  | SDIO_STA_STBITERR )) != 0 )
    {
        sdio_transfer_failed = true;
        SDIO->ICR = (uint32_t) 0xffffffff;
        mico_rtos_set_semaphore( &sdio_transfer_finished_semaphore );
    }
    else
    {
        if ((intstatus & (SDIO_STA_CMDREND | SDIO_STA_CMDSENT)) != 0)
        {
            if ( ( SDIO->RESP1 & 0x800 ) != 0 )
            {
                sdio_transfer_failed = true;
                mico_rtos_set_semaphore( &sdio_transfer_finished_semaphore );
            }
            else if (current_command == SDIO_CMD_53)
            {
                if (current_transfer_direction == BUS_WRITE)
                {
                    DMA2_Stream3->CR = DMA_DIR_MemoryToPeripheral |
                                       DMA_Channel_4 | DMA_PeripheralInc_Disable | DMA_MemoryInc_Enable |
                                       DMA_PeripheralDataSize_Word | DMA_MemoryDataSize_Word |
                                       DMA_Mode_Normal | DMA_Priority_VeryHigh |
                                       DMA_MemoryBurst_INC4 | DMA_PeripheralBurst_INC4 | DMA_SxCR_PFCTRL | DMA_SxCR_EN | DMA_SxCR_TCIE;
                }
                else
                {
                    DMA2_Stream3->CR = DMA_DIR_PeripheralToMemory |
                                       DMA_Channel_4 | DMA_PeripheralInc_Disable | DMA_MemoryInc_Enable |
                                       DMA_PeripheralDataSize_Word | DMA_MemoryDataSize_Word |
                                       DMA_Mode_Normal | DMA_Priority_VeryHigh |
                                       DMA_MemoryBurst_INC4 | DMA_PeripheralBurst_INC4 | DMA_SxCR_PFCTRL | DMA_SxCR_EN | DMA_SxCR_TCIE;
                }
            }

            /* Clear all command/response interrupts */
            SDIO->ICR = (SDIO_STA_CMDREND | SDIO_STA_CMDSENT);
        }
    }

#ifndef SDIO_1_BIT
    /* Check whether the external interrupt was triggered */
    if ( ( intstatus & SDIO_STA_SDIOIT ) != 0 )
    {
        /* Clear the interrupt and then inform MiCO thread */
#if defined (STM32F412xG)
        SDIO->MASK &= ~SDIO_MASK_SDIOITIE;
#endif
        SDIO->ICR = SDIO_ICR_SDIOITC;
        wlan_notify_irq( );
    }
#endif
}

/* Function picked up by linker script */
void dma_irq( void )
{
    OSStatus result;

    /* Clear interrupt */
    DMA2->LIFCR = (uint32_t) (0x3F << 22);

    result = mico_rtos_set_semaphore( &sdio_transfer_finished_semaphore );

    /* check result if in debug mode */
    check_string(result == kNoErr, "failed to set dma semaphore" );

    (void) result; /* ignore result if in release mode */
}

OSStatus host_platform_bus_init( void )
{
    SDIO_InitTypeDef sdio_init_structure;
    OSStatus         result;
    uint8_t          a;

    platform_mcu_powersave_disable();

    result = mico_rtos_init_semaphore( &sdio_transfer_finished_semaphore, 1 );
    if ( result != kNoErr )
    {
        return result;
    }

    /* Turn on SDIO IRQ */
    SDIO->ICR = (uint32_t) 0xffffffff;

    /* Must be lower priority than the value of configMAX_SYSCALL_INTERRUPT_PRIORITY */
    /* otherwise FreeRTOS will not be able to mask the interrupt */
    /* keep in mind that ARMCM3 interrupt priority logic is inverted, the highest value */
    /* is the lowest priority */
    NVIC_EnableIRQ( SDIO_IRQ_CHANNEL );
    NVIC_EnableIRQ( DMA2_3_IRQ_CHANNEL );
    
#ifdef MICO_WIFI_USE_GPIO_FOR_BOOTSTRAP_0
    /* Set GPIO_B[1:0] to 00 to put WLAN module into SDIO mode */
    platform_gpio_init( &wifi_control_pins[WIFI_PIN_BOOTSTRAP_0], OUTPUT_PUSH_PULL );
    platform_gpio_output_low( &wifi_control_pins[WIFI_PIN_BOOTSTRAP_0] );
#endif //MICO_WIFI_USE_GPIO_FOR_BOOTSTRAP_0

#ifdef MICO_WIFI_USE_GPIO_FOR_BOOTSTRAP_1
    platform_gpio_init( &wifi_control_pins[WIFI_PIN_BOOTSTRAP_1], OUTPUT_PUSH_PULL );
#ifdef MICO_WIFI_USE_GPIO_FOR_BOOTSTRAP_1_HIGH_FOR_SDIO
    platform_gpio_output_high( &wifi_control_pins[WIFI_PIN_BOOTSTRAP_1] );
#else
    platform_gpio_output_low( &wifi_control_pins[WIFI_PIN_BOOTSTRAP_1] );
#endif //MICO_WIFI_USE_GPIO_FOR_BOOTSTRAP_1_HIGH_FOR_SDIO
#endif  //MICO_WIFI_USE_GPIO_FOR_BOOTSTRAP_1

    /* Setup GPIO pins for SDIO data & clock */
    for ( a = WIFI_PIN_SDIO_CLK; a < WIFI_PIN_SDIO_MAX; a++ )
    {
        platform_gpio_set_alternate_function( wifi_sdio_pins[ a ].port, wifi_sdio_pins[ a ].pin_number, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_AF_SDIO );
    }

#ifdef SDIO_1_BIT
    platform_gpio_init( &wifi_sdio_pins[WIFI_PIN_SDIO_IRQ], INPUT_PULL_UP );
    platform_gpio_irq_enable( &wifi_sdio_pins[WIFI_PIN_SDIO_IRQ], IRQ_TRIGGER_FALLING_EDGE, sdio_int_pin_irq_handler, 0 );
#endif

    /*!< Enable the SDIO AHB Clock and the DMA2 Clock */
    RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_DMA2, ENABLE );
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_SDIO, ENABLE );

    SDIO_DeInit( );
    sdio_init_structure.SDIO_ClockDiv            = (uint8_t) 120; /* 0x78, clock is taken from the high speed APB bus ; */ /* About 400KHz */
    sdio_init_structure.SDIO_ClockEdge           = SDIO_ClockEdge_Rising;
    sdio_init_structure.SDIO_ClockBypass         = SDIO_ClockBypass_Disable;
    sdio_init_structure.SDIO_ClockPowerSave      = SDIO_ClockPowerSave_Enable;
    sdio_init_structure.SDIO_BusWide             = SDIO_BusWide_1b;
    sdio_init_structure.SDIO_HardwareFlowControl = SDIO_HardwareFlowControl_Disable;
    SDIO_Init( &sdio_init_structure );
    SDIO_SetPowerState( SDIO_PowerState_ON );
    SDIO_SetSDIOReadWaitMode( SDIO_ReadWaitMode_CLK );
    SDIO_ClockCmd( ENABLE );

    platform_mcu_powersave_enable();

    return kNoErr;
}

OSStatus host_platform_sdio_enumerate( void )
{
    OSStatus       result;
    uint32_t       loop_count;
    uint32_t       data = 0;

    loop_count = 0;
    do
    {
        /* Send CMD0 to set it to idle state */
        host_platform_sdio_transfer( BUS_WRITE, SDIO_CMD_0, SDIO_BYTE_MODE, SDIO_1B_BLOCK, 0, 0, 0, NO_RESPONSE, NULL );

        /* CMD5. */
        host_platform_sdio_transfer( BUS_READ, SDIO_CMD_5, SDIO_BYTE_MODE, SDIO_1B_BLOCK, 0, 0, 0, NO_RESPONSE, NULL );

        /* Send CMD3 to get RCA. */
        result = host_platform_sdio_transfer( BUS_READ, SDIO_CMD_3, SDIO_BYTE_MODE, SDIO_1B_BLOCK, 0, 0, 0, RESPONSE_NEEDED, &data );
        loop_count++;
        if ( loop_count >= (uint32_t) SDIO_ENUMERATION_TIMEOUT_MS )
        {
            return kTimeoutErr;
        }
    } while ( ( result != kNoErr ) && ( mico_rtos_delay_milliseconds( (uint32_t) 1 ), ( 1 == 1 ) ) );
    /* If you're stuck here, check the platform matches your hardware */

    /* Send CMD7 with the returned RCA to select the card */
    host_platform_sdio_transfer( BUS_WRITE, SDIO_CMD_7, SDIO_BYTE_MODE, SDIO_1B_BLOCK, data, 0, 0, RESPONSE_NEEDED, NULL );

    return kNoErr;
}

OSStatus host_platform_bus_deinit( void )
{
    OSStatus result;
    uint32_t     a;

    result = mico_rtos_deinit_semaphore( &sdio_transfer_finished_semaphore );

    platform_mcu_powersave_disable();

    /* Disable SPI and SPI DMA */
    sdio_disable_bus_irq( );
    SDIO_ClockCmd( DISABLE );
    SDIO_SetPowerState( SDIO_PowerState_OFF );
    SDIO_DeInit( );
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_SDIO, DISABLE );

#ifdef SDIO_1_BIT
    platform_gpio_deinit( &wifi_sdio_pins[WIFI_PIN_SDIO_IRQ] );
    platform_gpio_irq_disable( &wifi_sdio_pins[WIFI_PIN_SDIO_IRQ] );
#endif

    for ( a = 0; a < WIFI_PIN_SDIO_MAX; a++ )
    {
        platform_gpio_deinit( &wifi_sdio_pins[ a ] );
    }

#ifdef MICO_WIFI_USE_GPIO_FOR_BOOTSTRAP_0
    platform_gpio_deinit( &wifi_control_pins[WIFI_PIN_BOOTSTRAP_0] );
#endif
#ifdef MICO_WIFI_USE_GPIO_FOR_BOOTSTRAP_1
    platform_gpio_deinit( &wifi_control_pins[WIFI_PIN_BOOTSTRAP_1] );
#endif

    /* Turn off SDIO IRQ */
    NVIC_DisableIRQ( SDIO_IRQ_CHANNEL );
    NVIC_DisableIRQ( DMA2_3_IRQ_CHANNEL );

    platform_mcu_powersave_enable();

    return result;
}

OSStatus host_platform_sdio_transfer( bus_transfer_direction_t direction, sdio_command_t command, sdio_transfer_mode_t mode, sdio_block_size_t block_size, uint32_t argument, /*@null@*/ uint32_t* data, uint16_t data_size, sdio_response_needed_t response_expected, /*@out@*/ /*@null@*/ uint32_t* response )
{
    uint32_t loop_count = 0;
    OSStatus result;
    uint16_t attempts = 0;

    check_string(!((command == SDIO_CMD_53) && (data == NULL)), "Bad args" );

    if ( response != NULL )
    {
        *response = 0;
    }

    platform_mcu_powersave_disable();

    /* Ensure the bus isn't stuck half way through transfer */
    DMA2_Stream3->CR   = 0;

restart:
    SDIO->ICR = (uint32_t) 0xFFFFFFFF;
    sdio_transfer_failed = false;
    ++attempts;

    /* Check if we've tried too many times */
    if (attempts >= (uint16_t) BUS_LEVEL_MAX_RETRIES)
    {
        result = kGeneralErr;
        goto exit;
    }

    /* Prepare the data transfer register */
    current_command = command;
    if ( command == SDIO_CMD_53 )
    {
        sdio_enable_bus_irq();

        /* Dodgy STM32 hack to set the CMD53 byte mode size to be the same as the block size */
        if ( mode == SDIO_BYTE_MODE )
        {
            block_size = find_optimal_block_size( data_size );
            if ( block_size < SDIO_512B_BLOCK )
            {
                argument = ( argument & (uint32_t) ( ~0x1FF ) ) | block_size;
            }
            else
            {
                argument = ( argument & (uint32_t) ( ~0x1FF ) );
            }
        }

        /* Prepare the SDIO for a data transfer */
        current_transfer_direction = direction;
        sdio_prepare_data_transfer( direction, block_size, (uint8_t*) data, data_size );

        /* Send the command */
        SDIO->ARG = argument;
        SDIO->CMD = (uint32_t) ( command | SDIO_Response_Short | SDIO_Wait_No | SDIO_CPSM_Enable );

        /* Wait for the whole transfer to complete */
        result = mico_rtos_get_semaphore( &sdio_transfer_finished_semaphore, (uint32_t) 50 );
        if ( result != kNoErr )
        {
            goto exit;
        }

        if ( sdio_transfer_failed == true )
        {
            goto restart;
        }

        /* Check if there were any SDIO errors */
        require(( SDIO->STA & ( SDIO_STA_DTIMEOUT | SDIO_STA_CTIMEOUT ) ) == 0, restart);
        require_string(( SDIO->STA & ( SDIO_STA_CCRCFAIL | SDIO_STA_DCRCFAIL | SDIO_STA_TXUNDERR | SDIO_STA_RXOVERR ) ) == 0, restart, "SDIO communication failure");

        /* Wait till complete */
        loop_count = (uint32_t) SDIO_TX_RX_COMPLETE_TIMEOUT_LOOPS;
        do
        {
            loop_count--;
            if ( loop_count == 0 || ( ( SDIO->STA & SDIO_ERROR_MASK ) != 0 ) )
            {
                goto restart;
            }
        } while ( ( SDIO->STA & ( SDIO_STA_TXACT | SDIO_STA_RXACT ) ) != 0 );

        if ( direction == BUS_READ )
        {
            memcpy( user_data, dma_data_source, (size_t) user_data_size );
        }
    }
    else
    {
        uint32_t temp_sta;

        /* Send the command */
        SDIO->ARG = argument;
        SDIO->CMD = (uint32_t) ( command | SDIO_Response_Short | SDIO_Wait_No | SDIO_CPSM_Enable );

        loop_count = (uint32_t) COMMAND_FINISHED_CMD52_TIMEOUT_LOOPS;
        do
        {
            temp_sta = SDIO->STA;
            loop_count--;
            if ( loop_count == 0 || ( ( response_expected == RESPONSE_NEEDED ) && ( ( temp_sta & SDIO_ERROR_MASK ) != 0 ) ) )
            {
                goto restart;
            }
        } while ( ( temp_sta & SDIO_FLAG_CMDACT ) != 0 );
    }

    if ( response != NULL )
    {
        *response = SDIO->RESP1;
    }
    result = kNoErr;

exit:
    platform_mcu_powersave_enable();
    return result;
}


static void sdio_prepare_data_transfer( bus_transfer_direction_t direction, sdio_block_size_t block_size, /*@unique@*/ uint8_t* data, uint16_t data_size ) /*@modifies dma_data_source, user_data, user_data_size, dma_transfer_size@*/
{
    /* Setup a single transfer using the temp buffer */
    user_data         = data;
    user_data_size    = data_size;
    dma_transfer_size = (uint32_t) ( ( ( data_size + (uint16_t) block_size - 1 ) / (uint16_t) block_size ) * (uint16_t) block_size );

    if ( direction == BUS_WRITE )
    {
        dma_data_source = data;
    }
    else
    {
        dma_data_source = temp_dma_buffer;
    }

    SDIO->DTIMER = (uint32_t) 0xFFFFFFFF;
    SDIO->DLEN   = dma_transfer_size;
    SDIO->DCTRL  = (uint32_t)sdio_get_blocksize_dctrl(block_size) | bus_direction_mapping[(int)direction] | SDIO_TransferMode_Block | SDIO_DPSM_Enable | (1 << 3) | (1 << 11);

    /* DMA2 Stream3 */
    DMA2_Stream3->CR   = 0;
    DMA2->LIFCR        = (uint32_t) ( 0x3F << 22 );
    DMA2_Stream3->FCR  = (uint32_t) ( 0x00000021 | DMA_FIFOMode_Enable | DMA_FIFOThreshold_Full );
    DMA2_Stream3->PAR  = (uint32_t) &SDIO->FIFO;
    DMA2_Stream3->M0AR = (uint32_t) dma_data_source;
    DMA2_Stream3->NDTR = dma_transfer_size/4;
}



void host_platform_enable_high_speed_sdio( void )
{
    SDIO_InitTypeDef sdio_init_structure;
    uint8_t          a;

    for ( a = WIFI_PIN_SDIO_CLK; a < WIFI_PIN_SDIO_MAX; a++ )
    {
        platform_gpio_set_alternate_function( wifi_sdio_pins[ a ].port, wifi_sdio_pins[ a ].pin_number, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_AF_SDIO );
    }

    sdio_init_structure.SDIO_ClockDiv       = (uint8_t) 0; /* 0 = 24MHz if SDIO clock = 48MHz */
    sdio_init_structure.SDIO_ClockEdge      = SDIO_ClockEdge_Rising;
    sdio_init_structure.SDIO_ClockBypass    = SDIO_ClockBypass_Disable;
    sdio_init_structure.SDIO_ClockPowerSave = SDIO_ClockPowerSave_Disable;
#ifndef SDIO_1_BIT
    sdio_init_structure.SDIO_BusWide = SDIO_BusWide_4b;
#else
    sdio_init_structure.SDIO_BusWide = SDIO_BusWide_1b;
#endif
    sdio_init_structure.SDIO_HardwareFlowControl = SDIO_HardwareFlowControl_Disable;

    SDIO_DeInit( );
    SDIO_Init( &sdio_init_structure );
    SDIO_SetPowerState( SDIO_PowerState_ON );
    SDIO_ClockCmd( ENABLE );
    sdio_enable_bus_irq( );
}

static sdio_block_size_t find_optimal_block_size( uint32_t data_size )
{
    if ( data_size > (uint32_t) 256 )
        return SDIO_512B_BLOCK;
    if ( data_size > (uint32_t) 128 )
        return SDIO_256B_BLOCK;
    if ( data_size > (uint32_t) 64 )
        return SDIO_128B_BLOCK;
    if ( data_size > (uint32_t) 32 )
        return SDIO_64B_BLOCK;
    if ( data_size > (uint32_t) 16 )
        return SDIO_32B_BLOCK;
    if ( data_size > (uint32_t) 8 )
        return SDIO_16B_BLOCK;
    if ( data_size > (uint32_t) 4 )
        return SDIO_8B_BLOCK;
    if ( data_size > (uint32_t) 2 )
        return SDIO_4B_BLOCK;

    return SDIO_4B_BLOCK;
}

static uint32_t sdio_get_blocksize_dctrl(sdio_block_size_t block_size)
{
    switch (block_size)
    {
        case SDIO_1B_BLOCK:    return SDIO_DataBlockSize_1b;
        case SDIO_2B_BLOCK:    return SDIO_DataBlockSize_2b;
        case SDIO_4B_BLOCK:    return SDIO_DataBlockSize_4b;
        case SDIO_8B_BLOCK:    return SDIO_DataBlockSize_8b;
        case SDIO_16B_BLOCK:   return SDIO_DataBlockSize_16b;
        case SDIO_32B_BLOCK:   return SDIO_DataBlockSize_32b;
        case SDIO_64B_BLOCK:   return SDIO_DataBlockSize_64b;
        case SDIO_128B_BLOCK:  return SDIO_DataBlockSize_128b;
        case SDIO_256B_BLOCK:  return SDIO_DataBlockSize_256b;
        case SDIO_512B_BLOCK:  return SDIO_DataBlockSize_512b;
        case SDIO_1024B_BLOCK: return SDIO_DataBlockSize_1024b;
        case SDIO_2048B_BLOCK: return SDIO_DataBlockSize_2048b;
        default: return 0;
    }
}

OSStatus host_platform_bus_enable_interrupt( void )
{
    return  kNoErr;
}

OSStatus host_platform_bus_disable_interrupt( void )
{
    return  kNoErr;
}

void SDIO_IRQHandler(void)
{
  /* Process All SDIO Interrupt Sources */
  sdio_irq();
}

void DMA2_Stream3_IRQHandler(void)
{
  dma_irq();
}

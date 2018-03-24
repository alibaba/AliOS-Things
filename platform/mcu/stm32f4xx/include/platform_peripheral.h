

/** @file
 *  Defines generic data and enumerated types used by Platform Peripheral API
 *  Declares function prototypes for Platform peripheral API
 */
#pragma once
#include "stdint.h"
#include "platform_logging.h"
#include "platform_mcu_peripheral.h" /* Include MCU-specific types */
#include "RingBufferUtils.h"
#include "platform_config.h"
#include "hal/soc/gpio.h"
#include "hal/soc/soc.h"
#include "hal/soc/flash.h"
#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif
/*
------------------------------------------------------------------------------------------------------------
Porting Notes
------------------------------------------------------------------------------------------------------------
* This file defines and consolidates various standardized data types required by the Platform Peripheral API
* Generic Platform Peripheral data types are defined in this file
* MCU-specific data types are defined in <MiCO-SDK>/platform/MCU/<MCU_family>/platform_mcu_peripheral.h
* platform_mcu_peripheral.h may include declarations, definitions, and function prototypes which are local to the MCU family
* The following table outlines the structures that need to be defined in platform_mcu_peripheral.h:
+------------------------+-----------------------+
| Name                   | Description           |
+------------------------+-----------------------+
| platform_gpio_t        | GPIO interface        |
| platform_uart_t        | UART interface        |
| platform_uart_driver_t | UART driver interface |
| platform_spi_t         | SPI  interface        |
| platform_i2c_t         | I2C  interface        |
| platform_pwm_t         | PWM  interface        |
| platform_adc_t         | ADC  interface        |
+------------------------+-----------------------+
*/

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

#define MICO_PERIPHERAL_UNSUPPORTED ( 0xFFFFFFFF )

/* SPI mode constants */
#define SPI_CLOCK_RISING_EDGE  ( 1 << 0 )
#define SPI_CLOCK_FALLING_EDGE ( 0 << 0 )
#define SPI_CLOCK_IDLE_HIGH    ( 1 << 1 )
#define SPI_CLOCK_IDLE_LOW     ( 0 << 1 )
#define SPI_USE_DMA            ( 1 << 2 )
#define SPI_NO_DMA             ( 0 << 2 )
#define SPI_MSB_FIRST          ( 1 << 3 )
#define SPI_LSB_FIRST          ( 0 << 3 )

/* I2C flags constants */
#define I2C_DEVICE_DMA_MASK_POSN ( 0 )
#define I2C_DEVICE_NO_DMA        ( 0 << I2C_DEVICE_DMA_MASK_POSN )
#define I2C_DEVICE_USE_DMA       ( 1 << I2C_DEVICE_DMA_MASK_POSN )

#define USE_RTC_BKP 0x00BB32F2 // Use RTC BKP to initilize system time.


#define UART_WAKEUP_MASK_POSN   0
#define UART_WAKEUP_DISABLE    (0 << UART_WAKEUP_MASK_POSN) /**< UART can not wakeup MCU from stop mode */
#define UART_WAKEUP_ENABLE     (1 << UART_WAKEUP_MASK_POSN) /**< UART can wake up MCU from stop mode */
 
/******************************************************
 *                   Enumerations
 ******************************************************/

/**
 * Pin configuration
 */
typedef gpio_config_t platform_pin_config_t;

/**
 * GPIO interrupt trigger
 */
typedef gpio_irq_trigger_t platform_gpio_irq_trigger_t;

/**
 * UART data width
 */
typedef hal_uart_data_width_t platform_uart_data_width_t;

/**
 * UART stop bits
 */
typedef hal_uart_stop_bits_t platform_uart_stop_bits_t;

/**
 * UART flow control
 */
typedef hal_uart_flow_control_t platform_uart_flow_control_t;

/**
 * UART parity
 */
typedef hal_uart_parity_t platform_uart_parity_t;
/**
 * I2C address width
 */
typedef enum
{
    I2C_ADDRESS_WIDTH_7BIT,
    I2C_ADDRESS_WIDTH_10BIT,
    I2C_ADDRESS_WIDTH_16BIT,
} platform_i2c_bus_address_width_t;

/**
 * I2C speed mode
 */
typedef enum
{
    I2C_LOW_SPEED_MODE,         /* 10Khz devices */
    I2C_STANDARD_SPEED_MODE,    /* 100Khz devices */
    I2C_HIGH_SPEED_MODE         /* 400Khz devices */
} platform_i2c_speed_mode_t;

/**
 * GTimer mode
 */
typedef enum
{
    ONE_SHOT,
    PERIOIC,
} platform_gtimer_mode_t;

/**
 * SPI slave transfer direction
 */
typedef enum
{
    SPI_SLAVE_TRANSFER_WRITE, /* SPI master writes data to the SPI slave device */
    SPI_SLAVE_TRANSFER_READ   /* SPI master reads data from the SPI slave device */
} platform_spi_slave_transfer_direction_t;

typedef enum
{
    SPI_SLAVE_TRANSFER_SUCCESS,             /* SPI transfer successful */
    SPI_SLAVE_TRANSFER_INVALID_COMMAND,     /* Command is invalid */
    SPI_SLAVE_TRANSFER_ADDRESS_UNAVAILABLE, /* Address specified in the command is unavailable */
    SPI_SLAVE_TRANSFER_LENGTH_MISMATCH,     /* Length specified in the command doesn't match with the actual data length */
    SPI_SLAVE_TRANSFER_READ_NOT_ALLOWED,    /* Read operation is not allowed for the address specified */
    SPI_SLAVE_TRANSFER_WRITE_NOT_ALLOWED,   /* Write operation is not allowed for the address specified */
    SPI_SLAVE_TRANSFER_HARDWARE_ERROR,      /* Hardware error occurred during transfer */
    SPI_SLAVE_TRANSFER_STATUS_MAX = 0xff,   /* Denotes maximum value. Not a valid status */
} platform_spi_slave_transfer_status_t;


typedef enum
{
    FLASH_TYPE_EMBEDDED,
    FLASH_TYPE_SPI,
    FLASH_TYPE_QSPI,
} platform_flash_type_t;

/******************************************************
 *                 Type Definitions
 ******************************************************/

/**
 * GPIO interrupt callback handler
 */
typedef void (*platform_gpio_irq_callback_t)( void* arg );

/**
 * Gtimer interrupt callback handler
 */
typedef void (*platform_gtimer_irq_callback_t)( void* arg );

/******************************************************
 *                    Structures
 ******************************************************/

/**
 * UART configuration
 */
typedef struct
{
    uint32_t                     baud_rate;
    platform_uart_data_width_t   data_width;
    platform_uart_parity_t       parity;
    platform_uart_stop_bits_t    stop_bits;
    platform_uart_flow_control_t flow_control;
    uint8_t                      flags;          /**< if set, UART can wake up MCU from stop mode, reference: @ref UART_WAKEUP_DISABLE and @ref UART_WAKEUP_ENABLE*/
} platform_uart_config_t;

/**
 * SPI configuration
 */
typedef struct
{
    uint32_t               speed;
    uint8_t                mode;
    uint8_t                bits;
    const platform_gpio_t* chip_select;
} platform_spi_config_t;

#pragma pack(1)
typedef struct platform_spi_slave_command
{
    platform_spi_slave_transfer_direction_t direction;
    uint16_t                                address;
    uint16_t                                data_length;
} platform_spi_slave_command_t;

typedef struct
{
    uint16_t                             data_length;
    platform_spi_slave_transfer_status_t status;
    uint8_t                              data[1];
} platform_spi_slave_data_buffer_t;

/**
 * SPI slave configuration
 */
typedef struct platform_spi_slave_config
{
    uint32_t speed;
    uint8_t  mode;
    uint8_t  bits;
} platform_spi_slave_config_t;

#pragma pack()



/**
 * SPI message segment
 */
typedef struct
{
    const void* tx_buffer;
    void*       rx_buffer;
    uint32_t    length;
} platform_spi_message_segment_t;

/**
 * IIS message segment
 */
typedef struct
{
    const void* tx_buffer;
    void*       rx_buffer;
    uint32_t    length;
} platform_iis_message_segment_t;

/**
 * I2C configuration
 */
typedef struct
{
    uint16_t                         address;       /* the address of the device on the i2c bus */
    platform_i2c_bus_address_width_t address_width;
    uint8_t                          flags;
    platform_i2c_speed_mode_t        speed_mode;    /* speed mode the device operates in */
} platform_i2c_config_t;

/**
 * I2C message
 */
typedef struct
{
    const void*  tx_buffer;
    void*        rx_buffer;
    uint16_t     tx_length;
    uint16_t     rx_length;
    uint16_t     retries;    /* Number of times to retry the message */
    bool combined;           /**< If set, this message is used for both tx and rx. */
    //uint8_t      flags;      /* MESSAGE_DISABLE_DMA : if set, this flag disables use of DMA for the message */
} platform_i2c_message_t;

/**
 * RTC time
 */
typedef struct
{
    uint8_t sec;
    uint8_t min;
    uint8_t hr;
    uint8_t weekday;/* 1-sunday... 7-saturday */
    uint8_t date;
    uint8_t month;
    uint8_t year;
} platform_rtc_time_t;

typedef struct
{
    mico_flash_t               partition_owner;
    const char*                partition_description;
    uint32_t                   partition_start_addr;
    uint32_t                   partition_length;
    uint32_t                   partition_options;
} platform_logic_partition_t;

/******************************************************
 *                 Global Variables
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/

/**
 * performs complete reset operation
 */

void platform_mcu_reset( void );


/**
 * Initialise the specified GPIO pin
 *
 * @param[in] gpio   : gpio pin
 * @param[in] config : pin configuration
 *
 * @return @ref OSStatus
 */
OSStatus platform_gpio_init( const platform_gpio_t* gpio, platform_pin_config_t config );


/**
 * Deinitialise the specified GPIO pin
 *
 * @param[in] gpio : gpio pin
 *
 * @return @ref OSStatus
 */
OSStatus platform_gpio_deinit( const platform_gpio_t* gpio );


/**
 * Toggle the specified GPIO pin output high
 *
 * @param[in] gpio : gpio pin
 *
 * @return @ref OSStatus
 */
OSStatus platform_gpio_output_high( const platform_gpio_t* gpio );


/**
 * Toggle the specified GPIO pin output low
 *
 * @param[in] gpio : gpio pin
 *
 * @return @ref OSStatus
 */
OSStatus platform_gpio_output_low( const platform_gpio_t* gpio );


/**
 * Toggle the specified GPIO pin
 *
 * @param[in] gpio : gpio pin
 *
 * @return @ref OSStatus
 */
OSStatus platform_gpio_output_trigger( const platform_gpio_t* gpio );


/**
 * Retrieve logic level of the GPIO input pin specified
 *
 * @param[in] gpio : gpio pin
 *
 * @return @ref OSStatus
 */
bool platform_gpio_input_get( const platform_gpio_t* gpio );


/**
 * Enable interrupt on the GPIO input pin specified
 *
 * @param[in] gpio    : gpio pin
 * @param[in] trigger : interrupt trigger type
 * @param[in] handler : callback function that will be called when an interrupt occurs
 * @param[in] arg     : argument that will be passed into the callback function
 *
 * @return @ref OSStatus
 */
OSStatus platform_gpio_irq_enable( const platform_gpio_t* gpio, platform_gpio_irq_trigger_t trigger, platform_gpio_irq_callback_t handler, void* arg );


/**
 * Disable interrupt on the GPIO input pin specified
 *
 * @param[in] gpio : gpio pin
 *
 * @return @ref OSStatus
 */
OSStatus platform_gpio_irq_disable( const platform_gpio_t* gpio );


/**
 * Enable MCU powersave
 *
 * @return @ref OSStatus
 */
OSStatus platform_mcu_powersave_enable( void );


/**
 * Disable MCU powersave
 *
 * @return @ref OSStatus
 */
OSStatus platform_mcu_powersave_disable( void );

/**
 * Enter standby mode, and wait a period to wakup
 *
 * @param[in] secondsToWakeup : seconds to wakeup
 */
void platform_mcu_enter_standby( uint32_t secondsToWakeup );


/**
 * Notify the software stack that MCU has exited powersave mode due to interrupt
 *
 * @return @ref OSStatus
 */
void platform_mcu_powersave_exit_notify( void );


OSStatus platform_watchdog_init( uint32_t timeout_ms );

/**
 * Refresh the watchdog
 *
 * @return @ref OSStatus
 */
OSStatus platform_watchdog_kick( void );


/**
 * Check if last reset occurred due to watchdog reset
 *
 * @return @ref OSStatus
 */
bool platform_watchdog_check_last_reset( void );


/**
 * Initialise the specified UART port
 *
 * @return @ref OSStatus
 */

OSStatus platform_uart_init( platform_uart_driver_t* driver, const platform_uart_t* peripheral, const platform_uart_config_t* config, ring_buffer_t* optional_ring_buffer );


/**
 * Deinitialise the specified UART port
 *
 * @return @ref OSStatus
 */
OSStatus platform_uart_deinit( platform_uart_driver_t* driver );


/**
 * Transmit data over the specified UART port
 *
 * @return @ref OSStatus
 */
OSStatus platform_uart_transmit_bytes( platform_uart_driver_t* driver, const uint8_t* data_out, uint32_t size );


/**
 * Receive data over the specified UART port
 *
 * @return @ref OSStatus
 */
OSStatus platform_uart_receive_bytes( platform_uart_driver_t* driver, uint8_t* data_in, uint32_t expected_data_size, uint32_t timeout_ms );


/**
 * Get the received data length in ring buffer over the specified UART port
 *
 * @return 
 */
uint32_t platform_uart_get_length_in_buffer( platform_uart_driver_t* driver );

/**
 * Initialise the specified SPI interface
 *
 * @param[in] spi_interface : SPI interface
 * @param[in] config        : SPI configuratin
 *
 * @return @ref OSStatus
 */
OSStatus platform_spi_init( platform_spi_driver_t* driver, const platform_spi_t* peripheral, const platform_spi_config_t* config );
OSStatus platform_wlan_spi_init( const platform_gpio_t* chip_select );


/**
 * Deinitialise the specified SPI interface
 *
 * @return @ref OSStatus
 */
OSStatus platform_spi_deinit( platform_spi_driver_t* driver );
OSStatus platform_wlan_spi_deinit( const platform_gpio_t* chip_select );


/**
 * Transfer data over the specified SPI interface
 *
 * @return @ref OSStatus
 */
OSStatus platform_spi_transfer( platform_spi_driver_t* driver, const platform_spi_config_t* config, const platform_spi_message_segment_t* segments, uint16_t number_of_segments );
OSStatus platform_wlan_spi_transfer( const platform_gpio_t* chip_select, const platform_spi_message_segment_t* segments, uint16_t number_of_segments );


/** Initialises a SPI slave interface
 *
 * @param[in]  driver     : the SPI slave driver to be initialised
 * @param[in]  peripheral : the SPI peripheral interface to be initialised
 * @param[in]  config     : SPI slave configuration
 *
 * @return @ref OSStatus
 */
WEAK OSStatus platform_spi_slave_init( platform_spi_slave_driver_t* driver, const platform_spi_t* peripheral, const platform_spi_slave_config_t* config );


/** De-initialises a SPI slave interface
 *
 * @param[in]  driver : the SPI slave driver to be de-initialised
 *
 * @return @ref OSStatus
 */

WEAK OSStatus platform_spi_slave_deinit( platform_spi_slave_driver_t* driver );


/** Receive command from the remote SPI master
 *
 * @param[in]   driver      : the SPI slave driver
 * @param[out]  command     : pointer to the variable which will contained the received command
 * @param[in]   timeout_ms  : timeout in milliseconds
 *
 * @return @ref OSStatus
 */
WEAK OSStatus platform_spi_slave_receive_command( platform_spi_slave_driver_t* driver, platform_spi_slave_command_t* command, uint32_t timeout_ms );


/** Transfer data to/from the remote SPI master
 *
 * @param[in]  driver      : the SPI slave driver
 * @param[in]  direction   : transfer direction
 * @param[in]  buffer      : the buffer which contain the data to transfer
 * @param[in]  timeout_ms  : timeout in milliseconds
 *
 * @return @ref OSStatus
 */
WEAK OSStatus platform_spi_slave_transfer_data( platform_spi_slave_driver_t* driver, platform_spi_slave_transfer_direction_t direction, platform_spi_slave_data_buffer_t* buffer, uint32_t timeout_ms );


/** Send an error status over the SPI slave interface
 *
 * @param[in]  driver       : the SPI slave driver
 * @param[in]  error_status : SPI slave error status
 *
 * @return @ref OSStatus
 */
WEAK OSStatus platform_spi_slave_send_error_status( platform_spi_slave_driver_t* driver, platform_spi_slave_transfer_status_t error_status );


/** Generate an interrupt on the SPI slave interface
 *
 * @param[in]  driver            : the SPI slave driver
 * @param[in]  pulse_duration_ms : interrupt pulse duration in milliseconds
 *
 * @return @ref OSStatus
 */
WEAK OSStatus platform_spi_slave_generate_interrupt( platform_spi_slave_driver_t* driver, uint32_t pulse_duration_ms );


/**
 * Initialise ADC interface
 *
 * @param[in] adc_interface : adc_interface
 * @param[in] sample_cycle  : sample cycle
 *
 * @return @ref OSStatus
 */
OSStatus platform_adc_init( const platform_adc_t* adc, uint32_t sample_cycle );


/**
 * Deinitialise ADC interface
 *
 * @param[in] adc_interface : adc_interface
 *
 * @return @ref OSStatus
 */
OSStatus platform_adc_deinit( const platform_adc_t* adc );


/**
 * Take ADC sample
 *
 * @param[in]  adc_interface : adc_interface
 * @param[out] output        : variable that will contain the sample output
 *
 * @return @ref OSStatus
 */
OSStatus platform_adc_take_sample( const platform_adc_t* adc, uint16_t* output );


/**
 * Take ADC sample
 *
 * @param[in]  adc_interface : ADC interface
 * @param[out] buffer        : buffer that will contain the sample stream output
 * @param[in]  buffer_length : buffer length
 *
 * @return @ref OSStatus
 */
OSStatus platform_adc_take_sample_stream( const platform_adc_t* adc, void* buffer, uint16_t buffer_length );


/**
 * Initialise I2C interface
 *
 * @param[in] i2c_interface : I2C interface
 * @param[in] config        : I2C configuration
 *
 * @return @ref OSStatus
 */
OSStatus platform_i2c_init( const platform_i2c_t* i2c, const platform_i2c_config_t* config );


/**
 * Deinitialise I2C interface
 *
 * @param[in] i2c_interface : I2C interface
 *
 * @return @ref OSStatus
 */
OSStatus platform_i2c_deinit( const platform_i2c_t* i2c, const platform_i2c_config_t* config );


/**
 * Probe I2C slave device
 *
 * @param[in] i2c_interface : I2C interface
 * @param[in] retries       : number of retries
 *
 * @return @ref OSStatus
 */
bool platform_i2c_probe_device( const platform_i2c_t* i2c, const platform_i2c_config_t* config, int retries );


/**
 * Initialise I2C transmit message
 *
 * @param[in,out] message          : I2C message
 * @param[in]     tx_buffer        : transmit buffer
 * @param[in]     tx_buffer_length : transmit buffer length is bytes
 * @param[in]     retries          : number of transmission retries
 *
 * @return @ref OSStatus
 */
OSStatus platform_i2c_init_tx_message( platform_i2c_message_t* message, const void* tx_buffer, uint16_t tx_buffer_length, uint16_t retries );


/**
 * Initialise I2C receive message
 *
 * @param[in,out] message          : I2C message
 * @param[in]     rx_buffer        : receive buffer
 * @param[in]     rx_buffer_length : receive buffer length is bytes
 * @param[in]     retries          : number of transmission retries
 *
 * @return @ref OSStatus
 */
OSStatus platform_i2c_init_rx_message( platform_i2c_message_t* message, void* rx_buffer, uint16_t rx_buffer_length, uint16_t retries );


/**
 * Initialise I2C combined message
 *
 * @param[in,out] message          : I2C message
 * @param[in]     tx_buffer        : transmit buffer
 * @param[in]     rx_buffer        : receive buffer
 * @param[in]     tx_buffer_length : transmit buffer length is bytes
 * @param[in]     rx_buffer_length : receive buffer length is bytes
 * @param[in]     retries          : number of transmission retries
 *
 * @return @ref OSStatus
 */
OSStatus platform_i2c_init_combined_message( platform_i2c_message_t* message, const void* tx_buffer, void* rx_buffer, uint16_t tx_buffer_length, uint16_t rx_buffer_length, uint16_t retries );


/**
 * Transfer data via the I2C interface
 *
 * @param[in] i2c_interface      : I2C interface
 * @param[in] messages           : pointer to an array of messages to transceive
 * @param[in] number_of_messages : number of messages in the array
 *
 * @return @ref OSStatus
 */
OSStatus platform_i2c_transfer( const platform_i2c_t* i2c, const platform_i2c_config_t* config, platform_i2c_message_t* messages, uint16_t number_of_messages );


/**
 * Initialise PWM interface
 *
 * @param[in] pwm_interface : PWM interface
 * @param[in] frequency     : PWM signal frequency in Hz
 * @param[in] duty_cycle    : PWM signal duty cycle in percentage point
 *
 * @return @ref OSStatus
 */
OSStatus platform_pwm_init( const platform_pwm_t* pwm, uint32_t frequency, float duty_cycle );


/**
 * Start generating PWM signal on the specified PWM interface
 *
 * @param[in] pwm_interface : PWM interface
 *
 * @return @ref OSStatus
 */
OSStatus platform_pwm_start( const platform_pwm_t* pwm );


/**
 * Stop generating PWM signal on the specified PWM interface
 *
 * @param[in] pwm_interface : PWM interface
 *
 * @return @ref OSStatus
 */
OSStatus platform_pwm_stop( const platform_pwm_t* pwm );


/**
 * Get current real-time clock
 *
 * @param[in] time : variable that will contain the current real-time clock
 *
 * @return @ref OSStatus
 */
OSStatus platform_rtc_get_time( platform_rtc_time_t* time );


/**
 * Set real-time clock
 *
 * @param[in] time : real-time clock
 *
 * @return @ref OSStatus
 */
OSStatus platform_rtc_set_time( const platform_rtc_time_t* time );


/**
 * Initialise UART standard I/O
 *
 * @param[in,out] driver    : UART STDIO driver
 * @param[in]     interface : UART STDIO interface
 * @param[in]     config    : UART STDIO configuration
 *
 * @return @ref OSStatus
 */
OSStatus platform_stdio_init ( platform_uart_driver_t* driver, const platform_uart_t* interface, const platform_uart_config_t* config );


/**
 * Get current value of nanosecond clock
 *
*/
uint64_t platform_get_nanosecond_clock_value( void );


/**
 * Deinitialize nanosecond clock
 *
 */
void platform_deinit_nanosecond_clock( void );


/**
 * Reset nanosecond clock
 *
 */
void platform_reset_nanosecond_clock( void );


/**
 * Initialize nanosecond clock
 *
 */
void platform_init_nanosecond_clock( void );

/**
 * Nanosecond delay
 *
 */
void platform_nanosecond_delay( uint64_t delayns );

/**
 * Read random numbers
 *
 */
OSStatus platform_random_number_read( void *inBuffer, int inByteCount );

/**
 * Init flash driver and hardware interface
 *
 */
OSStatus platform_flash_init( const platform_flash_t *peripheral );

/**
 * Erase flash 
 *
 */
OSStatus platform_flash_erase( const platform_flash_t *peripheral, uint32_t start_address, uint32_t end_address  );

/**
 * Write flash 
 *
 */
OSStatus platform_flash_write( const platform_flash_t *peripheral, volatile uint32_t* start_address, uint8_t* data ,uint32_t length  );

/**
 * Read flash 
 *
 */
OSStatus platform_flash_read( const platform_flash_t *peripheral, volatile uint32_t* start_address, uint8_t* data ,uint32_t length  );

/**
 * Flash protect operation
 *
 */
OSStatus platform_flash_enable_protect( const platform_flash_t *peripheral, uint32_t start_address, uint32_t end_address );
OSStatus platform_flash_disable_protect( const platform_flash_t *peripheral, uint32_t start_address, uint32_t end_address );



#ifdef __cplusplus
} /*"C" */
#endif

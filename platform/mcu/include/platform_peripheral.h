/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#pragma once
#include "stdint.h"
#include "platform_logging.h"
#include "platform_mcu_peripheral.h" /* Include MCU-specific types */
#include "RingBufferUtils.h"
#include "board.h"
#include "gpio.h"
#include "uart.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MICO_PERIPHERAL_UNSUPPORTED ( 0xFFFFFFFF )

#define UART_WAKEUP_MASK_POSN   0
#define UART_WAKEUP_DISABLE    (0 << UART_WAKEUP_MASK_POSN) 
#define UART_WAKEUP_ENABLE     (1 << UART_WAKEUP_MASK_POSN) 

#define I2C_DEVICE_DMA_MASK_POSN ( 0 )
#define I2C_DEVICE_NO_DMA        ( 0 << I2C_DEVICE_DMA_MASK_POSN )
#define I2C_DEVICE_USE_DMA       ( 1 << I2C_DEVICE_DMA_MASK_POSN )

#define SPI_CLOCK_RISING_EDGE  ( 1 << 0 )
#define SPI_CLOCK_FALLING_EDGE ( 0 << 0 )
#define SPI_CLOCK_IDLE_HIGH    ( 1 << 1 )
#define SPI_CLOCK_IDLE_LOW     ( 0 << 1 )
#define SPI_USE_DMA            ( 1 << 2 )
#define SPI_NO_DMA             ( 0 << 2 )
#define SPI_MSB_FIRST          ( 1 << 3 )
#define SPI_LSB_FIRST          ( 0 << 3 )

#define USE_RTC_BKP 0x00BB32F2 




typedef hal_uart_parity_t platform_uart_parity_t;

typedef gpio_config_t platform_pin_config_t;

typedef gpio_irq_trigger_t platform_gpio_irq_trigger_t;

typedef hal_uart_flow_control_t platform_uart_flow_control_t;

typedef hal_uart_stop_bits_t platform_uart_stop_bits_t;


typedef hal_uart_data_width_t platform_uart_data_width_t;


typedef enum
{
    FLASH_TYPE_EMBEDDED,
    FLASH_TYPE_SPI,
    FLASH_TYPE_QSPI,
} platform_flash_type_t;

typedef enum
{
    I2C_ADDRESS_WIDTH_7BIT,
    I2C_ADDRESS_WIDTH_10BIT,
    I2C_ADDRESS_WIDTH_16BIT,
} platform_i2c_bus_address_width_t;

typedef enum
{
    I2C_LOW_SPEED_MODE,        
    I2C_STANDARD_SPEED_MODE,    
    I2C_HIGH_SPEED_MODE        
} platform_i2c_speed_mode_t;

typedef enum
{
    SPI_SLAVE_TRANSFER_WRITE, 
    SPI_SLAVE_TRANSFER_READ   
} platform_spi_slave_transfer_direction_t;

typedef enum
{
    SPI_SLAVE_TRANSFER_SUCCESS,            
    SPI_SLAVE_TRANSFER_INVALID_COMMAND,    
    SPI_SLAVE_TRANSFER_ADDRESS_UNAVAILABLE, 
    SPI_SLAVE_TRANSFER_LENGTH_MISMATCH,     
    SPI_SLAVE_TRANSFER_READ_NOT_ALLOWED,   
    SPI_SLAVE_TRANSFER_WRITE_NOT_ALLOWED,   
    SPI_SLAVE_TRANSFER_HARDWARE_ERROR,      
    SPI_SLAVE_TRANSFER_STATUS_MAX = 0xff,  
} platform_spi_slave_transfer_status_t;



typedef void (*platform_gpio_irq_callback_t)( void* arg );

typedef struct
{
    uint32_t                     baud_rate;
    platform_uart_data_width_t   data_width;
    platform_uart_parity_t       parity;
    platform_uart_stop_bits_t    stop_bits;
    platform_uart_flow_control_t flow_control;
    uint8_t                      flags;         
} platform_uart_config_t;



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

typedef struct platform_spi_slave_config
{
    uint32_t speed;
    uint8_t  mode;
    uint8_t  bits;
} platform_spi_slave_config_t;

#pragma pack()



typedef struct
{
    const void* tx_buffer;
    void*       rx_buffer;
    uint32_t    length;
} platform_spi_message_segment_t;

typedef struct
{
    uint16_t                         address;      
    platform_i2c_bus_address_width_t address_width;
    uint8_t                          flags;
    platform_i2c_speed_mode_t        speed_mode;   
} platform_i2c_config_t;

typedef struct
{
    const void*  tx_buffer;
    void*        rx_buffer;
    uint16_t     tx_length;
    uint16_t     rx_length;
    uint16_t     retries;    
    bool combined;          
} platform_i2c_message_t;

typedef struct
{
    uint8_t sec;
    uint8_t min;
    uint8_t hr;
    uint8_t weekday;
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


void platform_mcu_reset( void );


OSStatus platform_gpio_init( const platform_gpio_t* gpio, platform_pin_config_t config );


OSStatus platform_gpio_deinit( const platform_gpio_t* gpio );


OSStatus platform_gpio_output_high( const platform_gpio_t* gpio );


OSStatus platform_gpio_output_low( const platform_gpio_t* gpio );


OSStatus platform_gpio_output_trigger( const platform_gpio_t* gpio );


bool platform_gpio_input_get( const platform_gpio_t* gpio );


OSStatus platform_gpio_irq_enable( const platform_gpio_t* gpio, platform_gpio_irq_trigger_t trigger, platform_gpio_irq_callback_t handler, void* arg );


OSStatus platform_gpio_irq_disable( const platform_gpio_t* gpio );


OSStatus platform_mcu_powersave_enable( void );


OSStatus platform_mcu_powersave_disable( void );

void platform_mcu_enter_standby( uint32_t secondsToWakeup );


void platform_mcu_powersave_exit_notify( void );


OSStatus platform_watchdog_init( uint32_t timeout_ms );

OSStatus platform_watchdog_kick( void );


bool platform_watchdog_check_last_reset( void );


OSStatus platform_uart_init( platform_uart_driver_t* driver, const platform_uart_t* peripheral, const platform_uart_config_t* config, ring_buffer_t* optional_ring_buffer );


OSStatus platform_uart_deinit( platform_uart_driver_t* driver );


OSStatus platform_uart_transmit_bytes( platform_uart_driver_t* driver, const uint8_t* data_out, uint32_t size );


OSStatus platform_uart_receive_bytes( platform_uart_driver_t* driver, uint8_t* data_in, uint32_t expected_data_size, uint32_t timeout_ms );


uint32_t platform_uart_get_length_in_buffer( platform_uart_driver_t* driver );

OSStatus platform_spi_init( platform_spi_driver_t* driver, const platform_spi_t* peripheral, const platform_spi_config_t* config );
OSStatus platform_wlan_spi_init( const platform_gpio_t* chip_select );


OSStatus platform_spi_deinit( platform_spi_driver_t* driver );
OSStatus platform_wlan_spi_deinit( const platform_gpio_t* chip_select );


OSStatus platform_spi_transfer( platform_spi_driver_t* driver, const platform_spi_config_t* config, const platform_spi_message_segment_t* segments, uint16_t number_of_segments );
OSStatus platform_wlan_spi_transfer( const platform_gpio_t* chip_select, const platform_spi_message_segment_t* segments, uint16_t number_of_segments );


WEAK OSStatus platform_spi_slave_init( platform_spi_slave_driver_t* driver, const platform_spi_t* peripheral, const platform_spi_slave_config_t* config );


WEAK OSStatus platform_spi_slave_deinit( platform_spi_slave_driver_t* driver );


WEAK OSStatus platform_spi_slave_receive_command( platform_spi_slave_driver_t* driver, platform_spi_slave_command_t* command, uint32_t timeout_ms );


WEAK OSStatus platform_spi_slave_transfer_data( platform_spi_slave_driver_t* driver, platform_spi_slave_transfer_direction_t direction, platform_spi_slave_data_buffer_t* buffer, uint32_t timeout_ms );


WEAK OSStatus platform_spi_slave_send_error_status( platform_spi_slave_driver_t* driver, platform_spi_slave_transfer_status_t error_status );


WEAK OSStatus platform_spi_slave_generate_interrupt( platform_spi_slave_driver_t* driver, uint32_t pulse_duration_ms );


OSStatus platform_adc_init( const platform_adc_t* adc, uint32_t sample_cycle );


OSStatus platform_adc_deinit( const platform_adc_t* adc );


OSStatus platform_adc_take_sample( const platform_adc_t* adc, uint16_t* output );


OSStatus platform_adc_take_sample_stream( const platform_adc_t* adc, void* buffer, uint16_t buffer_length );


OSStatus platform_i2c_init( const platform_i2c_t* i2c, const platform_i2c_config_t* config );


OSStatus platform_i2c_deinit( const platform_i2c_t* i2c, const platform_i2c_config_t* config );


bool platform_i2c_probe_device( const platform_i2c_t* i2c, const platform_i2c_config_t* config, int retries );



OSStatus platform_i2c_init_tx_message( platform_i2c_message_t* message, const void* tx_buffer, uint16_t tx_buffer_length, uint16_t retries );



OSStatus platform_i2c_init_rx_message( platform_i2c_message_t* message, void* rx_buffer, uint16_t rx_buffer_length, uint16_t retries );



OSStatus platform_i2c_init_combined_message( platform_i2c_message_t* message, const void* tx_buffer, void* rx_buffer, uint16_t tx_buffer_length, uint16_t rx_buffer_length, uint16_t retries );



OSStatus platform_i2c_transfer( const platform_i2c_t* i2c, const platform_i2c_config_t* config, platform_i2c_message_t* messages, uint16_t number_of_messages );



OSStatus platform_pwm_init( const platform_pwm_t* pwm, uint32_t frequency, float duty_cycle );



OSStatus platform_pwm_start( const platform_pwm_t* pwm );


OSStatus platform_pwm_stop( const platform_pwm_t* pwm );


OSStatus platform_rtc_get_time( platform_rtc_time_t* time );



OSStatus platform_rtc_set_time( const platform_rtc_time_t* time );



OSStatus platform_stdio_init ( platform_uart_driver_t* driver, const platform_uart_t* interface, const platform_uart_config_t* config );


uint64_t platform_get_nanosecond_clock_value( void );



void platform_deinit_nanosecond_clock( void );



void platform_reset_nanosecond_clock( void );


void platform_init_nanosecond_clock( void );


void platform_nanosecond_delay( uint64_t delayns );


OSStatus platform_random_number_read( void *inBuffer, int inByteCount );


OSStatus platform_flash_init( const platform_flash_t *peripheral );


OSStatus platform_flash_erase( const platform_flash_t *peripheral, uint32_t start_address, uint32_t end_address  );


OSStatus platform_flash_write( const platform_flash_t *peripheral, volatile uint32_t* start_address, uint8_t* data ,uint32_t length  );


OSStatus platform_flash_read( const platform_flash_t *peripheral, volatile uint32_t* start_address, uint8_t* data ,uint32_t length  );


OSStatus platform_flash_enable_protect( const platform_flash_t *peripheral, uint32_t start_address, uint32_t end_address );
OSStatus platform_flash_disable_protect( const platform_flash_t *peripheral, uint32_t start_address, uint32_t end_address );



#ifdef __cplusplus
} /*"C" */
#endif

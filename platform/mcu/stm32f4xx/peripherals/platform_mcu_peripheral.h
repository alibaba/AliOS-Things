
#pragma once

#include "stm32f4xx.h"
#include "stm32f4xx_dma.h"
#include "stm32f4xx_adc.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_rtc.h"
#include "stm32f4xx_pwr.h"
#include "stm32f4xx_rng.h"

#include "mico_rtos.h"
#include "RingBufferUtils.h"

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

 /* GPIOA to I */
#define NUMBER_OF_GPIO_PORTS      (8)

/* Interrupt line 0 to 15. Each line is shared among the same numbered pins across all GPIO ports */
#define NUMBER_OF_GPIO_IRQ_LINES  (16)

/* USART1 to 6 */
#define NUMBER_OF_UART_PORTS      (6)


/* Invalid UART port number */
#define INVALID_UART_PORT_NUMBER  (0xff)

 /* SPI1 to SPI3 */
#define NUMBER_OF_SPI_PORTS       (3)

/******************************************************
 *                   Enumerations
 ******************************************************/
 
/******************************************************
 *                 Type Definitions
 ******************************************************/

/* GPIO port */
typedef GPIO_TypeDef  platform_gpio_port_t;

/* UART port */
typedef USART_TypeDef platform_uart_port_t;

/* SPI port */
typedef SPI_TypeDef   platform_spi_port_t;

/* QSPI port */
#if defined(STM32F412xG) || defined(STM32F446xx) || defined(STM32F469_479xx)
typedef QUADSPI_TypeDef   platform_qspi_port_t;
#endif

/* I2C port */
typedef I2C_TypeDef   platform_i2c_port_t;

/* GPIO alternate function */
typedef uint8_t       platform_gpio_alternate_function_t;

/* Peripheral clock function */
typedef void (*platform_peripheral_clock_function_t)(uint32_t clock, FunctionalState state );

typedef DMA_TypeDef     dma_registers_t;
typedef FunctionalState functional_state_t;
typedef uint32_t        peripheral_clock_t;

/******************************************************
 *                    Structures
 ******************************************************/
typedef struct
{
    DMA_TypeDef*        controller;
    DMA_Stream_TypeDef* stream;
    uint32_t            channel;
    IRQn_Type           irq_vector;
    uint32_t            complete_flags;
    uint32_t            error_flags;
} platform_dma_config_t;

typedef struct
{
    platform_gpio_port_t* port; 
    uint8_t               pin_number;
} platform_gpio_t;

typedef struct
{
    ADC_TypeDef*           port;
    uint8_t                channel;
    uint32_t               adc_peripheral_clock;
    uint8_t                rank;
    const platform_gpio_t* pin;
} platform_adc_t;

typedef struct
{
    TIM_TypeDef*           tim;
    uint8_t                channel;
    uint32_t               tim_peripheral_clock;
    uint8_t                gpio_af;
    const platform_gpio_t* pin;
} platform_pwm_t;

#if defined(STM32F412xG) || defined(STM32F446xx) || defined(STM32F469_479xx)
typedef struct
{
    platform_qspi_port_t*                port;
    uint32_t                             FSelect;
    uint32_t                             peripheral_clock_reg;
    platform_peripheral_clock_function_t peripheral_clock_func;
    const platform_gpio_t*               pin_d0;
    const platform_gpio_t*               pin_d1;
    const platform_gpio_t*               pin_d2;
    const platform_gpio_t*               pin_d3;
    const platform_gpio_t*               pin_clock;
    const platform_gpio_t*               pin_cs;
    platform_dma_config_t                dma;
    uint8_t                              gpio_af_d0;
    uint8_t                              gpio_af_d1;
    uint8_t                              gpio_af_d2;
    uint8_t                              gpio_af_d3;
    uint8_t                              gpio_af_clk;
    uint8_t                              gpio_af_cs;
} platform_qspi_t;
#endif

/* DMA can be enabled by setting SPI_USE_DMA */
typedef struct
{
    platform_spi_port_t*                 port;
    uint8_t                              gpio_af;
    uint32_t                             peripheral_clock_reg;
    platform_peripheral_clock_function_t peripheral_clock_func;
    const platform_gpio_t*               pin_mosi;
    const platform_gpio_t*               pin_miso;
    const platform_gpio_t*               pin_clock;
    platform_dma_config_t                tx_dma;
    platform_dma_config_t                rx_dma;
} platform_spi_t;

typedef struct
{
    platform_spi_t*           peripheral;
    mico_mutex_t              spi_mutex;
} platform_spi_driver_t;

typedef struct
{
    uint8_t unimplemented;
} platform_spi_slave_driver_t;

typedef struct
{
    platform_i2c_port_t*   port;
    const platform_gpio_t* pin_scl;
    const platform_gpio_t* pin_sda;
    uint32_t               peripheral_clock_reg;
    dma_registers_t*       tx_dma;
    peripheral_clock_t     tx_dma_peripheral_clock;
    DMA_Stream_TypeDef*    tx_dma_stream;
    DMA_Stream_TypeDef*    rx_dma_stream;
    int                    tx_dma_stream_id;
    int                    rx_dma_stream_id;
    uint32_t               tx_dma_channel;
    uint32_t               rx_dma_channel;
    uint8_t                gpio_af_scl;
    uint8_t                gpio_af_sda;
} platform_i2c_t;

typedef struct
{
    mico_mutex_t              i2c_mutex;
} platform_i2c_driver_t;

typedef void (* wakeup_irq_handler_t)(void *arg);

typedef struct
{
    platform_uart_port_t*  port;
    const platform_gpio_t* pin_tx;
    const platform_gpio_t* pin_rx;
    const platform_gpio_t* pin_cts;
    const platform_gpio_t* pin_rts;
    platform_dma_config_t  tx_dma_config;
    platform_dma_config_t  rx_dma_config;
} platform_uart_t;

typedef struct
{
    platform_uart_t*           peripheral;
    ring_buffer_t*             rx_buffer;
    mico_semaphore_t           rx_complete;
    mico_semaphore_t           tx_complete;
    mico_mutex_t               tx_mutex;
    mico_semaphore_t           sem_wakeup;
    volatile uint32_t          tx_size;
    volatile uint32_t          rx_size;
    volatile OSStatus          last_receive_result;
    volatile OSStatus          last_transmit_result;
    volatile bool              initialized;
    volatile bool              is_flow_control;
    volatile bool              is_recv_over_flow;
} platform_uart_driver_t;

typedef struct
{
    uint32_t                   flash_type;
    uint32_t                   flash_start_addr;
    uint32_t                   flash_length;
    uint32_t                   flash_protect_opt;
} platform_flash_t;

typedef struct
{
    const platform_flash_t*    peripheral;
    mico_mutex_t               flash_mutex;
    volatile bool              initialized;
} platform_flash_driver_t;

/******************************************************
 *                 Global Variables
 ******************************************************/


/******************************************************
 *               Function Declarations
 ******************************************************/
OSStatus platform_gpio_irq_manager_init      ( void );
uint8_t  platform_gpio_get_port_number       ( platform_gpio_port_t* gpio_port );
OSStatus platform_gpio_enable_clock          ( const platform_gpio_t* gpio );
OSStatus platform_gpio_set_alternate_function( platform_gpio_port_t* gpio_port, uint8_t pin_number, GPIOOType_TypeDef output_type, GPIOPuPd_TypeDef pull_up_down_type, uint8_t alternation_function );

OSStatus platform_mcu_powersave_init         ( void );

OSStatus platform_rtc_init                   ( void );
OSStatus platform_rtc_enter_powersave        ( void );
OSStatus platform_rtc_abort_powersave        ( void );
OSStatus platform_rtc_exit_powersave         ( uint32_t requested_sleep_time, uint32_t *cpu_sleep_time );

uint8_t  platform_uart_get_port_number       ( platform_uart_port_t* uart );
void     platform_uart_irq                   ( platform_uart_driver_t* driver );
void     platform_uart_tx_dma_irq            ( platform_uart_driver_t* driver );
void     platform_uart_rx_dma_irq            ( platform_uart_driver_t* driver );

uint8_t  platform_spi_get_port_number        ( platform_spi_port_t* spi );

#ifdef __cplusplus
} /* extern "C" */
#endif






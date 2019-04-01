#ifndef HAL_SPISLV
#define HAL_SPISLV

#include "sys/intc.h"   // components/
#include "hal_gpio.h"

#define SPISLV_ERROR_PRINTF (0)
#define SPISLV_DEBUG_PRINTF (0)

#define SPI_RAW_DATA_SIZE 1024


typedef enum {
    ST_IDLE         = 0,
    ST_PRE_IDLE     = 1,
    ST_PRE_BUSY     = 2,
    ST_BUSY         = 3    
} ENUM_SPI_SLV_STATUS_T;

typedef enum {
    INT_TRX_DONE    = 1        
} ENUM_SPI_SLV_INT_STATUS_T;

int8_t hal_spi_slv_disable(void);
int8_t hal_spi_slv_enable(void);
int8_t hal_spi_slv_pedge_mode_enable(void);
int8_t hal_spi_slv_pedge_mode_disable(void);
int8_t hal_spi_slv_init(gpio_pin_t gpio_interrupt, uint8_t priority, isr_func handler);
int8_t hal_spi_slv_interrupt_init(void);
int8_t hal_spi_slv_interrupt(uint8_t enable);
int8_t hal_spi_slv_trx_done_isr(isr_func irq_spi_slv_handler);
int8_t hal_spi_slv_reset(void);
int8_t hal_spi_slv_error_reduce(void) __attribute__((section(".prog_in_sram")));
int8_t hal_spi_slv_set_idle(void) __attribute__((section(".prog_in_sram")));
uint8_t hal_spi_slv_get_status(ENUM_SPI_SLV_STATUS_T status) __attribute__((section(".prog_in_sram")));
uint8_t hal_spi_slv_get_trx_done_interrupt_status(void) __attribute__((section(".prog_in_sram")));
int8_t hal_spi_slv_read_host_tx_data(uint32_t data_len, uint8_t *read_data) __attribute__((section(".prog_in_sram")));
int8_t hal_spi_slv_write_host_rx_data(uint32_t data_len, uint8_t *write_data) __attribute__((section(".prog_in_sram")));
int8_t hal_spi_slv_trx_done_isr(isr_func irq_spi_slv_handler);

#endif /* end of include guard */

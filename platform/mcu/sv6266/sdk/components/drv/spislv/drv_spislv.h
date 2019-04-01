#ifndef SPISLV
#define SPISLV
#include "sys/intc.h"

typedef enum {
    SPI_SLV_DEFAULT         = 0,
    SPI_SLV_HIGHT           = 1 
} spi_slv_priority_t;


/**
 * Disables SPI Slave operation.
 * 
 * @return  -1                              The operation error.
 * @return   0                              The operation completed successfully.
 */
int8_t drv_spi_slv_disable(void);


/**
 * Enables SPI Slave operation.
 * 
 * @return  -1                              The operation error.
 * @return   0                              The operation completed successfully.
 */
int8_t drv_spi_slv_enable(void);


/**
 * Initialize the SPI Slave.
 *
 * @param  gpio_interrupt                   Initialize gpio pin to setup SPI slave gpio interrupt
 * @param  priority                         change SPI slave bus priority.
 * @param  irq_spi_slv_handler              A pointer to an interrupt handler that is called when transfer done interrupt occurs.
 *
 * @retval  -1                              The operation error.
 * @retval   0                              The operation completed successfully.
 */
int8_t drv_spi_slv_init(uint8_t gpio_interrupt, spi_slv_priority_t priority, isr_func irq_spi_slv_handler);


/**
 * control the SPI Slave gpio interrupt.
 *
 * @param  enable                           control SPI gpio interrupt(1:high, 0:low)
 *
 * @retval  -1                              The operation error.
 * @retval   0                              The operation completed successfully.
 */
int8_t drv_spi_slv_interrupt(uint8_t enable);


/**
 * reduce SPI slave data error, have to use this api in the trx done interrupt handler
 *
 * @retval  -1                              The operation error.
 * @retval   0                              The operation completed successfully.
 */
int8_t drv_spi_slv_error_reduce(void)__attribute__((section(".prog_in_sram")));


/**
 * Returns the received data available in the receive buffer.
 * 
 * @param   length                          Size of the read buffer exposed to the SPI slave.
 * @param   data                            Pointer to the data buffer to be read by the SPI slave.
 *
 * @return  -1                              The operation error.
 * @return   0                              The operation completed successfully.
 */
int8_t drv_spi_slv_read_data(uint32_t data_len, uint8_t *read_data) __attribute__((section(".prog_in_sram")));


/**
 * Places the data in the transmit buffer that will be sent 
 * 
 * @param   length                          Size of the write buffer exposed to the SPI slave.
 * @param   data                            Pointer to the data buffer to be send by the SPI slave.
 *
 * @return  -1                              The operation error.
 * @return   0                              The operation completed successfully.
 */
int8_t drv_spi_slv_write_data(uint32_t data_len, uint8_t *write_data) __attribute__((section(".prog_in_sram")));


/**
 * set spi slave ready for transmission status.
 *
 * @retval  -1                              The operation error.
 * @retval   0                              The operation completed successfully.
 */
int8_t drv_spi_slv_set_data_ready(void) __attribute__((section(".prog_in_sram")));

#endif /* end of include guard */



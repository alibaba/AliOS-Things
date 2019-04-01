#ifndef _CLI_CMD_H_
#define _CLI_CMD_H_
#define SPI_ATTRIBUTE __attribute__((section(".prog_in_sram")))
void irq_hal_spi_raw_handler(void) SPI_ATTRIBUTE;
void spi_raw_data_task(void *param) SPI_ATTRIBUTE;
int test_spi_raw_slave(s32 argc, s8 *argv[]) SPI_ATTRIBUTE;
#endif /* _CLI_CMD_H_ */


#ifndef _CLI_CMD_H_
#define _CLI_CMD_H_
#define SPI_ATTRIBUTE __attribute__((section(".prog_in_sram")))
int test_spi_master_data_transfer(s32 argc, s8 *argv[]) SPI_ATTRIBUTE;

#endif /* _CLI_CMD_H_ */


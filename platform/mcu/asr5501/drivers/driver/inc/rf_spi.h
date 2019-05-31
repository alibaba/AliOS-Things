#ifndef __RF_SPI_H__
#define __RF_SPI_H__

#define RC_SW_CTRL_ADDR             0x60C0C004
#define SPI_COMMAND                 RC_SW_CTRL_ADDR
#define SPI_RDATA                   RC_SW_CTRL_ADDR

void spi_mst_write(uint16_t addr, uint16_t data );
uint16_t spi_mst_read(uint16_t addr);

#endif //__RF_SPI_H__
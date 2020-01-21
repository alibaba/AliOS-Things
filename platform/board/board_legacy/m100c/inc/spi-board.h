#ifndef __SPI_H
#define __SPI_H

#define HFRCO_FREQUENCY         14000000
#define SPI_PERCLK_FREQUENCY    HFRCO_FREQUENCY
#define SPI_BAUDRATE            1000000

void lora_spi_setup(void);
#if 0
void SPI2_setupRXInt(char* receiveBuffer, int bytesToReceive);
void USART2_RX_IRQHandler(void);
#endif
uint8_t lora_spi_inout(uint8_t tx_data);

#endif

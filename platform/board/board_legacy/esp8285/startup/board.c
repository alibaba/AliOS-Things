#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "aos/hal/uart.h"

#include "k_config.h"
#include "board.h"
#include "hal_spi.h"
#include "spi_interface.h"

SPI_MAPPING SPI_MAPPING_TABLE[PORT_SPI_SIZE] = {
{PORT_SPI_1, SpiNum_SPI, SpiSubMode_0, SpiBitOrder_MSBFirst},
{PORT_SPI_2, SpiNum_HSPI, SpiSubMode_0, SpiBitOrder_MSBFirst},
};

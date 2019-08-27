#include <stdint.h>
#include "lega_cm4.h"
#include "rf_spi.h"

void spi_mst_write(uint16_t addr, uint16_t data)
{
    uint32_t pre = 8<<28;
    uint32_t start = 1 <<27;
    uint32_t readnotwrite = 0 <<26;
    uint32_t addr_t = addr <<16;
    uint32_t data_t = data << 0;
    uint32_t register_00 = pre | start  | readnotwrite | addr_t | data_t ;

    uint32_t var=0;
    *((volatile int *) SPI_COMMAND)= register_00;

    do
    {
        var =(*((volatile int *)SPI_COMMAND)) & (0x1 << 27);
    }while(var);
}


uint16_t spi_mst_read(uint16_t addr)
{
    uint32_t pre = 8 << 28;
    uint32_t start = 1 <<27;
    uint32_t readnotwrite = 1 <<26;
    uint32_t addr_t = addr <<16;
    uint32_t data_t = 0;
    uint32_t register_00 = pre | start  | readnotwrite | addr_t | data_t ;

    uint32_t var=0;
    *((volatile uint32_t *) SPI_COMMAND) = register_00;

    do
    {
        var =(*((volatile uint32_t *) SPI_COMMAND) ) & (0x1 << 27);
    }while(var);

    return((uint16_t)*((volatile uint32_t *) SPI_RDATA));
}

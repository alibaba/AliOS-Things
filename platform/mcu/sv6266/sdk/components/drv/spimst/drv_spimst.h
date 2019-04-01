#ifndef SPIMST
#define SPIMST
#include "hal_spimst.h"


typedef enum {
    SPI_MST_CPHA_0          = 0,
    SPI_MST_CPHA_1          = 1
} ENUM_SPI_MST_CPHA_T;

typedef enum {
    SPI_MST_CPOL_0          = 0,
    SPI_MST_CPOL_1          = 1
} ENUM_SPI_MST_CPOL_T;


/**
 * Initilize SPI bus to enable SPI operations.
 * 
 * @param   clk_Hz     SPI Clock (BUS 40MHz : 1M - 20M, BUS 80MHz : 1M - 40M)
 *              
 * @param   CPHA       Serial Clock Phase. 
 *              0x00 - Serial clock toggles in middle of first data bit
 *              0x01 - Serial clock toggles at start of first data bit
 * @param   CPOL       Serial Clock Polarity. 
 *              0x00 - Inactive state of serial clock is low
 *              0x01 - Inactive state of serial clock is high
 *
 * @return  -1              The operation error.
 * @return   0              The operation completed successfully.
 */
int8_t drv_spi_mst_init(uint32_t clk_Hz, ENUM_SPI_MST_CPHA_T CPHA, ENUM_SPI_MST_CPOL_T CPOL);



/**
 * DDeinitialize SPI bus to disable SPI operations
 * 
 * @return  -1              The operation error.
 * @return   0              The operation completed successfully.
 */
int8_t drv_spi_mst_deinit(void);


/**
 * Transmit and receive the spi data by dma.
 * 
 * @param   write_data      Pointer to the data buffer to be written by the master.
 * @param   read_data       Pointer to the data buffer to be read by the master.
 * @param   length          Size of the data buffer exposed to the SPI master - max dma length : 1024 byte.
 *
 * @return  -1              The operation error.
 * @return   0              The operation completed successfully.
 */
int8_t drv_spi_mst_dma_trx(uint8_t *write_data, uint8_t *read_data, uint32_t length);


/**
 * Transmit and receive the spi data.
 * 
 * @param   write_data      Pointer to the data buffer to be written by the master.
 * @param   read_data       Pointer to the data buffer to be read by the master.
 * @param   length          Size of the data buffer exposed to the SPI master.
 * @param   csn               Chip select pin.
 *
 * @return  -1              The operation error.
 * @return   0              The operation completed successfully.
 */
int8_t drv_spi_mst_trx(uint8_t *write_data, uint8_t *read_data, uint32_t length, uint32_t csn);


#endif /* end of include guard */

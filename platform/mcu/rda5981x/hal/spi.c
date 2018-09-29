#include "hal/soc/soc.h"
#include "spi_api.h"
#include "rda5981x_pinconfig.h"

#define SPI_NUM 1

static spi_t spi_obj[SPI_NUM];

int32_t hal_spi_init(spi_dev_t *spi)
{
    if (spi->port >= SPI_NUM)
        return -1;

    spi->priv = &spi_obj[spi->port];
    spi_init(spi->priv, RDA_SPI0_MOSI, RDA_SPI0_MISO, RDA_SPI0_CLK, RDA_SPI0_CS);
    spi_format(spi->priv, 8, spi->config.mode, 0);
    spi_frequency(spi->priv, spi->config.freq);

    return 0;
}

int32_t hal_spi_send(spi_dev_t *spi, const uint8_t *data, uint16_t size, uint32_t timeout)
{
    if (NULL == data)
        return -1;
    
    while (size > 0) {
        spi_master_write(spi->priv, *data);
        data++;
        size--;
    }

    return 0;
}

int32_t hal_spi_recv(spi_dev_t *spi, uint8_t *data, uint16_t size, uint32_t timeout)
{
    uint8_t val;

    if (NULL == data)
        return -1;

    while (size > 0) {
        val = (uint8_t)spi_master_write(spi->priv, 0xFF);
        *data = val;
        data++;
        size--;
    }

    return 0;
}

int32_t hal_spi_send_recv(spi_dev_t *spi, uint8_t *tx_data, uint8_t *rx_data,
                      uint16_t size, uint32_t timeout)
{
#if 0
    uint8_t val;
    uint16_t rx_size = size, tx_size = size;

    if ((NULL == tx_data) || (NULL == rx_data))
        return -1;
    
    while (tx_size > 0) {
        val = (uint8_t)spi_master_write(spi->priv, *tx_data);
        tx_data++;
        tx_size--;
        if (rx_size > 0) {
            *rx_data = val;
            rx_data++;
            rx_size--;
        }
    }
    
    while (rx_size > 0) {
        val = val = (uint8_t)spi_master_write(spi->priv, 0xFF);
        *rx_data = val;
        rx_data++;
        rx_size--;
    }
#endif
    return 0;
}

int32_t hal_spi_finalize(spi_dev_t *spi)
{
    return 0;
}


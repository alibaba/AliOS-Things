#include <stdio.h>
#include <time.h>
#include "lega_cm4.h"
#include "lega_common.h"
#include "lega_spi.h"
#include "aos/hal/spi.h"


int32_t hal_spi_init(spi_dev_t *spi)
{
    lega_spi_dev_t * plega_spi = NULL;
    plega_spi = malloc(sizeof(lega_spi_dev_t));
    if(NULL == plega_spi){
        return -1;
    }
    spi->priv = plega_spi;
    plega_spi->port = spi->port;
    plega_spi->config.freq = spi->config.freq;
    plega_spi->config.mode = 0; //CPOL = CPHA = 0

    return lega_spi_init(plega_spi);
}

int32_t hal_spi_send(spi_dev_t *spi, const uint8_t *data, uint16_t size, uint32_t timeout)
{
    return lega_spi_send(spi->priv, data, size, timeout);
}

int32_t hal_spi_recv(spi_dev_t *spi, uint8_t *data, uint16_t size, uint32_t timeout)
{
    return lega_spi_recv(spi->priv, data, size, timeout);
}

int32_t hal_spi_send_recv(spi_dev_t *spi, uint8_t *tx_data, uint8_t *rx_data,
                          uint16_t size, uint32_t timeout)
{
    return lega_spi_send_recv(spi->priv, tx_data, rx_data, size, timeout);
}

int32_t hal_spi_finalize(spi_dev_t *spi)
{
    int32_t ret;
    ret = lega_spi_finalize(spi->priv);
    free(spi->priv);

    return ret;
}

#include <stdlib.h>
#include <errno.h>
#include <driver/spi_common.h>
#include <driver/spi_master.h>
#include <aos_hal_spi_internal.h>

typedef struct {
    spi_device_handle_t dev_handle;
} spi_pdata_t;

int32_t aos_hal_spi_init(aos_hal_spi_dev_t *spi)
{
    spi_host_device_t host_id;
    spi_bus_config_t bus_config = {
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 1024,
        .flags = SPICOMMON_BUSFLAG_MASTER,
        .intr_flags = 0,
    };
    spi_device_interface_config_t dev_config = {
        .command_bits = 0,
        .address_bits = 0,
        .dummy_bits = 0,
        .duty_cycle_pos = 0,
        .cs_ena_pretrans = 0,
        .cs_ena_posttrans = 0,
        .input_delay_ns = 0,
        .flags = SPI_DEVICE_HALFDUPLEX,
        .queue_size = 1,
        .pre_cb = NULL,
        .post_cb = NULL,
    };

    if (!spi)
        return -EINVAL;

    switch (spi->port) {
    case 3:
        host_id = SPI3_HOST;
        bus_config.mosi_io_num = 23;
        bus_config.sclk_io_num = 18;
#ifdef BOARD_M5STACKCORE2
        bus_config.miso_io_num = 38;
        dev_config.spics_io_num = 25;
#else
        bus_config.miso_io_num = 19;
        dev_config.spics_io_num = 5;
#endif
        break;
    default:
        return -EINVAL;
    }

    switch (spi->config.role) {
    case SPI_ROLE_MASTER:
        break;
    default:
        return -EINVAL;
    }

    switch (spi->config.firstbit) {
    case SPI_FIRSTBIT_MSB:
        break;
    case SPI_FIRSTBIT_LSB:
        dev_config.flags |= SPI_DEVICE_BIT_LSBFIRST;
        break;
    default:
        return -EINVAL;
    }

    switch (spi->config.mode) {
    case SPI_WORK_MODE_0:
        dev_config.mode = 0;
        break;
    case SPI_WORK_MODE_1:
        dev_config.mode = 1;
        break;
    case SPI_WORK_MODE_2:
        dev_config.mode = 2;
        break;
    case SPI_WORK_MODE_3:
        dev_config.mode = 3;
        break;
    default:
        return -EINVAL;
    }

    switch (spi->config.t_mode) {
    case SPI_TRANSFER_DMA:
        break;
    case SPI_TRANSFER_NORMAL:
        break;
    default:
        return -EINVAL;
    }

    dev_config.clock_speed_hz = spi->config.freq;

    switch (spi->config.data_size) {
    case SPI_DATA_SIZE_8BIT:
        break;
    default:
        return -EINVAL;
    }

    switch (spi->config.cs) {
    case SPI_CS_DIS:
        break;
    case SPI_CS_EN:
        break;
    default:
        return -EINVAL;
    }

    spi->priv = malloc(sizeof(spi_pdata_t));
    if (!spi->priv)
        return -ENOMEM;

    /* Change SPI_DMA_CH_AUTO to 0 as no supported by IDF-V4.2 */
    if (spi_bus_initialize(host_id, &bus_config, 0) != ESP_OK) {
        free(spi->priv);
        spi->priv = NULL;
        return -EINVAL;
    }

    if (spi_bus_add_device(host_id, &dev_config, &((spi_pdata_t *)spi->priv)->dev_handle) != ESP_OK) {
        (void)spi_bus_free(host_id);
        free(spi->priv);
        spi->priv = NULL;
        return -EINVAL;
    }

    return 0;
}

int32_t aos_hal_spi_finalize(aos_hal_spi_dev_t *spi)
{
    spi_host_device_t host_id;

    if (!spi || !spi->priv)
        return -EINVAL;

    switch (spi->port) {
    case 3:
        host_id = SPI3_HOST;
        break;
    default:
        return -EINVAL;
    }

    (void)spi_bus_remove_device(((spi_pdata_t *)spi->priv)->dev_handle);
    (void)spi_bus_free(host_id);
    free(spi->priv);
    spi->priv = NULL;

    return 0;
}

int32_t aos_hal_spi_send(aos_hal_spi_dev_t *spi, const uint8_t *data, uint32_t size, uint32_t timeout)
{
    spi_transaction_t trans_desc = {
        .flags = 0,
        .cmd = 0,
        .addr = 0,
        .rxlength = 0,
        .user = NULL,
        .tx_data = { 0, 0, 0, 0, },
        .rx_buffer = NULL,
        .rx_data = { 0, 0, 0, 0, },
    };

    if (!spi || !spi->priv || !data || size == 0)
        return -EINVAL;

    trans_desc.length = size * 8;
    trans_desc.tx_buffer = data;

    if (spi_device_transmit(((spi_pdata_t *)spi->priv)->dev_handle, &trans_desc) != ESP_OK)
        return -EIO;

    return 0;
}

int32_t aos_hal_spi_recv(aos_hal_spi_dev_t *spi, uint8_t *data, uint32_t size, uint32_t timeout)
{
    spi_transaction_t trans_desc = {
        .flags = 0,
        .cmd = 0,
        .addr = 0,
        .length = 0,
        .user = NULL,
        .tx_buffer = NULL,
        .tx_data = { 0, 0, 0, 0, },
        .rx_data = { 0, 0, 0, 0, },
    };

    if (!spi || !spi->priv || !data || size == 0)
        return -EINVAL;

    trans_desc.rxlength = size * 8;
    trans_desc.rx_buffer = data;

    if (spi_device_transmit(((spi_pdata_t *)spi->priv)->dev_handle, &trans_desc) != ESP_OK)
        return -EIO;

    return 0;
}

int32_t aos_hal_spi_send_recv(aos_hal_spi_dev_t *spi, uint8_t *tx_data, uint8_t *rx_data,
                              uint32_t rx_size, uint32_t timeout)
{
    spi_transaction_t trans_desc = {
        .flags = 0,
        .cmd = 0,
        .addr = 0,
        .user = NULL,
        .tx_data = { 0, 0, 0, 0, },
        .rx_data = { 0, 0, 0, 0, },
    };

    if (!spi || !spi->priv || !tx_data || !rx_data || rx_size == 0)
        return -EINVAL;

    trans_desc.length = 1 * 8;
    trans_desc.tx_buffer = tx_data;
    trans_desc.rxlength = rx_size * 8;
    trans_desc.rx_buffer = rx_data;

    if (spi_device_transmit(((spi_pdata_t *)spi->priv)->dev_handle, &trans_desc) != ESP_OK)
        return -EIO;

    return 0;
}

int32_t aos_hal_spi_send_and_recv(aos_hal_spi_dev_t *spi, uint8_t *tx_data, uint16_t tx_size, uint8_t *rx_data,
                                  uint16_t rx_size, uint32_t timeout)
{
    return -ENOTSUP;
}

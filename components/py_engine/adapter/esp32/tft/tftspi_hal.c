
#include "tftspi_hal.h"
#include "soc/spi_reg.h"
#include "esp_log.h"
#include "freertos/task.h"
#include "stdatomic.h"
#include "driver/gpio.h"
#include "string.h"
#include "stdatomic.h"

// SPI bus configuration is used by generic SPI, Display and SDCard SPI
#if CONFIG_SPIRAM_SPEED_80M
static spi_bus_config_t HSPI_buscfg = {-1, -1, -1, -1, -1, 0, 0, 0};
spi_bus_config_t *SPIbus_configs[3] = {NULL, &HSPI_buscfg, NULL, 0};
#else
static spi_bus_config_t HSPI_buscfg = {-1, -1, -1, -1, -1, 0, 0, 0};
static spi_bus_config_t VSPI_buscfg = {-1, -1, -1, -1, -1, 0, 0, 0};
spi_bus_config_t *SPIbus_configs[3] = {NULL, &HSPI_buscfg, &VSPI_buscfg};
#endif

QueueHandle_t spi_utils_mutex = NULL;

// static const char TAG[] = "[SPI_UTILS]";
void transfer_test(exspi_device_handle_t *spidev);

void __attribute__((weak)) IRAM_ATTR tft_spi_post_cb(spi_transaction_t *trans)
{
}

void IRAM_ATTR spi_post_cb(spi_transaction_t *trans)
{
    tft_spi_post_cb(trans);
}

esp_err_t spi_device_init(exspi_device_handle_t *spidev)
{
    esp_err_t ret;
    spidev->devcfg.post_cb = spi_post_cb;
    ret = spi_bus_initialize(spidev->spihost, spidev->buscfg, spidev->dma_channel);
    ret |= spi_bus_add_device(spidev->spihost, &spidev->devcfg, &spidev->handle);
    if (spi_lock == NULL)
    {
        spi_lock = xSemaphoreCreateMutex();
    }
    return ret;
}

//-------------------------------------------------------------------
esp_err_t spi_device_select(exspi_device_handle_t *spidev, int force)
{
    if (spidev->handle == NULL)
        return ESP_ERR_INVALID_ARG;
    while (spidev->handle->host->hal.hw->cmd.usr)
        ;
    // printf("--deselect enter --\r\n");
    xSemaphoreTake(spi_lock, portMAX_DELAY);
    spi_device_acquire_bus(spidev->handle, portMAX_DELAY);
    // ESP_EARLY_LOGE("T", "enter");
    return ESP_OK;
}

//----------------------------------------------------------
esp_err_t spi_device_deselect(exspi_device_handle_t *spidev)
{
    if (spidev->handle == NULL)
        return ESP_ERR_INVALID_ARG;
    while (spidev->handle->host->hal.hw->cmd.usr)
        ;
    spi_device_release_bus(spidev->handle);
    xSemaphoreGive(spi_lock);
    // printf("--deselect exit --\r\n");
    // ESP_EARLY_LOGE("T", "exit");
    return ESP_OK;
}

esp_err_t spi_device_deselect_isr(exspi_device_handle_t *spidev)
{
    static BaseType_t xHigherPriorityTaskWoken;
    if (spidev->handle == NULL)
        return ESP_ERR_INVALID_ARG;
    while (spidev->handle->host->hal.hw->cmd.usr)
        ;
    spi_device_release_bus(spidev->handle);
    xSemaphoreGiveFromISR(spi_lock, &xHigherPriorityTaskWoken);
    // printf("-- exit --\r\n");
    // ESP_EARLY_LOGE("T", "exit");
    return ESP_OK;
}

void IRAM_ATTR spi_transfer_cmd(exspi_device_handle_t *spidev, const uint8_t cmd)
{
    while (spidev->handle->host->hal.hw->cmd.usr)
        ; // Wait for SPI bus ready

    spidev->handle->host->hal.hw->data_buf[0] = (uint32_t)cmd;
    gpio_set_level(spidev->dc, 0);
    _spi_transfer_start(spidev, 8, 0);
}

void IRAM_ATTR spi_transfer_cmd_data(exspi_device_handle_t *spidev, const uint8_t cmd, uint8_t *data, int len)
{
    while (spidev->handle->host->hal.hw->cmd.usr)
        ; // Wait for SPI bus ready
    gpio_set_level(spidev->dc, 0);

    spidev->handle->host->hal.hw->data_buf[0] = (uint32_t)cmd;
    _spi_transfer_start(spidev, 8, 0);

    while (spidev->handle->host->hal.hw->cmd.usr)
        ; // Wait for SPI bus ready
    gpio_set_level(spidev->dc, 1);

    uint8_t idx = 0, bidx = 0;
    uint32_t bits = 0;
    uint32_t count = 0;
    uint32_t wd = 0;
    while (count < len)
    {
        // get data byte from buffer, fill spi buffer
        wd |= (uint32_t)data[count] << bidx;
        count++;
        bits += 8;
        bidx += 8;
        if (count == len)
        {
            while (spidev->handle->host->hal.hw->cmd.usr)
            {
            }; // Wait for SPI bus ready
            spidev->handle->host->hal.hw->data_buf[idx] = wd;
            break;
        }

        if (bidx == 32)
        {
            while (spidev->handle->host->hal.hw->cmd.usr)
            {
            }; // Wait for SPI bus ready
            spidev->handle->host->hal.hw->data_buf[idx] = wd;
            idx++;
            bidx = 0;
            wd = 0;
        }

        if (idx == 16)
        {
            // SPI buffer full, send data
            _spi_transfer_start(spidev, bits, 0);
            bits = 0;
            idx = 0;
            bidx = 0;
        }
    }
    if (bits > 0)
        _spi_transfer_start(spidev, bits, 0);
}

void IRAM_ATTR spi_transfer_data(exspi_device_handle_t *spidev, uint8_t *data, int len)
{
    while (spidev->handle->host->hal.hw->cmd.usr)
        ; // Wait for SPI bus ready

    gpio_set_level(spidev->dc, 1);

    uint8_t idx = 0, bidx = 0;
    uint32_t bits = 0;
    uint32_t count = 0;
    uint32_t wd = 0;
    while (count < len)
    {
        // get data byte from buffer, fill spi buffer
        wd |= (uint32_t)data[count] << bidx;
        count++;
        bits += 8;
        bidx += 8;
        if (count == len)
        {
            while (spidev->handle->host->hal.hw->cmd.usr)
            {
            }; // Wait for SPI bus ready
            spidev->handle->host->hal.hw->data_buf[idx] = wd;
            break;
        }

        if (bidx == 32)
        {
            while (spidev->handle->host->hal.hw->cmd.usr)
            {
            }; // Wait for SPI bus ready
            spidev->handle->host->hal.hw->data_buf[idx] = wd;
            idx++;
            bidx = 0;
            wd = 0;
        }

        if (idx == 16)
        {
            // SPI buffer full, send data
            _spi_transfer_start(spidev, bits, 0);
            bits = 0;
            idx = 0;
            bidx = 0;
        }
    }
    if (bits > 0)
        _spi_transfer_start(spidev, bits, 0);
}

// -----------------------------
// Direct (no DMA) data transfer
//----------------------------------------------------------------------------------------
esp_err_t spi_transfer_data_nodma(exspi_device_handle_t *spidev, spi_transaction_t *trans)
{
    if (!spidev->handle)
        return ESP_ERR_INVALID_ARG;
    gpio_set_level(spidev->dc, 1);
    esp_err_t ret = spi_device_polling_transmit(spidev->handle, trans);
    return ret;
}

//---------------------------------------------------
uint32_t spi_get_speed(exspi_device_handle_t *spidev)
{
    if (spidev->handle == NULL)
        return 0;
    return 0xffffffff;
}

//-------------------------------------------------------------------
uint32_t spi_set_speed(exspi_device_handle_t *spidev, uint32_t speed)
{
    if (spidev->handle == NULL)
        return 0;
    return ESP_OK;
}

// Start hardware SPI data transfer, spi device must be selected
// No address or command phase is executed
//----------------------------------------------------------------------------------------
void IRAM_ATTR _spi_transfer_start(exspi_device_handle_t *spidev, int wrbits, int rdbits)
{
    if (spidev->handle == NULL)
        return;
    while (spidev->handle->host->hal.hw->cmd.usr)
        ; // Wait for SPI bus ready
          // Wait for SPI bus ready
    spidev->handle->host->hal.hw->user1.usr_addr_bitlen = 0;
    spidev->handle->host->hal.hw->user2.usr_command_bitlen = 0;
    spidev->handle->host->hal.hw->user.usr_addr = 0;
    spidev->handle->host->hal.hw->user.usr_command = 0;
    spidev->handle->host->hal.hw->user.usr_mosi_highpart = 0;

    // tx enable
    spidev->handle->host->hal.hw->mosi_dlen.usr_mosi_dbitlen = wrbits - 1;
    spidev->handle->host->hal.hw->user.usr_mosi = 1;

    // rx disable
    spidev->handle->host->hal.hw->miso_dlen.usr_miso_dbitlen = 0;
    spidev->handle->host->hal.hw->user.usr_miso = 0;

    spidev->handle->host->hal.hw->user.usr_miso_highpart = 0;
    // Start transfer
    spidev->handle->host->hal.hw->cmd.usr = 1;
}

//-----------------------------------------------------------------------------------
void IRAM_ATTR _dma_send(exspi_device_handle_t *spidev, uint8_t *data, uint32_t size)
{
    if (spidev->handle == NULL)
        return;
    while (spidev->handle->host->hal.hw->cmd.usr)
        ; // Wait for SPI bus ready

    gpio_set_level(spidev->dc, 1);

    spicommon_dmaworkaround_idle(spidev->dma_channel);

    // Reset DMA
    spidev->handle->host->hal.hw->dma_conf.val |= SPI_OUT_RST | SPI_IN_RST | SPI_AHBM_RST | SPI_AHBM_FIFO_RST;
    spidev->handle->host->hal.hw->dma_out_link.start = 0;
    spidev->handle->host->hal.hw->dma_in_link.start = 0;
    spidev->handle->host->hal.hw->dma_conf.val &= ~(SPI_OUT_RST | SPI_IN_RST | SPI_AHBM_RST | SPI_AHBM_FIFO_RST);
    spidev->handle->host->hal.hw->dma_conf.out_data_burst_en = 1;

    lldesc_setup_link(spidev->handle->host->hal.dmadesc_tx, data, size, false);
    spidev->handle->host->hal.hw->user.usr_addr = 0;
    spidev->handle->host->hal.hw->user.usr_command = 0;
    spidev->handle->host->hal.hw->user.usr_mosi_highpart = 0;
    spidev->handle->host->hal.hw->dma_out_link.addr = (int)(&spidev->handle->host->hal.dmadesc_tx[0]) & 0xFFFFF;
    spidev->handle->host->hal.hw->dma_out_link.start = 1;
    spidev->handle->host->hal.hw->user.usr_mosi_highpart = 0;

    spidev->handle->host->hal.hw->mosi_dlen.usr_mosi_dbitlen = (size * 8) - 1;
    spidev->handle->host->hal.hw->cmd.usr = 1;
}

//--------------------------------------------------------------------
esp_err_t IRAM_ATTR _wait_trans_finish(exspi_device_handle_t *spidev)
{
    if (spidev->handle == NULL)
        return ESP_OK;
    uint32_t err = 0;
    while (spidev->handle->host->hal.hw->cmd.usr)
        ; // Wait for SPI bus ready

    return err;
}

// static esp_err_t poll_busy(exspi_device_handle_t *spidev) {
//     uint8_t t_rx;
//     t_rx = 0xaa;
//     spi_transaction_t t = {
//         .tx_buffer = &t_rx,
//         .flags = SPI_TRANS_USE_RXDATA,  //data stored in rx_data
//         .length = 8,
//     };
//     esp_err_t ret;

//     t.rx_data[0] = 0;
//     ret = spi_device_polling_transmit(spidev->handle, &t);

//     return ret;
// }

// void test2(exspi_device_handle_t *spidev) {
//     poll_busy(spidev,)
//     uint8_t* buf =  (uint8_t *)malloc(20 * sizeof(uint8_t *));
//     memset(buf, 0xaa, 20);

//     uint8_t cmd[20] = {0x55, 0xaa, 0x55};
//     esp_err_t ret;
//     spi_transaction_t t;
//     memset(&t, 0, sizeof(t));       //Zero out the transaction
//     t.flags = 0;
//     t.rx_buffer = buf;
//     t.tx_buffer = cmd;
//     t.rxlength = 16 * 8;
//     t.length = 16 * 8;                     //Command is 8 bits
//     ret=spi_device_polling_transmit(spidev->handle, &t);  //Transmit!

//     assert(ret==ESP_OK);            //Should have had no issues.
//     ESP_LOG_BUFFER_HEX_LEVEL("SPI_TEST", buf, 16, ESP_LOG_INFO);
//     free(buf);
//     vTaskDelay(pdMS_TO_TICKS(1000));
// }

// void transfer_test(exspi_device_handle_t *spidev) {
//     uint8_t test[] = {0x11, 0xaa, 0x55};
//     spi_device_acquire_bus(spidev->handle, portMAX_DELAY);
//     spi_transfer_cmd_data(spidev, 0x01, test, 3);
//     _wait_trans_finish(spidev);
//     spi_device_release_bus(spidev->handle);

//     for (uint8_t i = 0; i < 1; i++) {
//         spi_device_acquire_bus(spidev->handle, portMAX_DELAY);
//         test2(spidev);
//         spi_device_release_bus(spidev->handle);

//     }
// }

#ifndef _DRIVER_TFT_SPI_HAL_H_
#define _DRIVER_TFT_SPI_HAL_H_

#include "driver/spi_master.h"
#include "freertos/queue.h"
#include "hal/spi_hal.h"
#include "driver/spi_common_internal.h"
#include "hal/spi_hal.h"
#include "hal/spi_ll.h"
#include "freertos/semphr.h"

typedef struct {
    spi_transaction_t   *trans;
    const uint32_t *buffer_to_send;   //equals to tx_data, if SPI_TRANS_USE_RXDATA is applied; otherwise if original buffer wasn't in DMA-capable memory, this gets the address of a temporary buffer that is;
                                //otherwise sets to the original buffer or NULL if no buffer is assigned.
    uint32_t *buffer_to_rcv;    // similar to buffer_to_send
} spi_trans_priv_t;

typedef struct spi_device_t spi_device_t;
static SemaphoreHandle_t spi_lock = NULL;

typedef struct {
    int id;
    spi_device_t* device[DEV_NUM_MAX];
    intr_handle_t intr;
    spi_hal_context_t  hal;
    spi_trans_priv_t cur_trans_buf;
    int cur_cs;     //current device doing transaction
    const spi_bus_attr_t* bus_attr;

    /**
     * the bus is permanently controlled by a device until `spi_bus_release_bus`` is called. Otherwise
     * the acquiring of SPI bus will be freed when `spi_device_polling_end` is called.
     */
    spi_device_t* device_acquiring_lock;

//debug information
    bool polling;   //in process of a polling, avoid of queue new transactions into ISR
} spi_host_t;

struct spi_device_t {
    int id;
    QueueHandle_t trans_queue;
    QueueHandle_t ret_queue;
    spi_device_interface_config_t cfg;
    spi_hal_timing_conf_t timing_conf;
    spi_host_t *host;

    spi_bus_lock_dev_handle_t dev_lock;
};

typedef struct {
    spi_device_handle_t           handle;
    int8_t                        cs;
    int8_t                        dc;
    uint8_t                       selected;
    uint8_t                       spihost;
    uint8_t                       dma_channel;
    uint32_t                      curr_clock;
    spi_bus_config_t              *buscfg;
    spi_device_interface_config_t devcfg;
} exspi_device_handle_t;



#define SPI_SEMAPHORE_WAIT  2000    // Time in ms to wait for SPI mutex
#define SDSPI_HOST_ID       -2      // sdspi ID
#define TOTAL_CS            (NO_CS*2)

extern spi_bus_config_t *SPIbus_configs[3];
extern QueueHandle_t spi_utils_mutex;

esp_err_t spi_device_init(exspi_device_handle_t *spidev);

esp_err_t spi_device_select(exspi_device_handle_t *spidev, int force);

esp_err_t spi_device_deselect(exspi_device_handle_t *spidev);

void spi_transfer_cmd(exspi_device_handle_t *spidev, const uint8_t cmd);

void spi_transfer_cmd_data(exspi_device_handle_t *spidev, const uint8_t cmd, uint8_t* data, int len);

void spi_transfer_data(exspi_device_handle_t *spidev, uint8_t* data, int len);

esp_err_t spi_transfer_data_nodma(exspi_device_handle_t *spidev, spi_transaction_t *trans);

uint32_t spi_get_speed(exspi_device_handle_t *spidev);

uint32_t spi_set_speed(exspi_device_handle_t *spidev, uint32_t speed);

void _spi_transfer_start(exspi_device_handle_t *spi_dev, int wrbits, int rdbits);

void _dma_send(exspi_device_handle_t *spi_dev, uint8_t *data, uint32_t size);

esp_err_t _wait_trans_finish(exspi_device_handle_t *spi_dev);


#endif

#include "spi_interface.h"
#include "esp8266/eagle_soc.h"
#include "esp8266/ets_sys.h"
#include "esp8266/pin_mux_register.h"
#include "esp_libc.h"
#include "aos/hal/spi.h"

#ifdef __cplusplus
extern "C"
{
#endif
extern SPI_MAPPING SPI_MAPPING_TABLE[PORT_SPI_SIZE];
static SPI_MAPPING* GetSPIMapping(const PORT_SPI_TYPE port)
{
    int8_t i = 0;
    SPI_MAPPING* rc = NULL;
    for(i=0; i<PORT_SPI_SIZE; i++)
    {
        if(SPI_MAPPING_TABLE[i].spiFuncP == port)
        {
            rc = &SPI_MAPPING_TABLE[i];
            break;
        }
    }
    return rc;
}

static int32_t spi_freq_transform(uint32_t freq_hal, uint32_t *spi_speed_num)
{
    int32_t ret = 0;
    //uint32_t plck=0;
    uint32_t freq_ratio= freq_hal;

    //plck = CPU_CLK_FREQ;
    if(freq_ratio<=2 * 1000 * 1000)
        *spi_speed_num = SpiSpeed_2MHz;
    else if(freq_ratio<=5 * 1000 * 1000)
        *spi_speed_num = SpiSpeed_5MHz;
    else if(freq_ratio<=10 * 1000 * 1000)
        *spi_speed_num = SpiSpeed_10MHz;
    else if(freq_ratio<=16 * 1000 * 1000)
        *spi_speed_num = SpiSpeed_16MHz;
    else if(freq_ratio<=20 * 1000 * 1000)
        *spi_speed_num = SpiSpeed_20MHz;
    else
        *spi_speed_num = SpiSpeed_20MHz;

    return ret;
}

int32_t hal_spi_init(spi_dev_t *spi)
{
    SpiNum spiNum;
    SpiAttr Attr;
    uint32_t spi_speed_num;
    SPI_MAPPING* spi_mapping;

    spi_mapping = GetSPIMapping(spi->port);
    if(NULL == spi_mapping){
        return -1;
    }

    spi_freq_transform(spi->config.freq, &spi_speed_num);

    spiNum = spi_mapping->spiPhyP;
    Attr.mode = spi->config.mode == HAL_SPI_MODE_MASTER ? SpiMode_Master : SpiMode_Slave;
    Attr.subMode = spi_mapping->subMode;
    Attr.speed = spi_speed_num;
    Attr.bitOrder = spi_mapping->bitOrder;

    SPIInit(spiNum, &Attr);
}

int32_t hal_spi_send(spi_dev_t *spi, const uint8_t *data, uint16_t size, uint32_t timeout)
{
    SpiData InData = {0};
    SPI_MAPPING* spi_mapping;
    SpiNum spiNum;
    (void)timeout;

    spi_mapping = GetSPIMapping(spi->port);
    if(NULL == spi_mapping){
        return -1;
    }
    spiNum = spi_mapping->spiPhyP;

    InData.data = data;
    InData.dataLen = size;

    return SPIMasterSendData(spiNum, &InData);
}

int32_t hal_spi_recv(spi_dev_t *spi, uint8_t *data, uint16_t size, uint32_t timeout)
{
    SpiData OutData = {0};
    SPI_MAPPING* spi_mapping;
    SpiNum spiNum;
    (void)timeout;

    spi_mapping = GetSPIMapping(spi->port);
    if(NULL == spi_mapping){
        return -1;
    }
    spiNum = spi_mapping->spiPhyP;

    OutData.data = data;
    OutData.dataLen = size;

    return SPIMasterRecvData(spiNum, &OutData);
}

int32_t hal_spi_send_recv(spi_dev_t *spi, uint8_t *tx_data, uint8_t *rx_data,
                          uint16_t size, uint32_t timeout)
{
    return -1;
}

int32_t hal_spi_finalize(spi_dev_t *spi)
{
    return -1;
}

#ifdef __cplusplus
}
#endif

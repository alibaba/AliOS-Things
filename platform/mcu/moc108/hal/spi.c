#include <stdio.h>
#include <aos/aos.h>
#include "hal/soc/soc.h"
#include "hal_peripherals.h"
#include "hal/soc/spi.h"

typedef enum 
{
    GPIO0 = 0,
    GPIO1,
    GPIO2,
    GPIO3,
    GPIO4,
    GPIO5,
    GPIO6,
    GPIO7,
    GPIO8,
    GPIO9,
    GPIO10,
    GPIO11,
    GPIO12,
    GPIO13,
    GPIO14,
    GPIO15,
    GPIO16,
    GPIO17,
    GPIO18,
    GPIO19,
    GPIO20,
    GPIO21,
    GPIO22,
    GPIO23,
    GPIO24,
    GPIO25,
    GPIO26,
    GPIO27,
    GPIO28,
    GPIO29,
    GPIO30,
    GPIO31,
    GPIONUM
} GPIO_INDEX ;

void gpio_output(uint32_t id, uint32_t val);
void spi_master_init(const spi_dev_t* dev);
int SPI_Master_Tx(const spi_dev_t* dev, const spi_message_t* segments);
int SPI_Master_Rx(const spi_dev_t* dev, const spi_message_t* segments);
uint8_t SPI_Master_Tx_Rx_OneByte(uint8_t data);

bool hal_spi_inited = false;

int32_t hal_spi_init(spi_dev_t *spi)
{
    int32_t ret = 0;
    gpio_dev_t gpio = {6,OUTPUT_PUSH_PULL,NULL}; //[6]=15

    if (hal_spi_inited == false)
    {
        spi_master_init(spi);
        hal_spi_inited = true;
    }
    hal_gpio_init(&gpio);
    gpio_output(GPIO15,1);

    return ret;
}

int32_t hal_spi_send(spi_dev_t *spi, const uint8_t *data, uint16_t size, uint32_t timeout)
{
    int32_t ret = 0;
    if(data == NULL) return -1;

    gpio_output(GPIO15,0);
    spi_message_t segments = {data,NULL,size};

    ret = SPI_Master_Tx(spi,&segments);
  
    gpio_output(GPIO15,1);

    return ret;
}

int32_t hal_spi_recv(spi_dev_t *spi, uint8_t *data, uint16_t size, uint32_t timeout)
{
    int32_t ret;

    gpio_output(GPIO15,0);
    spi_message_t segments = {NULL,data,size};

    ret = SPI_Master_Rx(spi,&segments);

    gpio_output(GPIO15,1);
    return ret;
}

int32_t spi_send_recv(spi_dev_t *spi, spi_message_t* segments, uint16_t size, uint32_t timeout)
{
  int32_t ret = 0;
  int i;
  uint32_t count = 0;
  
  gpio_output(GPIO15,0);
  for (i = 0; i < size; i++)
  {
    count = segments[i].length;
    /* in interrupt-less mode */
    const uint8_t *send_ptr = (const uint8_t *)segments[i].tx_buffer;
    uint8_t *rcv_ptr = (uint8_t *)segments[i].rx_buffer;

    while (count--)
    {
      uint8_t data = 0xFF;

      if (send_ptr != NULL)
      {
        data = *send_ptr++;
      }

      data = SPI_Master_Tx_Rx_OneByte(data);

      if (rcv_ptr != NULL)
      {
        *rcv_ptr++ = (uint8_t)data;
      }
    }
  }
  gpio_output(GPIO15,1);
  return ret;
}

int32_t hal_spi_send_recv(spi_dev_t *spi, uint8_t *tx_data, uint8_t *rx_data,
	                      uint16_t size, uint32_t timeout)
{
  spi_message_t msg = {tx_data, rx_data, size};
  return spi_send_recv(spi, &msg, 1, timeout);
}

int32_t hal_spi_finalize(spi_dev_t *spi)
{
    int32_t ret = 0;
    hal_spi_inited = false;
    return ret;
}
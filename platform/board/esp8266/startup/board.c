#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "aos/hal/uart.h"
#include "network/hal/wifi.h"
#include "aos/init.h"
#include "k_config.h"
#include "board.h"
#include "hal_spi.h"
#include "spi_interface.h"

uart_dev_t uart_0 = { .port = 0,};

extern hal_wifi_module_t aos_wifi_esp8266;
extern char _bss_start;
extern char _bss_end;
extern int32_t hal_uart_init(uart_dev_t *uart);
extern void key_gpio_init(void);
extern int ets_printf(const char *fmt, ...);
extern void PendSV( char req );

SPI_MAPPING SPI_MAPPING_TABLE[PORT_SPI_SIZE] = {
{PORT_SPI_1, SpiNum_SPI, SpiSubMode_0, SpiBitOrder_MSBFirst},
{PORT_SPI_2, SpiNum_HSPI, SpiSubMode_0, SpiBitOrder_MSBFirst},
};

/**
  * @general board init entry board_basic_init
  * @retval None
  */
void board_basic_init(void)
{

}

/**
  * @general board tick init entry board_tick_init
  * @retval None
  */
void board_tick_init(void)
{

}

/**
* @init the default uart init example.
*/
void board_stduart_init(void)
{
    hal_uart_init(&uart_0);
}

/**
  * Enable DMA controller clock
  */
void board_dma_init(void)
{

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
void board_gpio_init(void)
{
    key_gpio_init();
}

/**
  * @brief WIFI Initialization Function
  * @param None
  * @retval None
  */
void board_wifi_init(void)
{
    int ret;
    hal_wifi_register_module(&aos_wifi_esp8266);
    ret = hal_wifi_init();
    if (ret){
        printf("waring: wifi init fail ret is %d \r\n", ret);
    }
}

void board_flash_init(void)
{
    flash_partition_init();
}

void board_network_init(void)
{
    board_wifi_init();
}

void board_kinit_init(kinit_t* init_args)
{
    return;
}

/****************************************************************************
* vproc_common.c - Hal functions for the VPROC API
*
*
****************************************************************************
* Copyright Microsemi Inc, 2018. All rights reserved.
* Licensed under the MIT License. See LICENSE.txt in the project
* root for license information.
*
***************************************************************************/

#include "vproc_common.h"
#include <stdio.h>
#include "driver/spi_master.h"
#include "soc/gpio_struct.h"
#include "driver/gpio.h"
#include "mbedtls/net.h"
#include "lwip/def.h"
#include "board.h"
#include "audio_idf_version.h"

static spi_device_handle_t g_spi = NULL;

int VprocHALInit(void)
{
    /*if the customer platform requires any init
    * then implement such init here.
    * Otherwise the implementation of this function is complete
    */
    esp_err_t ret = ESP_OK;

    spi_bus_config_t buscfg = {0};
    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 1000000,              // Clock out at 10 MHz
        .mode = 0,                              // SPI mode 0
        .queue_size = 6,                        //queue 7 transactions at a time
    };

    get_spi_pins(&buscfg, &devcfg);
    //Initialize the SPI bus
    if (g_spi) {
        return ret;
    }
#if (ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(4, 0, 0))
    ret = spi_bus_initialize(HSPI_HOST, &buscfg, 0);
    assert(ret == ESP_OK);
    ret = spi_bus_add_device(HSPI_HOST, &devcfg, &g_spi);
#else
    ret = spi_bus_initialize(SPI3_HOST, &buscfg, 0);
    assert(ret == ESP_OK);
    ret = spi_bus_add_device(SPI3_HOST, &devcfg, &g_spi);
#endif
    assert(ret == ESP_OK);
    gpio_set_pull_mode(0, GPIO_FLOATING);
    return ret;
}

/*HAL clean up function - To close any open file connection
* microsemi_spis_tw kernel char driver
*
* return: a positive integer value for success, a negative integer value for failure
*/

void VprocHALcleanup(void)
{
    /*if the customer platform requires any cleanup function
    * then implement such function here.
    * Otherwise the implementation of this function is complete
    */
    int ret = 0;
    ret = spi_bus_remove_device(g_spi);
    assert(ret == ESP_OK);
#if (ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(4, 0, 0))
    ret = spi_bus_free(HSPI_HOST);
#else
    ret = spi_bus_free(SPI3_HOST);
#endif

    assert(ret == ESP_OK);
}
/*Note - These functions are PLATFORM SPECIFIC- They must be modified
 *       accordingly
 **********************************************************************/

/* Vproc_msDelay(): use this function to
 *     force a delay of specified time in resolution of milli-second
 *
 * Input Argument: time in unsigned 16-bit
 * Return: none
 */

void Vproc_msDelay(unsigned short time)
{
    ets_delay_us(time * 1000);
}

/* VprocWait(): use this function to
*     force a delay of specified time in resolution of 125 micro-Seconds
*
* Input Argument: time in unsigned 32-bit
* Return: none
*/
void VprocWait(unsigned long int time)
{
    ets_delay_us(time * 1000);
}

#define BIGENDIAN 1

/* This is the platform dependent low level spi
 * function to write 16-bit data to the ZL380xx device
 */
int VprocHALWrite(unsigned short val)
{
    /*Note: Implement this as per your platform*/
    esp_err_t ret;
    spi_transaction_t t;
    unsigned short data = 0;
    memset(&t, 0, sizeof(t));              //Zero out the transaction
    t.length = sizeof(unsigned short) * 8; //Len is in bytes, transaction length is in bits.
#if BIGENDIAN
    data = htons(val);
    t.tx_buffer = &data;                   //Data
#else
    t.tx_buffer = &val;
#endif
    ret = spi_device_transmit(g_spi, &t); //Transmit
    assert(ret == ESP_OK);

    return 0;
}

/* This is the platform dependent low level spi
 * function to read 16-bit data from the ZL380xx device
 */
int VprocHALRead(unsigned short *pVal)
{
    /*Note: Implement this as per your platform*/
    esp_err_t ret;
    spi_transaction_t t;
    unsigned short data = 0;

    memset(&t, 0, sizeof(t));           //Zero out the transaction
    t.length = sizeof(unsigned short) * 8;
    t.rxlength = sizeof(unsigned short) * 8; //The unit of len is byte, and the unit of length is bit.
    t.rx_buffer = &data;
    ret = spi_device_transmit(g_spi, &t);   //Transmit!
#if BIGENDIAN
    *pVal = ntohs(data);
#else
    *pVal = data;
#endif
    assert(ret == ESP_OK);

    return 0;
}


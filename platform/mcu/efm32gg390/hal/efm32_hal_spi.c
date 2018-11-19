/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>
#include "hal/hal.h"
#include "BCDS_SpiDriver.h"
#include "em_gpio.h"
#include "em_cmu.h"
#include "BCDS_MCU_SPI_Handle.h"

#define SPI2_MISO_PIN                                                           (4)
#define SPI2_MISO_PORT                                                          (gpioPortB)

#define SPI2_MOSI_PIN                                                           (3)
#define SPI2_MOSI_PORT                                                          (gpioPortB)

#define SPI2_SCK_PIN                                                            (5)
#define SPI2_SCK_PORT                                                           (gpioPortB)
#define SPI2_CS_PIN                                                             (8)
#define SPI2_CS_PORT                                                            (gpioPortD)
#define SPI_DATA_TRANSFER_BAUDRATE              UINT32_C(200000) /**< Macro used to define the SPI Baudrate */
#define EFM32_HFXO_FREQ                         UINT32_C(48000000)


PeripheralDriver_T SPIInstance[3];

/* Init and deInit function for spi */
static int32_t spi_init(spi_dev_t *spi);
static int32_t spi_DeInit(spi_dev_t *spi);

/* function used to transform hal para to stm32l4 para */
static int32_t spi_mode_transform(uint32_t mode_hal, uint32_t *mode_stm32l4);
static int32_t spi_freq_transform(uint32_t freq_hal, uint32_t *BaudRatePrescaler_stm32l4_stm32l4);
bool spi2enable = false;
/* handle for spi */
struct MCU_SPI_Handle_S BSP_SPI2_Handle;



/*
 * Refer header BSP_IrqHandler.h for description
 */
void USART2_TX_IRQHandler(void)
{
    MCU_SPI_IRQ_TX_Callback((SPI_T) &BSP_SPI2_Handle);
}

/*
 * Refer header BSP_IrqHandler.h for description
 */
void USART2_RX_IRQHandler(void)
{
    MCU_SPI_IRQ_RX_Callback((SPI_T) &BSP_SPI2_Handle);
}


int32_t spi2_init(spi_dev_t *spi)
{

    int32_t ret = -1;

    if (spi == NULL) {
        return -1;
    }


    //BSP_SPI2_Handle.Link1 = (void*) &BSP_WiFi_CC3100MOD_SPI_TX_Channel;
    //BSP_SPI2_Handle.Link2 = (void*) &BSP_WiFi_CC3100MOD_SPI_RX_Channel;
    
    //BSP_WiFi_CC3100MOD_SPI_Handle.TransferMode = BCDS_HAL_TRANSFER_MODE_DMA;
    BSP_SPI2_Handle.TransferMode = BCDS_HAL_TRANSFER_MODE_INTERRUPT;

    BSP_SPI2_Handle.TxDefaultData = 0x00;
    BSP_SPI2_Handle.Instance = USART2;
    ret = SpiDriver_Initialize(&SPIInstance[2], &BSP_SPI2_Handle, NULL);
    if (0 != ret)
    {
        return -1;
    }
    //CMU_ClockEnable(cmuClock_USART1, true);
    CMU_ClockEnable(cmuClock_USART2, true);
    CMU_ClockEnable(cmuClock_GPIO, true);



    /* Setting baudrate */
     USART2->CLKDIV = 128 * (EFM32_HFXO_FREQ / SPI_DATA_TRANSFER_BAUDRATE - 2);
    
     /* Configure SPI */
     /* Using synchronous (SPI) mode*/
     USART2->CTRL = USART_CTRL_SYNC;
     /* Clearing old transfers/receptions, and disabling interrupts */
     USART2->CMD = USART_CMD_CLEARRX | USART_CMD_CLEARTX;
     USART2->IEN = 0;
     /* Enabling pins and setting location */
     USART2->ROUTE = USART_ROUTE_TXPEN | USART_ROUTE_RXPEN | USART_ROUTE_CLKPEN | USART_ROUTE_LOCATION_LOC1;
     
     
     /* Set to master and to control the CS line */

     /* Enabling Master, TX and RX */
     USART2->CMD   = USART_CMD_MASTEREN | USART_CMD_TXEN | USART_CMD_RXEN;
     USART2->CTRL |= USART_CTRL_AUTOCS;
       
     /* Clear previous interrupts */
     USART2->IFC = _USART_IFC_MASK;
     GPIO_PinModeSet(SPI2_CS_PORT, SPI2_CS_PIN, gpioModePushPull, 0);
     
     GPIO_PinModeSet(SPI2_MISO_PORT, SPI2_MISO_PIN, gpioModeInput, 0);
     GPIO_PinModeSet(SPI2_MOSI_PORT, SPI2_MOSI_PIN, gpioModePushPull, 0);
     GPIO_PinModeSet(SPI2_SCK_PORT, SPI2_SCK_PIN, gpioModePushPull, 0);


     /* Enable interrupts */
     USART2->CMD = USART_CMD_CLEARRX;
     NVIC_ClearPendingIRQ(USART2_RX_IRQn);
     NVIC_EnableIRQ(USART2_RX_IRQn);
     USART2->IEN |= USART_IEN_RXDATAV;
     USART2->CMD = USART_CMD_CLEARTX;
     NVIC_ClearPendingIRQ(USART2_TX_IRQn);
     NVIC_EnableIRQ(USART2_TX_IRQn);
     USART2->IEN |= USART_IEN_TXBL;

    USART2->CTRL |= USART_CTRL_MSBF;

    return 0;


    return ret;
}


int32_t hal_spi_init(spi_dev_t *spi)
{
    /*init spi handle*/
    int32_t ret = -1;
    switch (spi->port) {
        case 2:
            memset(&SPIInstance[2], 0, sizeof(SPIInstance[2]));
            spi->priv = &SPIInstance[2];
            ret = spi2_init(spi);
            break;

    /* if ohter spi exist add init code here */

        default:
            break;
    }

    return ret;
}
/*
static inline SPI_HandleTypeDef *get_priv(uint8_t port)
{
    SPI_HandleTypeDef *hspi = NULL;


    return hspi;
}
*/

int32_t hal_spi_send(spi_dev_t *spi, const uint8_t *data, uint16_t size, uint32_t timeout)
{
    int32_t ret = -1;
    ret = SpiDriver_Open(&SPIInstance[spi->port], NULL, timeout);
    if (RETCODE_OK == ret)
    {
        GPIO_PinOutClear(SPI2_CS_PORT, SPI2_CS_PIN);
    }
    if (RETCODE_OK == ret)
    {
        ret = SpiDriver_Write(&SPIInstance[spi->port], (uint8_t*) data, (uint32_t) size, timeout);
    }
    if (RETCODE_OK == ret)
    {
        GPIO_PinOutSet(SPI2_CS_PORT, SPI2_CS_PIN);
    }
    if (RETCODE_OK == ret)
    {
        ret = SpiDriver_Close(&SPIInstance[spi->port]);
    }

    return ret;
}

int32_t hal_spi_recv(spi_dev_t *spi, uint8_t *data, uint16_t size, uint32_t timeout)
{
    int32_t ret = -1;
    ret = SpiDriver_Open(&SPIInstance[spi->port], NULL, timeout);
    if (RETCODE_OK == ret)
    {
        GPIO_PinOutClear(SPI2_CS_PORT, SPI2_CS_PIN);
    }
    if (RETCODE_OK == ret)
    {
        ret = SpiDriver_Read(&SPIInstance[spi->port], (uint8_t*) data, (uint32_t) size, timeout);
    }
    if (RETCODE_OK == ret)
    {
        GPIO_PinOutSet(SPI2_CS_PORT, SPI2_CS_PIN);
    }
    if (RETCODE_OK == ret)
    {
        ret = SpiDriver_Close(&SPIInstance[spi->port]);
    }

    return ret;
}

int32_t hal_spi_send_recv(spi_dev_t *spi, uint8_t *tx_data, uint8_t *rx_data,
        uint16_t size, uint32_t timeout)
{
    int32_t ret = -1;
    ret = SpiDriver_Open(&SPIInstance[spi->port], NULL, 5000);
    if (RETCODE_OK == ret)
    {
        GPIO_PinOutClear(SPI2_CS_PORT, SPI2_CS_PIN);
    }

    if (RETCODE_OK == ret)
    {
        ret = SpiDriver_transfer(&SPIInstance[spi->port],(uint8_t*) tx_data,(uint8_t*) rx_data,(uint32_t) size, 5000);
        printf("SpiDriver_transfer ret=%d\n",ret);
    }

    if (RETCODE_OK == ret)
    {
        GPIO_PinOutSet(SPI2_CS_PORT, SPI2_CS_PIN);
    }
    if (RETCODE_OK == ret)
    {
        ret = SpiDriver_Close(&SPIInstance[spi->port]);
    }

    return ret;
}

int32_t hal_spi_finalize(spi_dev_t *spi)
{
    int32_t ret = -1;
    if(spi == NULL)
    {
        return -1;
    }
    ret = SpiDriver_Deinitialize(&SPIInstance[spi->port],5000);

    return ret;
}
#if 0
int32_t spi_init(spi_dev_t *spi)
{
    int32_t ret = -1;


    return ret;
}

int32_t spi_DeInit(spi_dev_t *spi)
{
    int32_t ret = -1;
    return ret;
}

static int32_t spi_mode_transform(uint32_t mode_hal, uint32_t *mode_stm32l4)
{
    uint32_t mode = 0;
    int32_t	ret = 0;


    return ret;
}

static int32_t spi_freq_transform(uint32_t freq_hal, uint32_t *BaudRatePrescaler_stm32l4)
{
    int32_t	ret = 0;

    /* calc BaudRatePrescaler according to freq_hal */

    return ret;
}
#endif

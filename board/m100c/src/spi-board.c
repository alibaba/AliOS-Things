/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "em_device.h"
#include "em_gpio.h"
#include "em_usart.h"
#include "em_cmu.h"
#include "spi-board.h"
#include "system/spi.h"

void SpiInit(Spi_t *obj, SpiId_t spiId, PinNames mosi, PinNames miso, PinNames sclk, PinNames nss)
{
    USART_InitSync_TypeDef      spiInit;
    uint32_t                    cpu_frequency;

    cpu_frequency = CMU_ClockFreqGet(cmuClock_HFPER);

    spiInit.enable       = usartDisable;                                       /* Don't enable UART upon intialization */
    spiInit.refFreq      = cpu_frequency;                                              /* Provide information on reference frequency. When set to 0, the reference frequency is */
    spiInit.baudrate     = 2000000;                                               /* Baud rate */
    spiInit.databits     = usartDatabits8;                                     /* Number of data bits. Range is 4 to 10 */
    spiInit.master       = true;
    spiInit.msbf         = true;
    spiInit.clockMode    = usartClockMode0;

#if defined( USART_INPUT_RXPRS ) && defined( USART_TRIGCTRL_AUTOTXTEN )
    spiInit.prsRxEnable        = false;
    spiInit.prsRxEnable        = usartPrsRxCh0;
    spiInit.autoTx             = false;
#endif

    USART_InitSync(USART0, &spiInit);

    USART0->ROUTE =USART_ROUTE_LOCATION_LOC0 | USART_ROUTE_TXPEN | USART_ROUTE_RXPEN | USART_ROUTE_CLKPEN;

    GpioInit(&obj->Sclk, sclk, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0);
    GpioInit(&obj->Miso, miso, PIN_INPUT, PIN_PUSH_PULL, PIN_PULL_UP, 1);
    GpioInit(&obj->Mosi, mosi, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0);
    GpioInit(&obj->Nss, nss, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 1);


    USART_Enable(USART0, usartEnable);
}

uint16_t SpiInOut(Spi_t *obj, uint16_t tx_data)
{
    USART_TypeDef *spi = USART0;

    return (uint16_t)USART_SpiTransfer(spi, (uint8_t)tx_data);
}

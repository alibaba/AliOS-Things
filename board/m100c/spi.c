/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "em_device.h"
#include "spi.h"
#include "em_gpio.h"
#include "em_usart.h"
#include "em_cmu.h"


void lora_spi_setup(void)
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

    GPIO_PinModeSet(gpioPortE, 10, gpioModePushPull, 0);  /* MOSI */
    GPIO_PinModeSet(gpioPortE, 11, gpioModeInput, 1);  /* MISO */
    GPIO_PinModeSet(gpioPortE, 13, gpioModePushPull,  1);  /* CS */
    GPIO_PinModeSet(gpioPortE, 12, gpioModePushPull,  0);  /* Clock */

    USART_Enable(USART0, usartEnable);
}

uint8_t lora_spi_inout(uint8_t tx_data)
{
    USART_TypeDef *spi = USART0;

    return USART_SpiTransfer(spi, tx_data);
}


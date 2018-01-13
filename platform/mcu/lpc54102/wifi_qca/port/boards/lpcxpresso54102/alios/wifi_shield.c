/*
 * Copyright (c) 2017, NXP Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of NXP Semiconductor, Inc. nor the names of its
 *   contributors may be used tom  endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdint.h>

#include "wifi_common.h"
#include "fsl_gpio.h"
#include "driver_cxt.h"
#include "wifi_spi.h"
#include "wifi_shield.h"
#include "fsl_pint.h"
#include "fsl_inputmux.h"
#include "fsl_inputmux_connections.h"

static void PINT_callback(pint_pin_int_t pintr, uint32_t pmatch_status);

/*!
 * @brief Low level initialization, RTOS does not have to run yet
 */
A_STATUS WIFISHIELD_Init(void)
{
    gpio_pin_config_t config;
    memset(&config, 0, sizeof(config));

    /* Initialize pinmux */
#ifdef WIFISHIELD_PINMUX_INIT
    WIFISHIELD_PINMUX_INIT();
#endif

    /* Enable clocks for GPIO */
    CLOCK_EnableClock(WIFISHIELD_WLAN_PWRON_CLOCKSRC);
    CLOCK_EnableClock(WIFISHIELD_WLAN_IRQ_CLOCKSRC);

    /* Enable clocks for SPI */
    CLOCK_AttachClk(WIFISHIELD_SPI_CLK_CONNECT);
    RESET_PeripheralReset(WIFISHIELD_SPI_IP_RESET);

    /* Set up WLAN_PWRON signal */
    config.pinDirection = (gpio_pin_direction_t)WIFISHIELD_WLAN_PWRON_DIRECTION;
    GPIO_PinInit(WIFISHIELD_WLAN_PWRON_GPIO, WIFISHIELD_WLAN_PWRON_PORT, WIFISHIELD_WLAN_PWRON_PIN, &config);

    /* Set up WLAN_IRQ signal */
    config.pinDirection = (gpio_pin_direction_t)WIFISHIELD_WLAN_IRQ_DIRECTION;
    GPIO_PinInit(WIFISHIELD_WLAN_IRQ_GPIO, WIFISHIELD_WLAN_IRQ_PORT, WIFISHIELD_WLAN_IRQ_PIN, &config);

    CLOCK_EnableClock(kCLOCK_Gpio1);
    config.pinDirection = kPIN_MUX_DirectionOutput;
    config.outputLogic = 1;
    GPIO_PinInit(GPIO, 1, 15, &config);

    /* Set up Inputmux */
    INPUTMUX_Init(INPUTMUX);
    INPUTMUX_AttachSignal(INPUTMUX, WIFISHIELD_WLAN_PINT, WIFISHIELD_WLAN_PINT_CONNECT);
    INPUTMUX_Deinit(INPUTMUX);

    /* Set up PINT for WLAN_IRQ */
    PINT_Init(PINT);
    PINT_PinInterruptConfig(PINT, WIFISHIELD_WLAN_PINT, WIFISHIELD_WLAN_PINT_EDGE, PINT_callback);
    PINT_EnableCallback(PINT);

    /* Enable NVIC interrupt for WLAN_IRQ */
    NVIC_EnableIRQ(WIFISHIELD_WLAN_PINT_IRQ);

    /* Set NVIC priority if is required by Freertos */
    NVIC_SetPriority(WIFISHIELD_WLAN_PINT_IRQ, WIFISHIELD_WLAN_IRQ_PRIORITY);

    return A_OK;
}

/*!
 * @brief Initialization is maintained by WIFI stack
 */
A_STATUS WIFISHIELD_InitDrivers(void *param)
{
    /* Set up capabilities of SPI transfer, used in upper layer */
    A_DRIVER_CONTEXT *pDCxt = GET_DRIVER_COMMON(param);

    /* Force WIFI stack to use this SPI settings */
    pDCxt->spi_hcd.PowerUpDelay = 1;
    pDCxt->spi_hcd.SpiHWCapabilitiesFlags = (HW_SPI_FRAME_WIDTH_8 | HW_SPI_NO_DMA | HW_SPI_INT_EDGE_DETECT);

    /* Complex DMAMUX/DMA/SPI config structure */
    WIFIDRVS_SPI_config_t spi_config = {0};

    /* Load default settings */
    WIFIDRVS_SPI_GetDefaultConfig(&spi_config);

#if defined(WIFISHIELD_DMA)
    /* Configure dma_mode */
    spi_config.dma_mode.enabled = true;
    spi_config.dma_mode.dma_base = (void*)WIFISHIELD_DMA;
    spi_config.dma_mode.dma_rx_chnl = WIFISHIELD_DMA_RX_CHNL;
    spi_config.dma_mode.dma_rx_chnl_prio = kDMA_ChannelPriority3;
    spi_config.dma_mode.dma_tx_chnl = WIFISHIELD_DMA_TX_CHNL;
    spi_config.dma_mode.dma_tx_chnl_prio = kDMA_ChannelPriority4;
    spi_config.dma_mode.dma_irq_prio = WIFISHIELD_DMA_IRQ_PRIORITY;
#endif

#if defined(WIFISHIELD_SPI_IRQ_PRIORITY)
    /* Configure irq_mode */
    spi_config.irq_mode.enabled = true;
    spi_config.irq_mode.spi_irq_prio = WIFISHIELD_SPI_IRQ_PRIORITY;
#endif

    /* Configure spi */
    spi_config.spi.base = (void*)WIFISHIELD_SPI;
    spi_config.spi.clk_hz = CLOCK_GetFreq(kCLOCK_Spi);
    spi_config.spi.baudrate = WIFISHIELD_SPI_BAUDRATE;
    spi_config.spi.irq_threshold = WIFISHIELD_SPI_THRESHOLD;

    /* Load recommended SPI settings */
    WIFIDRVS_SPI_GetSPIConfig(&spi_config.spi.config, WIFISHIELD_SPI_BAUDRATE, WIFISHIELD_SPI_INIT_CS);

    /* Initialize driver */
    A_STATUS result = WIFIDRVS_SPI_Init(&spi_config);
    assert(A_OK == result);

    return result;
}

/*!
 * @brief Deinitialization is maintained by WIFI stack
 */
A_STATUS WIFISHIELD_DeinitDrivers(void *param)
{
    // WIFI_Spi_Deinit();
    return A_OK;
}

/*!
 * @brief Power up WiFi shield, RTOS does not have to run yet
 */
A_STATUS WIFISHIELD_PowerUp(uint32_t enable)
{
    if (enable)
    {
        GPIO_WritePinOutput(WIFISHIELD_WLAN_PWRON_GPIO, WIFISHIELD_WLAN_PWRON_PORT, WIFISHIELD_WLAN_PWRON_PIN, 1);
    }
    else
    {
        GPIO_WritePinOutput(WIFISHIELD_WLAN_PWRON_GPIO, WIFISHIELD_WLAN_PWRON_PORT, WIFISHIELD_WLAN_PWRON_PIN, 0);
    }
    return A_OK;
}

/*!
 * @brief Fn post DriverTask semaphore, can be called only from WLAN_IRQ ISR
 */
void WIFISHIELD_NotifyDriverTask(void *param)
{
    extern QCA_CONTEXT_STRUCT wifiCtx;
    void HW_InterruptHandler(void *pCxt);

    void *pCxt = wifiCtx.MAC_CONTEXT_PTR;
    if (pCxt)
    {
        HW_InterruptHandler(pCxt);
    }
}

/* ISR callback for WLAN_IRQ */
static void PINT_callback(pint_pin_int_t pintr, uint32_t pmatch_status)
{
//    PRINTF("PINT CALLBACK \r\n");
    WIFISHIELD_NotifyDriverTask(NULL);
}


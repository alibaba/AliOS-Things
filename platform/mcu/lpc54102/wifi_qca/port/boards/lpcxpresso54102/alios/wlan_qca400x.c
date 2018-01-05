/**HEADER********************************************************************
*
* The Clear BSD License
* Copyright (c) 2008 Freescale Semiconductor;
* All Rights Reserved
*
***************************************************************************
* Redistribution and use in source and binary forms, with or without modification,
* are permitted (subject to the limitations in the disclaimer below) provided
* that the following conditions are met:
*
* o Redistributions of source code must retain the above copyright notice, this list
*   of conditions and the following disclaimer.
*
* o Redistributions in binary form must reproduce the above copyright notice, this
*   list of conditions and the following disclaimer in the documentation and/or
*   other materials provided with the distribution.
*
* o Neither the name of the copyright holder nor the names of its
*   contributors may be used to endorse or promote products derived from this
*   software without specific prior written permission.
*
* NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
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
*
**************************************************************************
*
* $FileName: wlan_qca400x.c$
* $Version : $
* $Date    : $
*
* Comments:
*
*   This file contains the function that reads the timer and returns
*   the number of nanoseconds elapsed since the last interrupt.
*
*END************************************************************************/

#include "board.h"

#include <atheros_wifi.h>
#include <common_api.h>
#include "qcom_api.h"

// ============================================================================
// Definitions
// ============================================================================


// ============================================================================
// Globals
// ============================================================================

// This is the structure passed on to the QCA WIFI driver.
// It contains primarily SPI hardware configuration and little else
QCA_CONTEXT_STRUCT wifiCtx;

// This is the WIFI function pointer struct defined in QCA driver file cust_api_init.c
extern const QCA_MAC_IF_STRUCT ATHEROS_WIFI_IF;

/// SPI hardware configuration that is used by cust_spi_hcd.c - the QCA SPI driver
// struct wlan_hardware_config_s wlanHardwareConfig = {
//    .spi_instance    = SPI_INSTANCE_NUMBER,
//    .spi_cs          = kDspiPcs0,
//    .wlan_irq_number = WLAN_IRQ_NUMBER,
//    .wlan_power_pin  = WLAN_CHIP_POWER_PIN,
//};

const QCA_IF_STRUCT ENET_0 = {
    .MAC_IF = &ATHEROS_WIFI_IF, .MAC_NUMBER = 0, .PHY_NUMBER = 0, .PHY_ADDRESS = 0,
};

const QCA_PARAM_STRUCT ENET_default_params = {
    .QCA_IF = &ENET_0,
    .MODE = Auto_Negotiate,
    .OPTIONS = 0,
    .NUM_RX_PCBS = WLAN_CONFIG_NUM_PRE_ALLOC_RX_BUFFERS, //  # Number of RX PCBs (Packet Control Blocks)

    // cust_spi_hcd.c - the QCA hardware specific SPI driver uses this config
    //    .MAC_PARAM         = &wlanHardwareConfig,
};

// ============================================================================
// WIFI Interrupt handler and Initialization
// ============================================================================

static int numIrqs = 0;
static int initTime = 0;

/** Initialize the QCA400x WLAN driver and start the associated driver task
 *  This function MUST be called from a task/thread context
 */
int wlan_driver_start(void)
{
    uint32_t result = A_OK;
    uint32_t time = 0;

    // Power off the WLAN and wait 30ms
    CUSTOM_HW_POWER_UP_DOWN(NULL, false);

    A_MDELAY(MSEC_TO_TICK(30));

    // Initialize WIFI driver by calling Custom_Api_Initialize() which
    // allocates memory for various structures and starts the driver RTOS task
    // represented by Atheros_Driver_Task() function.
    // The driver task establishes SPI communication with the QCA400x chip and sets
    // up various registers of the WLAN chip.
    // Note that this initialization involves the currently running task to wait
    // for the ready signal from the Atheros_Driver_Task() where upon we return
    // from the function call. This process takes approximately 60ms.
    time = A_TIME_GET_MSEC();
    wifiCtx.PARAM_PTR = &ENET_default_params;
    result = ATHEROS_WIFI_IF.INIT(&wifiCtx);
    if (A_OK != result)
    {
        PRINTF("WLAN driver initialization failed \r\n");
        return result;
    }

    // Print init time
    initTime = time = A_TIME_GET_MSEC() - time;
    PRINTF("WLAN driver initialized in %dms with %d irqs\r\n", time, numIrqs);

    // Amount of malloc'ed memory as counted by the QCA400x driver
    extern uint32_t g_totAlloc;
    PRINTF("WLAN driver malloc'ed %d bytes (not counting OS structures!)\r\n", g_totAlloc);

    // avoid SPI bus flood
    qcom_power_set_mode(0, MAX_PERF_POWER, USER);

    return result;
}

QCA_CONTEXT_STRUCT *wlan_get_context(void)
{
    return &wifiCtx;
}

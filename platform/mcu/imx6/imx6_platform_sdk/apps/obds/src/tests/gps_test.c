/*
 * Copyright (c) 2011-2012, Freescale Semiconductor, Inc.
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
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
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

#include "obds.h"
#include "io.h"
#include "registers/regsuart.h"

static uint32_t uart_instance = HW_UART2;

int gps_test_enable;

int gps_test(void)
{
    int result = 0;

    if (!gps_test_enable) {
        return TEST_NOT_PRESENT;
    }

    PROMPT_RUN_TEST("GPS", NULL);

    /* IOMUX */
//    writel(ALT3, IOMUXC_SW_MUX_CTL_PAD_PATA_BUFFER_EN); //uart2 rxd
//    writel(ALT3, IOMUXC_SW_MUX_CTL_PAD_PATA_DMARQ); //uart2 txd
//    writel(ALT3, IOMUXC_SW_MUX_CTL_PAD_PATA_INTRQ); //uart2 cts
//    writel(ALT3, IOMUXC_SW_MUX_CTL_PAD_PATA_DIOR);  //uart2 rts
//    writel(0x3, IOMUXC_UART2_IPP_UART_RXD_MUX_SELECT_INPUT);    //uart2 rxd daisy chain

//    init_uart(&uart2, 115200);

    uart2_iomux_config();
    uart_init(uart_instance, 115200, PARITY_NONE, STOPBITS_ONE, EIGHTBITS, FLOWCTRL_OFF);

   return result;
}

//RUN_TEST("GPS", gps_test)

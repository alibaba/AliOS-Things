/*
 * Copyright (c) 2009-2012, Freescale Semiconductor, Inc.
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

/*!
 * @defgroup diag_gpio_test
 */

/*!
 * @file  gpio_led.c
 *
 * @ingroup diag_gpio_test
 */

#include "obds.h"
#include "registers/regsgpio.h"
#include "registers/regsiomuxc.h"

const char g_gpio_led_test_name[] = "GPIO LED Test";

test_return_t gpio_led_test(void)
{
    unsigned char recvCh = NONE_CHAR;
    const char* indent = menu_get_indent();
    unsigned int bit, gpio_inst = 0, mux_val;

#if defined(BOARD_SABRE_AI)
    mux_val = HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA21_RD();               // save the original value
    HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA21_WR(
            BF_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA21_MUX_MODE_V(ALT5));   // GPIO5[15]
    gpio_inst = HW_GPIO5;
    bit = 15;
#elif defined(BOARD_EVB)
    mux_val = HW_IOMUXC_SW_MUX_CTL_PAD_EIM_DATA25_RD();                 // save the original value
    HW_IOMUXC_SW_MUX_CTL_PAD_EIM_DATA25_WR(
            BF_IOMUXC_SW_MUX_CTL_PAD_EIM_DATA25_MUX_MODE_V(ALT5));      // GPIO3[25]
    gpio_inst = HW_GPIO3;
    bit = 25;
#elif defined(BOARD_SMART_DEVICE)
    // user-def-red, gpio_2 pin
    mux_val = HW_IOMUXC_SW_MUX_CTL_PAD_GPIO02_RD();                     // save the original value
    HW_IOMUXC_SW_MUX_CTL_PAD_GPIO02_WR(
            BF_IOMUXC_SW_MUX_CTL_PAD_GPIO02_MUX_MODE_V(ALT5));          // GPIO1[2]
    gpio_inst = HW_GPIO1;
    bit = 2;
    // user-def-green, gpio_1 pin
#elif defined(CHIP_MX6SL) && defined(BOARD_EVK)
    mux_val = HW_IOMUXC_SW_MUX_CTL_PAD_USB_H_STROBE_RD();                // save the original value
    HW_IOMUXC_SW_MUX_CTL_PAD_USB_H_STROBE_WR(
            BF_IOMUXC_SW_MUX_CTL_PAD_USB_H_STROBE_MUX_MODE(ALT5));       // GPIO3[20]
    gpio_inst = HW_GPIO3;
    bit = 20;
#endif

    if ( gpio_inst == 0 )
        return TEST_NOT_IMPLEMENTED;

    gpio_set_direction(gpio_inst, bit, GPIO_GDIR_OUTPUT);

    printf("%sIs the USER LED blinking? [y/n]\n", indent);
    while (1)  
    {
        gpio_set_level(gpio_inst, bit, GPIO_HIGH_LEVEL);

        hal_delay_us(200000);

        gpio_set_level(gpio_inst, bit, GPIO_LOW_LEVEL);

        hal_delay_us(200000);
       
        recvCh = getchar();
        if ((recvCh == 'y') || (recvCh == 'Y') || (recvCh == 'n') || (recvCh == 'N'))
            break;
     }  

    // put back the original pin mux value
#if defined(BOARD_SABRE_AI)
    HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA21_WR(mux_val);
#elif defined(BOARD_EVB)
    HW_IOMUXC_SW_MUX_CTL_PAD_EIM_DATA25_WR(mux_val);
#elif defined(BOARD_SMART_DEVICE)
    HW_IOMUXC_SW_MUX_CTL_PAD_GPIO02_WR(mux_val);
#elif defined(CHIP_MX6SL) && defined(BOARD_EVK)
    HW_IOMUXC_SW_MUX_CTL_PAD_USB_H_STROBE_WR(mux_val);
#endif

    if (recvCh == 'y' || recvCh == 'Y')
        return TEST_PASSED;
    else
        return TEST_FAILED;
}

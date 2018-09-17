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

/*!
 * @file ds90ur124.c
 * @brief Driver and test for the deserializer DS90UR124 controlled through I2C.
 *
 * @ingroup diag_deserializer
 */

#include "obds.h"

extern void adv7180_set_gpio_output(unsigned int, unsigned int);
extern void adv7180_i2c_init_obds(void);
extern void adv7180_set_tristate_output(void);
extern void csi_port0_iomux_config(void);
void deserializer_io_config(void);

/* I/O config for the DS90UR124QVS - LVDS camera in */
void deserializer_io_config(void)
{
   // REN and RPWDB controlled by ADV7180 configured in the driver
#if defined (CHIP_MX6SL)
    csi_iomux_config();
#else
    csi_port0_iomux_config();
#endif
}

/*!
 * Init function of the deserializer DS90UR124.
 */
void ds90ur124_init(void)
{
    /* init the ADV7180 which handles PWRDN and OE signals
       of the DS90UR124 */
    adv7180_i2c_init_obds();
    /* Data, hsync, and vsync signals are shared between
       the deserializer and video in chip */
    /* video in I/O placed in tri-state */
    adv7180_set_tristate_output();
    /* Enable power to DS90UR124 */
    adv7180_set_gpio_output(0, GPIO_HIGH_LEVEL);
    /* Enable receiver of DS90UR124 */
    adv7180_set_gpio_output(1, GPIO_HIGH_LEVEL);
    /* call hardware.h I/O init */
    deserializer_io_config();
}

/*!
 * This test uses a demo kit from National with a serializer DS90UR214.
 * A clock is connected at J1.TCLK of this board, and JP1 is removed to
 * avoid conneting RED signal of the USB cable to +5V.
 * The demo board is connected to the SABRE AI with a USB cable
 * The LED D56 is used to observe if DS90UR124' PLL is locked, if light up
 * it means the clock is correctly received and de-serialized.
 * Connect a cable to JP1.VDD of the demo board, the other side will
 * be sequentially connected to the asked contact of J1.
 * As long as the test runs, the value is received correctly on the
 * observed pin.
 */
void ds90ur124_receive_cnx_test(void)
{
    int i;

    /* IOMUXC_SW_MUX_CTL_PAD_CSI0_VSYNC => (SW_MUX_BASE_ADDR + 0xac)
       IOMUXC_SW_MUX_CTL_PAD_CSI0_DAT4 => (SW_MUX_BASE_ADDR + 0xb0)
       ...
       IOMUXC_SW_MUX_CTL_PAD_CSI0_DAT18 (SW_MUX_BASE_ADDR + 0xe8)
       IOMUXC_SW_MUX_CTL_PAD_CSI0_DAT19 (SW_MUX_BASE_ADDR + 0xec) */

    /* configure everything as an I/O and test it receives a high level
       CSI0_DAT[19:4]
       GPIO5_22 to GPIO5_31 */
    for (i = 0; i < 10; i++) {
        writel(ALT1, SW_MUX_BASE_ADDR + 0xb0 + i * 4);
        gpio_set_direction(GPIO_PORT5, 22 + i, GPIO_GDIR_INPUT);
        printf("  Please set serializer pin J1.%d to high\n", i);
        while (gpio_get_level(GPIO_PORT5, 22 + i) != GPIO_HIGH_LEVEL) ;
    }

    /* GPIO6_0 to GPIO0_5 */
    for (i = 0; i < 6; i++) {
        writel(ALT1, SW_MUX_BASE_ADDR + 0xd8 + i * 4);
        gpio_set_direction(GPIO_PORT6, i, GPIO_GDIR_INPUT);
        printf("  Please set serializer pin J1.%d to high\n", i + 10);
        while (gpio_get_level(GPIO_PORT6, i) != GPIO_HIGH_LEVEL) ;
    }

    /* GPIO5_19 <=> CSI0_HSYNC */
    writel(ALT1, SW_MUX_BASE_ADDR + 0xa4);
    gpio_set_direction(GPIO_PORT5, 19, GPIO_GDIR_INPUT);
    printf("  Please set serializer pin J1.22 to high\n");
    while (gpio_get_level(GPIO_PORT5, 19) != GPIO_HIGH_LEVEL) ;

    /* GPIO5_21 <=> CSI0_VSYNC */
    writel(ALT1, SW_MUX_BASE_ADDR + 0xac);
    gpio_set_direction(GPIO_PORT5, 21, GPIO_GDIR_INPUT);
    printf("  Please set serializer pin J1.23 to high\n");
    while (gpio_get_level(GPIO_PORT5, 21) != GPIO_HIGH_LEVEL) ;

    printf("Test is finished, you can get back to your doze state :-D\n");
}

/*!
 * The test reads some data from the deserializer on the CSI.
 * 
 * @return TEST_PASSED or TEST_FAILED
 */
int ds90ur124_test_enable;
int ds90ur124_test_main(void)
{
    if (!ds90ur124_test_enable) {
        return TEST_NOT_PRESENT;
    }

    PROMPT_RUN_TEST("DE-SERIALIZER DS90UR124", NULL);

    printf(" Please ensure that the demo board is connected through a USB cable.\n");
    printf(" JP1 should be removed from this board.\n");
    printf
        (" A clock should be provided on J1.TCLK, and LED D56 of the SABRE AI should light up.\n");

    if (!is_input_char('y', NULL)) {
        return TEST_BYPASSED;
    }

    ds90ur124_init();
    ds90ur124_receive_cnx_test();

    // the test can't fail because the above function uses while conditions !
    return TEST_PASSED;
}

//RUN_TEST_INTERACTIVE("DE-SERIALIZER DS90UR124", ds90ur124_test_main)

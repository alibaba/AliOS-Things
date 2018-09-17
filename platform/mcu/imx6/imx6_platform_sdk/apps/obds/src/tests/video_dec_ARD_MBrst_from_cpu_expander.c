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

const char g_ard_mb_expander_reset_test_name[] = "SABRE AI Main Board Reset via IO Expander";

// adv7180 functions
extern void adv7180_i2c_init_obds(void);
extern unsigned char adv7180_reg_read(unsigned char reg_addr);
extern int adv7180_reg_write(unsigned char reg_addr, unsigned char data);
extern void adv7180_set_gpio_output(unsigned int io_x, unsigned int level);

test_return_t ard_mb_expander_reset_test(void)
{
    test_return_t result = TEST_FAILED;
    const char * indent = menu_get_indent();

    printf("\n%sChecking if IO expander GPIO can reset MB peripherals\n", indent);

    /* blink LED1 and LED2 attached to gpios of the ADV7180 */
    adv7180_i2c_init_obds();
    adv7180_reg_write(0x00, 0x03);  //when reset will be set to 0

    // rset via io expander on CPU card
    /* toggle MAIN_PER_RST_B(PORT_EXP_A3) on Expander A  */
    board_ioexpander_iomux_config();
    max7310_set_gpio_output(0, 3, GPIO_LOW_LEVEL);  //expander a, io3
    hal_delay_us(500000);
    max7310_set_gpio_output(0, 3, GPIO_HIGH_LEVEL); //expander a, io3 

    if (adv7180_reg_read(0x00) == 0)
    {
        result = TEST_PASSED;             //test passed, adv7180 was reset
    }

    return result;
}

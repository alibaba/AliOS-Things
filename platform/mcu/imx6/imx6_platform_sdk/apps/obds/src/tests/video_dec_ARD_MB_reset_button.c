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

const char g_ard_main_board_reset_button_test[] = "SABRE AI Main Board Reset Button";

// adv7180 functions
extern void adv7180_i2c_init_obds(void);
extern unsigned char adv7180_reg_read(unsigned char reg_addr);
extern int adv7180_reg_write(unsigned char reg_addr, unsigned char data);
extern void adv7180_set_gpio_output(unsigned int io_x, unsigned int level);

test_return_t ard_mb_reset_test(void)
{
    int time = 10;
    test_return_t result = TEST_FAILED;
    const char* indent = menu_get_indent();

    printf("\n%sLED1 and LED2 around ADV7180/U29 will stop blinking after\n", indent);
    printf("%s pressing MB Reset button (SW2) on SABRE AI main board. \n", indent);
    printf("%s You only have %d seconds to hit the button, so make haste! \n", indent, time);

    /* blink LED1 and LED2 attached to gpios of the ADV7180 */
    adv7180_i2c_init_obds();
    adv7180_reg_write(0x00, 0x03);  //when reset will be set to 0
    printf("%s time left: %ds..............................\n", indent, time--);
    do {
        adv7180_set_gpio_output(2, GPIO_LOW_LEVEL);
        adv7180_set_gpio_output(3, GPIO_LOW_LEVEL);
        hal_delay_us(500000);
        adv7180_set_gpio_output(2, GPIO_HIGH_LEVEL);
        adv7180_set_gpio_output(3, GPIO_HIGH_LEVEL);
        hal_delay_us(500000);
        printf("%s %ds.............................................\n", indent, time--);
        if (adv7180_reg_read(0x00) == 0)
        {
            result = TEST_PASSED;         //test passed, adv7180 was reset
            break;
        }
    }
    while (time != 0);
    printf("\n");

    return result;
}

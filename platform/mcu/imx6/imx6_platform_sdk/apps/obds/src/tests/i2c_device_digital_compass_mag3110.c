/*
 * Copyright (c) 2012, Freescale Semiconductor, Inc.
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

const char g_mag3110_i2c_device_id_test_name[] = "Compass MAG3110 I2C Device ID Test";

unsigned char mag3110_reg_read(unsigned int i2c_base_addr, unsigned char reg_addr)
{
    struct imx_i2c_request rq = {0};
    unsigned char buf[1];
    unsigned char reg_data = 0;

    rq.ctl_addr = i2c_base_addr;
    rq.dev_addr = MAG3110_I2C_ID;
    rq.reg_addr = reg_addr;
    rq.reg_addr_sz = 1;
    rq.buffer_sz = 1;
    rq.buffer = buf;

    if (i2c_xfer(&rq, I2C_READ) != 0) {
        printf("%s() error. return\n", __FUNCTION__);
        return -1;
    }

    reg_data = buf[0];

    return reg_data;
}

/*!
 * @return      TEST_PASSED or  TEST_FAILED
 */
test_return_t i2c_device_id_check_mag3110(void)
{
    unsigned char ret_data;
    unsigned int i2c_base_addr = I2C3_BASE_ADDR;
    const char* indent = menu_get_indent();

#if defined(BOARD_SMART_DEVICE)
    //  USB_OTG_PWR_EN (EIM_D22)
    writel(ALT5, IOMUXC_SW_MUX_CTL_PAD_EIM_EB3);
    gpio_set_direction(GPIO_PORT2, 31, GPIO_GDIR_OUTPUT);
    gpio_set_level(GPIO_PORT2, 31, GPIO_LOW_LEVEL);
    hal_delay_us(1000);
    gpio_set_level(GPIO_PORT2, 31, GPIO_HIGH_LEVEL);
#endif    

    i2c_init(i2c_base_addr, 170000);

    ret_data = mag3110_reg_read(i2c_base_addr, 0x07);

    if (ret_data != 0xC4) {
        printf("%s * MAG3110 device ID test failed (0x%02X). Expected 0xC4\n", indent, ret_data);
        return TEST_FAILED;
    } else {
        printf("%s MAG3110 I2C device check passed(0xC4).\n", indent);
        return TEST_PASSED;
    }
}

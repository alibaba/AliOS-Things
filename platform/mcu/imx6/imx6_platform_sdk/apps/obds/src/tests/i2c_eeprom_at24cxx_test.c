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
//#include "imx_i2c.h"
//#include "functions.h"

static int at24cx_read(unsigned int i2c_base_addr, unsigned int addr, unsigned char buf[])
{
    struct imx_i2c_request rq = {0};
    rq.ctl_addr = i2c_base_addr;
    rq.dev_addr = g_at24cx_i2c_device.address;
    rq.reg_addr = addr;
    rq.reg_addr_sz = 2;
    rq.buffer_sz = 3;
    rq.buffer = buf;
    return i2c_xfer(&rq, I2C_READ);
//    return i2c_xfer(i2c_base_addr, &rq, I2C_READ);
}

static int at24cx_write(unsigned int i2c_base_addr, unsigned int addr, unsigned char buf[])
{
    struct imx_i2c_request rq = {0};
    rq.ctl_addr = i2c_base_addr;
    rq.dev_addr = g_at24cx_i2c_device.address;
    rq.reg_addr = addr;
    rq.reg_addr_sz = 2;
    rq.buffer_sz = 3;
    rq.buffer = buf;
    return i2c_xfer(&rq, I2C_WRITE);
//    return i2c_xfer(i2c_base_addr, &rq, I2C_WRITE);
}

int eeprom_test_enable;
static int i2c_eeprom_at24cxx_test(void)
{
    unsigned char buf1[4], buf2[4];

    if (!eeprom_test_enable) {
        return TEST_NOT_PRESENT;
    }

    PROMPT_RUN_TEST("EEPROM test", NULL);

    i2c_init(g_at24cx_i2c_device.port, g_at24cx_i2c_device.freq);

#if (!defined(CHIP_MX6SL) && defined(BOARD_EVB))
    /*Set iomux and daisy chain for eeprom test */
    reg32_write(IOMUXC_SW_MUX_CTL_PAD_EIM_D17, ALT6 | 0x10);
    reg32_write(IOMUXC_I2C3_IPP_SCL_IN_SELECT_INPUT, 0x00);
    reg32_write(IOMUXC_SW_PAD_CTL_PAD_EIM_D17, 0x1b8b0);

    reg32_write(IOMUXC_SW_MUX_CTL_PAD_EIM_D18, ALT6 | 0x10);
    reg32_write(IOMUXC_I2C3_IPP_SDA_IN_SELECT_INPUT, 0x00);
    reg32_write(IOMUXC_SW_PAD_CTL_PAD_EIM_D18, 0x1b8b0);
#endif

    buf1[0] = 'F';              //0x89;
    buf1[1] = 'S';              //0x11;
    buf1[2] = 'L';

    at24cx_write(g_at24cx_i2c_device.port, 0, buf1);

    buf2[0] = 0;
    buf2[1] = 0;
    buf2[2] = 0;
    at24cx_read(g_at24cx_i2c_device.port, 0, buf2);

#if (!defined(CHIP_MX6SL) && defined(BOARD_EVB))
    /*Restore iomux and daisy chain setting */
    i2c_init(g_at24cx_i2c_device.port, g_at24cx_i2c_device.freq);
#endif

    if ((buf2[0] != buf1[0]) || (buf2[1] != buf1[1]) || (buf2[2] != buf1[2])) {
        printf("i2c eeprom test fail.\n");
        printf("please make sure EEPROM is mounted on board\n");
        return TEST_FAILED;
    } else {
        printf("i2c eeprom test pass.\n");
        return TEST_PASSED;
    }
}

//RUN_TEST("EEPROM", i2c_eeprom_at24cxx_test)

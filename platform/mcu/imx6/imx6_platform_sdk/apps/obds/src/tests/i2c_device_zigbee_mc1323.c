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

#define MC1323_REG_SDIDH	0x06
#define MC1323_REG_SDIDL	0x07

unsigned char buf[] = { 0, 0, 0, 0 };

static const char * const test_name = "I2C_DEVICE_MC1323 Test";

static unsigned int mc1323_reg_read(unsigned int i2c_base_addr, unsigned int reg_addr)
{
    struct imx_i2c_request rq = {0};
    unsigned int reg_data;

    reg_data = 0;
    rq.ctl_addr = i2c_base_addr;
    rq.dev_addr = MC1323_I2C_ID;
    rq.reg_addr = reg_addr;
    rq.reg_addr_sz = 1;
    rq.buffer_sz = 1;
    rq.buffer = buf;
    if (i2c_xfer(&rq, I2C_READ) != 0) {
        printf("Failed to read from i2c\n");
    }
    reg_data = buf[0] | buf[1] << 8 | buf[2] << 16 | buf[3] << 24;

    return reg_data;
}

static int mc1323_reg_write(unsigned int i2c_base_addr, unsigned int reg_addr, unsigned int reg_val)
{
    struct imx_i2c_request rq = {0};

    buf[0] = reg_val;
    rq.ctl_addr = i2c_base_addr;
    rq.dev_addr = MC1323_I2C_ID;
    rq.reg_addr = reg_addr;
    rq.reg_addr_sz = 1;
    rq.buffer_sz = 1;
    rq.buffer = buf;
    
    return i2c_xfer(&rq, I2C_WRITE);
}

int i2c_device_id_check_mc1323(unsigned int i2c_base_addr)
{
    unsigned int sdidh = 0;
    unsigned int sdidl = 0;
    printf("\tTest MC1323 (Zigbee) Device ID ");
    i2c_init(i2c_base_addr, 170000);

    sdidh = mc1323_reg_read(i2c_base_addr, MC1323_REG_SDIDH);   //read  WHO_AM_I reg

    sdidl = mc1323_reg_read(i2c_base_addr, MC1323_REG_SDIDL);   //read  WHO_AM_I reg

    if ((sdidh == 0x20 || sdidh == 0x30) && sdidl == 0x28) {
        printf("passed 0x%02X%02X\n", sdidh, sdidl);
        return 0;
    } else {
        printf("failed, 0x2028 vs 0x%02X%02X\n", sdidh, sdidl);
        return 1;
    }
}

/*!
 * @return      TEST_PASSED or  TEST_FAILED    
 */
test_return_t i2c_device_mc1323_test(void)
{
    const char * indent = menu_get_indent();

    if ( prompt_run_test(test_name, indent) != TEST_CONTINUE )
        return TEST_BYPASSED;

    return i2c_device_id_check_mc1323(I2C2_BASE_ADDR);
}

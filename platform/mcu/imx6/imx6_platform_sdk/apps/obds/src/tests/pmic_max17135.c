/*
 * Copyright (c) 2010-2012, Freescale Semiconductor, Inc.
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

#define PMIC_MAX17135_I2C_REG_BYTE    0x1   // Number of Bytes to transfer the PMIC reg number
#define PMIC_MAX17135_I2C_DATA_BYTE   0x1   // Number of Bytes to transfer the PMIC reg data

const char g_max17135_i2c_device_id_test_name[] = "PMIC MAX17135 I2C Device ID Test";

unsigned char max17135_reg_read(unsigned char reg)
{
    struct imx_i2c_request rq = {0};
    unsigned char buf = 0;

    /* Initialize some of the I2C imx_i2c_request structure, these parameters shouldn't need to be changed */
    rq.ctl_addr = g_pmic_max17135_i2c_device.port;
    rq.dev_addr = g_pmic_max17135_i2c_device.address;
    rq.reg_addr_sz = PMIC_MAX17135_I2C_REG_BYTE;
    rq.buffer_sz = PMIC_MAX17135_I2C_DATA_BYTE;
    rq.reg_addr = reg;
    rq.buffer = &buf;

    i2c_xfer(&rq, I2C_READ);

    return buf;
}

void max17135_reg_write(unsigned char reg, unsigned char reg_data)
{
    struct imx_i2c_request rq = {0};

    rq.ctl_addr = g_pmic_max17135_i2c_device.port;
    rq.dev_addr = g_pmic_max17135_i2c_device.address;
    rq.reg_addr_sz = PMIC_MAX17135_I2C_REG_BYTE;
    rq.buffer_sz = PMIC_MAX17135_I2C_DATA_BYTE;
    rq.reg_addr = reg;
    rq.buffer = &reg_data;

    i2c_xfer(&rq, I2C_READ);
}

static void max17135_output_seq_set(void)
{
    /*power up sequence */
    max17135_reg_write(0x10, 0x0A); //delay 10ms
    max17135_reg_write(0x12, 0x14);
    max17135_reg_write(0x11, 0x1E);
    max17135_reg_write(0x13, 0x28);

    /*power down sequence */
    max17135_reg_write(0x14, 0x0A);
    max17135_reg_write(0x16, 0x14);
    max17135_reg_write(0x15, 0x1E);
    max17135_reg_write(0x17, 0x28);

}

/*!
 * @return      TEST_PASSED or  TEST_FAILED
 */
test_return_t i2c_device_id_check_MAX17135(void)
{
    unsigned char reg_data = 0;
    const char* indent = menu_get_indent();

    i2c_init(g_pmic_max17135_i2c_device.port, g_pmic_max17135_i2c_device.freq);

    reg_data = max17135_reg_read(0x07);
    if (0x4D == reg_data) {
        printf("%spassed 0x%02X\n\n", indent, reg_data);
//      max17135_output_seq_set();
        return TEST_PASSED;
    } else {
        printf("%sfailed, 0x4D vs 0x%02X\n\n", indent, reg_data);
        return TEST_FAILED;
    }
}

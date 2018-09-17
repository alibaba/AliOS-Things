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
 * @file si476x.c
 * @brief Driver and test for FM tuner SI476x controlled through I2C.
 *
 * @ingroup diag_fm_tuner
 */

#include "obds.h"
//#include "imx_i2c.h"

#define POWER_UP_CMD 0x01
#define FUNC_INFO_CMD 0x12

//extern void max7310_set_gpio_output(unsigned int, unsigned int, unsigned int);
//extern void hal_delay_us(unsigned int);

struct imx_i2c_request si476x_i2c_req;

/*!
 * I2C init function to control the FM tuner SI476x.
 */
static void si476x_i2c_init(void)
{
    /* reset the si476x */
    board_ioexpander_iomux_config();
    max7310_set_gpio_output(0, 3, GPIO_LOW_LEVEL);
    hal_delay_us(5000);
    max7310_set_gpio_output(0, 3, GPIO_HIGH_LEVEL);
    /* I2C initialization */
    i2c_init(g_si476x_i2c_device.port, g_si476x_i2c_device.freq);
    
    si476x_i2c_req.ctl_addr = g_si476x_i2c_device.port; 
    si476x_i2c_req.dev_addr = g_si476x_i2c_device.address;    // the I2C DEVICE address
    /* this device does not use registers but command passed through data buffer */
    si476x_i2c_req.reg_addr_sz = 0; // number of bytes of I2C device register's address
}

/*!
 * The function sends a power-up command to the FM tuner SI476x.
 * Command is successfull if the returned status is correct.
 * 
 * @return TEST_PASSED or TEST_FAILED
 */
static int si476x_PowerUp(void)
{
    unsigned char buf[10];

    si476x_i2c_req.buffer = buf;
    /* command for power up */
    si476x_i2c_req.buffer_sz = 6;
    buf[0] = POWER_UP_CMD;
    buf[1] = 0xF7;              /* reserved */
    buf[2] = 0x28;              /* reserved */
    buf[3] = 0x07;              /* reserved */
    buf[4] = 0x01;              /* FUNC[7:4] : FREQ[3:0] */
    buf[5] = 0x11;              /* reserved */
    i2c_xfer(&si476x_i2c_req, I2C_WRITE);
    /* read the status */
    si476x_i2c_req.buffer_sz = 1;
    i2c_xfer(&si476x_i2c_req, I2C_READ);

    if (buf[0] != 0x80) {
        printf("Expected status 0x80, read 0x%X\n", buf[0]);
        return TEST_FAILED;
    }

    /* command for function info */
    si476x_i2c_req.buffer_sz = 1;
    buf[0] = FUNC_INFO_CMD;
    i2c_xfer(&si476x_i2c_req, I2C_WRITE);
    /* read the status */
    si476x_i2c_req.buffer_sz = 7;
    i2c_xfer(&si476x_i2c_req, I2C_READ);

    if (buf[0] != 0x80) {
        printf("Expected status 0x80, read 0x%X\n", buf[0]);
        return TEST_FAILED;
    }

    return TEST_PASSED;
}

/*!
 * The test sends a power-up command to the FM tuner SI476x.
 * Test is successfull if the returned status is correct.
 * 
 * @return TEST_PASSED or TEST_FAILED
 */
int si476x_test_enable;
int si476x_test_main(void)
{
    if (!si476x_test_enable) {
        return TEST_NOT_PRESENT;
    }
    PROMPT_RUN_TEST("FM TUNER SI476x", NULL);

    si476x_i2c_init();
    return si476x_PowerUp();
}

//RUN_TEST("FM TUNER SI476x", si476x_test_main)

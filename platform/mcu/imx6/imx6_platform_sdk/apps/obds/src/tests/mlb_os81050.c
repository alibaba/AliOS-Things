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
 * @file os81050.c
 * @brief Driver and test for the MOST OS81050 controlled through I2C.
 *
 * @ingroup diag_video_dec
 */

#include "obds.h"

static struct imx_i2c_request os81050_i2c_req;

static unsigned char ref_status[6] = { 0x00, 0x03, 0x02, 0x1A, 0x80, 0x00 };

//MLB iomux configuration
void mlb_io_config(void)
{
#ifdef MLB_SUPPORT
   //default board will use I2C3, but might be I2C2
   io_cfg_i2c(OS81050_I2C_BASE);
   mlb_iomux_config();
   //MLB_PWRDN input
   writel(ALT1, IOMUXC_SW_MUX_CTL_PAD_DISP0_DAT15);
   gpio_set_direction(GPIO_PORT5, 9, GPIO_GDIR_INPUT);
#endif
}

/*!
 * I2C init function to control the OS81050.
 */
void os81050_i2c_init(void)
{
    /* set os81050 into reset */
    board_ioexpander_iomux_config();
    max7310_set_gpio_output(0, 5, GPIO_LOW_LEVEL);
    /* Init MLB I/Os */
    mlb_io_config();
    /* exit reset */
    max7310_set_gpio_output(0, 5, GPIO_HIGH_LEVEL);
    /* have to wait 100ms after reset */
    hal_delay_us(100000);

    /* I2C initialization */
    i2c_init(g_os81050_i2c_device.port, g_os81050_i2c_device.freq);
    os81050_i2c_req.dev_addr = g_os81050_i2c_device.address;  // the I2C DEVICE address
}

/*!
 * Function to read the status of the OS81050.
 *
 * @return 0 on success; non-zero otherwise
 */
int os81050_read_status(void)
{
    unsigned int i;
    unsigned char buf[6] = { 0, 0, 0, 0, 0, 0 };

    os81050_i2c_req.buffer = buf;

    os81050_i2c_req.ctl_addr = g_os81050_i2c_device.port;
    os81050_i2c_req.reg_addr_sz = 0;
    os81050_i2c_req.buffer_sz = 6;
    i2c_xfer(&os81050_i2c_req, I2C_READ);
//    i2c_xfer(OS81050_I2C_BASE, &os81050_i2c_req, I2C_READ);
    for (i = 0; i < os81050_i2c_req.buffer_sz; i++) {
        if (buf[i] != ref_status[i]) {
            printf("\nStatus error. Read 0x%X instead of 0x%X\n", buf[i], ref_status[i]);
            return -1;
        }
    }

    return 0;
}

/*!
 * Function to read the status of the OS81050.
 *
 * This function is still in development.
 *
 * @return 0 on success; non-zero otherwise
 */
int os81050_get_message(void)
{
    unsigned char buf[30];

    os81050_i2c_req.ctl_addr = g_os81050_i2c_device.port;
    os81050_i2c_req.buffer = buf;
    os81050_i2c_req.reg_addr_sz = 0;

    /* PML.01.14.FktID.OpType.Data => 6bytes */
    os81050_i2c_req.buffer_sz = 10;
    buf[0] = 0x00;
    buf[1] = os81050_i2c_req.buffer_sz - 2;
    buf[2] = 0x01;
    buf[3] = 0x14;
    buf[4] = 0x20;
    buf[5] = 0x01;
    buf[6] = 0x00;
    buf[7] = 0x02;
    buf[8] = 0x00;
    buf[9] = 0x00;
    i2c_xfer(&os81050_i2c_req, I2C_WRITE);
//    i2c_xfer(OS81050_I2C_BASE, &os81050_i2c_req, I2C_WRITE);
    hal_delay_us(50000);

    i2c_xfer(&os81050_i2c_req, I2C_WRITE);
//    i2c_xfer(OS81050_I2C_BASE, &os81050_i2c_req, I2C_WRITE);
    hal_delay_us(50000);

    os81050_i2c_req.buffer_sz = 24;
    i2c_xfer(&os81050_i2c_req, I2C_WRITE);
//    i2c_xfer(OS81050_I2C_BASE, &os81050_i2c_req, I2C_READ);

//    printf("0x%X\n",buf);

    return 0;
}

/*!
 * The test reads and checks the status of the OS81050.
 * Later, it could retrieve a message => to do.
 * 
 * @return TEST_PASSED or TEST_FAILED
 */
int mlb_os81050_test_enable;
int os81050_test_main(void)
{
    int ret;

    if (!mlb_os81050_test_enable) {
        return TEST_NOT_PRESENT;
    }

    PROMPT_RUN_TEST("MLB OS81050", NULL);

    os81050_i2c_init();
    ret = os81050_read_status();
//    ret = os81050_get_message();

    return ret;
}

//RUN_TEST("MLB OS81050", os81050_test_main)

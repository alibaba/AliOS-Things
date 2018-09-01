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
 * @file adv7180.c
 * @brief Driver and test for the video A/D converter ADV7180 controlled through I2C.
 *
 * @ingroup diag_video_dec
 */

#include "obds.h"
//#include "imx_i2c.h"

const char g_video_dec_test_name[] = "Video Input Test";
#define IDENT_REG 0x11
#define GPO_REG 0x59
#define OUTPUT_CTRL_REG 0x03

static imx_i2c_request_t adv7180_i2c_req;

/*!
 * I2C init function to control the video A/D converter ADV7180.
 */
void adv7180_i2c_init_obds(void)
{
    /* I2C initialization */
    i2c_init(g_adv7180_i2c_device.port, g_adv7180_i2c_device.freq);

    max7310_set_gpio_output(1, 2, GPIO_LOW_LEVEL);
    hal_delay_us(5000);
    max7310_set_gpio_output(1, 2, GPIO_HIGH_LEVEL);
}

/*!
 * Function to read a register of the video A/D converter ADV7180.
 *
 * @param reg_addr   register address
 *
 * @return the value of the read register
 */
unsigned char adv7180_reg_read(unsigned char reg_addr)
{
    unsigned char buf[1];
    adv7180_i2c_req.ctl_addr = g_adv7180_i2c_device.port;
    adv7180_i2c_req.dev_addr = g_adv7180_i2c_device.address;
    adv7180_i2c_req.reg_addr = reg_addr;
    adv7180_i2c_req.buffer = buf;
    adv7180_i2c_req.reg_addr_sz = 1;
    adv7180_i2c_req.buffer_sz = 1;
//    i2c_xfer(ADV7180_I2C_BASE, &adv7180_i2c_req, I2C_READ);
    i2c_xfer(&adv7180_i2c_req, I2C_READ);
    return buf[0];
}

/*!
 * Function to write a register of the video A/D converter ADV7180.
 *
 * @param reg_addr   register address
 * @param data       written data
 *
 * @return 0 on success; non-zero otherwise
 */
int adv7180_reg_write(unsigned char reg_addr, unsigned char data)
{
    adv7180_i2c_req.ctl_addr = g_adv7180_i2c_device.port; 
    adv7180_i2c_req.dev_addr = g_adv7180_i2c_device.address;
    adv7180_i2c_req.reg_addr = reg_addr;
    adv7180_i2c_req.buffer = &data;
    adv7180_i2c_req.reg_addr_sz = 1;
    adv7180_i2c_req.buffer_sz = 1;
  
    return i2c_xfer(&adv7180_i2c_req, I2C_WRITE);
//    return i2c_xfer(ADV7180_I2C_BASE, &adv7180_i2c_req, I2C_WRITE);
}

void adv7180_set_tristate_output(void)
{
    /* set bit TDO */
    adv7180_reg_write(OUTPUT_CTRL_REG, 0x48);
}

/*!
 * Function to check the register ID the video A/D converter ADV7180.
 *
 * @return 0 on success; non-zero otherwise
 */
test_return_t adv7180_i2c_device_id_check(void)
{
    unsigned char data;

    /* read the ID register */
    data = adv7180_reg_read(IDENT_REG);
    if (data != 0x1C) {
        printf("Expected id 0x1C, read 0x%X\n", data);
        return TEST_FAILED;
    }

    return TEST_PASSED;
}

/*!
 * Function to control the ADV7181 GPIO output state.
 *
 * @param io_x       I/O number (0=IO_0, 1=IO_1, ...)
 * @param level      I/O state (0=low, 1=high)
 * 
 * @return 0 on success; non-zero otherwise
 */
void adv7180_set_gpio_output(unsigned int io_x, unsigned int level)
{
    unsigned char data;
    /* read output state first */
    data = adv7180_reg_read(GPO_REG);
    /* clear I/O's old value */
    data &= ~(1 << io_x);
    /* set I/O's new value + enable GPO */
    data |= (level << io_x) | (1 << 4);
    /* write result in output register */
    adv7180_reg_write(GPO_REG, data);
}

/*!
 * The test reads and checks the ADV7180 ID.
 * Then, it toggles LED connected on GPIO 2 and 3.
 * 
 * @return TEST_PASSED or TEST_FAILED
 */
test_return_t adv7180_test_main(void)
{
    test_return_t ret = TEST_NOT_STARTED;
    char recvCh;
    const char* indent = menu_get_indent();

    adv7180_i2c_init_obds();
    ret = adv7180_i2c_device_id_check();

    /* if ID check test pass then test LED1 and LED2 attached
       to general purpose outputs of the ADV7180 */
    if (ret == TEST_PASSED) {
        printf("%s ID read successfully\n", indent);
        printf("%s Do you see blinking LED1 and LED2 around ADV7180/U29? (y/n) \n", indent);

        do {
            adv7180_set_gpio_output(2, GPIO_LOW_LEVEL);
            adv7180_set_gpio_output(3, GPIO_LOW_LEVEL);
            hal_delay_us(500000);
            adv7180_set_gpio_output(2, GPIO_HIGH_LEVEL);
            adv7180_set_gpio_output(3, GPIO_HIGH_LEVEL);
            hal_delay_us(500000);
            
            if (getchar() != (uint8_t) 0xFF) {
                break;
            }
        }
        while (1);

        if ((recvCh == 'n') || (recvCh == 'N'))
            ret = TEST_FAILED;

        adv7180_set_gpio_output(2, GPIO_LOW_LEVEL);
        adv7180_set_gpio_output(3, GPIO_LOW_LEVEL);
    }

    /* To do a capture test if possible or/and make sense here */
    return ret;
}

//RUN_TEST("VIDEO INPUT", adv7180_test_main)

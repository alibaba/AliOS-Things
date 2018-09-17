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

const char g_mma8450_i2c_device_id_test_name[] = "Accelerometer MMA8450 I2C Device ID Test";

#define REG_CTRL_REG1		0x38
#define REG_XYZ_DATA_CFG	0x16
#define REG_SYS_MODE		0x14
#define REG_F_SETUP			0x13
#define REG_DATA_STATUS		0x00
#define REG_OUT_X_LSB		0x05
#define REG_OUT_X_MSB		0x06
#define REG_OUT_Y_LSB		0x07
#define REG_OUT_Y_MSB		0x08
#define REG_OUT_Z_LSB		0x09
#define REG_OUT_Z_MSB		0x0A

int mma8450_show_accel(unsigned int i2c_base_addr);

static unsigned char mma8450_reg_read(unsigned int i2c_base_addr, unsigned char reg_addr)
{
    struct imx_i2c_request rq = {0};
    unsigned char buf[1];
    unsigned char reg_data = 0;

    rq.ctl_addr = i2c_base_addr;
    rq.dev_addr = MMA8450_I2C_ID;
    rq.reg_addr = reg_addr;
    rq.reg_addr_sz = 1;
    rq.buffer_sz = 1;
    rq.buffer = buf;
    i2c_xfer(&rq, I2C_READ);
    reg_data = buf[0];

    return reg_data;
}

static int mma8450_reg_write(unsigned int i2c_base_addr, unsigned char reg_addr,
                             unsigned char reg_val)
{
    struct imx_i2c_request rq = {0};
    unsigned char buf[1];

    buf[0] = reg_val;
    rq.ctl_addr = i2c_base_addr;
    rq.dev_addr = MMA8450_I2C_ID;
    rq.reg_addr = reg_addr;
    rq.reg_addr_sz = 1;
    rq.buffer_sz = 1;
    rq.buffer = buf;

    return i2c_xfer(&rq, I2C_WRITE);
}

/*!
 * @return      TEST_PASSED or  TEST_FAILED
 */
test_return_t i2c_device_id_check_MMA8450(void)
{
    unsigned int reg_data = 0;
    const char* indent = menu_get_indent();
    unsigned int i2c_base_addr = 0;
    i2c_base_addr = I2C1_BASE_ADDR;

#if defined(BOARD_SMART_DEVICE)
    //  USB_OTG_PWR_EN (EIM_D22)
    writel(ALT5, IOMUXC_SW_MUX_CTL_PAD_EIM_EB3);
    gpio_set_direction(GPIO_PORT2, 31, GPIO_GDIR_OUTPUT);
    gpio_set_level(GPIO_PORT2, 31, GPIO_LOW_LEVEL);
    hal_delay_us(1000);
    gpio_set_level(GPIO_PORT2, 31, GPIO_HIGH_LEVEL);
#endif    

    i2c_init(i2c_base_addr, 170000);
    reg_data = mma8450_reg_read(i2c_base_addr, 0x0F);   //read  WHO_AM_I reg

    if (0xC6 == reg_data) {
        printf("%sRead I2C ID passed (0xC6).\n\n", indent);
        mma8450_show_accel(i2c_base_addr);
        return TEST_PASSED;
    } else {
        printf("%sRead I2C ID failed (0x%02X). Expected 0xC6\n\n", indent, reg_data);
        return TEST_FAILED;
    }
}

int mma8450_set_config(unsigned int i2c_base_addr)
{
    unsigned char val = 0;

    /*Put the mma8450 into standby mode */
    val = mma8450_reg_read(i2c_base_addr, REG_CTRL_REG1);
    val &= ~(0x03);
    mma8450_reg_write(i2c_base_addr, REG_CTRL_REG1, val);

    /*Set the F_MODE, disable FIFO */
    val = mma8450_reg_read(i2c_base_addr, REG_F_SETUP);
    val &= 0x3F;
    mma8450_reg_write(i2c_base_addr, REG_F_SETUP, val);

    /*Set XYZ event detection */
    val = mma8450_reg_read(i2c_base_addr, REG_XYZ_DATA_CFG);
    val |= 0x7;                 //enable XYZ event
    mma8450_reg_write(i2c_base_addr, REG_XYZ_DATA_CFG, val);
    
    /*Set the range, -8g to 8g and activate the sensor, and active the sensor */
    val = mma8450_reg_read(i2c_base_addr, REG_CTRL_REG1);
    val &= ~0x03;
    val |= 0x03;
    mma8450_reg_write(i2c_base_addr, REG_CTRL_REG1, val);

    return 0;
}

typedef struct {
    short accel_x;
    short accel_y;
    short accel_z;
} Accel, *pAccel;

int mma8450_get_accel(unsigned int i2c_base_addr, Accel * acc)
{
    unsigned char ucVal1 = 0, ucVal2 = 0;
    unsigned char ucStatus = 0;
    unsigned short iTemp = 0;
    int sign = 1;
    char signch = '+';

    /*wait until there is new data overwritten */
    do {
        ucStatus = mma8450_reg_read(i2c_base_addr, REG_DATA_STATUS);
    } while(!(ucStatus & 0x08)); 

    ucVal1 = mma8450_reg_read(i2c_base_addr, REG_OUT_X_MSB);
    ucVal2 = mma8450_reg_read(i2c_base_addr, REG_OUT_X_LSB);
    sign = (ucVal1 & 0x80) ? (-1) : 1;
    iTemp = ((ucVal1 & 0x7F) << 4) + (ucVal2 & 0xF);
    if (sign == -1)             //2's completement
        iTemp = (~iTemp + 1) & 0x7FF;
    signch = (sign == -1) ? '-' : '+';
    acc->accel_x = iTemp * sign;
    printf("\r 	  The acceleration is: (%c%01d.%02dg,",
           signch, iTemp * 8 / 0x800, (iTemp & 0xFF) * 800 / 0x800);

    ucVal1 = mma8450_reg_read(i2c_base_addr, REG_OUT_Y_MSB);
    ucVal2 = mma8450_reg_read(i2c_base_addr, REG_OUT_Y_LSB);
    sign = (ucVal1 & 0x80) ? (-1) : 1;
    iTemp = ((ucVal1 & 0x7F) << 4) + (ucVal2 & 0xF);
    if (sign == -1)             //2's completement
        iTemp = (~iTemp + 1) & 0x7FF;
    signch = (sign == -1) ? '-' : '+';
    acc->accel_x = iTemp * sign;
    printf(" %c%01d.%02dg,", signch, iTemp * 8 / 0x800, (iTemp & 0xFF) * 800 / 0x800);

    ucVal1 = mma8450_reg_read(i2c_base_addr, REG_OUT_Z_MSB);
    ucVal2 = mma8450_reg_read(i2c_base_addr, REG_OUT_Z_LSB);
    sign = (ucVal1 & 0x80) ? (-1) : 1;
    iTemp = ((ucVal1 & 0x7F) << 4) + (ucVal2 & 0xF);
    if (sign == -1)             //2's completement
        iTemp = (~iTemp + 1) & 0x7FF;
    signch = (sign == -1) ? '-' : '+';
    acc->accel_x = iTemp * sign;
    printf(" %c%01d.%02dg)", signch, iTemp * 8 / 0x800, (iTemp & 0xFF) * 800 / 0x800);

    return 0;
}

int mma8450_show_accel(unsigned int i2c_base_addr)
{
    unsigned char uc = NONE_CHAR;
    const char* indent = menu_get_indent();
    Accel acc;

    printf("%s    Do you want to check the accelerometer?(y/n)\n\n", indent);

    do {
       uc = getchar();
    } while (uc == NONE_CHAR);

    if (uc == 'y' || uc == 'Y') {
        mma8450_set_config(i2c_base_addr);

        printf("%s    Start show acceleration. Type 'x' to exit.\n\n", indent);
        while (1) {
            mma8450_get_accel(i2c_base_addr, &acc);
            uc = getchar();
            if (uc == 'x' || uc == 'X') {
                printf("\n\n");
                break;
            }
        }
    } else {
        return 1;
    }
    return 0;
}

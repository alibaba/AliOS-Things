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

#define EMC1046_REG_DEVICE_ID_OFF	0xFD

const char g_emc1046_i2c_device_id_test_name[] = "Temp Sensor EMC1046 I2C Device ID Test";

enum {
    INTERNAL_DIODE,
    EXTERNAL_DIODE1,
    EXTERNAL_DIODE2,
    EXTERNAL_DIODE3,
    EXTERNAL_DIODE4,
    EXTERNAL_DIODE5,
    EXTERNAL_DIODE6,
};

static unsigned char emc1046_reg_read(unsigned int i2c_base_addr, unsigned char reg_addr)
{
    struct imx_i2c_request rq = {0};
    unsigned char buf[1];
    unsigned char reg_data = 0;

    rq.ctl_addr = i2c_base_addr;
    rq.dev_addr = EMC1046_I2C_ID;
    rq.reg_addr = reg_addr;
    rq.reg_addr_sz = 1;
    rq.buffer_sz = 1;
    rq.buffer = buf;
    i2c_xfer(&rq, I2C_READ);
    reg_data = buf[0];
    return reg_data;
}

static int emc1046_reg_write(unsigned int i2c_base_addr, unsigned char reg_addr,
                             unsigned char reg_val)
{
    struct imx_i2c_request rq = {0};
    unsigned char buf[1];
    //unsigned char reg_data = 0;
    buf[0] = reg_val;
    rq.ctl_addr = i2c_base_addr;
    rq.dev_addr = EMC1046_I2C_ID;
    rq.reg_addr = reg_addr;
    rq.reg_addr_sz = 1;
    rq.buffer_sz = 1;
    rq.buffer = buf;
    return i2c_xfer(&rq, I2C_WRITE);
}

static void emc1046_convert_temp(unsigned char high_byte, unsigned char low_byte)
{
    unsigned char sign = high_byte >> 7;
    unsigned char integ = high_byte & 0x7F;
    unsigned char frac = (low_byte & 0xE0) >> 5;
    unsigned short temp = (integ << 3) | frac;
    if (sign == 1) {
        temp = (~temp + 1) & 0x3FF; //2's conversion in 10 bit
    }
    printf("%c%d.%03d centigrade\n", (sign == 1) ? '-' : '+', (temp >> 3), (temp & 0x7) * 1000 / 8);
}

static void emc1046_report_temp(unsigned int i2c_base_addr, int diode_id)
{
    unsigned char hbyte = 0, lbyte = 0;
    switch (diode_id) {
    case INTERNAL_DIODE:
        hbyte = emc1046_reg_read(i2c_base_addr, 0x00);
        lbyte = emc1046_reg_read(i2c_base_addr, 0x29);
        printf("Temperature of INTERNAL DIODE is ");
        emc1046_convert_temp(hbyte, lbyte);
        break;
    case EXTERNAL_DIODE1:
        hbyte = emc1046_reg_read(i2c_base_addr, 0x01);
        lbyte = emc1046_reg_read(i2c_base_addr, 0x10);
        printf("Temperature of EXTERNAL DIODE0 is ");
        emc1046_convert_temp(hbyte, lbyte);
        break;
    case EXTERNAL_DIODE2:
        hbyte = emc1046_reg_read(i2c_base_addr, 0x23);
        lbyte = emc1046_reg_read(i2c_base_addr, 0x24);
        printf("Temperature of EXTERNAL DIODE0 is ");
        emc1046_convert_temp(hbyte, lbyte);
        break;
    case EXTERNAL_DIODE3:
        hbyte = emc1046_reg_read(i2c_base_addr, 0x2A);
        lbyte = emc1046_reg_read(i2c_base_addr, 0x2B);
        printf("Temperature of EXTERNAL DIODE0 is ");
        emc1046_convert_temp(hbyte, lbyte);
        break;
    case EXTERNAL_DIODE4:
        hbyte = emc1046_reg_read(i2c_base_addr, 0x41);
        lbyte = emc1046_reg_read(i2c_base_addr, 0x42);
        printf("Temperature of EXTERNAL DIODE0 is ");
        emc1046_convert_temp(hbyte, lbyte);
        break;
    case EXTERNAL_DIODE5:
        hbyte = emc1046_reg_read(i2c_base_addr, 0x43);
        lbyte = emc1046_reg_read(i2c_base_addr, 0x44);
        printf("Temperature of EXTERNAL DIODE0 is ");
        emc1046_convert_temp(hbyte, lbyte);
        break;
    case EXTERNAL_DIODE6:
        hbyte = emc1046_reg_read(i2c_base_addr, 0x45);
        lbyte = emc1046_reg_read(i2c_base_addr, 0x46);
        printf("Temperature of EXTERNAL DIODE0 is ");
        emc1046_convert_temp(hbyte, lbyte);
        break;
    }
}

int wait_convertion_done(unsigned int i2c_base_addr, int timeout_ms)
{
    while (timeout_ms-- > 0) {
        hal_delay_us(1000);
        if ((emc1046_reg_read(i2c_base_addr, 0x02) & 0x80) == 1)
            return 0;
    }
    printf("Temperature convertion time out!!\n");
    return 1;
}

int i2c_device_id_check_emc1046(unsigned int i2c_base_addr)
{
    unsigned int reg_data = 0;
    printf("\tTest EMC1046 Device ID ");
    i2c_init(i2c_base_addr, 170000);
    reg_data = emc1046_reg_read(i2c_base_addr, EMC1046_REG_DEVICE_ID_OFF);

    if (0x1A == reg_data) {
        printf("ID read passed 0x%02X\n", reg_data);
        emc1046_reg_write(i2c_base_addr, 0x03, 0x40);   //set sensor in ACTIVE mode
        if (wait_convertion_done(i2c_base_addr, 1000) == 1)
            return 1;
        emc1046_report_temp(i2c_base_addr, INTERNAL_DIODE);
        emc1046_report_temp(i2c_base_addr, EXTERNAL_DIODE1);
        emc1046_report_temp(i2c_base_addr, EXTERNAL_DIODE2);
        emc1046_report_temp(i2c_base_addr, EXTERNAL_DIODE3);
        emc1046_report_temp(i2c_base_addr, EXTERNAL_DIODE4);
        emc1046_report_temp(i2c_base_addr, EXTERNAL_DIODE5);
        emc1046_report_temp(i2c_base_addr, EXTERNAL_DIODE6);
        return 0;
    } else {
        printf("failed, 0x1A vs 0x%02X\n", reg_data);
        return 1;
    }
}

/*!
 * @return      TEST_PASSED or  TEST_FAILED    
 */
test_return_t i2c_device_emc1046_test(void)
{
//    const char * indent = menu_get_indent();

#if defined(BOARD_SMART_DEVICE)
    //  USB_OTG_PWR_EN (EIM_D22)
    writel(ALT5, IOMUXC_SW_MUX_CTL_PAD_EIM_EB3);
    gpio_set_direction(GPIO_PORT2, 31, GPIO_GDIR_OUTPUT);
    gpio_set_level(GPIO_PORT2, 31, GPIO_LOW_LEVEL);
    hal_delay_us(1000);
    gpio_set_level(GPIO_PORT2, 31, GPIO_HIGH_LEVEL);
#endif    

    return i2c_device_id_check_emc1046(I2C3_BASE_ADDR);
}

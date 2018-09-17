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

//#include "imx_i2c.h"
#include "obds.h"
#include "registers/regsi2c.h"

const char g_hs_tsc_p1003_i2c_device_id_test_name[] = "HannStar TSC P1003 I2C Device ID Test";

extern const i2c_device_info_t g_p1003_tsc_i2c_device;
int p1003_show_touch(unsigned int i2c_port, const char* indent);

unsigned char p1003_version_reg_read(unsigned int i2c_port, unsigned char reg_addr,
                                     unsigned char *ret_val)
{
    struct imx_i2c_request rq = {0};

    rq.ctl_addr = REGS_I2C_BASE(i2c_port);
    rq.dev_addr = g_p1003_tsc_i2c_device.address; // 4
    rq.reg_addr = reg_addr;
    rq.reg_addr_sz = 1;
    rq.buffer_sz = 10;
    rq.buffer = ret_val;

    if (i2c_xfer(&rq, I2C_READ) != 0) {
        printf("%s() error. return\n", __FUNCTION__);
        return -1;
    }

    return 0;
}

unsigned char p1003_version_reg_write(unsigned int i2c_port, unsigned char reg_addr,
                                     unsigned char *buffer)
{
	struct imx_i2c_request rq = {0};
    rq.ctl_addr = REGS_I2C_BASE(i2c_port);
    rq.dev_addr = g_p1003_tsc_i2c_device.address; // 4
    rq.reg_addr = reg_addr;
    rq.reg_addr_sz = 1;
    rq.buffer_sz = 2 + buffer[1];
    rq.buffer = buffer;

    if (i2c_xfer(&rq, I2C_WRITE) != 0) {
        printf("%s() error. return\n", __FUNCTION__);
        return -1;
    }

    return 0;
}

/*!
 * @return      TEST_PASSED or  TEST_FAILED
 */
test_return_t i2c_device_id_check_p1003(void)
{
    const char* indent = menu_get_indent();
    //TO be confirmed - i2c-base_addr
    unsigned int i2c_port = g_p1003_tsc_i2c_device.port;
    i2c_init(i2c_port, g_p1003_tsc_i2c_device.freq);
    hal_delay_us(10000);

	unsigned char ret_buf1[10] = {0};

	// read firmware version
	unsigned char buf[] = {0x03, 0x03, 0x0A, 0x01, 'D', 0, 0, 0, 0, 0};
    if (p1003_version_reg_write(i2c_port, 0, buf) != 0) {
        printf("%s  failed to send get_hanstar_firmware_version command.\n", indent);
        return TEST_FAILED;
    } else {
		if (p1003_version_reg_read(i2c_port, 0, ret_buf1) != 0) {
			printf("%s  failed to read 1st HannStar firmware version packet\n", indent);
			return TEST_FAILED;
		}
		printf("%sHannStar firmware version: 0x%02X\n", indent, ret_buf1[5]);
    }

	printf("%s  Read HannStar firmware version passed.\n", indent);
    p1003_show_touch(i2c_port, indent);
    return TEST_PASSED;
}

typedef struct {
    short accel_x;
    short accel_y;
    short accel_z;
} Accel, *pAccel;

#define REPORTID_MTOUCH 0x04

int p1003_get_touch(unsigned int i2c_port, Accel * acc, const char* indent)
{

	unsigned char ret_buf[10] = {0};
	char status = 0;
	char untouched_count = 0;

	do {
		if (p1003_version_reg_read(i2c_port, 0, ret_buf) != 0) {
			printf("%s  failed to read HannStar position packet\n", indent);
			return 1;
		}

		status = ret_buf[1] & 0x01;
		if ( status )
			untouched_count = 0;
		else
			++untouched_count;

	} while ((ret_buf[0] != REPORTID_MTOUCH) && (untouched_count < 10));

	char contactID = (ret_buf[1] & 0x7C) >> 2;
	static short x0 = 0, y0 = 0, x1 = 0, y1 = 0;
	if ( contactID == 0)
	{
		if ( status )
		{
			x0 = (((short)(ret_buf[3])<<8) + (short)ret_buf[2]) >> 4;
			y0 = (((short)(ret_buf[5])<<8) + (short)ret_buf[4]) >> 4;
		}
		else
		{
			x0 = y0 = 0;
		}
	}
	else
	{
		if ( status )
		{
			x1 = (((short)(ret_buf[3])<<8) + (short)ret_buf[2]) >> 4;
			y1 = (((short)(ret_buf[5])<<8) + (short)ret_buf[4]) >> 4;
		}
		else
		{
			x1 = y1 = 0;
		}

	}
	printf("\r%s  The touch data is: contact 0: X = %d, Y = %d contact 1: X = %d, Y = %d          ", indent, x0, y0, x1, y1);
//	  printf("0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X\n",   ret_buf[0], ret_buf[1], ret_buf[2], ret_buf[3], ret_buf[4], ret_buf[5], ret_buf[6], ret_buf[7], ret_buf[8], ret_buf[9]);

    return 0;
}

int p1003_show_touch(unsigned int i2c_port, const char* indent)
{
    unsigned char uc = NONE_CHAR;
    Accel acc;
    int fail_count = 0;

    printf("%sDo you want to check the touch screen?(y/n)\n\n", indent);

    do {
        uc = getchar();
    } while (uc == NONE_CHAR);

    if (uc == 'y' || uc == 'Y')
    {
        printf("    Start show touch screen. Type 'x' to exit.\n\n");
        while (1) {
            fail_count = 0;
            while(p1003_get_touch(i2c_port, &acc, indent))
            {
                if(++fail_count > 10) // if failed retry to read the coordination
                    return 1;
            } 
            
            uc = getchar();
            if (uc == 'x' || uc == 'X') {
                printf("\n\n");
                break;
            }
            hal_delay_us(10000);
        }
    } else {
        return 1;
    }
    return 0;
}

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

#include "sdk.h"
#include "siHdmiTx_902x_TPI.h"

unsigned int hdmi_i2c_base = 0;

int sil9024_write_reg(unsigned short regAddrs, unsigned char *regVal, int data_len)
{
    int retVal = 0;
    struct imx_i2c_request rq = {0};
    rq.ctl_addr = hdmi_i2c_base;
    rq.dev_addr = 0x72 >> 1;
    rq.reg_addr = regAddrs;
    rq.reg_addr_sz = 1;
    rq.buffer_sz = data_len;
    rq.buffer = regVal;
    retVal = i2c_xfer(&rq, I2C_WRITE);
    return retVal;
}

int sil9024_read_reg(unsigned short regAddrs, unsigned char *regVal, int data_len)
{
    int retVal = 0;
    struct imx_i2c_request rq = {0};
    rq.ctl_addr = hdmi_i2c_base;
    rq.dev_addr = 0x72 >> 1;
    rq.reg_addr = regAddrs;
    rq.reg_addr_sz = 1;
    rq.buffer_sz = data_len;
    rq.buffer = regVal;
    retVal = i2c_xfer(&rq, I2C_READ);

    return retVal;
}

int sil9024_reg_check(unsigned int reg, unsigned char data_input, unsigned char data_back)
{
    int ret_val = 0;
    ret_val = sil9024_read_reg(reg, &data_back, 1);

    if (data_input == data_back) {
        return 0;
    } else {
        printf("data invalid in reg@0x%02x: 0x%x VS 0x%x\n", reg, data_input, data_back);
        return -1;
    }

    return ret_val;
}

extern int ips_ext_hdmi_transmitter_1080P60_stream(int ipu_index);
int config_hdmi_si9022(int ipu_index, int di_port)
{
    int retVal = 0;
    int hdmi_mode = SHDMI_1080P60;
    int hpd_time_ms = 300;

    ips_ext_hdmi_transmitter_1080P60_stream(ipu_index);

    /*config SiI9024 HDMI Tx through I2C */
    hdmi_i2c_base = I2C3_BASE_ADDR;
    retVal = i2c_init(hdmi_i2c_base, 97000);
    if (retVal != 0) {
        printf("I2C initialization failed!\n");
        return -1;
    }

    /*Ray's note: decrease the delay time */
    siHdmiTx_TPI_Init();
    siHdmiTx_VideoSel(hdmi_mode);
    siHdmiTx_AudioSel(AFS_48K);
    if (!siHdmiTx_TPI_Poll(hpd_time_ms))
        return 0;
    else
        return -1;
}

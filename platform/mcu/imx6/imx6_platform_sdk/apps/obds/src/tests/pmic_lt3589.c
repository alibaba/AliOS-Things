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

#define PMIC_LTC3589_I2C_NUM        1   // Which I2C used to interface with the PMIC; 0=I2C1, 1=I2C2
#define PMIC_LTC3589_I2C_REG_BYTE   0x1 // Number of Bytes to transfer the PMIC reg number
#define PMIC_LTC3589_I2C_DATA_BYTE  0x1 // Number of Bytes to transfer the PMIC reg data

#define PGSTAT_REG 0x13

static const char * const test_name = "PMIC_LT3589 Test";

unsigned char ltc3589_reg_read(unsigned char reg_addr)
{
    struct imx_i2c_request rq = {0};
    unsigned char buf[1];
    unsigned char reg_data = 0;

    rq.ctl_addr = g_pmic_ltc3589_i2c_device.port;
    rq.dev_addr = g_pmic_ltc3589_i2c_device.address;
    rq.reg_addr = reg_addr;
    rq.reg_addr_sz = 1;
    rq.buffer_sz = 1;
    rq.buffer = buf;
    i2c_xfer(&rq, I2C_READ);
    reg_data = buf[0];
    return reg_data;
}

void ltc3589_reg_write(unsigned char reg_addr, unsigned char reg_data)
{
    struct imx_i2c_request rq = {0};
    rq.ctl_addr = g_pmic_ltc3589_i2c_device.port;
    rq.dev_addr = g_pmic_ltc3589_i2c_device.address;
    rq.reg_addr = reg_addr;
    rq.reg_addr_sz = 1;
    rq.buffer_sz = 1;
    rq.buffer = &reg_data;
    i2c_xfer(&rq, I2C_WRITE);
}

void ltc3589_eval(void)
{
    struct imx_i2c_request rq = {0};
    unsigned char buf[4];
    int rc = 0;
    unsigned char byteOutData;
    uint8_t recvCh;
    i2c_init(g_pmic_ltc3589_i2c_device.port, g_pmic_ltc3589_i2c_device.freq);
    
    /* Initialize some of the I2C imx_i2c_request structure, these parameters shouldn't need to be changed */
    rq.ctl_addr = g_pmic_ltc3589_i2c_device.port;
    rq.dev_addr = g_pmic_ltc3589_i2c_device.address;
    rq.reg_addr_sz = PMIC_LTC3589_I2C_REG_BYTE;
    rq.buffer_sz = PMIC_LTC3589_I2C_DATA_BYTE;
    /* Read BUCKCORE register */
    rq.reg_addr = 46;
    rq.buffer = buf;
    rc = i2c_xfer(&rq, I2C_READ);

    if (rc != 0) {
        printf("I2C bus error, rc= %d\n", rc);
        return;
    }

    printf(" Initial BUCKCORE (VDDGP) register read is: 0x%x\n", buf[0]);

    if (buf[0] != 0x56)
        printf("  Error, BUCKCORE should be 0x56 which translates to 1.05V \n");
    else
        printf("  BUCKCORE read back correct,  translates to 1.05V \n");

    /* Read LDO6 register */
    rq.reg_addr = 55;
    rq.buffer = buf;
    rc = i2c_xfer(&rq, I2C_READ);

    if (rc != 0) {
        printf("I2C bus error, rc= %d\n", rc);
        return;
    }

    printf(" Initial LDO6 (VDDAL1) register read is: 0x%x\n", buf[0]);

    if (buf[0] != 0x40)
        printf("  Error, LDO6 should be 0x40 which translates to 1.2V \n");
    else
        printf("  LDO6 read back correct,  translates to 1.2V \n");

    /* Read BUCKPRO register */
    rq.reg_addr = 47;
    rq.buffer = buf;
    rc = i2c_xfer(&rq, I2C_READ);

    if (rc != 0) {
        printf("I2C bus error, rc= %d\n", rc);
        return;
    }

    printf(" Initial BUCKPRO (VCC) register read is: 0x%x\n", buf[0]);

    if (buf[0] != 0x5D)
        printf("  Error, BUCKPRO should be 0x5D which translates to 1.225V \n");
    else
        printf("  BUCKPRO read back correct,  translates to 1.225V \n");

    /* Read LDO1 register */
    rq.reg_addr = 50;
    rq.buffer = buf;
    rc = i2c_xfer(&rq, I2C_READ);

    if (rc != 0) {
        printf("I2C bus error, rc= %d\n", rc);
        return;
    }

    printf(" Initial LDO1 (SRTC) register read is: 0x%x\n", buf[0]);

    if (buf[0] != 0x4C)
        printf("  Error, LDO1 should be 0x4C which translates to 1.2V \n");
    else
        printf("  LDO1 read back correct,  translates to 1.2V \n");

    /* BUCKCORE test, increase voltage */
    byteOutData = 0x58;
    rq.reg_addr = 46;
    rq.buffer = &byteOutData;
    rc = i2c_xfer(&rq, I2C_WRITE);

    if (rc != 0) {
        printf("I2C bus error, rc= %d\n", rc);
        return;
    }

    /* read back BUCKCORE register */
    rq.reg_addr = 46;
    rq.buffer = buf;
    rc = i2c_xfer(&rq, I2C_READ);

    if (rc != 0) {
        printf("I2C bus error, rc= %d\n", rc);
        return;
    }

    printf("\n BUCKCORE register read after programming is: 0x%x\n", buf[0]);
    /* Once the BUCKCORE register has been written the next step is to assert the VB_CORE_GO bit in the
     * Supply register(R60)
     */
    byteOutData = 0x61;
    rq.reg_addr = 60;
    rq.buffer = &byteOutData;
    rc = i2c_xfer(&rq, I2C_WRITE);

    if (rc != 0) {
        printf("I2C bus error, rc= %d\n", rc);
        return;
    }

    printf(" Probe VDDGP, it should read 1.10V, does it? (y/n) \n");

    do {
        recvCh = getchar();
    }
    while (recvCh == NONE_CHAR);

    if ((recvCh == 'y') || (recvCh == 'Y')) {
        printf("  BUCKCORE (VDDGP) voltage increased correctly \n");
    } else {
        printf("  BUCKORE voltage not raised correctly \n");
    }

    /* Set BUCKCORE back to original value */
    byteOutData = 0x56;
    rq.reg_addr = 46;
    rq.buffer = &byteOutData;
    rc = i2c_xfer(&rq, I2C_WRITE);

    if (rc != 0) {
        printf("I2C bus error, rc= %d\n", rc);
        return;
    }

    /* Once the BUCKCORE register has been written the next step is to assert the VB_CORE_GO bit in the
     * Supply register(R60)
     */
    byteOutData = 0x61;
    rq.reg_addr = 60;
    rq.buffer = &byteOutData;
    rc = i2c_xfer(&rq, I2C_WRITE);

    if (rc != 0) {
        printf("I2C bus error, rc= %d\n", rc);
        return;
    }

    printf(" Now re-probe VDDGP, it should read 1.05V, does it? (y/n) \n");

    do {
        recvCh = getchar();
    }
    while (recvCh == NONE_CHAR);

    if ((recvCh == 'y') || (recvCh == 'Y')) {
        printf("  BUCKCORE (VDDGP) voltage decreased correctly \n");
    } else {
        printf("  BUCKCORE voltage not decreased correctly \n");
    }

    /******/
    /* BUCKPRO test, increase voltage */
    byteOutData = 0x5F;
    rq.reg_addr = 47;
    rq.buffer = &byteOutData;
    rc = i2c_xfer(&rq, I2C_WRITE);

    if (rc != 0) {
        printf("I2C bus error, rc= %d\n", rc);
        return;
    }

    /* read back BUCKPRO register */
    rq.reg_addr = 47;
    rq.buffer = buf;
    rc = i2c_xfer(&rq, I2C_READ);

    if (rc != 0) {
        printf("I2C bus error, rc= %d\n", rc);
        return;
    }

    printf("\n BUCKPRO register read after programming is: 0x%x\n", buf[0]);
    /* Once the BUCKPRO register has been written the next step is to assert the VB_PRO_GO bit in the
     * Supply register(R60)
     */
    byteOutData = 0x62;
    rq.reg_addr = 60;
    rq.buffer = &byteOutData;
    rc = i2c_xfer(&rq, I2C_WRITE);

    if (rc != 0) {
        printf("I2C bus error, rc= %d\n", rc);
        return;
    }

    printf(" Probe VCC, it should read 1.275V, does it? (y/n) \n");

    do {
        recvCh = getchar();
    }
    while (recvCh == NONE_CHAR);

    if ((recvCh == 'y') || (recvCh == 'Y')) {
        printf("  BUCKPRO (VCC) voltage increased correctly \n");
    } else {
        printf("  BUCKPRO voltage not raised correctly \n");
    }

    /* Set BUCKPRO back to original value */
    byteOutData = 0x5D;
    rq.reg_addr = 47;
    rq.buffer = &byteOutData;
    rc = i2c_xfer(&rq, I2C_WRITE);

    if (rc != 0) {
        printf("I2C bus error, rc= %d\n", rc);
        return;
    }

    /* Once the BUCKPRO register has been written the next step is to assert the VB_CORE_GO bit in the
     * Supply register(R60)
     */
    byteOutData = 0x62;
    rq.reg_addr = 60;
    rq.buffer = &byteOutData;
    rc = i2c_xfer(&rq, I2C_WRITE);

    if (rc != 0) {
        printf("I2C bus error, rc= %d\n", rc);
        return;
    }

    printf(" Now re-probe VCC, it should read 1.225V, does it? (y/n) \n");

    do {
        recvCh = getchar();
    }
    while (recvCh == NONE_CHAR);

    if ((recvCh == 'y') || (recvCh == 'Y')) {
        printf("  BUCKPRO (VCC) voltage decreased correctly \n");
    } else {
        printf("  BUCKPRO voltage not decreased correctly \n");
    }

    /******/
    /* LDO6 test, increase voltage */
    byteOutData = 0x41;
    rq.reg_addr = 55;
    rq.buffer = &byteOutData;
    rc = i2c_xfer(&rq, I2C_WRITE);

    if (rc != 0) {
        printf("I2C bus error, rc= %d\n", rc);
        return;
    }

    /* read back LDO6 register */
    rq.reg_addr = 55;
    rq.buffer = buf;
    rc = i2c_xfer(&rq, I2C_READ);

    if (rc != 0) {
        printf("I2C bus error, rc= %d\n", rc);
        return;
    }

    printf("\n LD06 register read after programming is: 0x%x\n", buf[0]);
    printf(" Probe VDDAL1, it should read 1.25V, does it? (y/n) \n");

    do {
        recvCh = getchar();
    }
    while (recvCh == NONE_CHAR);

    if ((recvCh == 'y') || (recvCh == 'Y')) {
        printf("  LDO6 (VDDAL1) voltage increased correctly \n");
    } else {
        printf("  LDO6 voltage not raised correctly \n");
    }

    /* Set LDO6 back to original value */
    byteOutData = 0x40;
    rq.reg_addr = 55;
    rq.buffer = &byteOutData;
    rc = i2c_xfer(&rq, I2C_WRITE);

    if (rc != 0) {
        printf("I2C bus error, rc= %d\n", rc);
        return;
    }

    printf(" Now re-probe VDDAL1, it should read 1.2V, does it? (y/n) \n");

    do {
        recvCh = getchar();
    }
    while (recvCh == NONE_CHAR);

    if ((recvCh == 'y') || (recvCh == 'Y')) {
        printf("  LDO6 (VDDAL1) voltage decreased correctly \n");
    } else {
        printf("  LDO6 voltage not decreased correctly \n");
    }
}

void ltc3589_backlight_en(void)
{
    struct imx_i2c_request rq = {0};
    int rc = 0;
    unsigned char byteOutData;

    i2c_init(g_pmic_ltc3589_i2c_device.port, g_pmic_ltc3589_i2c_device.freq);
    
    /* Initialize some of the I2C imx_i2c_request structure, these parameters shouldn't need to be changed */
    rq.ctl_addr = g_pmic_ltc3589_i2c_device.port;
    rq.dev_addr = g_pmic_ltc3589_i2c_device.address;
    rq.reg_addr_sz = PMIC_LTC3589_I2C_REG_BYTE;
    rq.buffer_sz = PMIC_LTC3589_I2C_DATA_BYTE;
    /* Write to R73 with value of 0xAF */
    byteOutData = 0xAF;
    rq.reg_addr = 73;
    rq.buffer = &byteOutData;
    rc = i2c_xfer(&rq, I2C_WRITE);

    if (rc != 0) {
        printf("I2C bus error, rc= %d\n", rc);
        return;
    }

    /* Write to R74 with value of 0xAF */
    byteOutData = 0xAF;
    rq.reg_addr = 74;
    rq.buffer = &byteOutData;
    rc = i2c_xfer(&rq, I2C_WRITE);

    if (rc != 0) {
        printf("I2C bus error, rc= %d\n", rc);
        return;
    }

    /* Write to R75 with value of 0xAF */
    byteOutData = 0xAF;
    rq.reg_addr = 75;
    rq.buffer = &byteOutData;
    rc = i2c_xfer(&rq, I2C_WRITE);

    if (rc != 0) {
        printf("I2C bus error, rc= %d\n", rc);
        return;
    }

    /* Write to R70 with value of 0x3F to enable */
    byteOutData = 0x3F;
    rq.reg_addr = 70;
    rq.buffer = &byteOutData;
    rc = i2c_xfer(&rq, I2C_WRITE);

    if (rc != 0) {
        printf("I2C bus error, rc= %d\n", rc);
        return;
    }
}

void ltc3589_camera_power_on(void)
{
    struct imx_i2c_request rq = {0};
    int rc = 0;
    unsigned char byteOutData;
    i2c_init(g_pmic_ltc3589_i2c_device.port, g_pmic_ltc3589_i2c_device.freq);
    
    /* Initialize some of the I2C imx_i2c_request structure, these parameters shouldn't need to be changed */
    rq.ctl_addr = g_pmic_ltc3589_i2c_device.port;
    rq.dev_addr = g_pmic_ltc3589_i2c_device.address;
    rq.reg_addr_sz = PMIC_LTC3589_I2C_REG_BYTE;
    rq.buffer_sz = PMIC_LTC3589_I2C_DATA_BYTE;
    /* Write to R56 for 2V75 LDO7 output */
    byteOutData = 0x5F;
    rq.reg_addr = 56;
    rq.buffer = &byteOutData;
    rc = i2c_xfer(&rq, I2C_WRITE);

    if (rc != 0) {
        printf("I2C bus error, rc= %d\n", rc);
        return;
    }

    /* Write to R57 for 1V8 LDO8 output */
    byteOutData = 0x4C;
    rq.reg_addr = 57;
    rq.buffer = &byteOutData;
    rc = i2c_xfer(&rq, I2C_WRITE);

    if (rc != 0) {
        printf("I2C bus error, rc= %d\n", rc);
        return;
    }

    /* Write to R58 for 1V5 LDO9 output */
    byteOutData = 0x45;
    rq.reg_addr = 58;
    rq.buffer = &byteOutData;
    rc = i2c_xfer(&rq, I2C_WRITE);

    if (rc != 0) {
        printf("I2C bus error, rc= %d\n", rc);
        return;
    }
}

void sil9024_hdmi_power_on1(void)
{
    struct imx_i2c_request rq = {0};
    int rc = 0;
    unsigned char byteOutData;
    i2c_init(g_pmic_ltc3589_i2c_device.port, g_pmic_ltc3589_i2c_device.freq);
    
    /* Initialize some of the I2C imx_i2c_request structure, these parameters shouldn't need to be changed */
    rq.ctl_addr = g_pmic_ltc3589_i2c_device.port;
    rq.dev_addr = g_pmic_ltc3589_i2c_device.address;
    rq.reg_addr_sz = PMIC_LTC3589_I2C_REG_BYTE;
    rq.buffer_sz = PMIC_LTC3589_I2C_DATA_BYTE;
    /* Write to R55 for 1V2 LDO6 output */
    byteOutData = 0x40;
    rq.reg_addr = 55;
    rq.buffer = &byteOutData;
    rc = i2c_xfer(&rq, I2C_WRITE);

    if (rc != 0) {
        printf("I2C bus error, rc= %d\n", rc);
        return;
    }
}

int ltc3589_i2c_device_id_check(void)
{
    unsigned char data;

    i2c_init(g_pmic_ltc3589_i2c_device.port, g_pmic_ltc3589_i2c_device.freq);

#ifdef UNDER_REDESIGN
    for (ret = 0; ret < 0x30; ret++) {
        printf("reg0x%02X = 0x%02X\n", ret, ltc3589_reg_read(ret));
    }

    data = ltc3589_reg_read(0x10);
    printf("OVEN register 0x10 value is: 0x%02X\n", data);

    printf("Enabling everything except LDO3 for testing as it is not used\n");
    ltc3589_reg_write(0x10, 0x5F);
    ltc3589_reg_write(0x10, 0x5F | 0x80);
    data = ltc3589_reg_read(0x10);
    printf("OVEN register 0x10 value is: 0x%02X\n", data);
#endif

    /* There's no ID register, so read PGSTAT register instead */
    /* It should show that all supplies are ready */
    /* LTC3589 should be 0xFF */
    /* LTC3589-1 should be 0x7F  (LDO4 is off by default) */
    data = 0x0;
    data = ltc3589_reg_read(PGSTAT_REG);
    if ((data != 0xFF) && (data != 0x7F)) {
        printf("Expected id 0xFF (LTC3589) or 0x7F (LTC3589-1), read 0x%X\n", data);
        return TEST_FAILED;
    }

    printf(" PMIC LTC3589 test passed \n");
    return TEST_PASSED;
}

/*!
 * @return      TEST_PASSED or  TEST_FAILED    
 */
test_return_t pmic_lt3589_test(void)
{
    if ( prompt_run_test(test_name, NULL) != TEST_CONTINUE )
        return TEST_BYPASSED;

    return ltc3589_i2c_device_id_check();
}


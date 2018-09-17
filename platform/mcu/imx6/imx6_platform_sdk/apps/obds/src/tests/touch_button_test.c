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
#if defined(BOARD_SMART_DEVICE)

#include "obds.h"

#define TOUCHBUTTON_I2C_ADDR    0x5A

const char g_touch_buttons_test_name[] = "TOUCH BUTTONS Test";

static int touch_button_reg_write(unsigned int i2c_base_addr, unsigned char reg_addr,
                                  unsigned char reg_val)
{
    struct imx_i2c_request rq = {0};
    unsigned char buf[1];

    buf[0] = reg_val;
    rq.ctl_addr = i2c_base_addr;
    rq.dev_addr = TOUCHBUTTON_I2C_ADDR;
    rq.reg_addr = reg_addr;
    rq.reg_addr_sz = 1;
    rq.buffer_sz = 1;
    rq.buffer = buf;

    return i2c_xfer(&rq, I2C_WRITE);

//    return i2c_xfer(i2c_base_addr, &rq, I2C_WRITE);
}

static unsigned char touch_button_reg_read(unsigned int i2c_base_addr, unsigned char reg_addr)
{
    struct imx_i2c_request rq = {0};
    unsigned char buf[1];
    unsigned char reg_data = 0;

    rq.ctl_addr = i2c_base_addr;
    rq.dev_addr = TOUCHBUTTON_I2C_ADDR;
    rq.reg_addr = reg_addr;
    rq.reg_addr_sz = 1;
    rq.buffer_sz = 1;
    rq.buffer = buf;
    i2c_xfer(&rq, I2C_READ);
//    i2c_xfer(i2c_base_addr, &rq, I2C_READ);
    reg_data = buf[0];

    return reg_data;
}

int i2c_device_id_check_touch_button(unsigned int i2c_base_addr)
{
    int ret;
    unsigned int reg_data = 0;

    printf("\nTest Touch Button Device ID ");
    ret = i2c_init(i2c_base_addr, 170000);

    if (ret != 0) {
        printf("I2C2 initialization failed!\n");
        return ret;
    }

    reg_data = touch_button_reg_read(i2c_base_addr, 0x00);  //read  WHO_AM_I reg

    if (0x00 == reg_data) {
        printf("passed 0x%02X\n", reg_data);
        return 0;
    } else {
        printf("failed, 0x1A vs 0x%02X\n", reg_data);
        return 1;
    }
}

/* Touch Button Initialization 
    1. Configure collector interrupt pin (IRQ) throught KEY_INT
    2. Configure MPR121 through I2C
  */
static int touch_button_init(unsigned int i2c_base_addr)
{
    int ret = 0;

#if defined (CHIP_MX6DQ)
    int tmp = 0;
    /* MS61_SMART_DEVICE touch button IRQ through GPIO2_1 */
    writel(0x5, IOMUXC_SW_MUX_CTL_PAD_NANDF_D1);    // reg32_write(IOMUXC_SW_MUX_CTL_PAD_NANDF_D1, 0x5);
    //reg32clrbit(GPIO2_BASE_ADDR + 0x0004, 1);   // set GPIO as input
    tmp = readl(GPIO2_BASE_ADDR + 0x0004);
    writel(tmp & (~(1 << 1)), GPIO2_BASE_ADDR + 0x0004);

    writel(GPIO_ICR_FALL_EDGE << (1 * 2), GPIO2_BASE_ADDR + GPIO_ICR1_OFFSET);  // GPIOs 4 use ICR1 reg
    tmp = readl(GPIO2_BASE_ADDR + GPIO_IMR_OFFSET);
    writel(tmp | (1 << 1), GPIO2_BASE_ADDR + GPIO_IMR_OFFSET);

    // w1c, clear initial status in-case set
    writel((1 << 1), GPIO2_BASE_ADDR + GPIO_ISR_OFFSET);
#endif

#if defined(CHIP_MX53)
    int tmp = 0;
    /* MS53_SMD camera reset through GPIO5_4 */
    writel(0x1, IOMUXC_SW_MUX_CTL_PAD_EIM_A24); // reg32_write(IOMUXC_SW_MUX_CTL_PAD_EIM_A24, 0x1);
    //reg32clrbit(GPIO5_BASE_ADDR + 0x0004, 4);   // set GPIO as input
    tmp = readl(GPIO5_BASE_ADDR + 0x0004);
    writel(tmp & (~(1 << 4)), GPIO5_BASE_ADDR + 0x0004);
    writel(GPIO_ICR_FALL_EDGE << (4 * 2), GPIO5_BASE_ADDR + GPIO_ICR1_OFFSET);  // GPIOs 4 use ICR1 reg
    tmp = readl(GPIO5_BASE_ADDR + GPIO_IMR_OFFSET);
    writel(tmp | (1 << 4), GPIO5_BASE_ADDR + GPIO_IMR_OFFSET);

    // w1c, clear initial status in-case set
    writel((1 << 4), GPIO5_BASE_ADDR + GPIO_ISR_OFFSET);
#endif

    ret = i2c_init(i2c_base_addr, 170000);

    if (ret != 0) {
        printf("I2C2 initialization failed!\n");
        return ret;
    }

    touch_button_reg_write(i2c_base_addr, 0x2B, 0x01);  // MHD Rising
    touch_button_reg_write(i2c_base_addr, 0x2C, 0x01);  // NHD Amount Rising
    touch_button_reg_write(i2c_base_addr, 0x2D, 0x00);  // NCL Rising
    touch_button_reg_write(i2c_base_addr, 0x2E, 0x00);  // FDL Rising
    touch_button_reg_write(i2c_base_addr, 0x2F, 0x01);  // MHD Falling
    touch_button_reg_write(i2c_base_addr, 0x30, 0x01);  // MHD Amount Falling
    touch_button_reg_write(i2c_base_addr, 0x31, 0xFF);  // NCL Falling
    touch_button_reg_write(i2c_base_addr, 0x32, 0x02);  // FDL Falling

    touch_button_reg_write(i2c_base_addr, 0x41, 0x0F);  // ELE0 Touch Threshold
    touch_button_reg_write(i2c_base_addr, 0x42, 0x0A);  // ELE0 Release Threshold
    touch_button_reg_write(i2c_base_addr, 0x43, 0x0F);  // ELE1 Touch Threshold
    touch_button_reg_write(i2c_base_addr, 0x44, 0x0A);  // ELE1 Release Threshold
    touch_button_reg_write(i2c_base_addr, 0x45, 0x0F);  // ELE2 Touch Threshold
    touch_button_reg_write(i2c_base_addr, 0x46, 0x0A);  // ELE2 Release Threshold
    touch_button_reg_write(i2c_base_addr, 0x47, 0x0F);  // ELE3 Touch Threshold
    touch_button_reg_write(i2c_base_addr, 0x48, 0x0A);  // ELE3 Release Threshold

    touch_button_reg_write(i2c_base_addr, 0x5D, 0x04);  // Filter Configuration
    touch_button_reg_write(i2c_base_addr, 0x7B, 0x0B);  // Auto-config control
    touch_button_reg_write(i2c_base_addr, 0x7D, 0x9C);  // Auto-config USL
    touch_button_reg_write(i2c_base_addr, 0x7E, 0x65);  // Auto-config LSL
    touch_button_reg_write(i2c_base_addr, 0x7F, 0x8C);  // Auto-config target level

    touch_button_reg_write(i2c_base_addr, 0x5E, 0x2C);

    return ret;
}

test_return_t touch_button_test(void)
{
    unsigned char pval_old = 0, pval_new;
    unsigned char input;
    const char* const indent = menu_get_indent();
    int i;

    printf("%sPlease press four touch buttons and pressed information will be printed on screen.\n", indent);
    printf("%sPressing any key on the keyboard exits this test\n", indent);

    touch_button_init(I2C2_BASE_ADDR);

    while (1) {
#if defined(CHIP_MX6DQ)
        if ((1 << 1) & readl(GPIO2_BASE_ADDR + GPIO_ISR_OFFSET)) {
#else
        if ((1 << 4) & readl(GPIO5_BASE_ADDR + GPIO_ISR_OFFSET)) {
#endif
            pval_new = touch_button_reg_read(I2C2_BASE_ADDR, 0x00);
            if ((pval_new ^ pval_old) & 0x01)
                printf("%sSEARCH Press\n", indent);
            if ((pval_new ^ pval_old) & 0x02)
                printf("%sBACK Press\n", indent);
            if ((pval_new ^ pval_old) & 0x04)
                printf("%sHOME Press\n", indent);
            if ((pval_new ^ pval_old) & 0x08)
                printf("%sMENU Press\n", indent);

#if defined(CHIP_MX6DQ)
            writel((1 << 1), GPIO2_BASE_ADDR + GPIO_ISR_OFFSET);    // clear ISR status
#else
            writel((1 << 4), GPIO5_BASE_ADDR + GPIO_ISR_OFFSET);    // clear ISR status
#endif
        }
        for (i = 0; i < 20000; i++) ;
        input = getchar();
        if (input != NONE_CHAR) {
            break;
        }
    }
    
    return TEST_PASSED;
}

#endif

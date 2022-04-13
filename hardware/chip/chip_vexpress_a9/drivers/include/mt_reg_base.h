/*
 * Copyright (c) 2019 MediaTek Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once
#define KERNEL_ASPACE_BASE    0xffff000000000000
#define PERIPHERAL_BASE_PHYS (0x10000000)
#define PERIPHERAL_BASE_VIRT (KERNEL_ASPACE_BASE + PERIPHERAL_BASE_PHYS)
#define SRC_CLOCK               (26*1000*1000)

/* I/O mapping */
#define IO_PHYS             PERIPHERAL_BASE_VIRT
#define IO_SIZE             PERIPHERAL_BASE_SIZE
/* IO register definitions */
#define CKSYS_BASE          (IO_PHYS + 0x00000000)
#define INFRACFG_AO_BASE    (IO_PHYS + 0x00001000)
#define SCPSYS_BASE	    (IO_PHYS + 0x00006000)
#define TOP_RGU_BASE        (IO_PHYS + 0x00007000)
#define APMIXED_BASE        (IO_PHYS + 0x0000C000)
#define MCUSYS_CFGREG_BASE  (IO_PHYS + 0x00200000)
#define INT_POL_CTL0        (MCUSYS_CFGREG_BASE + 0xA80)
#define INT_POL_SECCTL0     (MCUSYS_CFGREG_BASE + 0xA00)
#define SEC_POL_CTL_EN0     INT_POL_SECCTL0
#define write_r(a, v) writel(v, a)
#define GPIO_BASE           (IO_PHYS + 0x00005000)
#define EINT_BASE           (IO_PHYS + 0x0000B000)
#define DSPCFG_BASE         (IO_PHYS + 0x00018000)
#define DSP_DTCM_BASE       (KERNEL_ASPACE_BASE + 0x00110000)
#define TRNG_BASE           (IO_PHYS + 0x0020F000)
#define GCPU_BASE           (IO_PHYS + 0x00241000)
#define AUXADC_BASE         (IO_PHYS + 0x01001000)
#define UART0_BASE          (IO_PHYS + 0x01002000)
#define UART1_BASE          (IO_PHYS + 0x01003000)
#define UART2_BASE          (IO_PHYS + 0x01004000)
#define DSP_UART_BASE       (IO_PHYS + 0x00015000)
#define SPIM_BASE			(IO_PHYS + 0x0100A000)
#define SPIS_BASE			(IO_PHYS + 0x0100D000)
#define MSDC0_BASE          (IO_PHYS + 0x01230000)
#define MSDC1_BASE          (IO_PHYS + 0x01240000)
#define MSDC0_TOP_BASE      (IO_PHYS + 0x01C00000)
#define MSDC1_TOP_BASE      (IO_PHYS + 0x01C90000)
#define NFI_BASE	    (IO_PHYS + 0x1018000)
#define NFIECC_BASE	    (IO_PHYS + 0x1019000)
#define IPSYS_BASE	    (IO_PHYS + 0x03002000)
#define MMSYS_CONFIG_BASE   (IO_PHYS + 0x04000000)
#define IMGSYS_CONFIG_BASE  (IO_PHYS + 0x05000000)

#define MTK_I2C0_BASE (IO_PHYS+0x1007000)
#define MTK_I2C1_BASE (IO_PHYS+0x0019000)
#define MTK_I2C2_BASE (IO_PHYS+0x001E000)

#define MTK_I2C0_DMA (IO_PHYS+0x1000080)
#define MTK_I2C1_DMA (IO_PHYS+0x1000100)
#define MTK_I2C2_DMA (IO_PHYS+0x1000180)

#define MTK_I2C_CLK_SET  (IO_PHYS+0x00010C0)
#define MTK_I2C_CLK_CLR  (IO_PHYS+0x00010C4)
#define MTK_I2C_CLK_STA  (IO_PHYS+0x00010C8)

#define MTK_APDMA_CLK_SET (IO_PHYS+0x0001088)
#define MTK_APDMA_CLK_CLR (IO_PHYS+0x000108C)
#define MTK_APDMA_CLK_STA (IO_PHYS+0x0001094)

#define MTK_GPIO_I2C_BASE (IO_PHYS+0x0005240)//i2c0 i2c1 i2c2
#define MTK_GPIO_I2C_PULL_ENABLE_BASE (IO_PHYS+0x0005880)//i2c0 i2c1 i2c2
#define MTK_GPIO_I2C_PULL_SEL_BASE (IO_PHYS+0x0005920)//i2c0 i2c1 i2c2
#define MTK_GPIO_I2C_RESL_BASE (IO_PHYS+0x0005060)//i2c0 i2c1 i2c2

/* GIC */
/*
#define GICD_BASE      (KERNEL_ASPACE_BASE + 0x0C000000)
#define GICR_BASE      (KERNEL_ASPACE_BASE + 0x0C080000)
#define GICR_BASE_PHY  (0x0C080000)
*/

/* PWM */
#define PWM_BASE            (IO_PHYS + 0x0001D000)
#define GCPU_BASE           (IO_PHYS + 0x00241000)


/* SMI-Larb */
#define SMI_LARB0_BASE      (0x14003000)
#define SMI_LARB1_BASE      (0x17010000)
#define SMI_LARB2_BASE      (0x15001000)
#define SMI_LARB3_BASE      (0x16010000)

#define DPI_BASE     (MMSYS_CONFIG_BASE + 0x6000)
#define TCON_TOP_BASE     (MMSYS_CONFIG_BASE + 0xd000)

#define MAX_INT 256

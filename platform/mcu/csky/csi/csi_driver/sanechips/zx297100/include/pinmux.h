/*
 * Copyright (C) 2017 C-SKY Microsystems Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/******************************************************************************
 * @file     pinmux.h
 * @brief    Header file for the pinmux
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/
#ifndef ZX297100_PINMUX_H
#define ZX297100_PINMUX_H

/*******************************************************************************
 *                             Include header files                               *
 ******************************************************************************/
#include <stdint.h>
#include <addrspace.h>

/*******************************************************************************
 *                               Macro definitions                               *
 ******************************************************************************/
#define    TOP_FUNC_SEL_BASE           (PAD_CTRL_AO_BASEADDR)
#define    AON_FUNC_SEL_BASE           (PAD_CTRL_AO_BASEADDR)
#define    PD_FUNC_SEL_BASE            (PAD_CTRL_PD_BASEADDR)
#define    IO_CFG_BASE                 (PAD_CTRL_AO_BASEADDR+0x800)
#define    IO_CFG_PD_BASE              (PAD_CTRL_PD_BASEADDR+0x100)

#define PINMUX_TOP_REG(val)            (TOP_FUNC_SEL_BASE + (val >> 9))
#define PINMUX_AO_REG(val)             (AON_FUNC_SEL_BASE + (val >> 9))
#define PINMUX_PD_REG(val)             (PD_FUNC_SEL_BASE + (val >> 9))
#define PINMUX_IO_CFG_REG(val)         (IO_CFG_BASE + (val >> 9))
#define PINMUX_IO_CFG_PD_REG(val)      (IO_CFG_PD_BASE + (val >> 9))

#define BIT_OFFSET(val)                ((uint8_t)((val >> 4) & 0x1f))
#define BIT_WIDTH(val)                 ((uint8_t)(val & 0xf))

#define FUN_SEL_SHIFT                  (12)
#define GPIO_ID_SHIFT                  (24)
#define FUNC_SEL_MASK                  BIT_MASK(10)


/*[15:9]:reg_addr_offset    [8:4]:reg_bit_offset     [3:0]:reg_bit_size*/
#define PINMUX_FIELD(reg_addr_offset, reg_bit_offset, reg_bit_size) ((uint16_t)((reg_addr_offset << 9) | (reg_bit_offset << 4) | (reg_bit_size) << 0))

/*******************************************************************************
 *                               Type definitions                              *
 ******************************************************************************/
enum top_func
{
    PIN_FUNC_SEL_AON = 0,
    PIN_FUNC_SEL_PD  = 1,
};

struct si_pin_info {
    uint16_t top_sel_field;    /*0--aon func  1--pd func*/
    uint16_t aon_sel_field;
    uint16_t pd_sel_field;
    uint16_t io_cfg_field;
};

/*******************************************************************************
 *                                            Global variable declarations                                      *
 ******************************************************************************/
static const struct si_pin_info pin_info[] =
{
    /*GPIO 0*/
    {
        .top_sel_field   = PINMUX_FIELD(0xc, 0, 1),
        .aon_sel_field   = PINMUX_FIELD(0x0, 0, 2),
        .pd_sel_field    = PINMUX_FIELD(0x4, 0, 3),
        .io_cfg_field    = PINMUX_FIELD(0x4, 0, 4),
    },
    /*GPIO 1*/
    {
         .top_sel_field   = PINMUX_FIELD(0xc, 1, 1),
         .aon_sel_field   = PINMUX_FIELD(0x0, 2, 2),
         .pd_sel_field    = PINMUX_FIELD(0x4, 3, 3),
         .io_cfg_field    = PINMUX_FIELD(0x4, 4, 4),
    },
    /*GPIO 2*/
    {
         .top_sel_field   = PINMUX_FIELD(0xc, 2, 1),
         .aon_sel_field   = PINMUX_FIELD(0x0, 4, 2),
         .pd_sel_field    = PINMUX_FIELD(0x4, 6, 3),
         .io_cfg_field    = PINMUX_FIELD(0x4, 8, 4),
    },
    /*GPIO 3*/
    {
         .top_sel_field   = PINMUX_FIELD(0xc, 3, 1),
         .aon_sel_field   = PINMUX_FIELD(0x0, 6, 2),
         .pd_sel_field    = PINMUX_FIELD(0x4, 9, 3),
         .io_cfg_field    = PINMUX_FIELD(0x4, 12, 4),
    },
    /*GPIO 4*/
    {
         .top_sel_field   = PINMUX_FIELD(0xc, 4, 1),
         .aon_sel_field   = PINMUX_FIELD(0x0, 8, 2),
         .pd_sel_field    = PINMUX_FIELD(0x4, 12, 3),
         .io_cfg_field    = PINMUX_FIELD(0x4, 16, 4),
    },
    /*GPIO 5*/
    {
         .top_sel_field   = PINMUX_FIELD(0xc, 5, 1),
         .aon_sel_field   = PINMUX_FIELD(0x0, 10, 2),
         .pd_sel_field    = PINMUX_FIELD(0x4, 15, 3),
         .io_cfg_field    = PINMUX_FIELD(0x4, 20, 4),
    },
    /*GPIO 6*/
    {
         .top_sel_field   = PINMUX_FIELD(0xc, 6, 1),
         .aon_sel_field   = PINMUX_FIELD(0x0, 12, 2),
         .pd_sel_field    = PINMUX_FIELD(0x4, 18, 3),
         .io_cfg_field    = PINMUX_FIELD(0x4, 24, 4),
    },
    /*GPIO 7*/
    {
        .top_sel_field    = PINMUX_FIELD(0xc, 7, 1),
        .aon_sel_field    = PINMUX_FIELD(0x0, 14, 2),
        .pd_sel_field     = PINMUX_FIELD(0x4, 21, 3),
        .io_cfg_field     = PINMUX_FIELD(0x4, 28, 4),

    },
    /*GPIO 8*/
    {
         .pd_sel_field    = PINMUX_FIELD(0x8, 0, 3),
         .io_cfg_field    = PINMUX_FIELD(0x0, 0, 4),
    },
    /*GPIO 9*/
    {
        .pd_sel_field     = PINMUX_FIELD(0x8, 3, 3),
        .io_cfg_field     = PINMUX_FIELD(0x0, 4, 4),
    },
    /*GPIO 10*/
    {
        .pd_sel_field     = PINMUX_FIELD(0x8, 6, 3),
        .io_cfg_field     = PINMUX_FIELD(0x0, 8, 4),
    },
    /*GPIO 11*/
    {
        .pd_sel_field     = PINMUX_FIELD(0x8, 9, 3),
        .io_cfg_field     = PINMUX_FIELD(0x0, 12, 4),
    },
    /*GPIO 12*/
    {
        .pd_sel_field     = PINMUX_FIELD(0x8, 12, 3),
        .io_cfg_field     = PINMUX_FIELD(0x0, 16, 4),
    },
    /*GPIO 13*/
    {
        .pd_sel_field     = PINMUX_FIELD(0x8, 15, 3),
        .io_cfg_field     = PINMUX_FIELD(0x0, 20, 4),
    },
    /*GPIO 14*/
    {
        .pd_sel_field     = PINMUX_FIELD(0x8, 18, 3),
        .io_cfg_field     = PINMUX_FIELD(0x0, 24, 4),
    },
    /*GPIO 15*/
    {
        .pd_sel_field     = PINMUX_FIELD(0x8, 21, 3),
        .io_cfg_field     = PINMUX_FIELD(0x0, 28, 4),
    },
    /*GPIO 16*/
    {
        .pd_sel_field     = PINMUX_FIELD(0xC, 0, 3),
        .io_cfg_field     = PINMUX_FIELD(0x4, 0, 4),
    },
    /*GPIO 17*/
    {
        .pd_sel_field     = PINMUX_FIELD(0xC, 3, 3),
        .io_cfg_field     = PINMUX_FIELD(0x4, 4, 4),
    },
    /*GPIO 18*/
    {
        .pd_sel_field     = PINMUX_FIELD(0xC, 6, 3),
        .io_cfg_field     = PINMUX_FIELD(0x4, 8, 4),
    },
    /*GPIO 19*/
    {
        .pd_sel_field     = PINMUX_FIELD(0xC, 9, 3),
        .io_cfg_field     = PINMUX_FIELD(0x4, 12, 4),
    },
    /*GPIO 20*/
    {
        .pd_sel_field     = PINMUX_FIELD(0xC, 12, 3),
        .io_cfg_field     = PINMUX_FIELD(0x4, 16, 4),
    },
    /*GPIO 21*/
    {
        .pd_sel_field     = PINMUX_FIELD(0xC, 15, 3),
        .io_cfg_field     = PINMUX_FIELD(0x4, 20, 4),
    },
    /*GPIO 21*/
    {
        .pd_sel_field     = PINMUX_FIELD(0xC, 15, 3),
        .io_cfg_field     = PINMUX_FIELD(0x4, 20, 4),
    },
    /*GPIO 22*/
    {
        .pd_sel_field     = PINMUX_FIELD(0xC, 18, 3),
        .io_cfg_field     = PINMUX_FIELD(0x4, 24, 4),
    },
    /*GPIO 23*/
    {
        .pd_sel_field     = PINMUX_FIELD(0xC, 21, 3),
        .io_cfg_field     = PINMUX_FIELD(0x4, 28, 4),
    },
    /*GPIO 24*/
    {
        .pd_sel_field     = PINMUX_FIELD(0x10, 0, 3),
        .io_cfg_field     = PINMUX_FIELD(0x8, 0, 4),
    },
    /*GPIO 25*/
    {
        .pd_sel_field     = PINMUX_FIELD(0x10, 3, 2),
        .io_cfg_field     = PINMUX_FIELD(0x8, 4, 4),
    },
    /*GPIO 26*/
    {
        .pd_sel_field     = PINMUX_FIELD(0x10, 5, 2),
        .io_cfg_field     = PINMUX_FIELD(0x8, 8, 4),
    },
    /*GPIO 27*/
    {
        .pd_sel_field     = PINMUX_FIELD(0x10, 7, 3),
        .io_cfg_field     = PINMUX_FIELD(0x8, 12, 4),
    },
    /*GPIO 28*/
    {
        .pd_sel_field     = PINMUX_FIELD(0x10, 10, 2),
        .io_cfg_field     = PINMUX_FIELD(0x8, 16, 4),
    },
    /*GPIO 29*/
    {

    },
    /*GPIO 30*/
    {
        .io_cfg_field     = PINMUX_FIELD(0xC, 4, 4),
    },
    /*GPIO 31*/
    {
        .pd_sel_field     = PINMUX_FIELD(0x14, 0, 3),
        .io_cfg_field     = PINMUX_FIELD(0x10, 0, 4),
    },
    /*GPIO 32*/
    {
        .pd_sel_field     = PINMUX_FIELD(0x14, 3, 3),
        .io_cfg_field     = PINMUX_FIELD(0x10, 4, 4),
    },
    /*GPIO 33*/
    {
        .pd_sel_field     = PINMUX_FIELD(0x14, 6, 3),
        .io_cfg_field     = PINMUX_FIELD(0x10, 8, 4),
    },
    /*GPIO 34*/
    {
        .pd_sel_field     = PINMUX_FIELD(0x14, 9, 3),
        .io_cfg_field     = PINMUX_FIELD(0x10, 12, 4),
    },
    /*GPIO 35*/
    {
        .pd_sel_field     = PINMUX_FIELD(0x14, 12, 3),
        .io_cfg_field     = PINMUX_FIELD(0x10, 16, 4),
    },
    /*GPIO 36*/
    {
        .pd_sel_field     = PINMUX_FIELD(0x14, 15, 1),
        .io_cfg_field     = PINMUX_FIELD(0x10, 20, 4),
    },
    /*GPIO 37*/
    {
        .pd_sel_field     = PINMUX_FIELD(0x18, 0, 1),
        .io_cfg_field     = PINMUX_FIELD(0x14, 0, 4),
    },
    /*GPIO 38*/
    {
        .pd_sel_field     = PINMUX_FIELD(0x18, 1, 1),
        .io_cfg_field     = PINMUX_FIELD(0x14, 4, 4),
    },
    /*GPIO 39*/
    {
        .pd_sel_field     = PINMUX_FIELD(0x18, 2, 1),
        .io_cfg_field     = PINMUX_FIELD(0x14, 8, 4),
    },
    /*GPIO 40*/
    {
        .pd_sel_field     = PINMUX_FIELD(0x1c, 0, 2),
        .io_cfg_field     = PINMUX_FIELD(0x18, 0, 4),
    },
    /*GPIO 41*/
    {
        .pd_sel_field     = PINMUX_FIELD(0x1c, 2, 2),
        .io_cfg_field     = PINMUX_FIELD(0x18, 4, 4),
    },
    /*GPIO 42*/
    {

    },
    /*GPIO 43*/
    {

    },
    /*GPIO 44*/
    {
        .pd_sel_field     = PINMUX_FIELD(0x1c, 4, 1),
        .io_cfg_field     = PINMUX_FIELD(0x18, 8, 4),
    },
    /*GPIO 45*/
    {
        .pd_sel_field     = PINMUX_FIELD(0x1c, 5, 1),
        .io_cfg_field     = PINMUX_FIELD(0x18, 12, 4),
    },
    /*GPIO 46*/
    {
        .pd_sel_field     = PINMUX_FIELD(0x1c, 6, 1),
        .io_cfg_field     = PINMUX_FIELD(0x18, 16, 4),
    },
    /*GPIO 47*/
    {
        .pd_sel_field     = PINMUX_FIELD(0x1c, 7, 1),
        .io_cfg_field     = PINMUX_FIELD(0x18, 20, 4),
    },
    /*GPIO 48*/
    {
        .pd_sel_field     = PINMUX_FIELD(0x1c, 8, 1),
        .io_cfg_field     = PINMUX_FIELD(0x1c, 0, 4),
    },
    /*GPIO 49*/
    {
        .pd_sel_field     = PINMUX_FIELD(0x1c, 9, 1),
        .io_cfg_field     = PINMUX_FIELD(0x1c, 4, 4),
    },
    /*GPIO 50*/
    {
        .pd_sel_field     = PINMUX_FIELD(0x1c, 10, 1),
        .io_cfg_field     = PINMUX_FIELD(0x1c, 8, 4),
    },
    /*GPIO 51*/
    {

    },
    /*GPIO 52*/
    {

    },
    /*GPIO 53*/
    {
        .io_cfg_field     = PINMUX_FIELD(0x24, 12, 4),
    },
    /*GPIO 54*/
    {
        .io_cfg_field     = PINMUX_FIELD(0xc, 8, 4),
    },

};

enum gpio_func
{
    /*[31:24]:gpio_id   [23:12]:level1_sel   [11:0]:level2_sel*/
    GPIO0_GPIO0             = 0x00000000,
    GPIO0_AON_TEST_PIN0     = 0x00000001,
    GPIO0_LPUART_CTS        = 0x00000002,
    GPIO0_LPUART_RTS        = 0x00000003,
    GPIO0_I2S_WS            = 0x00001000,
    GPIO0_TIM0_ETR          = 0x00001001,
    GPIO0_UART2_RXD         = 0x00001002,
    GPIO0_UART2_TXD         = 0x00001003,
    GPIO0_UART0_RXD         = 0x00001004,
    GPIO0_UART0_TXD         = 0x00001005,

    GPIO1_GPIO1             = 0x01000000,
    GPIO1_AON_TEST_PIN1     = 0x01000001,
    GPIO1_LPUART_RXD        = 0x01000002,
    GPIO1_LPUART_TXD        = 0x01000003,
    GPIO1_I2S_CLK           = 0x01001000,
    GPIO1_TIM0_CH0          = 0x01001001,
    GPIO1_ADC_TRIG          = 0x01001002,
    GPIO1_DAC_TRIG          = 0x01001003,
    GPIO1_UART0_TXD         = 0x01001004,
    GPIO1_UART0_RXD         = 0x01001005,

    GPIO2_GPIO2             = 0x02000000,
    GPIO2_AON_TEST_PIN2     = 0x02000001,
    GPIO2_LPUART_TXD        = 0x02000002,
    GPIO2_LPUART_RXD        = 0x02000003,
    GPIO2_I2S_DIN           = 0x02001000,
    GPIO2_TIM0_CH1          = 0x02001001,
    GPIO2_DAC_TRIG          = 0x02001002,
    GPIO2_ADC_TRIG          = 0x02001003,
    GPIO2_UART0_CTS         = 0x02001004,
    GPIO2_UART0_RTS         = 0x02001005,

    GPIO3_GPIO3             = 0x03000000,
    GPIO3_AON_TEST_PIN3     = 0x03000001,
    GPIO3_LPUART_RTS        = 0x03000002,
    GPIO3_LPUART_CTS        = 0x03000003,
    GPIO3_I2S_DOUT          = 0x03001000,
    GPIO3_TIM0_CH2          = 0x03001001,
    GPIO3_UART2_TXD         = 0x03001002,
    GPIO3_UART2_RXD         = 0x03001003,
    GPIO3_UART0_RTS         = 0x03001004,
    GPIO3_UART0_CTS         = 0x03001005,

    GPIO4_GPIO4             = 0x04000000,
    GPIO4_AON_TEST_PIN4     = 0x04000001,
    GPIO4_LPTIM_OUT         = 0x04000002,
    GPIO4_SCL1              = 0x04001000,
    GPIO4_TIM0_CH3          = 0x04001001,
    GPIO4_UART0_RXD         = 0x04001002,
    GPIO4_UART0_TXD         = 0x04001003,
    GPIO4_SIRIN             = 0x04001004,

    GPIO5_GPIO5             = 0x05000000,
    GPIO5_AON_TEST_PIN5     = 0x05000001,
    GPIO5_LPTIM_ETR         = 0x05000002,
    GPIO5_SDA1              = 0x05001000,
    GPIO5_TIM0_ETR          = 0x05001001,
    GPIO5_UART0_TXD         = 0x05001002,
    GPIO5_UART0_RXD         = 0x05001003,
    GPIO5_NSIROUT           = 0x05001004,

    GPIO6_GPIO6             = 0x06000000,
    GPIO6_AON_TEST_PIN6     = 0x06000001,
    GPIO6_LPTIM_CH0         = 0x06000002,
    GPIO6_EXT_INT0          = 0x06000003,
    GPIO6_SCL0              = 0x06001000,
    GPIO6_TIM0_CH2          = 0x06001001,
    GPIO6_UART0_CTS         = 0x06001002,
    GPIO6_UART0_RTS         = 0x06001003,
    GPIO6_COMP_OUT          = 0x06001004,

    GPIO7_GPIO7             = 0x07000000,
    GPIO7_AON_TEST_PIN7     = 0x07000001,
    GPIO7_LPTIM_CH1         = 0x07000002,
    GPIO7_EXT_INT1          = 0x07000003,
    GPIO7_SDA0              = 0x07001000,
    GPIO7_TIM0_CH3          = 0x07001001,
    GPIO7_UART0_RTS         = 0x07001002,
    GPIO7_UART0_CTS         = 0x07001003,
    GPIO7_I2S_MCLK          = 0x07001004,

    GPIO8_I2S_WS            = 0x08001000,
    GPIO8_SSP0_CS           = 0x08001001,
    GPIO8_TIM1_ETR          = 0x08001002,
    GPIO8_GPIO8             = 0x08001003,
    GPIO8_DONGLE_SER_I      = 0x08001004,
    GPIO8_ADC_TRIG          = 0x08001005,

    GPIO9_I2S_CLK           = 0x09001000,
    GPIO9_SSP0_CLK          = 0x09001001,
    GPIO9_TIM1_CH0          = 0x09001002,
    GPIO9_GPIO9             = 0x09001003,
    GPIO9_DONGLE_SER_Q      = 0x09001004,
    GPIO9_I2S_MCLK          = 0x09001005,

    GPIO10_I2S_DIN          = 0x0A001000,
    GPIO10_SSP0_RXD         = 0x0A001001,
    GPIO10_TIM1_CH1         = 0x0A001002,
    GPIO10_GPIO10           = 0x0A001003,
    GPIO10_UART2_TXD        = 0x0A001004,
    GPIO10_UART2_RXD        = 0x0A001005,
    GPIO10_DAC_TRIG         = 0x0A001006,
    GPIO10_DONGLE_SER_FP    = 0x0A001007,

    GPIO11_I2S_DOUT         = 0x0B001000,
    GPIO11_SSP0_TXD         = 0x0B001001,
    GPIO11_TIM1_CH2         = 0x0B001002,
    GPIO11_GPIO11           = 0x0B001003,
    GPIO11_UART2_RXD        = 0x0B001004,
    GPIO11_UART2_TXD        = 0x0B001005,
    GPIO11_MCO              = 0x0B001006,
    GPIO11_DONGLE_SER_CLK   = 0x0B001007,

    GPIO12_SCL1             = 0x0C001000,
    GPIO12_SSP1_CS          = 0x0C001001,
    GPIO12_TIM1_CH3         = 0x0C001002,
    GPIO12_GPIO12           = 0x0C001003,
    GPIO12_UART0_TXD        = 0x0C001004,
    GPIO12_UART0_RXD        = 0x0C001005,
    GPIO12_TESTPIN0         = 0x0C001006,
    GPIO12_SIRIN            = 0x0C001007,

    GPIO13_SDA1             = 0x0D001000,
    GPIO13_SSP1_CLK         = 0x0D001001,
    GPIO13_TIM0_ETR         = 0x0D001002,
    GPIO13_GPIO13           = 0x0D001003,
    GPIO13_UART0_RXD        = 0x0D001004,
    GPIO13_UART0_TXD        = 0x0D001005,
    GPIO13_EXT_INT2         = 0x0D001006,
    GPIO13_NSIROUT          = 0x0D001007,

    GPIO14_SCL0             = 0x0E001000,
    GPIO14_SSP1_RXD         = 0x0E001001,
    GPIO14_TIM1_ETR         = 0x0E001002,
    GPIO14_GPIO14           = 0x0E001003,
    GPIO14_UART0_RTS        = 0x0E001004,
    GPIO14_UART0_CTS        = 0x0E001005,
    GPIO14_EXT_INT3         = 0x0E001006,
    GPIO14_TESTPIN1         = 0x0E001007,

    GPIO15_SDA0             = 0x0F001000,
    GPIO15_SSP1_TXD         = 0x0F001001,
    GPIO15_MCO              = 0x0F001002,
    GPIO15_GPIO15           = 0x0F001003,
    GPIO15_UART0_CTS        = 0x0F001004,
    GPIO15_UART0_RTS        = 0x0F001005,
    GPIO15_EXT_INT4         = 0x0F001006,
    GPIO15_TESTPIN2         = 0x0F001007,

    GPIO16_SCL0             = 0x10001000,
    GPIO16_SSP1_CS          = 0x10001001,
    GPIO16_TIM1_CH0         = 0x10001002,
    GPIO16_GPIO16           = 0x10001003,
    GPIO16_COMP_OUT         = 0x10001004,
    GPIO16_TESTPIN3         = 0x10001005,

    GPIO17_SDA0             = 0x11001000,
    GPIO17_SSP1_CLK         = 0x11001001,
    GPIO17_TIM1_CH1         = 0x11001002,
    GPIO17_GPIO17           = 0x11001003,
    GPIO17_ADC_TRIG         = 0x11001004,
    GPIO17_TESTPIN4         = 0x11001005,
    GPIO17_I2S_MCLK         = 0x11001006,

    GPIO18_SCL1             = 0x12001000,
    GPIO18_SSP1_RXD         = 0x12001001,
    GPIO18_TIM1_CH2         = 0x12001002,
    GPIO18_GPIO18           = 0x12001003,
    GPIO18_UART2_TXD        = 0x12001004,
    GPIO18_UART2_RXD        = 0x12001005,
    GPIO18_EXT_INT5         = 0x12001006,
    GPIO18_TESTPIN5         = 0x12001007,

    GPIO19_SDA1             = 0x13001000,
    GPIO19_SSP1_TXD         = 0x13001001,
    GPIO19_TIM1_CH3         = 0x13001002,
    GPIO19_GPIO19           = 0x13001003,
    GPIO19_UART2_RXD        = 0x13001004,
    GPIO19_UART2_TXD        = 0x13001005,
    GPIO19_EXT_INT6         = 0x13001006,
    GPIO19_MCO              = 0x13001007,

    GPIO20_I2S_WS           = 0x14001000,
    GPIO20_SSP0_CS          = 0x14001001,
    GPIO20_TIM0_CH0         = 0x14001002,
    GPIO20_GPIO20           = 0x14001003,
    GPIO20_UART0_TXD        = 0x14001004,
    GPIO20_UART0_RXD        = 0x14001005,
    GPIO20_EXT_INT7         = 0x14001006,
    GPIO20_SIRIN            = 0x14001007,

    GPIO21_I2S_CLK          = 0x15001000,
    GPIO21_SSP0_CLK         = 0x15001001,
    GPIO21_TIM0_CH1         = 0x15001002,
    GPIO21_GPIO21           = 0x15001003,
    GPIO21_UART0_RXD        = 0x15001004,
    GPIO21_UART0_TXD        = 0x15001005,
    GPIO21_MCO              = 0x15001006,
    GPIO21_NSIROUT          = 0x15001007,

    GPIO22_I2S_DIN          = 0x16001000,
    GPIO22_SSP0_RXD         = 0x16001001,
    GPIO22_TIM0_CH2         = 0x16001002,
    GPIO22_GPIO22           = 0x16001003,
    GPIO22_UART0_RTS        = 0x16001004,
    GPIO22_UART0_CTS        = 0x16001005,
    GPIO22_TESTPIN6         = 0x16001006,
    GPIO22_DAC_TRIG         = 0x16001007,

    GPIO23_I2S_DOUT         = 0x17001000,
    GPIO23_SSP0_TXD         = 0x17001001,
    GPIO23_TIM0_CH3         = 0x17001002,
    GPIO23_GPIO23           = 0x17001003,
    GPIO23_UART0_CTS        = 0x17001004,
    GPIO23_UART0_RTS        = 0x17001005,
    GPIO23_TESTPIN7         = 0x17001006,

    GPIO24_M0_JTAG_TCK      = 0x18001000,
    GPIO24_DSP_JTAG_TCK     = 0x18001001,
    GPIO24_RF_JTAG_TCK      = 0x18001002,
    GPIO24_GPIO24           = 0x18001003,
    GPIO24_CK_SWD_TCK       = 0x18001004,

    GPIO25_M0_JTAG_TDI      = 0x19001000,
    GPIO25_DSP_JTAG_TDI     = 0x19001001,
    GPIO25_RF_JTAG_TDI      = 0x19001002,
    GPIO25_GPIO25           = 0x19001003,

    GPIO26_M0_JTAG_TDO      = 0x1A001000,
    GPIO26_DSP_JTAG_TDO     = 0x1A001001,
    GPIO26_RF_JTAG_TDO      = 0x1A001002,
    GPIO26_GPIO26           = 0x1A001003,

    GPIO27_M0_JTAG_TMS      = 0x1B001000,
    GPIO27_DSP_JTAG_TMS     = 0x1B001001,
    GPIO27_RF_JTAG_TMS      = 0x1B001002,
    GPIO27_GPIO27           = 0x1B001003,
    GPIO27_CK_SWD_TMS       = 0x1B001004,

    GPIO28_M0_JTAG_TRST     = 0x1C001000,
    GPIO28_DSP_JTAG_RTCK    = 0x1C001001,
    GPIO28_RF_JTAG_TRST     = 0x1C001002,
    GPIO28_GPIO28           = 0x1C001003,

    /*GPIO29 - GPIO30*/

    GPIO31_GPIO31           = 0x1F001000,
    GPIO31_SPIFC_CLK        = 0x1F001001,
    GPIO31_DSP_JTAG_TCK     = 0x1F001002,
    GPIO31_RF_JTAG_TCK      = 0x1F001003,
    GPIO31_M0_JTAG_TCK      = 0x1F001004,
    GPIO31_CK_SWD_TCK       = 0x1F001005,

    GPIO32_GPIO32           = 0x20001000,
    GPIO32_SPIFC_CS         = 0x20001001,
    GPIO32_DSP_JTAG_TDI     = 0x20001002,
    GPIO32_RF_JTAG_TDI      = 0x20001003,
    GPIO32_M0_JTAG_TDI      = 0x20001004,

    GPIO33_GPIO33           = 0x21001000,
    GPIO33_SPIFC_DATA0      = 0x21001001,
    GPIO33_DSP_JTAG_TDO     = 0x21001002,
    GPIO33_RF_JTAG_TDO      = 0x21001003,
    GPIO33_M0_JTAG_TDO      = 0x21001004,

    GPIO34_GPIO34           = 0x22001000,
    GPIO34_SPIFC_DATA1      = 0x22001001,
    GPIO34_DSP_JTAG_TMS     = 0x22001002,
    GPIO34_RF_JTAG_TMS      = 0x22001003,
    GPIO34_M0_JTAG_TMS      = 0x22001004,
    GPIO34_CK_SWD_TMS       = 0x22001005,

    GPIO35_GPIO35           = 0x23001000,
    GPIO35_SPIFC_DATA2      = 0x23001001,
    GPIO35_DSP_JTAG_RTCK    = 0x23001002,
    GPIO35_RF_JTAG_TRST     = 0x23001003,
    GPIO35_M0_JTAG_TRST     = 0x23001004,

    GPIO36_GPIO36           = 0x24001000,
    GPIO36_SPIFC_DATA3      = 0x24001001,

    GPIO37_SIM_RST          = 0x25001000,
    GPIO37_GPIO37           = 0x25001001,

    GPIO38_SIM_CLK          = 0x26001000,
    GPIO38_GPIO38           = 0x26001001,

    GPIO39_SIM_DATA         = 0x27001000,
    GPIO39_GPIO39           = 0x27001001,

    GPIO40_GPIO40           = 0x28001000,
    GPIO40_LTE_TPU_OUT0_0   = 0x28001001,
    GPIO40_MIPI_RFFE_CLK    = 0x28001002,

    GPIO41_GPIO41           = 0x29001000,
    GPIO41_LTE_TPU_OUT0_1   = 0x29001001,
    GPIO41_MIPI_RFFE_DATA   = 0x29001002,

    /*GPIO42 - GPIO43*/

    GPIO44_GPIO44           = 0x2C001000,
    GPIO44_LTE_TPU_OUT0_2   = 0x2C001001,

    GPIO45_GPIO45           = 0x2D001000,
    GPIO45_LTE_TPU_OUT0_3   = 0x2D001001,

    GPIO46_GPIO46           = 0x2E001000,
    GPIO46_LTE_TPU_OUT0_4   = 0x2E001001,

    GPIO47_GPIO47           = 0x2F001000,
    GPIO47_LTE_TPU_OUT0_5   = 0x2F001001,

    GPIO48_GPIO48           = 0x30001000,
    GPIO48_LTE_TPU_OUT0_6   = 0x30001001,

    GPIO49_GPIO49           = 0x31001000,
    GPIO49_LTE_TPU_OUT0_7   = 0x31001001,

    GPIO50_GPIO50           = 0x32001000,
    GPIO50_LTE_TPU_OUT0_8   = 0x32001001,

    /*GPIO51 - GPIO52*/

    GPIO53_GPIO53           = 0x35001000,

    GPIO54_GPIO54           = 0x36001000,
};

int32_t pin_mux(uint32_t pin, enum gpio_func func);

#endif /* ZX297100_PINMUX_H */


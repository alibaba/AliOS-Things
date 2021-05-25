/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     drv_clk.h
 * @brief    header file for clk driver
 * @version  V1.0
 * @date     02. June 2017
 * @model    clk
 ******************************************************************************/

#ifndef _CSI_CLK_H_
#define _CSI_CLK_H_


#include <drv/common.h>

#ifdef __cplusplus
extern "C" {
#endif
typedef enum {
    CK_TIMER0_PCLK = 0,
    CK_TIMER1_PCLK,
    CK_TIMER0_WCLK,
    CK_TIMER1_WCLK,
    AP_WDT_PCLK,
    AP_WDT_WCLK,
    UART0_PCLK,
    UART1_PCLK,
    UART2_PCLK,
    LP_UART_PCLK,
    UART0_WCLK_L1,
    UART0_WCLK_L2,
    UART1_WCLK,
    UART2_WCLK,
    LP_UART_WCLK,
    I2C0_PCLK,
    I2C1_PCLK,
    I2C0_WCLK,
    I2C1_WCLK,
    SSP0_PCLK,
    SSP1_PCLK,
    SSP2_PCLK,
    SSP0_WCLK,
    SSP1_WCLK,
    SSP2_WCLK,
    GPIO_PD_PCLK,
    ADC_PCLK,
    ADC_WCLK,
    DAC_PCLK,
    DAC_WCLK,
    AD_TIMER0_PCLK,
    AD_TIMER1_PCLK,
    AD_TIMER0_WCLK,
    AD_TIMER1_WCLK,
    AP_DMA_PCLK,
    LP_TIMER_PCLK,
    LP_TIMER_WCLK,
    I2S0_PCLK,
    I2S0_WCLK,
    PD_LOGIC_MATRIX_CLK,
    PD_PINMUX_CLK,
    ICP_CLK,
    SPINOR_PCLK,
    SPINOR_WCLK,
    IRAM_CLK,
    GPIO_PCLK,
    AO_PINMUX_PCLK,
    AO_PCU_PCLK,
    RTC_PCLK,
    EFLASH2_CLK,
    EFLASH1_CTRL_CLK,
    EFLASH1_CFG_CLK,
    LSP_61M44_WCLK,
    AON_76M8_CLK,
    RTC_32K_WCLK,
    IWDT_PCLK,
    IWDT_WCLK,
}
clk_name;


/*
*  bits domain definition  for clock frequency
*  base clock name   same clock number  reserved       clock selection   frequency division
*  31...28 27...24            23...20            19...16        15...12 11...8         7...4 3...0
*  invalid value:
*          0xff                  0xf or 0x0       0x0 or ignore         0xff                    0xff
*/
typedef enum {
    FIXED_FREQ = 0xffffffff,

    /*ck timer0&1/rm timer0&1/ceva timer0&1*/
    WCLK_TIMER_32K  = (CK_TIMER0_WCLK << 24) | 0x00600000,
    WCLK_TIMER_19M2 = (CK_TIMER0_WCLK << 24) | 0x06000100,    /* 1 div*/
    WCLK_TIMER_9M6  = (CK_TIMER0_WCLK << 24) | 0x00600101,     /* 2 div*/

    /*ap wdt/cp wdt*/
    WCLK_WDT_32K      = (AP_WDT_WCLK << 24) | 0x00200000,
    WCLK_WDT_19M2     = (AP_WDT_WCLK << 24) | 0x00200100,
    WCLK_WDT_9M6      = (AP_WDT_WCLK << 24) | 0x00200101,

    /*i2c0&1*/
    WCLK_I2C_76M8    = (I2C0_WCLK << 24) | 0x00200100,
    WCLK_I2C_19M2    = (I2C0_WCLK << 24) | 0x00200000,

    /*uart0&1&2&lp uart*/
    WCLK_UART_19M2     = (UART0_WCLK_L1 << 24) | 0x00400000,
    WCLK_UART_76M8     = (UART0_WCLK_L1 << 24) | 0x00400100,
    /*only for uart0& lp uart*/
    WCLK_UART_32K      = (UART0_WCLK_L1 << 24) | 0x00400200,

    /*ssp0&1&2*/
    WCLK_SSP_19M2      = (SSP0_WCLK << 24) | 0x00300000,
    WCLK_SSP_76M8      = (SSP0_WCLK << 24) | 0x00300100,
    WCLK_SSP_61m44     = (SSP0_WCLK << 24) | 0x00300200,

    /*adc*/
    WCLK_ADC_19M2     = (ADC_WCLK << 24) | 0x00100000,
    WCLK_ADC_38M4     = (ADC_WCLK << 24) | 0x00100100,
    WCLK_ADC_61M44    = (ADC_WCLK << 24) | 0x00100200,
    WCLK_ADC_32K      = (ADC_WCLK << 24) | 0x00100300,

    /*dac*/
    WCLK_DAC_19M2     = (DAC_WCLK << 24) | 0x001001000,
    WCLK_DAC_32K      = (DAC_WCLK << 24) | 0x00100200,

    /*ad timer0&1*/
    WCLK_AD_TIMER_32K      = (AD_TIMER0_WCLK << 24) | 0x002000000,
    WCLK_AD_TIMER_76M8     = (AD_TIMER0_WCLK << 24) | 0x00200100,
    WCLK_AD_TIMER_19M2     = (AD_TIMER0_WCLK << 24) | 0x00200200,

    /*lp timer*/
    WCLK_LP_TIMER_32K      = (LP_TIMER_WCLK << 24) | 0x001000000,
    WCLK_LP_TIMER_76M8     = (LP_TIMER_WCLK << 24) | 0x001000100,
    WCLK_LP_TIMER_19M2     = (LP_TIMER_WCLK << 24) | 0x001000200,

    /*I2S0*/
    WCLK_I2S_19M2         = (I2S0_WCLK << 24) | 0x00100000,
    WCLK_I2S_76M8         = (I2S0_WCLK << 24) | 0x00100100,
    WCLK_I2S_61M44        = (I2S0_WCLK << 24) | 0x00100200,
    /*spi nor*/
    WCLK_SPINOR_19M2      = (SPINOR_WCLK << 24) | 0x00100000,
    WCLK_SPINOR_76M8      = (SPINOR_WCLK << 24) | 0x00100100,
    WCLK_SPINOR_61M44     = (SPINOR_WCLK << 24) | 0x00100200,

    /*iwdt*/
    WCLK_IWDT_RC32K       = (IWDT_WCLK << 24) | 0x00100000,
    WCLK_IWDT_XTAL32K     = (IWDT_WCLK << 24) | 0x00100100,

} clk_freq_sel;

/**
  \brief       set software clock gate.
  \param[in]   name  the clock name to set.
  \param[in]   ctrl 0 enable; 1 disable
  \return      error code
 */
int csi_clk_sw_gate_ctrl(clk_name name, uint32_t ctrl);

/**
  \brief       set clock autogate.
  \param[in]   name  the clock name to set.
  \param[in]   ctrl 0 enable; 1 disable
  \return      error code
 */
int csi_clk_hw_gate_ctrl(clk_name name, uint32_t ctrl);

/**
  \brief       config clock reset ctrl.
  \param[in]   name  the clock name to set.
  \param[in]   ctrl 0 enable; 1 disable
  \return      error code
 */
int csi_clk_reset(clk_name name, uint32_t ctrl);

/**
  \brief       set clock freq.
  \param[in]   name  the clock name to set.
  \param[in]   freq  teh corresponding freq to set
  \return      error code
 */
int csi_clk_set_freq(clk_name name, clk_freq_sel freq);


#ifdef __cplusplus
}
#endif

#endif /* _CSI_CLK_H_ */

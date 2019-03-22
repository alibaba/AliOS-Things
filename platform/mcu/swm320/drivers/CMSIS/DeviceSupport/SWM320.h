#ifndef __SWM320_H__
#define __SWM320_H__

/*
 * ==========================================================================
 * ---------- Interrupt Number Definition -----------------------------------
 * ==========================================================================
 */
typedef enum IRQn
{
/******  Cortex-M0 Processor Exceptions Numbers **********************************************/
    NonMaskableInt_IRQn         = -14,    /*!< 2 Non Maskable Interrupt                        */
    MemoryManagement_IRQn       = -12,    /*!< 4 Cortex-M4 Memory Management Interrupt         */
    BusFault_IRQn               = -11,    /*!< 5 Cortex-M4 Bus Fault Interrupt                 */
    UsageFault_IRQn             = -10,    /*!< 6 Cortex-M4 Usage Fault Interrupt               */
    SVCall_IRQn                 = -5,     /*!< 11 Cortex-M4 SV Call Interrupt                  */
    DebugMonitor_IRQn           = -4,     /*!< 12 Cortex-M4 Debug Monitor Interrupt            */
    PendSV_IRQn                 = -2,     /*!< 14 Cortex-M4 Pend SV Interrupt                  */
    SysTick_IRQn                = -1,     /*!< 15 Cortex-M4 System Tick Interrupt              */

/******  Cortex-M4 specific Interrupt Numbers ************************************************/
    GPIOA0_IRQn         = 0,
    GPIOA1_IRQn         = 1,
    GPIOA2_IRQn         = 2,
    GPIOA3_IRQn         = 3,
    GPIOA4_IRQn         = 4,
    GPIOA5_IRQn         = 5,
    GPIOA6_IRQn         = 6,
    GPIOA7_IRQn         = 7,
    GPIOB0_IRQn         = 8,
    GPIOB1_IRQn         = 9,
    GPIOB2_IRQn         = 10,
    GPIOB3_IRQn         = 11,
    GPIOB4_IRQn         = 12,
    GPIOB5_IRQn         = 13,
    GPIOB6_IRQn         = 14,
    GPIOB7_IRQn         = 15,
    GPIOC0_IRQn       = 16,
    GPIOC1_IRQn         = 17,
    GPIOC2_IRQn         = 18,
    GPIOC3_IRQn         = 19,
    GPIOC4_IRQn         = 20,
    GPIOC5_IRQn         = 21,
    GPIOC6_IRQn         = 22,
    GPIOC7_IRQn         = 23,
    GPIOM0_IRQn         = 24,
    GPIOM1_IRQn         = 25,
    GPIOM2_IRQn         = 26,
    GPIOM3_IRQn         = 27,
    GPIOM4_IRQn         = 28,
    GPIOM5_IRQn         = 29,
    GPIOM6_IRQn         = 30,
    GPIOM7_IRQn         = 31,
    DMA_IRQn          = 32,
    LCD_IRQn            = 33,
    NORFLC_IRQn         = 34,
    CAN_IRQn              = 35,
    PULSE_IRQn          = 36,
    WDT_IRQn              = 37,
    PWM_IRQn              = 38,
    UART0_IRQn          = 39,
    UART1_IRQn          = 40,
    UART2_IRQn          = 41,
    UART3_IRQn          = 42,
    UART4_IRQn          = 43,
    I2C0_IRQn           = 44,
    I2C1_IRQn           = 45,
    SPI0_IRQn           = 46,
    ADC0_IRQn           = 47,
    RTC_IRQn            = 48,
    BOD_IRQn            = 49,
    SDIO_IRQn           = 50,
    GPIOA_IRQn          = 51,
    GPIOB_IRQn          = 52,
    GPIOC_IRQn          = 53,
    GPIOM_IRQn          = 54,
    GPION_IRQn          = 55,
    GPIOP_IRQn          = 56,
    ADC1_IRQn           = 57,
    FPU_IRQn            = 58,
    SPI1_IRQn           = 59,
    TIMR0_IRQn          = 60,
    TIMR1_IRQn          = 61,
    TIMR2_IRQn          = 62,
    TIMR3_IRQn          = 63,
    TIMR4_IRQn          = 64,
    TIMR5_IRQn          = 65,
} IRQn_Type;

/*
 * ==========================================================================
 * ----------- Processor and Core Peripheral Section ------------------------
 * ==========================================================================
 */

/* Configuration of the Cortex-M0 Processor and Core Peripherals */
#define __CM4_REV                 0x0001  /*!< Core revision r0p1                            */
#define __MPU_PRESENT             1       /*!< SWM320 provides an MPU                       */
#define __NVIC_PRIO_BITS          3       /*!< SWM320 uses 3 Bits for the Priority Levels   */
#define __Vendor_SysTickConfig    0       /*!< Set to 1 if different SysTick Config is used  */
#define __FPU_PRESENT             0       /*!< FPU present                                   */

#if   defined ( __CC_ARM )
    #pragma anon_unions
#endif

#include <stdio.h>
#include "core_cm4.h"                  /* Cortex-M0 processor and core peripherals           */
#include "system_SWM320.h"


/******************************************************************************/
/*              Device Specific Peripheral registers structures          */
/******************************************************************************/
typedef struct {
    __IO uint32_t CLKSEL;                   //Clock Select

    __IO uint32_t CLKDIV;

    __IO uint32_t CLKEN;                    //Clock Enable

    __IO uint32_t SLEEP;

    uint32_t RESERVED0[6];

    __IO uint32_t RTCBKP_ISO;               //[0] 1 RTC备份电源域处于隔离状态    0 RTC备份电源域可访问

    __IO uint32_t RTCWKEN;                  //[0] 1 使能RTC唤醒功能

    uint32_t RESERVED[52+64];

    __IO uint32_t PAWKEN;                   //Port A Wakeup Enable
    __IO uint32_t PBWKEN;
    __IO uint32_t PCWKEN;

    uint32_t RESERVED2[1+4];

    __IO uint32_t PAWKSR;                   //Port A Wakeup Status Register，写1清零
    __IO uint32_t PBWKSR;
    __IO uint32_t PCWKSR;

    uint32_t RESERVED3[64-10];

    __IO uint32_t RSTCR;                    //Reset Control Register
    __IO uint32_t RSTSR;                    //Reset Status Register

    uint32_t RESERVED4[61+64];

    __IO uint32_t BKP[3];

    //RTC Power Domain: 0x4001E000
    uint32_t RESERVED5[(0x4001E000-0x40000508)/4-1];

    __IO uint32_t RTCBKP[8];

    __IO uint32_t LRCCR;                    //Low speed RC Control Register
    __IO uint32_t LRCTRIM0;                 //Low speed RC Trim
    __IO uint32_t LRCTRIM1;

    uint32_t RESERVED6;

    __IO uint32_t RTCLDOTRIM;               //RTC Power Domain LDO Trim

    //Analog Control: 0x40031000
    uint32_t RESERVED7[(0x40031000-0x4001E030)/4-1];

    __IO uint32_t HRCCR;                    //High speed RC Control Register
    __IO uint32_t HRC20M;                   //[24:0] High speed RC Trim Value for 20MHz
    __IO uint32_t HRC40M;                   //[24:0] High speed RC Trim Value for 40MHz

    uint32_t RESERVED8[3];

    __IO uint32_t BGTRIM;

    __IO uint32_t TEMPCR;

    __IO uint32_t XTALCR;

    __IO uint32_t PLLCR;
    __IO uint32_t PLLDIV;
    __IO uint32_t PLLSET;
    __IO uint32_t PLLLOCK;

    __IO uint32_t BODIE;
    __IO uint32_t BODIF;

    __IO uint32_t ADC1IN7;
} SYS_TypeDef;


#define SYS_CLKSEL_LFCK_Pos         0       //Low Frequency Clock Source    0 LRC   1 PLL
#define SYS_CLKSEL_LFCK_Msk         (0x01 << SYS_CLKSEL_LFCK_Pos)
#define SYS_CLKSEL_HFCK_Pos         1       //High Frequency Clock Source   0 HRC   1 XTAL
#define SYS_CLKSEL_HFCK_Msk         (0x01 << SYS_CLKSEL_HFCK_Pos)
#define SYS_CLKSEL_SYS_Pos          2
#define SYS_CLKSEL_SYS_Msk          (0x01 << SYS_CLKSEL_SYS_Pos)

#define SYS_CLKDIV_SYS_Pos          0
#define SYS_CLKDIV_SYS_Msk          (0x01 << SYS_CLKDIV_SYS_Pos)
#define SYS_CLKDIV_PWM_Pos          1
#define SYS_CLKDIV_PWM_Msk          (0x01 << SYS_CLKDIV_PWM_Pos)
#define SYS_CLKDIV_SDRAM_Pos        2
#define SYS_CLKDIV_SDRAM_Msk        (0x03 << SYS_CLKDIV_SDRAM_Pos)
#define SYS_CLKDIV_SDIO_Pos         4
#define SYS_CLKDIV_SDIO_Msk         (0x03 << SYS_CLKDIV_SDIO_Pos)

#define SYS_CLKEN_GPIOA_Pos         0
#define SYS_CLKEN_GPIOA_Msk         (0x01 << SYS_CLKEN_GPIOA_Pos)
#define SYS_CLKEN_GPIOB_Pos         1
#define SYS_CLKEN_GPIOB_Msk         (0x01 << SYS_CLKEN_GPIOB_Pos)
#define SYS_CLKEN_GPIOC_Pos         2
#define SYS_CLKEN_GPIOC_Msk         (0x01 << SYS_CLKEN_GPIOC_Pos)
#define SYS_CLKEN_GPIOM_Pos         4
#define SYS_CLKEN_GPIOM_Msk         (0x01 << SYS_CLKEN_GPIOM_Pos)
#define SYS_CLKEN_GPION_Pos         5
#define SYS_CLKEN_GPION_Msk         (0x01 << SYS_CLKEN_GPION_Pos)
#define SYS_CLKEN_TIMR_Pos          6
#define SYS_CLKEN_TIMR_Msk          (0x01 << SYS_CLKEN_TIMR_Pos)
#define SYS_CLKEN_WDT_Pos           7
#define SYS_CLKEN_WDT_Msk           (0x01 << SYS_CLKEN_WDT_Pos)
#define SYS_CLKEN_ADC0_Pos          8
#define SYS_CLKEN_ADC0_Msk          (0x01 << SYS_CLKEN_ADC0_Pos)
#define SYS_CLKEN_PWM_Pos           9
#define SYS_CLKEN_PWM_Msk           (0x01 << SYS_CLKEN_PWM_Pos)
#define SYS_CLKEN_RTC_Pos           10
#define SYS_CLKEN_RTC_Msk           (0x01 << SYS_CLKEN_RTC_Pos)
#define SYS_CLKEN_UART0_Pos         11
#define SYS_CLKEN_UART0_Msk         (0x01 << SYS_CLKEN_UART0_Pos)
#define SYS_CLKEN_UART1_Pos         12
#define SYS_CLKEN_UART1_Msk         (0x01 << SYS_CLKEN_UART1_Pos)
#define SYS_CLKEN_UART2_Pos         13
#define SYS_CLKEN_UART2_Msk         (0x01 << SYS_CLKEN_UART2_Pos)
#define SYS_CLKEN_UART3_Pos         14
#define SYS_CLKEN_UART3_Msk         (0x01 << SYS_CLKEN_UART3_Pos)
#define SYS_CLKEN_UART4_Pos         15
#define SYS_CLKEN_UART4_Msk         (0x01 << SYS_CLKEN_UART4_Pos)
#define SYS_CLKEN_SPI0_Pos          16
#define SYS_CLKEN_SPI0_Msk          (0x01 << SYS_CLKEN_SPI0_Pos)
#define SYS_CLKEN_I2C0_Pos          17
#define SYS_CLKEN_I2C0_Msk          (0x01 << SYS_CLKEN_I2C0_Pos)
#define SYS_CLKEN_I2C1_Pos          18
#define SYS_CLKEN_I2C1_Msk          (0x01 << SYS_CLKEN_I2C1_Pos)
#define SYS_CLKEN_I2C2_Pos          19
#define SYS_CLKEN_I2C2_Msk          (0x01 << SYS_CLKEN_I2C2_Pos)
#define SYS_CLKEN_LCD_Pos           20
#define SYS_CLKEN_LCD_Msk           (0x01 << SYS_CLKEN_LCD_Pos)
#define SYS_CLKEN_GPIOP_Pos         21
#define SYS_CLKEN_GPIOP_Msk         (0x01 << SYS_CLKEN_GPIOP_Pos)
#define SYS_CLKEN_ANAC_Pos          22
#define SYS_CLKEN_ANAC_Msk          (0x01 << SYS_CLKEN_ANAC_Pos)
#define SYS_CLKEN_CRC_Pos           23
#define SYS_CLKEN_CRC_Msk           (0x01 << SYS_CLKEN_CRC_Pos)
#define SYS_CLKEN_RTCBKP_Pos        24
#define SYS_CLKEN_RTCBKP_Msk        (0x01 << SYS_CLKEN_RTCBKP_Pos)
#define SYS_CLKEN_CAN_Pos           25
#define SYS_CLKEN_CAN_Msk           (0x01 << SYS_CLKEN_CAN_Pos)
#define SYS_CLKEN_SDRAM_Pos         26
#define SYS_CLKEN_SDRAM_Msk         (0x01 << SYS_CLKEN_SDRAM_Pos)
#define SYS_CLKEN_NORFL_Pos         27      //NOR Flash
#define SYS_CLKEN_NORFL_Msk         (0x01 << SYS_CLKEN_NORFL_Pos)
#define SYS_CLKEN_RAMC_Pos          28
#define SYS_CLKEN_RAMC_Msk          (0x01 << SYS_CLKEN_RAMC_Pos)
#define SYS_CLKEN_SDIO_Pos          29
#define SYS_CLKEN_SDIO_Msk          (0x01 << SYS_CLKEN_SDIO_Pos)
#define SYS_CLKEN_ADC1_Pos          30
#define SYS_CLKEN_ADC1_Msk          (0x01 << SYS_CLKEN_ADC1_Pos)
#define SYS_CLKEN_ALIVE_Pos         31
#define SYS_CLKEN_ALIVE_Msk         (0x01 << SYS_CLKEN_ALIVE_Pos)

#define SYS_SLEEP_SLEEP_Pos         0
#define SYS_SLEEP_SLEEP_Msk         (0x01 << SYS_SLEEP_SLEEP_Pos)
#define SYS_SLEEP_DEEP_Pos          1
#define SYS_SLEEP_DEEP_Msk          (0x01 << SYS_SLEEP_DEEP_Pos)

#define SYS_RSTCR_SYS_Pos           0
#define SYS_RSTCR_SYS_Msk           (0x01 << SYS_RSTCR_SYS_Pos)
#define SYS_RSTCR_FLASH_Pos         1
#define SYS_RSTCR_FLASH_Msk         (0x01 << SYS_RSTCR_FLASH_Pos)
#define SYS_RSTCR_PWM_Pos           2
#define SYS_RSTCR_PWM_Msk           (0x01 << SYS_RSTCR_PWM_Pos)
#define SYS_RSTCR_CPU_Pos           3
#define SYS_RSTCR_CPU_Msk           (0x01 << SYS_RSTCR_CPU_Pos)
#define SYS_RSTCR_DMA_Pos           4
#define SYS_RSTCR_DMA_Msk           (0x01 << SYS_RSTCR_DMA_Pos)
#define SYS_RSTCR_NORFLASH_Pos      5
#define SYS_RSTCR_NORFLASH_Msk      (0x01 << SYS_RSTCR_NORFLASH_Pos)
#define SYS_RSTCR_SRAM_Pos          6
#define SYS_RSTCR_SRAM_Msk          (0x01 << SYS_RSTCR_SRAM_Pos)
#define SYS_RSTCR_SDRAM_Pos         7
#define SYS_RSTCR_SDRAM_Msk         (0x01 << SYS_RSTCR_SDRAM_Pos)
#define SYS_RSTCR_SDIO_Pos          8
#define SYS_RSTCR_SDIO_Msk          (0x01 << SYS_RSTCR_SDIO_Pos)
#define SYS_RSTCR_LCD_Pos           9
#define SYS_RSTCR_LCD_Msk           (0x01 << SYS_RSTCR_LCD_Pos)
#define SYS_RSTCR_CAN_Pos           10
#define SYS_RSTCR_CAN_Msk           (0x01 << SYS_RSTCR_CAN_Pos)

#define SYS_RSTSR_POR_Pos           0
#define SYS_RSTSR_POR_Msk           (0x01 << SYS_RSTSR_POR_Pos)
#define SYS_RSTSR_BOD_Pos           1
#define SYS_RSTSR_BOD_Msk           (0x01 << SYS_RSTSR_BOD_Pos)
#define SYS_RSTSR_PIN_Pos           2
#define SYS_RSTSR_PIN_Msk           (0x01 << SYS_RSTSR_PIN_Pos)
#define SYS_RSTSR_WDT_Pos           3
#define SYS_RSTSR_WDT_Msk           (0x01 << SYS_RSTSR_WDT_Pos)
#define SYS_RSTSR_SWRST_Pos         4
#define SYS_RSTSR_SWRST_Msk         (0x01 << SYS_RSTSR_SWRST_Pos)

#define SYS_LRCCR_OFF_Pos           0
#define SYS_LRCCR_OFF_Msk           (0x01 << SYS_LRCCR_OFF_Pos)

#define SYS_LRCTRIM0_R_Pos          0
#define SYS_LRCTRIM0_R_Msk          (0x7FFF << SYS_LRCTRIM0_R_Pos)
#define SYS_LRCTRIM0_M_Pos          15
#define SYS_LRCTRIM0_M_Msk          (0x3F << SYS_LRCTRIM2_M_Pos)
#define SYS_LRCTRIM0_F_Pos          21
#define SYS_LRCTRIM0_F_Msk          (0x7FF << SYS_LRCTRIM0_F_Pos)

#define SYS_LRCTRIM1_U_Pos          0
#define SYS_LRCTRIM1_U_Msk          (0x7FFF << SYS_LRCTRIM1_U_Pos)


#define SYS_HRCCR_DBL_Pos           0
#define SYS_HRCCR_DBL_Msk           (0x01 << SYS_HRCCR_DBL_Pos)
#define SYS_HRCCR_OFF_Pos           1
#define SYS_HRCCR_OFF_Msk           (0x01 << SYS_HRCCR_OFF_Pos)

#define SYS_HRC20M_R_Pos            0
#define SYS_HRC20M_R_Msk            (0x3FFF << SYS_HRC20M_R_Pos)
#define SYS_HRC20M_F_Pos            16
#define SYS_HRC20M_F_Msk            (0x7FF << SYS_HRC20M_F_Pos)

#define SYS_HRC40M_R_Pos            0
#define SYS_HRC40M_R_Msk            (0x3FFF << SYS_HRC40M_R_Pos)
#define SYS_HRC40M_F_Pos            16
#define SYS_HRC40M_F_Msk            (0x7FF << SYS_HRC40M_F_Pos)

#define SYS_TEMPCR_OFF_Pos          0
#define SYS_TEMPCR_OFF_Msk          (0x01 << SYS_TEMPCR_OFF_Pos)
#define SYS_TEMPCR_TRIM_Pos         4
#define SYS_TEMPCR_TRIM_Msk         (0x3F << SYS_TEMPCR_TRIM_Pos)

#define SYS_XTALCR_EN_Pos           0
#define SYS_XTALCR_EN_Msk           (0x01 << SYS_XTALCR_EN_Pos)

#define SYS_PLLCR_OUTEN_Pos         0
#define SYS_PLLCR_OUTEN_Msk         (0x01 << SYS_PLLCR_OUTEN_Pos)
#define SYS_PLLCR_INSEL_Pos         1
#define SYS_PLLCR_INSEL_Msk         (0x01 << SYS_PLLCR_INSEL_Pos)
#define SYS_PLLCR_OFF_Pos           2
#define SYS_PLLCR_OFF_Msk           (0x01 << SYS_PLLCR_OFF_Pos)

#define SYS_PLLDIV_FBDIV_Pos        0
#define SYS_PLLDIV_FBDIV_Msk        (0x1FF << SYS_PLLDIV_FBDIV_Pos)
#define SYS_PLLDIV_ADDIV_Pos        9
#define SYS_PLLDIV_ADDIV_Msk        (0x1F << SYS_PLLDIV_ADDIV_Pos)
#define SYS_PLLDIV_ADVCO_Pos        14
#define SYS_PLLDIV_ADVCO_Msk        (0x03 << SYS_PLLDIV_ADVCO_Pos)
#define SYS_PLLDIV_INDIV_Pos        16
#define SYS_PLLDIV_INDIV_Msk        (0x1F << SYS_PLLDIV_INDIV_Pos)
#define SYS_PLLDIV_OUTDIV_Pos       24
#define SYS_PLLDIV_OUTDIV_Msk       (0x03 << SYS_PLLDIV_OUTDIV_Pos)

#define SYS_PLLSET_LPFBW_Pos        0       //PLL Low Pass Filter Bandwidth
#define SYS_PLLSET_LPFBW_Msk        (0x0F << SYS_PLLSET_LPFBW_Pos)
#define SYS_PLLSET_BIASADJ_Pos      4       //PLL Current Bias Adjustment
#define SYS_PLLSET_BIASADJ_Msk      (0x03 << SYS_PLLSET_BIASADJ_Pos)
#define SYS_PLLSET_REFVSEL_Pos      6       //PLL Reference Voltage Select
#define SYS_PLLSET_REFVSEL_Msk      (0x03 << SYS_PLLSET_REFVSEL_Pos)
#define SYS_PLLSET_CHPADJL_Pos      8       //PLL charge pump LSB current Adjustment
#define SYS_PLLSET_CHPADJL_Msk      (0x07 << SYS_PLLSET_CHPADJL_Pos)
#define SYS_PLLSET_CHPADJM_Pos      11      //PLL charge pump MSB current Adjustment
#define SYS_PLLSET_CHPADJM_Msk      (0x03 << SYS_PLLSET_CHPADJM_Pos)

#define SYS_BODIE_2V2_Pos           1
#define SYS_BODIE_2V2_Msk           (0x01 << SYS_BODIE_2V2_Pos)

#define SYS_BODIF_2V2_Pos           1
#define SYS_BODIF_2V2_Msk           (0x01 << SYS_BODIF_2V2_Pos)

#define SYS_ADC1IN7_SEL_Pos         0
#define SYS_ADC1IN7_SEL_Msk         (0x0F << SYS_ADC1IN7_SEL_Pos)
#define SYS_ADC1IN7_IOON_Pos        4
#define SYS_ADC1IN7_IOON_Msk        (0x01 << SYS_ADC1IN7_IOON_Pos)




typedef struct {
    __IO uint32_t PORTA_SEL;

    __IO uint32_t PORTB_SEL;

    __IO uint32_t PORTC_SEL;

    uint32_t RESERVED[5];

    __IO uint32_t PORTM_SEL0;

    __IO uint32_t PORTM_SEL1;

    uint32_t RESERVED2[2];

    __IO uint32_t PORTN_SEL0;

    __IO uint32_t PORTN_SEL1;

    uint32_t RESERVED3[2];

    __IO uint32_t PORTP_SEL0;

    __IO uint32_t PORTP_SEL1;

    uint32_t RESERVED4[46];

    __IO uint32_t PORTA_MUX0;

    __IO uint32_t PORTA_MUX1;

    uint32_t RESERVED5[2];

    __IO uint32_t PORTB_MUX0;

    __IO uint32_t PORTB_MUX1;

    uint32_t RESERVED6[2];

    __IO uint32_t PORTC_MUX0;

    __IO uint32_t PORTC_MUX1;

    uint32_t RESERVED7[14];

    __IO uint32_t PORTM_MUX0;

    __IO uint32_t PORTM_MUX1;

    __IO uint32_t PORTM_MUX2;

    __IO uint32_t PORTM_MUX3;

    __IO uint32_t PORTN_MUX0;

    __IO uint32_t PORTN_MUX1;

    __IO uint32_t PORTN_MUX2;

    uint32_t RESERVED8;

    __IO uint32_t PORTP_MUX0;

    __IO uint32_t PORTP_MUX1;

    __IO uint32_t PORTP_MUX2;

    __IO uint32_t PORTP_MUX3;

    uint32_t RESERVED9[28];

    __IO uint32_t PORTA_PULLU;

    uint32_t RESERVED10[3];

    __IO uint32_t PORTC_PULLU;

    uint32_t RESERVED11[3];

    __IO uint32_t PORTM_PULLU;

    uint32_t RESERVED12[3];

    __IO uint32_t PORTP_PULLU;

    uint32_t RESERVED13[51];

    __IO uint32_t PORTB_PULLD;

    uint32_t RESERVED14[3];

    __IO uint32_t PORTD_PULLD;

    uint32_t RESERVED15[3];

    __IO uint32_t PORTN_PULLD;

    uint32_t RESERVED16[135];

    __IO uint32_t PORTM_DRIVS;

    uint32_t RESERVED17[3];

    __IO uint32_t PORTN_DRIVS;

    uint32_t RESERVED18[3];

    __IO uint32_t PORTP_DRIVS;

    uint32_t RESERVED19[39];

    __IO uint32_t PORTA_INEN;

    uint32_t RESERVED20[3];

    __IO uint32_t PORTB_INEN;

    uint32_t RESERVED21[3];

    __IO uint32_t PORTC_INEN;

    uint32_t RESERVED22[7];

    __IO uint32_t PORTM_INEN;

    uint32_t RESERVED23[3];

    __IO uint32_t PORTN_INEN;

    uint32_t RESERVED24[3];

    __IO uint32_t PORTP_INEN;
} PORT_TypeDef;


typedef struct {
    __IO uint32_t DATA;
#define PIN0    0
#define PIN1    1
#define PIN2    2
#define PIN3    3
#define PIN4    4
#define PIN5    5
#define PIN6    6
#define PIN7    7
#define PIN8    8
#define PIN9    9
#define PIN10   10
#define PIN11   11
#define PIN12   12
#define PIN13   13
#define PIN14   14
#define PIN15   15
#define PIN16   16
#define PIN17   17
#define PIN18   18
#define PIN19   19
#define PIN20   20
#define PIN21   21
#define PIN22   22
#define PIN23   23
#define PIN24   24

    __IO uint32_t DIR;

    __IO uint32_t INTLVLTRG;

    __IO uint32_t INTBE;

    __IO uint32_t INTRISEEN;

    __IO uint32_t INTEN;

    __IO uint32_t INTRAWSTAT;

    __IO uint32_t INTSTAT;

    __IO uint32_t INTCLR;
} GPIO_TypeDef;




typedef struct {
    __IO uint32_t LDVAL;

    __I  uint32_t CVAL;

    __IO uint32_t CTRL;
} TIMR_TypeDef;


#define TIMR_CTRL_EN_Pos            0
#define TIMR_CTRL_EN_Msk            (0x01 << TIMR_CTRL_EN_Pos)
#define TIMR_CTRL_CLKSRC_Pos        1
#define TIMR_CTRL_CLKSRC_Msk        (0x01 << TIMR_CTRL_CLKSRC_Pos)
#define TIMR_CTRL_CASCADE_Pos       2
#define TIMR_CTRL_CASCADE_Msk       (0x01 << TIMR_CTRL_CASCADE_Pos)


typedef struct {
    __IO uint32_t PCTRL;

    __I  uint32_t PCVAL;

    uint32_t RESERVED[2];

    __IO uint32_t IE;

    __IO uint32_t IF;

    __IO uint32_t HALT;
} TIMRG_TypeDef;


#define TIMRG_PCTRL_EN_Pos          0
#define TIMRG_PCTRL_EN_Msk          (0x01 << TIMRG_PCTRL_EN_Pos)
#define TIMRG_PCTRL_HIGH_Pos        1
#define TIMRG_PCTRL_HIGH_Msk        (0x01 << TIMRG_PCTRL_HIGH_Pos)
#define TIMRG_PCTRL_CLKSRC_Pos      2
#define TIMRG_PCTRL_CLKSRC_Msk      (0x01 << TIMRG_PCTRL_CLKSRC_Pos)

#define TIMRG_IE_TIMR0_Pos          0
#define TIMRG_IE_TIMR0_Msk          (0x01 << TIMRG_IE_TIMR0_Pos)
#define TIMRG_IE_TIMR1_Pos          1
#define TIMRG_IE_TIMR1_Msk          (0x01 << TIMRG_IE_TIMR1_Pos)
#define TIMRG_IE_TIMR2_Pos          2
#define TIMRG_IE_TIMR2_Msk          (0x01 << TIMRG_IE_TIMR2_Pos)
#define TIMRG_IE_TIMR3_Pos          3
#define TIMRG_IE_TIMR3_Msk          (0x01 << TIMRG_IE_TIMR3_Pos)
#define TIMRG_IE_TIMR4_Pos          4
#define TIMRG_IE_TIMR4_Msk          (0x01 << TIMRG_IE_TIMR4_Pos)
#define TIMRG_IE_TIMR5_Pos          5
#define TIMRG_IE_TIMR5_Msk          (0x01 << TIMRG_IE_TIMR5_Pos)
#define TIMRG_IE_PULSE_Pos          16
#define TIMRG_IE_PULSE_Msk          (0x01 << TIMRG_IE_PULSE_Pos)

#define TIMRG_IF_TIMR0_Pos          0
#define TIMRG_IF_TIMR0_Msk          (0x01 << TIMRG_IF_TIMR0_Pos)
#define TIMRG_IF_TIMR1_Pos          1
#define TIMRG_IF_TIMR1_Msk          (0x01 << TIMRG_IF_TIMR1_Pos)
#define TIMRG_IF_TIMR2_Pos          2
#define TIMRG_IF_TIMR2_Msk          (0x01 << TIMRG_IF_TIMR2_Pos)
#define TIMRG_IF_TIMR3_Pos          3
#define TIMRG_IF_TIMR3_Msk          (0x01 << TIMRG_IF_TIMR3_Pos)
#define TIMRG_IF_TIMR4_Pos          4
#define TIMRG_IF_TIMR4_Msk          (0x01 << TIMRG_IF_TIMR4_Pos)
#define TIMRG_IF_TIMR5_Pos          5
#define TIMRG_IF_TIMR5_Msk          (0x01 << TIMRG_IF_TIMR5_Pos)
#define TIMRG_IF_PULSE_Pos          16
#define TIMRG_IF_PULSE_Msk          (0x01 << TIMRG_IF_PULSE_Pos)

#define TIMRG_HALT_TIMR0_Pos        0
#define TIMRG_HALT_TIMR0_Msk        (0x01 << TIMRG_HALT_TIMR0_Pos)
#define TIMRG_HALT_TIMR1_Pos        1
#define TIMRG_HALT_TIMR1_Msk        (0x01 << TIMRG_HALT_TIMR1_Pos)
#define TIMRG_HALT_TIMR2_Pos        2
#define TIMRG_HALT_TIMR2_Msk        (0x01 << TIMRG_HALT_TIMR2_Pos)
#define TIMRG_HALT_TIMR3_Pos        3
#define TIMRG_HALT_TIMR3_Msk        (0x01 << TIMRG_HALT_TIMR3_Pos)
#define TIMRG_HALT_TIMR4_Pos        4
#define TIMRG_HALT_TIMR4_Msk        (0x01 << TIMRG_HALT_TIMR4_Pos)
#define TIMRG_HALT_TIMR5_Pos        5
#define TIMRG_HALT_TIMR5_Msk        (0x01 << TIMRG_HALT_TIMR5_Pos)




typedef struct {
    __IO uint32_t DATA;

    __IO uint32_t CTRL;

    __IO uint32_t BAUD;

    __IO uint32_t FIFO;

    __IO uint32_t LINCR;

    union {
        __IO uint32_t CTSCR;

        __IO uint32_t RTSCR;
    };
} UART_TypeDef;


#define UART_DATA_DATA_Pos          0
#define UART_DATA_DATA_Msk          (0x1FF << UART_DATA_DATA_Pos)
#define UART_DATA_VALID_Pos         9
#define UART_DATA_VALID_Msk         (0x01 << UART_DATA_VALID_Pos)
#define UART_DATA_PAERR_Pos         10
#define UART_DATA_PAERR_Msk         (0x01 << UART_DATA_PAERR_Pos)

#define UART_CTRL_TXIDLE_Pos        0
#define UART_CTRL_TXIDLE_Msk        (0x01 << UART_CTRL_TXIDLE_Pos)
#define UART_CTRL_TXFF_Pos          1
#define UART_CTRL_TXFF_Msk          (0x01 << UART_CTRL_TXFF_Pos)
#define UART_CTRL_TXIE_Pos          2
#define UART_CTRL_TXIE_Msk          (0x01 << UART_CTRL_TXIE_Pos)
#define UART_CTRL_RXNE_Pos          3
#define UART_CTRL_RXNE_Msk          (0x01 << UART_CTRL_RXNE_Pos)
#define UART_CTRL_RXIE_Pos          4
#define UART_CTRL_RXIE_Msk          (0x01 << UART_CTRL_RXIE_Pos)
#define UART_CTRL_RXOV_Pos          5
#define UART_CTRL_RXOV_Msk          (0x01 << UART_CTRL_RXOV_Pos)
#define UART_CTRL_TXDOIE_Pos        6
#define UART_CTRL_TXDOIE_Msk        (0x01 << UART_CTRL_TXDOIE_Pos)
#define UART_CTRL_EN_Pos            9
#define UART_CTRL_EN_Msk            (0x01 << UART_CTRL_EN_Pos)
#define UART_CTRL_LOOP_Pos          10
#define UART_CTRL_LOOP_Msk          (0x01 << UART_CTRL_LOOP_Pos)
#define UART_CTRL_BAUDEN_Pos        13
#define UART_CTRL_BAUDEN_Msk        (0x01 << UART_CTRL_BAUDEN_Pos)
#define UART_CTRL_TOIE_Pos          14
#define UART_CTRL_TOIE_Msk          (0x01 << UART_CTRL_TOIE_Pos)
#define UART_CTRL_BRKDET_Pos        15
#define UART_CTRL_BRKDET_Msk        (0x01 << UART_CTRL_BRKDET_Pos)
#define UART_CTRL_BRKIE_Pos         16
#define UART_CTRL_BRKIE_Msk         (0x01 << UART_CTRL_BRKIE_Pos)
#define UART_CTRL_GENBRK_Pos        17
#define UART_CTRL_GENBRK_Msk        (0x01 << UART_CTRL_GENBRK_Pos)
#define UART_CTRL_DATA9b_Pos        18
#define UART_CTRL_DATA9b_Msk        (0x01 << UART_CTRL_DATA9b_Pos)
#define UART_CTRL_PARITY_Pos        19
#define UART_CTRL_PARITY_Msk        (0x07 << UART_CTRL_PARITY_Pos)
#define UART_CTRL_STOP2b_Pos        22
#define UART_CTRL_STOP2b_Msk        (0x03 << UART_CTRL_STOP2b_Pos)
#define UART_CTRL_TOTIME_Pos        24
//#define UART_CTRL_TOTIME_Msk      (0xFF << UART_CTRL_TOTIME_Pos)
#define UART_CTRL_TOTIME_Msk        ((uint32_t)0xFF << UART_CTRL_TOTIME_Pos)

#define UART_BAUD_BAUD_Pos          0
#define UART_BAUD_BAUD_Msk          (0x3FFF << UART_BAUD_BAUD_Pos)
#define UART_BAUD_TXD_Pos           14
#define UART_BAUD_TXD_Msk           (0x01 << UART_BAUD_TXD_Pos)
#define UART_BAUD_RXD_Pos           15
#define UART_BAUD_RXD_Msk           (0x01 << UART_BAUD_RXD_Pos)
#define UART_BAUD_RXTOIF_Pos        16
#define UART_BAUD_RXTOIF_Msk        (0x01 << UART_BAUD_RXTOIF_Pos)
#define UART_BAUD_TXIF_Pos          17
#define UART_BAUD_TXIF_Msk          (0x01 << UART_BAUD_TXIF_Pos)
#define UART_BAUD_BRKIF_Pos         18
#define UART_BAUD_BRKIF_Msk         (0x01 << UART_BAUD_BRKIF_Pos)
#define UART_BAUD_RXTHRF_Pos        19
#define UART_BAUD_RXTHRF_Msk        (0x01 << UART_BAUD_RXTHRF_Pos)
#define UART_BAUD_TXTHRF_Pos        20
#define UART_BAUD_TXTHRF_Msk        (0x01 << UART_BAUD_TXTHRF_Pos)
#define UART_BAUD_TOIF_Pos          21
#define UART_BAUD_TOIF_Msk          (0x01 << UART_BAUD_TOIF_Pos)
#define UART_BAUD_RXIF_Pos          22
#define UART_BAUD_RXIF_Msk          (0x01 << UART_BAUD_RXIF_Pos)
#define UART_BAUD_ABREN_Pos         23
#define UART_BAUD_ABREN_Msk         (0x01 << UART_BAUD_ABREN_Pos)
#define UART_BAUD_ABRBIT_Pos        24
#define UART_BAUD_ABRBIT_Msk        (0x03 << UART_BAUD_ABRBIT_Pos)
#define UART_BAUD_ABRERR_Pos        26
#define UART_BAUD_ABRERR_Msk        (0x01 << UART_BAUD_ABRERR_Pos)
#define UART_BAUD_TXDOIF_Pos        27
#define UART_BAUD_TXDOIF_Msk        (0x01 << UART_BAUD_TXDOIF_Pos)

#define UART_FIFO_RXLVL_Pos         0
#define UART_FIFO_RXLVL_Msk         (0xFF << UART_FIFO_RXLVL_Pos)
#define UART_FIFO_TXLVL_Pos         8
#define UART_FIFO_TXLVL_Msk         (0xFF << UART_FIFO_TXLVL_Pos)
#define UART_FIFO_RXTHR_Pos         16
#define UART_FIFO_RXTHR_Msk         (0xFF << UART_FIFO_RXTHR_Pos)
#define UART_FIFO_TXTHR_Pos         24
#define UART_FIFO_TXTHR_Msk         ((uint32_t)0xFF << UART_FIFO_TXTHR_Pos)

#define UART_LINCR_BRKDETIE_Pos     0
#define UART_LINCR_BRKDETIE_Msk     (0xFF << UART_LINCR_BRKDETIE_Pos)
#define UART_LINCR_BRKDETIF_Pos     1
#define UART_LINCR_BRKDETIF_Msk     (0xFF << UART_LINCR_BRKDETIF_Pos)
#define UART_LINCR_GENBRKIE_Pos     2
#define UART_LINCR_GENBRKIE_Msk     (0xFF << UART_LINCR_GENBRKIE_Pos)
#define UART_LINCR_GENBRKIF_Pos     3
#define UART_LINCR_GENBRKIF_Msk     (0xFF << UART_LINCR_GENBRKIF_Pos)
#define UART_LINCR_GENBRK_Pos       4
#define UART_LINCR_GENBRK_Msk       (0xFF << UART_LINCR_GENBRK_Pos)

#define UART_CTSCR_EN_Pos           0
#define UART_CTSCR_EN_Msk           (0x01 << UART_CTSCR_EN_Pos)
#define UART_CTSCR_POL_Pos          2
#define UART_CTSCR_POL_Msk          (0x01 << UART_CTSCR_POL_Pos)
#define UART_CTSCR_STAT_Pos         7
#define UART_CTSCR_STAT_Msk         (0x01 << UART_CTSCR_STAT_Pos)

#define UART_RTSCR_EN_Pos           1
#define UART_RTSCR_EN_Msk           (0x01 << UART_RTSCR_EN_Pos)
#define UART_RTSCR_POL_Pos          3
#define UART_RTSCR_POL_Msk          (0x01 << UART_RTSCR_POL_Pos)
#define UART_RTSCR_THR_Pos          4
#define UART_RTSCR_THR_Msk          (0x07 << UART_RTSCR_THR_Pos)
#define UART_RTSCR_STAT_Pos         8
#define UART_RTSCR_STAT_Msk         (0x01 << UART_RTSCR_STAT_Pos)




typedef struct {
    __IO uint32_t CTRL;

    __IO uint32_t DATA;

    __IO uint32_t STAT;

    __IO uint32_t IE;

    __IO uint32_t IF;
} SPI_TypeDef;


#define SPI_CTRL_CLKDIV_Pos         0
#define SPI_CTRL_CLKDIV_Msk         (0x07 << SPI_CTRL_CLKDIV_Pos)
#define SPI_CTRL_EN_Pos             3
#define SPI_CTRL_EN_Msk             (0x01 << SPI_CTRL_EN_Pos)
#define SPI_CTRL_SIZE_Pos           4
#define SPI_CTRL_SIZE_Msk           (0x0F << SPI_CTRL_SIZE_Pos)
#define SPI_CTRL_CPHA_Pos           8
#define SPI_CTRL_CPHA_Msk           (0x01 << SPI_CTRL_CPHA_Pos)
#define SPI_CTRL_CPOL_Pos           9
#define SPI_CTRL_CPOL_Msk           (0x01 << SPI_CTRL_CPOL_Pos)
#define SPI_CTRL_FFS_Pos            10
#define SPI_CTRL_FFS_Msk            (0x03 << SPI_CTRL_FFS_Pos)
#define SPI_CTRL_MSTR_Pos           12
#define SPI_CTRL_MSTR_Msk           (0x01 << SPI_CTRL_MSTR_Pos)
#define SPI_CTRL_FAST_Pos           13
#define SPI_CTRL_FAST_Msk           (0x01 << SPI_CTRL_FAST_Pos)
#define SPI_CTRL_FILTE_Pos          16
#define SPI_CTRL_FILTE_Msk          (0x01 << SPI_CTRL_FILTE_Pos)
#define SPI_CTRL_SSN_H_Pos          17
#define SPI_CTRL_SSN_H_Msk          (0x01 << SPI_CTRL_SSN_H_Pos)
#define SPI_CTRL_TFCLR_Pos          24
#define SPI_CTRL_TFCLR_Msk          (0x01 << SPI_CTRL_TFCLR_Pos)
#define SPI_CTRL_RFCLR_Pos          25
#define SPI_CTRL_RFCLR_Msk          (0x01 << SPI_CTRL_RFCLR_Pos)

#define SPI_STAT_WTC_Pos            0
#define SPI_STAT_WTC_Msk            (0x01 << SPI_STAT_WTC_Pos)
#define SPI_STAT_TFE_Pos            1
#define SPI_STAT_TFE_Msk            (0x01 << SPI_STAT_TFE_Pos)
#define SPI_STAT_TFNF_Pos           2
#define SPI_STAT_TFNF_Msk           (0x01 << SPI_STAT_TFNF_Pos)
#define SPI_STAT_RFNE_Pos           3
#define SPI_STAT_RFNE_Msk           (0x01 << SPI_STAT_RFNE_Pos)
#define SPI_STAT_RFF_Pos            4
#define SPI_STAT_RFF_Msk            (0x01 << SPI_STAT_RFF_Pos)
#define SPI_STAT_RFOVF_Pos          5
#define SPI_STAT_RFOVF_Msk          (0x01 << SPI_STAT_RFOVF_Pos)
#define SPI_STAT_TFLVL_Pos          6
#define SPI_STAT_TFLVL_Msk          (0x07 << SPI_STAT_TFLVL_Pos)
#define SPI_STAT_RFLVL_Pos          9
#define SPI_STAT_RFLVL_Msk          (0x07 << SPI_STAT_RFLVL_Pos)
#define SPI_STAT_BUSY_Pos           15
#define SPI_STAT_BUSY_Msk           (0x01 << SPI_STAT_BUSY_Pos)

#define SPI_IE_RFOVF_Pos            0
#define SPI_IE_RFOVF_Msk            (0x01 << SPI_IE_RFOVF_Pos)
#define SPI_IE_RFF_Pos              1
#define SPI_IE_RFF_Msk              (0x01 << SPI_IE_RFF_Pos)
#define SPI_IE_RFHF_Pos             2
#define SPI_IE_RFHF_Msk             (0x01 << SPI_IE_RFHF_Pos)
#define SPI_IE_TFE_Pos              3
#define SPI_IE_TFE_Msk              (0x01 << SPI_IE_TFE_Pos)
#define SPI_IE_TFHF_Pos             4
#define SPI_IE_TFHF_Msk             (0x01 << SPI_IE_TFHF_Pos)
#define SPI_IE_WTC_Pos              8
#define SPI_IE_WTC_Msk              (0x01 << SPI_IE_WTC_Pos)
#define SPI_IE_FTC_Pos              9
#define SPI_IE_FTC_Msk              (0x01 << SPI_IE_FTC_Pos)

#define SPI_IF_RFOVF_Pos            0
#define SPI_IF_RFOVF_Msk            (0x01 << SPI_IF_RFOVF_Pos)
#define SPI_IF_RFF_Pos              1
#define SPI_IF_RFF_Msk              (0x01 << SPI_IF_RFF_Pos)
#define SPI_IF_RFHF_Pos             2
#define SPI_IF_RFHF_Msk             (0x01 << SPI_IF_RFHF_Pos)
#define SPI_IF_TFE_Pos              3
#define SPI_IF_TFE_Msk              (0x01 << SPI_IF_TFE_Pos)
#define SPI_IF_TFHF_Pos             4
#define SPI_IF_TFHF_Msk             (0x01 << SPI_IF_TFHF_Pos)
#define SPI_IF_WTC_Pos              8
#define SPI_IF_WTC_Msk              (0x01 << SPI_IF_WTC_Pos)
#define SPI_IF_FTC_Pos              9
#define SPI_IF_FTC_Msk              (0x01 << SPI_IF_FTC_Pos)




typedef struct {
    __IO uint32_t CLKDIV;

    __IO uint32_t CTRL;

    __IO uint32_t MSTDAT;

    __IO uint32_t MSTCMD;

    __IO uint32_t SLVCR;

    __IO uint32_t SLVIF;

    __IO uint32_t SLVTX;

    __IO uint32_t SLVRX;
} I2C_TypeDef;


#define I2C_CTRL_MSTIE_Pos          6
#define I2C_CTRL_MSTIE_Msk          (0x01 << I2C_CTRL_MSTIE_Pos)
#define I2C_CTRL_EN_Pos             7
#define I2C_CTRL_EN_Msk             (0x01 << I2C_CTRL_EN_Pos)

#define I2C_MSTCMD_IF_Pos           0
#define I2C_MSTCMD_IF_Msk           (0x01 << I2C_MSTCMD_IF_Pos)
#define I2C_MSTCMD_TIP_Pos          1
#define I2C_MSTCMD_TIP_Msk          (0x01 << I2C_MSTCMD_TIP_Pos)
#define I2C_MSTCMD_ACK_Pos          3
#define I2C_MSTCMD_ACK_Msk          (0x01 << I2C_MSTCMD_ACK_Pos)
#define I2C_MSTCMD_WR_Pos           4
#define I2C_MSTCMD_WR_Msk           (0x01 << I2C_MSTCMD_WR_Pos)
#define I2C_MSTCMD_RD_Pos           5
#define I2C_MSTCMD_RD_Msk           (0x01 << I2C_MSTCMD_RD_Pos)
#define I2C_MSTCMD_BUSY_Pos         6
#define I2C_MSTCMD_BUSY_Msk         (0x01 << I2C_MSTCMD_BUSY_Pos)
#define I2C_MSTCMD_STO_Pos          6
#define I2C_MSTCMD_STO_Msk          (0x01 << I2C_MSTCMD_STO_Pos)
#define I2C_MSTCMD_RXACK_Pos        7
#define I2C_MSTCMD_RXACK_Msk        (0x01 << I2C_MSTCMD_RXACK_Pos)
#define I2C_MSTCMD_STA_Pos          7
#define I2C_MSTCMD_STA_Msk          (0x01 << I2C_MSTCMD_STA_Pos)

#define I2C_SLVCR_IM_RXEND_Pos      0
#define I2C_SLVCR_IM_RXEND_Msk      (0x01 << I2C_SLVCR_IM_RXEND_Pos)
#define I2C_SLVCR_IM_TXEND_Pos      1
#define I2C_SLVCR_IM_TXEND_Msk      (0x01 << I2C_SLVCR_IM_TXEND_Pos)
#define I2C_SLVCR_IM_STADET_Pos     2
#define I2C_SLVCR_IM_STADET_Msk     (0x01 << I2C_SLVCR_IM_STADET_Pos)
#define I2C_SLVCR_IM_STODET_Pos     3
#define I2C_SLVCR_IM_STODET_Msk     (0x01 << I2C_SLVCR_IM_STODET_Pos)
#define I2C_SLVCR_IM_RDREQ_Pos      4
#define I2C_SLVCR_IM_RDREQ_Msk      (0x01 << I2C_SLVCR_IM_RDREQ_Pos)
#define I2C_SLVCR_IM_WRREQ_Pos      5
#define I2C_SLVCR_IM_WRREQ_Msk      (0x01 << I2C_SLVCR_IM_WRREQ_Pos)
#define I2C_SLVCR_ADDR7b_Pos        16
#define I2C_SLVCR_ADDR7b_Msk        (0x01 << I2C_SLVCR_ADDR7b_Pos)
#define I2C_SLVCR_ACK_Pos           17
#define I2C_SLVCR_ACK_Msk           (0x01 << I2C_SLVCR_ACK_Pos)
#define I2C_SLVCR_SLAVE_Pos         18
#define I2C_SLVCR_SLAVE_Msk         (0x01 << I2C_SLVCR_SLAVE_Pos)
#define I2C_SLVCR_DEBOUNCE_Pos      19
#define I2C_SLVCR_DEBOUNCE_Msk      (0x01 << I2C_SLVCR_DEBOUNCE_Pos)
#define I2C_SLVCR_ADDR_Pos          20
#define I2C_SLVCR_ADDR_Msk          (0x3FF << I2C_SLVCR_ADDR_Pos)

#define I2C_SLVIF_RXEND_Pos         0
#define I2C_SLVIF_RXEND_Msk         (0x01 << I2C_SLVIF_RXEND_Pos)
#define I2C_SLVIF_TXEND_Pos         1
#define I2C_SLVIF_TXEND_Msk         (0x01 << I2C_SLVIF_TXEND_Pos)
#define I2C_SLVIF_STADET_Pos        2
#define I2C_SLVIF_STADET_Msk        (0x01 << I2C_SLVIF_STADET_Pos)
#define I2C_SLVIF_STODET_Pos        3
#define I2C_SLVIF_STODET_Msk        (0x01 << I2C_SLVIF_STODET_Pos)
#define I2C_SLVIF_RDREQ_Pos         4
#define I2C_SLVIF_RDREQ_Msk         (0x01 << I2C_SLVIF_RDREQ_Pos)
#define I2C_SLVIF_WRREQ_Pos         5
#define I2C_SLVIF_WRREQ_Msk         (0x01 << I2C_SLVIF_WRREQ_Pos)
#define I2C_SLVIF_ACTIVE_Pos        6
#define I2C_SLVIF_ACTIVE_Msk        (0x01 << I2C_SLVIF_ACTIVE_Pos)


typedef struct {
    __IO uint32_t CTRL;

    __IO uint32_t START;

    __IO uint32_t IE;

    __IO uint32_t IF;

    struct {
        __IO uint32_t STAT;

        __IO uint32_t DATA;

        uint32_t RESERVED[2];
    } CH[8];

    __IO uint32_t CTRL1;

    __IO uint32_t CTRL2;

    uint32_t RESERVED[2];

    __IO uint32_t CALIBSET;

    __IO uint32_t CALIBEN;
} ADC_TypeDef;


#define ADC_CTRL_CH0_Pos            0       //通道选中
#define ADC_CTRL_CH0_Msk            (0x01 << ADC_CTRL_CH0_Pos)
#define ADC_CTRL_CH1_Pos            1
#define ADC_CTRL_CH1_Msk            (0x01 << ADC_CTRL_CH1_Pos)
#define ADC_CTRL_CH2_Pos            2
#define ADC_CTRL_CH2_Msk            (0x01 << ADC_CTRL_CH2_Pos)
#define ADC_CTRL_CH3_Pos            3
#define ADC_CTRL_CH3_Msk            (0x01 << ADC_CTRL_CH3_Pos)
#define ADC_CTRL_CH4_Pos            4
#define ADC_CTRL_CH4_Msk            (0x01 << ADC_CTRL_CH4_Pos)
#define ADC_CTRL_CH5_Pos            5
#define ADC_CTRL_CH5_Msk            (0x01 << ADC_CTRL_CH5_Pos)
#define ADC_CTRL_CH6_Pos            6
#define ADC_CTRL_CH6_Msk            (0x01 << ADC_CTRL_CH6_Pos)
#define ADC_CTRL_CH7_Pos            7
#define ADC_CTRL_CH7_Msk            (0x01 << ADC_CTRL_CH7_Pos)
#define ADC_CTRL_AVG_Pos            8
#define ADC_CTRL_AVG_Msk            (0x0F << ADC_CTRL_AVG_Pos)
#define ADC_CTRL_EN_Pos             12
#define ADC_CTRL_EN_Msk             (0x01 << ADC_CTRL_EN_Pos)
#define ADC_CTRL_CONT_Pos           13
#define ADC_CTRL_CONT_Msk           (0x01 << ADC_CTRL_CONT_Pos)
#define ADC_CTRL_TRIG_Pos           14
#define ADC_CTRL_TRIG_Msk           (0x01 << ADC_CTRL_TRIG_Pos)
#define ADC_CTRL_CLKSRC_Pos         15
#define ADC_CTRL_CLKSRC_Msk         (0x01 << ADC_CTRL_CLKSRC_Pos)
#define ADC_CTRL_FIFOCLR_Pos        24
#define ADC_CTRL_FIFOCLR_Msk        (((uint32_t)0xFF) << ADC_CTRL_FIFOCLR_Pos)

#define ADC_START_GO_Pos            0
#define ADC_START_GO_Msk            (0x01 << ADC_START_GO_Pos)
#define ADC_START_BUSY_Pos          4
#define ADC_START_BUSY_Msk          (0x01 << ADC_START_BUSY_Pos)

#define ADC_IE_CH0EOC_Pos           0       //End Of Convertion
#define ADC_IE_CH0EOC_Msk           (0x01 << ADC_IE_CH0EOC_Pos)
#define ADC_IE_CH0OVF_Pos           1       //Overflow
#define ADC_IE_CH0OVF_Msk           (0x01 << ADC_IE_CH0OVF_Pos)
#define ADC_IE_CH0HFULL_Pos         2       //FIFO Half Full
#define ADC_IE_CH0HFULL_Msk         (0x01 << ADC_IE_CH0HFULL_Pos)
#define ADC_IE_CH0FULL_Pos          3       //FIFO Full
#define ADC_IE_CH0FULL_Msk          (0x01 << ADC_IE_CH0FULL_Pos)
#define ADC_IE_CH1EOC_Pos           4
#define ADC_IE_CH1EOC_Msk           (0x01 << ADC_IE_CH1EOC_Pos)
#define ADC_IE_CH1OVF_Pos           5
#define ADC_IE_CH1OVF_Msk           (0x01 << ADC_IE_CH1OVF_Pos)
#define ADC_IE_CH1HFULL_Pos         6
#define ADC_IE_CH1HFULL_Msk         (0x01 << ADC_IE_CH1HFULL_Pos)
#define ADC_IE_CH1FULL_Pos          7
#define ADC_IE_CH1FULL_Msk          (0x01 << ADC_IE_CH1FULL_Pos)
#define ADC_IE_CH2EOC_Pos           8
#define ADC_IE_CH2EOC_Msk           (0x01 << ADC_IE_CH2EOC_Pos)
#define ADC_IE_CH2OVF_Pos           9
#define ADC_IE_CH2OVF_Msk           (0x01 << ADC_IE_CH2OVF_Pos)
#define ADC_IE_CH2HFULL_Pos         10
#define ADC_IE_CH2HFULL_Msk         (0x01 << ADC_IE_CH2HFULL_Pos)
#define ADC_IE_CH2FULL_Pos          11
#define ADC_IE_CH2FULL_Msk          (0x01 << ADC_IE_CH2FULL_Pos)
#define ADC_IE_CH3EOC_Pos           12
#define ADC_IE_CH3EOC_Msk           (0x01 << ADC_IE_CH3EOC_Pos)
#define ADC_IE_CH3OVF_Pos           13
#define ADC_IE_CH3OVF_Msk           (0x01 << ADC_IE_CH3OVF_Pos)
#define ADC_IE_CH3HFULL_Pos         14
#define ADC_IE_CH3HFULL_Msk         (0x01 << ADC_IE_CH3HFULL_Pos)
#define ADC_IE_CH3FULL_Pos          15
#define ADC_IE_CH3FULL_Msk          (0x01 << ADC_IE_CH3FULL_Pos)
#define ADC_IE_CH4EOC_Pos           16
#define ADC_IE_CH4EOC_Msk           (0x01 << ADC_IE_CH4EOC_Pos)
#define ADC_IE_CH4OVF_Pos           17
#define ADC_IE_CH4OVF_Msk           (0x01 << ADC_IE_CH4OVF_Pos)
#define ADC_IE_CH4HFULL_Pos         18
#define ADC_IE_CH4HFULL_Msk         (0x01 << ADC_IE_CH4HFULL_Pos)
#define ADC_IE_CH4FULL_Pos          19
#define ADC_IE_CH4FULL_Msk          (0x01 << ADC_IE_CH4FULL_Pos)
#define ADC_IE_CH5EOC_Pos           20
#define ADC_IE_CH5EOC_Msk           (0x01 << ADC_IE_CH5EOC_Pos)
#define ADC_IE_CH5OVF_Pos           21
#define ADC_IE_CH5OVF_Msk           (0x01 << ADC_IE_CH5OVF_Pos)
#define ADC_IE_CH5HFULL_Pos         22
#define ADC_IE_CH5HFULL_Msk         (0x01 << ADC_IE_CH5HFULL_Pos)
#define ADC_IE_CH5FULL_Pos          23
#define ADC_IE_CH5FULL_Msk          (0x01 << ADC_IE_CH5FULL_Pos)
#define ADC_IE_CH6EOC_Pos           24
#define ADC_IE_CH6EOC_Msk           (0x01 << ADC_IE_CH6EOC_Pos)
#define ADC_IE_CH6OVF_Pos           25
#define ADC_IE_CH6OVF_Msk           (0x01 << ADC_IE_CH6OVF_Pos)
#define ADC_IE_CH6HFULL_Pos         26
#define ADC_IE_CH6HFULL_Msk         (0x01 << ADC_IE_CH6HFULL_Pos)
#define ADC_IE_CH6FULL_Pos          27
#define ADC_IE_CH6FULL_Msk          (0x01 << ADC_IE_CH6FULL_Pos)
#define ADC_IE_CH7EOC_Pos           28
#define ADC_IE_CH7EOC_Msk           (0x01 << ADC_IE_CH7EOC_Pos)
#define ADC_IE_CH7OVF_Pos           29
#define ADC_IE_CH7OVF_Msk           (0x01 << ADC_IE_CH7OVF_Pos)
#define ADC_IE_CH7HFULL_Pos         30
#define ADC_IE_CH7HFULL_Msk         (0x01 << ADC_IE_CH7HFULL_Pos)
#define ADC_IE_CH7FULL_Pos          31
//#define ADC_IE_CH7FULL_Msk            (0x01 << ADC_IE_CH7FULL_Pos)
#define ADC_IE_CH7FULL_Msk          ((uint32_t)0x01 << ADC_IE_CH7FULL_Pos)

#define ADC_IF_CH0EOC_Pos           0
#define ADC_IF_CH0EOC_Msk           (0x01 << ADC_IF_CH0EOC_Pos)
#define ADC_IF_CH0OVF_Pos           1
#define ADC_IF_CH0OVF_Msk           (0x01 << ADC_IF_CH0OVF_Pos)
#define ADC_IF_CH0HFULL_Pos         2
#define ADC_IF_CH0HFULL_Msk         (0x01 << ADC_IF_CH0HFULL_Pos)
#define ADC_IF_CH0FULL_Pos          3
#define ADC_IF_CH0FULL_Msk          (0x01 << ADC_IF_CH0FULL_Pos)
#define ADC_IF_CH1EOC_Pos           4
#define ADC_IF_CH1EOC_Msk           (0x01 << ADC_IF_CH1EOC_Pos)
#define ADC_IF_CH1OVF_Pos           5
#define ADC_IF_CH1OVF_Msk           (0x01 << ADC_IF_CH1OVF_Pos)
#define ADC_IF_CH1HFULL_Pos         6
#define ADC_IF_CH1HFULL_Msk         (0x01 << ADC_IF_CH1HFULL_Pos)
#define ADC_IF_CH1FULL_Pos          7
#define ADC_IF_CH1FULL_Msk          (0x01 << ADC_IF_CH1FULL_Pos)
#define ADC_IF_CH2EOC_Pos           8
#define ADC_IF_CH2EOC_Msk           (0x01 << ADC_IF_CH2EOC_Pos)
#define ADC_IF_CH2OVF_Pos           9
#define ADC_IF_CH2OVF_Msk           (0x01 << ADC_IF_CH2OVF_Pos)
#define ADC_IF_CH2HFULL_Pos         10
#define ADC_IF_CH2HFULL_Msk         (0x01 << ADC_IF_CH2HFULL_Pos)
#define ADC_IF_CH2FULL_Pos          11
#define ADC_IF_CH2FULL_Msk          (0x01 << ADC_IF_CH2FULL_Pos)
#define ADC_IF_CH3EOC_Pos           12
#define ADC_IF_CH3EOC_Msk           (0x01 << ADC_IF_CH3EOC_Pos)
#define ADC_IF_CH3OVF_Pos           13
#define ADC_IF_CH3OVF_Msk           (0x01 << ADC_IF_CH3OVF_Pos)
#define ADC_IF_CH3HFULL_Pos         14
#define ADC_IF_CH3HFULL_Msk         (0x01 << ADC_IF_CH3HFULL_Pos)
#define ADC_IF_CH3FULL_Pos          15
#define ADC_IF_CH3FULL_Msk          (0x01 << ADC_IF_CH3FULL_Pos)
#define ADC_IF_CH4EOC_Pos           16
#define ADC_IF_CH4EOC_Msk           (0x01 << ADC_IF_CH4EOC_Pos)
#define ADC_IF_CH4OVF_Pos           17
#define ADC_IF_CH4OVF_Msk           (0x01 << ADC_IF_CH4OVF_Pos)
#define ADC_IF_CH4HFULL_Pos         18
#define ADC_IF_CH4HFULL_Msk         (0x01 << ADC_IF_CH4HFULL_Pos)
#define ADC_IF_CH4FULL_Pos          19
#define ADC_IF_CH4FULL_Msk          (0x01 << ADC_IF_CH4FULL_Pos)
#define ADC_IF_CH5EOC_Pos           20
#define ADC_IF_CH5EOC_Msk           (0x01 << ADC_IF_CH5EOC_Pos)
#define ADC_IF_CH5OVF_Pos           21
#define ADC_IF_CH5OVF_Msk           (0x01 << ADC_IF_CH5OVF_Pos)
#define ADC_IF_CH5HFULL_Pos         22
#define ADC_IF_CH5HFULL_Msk         (0x01 << ADC_IF_CH5HFULL_Pos)
#define ADC_IF_CH5FULL_Pos          23
#define ADC_IF_CH5FULL_Msk          (0x01 << ADC_IF_CH5FULL_Pos)
#define ADC_IF_CH6EOC_Pos           24
#define ADC_IF_CH6EOC_Msk           (0x01 << ADC_IF_CH6EOC_Pos)
#define ADC_IF_CH6OVF_Pos           25
#define ADC_IF_CH6OVF_Msk           (0x01 << ADC_IF_CH6OVF_Pos)
#define ADC_IF_CH6HFULL_Pos         26
#define ADC_IF_CH6HFULL_Msk         (0x01 << ADC_IF_CH6HFULL_Pos)
#define ADC_IF_CH6FULL_Pos          27
#define ADC_IF_CH6FULL_Msk          (0x01 << ADC_IF_CH6FULL_Pos)
#define ADC_IF_CH7EOC_Pos           28
#define ADC_IF_CH7EOC_Msk           (0x01 << ADC_IF_CH7EOC_Pos)
#define ADC_IF_CH7OVF_Pos           29
#define ADC_IF_CH7OVF_Msk           (0x01 << ADC_IF_CH7OVF_Pos)
#define ADC_IF_CH7HFULL_Pos         30
#define ADC_IF_CH7HFULL_Msk         (0x01 << ADC_IF_CH7HFULL_Pos)
#define ADC_IF_CH7FULL_Pos          31
#define ADC_IF_CH7FULL_Msk          (0x01 << ADC_IF_CH7FULL_Pos)

#define ADC_STAT_EOC_Pos            0
#define ADC_STAT_EOC_Msk            (0x01 << ADC_STAT_EOC_Pos)
#define ADC_STAT_OVF_Pos            1
#define ADC_STAT_OVF_Msk            (0x01 << ADC_STAT_OVF_Pos)
#define ADC_STAT_HFULL_Pos          2
#define ADC_STAT_HFULL_Msk          (0x01 << ADC_STAT_HFULL_Pos)
#define ADC_STAT_FULL_Pos           3
#define ADC_STAT_FULL_Msk           (0x01 << ADC_STAT_FULL_Pos)
#define ADC_STAT_EMPTY_Pos          4
#define ADC_STAT_EMPTY_Msk          (0x01 << ADC_STAT_EMPTY_Pos)

#define ADC_CTRL1_RIN_Pos           4
#define ADC_CTRL1_RIN_Msk           (0x07 << ADC_CTRL1_RIN_Pos)

#define ADC_CTRL2_RESET_Pos         0
#define ADC_CTRL2_RESET_Msk         (0x01 << ADC_CTRL2_RESET_Pos)
#define ADC_CTRL2_ADCEVCM_Pos       1
#define ADC_CTRL2_ADCEVCM_Msk       (0x01 << ADC_CTRL2_ADCEVCM_Pos)
#define ADC_CTRL2_PGAIVCM_Pos       2
#define ADC_CTRL2_PGAIVCM_Msk       (0x01 << ADC_CTRL2_PGAIVCM_Pos)
#define ADC_CTRL2_PGAGAIN_Pos       3
#define ADC_CTRL2_PGAGAIN_Msk       (0x07 << ADC_CTRL2_PGAGAIN_Pos)
#define ADC_CTRL2_REFPOUT_Pos       23
#define ADC_CTRL2_REFPOUT_Msk       (0x01 << ADC_CTRL2_REFPOUT_Pos
#define ADC_CTRL2_CLKDIV_Pos        24
#define ADC_CTRL2_CLKDIV_Msk        (0x1F << ADC_CTRL2_CLKDIV_Pos)
#define ADC_CTRL2_PGAVCM_Pos        29
#define ADC_CTRL2_PGAVCM_Msk        (((uint32_t)0x07) << ADC_CTRL2_PGAVCM_Pos)

#define ADC_CALIBSET_OFFSET_Pos     0
#define ADC_CALIBSET_OFFSET_Msk     (0x1FF<< ADC_CALIBSET_OFFSET_Pos)
#define ADC_CALIBSET_K_Pos          16
#define ADC_CALIBSET_K_Msk          (0x1FF<< ADC_CALIBSET_K_Pos)

#define ADC_CALIBEN_OFFSET_Pos      0
#define ADC_CALIBEN_OFFSET_Msk      (0x01 << ADC_CALIBEN_OFFSET_Pos)
#define ADC_CALIBEN_K_Pos           1
#define ADC_CALIBEN_K_Msk           (0x01 << ADC_CALIBEN_K_Pos)




typedef struct {
    __IO uint32_t MODE;

    __IO uint32_t PERA;

    __IO uint32_t HIGHA;

    __IO uint32_t DZA;

    __IO uint32_t PERB;

    __IO uint32_t HIGHB;

    __IO uint32_t DZB;

    __IO uint32_t INIOUT;
} PWM_TypeDef;


#define PWM_INIOUT_PWMA_Pos     0
#define PWM_INIOUT_PWMA_Msk     (0x01 << PWM_INIOUT_PWMA_Pos)
#define PWM_INIOUT_PWMB_Pos     1
#define PWM_INIOUT_PWMB_Msk     (0x01 << PWM_INIOUT_PWMB_Pos)


typedef struct {
    __IO uint32_t FORCEH;

    __IO uint32_t ADTRG0A;
    __IO uint32_t ADTRG0B;

    __IO uint32_t ADTRG1A;
    __IO uint32_t ADTRG1B;

    __IO uint32_t ADTRG2A;
    __IO uint32_t ADTRG2B;

    __IO uint32_t ADTRG3A;
    __IO uint32_t ADTRG3B;

    __IO uint32_t ADTRG4A;
    __IO uint32_t ADTRG4B;

    __IO uint32_t ADTRG5A;
    __IO uint32_t ADTRG5B;

    uint32_t RESERVED[3];

    __IO uint32_t HALT;

    __IO uint32_t CHEN;

    __IO uint32_t IE;

    __IO uint32_t IF;

    __IO uint32_t IM;

    __IO uint32_t IRS;
} PWMG_TypeDef;


#define PWMG_FORCEH_PWM0_Pos        0
#define PWMG_FORCEH_PWM0_Msk        (0x01 << PWMG_FORCEH_PWM0_Pos)
#define PWMG_FORCEH_PWM1_Pos        1
#define PWMG_FORCEH_PWM1_Msk        (0x01 << PWMG_FORCEH_PWM1_Pos)
#define PWMG_FORCEH_PWM2_Pos        2
#define PWMG_FORCEH_PWM2_Msk        (0x01 << PWMG_FORCEH_PWM2_Pos)
#define PWMG_FORCEH_PWM3_Pos        3
#define PWMG_FORCEH_PWM3_Msk        (0x01 << PWMG_FORCEH_PWM3_Pos)
#define PWMG_FORCEH_PWM4_Pos        4
#define PWMG_FORCEH_PWM4_Msk        (0x01 << PWMG_FORCEH_PWM4_Pos)
#define PWMG_FORCEH_PWM5_Pos        5
#define PWMG_FORCEH_PWM5_Msk        (0x01 << PWMG_FORCEH_PWM5_Pos)

#define PWMG_ADTRG_VALUE_Pos        0
#define PWMG_ADTRG_VALUE_Msk        (0xFFFF << PWMG_ADTRG0A_VALUE_Pos)
#define PWMG_ADTRG_EVEN_Pos         16
#define PWMG_ADTRG_EVEN_Msk         (0x01 << PWMG_ADTRG0A_EVEN_Pos)
#define PWMG_ADTRG_EN_Pos           17
#define PWMG_ADTRG_EN_Msk           (0x01 << PWMG_ADTRG0A_EN_Pos)

#define PWMG_HALT_EN_Pos            0
#define PWMG_HALT_EN_Msk            (0x01 << PWMG_HALT_EN_Pos)
#define PWMG_HALT_PWM0_Pos          1
#define PWMG_HALT_PWM0_Msk          (0x01 << PWMG_HALT_PWM0_Pos)
#define PWMG_HALT_PWM1_Pos          2
#define PWMG_HALT_PWM1_Msk          (0x01 << PWMG_HALT_PWM1_Pos)
#define PWMG_HALT_PWM2_Pos          3
#define PWMG_HALT_PWM2_Msk          (0x01 << PWMG_HALT_PWM2_Pos)
#define PWMG_HALT_PWM3_Pos          4
#define PWMG_HALT_PWM3_Msk          (0x01 << PWMG_HALT_PWM3_Pos)
#define PWMG_HALT_PWM4_Pos          5
#define PWMG_HALT_PWM4_Msk          (0x01 << PWMG_HALT_PWM4_Pos)
#define PWMG_HALT_PWM5_Pos          6
#define PWMG_HALT_PWM5_Msk          (0x01 << PWMG_HALT_PWM5_Pos)
#define PWMG_HALT_STOPCNT_Pos       7
#define PWMG_HALT_STOPCNT_Msk       (0x01 << PWMG_HALT_STOPCNT_Pos)
#define PWMG_HALT_INLVL_Pos         8
#define PWMG_HALT_INLVL_Msk         (0x01 << PWMG_HALT_INLVL_Pos)
#define PWMG_HALT_OUTLVL_Pos        9
#define PWMG_HALT_OUTLVL_Msk        (0x01 << PWMG_HALT_OUTLVL_Pos)
#define PWMG_HALT_STAT_Pos          10
#define PWMG_HALT_STAT_Msk          (0x01 << PWMG_HALT_STAT_Pos)

#define PWMG_CHEN_PWM0A_Pos         0
#define PWMG_CHEN_PWM0A_Msk         (0x01 << PWMG_CHEN_PWM0A_Pos)
#define PWMG_CHEN_PWM0B_Pos         1
#define PWMG_CHEN_PWM0B_Msk         (0x01 << PWMG_CHEN_PWM0B_Pos)
#define PWMG_CHEN_PWM1A_Pos         2
#define PWMG_CHEN_PWM1A_Msk         (0x01 << PWMG_CHEN_PWM1A_Pos)
#define PWMG_CHEN_PWM1B_Pos         3
#define PWMG_CHEN_PWM1B_Msk         (0x01 << PWMG_CHEN_PWM1B_Pos)
#define PWMG_CHEN_PWM2A_Pos         4
#define PWMG_CHEN_PWM2A_Msk         (0x01 << PWMG_CHEN_PWM2A_Pos)
#define PWMG_CHEN_PWM2B_Pos         5
#define PWMG_CHEN_PWM2B_Msk         (0x01 << PWMG_CHEN_PWM2B_Pos)
#define PWMG_CHEN_PWM3A_Pos         6
#define PWMG_CHEN_PWM3A_Msk         (0x01 << PWMG_CHEN_PWM3A_Pos)
#define PWMG_CHEN_PWM3B_Pos         7
#define PWMG_CHEN_PWM3B_Msk         (0x01 << PWMG_CHEN_PWM3B_Pos)
#define PWMG_CHEN_PWM4A_Pos         8
#define PWMG_CHEN_PWM4A_Msk         (0x01 << PWMG_CHEN_PWM4A_Pos)
#define PWMG_CHEN_PWM4B_Pos         9
#define PWMG_CHEN_PWM4B_Msk         (0x01 << PWMG_CHEN_PWM4B_Pos)
#define PWMG_CHEN_PWM5A_Pos         10
#define PWMG_CHEN_PWM5A_Msk         (0x01 << PWMG_CHEN_PWM5A_Pos)
#define PWMG_CHEN_PWM5B_Pos         11
#define PWMG_CHEN_PWM5B_Msk         (0x01 << PWMG_CHEN_PWM5B_Pos)


#define PWMG_IE_NEWP0A_Pos          0
#define PWMG_IE_NEWP0A_Msk          (0x01 << PWMG_IE_NEWP0A_Pos)
#define PWMG_IE_NEWP0B_Pos          1
#define PWMG_IE_NEWP0B_Msk          (0x01 << PWMG_IE_NEWP0B_Pos)
#define PWMG_IE_NEWP1A_Pos          2
#define PWMG_IE_NEWP1A_Msk          (0x01 << PWMG_IE_NEWP1A_Pos)
#define PWMG_IE_NEWP1B_Pos          3
#define PWMG_IE_NEWP1B_Msk          (0x01 << PWMG_IE_NEWP1B_Pos)
#define PWMG_IE_NEWP2A_Pos          4
#define PWMG_IE_NEWP2A_Msk          (0x01 << PWMG_IE_NEWP2A_Pos)
#define PWMG_IE_NEWP2B_Pos          5
#define PWMG_IE_NEWP2B_Msk          (0x01 << PWMG_IE_NEWP2B_Pos)
#define PWMG_IE_NEWP3A_Pos          6
#define PWMG_IE_NEWP3A_Msk          (0x01 << PWMG_IE_NEWP3A_Pos)
#define PWMG_IE_NEWP3B_Pos          7
#define PWMG_IE_NEWP3B_Msk          (0x01 << PWMG_IE_NEWP3B_Pos)
#define PWMG_IE_NEWP4A_Pos          8
#define PWMG_IE_NEWP4A_Msk          (0x01 << PWMG_IE_NEWP4A_Pos)
#define PWMG_IE_NEWP4B_Pos          9
#define PWMG_IE_NEWP4B_Msk          (0x01 << PWMG_IE_NEWP4B_Pos)
#define PWMG_IE_NEWP5A_Pos          10
#define PWMG_IE_NEWP5A_Msk          (0x01 << PWMG_IE_NEWP5A_Pos)
#define PWMG_IE_NEWP5B_Pos          11
#define PWMG_IE_NEWP5B_Msk          (0x01 << PWMG_IE_NEWP5B_Pos)
#define PWMG_IE_HEND0A_Pos          12
#define PWMG_IE_HEND0A_Msk          (0x01 << PWMG_IE_HEND0A_Pos)
#define PWMG_IE_HEND0B_Pos          13
#define PWMG_IE_HEND0B_Msk          (0x01 << PWMG_IE_HEND0B_Pos)
#define PWMG_IE_HEND1A_Pos          14
#define PWMG_IE_HEND1A_Msk          (0x01 << PWMG_IE_HEND1A_Pos)
#define PWMG_IE_HEND1B_Pos          15
#define PWMG_IE_HEND1B_Msk          (0x01 << PWMG_IE_HEND1B_Pos)
#define PWMG_IE_HEND2A_Pos          16
#define PWMG_IE_HEND2A_Msk          (0x01 << PWMG_IE_HEND2A_Pos)
#define PWMG_IE_HEND2B_Pos          17
#define PWMG_IE_HEND2B_Msk          (0x01 << PWMG_IE_HEND2B_Pos)
#define PWMG_IE_HEND3A_Pos          18
#define PWMG_IE_HEND3A_Msk          (0x01 << PWMG_IE_HEND3A_Pos)
#define PWMG_IE_HEND3B_Pos          19
#define PWMG_IE_HEND3B_Msk          (0x01 << PWMG_IE_HEND3B_Pos)
#define PWMG_IE_HEND4A_Pos          20
#define PWMG_IE_HEND4A_Msk          (0x01 << PWMG_IE_HEND4A_Pos)
#define PWMG_IE_HEND4B_Pos          21
#define PWMG_IE_HEND4B_Msk          (0x01 << PWMG_IE_HEND4B_Pos)
#define PWMG_IE_HEND5A_Pos          22
#define PWMG_IE_HEND5A_Msk          (0x01 << PWMG_IE_HEND5A_Pos)
#define PWMG_IE_HEND5B_Pos          23
#define PWMG_IE_HEND5B_Msk          (0x01 << PWMG_IE_HEND5B_Pos)
#define PWMG_IE_HALT_Pos            24
#define PWMG_IE_HALT_Msk            (0x01 << PWMG_IE_HALT_Pos)

#define PWMG_IF_NEWP0A_Pos          0
#define PWMG_IF_NEWP0A_Msk          (0x01 << PWMG_IF_NEWP0A_Pos)
#define PWMG_IF_NEWP0B_Pos          1
#define PWMG_IF_NEWP0B_Msk          (0x01 << PWMG_IF_NEWP0B_Pos)
#define PWMG_IF_NEWP1A_Pos          2
#define PWMG_IF_NEWP1A_Msk          (0x01 << PWMG_IF_NEWP1A_Pos)
#define PWMG_IF_NEWP1B_Pos          3
#define PWMG_IF_NEWP1B_Msk          (0x01 << PWMG_IF_NEWP1B_Pos)
#define PWMG_IF_NEWP2A_Pos          4
#define PWMG_IF_NEWP2A_Msk          (0x01 << PWMG_IF_NEWP2A_Pos)
#define PWMG_IF_NEWP2B_Pos          5
#define PWMG_IF_NEWP2B_Msk          (0x01 << PWMG_IF_NEWP2B_Pos)
#define PWMG_IF_NEWP3A_Pos          6
#define PWMG_IF_NEWP3A_Msk          (0x01 << PWMG_IF_NEWP3A_Pos)
#define PWMG_IF_NEWP3B_Pos          7
#define PWMG_IF_NEWP3B_Msk          (0x01 << PWMG_IF_NEWP3B_Pos)
#define PWMG_IF_NEWP4A_Pos          8
#define PWMG_IF_NEWP4A_Msk          (0x01 << PWMG_IF_NEWP4A_Pos)
#define PWMG_IF_NEWP4B_Pos          9
#define PWMG_IF_NEWP4B_Msk          (0x01 << PWMG_IF_NEWP4B_Pos)
#define PWMG_IF_NEWP5A_Pos          10
#define PWMG_IF_NEWP5A_Msk          (0x01 << PWMG_IF_NEWP5A_Pos)
#define PWMG_IF_NEWP5B_Pos          11
#define PWMG_IF_NEWP5B_Msk          (0x01 << PWMG_IF_NEWP5B_Pos)
#define PWMG_IF_HEND0A_Pos          12
#define PWMG_IF_HEND0A_Msk          (0x01 << PWMG_IF_HEND0A_Pos)
#define PWMG_IF_HEND0B_Pos          13
#define PWMG_IF_HEND0B_Msk          (0x01 << PWMG_IF_HEND0B_Pos)
#define PWMG_IF_HEND1A_Pos          14
#define PWMG_IF_HEND1A_Msk          (0x01 << PWMG_IF_HEND1A_Pos)
#define PWMG_IF_HEND1B_Pos          15
#define PWMG_IF_HEND1B_Msk          (0x01 << PWMG_IF_HEND1B_Pos)
#define PWMG_IF_HEND2A_Pos          16
#define PWMG_IF_HEND2A_Msk          (0x01 << PWMG_IF_HEND2A_Pos)
#define PWMG_IF_HEND2B_Pos          17
#define PWMG_IF_HEND2B_Msk          (0x01 << PWMG_IF_HEND2B_Pos)
#define PWMG_IF_HEND3A_Pos          18
#define PWMG_IF_HEND3A_Msk          (0x01 << PWMG_IF_HEND3A_Pos)
#define PWMG_IF_HEND3B_Pos          19
#define PWMG_IF_HEND3B_Msk          (0x01 << PWMG_IF_HEND3B_Pos)
#define PWMG_IF_HEND4A_Pos          20
#define PWMG_IF_HEND4A_Msk          (0x01 << PWMG_IF_HEND4A_Pos)
#define PWMG_IF_HEND4B_Pos          21
#define PWMG_IF_HEND4B_Msk          (0x01 << PWMG_IF_HEND4B_Pos)
#define PWMG_IF_HEND5A_Pos          22
#define PWMG_IF_HEND5A_Msk          (0x01 << PWMG_IF_HEND5A_Pos)
#define PWMG_IF_HEND5B_Pos          23
#define PWMG_IF_HEND5B_Msk          (0x01 << PWMG_IF_HEND5B_Pos)
#define PWMG_IF_HALT_Pos            24
#define PWMG_IF_HALT_Msk            (0x01 << PWMG_IF_HALT_Pos)

#define PWMG_IM_NEWP0A_Pos          0
#define PWMG_IM_NEWP0A_Msk          (0x01 << PWMG_IM_NEWP0A_Pos)
#define PWMG_IM_NEWP0B_Pos          1
#define PWMG_IM_NEWP0B_Msk          (0x01 << PWMG_IM_NEWP0B_Pos)
#define PWMG_IM_NEWP1A_Pos          2
#define PWMG_IM_NEWP1A_Msk          (0x01 << PWMG_IM_NEWP1A_Pos)
#define PWMG_IM_NEWP1B_Pos          3
#define PWMG_IM_NEWP1B_Msk          (0x01 << PWMG_IM_NEWP1B_Pos)
#define PWMG_IM_NEWP2A_Pos          4
#define PWMG_IM_NEWP2A_Msk          (0x01 << PWMG_IM_NEWP2A_Pos)
#define PWMG_IM_NEWP2B_Pos          5
#define PWMG_IM_NEWP2B_Msk          (0x01 << PWMG_IM_NEWP2B_Pos)
#define PWMG_IM_NEWP3A_Pos          6
#define PWMG_IM_NEWP3A_Msk          (0x01 << PWMG_IM_NEWP3A_Pos)
#define PWMG_IM_NEWP3B_Pos          7
#define PWMG_IM_NEWP3B_Msk          (0x01 << PWMG_IM_NEWP3B_Pos)
#define PWMG_IM_NEWP4A_Pos          8
#define PWMG_IM_NEWP4A_Msk          (0x01 << PWMG_IM_NEWP4A_Pos)
#define PWMG_IM_NEWP4B_Pos          9
#define PWMG_IM_NEWP4B_Msk          (0x01 << PWMG_IM_NEWP4B_Pos)
#define PWMG_IM_NEWP5A_Pos          10
#define PWMG_IM_NEWP5A_Msk          (0x01 << PWMG_IM_NEWP5A_Pos)
#define PWMG_IM_NEWP5B_Pos          11
#define PWMG_IM_NEWP5B_Msk          (0x01 << PWMG_IM_NEWP5B_Pos)
#define PWMG_IM_HEND0A_Pos          12
#define PWMG_IM_HEND0A_Msk          (0x01 << PWMG_IM_HEND0A_Pos)
#define PWMG_IM_HEND0B_Pos          13
#define PWMG_IM_HEND0B_Msk          (0x01 << PWMG_IM_HEND0B_Pos)
#define PWMG_IM_HEND1A_Pos          14
#define PWMG_IM_HEND1A_Msk          (0x01 << PWMG_IM_HEND1A_Pos)
#define PWMG_IM_HEND1B_Pos          15
#define PWMG_IM_HEND1B_Msk          (0x01 << PWMG_IM_HEND1B_Pos)
#define PWMG_IM_HEND2A_Pos          16
#define PWMG_IM_HEND2A_Msk          (0x01 << PWMG_IM_HEND2A_Pos)
#define PWMG_IM_HEND2B_Pos          17
#define PWMG_IM_HEND2B_Msk          (0x01 << PWMG_IM_HEND2B_Pos)
#define PWMG_IM_HEND3A_Pos          18
#define PWMG_IM_HEND3A_Msk          (0x01 << PWMG_IM_HEND3A_Pos)
#define PWMG_IM_HEND3B_Pos          19
#define PWMG_IM_HEND3B_Msk          (0x01 << PWMG_IM_HEND3B_Pos)
#define PWMG_IM_HEND4A_Pos          20
#define PWMG_IM_HEND4A_Msk          (0x01 << PWMG_IM_HEND4A_Pos)
#define PWMG_IM_HEND4B_Pos          21
#define PWMG_IM_HEND4B_Msk          (0x01 << PWMG_IM_HEND4B_Pos)
#define PWMG_IM_HEND5A_Pos          22
#define PWMG_IM_HEND5A_Msk          (0x01 << PWMG_IM_HEND5A_Pos)
#define PWMG_IM_HEND5B_Pos          23
#define PWMG_IM_HEND5B_Msk          (0x01 << PWMG_IM_HEND5B_Pos)
#define PWMG_IM_HALT_Pos            24
#define PWMG_IM_HALT_Msk            (0x01 << PWMG_IM_HALT_Pos)

#define PWMG_IRS_NEWP0A_Pos         0
#define PWMG_IRS_NEWP0A_Msk         (0x01 << PWMG_IRS_NEWP0A_Pos)
#define PWMG_IRS_NEWP0B_Pos         1
#define PWMG_IRS_NEWP0B_Msk         (0x01 << PWMG_IRS_NEWP0B_Pos)
#define PWMG_IRS_NEWP1A_Pos         2
#define PWMG_IRS_NEWP1A_Msk         (0x01 << PWMG_IRS_NEWP1A_Pos)
#define PWMG_IRS_NEWP1B_Pos         3
#define PWMG_IRS_NEWP1B_Msk         (0x01 << PWMG_IRS_NEWP1B_Pos)
#define PWMG_IRS_NEWP2A_Pos         4
#define PWMG_IRS_NEWP2A_Msk         (0x01 << PWMG_IRS_NEWP2A_Pos)
#define PWMG_IRS_NEWP2B_Pos         5
#define PWMG_IRS_NEWP2B_Msk         (0x01 << PWMG_IRS_NEWP2B_Pos)
#define PWMG_IRS_NEWP3A_Pos         6
#define PWMG_IRS_NEWP3A_Msk         (0x01 << PWMG_IRS_NEWP3A_Pos)
#define PWMG_IRS_NEWP3B_Pos         7
#define PWMG_IRS_NEWP3B_Msk         (0x01 << PWMG_IRS_NEWP3B_Pos)
#define PWMG_IRS_NEWP4A_Pos         8
#define PWMG_IRS_NEWP4A_Msk         (0x01 << PWMG_IRS_NEWP4A_Pos)
#define PWMG_IRS_NEWP4B_Pos         9
#define PWMG_IRS_NEWP4B_Msk         (0x01 << PWMG_IRS_NEWP4B_Pos)
#define PWMG_IRS_NEWP5A_Pos         10
#define PWMG_IRS_NEWP5A_Msk         (0x01 << PWMG_IRS_NEWP5A_Pos)
#define PWMG_IRS_NEWP5B_Pos         11
#define PWMG_IRS_NEWP5B_Msk         (0x01 << PWMG_IRS_NEWP5B_Pos)
#define PWMG_IRS_HEND0A_Pos         12
#define PWMG_IRS_HEND0A_Msk         (0x01 << PWMG_IRS_HEND0A_Pos)
#define PWMG_IRS_HEND0B_Pos         13
#define PWMG_IRS_HEND0B_Msk         (0x01 << PWMG_IRS_HEND0B_Pos)
#define PWMG_IRS_HEND1A_Pos         14
#define PWMG_IRS_HEND1A_Msk         (0x01 << PWMG_IRS_HEND1A_Pos)
#define PWMG_IRS_HEND1B_Pos         15
#define PWMG_IRS_HEND1B_Msk         (0x01 << PWMG_IRS_HEND1B_Pos)
#define PWMG_IRS_HEND2A_Pos         16
#define PWMG_IRS_HEND2A_Msk         (0x01 << PWMG_IRS_HEND2A_Pos)
#define PWMG_IRS_HEND2B_Pos         17
#define PWMG_IRS_HEND2B_Msk         (0x01 << PWMG_IRS_HEND2B_Pos)
#define PWMG_IRS_HEND3A_Pos         18
#define PWMG_IRS_HEND3A_Msk         (0x01 << PWMG_IRS_HEND3A_Pos)
#define PWMG_IRS_HEND3B_Pos         19
#define PWMG_IRS_HEND3B_Msk         (0x01 << PWMG_IRS_HEND3B_Pos)
#define PWMG_IRS_HEND4A_Pos         20
#define PWMG_IRS_HEND4A_Msk         (0x01 << PWMG_IRS_HEND4A_Pos)
#define PWMG_IRS_HEND4B_Pos         21
#define PWMG_IRS_HEND4B_Msk         (0x01 << PWMG_IRS_HEND4B_Pos)
#define PWMG_IRS_HEND5A_Pos         22
#define PWMG_IRS_HEND5A_Msk         (0x01 << PWMG_IRS_HEND5A_Pos)
#define PWMG_IRS_HEND5B_Pos         23
#define PWMG_IRS_HEND5B_Msk         (0x01 << PWMG_IRS_HEND5B_Pos)
#define PWMG_IRS_HALT_Pos           24
#define PWMG_IRS_HALT_Msk           (0x01 << PWMG_IRS_HALT_Pos)




typedef struct {
    __IO uint32_t EN;

    __IO uint32_t IE;

    __IO uint32_t IM;

    __IO uint32_t IF;

    uint32_t RESERVED[12];

    struct {
        __IO uint32_t CR;

        __IO uint32_t AM;

        __IO uint32_t SRC;

        __IO uint32_t SRCSGADDR1;

        __IO uint32_t SRCSGADDR2;

        __IO uint32_t SRCSGADDR3;

        __IO uint32_t SRCSGLEN;

        __IO uint32_t DST;

        __IO uint32_t DSTSGADDR1;

        __IO uint32_t DSTSGADDR2;

        __IO uint32_t DSTSGADDR3;

        __IO uint32_t DSTSGLEN;

        uint32_t RESERVED[4];
    } CH[3];
} DMA_TypeDef;


#define DMA_IE_CH0_Pos              0
#define DMA_IE_CH0_Msk              (0x01 << DMA_IE_CH0_Pos)
#define DMA_IE_CH1_Pos              1
#define DMA_IE_CH1_Msk              (0x01 << DMA_IE_CH1_Pos)
#define DMA_IE_CH2_Pos              2
#define DMA_IE_CH2_Msk              (0x01 << DMA_IE_CH2_Pos)
#define DMA_IE_CH3_Pos              3
#define DMA_IE_CH3_Msk              (0x01 << DMA_IE_CH3_Pos)
#define DMA_IE_CH4_Pos              4
#define DMA_IE_CH4_Msk              (0x01 << DMA_IE_CH4_Pos)
#define DMA_IE_CH5_Pos              5
#define DMA_IE_CH5_Msk              (0x01 << DMA_IE_CH5_Pos)
#define DMA_IE_CH6_Pos              6
#define DMA_IE_CH6_Msk              (0x01 << DMA_IE_CH6_Pos)
#define DMA_IE_CH7_Pos              7
#define DMA_IE_CH7_Msk              (0x01 << DMA_IE_CH7_Pos)

#define DMA_IM_CH0_Pos              0
#define DMA_IM_CH0_Msk              (0x01 << DMA_IM_CH0_Pos)
#define DMA_IM_CH1_Pos              1
#define DMA_IM_CH1_Msk              (0x01 << DMA_IM_CH1_Pos)
#define DMA_IM_CH2_Pos              2
#define DMA_IM_CH2_Msk              (0x01 << DMA_IM_CH2_Pos)
#define DMA_IM_CH3_Pos              3
#define DMA_IM_CH3_Msk              (0x01 << DMA_IM_CH3_Pos)
#define DMA_IM_CH4_Pos              4
#define DMA_IM_CH4_Msk              (0x01 << DMA_IM_CH4_Pos)
#define DMA_IM_CH5_Pos              5
#define DMA_IM_CH5_Msk              (0x01 << DMA_IM_CH5_Pos)
#define DMA_IM_CH6_Pos              6
#define DMA_IM_CH6_Msk              (0x01 << DMA_IM_CH6_Pos)
#define DMA_IM_CH7_Pos              7
#define DMA_IM_CH7_Msk              (0x01 << DMA_IM_CH7_Pos)

#define DMA_IF_CH0_Pos              0
#define DMA_IF_CH0_Msk              (0x01 << DMA_IF_CH0_Pos)
#define DMA_IF_CH1_Pos              1
#define DMA_IF_CH1_Msk              (0x01 << DMA_IF_CH1_Pos)
#define DMA_IF_CH2_Pos              2
#define DMA_IF_CH2_Msk              (0x01 << DMA_IF_CH2_Pos)
#define DMA_IF_CH3_Pos              3
#define DMA_IF_CH3_Msk              (0x01 << DMA_IF_CH3_Pos)
#define DMA_IF_CH4_Pos              4
#define DMA_IF_CH4_Msk              (0x01 << DMA_IF_CH4_Pos)
#define DMA_IF_CH5_Pos              5
#define DMA_IF_CH5_Msk              (0x01 << DMA_IF_CH5_Pos)
#define DMA_IF_CH6_Pos              6
#define DMA_IF_CH6_Msk              (0x01 << DMA_IF_CH6_Pos)
#define DMA_IF_CH7_Pos              7
#define DMA_IF_CH7_Msk              (0x01 << DMA_IF_CH7_Pos)

#define DMA_CR_LEN_Pos              0
#define DMA_CR_LEN_Msk              (0xFFF << DMA_CR_LEN_Pos)
#define DMA_CR_RXEN_Pos             16
#define DMA_CR_RXEN_Msk             (0x01 << DMA_CR_RXEN_Pos)
#define DMA_CR_TXEN_Pos             17
#define DMA_CR_TXEN_Msk             (0x01 << DMA_CR_TXEN_Pos)
#define DMA_CR_AUTORE_Pos           18
#define DMA_CR_AUTORE_Msk           (0x01 << DMA_CR_AUTORE_Pos)

#define DMA_AM_SRCAM_Pos            0
#define DMA_AM_SRCAM_Msk            (0x03 << DMA_AM_SRCAM_Pos)
#define DMA_AM_DSTAM_Pos            8
#define DMA_AM_DSTAM_Msk            (0x03 << DMA_AM_DSTAM_Pos)
#define DMA_AM_BURST_Pos            16
#define DMA_AM_BURST_Msk            (0x01 << DMA_AM_BURST_Pos)




typedef struct {
    __IO uint32_t CR;                       //Control Register

    __O  uint32_t CMD;                      //Command Register

    __I  uint32_t SR;                       //Status Register

    __I  uint32_t IF;                       //Interrupt Flag

    __IO uint32_t IE;                       //Interrupt Enable

    uint32_t RESERVED;

    __IO uint32_t BT0;                      //Bit Time Register 0

    __IO uint32_t BT1;                      //Bit Time Register 1

    uint32_t RESERVED2[3];

    __I  uint32_t ALC;                      //Arbitration Lost Capture

    __I  uint32_t ECC;                      //Error code capture

    __IO uint32_t EWLIM;                    //Error Warning Limit

    __IO uint32_t RXERR;                    //RX

    __IO uint32_t TXERR;                    //TX

    union {
        struct {
            __IO uint32_t ACR[4];           //Acceptance Check Register,

            __IO uint32_t AMR[4];           //Acceptance Mask Register,

                 uint32_t RESERVED[5];
        } FILTER;

        union {
            struct {
                __O  uint32_t INFO;

                __O  uint32_t DATA[12];
            } TXFRAME;

            struct {
                __I  uint32_t INFO;

                __I  uint32_t DATA[12];
            } RXFRAME;
        };
    };

    __I  uint32_t RMCNT;                    //Receive Message Count

    uint32_t RESERVED3[66];

    struct {
        __I  uint32_t INFO;

        __I  uint32_t DATA[12];
    } TXFRAME_R;
} CAN_TypeDef;


#define CAN_CR_RST_Pos              0
#define CAN_CR_RST_Msk              (0x01 << CAN_CR_RST_Pos)
#define CAN_CR_LOM_Pos              1       //Listen Only Mode
#define CAN_CR_LOM_Msk              (0x01 << CAN_CR_LOM_Pos)
#define CAN_CR_STM_Pos              2       //Self Test Mode,
#define CAN_CR_STM_Msk              (0x01 << CAN_CR_STM_Pos)
#define CAN_CR_AFM_Pos              3       //Acceptance Filter Mode,
#define CAN_CR_AFM_Msk              (0x01 << CAN_CR_AFM_Pos)
#define CAN_CR_SLEEP_Pos            4
#define CAN_CR_SLEEP_Msk            (0x01 << CAN_CR_SLEEP_Pos)
#define CAN_CR_DMAEN_Pos            5
#define CAN_CR_DMAEN_Msk            (0x01 << CAN_CR_DMAEN_Pos)

#define CAN_CMD_TXREQ_Pos           0       //Transmission Request
#define CAN_CMD_TXREQ_Msk           (0x01 << CAN_CMD_TXREQ_Pos)
#define CAN_CMD_ABTTX_Pos           1       //Abort Transmission
#define CAN_CMD_ABTTX_Msk           (0x01 << CAN_CMD_ABTTX_Pos)
#define CAN_CMD_RRB_Pos             2       //Release Receive Buffer
#define CAN_CMD_RRB_Msk             (0x01 << CAN_CMD_RRB_Pos)
#define CAN_CMD_CLROV_Pos           3       //Clear Data Overrun
#define CAN_CMD_CLROV_Msk           (0x01 << CAN_CMD_CLROV_Pos)
#define CAN_CMD_SRR_Pos             4       //Self Reception Request
#define CAN_CMD_SRR_Msk             (0x01 << CAN_CMD_SRR_Pos)

#define CAN_SR_RXDA_Pos             0       //Receive Data Available
#define CAN_SR_RXDA_Msk             (0x01 << CAN_SR_RXDA_Pos)
#define CAN_SR_RXOV_Pos             1       //Receive FIFO Overrun
#define CAN_SR_RXOV_Msk             (0x01 << CAN_SR_RXOV_Pos)
#define CAN_SR_TXBR_Pos             2       //Transmit Buffer Release
#define CAN_SR_TXBR_Msk             (0x01 << CAN_SR_TXBR_Pos)
#define CAN_SR_TXOK_Pos             3       //Transmit OK，successfully completed
#define CAN_SR_TXOK_Msk             (0x01 << CAN_SR_TXOK_Pos)
#define CAN_SR_RXBUSY_Pos           4       //Receive Busy
#define CAN_SR_RXBUSY_Msk           (0x01 << CAN_SR_RXBUSY_Pos)
#define CAN_SR_TXBUSY_Pos           5       //Transmit Busy
#define CAN_SR_TXBUSY_Msk           (0x01 << CAN_SR_TXBUSY_Pos)
#define CAN_SR_ERRWARN_Pos          6
#define CAN_SR_ERRWARN_Msk          (0x01 << CAN_SR_ERRWARN_Pos)
#define CAN_SR_BUSOFF_Pos           7
#define CAN_SR_BUSOFF_Msk           (0x01 << CAN_SR_BUSOFF_Pos)

#define CAN_IF_RXDA_Pos             0       //IF.RXDA = SR.RXDA & IE.RXDA
#define CAN_IF_RXDA_Msk             (0x01 << CAN_IF_RXDA_Pos)
#define CAN_IF_TXBR_Pos             1
#define CAN_IF_TXBR_Msk             (0x01 << CAN_IF_TXBR_Pos)
#define CAN_IF_ERRWARN_Pos          2
#define CAN_IF_ERRWARN_Msk          (0x01 << CAN_IF_ERRWARN_Pos)
#define CAN_IF_RXOV_Pos             3       //IF.RXOV = SR.RXOV & IE.RXOV
#define CAN_IF_RXOV_Msk             (0x01 << CAN_IF_RXOV_Pos)
#define CAN_IF_WKUP_Pos             4
#define CAN_IF_WKUP_Msk             (0x01 << CAN_IF_WKUP_Pos)
#define CAN_IF_ERRPASS_Pos          5
#define CAN_IF_ERRPASS_Msk          (0x01 << CAN_IF_ERRPASS_Pos)
#define CAN_IF_ARBLOST_Pos          6
#define CAN_IF_ARBLOST_Msk          (0x01 << CAN_IF_ARBLOST_Pos)
#define CAN_IF_BUSERR_Pos           7
#define CAN_IF_BUSERR_Msk           (0x01 << CAN_IF_BUSERR_Pos)

#define CAN_IE_RXDA_Pos             0
#define CAN_IE_RXDA_Msk             (0x01 << CAN_IE_RXDA_Pos)
#define CAN_IE_TXBR_Pos             1
#define CAN_IE_TXBR_Msk             (0x01 << CAN_IE_TXBR_Pos)
#define CAN_IE_ERRWARN_Pos          2
#define CAN_IE_ERRWARN_Msk          (0x01 << CAN_IE_ERRWARN_Pos)
#define CAN_IE_RXOV_Pos             3
#define CAN_IE_RXOV_Msk             (0x01 << CAN_IE_RXOV_Pos)
#define CAN_IE_WKUP_Pos             4
#define CAN_IE_WKUP_Msk             (0x01 << CAN_IE_WKUP_Pos)
#define CAN_IE_ERRPASS_Pos          5
#define CAN_IE_ERRPASS_Msk          (0x01 << CAN_IE_ERRPASS_Pos)
#define CAN_IE_ARBLOST_Pos          6
#define CAN_IE_ARBLOST_Msk          (0x01 << CAN_IE_ARBLOST_Pos)
#define CAN_IE_BUSERR_Pos           7
#define CAN_IE_BUSERR_Msk           (0x01 << CAN_IE_BUSERR_Pos)

#define CAN_BT0_BRP_Pos             0       //Baud Rate Prescaler
#define CAN_BT0_BRP_Msk             (0x3F << CAN_BT0_BRP_Pos)
#define CAN_BT0_SJW_Pos             6       //Synchronization Jump Width
#define CAN_BT0_SJW_Msk             (0x03 << CAN_BT0_SJW_Pos)

#define CAN_BT1_TSEG1_Pos           0
#define CAN_BT1_TSEG1_Msk           (0x0F << CAN_BT1_TSEG1_Pos)
#define CAN_BT1_TSEG2_Pos           4
#define CAN_BT1_TSEG2_Msk           (0x07 << CAN_BT1_TSEG2_Pos)
#define CAN_BT1_SAM_Pos             7
#define CAN_BT1_SAM_Msk             (0x01 << CAN_BT1_SAM_Pos)

#define CAN_ECC_SEGCODE_Pos         0       //Segment Code
#define CAN_ECC_SEGCODE_Msk         (0x0F << CAN_ECC_SEGCODE_Pos)
#define CAN_ECC_DIR_Pos             4       //0 error occurred during transmission   1 during reception
#define CAN_ECC_DIR_Msk             (0x01 << CAN_ECC_DIR_Pos)
#define CAN_ECC_ERRCODE_Pos         5       //Error Code：0 Bit error   1 Form error   2 Stuff error   3 other error
#define CAN_ECC_ERRCODE_Msk         (0x03 << CAN_ECC_ERRCODE_Pos)

#define CAN_INFO_DLC_Pos            0       //Data Length Control
#define CAN_INFO_DLC_Msk            (0x0F << CAN_INFO_DLC_Pos)
#define CAN_INFO_RTR_Pos            6
#define CAN_INFO_RTR_Msk            (0x01 << CAN_INFO_RTR_Pos)
#define CAN_INFO_FF_Pos             7
#define CAN_INFO_FF_Msk             (0x01 << CAN_INFO_FF_Pos)




typedef struct {
    __IO uint32_t IE;

    __IO uint32_t IF;

    __IO uint32_t IM;

    __IO uint32_t START;

    __IO uint32_t SRCADDR;

    __IO uint32_t CR0;

    __IO uint32_t CR1;

    __IO uint32_t PRECMDV;
} LCD_TypeDef;


#define LCD_START_MPUEN_Pos         0
#define LCD_START_MPUEN_Msk         (0x01 << LCD_START_MPUEN_Pos)
#define LCD_START_GO_Pos            1
#define LCD_START_GO_Msk            (0x01 << LCD_START_GO_Pos)
#define LCD_START_BURST_Pos         2
#define LCD_START_BURST_Msk         (0x01 << LCD_START_BURST_Pos)
#define LCD_START_POSTCMDE_Pos      3
#define LCD_START_POSTCMDE_Msk      (0x01 << LCD_START_POSTCMDE_Pos)
#define LCD_START_POSTCMDV_Pos      4
#define LCD_START_POSTCMDV_Msk      (0xFFFF << LCD_START_POSTCMDV_Pos)

#define LCD_CR0_VPIX_Pos            0
#define LCD_CR0_VPIX_Msk            (0x3FF << LCD_CR0_VPIX_Pos)
#define LCD_CR0_HPIX_Pos            10
#define LCD_CR0_HPIX_Msk            (0x3FF << LCD_CR0_HPIX_Pos)
#define LCD_CR0_DCLK_Pos            20
#define LCD_CR0_DCLK_Msk            (0x01 << LCD_CR0_DCLK_Pos)
#define LCD_CR0_HLOW_Pos            21
#define LCD_CR0_HLOW_Msk            (0x03 << LCD_CR0_HLOW_Pos)

#define LCD_CR0_DLEN_Pos            0
#define LCD_CR0_DLEN_Msk            (0x1FFFFF << LCD_CR0_DLEN_Pos)

#define LCD_CR1_DIRV_Pos            0
#define LCD_CR1_DIRV_Msk            (0x01 << LCD_CR1_DIRV_Pos)
#define LCD_CR1_VFP_Pos             1
#define LCD_CR1_VFP_Msk             (0x07 << LCD_CR1_VFP_Pos)
#define LCD_CR1_VBP_Pos             4
#define LCD_CR1_VBP_Msk             (0x1F << LCD_CR1_VBP_Pos)
#define LCD_CR1_HFP_Pos             9
#define LCD_CR1_HFP_Msk             (0x1F << LCD_CR1_HFP_Pos)
#define LCD_CR1_HBP_Pos             14
#define LCD_CR1_HBP_Msk             (0x7F << LCD_CR1_HBP_Pos)
#define LCD_CR1_DCLKDIV_Pos         21
#define LCD_CR1_DCLKDIV_Msk         (0x1F << LCD_CR1_DCLKDIV_Pos)
#define LCD_CR1_DCLKINV_Pos         26
#define LCD_CR1_DCLKINV_Msk         (0x01 << LCD_CR1_DCLKINV_Pos)

#define LCD_CR1_REG_Pos             0
#define LCD_CR1_REG_Msk             (0xFFFF << LCD_CR1_REG_Pos)
#define LCD_CR1_I80_Pos             16
#define LCD_CR1_I80_Msk             (0x01 << LCD_CR1_I80_Pos)
#define LCD_CR1_CMD_Pos             17
#define LCD_CR1_CMD_Msk             (0x01 << LCD_CR1_CMD_Pos)
#define LCD_CR1_TTAIL_Pos           18
#define LCD_CR1_TTAIL_Msk           (0x07 << LCD_CR1_TTAIL_Pos)
#define LCD_CR1_TAH_Pos             21
#define LCD_CR1_TAH_Msk             (0x03 << LCD_CR1_TAH_Pos)
#define LCD_CR1_TPWLW_Pos           23
#define LCD_CR1_TPWLW_Msk           (0x07 << LCD_CR1_TPWLW_Pos)
#define LCD_CR1_TAS_Pos             26
#define LCD_CR1_TAS_Msk             (0x03 << LCD_CR1_TAS_Pos)




typedef struct {
        __IO uint32_t DMA_MEM_ADDR;

        __IO uint32_t BLK;                      //Block Size and Count

        __IO uint32_t ARG;                      //Argument

        __IO uint32_t CMD;                      //Command

        __IO uint32_t RESP[4];                  //Response

        __IO uint32_t DATA;

        __IO uint32_t STAT;

        __IO uint32_t CR1;

        __IO uint32_t CR2;

        __IO uint32_t IF;

        __IO uint32_t IE;

        __IO uint32_t IM;

        __IO uint32_t CMD12ERR;

        __IO uint32_t INFO;

        __IO uint32_t MAXCURR;
} SDIO_TypeDef;


#define SDIO_BLK_SIZE_Pos           0
#define SDIO_BLK_SIZE_Msk           (0xFFF << SDIO_BLK_SIZE_Pos)
#define SDIO_BLK_COUNT_Pos          16
#define SDIO_BLK_COUNT_Msk          (0xFFF << SDIO_BLK_COUNT_Pos)

#define SDIO_CMD_DMAEN_Pos          0
#define SDIO_CMD_DMAEN_Msk          (0x01 << SDIO_CMD_DMAEN_Pos)
#define SDIO_CMD_BLKCNTEN_Pos       1
#define SDIO_CMD_BLKCNTEN_Msk       (0x01 << SDIO_CMD_BLKCNTEN_Pos)
#define SDIO_CMD_AUTOCMD12_Pos      2
#define SDIO_CMD_AUTOCMD12_Msk      (0x01 << SDIO_CMD_AUTOCMD12_Pos)
#define SDIO_CMD_DIRREAD_Pos        4
#define SDIO_CMD_DIRREAD_Msk        (0x01 << SDIO_CMD_DIRREAD_Pos)
#define SDIO_CMD_MULTBLK_Pos        5
#define SDIO_CMD_MULTBLK_Msk        (0x01 << SDIO_CMD_MULTBLK_Pos)
#define SDIO_CMD_RESPTYPE_Pos       16
#define SDIO_CMD_RESPTYPE_Msk       (0x03 << SDIO_CMD_RESPTYPE_Pos)
#define SDIO_CMD_CRCCHECK_Pos       19
#define SDIO_CMD_CRCCHECK_Msk       (0x01 << SDIO_CMD_CRCCHECK_Pos)
#define SDIO_CMD_IDXCHECK_Pos       20       //Command Index Check Enable
#define SDIO_CMD_IDXCHECK_Msk       (0x01 << SDIO_CMD_IDXCHECK_Pos)
#define SDIO_CMD_HAVEDATA_Pos       21       //0 No Data Present    1 Data Present
#define SDIO_CMD_HAVEDATA_Msk       (0x01 << SDIO_CMD_HAVEDATA_Pos)
#define SDIO_CMD_CMDTYPE_Pos        22       //0 NORMAL   1 SUSPEND    2 RESUME    3 ABORT
#define SDIO_CMD_CMDTYPE_Msk        (0x03 << SDIO_CMD_CMDTYPE_Pos)
#define SDIO_CMD_CMDINDX_Pos        24
#define SDIO_CMD_CMDINDX_Msk        (0x3F << SDIO_CMD_CMDINDX_Pos)

#define SDIO_CR1_4BIT_Pos           1       //1 4 bit mode    0 1 bit mode
#define SDIO_CR1_4BIT_Msk           (0x01 << SDIO_CR1_4BIT_Pos)
#define SDIO_CR1_8BIT_Pos           5       //1 8 bit mode is selected    0 8 bit mode is not selected
#define SDIO_CR1_8BIT_Msk           (0x01 << SDIO_CR1_8BIT_Pos)
#define SDIO_CR1_CDBIT_Pos          6       //0 No Card    1 Card Inserted
#define SDIO_CR1_CDBIT_Msk          (0x01 << SDIO_CR1_CDBIT_Pos)
#define SDIO_CR1_CDSRC_Pos          7       //Card Detect Source, 1 CR1.CDBIT位    0 SD_Detect引脚
#define SDIO_CR1_CDSRC_Msk          (0x01 << SDIO_CR1_CDSRC_Pos)
#define SDIO_CR1_PWRON_Pos          8       //1 Power on    0 Power off
#define SDIO_CR1_PWRON_Msk          (0x01 << SDIO_CR1_PWRON_Pos)
#define SDIO_CR1_VOLT_Pos           9       //7 3.3V    6 3.0V    5 1.8V
#define SDIO_CR1_VOLT_Msk           (0x07 << SDIO_CR1_VOLT_Pos)

#define SDIO_CR2_CLKEN_Pos          0       //Internal Clock Enable
#define SDIO_CR2_CLKEN_Msk          (0x01 << SDIO_CR2_CLKEN_Pos)
#define SDIO_CR2_CLKRDY_Pos         1       //Internal Clock Stable/Ready
#define SDIO_CR2_CLKRDY_Msk         (0x01 << SDIO_CR2_CLKRDY_Pos)
#define SDIO_CR2_SDCLKEN_Pos        2       //SDCLK Enable
#define SDIO_CR2_SDCLKEN_Msk        (0x01 << SDIO_CR2_SDCLKEN_Pos)
#define SDIO_CR2_SDCLKDIV_Pos       8       //SDCLK Frequency Div, 0x00 不分频    0x01 2分频    0x02 4分频    0x04 8分频    0x08    16分频    ...    0x80 256分频
#define SDIO_CR2_SDCLKDIV_Msk       (0xFF << SDIO_CR2_SDCLKDIV_Pos)
#define SDIO_CR2_TIMEOUT_Pos        16      //0000 TMCLK*2^13
#define SDIO_CR2_TIMEOUT_Msk        (0x0F << SDIO_CR2_TIMEOUT_Pos)
#define SDIO_CR2_RSTALL_Pos         24      //Software Reset for All
#define SDIO_CR2_RSTALL_Msk         (0x01 << SDIO_CR2_RSTALL_Pos)
#define SDIO_CR2_RSTCMD_Pos         25      //Software Reset for CMD Line
#define SDIO_CR2_RSTCMD_Msk         (0x01 << SDIO_CR2_RSTCMD_Pos)
#define SDIO_CR2_RSTDAT_Pos         26      //Software Reset for DAT Line
#define SDIO_CR2_RSTDAT_Msk         (0x01 << SDIO_CR2_RSTDAT_Pos)

#define SDIO_IF_CMDDONE_Pos         0
#define SDIO_IF_CMDDONE_Msk         (0x01 << SDIO_IF_CMDDONE_Pos)
#define SDIO_IF_TRXDONE_Pos         1
#define SDIO_IF_TRXDONE_Msk         (0x01 << SDIO_IF_TRXDONE_Pos)
#define SDIO_IF_BLKGAP_Pos          2
#define SDIO_IF_BLKGAP_Msk          (0x01 << SDIO_IF_BLKGAP_Pos)
#define SDIO_IF_DMADONE_Pos         3
#define SDIO_IF_DMADONE_Msk         (0x01 << SDIO_IF_DMADONE_Pos)
#define SDIO_IF_BUFWRRDY_Pos        4
#define SDIO_IF_BUFWRRDY_Msk        (0x01 << SDIO_IF_BUFWRRDY_Pos)
#define SDIO_IF_BUFRDRDY_Pos        5
#define SDIO_IF_BUFRDRDY_Msk        (0x01 << SDIO_IF_BUFRDRDY_Pos)
#define SDIO_IF_CARDINSR_Pos        6
#define SDIO_IF_CARDINSR_Msk        (0x01 << SDIO_IF_CARDINSR_Pos)
#define SDIO_IF_CARDRMOV_Pos        7
#define SDIO_IF_CARDRMOV_Msk        (0x01 << SDIO_IF_CARDRMOV_Pos)
#define SDIO_IF_CARD_Pos            8
#define SDIO_IF_CARD_Msk            (0x01 << SDIO_IF_CARD_Pos)
#define SDIO_IF_ERROR_Pos           15
#define SDIO_IF_ERROR_Msk           (0x01 << SDIO_IF_ERROR_Pos)
#define SDIO_IF_CMDTIMEOUT_Pos      16
#define SDIO_IF_CMDTIMEOUT_Msk      (0x01 << SDIO_IF_CMDTIMEOUT_Pos)
#define SDIO_IF_CMDCRCERR_Pos       17
#define SDIO_IF_CMDCRCERR_Msk       (0x01 << SDIO_IF_CMDCRCERR_Pos)
#define SDIO_IF_CMDENDERR_Pos       18
#define SDIO_IF_CMDENDERR_Msk       (0x01 << SDIO_IF_CMDENDCERR_Pos)
#define SDIO_IF_CMDIDXERR_Pos       19
#define SDIO_IF_CMDIDXERR_Msk       (0x01 << SDIO_IF_CMDIDXCERR_Pos)
#define SDIO_IF_DATTIMEOUT_Pos      20
#define SDIO_IF_DATTIMEOUT_Msk      (0x01 << SDIO_IF_DATTIMEOUT_Pos)
#define SDIO_IF_DATCRCERR_Pos       21
#define SDIO_IF_DATCRCERR_Msk       (0x01 << SDIO_IF_DATCRCERR_Pos)
#define SDIO_IF_DATENDERR_Pos       22
#define SDIO_IF_DATENDERR_Msk       (0x01 << SDIO_IF_DATENDCERR_Pos)
#define SDIO_IF_CURLIMERR_Pos       23
#define SDIO_IF_CURLIMERR_Msk       (0x01 << SDIO_IF_CURLIMERR_Pos)
#define SDIO_IF_CMD12ERR_Pos        24
#define SDIO_IF_CMD12ERR_Msk        (0x01 << SDIO_IF_CMD12ERR_Pos)
#define SDIO_IF_DMAERR_Pos          25
#define SDIO_IF_DMAERR_Msk          (0x01 << SDIO_IF_DMAERR_Pos)
#define SDIO_IF_RESPERR_Pos         28
#define SDIO_IF_RESPERR_Msk         (0x01 << SDIO_IF_RESPERR_Pos)

#define SDIO_IE_CMDDONE_Pos         0       //Command Complete Status Enable
#define SDIO_IE_CMDDONE_Msk         (0x01 << SDIO_IE_CMDDONE_Pos)
#define SDIO_IE_TRXDONE_Pos         1       //Transfer Complete Status Enable
#define SDIO_IE_TRXDONE_Msk         (0x01 << SDIO_IE_TRXDONE_Pos)
#define SDIO_IE_BLKGAP_Pos          2       //Block Gap Event Status Enable
#define SDIO_IE_BLKGAP_Msk          (0x01 << SDIO_IE_BLKGAP_Pos)
#define SDIO_IE_DMADONE_Pos         3       //DMA Interrupt Status Enable
#define SDIO_IE_DMADONE_Msk         (0x01 << SDIO_IE_DMADONE_Pos)
#define SDIO_IE_BUFWRRDY_Pos        4       //Buffer Write Ready Status Enable
#define SDIO_IE_BUFWRRDY_Msk        (0x01 << SDIO_IE_BUFWRRDY_Pos)
#define SDIO_IE_BUFRDRDY_Pos        5       //Buffer Read Ready Status Enable
#define SDIO_IE_BUFRDRDY_Msk        (0x01 << SDIO_IE_BUFRDRDY_Pos)
#define SDIO_IE_CARDINSR_Pos        6       //Card Insertion Status Enable
#define SDIO_IE_CARDINSR_Msk        (0x01 << SDIO_IE_CARDINSR_Pos)
#define SDIO_IE_CARDRMOV_Pos        7       //Card Removal Status Enable
#define SDIO_IE_CARDRMOV_Msk        (0x01 << SDIO_IE_CARDRMOV_Pos)
#define SDIO_IE_CARD_Pos            8
#define SDIO_IE_CARD_Msk            (0x01 << SDIO_IE_CARD_Pos)
#define SDIO_IE_CMDTIMEOUT_Pos      16      //Command Timeout Error Status Enable
#define SDIO_IE_CMDTIMEOUT_Msk      (0x01 << SDIO_IE_CMDTIMEOUT_Pos)
#define SDIO_IE_CMDCRCERR_Pos       17      //Command CRC Error Status Enable
#define SDIO_IE_CMDCRCERR_Msk       (0x01 << SDIO_IE_CMDCRCERR_Pos)
#define SDIO_IE_CMDENDERR_Pos       18      //Command End Bit Error Status Enable
#define SDIO_IE_CMDENDERR_Msk       (0x01 << SDIO_IE_CMDENDCERR_Pos)
#define SDIO_IE_CMDIDXERR_Pos       19      //Command Index Error Status Enable
#define SDIO_IE_CMDIDXERR_Msk       (0x01 << SDIO_IE_CMDIDXCERR_Pos)
#define SDIO_IE_DATTIMEOUT_Pos      20      //Data Timeout Error Status Enable
#define SDIO_IE_DATTIMEOUT_Msk      (0x01 << SDIO_IE_DATTIMEOUT_Pos)
#define SDIO_IE_DATCRCERR_Pos       21      //Data CRC Error Status Enable
#define SDIO_IE_DATCRCERR_Msk       (0x01 << SDIO_IE_DATCRCERR_Pos)
#define SDIO_IE_DATENDERR_Pos       22      //Data End Bit Error Status Enable
#define SDIO_IE_DATENDERR_Msk       (0x01 << SDIO_IE_DATENDCERR_Pos)
#define SDIO_IE_CURLIMERR_Pos       23      //Current Limit Error Status Enable
#define SDIO_IE_CURLIMERR_Msk       (0x01 << SDIO_IE_CURLIMERR_Pos)
#define SDIO_IE_CMD12ERR_Pos        24      //Auto CMD12 Error Status Enable
#define SDIO_IE_CMD12ERR_Msk        (0x01 << SDIO_IE_CMD12ERR_Pos)
#define SDIO_IE_DMAERR_Pos          25      //ADMA Error Status Enable
#define SDIO_IE_DMAERR_Msk          (0x01 << SDIO_IE_DMAERR_Pos)
#define SDIO_IE_RESPERR_Pos         28      //Target Response Error Status Enable
#define SDIO_IE_RESPERR_Msk         (0x01 << SDIO_IE_RESPERR_Pos)

#define SDIO_IM_CMDDONE_Pos         0
#define SDIO_IM_CMDDONE_Msk         (0x01 << SDIO_IM_CMDDONE_Pos)
#define SDIO_IM_TRXDONE_Pos         1
#define SDIO_IM_TRXDONE_Msk         (0x01 << SDIO_IM_TRXDONE_Pos)
#define SDIO_IM_BLKGAP_Pos          2
#define SDIO_IM_BLKGAP_Msk          (0x01 << SDIO_IM_BLKGAP_Pos)
#define SDIO_IM_DMADONE_Pos         3
#define SDIO_IM_DMADONE_Msk         (0x01 << SDIO_IM_DMADONE_Pos)
#define SDIO_IM_BUFWRRDY_Pos        4
#define SDIO_IM_BUFWRRDY_Msk        (0x01 << SDIO_IM_BUFWRRDY_Pos)
#define SDIO_IM_BUFRDRDY_Pos        5
#define SDIO_IM_BUFRDRDY_Msk        (0x01 << SDIO_IM_BUFRDRDY_Pos)
#define SDIO_IM_CARDINSR_Pos        6
#define SDIO_IM_CARDINSR_Msk        (0x01 << SDIO_IM_CARDINSR_Pos)
#define SDIO_IM_CARDRMOV_Pos        7
#define SDIO_IM_CARDRMOV_Msk        (0x01 << SDIO_IM_CARDRMOV_Pos)
#define SDIO_IM_CARD_Pos            8
#define SDIO_IM_CARD_Msk            (0x01 << SDIO_IM_CARD_Pos)
#define SDIO_IM_CMDTIMEOUT_Pos      16
#define SDIO_IM_CMDTIMEOUT_Msk      (0x01 << SDIO_IM_CMDTIMEOUT_Pos)
#define SDIO_IM_CMDCRCERR_Pos       17
#define SDIO_IM_CMDCRCERR_Msk       (0x01 << SDIO_IM_CMDCRCERR_Pos)
#define SDIO_IM_CMDENDERR_Pos       18
#define SDIO_IM_CMDENDERR_Msk       (0x01 << SDIO_IM_CMDENDCERR_Pos)
#define SDIO_IM_CMDIDXERR_Pos       19
#define SDIO_IM_CMDIDXERR_Msk       (0x01 << SDIO_IM_CMDIDXCERR_Pos)
#define SDIO_IM_DATTIMEOUT_Pos      20
#define SDIO_IM_DATTIMEOUT_Msk      (0x01 << SDIO_IM_DATTIMEOUT_Pos)
#define SDIO_IM_DATCRCERR_Pos       21
#define SDIO_IM_DATCRCERR_Msk       (0x01 << SDIO_IM_DATCRCERR_Pos)
#define SDIO_IM_DATENDERR_Pos       22
#define SDIO_IM_DATENDERR_Msk       (0x01 << SDIO_IM_DATENDCERR_Pos)
#define SDIO_IM_CURLIMERR_Pos       23
#define SDIO_IM_CURLIMERR_Msk       (0x01 << SDIO_IM_CURLIMERR_Pos)
#define SDIO_IM_CMD12ERR_Pos        24
#define SDIO_IM_CMD12ERR_Msk        (0x01 << SDIO_IM_CMD12ERR_Pos)
#define SDIO_IM_DMAERR_Pos          25
#define SDIO_IM_DMAERR_Msk          (0x01 << SDIO_IM_DMAERR_Pos)
#define SDIO_IM_RESPERR_Pos         28
#define SDIO_IM_RESPERR_Msk         (0x01 << SDIO_IM_RESPERR_Pos)




typedef struct {
    __IO uint32_t DATA;
    __IO uint32_t ADDR;
    __IO uint32_t ERASE;
    __IO uint32_t CACHE;
    __IO uint32_t CFG0;
    __IO uint32_t CFG1;
    __IO uint32_t CFG2;
    __IO uint32_t CFG3;
    __IO uint32_t STAT;
} FLASH_Typedef;


#define FLASH_ERASE_REQ_Pos         31
#define FLASH_ERASE_REQ_Msk         ((uint32_t)0x01 << FLASH_ERASE_REQ_Pos)

#define FLASH_CACHE_PROG_Pos        2
#define FLASH_CACHE_PROG_Msk        (0x01 << FLASH_CACHE_PROG_Pos)
#define FLASH_CACHE_CLEAR_Pos       3
#define FLASH_CACHE_CLEAR_Msk       (0x01 << FLASH_CACHE_CLEAR_Pos)

#define FLASH_STAT_ERASE_GOING_Pos  0
#define FLASH_STAT_ERASE_GOING_Msk  (0X01 << FLASH_STAT_ERASE_GOING_Pos)
#define FLASH_STAT_PROG_GOING_Pos   1
#define FLASH_STAT_PROG_GOING_Msk   (0x01 << FLASH_STAT_PROG_GOING_Pos)
#define FALSH_STAT_FIFO_EMPTY_Pos   3
#define FLASH_STAT_FIFO_EMPTY_Msk   (0x01 << FALSH_STAT_FIFO_EMPTY_Pos)
#define FALSH_STAT_FIFO_FULL_Pos    4
#define FLASH_STAT_FIFO_FULL_Msk    (0x01 << FALSH_STAT_FIFO_FULL_Pos)




typedef struct {
        __IO uint32_t CR;
} SRAMC_TypeDef;


#define SRAMC_CR_RWTIME_Pos         0
#define SRAMC_CR_RWTIME_Msk         (0x0F << SRAMC_CR_RWTIME_Pos)
#define SRAMC_CR_BYTEIF_Pos         4
#define SRAMC_CR_BYTEIF_Msk         (0x01 << SRAMC_CR_BYTEIF_Pos)
#define SRAMC_CR_HBLBDIS_Pos        5
#define SRAMC_CR_HBLBDIS_Msk        (0x01 << SRAMC_CR_HBLBDIS_Pos)



typedef struct {
        __IO uint32_t CR0;

        __IO uint32_t CR1;

        __IO uint32_t REFRESH;

        __IO uint32_t NOPNUM;

        __IO uint32_t LATCH;

        __IO uint32_t REFDONE;
} SDRAMC_TypeDef;


#define SDRAMC_CR0_BURSTLEN_Pos     0
#define SDRAMC_CR0_BURSTLEN_Msk     (0x07 << SDRAMC_CR0_BURSTLEN_Pos)
#define SDRAMC_CR0_CASDELAY_Pos     4
#define SDRAMC_CR0_CASDELAY_Msk     (0x07 << SDRAMC_CR0_CASDELAY_Pos)

#define SDRAMC_CR1_TRP_Pos          0
#define SDRAMC_CR1_TRP_Msk          (0x07 << SDRAMC_CR1_TRP_Pos)
#define SDRAMC_CR1_TRCD_Pos         3
#define SDRAMC_CR1_TRCD_Msk         (0x07 << SDRAMC_CR1_TRCD_Pos)
#define SDRAMC_CR1_TRC_Pos          6
#define SDRAMC_CR1_TRC_Msk          (0x0F << SDRAMC_CR1_TRC_Pos)
#define SDRAMC_CR1_TRAS_Pos         10
#define SDRAMC_CR1_TRAS_Msk         (0x07 << SDRAMC_CR1_TRAS_Pos)
#define SDRAMC_CR1_TRRD_Pos         13
#define SDRAMC_CR1_TRRD_Msk         (0x03 << SDRAMC_CR1_TRRD_Pos)
#define SDRAMC_CR1_TMRD_Pos         15
#define SDRAMC_CR1_TMRD_Msk         (0x07 << SDRAMC_CR1_TMRD_Pos)
#define SDRAMC_CR1_32BIT_Pos        18
#define SDRAMC_CR1_32BIT_Msk        (0x01 << SDRAMC_CR1_32BIT_Pos)
#define SDRAMC_CR1_BANK_Pos         19
#define SDRAMC_CR1_BANK_Msk         (0x01 << SDRAMC_CR1_BANK_Pos)
#define SDRAMC_CR1_CELL32BIT_Pos    20
#define SDRAMC_CR1_CELL32BIT_Msk    (0x01 << SDRAMC_CR1_CELL32BIT_Pos)
#define SDRAMC_CR1_CELLSIZE_Pos     21
#define SDRAMC_CR1_CELLSIZE_Msk     (0x03 << SDRAMC_CR1_CELLSIZE_Pos)
#define SDRAMC_CR1_HIGHSPEED_Pos    23
#define SDRAMC_CR1_HIGHSPEED_Msk    (0x01 << SDRAMC_CR1_HIGHSPEED_Pos)

#define SDRAMC_REFRESH_RATE_Pos     0
#define SDRAMC_REFRESH_RATE_Msk     (0xFFF << SDRAMC_REFRESH_RATE_Pos)
#define SDRAMC_REFRESH_EN_Pos       12
#define SDRAMC_REFRESH_EN_Msk       (0x01 << SDRAMC_REFRESH_EN_Pos)

#define SDRAMC_LATCH_INEDGE_Pos     0
#define SDRAMC_LATCH_INEDGE_Msk     (0x01 << SDRAMC_LATCH_INEDGE_Pos)
#define SDRAMC_LATCH_OUTEDGE_Pos    1
#define SDRAMC_LATCH_OUTEDGE_Msk    (0x01 << SDRAMC_LATCH_OUTEDGE_Pos)
#define SDRAMC_LATCH_WAITST_Pos     2
#define SDRAMC_LATCH_WAITST_Msk     (0x01 << SDRAMC_LATCH_WAITST_Pos)




typedef struct {
        __IO uint32_t IE;

        __IO uint32_t IF;

        __IO uint32_t IM;

        __IO uint32_t CR;

        __IO uint32_t ADDR;

        __IO uint32_t CMD;
} NORFLC_TypeDef;


#define NORFLC_IE_FINISH_Pos        0
#define NORFLC_IE_FINISH_Msk        (0x01 << NORFLC_IE_FINISH_Pos)
#define NORFLC_IE_TIMEOUT_Pos       1
#define NORFLC_IE_TIMEOUT_Msk       (0x01 << NORFLC_IE_TIMEOUT_Pos)

#define NORFLC_IF_FINISH_Pos        0
#define NORFLC_IF_FINISH_Msk        (0x01 << NORFLC_IF_FINISH_Pos)
#define NORFLC_IF_TIMEOUT_Pos       1
#define NORFLC_IF_TIMEOUT_Msk       (0x01 << NORFLC_IF_TIMEOUT_Pos)

#define NORFLC_IM_FINISH_Pos        0
#define NORFLC_IM_FINISH_Msk        (0x01 << NORFLC_IM_FINISH_Pos)
#define NORFLC_IM_TIMEOUT_Pos       1
#define NORFLC_IM_TIMEOUT_Msk       (0x01 << NORFLC_IM_TIMEOUT_Pos)

#define NORFLC_CR_RDTIME_Pos        0
#define NORFLC_CR_RDTIME_Msk        (0x1F << NORFLC_CR_RDTIME_Pos)
#define NORFLC_CR_WRTIME_Pos        5
#define NORFLC_CR_WRTIME_Msk        (0x07 << NORFLC_CR_WRTIME_Pos)
#define NORFLC_CR_BYTEIF_Pos        8
#define NORFLC_CR_BYTEIF_Msk        (0x01 << NORFLC_CR_BYTEIF_Pos)

#define NORFLC_CMD_DATA_Pos         0
#define NORFLC_CMD_DATA_Msk         (0xFFFF << NORFLC_CMD_DATA_Pos)
#define NORFLC_CMD_CMD_Pos          16
#define NORFLC_CMD_CMD_Msk          (0x07 << NORFLC_CMD_CMD_Pos)




typedef struct {
    __IO uint32_t CR;

    __O  uint32_t DATAIN;

    __IO uint32_t INIVAL;

    __I  uint32_t RESULT;
} CRC_TypeDef;


#define CRC_CR_EN_Pos               0
#define CRC_CR_EN_Msk               (0x01 << CRC_CR_EN_Pos)
#define CRC_CR_OREV_Pos             1
#define CRC_CR_OREV_Msk             (0x01 << CRC_CR_OREV_Pos)
#define CRC_CR_ONOT_Pos             2
#define CRC_CR_ONOT_Msk             (0x01 << CRC_CR_ONOT_Pos)
#define CRC_CR_CRC16_Pos            3       //1 CRC16    0 CRC32
#define CRC_CR_CRC16_Msk            (0x01 << CRC_CR_CRC16_Pos)
#define CRC_CR_IBITS_Pos            4
#define CRC_CR_IBITS_Msk            (0x03 << CRC_CR_IBITS_Pos)




typedef struct {
        __IO uint32_t MINSEC;

        __IO uint32_t DATHUR;

        __IO uint32_t MONDAY;

        __IO uint32_t YEAR;

        __IO uint32_t MINSECAL;

        __IO uint32_t DAYHURAL;

        __IO uint32_t LOAD;

        __IO uint32_t IE;

        __IO uint32_t IF;

        __IO uint32_t EN;

        __IO uint32_t CFGABLE;

        __IO uint32_t TRIM;

        __IO uint32_t TRIMM;
} RTC_TypeDef;


#define RTC_LOAD_TIME_Pos           0
#define RTC_LOAD_TIME_Msk           (0x01 << RTC_LOAD_TIME_Pos)
#define RTC_LOAD_ALARM_Pos          1
#define RTC_LOAD_ALARM_Msk          (0x01 << RTC_LOAD_ALARM_Pos)

#define RTC_MINSEC_SEC_Pos          0
#define RTC_MINSEC_SEC_Msk          (0x3F << RTC_MINSEC_SEC_Pos)
#define RTC_MINSEC_MIN_Pos          6
#define RTC_MINSEC_MIN_Msk          (0x3F << RTC_MINSEC_MIN_Pos)

#define RTC_DATHUR_HOUR_Pos         0
#define RTC_DATHUR_HOUR_Msk         (0x1F << RTC_DATHUR_HOUR_Pos)
#define RTC_DATHUR_DATE_Pos         5
#define RTC_DATHUR_DATE_Msk         (0x1F << RTC_DATHUR_DATE_Pos)

#define RTC_MONDAY_DAY_Pos          0
#define RTC_MONDAY_DAY_Msk          (0x07 << RTC_MONDAY_DAY_Pos)
#define RTC_MONDAY_MON_Pos          3
#define RTC_MONDAY_MON_Msk          (0x0F << RTC_MONDAY_MON_Pos)

#define RTC_MINSECAL_SEC_Pos        0
#define RTC_MINSECAL_SEC_Msk        (0x3F << RTC_MINSECAL_SEC_Pos)
#define RTC_MINSECAL_MIN_Pos        6
#define RTC_MINSECAL_MIN_Msk        (0x3F << RTC_MINSECAL_MIN_Pos)

#define RTC_DAYHURAL_HOUR_Pos       0
#define RTC_DAYHURAL_HOUR_Msk       (0x1F << RTC_DAYHURAL_HOUR_Pos)
#define RTC_DAYHURAL_SUN_Pos        5
#define RTC_DAYHURAL_SUN_Msk        (0x01 << RTC_DAYHURAL_SUN_Pos)
#define RTC_DAYHURAL_MON_Pos        6
#define RTC_DAYHURAL_MON_Msk        (0x01 << RTC_DAYHURAL_MON_Pos)
#define RTC_DAYHURAL_TUE_Pos        7
#define RTC_DAYHURAL_TUE_Msk        (0x01 << RTC_DAYHURAL_TUE_Pos)
#define RTC_DAYHURAL_WED_Pos        8
#define RTC_DAYHURAL_WED_Msk        (0x01 << RTC_DAYHURAL_WED_Pos)
#define RTC_DAYHURAL_THU_Pos        9
#define RTC_DAYHURAL_THU_Msk        (0x01 << RTC_DAYHURAL_THU_Pos)
#define RTC_DAYHURAL_FRI_Pos        10
#define RTC_DAYHURAL_FRI_Msk        (0x01 << RTC_DAYHURAL_FRI_Pos)
#define RTC_DAYHURAL_SAT_Pos        11
#define RTC_DAYHURAL_SAT_Msk        (0x01 << RTC_DAYHURAL_SAT_Pos)

#define RTC_IE_SEC_Pos              0
#define RTC_IE_SEC_Msk              (0x01 << RTC_IE_SEC_Pos)
#define RTC_IE_MIN_Pos              1
#define RTC_IE_MIN_Msk              (0x01 << RTC_IE_MIN_Pos)
#define RTC_IE_HOUR_Pos             2
#define RTC_IE_HOUR_Msk             (0x01 << RTC_IE_HOUR_Pos)
#define RTC_IE_DATE_Pos             3
#define RTC_IE_DATE_Msk             (0x01 << RTC_IE_DATE_Pos)
#define RTC_IE_ALARM_Pos            4
#define RTC_IE_ALARM_Msk            (0x01 << RTC_IE_ALARM_Pos)

#define RTC_IF_SEC_Pos              0
#define RTC_IF_SEC_Msk              (0x01 << RTC_IF_SEC_Pos)
#define RTC_IF_MIN_Pos              1
#define RTC_IF_MIN_Msk              (0x01 << RTC_IF_MIN_Pos)
#define RTC_IF_HOUR_Pos             2
#define RTC_IF_HOUR_Msk             (0x01 << RTC_IF_HOUR_Pos)
#define RTC_IF_DATE_Pos             3
#define RTC_IF_DATE_Msk             (0x01 << RTC_IF_DATE_Pos)
#define RTC_IF_ALARM_Pos            4
#define RTC_IF_ALARM_Msk            (0x01 << RTC_IF_ALARM_Pos)

#define RTC_TRIM_ADJ_Pos            0
#define RTC_TRIM_ADJ_Msk            (0xFF << RTC_TRIM_ADJ_Pos)
#define RTC_TRIM_DEC_Pos            8
#define RTC_TRIM_DEC_Msk            (0x01 << RTC_TRIM_DEC_Pos)

#define RTC_TRIMM_CYCLE_Pos         0
#define RTC_TRIMM_CYCLE_Msk         (0x07 << RTC_TRIMM_CYCLE_Pos)
#define RTC_TRIMM_INC_Pos           3
#define RTC_TRIMM_INC_Msk           (0x01 << RTC_TRIMM_INC_Pos)




typedef struct {
    __IO uint32_t LOAD;

    __I  uint32_t VALUE;

    __IO uint32_t CR;

    __IO uint32_t IF;

    __IO uint32_t FEED;
} WDT_TypeDef;


#define WDT_CR_EN_Pos               0
#define WDT_CR_EN_Msk               (0x01 << WDT_CR_EN_Pos)
#define WDT_CR_RSTEN_Pos            1
#define WDT_CR_RSTEN_Msk            (0x01 << WDT_CR_RSTEN_Pos)


/******************************************************************************/
/*                       Peripheral memory map                            */
/******************************************************************************/
#define RAM_BASE            0x20000000
#define AHB_BASE            0x40000000
#define APB_BASE            0x40010000

#define NORFLC_BASE         0x60000000
#define NORFLM_BASE         0x61000000

#define SRAMC_BASE          0x68000000
#define SRAMM_BASE          0x69000000

#define SDRAMC_BASE         0x78000000
#define SDRAMM_BASE         0x70000000

/* AHB Peripheral memory map */
#define SYS_BASE            (AHB_BASE + 0x00000)

#define DMA_BASE            (AHB_BASE + 0x01000)

#define LCD_BASE            (AHB_BASE + 0x02000)

#define CRC_BASE            (AHB_BASE + 0x03000)

#define SDIO_BASE           (AHB_BASE + 0x04000)

/* APB Peripheral memory map */
#define PORT_BASE           (APB_BASE + 0x00000)

#define GPIOA_BASE          (APB_BASE + 0x01000)
#define GPIOB_BASE          (APB_BASE + 0x02000)
#define GPIOC_BASE          (APB_BASE + 0x03000)
#define GPIOD_BASE          (APB_BASE + 0x04000)
#define GPIOM_BASE          (APB_BASE + 0x05000)
#define GPION_BASE          (APB_BASE + 0x06000)
#define GPIOP_BASE          (APB_BASE + 0x08000)

#define TIMR0_BASE          (APB_BASE + 0x07000)
#define TIMR1_BASE          (APB_BASE + 0x0700C)
#define TIMR2_BASE          (APB_BASE + 0x07018)
#define TIMR3_BASE          (APB_BASE + 0x07024)
#define TIMR4_BASE          (APB_BASE + 0x07030)
#define TIMR5_BASE          (APB_BASE + 0x0703C)
#define TIMRG_BASE          (APB_BASE + 0x07060)

#define WDT_BASE            (APB_BASE + 0x09000)

#define PWM0_BASE           (APB_BASE + 0x0A000)
#define PWM1_BASE           (APB_BASE + 0x0A020)
#define PWM2_BASE           (APB_BASE + 0x0A040)
#define PWM3_BASE           (APB_BASE + 0x0A060)
#define PWM4_BASE           (APB_BASE + 0x0A080)
#define PWM5_BASE           (APB_BASE + 0x0A0A0)
#define PWMG_BASE           (APB_BASE + 0x0A180)

#define RTC_BASE            (APB_BASE + 0x0B000)

#define ADC0_BASE           (APB_BASE + 0x0C000)
#define ADC1_BASE           (APB_BASE + 0x0D000)

#define FLASH_BASE          (APB_BASE + 0x0F000)

#define UART0_BASE          (APB_BASE + 0x10000)
#define UART1_BASE          (APB_BASE + 0x11000)
#define UART2_BASE          (APB_BASE + 0x12000)
#define UART3_BASE          (APB_BASE + 0x13000)

#define I2C0_BASE           (APB_BASE + 0x18000)
#define I2C1_BASE           (APB_BASE + 0x19000)

#define SPI0_BASE           (APB_BASE + 0x1C000)
#define SPI1_BASE           (APB_BASE + 0x1D000)

#define CAN_BASE            (APB_BASE + 0x20000)


/******************************************************************************/
/*                       Peripheral declaration                          */
/******************************************************************************/
#define SYS                 ((SYS_TypeDef  *) SYS_BASE)

#define PORT                ((PORT_TypeDef *) PORT_BASE)

#define GPIOA               ((GPIO_TypeDef *) GPIOA_BASE)
#define GPIOB               ((GPIO_TypeDef *) GPIOB_BASE)
#define GPIOC               ((GPIO_TypeDef *) GPIOC_BASE)
#define GPIOM               ((GPIO_TypeDef *) GPIOM_BASE)
#define GPION               ((GPIO_TypeDef *) GPION_BASE)
#define GPIOP               ((GPIO_TypeDef *) GPIOP_BASE)

#define TIMR0               ((TIMR_TypeDef *) TIMR0_BASE)
#define TIMR1               ((TIMR_TypeDef *) TIMR1_BASE)
#define TIMR2               ((TIMR_TypeDef *) TIMR2_BASE)
#define TIMR3               ((TIMR_TypeDef *) TIMR3_BASE)
#define TIMR4               ((TIMR_TypeDef *) TIMR4_BASE)
#define TIMR5               ((TIMR_TypeDef *) TIMR5_BASE)
#define TIMRG               ((TIMRG_TypeDef*) TIMRG_BASE)

#define UART0               ((UART_TypeDef *) UART0_BASE)
#define UART1               ((UART_TypeDef *) UART1_BASE)
#define UART2               ((UART_TypeDef *) UART2_BASE)
#define UART3               ((UART_TypeDef *) UART3_BASE)

#define SPI0                ((SPI_TypeDef  *) SPI0_BASE)
#define SPI1                ((SPI_TypeDef  *) SPI1_BASE)

#define I2C0                ((I2C_TypeDef  *) I2C0_BASE)
#define I2C1                ((I2C_TypeDef  *) I2C1_BASE)

#define ADC0                ((ADC_TypeDef  *) ADC0_BASE)
#define ADC1                ((ADC_TypeDef  *) ADC1_BASE)

#define PWM0                ((PWM_TypeDef  *) PWM0_BASE)
#define PWM1                ((PWM_TypeDef  *) PWM1_BASE)
#define PWM2                ((PWM_TypeDef  *) PWM2_BASE)
#define PWM3                ((PWM_TypeDef  *) PWM3_BASE)
#define PWM4                ((PWM_TypeDef  *) PWM4_BASE)
#define PWM5                ((PWM_TypeDef  *) PWM5_BASE)
#define PWMG                ((PWMG_TypeDef *) PWMG_BASE)

#define SDIO                ((SDIO_TypeDef *) SDIO_BASE)

#define DMA                 ((DMA_TypeDef  *) DMA_BASE)

#define CAN                 ((CAN_TypeDef  *) CAN_BASE)

#define LCD                 ((LCD_TypeDef  *) LCD_BASE)

#define CRC                 ((CRC_TypeDef  *) CRC_BASE)

#define RTC                 ((RTC_TypeDef  *) RTC_BASE)

#define WDT                 ((WDT_TypeDef  *) WDT_BASE)

#define FLASH               ((FLASH_Typedef*) FLASH_BASE)

#define SRAMC               ((SRAMC_TypeDef*) SRAMC_BASE)

#define NORFLC              ((NORFLC_TypeDef*) NORFLC_BASE)

#define SDRAMC              ((SDRAMC_TypeDef*) SDRAMC_BASE)

#include "SWM320_port.h"
#include "SWM320_gpio.h"
#include "SWM320_exti.h"
#include "SWM320_timr.h"
#include "SWM320_uart.h"
#include "SWM320_spi.h"
#include "SWM320_i2c.h"
#include "SWM320_pwm.h"
#include "SWM320_adc.h"
#include "SWM320_dma.h"
#include "SWM320_lcd.h"
#include "SWM320_can.h"
#include "SWM320_sdio.h"
#include "SWM320_flash.h"
#include "SWM320_norflash.h"
#include "SWM320_sdram.h"
#include "SWM320_crc.h"
#include "SWM320_rtc.h"
#include "SWM320_wdt.h"


#endif //__SWM320_H__

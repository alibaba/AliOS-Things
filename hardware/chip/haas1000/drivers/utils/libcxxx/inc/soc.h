/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */


/**************************************************************************//**
 * @file     soc.h
 * @brief    CSI Core Peripheral Access Layer Header File for
 *           CSKYSOC Device Series
 * @version  V1.0
 * @date     06. Mar 2019
 * @vendor   csky
 * @chip     pangu
 ******************************************************************************/
#ifndef _SOC_H_
#define _SOC_H_

#ifdef __CK804__

#ifndef SYSTEM_CLOCK
#define SYSTEM_CLOCK        (48000000) // 48Mhz fpga
#define  IHS_VALUE          (48000000)
#endif

#ifndef LSP_DEFAULT_FREQ
#define LSP_DEFAULT_FREQ    (48000000) // 48Mhz fpga
#define EHS_VALUE           (12000000)
#endif

#ifndef DSP_CLOCK
#define DSP_CLOCK           (250000000) // 250Mhz
#endif

#ifndef RTC_CLOCK
#define RTC_CLOCK           (32768)  // 32.768Mhz
#endif

/* ----------------------  YunVoice_V2 804ef Specific Interrupt Numbers  --------------------- */
typedef enum IRQn {
    ERROR_INTC_IRQn                 =   0,      /* error Interrupt */
    WDT_IRQn                        =   1,      /* wdt Interrupt */
    PLL_IRQn                        =   2,      /* pll lose lock Interrupt */
    PLL_FRAC_IRQn                   =   3,      /* pll lose lock Interrupt */
    WCOM_IRQn                       =   4,      /* cpu0 wcom Interrupt */
    RCOM_IRQn                       =   5,      /* cpu0 rcom Interrupt */
    PMU_IRQn                        =   6,      /* PMU Interrupt */
    MCA_IRQn                        =   7,      /* MCA Interrupt */
    DMAC0_IRQn                      =   8,      /* DMA0 Interrupt */
    DMAC1_IRQn                      =   9,      /* DMA1 Interrupt */
    CORET_IRQn                      =   10,     /* CTIM0 Interrupt */
    RTC_IRQn                        =   11,     /* RTC Interrupt */
    SDIO1_IRQn                      =   12,     /* SDMMC Interrupt */
    USB_IRQn                        =   13,     /* USB Interrupt */
    QSPI_IRQn                       =   14,     /* QSPI Interrupt */
    SDIO0_IRQn                      =   15,     /* SDIO Interrupt */
    CODEC_WHISPER_IRQn              =   16,     /* codec whisper Interrupt */
    CODEC_INTC_IRQn                 =   17,     /* code Interrupt */
    GSK_INTC_IRQn                   =   18,     /* gsk Interrupt */
    LPTIMER_INT0_IRQn               =   19,     /* timer0 Interrupt */
    LPTIMER_INT1_IRQn               =   20,     /* timer0  Interrupt */
    TIMER0_INT0_IRQn                =   21,     /* timer1 Interrupt */
    TIMER0_INT1_IRQn                =   22,     /* timer1 Interrupt */
    TIMER1_INT0_IRQn                =   23,     /* timer2 Interrupt */
    TIMER1_INT1_IRQn                =   24,     /* timer2  Interrupt */
    TIMER2_INT0_IRQn                =   25,     /* timer3 Interrupt */
    TIMER2_INT1_IRQn                =   26,     /* timer3  Interrupt */
    USI0_IRQn                       =   27,     /* USI0 Interrupt */
    USI1_IRQn                       =   28,     /* USI1 Interrupt */
    USI2_IRQn                       =   29,     /* USI2 Interrupt */
    USI3_IRQn                       =   30,     /* USI3 Interrupt */
    I2S0_IRQn                       =   31,     /* I2S0 Interrupt */
    I2S1_IRQn                       =   32,     /* I2S1 Interrupt */
    I2S2_IRQn                       =   33,     /* I2S2 Interrupt */
    I2S3_IRQn                       =   34,     /* I2S3 Interrupt */
    SPDIF0_IRQn                     =   35,     /* spdif0 Interrupt */
    UART0_HS_IRQn                   =   36,     /* I2S0 Interrupt */
    UART1_HS_IRQn                   =   37,     /* I2S1 Interrupt */
    UART2_IRQn                      =   38,     /* I2S2 Interrupt */
    UART3_IRQn                      =   39,     /* I2S3 Interrupt */
    TDM_IRQn                        =   40,     /* tdm Interrupt */
    PDM_IRQn                        =   41,     /* pdm Interrupt */
    GPIO0_IRQn                      =   42,     /* gpio0 Interrupt */
    PLL_LL_IRQn                     =   43,     /* pll lose lock Interrupt */
    PLL_LL_FRAC_IRQn                =   44,     /* pll lose lock FRAC Interrupt */
    PWM0_IRQn                       =   45,     /* pwm0 Interrupt */
    RSA_IRQn                        =   46,     /* rsa Interrupt */
    SHA_IRQn                        =   47,     /* sha Interrupt */
    AES_IRQn                        =   48,     /* aes Interrupt */
    TRNG_IRQn                       =   49,     /* trng Interrupt */
    GPIO1_IRQn                      =   50,     /* gpio1 Interrupt */
    PWM1_IRQn                       =   51,     /* pwm1 Interrupt */
} IRQn_Type;

/*cpu0*/
#define CONFIG_DMA_CHANNEL_NUM  8
#define CONFIG_PER_DMAC0_CHANNEL_NUM 0
#define CONFIG_PER_DMAC1_CHANNEL_NUM 8
#define CONFIG_AES_NUM          1
#define CONFIG_RSA_NUM          1
#define CONFIG_SHA_NUM          1
#define CONFIG_TRNG_NUM         1
#define CONFIG_RTC_NUM          1
#define CONFIG_WDT_NUM          1
#define CONFIG_QSPI_NUM         1
#define CONFIG_SPIFLASH_NUM     1
#define CONFIG_MAILBOX_NUM      3
#define CONFIG_MAILBOX_CHANNEL_NUM      2
#define CONFIG_EFUSEC_NUM       1
#define CONFIG_SDIO_NUM         2
#define CSKY_TIPC_MOUDULE_NUM   16
#define CONFIG_CODEC_NUM 1

#else           /* ck805 */

#define CONFIG_QSPI_NUM         1
#define CONFIG_SPIFLASH_NUM     1
#ifndef SYSTEM_CLOCK
#define SYSTEM_CLOCK        (18000000) // 18Mhz fpga
#define  IHS_VALUE          (18000000)
#endif

#ifndef LSP_DEFAULT_FREQ
#define LSP_DEFAULT_FREQ    (18000000) // 18Mhz fpga
#define EHS_VALUE          (12000000)
#endif

#ifndef DSP_CLOCK
#define DSP_CLOCK           (250000000) // 250Mhz
#endif

#ifndef RTC_CLOCK
#define RTC_CLOCK           (32768)  // 32.768Khz
#endif

/* ----------------------  YunVoice_V2 805 Specific Interrupt Numbers  --------------------- */
typedef enum IRQn {
    ERROR_INTC_IRQn                 =   0,      /* error Interrupt */
    WDT_IRQn                        =   1,      /* wdt Interrupt */
    PLL_IRQn                        =   2,      /* pll lose lock Interrupt */
    PLL_FRAC_IRQn                   =   3,      /* pll lose lock Interrupt */
    WCOM_IRQn                       =   4,      /* cpu0 wcom Interrupt */
    RCOM_IRQn                       =   5,      /* cpu0 rcom Interrupt */
    PMU_IRQn                        =   6,      /* PMU Interrupt */
    MCA_IRQn                        =   7,      /* MCA Interrupt */
    DMAC0_IRQn                      =   8,      /* DMA0 Interrupt */
    DMAC1_IRQn                      =   9,      /* DMA1 Interrupt */
    CORET_IRQn                      =   10,     /* CTIM0 Interrupt */
    RTC_IRQn                        =   11,     /* RTC Interrupt */
    SDIO1_IRQn                      =   12,     /* SDMMC Interrupt */
    USB_IRQn                        =   13,     /* USB Interrupt */
    QSPI_IRQn                       =   14,     /* QSPI Interrupt */
    SDIO0_IRQn                      =   15,     /* SDIO Interrupt */
    CODEC_WHISPER_IRQn              =   16,     /* codec whisper Interrupt */
    CODEC_INTC_IRQn                 =   17,     /* code Interrupt */
    GSK_INTC_IRQn                   =   18,     /* gsk Interrupt */
    LPTIMER_INT0_IRQn               =   19,     /* timer0 Interrupt */
    LPTIMER_INT1_IRQn               =   20,     /* timer0  Interrupt */
    TIMER0_INT0_IRQn                =   21,     /* timer1 Interrupt */
    TIMER0_INT1_IRQn                =   22,     /* timer1 Interrupt */
    TIMER1_INT0_IRQn                =   23,     /* timer2 Interrupt */
    TIMER1_INT1_IRQn                =   24,     /* timer2  Interrupt */
    TIMER2_INT0_IRQn                =   25,     /* timer3 Interrupt */
    TIMER2_INT1_IRQn                =   26,     /* timer3  Interrupt */
    USI0_IRQn                       =   27,     /* USI0 Interrupt */
    USI1_IRQn                       =   28,     /* USI1 Interrupt */
    USI2_IRQn                       =   29,     /* USI2 Interrupt */
    USI3_IRQn                       =   30,     /* USI3 Interrupt */
    I2S0_IRQn                       =   31,     /* I2S0 Interrupt */
    I2S1_IRQn                       =   32,     /* I2S1 Interrupt */
    I2S2_IRQn                       =   33,     /* I2S2 Interrupt */
    I2S3_IRQn                       =   34,     /* I2S3 Interrupt */
    SPDIF0_IRQn                     =   35,     /* spdif0 Interrupt */
    UART0_HS_IRQn                   =   36,     /* uart0 Interrupt */
    UART1_HS_IRQn                   =   37,     /* uart1 Interrupt */
    UART2_IRQn                      =   38,     /* uart2 Interrupt */
    UART3_IRQn                      =   39,     /* uart3 Interrupt */
    TDM_IRQn                        =   40,     /* tdm Interrupt */
    PDM_IRQn                        =   41,     /* pdm Interrupt */
    GPIO0_IRQn                      =   42,     /* gpio0 Interrupt */
    PLL_LL_IRQn                     =   43,     /* pll lose lock Interrupt */
    PLL_LL_FRAC_IRQn                =   44,     /* pll lose lock FRAC Interrupt */
    PWM0_IRQn                       =   45,     /* pwm0 Interrupt */
    RSA_IRQn                        =   46,     /* rsa Interrupt */
    SHA_IRQn                        =   47,     /* sha Interrupt */
    AES_IRQn                        =   48,     /* aes Interrupt */
    TRNG_IRQn                       =   49,     /* trng Interrupt */
    GPIO1_IRQn                      =   50,     /* gpio1 Interrupt */
    PWM1_IRQn                       =   51,     /* pwm1 Interrupt */
} IRQn_Type;

#define CONFIG_DMA_CHANNEL_NUM  4
#define CONFIG_PER_DMAC0_CHANNEL_NUM 4
#define CONFIG_PER_DMAC1_CHANNEL_NUM 0
#define CONFIG_RTC_NUM          1
#define CONFIG_WDT_NUM          1
#define CONFIG_MAILBOX_NUM      3
#define CONFIG_MAILBOX_CHANNEL_NUM      2
#define CONFIG_CODEC_NUM    1
#define CONFIG_SDIO_NUM         2

#endif

/* ================================================================================ */
/* ================      Processor and Core Peripheral Section     ================ */
/* ================================================================================ */

/* --------  Configuration of the CK805 Processor and Core Peripherals  ------- */
#define __CK805_REV               0x0000U   /* Core revision r0p0 */
#define __MGU_PRESENT             0         /* MGU present or not */
#define __NVIC_PRIO_BITS          2         /* Number of Bits used for Priority Levels */

#include "csi_core.h"                     /* Processor and core peripherals */
#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    //DMA0
    DWENUM_DMA_I2S0_TX,     //0
    DWENUM_DMA_I2S0_RX,
    DWENUM_DMA_I2S1_TX,
    DWENUM_DMA_I2S1_RX,
    DWENUM_DMA_USI0_TX,
    DWENUM_DMA_USI0_RX,     //5
    DWENUM_DMA_USI1_TX,
    DWENUM_DMA_USI1_RX,
    DWENUM_DMA_UART0_TX,
    DWENUM_DMA_UART0_RX,
    DWENUM_DMA_UART1_TX,    //10
    DWENUM_DMA_UART1_RX,    //11
    //DMA1
    //fixed
    DWENUM_DMA_CODEC_ADC_0, //12
    DWENUM_DMA_CODEC_ADC_1,
    DWENUM_DMA_CODEC_ADC_4,
    DWENUM_DMA_CODEC_ADC_5, //15
    DWENUM_DMA_UART2_TX,
    DWENUM_DMA_UART2_RX,
    DWENUM_DMA_USI2_TX,
    DWENUM_DMA_USI2_RX,     //19
    //variable
    DWENUM_DMA_I2S2_TX,     //20
    DWENUM_DMA_I2S2_RX,
    DWENUM_DMA_CODEC_ADC_6,
    DWENUM_DMA_CODEC_ADC_7,
    DWENUM_DMA_CODEC_DAC,
    DWENUM_DMA_RESERVED_1,  //25
    DWENUM_DMA_I2S3_TX,
    DWENUM_DMA_I2S3_RX,
    DWENUM_DMA1_USI3_TX,
    DWENUM_DMA1_USI3_RX,
    DWENUM_DMA_UART3_TX,    //30
    DWENUM_DMA_UART3_RX,
    DWENUM_DMA_RESERVED_2,  //32
    DWENUM_DMA_RESERVED_3,
    DWENUM_DMA_SPDIF_TX,
    DWENUM_DMA_SPDIF_RX,    //35
    DWENUM_DMA_RESERVED_4,
    DWENUM_DMA_RESERVED_5,  //37
    DWENUM_DMA_TDM_0,
    DWENUM_DMA_TDM_1,
    DWENUM_DMA_TDM_2,       //40
    DWENUM_DMA_TDM_3,
    DWENUM_DMA_TDM_4,       //42
    DWENUM_DMA_TDM_5,
    DWENUM_DMA_TDM_6,
    DWENUM_DMA_TDM_7,       //45
    DWENUM_DMA_PDM_0,
    DWENUM_DMA_PDM_1,       //47
    DWENUM_DMA_PDM_2,
    DWENUM_DMA_PDM_3,
    DWENUM_DMA_CODEC_ADC_2, //50
    DWENUM_DMA_CODEC_ADC_3, //51
    DWENUM_DMA_MEMORY,
} dwenum_dma_device_e;

typedef enum {
    ADDR_SPACE_ERROR      = -1,
    ADDR_SPACE_EFLASH     =  0,
    ADDR_SPACE_SRAM       =  1,
    ADDR_SPACE_PERIPHERAL =  2,
    ADDR_SPACE_TCIP       =  3
} addr_space_e;

/* -------------------------  Wakeup Number Definition  ------------------------ */
typedef enum Wakeupn {
    GPIO0_Wakeupn         =   0,     /* GPIO0 wakeup */
    AONTIM_Wakeupn        =   1,     /* AONTIM wakeup */
    RTC_Wakeupn           =   2,     /* RTC wakeup */
    CODEC_WT_Wakeun       =   3      /* codec whisper wakeup */
}
Wakeupn_Type;

#define CONFIG_TIMER_NUM        8
#define CONFIG_USI_NUM          4
#define CONFIG_USART_NUM        4
#define CONFIG_PWM_NUM          2
#define CONFIG_PER_PWM_CHANNEL_NUM    12
#define CONFIG_I2S_NUM          4
#define CONFIG_GPIO_PIN_NUM     64
#define CONFIG_GPIO_NUM         2
#define CONFIG_PMU_NUM          1

/* ================================================================================ */
/* ================              Peripheral memory map             ================ */
/* ================================================================================ */
#define CSKY_QSPIMEM_BASE               (0x08000000UL)
#define CSKY_CK804_TCM0_BASE            (0xFFE0000UL)
#define CSKY_CK804_TCM1_BASE            (0xFFE8000UL)
#define CSKY_CK804_TCM2_BASE            (0xFFF0000UL)
#define CSKY_CK804_TCM3_BASE            (0xFFF8000UL)
#define CSKY_CK804_TCM4_BASE            (0x10000000UL)
#define CSKY_CK804_TCM5_BASE            (0x10008000UL)
#define CSKY_CK804_TCM6_BASE            (0x10010000UL)
#define CSKY_CK804_TCM7_BASE            (0x10018000UL)
#define CSKY_CK804_TCM_SIZE             (0x8000UL)

#define CSKY_FMC_BASE                   (0x18000000UL)
#define CSKY_FMC_SIZE                   (0x800000UL)
#define CSKY_SRAM_BASE                  (CSKY_CK804_TCM2_BASE)

/* AHB SUB0 */
#define CSKY_SDIO0_BASE                 (0x21000000UL)
#define CSKY_SDIO1_BASE                 (0x22000000UL)
#define CSKY_DMAC0_BASE                 (0x23000000UL)
#define CSKY_SASC_BASE                  (0x24000000UL)
/* AHB SUB1 */
#define CSKY_AES_BASE                   (0x28000000UL)
#define CSKY_SHA_BASE                   (0x29000000UL)
#define CSKY_FFT_BASE                   (0x2A000000UL)
/* AHB SUB2 */
#define CSKY_AONCPR_BASE                (0x30000000UL)
#define CSKY_CPR0_BASE                  (0x31000000UL)
#define CSKY_MCC0_BASE                  (0x32000000UL)
#define CSKY_RSA_BASE                   (0x33000000UL)
#define CSKY_TRNG_BASE                  (0x34000000UL)

/* CPU0 CK804 APB0 */
#define CSKY_WDT_BASE                   (0x38000000UL)
#define CSKY_RTC_BASE                   (0x38800000UL)
#define CSKY_LPTIM_BASE                 (0x39000000UL)
#define CSKY_TIM0_BASE                  (0x39800000UL)
#define CSKY_EFUSE_BASE                 (0x3A000000UL)
#define CSKY_QSPIC0_BASE                (0x3A800000UL)
#define CSKY_PADREG_BASE                (0x3B000000UL)
#define CSKY_TIPC_BASE                  (0x3B800000UL)
#define CSKY_GPIO0_BASE                 (0x3C000000UL)
#define CSKY_PWM0_BASE                  (0x3C800000UL)
#define CSKY_I2S0_BASE                  (0x3D000000UL)
#define CSKY_I2S1_BASE                  (0x3D800000UL)
#define CSKY_USI0_BASE                  (0x3E000000UL)
#define CSKY_USI1_BASE                  (0x3E800000UL)
#define CSKY_UART0_BASE                 (0x3F000000UL)
#define CSKY_UART1_BASE                 (0x3F800000UL)

#define CSKY_CK805_TCM0_BASE            (0x80000000UL)
#define CSKY_CK805_TCM1_BASE            (0x80020000UL)
#define CSKY_CK805_TCM0_SIZE            (0x20000UL)
#define CSKY_CK805_TCM1_SIZE            (0x10000UL)
/* CK805 SUB0 */
#define CSKY_DMAC1_BASE                 (0x88000000UL)
#define CSKY_MCA_BASE                   (0x89000000UL)
#define CSKY_MCC1_BASE                  (0x8A000000UL)
#define CSKY_CPR1_BASE                  (0x8B000000UL)

/* CK805 APB1 */
#define CSKY_CODEC_BASE                 (0x90000000UL)
#define CSKY_TIM1_BASE                  (0x90800000UL)
#define CSKY_TIM2_BASE                  (0x91000000UL)
#define CSKY_SPDIF_BASE                 (0x91800000UL)
#define CSKY_PDM_BASE                   (0x92000000UL)
#define CSKY_TDM_BASE                   (0x92800000UL)
#define CSKY_GPIO1_BASE                 (0x93000000UL)
#define CSKY_PWM1_BASE                  (0x93800000UL)
#define CSKY_I2S2_BASE                  (0x94000000UL)
#define CSKY_I2S3_BASE                  (0x94800000UL)
#define CSKY_USI2_BASE                  (0x95000000UL)
#define CSKY_USI3_BASE                  (0x95800000UL)
#define CSKY_UART2_BASE                 (0x96000000UL)
#define CSKY_UART3_BASE                 (0x96800000UL)

#define SDMMC_BASE_ADDR                 CSKY_SDIO0_BASE
#define SHA_CONTEXT_SIZE                (224)
#define CONFIG_USI_V25
#define USI_USART_FUN_DIS               1
/* ================================================================================ */
/* ================             Peripheral declaration             ================ */
/* ================================================================================ */

#define YUN_PMU                   ((pmu_reg_t *)CSKY_AONCPR_BASE)
#define YUN_CPR0                  ((cpr0_reg_t *)CSKY_CPR0_BASE)
#define YUN_CPR1                  ((cpr1_reg_t *)CSKY_CPR1_BASE)

#include <sys_freq.h>
#include <cpr_regs.h>
#include <pmu_regs.h>

/* ================================================================================ */
/* ================                  otp declaration               ================ */
/* ================================================================================ */
#define CONFIG_OTP_BASE_ADDR 0
#define CONFIG_OTP_BANK_SIZE 640


#ifdef __cplusplus
}
#endif

#endif  /* SOC_H_ */

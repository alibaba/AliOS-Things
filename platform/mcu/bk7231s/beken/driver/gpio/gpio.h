#ifndef _GPIO_H_
#define _GPIO_H_

#include "sys_config.h"

#define GPIO_PRT                        os_printf
#define WARN_PRT                        os_printf
#define FATAL_PRT                       os_printf

#if CFG_JTAG_ENABLE
#define JTAG_GPIO_FILTER
#endif

#define GPIO_INIT_FLAG                   ((UINT32)1)
#define GPIO_UNINIT_FLAG                 ((UINT32)-1)

#define GPIO_BASE_ADDR                       (0x0802800)

#define REG_GPIO_0_CONFIG                    (GPIO_BASE_ADDR + 0*4)
#define REG_GPIO_1_CONFIG                    (GPIO_BASE_ADDR + 1*4)
#define REG_GPIO_2_CONFIG                    (GPIO_BASE_ADDR + 2*4)
#define REG_GPIO_3_CONFIG                    (GPIO_BASE_ADDR + 3*4)
#define REG_GPIO_4_CONFIG                    (GPIO_BASE_ADDR + 4*4)
#define REG_GPIO_5_CONFIG                    (GPIO_BASE_ADDR + 5*4)
#define REG_GPIO_6_CONFIG                    (GPIO_BASE_ADDR + 6*4)
#define REG_GPIO_7_CONFIG                    (GPIO_BASE_ADDR + 7*4)
#define REG_GPIO_8_CONFIG                    (GPIO_BASE_ADDR + 8*4)
#define REG_GPIO_9_CONFIG                    (GPIO_BASE_ADDR + 9*4)
#define REG_GPIO_10_CONFIG                   (GPIO_BASE_ADDR + 10*4)
#define REG_GPIO_11_CONFIG                   (GPIO_BASE_ADDR + 11*4)
#define REG_GPIO_12_CONFIG                   (GPIO_BASE_ADDR + 12*4)
#define REG_GPIO_13_CONFIG                   (GPIO_BASE_ADDR + 13*4)
#define REG_GPIO_14_CONFIG                   (GPIO_BASE_ADDR + 14*4)
#define REG_GPIO_15_CONFIG                   (GPIO_BASE_ADDR + 15*4)
#define REG_GPIO_16_CONFIG                   (GPIO_BASE_ADDR + 16*4)
#define REG_GPIO_17_CONFIG                   (GPIO_BASE_ADDR + 17*4)
#define REG_GPIO_18_CONFIG                   (GPIO_BASE_ADDR + 18*4)
#define REG_GPIO_19_CONFIG                   (GPIO_BASE_ADDR + 19*4)
#define REG_GPIO_20_CONFIG                   (GPIO_BASE_ADDR + 20*4)
#define REG_GPIO_21_CONFIG                   (GPIO_BASE_ADDR + 21*4)
#define REG_GPIO_22_CONFIG                   (GPIO_BASE_ADDR + 22*4)
#define REG_GPIO_23_CONFIG                   (GPIO_BASE_ADDR + 23*4)
#define REG_GPIO_24_CONFIG                   (GPIO_BASE_ADDR + 24*4)
#define REG_GPIO_25_CONFIG                   (GPIO_BASE_ADDR + 25*4)
#define REG_GPIO_26_CONFIG                   (GPIO_BASE_ADDR + 26*4)
#define REG_GPIO_27_CONFIG                   (GPIO_BASE_ADDR + 27*4)
#define REG_GPIO_28_CONFIG                   (GPIO_BASE_ADDR + 28*4)
#define REG_GPIO_29_CONFIG                   (GPIO_BASE_ADDR + 29*4)
#define REG_GPIO_30_CONFIG                   (GPIO_BASE_ADDR + 30*4)
#define REG_GPIO_31_CONFIG                   (GPIO_BASE_ADDR + 31*4)

#define REG_GPIO_CFG_BASE_ADDR               (REG_GPIO_0_CONFIG)


#define GCFG_INPUT_POS                       0
#define GCFG_OUTPUT_POS                      1
#define GCFG_INPUT_ENABLE_POS                2
#define GCFG_OUTPUT_ENABLE_POS               3
#define GCFG_PULL_MODE_POS                   4
#define GCFG_PULL_ENABLE_POS                 5
#define GCFG_FUNCTION_ENABLE_POS             6
#define GCFG_INPUT_MONITOR_POS               7


#define GCFG_INPUT_BIT                       (1 << 0)
#define GCFG_OUTPUT_BIT                      (1 << 1)
#define GCFG_INPUT_ENABLE_BIT                (1 << 2)
#define GCFG_OUTPUT_ENABLE_BIT               (1 << 3)
#define GCFG_PULL_MODE_BIT                   (1 << 4)
#define GCFG_PULL_ENABLE_BIT                 (1 << 5)
#define GCFG_FUNCTION_ENABLE_BIT             (1 << 6)
#define GCFG_INPUT_MONITOR_BIT               (1 << 7)



#define REG_GPIO_FUNC_CFG                    (GPIO_BASE_ADDR + 32*4)
#define PERIAL_MODE_1                         (0)
#define PERIAL_MODE_2                         (1)
#define PERIAL_MODE_3                         (2)
#define PERIAL_MODE_4                         (3)

#define REG_GPIO_INTEN                       (GPIO_BASE_ADDR + 33*4)
#define REG_GPIO_INTLV0                      (GPIO_BASE_ADDR + 34*4)
#define REG_GPIO_INTLV1                      (GPIO_BASE_ADDR + 35*4)
#define REG_GPIO_INTSTA                      (GPIO_BASE_ADDR + 36*4)

#define REG_GPIO_EXTRAL_INT_CFG              (GPIO_BASE_ADDR + 38*4)
#if (CFG_SOC_NAME == SOC_BK7231)
#define DPLL_UNLOCK_INT                       (1 << 0)
#else
#define DPLL_UNLOCK_INT_EN                    (1 << 0)
#define AUDIO_DPLL_UNLOCK_INT_EN              (1 << 1)
#define DPLL_UNLOCK_INT                       (1 << 2)
#define AUDIO_DPLL_UNLOCK_INT                 (1 << 3)
#define GPIO_EXTRAL_INT_MASK                  (DPLL_UNLOCK_INT | AUDIO_DPLL_UNLOCK_INT)
#if (CFG_SOC_NAME == SOC_BK7221U)
#define USB_PLUG_IN_INT_EN                    (1 << 4)
#define USB_PLUG_OUT_INT_EN                   (1 << 5)
#define USB_PLUG_IN_INT                       (1 << 6)
#define USB_PLUG_OUT_INT                      (1 << 7)
#define GPIO_EXTRAL_INT_MASK                  (DPLL_UNLOCK_INT | AUDIO_DPLL_UNLOCK_INT | USB_PLUG_IN_INT | USB_PLUG_OUT_INT)
#endif  // (CFG_SOC_NAME == SOC_BK7221U)
#endif  //  (CFG_SOC_NAME == SOC_BK7231)
                 
#define REG_GPIO_DETECT                      (GPIO_BASE_ADDR + 39*4)
#define IS_OVER_TEMP_DECT_BIT                 (1 << 0)
#define IS_USB_PLUG_IN_BIT                    (1 << 1)

#define REG_GPIO_ENC_WORD                    (GPIO_BASE_ADDR + 40*4)
#define REG_GPIO_DBG_MSG                     (GPIO_BASE_ADDR + 41*4)
#define REG_GPIO_DBG_MUX                     (GPIO_BASE_ADDR + 42*4)
#define REG_GPIO_DBG_CFG                     (GPIO_BASE_ADDR + 43*4)
#define REG_GPIO_DBG_REPORT                  (GPIO_BASE_ADDR + 44*4)

#define REG_GPIO_MODULE_SELECT               (GPIO_BASE_ADDR + 45*4)
#define GPIO_MODUL_NONE                       0xff

#if (CFG_SOC_NAME != SOC_BK7231)
#define GPIO_SD_DMA_MODULE                    (0 << 2)
#define GPIO_SD_HOST_MODULE                   (1 << 2)
#define GPIO_SD1_DMA_MODULE                   (2 << 2)
#define GPIO_SD1_HOST_MODULE                  (3 << 2)
#define GPIO_SD_MODULE_POS                    (2)
#define GPIO_SD_MODULE_MASK                   (0x3 << 2)
#else
#define GPIO_SD_DMA_MODULE                    (0 << 1)
#define GPIO_SD_HOST_MODULE                   (1 << 1)
#define GPIO_SD_MODULE_POS                    (1)
#define GPIO_SD_MODULE_MASK                   (0x1 << 1)
#endif // (CFG_SOC_NAME != SOC_BK7231)

#if (CFG_SOC_NAME != SOC_BK7231)
#define GPIO_SPI1_DMA_MODULE                  (2 << 0)
#define GPIO_SPI1_MODULE                      (3 << 0)
#define GPIO_SPI_DMA_MODULE                   (0 << 0)
#define GPIO_SPI_MODULE                       (1 << 0)
#define GPIO_SPI_MODULE_POS                   (0)
#define GPIO_SPI_MODULE_MASK                  (0x3 << 0)
#else
#define GPIO_SPI_DMA_MODULE                   (0 << 0)
#define GPIO_SPI_MODULE                       (1 << 0)
#define GPIO_SPI_MODULE_POS                   (0)
#define GPIO_SPI_MODULE_MASK                  (0x1 << 0)
#endif

#if (CFG_SOC_NAME == SOC_BK7231)
#define REG_A0_CONFIG                        (GPIO_BASE_ADDR + 48*4)
#define REG_A1_CONFIG                        (GPIO_BASE_ADDR + 49*4)
#define REG_A2_CONFIG                        (GPIO_BASE_ADDR + 50*4)
#define REG_A3_CONFIG                        (GPIO_BASE_ADDR + 51*4)
#define REG_A4_CONFIG                        (GPIO_BASE_ADDR + 52*4)
#define REG_A5_CONFIG                        (GPIO_BASE_ADDR + 53*4)
#define REG_A6_CONFIG                        (GPIO_BASE_ADDR + 54*4)
#define REG_A7_CONFIG                        (GPIO_BASE_ADDR + 55*4)
#else
// for GPIO 16-31
#define REG_GPIO_FUNC_CFG_2                  (GPIO_BASE_ADDR + 46*4)
#define GPIO_PCFG2_POSI(x)                   (((x)-16)*2)
#define GPIO_PCFG2_MASK(x)                   (0x03UL << GPIO_PCFG2_POSI(x))
#define GPIO_PCFG2_1_FUNC(x)                 (0x00UL << GPIO_PCFG2_POSI(x))
#define GPIO_PCFG2_2_FUNC(x)                 (0x01UL << GPIO_PCFG2_POSI(x))
#define GPIO_PCFG2_3_FUNC(x)                 (0x02UL << GPIO_PCFG2_POSI(x))
#define GPIO_PCFG2_4_FUNC(x)                 (0x03UL << GPIO_PCFG2_POSI(x))


// for GPIO 32-39
#define REG_GPIO_FUNC_CFG_3                  (GPIO_BASE_ADDR + 47*4)
#define GPIO_PCFG3_POSI(x)                   (((x)-32)*2)
#define GPIO_PCFG3_MASK(x)                   (0x03UL << GPIO_PCFG3_POSI(x))
#define GPIO_PCFG3_1_FUNC(x)                 (0x00UL << GPIO_PCFG3_POSI(x))
#define GPIO_PCFG3_2_FUNC(x)                 (0x01UL << GPIO_PCFG3_POSI(x))
#define GPIO_PCFG3_3_FUNC(x)                 (0x02UL << GPIO_PCFG3_POSI(x))
#define GPIO_PCFG3_4_FUNC(x)                 (0x03UL << GPIO_PCFG3_POSI(x))

#define REG_GPIO_32_CONFIG                   (GPIO_BASE_ADDR + 48*4)
#define REG_GPIO_33_CONFIG                   (GPIO_BASE_ADDR + 49*4)
#define REG_GPIO_34_CONFIG                   (GPIO_BASE_ADDR + 50*4)
#define REG_GPIO_35_CONFIG                   (GPIO_BASE_ADDR + 51*4)
#define REG_GPIO_36_CONFIG                   (GPIO_BASE_ADDR + 52*4)
#define REG_GPIO_37_CONFIG                   (GPIO_BASE_ADDR + 53*4)
#define REG_GPIO_38_CONFIG                   (GPIO_BASE_ADDR + 54*4)
#define REG_GPIO_39_CONFIG                   (GPIO_BASE_ADDR + 55*4)
#endif // #if (CFG_SOC_NAME == SOC_BK7231)

#endif // _GPIO_H_

// EOF


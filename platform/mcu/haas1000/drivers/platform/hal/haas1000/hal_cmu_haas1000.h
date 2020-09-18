/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __HAL_CMU_BEST2001_H__
#define __HAL_CMU_BEST2001_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef FPGA
#define HAL_CMU_DEFAULT_CRYSTAL_FREQ        26000000
#define HAL_CMU_VALID_CRYSTAL_FREQ          { HAL_CMU_DEFAULT_CRYSTAL_FREQ, }
#else
#define HAL_CMU_VALID_CRYSTAL_FREQ          { 26000000, 40000000, }
#endif

enum HAL_CMU_MOD_ID_T {
    // HCLK/HRST
    HAL_CMU_MOD_H_MCU,          // 0
    HAL_CMU_MOD_H_CACHE0,       // 1
    HAL_CMU_MOD_H_CP,           // 2
    HAL_CMU_MOD_H_CACHE1,       // 3
    HAL_CMU_MOD_H_ADMA,         // 4
    HAL_CMU_MOD_H_GDMA,         // 5
    HAL_CMU_MOD_H_SEC_ENG,      // 6
    HAL_CMU_MOD_H_USBC,         // 7
    HAL_CMU_MOD_H_USBH,         // 8
    HAL_CMU_MOD_H_I2C_SLAVE,    // 9
    HAL_CMU_MOD_H_AX2H_A7,      // 10
    HAL_CMU_MOD_H_AH2H_WF,      // 11
    HAL_CMU_MOD_H_AH2H_BT,      // 12
    HAL_CMU_MOD_H_CODEC,        // 13
    HAL_CMU_MOD_H_AHB1,         // 14
    HAL_CMU_MOD_H_AHB0,         // 15
    HAL_CMU_MOD_H_PSRAM1G,      // 16
    HAL_CMU_MOD_H_PSRAM200,     // 17
    HAL_CMU_MOD_H_FLASH,        // 18
    HAL_CMU_MOD_H_RAM9,         // 19
    HAL_CMU_MOD_H_RAM8,         // 20
    HAL_CMU_MOD_H_RAM7,         // 21
    HAL_CMU_MOD_H_RAM3_6,       // 22
    HAL_CMU_MOD_H_RAM1_2,       // 23
    HAL_CMU_MOD_H_RAM0,         // 24
    HAL_CMU_MOD_H_ROM0,         // 25
    HAL_CMU_MOD_H_BT_DUMP,      // 26
    HAL_CMU_MOD_H_WF_DUMP,      // 27
    HAL_CMU_MOD_H_SDMMC,        // 28
    HAL_CMU_MOD_H_CHECKSUM,     // 29
    HAL_CMU_MOD_H_CRC,          // 30
    // PCLK/PRST
    HAL_CMU_MOD_P_CMU,          // 31
    HAL_CMU_MOD_P_WDT,          // 32
    HAL_CMU_MOD_P_TIMER0,       // 33
    HAL_CMU_MOD_P_TIMER1,       // 34
    HAL_CMU_MOD_P_TIMER2,       // 35
    HAL_CMU_MOD_P_I2C0,         // 36
    HAL_CMU_MOD_P_I2C1,         // 37
    HAL_CMU_MOD_P_SPI,          // 38
    HAL_CMU_MOD_P_SLCD,         // 39
    HAL_CMU_MOD_P_SPI_ITN,      // 40
    HAL_CMU_MOD_P_SPI_PHY,      // 41
    HAL_CMU_MOD_P_UART0,        // 42
    HAL_CMU_MOD_P_UART1,        // 43
    HAL_CMU_MOD_P_UART2,        // 44
    HAL_CMU_MOD_P_PCM,          // 45
    HAL_CMU_MOD_P_I2S0,         // 46
    HAL_CMU_MOD_P_SPDIF0,       // 47
    HAL_CMU_MOD_P_TQWF,         // 48
    HAL_CMU_MOD_P_TQA7,         // 49
    HAL_CMU_MOD_P_TRNG,         // 50
    HAL_CMU_MOD_P_SEC_ENG,      // 51
    HAL_CMU_MOD_P_TZC,          // 52
    // OCLK/ORST
    HAL_CMU_MOD_O_SLEEP,        // 53
    HAL_CMU_MOD_O_USB,          // 54
    HAL_CMU_MOD_O_USB32K,       // 55
    HAL_CMU_MOD_O_PSRAM1G,      // 56
    HAL_CMU_MOD_O_PSRAM200,     // 57
    HAL_CMU_MOD_O_FLASH,        // 58
    HAL_CMU_MOD_O_SDMMC,        // 59
    HAL_CMU_MOD_O_WDT,          // 60
    HAL_CMU_MOD_O_TIMER0,       // 61
    HAL_CMU_MOD_O_TIMER1,       // 62
    HAL_CMU_MOD_O_TIMER2,       // 63
    HAL_CMU_MOD_O_I2C0,         // 64
    HAL_CMU_MOD_O_I2C1,         // 65
    HAL_CMU_MOD_O_SPI,          // 66
    HAL_CMU_MOD_O_SLCD,         // 67
    HAL_CMU_MOD_O_SPI_ITN,      // 68
    HAL_CMU_MOD_O_SPI_PHY,      // 69
    HAL_CMU_MOD_O_UART0,        // 70
    HAL_CMU_MOD_O_UART1,        // 71
    HAL_CMU_MOD_O_UART2,        // 72
    HAL_CMU_MOD_O_PCM,          // 73
    HAL_CMU_MOD_O_I2S0,         // 74
    HAL_CMU_MOD_O_SPDIF0,       // 75
    HAL_CMU_MOD_O_I2S1,         // 76
    HAL_CMU_MOD_O_A7,           // 77
    HAL_CMU_MOD_O_TSF,          // 78
    HAL_CMU_MOD_O_WDT_AP,       // 79
    HAL_CMU_MOD_O_TIMER0_AP,    // 80
    HAL_CMU_MOD_O_TIMER1_AP,    // 81

    // AON ACLK/ARST
    HAL_CMU_AON_A_CMU,          // 82
    HAL_CMU_AON_A_GPIO,         // 83
    HAL_CMU_AON_A_GPIO_INT,     // 84
    HAL_CMU_AON_A_WDT,          // 85
    HAL_CMU_AON_A_PWM,          // 86
    HAL_CMU_AON_A_TIMER,        // 87
    HAL_CMU_AON_A_IOMUX,        // 88
    HAL_CMU_AON_A_SPIDPD,       // 89
    HAL_CMU_AON_A_APBC,         // 90
    HAL_CMU_AON_A_H2H_MCU,      // 91
    // AON OCLK/ORST
    HAL_CMU_AON_O_WDT,          // 92
    HAL_CMU_AON_O_TIMER,        // 93
    HAL_CMU_AON_O_GPIO,         // 94
    HAL_CMU_AON_O_PWM0,         // 95
    HAL_CMU_AON_O_PWM1,         // 96
    HAL_CMU_AON_O_PWM2,         // 97
    HAL_CMU_AON_O_PWM3,         // 98
    HAL_CMU_AON_O_IOMUX,        // 99
    HAL_CMU_AON_O_SLP32K,       // 100
    HAL_CMU_AON_O_SLP26M,       // 101
    HAL_CMU_AON_O_SPIDPD,       // 102
    HAL_CMU_AON_RESERVED0,      // 103
    // AON SUBSYS
    HAL_CMU_AON_A7,             // 104
    HAL_CMU_AON_A7CPU,          // 105
    HAL_CMU_AON_MCU,            // 106
    HAL_CMU_AON_CODEC,          // 107
    HAL_CMU_AON_WF,             // 108
    HAL_CMU_AON_BT,             // 109
    HAL_CMU_AON_MCUCPU,         // 110
    HAL_CMU_AON_WFCPU,          // 111
    HAL_CMU_AON_BTCPU,          // 112
    HAL_CMU_AON_GLOBAL,         // 113

    HAL_CMU_MOD_QTY,

    HAL_CMU_MOD_GLOBAL = HAL_CMU_AON_GLOBAL,
    HAL_CMU_MOD_BT = HAL_CMU_AON_BT,
    HAL_CMU_MOD_BTCPU = HAL_CMU_AON_BTCPU,
    HAL_CMU_MOD_WF = HAL_CMU_AON_WF,
    HAL_CMU_MOD_WFCPU = HAL_CMU_AON_WFCPU,

    HAL_CMU_MOD_P_PWM = HAL_CMU_AON_A_PWM,
    HAL_CMU_MOD_O_PWM0 = HAL_CMU_AON_O_PWM0,
    HAL_CMU_MOD_O_PWM1 = HAL_CMU_AON_O_PWM1,
    HAL_CMU_MOD_O_PWM2 = HAL_CMU_AON_O_PWM2,
    HAL_CMU_MOD_O_PWM3 = HAL_CMU_AON_O_PWM3,

    HAL_CMU_H_ICACHE = HAL_CMU_MOD_H_CACHE0,
    HAL_CMU_H_DCACHE = HAL_CMU_MOD_QTY,
    HAL_CMU_H_ICACHECP = HAL_CMU_MOD_H_CACHE1,
    HAL_CMU_H_DCACHECP = HAL_CMU_MOD_QTY,

    HAL_CMU_MOD_P_SPI_DPD = HAL_CMU_AON_A_SPIDPD,
    HAL_CMU_MOD_O_SPI_DPD = HAL_CMU_AON_O_SPIDPD,
    HAL_CMU_MOD_H_PSRAM = HAL_CMU_MOD_H_PSRAM200,
    HAL_CMU_MOD_O_PSRAM = HAL_CMU_MOD_O_PSRAM200,
    HAL_CMU_MOD_H_PSRAMUHS = HAL_CMU_MOD_H_PSRAM1G,
    HAL_CMU_MOD_O_PSRAMUHS = HAL_CMU_MOD_O_PSRAM1G,
};

enum HAL_CMU_CLOCK_OUT_ID_T {
    HAL_CMU_CLOCK_OUT_AON_32K           = 0x00,
    HAL_CMU_CLOCK_OUT_AON_26M           = 0x01,
    HAL_CMU_CLOCK_OUT_AON_52M           = 0x02,
    HAL_CMU_CLOCK_OUT_AON_DIG_52M       = 0x03,
    HAL_CMU_CLOCK_OUT_AON_DIG_104M      = 0x04,
    HAL_CMU_CLOCK_OUT_AON_PER           = 0x05,
    HAL_CMU_CLOCK_OUT_AON_USB           = 0x06,
    HAL_CMU_CLOCK_OUT_AON_DCDC          = 0x07,
    HAL_CMU_CLOCK_OUT_AON_CHCLK         = 0x08,
    HAL_CMU_CLOCK_OUT_AON_SPDIF0        = 0x09,
    HAL_CMU_CLOCK_OUT_AON_MCU           = 0x0A,
    HAL_CMU_CLOCK_OUT_AON_FLASH         = 0x0B,
    HAL_CMU_CLOCK_OUT_AON_SYS           = 0x0C,

    HAL_CMU_CLOCK_OUT_BT_32K            = 0x40,
    HAL_CMU_CLOCK_OUT_BT_SYS            = 0x41,
    HAL_CMU_CLOCK_OUT_BT_52M            = 0x42,
    HAL_CMU_CLOCK_OUT_BT_26MI           = 0x43,
    HAL_CMU_CLOCK_OUT_BT_13M            = 0x44,
    HAL_CMU_CLOCK_OUT_BT_12M            = 0x45,
    HAL_CMU_CLOCK_OUT_BT_ADC            = 0x46,
    HAL_CMU_CLOCK_OUT_BT_ADC2           = 0x47,
    HAL_CMU_CLOCK_OUT_BT_24M            = 0x48,
    HAL_CMU_CLOCK_OUT_BT_26M            = 0x49,

    HAL_CMU_CLOCK_OUT_MCU_32K           = 0x60,
    HAL_CMU_CLOCK_OUT_MCU_SYS           = 0x61,
    HAL_CMU_CLOCK_OUT_MCU_FLASH         = 0x62,
    HAL_CMU_CLOCK_OUT_MCU_USB           = 0x63,
    HAL_CMU_CLOCK_OUT_MCU_PCLK          = 0x64,
    HAL_CMU_CLOCK_OUT_MCU_I2S           = 0x65,
    HAL_CMU_CLOCK_OUT_MCU_PCM           = 0x66,
    HAL_CMU_CLOCK_OUT_MCU_SPDIF0        = 0x67,
    HAL_CMU_CLOCK_OUT_MCU_SDMMC         = 0x68,
    HAL_CMU_CLOCK_OUT_MCU_SPI2          = 0x69,
    HAL_CMU_CLOCK_OUT_MCU_SPI0          = 0x6A,
    HAL_CMU_CLOCK_OUT_MCU_SPI1          = 0x6B,

    HAL_CMU_CLOCK_OUT_CODEC_ADC_ANA     = 0x80,
    HAL_CMU_CLOCK_OUT_CODEC_CODEC       = 0x81,
    HAL_CMU_CLOCK_OUT_CODEC_IIR         = 0x82,
    HAL_CMU_CLOCK_OUT_CODEC_RS          = 0x83,
    HAL_CMU_CLOCK_OUT_CODEC_HCLK        = 0x84,
};

enum HAL_CMU_I2S_MCLK_ID_T {
    HAL_CMU_I2S_MCLK_PLLCODEC           = 0x00,
    HAL_CMU_I2S_MCLK_CODEC              = 0x01,
    HAL_CMU_I2S_MCLK_PLLIIR             = 0x02,
    HAL_CMU_I2S_MCLK_PLLRS              = 0x03,
    HAL_CMU_I2S_MCLK_PLLSPDIF0          = 0x04,
    HAL_CMU_I2S_MCLK_PLLPCM             = 0x05,
    HAL_CMU_I2S_MCLK_PER                = 0x06,
    HAL_CMU_I2S_MCLK_CLK_OUT            = 0x07,
};

enum HAL_CMU_LOW_SYS_FREQ_T {
    HAL_CMU_LOW_SYS_FREQ_NONE,
    HAL_CMU_LOW_SYS_FREQ_13M,
    HAL_CMU_LOW_SYS_FREQ_6P5M,
};

enum HAL_CMU_FREQ_T {
    HAL_CMU_FREQ_32K,
    HAL_CMU_FREQ_26M,
    HAL_CMU_FREQ_52M,
    HAL_CMU_FREQ_78M,
    HAL_CMU_FREQ_104M,
    HAL_CMU_FREQ_156M,
    HAL_CMU_FREQ_208M,
    HAL_CMU_FREQ_260M,
    HAL_CMU_FREQ_390M,
    HAL_CMU_FREQ_780M,

    HAL_CMU_FREQ_QTY
};
#define HAL_CMU_FREQ_T                      HAL_CMU_FREQ_T

enum HAL_CMU_PLL_T {
    HAL_CMU_PLL_USB = 0,
    HAL_CMU_PLL_AUD = HAL_CMU_PLL_USB,
    HAL_CMU_PLL_DDR,
    HAL_CMU_PLL_DSP,
    HAL_CMU_PLL_BB,
    HAL_CMU_PLL_BB_PSRAM,

    HAL_CMU_PLL_QTY
};
#define HAL_CMU_PLL_T                       HAL_CMU_PLL_T

enum HAL_CMU_PLL_USER_T {
    HAL_CMU_PLL_USER_SYS,
    HAL_CMU_PLL_USER_AUD,
    HAL_CMU_PLL_USER_USB,
    HAL_CMU_PLL_USER_FLASH,
    HAL_CMU_PLL_USER_PSRAM,
    HAL_CMU_PLL_USER_DSP,

    HAL_CMU_PLL_USER_QTY,
    HAL_CMU_PLL_USER_ALL = HAL_CMU_PLL_USER_QTY,
};
#define HAL_CMU_PLL_USER_T                  HAL_CMU_PLL_USER_T

void hal_cmu_low_sys_clock_set(enum HAL_CMU_LOW_SYS_FREQ_T freq);

int hal_cmu_low_sys_clock_enabled(void);

int hal_cmu_ddr_clock_enable();

void hal_cmu_codec_vad_clock_enable(uint32_t enabled);

uint32_t hal_cmu_get_aon_chip_id(void);

uint32_t hal_cmu_get_aon_revision_id(void);

void hal_cmu_dma_dsd_enable(void);

void hal_cmu_dma_dsd_disable(void);

void hal_cmu_cp_enable(uint32_t sp, uint32_t entry);

void hal_cmu_cp_disable(void);

uint32_t hal_cmu_cp_get_entry_addr(void);

void hal_cmu_wifi_clock_enable(void);

void hal_cmu_wifi_clock_disable(void);

void hal_cmu_wifi_reset_set(void);

void hal_cmu_wifi_reset_clear(void);
void hal_cmu_wifi_module_reset_clear(void);
void hal_cmu_wifi_cpu_reset_clear(void);
void hal_cmu_wifi_clock_cfg_80M(void);

void hal_cmu_dsp_clock_enable(void);

void hal_cmu_dsp_clock_disable(void);

void hal_cmu_dsp_reset_set(void);

void hal_cmu_dsp_reset_clear(void);

void hal_cmu_dsp_init_boot_reg(uint32_t entry);

void hal_cmu_dsp_start_cpu(void);

void hal_cmu_dsp_setup(void);

void hal_cmu_dsp_stop_cpu(void);

#ifdef __cplusplus
}
#endif

#endif


#ifndef _SCTRL_H_
#define _SCTRL_H_

//#define SCTRL_DEBUG

#ifdef SCTRL_DEBUG
#define SCTRL_PRT      os_printf
#else
#define SCTRL_PRT      os_null_printf
#endif

#define DPLL_DELAY_TIME_10US	              120
#define DPLL_DELAY_TIME_200US	              3400

#define SCTRL_BASE                            (0x00800000)

#define SCTRL_CHIP_ID                         (SCTRL_BASE + 00 * 4)
#define CHIP_ID_DEFAULT                          (0x7111)

#define SCTRL_DEVICE_ID                       (SCTRL_BASE + 01 * 4)
#define DEVICE_ID_DEFAULT                        (0x20150414)

#define SCTRL_CONTROL                         (SCTRL_BASE + 02 * 4)
#if (CFG_SOC_NAME == SOC_BK7221U)
#define FLASH_SPI_MUX_BIT                        (1 << 22)
#endif // (CFG_SOC_NAME == SOC_BK7221U)
#if (CFG_SOC_NAME != SOC_BK7231)
#define EFUSE_VDD25_EN                           (1 << 23)
#define PSRAM_VDDPAD_VOLT_POSI                   (20)
#define PSRAM_VDDPAD_VOLT_MASK                   (0x3)
#define FLASH_SCK_IOCAP_POSI                     (18)
#define FLASH_SCK_IOCAP_MASK                     (0x3)
#define QSPI_IO_VOLT_POSI                        (16)
#define QSPI_IO_VOLT_MASK                        (0x3)
#define BLE_RF_EN_BIT                            (1 << 15)
#endif // (CFG_SOC_NAME != SOC_BK7231)
#define DPLL_CLKDIV_RESET_BIT                    (1 << 14)
#define SDIO_CLK_INVERT_BIT                      (1 << 13)
#define MPIF_CLK_INVERT_BIT                      (1 << 12)
#define MAC_CLK480M_PWD_BIT                      (1 << 11)
#define MODEM_CLK480M_PWD_BIT                    (1 << 10)
#define HCLK_DIV2_EN_BIT                         (1 << 9)
#define FLASH_26M_MUX_BIT                        (1 << 8)

#define MCLK_DIV_MASK                            (0x0F)
#define MCLK_DIV_POSI                            (4)
#define MCLK_DIV_1                               (1)
#define MCLK_DIV_2                               (2)
#define MCLK_DIV_3                               (3)
#define MCLK_DIV_5                               (5)
#define MCLK_DIV_6                               (6)
#define MCLK_DIV_7                               (7)
#define MCLK_DIV_10                              (10)
#define MCLK_DIV_16                              (16)

#define MCLK_MUX_MASK                            (0x03)
#define MCLK_MUX_POSI                            (0)
#define MCLK_FIELD_DCO                           (0x0)
#define MCLK_FIELD_26M_XTAL                      (0x1)
#define MCLK_FIELD_DPLL                          (0x2)
#define MCLK_FIELD_LPO                           (0x3)

#define SCTRL_CLK_GATING                      (SCTRL_BASE + 3 * 4)
#define MAC_PI_CLKGATING_BIT                     (1 << 8)
#define MAC_PI_RX_CLKGATING_BIT                  (1 << 7)
#define MAC_PI_TX_CLKGATING_BIT                  (1 << 6)
#define MAC_PRI_CLKGATING_BIT                    (1 << 5)
#define MAC_CRYPT_CLKGATING_BIT                  (1 << 4)
#define MAC_CORE_TX_CLKGATING_BIT                (1 << 3)
#define MAC_CORE_RX_CLKGATING_BIT                (1 << 2)
#define MAC_WT_CLKGATING_BIT                     (1 << 1)
#define MAC_MPIF_CLKGATING_BIT                   (1 << 0)

#define SCTRL_RESET                           (SCTRL_BASE + 4 * 4)
#define SCTRL_RESET_MASK                         (0x7F)
#if (CFG_SOC_NAME != SOC_BK7231)
#define BLE_SUBSYS_RESET                         (1 << 8)
#endif // (CFG_SOC_NAME != SOC_BK7231)
#define MAC_WAKEUP_ARM                           (1 << 7)
#define MODEM_CORE_RESET_BIT                     (1 << 6)
#define TL410_EXT_WAIT_BIT                       (1 << 5)
#define USB_SUBSYS_RESET_BIT                     (1 << 4)
#define TL410_BOOT_BIT                           (1 << 3)
#define MAC_SUBSYS_RESET_BIT                     (1 << 2)
#define DSP_SUBSYS_RESET_BIT                     (1 << 1)
#define MODEM_SUBCHIP_RESET_BIT                  (1 << 0)

#define SCTRL_FLASH_DELAY                     (SCTRL_BASE + 7 * 4)
#define FLASH_DELAY_CYCLE_POSI                   (0)
#define FLASH_DELAY_CYCLE_MASK                   (0xFFF)

#define SCTRL_MODEM_SUBCHIP_RESET_REQ         (SCTRL_BASE + 8 * 4)
#define MODEM_SUBCHIP_RESET_WORD                 (0x7111e802)

#define SCTRL_MAC_SUBSYS_RESET_REQ            (SCTRL_BASE + 9 * 4)
#define MAC_SUBSYS_RESET_WORD                    (0x7111C802)

#define SCTRL_USB_SUBSYS_RESET_REQ            (SCTRL_BASE + 10 * 4)
#define USB_SUBSYS_RESET_WORD                    (0x7111C12B)

#define SCTRL_DSP_SUBSYS_RESET_REQ            (SCTRL_BASE + 11 * 4)
#define DSP_SUBSYS_RESET_WORD                    (0x7111C410)

#define SCTRL_MODEM_CORE_RESET_PHY_HCLK       (SCTRL_BASE + 12 * 4)
#define MODEM_CORE_RESET_POSI                    (16)
#define MODEM_CORE_RESET_MASK                    (0xFFFF)
#define MODEM_CORE_RESET_WORD                    (0xE802U)
#define MAC_HCLK_EN_BIT                          (1 << 1)
#define PHY_HCLK_EN_BIT                          (1 << 0)

#define SCTRL_DEBUG13                         (SCTRL_BASE + 13 * 4)
#define DBGMACCLKSEL_POSI                        (20)
#define DBGMACCLKSEL_MASK                        (0x1)
#define SMPSOURCE_POSI                           (16)
#define SMPSOURCE_MASK                           (0xF)
#define DBGMDMBANK1MUX_POSI                      (8)
#define DBGMDMBANK1MUX_MASK                      (0x1F)
#define DBGMDMBANK0MUX_POSI                      (02)
#define DBGMDMBANK0MUX_MASK                      (0x1F)

#define SCTRL_DEBUG14_MODEM_BANK              (SCTRL_BASE + 14 * 4)
#define SCTRL_DEBUG15_MAC_BANK                (SCTRL_BASE + 15 * 4)


#define SCTRL_ANALOG_SPI                      (SCTRL_BASE + 16 * 4)
#define ANA_SPI_FREQ_DIV_POSI                    (26)
#define ANA_SPI_FREQ_DIV_MASK                    (0x3F)
#if (CFG_SOC_NAME == SOC_BK7231)
#define ANA_SPI_STATE_POSI                       (0)
#define ANA_SPI_STAET_MASK                       (0x1F)
#elif (CFG_SOC_NAME == SOC_BK7231U)
#define ANA_SPI_STATE_POSI                       (0)
#define ANA_SPI_STAET_MASK                       (0x7F)
#elif (CFG_SOC_NAME == SOC_BK7221U)
#define ANA_SPI_STATE_POSI                       (0)
#define ANA_SPI_STAET_MASK                       (0x7FF)
#endif

#define SCTRL_LA_SAMPLE                       (SCTRL_BASE + 18 * 4)
#define LA_SMP_LEN_POSI                          (16)
#define LA_SMP_LEN_MASK                          (0xFFFF)
#define LA_SMP_FINISH_BIT                        (1 << 3)
#define LA_SMP_CLK_INVERT                        (1 << 2)
#define LA_SMP_MODE_POSI                         (0)
#define LA_SMP_MODE_MASK                         (0x3)

#define SCTRL_LA_SAMPLE_VALUE                 (SCTRL_BASE + 19 * 4)
#define SCTRL_LA_SAMPLE_MASK                  (SCTRL_BASE + 20 * 4)
#define SCTRL_LA_SAMPLE_DMA_START_ADDR        (SCTRL_BASE + 21 * 4)

#define SCTRL_ANALOG_CTRL0                    (SCTRL_BASE + 22 * 4)
#define SPI_TRIG_BIT                             (1 << 19)
#define SPI_DET_EN                               (1 << 4)

#define SCTRL_ANALOG_CTRL1                    (SCTRL_BASE + 23 * 4)
#define DIV_BYPASS_BIT                           (1 << 31)
#define SPI_RST_BIT                              (1 << 25)
#define DCO_CNTI_POSI                            (16)
#define DCO_CNTI_MASK                            (0x1FFU)
#define DCO_TRIG_BIT                             (1 << 15)
#define DCO_DIV_POSI                             (12)
#define DCO_DIV_MASK                             (0x3U)

#define SCTRL_ANALOG_CTRL2                    (SCTRL_BASE + 24 * 4)
#if (CFG_SOC_NAME != SOC_BK7231)
#define XTALH_CTUNE_POSI                         (2)
#define XTALH_CTUNE_MASK                         (0x3FU)
#endif // (CFG_SOC_NAME == SOC_BK7231)

#define SCTRL_ANALOG_CTRL3                    (SCTRL_BASE + 25 * 4)

#define SCTRL_ANALOG_CTRL4                    (SCTRL_BASE + 26 * 4)
#define VSEL_SYS_LDO_POSI                        (27)
#define VSEL_SYS_LDO_MASK                        (0x3)

#define SCTRL_ANALOG_CTRL5                    (SCTRL_BASE + 0x1B*4)

#if (CFG_SOC_NAME != SOC_BK7231)
#define SCTRL_ANALOG_CTRL6                    (SCTRL_BASE + 0x1C*4)
#define DPLL_CLK_FOR_AUDIO_EN                    (1 << 31)
#define DPLL_CLK_FOR_USB_EN                      (1 << 30)
#define DPLL_DIVIDER_CLK_SEL                     (1 << 29)
#define DPLL_DIVIDER_CTRL_MASK                   (0x7)
#define DPLL_DIVIDER_CTRL_POSI                   (26)
#define DPLL_CLKOUT_PAD_EN                       (1 << 25)
#define DPLL_XTAL26M_CLK_AUDIO_EN                (1 << 24)
#define DPLL_REF_CLK_SELECT                      (1 << 23)
#define DPLL_CHARGE_PUMP_CUR_CTRL_MASK           (0x7)
#define DPLL_CHARGE_PUMP_CUR_CTRL_POSI           (20)
#define DPLL_DPLL_VCO_BAND_MANUAL_EN             (1 << 19)
#define DPLL_SPI_TRIGGER                         (1 << 18)
#define DPLL_CLK_REF_LOOP_SEL                    (1 << 17)
#define DPLL_KVCO_CTRL_MASK                      (0x3)
#define DPLL_KVCO_CTRL_POSI                      (15)
#define DPLL_VSEL_CAL                            (1 << 14)
#define DPLL_RP_CTRL_LPF_MASK                    (0x7)
#define DPLL_RP_CTRL_LPF_POSI                    (11)

#define SCTRL_EFUSE_CTRL                      (SCTRL_BASE + 0x1D*4)
#define EFUSE_OPER_EN                            (1 << 0)
#define EFUSE_OPER_DIR                           (1 << 1)
#define EFUSE_OPER_ADDR_POSI                     (8)
#define EFUSE_OPER_ADDR_MASK                     (0x1F)
#define EFUSE_OPER_WR_DATA_POSI                  (16)
#define EFUSE_OPER_WR_DATA_MASK                  (0xFF)

#define SCTRL_EFUSE_OPTR                      (SCTRL_BASE + 0x1E*4)
#define EFUSE_OPER_RD_DATA_POSI                  (0)
#define EFUSE_OPER_RD_DATA_MASK                  (0xFF)
#define EFUSE_OPER_RD_DATA_VALID                 (1 << 8)


#define SCTRL_DMA_PRIO_VAL                    (SCTRL_BASE + 0x1F*4)

#define SCTRL_BLE_SUBSYS_RESET_REQ            (SCTRL_BASE + 0x20*4)
#if (CFG_SOC_NAME == SOC_BK7221U)
#define SCTRL_CHARGE_STATUS                   (SCTRL_BASE + 0x21*4)

#define SCTRL_ANALOG_CTRL7                    (SCTRL_BASE + 0x22*4)

#define SCTRL_ANALOG_CTRL8                    (SCTRL_BASE + 0x23*4)
#define LINE_IN_EN                               (1 << 24)
#define LINE_IN_GAIN_MASK                        (0x3)
#define LINE_IN_GAIN_POSI                        (22)
#define SPI_PWD_AUD_ADC_L                        (1 << 21)
#define SPI_PWD_AUD_ADC_R                        (1 << 20)
#define AUD_DAC_GAIN_MASK                        (0x1F)
#define AUD_DAC_GAIN_POSI                        (2)
#define AUD_DAC_MUTE_EN                          (1 << 0)

#define SCTRL_ANALOG_CTRL9                    (SCTRL_BASE + 0x24*4)
#define DAC_DIFF_EN                              (1 << 31)
#define EN_AUD_DAC_L                             (1 << 30)
#define EN_AUD_DAC_R                             (1 << 29)
#define DAC_PA_OUTPUT_EN                         (1 << 24)
#define DAC_DRIVER_OUTPUT_EN                     (1 << 23)
#define AUD_DAC_DGA_EN                           (1 << 6)

#define SCTRL_ANALOG_CTRL10                   (SCTRL_BASE + 0x25*4)
#define DAC_N_END_OUPT_L                         (1 << 8)
#define DAC_N_END_OUPT_R                         (1 << 7)
#define DAC_VSEL_MASK                            (0x3)
#define DAC_VSEL_POSI                            (1)

#endif // (CFG_SOC_NAME == SOC_BK7221U)
#endif // (CFG_SOC_NAME != SOC_BK7221U)

#define SCTRL_LOW_PWR_CLK                     (SCTRL_BASE + 64 * 4)
#define LPO_CLK_MUX_POSI                         (0)
#define LPO_CLK_MUX_MASK                         (0x3)
#define LPO_SRC_ROSC                             (0x0)
#define LPO_SRC_32K_XTAL                         (0x1)
#define LPO_SRC_32K_DIV                          (0x2)

#define SCTRL_SLEEP                           (SCTRL_BASE + 65 * 4)
#define PROCORE_DLY_POSI                            (20)
#define PROCORE_DLY_MASK                            (0xF)
#define DCO_PWD_SLEEP_BIT                        (1 << 18)
#define FLASH_PWD_SLEEP_BIT                      (1 << 17)
#define ROSC_PWD_DEEPSLEEP_BIT                   (1 << 16)
#define SLEEP_MODE_POSI                          (0)
#define SLEEP_MODE_MASK                          (0xFFFF)
#define SLEEP_MODE_CFG_NORMAL_VOL_WORD           (0x4F89)
#define SLEEP_MODE_CFG_LOW_VOL_WORD              (0xB706)
#define SLEEP_MODE_CFG_DEEP_WORD                 (0xADC1)

#define SCTRL_DIGTAL_VDD                      (SCTRL_BASE + 66 * 4)
#define DIG_VDD_ACTIVE_POSI                      (4)
#define DIG_VDD_ACTIVE_MASK                      (0x7)
#define DIG_VDD_SLEEP_POSI                       (0)
#define DIG_VDD_SLEEP_MASK                       (0x7)
#define VDD_1_DOT_51                             (0x7)
#define VDD_1_DOT_38                             (0x6)
#define VDD_1_DOT_25                             (0x5)
#define VDD_1_DOT_12                             (0x4)
#define VDD_0_DOT_99                             (0x3)
#define VDD_0_DOT_86                             (0x2)
#define VDD_0_DOT_73                             (0x1)
#define VDD_0_DOT_60                             (0x0)

#define SCTRL_PWR_MAC_MODEM                   (SCTRL_BASE + 67 * 4)
#define MAC_PWD_POSI                             (16)
#define MAC_PWD_MASK                             (0xFFFFU)
#define MODEM_PWD_POSI                           (0)
#define MODEM_PWD_MASK                           (0xFFFFU)
#define MAC_PWD                                  (0xD802U)
#define MODEM_PWD                                (0xD802U)
#define MAC_PWU                                  (0x0001U)
#define MODEM_PWU                                (0x0001U)

#define SCTRL_DSP_PWR                         (SCTRL_BASE + 68 * 4)
#define DSP_PWD_POSI                             (0)
#define DSP_PWD_MASK                             (0xFFFF)
#define DSP_PWD                                  (0xD410)
#define DSP_PWU                                  (0x0001)

#define SCTRL_USB_PWR                         (SCTRL_BASE + 69 * 4)
#define BLE_PWD_POSI                             (16)
#define BLE_PWD_MASK                             (0xFFFF)
#define BLE_PWD                                  (0xDB1E)
#define BLE_PWU                                  (0x0001)
#define USB_PWD_POSI                             (0)
#define USB_PWD_MASK                             (0xFFFF)
#define USB_PWD                                  (0xD12B)
#define USB_PWU                                  (0x0001)

#define SCTRL_PMU_STATUS                      (SCTRL_BASE + 70 * 4)
#define PMU_MAC_POWER_DOWN_BIT                   (1 << 3)
#define PMU_MODEM_POWER_DOWN_BIT                 (1 << 2)
#define PMU_DSP_POWER_DOWN_BIT                   (1 << 1)
#define PMU_USB_POWER_DOWN_BIT                   (1 << 0)

#define SCTRL_ROSC_TIMER                      (SCTRL_BASE + 71 * 4)
#define ROSC_TIMER_PERIOD_POSI                   (16)
#define ROSC_TIMER_PERIOD_MASK                   (0xFFFF)
#define ROSC_TIMER_INT_STATUS_BIT                (1 << 8)
#define ROSC_TIMER_ENABLE_BIT                    (1 << 0)

#define SCTRL_GPIO_WAKEUP_EN                  (SCTRL_BASE + 72 * 4)
#define SCTRL_GPIO_WAKEUP_TYPE                (SCTRL_BASE + 73 * 4)
#define SCTRL_GPIO_WAKEUP_INT_STATUS          (SCTRL_BASE + 74 * 4)
#define GPIO_WAKEUP_ENABLE_FLAG                  (1)
#define GPIO_WAKEUP_TYPE_POSITIVE_EDGE           (0)
#define GPIO_WAKEUP_TYPE_NEGATIVE_EDGE           (1)

enum
{
    SYSCTRL_GPIO00_ID = 0,
    SYSCTRL_GPIO01_ID,
    SYSCTRL_GPIO02_ID,
    SYSCTRL_GPIO03_ID,
    SYSCTRL_GPIO04_ID,
    SYSCTRL_GPIO05_ID,
    SYSCTRL_GPIO06_ID,
    SYSCTRL_GPIO07_ID,
    SYSCTRL_GPIO08_ID,
    SYSCTRL_GPIO09_ID,
    SYSCTRL_GPIO10_ID,
    SYSCTRL_GPIO11_ID,
    SYSCTRL_GPIO12_ID,
    SYSCTRL_GPIO13_ID,
    SYSCTRL_GPIO14_ID,
    SYSCTRL_GPIO15_ID,
    SYSCTRL_GPIO16_ID,
    SYSCTRL_GPIO17_ID,
    SYSCTRL_GPIO18_ID,
    SYSCTRL_GPIO19_ID,
    SYSCTRL_GPIO20_ID,
    SYSCTRL_GPIO21_ID,
    SYSCTRL_GPIO22_ID,
    SYSCTRL_GPIO23_ID,
    SYSCTRL_GPIO24_ID,
    SYSCTRL_GPIO25_ID,
    SYSCTRL_GPIO26_ID,
    SYSCTRL_GPIO27_ID,
    SYSCTRL_GPIO28_ID,
    SYSCTRL_GPIO29_ID,
    SYSCTRL_GPIO30_ID,
    SYSCTRL_GPIO31_ID
};


#define SCTRL_BLOCK_EN_CFG                    (SCTRL_BASE + 75 * 4)
#define BLOCK_EN_WORD_POSI                       (20)
#define BLOCK_EN_WORD_MASK                       (0xFFFUL)
#define BLOCK_EN_WORD_PWD                        (0xA5CUL)

#define BLOCK_EN_VALID_MASK                      (0xFFFFF)

#define BLK_EN_LINEIN                            (1 << 19)
#define BLK_EN_MIC_R_CHANNEL                     (1 << 18)
#define BLK_EN_MIC_L_CHANNEL                     (1 << 17)
#define BLK_EN_AUDIO_R_CHANNEL                   (1 << 16)
#define BLK_EN_AUDIO_L_CHANNEL                   (1 << 15)
#define BLK_EN_NC1                               (1 << 14)
#define BLK_EN_SARADC                            (1 << 13)
#define BLK_EN_TEMPRATURE_SENSOR                 (1 << 12)
#define BLK_EN_26M_XTAL_LOW_POWER                (1 << 11)
#define BLK_EN_XTAL2RF                           (1 << 10)
#define BLK_EN_IO_LDO_LOW_POWER                  (1 << 09)
#define BLK_EN_ANALOG_SYS_LDO                    (1 << 8)
#define BLK_EN_DIGITAL_CORE_LDO_LOW_POWER        (1 << 07)
#define BLK_EN_NC0                               (1 << 06)
#define BLK_EN_DPLL_480M                         (1 << 05)
#define BLK_EN_32K_XTAL                          (1 << 04)
#define BLK_EN_26M_XTAL                          (1 << 03)
#define BLK_EN_ROSC32K                           (1 << 02)
#define BLK_EN_DCO                               (1 << 01)
#define BLK_EN_FLASH                             (1 << 00)


#define SCTRL_BIAS                            (SCTRL_BASE + 76 * 4)
#define BIAS_CAL_OUT_POSI                        (16)
#define BIAS_CAL_OUT_MASK                        (0x1F)
#define LDO_VAL_MANUAL_POSI                      (8)
#define LDO_VAL_MANUAL_MASK                      (0x1F)
#define BIAS_CAL_MANUAL_BIT                      (1 << 4)
#define BIAS_CAL_TRIGGER_BIT                     (1 << 0)


#define SCTRL_ROSC_CAL                        (SCTRL_BASE + 77 * 4)
#define ROSC_CAL_MANU_FIN_POSI                   (22)
#define ROSC_CAL_MANU_FIN_MASK                   (0x1FF)
#define ROSC_CAL_MANU_CIN_POSI                   (17)
#define ROSC_CAL_MANU_CIN_MASK                   (0x1F)
#define ROSC_CAL_MANU_EN_BIT                     (1 << 16)
#define ROSC_CAL_INTVAL_POSI                     (4)
#define ROSC_CAL_INTVAL_MASK                     (0x7)
#define ROSC_CAL_MODE_BIT                        (1 << 2)
#define ROSC_CAL_TRIG_BIT                        (1 << 1)
#define ROSC_CAL_EN_BIT                          (1 << 0)

#define SCTRL_BLOCK_EN_MUX                     (SCTRL_BASE + 79 * 4)


#define DCO_CNTI_120M           (0x127U)  // set DCO out clk with 120M

__inline static UINT32 sctrl_analog_get(UINT32 address)
{
	while(REG_READ(SCTRL_ANALOG_SPI) & (ANA_SPI_STAET_MASK<<ANA_SPI_STATE_POSI));
	return REG_READ(address);                      
}

__inline static void sctrl_analog_set(UINT32 address, UINT32 value)
{
	REG_WRITE(address, value);
	while(REG_READ(SCTRL_ANALOG_SPI) & (ANA_SPI_STAET_MASK<<ANA_SPI_STATE_POSI));
}

typedef struct  sctrl_ps_save_values{
UINT32    peri_clk_cfg;
UINT32    int_enable_cfg;
}SCTRL_PS_SAVE_VALUES;

#define PS_CLOSE_PERI_CLK            0

/*******************************************************************************
* Function Declarations
*******************************************************************************/
extern UINT32 sctrl_ctrl(UINT32 cmd, void *parm);
extern void sctrl_sub_reset(void);
#endif // _SCTRL_H_
// eof


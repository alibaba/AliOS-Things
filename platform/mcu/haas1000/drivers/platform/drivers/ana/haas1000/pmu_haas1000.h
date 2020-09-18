/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __PMU_HAAS1000_H__
#define __PMU_HAAS1000_H__

#ifdef __cplusplus
extern "C" {
#endif

#define ISPI_PMU_REG(reg)                   (((reg) & 0xFFF) | 0x0000)
#define ISPI_ANA_REG(reg)                   (((reg) & 0xFFF) | 0x1000)
#define ISPI_RF_REG(reg)                    (((reg) & 0xFFF) | 0x2000)
#define ISPI_WFRF_REG(reg)                  (((reg) & 0xFFF) | 0x3000)

#define ana_read(reg,val)               hal_analogif_reg_read(ISPI_ANA_REG(reg),val)
#define ana_write(reg,val)              hal_analogif_reg_write(ISPI_ANA_REG(reg),val)
#define rf_read(reg,val)                hal_analogif_reg_read(ISPI_RF_REG(reg),val)
#define rf_write(reg,val)               hal_analogif_reg_write(ISPI_RF_REG(reg),val)
#define wfrf_read(reg,val)              hal_analogif_reg_read(ISPI_WFRF_REG(reg),val)
#define wfrf_write(reg,val)             hal_analogif_reg_write(ISPI_WFRF_REG(reg),val)

enum PMU_EFUSE_PAGE_T {
    PMU_EFUSE_PAGE_SECURITY     = 0,
    PMU_EFUSE_PAGE_BOOT         = 1,
    PMU_EFUSE_PAGE_FEATURE      = 2,
    PMU_EFUSE_PAGE_BATTER_LV    = 3,

    PMU_EFUSE_PAGE_BATTER_HV    = 4,
    PMU_EFUSE_PAGE_SW_CFG       = 5,
    PMU_EFUSE_PAGE_DCDC_CALI    = 6,
    PMU_EFUSE_PAGE_RESERVED_7   = 7,

    PMU_EFUSE_PAGE_RESERVED_8   = 8,
    PMU_EFUSE_PAGE_DCCALIB2_L   = 9,
    PMU_EFUSE_PAGE_DCCALIB2_R   = 10,
    PMU_EFUSE_PAGE_DCCALIB_L    = 11,

    PMU_EFUSE_PAGE_DCCALIB_R    = 12,
    PMU_EFUSE_PAGE_DCXO         = 13,
    PMU_EFUSE_PAGE_KEY_INFO1    = 14,
    PMU_EFUSE_PAGE_KEY_INFO2    = 15,
};

enum PMU_PLL_DIV_TYPE_T {
    PMU_PLL_DIV_DIG,
    PMU_PLL_DIV_CODEC,
    PMU_PLL_DIV_PSRAM,
};

enum PMU_IRQ_TYPE_T {
    PMU_IRQ_TYPE_GPADC,
    PMU_IRQ_TYPE_RTC,
    PMU_IRQ_TYPE_CHARGER,
    PMU_IRQ_TYPE_USB_PIN,
    PMU_IRQ_TYPE_GPIO,
    PMU_IRQ_TYPE_WDT,

    PMU_IRQ_TYPE_QTY
};

uint8_t pmu_gpio_setup_irq(enum HAL_GPIO_PIN_T pin, const struct HAL_GPIO_IRQ_CFG_T *cfg);

void pmu_codec_hppa_enable(int enable);

void pmu_codec_mic_bias_enable(uint32_t map);

void pmu_pll_div_reset_set(enum HAL_CMU_PLL_T pll);

void pmu_pll_div_reset_clear(enum HAL_CMU_PLL_T pll);

void pmu_pll_div_set(enum HAL_CMU_PLL_T pll, enum PMU_PLL_DIV_TYPE_T type, uint32_t div);

void pmu_led_uart_enable(enum HAL_IOMUX_PIN_T pin);

void pmu_led_uart_disable(enum HAL_IOMUX_PIN_T pin);

void pmu_wdt_set_force_reboot(uint32_t force);

int pmu_set_security_value_to_efuse(void);

void pmu_reboot_hook();

void pmu_vcore_set_high_volt();

void pmu_vcore_set_normal_volt();

#ifdef __cplusplus
}
#endif

#endif


/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __HAL_IOMUX_H__
#define __HAL_IOMUX_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "plat_types.h"
#include "plat_addr_map.h"
#include CHIP_SPECIFIC_HDR(hal_iomux)

enum HAL_IOMUX_OP_TYPE_T {
    HAL_IOMUX_OP_KEEP_OTHER_FUNC_BIT = 0,
    HAL_IOMUX_OP_CLEAN_OTHER_FUNC_BIT,
};

enum HAL_IOMUX_PIN_VOLTAGE_DOMAINS_T {
    HAL_IOMUX_PIN_VOLTAGE_VIO = 0,
    HAL_IOMUX_PIN_VOLTAGE_MEM,
    // For PMU LED pins only
    HAL_IOMUX_PIN_VOLTAGE_VBAT,
};

enum HAL_IOMUX_PIN_PULL_SELECT_T {
    HAL_IOMUX_PIN_NOPULL = 0,
    HAL_IOMUX_PIN_PULLUP_ENALBE,
    HAL_IOMUX_PIN_PULLDOWN_ENALBE,
};

enum HAL_PWRKEY_IRQ_T {
    HAL_PWRKEY_IRQ_NONE             = 0,
    HAL_PWRKEY_IRQ_FALLING_EDGE     = (1 << 0),
    HAL_PWRKEY_IRQ_RISING_EDGE      = (1 << 1),
    HAL_PWRKEY_IRQ_BOTH_EDGE        = (HAL_PWRKEY_IRQ_FALLING_EDGE | HAL_PWRKEY_IRQ_RISING_EDGE),
};

struct HAL_IOMUX_PIN_FUNCTION_MAP {
    enum HAL_IOMUX_PIN_T pin;
    enum HAL_IOMUX_FUNCTION_T function;
    enum HAL_IOMUX_PIN_VOLTAGE_DOMAINS_T volt;
    enum HAL_IOMUX_PIN_PULL_SELECT_T pull_sel;
};

uint32_t hal_iomux_check(const struct HAL_IOMUX_PIN_FUNCTION_MAP *map, uint32_t count);
uint32_t hal_iomux_init(const struct HAL_IOMUX_PIN_FUNCTION_MAP *map, uint32_t count);
uint32_t hal_iomux_set_function(enum HAL_IOMUX_PIN_T pin, enum HAL_IOMUX_FUNCTION_T func, enum HAL_IOMUX_OP_TYPE_T type);
enum HAL_IOMUX_FUNCTION_T hal_iomux_get_function(enum HAL_IOMUX_PIN_T pin);
uint32_t hal_iomux_set_io_voltage_domains(enum HAL_IOMUX_PIN_T pin, enum HAL_IOMUX_PIN_VOLTAGE_DOMAINS_T volt);
uint32_t hal_iomux_set_io_pull_select(enum HAL_IOMUX_PIN_T pin, enum HAL_IOMUX_PIN_PULL_SELECT_T pull_sel);

void hal_iomux_set_default_config(void);
void hal_iomux_set_uart0_voltage(enum HAL_IOMUX_PIN_VOLTAGE_DOMAINS_T volt);
void hal_iomux_set_uart1_voltage(enum HAL_IOMUX_PIN_VOLTAGE_DOMAINS_T volt);
void hal_iomux_set_uart2_voltage(enum HAL_IOMUX_PIN_VOLTAGE_DOMAINS_T volt);
bool hal_iomux_uart0_connected(void);
bool hal_iomux_uart1_connected(void);
void hal_iomux_set_uart0(void);
void hal_iomux_set_uart1(void);
void hal_iomux_set_uart2(void);
void hal_iomux_set_analog_i2c(void);
void hal_iomux_set_analog_i2c_master_slave(void);
void hal_iomux_set_jtag(void);
void hal_iomux_set_sdmmc_dt_n_out_group(int enable);
void hal_iomux_set_i2s0(void);
void hal_iomux_set_i2s1(void);
void hal_iomux_set_spdif0(void);
void hal_iomux_set_spdif1(void);
void hal_iomux_set_dig_mic_clock_pin(enum HAL_IOMUX_PIN_T pin);
void hal_iomux_set_dig_mic_data0_pin(enum HAL_IOMUX_PIN_T pin);
void hal_iomux_set_dig_mic_data1_pin(enum HAL_IOMUX_PIN_T pin);
void hal_iomux_set_dig_mic_data2_pin(enum HAL_IOMUX_PIN_T pin);
void hal_iomux_set_dig_mic(uint32_t map);
void hal_iomux_set_spi(void);
void hal_iomux_set_spilcd(void);
void hal_iomux_set_i2c0(void);
void hal_iomux_set_i2c1(void);
void hal_iomux_set_clock_out(void);
void hal_iomux_set_clock_12m(void);
void hal_iomux_clear_clock_12m(void);
void hal_iomux_set_bt_tport(void);
void hal_iomux_set_bt_rf_sw(int rx_on, int tx_on);

void hal_iomux_ispi_access_init(void);
enum HAL_IOMUX_ISPI_ACCESS_T hal_iomux_ispi_access_enable(enum HAL_IOMUX_ISPI_ACCESS_T access);
enum HAL_IOMUX_ISPI_ACCESS_T hal_iomux_ispi_access_disable(enum HAL_IOMUX_ISPI_ACCESS_T access);

int hal_pwrkey_set_irq(enum HAL_PWRKEY_IRQ_T type);
bool hal_pwrkey_pressed(void);
bool hal_pwrkey_startup_pressed(void);
enum HAL_PWRKEY_IRQ_T hal_pwrkey_get_irq_state(void);

#ifdef __cplusplus
}
#endif

#endif

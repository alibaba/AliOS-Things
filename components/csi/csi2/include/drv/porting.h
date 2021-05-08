/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     drv/porting.h
 * @brief    Header File for SOC Porting
 * @version  V1.0
 * @date     8. Apr 2020
 * @model    porting
 ******************************************************************************/

#ifndef _DRV_PORTING_H_
#define _DRV_PORTING_H_

#include <stdint.h>
#include <stdbool.h>
#include <drv/common.h>

/**
  \brief       Soc get device frequence.
  \param[in]   idx      Device index
  \return      frequence
*/
uint32_t soc_get_apb_freq(uint32_t idx);
uint32_t soc_get_ahb_freq(uint32_t idx);
uint32_t soc_get_cpu_freq(uint32_t idx);
uint32_t soc_get_uart_freq(uint32_t idx);
uint32_t soc_get_spi_freq(uint32_t idx);
uint32_t soc_get_iic_freq(uint32_t idx);
uint32_t soc_get_i2s_freq(uint32_t idx);
uint32_t soc_get_pwm_freq(uint32_t idx);
uint32_t soc_get_adc_freq(uint32_t idx);
uint32_t soc_get_qspi_freq(uint32_t idx);
uint32_t soc_get_usi_freq(uint32_t idx);
uint32_t soc_get_timer_freq(uint32_t idx);
uint32_t soc_get_rtc_freq(uint32_t idx);
uint32_t soc_get_wdt_freq(uint32_t idx);
uint32_t soc_get_sdio_freq(uint32_t idx);
uint32_t soc_get_coretim_freq(void);
uint32_t soc_get_cur_cpu_freq(void);

/**
  \brief       Soc get device frequence.
  \param[in]   freq     CPU frequence
  \return      none
*/
void soc_set_sys_freq(uint32_t freq);

/*
  \brief       Soc enable device clock
  \param[in]   module   Clock module, defined in sys_clk.h, \ref clk_module_t
  \return      none
*/
void soc_clk_enable(int32_t module);

/*
  \brief       Soc disable device clock
  \param[in]   module   Clock module, defined in sys_clk.h, \ref clk_module_t
  \return      none
*/
void soc_clk_disable(int32_t module);

/*
  \brief       Get CPU ID
  \return      CPU ID, the val is 0, 1, 2...
*/
uint32_t soc_get_cpu_id(void);

/**
  \brief       SOC Dcache clean & invalid by range.
  \return      None
*/
void soc_dcache_clean_invalid_range(unsigned long addr, uint32_t size);

/**
  \brief       SOC Dcache clean & invalid all.
  \return      None
*/
void soc_dcache_clean_invalid_all(void);

/**
  \brief       SOC Dcache invalid by range.
  \return      None
*/
void soc_dcache_invalid_range(unsigned long addr, uint32_t size);

/**
  \brief       SOC Dcache clean all.
  \return      None
*/
void soc_dcache_clean(void);

/**
  \brief       SOC Icache invalid all.
  \return      None
*/
void soc_icache_invalid(void);

/**
  \brief       SOC dma address remap.
  \return      Remaped address
*/
extern unsigned long soc_dma_address_remap(unsigned long addr);


#ifdef CONFIG_PM
/**
  \brief       SoC enter low-power mode, each chip's implementation is different
               called by csi_pm_enter_sleep
  \param[in]   mode        low-power mode
  \return      Error code
*/
csi_error_t soc_pm_enter_sleep(csi_pm_mode_t mode);

/**
  \brief       SoC the wakeup source.
  \param[in]   wakeup_num  Wakeup source num
  \param[in]   enable      Flag control the wakeup source is enable or not
  \return      Error code
*/
csi_error_t soc_pm_config_wakeup_source(uint32_t wakeup_num, bool enable);
#endif

#endif /* _DRV_PORTING_H_ */

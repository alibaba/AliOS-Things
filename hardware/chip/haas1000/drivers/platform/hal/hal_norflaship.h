/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __HAL_NORFLASHIP_H__
#define __HAL_NORFLASHIP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "plat_types.h"

#if (CHIP_FLASH_CTRL_VER <= 1)
#define NORFLASHIP_RXFIFO_SIZE                  8
#define NORFLASHIP_TXFIFO_SIZE                  256
#else
#define NORFLASHIP_RXFIFO_SIZE                  8
#define NORFLASHIP_TXFIFO_SIZE                  16

#if !defined(CHIP_BEST2300)
#define NORFLASHIP_HAS_SECURITY
#endif

#if (defined(CHIP_BEST2300) || defined(CHIP_BEST1400))
#define NORFLASHIP_REMAP_NUM                    0
#else
#define NORFLASHIP_HAS_IDLE_IO_CTRL
#define NORFLASHIP_HAS_REMAP
#define NORFLASHIP_REMAP_NUM                    4
#endif
#endif

#define REMAP_SECTOR_SIZE                       0x1000

uint8_t norflaship_continuous_read_mode_bit(uint8_t mode_bit);

uint8_t norflaship_continuous_read_off(void);

uint8_t norflaship_continuous_read_on(void);

uint32_t norflaship_write_txfifo(const uint8_t *val, uint32_t len);

#if (CHIP_FLASH_CTRL_VER <= 1)
uint32_t norflaship_v1_write_txfifo_safe(const uint8_t *val, uint32_t len);

uint32_t norflaship_v1_write_txfifo_all(const uint8_t *val, uint32_t len);
#endif

uint8_t norflaship_read_rxfifo_count(void);

uint8_t norflaship_read_rxfifo(void);

void norflaship_blksize(uint32_t blksize);

void norflaship_cmd_addr(uint8_t cmd, uint32_t address);

void norflaship_ext_tx_cmd(uint8_t cmd, uint32_t tx_len);

void norflaship_ext_rx_cmd(uint8_t cmd, uint32_t tx_len, uint32_t rx_len);

void norflaship_cmd_done(void);

void norflaship_rxfifo_count_wait(uint8_t cnt);

void norflaship_rxfifo_empty_wait(void);

void norflaship_busy_wait(void);

int norflaship_is_busy(void);

void norflaship_clear_fifos(void);

void norflaship_clear_rxfifo(void);

void norflaship_clear_txfifo(void);

void norflaship_div(uint32_t div);

uint32_t norflaship_get_div(void);

void norflaship_cmdquad(uint32_t v);

uint32_t norflaship_get_pos_neg(void);

void norflaship_pos_neg(uint32_t v);

uint32_t norflaship_get_neg_phase(void);

void norflaship_neg_phase(uint32_t v);

uint32_t norflaship_get_samdly(void);

void norflaship_samdly(uint32_t v);

void norflaship_dual_mode(uint32_t v);

void norflaship_hold_pin(uint32_t v);

void norflaship_wpr_pin(uint32_t v);

void norflaship_quad_mode(uint32_t v);

void norflaship_dummyclc(uint32_t v);

void norflaship_dummyclcen(uint32_t v);

void norflaship_addrbyte4(uint32_t v);

void norflaship_ruen(uint32_t v);

void norflaship_rden(uint32_t v);

void norflaship_dualiocmd(uint32_t v);

void norflaship_rdcmd(uint32_t v);

void norflaship_frdcmd(uint32_t v);

void norflaship_qrdcmd(uint32_t v);

uint32_t norflaship_get_rdcmd(void);

void norflaship_set_idle_io_dir(uint32_t v);

void norflaship_sleep(void);

void norflaship_wakeup(void);

void norflaship_dec_index(uint32_t idx);

void norflaship_dec_saddr(uint32_t addr);

void norflaship_dec_eaddr(uint32_t addr);

void norflaship_dec_enable(void);

void norflaship_dec_disable(void);

void norflaship_man_wrap_width(uint32_t width);

void norflaship_man_wrap_enable(void);

void norflaship_man_wrap_disable(void);

void norflaship_auto_wrap_cmd(uint32_t cmd);

void norflaship_man_mode_enable(void);

void norflaship_man_mode_disable(void);

int norflaship_config_remap_section(uint32_t id, uint32_t addr, uint32_t len, uint32_t new_addr);

void norflaship_enable_remap(void);

void norflaship_disable_remap(void);

int norflaship_get_remap_status(void);

void norflaship_dummy_others(uint32_t v);

void norflaship_fetch_disable();

#ifdef __cplusplus
}
#endif

#endif

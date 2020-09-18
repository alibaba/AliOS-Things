/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef DRV_NORFLASH_H
#define DRV_NORFLASH_H

#include "plat_types.h"
#include "hal_norflash.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NORFLASH_ID_LEN                     3

enum DRV_NORFLASH_ERASE_T {
#ifdef PUYA_FLASH_ERASE_PAGE_ENABLE
    DRV_NORFLASH_ERASE_PAGE,
#endif
    DRV_NORFLASH_ERASE_SECTOR,
    DRV_NORFLASH_ERASE_BLOCK,
    DRV_NORFLASH_ERASE_CHIP,
};

enum DRV_NORFLASH_W_STATUS_T {
    DRV_NORFLASH_W_STATUS_INIT,
    DRV_NORFLASH_W_STATUS_QE,
    DRV_NORFLASH_W_STATUS_LB,
    DRV_NORFLASH_W_STATUS_BP,
};

enum SPEED_RATIO_T {
    SPEED_RATIO_1_EIGHTH,
    SPEED_RATIO_2_EIGHTH,
    SPEED_RATIO_3_EIGHTH,
    SPEED_RATIO_4_EIGHTH,
    SPEED_RATIO_5_EIGHTH,
    SPEED_RATIO_6_EIGHTH,
    SPEED_RATIO_7_EIGHTH,
    SPEED_RATIO_8_EIGHTH,
};

union DRV_NORFLASH_SPEED_RATIO_T {
    struct {
        uint8_t std_read    :3;
        uint8_t others      :3;
        uint8_t reserved    :2;
    } s;
    uint8_t v;
};

union DRV_NORFLASH_SEC_REG_CFG_T {
    struct {
        uint16_t enabled    :1;
        uint16_t base       :2;
        uint16_t size       :2;
        uint16_t offset     :2;
        uint16_t cnt        :2;
        uint16_t pp         :2;
        uint16_t lb         :2;
        uint16_t reserved   :3;
    } s;
    uint16_t v;
};

typedef int (*NORFLASH_WRITE_STATUS_CB)(enum DRV_NORFLASH_W_STATUS_T type, uint32_t param);

struct NORFLASH_CFG_T {
    const uint8_t id[NORFLASH_ID_LEN];
    const union DRV_NORFLASH_SPEED_RATIO_T speed_ratio;
    const uint8_t crm_en_bits;
    const uint8_t crm_dis_bits;
    const uint16_t block_protect_mask;
    const union DRV_NORFLASH_SEC_REG_CFG_T sec_reg_cfg;
    const uint16_t page_size;
    const uint16_t sector_size;
    const uint32_t block_size;
    const uint32_t total_size;
    const uint32_t max_speed;
    const uint32_t mode;
    const NORFLASH_WRITE_STATUS_CB write_status;
};

uint8_t norflash_read_status_s0_s7(void);

uint8_t norflash_read_status_s8_s15(void);

void norflash_status_WEL_0_wait(void);

enum HAL_NORFLASH_RET_T norflash_status_WIP_1_wait(int suspend);

uint32_t norflash_get_supported_mode(void);

uint32_t norflash_get_current_mode(void);

union DRV_NORFLASH_SEC_REG_CFG_T norflash_get_security_register_config(void);

uint32_t norflash_get_block_protect_mask(void);

void norflash_reset(void);

int norflash_get_size(uint32_t *total_size, uint32_t *block_size, uint32_t *sector_size, uint32_t *page_size);

int norflash_set_mode(uint32_t op);

int norflash_pre_operation(void);

int norflash_post_operation(void);

int norflash_read_reg(uint8_t cmd, uint8_t *val, uint32_t len);

int norflash_read_reg_ex(uint8_t cmd, uint8_t *param, uint32_t param_len, uint8_t *val, uint32_t len);

int norflash_write_reg(uint8_t cmd, const uint8_t *val, uint32_t len);

int norflash_init_sample_delay_by_div(uint32_t div);

void norflash_set_sample_delay(uint32_t index);

int norflash_sample_delay_calib(void);

int norflash_init_div(const struct HAL_NORFLASH_CONFIG_T *cfg);

int norflash_match_chip(const uint8_t *id, uint32_t len);

int norflash_get_id(uint8_t *value, uint32_t len);

int norflash_get_unique_id(uint8_t *value, uint32_t len);

enum HAL_NORFLASH_RET_T norflash_erase(uint32_t start_address, enum DRV_NORFLASH_ERASE_T type, int suspend);

enum HAL_NORFLASH_RET_T norflash_erase_resume(int suspend);

enum HAL_NORFLASH_RET_T norflash_write(uint32_t start_address, const uint8_t *buffer, uint32_t len, int suspend);

enum HAL_NORFLASH_RET_T norflash_write_resume(int suspend);

int norflash_suspend_check_irq(uint32_t irq_num);

int norflash_read(uint32_t start_address, uint8_t *buffer, uint32_t len);

void norflash_sleep(void);

void norflash_wakeup(void);

int norflash_init_status(uint32_t status);

int norflash_set_block_protection(uint32_t bp);

int norflash_security_register_lock(uint32_t id);

enum HAL_NORFLASH_RET_T norflash_security_register_erase(uint32_t start_address);

enum HAL_NORFLASH_RET_T norflash_security_register_write(uint32_t start_address, const uint8_t *buffer, uint32_t len);

int norflash_security_register_read(uint32_t start_address, uint8_t *buffer, uint32_t len);

uint32_t norflash_security_register_enable_read(void);

void norflash_security_register_disable_read(uint32_t mode);

#ifdef __cplusplus
}
#endif

#endif /* DRV_NORFLASH_H */

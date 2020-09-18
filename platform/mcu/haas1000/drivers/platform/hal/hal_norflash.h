/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef NORFLASH_HAL_H
#define NORFLASH_HAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "plat_types.h"
#include "hal_cmu.h"

#define HAL_NORFLASH_DEVICE_ID_LEN          3

#define HAL_NORFLASH_UNIQUE_ID_LEN          16
#define FLASH_SECTOR_SIZE_IN_BYTES          4096
#define FLASH_BLOCK_SIZE_IN_BYTES           (32*1024)

enum HAL_NORFLASH_ID_T {
    HAL_NORFLASH_ID_0 = 0,
    HAL_NORFLASH_ID_NUM,
};

enum HAL_NORFLASH_RET_T {
    HAL_NORFLASH_OK,
    HAL_NORFLASH_SUSPENDED,
    HAL_NORFLASH_ERR,
    HAL_NORFLASH_BAD_ID,
    HAL_NORFLASH_BAD_DIV,
    HAL_NORFLASH_BAD_DIV_VERIF,
    HAL_NORFLASH_BAD_CFG,
    HAL_NORFLASH_BAD_OP,
    HAL_NORFLASH_BAD_CALIB,
    HAL_NORFLASH_BAD_ADDR,
    HAL_NORFLASH_BAD_LEN,
    HAL_NORFLASH_NOT_OPENED,
};

enum HAL_NORFLASH_SPEED {
    HAL_NORFLASH_SPEED_13M  = 13000000,
    HAL_NORFLASH_SPEED_26M  = 26000000,
    HAL_NORFLASH_SPEED_52M  = 52000000,
    HAL_NORFLASH_SPEED_78M  = 78000000,
    HAL_NORFLASH_SPEED_104M = 104000000,
    HAL_NORFLASH_SPEED_130M = 130000000,
    HAL_NORFLASH_SPEED_156M = 156000000,
    HAL_NORFLASH_SPEED_182M = 182000000,
    HAL_NORFLASH_SPEED_208M = 208000000,
    HAL_NORFLASH_SPEED_234M = 234000000,
};

enum HAL_NORFLASH_OP_MODE {
    // Different groups can be used together, different flash-device may support different option(s)

    // (1) basic read mode
    /* standard spi mode */
    HAL_NORFLASH_OP_MODE_STAND_SPI              = 0x1,
    /* fast spi mode*/
    HAL_NORFLASH_OP_MODE_FAST_SPI               = 0x2,
     /* dual mode */
    HAL_NORFLASH_OP_MODE_DUAL_OUTPUT            = 0x4,
    /* dual mode */
    HAL_NORFLASH_OP_MODE_DUAL_IO                = 0x8,
    /* quad mode */
    HAL_NORFLASH_OP_MODE_QUAD_OUTPUT            = 0x10,
    /* quad mode */
    HAL_NORFLASH_OP_MODE_QUAD_IO                = 0x20,

    // (2) extend read mode
    // read accelerate (no cmd bettween read operation) :
    // may need Dual or Quad Mode
    HAL_NORFLASH_OP_MODE_CONTINUOUS_READ        = 0x40,
    // read high performance mode
    HAL_NORFLASH_OP_MODE_HIGH_PERFORMANCE       = 0x80,
    // read wrap mode
    HAL_NORFLASH_OP_MODE_READ_WRAP              = 0x100,

    // (3) program mode.
    // page program mode
    HAL_NORFLASH_OP_MODE_PAGE_PROGRAM           = 0x200,
    // dual program mode
    HAL_NORFLASH_OP_MODE_DUAL_PAGE_PROGRAM      = 0x400,
    // quad program mode
    HAL_NORFLASH_OP_MODE_QUAD_PAGE_PROGRAM      = 0x800,

    // (4) advanced features
    // suspend and resume
    HAL_NORFLASH_OP_MODE_SUSPEND                = 0x1000,
    // erase in standard spi mode
    HAL_NORFLASH_OP_MODE_ERASE_IN_STD           = 0x2000,

    HAL_NORFLASH_OP_MODE_RESERVED               = 0xFFFFFFFF,
};

struct HAL_NORFLASH_CONFIG_T {
    uint32_t source_clk;
    uint32_t speed;
    enum HAL_NORFLASH_OP_MODE mode;

    /* internal use : can be config if need to (useful for rom) */
    uint8_t override_config:1;
    uint8_t neg_phase:1;
    uint8_t pos_neg:1;
    uint8_t cmdquad:1;
    uint8_t samdly:3;
    uint8_t div; /* least 2 */
    uint8_t dualmode:1;
    uint8_t holdpin:1;
    uint8_t wprpin:1;
    uint8_t quadmode:1;

    uint8_t spiruen:3;
    uint8_t spirden:3;

    uint8_t dualiocmd;
    uint8_t rdcmd;
    uint8_t frdcmd;
    uint8_t qrdcmd; /* quad io cmd */
#if defined(CHIP_BEST1400) || defined(CHIP_BEST1402)
    uint8_t  dec_enable;  /* 1: enable decoder, 0: disable decoder */
    uint8_t  dec_idx;     /* decoder key index ,from 0 to 3 */
    uint32_t dec_addr;    /* start address where to decode */
    uint32_t dec_size;    /* bytes number will be decoded */
#endif
};

/* hal api */
void hal_norflash_set_freq(enum HAL_CMU_FREQ_T freq);
enum HAL_NORFLASH_RET_T hal_norflash_init(void);
enum HAL_NORFLASH_RET_T hal_norflash_deinit(void);
enum HAL_NORFLASH_RET_T hal_norflash_open(enum HAL_NORFLASH_ID_T id, const struct HAL_NORFLASH_CONFIG_T *cfg);
enum HAL_CMU_FREQ_T hal_norflash_clk_to_cmu_freq(uint32_t clk);
enum HAL_NORFLASH_RET_T hal_norflash_get_size(enum HAL_NORFLASH_ID_T id, uint32_t *total_size, uint32_t *block_size, uint32_t *sector_size, uint32_t *page_size);
enum HAL_NORFLASH_RET_T hal_norflash_get_boundary(enum HAL_NORFLASH_ID_T id, uint32_t address, uint32_t* block_boundary, uint32_t* sector_boundary);
enum HAL_NORFLASH_RET_T hal_norflash_get_id(enum HAL_NORFLASH_ID_T id, uint8_t *value, uint32_t len);
enum HAL_NORFLASH_RET_T hal_norflash_get_unique_id(enum HAL_NORFLASH_ID_T id, uint8_t *value, uint32_t len);
enum HAL_NORFLASH_RET_T hal_norflash_enable_protection(enum HAL_NORFLASH_ID_T id);
enum HAL_NORFLASH_RET_T hal_norflash_disable_protection(enum HAL_NORFLASH_ID_T id);
enum HAL_NORFLASH_RET_T hal_norflash_erase_chip(enum HAL_NORFLASH_ID_T id);
enum HAL_NORFLASH_RET_T hal_norflash_erase_suspend(enum HAL_NORFLASH_ID_T id, uint32_t start_address, uint32_t len, int suspend);
enum HAL_NORFLASH_RET_T hal_norflash_erase(enum HAL_NORFLASH_ID_T id, uint32_t start_address, uint32_t len);
enum HAL_NORFLASH_RET_T hal_norflash_erase_resume(enum HAL_NORFLASH_ID_T id, int suspend);
enum HAL_NORFLASH_RET_T hal_norflash_write_suspend(enum HAL_NORFLASH_ID_T id, uint32_t start_address, const uint8_t *buffer, uint32_t len, int suspend);
enum HAL_NORFLASH_RET_T hal_norflash_write(enum HAL_NORFLASH_ID_T id, uint32_t start_address, const uint8_t *buffer, uint32_t len);
enum HAL_NORFLASH_RET_T hal_norflash_write_resume(enum HAL_NORFLASH_ID_T id, int suspend);
enum HAL_NORFLASH_RET_T hal_norflash_suspend_check_irq(enum HAL_NORFLASH_ID_T id, uint32_t irq_num);
enum HAL_NORFLASH_RET_T hal_norflash_read(enum HAL_NORFLASH_ID_T id, uint32_t start_address, uint8_t *buffer, uint32_t len);
enum HAL_NORFLASH_RET_T hal_norflash_close(enum HAL_NORFLASH_ID_T id);
void hal_norflash_sleep(enum HAL_NORFLASH_ID_T id);
void hal_norflash_wakeup(enum HAL_NORFLASH_ID_T id);
int hal_norflash_busy(void);
uint32_t hal_norflash_get_flash_total_size(enum HAL_NORFLASH_ID_T id);
int hal_norflash_opened(enum HAL_NORFLASH_ID_T id);
enum HAL_NORFLASH_RET_T hal_norflash_get_open_state(enum HAL_NORFLASH_ID_T id);
enum HAL_NORFLASH_RET_T hal_norflash_security_register_lock(enum HAL_NORFLASH_ID_T id, uint32_t start_address, uint32_t len);
enum HAL_NORFLASH_RET_T hal_norflash_security_register_erase(enum HAL_NORFLASH_ID_T id, uint32_t start_address, uint32_t len);
enum HAL_NORFLASH_RET_T hal_norflash_security_register_write(enum HAL_NORFLASH_ID_T id, uint32_t start_address, const uint8_t *buffer, uint32_t len);
enum HAL_NORFLASH_RET_T hal_norflash_security_register_read(enum HAL_NORFLASH_ID_T id, uint32_t start_address, uint8_t *buffer, uint32_t len);
enum HAL_NORFLASH_RET_T hal_norflash_enable_remap(enum HAL_NORFLASH_ID_T id, uint32_t addr, uint32_t len, uint32_t offset);
enum HAL_NORFLASH_RET_T hal_norflash_disable_remap(enum HAL_NORFLASH_ID_T id);
enum HAL_NORFLASH_RET_T hal_norflash_re_enable_remap(enum HAL_NORFLASH_ID_T id);
int hal_norflash_get_remap_status(enum HAL_NORFLASH_ID_T id);

#ifdef __cplusplus
}
#endif

#endif /* NORFLASH_HAL_H */

/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __MPU_H__
#define __MPU_H__

#include "plat_types.h"

#ifdef __cplusplus
extern "C" {
#endif

enum MPU_ID_T {
    MPU_ID_NULL_POINTER = 0,
    MPU_ID_1,
    MPU_ID_2,
    MPU_ID_3,
    MPU_ID_4,
    MPU_ID_5,
    MPU_ID_6,
    MPU_ID_7,

    MPU_ID_QTY,
};

/*mcu sections */
#define MPU_ID_USER_DATA_SECTION    MPU_ID_1
#define MPU_ID_FRAM_TEXT1           MPU_ID_2
#define MPU_ID_FRAM_TEXT2           MPU_ID_3

/*cp sections */
#define MPU_ID_CP_FLASHX            MPU_ID_2
#define MPU_ID_CP_FLASH             MPU_ID_3
#define MPU_ID_CP_FLASH_NC          MPU_ID_4

#define MPU_ID_CP_PERIPHERAL        MPU_ID_NULL_POINTER
#define MPU_ID_CP_SRAM              MPU_ID_1
#define MPU_ID_CP_SRAMX             MPU_ID_5

/*both mcu and cp section*/
#define MPU_ID_PSRAMUHS             MPU_ID_6
#define MPU_ID_PSRAMUHSX            MPU_ID_7

enum MPU_ATTR_T {
    MPU_ATTR_READ_WRITE_EXEC = 0,
    MPU_ATTR_READ_EXEC,
    MPU_ATTR_EXEC,
    MPU_ATTR_READ_WRITE,
    MPU_ATTR_READ,
    MPU_ATTR_NO_ACCESS,

    MPU_ATTR_QTY,
};

int mpu_open(void);

int mpu_close(void);

// VALID LENGTH: 32, 64, 128, 256, 512, 1K, 2K, ..., 4G
// ADDR must be aligned to len
// Note, srd_bits, mpu subregion bits, which can be divided to 8 sub regions
// per region, if don't need, always set the arguments to 0;
int mpu_set(enum MPU_ID_T id, uint32_t addr, uint32_t len, int srd_bits,
                                                    enum MPU_ATTR_T attr);

int mpu_clear(enum MPU_ID_T id);

int mpu_null_check_enable(void);

int mpu_fast_ram_protect(void);

void mpu_open_for_psramuhs();

int mpu_set_armv8_psramuhs(enum MPU_ID_T id, uint32_t addr, uint32_t len, enum MPU_ATTR_T attr);

#ifdef __cplusplus
}
#endif

#endif


/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef HAL_OTA_H
#define HAL_OTA_H

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "base.h"

typedef enum {
    OTA_KERNEL,
    OTA_APP,
    OTA_ALL
} OTA_ENUM_UPDATE_TYPE;

typedef enum {
    OTA_FINISH,
    OTA_BREAKPOINT
} OTA_ENUM_RESULT_TYPE;

enum ota_parti_e {
    OTA_PARTITION_KERNEL,
    OTA_PARTITION_APP,
    OTA_PARTITION_DEFAULT,
};

typedef enum {
    OTA_RAW,
    OTA_DIFF,
} OTA_ENUM_FIRMWARE_TYPE;

typedef struct  {
    OTA_ENUM_UPDATE_TYPE update_type;
    OTA_ENUM_RESULT_TYPE result_type;
    OTA_ENUM_FIRMWARE_TYPE  firmware_type;
    uint32_t splict_size;
    uint8_t diff_version;
    uint8_t boot_count;
} ota_finish_param_t;



typedef struct {
    uint32_t start_address; /* the address of the bin saved on flash. */
    uint32_t length;        /* file real length */
    uint8_t  version[8];
    uint8_t  type;          /* B:bootloader, P:boot_table, A:application, D: 8782 driver */
    uint8_t  upgrade_type;  /* u:upgrade */
    uint16_t crc;
    uint8_t  reserved[4];
} boot_table_t;

typedef struct hal_ota_module_s hal_ota_module_t;

typedef int hal_stat_t;

struct hal_ota_module_s {
    hal_module_base_t base;

    /* Link to HW */
    int (*init)(hal_ota_module_t *m, void *something);
    int (*ota_write)(hal_ota_module_t *m, volatile uint32_t *off_set,
                     uint8_t *in_buf , uint32_t in_buf_len);
    int (*ota_read)(hal_ota_module_t *m,  volatile uint32_t *off_set,
                    uint8_t *out_buf , uint32_t out_buf_len);
    int (*ota_set_boot)(hal_ota_module_t *m, void *something);
    int (*ota_rollback)(hal_ota_module_t *m, void *something);
};

/**
 * Arch register a new module before HAL startup
 */
void hal_ota_register_module(hal_ota_module_t *module);

/**
 * init ota partition
 *
 * @note   when ota start, maybe it need init something
 * @param  something  extra info for ota init
 *
 * @return  0 : On success, 1 : If an error occurred with any step
 */
hal_stat_t hal_ota_init(void *something);

/**
 * Write data to an area on ota partition
 *
 * @param  m           Refer the ota module which will be used,default module will be used if value is NULL
 * @param  off_set     Point to the start address that the data is written to, and
 *                     point to the last unwritten address after this function is
 *                     returned, so you can call this function serval times without
 *                     update this start address.
 * @param  inbuf       point to the data buffer that will be written to flash
 * @param  in_buf_len  The length of the buffer
 *
 * @return  0 : On success, 1 : If an error occurred with any step
 */
hal_stat_t hal_ota_write(hal_ota_module_t *m, volatile uint32_t *off_set,
                         uint8_t *in_buf , uint32_t in_buf_len);

/**
 * Read data from an area on ota Flash to data buffer in RAM
 *
 * @param  m            Refer the ota module which will be used,default module will be used if value is NULL
 * @param  off_set      Point to the start address that the data is read, and
 *                      point to the last unread address after this function is
 *                      returned, so you can call this function serval times without
 *                      update this start address.
 * @param  out_buf      Point to the data buffer that stores the data read from flash
 * @param  out_buf_len  The length of the buffer
 *
 * @return  0 : On success, 1 : If an error occurred with any step
 */
hal_stat_t hal_ota_read(hal_ota_module_t *m, volatile uint32_t *off_set,
                        uint8_t *out_buf, uint32_t out_buf_len);

/**
 * Set boot options when ota reboot
 *
 * @param  m          Refer the ota module which will be used,default module will be used if value is NULL
 * @param  something  boot parms
 *
 * @return  kNoErr : On success. kGeneralErr : If an error occurred with any step
 */
hal_stat_t hal_ota_set_boot(hal_ota_module_t *m, void *something);

/**
 * Set rollback when failed to boot
 *
 * @param  m          Refer the ota module which will be used,default module will be used if value is NULL
 * @param  something  boot parms
 *
 * @return  kNoErr : On success. kGeneralErr : If an error occurred with any step
 */
hal_stat_t hal_ota_rollback(hal_ota_module_t *m, void *something);

/**
 * Get the default ota module
 *
 * @return  the first registered ota module ,which is the head of module list
 */
hal_ota_module_t *hal_ota_get_default_module(void);

#ifdef __cplusplus
}
#endif

#endif /* HAL_OTA_H */


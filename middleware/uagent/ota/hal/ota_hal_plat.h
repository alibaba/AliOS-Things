/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef OTA_HAL_PLAT_H
#define OTA_HAL_PLAT_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    OTA_RAW    = 0,
    OTA_DIFF   = 1,
    OTA_KERNEL = 2,
    OTA_APP    = 4,
} OTA_UPD_TYPE_E;

typedef enum {
    OTA_FINISH,
    OTA_BREAKPOINT
} OTA_RES_TYPE_E;

typedef struct  {
    unsigned int   dst_adr;
    unsigned int   src_adr;
    unsigned int   len;
    unsigned short crc;
    unsigned int   upg_flag;
    unsigned char  boot_count;
    unsigned int   rec_size;
    unsigned int   splict_size;
    int            off_bp;               /*Break point offset*/
    OTA_RES_TYPE_E res_type; /*result type: OTA_FINISH, OTA_BREAKPOINT*/
    unsigned short param_crc; /*Parameter crc*/
} __attribute__((packed)) ota_boot_param_t;

typedef struct ota_hal_module_s {
    int (*init)(void *something);
    int (*write)(int *off_set, char *in_buf , int in_buf_len);
    int (*read)(int *off_set, char *out_buf , int out_buf_len);
    int (*boot)(void *something);
    int (*rollback)(void *something);
    const char *(*version)(unsigned char dev_type);
}ota_hal_module_t;

/**
 * Arch register a new module before HAL startup
 */
void ota_hal_register_module(ota_hal_module_t *module);

/**
 * init ota partition
 *
 * @note   when ota start, maybe it need init something
 * @param  something  extra info for ota init
 *
 * @return  0 : On success, 1 : If an error occurred with any step
 */
int ota_hal_init(void *something);

/**
 * Write data to an area on ota partition
 *
 * @param  off_set     Point to the start address that the data is written to, and
 *                     point to the last unwritten address after this function is
 *                     returned, so you can call this function serval times without
 *                     update this start address.
 * @param  inbuf       point to the data buffer that will be written to flash
 * @param  in_buf_len  The length of the buffer
 *
 * @return  0 : On success, 1 : If an error occurred with any step
 */
int ota_hal_write(int *off_set, char *in_buf, int in_buf_len);

/**
 * Read data from an area on ota Flash to data buffer in RAM
 *
 * @param  off_set      Point to the start address that the data is read, and
 *                      point to the last unread address after this function is
 *                      returned, so you can call this function serval times without
 *                      update this start address.
 * @param  out_buf      Point to the data buffer that stores the data read from flash
 * @param  out_buf_len  The length of the buffer
 *
 * @return  0 : On success, 1 : If an error occurred with any step
 */
int ota_hal_read(int *off_set, char *out_buf, int out_buf_len);

/**
 * Set boot options when ota reboot
 *
 * @param  something  boot parms
 *
 * @return  kNoErr : On success. kGeneralErr : If an error occurred with any step
 */
int ota_hal_boot(void *something);

/**
 * Set rollback when failed to boot
 *
 * @param  something
 *
 * @return  kNoErr : On success. kGeneralErr : If an error occurred with any step
 */
int ota_hal_rollback(void *something);


/**
 * get system version
 *
 * @param  device type
 *
 * @return  version
 */
const char *ota_hal_get_version(unsigned char dev_type);
#ifdef __cplusplus
}
#endif
#endif /* OTA_HAL_PLATFORM_H */


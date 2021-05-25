/** @defgroup ota_hal_api
 * @{
 *
 * This is an include file of OTA HAL interface.
 *
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */
#ifndef OTA_HAL_H
#define OTA_HAL_H
#include "ota_agent.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * ota_hal_init   ota hal init main OTA partition.
 *
 * @param[in] ota_boot_param_t *param ota parameter
 *
 * @return OTA_SUCCESS              OTA success.
 * @return OTA_UPGRADE_WRITE_FAIL   OTA upgrade write fail.
 * @return OTA_UPGRADE_PARAM_FAIL   OTA upgrade parameter fail.
 * @return OTA_UPGRADE_FW_SIZE_FAIL OTA upgrade FW too big.
 * @return OTA_UPGRADE_SET_BOOT_FAIL OTA upgrade set boot fail.
 */
int ota_hal_init(ota_boot_param_t *param);

/**
 * ota_hal_read   ota hal read main OTA partition.
 *
 * @param[in] unsigned int *off      read offset.
 * @param[in] char *buf              read buffer.
 * @param[in] unsigned int len       read buffer len.
 *
 * @return OTA_SUCCESS              OTA success.
 * @return OTA_UPGRADE_WRITE_FAIL   OTA upgrade write fail.
 * @return OTA_UPGRADE_PARAM_FAIL   OTA upgrade parameter fail.
 * @return OTA_UPGRADE_FW_SIZE_FAIL OTA upgrade FW too big.
 * @return OTA_UPGRADE_SET_BOOT_FAIL OTA upgrade set boot fail.
 */
int ota_hal_read(unsigned int *off, char *buf, unsigned int len);

/**
 * ota_hal_write   ota hal write main OTA partition.
 *
 * @param[in] unsigned int *off      write offset.
 * @param[in] char *buf              write buf.
 * @param[in] unsigned int len       write buf len.
 *
 * @return OTA_SUCCESS              OTA success.
 * @return OTA_UPGRADE_WRITE_FAIL   OTA upgrade write fail.
 * @return OTA_UPGRADE_PARAM_FAIL   OTA upgrade parameter fail.
 * @return OTA_UPGRADE_FW_SIZE_FAIL OTA upgrade FW too big.
 * @return OTA_UPGRADE_SET_BOOT_FAIL OTA upgrade set boot fail.
 */
int ota_hal_write(unsigned int *off, char *buf, unsigned int len);

/**
 * ota_hal_boot  ota hal boot.
 *
 * @param[in] ota_boot_param_t *param ota parameter
 *
 * @return OTA_SUCCESS              OTA success.
 * @return OTA_UPGRADE_WRITE_FAIL   OTA upgrade write fail.
 * @return OTA_UPGRADE_PARAM_FAIL   OTA upgrade parameter fail.
 * @return OTA_UPGRADE_FW_SIZE_FAIL OTA upgrade FW too big.
 * @return OTA_UPGRADE_SET_BOOT_FAIL OTA upgrade set boot fail.
 */
int ota_hal_boot(ota_boot_param_t *parm);

/**
 * ota_hal_reboot_bank  ota hal reboot banker
 *
 * @param[in] void
 *
 * @return OTA_SUCCESS              OTA success.
 * @return OTA_UPGRADE_WRITE_FAIL   OTA upgrade write fail.
 * @return OTA_UPGRADE_PARAM_FAIL   OTA upgrade parameter fail.
 * @return OTA_UPGRADE_FW_SIZE_FAIL OTA upgrade FW too big.
 * @return OTA_UPGRADE_SET_BOOT_FAIL OTA upgrade set boot fail.
 */
int ota_hal_reboot_bank(void);

/**
 * ota_hal_rollback ota hal rollback.
 *
 * @return OTA_SUCCESS              OTA success.
 * @return OTA_UPGRADE_WRITE_FAIL   OTA upgrade write fail.
 * @return OTA_UPGRADE_PARAM_FAIL   OTA upgrade parameter fail.
 * @return OTA_UPGRADE_FW_SIZE_FAIL OTA upgrade FW too big.
 * @return OTA_UPGRADE_SET_BOOT_FAIL OTA upgrade set boot fail.
 */
int ota_hal_rollback(void);

/**
 * @}
 */
#ifdef __cplusplus
}
#endif
#endif  /* OTA_HAL_H */

/*
 * @file ota_hal.h
 *
 * This is an include file of OTA HAL interface.
 *
 * @author    zixun
 * @version   V3.0
 * @date      2019-12-01
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#ifndef OTA_HAL_H
#define OTA_HAL_H
#include "ota_agent.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup aos_ota OTA
 *  OTA upgrade service.
 *
 *  @{
 */

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
 * ota_hal_version  ota hal get system vesion.
 *
 * @param[in] unsigned char dev_type device type: 0: main 1:sub.
 * @param[in] char *dn               device name.
 *
 * @return OTA_SUCCESS              OTA success.
 * @return OTA_UPGRADE_WRITE_FAIL   OTA upgrade write fail.
 * @return OTA_UPGRADE_PARAM_FAIL   OTA upgrade parameter fail.
 * @return OTA_UPGRADE_FW_SIZE_FAIL OTA upgrade FW too big.
 * @return OTA_UPGRADE_SET_BOOT_FAIL OTA upgrade set boot fail.
 */
const char *ota_hal_version(unsigned char dev_type, char *dn);

/**
 * ota_mcu_hal_init   ota mcu hal init MCU OTA partition.
 *
 * @return OTA_SUCCESS              OTA success.
 * @return OTA_MCU_INIT_FAIL         OTA MCU init fail.
 * @return OTA_MCU_VERSION_FAIL      OTA MCU version fail.
 * @return OTA_MCU_NOT_READY         OTA MCU not ready.
 * @return OTA_MCU_REBOOT_FAIL       OTA MCU fail to reboot.
 * @return OTA_MCU_HEADER_FAIL       OTA MCU header error.
 * @return OTA_MCU_UPGRADE_FAIL      OTA MCU upgrade fail.
 */
int ota_mcu_hal_init(void);

/**
 * ota_mcu_hal_read   ota mcu hal read MCU OTA partition.
 *
 * @param[in] unsigned int *off      read offset.
 * @param[in] char *buf              read buffer.
 * @param[in] unsigned int len       read buffer len.
 *
 * @return OTA_SUCCESS               OTA success.
 * @return OTA_MCU_INIT_FAIL         OTA MCU init fail.
 * @return OTA_MCU_VERSION_FAIL      OTA MCU version fail.
 * @return OTA_MCU_NOT_READY         OTA MCU not ready.
 * @return OTA_MCU_REBOOT_FAIL       OTA MCU fail to reboot.
 * @return OTA_MCU_HEADER_FAIL       OTA MCU header error.
 * @return OTA_MCU_UPGRADE_FAIL      OTA MCU upgrade fail.
 */
int ota_mcu_hal_read(unsigned int *off, unsigned char *buf, unsigned int len);

/**
 * ota_mcu_hal_write   ota mcu hal write MCU OTA partition.
 *
 * @param[in] unsigned int *off      write offset.
 * @param[in] char *buf              write buffer.
 * @param[in] unsigned int len       write buffer len.
 *
 * @return OTA_SUCCESS               OTA success.
 * @return OTA_MCU_INIT_FAIL         OTA MCU init fail.
 * @return OTA_MCU_VERSION_FAIL      OTA MCU version fail.
 * @return OTA_MCU_NOT_READY         OTA MCU not ready.
 * @return OTA_MCU_REBOOT_FAIL       OTA MCU fail to reboot.
 * @return OTA_MCU_HEADER_FAIL       OTA MCU header error.
 * @return OTA_MCU_UPGRADE_FAIL      OTA MCU upgrade fail.
 */
int ota_mcu_hal_write(unsigned int* off, unsigned char* buf, unsigned int len);

/**
 * ota_mcu_hal_version   ota hal get MCU version.
 *
 * @param[in] void *device           MCU device node/name.
 * @param[in] char *version          MCU device version.
 *
 * @return OTA_SUCCESS               OTA success.
 * @return OTA_MCU_INIT_FAIL         OTA MCU init fail.
 * @return OTA_MCU_VERSION_FAIL      OTA MCU version fail.
 * @return OTA_MCU_NOT_READY         OTA MCU not ready.
 * @return OTA_MCU_REBOOT_FAIL       OTA MCU fail to reboot.
 * @return OTA_MCU_HEADER_FAIL       OTA MCU header error.
 * @return OTA_MCU_UPGRADE_FAIL      OTA MCU upgrade fail.
 */
int ota_mcu_hal_version(void *device, char *version);

/**
 * ota_mcu_hal_recv   ota hal recv data from MCU.
 *
 * @param[in] void *device           MCU device node/name.
 * @param[in] char *buf              MCU device recv buffer.
 * @param[in] unsigned int len       MCU device recv buffer len.
 *
 * @return OTA_SUCCESS               OTA success.
 * @return OTA_MCU_INIT_FAIL         OTA MCU init fail.
 * @return OTA_MCU_VERSION_FAIL      OTA MCU version fail.
 * @return OTA_MCU_NOT_READY         OTA MCU not ready.
 * @return OTA_MCU_REBOOT_FAIL       OTA MCU fail to reboot.
 * @return OTA_MCU_HEADER_FAIL       OTA MCU header error.
 * @return OTA_MCU_UPGRADE_FAIL      OTA MCU upgrade fail.
 */
int ota_mcu_hal_recv(void* device, unsigned char *buf, unsigned int len);

/**
 * ota_mcu_hal_send   ota hal send data to MCU.
 *
 * @param[in] void *device           MCU device node/name.
 * @param[in] char *buf              MCU device send buffer.
 * @param[in] unsigned int len       MCU device send buffer len.
 *
 * @return OTA_SUCCESS               OTA success.
 * @return OTA_MCU_INIT_FAIL         OTA MCU init fail.
 * @return OTA_MCU_VERSION_FAIL      OTA MCU version fail.
 * @return OTA_MCU_NOT_READY         OTA MCU not ready.
 * @return OTA_MCU_REBOOT_FAIL       OTA MCU fail to reboot.
 * @return OTA_MCU_HEADER_FAIL       OTA MCU header error.
 * @return OTA_MCU_UPGRADE_FAIL      OTA MCU upgrade fail.
 */
int ota_mcu_hal_send(void* device, unsigned char *buf, unsigned int len);

/**
 * ota_mcu_upgrade_start   ota MCU upgrade start.
 *
 * @param[in] unsigned int size      MCU image size.
 * @param[in] char* ver              MCU image version.
 * @param[in] char* md5              MCU image md5.
 *
 * @return OTA_SUCCESS               OTA success.
 * @return OTA_MCU_INIT_FAIL         OTA MCU init fail.
 * @return OTA_MCU_VERSION_FAIL      OTA MCU version fail.
 * @return OTA_MCU_NOT_READY         OTA MCU not ready.
 * @return OTA_MCU_REBOOT_FAIL       OTA MCU fail to reboot.
 * @return OTA_MCU_HEADER_FAIL       OTA MCU header error.
 * @return OTA_MCU_UPGRADE_FAIL      OTA MCU upgrade fail.
 */
int ota_mcu_upgrade_start(unsigned int size, char* ver, char* md5);
/** @} */
#ifdef __cplusplus
}
#endif
#endif  /* OTA_HAL_H */

/*
 * @file ota_updater.h
 *
 * This is an include file of OTA updater to install new Firmware.
 *
 * @author    zixun
 * @version   V3.0
 * @date      2019-12-01
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#ifndef OTA_UPDATE_H
#define OTA_UPDATE_H
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
 *  ENUM: OTA Updater ERRNO.
 */
typedef enum {
     OTA_UPDATER_OK         = 0,
     OTA_NB_INVALID_PARAM   = -1,  /* ota patch invalid parameter */
     OTA_NB_HEADER_FAIL     = -2,  /* ota patch header fail */
     OTA_NB_MEMORY_FAIL     = -3,  /* ota patch memory fail */
     OTA_NB_READ_CTRL_FAIL  = -4,  /* ota patch read control fail */
     OTA_NB_READ_DIFF_FAIL  = -5,  /* ota patch read diff fail */
     OTA_NB_READ_OLD_FAIL   = -6,  /* ota patch read old fail */
     OTA_NB_READ_EXTRA_FAIL = -7,  /* ota patch read extra fail */
     OTA_NB_WRITE_DATA_FAIL = -8,  /* ota patch write data fail */
     OTA_NB_CRC_COMP_FAIL   = -9,  /* ota patch crc fail */
     OTA_XZ_PARAM_FAIL      = -10, /* ota XZ parameter fail */
     OTA_XZ_CRC_FAIL        = -11, /* ota XZ crc fail */
     OTA_XZ_UNCOMP_FAIL     = -12, /* ota XZ uncompress fail */
     OTA_XZ_VERIFY_FAIL     = -13, /* ota XZ verify fail */
     OTA_XZ_MEM_FAIL        = -14, /* ota XZ memory fail */
     OTA_YMODEM_UP_FAIL     = -15, /* ota Ymodem upgrade fail */
     OTA_USB_UP_FAIL        = -16, /* ota USB upgrade fail */
     OTA_IMAGE_CRC_FAIL     = -17, /* ota image crc fail */
     OTA_IMAGE_COPY_FAIL    = -18, /* ota image copy fail */
} OTA_UPDATER_E;

/**
 * ota_ymodem_upgrade   ymodem upgrade.
 *
 * @return OTA_SUCCESS               OTA success.
 * @return OTA_YMODEM_UP_FAIL        OTA Ymodem upgrade fail.
 */
int ota_ymodem_upgrade(void);

/**
 * ota_usb_upgrade   usb upgrade.
 *
 * @return OTA_SUCCESS               OTA success.
 * @return OTA_USB_UP_FAIL           OTA USB upgrade fail.
 */
int ota_usb_upgrade(void);

/**
 * ota_nbpatch_main  ota nbpatch main.
 *
 * @return OTA_SUCCESS            OTA success.
 * @return OTA_NB_INVALID_PARAM   OTA patch invalid parameter.
 * @return OTA_NB_HEADER_FAIL     OTA patch header fail.
 * @return OTA_NB_MEMORY_FAIL     OTA patch memory fail.
 * @return OTA_NB_READ_CTRL_FAIL  OTA patch read control fail.
 * @return OTA_NB_READ_DIFF_FAIL  OTA patch read diff fail.
 * @return OTA_NB_READ_OLD_FAIL   OTA patch read old fail.
 * @return OTA_NB_READ_EXTRA_FAIL OTA patch read extra fail.
 * @return OTA_NB_WRITE_DATA_FAIL OTA patch write data fail.
 * @return OTA_NB_CRC_COMP_FAIL   OTA patch crc fail.
 */
int ota_nbpatch_main(void);

/**
 * ota_xz_main   ota xz uncompress main.
 *
 * @return OTA_SUCCESS              OTA success.
 * @return OTA_XZ_PARAM_FAIL        OTA XZ parameter fail.
 * @return OTA_XZ_CRC_FAIL          OTA XZ crc fail.
 * @return OTA_XZ_UNCOMP_FAIL       OTA XZ uncompress fail.
 * @return OTA_XZ_VERIFY_FAIL       OTA XZ verify fail.
 * @return OTA_XZ_MEM_FAIL          OTA XZ memory fail.
 */
int ota_xz_main(void);

/**
 * ota_image_check   ota image crc check.
 *
 * @param[in] unsigned int addr     OTA check addr.
 * @param[in] unsigned int size     OTA image size.
 * @param[in] unsigned int crc      OTA image crc.
 *
 * @return OTA_SUCCESS              OTA success.
 * @return OTA_IMAGE_CRC_FAIL       OTA image crc fail.
 * @return OTA_IMAGE_COPY_FAIL      OTA image copy fail.
 */
int ota_image_check(unsigned int addr, unsigned int size, unsigned int crc);

/**
 * ota_image_copy   ota image copy.
 *
 * @param[in] unsigned int addr     OTA image des addr.
 * @param[in] unsigned int src      OTA image src addr.
 * @param[in] unsigned int size     OTA image size.
 *
 * @return OTA_SUCCESS              OTA success.
 * @return OTA_IMAGE_CRC_FAIL       OTA image crc fail.
 * @return OTA_IMAGE_COPY_FAIL      OTA image copy fail.
 */
int ota_image_copy(unsigned int dst, unsigned int src, unsigned int size);

/**
 * ota_read_parameter   ota read parameter from flash.
 *
 * @param[in] ota_boot_param_t *param ota parameter
 *
 * @return OTA_SUCCESS              OTA success.
 * @return OTA_UPGRADE_WRITE_FAIL   OTA upgrade write fail.
 * @return OTA_UPGRADE_PARAM_FAIL   OTA upgrade parameter fail.
 * @return OTA_UPGRADE_FW_SIZE_FAIL OTA upgrade FW too big.
 * @return OTA_UPGRADE_SET_BOOT_FAIL OTA upgrade set boot fail.
 */
int ota_patch_read_param(ota_boot_param_t *param);

/**
 * ota_update_parameter  ota update parameter to flash.
 *
 * @param[in] ota_boot_param_t *param ota parameter
 *
 * @return OTA_SUCCESS              OTA success.
 * @return OTA_UPGRADE_WRITE_FAIL   OTA upgrade write fail.
 * @return OTA_UPGRADE_PARAM_FAIL   OTA upgrade parameter fail.
 * @return OTA_UPGRADE_FW_SIZE_FAIL OTA upgrade FW too big.
 * @return OTA_UPGRADE_SET_BOOT_FAIL OTA upgrade set boot fail.
 */
int ota_patch_write_param(ota_boot_param_t *param);

/** @} */
#ifdef __cplusplus
}
#endif
#endif

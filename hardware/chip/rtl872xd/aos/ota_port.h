/**
 * Copyright (C) 2019 Alibaba.inc, All rights reserved.
 *
 * @file:    ota_port.h
 * @brief:   vendor ota interface
 * @author:  
 * @date:    2019/12/16
 * @version: 1.0
 */
#ifndef _TG_VENDOR_OTA_H_
#define _TG_VENDOR_OTA_H_

#include "aos/hal/flash.h"

#ifdef __cplusplus
extern "C" {
#endif

#define OTA_BOOT_INFO_SIZE          0x1000
#define OTA_BOOT_INFO_HEAD_LEN      4
#define OTA_BOOT_INFO_BODY_LEN      44

#define OTA_LINK_NOTUSED            0
#define OTA_LINK_USED               1

#define OTA_OK                      0
#define OTA_FAILE                   1

typedef struct
{
	char *bin_name;
    hal_partition_t partition;
} ota_bin_partition;

enum bootinfo_zone {
    OTA_BOOTINFO_ZONEA,
    OTA_BOOTINFO_ZONEB,
    OTA_BOOTINFO_ZONEMAX
};

typedef enum {
    ALI_C2H,
    ALI_C4H,
    ALI_C5A01,
    ALI_C1H,
    ALI_ODM_MAX
}ODM_TYPE;

enum ota_link {
    OTA_LINK_ERR = -1,
    OTA_LINK_A,
    OTA_LINK_B,
    OTA_LINK_MAX
};

struct ota_boot_info {
    uint32_t  crc32;
    uint16_t  info_len;
    uint8_t   odm_type;
    uint8_t   reserved;
    uint16_t  linkA_used_flag;
    uint16_t  linkB_used_flag;
    uint16_t  update_link;
    uint8_t   crash_reboot_count;
	uint8_t   secureERR_reboot_count;
    uint16_t  reboot_count_max;
    uint16_t  reboot_reason;
    uint16_t  fallback_disable;

    uint16_t  reserved1; //Tool request struct four byte align
    uint32_t  boot2a_addr;
    uint32_t  boot2b_addr;
    uint32_t  rtosa_addr;
    uint32_t  rtosb_addr;
    uint32_t  boot2_len;
    uint32_t  rtos_len;
};

int tg_flash_read(const uint32_t addr, uint8_t *dst, const uint32_t size);

int ota_adapt_flash_write(const hal_partition_t partition, const uint32_t addr, const uint8_t *src, const uint32_t size);

int ota_set_bootinfo_to_zoneAB(struct ota_boot_info *info);

enum bootinfo_zone ota_get_valid_bootinfo_zone(void);

int ota_get_bootinfo(struct ota_boot_info *info, enum bootinfo_zone zone);

ODM_TYPE ota_adapt_get_odm_type(void);

int ota_adapt_set_odm_type(ODM_TYPE type);

int ota_upgrade_link(void);
int ota_set_user_bootinfo(void *param);

int ota_clear_reboot_count(void);

/**
 * @brief Get current active A/B slot
 *
 * @returns "a" if current slot is A, or "b" if current slot is B.
 */
const char* tg_ota_get_current_ab(void);

/**
 * @brief To mark that the non-active slot is upgraded successfully.
 * It's time to set A/B related flag to let system switch to new system until next reboot.
 * It MAY do two actions:
 * 1. To set flag to indicate system boot from non-active slot next time.
 * 2. To enable A/B fallback to old system if new system boot failed
 * @return: - 0: success; - -1: fail
 */
int tg_ota_upgrade_end(void);

/**
 * @brief To mark the ota process is finished and present slot is workable,
 * It called by genie sdk after main boot flow done
 * @param[in] ab_fallback: 1: enable ab fallback, if it is not enabled; 0, disable ab_fallback.
 */
void tg_ota_finish(int ab_fallback);

#ifdef __cplusplus
}
#endif

#endif /* _TG_VENDOR_OTA_H_ */

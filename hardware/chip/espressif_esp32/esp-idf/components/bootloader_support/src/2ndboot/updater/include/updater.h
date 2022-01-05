#ifndef OTA_UPDATE
#define OTA_UPDATE
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
// #include "libc.h"
#include "2ndboot.h"
#include "ota_updater.h"

#ifndef XZ_BUF_SIZE
#define XZ_BUF_SIZE 2048
#endif

/* OTA upgrade magic <--> upg_flag */
#define OTA_UPGRADE_ALL    0x9669 /* upgrade all image: kernel+framework+app */
#define OTA_UPGRADE_XZ     0xA55A /* upgrade xz compressed image */
#define OTA_UPGRADE_DIFF   0xB44B /* upgrade diff compressed image */
#define OTA_UPGRADE_KERNEL 0xC33C /* upgrade kernel image only */
#define OTA_UPGRADE_APP    0xD22D /* upgrade app image only */

enum ota_link {
    OTA_LINK_ERR = -1,
    OTA_LINK_A,
    OTA_LINK_B,
    OTA_LINK_MAX
};
enum bootinfo_zone { 
    OTA_BOOTINFO_ZONEA, 
    OTA_BOOTINFO_ZONEB, 
    OTA_BOOTINFO_ZONEMAX 
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
#define FLASH_SECTOR_SIZE_IN_BYTES          4096
#define OTA_BOOT_INFO_SIZE          0x1000
#define OTA_BOOT_INFO_HEAD_LEN      4
#define OTA_BOOT_INFO_BODY_LEN      44

#define OTA_LINK_NOTUSED            0
#define OTA_LINK_USED               1

#define OTA_OK                      0
#define OTA_FAILE                   1

int ota_check_bootinfo(enum bootinfo_zone zone);
void ota_set_zoneAB_bootinfo_to_default(void);
int ota_check_bootinfo(enum bootinfo_zone zone);
enum bootinfo_zone ota_get_valid_bootinfo_zone(void);
int ota_set_bootinfo(struct ota_boot_info *info, enum bootinfo_zone zone);
int ota_set_bootinfo_to_zoneAB(struct ota_boot_info *info);
int ota_get_bootinfo(struct ota_boot_info *info, enum bootinfo_zone zone);
int ota_set_bootinfo_crc32value(struct ota_boot_info *info);
void ota_show_bootinfo_zoneAB(void);
#endif

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
#endif

#ifndef OTA_UPDATE
#define OTA_UPDATE
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "libc.h"
#include "2ndboot.h"

#ifndef XZ_BUF_SIZE
#define XZ_BUF_SIZE 2048
#endif

/* OTA update errorno. */
typedef enum {
     OTA_UPDATER_OK       = 0,
     OTA_NB_PARAMS_ERR    = -1,
     OTA_NB_MEM_OP_FAIL   = -2,
     OTA_NB_FILE_OP_FAIL  = -3,
     OTA_NB_LZAM_OP_FAIL  = -4,
     OTA_NB_PATCH_OP_FAIL = -5,
     OTA_NB_CRC_OP_FAIL   = -6,
     OTA_NB_LZMA_HEAD_FAIL= -7,
     OTA_XZ_PARAM_FAIL    = -8,
     OTA_XZ_CRC_FAIL      = -9,
     OTA_XZ_UNCOMP_FAIL   = -10,
     OTA_XZ_VERIFY_FAIL   = -11,
     OTA_XZ_MEM_FAIL      = -12,
} OTA_UPDATER_E;

/* OTA upgrade magic <--> upg_flag */
#define OTA_UPGRADE_ALL    0x9669 /* upgrade all image: kernel+framework+app */
#define OTA_UPGRADE_XZ     0xA55A /* upgrade xz compressed image */
#define OTA_UPGRADE_DIFF   0xB44B /* upgrade diff compressed image */
#define OTA_UPGRADE_KERNEL 0xC33C /* upgrade kernel image only */
#define OTA_UPGRADE_APP    0xD22D /* upgrade app image only */

#define OTA_URL_LEN        256 /*OTA download url max len*/
#define OTA_HASH_LEN       66  /*OTA download file hash len*/
#define OTA_SIGN_LEN       256 /*OTA download file sign len*/
#define OTA_VER_LEN        64  /*OTA version string max len*/
typedef struct  {
    unsigned int   dst_adr;     /*Single Bank: Destination Address: APP partition.*/
    unsigned int   src_adr;     /*Single Bank: Copy from Source Address: OTA partition.*/
    unsigned int   len;         /*Single Bank: Download file len */
    unsigned short crc;         /*Single Bank: Download file CRC */
    unsigned short upg_flag;    /*Upgrade flag: OTA_UPGRADE_ALL OTA_UPGRADE_XZ OTA_UPGRADE_DIFF*/
    unsigned char  boot_count;  /*Boot count: When >=3 Rollback to old version in BL for dual-banker boot*/
             int   upg_status;  /*OTA upgrade status*/
    unsigned char  hash_type;   /*OTA download hash type*/
    char  url[OTA_URL_LEN];     /*OTA download url*/
    char  sign[OTA_SIGN_LEN];   /*OTA download file sign*/
    char  hash[OTA_HASH_LEN];   /*OTA download file hash*/
    char  ver[OTA_VER_LEN];     /*OTA get version*/
    unsigned int   old_size;    /*Diff upgrade: patch old data size*/
    unsigned short patch_num;   /*Diff upgrade: patch num*/
    unsigned short patch_status;/*Diff upgrade: patch status*/
    unsigned int   patch_off;   /*Diff upgrade: patch offset*/
    unsigned int   new_off;     /*Diff upgrade: patch new data offset*/
    unsigned int   new_size;    /*Diff upgrade: patch new data size*/
    unsigned char  reserve[18];
    unsigned short param_crc;   /*OTA Parameter crc*/
} __attribute__((aligned(16))) ota_boot_param_t;

/* OTA updater component: User APIs. call it to complete FW uprade in your bootloader */
int ota_updater(void);                                         /* ota updater main entry:ota upgrade check */
int ota_nbpatch_main(void);                                    /* ota nbpatch main enrty. */
int ota_xz_main(void);                                         /* ota xz uncompress main enrty */
int ymodem_upgrade(void);                                      /* local uart ymodem upgrade */
int ota_image_check(unsigned int addr, unsigned int size, unsigned int crc); /* ota raw image crc check */
int ota_image_copy(unsigned int dst, unsigned int src, unsigned int size);   /* ota raw image copy upgrade */
int ota_patch_read_param(ota_boot_param_t *ota_param);         /* ota patch read parameter */
int ota_patch_write_param(ota_boot_param_t *ota_param);        /* ota patch update parameter */
#endif

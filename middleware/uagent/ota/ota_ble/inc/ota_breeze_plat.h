#ifndef OTA_BREEZE_PLAT_H
#define OTA_BREEZE_PLAT_H

#define BREEZE_OTA_INFO_CMD_LEN     (256)

#ifndef bool
#define bool unsigned char
#endif

#define ALI_OTA_BIN_TYPE_MAGIC_APP    0xabababab
#define ALI_OTA_BIN_TYPE_MAGIC_KERNEL 0xcdcdcdcd
#define ALI_OTA_BIN_TYPE_MAGIC_SINGLE 0xefefefef

#define PAGE_SIZE                   0x1000
#define PROGRAM_UNIT                4
#define ERASE_UNIT                  PAGE_SIZE
#define INIT_COMMAND_MAX_SIZE       256     /**< Maximum size of the init command stored in dfu_settings. */

#define NRF_DFU_CURRENT_BANK_0      0x00
#define NRF_DFU_CURRENT_BANK_1      0x01

#define NRF_DFU_BANK_LAYOUT_DUAL    0x00
#define NRF_DFU_BANK_LAYOUT_SINGLE  0x01

#define UPDATE_FINISHED             0xAB

/** @brief DFU bank state codes.
 *
 * @details The DFU bank state indicates the content of a bank:
 *          A valid image of a certain type or an invalid image.
 */

#define NRF_DFU_BANK_INVALID     0x00 /**< Invalid image. */
#define NRF_DFU_BANK_VALID_APP   0x01 /**< Valid application. */
#define NRF_DFU_BANK_VALID_SD    0xA5 /**< Valid SoftDevice. */
#define NRF_DFU_BANK_VALID_BL    0xAA /**< Valid bootloader. */
#define NRF_DFU_BANK_VALID_SD_BL 0xAC /**< Valid SoftDevice and bootloader. */

#define RESERVED_SIZE1      (20)
#define RESERVED_SIZE2      (20)
#define RESERVED_SIZE3      (17)
#define VERSION_BUF_SIZE    (64)

#define OTA_ONLY_UPGRADE_FW            (0x4F4F5546)//"OOUF"
#define OTA_DUAL_UPGRADE_FW            (0x4F445546)//"ODUF"
#define OTA_ROLLBACK_FW_VERSION        (0x4F524656)//"ORFV"
#define OTA_UPGRADE_FW_SUCC            (0x4F554653)//OUFS
#define OTA_UPGRADE_FINISH             (0xFFFFFFFF)

typedef struct
{
    unsigned int settings_crc32;
    unsigned int ota_flag;
    unsigned int src_addr;
    unsigned int dest_addr;
    unsigned int image_size;
    unsigned int image_crc32;
    unsigned int image_has_copy_len;                     /*remember the copy len for off line*/
    unsigned char reserved_buf1[RESERVED_SIZE1];
    /*backup image include store addr, recovery bank addr, image size and image crc32*/
    unsigned int  backup_store_addr;
    unsigned int  backup_dest_addr;
    unsigned int  backup_image_size;
    unsigned int  backup_image_crc32;
    unsigned int  backup_has_copy_len;                   /*remember the copy len for off line*/
    unsigned char reserved_buf2[RESERVED_SIZE2];
    /*down_loader image parameters*/
    unsigned int break_point_offset;                     /*breakpoint save when ota processing.*/
    unsigned char version_store_buf[VERSION_BUF_SIZE];   /*version save*/
    unsigned short image_info_crc16;                     /*image info crc value*/
    unsigned char bin_type;                              /*Image type*/
    unsigned char reserved_buf3[RESERVED_SIZE3];
} ota_settings_t;

void ota_breeze_set_image_info_crc16(unsigned char* data, unsigned int len);
void ota_breeze_set_bin_type(unsigned char type);
int ota_breeze_breakpoint_process(unsigned int iamge_size, unsigned int* break_point, bool breakpoint_valid);
int ota_breeze_write(unsigned int* off, char* in_buf ,int in_buf_len);
int ota_breeze_read(unsigned int* off, char* out_buf, int out_buf_len);
int ota_breeze_save_breakpoint(unsigned int break_point);
int ota_breeze_set_boot(void);
int ota_breeze_rollback(void);
#endif /*OTA_BREEZE_PLAT_H*/


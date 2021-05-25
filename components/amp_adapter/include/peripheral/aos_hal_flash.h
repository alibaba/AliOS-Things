/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef AOS_HAL_FLASH_H
#define AOS_HAL_FLASH_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define PAR_OPT_READ_POS  (0)
#define PAR_OPT_WRITE_POS (1)

#define PAR_OPT_READ_MASK  (0x1u << PAR_OPT_READ_POS)
#define PAR_OPT_WRITE_MASK (0x1u << PAR_OPT_WRITE_POS)

#define PAR_OPT_READ_DIS  (0x0u << PAR_OPT_READ_POS)
#define PAR_OPT_READ_EN   (0x1u << PAR_OPT_READ_POS)
#define PAR_OPT_WRITE_DIS (0x0u << PAR_OPT_WRITE_POS)
#define PAR_OPT_WRITE_EN  (0x1u << PAR_OPT_WRITE_POS)

typedef enum {
    HAL_PARTITION_ERROR = -1,
    HAL_PARTITION_BOOTLOADER,
    HAL_PARTITION_APPLICATION,
    HAL_PARTITION_ATE,
    HAL_PARTITION_OTA_TEMP,
    HAL_PARTITION_RF_FIRMWARE,
    HAL_PARTITION_PARAMETER_1,
    HAL_PARTITION_PARAMETER_2,
    HAL_PARTITION_PARAMETER_3,
    HAL_PARTITION_PARAMETER_4,
    HAL_PARTITION_BT_FIRMWARE,
    HAL_PARTITION_SPIFFS,
    HAL_PARTITION_LITTLEFS,
    HAL_PARTITION_LITTLEFS2,
    HAL_PARTITION_LITTLEFS3,
    HAL_PARTITION_CUSTOM_1,
    HAL_PARTITION_CUSTOM_2,
    HAL_PARTITION_2ND_BOOT,
    HAL_PARTITION_MBINS_APP,
    HAL_PARTITION_MBINS_KERNEL,
    HAL_PARTITION_GPT,
    HAL_PARTITION_ENV,
    HAL_PARTITION_ENV_REDUND,
    HAL_PARTITION_RTOSA,
    HAL_PARTITION_RTOSB,
    HAL_PARTITION_BOOT1,
    HAL_PARTITION_BOOT1_REDUND,
    HAL_PARTITION_FTL,
    HAL_PARTITION_MAX,
    HAL_PARTITION_NONE,
} hal_partition_t;

typedef enum {
    HAL_FLASH_EMBEDDED,
    HAL_FLASH_SPI,
    HAL_FLASH_QSPI,
    HAL_FLASH_MAX,
    HAL_FLASH_NONE,
} hal_flash_t;

typedef enum {
    HAL_FLASH_ERR_OK,           /* operation success */
    HAL_FLASH_ERR_NAND_BAD,     /* Bad block */
    HAL_FLASH_ERR_NAND_READ,    /* Read fail, can't correct */
    HAL_FLASH_ERR_NAND_WRITE,   /* Write fail */
    HAL_FLASH_ERR_NAND_ERASE,   /* Erase fail */
    HAL_FLASH_ERR_NAND_FLIPS,   /* Too many bitflips, uncorrected */
    /* add more hereafter */
} hal_flash_err_t;

typedef struct {
    hal_flash_t  partition_owner;
    const char  *partition_description;
    uint32_t     partition_start_addr;
    uint32_t     partition_length;
    uint32_t     partition_options;
} hal_logic_partition_t;

typedef struct {
    hal_partition_t p;
    char *descr;
    uint32_t offset;
    uint32_t siz;
} hal_mtdpart_info_t;

/**
 * init flash partition
 *
 * @param[in]  in_partition     The target flash logical partition
 *
 * @return  0: On success， otherwise is error
 */
int32_t aos_hal_flash_init(hal_partition_t in_partition);

/**
 * Get the information of the specified flash area
 *
 * @param[in]  in_partition     The target flash logical partition
 * @param[in]  partition        The buffer to store partition info
 *
 * @return  0: On success， otherwise is error
 */
int32_t aos_hal_flash_info_get(hal_partition_t in_partition, hal_logic_partition_t *partition);

/**
 * Get the information of all the flash partitions
 *
 * @param[in/out]  result_errar  The pointer to the array of MTD partition info
 * @param[in/out]  cnt           The count of the items in the result array
 *
 * @return  0 on success, otherwise failure.
 *
 * @note Caller is responsible to free the result array memory.
 */
int32_t aos_hal_flash_mtdpart_info_get(hal_mtdpart_info_t **result_array, int *cnt);

/**
 * Erase an area on a Flash logical partition
 *
 * @note  Erase on an address will erase all data on a sector that the
 *        address is belonged to, this function does not save data that
 *        beyond the address area but in the affected sector, the data
 *        will be lost.
 *
 * @param[in]  in_partition  The target flash logical partition which should be erased
 * @param[in]  off_set       Start address of the erased flash area
 * @param[in]  size          Size of the erased flash area
 *
 * @return  0 : On success, EIO : If an error occurred with any step
 */
int32_t aos_hal_flash_erase(hal_partition_t in_partition, uint32_t off_set, uint32_t size);

/**
 * Write data to an area on a flash logical partition without erase
 *
 * @param[in]  in_partition    The target flash logical partition which should be read which should be written
 * @param[in]  off_set         Point to the start address that the data is written to, and
 *                             point to the last unwritten address after this function is
 *                             returned, so you can call this function serval times without
 *                             update this start address.
 * @param[in]  inBuffer        point to the data buffer that will be written to flash
 * @param[in]  inBufferLength  The length of the buffer
 *
 * @return  0 : On success, EIO : If an error occurred with any step
 */
int32_t aos_hal_flash_write(hal_partition_t in_partition, uint32_t *off_set,
                        const void *in_buf, uint32_t in_buf_len);

/**
 * Write data to an area on a flash logical partition with erase first
 *
 * @param[in]  in_partition    The target flash logical partition which should be read which should be written
 * @param[in]  off_set         Point to the start address that the data is written to, and
 *                             point to the last unwritten address after this function is
 *                             returned, so you can call this function serval times without
 *                             update this start address.
 * @param[in]  inBuffer        point to the data buffer that will be written to flash
 * @param[in]  inBufferLength  The length of the buffer
 *
 * @return  0 : On success, EIO : If an error occurred with any step
 */
int32_t aos_hal_flash_erase_write(hal_partition_t in_partition, uint32_t *off_set,
                              const void *in_buf, uint32_t in_buf_len);

/**
 * Read data from an area on a Flash to data buffer in RAM
 *
 * @param[in]  in_partition    The target flash logical partition which should be read
 * @param[in]  off_set         Point to the start address that the data is read, and
 *                             point to the last unread address after this function is
 *                             returned, so you can call this function serval times without
 *                             update this start address.
 * @param[in]  outBuffer       Point to the data buffer that stores the data read from flash
 * @param[in]  inBufferLength  The length of the buffer
 *
 * @return  0 : On success, EIO : If an error occurred with any step
 */
int32_t aos_hal_flash_read(hal_partition_t in_partition, uint32_t *off_set,
                       void *out_buf, uint32_t in_buf_len);

/**
 * Set security options on a logical partition
 *
 * @param[in]  partition  The target flash logical partition
 * @param[in]  offset     Point to the start address that the data is read, and
 *                        point to the last unread address after this function is
 *                        returned, so you can call this function serval times without
 *                        update this start address.
 * @param[in]  size       Size of enabled flash area
 *
 * @return  0 : On success, EIO : If an error occurred with any step
 */
int32_t aos_hal_flash_enable_secure(hal_partition_t partition, uint32_t off_set, uint32_t size);

/**
 * Disable security options on a logical partition
 *
 * @param[in]  partition  The target flash logical partition
 * @param[in]  offset     Point to the start address that the data is read, and
 *                        point to the last unread address after this function is
 *                        returned, so you can call this function serval times without
 *                        update this start address.
 * @param[in]  size       Size of disabled flash area
 *
 * @return  0 : On success, EIO : If an error occurred with any step
 */
int32_t aos_hal_flash_dis_secure(hal_partition_t partition, uint32_t off_set, uint32_t size);

/**
 * Convert physical address to logic partition id and offset in partition
 *
 * @param[out]  in_partition Point to the logic partition id
 * @param[out]  off_set      Point to the offset in logic partition
 * @param[in]   addr         The physical address
 *
 * @return 0 : On success, EIO : If an error occurred with any step
 */
int32_t aos_hal_flash_addr2offset(hal_partition_t *in_partition, uint32_t *off_set, uint32_t addr);

#ifdef __cplusplus
}
#endif

#endif /* AOS_HAL_FLASH_H */


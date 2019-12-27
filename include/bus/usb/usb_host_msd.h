/**
 * @file usb_host_msd.h
 * @copyright Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __USB_HOST_MSD_H
#define __USB_HOST_MSD_H

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @addtogroup aos_usb_host usb_host
 * usb host API.
 *
 * @{
 */

/**
 * @brief Try To Connect MSD Device
 *
 * @return 0:success, otherwise is failed
 */
int usbh_msd_connect(void);

/**
 * @brief MSD: Get Max Lun Request
 *
 * @return uint8_t: max lun, 0xFF represent invalid lun
 */
uint8_t usbh_msd_get_max_lun(void);

/**
 * @brief MSD: Test Whether Unit Is Ready Or Not.
 *             Send scsi test unit cmd.
 * @param[in]  lun logic unit number
 *
 * @return 0:success, otherwise is failed
 */
int usbh_msd_unit_is_ready(uint8_t lun);

/**
 * @brief MSD: Get Inquiry Request
 *             Send scsi require cmd.
 * @param[in]   lun     logic unit number
 * @param[out]  inquiry pointer to the buffer which will store inquiry info
 *
 * @return 0:success, otherwise is failed
 */
int usbh_msd_get_inquiry(uint8_t lun, void *inquiry);

/**
 * @brief MSD: Initialize The Block Disk
 *
 * @note this function must call after usb_host_msd_connect return success
 *
 * @return 0:success, otherwise is failed
 */
int usbh_msd_disk_init(void);

/**
 * @brief MSD: Read Capacity Request
 *
 * @param[out]  block_size  pointer to buffer which will store block size
 * @param[out]  block_count pointer to buffer which will store block count
 *
 * @return 0:success, otherwise is failed
 */
int usbh_msd_read_capacity(uint32_t *block_size, uint32_t *block_count);

/**
 * @brief MSD: Block Read
 *
 * @param[in]   block_addr the address to read
 * @param[out]  buff       pointer to the read buffer
 * @param[in]   block_size the block size
 * @param[in]   count      the read block count
 *
 * @return 0:success, otherwise is failed
 */
int usbh_msd_block_read(uint32_t block_addr, uint8_t *buff, uint32_t block_size,
                        uint32_t count);

/**
 * @brief MSD: Block Write
 *
 * @param[in]  block_addr the address to write
 * @param[in]  buff       pointer to the write buffer
 * @param[in]  block_size the block size
 * @param[in]  count      the write block count
 *
 * @return 0:success, otherwise is failed
 */
int usbh_msd_block_write(uint32_t block_addr, uint8_t *buff, uint32_t block_size,
                         uint32_t count);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __USB_HOST_MSD_H */


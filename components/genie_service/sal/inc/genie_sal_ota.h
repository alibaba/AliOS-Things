/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __GENIE_SAL_OTA_H__
#define __GENIE_SAL_OTA_H__
#include <stdbool.h>

#ifndef CONFIG_AIS_TOTAL_FRAME
#define CONFIG_AIS_TOTAL_FRAME 16
#endif

/**
 * @brief reboot the device.
 */
void hal_reboot(void);

/**
 * @brief finish ota and reboot the device.
 */
void dfu_reboot(void);

/**
 * @brief check the dfu image.
 * @param[in] the image type.
 * @param[out] the crc of image.
 * @return the result of checksum.
 */
unsigned char dfu_check_checksum(short image_id, unsigned short *crc16_output);

/**
 * @brief write dfu data.
 * @param[in] the image type.
 * @param[in] the offset of flash.
 * @param[in] the length of data.
 * @param[in] the writting data.
 * @return the current runing partition.
 */
int ali_dfu_image_update(short signature, int offset, int length, int *p_void);

uint32_t ali_dfu_get_ota_partition_max_size(void);

#ifdef CONFIG_GENIE_OTA_PINGPONG
/**
 * @brief get the current runing partition.
 * @return the current runing partition.
 */
uint8_t genie_sal_ota_get_program_image(void);

/**
 * @brief switch the running partition, without reboot.
 * @param[in] the partition which switch to.
 * @return the runing partition when next boot.
 */
uint8_t genie_sal_ota_change_image_id(uint8_t target_id);
#endif

int erase_dfu_flash(void);

/*This api mainly for light product,if light is on we shouldn't reboot by ota*/
bool genie_sal_ota_is_allow_reboot(void);

#endif

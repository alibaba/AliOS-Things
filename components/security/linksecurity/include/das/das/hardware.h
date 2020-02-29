/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __DAS_HARDWARE_H__
#define __DAS_HARDWARE_H__

#include <das/configure.h>
#include <das/platform.h>

#include <das_board.h>

/* @brief get platform firmware version string.
 *
 * @param buf[out]: the firmware string, terminated with with '\0'.
 * @param size[in]: the length of buffer.
 *
 * @return: platform arch string.
 */
size_t das_hal_firmware_version(char *buf, size_t size);

/* @brief get device unique identifier string.
 *
 * @param buf[out]: the device id string, terminated with '\0'.
 * @param size[in]: the length of buffer.
 *
 * @return: @see error codes.
 */
size_t das_hal_device_id(char *buf, size_t size);

#ifdef DAS_SERVICE_ROM_ENABLED

typedef struct _das_rom_bank {
    uint8_t* address;
    size_t size;
} das_rom_bank_t;

/* @brief get rom block infos for detection.
 *
 * @param rom_info[out]: rom info array, containing rom block infos.
 *
 * @return: @see error codes.
 */
int das_hal_rom_info(das_rom_bank_t banks[DAS_ROM_BANK_NUMBER]);

#endif

#endif /* __DAS_HAL_H__ */

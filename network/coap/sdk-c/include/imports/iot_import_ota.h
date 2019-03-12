/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __IOT_IMPORT_OTA_H__
#define __IOT_IMPORT_OTA_H__

/***************************** firmware upgrade interface *****************************/

/** @defgroup group_platform_ota ota
 *  @{
 */

/**
 * @brief initialize a firmware upgrade.
 *
 * @param None
 * @return None.
 * @see None.
 * @note None.
 */
DLL_HAL_API void HAL_Firmware_Persistence_Start(void);


/**
 * @brief save firmware upgrade data to flash.
 *
 * @param[in] buffer: @n A pointer to a buffer to save data.
 * @param[in] length: @n The length, in bytes, of the data pointed to by the buffer parameter.
 * @return 0, Save success; -1, Save failure.
 * @see None.
 * @note None.
 */
DLL_HAL_API int HAL_Firmware_Persistence_Write(_IN_ char *buffer, _IN_ uint32_t length);


/**
 * @brief indicate firmware upgrade data complete, and trigger data integrity checking,
     and then reboot the system.
 *
 * @param None.
 * @return 0: Success; -1: Failure.
 * @see None.
 * @note None.
 */
DLL_HAL_API int HAL_Firmware_Persistence_Stop(void);

#endif  /* __IOT_IMPORT_UOTA_H__ */


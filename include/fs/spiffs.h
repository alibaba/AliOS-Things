/**
 * @file spiffs.h
 * @copyright Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef FS_SPIFFS_H
#define FS_SPIFFS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup fs_spiffs spiffs
 * spiffs API.
 * Used for projects' inside components. Use aos api for APP.
 * @{
 */

/**
 * spiffs register
 *
 * @return 0 on success, negative error on failure
 *
 */
int32_t spiffs_register(void);

/**
 * spiffs unregister
 *
 * @return 0 on success, negative error on failure
 *
 */
int32_t spiffs_unregister(void);

/** @} */

#ifdef __cplusplus
}
#endif

#endif


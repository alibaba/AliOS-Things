/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef BREEZE_AWSS_EXPORT_H
#define BREEZE_AWSS_EXPORT_H

#if defined(__cplusplus) /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif

#include "breeze_export.h"

/**
 * @brief Initialize breeze awss module.
 *
 * @param[in] cb    The callback to be called by breeze SDK when AP info ready.
 * @param[in] info  The device information required by breeze SDK.
 * @return None.
 * @see None.
 */
void breeze_awss_init(apinfo_ready_cb cb, breeze_dev_info_t *info);

/**
 * @brief Stop breeze awss process.
 *
 * @param None.
 * @return None.
 * @see None.
 *
 * @note When this API is called, stop and destroy all the BLE & Breeze releated resources.
 */
void breeze_awss_end();

#if defined(__cplusplus) /* If this is a C++ compiler, use C linkage */
}
#endif

#endif // BREEZE_API_EXPORT_H

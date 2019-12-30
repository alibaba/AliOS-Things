/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

#ifdef FEATURE_UND_SUPPORT

#ifdef FEATURE_UND_WITH_WIFI

#include "und_log.h"
#include "und_hal.h"
#include "network/hal/wifi.h"

#ifdef __cplusplus
extern "C" {
#endif

extern int hal_get_wireless_info(hal_wifi_module_t *m, void *info);

/**
 * @brief Get wireless information from driver real time.
 *
 * @param[in] info @n Driver returns wireless information with info.
 * @return 0: success, otherwise: fail.
 * @see None.
 * @note None.
 */
int HAL_GetWirelessInfo(struct und_wireless_info_t *info)
{
    if (info == NULL) {
        und_err("wireless info == NULL\n");
        return -1;
    }
    return hal_get_wireless_info(NULL, info);
}

#ifdef __cplusplus
}
#endif

#endif /* end of FEATURE_UND_WITH_WIFI */
#endif /* end of FEATURE_UND_SUPPORT */

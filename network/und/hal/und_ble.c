/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

#ifdef FEATURE_UND_SUPPORT

#ifdef FEATURE_UND_WITH_BLE

#ifdef __cplusplus
extern "C" {
#endif

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

    /* BLE needs to implement HAL API to get wirelesss information */

    return -1;
}

#ifdef __cplusplus
}
#endif

#endif /* end of FEATURE_UND_WITH_BLE */
#endif /* end of FEATURE_UND_SUPPORT */

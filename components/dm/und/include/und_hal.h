/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef UND_HAL_H
#define UND_HAL_H

#ifdef __cplusplus
extern "C" {
#endif

struct und_wireless_info_t {
    int rssi; /* Received Signal Strength Indication */
    int snr;  /* Signal to Noise Ratio */
    int per;  /* Packet Error Rate (Unit: PPM, Part Per Million) */
};

/**
 * @brief Get wireless information from driver real time.
 *
 * @param[in] info @n Driver returns wireless information with info.
 * @return 0: success, otherwise: fail.
 * @see None.
 * @note None.
 */
int HAL_GetWirelessInfo(struct und_wireless_info_t *info);

#ifdef __cplusplus
}
#endif

#endif /* UND_HAL_H */

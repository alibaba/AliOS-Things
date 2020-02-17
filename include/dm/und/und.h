/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

#ifndef UND_H
#define UND_H

#ifdef AOS_COMP_UND

#ifdef __cplusplus
extern "C" {
#endif

enum {
    /* Reason code used by NETWORK FAIL */
    UND_STATIS_NETWORK_WIFI_FAIL_REASON = 0x01,
    UND_STATIS_NETWORK_DHCP_FAIL_REASON,
    UND_STATIS_NETWORK_DNS_FIAL_REASON,
    UND_STATIS_NETWORK_TCP_FAIL_REASON,
    UND_STATIS_NETWORK_TLS_FAIL_REASON,
    UND_STATIS_NETWORK_CONN_FAIL_REASON,

    /* Reason code used by NETWORK EXCEPTION */
    UND_STATIS_NETWORK_RD_EXCEPTION_REASON = 0x01,
    UND_STATIS_NETWORK_WR_EXCEPTION_REASON,
    UND_STATIS_NETWORK_PING_EXCEPTION_REASON,

    /* Reason code used by DEV EXCEPTION */
    UND_STATIS_DEV_WD_RST_EXCEPTION_REASON = 0x01,
    UND_STATIS_DEV_PANIC_ERR_REASON,
    UND_STATIS_DEV_REPOWER_REASON,
    UND_STATIS_DEV_FATAL_ERR_REASON,

    /* Reason code used by MEM WATERMARK */
    UND_STATIS_MEM_WATERMARK_LOW_THRESHHOLD_REASON = 0x01,
    UND_STATIS_MEM_WATERMARK_ALLOC_FAIL_REASON,
};

enum {
    UND_STATIS_WIRELESS_ENV_IDX = 0x00,
    /*
     * RSSI: 0x01, SNR: 0x02, drop ratio: 0x03
     */

    UND_STATIS_NETWORK_FAIL_IDX = 0x10,
    /*
     * reason code:
     *           Wi-Fi fail: 0x01, DHCP fail: 0x02,
     *           DNS fail: 0x03, TCP fail: 0x04, TLS fail: 0x05,
     */

    UND_STATIS_NETWORK_EXCEPTION_IDX = 0x20,
    /*
     * reason code:
     *          CLOUD_REJECT: 0x01, RW_EXCEPTION: 0x02, PING_EXCEPTION: 0x03
     */

    UND_STATIS_DEV_EXCEPTION_IDX = 0x30,
    /*
     * reason code:
     *           WD_RST: 0x01, PANIC_ERR: 0x02, RE-POWER: 0x03, FATAL_ERR: 0x04
     */

    UND_STATIS_MEM_WATERMARK_IDX = 0x40,
    /*
     * type of total size: 0x01, type of free size: 0x02
     */

    UND_STATIS_BLE_FAIL_IDX = 0x50,
    /*
     * reason code:
     */
    UND_STATIS_CAP_IDX_MAX = 0x60,
};

/**
 * Initialize uND (Network Diagnosis).
 *
 * Note: uND would used KV to store captures,
 *       so make sure KV is initialized before the API called.
 *
 * @return  0 on success, otherwise failure.
 */
int und_init(void);

/**
 * Deinitialize uND (Network Diagnosis).
 *
 * @return  0 on success, otherwise failure.
 */
int und_deinit(void);

/**
 * update captures to uND.
 *
 * @return  0 on success, otherwise failure.
 */
int und_update_statis(int cap_idx, int reason_code);

#ifdef __cplusplus
}
#endif

#endif  /* end of AOS_COMP_UND */
#endif  /* end of UND_H */


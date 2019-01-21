/*
 * Driver interface definition
 * Copyright (c) 2003-2010, Jouni Malinen <j@w1.fi>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Alternatively, this software may be distributed under the terms of BSD
 * license.
 *
 * See README and COPYING for more details.
 *
 * This file defines a driver interface used by both %wpa_supplicant and
 * hostapd. The first part of the file defines data structures used in various
 * driver operations. This is followed by the struct wpa_driver_ops that each
 * driver wrapper will beed to define with callback functions for requesting
 * driver operations. After this, there are definitions for driver event
 * reporting with wpa_supplicant_event() and some convenience helper functions
 * that can be used to report events.
 */

#ifndef _DRV_SECURITY_H_
#define _DRV_SECURITY_H_

//#include <ssv_pktdef.h>

/**
* enum sec_type_en (sec_type) - Security type setting.
*
* @ SEC_DISABLE: no security
* @ SEC_WEP40: WEP-40
* @ SEC_WEP104: WEP-104
* @ SEC_TKIP: TKIP
* @ SEC_CCMP: CCMP
*/
typedef enum sec_type_en {
    SECURITY_NONE,
    SECURITY_WEP40,
    SECURITY_WEP104,
    SECURITY_TKIP,
    SECURITY_CCMP,

} sec_type;

enum SEC_ALG {
	SEC_ALG_NONE,
	SEC_ALG_WEP,
	SEC_ALG_TKIP,
	SEC_ALG_CCMP,
	SEC_ALG_IGTK,
	SEC_ALG_PMK
};

/**
 * Define Constant vlaues for WSID:
 *
 */
#define WSID_0                          0
#define WSID_1                          1
#define WSID_GROUP_ADDR               0xe  
#define WSID_NOT_FOUND                0xf



s32 drv_sec_init(void);
s32 drv_sec_reset_key_tbl(void);
int drv_sec_set_key(u8 vif_idx, enum SEC_ALG alg, const u8 *addr,
                                int key_idx, int set_tx,
                                const u8 *seq, size_t seq_len,
                                const u8 *key, size_t key_len);
void drv_buckup_hwkey(void);
void drv_reinstall_hwkey(void);

//s32 drv_sec_reply_check(PKT_RxInfo *pRxInfo);

#endif /* _DRV_SECURITY_H_ */


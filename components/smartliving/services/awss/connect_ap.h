/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __AWSS_CONNECT_AP_H__
#define __AWSS_CONNECT_AP_H__

#include "awss_log.h"

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif
#define AWSS_TOKEN_LEN       (16)

int awss_complete_token(char passwd[HAL_MAX_PASSWD_LEN], uint8_t *bssid, uint8_t bssid_len,
                        uint8_t *token_in, uint8_t token_len, uint8_t token_out[AWSS_TOKEN_LEN]);

int awss_connect(char ssid[HAL_MAX_SSID_LEN], char passwd[HAL_MAX_PASSWD_LEN], uint8_t *bssid, uint8_t bssid_len,
                 uint8_t *token, uint8_t token_len);

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif

#endif

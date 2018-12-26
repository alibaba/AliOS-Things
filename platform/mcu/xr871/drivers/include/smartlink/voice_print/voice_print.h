/*
 * Copyright (C) 2017 XRADIO TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of XRADIO TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _VOICE_PRINT_H_
#define _VOICE_PRINT_H_

#if (defined(__CONFIG_ARCH_DUAL_CORE) && defined(__CONFIG_ARCH_APP_CORE))

#include <stdint.h>
#include "net/wlan/wlan_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    WLAN_VOICEPRINT_SUCCESS = 0,   /* success */
    WLAN_VOICEPRINT_FAIL    = -1,  /* general error */
    WLAN_VOICEPRINT_TIMEOUT = -2,  /* wait timeout */
    WLAN_VOICEPRINT_INVALID = -3,  /* invalid argument */
    WLAN_VOICEPRINT_OVERFLOW = -4,  /* buffer overflow */
} voiceprint_ret_t;

/*
 * decoder_fedpcm() return value define
 */
#define RET_DEC_ERROR -1
#define RET_DEC_NORMAL 0
#define RET_DEC_NOTREADY 1
#define RET_DEC_END 2

typedef enum {
	VP_STATUS_NORMAL = 0,
	VP_STATUS_NOTREADY,
	VP_STATUS_DEC_ERROR,
	VP_STATUS_COMPLETE,
} voiceprint_status_t;

typedef struct wlan_voiceprint_result {
    uint8_t ssid[WLAN_SSID_MAX_LEN];
    uint8_t ssid_len;
    uint8_t passphrase[WLAN_PASSPHRASE_MAX_LEN + 1]; /* ASCII string ending with '\0' */
    uint8_t random_num;
} wlan_voiceprint_result_t;

int voice_print_start(struct netif *nif, const char *key);
voiceprint_status_t voiceprint_get_status(void);
voiceprint_status_t voice_print_wait_once(void);
voiceprint_ret_t voice_print_wait(uint32_t timeout_ms);
voiceprint_ret_t wlan_voiceprint_get_raw_result(char *buf_result, int *len);
voiceprint_ret_t wlan_voiceprint_connect_ack(struct netif *nif, uint32_t timeout_ms,
                                             wlan_voiceprint_result_t *result);
int voice_print_stop(uint32_t wait);

#ifdef __cplusplus
}
#endif

#endif /* (defined(__CONFIG_ARCH_DUAL_CORE) && defined(__CONFIG_ARCH_APP_CORE)) */

#endif /* _VOICE_PRINT_H_ */

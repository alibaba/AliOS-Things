/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __GUIDER_INTERNAL_H__
#define __GUIDER_INTERNAL_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>

#include "iot_import.h"

#include "iotx_log.h"
#include "iotx_utils.h"
#include "utils_md5.h"
#include "utils_base64.h"
#include "utils_hmac.h"
#include "utils_httpc.h"
#include "iotx_system.h"

#define GUIDER_IOT_ID_LEN           (128)
#define GUIDER_IOT_TOKEN_LEN        (64)
#define GUIDER_DEFAULT_TS_STR       "2524608000000"

#define CONN_SECMODE_LEN            (32)

#define GUIDER_SIGN_LEN             (66)

#define GUIDER_TS_LEN               (16)
#define GUIDER_URL_LEN              (256)

#define GUIDER_DEVCODE_LEN          (256)
#define GUIDER_URLENCODE_LEN        (256)

#define SHA_METHOD              "hmacsha256"        /* use hmacSha256 default */
#define MD5_METHOD              "hmacmd5"

#define sys_emerg(...)          log_emerg("sys", __VA_ARGS__)
#define sys_crit(...)           log_crit("sys", __VA_ARGS__)
#define sys_err(...)            log_err("sys", __VA_ARGS__)
#define sys_warning(...)        log_warning("sys", __VA_ARGS__)
#define sys_info(...)           log_info("sys", __VA_ARGS__)
#define sys_debug(...)          log_debug("sys", __VA_ARGS__)

typedef enum _SECURE_MODE {
    MODE_TLS_GUIDER             = -1,
    MODE_TCP_GUIDER_PLAIN       = 0,
    MODE_TCP_GUIDER_ID2_ENCRYPT = 1,
    MODE_TLS_DIRECT             = 2,
    MODE_TCP_DIRECT_PLAIN       = 3,
    MODE_TCP_DIRECT_ID2_ENCRYPT = 4,
    MODE_TLS_GUIDER_ID2_ENCRYPT = 5,
    MODE_TLS_DIRECT_ID2_ENCRYPT = 7,
    MODE_ITLS_DNS_ID2           = 8,
} SECURE_MODE;

extern const char *secmode_str[];
extern char *lvl_color[];

SECURE_MODE _secure_mode_num(void);
void _ident_partner(char *buf, int len);
void _ident_module(char *buf, int len);
int _fill_conn_string(char *dst, int len, const char *fmt, ...);
void guider_print_dev_guider_info(iotx_device_info_t *dev,
                                  char *partner_id,
                                  char *module_id,
                                  char *guider_url,
                                  int secure_mode,
                                  char *time_stamp,
                                  char *guider_sign);
void guider_print_conn_info(iotx_conn_info_t *conn);

#ifndef MQTT_DIRECT
int _http_response(char *payload,
                   const int payload_len,
                   const char *request_string,
                   const char *url,
                   const int port_num,
                   const char *pkey);
#endif

#endif  /* __GUIDER_INTERNAL_H__ */

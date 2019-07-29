/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef _ATHOST_IMPORT_H_
#define _ATHOST_IMPORT_H_

typedef void (*athost_atcmd_handle_cb_t)(void *arg, char *buf, int buflen);

int HAL_Athost_Read(char *outbuf, uint32_t len);

int HAL_Athost_Write(const char *header, const uint8_t *data, uint32_t len,
                    const char *tailer);

int HAL_Athost_HandleRegisterCb(const char *prefix, athost_atcmd_handle_cb_t fn);

#endif

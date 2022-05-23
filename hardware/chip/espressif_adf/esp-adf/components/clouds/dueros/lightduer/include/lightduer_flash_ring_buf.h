// Copyright (2018) Baidu Inc. All rights reserved.
/**
 * File: lightduer_flash_ring_buf.h
 * Auth: Gang Chen(chengang12@baidu.com)
 * Desc: APIs of store string list to flash.
 */
#ifndef BAIDU_DUER_LIGHTDUER_FLASH_RING_BUF_H
#define BAIDU_DUER_LIGHTDUER_FLASH_RING_BUF_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lightduer_types.h"
#include "lightduer_flash.h"

typedef struct {
    duer_flash_context_t ctx;
    int ele_count;
    unsigned int sequence_num;
    unsigned int ele_header;
    unsigned int ele_tail;
} duer_flash_ring_buf_context_t;

/**
 * DESC:
 * Should be called in duer_flash_init,
 * This function will only take effect when first time called.
 *
 * PARAM config: configuration of hardware flash.
 *
 * @RETURN: none
 */
extern void duer_flash_ring_buf_set_config(const duer_flash_config_t *config);

void duer_flash_ring_buf_load(duer_flash_ring_buf_context_t *ctx);

char *duer_flash_ring_buf_top(duer_flash_ring_buf_context_t *ctx);

duer_status_t duer_flash_ring_buf_header_remove(duer_flash_ring_buf_context_t *ctx);

duer_status_t duer_flash_ring_buf_append(duer_flash_ring_buf_context_t *ctx, const char *msg);

#ifdef __cplusplus
}
#endif

#endif//BAIDU_DUER_LIGHTDUER_FLASH_RING_BUF_H


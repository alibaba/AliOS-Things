/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#ifndef OTA_VERIFY_H_
#define OTA_VERIFY_H_
#include <stdint.h>
#include "ota_transport.h"
#include "ali_crypto.h"
typedef struct {
    hash_type_t sign_method;
    char sign_value[65];
} ota_sign_params;

typedef struct {
    hash_type_t sign_method;
    int ctx_size;
    void *ctx_sign;
} ota_sign_ctx_params;

void ota_free_global_context(void);
void ota_save_state(uint32_t breakpoint, ota_sign_ctx_params *signctx);
uint32_t ota_get_update_breakpoint(void);
int ota_malloc_global_context(hash_type_t type);
int ota_check_sign(ota_sign_params *downlaod_sign);
int ota_set_update_breakpoint(uint32_t offset);
int ota_get_last_sign_context(ota_sign_ctx_params *signctx);
int ota_set_cur_sign_context(ota_sign_ctx_params *signctx);
int ota_get_last_sign(char *value);
int ota_set_cur_sign(char *value);
int ota_verify_sign(ota_sign_params last_sign, ota_sign_params cur_sign);
ota_sign_ctx_params *ota_get_global_sign_context(void);

#endif

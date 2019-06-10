/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include "ota_log.h"
#include "ota_hal_plat.h"

static ota_hal_module_t *ota_module = NULL;

void ota_hal_register_module(ota_hal_module_t *module)
{
    ota_module = module;
}

int ota_hal_init(void *something)
{
#ifdef OTA_BOOT_PAPA_COMPATIBAL
    struct  {
        unsigned int dst_adr;
        unsigned int src_adr;
        unsigned int len;
        unsigned short crc;
        unsigned int  upg_flag;
        unsigned char boot_count;
        unsigned int  rec_size;
        unsigned int  splict_size;
        int off_bp;               /*Break point offset*/
        OTA_RES_TYPE_E  res_type; /*result type: OTA_FINISH, OTA_BREAKPOINT*/
        unsigned short param_crc; /*Parameter crc*/
    } __attribute__((packed)) boot_para_more;

    ota_boot_param_t *boot_para_less = (ota_boot_param_t *)something;

    boot_para_more.dst_adr      = boot_para_less->dst_adr    ;
    boot_para_more.src_adr      = boot_para_less->src_adr    ;
    boot_para_more.len          = boot_para_less->len        ;
    boot_para_more.crc          = boot_para_less->crc        ;
    boot_para_more.upg_flag     = boot_para_less->upg_flag   ;
    boot_para_more.boot_count   = boot_para_less->boot_count ;
    boot_para_more.rec_size     = 0                          ;
    boot_para_more.splict_size  = boot_para_less->splict_size;
    boot_para_more.off_bp       = boot_para_less->off_bp     ;
    boot_para_more.res_type     = boot_para_less->res_type   ;
    boot_para_more.param_crc    = boot_para_less->param_crc  ;

    something = (void *)&boot_para_more;
#endif

    if (ota_module == NULL) {
        return -1;
    }

    if (ota_module != NULL && ota_module->init != NULL) {
        return ota_module->init(something);
    }
    return 0;
}

int ota_hal_write(int *off_set, char *in_buf , int in_buf_len)
{
    if (ota_module == NULL) {
        return -1;
    }

    if (ota_module != NULL && ota_module->write != NULL) {
        return ota_module->write(off_set, in_buf, in_buf_len);
    }

    return 0;
}

int ota_hal_read(int *off_set, char *out_buf, int out_buf_len)
{
    if (ota_module == NULL) {
        return -1;
    }

    if (ota_module != NULL && ota_module->read != NULL) {
        return ota_module->read(off_set, out_buf, out_buf_len);
    }

    return 0;
}

int ota_hal_boot(void *something)
{
#ifdef OTA_BOOT_PAPA_COMPATIBAL
    struct  {
        unsigned int dst_adr;
        unsigned int src_adr;
        unsigned int len;
        unsigned short crc;
        unsigned int  upg_flag;
        unsigned char boot_count;
        unsigned int  rec_size;
        unsigned int  splict_size;
        int off_bp;               /*Break point offset*/
        OTA_RES_TYPE_E  res_type; /*result type: OTA_FINISH, OTA_BREAKPOINT*/
        unsigned short param_crc; /*Parameter crc*/
    } __attribute__((packed)) boot_para_more;

    ota_boot_param_t *boot_para_less = (ota_boot_param_t *)something;

    boot_para_more.dst_adr      = boot_para_less->dst_adr    ;
    boot_para_more.src_adr      = boot_para_less->src_adr    ;
    boot_para_more.len          = boot_para_less->len        ;
    boot_para_more.crc          = boot_para_less->crc        ;
    boot_para_more.upg_flag     = boot_para_less->upg_flag   ;
    boot_para_more.boot_count   = boot_para_less->boot_count ;
    boot_para_more.rec_size     = 0                          ;
    boot_para_more.splict_size  = boot_para_less->splict_size;
    boot_para_more.off_bp       = boot_para_less->off_bp     ;
    boot_para_more.res_type     = boot_para_less->res_type   ;
    boot_para_more.param_crc    = boot_para_less->param_crc  ;

    something = (void *)&boot_para_more;
#endif

    if (ota_module == NULL) {
        return -1;
    }

    if (ota_module != NULL && ota_module->boot != NULL) {
        return ota_module->boot(something);
    }

    return 0;
}

int ota_hal_rollback(void *something)
{
    if (ota_module == NULL) {
        return -1;
    }

    if (ota_module != NULL && ota_module->rollback != NULL) {
        return ota_module->rollback(something);
    }

    return 0;
}

const char *ota_hal_get_version(unsigned char dev_type)
{
    if (ota_module == NULL) {
        return NULL;
    }

    if (ota_module != NULL && ota_module->version != NULL) {
        return ota_module->version(dev_type);
    }
    return NULL;
}

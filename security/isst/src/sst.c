/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "sst_dbg.h"
#include "sst_wrapper.h"
#include "sst_osa.h"
#include "sst.h"

#define SST_VER_MAJOR 1
#define SST_VER_MINOR 0
#define SST_VER_BLDNR 0

uint32_t sst_init()
{
    uint32_t ret = 0;

#if 0
    SST_VER("ESS-SST version %d.%d.%d\n", SST_VER_MAJOR,
            SST_VER_MINOR, SST_VER_BLDNR);
#endif

    ret = sst_imp_init();
    if (SST_SUCCESS != ret) {
        SST_ERR("gen key error 0x%x!\n", (unsigned int)ret);
        return ret;
    }

    ret = sst_hal_init();
    if (SST_SUCCESS != ret) {
        SST_ERR("gen key error 0x%x!\n", (unsigned int)ret);
    }

    return ret;
}

uint32_t sst_add_item(const char *name,
                      const uint8_t *data,
                      uint32_t data_len,
                      uint32_t type,
                      uint32_t flag)
{
    void *p_sst = NULL;
    uint32_t ret = 0;
    uint32_t obj_len = 0;

    if (!name || !data || data_len == 0) {
        SST_ERR("create sst bad param!\n");
        return SST_ERROR_BAD_PARAMETERS;
    }

    ret = sst_imp_create_obj(data, data_len, type, &p_sst, &obj_len);
    if (SST_SUCCESS != ret) {
        SST_ERR("create obj failed 0x%x\n", (unsigned int)ret);
        goto clean;
    }

    SST_INF("store sst file len[%d]\n", (unsigned int)obj_len);

    ret = sst_store_obj(name, p_sst, obj_len, flag);
    if (SST_SUCCESS != ret) {
        SST_ERR("store sst file error 0x%x!\n", ret);
        goto clean;
    }

clean:
    sst_imp_destroy_obj(p_sst);

    return ret;
}

uint32_t sst_get_item(const char *name,
                      uint8_t *data,
                      uint32_t *data_len,
                      uint32_t *type)
{
    void *p_sst = NULL;
    uint32_t ret = 0;
    uint32_t obj_len = 0;
    uint32_t tmp_len = 0;

    if (NULL == name || !data_len || (!data && *data_len)) {
        SST_ERR("get item bad param!\n");
        return SST_ERROR_BAD_PARAMETERS;
    }

    tmp_len = *data_len;
    obj_len = sst_imp_get_obj_len(*data_len);
    ret = sst_get_obj(name, &p_sst, &obj_len);
    if (SST_SUCCESS != ret) {
        if (ret == SST_ERROR_SHORT_BUFFER) {
            *data_len = sst_imp_get_data_len(p_sst, obj_len);
            goto clean;
        }
        SST_ERR("get sst file error 0x%x!\n", ret);
        goto clean;
    }

    ret = sst_imp_get_obj_data(p_sst, obj_len,
                               data, data_len, type);
    if (ret) {
        if (ret == SST_ERROR_SHORT_BUFFER && tmp_len == 0) {
            goto clean;
        }
        SST_ERR("get obj data failed 0x%x\n", ret);
        goto clean;
    }

clean:
    if (p_sst) {
        sst_memset(p_sst, 0, obj_len);
        ls_osa_free(p_sst);
    }

    return ret;
}

uint32_t sst_delete_item(const char *name)
{
    if (!name) {
        SST_ERR("delete item failed null name\n");
        return SST_ERROR_BAD_PARAMETERS;
    }

    return sst_delete_obj(name);
}

void sst_cleanup()
{
    sst_hal_deinit();
}

#if CONFIG_SST_MIGRATION
void sst_mig_set_file_path(const char *path)
{
    sst_set_mig_obj_path(path);
}

uint32_t sst_mig_get_file(const char *name, uint8_t *key, uint32_t *key_len)
{
    return sst_get_mig_obj(name, key, key_len);
}

uint32_t sst_mig_store_file(const char *name, uint8_t *key, uint32_t key_len)
{
    return sst_store_mig_obj(name, key, key_len);
}
#endif /* CONFIG_DATA_MIGRATION */

#if CONFIG_DATA_MIGRATION
uint32_t sst_encrypt_data(uint8_t *data, uint32_t data_len, const char *file_path,
                          uint8_t *key, uint32_t *key_len)
{
    return sst_migration_enc_data(file_path, data, data_len, key, key_len);
}

uint32_t sst_decrypt_data (const char *name, uint8_t *key, uint32_t key_len,
                           uint8_t *data, uint32_t *data_len)
{
    return sst_migration_dec_data(name, data, data_len, key, key_len);
}
#endif /* CONFIG_SST_MIGRATION */


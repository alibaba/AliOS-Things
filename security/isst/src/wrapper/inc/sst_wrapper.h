/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _SST_WRAPPER_H_
#define _SST_WRAPPER_H_

uint32_t sst_imp_init(void);

uint32_t sst_imp_get_data_len(void *p_sst, uint32_t obj_len);

uint32_t sst_imp_set_obj_name(const char *item_name, char *obj_name);

uint32_t sst_imp_delete(void *p_sst);

uint32_t sst_imp_hash_data(uint8_t *data_in, uint32_t size, uint8_t *data_out);

uint32_t sst_imp_create_obj(const uint8_t *data, uint32_t data_len, uint32_t type, void **p_sst, uint32_t *obj_len);

uint32_t sst_imp_get_obj_data(void *p_sst, uint32_t obj_len, uint8_t *data, uint32_t *data_len, uint32_t *type);

uint32_t sst_imp_get_obj_len(uint32_t data_len);

void sst_imp_destroy_obj(void *p_sst);

#if CONFIG_SST_MIGRATION || CONFIG_DATA_MIGRATION
uint32_t sst_imp_enc_mig_data(uint32_t type, uint8_t *data_in, uint32_t data_size, uint8_t *key, uint8_t *data_out);

uint32_t sst_imp_dec_mig_data(uint32_t *type, void *p_sst_mig, uint8_t *key, uint8_t **p_data_out, uint32_t *data_size);
#endif

#endif /* _SST_WRAPPER_H_ */


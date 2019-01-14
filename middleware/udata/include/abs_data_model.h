/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

/**
 * @abstract data model
 * @brief abstract data api
 * @version since 0.0.1
 */

#ifndef ABS_SENSOR_MODEL_H
#define ABS_SENSOR_MODEL_H

#include "udata_com_desc.h"
#include "udata_parse.h"

int         abs_data_cali_init(void);
int         abs_data_model_init(void);
int         abs_data_open(udata_service_t *service);
int         abs_data_close(uint32_t abs_index);
int         abs_data_read(uint32_t abs_index, void *pdata, uint32_t nbyte);
int         abs_data_ioctl(uint32_t abs_index, void *config);
int         abs_cali_data_register(uint32_t abs_index, void *cb);
int         abs_cali_data_unregister(uint32_t abs_index);
bool        abs_data_check_dev_tree(uint32_t abs_index);
work_mode_e abs_sensor_mode_get(uint32_t abs_index);
void *      abs_sensor_buff_addr_get(uint32_t abs_index);
uint32_t    abs_sensor_buff_len_get(uint32_t abs_index);
int         abs_data_dev_enable(uint32_t abs_index);
uint32_t    abs_data_get_maxmum(void);
int         abs_data_get_abs_index(sensor_tag_e tag, uint8_t instance, uint32_t* pindex);
int         abs_data_get_sensor_info(uint32_t abs_index, sensor_tag_e* ptag, uint8_t* pinstance);
int         cali_example_example_init(void);

#endif /*ABS_SENSOR_MODEL_H*/

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

#include "uData_com_desc.h"

int abs_data_model_init(void);
int abs_data_open(uData_service_t *service);
int abs_data_close(sensor_tag_e tag);
int abs_data_read(sensor_tag_e tag, void* pdata, uint32_t nbyte);
int abs_data_ioctl(sensor_tag_e tag, void* config);
int abs_cali_data_register(sensor_tag_e tag, void* cb);
int abs_cali_data_unregister(udata_type_e type);

#endif /*ABS_SENSOR_MODEL_H*/


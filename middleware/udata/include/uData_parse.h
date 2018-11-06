/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

/**
 * @uData udata parse
 * @brief udata parse API
 * @version since 0.0.1
 */
#ifndef UDATA_PARSE_H
#define UDATA_PARSE_H
#include "uData_com_desc.h"
#include "service_data_to_cloud.h"

#define UDATA_SERVICE_BITMAP_SET(bitmap,index)   (bitmap[(index) / UDATA_ABS_BITMAP_UNIT] |= (0X1 << ((index) % UDATA_ABS_BITMAP_UNIT)))
#define ABS_DATA_EXSIST(bitmap, index)  ((bitmap[(index)/UDATA_ABS_BITMAP_UNIT]) & (0x1<<((index)%UDATA_ABS_BITMAP_UNIT)))



#define TO_STRING(A)                    #A
#define CONNECT(A,B)                    A##B
    

typedef enum{
    TASK_DFT = 0,   /* use the default service task*/
    TASK_NEW,       /* user will creat a new service task*/
}udata_service_task_e;


extern int uData_config_num_get(uint32_t* pNum);
extern int uData_dtc_config_parse(uint32_t index, service_pub_info_t* dtc);
extern int uData_service_config_parse(uint32_t index, uData_service_t* svc);

#endif /*UDATA_PARSE_H*/

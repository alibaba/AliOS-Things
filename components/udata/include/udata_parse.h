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
#include "udata_com_desc.h"
#include "service_data_to_cloud.h"

#define UDATA_BITMAP_SET(bitmap,index)   (bitmap[(index) / UDATA_ABS_BITMAP_UNIT] |= (0X1 << ((index) % UDATA_ABS_BITMAP_UNIT)))
#define UDATA_BITMAP_COMPARE(bitmap, index)  ((bitmap[(index)/UDATA_ABS_BITMAP_UNIT]) & (0x1<<((index)%UDATA_ABS_BITMAP_UNIT)))

#define TO_STRING(A)                    #A
#define CONNECT(A,B)                    A##B

int udata_service_num_get(uint32_t* pNum);
int udata_service_config_parse(uint32_t index, udata_service_t* svc);

int udata_dtc_num_get(uint32_t* pNum);
int udata_dtc_config_parse(uint32_t index, service_pub_info_t* dtc);

#endif /*UDATA_PARSE_H*/

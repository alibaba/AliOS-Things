/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 *
 *
 */

#ifndef SERVICE_DTC_H
#define SERVICE_DTC_H

#include <stdio.h>
#include <stdlib.h>

#define DTC_PARA_NUM_1 (1)
#define DTC_PARA_NUM_4 (4)
#define DTC_PARA_NUM_8 (8)

#define SERVICE_PUB_NAME_LEN (32)
#define SERVICE_PUB_VALUE_LEN (16)

typedef enum{
    UDATA_INT32,
    UDATA_UINT32,
    UDATA_FLOAT,
    UDATA_TYPE_MAX,
}udata_data_type;

typedef struct _service_pub_type_t
{
    sensor_tag_e tag;
    udata_type_e type;
    bool         dtcFlag;
    udata_data_type data_type;
    uint64_t     time_stamp;
    uint32_t     dtc_cycle;
    uint32_t     coeff;
    uint8_t      decimalPlace;
    int          name_num;
    void *       name_addr;
    char*        str;
    uint32_t     str_len;
} service_pub_info_t;
typedef int (*SET_VALUE_FUNC)(const void *, const char *, const void *,
                              const void *);
typedef int (*POST_PROPERTY_FUNC)(const void *, const char *);

extern int service_dtc_register(service_pub_info_t* dtc);

extern int service_dtc_init(void);

extern int service_dtc_connect_set(bool flag);

extern bool service_dtc_is_connect(void);

extern bool service_dtc_is_publish(udata_type_e type);

extern int service_dtc_publish_cycle_set(udata_type_e type, uint32_t cycle);

extern int service_dtc_name_set(udata_type_e type, char *src[], int num);

extern int service_dtc_init(void);

#define UDATA_ERROR(para1, para2, para3)                              \
    LOG("%s %s %d error  para1 0x%08x, para1 0x%08x, para1 0x%08x\n", \
        uDATA_STR, __func__, __LINE__, (para1), (para2), (para3))

#define UDATA_INFO(para1, para2, para3)                              \
    LOG("%s %s %d info  para1 0x%08x, para1 0x%08x, para1 0x%08x\n", \
        uDATA_STR, __func__, __LINE__, (para1), (para2), (para3))


#endif /*SERVICE_DTC_H*/

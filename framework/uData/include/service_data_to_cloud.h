/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 *
 *
 */

#ifndef SERVICE_DTC_H
#define SERVICE_DTC_H

#include <stdio.h>
#include <stdlib.h>

#define SERVICE_PUB_NAME_LEN (32)
#define SERVICE_PUB_VALUE_LEN (16)

typedef int (*SET_VALUE_FUNC)(const void *, const char *, const void *,
                              const void *);
typedef int (*POST_PROPERTY_FUNC)(const void *, const char *);

extern int service_dtc_register(void *thing_id, SET_VALUE_FUNC set_value,
                                POST_PROPERTY_FUNC post_property);

extern int service_dtc_init(void);

extern void service_dtc_connect_set(bool flag);

extern bool service_dtc_is_connect(void);

extern bool service_dtc_is_publish(udata_type_e type);

extern int service_dtc_publish_cycle_set(udata_type_e type, uint32_t cycle);

extern int service_dtc_name_set(udata_type_e type, char *src[], int num);


#define UDATA_ERROR(para1, para2, para3)                              \
    LOG("%s %s %d error  para1 0x%08x, para1 0x%08x, para1 0x%08x\n", \
        uDATA_STR, __func__, __LINE__, (para1), (para2), (para3))

#define UDATA_INFO(para1, para2, para3)                              \
    LOG("%s %s %d info  para1 0x%08x, para1 0x%08x, para1 0x%08x\n", \
        uDATA_STR, __func__, __LINE__, (para1), (para2), (para3))


#endif /*SERVICE_DTC_H*/

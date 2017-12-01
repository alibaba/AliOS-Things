/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

/*
 * aos_alink.h
 *
 *
 */


#ifndef AOS_ALINK_H_
#define AOS_ALINK_H_

#include "module_alink.h"


void aos_alink_init();
void aos_alink_exit();

int aos_alink_get_time(unsigned int *utc_time);


#ifdef __cplusplus
}
#endif

#endif /* AOS_ALINK_H_ */





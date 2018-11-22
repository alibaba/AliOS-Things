
/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef UDATA_H
#define UDATA_H

#include "udata_com_desc.h"

int uData_report_publish(udata_type_e type, void *pdata);

int uData_dev_ioctl(udata_t *pkg, uint8_t cmd, long long parm);
int uData_subscribe(udata_type_e type);
int uData_unsubscribe(udata_type_e type);
int uData_main(void);


#endif /*UDATA_H*/

/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

/**
 * @uData service manager
 * @brief serive manager API
 * @version since 0.0.1
 */

#ifndef SERVICE_MANAGER_H
#define SERVICE_MANAGER_H

#include "uData_com_desc.h"

int          uData_service_init(void);
int          uData_service_mgr_init(void);
int          uData_find_service(udata_type_e type);
int          uData_service_register(uData_service_t *service);
int          uData_service_unregister(udata_type_e type);
int          uData_get_report_pkg(void *buf, uint32_t index);
int          uData_service_subscribe(udata_type_e type);
int          uData_service_unsubscribe(udata_type_e type);
int          uData_service_get_payload(uint32_t abs_index, void **paddr, uint32_t *plen);


#endif /*SERVICE_MANAGER_H*/

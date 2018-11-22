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

#include "udata_com_desc.h"

int          uData_service_init(void);
int          uData_service_mgr_init(void);
int          uData_find_service(udata_type_e type);
int          uData_service_register(uData_service_t *service);
int          uData_service_unregister(udata_type_e type);
int          uData_get_report_pkg(udata_type_e type, void *buf);
int          uData_service_subscribe(udata_type_e type);
int          uData_service_unsubscribe(udata_type_e type);
int          uData_service_get_payload(uint32_t abs_index, void **pdata, uint32_t *plen);
int          uData_install_report_pkg(udata_type_e type, void *data, size_t len);
int          uData_service_cb_register(udata_type_e type, SERVICE_PROCESS_CB proc,SERVICE_IOCTL_CB ioctl);
int          uData_service_cb_unregister(udata_type_e type);


#endif /*SERVICE_MANAGER_H*/

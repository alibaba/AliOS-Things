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

int          udata_service_init(void);
int          udata_service_mgr_init(void);
int          udata_find_service(udata_type_e type);
int          udata_service_register(udata_service_t *service);
int          udata_service_unregister(udata_type_e type);
int          udata_get_report_pkg(udata_type_e type, void *buf);
int          udata_service_subscribe(udata_type_e type);
int          udata_service_unsubscribe(udata_type_e type);
int          udata_service_get_payload(uint32_t abs_index, void **pdata, uint32_t *plen);
int          udata_install_report_pkg(udata_type_e type, void *data, size_t len);
int          udata_service_cb_register(udata_type_e type, SERVICE_PROCESS_CB proc,SERVICE_IOCTL_CB ioctl);
int          udata_service_cb_unregister(udata_type_e type);


#endif /*SERVICE_MANAGER_H*/

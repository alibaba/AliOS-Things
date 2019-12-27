/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <das/service.h>

#ifdef DAS_SERVICE_ROM_ENABLED
extern das_service_t service_rom;
#endif

extern das_service_t service_sys;

das_service_t * das_service_table[] = {
#ifdef DAS_SERVICE_ROM_ENABLED
    &service_rom,
#endif
    &service_sys,
    NULL,
};


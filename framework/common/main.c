/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include <vfs.h>

#ifdef MESH_GATEWAY_SERVICE
#include "gateway_service.h"
#endif

extern void ota_service_init(void);
extern void version_init(void);
extern int uData_main(void);
int aos_framework_init(void)
{
    LOG("aos framework init.");
    
    version_init();
#ifdef MESH_GATEWAY_SERVICE
    gateway_service_init();
#endif

#ifdef AOS_FOTA
   // ota_service_init();
#endif

#ifdef AOS_UDATA
    uData_main();
#endif

    return 0;
}


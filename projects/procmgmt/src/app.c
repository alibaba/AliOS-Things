/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include "u_api.h"
#include <procmgmt.h>
#include "aos/kernel.h"

#include "process_service.h"
#include "irq/irqdesc.h"
#include "aos/vfs.h"
#ifdef AOS_COMP_LITTLEFS
#include "fs/littlefs.h"
#endif
#ifdef AOS_COMP_DEVICEVFS
#include "drivers/u_ld.h"
#endif
#include "path_manager.h"

#define SERVICE_INIT_TASK_NAME  "service_init"
#define SERVICE_INIT_TASK_PRI   (10)
#define SERVICE_INIT_TASK_STACK (0x1000)
#define SYSTEM_START_TASK_NAME  "system_start"
#define SYSTEM_START_TASK_PRI   (11)
#define SYSTEM_START_TASK_STACK (0x2000)

static aos_task_t  service_init_task;
static aos_task_t  system_start_task;
extern void aos_pm_ash_script(void);

static void proc_service_init(void* arg)
{
    /* start porcess management service */
    serive_proc_server_setup();
    /* regist comand */
    serive_proc_cmd_init();

    while (1) {
        krhino_task_sleep(5000);
    }
}

static void system_start(void* arg)
{
    aos_process_load(SYS_PROC);

    while (1) {
        krhino_task_sleep(5000);
    }
}
void testirq()
{
    printf("app testirq\r\n");
}

int application_start(int argc, char *argv[])
{
    printf("Process management is start\r\n");

    #ifdef AOS_PROC_LOAD_FROM_USAPCE
    extern void uproc_loadm_cli_cmds_register(void);
    uproc_loadm_cli_cmds_register();
    #endif

    /* the pri of service_init_task must higher system_start_task */
    aos_task_new_ext(&service_init_task, SERVICE_INIT_TASK_NAME, proc_service_init, (void*)NULL,
                     SERVICE_INIT_TASK_STACK, SERVICE_INIT_TASK_PRI, 1);

    aos_task_new_ext(&system_start_task, SYSTEM_START_TASK_NAME, system_start, (void*)NULL,
                     SYSTEM_START_TASK_STACK, SYSTEM_START_TASK_PRI, 1);

    /* start path management service */
    path_manager_server_setup();

    /* flash driver init */
#ifdef AOS_COMP_DEVICEVFS
    u_driver_entry("pm");
#endif

    /* file system init */
#ifdef AOS_COMP_LITTLEFS
    littlefs_register();
#endif
    fs_rpc_service_init();


    /* auto load /data/aos.d */
    aos_pm_ash_script();

    while (1) {
        krhino_task_sleep(5000);
    }
}


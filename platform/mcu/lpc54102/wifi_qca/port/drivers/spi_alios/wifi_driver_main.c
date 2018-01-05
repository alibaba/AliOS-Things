#include "wifi_driver_main.h"
#include "k_api.h"
#include "common_api.h"
#include "wifi_env.h"
#include "aos/kernel.h"

// The WLAN driver task priority is set to the next highest priority (0 is highest priority)
// because blockForResponse() needs to temporarily boost caller task's priority
// over that of the driver task

ktask_t atheros_wifi_task;
#define ATHEROS_TASK_STACKSIZE 1024

cpu_stack_t atheros_wifi_task_stack[ATHEROS_TASK_STACKSIZE];

/* NOTE: the whole file will be included as part of freertos env */
/* TODO: check stackSize/sizeof(portSTACK_TYPE) */

extern void Atheros_Driver_Task(void *param);

A_STATUS Driver_CreateThread(void *pCxt)
{
    kstat_t status;
    status = krhino_task_create(&atheros_wifi_task, "atheros_wifi", pCxt, (AOS_DEFAULT_APP_PRI - 10), 0, 
                                atheros_wifi_task_stack, ATHEROS_TASK_STACKSIZE, Atheros_Driver_Task, 1);
    PRINTF("Driver_CreateThread status = %d\r\n", status);

    return RHINO_SUCCESS == status ? A_OK : A_ERROR;
}

// Compliments CreateDriverThread
A_STATUS Driver_DestroyThread(void *pCxt)
{
    GET_DRIVER_COMMON(pCxt)->driverShutdown = true;
    DRIVER_WAKE_DRIVER(pCxt);
    return A_OK;
}

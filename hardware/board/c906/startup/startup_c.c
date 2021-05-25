
#include "board.h"
#include <k_api.h>

static ktask_t *g_main_task;
#define OS_MAIN_TASK_STACK (2*1024)
#define OS_MAIN_TASK_PRI 32
extern int aos_maintask(void);

int main(void)
{
    krhino_init();

    /*main task to run */
    krhino_task_dyn_create(&g_main_task, "main_task", 0, OS_MAIN_TASK_PRI, 0, OS_MAIN_TASK_STACK, (task_entry_t)aos_maintask, 1);
    /*kernel start schedule!*/
    krhino_start();

    while (1) {
        krhino_task_sleep(100);
    }

    /*never run here*/
    return 0;
}


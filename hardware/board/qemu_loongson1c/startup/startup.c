#include <k_api.h>
#include <board.h>
#include <aos/kernel.h>
#include <ls1c_uart.h>
#include <ls1c_irq.h>

#if (RHINO_CONFIG_MM_TLF > 0)
k_mm_region_t g_mm_region[1];
int g_region_num = 1;
extern int __heap_start;
#endif

extern void aos_maintask(void *arg);
static ktask_t *main_task;

static void main_task_entry(void *arg)
{
    aos_maintask(arg);
}

int main(int argc, char *argv[])
{
#if (RHINO_CONFIG_MM_TLF > 0)
    g_mm_region[0].start = (uint8_t *)&__heap_start;
    g_mm_region[0].len = 0x81000000 - (size_t)&__heap_start;
#endif

    uart2_init();
    exception_init();

    aos_init();
    (void)krhino_task_dyn_create(&main_task, "main_task", NULL, AOS_DEFAULT_APP_PRI, 0,
                                 8192 / sizeof(cpu_stack_t), main_task_entry, 1);
    aos_start();

    return 0;
}

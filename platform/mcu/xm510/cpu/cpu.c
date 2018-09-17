#include <stdio.h>
#include <aos/types.h>
#include <board.h>

/**
 *  shutdown CPU
 *
 */
void aos_hw_cpu_shutdown()
{
    uint32_t level;
    printf("cpu shutdown...\n");

    level = cpu_intrpt_save();
    while (level)
    {
        while(1);
    }
}


void hal_reboot(void) 
{
	writel(1, XM_SYS_SOFT_RSTEN);
	writel(0xca110000, XM_SYS_SOFT_RST);
    while (1);  /* loop forever and wait for reset to happen */

    /* NEVER REACHED */
}

#include <stdint.h>
#include <config.h>
#include <drv_intc.h>
#include <csi_core.h>
#include "interrupt.h"

int32_t request_irq(uint32_t u32IrqNo, irq_handler_t pHandler, 
                        uint32_t u32Flags, const char *name, void *dev)
{   
#ifdef CONFIG_CHIP_DH5010M
    if ((pHandler == NULL)||(u32IrqNo >= 64))
    {
        return -1;
    }

    csi_intc_set_vector(u32IrqNo, pHandler);
    
    csi_intc_enable_irq(u32IrqNo);
    
    return 0;
#else
    if ((u32IrqNo >= 64))
    {
        return -1;
    }

    csi_intc_set_vector(u32IrqNo, pHandler);

    csi_intc_enable_irq(u32IrqNo);
    
    return 0;
#endif
}

int32_t free_irq(uint32_t u32IrqNo, void* dev)
{   
    if (u32IrqNo >= 64)
    {
        return -1;
    }

    csi_intc_disable_irq(u32IrqNo);
    
    return 0;
}




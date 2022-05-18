#include <k_arch.h>
#include <k_api.h>

void dispatcher_interrupt(long irq)
{
    krhino_intrpt_enter();
    /*
        add board irq handler, to do...
     */
    krhino_intrpt_exit();
}

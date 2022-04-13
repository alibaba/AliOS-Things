#include "irq/irqdesc.h"

#define request_irq(irq, handler, type, name, *dev) \
         u_request_threaded_irq(irq, handler, handler, type, name, *dev)
		
#define free_irq(irq) u_free_irq(irq, NULL)
#define enable_irq(irq) u_enable_irq(irq)
#define disable_irq(irq) u_disable_irq(irq)
#define irq_set_nmi(irq) u_set_nmi_irq(irq)
#define gic_send_sgi(cluster,bitmap,irq)    u_send_mask_ipi(irq,bitmap) 
#define gic_send_sgi0(cluster,bitmap,irq)    u_send_mask_ipi(irq,bitmap)
